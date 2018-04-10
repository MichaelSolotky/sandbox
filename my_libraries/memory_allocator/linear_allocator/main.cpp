#include <iostream>
#include <malloc.h>
#include "linear_allocator.h"

using namespace std;

class myclass{
public:
	int a;
	int b;
	myclass() { cout << "constructor "; }
	void *operator new(size_t size, linear_allocator &A) {
		cout << "single instance\n";
		return A.my_malloc(size);
	}
	void *operator new[](size_t size, linear_allocator &A) {
		cout << "array allocated\n";
		return A.my_malloc(size);
	}
};

template <typename T>
void func(T *arr) {}

int main() {
	cin.sync_with_stdio(false);
	linear_allocator A;
	myclass *obj = new(A) myclass;
	myclass *arr = new(A) myclass[30];
	func(arr);
	return 0;
}