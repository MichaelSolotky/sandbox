struct tree {
    int key;
    struct tree *left, *right, *parent;
};

struct queue_for_tree {
    struct tree **elem;
    struct queue_for_tree *next;
};

struct tree *recursively_create_tree(int n) { // creates a full binary tree of given height
    struct tree *node = NULL;
    if(n) {
        node = malloc(sizeof(*node));
        node->parent = NULL;
        node->key = n;
        node->left = recursively_create_tree(n - 1);
        node->right = recursively_create_tree(n - 1);
        if(node->left)
            node->left->parent = node;
        if(node->right)
            node->right->parent = node;
    }
    return node;
}

struct tree *make_tree_from_array(int *a, int id_begin, int id_end) {
    struct tree *t = malloc(sizeof(struct tree));
    t->parent = NULL;
    int id = (id_end + id_begin) / 2;
    t->key = a[id];
    if(id_begin <= id - 1) {
        t->left = make_tree_from_array(a, id_begin, id - 1);
        t->left->parent = t;
    }
    else
        t->left = NULL;
    if(id_end >= id + 1) {
        t->right = make_tree_from_array(a, id + 1, id_end);
        t->right->parent = t;
    }
    else
        t->right = NULL;
    return t;
}

void add_in_end_of_list(struct queue_for_tree **head, struct queue_for_tree **end, struct tree **key) {
    if(*head) {
        (*end)->next = malloc(sizeof(**head));
        *end = (*end)->next;
        (*end)->next = NULL;
        (*end)->elem = key;
    }
    else {
        *head = malloc(sizeof(**head));
        (*head)->next = NULL;
        (*head)->elem = key;
        *end = *head;
    }
}

void delete_from_beginning_of_a_list(struct queue_for_tree **head) {
    if(!(*head)->next) {
        free(*head);
        *head = NULL;
    }
    else {
        struct queue_for_tree *tmp = (*head)->next;
        free(*head);
        *head = tmp;
    }
}

struct tree *string_search(struct tree *root, char *key) {
    if(root) {
        int r;
        while(1) {
            r = strcmp(key, root->key);
            if(!r)
                return root;
            else if(r < 0)
            if(root->left)
                root = root->left;
            else
                return NULL;
            else
            if(root->right)
                root = root->right;
            else
                return 0;
        }
    }
    else
        return NULL;
}

struct tree *int_search(struct tree *root, int key) {
    if(root)
        while(1) {
            if(root->key == key)
                return root;
            else if(key < root->key)
            if(root->left)
                root = root->left;
            else
                return NULL;
            else
            if(root->right)
                root = root->right;
            else
                return 0;
        }
    else
        return NULL;
}

void delete_tree(struct tree *root) {
    if(root->left)
        delete_tree(root->left);
    if(root->right)
        delete_tree(root->right);
    free(root);
}

int height_of_binary_tree(unsigned long long n) {
    int i = 1;
    unsigned long long *a = malloc(65 * sizeof(unsigned long long));
    a[0] = 1; a[1] = 2;
    while((n > a[i] - 1) || (n < a[i - 1] - 1)) {
        i++;
        a[i] = a[i - 1] * 2;
    }
    free(a);
    return i;
}

/* example of filling in (instead this figures are different meanings of a,
                                                 1 means you want to do on, 0 means you want to stop):
                                                           1 n_0
                                        1 n_1                               1 n_2
                              1 n_3              1 n_4               1 n_5          0
                           0         0        0        0          0        0
*/
struct tree *make_tree(void) {
    struct tree *root, *elem;
    struct queue_for_tree *head, *end;
    end = head = NULL;
    root = elem = NULL;
    int a, key;
    scanf("%d", &a);
    if(a) {
        root = elem = malloc(sizeof(*elem));
        scanf("%d", &key);
        elem->key = key;
        scanf("%d", &a);
        if(a) {
            scanf("%d", &key);
            elem->left = malloc(sizeof(*elem));
            elem->left->parent = elem;
            elem->left->key = key;
            add_in_end_of_list(&head, &end, &elem->left);
        }
        scanf("%d", &a);
        if(a) {
            scanf("%d", &key);
            elem->right = malloc(sizeof(*elem));
            elem->right->parent = elem;
            elem->right->key = key;
            add_in_end_of_list(&head, &end, &elem->right);
        }
        while(head) {
            elem = *head->elem;
            delete_from_beginning_of_a_list(&head);
            scanf("%d", &a);
            if(a) {
                scanf("%d", &key);
                elem->left = malloc(sizeof(*elem));
                elem->left->parent = elem;
                elem->left->key = key;
                add_in_end_of_list(&head, &end, &elem->left);
            }
            scanf("%d", &a);
            if(a) {
                scanf("%d", &key);
                elem->right = malloc(sizeof(*elem));
                elem->right->parent = elem;
                elem->right->key = key;
                add_in_end_of_list(&head, &end, &elem->right);
            }
        }
    }
    return root;
};

void right_single_rotate(struct tree **root, struct tree **a) {
    int flag;
    struct tree *b = (*a)->left, *p = (*a)->parent;
    if(p) {
        if(p->left == *a)
            flag = 1;
        else
            flag = 2;
    }
    else
        flag = 0;
    (*a)->left = b->right;
    if(b->right)
        b->right->parent = *a;
    b->right = *a;
    (*a)->parent = b;
    b->parent = p;
    if(flag) {
        if(flag == 1)
            p->left = b;
        else
            p->right = b;
        *a = b;
    }
    else
        *a = *root = b;
}

void left_single_rotate(struct tree **root, struct tree **a) {
    int flag;
    struct tree *b = (*a)->right, *p = (*a)->parent;
    if(p) {
        if(p->left == *a)
            flag = 1;
        else
            flag = 2;
    }
    else
        flag = 0;
    (*a)->right = b->left;
    if(b->left)
        b->left->parent = *a;
    b->left = *a;
    (*a)->parent = b;
    b->parent = p;
    if(flag) {
        if(flag == 1)
            p->left = b;
        else
            p->right = b;
        *a = b;
    }
    else
        *a = *root = b;
}

void printing(struct tree *t) {
    if(t) {
        char c;
        getchar();
        scanf("%c", &c);
        while(c != 's') { // s - stop, w - write, l - move to left, r - right, p - parent.
            if(c == 'w')
                printf("%d", t->key);
            else if(c == 'l') {
                if(t->left)
                    t = t->left;
                else
                    printf("NULL");
            }
            else if(c == 'r') {
                if(t->right)
                    t = t->right;
                else
                    printf("NULL");
            }
            else if(c == 'p') {
                if(t->parent)
                    t = t->parent;
                else
                    printf("NULL");
            }
            scanf("%c", &c);
        }
    }
}