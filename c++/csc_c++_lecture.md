
_Lecturer_: _Ivan Sorikin_, _JB_

## Статический анализ

 - Clang, статический анализ на уровне AST
 - GCC (GIMPLE), статический анализ на уровне IR
 - Coverity (предоставляют возможность пометить конкретное сбрабатывание как ложное/ненужное, чтобы в будущем оно не мешало)


## Динамические анализаторы

* подмена библиотечных функций
* инструментация кода компилятором (sanitizers)
* JIT-инструментация скомпилированного кода (valgrind)

### Санитайзеры

#### Address Sanitizer, ASan

Supported by: clang, gcc

`-fsanitize=address`

#### Memory Sanitizer

#### Thread Sanitizer

 * Детектирует data races

#### UB Sanitizer

### Valgrind Memcheck

 - Совмещает address + memory sanitizers
 - Не требует перекомпиляции программы
 - Не умеет определять ошибки обращения к объектам на стеке
 - Семейство инструментов: callgrind,...
 - Возможны ложные срабатывания на SIMD коде и битовых полях

Использование:

`valgrind ./a.out`
`valgrind --tool=callgrind ./a.out`


### libstdc++ debug mode

 * `_GLIBCXX_DEBUG` - будут использованы дебажные версии контейнеров

### Fuzzing

## Профилировщики

 * Intel VTune Profiler
 * Linux perf
 * valgrind callgrind

## Reversible-Debugging

* Возмозжность отладчика исполнять программу в обратную сторону

* UndoDB
* rr
* Time Travel Debugging












