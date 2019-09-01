#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>

using namespace std;

int main() {
    long long n;
    bool is_prime = true;
    cin >> n;
    if(!n || n == 1) {
        cout << "undefined";
        return 0;
    }
    int limit = (int) sqrt(n + 0.0);
    const int SQRT_MAX_N = 1000000000;
    const int S = 10000;
    int cnt = 0;
    bool *n_prime = (bool *) malloc(SQRT_MAX_N), *bl = (bool *) malloc(S);
    int *primes = (int *) malloc(SQRT_MAX_N * sizeof(int));
    int n_sqrt = (int) sqrt (limit + 0.0);
    for (int i = 2; i <= n_sqrt; ++i)
        if (!n_prime[i]) {
            primes[cnt++] = i;
            if (i * 1ll * i <= n_sqrt)
                for (int j = i * i; j <= n_sqrt; j += i)
                    n_prime[j] = true;
        }
    int result = 0;
    for (int k = 0, max_k = limit / S; k <= max_k; ++k) {
        memset (bl, 0, S);
        int start = k * S;
        for (int i = 0; i < cnt; ++i) {
            int start_idx = (start + primes[i] - 1) / primes[i];
            int j = max(start_idx, 2) * primes[i] - start;
            for (; j < S; j += primes[i])
                bl[j] = true;
        }
        if (!k)
            bl[0] = bl[1] = true;
        for (int i = 0; i < S && start + i <= limit; ++i)
            if (!bl[i]) {
                cout << i + k * S << " ";
                result++;
            }
        cout << endl;
    }
    free(n_prime), free(bl), free(primes);
    cout << result << endl;
    return 0;
}