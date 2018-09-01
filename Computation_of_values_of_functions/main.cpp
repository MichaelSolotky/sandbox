#include <iostream>
#include <gmpxx.h>
#include <gmp.h>
#include <mpfr.h>
#include <cmath>
#include <ctime>


template <typename T>
inline int __attribute((fastcall)) delete_array(const int res, T arr) {
    delete[] arr;
    return res;
}

inline int __attribute((fastcall)) integer_logarithm(const int a, const mpz_class &x) {
    // returns the integer patr of an real logarithm
    if (x <= 1) {
        return 0;
    mpz_class tmp = a, tmp1;
    mpz_class *arr = new mpz_class[100]; // it has to be enough
    int cur_pow = 1, result, cnt = 1;
    arr[0] = 1;
    do {
        arr[cnt++] = tmp;
        tmp *= tmp;
        cur_pow <<= 1;
    } while(tmp < x);
    if (tmp == x) {
        return delete_array(cur_pow, arr);
    }
    result = (cur_pow >>= 1);
    cur_pow >>= 1;
    tmp = arr[--cnt];
    for (int i = --cnt; i >= 0; cur_pow >>= 1) {
        tmp1 = arr[i--];
        tmp *= tmp1;
        if (tmp > x) {
            tmp /= tmp1;
        } else {
            result += cur_pow;
        }
    }
    return delete_array(result, arr);
}

inline void __attribute((fastcall)) integer_power(mpf_class &result, int a, int n) {
    result = 1;
    mpz_class x = a;
    while(n) {
        if(n & 1) {
            result *= x;
            n--;
        }
        x *= x;
        n /= 2;
    }
}

inline void __attribute((fastcall)) mpf_sqrt(mpf_class &result, const mpf_class &x, const int n) {
    // n == how many times to repeat
    result = 1; // how to find a starting value and how to estimate precision
    for (int i = 0; i < n; i++) {
        result = (result + x / result) / 2;
    }
}

inline void __attribute((fastcall)) arithmetic_geometric_mean(mpf_class &result,
                                                              const int x,
                                                              const mpf_class &y,
                                                              const int repeat) {
    mpf_class a, b, tmp;
    a = x;
    b = y;
    for (int i = 0; i < repeat; i++) {
        tmp = a;
        a = (a + b) / 2;
        mpf_sqrt(b, tmp, 100); // change to 10
    }
    result = a;
}

// real result differs from this not more than by 2 to the power -presison
inline void __attribute((fastcall)) ln(mpf_class &result, const mpz_class &x, int precision) {
    mpf_class tmp, s;
    integer_power(tmp, 2, precision + (precision % 2));
    tmp /= x;
    mpz_class tmp1 = (mpz_class) tmp + 1;
    int m = integer_logarithm(2, tmp1) + 1;
    integer_power(tmp, 2, m);
    tmp *= x;
    s = 4 / tmp;
    arithmetic_geometric_mean(tmp, 1, s, 100); // how many times to repeat
    result = M_PI / tmp / 2;
    result -= m * M_LN2;
}

int main(int argc, char * const argv[]) {
    #ifdef TIME
        time_t tm = clock();
    #endif
    mpz_class number;
    number = argv[1];
    mpf_class res;

    mpf_class tmp = 1.0 / 6;
    arithmetic_geometric_mean(res, 1, 1 / 6, 100);
    std::cout << res << endl;

    ln(res, number, atoi(argv[2]));
    std::cout << "ln = " << res << endl;
    #ifdef TIME
        tm = clock() - tm;
        printf("working time = %g s\n", (double) tm / 1000000);
    #endif
    return 0;
}
