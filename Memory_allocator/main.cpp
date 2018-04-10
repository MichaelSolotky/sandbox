#include <iostream>
#include <cstdlib>
#include <malloc.h>
#include <cstring>

using namespace std;

void *mem;
unsigned mask = 0xfffffffe;
unsigned size_of_heap = 4294967280;
//unsigned size_of_heap = 52;
unsigned minimal_appropriate_size = sizeof(unsigned) << 2;

void *initializing() { // the least significant bit is o <=> it's a free block
    mem = malloc((size_t) size_of_heap + (sizeof(unsigned) << 2));
    void *head_of_mem = mem;
    register char *tmp = (char *) mem;
    (*(unsigned *) tmp) = 1;
    tmp += sizeof(unsigned);
    (*(unsigned *) tmp) = size_of_heap; // header
    tmp += sizeof(unsigned);
    mem = (void *) tmp;
    tmp += size_of_heap;
    (*(unsigned *) tmp) = size_of_heap; // footer
    tmp += sizeof(unsigned);
    (*(unsigned *) tmp) = 1;
    return head_of_mem;
}

void cut_a_block(char *ptr, unsigned n) {
    unsigned *id_of_tag = (unsigned *) (ptr - sizeof(unsigned));
    unsigned tag = (*id_of_tag) & mask;
    *id_of_tag = n | 1;
    *(unsigned *) (ptr + n) = n | 1; // set it allocated
    unsigned new_size = tag - n - (sizeof(unsigned) << 1);
    *(unsigned *) (ptr + n + sizeof(unsigned)) = new_size;
    *(unsigned *) (ptr + tag) = new_size;
}

void *merge_blocks(unsigned n) {
    void *tmp_res;
    char *begin = (char *) mem, *next_ptr, *prev_ptr;
    unsigned *bgn = (unsigned *) begin;
    register unsigned tag = bgn[-1], cur_tag, next_tag, prev_tag;
    while (tag & 1) {
        if(tag == 1)
            break;
        begin += (tag & mask) + (sizeof(unsigned) << 1);
        bgn = (unsigned *) begin;
        tag = bgn[-1];
    }
    while(1) {
        bgn = (unsigned *) begin;
        tag = bgn[-1];
        if(tag == 1)
            break;
        if(!(tag & 1)) { // if a block is free
            for(next_ptr = begin, cur_tag = tag;;) {
                next_ptr += cur_tag + (sizeof(unsigned) << 1);
                next_tag = *(unsigned *) (next_ptr - sizeof(unsigned));
                if(next_tag & 1)
                    break;
                tag += next_tag + (sizeof(unsigned) << 1);
                if((long long) tag >= (long long) n)
                    break;
                cur_tag = next_tag;
            }
            bgn[-1] = tag;
            tmp_res = (void *) begin;
            begin += tag + (sizeof(unsigned) << 1); // beginning of the next block
            bgn = (unsigned *) begin;
            bgn[-2] = tag;
            if((long long) tag >= (long long) n)
                return tmp_res;
        } else
            begin += (tag & mask) + (sizeof(unsigned) << 1);
    }
    begin = (char *) mem;
    bgn = (unsigned *) begin;
    tag = bgn[-1];
    while (tag & 1) {
        if(tag == 1)
            break;
        begin -= (bgn[-2] & mask) + (sizeof(unsigned) << 1);
        bgn = (unsigned *) begin;
        tag = bgn[-1];
    }
    while(1) {
        bgn = (unsigned *) begin;
        tag = bgn[-1];
        if(tag == 1)
            break;
        if(!(tag & 1)) { // if a block is free
            for (prev_ptr = begin; ;) {
                prev_tag = bgn[-2];
                prev_ptr -= (bgn[-2] & mask) + (sizeof(unsigned) << 1);
                if (prev_tag & 1)
                    break;
                begin = prev_ptr;
                bgn = (unsigned *) begin;
                tag += prev_tag + (sizeof(unsigned) << 1);
                if ((long long) tag >= (long long) n)
                    break;
            }
            bgn[-1] = tag;
            if ((long long) tag >= (long long) n)
                return (void *) begin;
        }
        prev_tag = bgn[-2];
        if(prev_tag == 1)
            break;
        begin -= (bgn[-2] & mask) + (sizeof(unsigned) << 1);
    }
    return NULL;
}

void *my_new(unsigned n) {
    n = (n & mask) + ((n & 1) ? 2 : 0);
    char *tmp_res = (char *) mem; // pointer on memory which was allocated or freed the last time
    unsigned tag;
    while(1) {
        unsigned *id_of_tag = (unsigned *) (tmp_res - sizeof(unsigned));
        tag = *id_of_tag;
        if(tag == 1)
            break;
        if(!(tag & 1) && (long long) (tag & mask) >= (long long) n) {
            mem = (void *) tmp_res;
            if(((long long) tag - (long long) minimal_appropriate_size - (long long) (sizeof(unsigned) << 1)) >= (long long) n)
                cut_a_block(tmp_res, n);
            else {
                id_of_tag[0] |= 1;
                *(unsigned *) (tmp_res + tag) |= 1;
            }
            return mem;
        }
        tmp_res += (tag & mask) + (sizeof(unsigned) << 1);
    }
    tmp_res = (char *) mem;
    unsigned prev_tag;
    while(1) {
        unsigned *id_of_tag = (unsigned *) (tmp_res - (sizeof(unsigned) << 1));
        prev_tag = *id_of_tag;
        if(prev_tag == 1)
            break;
        if(!(prev_tag & 1) && (long long) (prev_tag & mask) >= (long long) n) {
            mem = (void *) tmp_res;
            if(((long long) prev_tag - (long long) minimal_appropriate_size - (long long) (sizeof(unsigned) << 1)) >= (long long) n)
                cut_a_block(tmp_res, n);
            else {
                id_of_tag[0] |= 1;
                *(unsigned *) (tmp_res + (prev_tag & mask)) |= 1;
            }
            return mem;
        }
        tmp_res -= (prev_tag & mask) + (sizeof(unsigned) << 1);
    }
    void *res = merge_blocks(n);
    if(res) {
        unsigned *tag_id = (unsigned *) ((char *) res - sizeof(unsigned));
        tag = *tag_id;
        *tag_id |= 1;
        if(((long long) tag - (long long) minimal_appropriate_size - (long long) (sizeof(unsigned) << 1)) >= (long long) n)
            cut_a_block((char *) res, n);
        tag = *tag_id;
        char *footer = (char *) res + tag;
        *(unsigned *) footer |= 1;
    }
    return res;
}

void my_delete(void *ptr) {
    if(!ptr)
        return;
    unsigned *tmp = (unsigned *) ptr;
    unsigned header = tmp[-1];
    if((header & 1) == header) {
        cout << "it's been already freed, address " << ptr << endl;
        abort();
    }
    if(!(header & mask)) {
        cout << "you're not allowed to free this block\n";
        abort();
    }
    tmp[-1] ^= 1;
    tmp = (unsigned *) ((char *) ptr + (header & mask));
    tmp[0] ^= 1;
    mem = ptr;
}

void *my_realloc(void *ptr, unsigned new_size) {
    unsigned *tmp_ptr = (unsigned *) ptr;
    unsigned size = tmp_ptr[-1] & 1;
    if ((long long) (tmp_ptr[-1] & mask) >= (long long) new_size)
        return ptr;
    void *new_ptr = my_new(new_size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, (size_t) size);
        my_delete(ptr);
    }
    mem = new_ptr;
    return new_ptr;
}

void testing_output(void *begin, void *end) {
    printf("start testing\n");
    char *bgn = (char *) begin, *end_ = (char *) end;
    unsigned dif = (unsigned) (end_ - bgn);
    for (unsigned i = 0;;) {
        unsigned tag = (*(unsigned *) (bgn - sizeof(unsigned)));
        unsigned len = tag & mask;
        if(i >= dif)
            break;
        printf("| header, %u ", tag & mask);// '|' is just to make a look better
        if(i + len <= dif) {
            if(tag & 1)
                for(unsigned j = 0; j < len; j++) {
                    if (j == (j & mask)) // if j is divisible by 2
                        printf ("| ");
                    printf ("a ");
                }
            else
                for(unsigned j = 0; j < len; j++) {
                    if (j == (j & mask))
                        printf ("| ");
                    printf ("f ");
                }
            printf("| footer, %u ", tag & mask);
        } else {
            if(tag & 1)
                for(unsigned j = i; j < dif; j++) {
                    if (j == (j & mask))
                        printf ("| ");
                    printf ("a ");
                }
            else
                for(unsigned j = i; j < dif; j++) {
                    if (j == (j & mask))
                        printf ("| ");
                    printf ("f ");
                }
            break;
        }
        i += len + (sizeof(unsigned) << 1);
        bgn += len + (sizeof(unsigned) << 1);
    }
    printf("|\n");
}

int main() {
    void *head_of_memory = initializing(), *tmp_mem = mem;
    unsigned n = 2;
    scanf("%u", &n);
    testing_output(tmp_mem, (void *) ((char *) tmp_mem + size_of_heap));
    void *ptr = my_new(n);
    void *ptr1 = my_new(n);
    void *ptr2 = my_new(n);
    void *ptr3 = my_new(n);
    testing_output(tmp_mem, (void *) ((char *) tmp_mem + size_of_heap));
    ptr = my_realloc(ptr, n + 32);
    testing_output(tmp_mem, (void *) ((char *) tmp_mem + size_of_heap));
    my_delete(ptr);
    my_delete(ptr1);
    my_delete(ptr2);
    my_delete(ptr3);
    testing_output(tmp_mem, (void *) ((char *) tmp_mem + size_of_heap));
    ptr = my_new(160);
    testing_output(tmp_mem, (void *) ((char *) tmp_mem + size_of_heap));
    if(!ptr)
        printf("NULL\n");
    testing_output(tmp_mem, (void *) ((char *) tmp_mem + size_of_heap));
    free(head_of_memory); // end of allocation
    return 0;
}
