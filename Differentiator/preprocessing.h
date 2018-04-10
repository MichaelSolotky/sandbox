#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <cstring>

#define V(x) {x, #x}

using namespace std;

typedef struct data {
    char symb; // if symb == 0 then this node contains number, else it contains operator or symbol of variable
    double num;
} data;

typedef struct list {
    struct data data;
    struct list *next;
} list;

void print_list(list *head);

char preprocess_symb(char *buf, int &i);

list *preprocessing(char *buf, int size, int &calc_flag, double &x0, char *possible_symbols);

void add_brackets_and_multipl(list *&head, list *&node, char last_read_symbol, char *possible_symbols, int &continue_flag);

void preprocess_number(list *&head, list *&node, char *buf, char *possible_symbols, int &i);

void preprocess_minus(list *&head, list *&node, char *possible_symbols);

void read_point_for_calculating(int &calc_flag, int &i, int size, char *buf, double &x0);

double get_number(char *string, int &curr_pos);

list *read_expression(double &x0, int &calc_flag, char *possible_symbols);

void add_in_list(list *&head, list *&node, char symb, double num);

int search_in_possible_symbols(char *possible_symbols, char symb, int begin, int end);

void write_mistake(char *string, char c);

void delete_head(list *&head);

void checking_flags(int &test_flag, int argv, char **argc);