struct double_linked_list {
    int key;
    struct double_linked_list *next, *prev;
};

void push_back(struct single_linked_list **head, struct single_linked_list **end, int n) {
    if(*end) {
        (*end)->next = malloc(sizeof(**head));
        (*end)->next->prev = *end;
        *end = (*end)->next;
    }
    else {
        *head = *end = malloc(sizeof(**head));
        (*head)->prev = NULL;
    }
    (*end)->key = n;
    (*end)->next = NULL;
}

void push_in_front(struct single_linked_list **head, int n) {
    if(*head) {
        struct stack *new_head = malloc(sizeof(**head));
        (*head)->prev = new_head;
        new_head->next = *head;
        *head = new_head;
    }
    else {
        *head = malloc(sizeof(**head));
        (*head)->next = NULL;
    }
    (*head)->prev = NULL;
    (*head)->figure = n;
}

void creation_of_a_list(struct double_linked_list **head, struct double_linked_list **end, int n) {
    for (int i = 0; i < n; i++) {
        int key;
        scanf("%d", &key);
        if (*end) {
            (*end)->next = malloc(sizeof(**end));
            (*end)->next->prev = *end;
            *end = (*end)->next;
            (*end)->key = key;
        }
        else {
            *head = malloc(sizeof(**end));
            (*head)->key = key;
            *end = *head;
            (*head)->prev = NULL;
        }
        if(*end)
            (*end)->next = NULL;
    }
}

void delete_list(struct double_linked_list *head) {
    struct double_linked_list *tmp;
    while(head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void remove_from_list(struct double_linked_list **head, struct double_linked_list *elem) {
    if(elem->prev) {
        elem->prev->next = elem->next;
        if(elem->next)
            elem->next->prev = elem->prev;
    }
    else {
        *head = (*head)->next;
        if(*head)
            (*head)->prev = NULL;
    }
    free(elem);
}