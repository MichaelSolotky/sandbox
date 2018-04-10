#include <iostream>
#include <cmath>
#include <fstream>

int TakeRandomNumber(std::ifstream& in) {
    const int mask = 0x3;
    char res = 0;
    in >> res;
    res &= mask;
    return res;
}

int main() {
    try {
        int num_of_0 = 0;
        int num_of_1 = 0;
        int num_of_2 = 0;
        int num_of_3 = 0;
        int total_sum = 0;
        double prob_of_0 = 0;
        double prob_of_1 = 0;
        double prob_of_2 = 0;
        double prob_of_3 = 0;
        std::ifstream in("/dev/urandom", std::ios::in);
        if (!in.good()) {
            throw -1;
        }
        std::ofstream out("out.txt", std::ios::out);
        const int num_of_iter = 100000;
        for (int i = 1; i <= num_of_iter; ++i) {
            int res_of_experiment = TakeRandomNumber(in);
            if (res_of_experiment == 0)
                ++num_of_0;
            else if (res_of_experiment == 1)
                ++num_of_1;
            else if (res_of_experiment == 2)
                ++num_of_2;
            else
                ++num_of_3;
            total_sum = num_of_0 + num_of_1 + num_of_2 + num_of_3;
            prob_of_0 = num_of_0;
            prob_of_0 /= total_sum;
            prob_of_1 = num_of_1;
            prob_of_1 /= total_sum;
            prob_of_2 = num_of_2;
            prob_of_2 /= total_sum;
            prob_of_3 = num_of_3;
            prob_of_3 /= total_sum;
        }
        std::cout << prob_of_0 << '\n';
        std::cout << prob_of_1 << '\n';
        std::cout << prob_of_2 << '\n';
        std::cout << prob_of_3 << '\n';
    } catch(...) {
        std::cout << "ooops\n";
    }
}