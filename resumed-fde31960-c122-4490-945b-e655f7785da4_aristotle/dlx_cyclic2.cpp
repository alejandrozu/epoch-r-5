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

void solve(int n, int q, int r) {
    vector<uint64_t> r_combs, q_combs;
    for(uint64_t i = 0; i < (1ULL << n); i++) {
        int cnt = 0;
        for(int j = 0; j < n; j++) if ((i >> j) & 1) cnt++;
        if (cnt == r) r_combs.push_back(i);
        if (cnt == q) q_combs.push_back(i);
    }

    set<uint64_t> r_orbits_set;
    for (auto c : r_combs) r_orbits_set.insert(canonical(c, n));
    vector<uint64_t> r_orbit_list(r_orbits_set.begin(), r_orbits_set.end());
    map<uint64_t, int> r_orbit_idx;
    for (int i = 0; i < r_orbit_list.size(); ++i) r_orbit_idx[r_orbit_list[i]] = i;

    vector<int> r_orbit_sizes(r_orbit_list.size(), 0);
    for(auto c : r_combs) {
        r_orbit_sizes[r_orbit_idx[canonical(c, n)]]++;
    }

    set<uint64_t> q_orbits_set;
    for (auto c : q_combs) q_orbits_set.insert(canonical(c, n));
    vector<uint64_t> q_orbit_list(q_orbits_set.begin(), q_orbits_set.end());

    vector<vector<int>> A;
    for (int i = 0; i < q_orbit_list.size(); ++i) {
        uint64_t q_orb = q_orbit_list[i];
        
        set<uint64_t> orbit_members;
        for(int shift=0; shift<n; shift++) {
            uint64_t mapped = 0;
            for(int j=0; j<n; j++) if((q_orb>>j)&1) mapped |= (1ULL<<((j+shift)%n));
            orbit_members.insert(mapped);
        }
        
        map<int, int> covers;
        for (uint64_t q_sub : orbit_members) {
            for (int k = 0; k < n; ++k) {
                if ((q_sub >> k) & 1) {
                    uint64_t r_sub = q_sub ^ (1ULL << k);
                    covers[r_orbit_idx[canonical(r_sub, n)]]++;
                }
            }
        }
        
        bool valid = true;
        vector<int> row;
        for(auto kv : covers) {
            // Number of times covered is kv.second
            // Each element in orbit kv.first is covered kv.second / r_orbit_sizes[kv.first] times.
            if (kv.second % r_orbit_sizes[kv.first] != 0) { valid = false; break; }
            int times = kv.second / r_orbit_sizes[kv.first];
            if (times > 1) { valid = false; break; }
            if (times == 1) row.push_back(kv.first);
        }
        if (valid) A.push_back(row);
    }

    cout << "n=" << n << " Valid rows: " << A.size() << " / " << q_orbit_list.size() << ", columns: " << r_orbit_list.size() << endl;
    
    // Save to file
    char filename[256];
    sprintf(filename, "dlx_cyclic_%d.txt", n);
    FILE* f = fopen(filename, "w");
    fprintf(f, "%d %d\n", (int)r_orbit_list.size(), (int)A.size());
    for(auto row : A) {
        for(int x : row) fprintf(f, "%d ", x);
        fprintf(f, "\n");
    }
    fclose(f);
}

int main() {
    solve(17, 7, 6);
    solve(19, 7, 6);
    solve(23, 7, 6);
    solve(25, 7, 6);
    return 0;
}
