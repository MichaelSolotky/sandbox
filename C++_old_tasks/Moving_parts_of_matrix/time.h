#include <ctime>

class time {
  private:
    clock_t time_clock;
  public:
    time() {
        time_clock = clock();
    }
    ~time() {
        printf("time = %g\n", (double) (clock() - time_clock) / CLOCKS_PER_SEC);
    }
};
