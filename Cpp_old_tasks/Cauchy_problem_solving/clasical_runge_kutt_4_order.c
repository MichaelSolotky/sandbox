#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double get_func(const double x, const double y) {
    return y;
}

void runge_kutt_4(const double x, const double y, const double h, const int n) {
    printf("%g\t%g\n", x, y);
    for (int i = 0; i < n; i++) {
        double k1 = get_func(x, y);
        double k2 = get_func(x + h / 2, y + k1 * h / 2);
        double k3 = get_func(x + h / 2, y + k2 * h / 2);
        double k4 = get_func(x + h, y + k3 * h);
        y += h / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
        x += h;
        printf("%g\t%g\n", x, y);
    }
}

// solving of Cauchi problem  y'  = f(x, y) on [a, b]
//                           y(a) = y0
// ./main a b n y0
// stdin, stdout
int main (int argc, char **argv) {
    const double a = atof(argv[1]);
    const double b = atof(argv[2]);
    const int n = atoi(argv[3]);
    const double h = (b - a) / n;
    double y0 = atof(argv[4]);
    runge_kutt_4(a, y0, h, n);
}
