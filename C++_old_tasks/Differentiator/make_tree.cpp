#include "preprocessing.h"
#include "make_tree.h"

void check_and_delete_tree(tree *&root) {
    if(root)
        delete_tree(root);
}

void delete_tree(tree *&root) {
    if(root->left)
        delete_tree(root->left);
    if(root->right)
        delete_tree(root->right);
    free(root);
    root = NULL;
}

void view_tree(tree *root) {
    if(root) {
        char c;
        scanf("%c", &c);
        getchar();
        while(c != 's') { // s - stop, w - write, l - move to left, r - right, p - parent.
            if(c == 'w') {
                if(root->data.symb)
                    printf("%c\n", root->data.symb);
                else
                    printf("%g\n", root->data.num);
            }
            else if(c == 'l') {
                if(root->left)
                    root = root->left;
                else
                    printf("NULL\n");
            }
            else if(c == 'r') {
                if(root->right)
                    root = root->right;
                else
                    printf("NULL\n");
            }
            else if(c == 'p') {
                if(root->parent)
                    root = root->parent;
                else
                    printf("NULL\n");
            }
            scanf("%c", &c);
        }
    }
}

int find_priority(char c, char *possible_symbols) {
    switch(c) {
        case 0:
        case 'x':
        case 'p':
        case 'e':
        case '(':
        case ')':
            return 0;
        case '*':
        case '/':
            return 2;
        case '+':
        case '-':
            return 3;
        default:
            if(c == '^' || search_in_possible_symbols(possible_symbols, c, 6, 21))
                return 1;
            else {
                char string[20] = "unknown symbol ";
                write_mistake(string, c);
            }
    }
}

tree *make_tree(list *&head, char *possible_symbols) {
    tree *root = NULL, *node = NULL;
    int logarithm_flag = 0;
    while(head) {
        tree *subroot;
        switch (head->data.symb) {
            case '(':
                delete_head(head);
                subroot = make_tree(head, possible_symbols);
                add_subtree_in_tree(root, node, subroot);
                delete_head(head);
                continue;
            case 'l':
            case '9':
                logarithm_flag = 1;
                break;
            case ')':
                return root;
            default:;
        }
        add_node_in_tree(head, root, node, possible_symbols, logarithm_flag);
        delete_head(head);
    }
    return root;
}

void add_subtree_in_tree(tree *&root, tree *&node, tree *&subroot) {
    if(!root) { // a crutch - taking a pow 1 of expression, without it lower case won't work
        root = node = (tree *) calloc(sizeof(tree), 1);
        root->data.symb = '^';
        node->left = subroot;
        node->left->parent = node;
        node->right = (tree *) calloc(sizeof(tree), 1);
        node->right->parent = node;
        node->right->data.num = 1;
        return;
    } else if(node->left && node->right) {
        char string[70] = "left and right sons are in their places, I don't know where to paste";
        write_mistake(string, 0xff);
    }
    else if(!node->left)
        node->left = subroot;
    else
        node->right = subroot;
    subroot->parent = node;
}

void add_node_in_tree(list *head, tree *&root, tree *&node, char *possible_symbols, int &logarithm_flag) {
    if(logarithm_flag && head->data.symb == '*') {
        logarithm_flag = 0;
        return;
    }
    if(!root) {
        node = root = (tree *) calloc(sizeof(tree), 1);
        root->data = head->data;
        return;
    } else {
        int cur_priority = find_priority(node->data.symb, possible_symbols);
        int priority_of_head = find_priority(head->data.symb, possible_symbols);
        if(priority_of_head >= cur_priority)
            not_lower_priority_case(root, node, head, priority_of_head, cur_priority, possible_symbols);
        else
            lower_priority_case(root, node, head, priority_of_head, cur_priority, possible_symbols);
    }
}

void not_lower_priority_case(tree *&root, tree *&node, list *&head, int adding_symb_priority, int cur_priority, char *possible_symbols) {
    register int no_parent_flag = 0;
    while(adding_symb_priority >= cur_priority) {
        if(node->parent) {
            int priority_of_parent = find_priority(node->parent->data.symb, possible_symbols);
            if(adding_symb_priority >= priority_of_parent) {
                node = node->parent;
                cur_priority = priority_of_parent;
                continue;
            } else
                break;
        } else {
            no_parent_flag = 1;
            break;
        }
    }
    if(no_parent_flag) {
        node->parent = (tree *) calloc(sizeof(tree), 1);
        node->parent->left = node;
        root = node = node->parent;
    } else {
        int left_side_flag = (node == node->parent->left);
        tree *parent = node->parent;
        node->parent = (tree *) calloc(sizeof(tree), 1);
        node->parent->left = node;
        node = node->parent;
        node->parent = parent;
        if(left_side_flag)
            node->parent->left = node;
        else
            node->parent->right = node;
    }
    node->data = head->data;
}

void lower_priority_case(tree *&root, tree *&node, list *&head, int adding_symbol_priority, int cur_priority, char *possible_symbols) {
    while(1) {
        int left_priority = find_priority(node->left->data.symb, possible_symbols);
        if(node->left) {
            if(node->right) {
                char string[70] = "left and right sons are in their places, I don't know where to paste";
                write_mistake(string, 0xff);
            }
            node->right = (tree *) calloc(sizeof(tree), 1);
            node->right->parent = node;
            node = node->right;
            break;
        } else if(node->right) {
            char string[50] = "there isn't left son, but is right";
            write_mistake(string, 0xff);
        } else {
            node->left = (tree *) calloc(sizeof(tree), 1);
            node->left->parent = node;
            node = node->left;
            break;
        }
    }
    node->data = head->data;
}