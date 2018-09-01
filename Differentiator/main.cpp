#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "differentiation.h"

// Note: the project is not finished

// you may add linear allocator for tree
// simplificator

void print_expression(tree *root) { // write DFS
    FILE *output = fopen("output.txt", "w");
    fclose(output);
}

double calculate(tree *root, double x0) {
    double res;
    return res;
}

// if in power 0 or 1, calculate if constants,
void simplify(tree *&root) { }

int main(int argv, const char **argc) {
    char possible_symbols[25] = {'-', '+', '*', '/', '^', '(', '1', '2', '3', '4',
                                 '5', '6', '7', '8', '9', 'g', 'l', 's', 'n', 't',
                                 'q', 'r', 'x', 'p', 'e'};
    int calc_flag = 0, preliminary_test_flag = 0;
    double x0;
    list *head = read_expression(x0, calc_flag, possible_symbols); // root of expression
    if (head) {
        print_list(head);
        printf("\n");
        checking_flags(preliminary_test_flag, argv, argc);
        tree *root = make_tree(head, possible_symbols);
        simplify(root);
        if (preliminary_test_flag) {
            view_tree(root);
        }
        tree *new_root = differentiate(root);
        print_expression(root);
        if (calc_flag) {
            double result = calculate(root, x0);
            FILE *output = fopen("/home/michael/ClionProjects/C++/differentiator/output.txt", "w");
            fprintf(output, "%g", result);
            fclose(output);
        }
        check_and_delete_tree(root);
        view_tree(new_root);
        check_and_delete_tree(new_root);
    }
    return 0;
}
