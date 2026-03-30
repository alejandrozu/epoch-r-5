import sys
from itertools import combinations
from collections import defaultdict

n = 17
q = 7
r = 6

def apply_perm(perm, subset):
    return tuple(sorted([perm[x] for x in subset]))

# Generate AGL(1, 17)
perms = []
for a in range(1, 17):
    for b in range(17):
        p = [(a * x + b) % 17 for x in range(17)]
        perms.append(p)

print("Group size:", len(perms))

def canonical(subset):
    best = subset
    for p in perms:
        mapped = apply_perm(p, subset)
        if mapped < best:
            best = mapped
    return best

r_orbits = set()
for c in combinations(range(n), r):
    r_orbits.add(canonical(c))

print("r_orbits size:", len(r_orbits))

q_orbits = set()
for c in combinations(range(n), q):
    q_orbits.add(canonical(c))

print("q_orbits size:", len(q_orbits))

