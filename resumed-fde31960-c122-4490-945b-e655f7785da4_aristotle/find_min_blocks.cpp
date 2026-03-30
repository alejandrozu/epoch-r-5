#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    long long min_blocks = 1e18;
    int best_n=0, best_q=0, best_r=0;
    
    for(int r=6; r<=9; r++) {
        for(int q=r+1; q<=15; q++) {
            for(int n=q+1; n<=200; n++) {
                bool valid = true;
                long long blocks = 1;
                for(int i=0; i<=r; i++) {
                    long long num = 1, den = 1;
                    for(int j=0; j<r-i; j++) {
                        num *= (n - i - j);
                        den *= (r - i - j);
                    }
                    long long num2 = 1, den2 = 1;
                    for(int j=0; j<r-i; j++) {
                        num2 *= (q - i - j);
                        den2 *= (r - i - j);
                    }
                    if (num2 == 0) { valid = false; break; }
                    long long ways1 = num / den;
                    long long ways2 = num2 / den2;
                    if (ways1 % ways2 != 0) { valid = false; break; }
                    if (i == 0) blocks = ways1 / ways2;
                }
                if (valid) {
                    cout << "Valid: n=" << n << ", q=" << q << ", r=" << r << " -> blocks=" << blocks << endl;
                    if (blocks < min_blocks) {
                        min_blocks = blocks;
                        best_n = n; best_q = q; best_r = r;
                    }
                }
            }
        }
    }
    cout << "Minimum blocks: " << min_blocks << " for " << best_n << "," << best_q << "," << best_r << endl;
    return 0;
}
