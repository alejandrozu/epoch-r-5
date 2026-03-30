#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct Node {
    int row, col;
    Node *L, *R, *U, *D;
};

Node *header;
vector<Node*> col_headers;
vector<int> col_sizes;
vector<int> solution;
bool found = false;

void cover(Node *c) {
    c->R->L = c->L;
    c->L->R = c->R;
    for (Node *i = c->D; i != c; i = i->D) {
        for (Node *j = i->R; j != i; j = j->R) {
            j->D->U = j->U;
            j->U->D = j->D;
            col_sizes[j->col]--;
        }
    }
}

void uncover(Node *c) {
    for (Node *i = c->U; i != c; i = i->U) {
        for (Node *j = i->L; j != i; j = j->L) {
            col_sizes[j->col]++;
            j->D->U = j;
            j->U->D = j;
        }
    }
    c->R->L = c;
    c->L->R = c;
}

void search(int k) {
    if (found) return;
    if (header->R == header) {
        found = true;
        return;
    }
    
    Node *c = header->R;
    int min_s = col_sizes[c->col];
    for (Node *j = c->R; j != header; j = j->R) {
        if (col_sizes[j->col] < min_s) {
            min_s = col_sizes[j->col];
            c = j;
            if (min_s == 0) break; // Optimization
        }
    }
    
    if (min_s == 0) return;

    cover(c);
    for (Node *r = c->D; r != c; r = r->D) {
        solution.push_back(r->row);
        for (Node *j = r->R; j != r; j = j->R) {
            cover(col_headers[j->col]);
        }
        search(k + 1);
        if (found) return;
        solution.pop_back();
        for (Node *j = r->L; j != r; j = j->L) {
            uncover(col_headers[j->col]);
        }
    }
    uncover(c);
}

// Generate combinations
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
    int N = 17, Q = 7, R = 6;
    vector<vector<int>> r_combs, q_combs;
    combinations(N, R, r_combs);
    combinations(N, Q, q_combs);
    
    int n_cols = r_combs.size();
    int n_rows = q_combs.size();
    cout << "Cols (r-subsets): " << n_cols << ", Rows (q-subsets): " << n_rows << endl;
    
    // Create map from r-subset to col index
    // Using bitmasks since n=17
    vector<int> r_idx(1<<N, -1);
    for (int i = 0; i < n_cols; i++) {
        int m = 0;
        for (int x : r_combs[i]) m |= (1 << x);
        r_idx[m] = i;
    }
    
    header = new Node{-1, -1, nullptr, nullptr, nullptr, nullptr};
    header->L = header->R = header;
    col_headers.resize(n_cols);
    col_sizes.assign(n_cols, 0);
    
    for (int i = 0; i < n_cols; i++) {
        Node *c = new Node{-1, i, nullptr, nullptr, nullptr, nullptr};
        c->U = c->D = c;
        c->L = header->L;
        c->R = header;
        header->L->R = c;
        header->L = c;
        col_headers[i] = c;
    }
    
    for (int i = 0; i < n_rows; i++) {
        Node *row_start = nullptr;
        int m = 0;
        for (int x : q_combs[i]) m |= (1 << x);
        
        for (int k = 0; k < N; k++) {
            if ((m >> k) & 1) {
                int r_sub = m ^ (1 << k);
                int col = r_idx[r_sub];
                if (col == -1) continue;
                
                Node *n = new Node{i, col, nullptr, nullptr, nullptr, nullptr};
                n->D = col_headers[col];
                n->U = col_headers[col]->U;
                col_headers[col]->U->D = n;
                col_headers[col]->U = n;
                col_sizes[col]++;
                
                if (!row_start) {
                    row_start = n;
                    n->L = n->R = n;
                } else {
                    n->L = row_start->L;
                    n->R = row_start;
                    row_start->L->R = n;
                    row_start->L = n;
                }
            }
        }
    }
    
    cout << "Starting search..." << endl;
    search(0);
    
    if (found) {
        cout << "Found solution!" << endl;
        FILE *f = fopen("design.txt", "w");
        fprintf(f, "#17,7,6\n");
        for (int i : solution) {
            for (int x : q_combs[i]) {
                fprintf(f, "%d ", x + 1);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    } else {
        cout << "No solution." << endl;
    }
    return 0;
}
