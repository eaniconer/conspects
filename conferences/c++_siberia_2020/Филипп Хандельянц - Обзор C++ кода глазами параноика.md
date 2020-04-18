
# Обзор C++ кода глазами параноика
__Speaker__: _Филипп Хандельянц_

_link_: https://youtu.be/Qfo_6AuOnWQ

## 'Auto'matic coding

```c++
void foo(const std::vector<T>& vec)
{
    for (auto i = 0; i < vec.size(); ++i)
    {

    }
}
```
Проблема с тем, что `i` имеет тип `int`, а `.size()` тип `vector<T>::size_type` (обычно беззнаковый)
Фикс: заменить `auto` на `vector<T>::size_type`, или `auto i = 0uLL`

## Reference

```c++
template <typename T>
int ColumnDecimal<T>::compareAt(size_t n, size_t m, const ICollumn& rhs_, int) const
{
    auto other = static_cast<const Self&>(rhs_);
    const T& a = data[n];
    const T& b = other.data[m];

    return decimalLess<T>(b, a, other.scale, scale)
        ? 1
        : (decimalLess<T>(a, b, scale, other.scale) ? -1 : 0);
}
```

в строке с `auto other = static_cast<const Self&>(rhs_);` при выводе типов ссылка отбрасывается.

Фикс: `decltype(auto) other = ` или `auto& other = `

## Vector-vector, `*pChar++`

_link_: [Incrementing vectors | Performance Matters](https://travisdowns.github.io/blog/2019/08/26/vector-inc.html)

```c++
void vector32_inc(std::vector<uint32_t>& v)
{
    for (size_t i = 0; i < v.size(); i++) {
        v[i]++;
    }
}

void vector8_inc(std::vector<uint8_t>& v)
{
    for (size_t i = 0; i < v.size(); i++) {
        v[i]++;
    }
}
```

#### Benchmark

| Compiler | Element | `O1` | `O2` | `O3` |
| --- | --- | --- | --- | --- |
| gcc 8 | `uint8_t` | 2.0 | 2.0 | 2.0 |
| gcc 8 | `uint32_t` | 2.3 | 1.3 | 0.2 |
| clang 8 | `uint8_t` | 9.2 | 2.0 | 2.0 |
| clang 8 | `uint32_t` | 9.2 | 0.2 | 0.2 |

`gcc O2` чуть медленее `clang O2`, т.к. он не применяет оптимизации разворачивания цикла и векторизации

Для uint32_t-версии `begin/end` вычисляются однажды, а для uint8_t вычисляются на каждой итерации цикла. Так происходит из-за pointer-aliasing'а, на каждой итерации разыменовывается указатель на uint8_t и компилятор думает, что таким образом вектор может быть изменен.

Если переписать код по-другому:

```c++
void vector8_inc(std::vector<uint8_t>& v)
{
    auto it = v.begin();
    const auto end = v.end();
    for (; it != end; ++it) {
        ++(*it);
    }
}
```

| Compiler | Element | `O1` | `O2` | `O3` |
| --- | --- | --- | --- | --- |
| gcc 8 | `uint8_t` | 1.3 | 1.3 | 0.06 |
| clang 8 | `uint8_t` | 20.3 | 0.06 | 0.06 |

или то же самое:

```c++
void vector8_inc(std::vector<uint8_t>& v)
{
    for (auto& elem : v) {
        ++elem
    }
}
```

## Security

```c++
#include <cstring>
#include <memory>

void inputPassword(char* pswd);
void processPassword(const char* pswd);

#define MAX_PASSWORD_LEN ...

void foo()
{
    char password[MAX_PASSWORD_LEN];
    inputPassword(password);
    processPassword(password);
    std::memset(password, 0, sizeof(password));
}
```

`memset` удаляется, т.к. компилятор считается, что это `Dead Store`

```assembly
foo():                                # @foo()
        push    rbx
        sub     rsp, 256
        mov     rbx, rsp
        mov     rdi, rbx
        call    inputPassword(char*)
        mov     rdi, rbx
        call    processPassword(char const*)
        add     rsp, 256
        pop     rbx
        ret
```

##### Try fix:

```c++
void foo()
{
    char* password = new char[MAX_PASSWORD_LEN];
    inputPassword(password);
    processPassword(password);
    std::memset(password, 0, sizeof(password));
    delete[] password;
}
```

В таком случае memset тоже выбрасывается

##### Моя проверка на godbolt:
 * x86-64 clang (any version) выбрасывает memset
 * x86-64 gcc (any version) не отбрасывает
 * x64 msvc 19 не отбрасывает


##### How to fix

 * custom `safe_memset` + disabled LTO/WPO (свой memset в отдельной единице трансляции, если не отключить LTO/WPO, то компилятор может опять его отбросить)
 * Volatile assembly code (может работать не везде)
 * Memset + memory barrier (не всегда работает)
 * `-fno-builtin-memset`, тогда все memset'ы не будут оптимизироваться
 * C11: `memset_s`
 * Window: `RtlSecureZeroMemory`
 * FreeBSD & OpenBSD: `explicit_bzero`
 * Linux Kernel: `memzero_explicit`
 * P1315 proposal

## Dirty data

```c++
static const char* basic_gets(int* cnt)
{
    ...
    int c = getchar();
    if (c < 0) {
        if (fgets(command_buf, sizeof(command_buf) - 1, stdin) != command_buf) {
            break;
        }
        /* remove endline */
        command_buf[strlen(command_buf) - 1] = '\0';
        break;
    }
    ...
}
```
Может прийти пустая строка, тогда `strlen(command_buff)` будет 0, будем записывать в недоступную область памяти.

## [Zero, one, two, Freddy's coming for you](https://www.viva64.com/en/b/0713/)

## Evil within Comparisons

### Pattern: A < B, B > A

```c++
// MongoDB:
if (_server < other._server)
    return true;
if (other._server > _server)
    return false;
```

### Pattern: Evaluation the Size of a pointer instead of the size of the class

```c++
// CryEngine
bool operator==(const T& other) const
{
    return 0 == memcmp(this, &other, sizeof(this)); // sizeof of pointer!!!
}
```

### Pattern: Incorrect use of the memcmp result

```c++
// firebird
SSHORT TextType::compare(ULONG len1, const UCHAR* str1, ULONG len2, const UCHAR* str2)
{
    ...
    SSHORT cmp = memcpy(str1, str2, MIN(len1, len2)); // обрезка типа при сохранении
    if (cmp == 0) {
        cmp = (len1 < len2 ? -1 : (len1 > len2 ? 1 : 0));
    }
    return cmp;
}
```

### Pattern: Incorrect Loops

```c++
bool Peptide::operator==(Peptide& p)
{
    ...
    for (i = 0, j = 0;
         i < this->stripped.length(), j < p.stripped.length();
         i++, j++) {
        ...
    }
}
```

```c++
// Qt
...
size_t size = ...
...
while (--size >= 0) { // while (true)
    ...
}
...
```

## Use `<=>`, Luke!

_article_: [Comparisons in C++20](https://brevzin.github.io/c++/2019/07/28/comparisons-cpp20/)

### Base equality comparison

```c++
struct Foo {
    int a, b;
};

bool operator==(Foo lhs, Foo rhs)
{
    return lhs.a == rhs.a && lhs.b == rhs.b;
}

bool operator!=(Foo lhs, Foo rhs)
{
    return !(lhs == rhs);
}

bool operator<(Foo lhs, Foo rhs)
{
    // return lhs.a < rhs.a && lhs.b < rhs.b; // - Bad way
    if (lhs.a < rhs.a) return true;
    if (rhs.a < lhs.a) return false;
    return lhs.b < rhs.b;
}
```

### Base 'less' comparison

```c++
struct Foo {
    double a;
}

bool operator<(Foo lhs, Foo rhs)
{
    return lhs.a < rhs.a;
}

bool operator>=(Foo lhs, Foo rhs)
{
    return !(lhs < rhs);
}

Foo{ 1.0 } < Foo { NaN }; // false
Foo{ 1.0 } >= Foo { NaN }; // true !! wrong

```

### Comparison in C++ to fix these issues

```c++
#include <compare>

struct Foo {
    double a;

    auto operator<=>(const Foo& rhs) = default;
};
```

## Payne, I can't feel my pointer

```c++
void Item_Paint(itemDef_t* item)
{
    vec4_t red;
    menuDef_t* parent = (menuDef_t*)item->parent;
    red[0] = red[3] = 1;
    red[1] = red[2] = 0;

    if (item == NULL) { // dereference above: item->parent
        return;
    }
    ...
}
```

## I will find you and insert you!

```c++
auto& infoMap = ...
if (auto it = infoMap.find(strFunctionInfo); it == infoMap.end()) {
    infoMap.emplace(strFunctionInfo, dangerousInfo);
} else {
    ...
}
```

Два раза ищем нужное место: `find` и `emplace`.

Fix:

```c++
auto& infoMap = ...
if (auto it = infoMap.lower_bound(strFunctionInfo);
    it != infoMap.end() && it->first == strFunctionInfo)
{
    ...
}
else
{
    infoMap.emplace_hint(it, strFunctionInfo, dangerousInfo);
}
```

## Is it alive?

Перенесли код с С на C++:

```c++
struct Foo {
    int i;
    double d;
};

Foo* bar()
{
    Foo* ptr = (Foo*) malloc(sizeof(Foo));
    if (ptr == NULL)
        return NULL;

    ptr->i = 0;         // OK in C, UB in C++ until c++20
    ptr->d = 0.0;       // OK in C UB in C++ until c++20
    return ptr;
}
```

Так как объект еще не был сконструирован, то получился UB. Начиная с c++20, `malloc` будет гарантировать, что объект жив, до с++20 нужно добавить `new (ptr) Foo;`.

## Выводы

 * Не использовать auto, если сомневаешься в том, какой тип выведется
 * Предпочитать range-based for loop циклу с использованием индекса
 * memset может не очистить данные
 * нужна защита от дурака
 * no copy-paste
