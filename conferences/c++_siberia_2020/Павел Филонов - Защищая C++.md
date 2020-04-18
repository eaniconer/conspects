
# Защищая C++
__Speaker__: _Павел Филонов_

_link_: https://youtu.be/wMUrE1iqFls

## Intro

### Safety

* Bugs,
* Crashes
* Data Corruption

### Security

* Leaks
* Denial of service
* Remote code execution

_Доклад о remote code execution (Security)_

## Instruments

### Регистры x86

Будем рассматривать x86.
* Регистры общего назначения (32bits): EAX, EBX, ECX, EDX, ESI, EDI

`[EAX-32bits] = [AX - 16bits][AH - 8bits][AL - 8bits]` (для EBX, ECX, EDX аналогично)

* Регистр ESP: stack pointer
* Регистр EBP: base pointer

### Assembly

Example:
```
push    ebp
mov     ebp, esp
sub     esp, 16
jmp     .L2
```

### Machine codes

Example:

```
// Address MachineCodes
00000000 6655
00000002 6689E5
00000005 6683EC10
00000009 EB25
```

### Memory Sections (Linux 32bit)

----- 0xffffffff -----

Kernel space.
User code CANNOT read from nor write to these addresses,
doing so results in a SegFault.

----- 0xc000000 == TASK_SIZE

Random stack offset.

----- stack_begin -----

STACK

----- stack_end -----

EMPTY STACK SPACE

----- max_stack_end, STACK LIMIT (eg. 8MB) -----

Random mmap offset

----- mmap_begin -----

Memory Mapping Segment.
File mappings (including dynamic libraries) and anonymous mappings.
Растет вниз.

----- mmap_end -----

EMPTY

----- brk, program break ----

HEAP (растет вверх).

----- start brk -----

Random brk offset

----- bss end -----

BSS segment
Uninitialized static variables, filled with zeros

----- end data -----

Data Segment
Static variables initialized by the programmer
Example: `static char* s = "string";`

----- end code, start data -----

Text Segment (ELF)
Stores the binary of the process

----- 0x08048000 -----
----- 0x00000000 -----

##### Пояснения:

* 1GB отдается ядру
* Приложение считает, что остальные 3GB принадлежат ему

### Stack frame

## Technics

### Security Code Review

```c++
const size_t BUFF_SIZE = 80;
void greetings(const char* str)
{
    char name[BUFF_SIZE];
    strcpy(name, str);
    printf("Hello, %s\n", name);
}

int main(int argc, char* argv[])
{
    greetings(argv[1]);
    return 0;
}
```

Если передать слишком длинную строку, то можно получить SegFault

Как может выглядеть `greetings` на ассемблере:

```assembly
greetings(const char*):
                        // ebp: 2000, esp: 1000, stack: (high)[...](low)
    push    ebp
                        // ebp: 2000, esp: 996, stack: [..., 2000]
    mov     ebp, esp
                        // ebp: 996, esp: 996, stack: [..., 2000]
    sub     esp, 104
            // esp: 892, stack: [... *str addr[1004] ret [addr=1000] 2000 [addr=996] chunk [addr=892]]

    mov     eax, [ebp + 8]      // strcpy(name, str);
                        // по адресу [ebp + 8] == 1004 хранится *str
                        // eax: *str

    mov     [esp + 4], eax
            // stack: [... [addr=996] ... [addr=900] *ptr [addr=896] ... [addr=892]]

    lea     eax, [ebp-88]
            // 996 - 88 = 908
            // eax: 908
            // где лежит name на стеке: [  [addr=988] name [addr=908]...]

    mov     [esp], eax
            // сохранить в то, что лежит по адресу esp
            // stack: [... *ptr [addr=896] 908(*name) [addr=892]]

    call    strcpy

    lea     eax, [ebp-88]       // printf
    mov     [esp+4], eax]
    mov     [esp], OFFSET FLAT:LC0
    call    printf
    leave                       // скинуть esp до ebp
    ret
```

Если строка будет больше 80 символов, то strcpy будет записывать вне выделенного для name буффера,

```bash
--- epb-88

# сюда будет писать strcpy

--- ebp

# saved ebp

--- ebp + 4

# ret addr

--- ebp + 8

# str
```

И можно, например, перезаписать адрес возврата

Например, туда можно записать (угадать) `ebp-88`
При завершении, со стека снимется 4 байта и произойдет jmp по адресу `ebp-88`. Следующей инструкцией, которая будет исполняться, будет инструкция по адресу `ebp-88`. Любой x86 валидный код, может исполняться.


## Protection

### Non Executable stack

Машинный код, который исполнялся, находился не в той секции. ОС дают возможность отключить исполняемый стек, и попытка исполнить код со стека будет прервана ОС.

Чтобы сделать стек исполняемым нужно использовать опцию `-z execstack` у компилятора.

Чтобы проверить исполняемый стек или нет, можно воспользоваться утилитой:

```bash
$execstack -q a.out
X a.out # X - неисполняемый

$execstack -c a.out
$execstack -q a.out
- a.out  # теперь стек исполняемый
```

Исполняемый стек используется для JIT.

#### Атака `Return to libc`

Отключение исполняемого стека не спасает в этом случае.

Можно позвать функцию из libc: подготовить входные данные и вызвать функцию из libc (например system("/bin/sh"))

### Address Space Layout Randomization

Начало стека выбирается случайно, чтобы трудно было подобрать нужный адрес (например ebp-88)

Чтобы отключить рандомизацию, нужно выполнить команду:

```bash
#echo 0 > /proc/sys/kernel/randomize_va_space
```

Но в 32bit не слишком много пространства для рандомизации. Если злоумышленник имеет возможность запускать программу много раз, то он может дождаться/перебрать нужного адреса.

### Stack Smashing Protection

Компиляторы могут защищать стек от переполнения

```c++
$g++ -fstack-protection hello.cpp -o hello
```

Как работает: выделяется немного больше места под локальные переменные. GCC сохраняет случайное число на стеке, а потом проверяет, что оно не изменилось. Но это влияет на производительность.

### Статический анализ: clang-tidy

### Динамический анализ: санитайзеры

`-fsanitize=address`

#### Fuzzing

```bash
$clang++ -fsanitize=fuzzer,address hello.cpp -o hello
```

## Secure Development Lifecycle

* Requirements
    - Map Security
    - Privacy Requirements
* Design
    - Threat Modeling
    - Security Design Review
* Development
    - Static Analysis
    - Peer review
* Test
    - Security Test Cases
    - Dynamic Analysis
* Deployment
    - Final Security Review
    - Application Security & Monitoring Response Plan

## References:

* Кодогенерация С++ кроссплатформенно - [youtu.be](https://youtu.be/49b_WqOKQkg)
* Anatomy of a Program in memory - [manybutfinite.com](https://manybutfinite.com/post/anatomy-of-a-program-in-memory/)
* MUST READ BOOK - Hacking: The Art of Exploitation - [wikipedia.org](https://en.wikipedia.org/wiki/Hacking:_The_Art_of_Exploitation)
* Anatomy of stack smashing Attack - [drdobbs.com](https://www.drdobbs.com/security/anatomy-of-a-stack-smashing-attack-and-h/240001832)
* Protection against buffer overflow - [.win.tue.nl](https://www.win.tue.nl/~aeb/linux/hh/protection.html)
* Bypassing ASLR - [sploitfun.wordpress.com](https://sploitfun.wordpress.com/2015/05/08/bypassing-aslr-part-i/)
* Getting around non-executable stack [seclists.org](https://seclists.org/bugtraq/1997/Aug/63)
* On the Effectiveness of Address-Space Randomization - [stanford.edu](https://web.stanford.edu/~blp/papers/asrandom.pdf)
* Clang-tidy security checks - [clang.llvm.org](https://clang.llvm.org/docs/analyzer/checkers.html)
* Address sanitizer - [github.com](https://github.com/google/sanitizers/wiki/AddressSanitizer)
* libFuzzer tutorial - [github.com](https://github.com/google/fuzzing/blob/master/tutorial/libFuzzerTutorial.md)
* Microsoft Secure Development Lifecycle [microsoft.com](https://www.microsoft.com/en-us/securityengineering/sdl/about)
