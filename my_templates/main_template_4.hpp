#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cmath>

#define fi first
#define se second
//#define NDEBUG

typedef double lf;
typedef long long lld;
typedef long long llu;

using namespace std;

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

int main() {
    ;
    return 0;
}