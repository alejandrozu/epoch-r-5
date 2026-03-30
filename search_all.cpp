#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <cstdint>

using namespace std;

void check_params(int n, int q, int r) {
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
        long long ways1 = num / den;
        long long ways2 = num2 / den2;
        if (ways1 % ways2 != 0) return;
    }
    cout << "Valid: n=" << n << ", q=" << q << ", r=" << r << endl;
}

int main() {
    for(int r=6; r<=9; r++) {
        for(int q=r+1; q<=15; q++) {
            for(int n=q+1; n<=100; n++) {
                check_params(n, q, r);
            }
        }
    }
    return 0;
}
