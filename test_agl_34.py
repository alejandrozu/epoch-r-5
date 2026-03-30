import sys
from itertools import combinations
from collections import defaultdict

n = 17
q = 7
r = 6

# AGL(1, 17) subgroup of size 34
eighth_powers = [1, 16]
perms = []
for a in eighth_powers:
    for b in range(17):
        p = [(a * x + b) % 17 for x in range(17)]
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

q_orbits = set()
for c in combinations(range(n), q):
    q_orbits.add(canonical(c))
q_orbit_list = list(q_orbits)

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
        if count != r_orbit_sizes[r_idx]:
            valid = False
            break
            
    if valid:
        Y[i] = list(covers.keys())

print("Valid Y sizes:", len(Y))

def solve_exact_cover(X, Y):
    X = {j: set() for j in X}
    for i, row in Y.items():
        for j in row:
            X[j].add(i)

    def solve(X, Y, solution):
        if not X:
            yield list(solution)
        else:
            c = min(X, key=lambda c: len(X[c]))
            for r in list(X[c]):
                solution.append(r)
                cols = solve_select(X, Y, r)
                yield from solve(X, Y, solution)
                solve_deselect(X, Y, r, cols)
                solution.pop()

    def solve_select(X, Y, r):
        cols = []
        for j in Y[r]:
            for i in X[j]:
                for k in Y[i]:
                    if k != j:
                        X[k].remove(i)
            cols.append(X.pop(j))
        return cols

    def solve_deselect(X, Y, r, cols):
        for j in reversed(Y[r]):
            X[j] = cols.pop()
            for i in X[j]:
                for k in Y[i]:
                    if k != j:
                        X[k].add(i)

    yield from solve(X, Y, [])

X = list(range(len(r_orbit_list)))
print("Starting DLX...")
for sol in solve_exact_cover(X, Y):
    print("Found solution!")
    sys.exit(0)
print("No solution.")
