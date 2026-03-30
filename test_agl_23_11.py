import sys
from itertools import combinations
from collections import defaultdict

n = 23
q = 7
r = 6

# AGL(1, 23) subgroup of size 11*23 = 253 (squares mod 23)
squares = [(x*x) % 23 for x in range(1, 12)]
perms = []
for a in squares:
    for b in range(23):
        p = [(a * x + b) % 23 for x in range(23)]
        perms.append(p)

print("Group size:", len(perms))

def canonical(subset):
    best = tuple(sorted(subset))
    for p in perms:
        mapped = tuple(sorted([p[x] for x in subset]))
        if mapped < best:
            best = mapped
    return best

r_orbits = set()
for c in combinations(range(n), r):
    r_orbits.add(canonical(c))
r_orbit_list = list(r_orbits)
r_orbit_idx = {o: i for i, o in enumerate(r_orbit_list)}
print("r_orbits size:", len(r_orbits))

q_orbits = set()
for c in combinations(range(n), q):
    q_orbits.add(canonical(c))
q_orbit_list = list(q_orbits)
print("q_orbits size:", len(q_orbits))

r_orbit_sizes = [0]*len(r_orbit_list)
for c in combinations(range(n), r):
    idx = r_orbit_idx[canonical(c)]
    r_orbit_sizes[idx] += 1

Y = {}
for i, q_orb in enumerate(q_orbit_list):
    q_orbit_set = set()
    for p in perms:
        q_orbit_set.add(tuple(sorted([p[x] for x in q_orb])))
        
    covers = defaultdict(int)
    for q_sub in q_orbit_set:
        for r_sub in combinations(q_sub, r):
            r_can = canonical(r_sub)
            covers[r_orbit_idx[r_can]] += 1
            
    valid = True
    for r_idx, count in covers.items():
        if count % r_orbit_sizes[r_idx] != 0:
            valid = False
            break
        if count // r_orbit_sizes[r_idx] > 1:
            valid = False
            break
            
    if valid:
        Y[i] = [k for k, v in covers.items() if v == r_orbit_sizes[k]]

print("Valid Y sizes:", len(Y))

# Output exact cover problem
with open("dlx_23_11.txt", "w") as f:
    f.write(f"{len(r_orbit_list)} {len(Y)}\n")
    for i, row in Y.items():
        for x in row:
            f.write(f"{x} ")
        f.write("\n")

