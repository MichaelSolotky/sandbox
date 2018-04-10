#ifndef LIN_ALL_H
#define LIN_ALL_H

class linear_allocator {
	void *cur_ptr;
	size_t busy_space;
	size_t size_of_heap;
public:
	linear_allocator();
    linear_allocator(size_t size);
	~linear_allocator();
	void init();
	void *my_malloc(size_t size);
	void my_free(size_t size);
};

#endif