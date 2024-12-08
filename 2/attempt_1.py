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
#print(lines[:1])

for i in range(len(lines) - 1):
    time.sleep(1)
    report = lines[i+1]
    report = list(map(int, report.split()))
    #print(report)
    if (report[0] > report[1]):
        lock = 1
        while (lock):
            for idx, _ in enumerate(report):
                if idx == 0:
                    #print("first pass")
                    continue
                if report[idx] < report[idx-1]:
                    #print(f"OK: {report[idx]} < {report[idx-1]}")
                    if report[idx-1] - report[idx] > 3:
                        print(f"SKIP: {report[idx-1]-report[idx]} > {report}")
                        UNSAFE += 1
                        lock = 0
                        break
                else:
                    #print(f"UNSAFE: NOT LESS THAN -> {report[idx]} > {report[idx-1]} -> {report}")
                    print(f"UNSAFE: {report}")
                    UNSAFE += 1
                    lock = 0
                    break
            lock = 0:
    elif (report[0] < report[1]):
        lock = 1
        while (lock):
            for idx, _ in enumerate(report):
                if idx == 0:
                    #print("first pass")
                    continue
                if report[idx] > report[idx-1]:
                    #print(f"OK: {report[idx]} < {report[idx-1]}")
                    if report[idx-1] - report[idx] > 3:
                        print(f"SKIP: {report[idx-1]-report[idx]} -> {report}")
                        UNSAFE += 1
                        lock = 0
                        break
                else:
                    #print(f"UNSAFE: NOT LESS THAN -> {report[idx]} < {report[idx-1]} -> {report}")
                    print(f"UNSAFE: {report}")
                    UNSAFE += 1
                    lock = 0
                    break
            lock = 0:

print(1000-UNSAFE)
