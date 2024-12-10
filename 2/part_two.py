# Day 2 of AOC

import time

"""
Read 'reports' from a file, where reports are a single line.
Read levels, where a level is an element in a report.

Check if each report is:
    1. either all increasing or decreasing,
    2. and the adjacent levels are deltas of 1-3 inclusive
"""

def is_monotonic(report):
    inc = dec = True
    for i in range(1, len(report)):
        if report[i] < report[i-1]:
            inc = False
        if report[i] > report[i-1]:
            dec = False
    return inc or dec


def delta(report, PARITY_BIT):
    ele = 0
    for i, j in zip(report, report[1:]):
        if abs(i-j) > 3 or abs(i-j) < 1:
            ele += 1
            if ele > 1:
                PARITY_BIT += 1
                return 1, PARITY_BIT

    if ele == 1:
        PARITY_BIT += 1

    return 0, PARITY_BIT


def main():
    SAFE = 0

    with open('example') as f:
        lines = f.read().splitlines()

    for i in range(6):
        PARITY_BIT = 0

        report = lines[i]
        report = list(map(int, report.split()))
        print(f"{report}")
        
        monotonic, PARITY_BIT = is_monotonic(report, PARITY_BIT)
        print("Parity bit after monotonic: ", PARITY_BIT)
        offset, PARITY_BIT = delta(report, PARITY_BIT)
        print("Parity bit after offset: ", PARITY_BIT)

        if monotonic and offset == 0 and PARITY_BIT <= 1:
            SAFE += 1

    print(SAFE)

if __name__ == "__main__":
    main()
