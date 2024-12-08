import numpy
import time

from collections import Counter

# part 1
with open('left') as f:
    lines = f.read().splitlines()
lc = lines
lc = list(map(int, lc))
list.sort(lc)
print(lc[:5])

with open('right') as f:
    lines = f.read().splitlines()
rc = lines
rc = list(map(int, rc))
list.sort(rc)
print(rc[:5])

absolute_dist = 0
for i in range(len(lc)):
    absolute_dist += abs(lc[i] - rc[i])

print(absolute_dist)

# part 2
rc_counter = Counter(rc)
print(len(rc_counter))

similarity = 0
for idx, val in enumerate(lc):
    try:
        similarity += val * rc_counter[val]
    except:
        pass

print(similarity)
