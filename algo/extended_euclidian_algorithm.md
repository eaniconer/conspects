
# Обобщенный алгоритм Евклида

*w.r.t Knuth, vol 1, page 41 (rus)*

> Даны два целых положительных числа `m` и `n`. Требуется найти их наибольший делитель `d` и два целых числа `a` и `b`, таких, что `am + bn = d`

### Моя попытка решения

Известно, что `m = nq + r`

1. Если `r == 0`, то `d = n, a = 0, b = 1`

    `am + bn = 0*m + 1*n = n = d`
    
2. Иначе
    
    Решим аналогичную задачу для `n` и `r`. 
    
    Найдем такие `x` и `y`, что `xn + yr == d`.
    
    Вычислим `a` и `b`:
    
    ```bash
    ## On the one hand:
    am + bn == d
    a(nq + r) + bn == d
    ar + (b+aq)n == d
    
    ## On the other hand:
    xn + yr == d
    
    ## Then
    a = y
    b + aq = x
    
    ## or
    a = y
    b = x - aq
    ```    
    
<details>

<summary> Code </summary>

```python
def extended_gcd(m, n):
    q, r = divmod(m, n)
    if r == 0:
        return (0, 1, n)

    x, y, d = extended_gcd(n, r)
    return (y, x - y * q, d)
```
</details>
    

