#include <iostream>
#include <gmp.h>

using namespace std;

int main() {
    mpz_t a, b, c;
    mpz_inits(a, b, c, NULL);
    mpz_set_str(a, "8", 10);
    mpz_set_str(b, "9", 10);
    mpz_add(c, a, b);
    cout << "a + b = ";
    mpz_out_str(stdout, 10, c);
    cout << endl;
    return 0;
}