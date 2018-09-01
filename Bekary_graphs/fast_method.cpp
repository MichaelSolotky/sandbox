#include <iostream>
#include <cstdio>
//#include <cstdlib>
//#include <cstring>
#include <climits>
//#include <cmath>

#define fi first
#define se second

typedef double lf;
typedef long long lld;
typedef long long llu;

int read_int() {
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

#define maxn 100009

int x[maxn], y[maxn], d[maxn];
char sources[maxn];

int main() {
	int n, m, k;
	n = read_int(), m = read_int(), k = read_int();
	for(int i = 0; i < m; i++)
		x[i] = read_int(), y[i] = read_int(), d[i] = read_int();
	for(int i = 0, tmp; i < k; i++)
		sources[read_int()] = 1;
	int mn = INT_MAX;
	char flag = 0;
	for(int i = 0; i < m; i++)
		if(sources[x[i]] != sources[y[i]]) {
			mn = std::min(mn, d[i]);
			flag = 1;
		}
	if(flag)
		printf("%d\n", mn);
	else
		puts("-1");
	return 0;
}
