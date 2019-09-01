#include "differentiation.h"

tree *copy_tree(tree *root) {
    if (!root) {
        return nullptr;
    }
    tree *copy_of_root = (tree *) calloc(sizeof(tree), 1);
    copy_of_root->data = root->data;
    if (root->left) {
        copy_of_root->left = copy_tree(root->left);
        copy_of_root->left->parent = copy_of_root;
    }
    if (root->right) {
        copy_of_root->right = copy_tree(root->right);
        copy_of_root->right->parent = copy_of_root;
    }
    return copy_of_root;
}

// subroot is result of differentiation of sum (it will be created further)
// node belongs to the tree of expression, it mustn't be changed
//
void variable_case(tree *subroot) {
    subroot->data.symb = 0;
    subroot->data.num = 1;
}

void sum_case(tree *&subroot, tree *node, char symb) { // symbol can be either '+' or '-'
    subroot->data.symb = symb;
    subroot->left = differentiate(node->left);
    subroot->right = differentiate(node->right);
    subroot->right->parent = subroot->left->parent = subroot;
}

void multiplication_case(tree *&subroot, tree *node) {
    subroot->data.symb = '+';
    tree *left = subroot->left = (tree *) calloc(sizeof(tree), 1);
    tree *right = subroot->right = (tree *) calloc(sizeof(tree), 1);
    left->parent = right->parent = subroot;
    left->data.symb = right->data.symb = '*';
    left->left = differentiate(node->left);
    left->right = copy_tree(node->right);
    left->left->parent = left->right->parent = left;
    right->left = copy_tree(node->left);
    right->right = differentiate(node->right);
    right->left->parent = right->right->parent = right;
}

void division_case(tree *&subroot, tree *node) {
    subroot->data.symb = '/';
    tree *left = subroot->left = (tree *) calloc(sizeof(tree), 1);
    left->parent = subroot;
    multiplication_case(left, node);
    left->data.symb = '-';
    tree *right = subroot->right = (tree *) calloc(sizeof(tree), 1);
    right->parent = subroot;
    right->data.symb = '^';
    right->left = copy_tree(node->right);
    right->left->parent = right;
    right->right = (tree *) calloc(sizeof(tree), 1);
    right->right->parent = right;
    right->right->data.num = 2;
}

tree *a_in_the_power_x_case(tree *node) {
    tree *subroot = (tree *) calloc(sizeof(tree), 1);
    subroot->data.symb = '*';
    subroot->left = differentiate(node->right);
    tree *right = subroot->right = (tree *) calloc(sizeof(tree), 1);
    right->data.symb = 'n';
    subroot->left->parent = right->parent = subroot;
    right->left = copy_tree(node->left);
    right->left->parent = right;
    return subroot;
}

tree *x_in_the_power_a_case(tree *node) {
    tree *subroot = (tree *) calloc(sizeof(tree), 1);
    subroot->data.symb = '*';
    tree *left = subroot->left = (tree *) calloc(sizeof(tree), 1);
    subroot->right = differentiate(node->left);
    left->parent = subroot->right->parent = subroot;
    left->data.symb = '/';
    left->left = copy_tree(node->right);
    left->right = copy_tree(node->left);
    left->left->parent = left->right->parent = left;
    return subroot;
}

void simplification_of_x_in_the_power_a_case(tree *&subroot, tree *node) {
    tree *left = subroot->left = (tree *) calloc(sizeof(tree), 1);
    tree *right = subroot->right = differentiate(node->left);
    left->parent = right->parent = subroot;
    left->data.symb = '*';
    left->right = copy_tree(node->right);
    left->left = copy_tree(node);
    left->left->parent = left->right->parent = left;
    left = left->left;
    tree *intermediate = (tree *) calloc(sizeof(tree), 1);
    intermediate->left = left->right;
    intermediate->parent = left;
    left->right = intermediate;
    intermediate->left->parent = intermediate;
    intermediate->data.symb = '-';
    intermediate->right = (tree *) calloc(sizeof(tree), 1);
    intermediate->right->parent = intermediate;
    intermediate->right->data.num = 1;
}

void power_case(tree *&subroot, tree *node) {
    if (node->left->data.symb || node->left->data.num) { // else return a node with number 0
        subroot->data.symb = '*';
        if (!node->left->data.symb) {
            subroot->left = copy_tree(node);
            subroot->right = a_in_the_power_x_case(node);
            subroot->left->parent = subroot->right->parent = subroot;
        } else if (!node->right->data.symb) {
            simplification_of_x_in_the_power_a_case(subroot, node);
        } else {
            subroot->left = copy_tree(node);
            tree *right = subroot->right = (tree *) calloc(sizeof(tree), 1);
            subroot->left->parent = right->parent = subroot;
            right->data.symb = '+';
            right->left = a_in_the_power_x_case(node);
            right->right = x_in_the_power_a_case(node);
            right->left->parent = right->right->parent = right;
        }
    } else
        subroot->data.symb = 0;
}

void arc_tangent_case(tree *&subroot, tree *node) {
    subroot->data.symb = '*';
    tree *left = subroot->left = (tree *) calloc(sizeof(tree), 1);
    left->parent = subroot;
    left->data.symb = '/';
    left->left = (tree *) calloc(sizeof(tree), 1);
    left->right = (tree *) calloc(sizeof(tree), 1);
    left->left->parent = left->right->parent = left;
    left->left->data.num = 1;
    left = left->right;
    left->data.symb = '+';
    left->left = (tree *) calloc(sizeof(tree), 1);
    left->right = (tree *) calloc(sizeof(tree), 1);
    left->left->data.num = 1;
    left->left->parent = left->right->parent = left;
    left = left->right;
    left->data.symb = '^';
    left->left = copy_tree(node->left);
    left->right = (tree *) calloc(sizeof(tree), 1);
    left->left->parent = left->right->parent = left;
    left->right->data.num = 2;
    subroot->right = differentiate(node->left);
    subroot->right->parent = subroot;
}

// '2', '3', '4', '5', '6', '7', '8', '9', 'g', 'l', 's', 'n', 't', 'q'
tree *differentiate(tree *node) {
    if (!node) {
        char string[30] = "Trying to differentiate NULL";
        write_mistake(string, (char) 0xff);
    }
    tree *subroot = (tree *) calloc(sizeof(tree), 1);
    char symb = node->data.symb;
    char string[35] = "I don't how to differentiate this";
    switch(symb) {
        case 'x':
            variable_case(subroot);
            return subroot;
        case 0:
            subroot->data.num = 0;
            return subroot;
        case '+':
        case '-':
            sum_case(subroot, node, symb);
            return subroot;
        case '*':
            multiplication_case(subroot, node);
            return subroot;
        case '/':
            division_case(subroot, node);
            return subroot;
        case '^':
            power_case(subroot, node);
            return subroot;
        case '1':
            arc_tangent_case(subroot, node);
            return subroot;
        /*case '2':
            arc_cotangent_case(subroot, node);
            return subroot;
        case '3':
            arc_sine_case(subroot, node);
            return subroot;
        case '4':
            arc_cosine_case(subroot, node);
            return subroot;
        case '5':
            hyperbolic_cosine_case(subroot, node);
            return subroot;
        case '6':
            hyperbolic_sine_case(subroot, node);
            return subroot;
        case '7':
            hyperbolic_tangent_case(subroot, node);
            return subroot;*/

        default:
            write_mistake(string, symb);
    }
}
