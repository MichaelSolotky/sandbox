#include <iostream>
#include <cstdio>
#include <cstdlib>

int main() {
    int n, m, q, r;
    scanf("%d%d%d", &n, &m, &q);
    printf("%d %d %d\n", n, m, q);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            r = rand();
            printf("%d ", r);
        }
        printf("\n");
    }
    int a = 0, b = 0, c = 0, d = 0, h = 0, w = 0;
    for (int i = 0; i < q; i++) {
        while (!a)
            a = rand() % (n - 5);
        while (!b)
            b = rand() % (m - 5);
        while (!h)
            h = rand() % min(n - a, m - b);
        while (!w)
            w = rand() % min(n - a, m - b);
        printf("%d %d %d %d %d %d\n", a, b, a + h, b + w, h, w);
    }
    return 0;
}
