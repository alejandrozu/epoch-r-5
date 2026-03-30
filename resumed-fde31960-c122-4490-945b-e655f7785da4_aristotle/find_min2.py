import math

res = []
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
                res.append((blocks, n, q, r))

res.sort()
for r in res[:20]:
    print(r)
