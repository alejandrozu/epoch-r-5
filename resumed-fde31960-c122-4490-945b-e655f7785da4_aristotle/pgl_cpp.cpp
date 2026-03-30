#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

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

int main() {
    vector<vector<int>> perms;
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
                        perms.push_back(perm);
                    }
                }
            }
        }
    }
    cout << "PGL(2, 16) size: " << perms.size() << endl;

    // The group is too large if we just generated it like this?
    // Wait, the number of matrices is 16^4 = 65536. Non-singular is 16^3-16 = 4080.
    // The permutations generated will have duplicates because scalar multiples give same fractional linear transformation.
    // Let's unique them.
    set<vector<int>> unique_perms(perms.begin(), perms.end());
    perms.assign(unique_perms.begin(), unique_perms.end());
    cout << "Unique PGL(2, 16) size: " << perms.size() << endl;

    return 0;
}
