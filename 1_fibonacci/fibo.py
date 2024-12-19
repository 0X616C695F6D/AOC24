"""

Problem Statement:
-------------------

Difficulty: Easy
Recommended Language: Python
Problem Statement:
Implement a program to compute the Nth Fibonacci number modulo 1,000,000,007.
The input provides N (0 ≤ N ≤ 10^7). Output the Nth Fibonacci number under the
given modulus. Optimize to handle large N efficiently (e.g., using fast doubling
or matrix exponentiation).

"""


MIN_NUMBER = 0
MAX_NUMBER = 1*10**7
MODULO     = 1*10**9 + 7


# Naive method of matmul
def matmul(mat_a, mat_b):
    sum = [[0 for row in range(2)] for col in range(2)]
    for i in range(2):
        for j in range(2):
            for k in range(2):
                sum[i][j] += mat_a[i][k] * mat_b[k][j]
                sum[i][j] %= MODULO
    return sum


def mat_exp(A, n):
    # base case
    if n == 1:
        return A

    even   = mat_exp(A, n // 2)
    square = matmul(even, even)

    # if n is odd
    if n % 2 != 0:
        return matmul(square, A)
    # if n is even
    return square


# Comput n-th fibonacci
def fibonacci(n):
    A = [[1, 1], [1, 0]] # Transformation matrix
    c = mat_exp(A, n - 1)
    print(f"Output fibonacci matrix -> {c}")
    return print(f"Fibonacci number -> {c[0][0]}")


# Validate the matmul function works
mat_a = [[1, 1], [1, 0]]
mat_b = [[1, 1], [1, 0]]

print(f"Validation of matmul")
print(f"MatrixA -> {mat_a}")
print(f"MatrixB -> {mat_b}\n")
print(f"Output Matrix -> {matmul(mat_a, mat_b)}\n")

print(fibonacci(5))
