//#include <iostream>
#include <cstdio>
#include <cstring>
#include <malloc.h>

typedef struct list {
    char *example;
    struct list *next;
} list;

typedef struct trie {
    char letter;
    list *translation;
    struct trie *next;
    struct trie *next_letter;
} trie;

void add_in_beginnig_of_trie(trie *&node, trie *&par, char c) {
    trie *tmp = (trie *) calloc(1, sizeof(trie));
    tmp->next = node;
    node = tmp;
    node->letter = c;
    par->next_letter = node;
}

void add_in_midlle_of_trie(trie *&node, char c) {
    trie *tmp = node->next;
    node->next = (trie *) calloc(1, sizeof(trie));
    node = node->next;
    node->next = tmp;
    node->letter = c;
}

void add_in_beginnig_of_list(list *&translation, trie *&node) {
    list *tmp = (list *) malloc(sizeof(list));
    tmp->next = translation;
    translation = tmp;
    node->translation = translation;
}

void add_in_midlle_of_list(list *&node) {
    list *tmp = node->next;
    node->next = (list *) malloc(sizeof(list));
    node = node->next;
    node->next = tmp;
}

void add_translation(trie *&node, char *example) {
    register list *translation = node->translation;
    if (translation) {
        int res = strcmp(translation->example, example);
        if (res > 0) {
            add_in_beginnig_of_list(translation, node);
        } else if (!res) {
            return;
        } else {
            do {
                if (!strcmp(translation->example, example))
                    return;
                if (!translation->next || strcmp(translation->next->example, example) > 0) {
                    add_in_midlle_of_list(translation);
                    break;
                }
                translation = translation->next;
            } while(1);
        }
    } else
        translation = node->translation = (list *) calloc(1, sizeof(list));
    translation->example = (char *) malloc(sizeof(char) * strlen(example));
    strcpy(translation->example, example);
}

void add_in_trie(trie *node, char *word, char *translation) {
    int len = strlen(word);
    trie *par;
    register char c;
    for (int i = 0; i < len; i++) {
        c = word[i];
        if (c >= 'A' && c <= 'Z')
            c = 'a' + c - 'A';
        if (node->next_letter) {
            par = node;
            node = node->next_letter;
            if (node->letter > c) {
                add_in_beginnig_of_trie(node, par, c);
                continue;
            }
            do {
                if (node->letter == c) {
                    break;
                }
                if (!node->next || node->next->letter > c) {
                    add_in_midlle_of_trie(node, c);
                    break;
				}
                node = node->next;
            } while(1);
        } else {
            par = node;
            node = node->next_letter = (trie *) calloc(1, sizeof(trie));
            node->letter = c;
        }
    }
    add_translation(node, translation);
}

void delete_list(list *node) {
    if (node->next) {
        delete_list(node->next);
    }
    free(node);
}

void delete_trie(trie *&node) {
    if (node->next_letter) {
        delete_trie(node->next_letter);
    }
    if (node->next) {
        delete_trie(node->next);
    }
    if (node->translation) {
        delete_list(node->translation);
    }
    free(node);
}

void preprocessing_strings_and_adding(trie *&dictionary, FILE *fi) {
    char *buf = (char *) malloc(100 * sizeof(char));
    fgets(buf, 1000, fi);
    int i = 0;
    for(; buf[i] != '-' && buf[i] != '\n'; i++);
    buf[i - 1] = 0;
    add_in_trie(dictionary, buf, buf + 2);
    free(buf);
}

void construct_trie(trie *&dictionary, FILE *fi) {
    void *read = NULL;
    char *buf = (char *) malloc(100 * sizeof(char));
    do {
        read = fgets(buf, 1000, fi);
        int i = 0;
        for (; buf[i] != '-' && buf[i] != '\n'; i++) { }
        buf[i - 1] = 0;
        add_in_trie(dictionary, buf, buf + 2);
    } while(read);
    free(buf);
}

void show(trie *node) {
    if (!node) {
        return;
    }
    do {
        putc(node->letter, stdout);
        putc('\n', stdout);
        if (node->next_letter) {
            show(node->next_letter);
        }
        if (node->next) {
            node = node->next;
        } else {
            break;
        }
    } while(1);
}

void get_words(trie *dictionary, FILE *out) { }

// put '\n' after all the translation in the file
int main(int argv, char *argc[]) {
    if (argv == 1) {
        return 0;
    }
    if (!strcmp(argc[1], "-help")) {
        printf("-help    print all the available keys.\n");
        printf("-add     add a word in a form [word - translation].\n");
        printf("If there wasn't any translation typed the word will be added anyway.\n");
        return 0;
    }
    FILE *fi = fopen("/home/michael/Documents/Programming/Projects/C++/Dictionary/file", "r+");
    trie *dictionary = (trie *) calloc(1, sizeof(trie));
    construct_trie(dictionary, fi);
    if (!strcmp(argc[1], "-add")) {
        preprocessing_strings_and_adding(dictionary, stdin);
        get_words(dictionary, fi);
    }
    fclose(fi);
    if (dictionary) {
        delete_trie(dictionary);
    }
    return 0;
}
