#include <iostream>
#define NDEBUG
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cmath>

#define fi first
#define se second

typedef double lf;
typedef long long lld;
typedef long long llu;

inline int read_int() {
    int x = 0; 
    bool f = 0; 
    char ch = getchar();
    while (ch < '0' || '9' < ch) {
    	f |= ch == '-';
    	ch = getchar();
    }
    while ('0' <= ch && ch <= '9') {
    	x = x * 10 + ch - '0';
    	ch = getchar();
    }
    return f ? -x : x;
}

#define maxn 1001

typedef struct triple { int val, down, right; } triple;

inline int find_link(int a, int b, int m, int n, triple *arr) {
    // a and b are ccordinates
    int start;
    start = (a - 1) * (m + 1);
    for (int i = 1; i < b; i++) {
        start = arr[start].right;
    }
    return start;
}

int main() {
	int n = read_int(), m = read_int(), q = read_int();
    triple *arr = (triple *) malloc((n + 1) * (m + 1) * sizeof(triple));
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {            
            arr[i * (m + 1) + j].val = read_int();
            arr[(i - 1) * (m + 1) + j - 1].down = i * (m + 1) + j - 1;
            arr[(i - 1) * (m + 1) + j - 1].right = (i - 1) * (m + 1) + j;
        }
        // making links for the last column
        arr[i * (m + 1) - 1].down = (i + 1) * (m + 1) - 1;
    }
    //making right links for the last line
    for (int i = 1; i <= m; i++)
        arr[n * (m + 1) + i - 1].right = n * (m + 1) + i;
    for (int i = 0, a, b, c, d, h, w; i < q; i++) {
        a = read_int(), b = read_int(), c = read_int();
        d = read_int(), h = read_int(), w = read_int();        
        // pos is an index of an element which on 1 position upper and 1 position lefter 
        // than the beginning of submatrix
        int pos1 = find_link(a, b, m, n, arr);
        int pos2 = find_link(c, d, m, n, arr);
        int p, r, j;
        // top
        for (p = pos1, r = pos2, j = 0; j < w; ) {
            j++;
            p = arr[p].right, r = arr[r].right;
            std::swap(arr[p].down, arr[r].down);
        }
        // right
        for (j = 0; j < h; ) {
            j++;
            p = arr[p].down, r = arr[r].down;
            std::swap(arr[p].right, arr[r].right);
        }
        // left
        for (p = pos1, r = pos2, j = 0; j < h; ) {
            j++;
            p = arr[p].down, r = arr[r].down;
            std::swap(arr[p].right, arr[r].right);
        }
        // bottom
        for (j = 0; j < w; ) {
            j++;
            p = arr[p].right, r = arr[r].right;
            std::swap(arr[p].down, arr[r].down);
        }
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 1, pos = i * (m + 1); j <= m; ) {
            j++;
            pos = arr[pos].right;
            printf("%d ", arr[pos].val);
        }
        putchar('\n');
    }
    free(arr);
    return 0;
}
