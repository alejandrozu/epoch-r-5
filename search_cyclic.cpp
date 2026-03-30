#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <cstdint>

using namespace std;

void search_params(int n, int q, int r) {
    cout << "Checking " << n << "," << q << "," << r << endl;
    
    // Check divisibility first
    for(int i=0; i<=r; i++) {
        // compute n-i choose r-i / q-i choose r-i
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
    
    cout << "Parameters valid: " << n << "," << q << "," << r << endl;
}

int main() {
    for(int q=7; q<=10; q++) {
        for(int n=q+1; n<=50; n++) {
            search_params(n, q, 6);
        }
    }
    return 0;
}
