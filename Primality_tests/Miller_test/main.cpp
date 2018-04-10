// g++ main.cpp -o IsPrime -lgmp -lgmpxx
#include <iostream>
#include <gmp.h>
#include <gmpxx.h>
#include <cmath>
#include <ctime>

#define trivial_div_lim 61
#define fault -1
#define prime 1
#define composite 0
#define undefined 2

using namespace std;

inline int __attribute((fastcall)) write_result_of_test(int res) {
	switch(res) {
		case prime:
			cout << "Prime.\n";
			return 0;
		case composite:
			cout << "Composite.\n";
			return 0;
		case undefined:
			cout << "Undefined.\n";
			return 0;
		case fault:
			cout << "Mistake. Unable to find number\n";
			return 1;
	}
}

inline int __attribute((fastcall)) looking_for_a_number(int argc, char * const argv[]) {
	for(int i = 1; i < argc; i++) {
		int flag = 1;
		for(int j = 0; argv[i][j]; j++) {
			if(argv[i][j] < '0' || argv[i][j] > '9') {
				flag = 0;
				break;
			}
		}
		if(flag)
			return i;
	}
	return fault;
}

inline int * __attribute((fastcall)) sieve_of_Eratosthenes(int length_of_resulting_array, int limit_of_sieve) {
    int *sieve = (int *) calloc((size_t) limit_of_sieve + 1, sizeof(int));
    int half_of_limit_of_sieve = limit_of_sieve / 2;
    for(int i = 2; i <= half_of_limit_of_sieve; i++) {
        if(!sieve[i])
            for(int j = 2 * i; j <= limit_of_sieve; j += i)
                sieve[j] = 1;
    }
    int *res_arr = new int[length_of_resulting_array];
    memset(res_arr, -1, length_of_resulting_array);
    int j = 2, i = 0;
    for(; j <= limit_of_sieve; j++)
        if(!sieve[j]) {
            res_arr[i] = j;
            i++;
        }
    free(sieve);
    return res_arr;
}

inline bool __attribute((fastcall)) trivial_div(const mpz_class &number) { // returns false if divisor was found
    int *array_of_primes = sieve_of_Eratosthenes(100, trivial_div_lim);
    if(number <= trivial_div_lim) {
    	for(int i = 0; array_of_primes[i] != -1; i++)
    		if(number == array_of_primes[i])
    			return true;
    	return false;
    }
    for(int i = 0; array_of_primes[i] != -1; i++) {
    	if(number % array_of_primes[i] == 0)
    		return false;
    }
    return true;
}

inline int __attribute((fastcall)) miller_function(const mpz_class &number) { // very bad approximation
	double ln_n = mpz_sizeinbase(number.get_mpz_t(), 2);
	return mpz_sizeinbase(number.get_mpz_t(), 2) * log(ln_n);
}

inline void __attribute((fastcall)) find_s_and_q(pair <int, mpz_class> &res, const mpz_class &number) {
	mpz_class tmp = number;
	tmp--;
	int s = 0;
	while(tmp % 2 == 0) {
		tmp /= 2;
		s++;
	}
	res.first = s;
	res.second = tmp;
}

template <typename T>
inline int __attribute((fastcall)) delete_arr(int res, T *arr) {
	delete[] arr;
	return res;
}

inline bool __attribute((fastcall)) miller_test(const mpz_class &number) {
    if(mpz_perfect_power_p(number.get_mpz_t()))
        return false;
    int f = miller_function(number);
    
}

int main(int argc, char * const argv[]) {
	if(argc < 2)
		return write_result_of_test(fault);
	int id = looking_for_a_number(argc, argv);
	if(id == fault)
		return write_result_of_test(fault);
	mpz_class number;
	number = argv[id];
	if(number == 1 || number == 0)
		return write_result_of_test(undefined);
	int res = (int) trivial_div(number);
	if(!res || number <= trivial_div_lim * trivial_div_lim)
		return write_result_of_test(res);
	res = (int) miller_test(number);
	return write_result_of_test(res);
}