#include <iostream>
#include <cstdio>
#include <malloc.h>

typedef struct list {
    int val;
    struct list *next, *prev;
} list;

typedef struct my_pair {
    list *begin, *end;
} my_pair;

void push_back(list *&begin, list *&end, int val) {
    if (end) {
        end->next = (list *) malloc(sizeof(list));
        end->next->prev = end;
        end = end->next;
    } else {
        begin = end = (list *) malloc(sizeof(list));
        begin->prev = NULL;
    }
    end->next = NULL;
    end->val = val;
}

// return pointer on a node of the list which number in order is num
// begin and end are pointers to the first and the last elements of the list
// size is quantity of nodes in the list
list *find_a_node(my_pair &a, int size, int num) {
    list *begin = a.begin, *end = a.end, *res;
    if (num <= size / 2) {
        res = begin;
        for (int i = 1; i < num; i++) {
            res = res->next;
        }
    } else {
        res = end;
        for(; size != num; --size) {
            res = res->prev;
        }
    }
    return res;
}

void reading(my_pair *arr, int n, int m) {
    int tmp;
    for (int i = 0; i < n; i++) {
        list *begin = NULL, *end = NULL;
        for(int j = 0; j < m; j++) {
            scanf("%d", &tmp);
            push_back(begin, end, tmp);
        }
        arr[i].begin = begin, arr[i].end = end;
    }
}

// src contains the first and the last elements of the sublist which is going to be swapped
// dst contains the second part of the data for swapping
/* real_src and real_dst are the parts of the arr, they're here because head and end of lists
can be changed, so real head and real en of the lists need to be overwritten*/
void swap_sublists(my_pair src, my_pair dst, my_pair &src_real, my_pair &dst_real) {
    list *src_begin_prev = src.begin->prev;
    list *src_end_next = src.end->next;
    list *dst_begin_prev = dst.begin->prev;
    list *dst_end_next = dst.end->next;
    int src_begin_flag = 0, src_end_flag = 0;
    int dst_begin_flag = 0, dst_end_flag = 0;
    dst.begin->prev = src_begin_prev;
    if (src_begin_prev) {
        src_begin_prev->next = dst.begin;
    } else {
        dst_begin_flag = 1;
    }
    dst.end->next = src_end_next;
    if (src_end_next) {
        src_end_next->prev = dst.end;
    } else {
        dst_end_flag = 1;
    }
    src.begin->prev = dst_begin_prev;
    if (dst_begin_prev) {
        dst_begin_prev->next = src.begin;
    } else {
        src_begin_flag = 1;
    }
    src.end->next = dst_end_next;
    if (dst_end_next) {
        dst_end_next->prev = src.end;
    } else {
        src_end_flag = 1;
    }
    if (dst_begin_flag) {
        src_real.begin = dst.begin;
    }
    if (dst_end_flag) {
        src_real.end = dst.end;
    }
    if (src_begin_flag) {
        dst_real.begin = src.begin;
    }
    if (src_end_flag) {
        dst_real.end = src.end;
    }
}

void print_all(my_pair *arr, int n) {
    printf("\n");
    for (int i = 0; i < n; i++) {
        list *tmp = arr[i].begin;
        while(tmp) {
            printf("%d ", tmp->val);
            tmp = tmp->next;
        }
        printf("\n");
    }
}

void processing(my_pair *&arr, int q, int m, int n) {
    int a, b, c, d, h, w;
    my_pair src, dst;
    for (int i = 0; i < q; i++) {
        scanf("%d%d%d%d%d%d", &a, &b, &c, &d, &h, &w);
        --a;
        --c;
        for (int j = 0; j < h; j++) {
            src.begin = find_a_node(arr[a + j], m, b);
            src.end = find_a_node(arr[a + j], m, b + w - 1);
            dst.begin = find_a_node(arr[c + j], m, d);
            dst.end = find_a_node(arr[c + j], m, d + w - 1);
            swap_sublists(src, dst, arr[a + j], arr[c + j]);
        }
    }
}

int main() {
    int n, m, q;
    scanf("%d%d%d", &n, &m, &q);
    my_pair *arr = (my_pair *) calloc(n, sizeof(my_pair));
    /*arr is an array of pairs which contain pointer to the first 
    and the last element of a lists which are lines of a matrix*/
    reading(arr, n, m);
    processing(arr, q, m, n);
    print_all(arr, n);
    return 0;
}
