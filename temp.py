import numpy as np

# Define a random matrix:

A = np.random.rand(3, 3)
B = np.random.rand(3, 3)

a = np.array([
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9],
])

b = np.array([
    [10, 20, 30],
    [40, 50, 60],
    [70, 80, 90],
])

print(A)
print(B)
print(A @ B)
print(a @ b)