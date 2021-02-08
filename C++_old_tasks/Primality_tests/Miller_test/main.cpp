#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <cmath>

#define len_of_cell 9
#define sieve_limit_for_trivial 256

// This code is written in C++, but in C style (it was long time ago,
// when I was better in C than in C++)

typedef struct res_of_sieve {
    int *sieve, num_of_primes;
} res_of_sieve;

void convert_string_into_vector(std::vector<int> &number, char *string) {
    int len = static_cast<int>(strlen(string));
    for (int i = len; i > 0; i -= len_of_cell) {
        string[i] = 0;
        number.push_back(atoi(i >= len_of_cell ? string + i - len_of_cell : string));
    }
    int size = static_cast<int>(number.size());
    while (size > 1 && !number[size - 1]) {
        number.pop_back();
        --size;
    }
}

int cmp_of_longs(const std::vector<int> &a, const std::vector<int> &b) {
    int size1 = (int) a.size(), size2 = static_cast<int>(b.size());
    if (size1 == size2) {
        for (int i = size1 - 1; i >= 0; i--) {
            if (a[i] > b[i]) {
                return 1;
            } else if (a[i] < b[i]) {
                return -1;
            }
        }
        return 0;
    } else {
        return size1 - size2;
    }
}

void long_sum(std::vector<int> &a, const std::vector<int> &b) {
    if ((b.size() == 1) && ((a[0] % 10 + b[0]) < 10)) {
        a[0] += b[0];
    } else {
        // Carry is a number which is 1 when arithmetical carry has happened and 0 else
        int carry = 0, base = 1000 * 1000 * 1000;
        for (int i = 0; i < std::max(a.size(), b.size()) || carry; ++i) {
            if (i == a.size()) {
                a.push_back (0);
            }
            a[i] += carry + (i < b.size() ? b[i] : 0);
            carry = a[i] >= base;
            if (carry) {
                a[i] -= base;
            }
        }
    }
}

void long_sub(std::vector<int>& a, const std::vector<int>& b) {
    // doesn't work correctly if a < b
    if ((b.size() == 1) && (b[0] <= (a[0] % 10))) {
        a[0] -= b[0];
    } else {
        int carry = 0, base = 1000 * 1000 * 1000;
        for (int i = 0; i < b.size() || carry; ++i) {
            a[i] -= carry + (i < b.size() ? b[i] : 0);
            carry = a[i] < 0;
            if (carry) {
                a[i] += base;
            }
        }
        unsigned size = (unsigned) a.size();
        while (size > 1 && !a[size - 1]) {
            a.pop_back();
            --size;
        }
    }
}

void long_mul_short(std::vector<int> &a, const int b) {
    int carry = 0, base = 1000 * 1000 * 1000;
    for (int i = 0; i < a.size() || carry; ++i) {
        if (i == a.size()) {
            a.push_back(0);
        }
        long long cur = carry + a[i] * 1ll * b;
        a[i] = static_cast<int>(cur % base);
        carry = static_cast<int>(cur / base);
    }
    int size = (int) a.size();
    while (size > 1 && !a.back()) {
        a.pop_back();
        --size;
    }
}

std::vector<int> long_mul_long(const std::vector<int> &a, const std::vector<int> &b) {
    int base = 1000 * 1000 * 1000, size1 = (int) a.size(), size2 = (int) b.size();
    std::vector<int> c(a.size() + b.size());
    for (int i = 0; i < size1; ++i)
        for (int j = 0, carry = 0; j < size2 || carry; ++j) {
            long long cur = c[i + j] + a[i] * 1ll * (j < size2 ? b[j] : 0) + carry;
            c[i + j] = static_cast<int>(cur % base);
            carry = static_cast<int>(cur / base);
        }
    int size3 = static_cast<int>(c.size());
    while (size3 > 1 && !c[size3 - 1]) {
        --size3;
        c.pop_back();
    }
    return c;
}

int long_div_short(std::vector<int> &number, const int divisor) {
    // cannot divide on numbers longer than 1 milliard
    int carry = 0;
    const int base = 1000 * 1000 * 1000;
    int size = (int) number.size();
    for (int i = size - 1; i >= 0; i--) {
        long long cur = number[i] + carry * 1ll * base;
        number[i] = static_cast<int>(cur / divisor);
        carry = static_cast<int>(cur % divisor);
    }
    while (size > 1 && !number.back()) {
        number.pop_back();
        --size;
    }
    return carry; // makes a new vector
}

int binary_logarithm(const std::vector<int> &num) { // calculates correctly if num >= 2
    int res = 1;
    std::vector<int> tmp(1);
    tmp[0] = 2;
    while (true) {
        if ((tmp.size() >= num.size()) && (cmp_of_longs(tmp, num) >= 0)) {
            break;
        }
        long_mul_short(tmp, 2);
        ++res;
    }
    return res;
}

int decimal_logarithm(const std::vector<int>& num) {
    int n = num.back();
    int cnt = 0;
    while (n) {
        n /= 10;
        ++cnt;
    }
    return static_cast<int>(num.size() - 1) * len_of_cell + cnt;
}

std::vector<int> long_div_long(std::vector<int> dividend,
                               std::vector<int> divisor) {
    int len_of_divisor = decimal_logarithm(divisor), len_of_dividend = decimal_logarithm(dividend);
    if (len_of_dividend < len_of_divisor) {
        return dividend;
    }
    std::vector<int> tmp(1), tmp2(1), tmp3(1), result(1);
    tmp[0] = result[0] = 0;
    int size_of_dividend;
    int cnt = size_of_dividend = (int) dividend.size(), cell = 0, power;
    size_of_dividend--;
    int place_for_number = len_of_dividend + (len_of_dividend % 9 ? 9 - (len_of_dividend % 9) : 0);
    for (int i = 0; i < place_for_number; i++) {
        if (!(i % 9)) {
            --cnt;
            cell = dividend[cnt];
        }
        tmp3[0] = cell;
        cnt == size_of_dividend ? power = decimal_logarithm(tmp3) - (i % 9) : power = 9 - (i % 9);
        if (power <= 0) continue;
        tmp2[0] = (cell % (int) pow(10, power)) / ((int) pow(10, power - 1));
        long_mul_short(tmp, 10);
        long_sum(tmp, tmp2);
        int cnt1 = 0;
        while (cmp_of_longs(tmp, divisor) >= 0) {
            ++cnt1;
            long_sub(tmp, divisor);
        }
        tmp2[0] = cnt1;
        long_mul_short(result, 10);
        long_sum(result, tmp2);
    }
    return tmp;
}

void long_pow(std::vector<int> &res, std::vector<int> a, std::vector <int> b) {
    while (b.size() > 1 || b[0]) {
        if (b[0] & 1) {
            res = long_mul_long(res, a);
        }
        a = long_mul_long(a, a);
        long_div_short(b, 2);
    }
}

std::vector <int> long_pow_mod_n(std::vector<int> a,
                                 std::vector<int> b,
                                 std::vector<int> number) {
    std::vector<int> res(1);
    res[0] = 1;
    while (b.size() > 1 || b[0]) {
        if (b[0] & 1) {
            res = long_mul_long(res, a);
            res = long_div_long(res, number);
        }
        a = long_mul_long(a, a);
        a = long_div_long(a, number);
        long_div_short(b, 2);
    }
    return res;
}

res_of_sieve Sieve_of_Eratosthenes(int len_of_sieve, int num_of_primes) {
    len_of_sieve++;
    int *sieve = (int *) calloc((size_t) len_of_sieve + 1, sizeof(int));
    for (int i = 2; i <= len_of_sieve; i++) {
        if (!sieve[i]) {
            for (int j = 2 * i; j < len_of_sieve; j += i) {
                sieve[j] = 1;
            }
        }
    }
    int *res_arr = (int *) calloc((size_t) num_of_primes, sizeof(int));
    int j = 2, i = 0;
    for (; (i < num_of_primes) && (j <= len_of_sieve); j++) {
        if (!sieve[j]) {
            res_arr[i] = j;
            i++;
        }
    }
    free(sieve);
    res_of_sieve res;
    res.num_of_primes = i;
    res.sieve = res_arr;
    return res; // returns an array with length equal to num of primes
}

// In the most cases the number is really composite and has trivial divisors like 3 or 5
// so this function checks if the number has trivial divisors
bool trivial_div(const std::vector<int>& number) {
    // returns false if divisor was found
    bool flag = true;
    const int trivial_limit = 54;
    int *divisors = Sieve_of_Eratosthenes(sieve_limit_for_trivial, trivial_limit).sieve;
    std::vector<int> tmp;
    for (int i = 0; i < trivial_limit; i++) {
        tmp = number;
        if (!(long_div_short(tmp, divisors[i])) && !((number.size() == 1) && (number[0] == divisors[i]))) {
            flag = false;
            break;
        }
    }
    free(divisors);
    return flag;
}

// Check if number is a power of prime
bool is_pow(const std::vector<int>& number, int log_2) {
    std::vector<int> tmp(1), tmp2(1), tmp3(1);
    tmp[0] = 257, tmp2[0] = 1;
    int log_10 = decimal_logarithm(number);
    std::vector<int> t10(1);
    t10[0] = 10;
    tmp3[0] = log_10 / log_2 + 1;
    std::vector<int> res(1);
    res[0] = 1;
    long_pow(res, t10, tmp3);
    t10 = res;
    std::vector<int> left_border = tmp, right_border = t10;
    for (int i = log_2 / 8; i > 1; --i) {
        while (true) {
            tmp = right_border;
            long_sum(tmp, left_border);
            long_div_short(tmp, 2);
            tmp3[0] = i;
            std::vector<int> tmp1(1);
            tmp1[0] = 1;
            long_pow(tmp1, tmp, tmp3);
            int res_of_comparison = cmp_of_longs(tmp1, number);
            if (!res_of_comparison) {
                left_border.clear();
                right_border.clear();
                tmp1.clear();
                return true;
            }
            if (res_of_comparison > 0) {
                right_border = tmp;
                long_sub(right_border, tmp2);
            }
            if (res_of_comparison < 0) {
                left_border = tmp;
                long_sum(left_border, tmp2);
            }
            tmp1.clear();
            if (cmp_of_longs(left_border, right_border) > 0) break;
        }
        right_border = number;
        left_border = tmp;
    }
    return false;
}

// The function gets a big number and produces testing by the following algorihm
// https://ru.wikipedia.org/wiki/%D0%A2%D0%B5%D1%81%D1%82_%D0%9C%D0%B8%D0%BB%D0%BB%D0%B5%D1%80%D0%B0_(%D1%82%D0%B5%D0%BE%D1%80%D0%B8%D1%8F_%D1%87%D0%B8%D1%81%D0%B5%D0%BB)
bool fast_test(const std::vector<int>& number) {
    if ((number.size() == 1) &&
       (number[0] <= sieve_limit_for_trivial * sieve_limit_for_trivial)) {
        return trivial_div(number);
    }
    if (!trivial_div(number)) {
        return false;
    }
    int log_2 = binary_logarithm(number);
    // Following the Miller algorithm, you first need to check if the number is a power of a prime
    if (is_pow(number, log_2)) {
        return false;
    }
    std::vector<int> ten_to_th_pow_36(5, 0);
    ten_to_th_pow_36[4] = 1;
    int *primes, num_of_primes_to_check;
    if (cmp_of_longs(number, ten_to_th_pow_36) > 0) {
        double log_10_e = log10(M_E), ln_N = decimal_logarithm(number) / log_10_e;
        int f = static_cast<int>(binary_logarithm(number) * log(ln_N)) + 4;
        std::vector<int> tmp(1);
        tmp[0] = f;
        res_of_sieve sieve_res = Sieve_of_Eratosthenes(f, f / 3 + (3 * f / 2 / binary_logarithm(tmp)) + 2);
        primes = sieve_res.sieve;
        num_of_primes_to_check = sieve_res.num_of_primes;
    }
    else {
        num_of_primes_to_check = 20;
        primes = Sieve_of_Eratosthenes(71, num_of_primes_to_check).sieve;
    }
    std::vector<int> q, tmp2(1), decrement_of_number;
    tmp2[0] = 1;
    decrement_of_number = number;
    long_sub(decrement_of_number, tmp2);
    q = decrement_of_number;
    int s = 0;
    while (!(q[0] & 1)) {
        long_div_short(q, 2);
        ++s;
    }
    std::vector<std::vector<int> > tmp_values((unsigned long) (s + 1));
    std::vector<int> tmp;
    for (int i = 0; i < num_of_primes_to_check; i++) {
        std::vector <int> tmp3(1);
        if (primes[i] > sieve_limit_for_trivial) {
            tmp = number;
            if (!long_div_short(tmp, primes[i])) {
                return false;
            }
        }
        tmp3[0] = primes[i];
        std::vector<int> tmp_res1 = long_pow_mod_n(tmp3, q, number);
        std::vector<int> previous_res = long_div_long(tmp_res1, number);
        tmp_values[0] = previous_res;
        for (int k = 1; k <= s; k++) {
            std::vector<int> tmp_res = long_mul_long(previous_res, previous_res);
            tmp_values[k] = tmp_res = long_div_long(tmp_res, number);
            previous_res = tmp_res;
        }
        std::vector<int> res = long_div_long(tmp_values[s], number);
        if (!((res.size() == 1) && (res[0] == 1))) {
            return false;
        }
        if ((tmp_values[0].size() == 1) && (tmp_values[0][0] == 1)) {
            tmp3.clear();
            continue;
        }
        int j = s - 1;
        for (; j >= 0; --j) {
            if (!((tmp_values[j].size() == 1) && (tmp_values[j][0]) == 1)) {
                break;
            }
        }
        if (!cmp_of_longs(tmp_values[j], decrement_of_number)) {
            tmp3.clear();
            continue;
        }
        return false;
    }
    return true;
}

int main(int argc, char **argv) {
    int len = static_cast<int>(strlen(argv[2]));
    if (((len == 1) && (argv[2][0] < '2')) || (argv[2][0] == '-')) {
        std::cout << "undefined\n";
        return 0;
    }
    std::vector<int> number;
    convert_string_into_vector(number, argv[2]);
    bool res = fast_test(number);
    std::cout << (res ? "prime\n" : "composite\n");
    return 0;
}
