import sys

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

n = 19
q = 7
r = 6

from itertools import combinations

def canonical(comb):
    # Find lexicographically smallest shift
    best = tuple(sorted(comb))
    for shift in range(1, n):
        shifted = tuple(sorted([(x + shift) % n for x in comb]))
        if shifted < best:
            best = shifted
    return best

# Generate orbits of r-subsets
r_orbits = set()
for comb in combinations(range(n), r):
    r_orbits.add(canonical(comb))

r_orbit_list = list(r_orbits)
r_orbit_idx = {o: i for i, o in enumerate(r_orbit_list)}

print("Number of r-orbits:", len(r_orbit_list))

# Generate orbits of q-subsets
q_orbits = set()
for comb in combinations(range(n), q):
    q_orbits.add(canonical(comb))

q_orbit_list = list(q_orbits)
print("Number of q-orbits:", len(q_orbit_list))

# For each q-orbit, what r-orbits does it cover?
Y = {}
for i, q_orb in enumerate(q_orbit_list):
    # Check if this q-orbit has full length (n)
    # A full length orbit covers exactly n elements
    orbit_size = len(set(tuple(sorted((x + s) % n for x in q_orb)) for s in range(n)))
    if orbit_size != n:
        continue
        
    covers = []
    # Take the canonical q-orbit representative, find its r-subsets
    for r_sub in combinations(q_orb, r):
        r_can = canonical(r_sub)
        covers.append(r_orbit_idx[r_can])
        
    # We must ensure that a q-orbit doesn't cover the SAME r-orbit multiple times
    if len(set(covers)) == len(covers):
        Y[i] = covers

print("Valid q-orbits:", len(Y))

X = list(range(len(r_orbit_list)))

print("Starting DLX...")
solutions = solve_exact_cover(X, Y)
for i, sol in enumerate(solutions):
    print("Found solution!")
    print(sol)
    sys.exit(0)
print("No solution found.")
