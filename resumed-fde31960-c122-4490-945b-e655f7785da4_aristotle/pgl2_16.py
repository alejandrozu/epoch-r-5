import sys
from itertools import combinations
from collections import defaultdict

# Finite field F_16
# Elements are polynomials in x mod x^4 + x + 1
# Represented as integers 0..15.
# Addition is XOR.
add = lambda a, b: a ^ b

# Multiplication table
mult = [[0]*16 for _ in range(16)]
for i in range(16):
    for j in range(16):
        p = 0
        a = i
        b = j
        for _ in range(4):
            if (b & 1):
                p ^= a
            a <<= 1
            if (a & 16):
                a ^= 19 # x^4 + x + 1
            b >>= 1
        mult[i][j] = p

inv = [0]*16
for i in range(1, 16):
    for j in range(1, 16):
        if mult[i][j] == 1:
            inv[i] = j

def div(a, b):
    if a == 0: return 0
    return mult[a][inv[b]]

# Points are 0..15 and 16 (which is infinity)
# Transform: z -> (az+b)/(cz+d)
perms = []
for a in range(16):
    for b in range(16):
        for c in range(16):
            for d in range(16):
                if add(mult[a][d], mult[b][c]) != 0:
                    perm = []
                    for z in range(16):
                        denom = add(mult[c][z], d)
                        if denom == 0:
                            perm.append(16)
                        else:
                            num = add(mult[a][z], b)
                            perm.append(div(num, denom))
                    # For infinity
                    if c == 0:
                        perm.append(16)
                    else:
                        perm.append(div(a, c))
                    perms.append(perm)

print("PGL(2, 16) size:", len(perms))

def apply_perm(perm, subset):
    return tuple(sorted([perm[x] for x in subset]))

r = 6
q = 7
n = 17

def canonical(subset):
    best = subset
    for p in perms:
        mapped = apply_perm(p, subset)
        if mapped < best:
            best = mapped
    return best

print("Finding r-orbits...")
r_orbits = set()
for c in combinations(range(n), r):
    r_orbits.add(canonical(c))

r_orbit_list = list(r_orbits)
r_orbit_idx = {o: i for i, o in enumerate(r_orbit_list)}
print("r-orbits:", len(r_orbit_list))

print("Finding q-orbits...")
q_orbits = set()
for c in combinations(range(n), q):
    q_orbits.add(canonical(c))

q_orbit_list = list(q_orbits)
print("q-orbits:", len(q_orbit_list))

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
    print(sol)
    
    # Generate the full design from the chosen q_orbits
    blocks = set()
    for q_idx in sol:
        q_orb = q_orbit_list[q_idx]
        for p in perms:
            blocks.add(tuple(sorted([p[x] for x in q_orb])))
            
    with open("design.txt", "w") as f:
        f.write("#17,7,6\n")
        for b in blocks:
            # map to 1-based index
            f.write(" ".join(str(x+1) for x in b) + "\n")
    print("Design written to design.txt")
    sys.exit(0)

print("No solution.")
