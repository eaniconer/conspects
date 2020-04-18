
# std::initializer_list — б̶ы̶л̶и̶н̶н̶ы̶й̶ отказ проектирования
__Speaker__: _Павел Новиков_

_link_: https://youtu.be/tL44rnyLQ9Y

## std::initializer_list

Легковесная обертка над множеством константных объектов


```c++
Foo widget;
auto v = std::vector<Foo>{ Foo{}, std::move(widget) };
```

эквивалентно

```c++
std::initializer_list<Foo> list{ Foo{}, std::move(widget) };
auto v = std::vector<Foo>{ list };
```

Происходит только копирование элементов из initializer_list.

### Пример 1

```c++
// Example

using namespace std;

struct T {
    T() { cout << 1; }
    T(const T&) { cout << 2; }
    T(T&&) { cout << 3; }
    T& operator=(const T&) { cout << 4; return *this; }
    T& operator=(T&&) { cout << 5; return *this; }
    ~T() { cout << 6; }
};

vector<vector<T>> getTs() {

    vector<T> v1{ T(), T() };
    cout << endl << "----" << endl;

    vector<T> v2 { T(), T() };
    cout << endl << "----" << endl;

    return { std::move(v1), std::move(v2) };
}

int main()
{
    auto v = getTs();
}
/*
[Out]:
112266
----
112266
----
222266666666
*/
```

### Пример 2

```c++
struct Baz {
    template <typename... Ts>
    Baz(Ts&&... args) { ... }
}

void qux(Baz) {}
void qux(std::vector<const char*>) {}

qux({ "hello", "world" }); // неоднозначный вызов
```

Если исправить класс `Baz` так:

```c++
struct Baz {
    template <typename... Ts,
              typename = std::enable_if_t<
                (std::is_convertible_v<Ts, int> && ...)>>
    // requires (std::is_convertible_v<T, int> && ...)
    Baz(Ts&&... args) { ... }

};
```

то будет вызываться функция `qux(vector<const char*>)`

### Пример 3

```c++
struct Gadget {
    Gadget(int) { cout << 1; }
    Gadget(int, char) { cout << 2; }
    template<size_t N>
    Gadget(char (&&a)[N]) { cout << 3; }
};

int main()
{
    Gadget g1{ 32 }; // 1
    Gadget g2{{ 32 }}; // 1    `int i = { 32 }`
    Gadget g3{{{ 32 }}}; // 3  `char a[1] = { {32} }`

    Gadget g4{32, 42}; // 2
    Gadget g5(32, 42); // 2
    Gadget g6{{32}, {42}}; // 2
    Gadget g7({32}, {42}); // 2
    // Gadget g8{ {{32}}, {{42}} }; // error
    Gadget g9{{32, 42}}; // 3        `char a[2] = {32, 33};
    Gadget g10({32, 42}); // 3
    // Gadget g11{{{32, 42}}}; // error
    Gadget g11{{{32}, {33}}}; // 3   `char a[2] = {{32}, {33}}`
}
```

### Вывод

 * Не использовать std::initializer_list

