#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

bool read_int_unlocked(int & out) {
    int c = getchar_unlocked();
    int x = 0;
    int neg = 0;
    for (; !('0' <= c && c <= '9') && c != '-'; c = getchar_unlocked()) {
        if (c == EOF) {
            return false;
        }
    }
    if (c == '-') {
        neg = 1;
        c = getchar_unlocked();
    }
    if (c == EOF) {
        return false;
    }
    for (; '0' <= c && c <= '9' ; c = getchar_unlocked()) {
        x = x * 10 + c - '0';
    }
    out = neg ? -x : x;
    return true;
}

int main() {
    //cin.tie(NULL);
    int n, m, q;
    //ios_base::sync_with_stdio(false);
    //scanf("%d%d%d", &n, &m, &q);
    read_int_unlocked(n);
    read_int_unlocked(m);
    read_int_unlocked(q);
    //std::cin >> m >> q;
    int *arr, *buf;
    arr = new int[n * m];
    buf = new int[m];
    for (int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            //scanf("%d", arr + i * m + j);
            //std::cin >> arr[i * m + j];
            read_int_unlocked(arr[i * m + j]);
		}
    }
    int a, b, c, d, h, w;
    for (int i = 0; i < q; i++) {
        //scanf("%d%d%d%d%d%d", &a, &b, &c, &d, &h, &w);
        //std::cin >> a >> b >> c >> d >> h >> w;
        read_int_unlocked(a);
        read_int_unlocked(b);
        read_int_unlocked(c);
        read_int_unlocked(d);
        read_int_unlocked(h);
        read_int_unlocked(w);
        int *src = arr + (a - 1) * m + b - 1;
        int *dst = arr + (c - 1) * m + d - 1;
        for (int j = 0; j < h; j++, src += m, dst += m) {
            memcpy(buf, src, w * sizeof(int));
            memcpy(src, dst, w * sizeof(int));
            memcpy(dst, buf, w * sizeof(int));
        }
    }
    printf("\n");
    //std::cout << '\n';
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d ", arr[i * m + j]);
            //std::cout << arr[i * m + j] << " ";
        }
        printf("\n");
        //std::cout << '\n';
    }
    delete[] arr, buf;
    return 0;
}
