#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <stdio.h>
#include <cstdint>

using namespace std;

int add(int a, int b) { return a ^ b; }
int mult(int a, int b) {
    int p = 0;
    for (int i = 0; i < 4; i++) {
        if (b & 1) p ^= a;
        a <<= 1;
        if (a & 16) a ^= 19;
        b >>= 1;
    }
    return p;
}
int inv(int a) {
    for (int i = 1; i < 16; i++) {
        if (mult(a, i) == 1) return i;
    }
    return 0;
}
int div_ff(int a, int b) {
    if (a == 0) return 0;
    return mult(a, inv(b));
}
// Square root in GF(16): x -> x^8
int sqrt_ff(int a) {
    int x = a;
    for (int i = 0; i < 3; i++) x = mult(x, x);
    return x;
}

vector<vector<int>> perms;

uint64_t apply_perm(const vector<int>& p, uint64_t m) {
    uint64_t res = 0;
    for (int i = 0; i < 17; ++i) {
        if ((m >> i) & 1) res |= (1ULL << p[i]);
    }
    return res;
}

uint64_t canonical(uint64_t m) {
    uint64_t best = m;
    for (const auto& p : perms) {
        uint64_t mapped = apply_perm(p, m);
        if (mapped < best) best = mapped;
    }
    return best;
}

int main() {
    vector<vector<int>> temp_perms;
    for (int a = 0; a < 16; a++) {
        for (int b = 0; b < 16; b++) {
            for (int c = 0; c < 16; c++) {
                for (int d = 0; d < 16; d++) {
                    int det = add(mult(a, d), mult(b, c));
                    if (det != 0) {
                        // PSL check: determinant must be a square in GF(q).
                        // In fields of characteristic 2, EVERY element is a square!
                        // So PSL(2, 16) is actually isomorphic to PGL(2, 16)!
                        // Wait, let's confirm: size of PSL(2, 2^n) is equal to PGL(2, 2^n).
                    }
                }
            }
        }
    }
    return 0;
}
