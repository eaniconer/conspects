

# std::to_string(int)faster than light
__Speaker__: _Иван Афанасьев_

_link_: https://youtu.be/09wygiy-ONs

_annotation_: Автор доклада ускоряет `to_string` и `to_wstring` с помощью буфера на стеке и SSO. Ревью: https://reviews.llvm.org/D59178

## `std::string to_string(int value)`

Возвращает строку, с таким же содержимым как и в `buf`, после выполнения `std::sprintf(buf, "%d", value)`

## Reference implementation

stdlib=_libc++_

```c++
string to_string(int value)
{
    return as_string(snprintf, initial_string<string, int>()(), "%d", value);
}

wstring to_wstring(int value)
{
    return as_string(get_swprintf(), initial_string<wstring, int>()(), L"%d", value);
}

template <class S, class V>
struct initial_string;


template <class V>
struct initial_string<string, V> {
    string operator()() const
    {
        string s;
        s.resize(s.capacity());
        return s;
    }
};

template <class V>
struct initial_string<string, V> {
    wstring operator()() const
    {
        wstring s(23, wchar_t()); // 23 for ull -> ALLOCATION
        s.resize(s.capacity());
        return s;
    }
};

template <class S, class P, class V>
S as_string(P sprintf_like, S s, const typename S::value_type* fmt, V a)
{
    size_t available = s.size();
    while (true) {
        int status = sprintf_like(&s[0], available + 1, fmt, a);
        if (status >= 0) {
            size_t used = static_cast<size_t>(status);
            if (used <= available) {
                s.resize(used);
                break;
            }
            available = used;
        } else { // случай ошибки в sprint_f
            available = available * 2 + 1;
        }
        s.resize(available);
    }
    return s;
}
```

Выводы:

* to_string
    * 64 bit: SSO + sprintf
    * 32 bit:
        * small numbers: SSO + sprintf
        * huge numbers: allocation + 2 * sprintf
* to_wstring
    * allocation + sprintf
* missing copy elision opportunities (см `return s` в `as_string`, т.к. `s` параметр функции)

## Proposal #1

Idea:
* Использовать буффер на стеке
* Вызвать sprintf на этом буфере
* Вернуть строку при успехе
* Иначе использовать старый алгоритм

```c++
template <class S, class P, class V>
S as_string(P sprintf_like, const typename S::value_type* fmt, V a)
{
    constexpr size_t size = BIG_ENOUGH_SIZE_FOR_TYPE_V;
    typename S::value_type tmp[size] = {};
    const int len = sprintf_like(tmp, size, fmt, a);
    if (len <= size) {
        return S(tmp, tmp+len); // RVO
    }

    S s;
    ... // fallback to previous algorithm
    return s;
}
```

#### Results:

* `to_string(1)`:
    * ref: 1x
    * v1: 0.84x
* `to_string(max)`:
    * ref: 1x
    * v1: 0.92x

## Proposal #2

* Использовать `(value / 10)` и `(value % 10)`: 2 деления на 1 char
* Заполнение буфера справа налево
* Поддержка отрицательных чисел:
    * `(value % 10) <= 0`
    * обработка знака
    * отдельный случай `std::numeric_limits<V>::min()`, так как отсутствует пара среди положительных чисел

#### Results:

* `to_string(1)`:
    * ref: 1x
    * v1: 0.84x
    * v2: 0.1x
* `to_string(max)`:
    * ref: 1x
    * v1: 0.92x
    * v2: 0.4x

## Why sprintf so slow?

- Алгоритм общего назначения

### Профиль sprintf для ULLONG_MAX

* `_itoa_word`: `38.99%` - полезная работа
* `vfprintf`: `29.43%` - парсинг формата
* `_IO_default_xsputn`: `10.84%` - копирование буферов

### Профиль sprintf для 1

* `vfprintf`: `47.45%` - парсинг формата
* `__strchrnul_avx2`: `10.35%` - зануление буфера
* `_IO_default_xsputn`: `8.14%` - копирование буферов

### Выводы

* Парсинг формата дорогой

## Proposal #3

переиспользовать [`std::to_chars`](https://en.cppreference.com/w/cpp/utility/to_chars)

```c++
template <class S, class V>
S i_to_string(const V v)
{
    constexpr size_t bufsize = numeric_limits<V>::digits10 + 2;
    char buf[bufsize];
    const auto res = to_chars(buf, buf + bufsize, v);
    return S(buf, res.ptr);
}
```
#### Results:

* `to_string(1)`:
    * ref: 1x
    * v1: 0.84x
    * v2: 0.1x
    * v3: 0.07x
* `to_string(max)`:
    * ref: 1x
    * v1: 0.92x
    * v2: 0.4x
    * v3: 0.19x


## `to_chars`

#### uint64 -> uint32

 если `uint64_t` можно привести к `uint32_t`, то нужно это сделать и делить 32-битные числа - может оказаться в 2-4 раза быстрее ([Instruction latencies and throughput](https://gmplib.org/~tege/x86-timing.pdf))

#### сокращение количества делений

* выполняется одно деление для получения одного char

```c++
static char digitLuts[201] =
    "00010203040506070809"
    "1011..."
    ...
    "90919293949596979899";

i = val % 100;
digitLuts[2 * i];
digitLuts[2 * i + 1];
val /= 100;
```

Рассмотрим две функции:

```c++
using T = uint64_t;
T f(T x, T y)
{
    return x / y;
}


pair<T, T> g(T x, T y)
{
    return { x / y, x % y };
}
```

c `clang-9 -O2` генерируется одинаковый ассемблерный код:

```assembly
f(unsigned long, unsigned long):                                 # @f(unsigned long, unsigned long)
        mov     rax, rdi
        xor     edx, edx
        div     rsi
        ret
g(unsigned long, unsigned long):                                 # @g(unsigned long, unsigned long)
        mov     rax, rdi
        xor     edx, edx
        div     rsi
        ret
```

То есть выполняется 0.5 делений на 1 char

Рассмотрим следующие функции:
```c++
int f(int x)
{
    return x / 10;
}

int f(int x, int y)
{
    return x / y;
}
```
и их ассемблерный код:

```assembly
f(int):                                  # @f(int)
        movsxd  rax, edi
        imul    rax, rax, 1717986919
        mov     rcx, rax
        shr     rcx, 63
        sar     rax, 34
        add     eax, ecx
        ret
f(int, int):                                 # @f(int, int)
        mov     eax, edi
        cdq
        idiv    esi
        ret
```

итого получаем не 0.5 делений, а "оптимизированную" divmod операцию

Ссылки:
- http://libdivide.com/
- https://lemire.me/blog/2019/02/08/faster-remainders-when-the-divisor-is-a-constant-beating-compilers-and-libdivide/

#### `memcpy`-trick

```c++
char* append2_1(char* buffer, std::uint32_t i)
{
    std::memcpy(buffer, &digitLuts[2 * i], 2);
    return buffer + 2;
}

vs

char* append2_2(char* buffer, std::uint32_t i)
{
    *buffer = digitLuts[2 * i];
    *(buffer + 1) = digitLuts[2 * i + 1];
    return buffer + 2;
}
```

- Первый вариант: 1x
- Второй вариант: 1.17x (медленнее)

Ассемблерный код:

```assembly
append2_1(char*, unsigned int):                        # @append2_1(char*, unsigned int)
        add     esi, esi
        movzx   eax, word ptr [rsi + digitLuts]
        mov     word ptr [rdi], ax
        lea     rax, [rdi + 2]
        ret

append2_2(char*, unsigned int):                        # @append2_2(char*, unsigned int)
        add     esi, esi
        mov     al, byte ptr [rsi + digitLuts]
        mov     byte ptr [rdi], al
        mov     al, byte ptr [rsi + digitLuts+1]
        mov     byte ptr [rdi + 1], al
        lea     rax, [rdi + 2]
        ret
```

memcpy это особый intrinsic компилятора, который очень хорошо может оптимизироваться.

#### Детектор длины

Неэффективно писать справа налево, так как придется делать memmove. Лучше сразу писать слева направо, а для этого нужно узнать длину.

```c++
// bisection/trisection-like algorithm
if (val < 10000) {
    if (val < 100) {
        ...
    } else {

    }
} else if (val < 100000000) {
    if (val < 1000000) {
        ...
    } else {
        ...
    }
}
```

## References:

* Converts floating point numbers to decimal strings: https://github.com/ulfjack/ryu
