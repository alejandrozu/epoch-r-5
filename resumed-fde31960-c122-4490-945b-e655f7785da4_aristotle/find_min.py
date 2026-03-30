import math

min_blocks = float('inf')
best = None

for r in range(6, 10):
    for q in range(r+1, 20):
        for n in range(q+1, 200):
            valid = True
            for i in range(r+1):
                if math.comb(n-i, r-i) % math.comb(q-i, r-i) != 0:
                    valid = False
                    break
            if valid:
                blocks = math.comb(n, r) // math.comb(q, r)
                print(f"Valid: n={n}, q={q}, r={r} -> blocks={blocks}")
                if blocks < min_blocks:
                    min_blocks = blocks
                    best = (n, q, r)

print(f"Minimum blocks: {min_blocks} for {best}")
