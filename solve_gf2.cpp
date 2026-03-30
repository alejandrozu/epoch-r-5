#include <iostream>
#include <vector>
#include <bitset>
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

const int MAX_COLS = 20000;

int main() {
    int N = 17, Q = 7, R = 6;
    vector<vector<int>> r_combs, q_combs;
    combinations(N, R, r_combs);
    combinations(N, Q, q_combs);
    
    int num_rows = r_combs.size(); // 12376
    int num_cols = q_combs.size(); // 19448
    
    vector<int> r_idx(1<<N, -1);
    for (int i = 0; i < num_rows; i++) {
        int m = 0;
        for (int x : r_combs[i]) m |= (1 << x);
        r_idx[m] = i;
    }
    
    vector<bitset<MAX_COLS>> A(num_rows);
    for (int j = 0; j < num_cols; j++) {
        int m = 0;
        for (int x : q_combs[j]) m |= (1 << x);
        for (int k = 0; k < N; k++) {
            if ((m >> k) & 1) {
                int r_sub = m ^ (1 << k);
                int r_i = r_idx[r_sub];
                if (r_i != -1) {
                    A[r_i].set(j);
                }
            }
        }
    }
    
    // Set the augmented column to 1
    for (int i = 0; i < num_rows; i++) {
        A[i].set(num_cols);
    }
    
    cout << "Performing Gaussian Elimination..." << endl;
    int r = 0;
    for (int c = 0; c < num_cols && r < num_rows; c++) {
        int pivot = r;
        while (pivot < num_rows && !A[pivot].test(c)) pivot++;
        if (pivot == num_rows) continue;
        
        swap(A[r], A[pivot]);
        for (int i = 0; i < num_rows; i++) {
            if (i != r && A[i].test(c)) {
                A[i] ^= A[r];
            }
        }
        r++;
    }
    
    bool possible = true;
    for (int i = r; i < num_rows; i++) {
        if (A[i].test(num_cols)) {
            possible = false;
            break;
        }
    }
    
    if (possible) {
        cout << "GF(2) system has a solution!" << endl;
    } else {
        cout << "NO solution in GF(2)! Thus no exact cover." << endl;
    }
    
    return 0;
}
