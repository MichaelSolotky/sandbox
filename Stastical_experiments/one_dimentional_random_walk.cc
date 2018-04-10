#include <iostream>
#include <cmath>
#include <fstream>

int TakeRandomNumber(std::ifstream& in) {
    const int mask = 0x3;
    char res = 0;
    in >> res;
    res &= mask;
    if (res == 0 || res == 1)
        return 1;
    else
        return -1;
}

int main() {
    try {
        std::ifstream in("/dev/urandom", std::ios::in);
        if (!in.good()) {
            throw -1;
        }
        std::ofstream out("out.txt", std::ios::out);
        int res_of_experiment;
        long long total_sum = 0;
        int num_of_iter;
        std::cin >> num_of_iter;
        double total_error = 0;
        long long max_sum = 0;
        long long min_sum = 0;
        for (int i = 1; i <= num_of_iter; ++i) {
            res_of_experiment = TakeRandomNumber(in);
            total_sum += res_of_experiment;
            max_sum = std::max(total_sum, max_sum);
            min_sum = std::min(total_sum, min_sum);
            total_error = total_sum;
            total_error /= i;
            out << total_error << '\n';
        }
        std::cout << "min = " << min_sum << "\nmax = " << max_sum << '\n';
    } catch(...) {
        std::cout << "ooops\n";
    }
}