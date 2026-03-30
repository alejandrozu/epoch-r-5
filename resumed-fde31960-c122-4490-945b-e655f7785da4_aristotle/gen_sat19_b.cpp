#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <map>

using namespace std;

void combinations(int n, int k, vector<vector<int>>& combs) {
    vector<int> c(k);
    for (int i = 0; i < k; ++i) c[i] = i;
    while (true) {
        combs.push_back(c);
        int i = k - 1;
        while (i >= 0 && c[i] == n - k + i) --i;
        if (i < 0) break;
        ++c[i];
        for (int j = i + 1; j < k; ++j) c[j] = c[j - 1] + 1;
    }
}

int main() {
    int N = 19, Q = 7, R = 6;
    vector<vector<int>> r_combs, q_combs;
    combinations(N, R, r_combs);
    combinations(N, Q, q_combs);
    
    vector<int> r_idx(1<<N, -1);
    for (int i = 0; i < r_combs.size(); i++) {
        int m = 0;
        for (int x : r_combs[i]) m |= (1 << x);
        r_idx[m] = i;
    }
    
    vector<vector<int>> covers(r_combs.size());
    for (int i = 0; i < q_combs.size(); i++) {
        int m = 0;
        for (int x : q_combs[i]) m |= (1 << x);
        for (int k = 0; k < N; k++) {
            if ((m >> k) & 1) {
                int r_sub = m ^ (1 << k);
                int col = r_idx[r_sub];
                if (col != -1) {
                    covers[col].push_back(i + 1); // 1-based index for SAT
                }
            }
        }
    }
    
    FILE *f = fopen("s67_19.cnf", "w");
    int num_vars = q_combs.size();
    int num_clauses = covers.size() * 22; // 1 + 21
    fprintf(f, "p cnf %d %d\n", num_vars, num_clauses);
    for (int i = 0; i < covers.size(); i++) {
        for (int x : covers[i]) fprintf(f, "%d ", x);
        fprintf(f, "0\n");
        for (int a = 0; a < covers[i].size(); a++) {
            for (int b = a + 1; b < covers[i].size(); b++) {
                fprintf(f, "-%d -%d 0\n", covers[i][a], covers[i][b]);
            }
        }
    }
    fclose(f);
    cout << "Generated s67_19.cnf" << endl;
    return 0;
}
