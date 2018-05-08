#include <chrono>
#include <iostream>


void func() {
    for (int i = 0; i < 2000000000; ++i) { }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "elapsed time: " << elapsed.count() << "s\n";
}