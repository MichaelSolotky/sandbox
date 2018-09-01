#include <iostream>
#include <cstdlib>
#include <malloc.h>
#include <cstring>

using namespace std;

static void *head_of_memory;
static void *cur_position;
static const unsigned size_of_heap = 4294967295;
static unsigned free_place_remained = size_of_heap;

typedef class stack_allocator {
public:
    void init();
    void *my_malloc(unsigned n);
    void my_free(void *ptr);
    stack_allocator() {
        init();
    }
    ~linear_allocator() {
        cout << "free\n";
        free(head_of_memory);
    }
} stack_allocator;

void stack_allocator :: init() {
    cur_position = head_of_memory = malloc(size_of_heap);
}

void *stack_allocator :: my_malloc(unsigned n) {
    if(n <= free_place_remained) {
        void *tmp = (char *) cur_position;
        (char *) cur_position += n;
        free_place_remained -= n;
        return tmp;
    } else
        return NULL;
}

void stack_allocator :: my_free(void *ptr) { // makes free of all the memory that is after the pointer
    char *head = (char *) head_of_memory;
    char *c_ptr = (char *) ptr;
    if(c_ptr < head || c_ptr >= head + size_of_heap)
        abort();
    char *c_cur_position = (char *) cur_position;
    if(c_ptr < c_cur_position) {
        free_place_remained += c_cur_position - c_ptr;
        cur_position = ptr;
    }
}

int main() {
    stack_allocator *A = new stack_allocator;

    delete A;
    return 0;
}