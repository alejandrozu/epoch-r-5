#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <stdio.h>

using namespace std;

// Galois Field arithmetic
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
    // Generate PGL(2, 16)
    vector<vector<int>> temp_perms;
    for (int a = 0; a < 16; a++) {
        for (int b = 0; b < 16; b++) {
            for (int c = 0; c < 16; c++) {
                for (int d = 0; d < 16; d++) {
                    if (add(mult(a, d), mult(b, c)) != 0) {
                        vector<int> perm;
                        for (int z = 0; z < 16; z++) {
                            int denom = add(mult(c, z), d);
                            if (denom == 0) perm.push_back(16);
                            else perm.push_back(div_ff(add(mult(a, z), b), denom));
                        }
                        if (c == 0) perm.push_back(16);
                        else perm.push_back(div_ff(a, c));
                        temp_perms.push_back(perm);
                    }
                }
            }
        }
    }
    set<vector<int>> unique_perms(temp_perms.begin(), temp_perms.end());
    perms.assign(unique_perms.begin(), unique_perms.end());

    cout << "PGL(2,16) size: " << perms.size() << endl;

    vector<uint64_t> r_combs, q_combs;
    for(uint64_t i = 0; i < (1ULL << 17); i++) {
        int cnt = 0;
        for(int j = 0; j < 17; j++) if ((i >> j) & 1) cnt++;
        if (cnt == 6) r_combs.push_back(i);
        if (cnt == 7) q_combs.push_back(i);
    }

    set<uint64_t> r_orbits_set;
    for (auto c : r_combs) r_orbits_set.insert(canonical(c));
    vector<uint64_t> r_orbit_list(r_orbits_set.begin(), r_orbits_set.end());
    map<uint64_t, int> r_orbit_idx;
    for (int i = 0; i < r_orbit_list.size(); ++i) r_orbit_idx[r_orbit_list[i]] = i;

    cout << "r_orbits: " << r_orbit_list.size() << endl;

    set<uint64_t> q_orbits_set;
    for (auto c : q_combs) q_orbits_set.insert(canonical(c));
    vector<uint64_t> q_orbit_list(q_orbits_set.begin(), q_orbits_set.end());
    
    cout << "q_orbits: " << q_orbit_list.size() << endl;

    vector<int> r_orbit_sizes(r_orbit_list.size(), 0);
    for (auto c : r_combs) {
        int idx = r_orbit_idx[canonical(c)];
        r_orbit_sizes[idx]++;
    }

    vector<vector<int>> Y;
    vector<int> Y_original_idx;

    for (int i = 0; i < q_orbit_list.size(); ++i) {
        uint64_t q_orb = q_orbit_list[i];
        set<uint64_t> q_orbit_members;
        for (const auto& p : perms) q_orbit_members.insert(apply_perm(p, q_orb));
        
        map<int, int> covers;
        for (uint64_t q_sub : q_orbit_members) {
            for (int k = 0; k < 17; ++k) {
                if ((q_sub >> k) & 1) {
                    uint64_t r_sub = q_sub ^ (1ULL << k);
                    covers[r_orbit_idx[canonical(r_sub)]]++;
                }
            }
        }
        
        bool valid = true;
        for (auto kv : covers) {
            if (kv.second != r_orbit_sizes[kv.first]) {
                valid = false;
                break;
            }
        }
        if (valid) {
            vector<int> cover_list;
            for (auto kv : covers) cover_list.push_back(kv.first);
            Y.push_back(cover_list);
            Y_original_idx.push_back(i);
        }
    }

    cout << "Valid Y: " << Y.size() << endl;

    FILE* f = fopen("dlx_input.txt", "w");
    fprintf(f, "%d %d\n", (int)r_orbit_list.size(), (int)Y.size());
    for(int i = 0; i < Y.size(); i++) {
        fprintf(f, "%d ", Y_original_idx[i]);
        for(int x : Y[i]) fprintf(f, "%d ", x);
        fprintf(f, "\n");
    }
    fclose(f);
    
    f = fopen("q_orbits.txt", "w");
    for(int i = 0; i < q_orbit_list.size(); i++) {
        fprintf(f, "%llu\n", q_orbit_list[i]);
    }
    fclose(f);

    return 0;
}
