#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <cstdint>

using namespace std;

int N, Q, R;

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
    N = n; Q = q; R = r;
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

    set<uint64_t> q_orbits_set;
    for (auto c : q_combs) q_orbits_set.insert(canonical(c, n));
    vector<uint64_t> q_orbit_list(q_orbits_set.begin(), q_orbits_set.end());

    vector<vector<int>> A;
    for (int i = 0; i < q_orbit_list.size(); ++i) {
        uint64_t q_orb = q_orbit_list[i];
        // Check orbit size
        set<uint64_t> orbit_members;
        for(int shift=0; shift<n; shift++) {
            uint64_t mapped = 0;
            for(int j=0; j<n; j++) if((q_orb>>j)&1) mapped |= (1ULL<<((j+shift)%n));
            orbit_members.insert(mapped);
        }
        if (orbit_members.size() != n) continue; // Only full orbits
        
        map<int, int> covers;
        for (uint64_t q_sub : orbit_members) {
            // Find all r-subsets
            vector<int> elems;
            for (int k = 0; k < n; ++k) if ((q_sub >> k) & 1) elems.push_back(k);
            
            // Generate r-combinations of this q-subset
            // Since q is small, just iterate
            for(uint64_t m = 0; m < (1ULL << q); m++) {
                int c2 = 0;
                for(int x=0; x<q; x++) if((m>>x)&1) c2++;
                if (c2 == r) {
                    uint64_t r_sub = 0;
                    for(int x=0; x<q; x++) if((m>>x)&1) r_sub |= (1ULL << elems[x]);
                    covers[r_orbit_idx[canonical(r_sub, n)]]++;
                }
            }
        }
        
        bool valid = true;
        vector<int> row;
        for(int j=0; j<r_orbit_list.size(); j++) {
            if (covers[j] > 1) valid = false; // We can't cover an orbit more than its size / orbit_size. Wait, if it's full orbit, the orbit size is n. The number of times it covers an r-orbit must be proportional to r-orbit size, which is n. So each r-orbit is covered exactly `covers[j] / (n / n)` = `covers[j]` times when taking ONE full q-orbit. We want exact cover, so we need `covers[j] \in {0, 1}`.
            if (covers[j] == 1) row.push_back(j);
        }
        if (valid) A.push_back(row);
    }

    cout << "n=" << n << " Valid rows: " << A.size() << " / " << q_orbit_list.size() << ", columns: " << r_orbit_list.size() << endl;
    
    // Exact cover check would be here
}

int main() {
    solve(17, 7, 6);
    solve(19, 7, 6);
    solve(23, 7, 6);
    return 0;
}
