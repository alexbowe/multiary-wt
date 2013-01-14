def factorial(n):
    a = 1
    for i in range(0, n):
        a *= (n - i)
    return a

def binomial(n, k):
    lim = k
    num = n

    if k is 0: return 1

    for i in range(1, lim):
        num *= (n - i)

    return num/factorial(k)
