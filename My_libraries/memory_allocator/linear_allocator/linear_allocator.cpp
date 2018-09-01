#include <iostream>
#include <malloc.h>
#include "linear_allocator.h"

using namespace std;

linear_allocator::linear_allocator() {
   	size_of_heap = 536870912;
   	init();
}

linear_allocator::linear_allocator(size_t size) {
   	size_of_heap = size;
   	init();
}

linear_allocator::~linear_allocator() {
	char *tmp = (char *) cur_ptr;
	tmp -= busy_space;
	cur_ptr = (void *) tmp;
	free(cur_ptr); 
}

void linear_allocator::init() {
	cur_ptr = malloc(size_of_heap);
	while(!cur_ptr) {
		cout << "Size of heap is unallowable. please, reenter it manually\n";
		cin >> size_of_heap;
		cur_ptr = malloc(size_of_heap);
	}
	busy_space = 0;
}

void* linear_allocator::my_malloc(size_t size) {
	if(busy_space + size <= size_of_heap) {
		busy_space += size;
		void *res = cur_ptr;
		char *tmp = (char *) cur_ptr;
		tmp += size;
		cur_ptr = (void *) tmp;
		return res;
	} else
		return NULL;
}

void linear_allocator::my_free(size_t size) {
	size = size < busy_space ? size : busy_space; // min(size, busy_space)
	char *tmp = (char *) cur_ptr;
	tmp -= size;
	busy_space -= size;
	cur_ptr = (void *) tmp;
}