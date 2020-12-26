#include <thread>  // NOLINT
#include <vector>

int main() {
    int num_of_threads = 6;

    auto worker = []() { for (int i = 0; ; ++i) {} };

    std::vector<std::thread> workers;
    for (int i = 0; i < num_of_threads; ++i) {
        workers.emplace_back(worker);
    }
    for (int i = 0; i < num_of_threads; ++i) {
        workers[i].join();
    }
}
