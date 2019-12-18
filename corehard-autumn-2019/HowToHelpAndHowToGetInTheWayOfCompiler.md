
__Speaker__: _Andrey Oleynikov_

# Как помочь и как помешать компилятору (link)[https://youtu.be/7nYfsiDGbak]

[cpp] --> [FrontEnd(clang)] --> [IR] --> [MiddleEnd] --> [IR] --> [BackEnd] --> [obj]

Optimization during MiddleEnd-phase: several passes to transform IR

## Inline

**Inline in Clang:**
- Passes: AlwaysInlinerPass, InlinerPass (depends on optimization level, e.g. `-O2`)
- `inline`
- `__attribute__((always_inline))`
- `__attribute__((noinline))`

##### inline

Related to linker. It allows to have the same code (function definition) in different obj-files.
Linker chooses only one of them and ignores the rest.
As well `inline` keyword is said to used as a hint for compiler.

###### Example 1

```c++
double calc_value(int i, int branch) {
    // here code that manipulates with `i`

    double di = static_cast<double>(i);
    if (branch) {
        return i + (di*di) + (di*di*di) + 0.2 * (di*di*di) + (i % 2);
    } else {
        return i * 0.1 + di * di / 2.0 + di * di * di * 0.3 + di * di * di * 0.4 + i / 2.0;
    }
}
```
Compiler calculates `cost` and `threshold` of function to make decision about inlining.
`Cost` depends on amount of instruction and conditional jumps.

```c++
double get_first_value(int i, int branch) { return calc_value(i, branch); }
double get_second_value(int i, int branch) { return calc_value(i, branch); }

double get_res() {
    const int COUNT = 100000000;
    double res = 0.;
    for (int i = 0; i < COUNT; ++i) {
        res += get_first_value(i, 0) + get_second_value(i, 0);
    }
    return res;
}
```

`clang -O2 -Rpass=inline -c main.cpp`  // ~ 1.2 sec (runtime)

`-Rpass=inline`: generates optimization remarks (see [Generating compiler optimization remarks in LLVM](https://developer.ibm.com/linuxonpower/2018/07/02/generating-compiler-optimization-remarks-llvm/))

Each remark contains information about cost and threshold of function. For example `get_first_value` and `get_second_value` had a cost=0 and was inlined.

If use `-Rpass-missed=inline` it shows functions (with cost) that were not inlined after optimization (cost > threshold).

Now add `inline`-keyword to `calc_value`

```c++
inline double calc_value(...) { ... }
```

What happens in llvm: [link](https://github.com/llvm/llvm-project/blob/308b8b76ceee805c964faf9f2176e3e05532a45b/llvm/lib/Analysis/InlineCost.cpp#L920)
```c++
    // Adjust the threshold based on inlinehint attribute and ...
    if (Callee.hasFnAttribute(Attribute::InlineHint))
      Threshold = MaxIfValid(Threshold, Params.HintThreshold);
```

`inline`-keyword changes threshold

`clang -O2 -Rpass=inline -c main.cpp`  // ~ 1.8 sec (runtime)

Now `calc_value` will be inlined, whereas `get_first_value` and `get_second_value` won't.

In the previous example compiler sees that after inlining we call `calc_value` twice with similar arguments.
Also the compiler knows that `calc_value` is a function without side effects. Therefore `calc_value` can be called once, and its result can be used in the sum-expression.

It the last example `get_first_value` and `get_second_value` won't be inlined and the compiler won't discover that they call the same function. As a result `calc_value` will be called twice.

###### Example 2

_changes_:

```c++
// remove inline-keyword from calc_val

double get_first_value(int i) { return calc_value(i, 0); }
double get_second_value(int i) { return calc_value(i, 1); }

// changes in loop-body
    res += get_first_value(i) + get_second_value(i);
```

`clang -O2 -Rpass=inline -c main.cpp`  // ~ 1.6 sec (runtime)

If we add `inline`-keyword to `calc_value`, we get:

`clang -O2 -Rpass=inline -c main.cpp`  // ~ 1.6 sec (runtime)

Now add `inline` to `get_first_value` and `get_second_value`:

`clang -O2 -Rpass=inline -c main.cpp`  // ~ 0.4 sec (runtime)

In `calc_value` we have big code-chunk which depends only on parameter `i`.
After inlining compiler discovered this similar chunks of code in for-loop-body and optimized them.

##### __attribute__((always_inline)) && __attribute__((noinline))

Force inline / noinline to facilitate compiler work

## Loop Unrolling

```c++
double calc(int i) { return std::sin(i); }
double get_res(int count) {
    double res = 0;
    for (int i = 0; i < count; ++i) {
        res += calc(i);
    }
    return res;
}
```

loop will be optimized approximately so:

```c++
int i = 0;
for (; count - i > 3; i += 4) {
    res += calc(i);
    res += calc(i + 1);
    res += calc(i + 2);
    res += calc(i + 3);
}

for (; i < count; ++i) {
    res += calc(i);
}
```

- Pass: LoopUnrollPass
- `#pragma unroll`
- `#pragma nounroll`
- `#pragma clang loop unroll(full)`
- `#pragma clang loop unroll_count(12)`

These pragma's are placed before loop. The pragma is regarded as a recommendation, not an obligation.

llvm [link](https://github.com/llvm/llvm-project/blob/c6a56c9a506e09e03fe4dcf1128799cbfbb6989e/llvm/lib/Transforms/Scalar/LoopUnrollPass.cpp#L778)

```c++
// llvm/lib/Transforms/Scalar/LoopUnrollPass.cpp
if (ExplicitUnroll && TripCount != 0) {
    UP.Threshold = std::max<unsigned>(UP.Threshold, PragmaUnrollThreshold);
    UP.PartialThreshold =
        std::max<unsigned>(UP.PartialThreshold, PragmaUnrollThreshold);
  }
```

## Instruction combining

```c++
int get_res(int a, int b, int c) {
    return a * b + a * c + a + a + a + c - a;
    // can be optimized to `a * (b + c + 2) + c`
    // in case of float, compiler can't optimize such an expression
}
```

 - Passes: InstCombinePass, ReassociatePass
 - `-ffast-math`: `-fno-honor-infinity`, `-fno-honor-nans`, `-fno-math-errno`, `-ffinite-math`, `-fassociative-math`, `-freciprocal-math`, `-fno-signed-zeros`, `-fnotrapping-math`, `-ffp-contract=fast`


<details><summary> reciprocal-math example </summary>

```c++
// two division
float q1 = a / c;
float q2 = b / c;
use(q1, q2);
```

transforms to

```c++
// one division
float tmp = 1.0f / c;
float q1 = a * tmp;
float q2 = b * tmp;
use(q1, q2);
```

</details>

## Branching

```c++
void calc(bool flag) {
    if (flag) {
        // long rarely used
    } else {
        // usual execution path
    }
}
```

- `__builtin_expect(long exp, long c)`

```c++
void calc(bool flag) {
    if (__builtin_expect(flag, 0)) {  // we expect that flag to be zero
        // long rarely used
    } else {
        // usual execution path
    }
}
```

[`__builtin_expect` doc](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)

## LTO (Link-Time-Optimization)

On compile-stage bitcode-files is generated (not obj-files). Then linker gets bitcode-files.
It means that all optimizations that have done for separate bitcode-file may be applied to group of bitcode files on linker-stage (run passes again).

Cons:
- It is much slower
- we should use only linker which can work with bitcode (llvm-IR).

`clang++ -O2 -flto -c main.cpp source.cpp`
`clang++ -O2 -flto main.o source.o`

`main.o` and `source.o` here is file with bitcode

`$ llvm-dis-6.0 source.o -o source.ll` (get LLVM Intermediate Representation)

#### speed up linkage

`clang++ -O2 -flto=thin -c main.cpp source.cpp`
`clang++ -O2 -flto=thin main.o source.o`

- generates metadata that help avoid unnecessary analysis when re-linking (incremental build faster)
- without `thin` unused function(code) will be removed -> size of binary file smaller

#### Conclusions about lto

- Restriction on the choice of linker
- Build time increased (especially incremental build)
- More context for optimizations

