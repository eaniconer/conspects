
__Speaker__: _Eric Niebler_

## Goals for async interface

- composable
- low abstraction overhead
- works with coroutines, fibers, threads
- extensible to multiple execution environment (concurrent and parallel)

## I Background and Introduction

- Parallelism vs Concurrency

## II Senders and Receivers

#### Why is standard futures slow?

```c++
future<int> async_algo() {
    promise<int> p;
    auto f = p.get_future();

    thread th { [p = move(p)]() mutable {
        int answer = 32;
        p.set_value(answer);
    }};
    th.detach();
    return f;
}

int main() {
   auto f = async_algo(); // calling code see that lambda goes after async_algo
   auto f2 = f.then([] (int i) { return i + rand(); } ); // boost-feature

   f2.wait();
   std::cout << f2.get() << std::endl;

   return 0;
}
```

(future) and (promise) have shared block
[value, continuation, mutex, cond_var, ref_count]

When `then` executes, there'are syncronization, allocation, type-erasure happen

Cases: (why do we need mutual exclusion for value)
1. set_value -> then ( `then` sees that value is set => exectues then at once (в каком потоке?) )
2. then -> set_value ( `set_value` sees that continuation is set => executes it )

1st try to fix: pass continuation
now no syncronizations

```c++
// Continuation Passing Style (Function Programming)
template <class Continuation>
future<int> async_algo(Continuation cont) {
    promise<int> p;
    auto f = p.get_future();

    thread th { [p = move(p), cont]() mutable {
        int answer = 32;
        p.set_value(cont(answer));
    }};
    th.detach();
    return f;
}

int main() {
   auto f = async_algo([] (int i) { return i + rand(); });

   f.wait();
   std::cout << f.get() << std::endl;

   return 0;
}
```

What if we don't want to launch a task at once.
We'd like to add a few continuations and only then start the task.

```c++
// -fconcepts

auto async_algo() {
    return [](auto yourPromise) {
        thread th { [p = move(yourPromise)]() mutable {
            int answer = 32;
            p.set_value(answer);
        }};
        th.detach();
    };
}

namespace {
    template <class Promise, class Continuation>
    struct ContinuatedPromise {
        void set_value(auto... args) {
            p_.set_value(continuation_(args...));
        }
        void set_exception(auto e) {
        	p_.set_exception(e);
        }
        Promise p_;
        Continuation continuation_;
    };

    template <class Promise, class Continuation>  // deduction guide
    ContinuatedPromise(Promise, Continuation) -> ContinuatedPromise<Promise, Continuation>;
} // namespace

auto then(auto task, auto continuation) {
    return [=](auto p) { // Promise
        task(ContinuatedPromise{std::move(p), continuation});
    };
}

namespace {
    struct SinkPromise {
        void set_value(auto...) { }
        void set_exception(auto) {}
    };
} // namespace

int main() {
    auto f = async_algo();
    auto f2 = then(f, [](int i) { return i + 10; });
    auto f3 = then(f2, [](int i) { std::cout << i << std::endl; return i; });
    f3(SinkPromise{});
    return 0;
}

```

- std::cout in wrong thread!
- no blocking! -> main might be finished earlier then async-code -> undefined behaviour

Let's fix it:

```c++
namespace {
    template <class T>
    struct State {
        std::mutex mtx;
        std::condition_variable cv;
        std::variant<std::monostate, std::exception_ptr, T> data;
    };

    template <class T>
    struct Promise {
        State<T>* state_;

        template <int I>
        void _set(auto... args) {
            std::unique_lock lk{state_->mtx};
            state_->data.template emplace<I>(args...);
            state_->cv.notify_one();
        }

        void set_value(auto... args) { _set<2>(args...); }
        void set_exception(auto e) { _set<1>(e); }
    };
} // namespace


template <class T, class Task>
T sync_wait(Task task) {
    State<T> state;
    task(Promise<T>{&state});
    {
        std::unique_lock lock{state.mtx};
        state.cv.wait(lock, [&state] { return state.data.index() != 0; });
    }

    if (state.data.index() == 1) {
        std::rethrow_exception(get<1>(state.data));
    }

    return std::move(get<2>(state.data));
}

int main() {
    auto f = async_algo();
    auto f2 = then(f, [](int i) { return i + 10; });

    std::cout << sync_wait<int>(f2) << std::endl;
    return 0;
}
```

#### Why sync_algo is responsible for creating thread?

Splitting

```c++
auto new_thread() {  // new_thread() is executor
    return [](auto yourPromise) {
        thread th { [p = move(yourPromise)]() mutable {
            p.set_value();
        }};
        th.detach();
    };
}

auto async_algo(auto task) {  // like execution context
    return then(task, [] {
            int answer = 32;
            return answer;
        });
}

```

Result: composed, no allocation, no syncronization, no type-erasure, composition - generic, blocking - generic

### LazyPromise

```c++
template <class P, class... Args>
concept LazyPromise =
    requires (P& p, Args&&... args) {
        p.set_value( (Args&&) args... );
        p.set_exception( std::exception_ptr{} );
        p.set_done(); // to request for cancellation
    };
```

### Generalize errors and renaming

```c++
template <class P, class E, class... Args>
concept Receiver =
    requires (P& p, E&& e, Args&&... args) {
        p.set_value( (Args&&) args... );
        p.set_error( (E&&) e );
        p.set_done(); // to request for cancellation
    };
```

#### Refactoring

```c++
template <class P, class E = std::exception_ptr>
concept Receiver =
    requires (P& p, E&& e) {
        p.set_error( (E&&) e );
        p.set_done();
    };

template <class P, class... Args>
concept ReceiverOf =
    Receiver<P> &&
    requires (P& p, Args&&... args) {
        p.set_value( (Args&&) args... );
    };
```

### Lazy Future

```c++
template <class F, class R>
concept LazyFuture = // invokable things which takes a receiver
    Receiver<R> &&
    requires (F&& f, R&& r) {
        std::forward<F>(f)( (R&&) r );
    };
```

Customization point: `submit`

```c++
template <class F, class R>
concept SenderTo =
    Receiver<R> &&
    requires (F&& f, R&& r) {
        submit( std::forward<F>(f), std::forward<R>(r) );
    };
```

Go further to Sender concept

```c++
template <class F>
concept Sender = is_sender_v<std::decay_t<F>>;

template <class F, class R>
concept SenderTo =
    Sender<F> &&
    Receiver<R> &&
    requires (F&& f, R&& r) {
        submit( std::forward<F>(f), std::forward<R>(r) );
    };
```

## Sender/Receiver & Coroutines

#### Coroutines

```c++
task<int> async_fib(int i);

task<void> async_algo() {

	int fibo = co_await async_fib(30);
	// code after co_await/co_yield treats as implicit callback

	std::cout << fibo;
}
```

1. (suspended coroutine is callback) && (callback is Receiver) => coroutine is Receiver
2. awatables are senders

	What would be in the future C++:
```c++
namespace std {
	// It helps you to make any Sender Awatable
	inline namespace awaitable_senders {
		template <Sender s>
		auto operator co_await(S&& s) { // ADL works
			return _awaiter_sndr{ (S&&) s };
		}
	}
}

struct DumbSender : std::sender_of<int> {
	void submit(ReceiverOf<int> auto r) {
		r(32);
	}
}

coro_task<int> async_algo(Sender auto s) {
	int answer = co_await s;
	co_return answer;
}

int main() {
	// coro_task (awaitables) might be treated as Senders
	int res = sync_wait<int>(async_algo(DumbSender{}));
}

```

Awaitables are Senders (again)

```c++
struct MyAwaitable {  // Satisfies requirements of Sender concept
	friend auto operator co_await(MyAwaitable) { return ...; }
};

struct MyReceiver {
	void set_value(auto...);
	void set_error(auto);
	void set_done();
};

template <Awaitable A, ReceiverOf<await_result_of<A>> R>
void submit(A awaitable, R to) noexcept {
	try {
		invoke([](A a, R&& r) -> oneway_task {
			R rCopy{(R&&) r};
			try {
				rCopy.set_value(co_await (A&&) a);
			} catch (...) {
				rCopy.set_error(current_exception());
			}
		}, (A&&) awaitable, (R&&) to);
	} catch (...) {
		to.set_error(current_exception());
	}
}

int main() {
	submit(MyAwaitable{}, MyReceiver{}); // awaitables as senders
}

```

## Building on Senders/Receivers

```c++
template <class T>
class MyFuture {
private:
	shared_ptr<MyState<T>> st_ = make_shared<MyState<T>>();
public:
	template <SenderOf<T> S>
	explicit MyFuture(S&& s) {
		((S&&) s).submit(st_->makeReceiver());
	}

	T get() && {
		return move(*st_).get();
	}
};
```












