#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <random>
#include <chrono>

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
long long nodes_visited = 0;

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

mt19937 rng(42);

void search(int k) {
    if (found) return;
    nodes_visited++;
    if (nodes_visited > 100000) return; // restart
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
            if (min_s <= 1) break;
        }
    }
    
    if (min_s == 0) return;

    vector<Node*> rows;
    for (Node *r = c->D; r != c; r = r->D) rows.push_back(r);
    
    // Shuffle rows for randomization
    shuffle(rows.begin(), rows.end(), rng);

    cover(c);
    for (Node *r : rows) {
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

void solve(const char* filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    int n_cols, n_rows;
    fscanf(f, "%d %d", &n_cols, &n_rows);
    
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
        char line[4096];
        int col;
        char c;
        do {
            if(fscanf(f, "%d%c", &col, &c) != 2) break;
            
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
        } while(c != '\n' && c != '\r' && c != EOF);
    }
    fclose(f);
    
    for(int trial = 0; trial < 1000; trial++) {
        rng.seed(trial);
        found = false;
        solution.clear();
        nodes_visited = 0;
        search(0);
        if (found) {
            cout << "Found solution on trial " << trial << "!" << endl;
            break;
        }
    }
    if (!found) cout << "No solution found." << endl;
}

int main() {
    solve("dlx_cyclic_23.txt");
    return 0;
}
