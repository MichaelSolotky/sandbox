typedef struct single_linked_list {
    int key;
    struct single_linked_list *next;
} slist;

void push_back(slist **head, slist **end, int n) {
    if(*end) {
        (*end)->next = malloc(sizeof(**head));
        *end = (*end)->next;
    }
    else {
        *head = *end = malloc(sizeof(**head));
    }
    (*end)->key = n;
    (*end)->next = NULL;
}

void push_in_front(slist **head, int n) {
    if(*head) {
        struct stack *new_head = malloc(sizeof(**head));
        new_head->next = *head;
        *head = new_head;
    }
    else {
        *head = malloc(sizeof(**head));
        (*head)->next = NULL;
    }
    (*head)->figure = n;
}

void creation_of_a_list(slist **head, slist **end, int n) {
    for(int i = 0; i < n; i++) {
        int key;
        scanf("%d", &key);
        if(*end) {
            (*end)->next = malloc(sizeof(**end));
            *end = (*end)->next;
            (*end)->key = key;
        }
        else {
            *head = malloc(sizeof(**end));
            (*head)->key = key;
            *end = *head;
        }
        if(*end)
            (*end)->next = NULL; // may be written in the main function
    }
}

void delete_list(slist *head) {
    slist *tmp;
    while(head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void remove_from_list(slist **head, slist *elem, slist *prev) {
    if(*head == elem)
        *head = (*head)->next;
    else
        prev->next = elem->next;
    free(elem);
}