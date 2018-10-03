// g++ main.cpp -o main -O2 -pipe -std=c++11

//#include <algorithm>
//#include <array>
//#include <bits/stdc++.h>
//#include <bitset>
//#include <cctype>
//#include <chrono>
//#include <climits>
//#include <cmath>
//#include <cstring>
//#include <cstdio>
//#include <deque>
#include <functional>
//#include <iostream>
//#include <limits>
//#include <list>
//#include <map>
//#include <memory>
//#include <new>
//#include <numeric>
//#include <queue>
//#include <random>
//#include <set>
//#include <stack>
//#include <string>
//#include <unordered_map>
//#include <unordered_set>
//#include <vector>

typedef double LF;
typedef long double LD;
typedef long long LL;
typedef unsigned long long ULL;

using namespace std;

#define gc getchar_unlocked
#define pc putchar_unlocked

#define newline pc('\n')
#define space pc(' ')
#define test printf("test\n")

#define _USE_MATH_DEFINES
#define eps 1e-8
/*----------------------fast I/O------------------------------------*/
#define scani2(a,b) scani(a) , scani(b)
#define scani3(a,b,c) scani(a), scani(b), scani(c)
#define scani4(a,b,c,d) scani(a), scani(b), scani(c), scani(d)
#define scani5(a,b,c,d,e) scani(a), scani(b), scani(c), scani(d), scani(e)
#define scani6(a,b,c,d,e,f) scani(a), scani(b), scani(c), scani(d), scani(e), scani(f)

//scans an integer value
template <typename T>
bool scani(T &n) {
	n = 0;
	bool got = false;
	bool negative = false;
	char c = gc();
	while( c < '0' || c > '9') {
		if(c == '-') 
			negative = true;
		c = gc();
	}
	while(c >= '0' && c <= '9') {
		got = true;
		n = n * 10 + c - '0';
		c = gc();
	}
	if(negative) 
		n = ~(n-1);
	return got;
}

template <typename T> 
void write(T n, int type = true) {
	if(n < 0) {
		pc('-');
		n = -n;
	}
	if(n) {
		char buff[22];
		int len = 0;
		while(n)
			buff[len++] = n % 10 + 48, n /= 10;
		for(int i = len - 1; i >= 0; i--) 
			pc(buff[i]);
	} else
		pc('0');
	if(type == ' ')
		pc(' ');
	else if(type)
		pc('\n');
}

int scans(char *a) {
	int i = 0;
	char c = 0;
	while(c < 33) 
		c = gc();
	while(c > 33) {
		a[i++] = c;
		c = gc();
	}
	a[i] = 0;
	return i;
}

/************************ End of template ************************/
