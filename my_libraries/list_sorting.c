#include <stdio.h>
#include <stdlib.h>

struct list {
    int key;
    struct list *next;
};

struct stack {
    struct list *list;
    int level;
};

void creation_of_a_list(struct list **head, struct list **end, FILE *fi) {
    int key;
    while(fscanf(fi, "%d", &key) != EOF) {
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
    }
    if(*end)
        (*end)->next = NULL;
}

void add_in_list(struct list **head, struct list **end, struct list *elem) {
    if(*end) {
        (*end)->next = elem;
        *end = (*end)->next;
    }
    else {
        *head = elem;
        *end = *head;
    }
    (*end)->next = NULL;
}

struct list *list_merge(struct list *head_1, struct list *head_2) {
    struct list *res_head = NULL, *res_end = NULL, *elem;
    if((head_1) && (head_2)) {
        while((head_1) && (head_2)) {
            if (head_1->key <= head_2->key) {
                elem = head_1;
                head_1 = head_1->next;
            }
            else {
                elem = head_2;
                head_2 = head_2->next;
            }
            elem->next = NULL;
            add_in_list(&res_head, &res_end, elem);
        }
        if(head_1)
            res_end->next = head_1;
        else
            res_end->next = head_2;
    }
    return res_head;
}

void list_sort(struct list **head) {
    int i = 0, j, k;
    struct list *tmp;
    struct stack *stack;
    stack = malloc(32 * sizeof(*stack));
    while(*head) {
        int flag = 0;
        tmp = *head;
        *head = (*head)->next;
        tmp->next = NULL;
        stack[i].list = tmp;
        stack[i].level = 1;
        i++;
        if(i > 1) {// I want to merge 2 lists if not only in top levels are equal but everywhere
            while(!flag) {
                flag = 1;
                for(j = i - 1; j > 0; j--)
                    if(stack[j].level == stack[j - 1].level) {
                        stack[j - 1].list = list_merge(stack[j].list, stack[j - 1].list);
                        stack[j - 1].level++;
                        i--;
                        for(k = j; k < i - 1; k++)
                            stack[k] = stack[k + 1];
                        flag = 0;
                    }
                    else
                        flag *= 1;
            }
        }
    }
    i--;
    while(i) {
        stack[i - 1].list = list_merge(stack[i].list, stack[i - 1].list);
        i--;
    }
    *head = stack[0].list;
    free(stack);
}

int main(void) {
    FILE *fi, *fo;
    fi = fopen("input.txt", "r");
    fo = fopen("output.txt", "w");
    struct list *head = NULL, *end = NULL, *tmp;
    creation_of_a_list(&head, &end, fi);
    if(head)
        list_sort(&head);
    while(head) {
        fprintf(fo, "%d ", head->key);
        printf("%d ", head->key);
        tmp = head;
        head = head->next;
        free(tmp);
    }
    fclose(fi); fclose(fo);
    return 0;
}