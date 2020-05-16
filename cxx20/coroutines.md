
# Coroutines TS: что это такое и как этим пользоваться

[link](https://www.youtube.com/watch?v=2tWNm_fDxX0)

```c++
auto answer()
{
    doSmth();
    co_return 42;
}

// clang 11.0.0:
// wandbox: include <experimental/coroutine>
// error: 'co_return' cannot be used in a function with a deduced return type

// gcc 10.0.1
// wandbox: compiler option: -fcoroutines
// error: 'co_return' cannot be used in a function with a deduced return type

```

### Псевдокод корутины

```c++

ret_type answer()
{
    struct frame_type {
        // local variables, parameters, temporaries
        ret_type::promise_type promise;
        // resume point, register values - информация для продолжения выполнения
    };

    frame_type* f = create_frame();
    auto& p = f->promise;
    new (ret_address) ret_type(p.get_return_object()); // ret_address определяется calling convention
    co_await p.initial_suspend();

    try {
        doSmth();
        p.return_value(42);
    } catch (...) {
        p.unhandled_exception();
    }

    co_await p.final_suspend();
    destroy(f);
}
```

##### co_await <expr>

`namespace stdx = std::experimental`

    - decltype(<expr>) == stdx::suspend_never -> no-op
    - decltype(<expr>) == stdx::suspend_always -> suspend (stores return point, registers values and returns control to caller)


##### ret_type, 1st try

```c++
#include <experimental/coroutine>

namespace stdx = std::experimental;

template <class T>
struct mysync {
    struct promise_type {
        stdx::suspend_never initial_suspend() { return {}; }
        stdx::suspend_never final_suspend()   { return {}; }
        void unhandled_exception() { std::terminate(); }

        T value;
        void return_value(T t) { value = std::move(t); }
        T& get_return_object() { return value; }
    };

    T& value;

    mysync(T& t)
        : value(t) {}

    friend std::ostream& operator<<(std::ostream& out, const mysync& self)
    {
        return out << self.value;
    }
};


mysync<int> answer()
{
    doSmth();
    co_return 42;
}

int main() {
    std::cout << "ans: " << answer(); // out: "ans: 1587969952"
    return 0;
}
```

Такой вывод получился из-за того, что после `destroy(f)` ссылка `value` указывает на значение в уничтоженном фрейме.


