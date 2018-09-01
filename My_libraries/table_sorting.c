#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stack {
    struct line_pointer *list;
    int level;
};

struct list {
    char *string;
    int num;
    struct list *next_column;
};

struct line_pointer {
    char *string_id;
    int num_id;
    struct list *first_column;
    struct line_pointer *next_line;
};

struct line_pointer *reading(FILE *fi) {
    struct line_pointer *head = malloc(sizeof(struct line_pointer)), *line = head;
    struct list *column = line->first_column = malloc(sizeof(struct list));
    int a, n, i = 0, j, res_of_scanning, flag = 0;
    char c, *s;
    fscanf(fi, "%d", &n);
    res_of_scanning = fscanf(fi, "%d", &a);
    while (res_of_scanning != EOF) {
        if (flag) {
            line->next_line = malloc(sizeof(struct line_pointer));
            line = line->next_line;
            column = line->first_column = malloc(sizeof(struct list));
            flag = 0;
        }
        else {
            column->next_column = malloc(sizeof(struct list));
            column = column->next_column;
        }
        if (res_of_scanning) {
            column->num = a;
            column->string = NULL;
            if(i == n) {
                line->num_id = a;
                line->string_id = NULL;
            }
            fscanf(fi, "%c", &c);
            while ((c != ';') && (c != '\n'))
                fscanf(fi, "%c", &c);
        }
        else {
            fscanf(fi, "%c", &c);
            while (c == ' ')
                fscanf(fi, "%c", &c);
            if (c == '"') {
                s = malloc(101 * sizeof(char));
                fscanf(fi, "%c", &c);
                j = 0;
                while (c != '"') {
                    s[j] = c;
                    j++;
                    fscanf(fi, "%c", &c);
                }
                s[j] = '\0';
                column->string = s;
                while ((c != ';') && (c != '\n'))
                    fscanf(fi, "%c", &c);
                if(i == n)
                    line->string_id = s;
            }
            else
                abort();
        }
        i++;
        if(c == '\n') {
            flag = 1;
            column->next_column = NULL;
            i = 0;
        }
        res_of_scanning = fscanf(fi, "%d", &a);
    }
    column = head->first_column;
    head->first_column = head->first_column->next_column;
    free(column);
    line->next_line = NULL;
    return head;
}

void add_in_list(struct line_pointer **head, struct line_pointer **end, struct line_pointer *elem) {
    if(*end) {
        (*end)->next_line = elem;
        *end = (*end)->next_line;
    }
    else {
        *head = elem;
        *end = *head;
    }
    (*end)->next_line = NULL;
}

struct line_pointer *string_list_merge(struct line_pointer *head_1, struct line_pointer *head_2) {
    struct line_pointer *res_head = NULL, *res_end = NULL, *elem;
    if((head_1) && (head_2)) {
        while((head_1) && (head_2)) {
            if (strcmp(head_1->string_id, head_2->string_id) < 0) {
                elem = head_1;
                head_1 = head_1->next_line;
            }
            else {
                elem = head_2;
                head_2 = head_2->next_line;
            }
            elem->next_line = NULL;
            add_in_list(&res_head, &res_end, elem);
        }
        if(head_1)
            res_end->next_line = head_1;
        else
            res_end->next_line = head_2;
    }
    return res_head;
}

struct line_pointer *int_list_merge(struct line_pointer *head_1, struct line_pointer *head_2) {
    struct line_pointer *res_head = NULL, *res_end = NULL, *elem;
    if((head_1) && (head_2)) {
        while((head_1) && (head_2)) {
            if (head_1->num_id < head_2->num_id) {
                elem = head_1;
                head_1 = head_1->next_line;
            }
            else {
                elem = head_2;
                head_2 = head_2->next_line;
            }
            elem->next_line = NULL;
            add_in_list(&res_head, &res_end, elem);
        }
        if(head_1)
            res_end->next_line = head_1;
        else
            res_end->next_line = head_2;
    }
    return res_head;
}

void string_list_sort(struct line_pointer **head) {
    int i = 0, j, k;
    struct line_pointer *tmp;
    struct stack *stack;
    stack = malloc(32 * sizeof(*stack));
    while(*head) {
        int flag = 0;
        tmp = *head;
        *head = (*head)->next_line;
        tmp->next_line = NULL;
        stack[i].list = tmp;
        stack[i].level = 1;
        i++;
        if(i > 1) {// I want to merge 2 lists if not only in top levels are equal but everywhere
            while(!flag) {
                flag = 1;
                for(j = i - 1; j > 0; j--)
                    if(stack[j].level == stack[j - 1].level) {
                        stack[j - 1].list = string_list_merge(stack[j].list, stack[j - 1].list);
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
        stack[i - 1].list = string_list_merge(stack[i].list, stack[i - 1].list);
        i--;
    }
    *head = stack[0].list;
    free(stack);
}

void int_list_sort(struct line_pointer **head) {
    int i = 0, j, k;
    struct line_pointer *tmp;
    struct stack *stack;
    stack = malloc(32 * sizeof(*stack));
    while(*head) {
        int flag = 0;
        tmp = *head;
        *head = (*head)->next_line;
        tmp->next_line = NULL;
        stack[i].list = tmp;
        stack[i].level = 1;
        i++;
        if(i > 1) {// I want to merge 2 lists if not only in top levels are equal but everywhere
            while(!flag) {
                flag = 1;
                for(j = i - 1; j > 0; j--)
                    if(stack[j].level == stack[j - 1].level) {
                        stack[j - 1].list = int_list_merge(stack[j].list, stack[j - 1].list);
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
        stack[i - 1].list = int_list_merge(stack[i].list, stack[i - 1].list);
        i--;
    }
    *head = stack[0].list;
    free(stack);
}

void printing(FILE *fo, struct line_pointer *head) {
    struct line_pointer *tmp;
    struct list *column, *tmp_column;
    while(head) {
        tmp = head;
        column = head->first_column;
        while(column) {
            if(column->string) {
                fprintf(fo, "\"%s\"", column->string);
                free(column->string);
            }
            else
                fprintf(fo, "%d", column->num);
            if(column->next_column)
                fprintf(fo, ";");
            tmp_column = column;
            column = column->next_column;
            free(tmp_column);
        }
        fprintf(fo, "\n");
        head = head->next_line;
        free(tmp);
    }
}

int main(void) {
    FILE *fi = fopen("input.txt", "r"), *fo = fopen("output.txt", "w");
    struct line_pointer *head = reading(fi);
    if (head->string_id)
        string_list_sort(&head);
    else
        int_list_sort(&head);
    printing(fo, head);
    fclose(fi); fclose(fo);
    return 0;
}