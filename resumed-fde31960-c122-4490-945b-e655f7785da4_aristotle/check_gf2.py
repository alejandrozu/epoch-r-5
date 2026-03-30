import sys
import numpy as np

N = 17
Q = 7
R = 6

from itertools import combinations
r_combs = list(combinations(range(N), R))
q_combs = list(combinations(range(N), Q))

r_idx = {c: i for i, c in enumerate(r_combs)}

# Build a sparse matrix or just do GF2 elimination?
# Actually, we can use scipy.sparse
from scipy.sparse import csc_matrix

rows = []
cols = []

for j, q_c in enumerate(q_combs):
    for r_c in combinations(q_c, R):
        rows.append(r_idx[r_c])
        cols.append(j)

import galois
print("Using galois...")
GF2 = galois.GF(2)

# It's a huge matrix 12376 x 19448. galois might be slow.
# Let's write a fast C++ GF2 solver using bitsets!
