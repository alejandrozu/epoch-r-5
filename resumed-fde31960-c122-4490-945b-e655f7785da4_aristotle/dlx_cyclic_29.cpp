#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <cstdint>

using namespace std;

uint64_t canonical(uint64_t m, int n) {
    uint64_t best = m;
    for (int shift = 1; shift < n; ++shift) {
        uint64_t mapped = 0;
        for (int i = 0; i < n; ++i) {
            if ((m >> i) & 1) mapped |= (1ULL << ((i + shift) % n));
        }
        if (mapped < best) best = mapped;
    }
    return best;
}

int main() {
    int n = 29, q = 8, r = 6;
    vector<uint64_t> r_combs, q_combs;
    for(uint64_t i = 0; i < (1ULL << n); i++) {
        int cnt = 0;
        for(int j = 0; j < n; j++) if ((i >> j) & 1) cnt++;
        if (cnt == r) r_combs.push_back(i);
        if (cnt == q) q_combs.push_back(i);
    }
    cout << "Generated combinations." << endl;
    return 0;
}
