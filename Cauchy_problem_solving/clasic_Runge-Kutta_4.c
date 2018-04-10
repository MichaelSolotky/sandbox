#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double fun(double x, double y) {
    return y;
}

void Runge_Kutta_4(double x, double y, double h, int n) {
    printf("%g\t%g\n", x, y);
    for (int i = 0; i < n; i++) {
        double k1 = fun(x, y);
        double k2 = fun(x + h / 2, y + k1 * h / 2);
        double k3 = fun(x + h / 2, y + k2 * h / 2);
        double k4 = fun(x + h, y + k3 * h);
        y += h / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
        x += h;
        printf("%g\t%g\n", x, y);
    }
}

// solving of Cauchi problem y' = f(x, y), y(a) = y0
// ./main a b n y0
// stdin, stdout
int main (int argc, char **argv) {
    const double a = atof(argv[1]);
    const double b = atof(argv[2]);
    const int n = atoi(argv[3]);
    const double h = (b - a) / n;
    double y0 = atof(argv[4]);
    method4(a, y0, h, n);
}
