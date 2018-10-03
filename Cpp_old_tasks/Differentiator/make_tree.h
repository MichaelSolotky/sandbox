typedef struct tree {
    struct data data;
    struct tree *left, *right, *parent;
} tree;

void check_and_delete_tree(tree *&root);

void delete_tree(tree *&root);

void view_tree(tree *root);

int find_priority(char c, char *possible_symbols);

tree *make_tree(list *&head, char *possible_symbols);

void add_node_in_tree(list *head, tree *&root, tree *&node, char *possible_symbols, int &logarithm_flag);

void add_subtree_in_tree(tree *&root, tree *&node, tree *&subroot);

void not_lower_priority_case(tree *&root, tree *&node, list *&head, int priority_of_head, int cur_priority, char *possible_symbols);

void lower_priority_case(tree *&root, tree *&node, list *&head, int priority_of_head, int cur_priority, char *possible_symbols);