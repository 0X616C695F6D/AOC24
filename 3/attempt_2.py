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
    lines = f.read()

muls = re.findall(r'mul\((\d+),(\d+)\)', lines) # regex ;_;
#print(muls)

adds = sum(int(a) * int(b) for a, b in muls)
print(adds)