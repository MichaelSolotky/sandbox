typedef struct single_linked_list {
    int key;
    struct single_linked_list *next;
} slist;

void push_back(slist *&head, slist *&end, int n) {
    if(end) {
        end->next = (slist *) calloc(1, sizeof(slist));
        end = end->next;
    } else
        head = end = (slist *) calloc(1, sizeof(slist));
    end->key = n;
}

// pushing in the ascending order of the key
void push_in_list(slist *&head, int n, int dist) {
    if(head->key == n)
        head->distance = min(head->distance, dist);
    else if(head->key > n) {
        list *tmp = (slist *) malloc(sizeof(list));
        tmp->key = n;
        tmp->distance = dist;
        head = tmp->next = head;
    } else {
        list *start = head;
        while(start->next) {
            if(start->next->key == n) {
                start->next->distance = min(start->next->distance, dist);
                return;
            } else if(start->next->key > n)
                break;
        }
        list *tmp = start-> next;
        start->next = (slist *) malloc(sizeof(list));
        start->next->key = n;
        start->next->dist = dist;
        start->next->next = tmp;
    }
}

bool search_in_list(slist *head, int key, int &res) {
    while(head) {
        if(head->key == key) {
            res = head->distance;
            return true;
        }
        head = head->next;
    }
    return false;
}

bool search_in_sorted_list(list *head, int key, int &res) {
    while(head) {
        if(head->key > key)
            break;
        if(head->key == key) {
            res = head->distance;
            return true;
        }
        head = head->next;
    }
    return false;
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