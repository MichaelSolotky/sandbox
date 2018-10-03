#include "preprocessing.h"
#include "make_tree.h"

tree *copy_tree(tree *root);

tree *differentiate(tree *node);

void variable_case(tree *root);

void sum_case(tree *&root, tree *node, char symb);

void multiplication_case(tree *&root, tree *node);

void division_case(tree *&root, tree *node);

void power_case(tree *&root, tree *node);

tree *a_in_the_power_x_case(tree *node);

tree *x_in_the_power_a_case(tree *node);

void simplification_of_x_in_the_power_a_case(tree *&subroot, tree *node);

void arc_tangent_case(tree *&subroot, tree *node);