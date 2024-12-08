# Day 2 of AOC

import time

"""
Read 'reports' from a file, where reports are a single line.
Read levels, where a level is an element in a report.

Check if each report is:
    1. either all increasing or decreasing,
    2. and the adjacent levels are deltas of 1-3 inclusive
"""

UNSAFE = 0

# Open and read all lines
with open('input') as f:
    lines = f.read().splitlines()
