# Day 2 of AOC

import time

"""
Read 'reports' from a file, where reports are a single line.
Read levels, where a level is an element in a report.

Check if each report is:
    1. either all increasing or decreasing,
    2. and the adjacent levels are deltas of 1-3 inclusive
"""

TOTAL_LINES = 1000 # In the input file

# Is report all increasing or decreasing? Monotonic
def is_monotonic(report):
    inc = dec = True
    for i in range(1, len(report)):
        if report[i] < report[i-1]:
            inc = False
        if report[i] > report[i-1]:
            dec = False
    return inc or dec

def delta(report):
    for i, j in zip(report, report[1:]):
        if abs(i-j) > 3:
            return 1
        elif abs(i-j) < 1:
            return 1
    return 0

# Open and read all lines
with open('input') as f:
    lines = f.read().splitlines()

def main():
    SAFE = 0
    for i in range(TOTAL_LINES - 1):
        #time.sleep(1)

        report = lines[i+1]
        report = list(map(int, report.split()))

        offset = delta(report)
        if is_monotonic(report) and offset == 0:
            SAFE += 1

    print(SAFE)

if __name__ == "__main__":
    main()
