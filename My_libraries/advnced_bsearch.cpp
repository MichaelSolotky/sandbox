// g++ main.cpp -o main -O2 -pipe -lm
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

/* array has to be sorted in ascending order
if the value is in array, returns the index of the last element 
with this key else returnes i with a condition 
arr[i] < key && arr[i + 1] > key
note: if arr[0] > key returnes -1 */
template <typename T>
int advanced_bsearch(T *arr, int n, T key) {
	int i = 0, j = n - 1, middle;
	while(i != j) {
		middle = (i + j) / 2;
		if(arr[middle] == key) {
			if(middle + 1 >= n || arr[middle + 1] > key)
				return middle;
			else {
				i = middle + 1;
				continue;
			}
		}
		if(arr[middle] > key)
			j = middle;
		else
			i = middle + 1;
	}
	if(arr[i] > key)
		return i - 1;
	else return i;
}

int main() {
	cin.tie(NULL);
	ios_base::sync_with_stdio(false);
	int n, q;
	cin >> n;
	int *arr = (int *) malloc(n * sizeof(int));
	for(int i = 0; i < n; i++)
		cin >> arr[i];
	std::sort(arr, arr + n, [](int a, int b) { return a < b; });
	cin >> q;
	int money;
	while(q--) {
		cin >> money;
		cout << advanced_bsearch(arr, n, money) + 1 << '\n';
	}
	return 0;
}