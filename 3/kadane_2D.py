"""
Difficulty: Medium
Recommended Language: Python
Problem Statement:
Read a matrix of integers (R rows × C columns) and find the maximum sum of any
rectangular submatrix. Implement an algorithm (like Kadane’s 2D) to handle large
matrices efficiently. Print the maximum sub-rectangle sum.
"""

def kadane_1d(arr):
    max_sum = 0
    current_sum = 0
    start = 0
    max_start = 0
    max_end = 0

    for i, val in enumerate(arr):
        # Condition to reset
        if current_sum <= 0:
            current_sum = val
            start = i
        else:
            # Condition to increase sum
            current_sum += val

        # Condition to reset
        if current_sum > max_sum:
            max_sum = current_sum
            max_start = start
            max_end = i

    return max_sum, max_start, max_end

def kadane_2d(mat):
    row = len(mat)
    col = len(mat[0])

    max_sum = 0
    top_left = (0,0)
    bottom_right = (0,0)

    # Left boundary
    for left in range(col):
        temp = [0] * row
        # Right boundary
        for right in range(left, col):
            for i in range(row):
                temp[i] += matrix[i][right]
                # Maximum sumarray sum
                current_sum, start, end = kadane_1d(temp)
                # Condition
                if current_sum > max_sum:
                    max_sum = current_sum
                    top_left = (start, left)
                    bottom_right = (end, right)

    return max_sum, top_left, bottom_right


matrix = [
    [1,2,3,-5,2,-4,-20],
    [-8,-3,2,9,1,0,-2],
    [-9,-2,3,5,1,12,11],
    [-3,8,-15,9,19,12,8],
]

max_sum, _, _ = kadane_2d(matrix)
print(f"Max sum: {max_sum}")
