
__Speaker__: _Nicolai Josuttis_

# Combining C++17 Features [link](https://www.youtube.com/watch?v=6HoxXeEBtW0)

Slides: https://meetingcpp.com/mcpp/slides


### Initialization

 - `int i = 0; // C`
 - `int i(0);  // C++98`
 - `int i{0};  // C++11`  (advice: use this by default)

 `int i{};` - no function declaration as with `int i();`

 ```c++
 unsigned long i{3.4};

 // clang++: error: type 'double' cannot be narrowed to 'unsigned long' in initializer list
 // g++: error: narrowing conversion of '3.3999999999999999e+0' from 'double' to 'long unsigned int'
 ```

 ```c++
 int i = 32;
 unsigned long ul{i};

 // clang++: error: non-constant-expression cannot be narrowed from type 'int' to 'unsigned long' in initializer list
 // g++: warning: narrowing conversion of 'i' from 'int' to 'long unsigned int' [-Wnarrowing]
 ```

Narrowing links:
1. https://www.modernescpp.com/index.php/c-core-guidelines-rules-for-conversions-and-casts
2. https://cppstyle.wordpress.com/narrowing-conversions-in-c-11/

 ```c++
 std::vector<int> v{8, 15}; // self-explanatory
 std::vector<int> v(8, 15);
 ```

 ```c++
 class Base {
    int i{30};  // since c++11 parentheses are not allowed
 };
 ```

 ```c++
 enum class IntegralEnum;
 IntegralEnum ie{10};   // works since c++17
 ```

 ```c++
 // Aggregate initialization for derived class
 struct Base {
    int i;
    double j;
 };

 struct Derived : public Base {
    int k;
    double p;
 };

 Derived d{{1, 2.0}, 3, 4.0}; // since c++17
 ```

### Example 1: if, if, Traits and Guards

**Problem:**

```c++
template <class T, class Collection>
void insertAsString(T x, Collection& collection) {
    collection.push_back(x);
}

// Usage
std::vector<std::string> values;
insertAsString("hello", values); // works
insertAsString(10, values);      // doesn't work

// use overloading:
template <class Collection>
void insertAsString(int x, Collection& collection) {
    collection.push_back(std::to_string(x));
}


// what if we want `double`
insertAsString(3.3, values); // requires overloading again
```

##### C++11 Solution Attempt:

```c++
template <class T, class Collection>
void insertAsString(T x, Collection& collection) {
    if (std::is_arithmetic<T>::value)
        collection.push_back(std::to_string(x));
    else
        collection.push_back(x);
}

insertAsString("hello", values); // compile-error
insertAsString(10, values);      // compile-error
insertAsString(3.3, values);     // compile-error
```

##### C++17 Solution Attempt:

```c++
template <class T, class Collection>
void insertAsString(T x, Collection& collection) {
    if constexpr (std::is_arithmetic_v<T>)
        collection.push_back(std::to_string(x));
    else
        collection.push_back(x);
}

insertAsString("hello", values);
insertAsString(10, values);
insertAsString(3.3, values);
```

Next example:

```c++
std::vector<std::string> values;
std::mutex valueMtx;

{
    std::lock_guard lg{valueMtx}; // C++17 CTAD - Class Template Argument Deduction <T = std::mutex>
    insertAsString("hello", values);
    insertAsString(10, values);
    insertAsString(3.3, values);
}
```

Let insertAsString is not simple function and we want to lock only when we touch values

```c++

template <class T, class Collection, class Mutex>
void insertAsString(T x, Collection& collection, Mutex& mtx) {
    // other code
    {
        std::lock_guard lg{mtx};
        if constexpr (std::is_arithmetic_v<T>)
            collection.push_back(std::to_string(x));
        else
            collection.push_back(x);
    }
    // other code
}

{
    insertAsString("hello", values, valueMtx);
    insertAsString(10, values, valueMtx);
    insertAsString(3.3, values, valueMtx);
}

```

C++17 makes it better:

```c++

template <class T, class Collection, class Mutex>
void insertAsString(T x, Collection& collection, Mutex& mtx) {
    // other code

    if constexpr (std::lock_guard lg{mtx}; std::is_arithmetic_v<T>)
        collection.push_back(std::to_string(x));
    else
        collection.push_back(x);

    // other code
}
```

```c++

template <class T, class Collection, class... Mutex>
void insertAsString(T x, Collection& collection, Mutex&... mtxs) {
    // other code

    // when multiple mutexes passed it uses the deadlock avoidance algorithm of c++11 std::lock
    // only `std::scoped_lock{mtxs...}` is bad, because we create temporary object,
    // and its life ends after the first semicolon
    if constexpr (std::scoped_lock lg{mtxs...}; std::is_arithmetic_v<T>) {
        collection.push_back(std::to_string(x));
    } else {
        collection.push_back(x);
    }

    // other code
}

{
    insertAsString("hello", values, valueMtx);
    insertAsString(10, values, valueMtx, readonlyMtx);
    insertAsString(3.3, values); // no mutex
}
```

scoped_lock was introduced because of binary compatibilty: lock guard could not be extended to template-version without breaking binary compatibility


**Summary**:
 - compile-time if
 - if with initialization
 - std::scoped_lock<>
 - CTAD
 - Type traits with `_v` suffix


### CTAD for std::vector

```c++
std::vector v{4, 10}; // std::vector<int> with 2 elements
// the best matching ctor of vector with initializer_list
// if you use braces the ctor with initializer_list has the highest priority unless you pass no arguments

std::vector v{8}; // the same: std::vector<int> with 1 element

std::vector v(4, 10); // std::vector<int> with 4 elements

std::vector v(8); // not compiled -> cant deduct element type

std::vector v(8, ""); // vector<char[1]> deduced (ctor takes T by-ref, T does not decay)
// here no const, because const is outside of T in ctor: `vector(size_t n, const T& value, const Allocator&)`
// ERROR: array 'new' cannot have initialization arguments


std::set<std::string> strs;
std::vector v{strs.begin(), strs.end()}; // vector<set<string>::iterator>
std::vector v(strs.begin(), strs.end()); // ERROR: cant deduct element type, but deductin guide helps here

template <typename Iter>
vector(Iter, Iter) -> vector<typename iterator_traits<Iter>::value_type>; // simplified
// see here: https://en.cppreference.com/w/cpp/container/vector/deduction_guides
std::vector v(strs.begin(), strs.end()); // OK: vector<string> deducted using deduction guide


std::vector v{"hello", "world"}; // vector<const char*> because initializer_list-ctor used
std::vector v("hello", "world"); // vector<const char> , deductin guide -> core dump at best

```

### Example 2: Fold, Evaluation, auto, String Literals

##### C++17 print

```c++
template <class T, class... Ts>
void print(const T& first, const Ts&... others) {
    std::cout << first << std::endl;
    if constexpr (sizeof...(others) > 0)
        print(others...);
}
```

##### Fold expression

```c++
template <class... Ts>
auto foldSum(Ts... ts) {
    return (... + ts); // ((t1 + t2) + t3) + ...
}

foldSum() // doesn't work

template <class... Ts>
auto foldSum2(Ts... ts) {
    return (0 + ... + ts);
}

foldSum2(); // works

```

```c++
template <class... Args>
void printAll(Args... args) {
    (std::cout << ... << args); // no space here
}
```

**With spaces:**
```c++
template <class T>
void spaceBefore(T arg) {
    std::cout << " ";
    return arg;
}

template <class Arg, class... Args>
void printAll(Arg first, Args... args) {
    std::cout << first;
    (std::cout << ... << spaceBefore(args));
    // here is problem before C++17, because order of evaluation for many spaceBefore is not fixed
    // example of effectively compiled code:
    // std::cout << spacedBefore(11) << spacedBefore(str);   // here oreder is not fixed -> errors
}
```

**Comma operator:**

```c++
template <class... Args>
void callFoo(const Args&... args) {
    foo(args...); // foo(arg1, arg2, ...);
    foo(args+args, ...);  // foo(arg1+arg1, arg2+arg2, ...);

    foo(args)...; // doesn't compile
    // fold-expresson with comma operator
    (..., (void)foo(args)); // c++17: foo(arg1), foo(arg2), ...
    // workaround
    std::initializer_list<int>{((void)foo(args), 0)...};
}
```

```c++
template <class Arg, class... Args>
void printAll(Arg first, Args... args) {
    std::cout << first;
    auto outWithSpace = [](const auto& arg) { std::cout << ' ' << arg; };
    (..., outWithSpace(args));
    std::cout << std::endl;
}
```

```c++
template <char sep = ' ', class Arg, class... Args>  // before c++17
void printAll(Arg first, Args... args) {
    std::cout << first;
    auto outWithSpace = [](const auto& arg) { std::cout << sep << arg; };
    (..., outWithSpace(args));
    std::cout << std::endl;
}
```

```c++
template <auto sep = ' ', class Arg, class... Args>  // auto since c++17 (any valid type)
void printAll(Arg first, Args... args) {
    std::cout << first;
    auto outWithSpace = [](const auto& arg) { std::cout << sep << arg; };
    (..., outWithSpace(args));
    std::cout << std::endl;
}

static const char sep[] = ", ";  // static const char* sep= ", "; doesn't work
// 'no linkage' now supported for string literals
printAll<sep>("hi", 47, str);
```

### Example 3: Strings, Views, Parallel

```c++
std::vector<std::string> strs;
strs.reserve(numElems);
for (size_t i = 0; i < numElems / 2; ++i) {
    strs.emplace_back("id" + std::to_string(i));
    strs.emplace_back("ID" + std::to_string(i));
}

std::sort(strs.begin(), strs.end());

// let's ignore 2 first characters
std::sort(
    strs.begin(), strs.end(),
    [](const auto& lhs, const auto& rhs)
    { return lhs.substr(2) < rhs.substr(2); }); // even with SOO it is slow (x2-x3)

std::sort(
    strs.begin(), strs.end(),
    [](const auto& lhs, const auto& rhs)
    { return std::string_view{lhs}.substr(2) < std::string_view{rhs}.substr(2); }); // it works as fast as sort wihtout lambda

```

**string_view considered harmful**

```c++
string operator+(string_view lhs, string_view rhs) { // OK
    return string{lhs} + string{rhs};
}

template <class T>
T sum(const T& lhs, const T& rhs) {
    return lhs + rhs;
}

string_view sv = "hi1";
auto xy = sum(sv, sv); // problem here

//fix:
template <class T>
auto sum(const T& lhs, const T& rhs) {  // better declare returned type in template as auto
    return lhs + rhs;
}
```

flag: `-Wlifetime` to track life-time of objects

**sorting again, make it faster**

```c++

std::sort(
    std::execution::seq, // by default
    strs.begin(), strs.end(),
    [](const auto& lhs, const auto& rhs)
    { return std::string_view{lhs}.substr(2) < std::string_view{rhs}.substr(2); });

std::sort(
    std::execution::par,
    strs.begin(), strs.end(),
    [](const auto& lhs, const auto& rhs)
    { return std::string_view{lhs}.substr(2) < std::string_view{rhs}.substr(2); });

```

##### Parallel partial sum

```c++
std::vector<int> vs{1,2,3,4,5,6,7,8};

std::partial_sum(
    vs.begin(), vs.end(),                           // source
    std::ostream_iterator<int>(std::cout, " "));    // destination

std::inclusive_scan(
    std::exectuion::par,
    vs.begin(), vs.end(),
    std::ostream_iterator<int>(std::cout, " "));   // x10 faster?
```

### Example 4: Unions, Polymorphism, Overloading lambda

```c++
std::variant<int, long, std::string> v;  // new kind of polymorphism
v = "abc";
v = 32;
v = 32L;

cout << std::get<1>(v); // 32
cout << std::get<long>(v); // 32
```

```c++
using GeoObj = std::variant<Circle, Line>;
std::vector<GeoObj> objs = createFig(); // no pointers, no life-time issues

for (const auto& obj : objs) {
    std::visit(
        [](const auto& o) { o.draw(); }, // polymorphic call
        // visitor uses local vtable to choose appropriate instantiation of lambda
        obj);

    // downcast
    if (auto objPtr = std::get_if<Circle>(&obj)) {
        auto center = objPtr->getCenter();
    }

    // downcast inside visitor
    std::visit(
        [](const auto& o) {
            o.draw();
            if constexpr (std::is_same_v<decltype(obj), const Cicle&>) {
                auto center = obj.getCenter();
            }
        },
        obj);
}

```

##### Overloading

```c++
template <class... Functors>
struct Olvd : Functors {
    using Functors::operator()...;
};

template <class Functors>
Ovld(Functors...) -> Ovld<Functors...>; // deduction guide

std::visit(
        Ovld{
            [](const auto& o) { o.draw(); },
            [](const Circle& c) { c.draw(); auto center = c.getCenter(); }
        },
        obj);
```

C++ Features used: std::variant, aggregates with base classes, deduction guides, variadic using


































