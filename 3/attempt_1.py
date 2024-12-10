# Advent of code - Day 3
# Title: Mull it over
# 
# Task 1: Clean string for mult() functions and multiply the two numbers

import re
import numpy

corrupted = """
xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))
"""

with open('input') as f:
    lines = f.read().splitlines()

# i hate regex
muls = [re.findall(r'mul\([^\[\]\{\}]*\)', element) for element in lines]
muls = list(numpy.concatenate(muls).flat)
comma = [re.split(r',+', element) for element in muls]
nums = [re.findall(r'\d+',element) for element in muls]
print(nums)

flat = list(numpy.concatenate(nums).flat)
flat = list(map(int, flat))

mul = 0
for i in range(0, len(flat) - 1, 2):
    mul += flat[i] * flat[i+1]
    #print(f"{flat[i]} {flat[i+1]}")

print(mul)