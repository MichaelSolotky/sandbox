#include "preprocessing.h"

void print_list(list *head) {
    while (head) {
        if (head->data.symb) {
            printf("%c ", head->data.symb);
        } else {
            printf("%g ", head->data.num);
        }
        head = head->next;
    }
}

char preprocess_symb(char *buf, int &i) {
    char c = *buf;
    switch(c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case 'x':
        case '(':
        case ')':
        case ' ':
        case '\n':
            i++;
            return c;
        case 's': // sine is encrypted with code 's'
            if (buf[1] == 'i' && buf[2] == 'n') {
                c = 's';
                i += 3;
            } else if (buf[1] == 'q' && buf[2] == 'r' && buf[3] == 't') {
                c = 'r'; // square is encrypted with q
                i += 4;
            } else if (buf[1] == 'q' && buf[2] == 'r') {
                c = 'q';  // square root is encrypted with q
                i += 3;
            } else if (buf[1] == 'h') {
                c = '6'; // hyperbolic sine is encrypted with code 6
                i += 2;
            } else {
                c = 0;
                i++;
            }
            return c;
        case 'X':
            c = 'x';
            i++;
            return c;
        case 'c': // cosine is encrypted with code 'c', ctg is encrypted with 'g'
            if (buf[1] == 'o' && buf[2] == 's') {
                c = 'c';
                i += 3;
            } else if (buf[1] == 't' && buf[2] == 'g') {
                c = 'g';
                i += 3;
            } else if (buf[1] == 't' && buf[2] == 'h') {
                c = '8'; // hyperbolic cotangent is encrypted with 8
                i += 3;
            } else if (buf[1] == 'h') {
                c = '5'; // hyperbolic cosine is encrypted with 5
                i += 2;
            } else {
                c = 0;
                i++;
            }
            return c;
        case 't': // tangent is encrypted with code 't'
            if (buf[1] == 'a' && buf[2] == 'n') {
                c = 't';
                i += 3;
            } else if (buf[1] == 'g') {
                c = 't';
                i += 2;
            } else if (buf[1] == 'h') {
                c = '7'; // hyperbolic tangent is encrypted with 7
                i += 2;
            } else {
                c = 0;
                i++;
            }
            return c;
        case 'p': // constant pi is encrypted with code 'p'
            if (buf[1] == 'i')
                c = 'p';
            else
                c = 0;
            i += 2;
            return c;
        case 'l': // natural logarithm is encrypted with code 'n', logarithm is encrypted with 'l'
            if (buf[1] == 'n') {
                c = 'n';
                i += 2;
            } else if (buf[1] == 'o' && buf[2] == 'g') {
                c = 'l';
                i += 3;
            } else if (buf[1] == 'g') { // decimal logarithm is encrypted with 9
                c = '9';
                i += 2;
            } else {
                c = 0;
                i++;
            }
            return c;
        case 'a':
            if (buf[1] == 'r' && buf[2] == 'c')
                if (buf[3] == 't' && buf[4] == 'a' && buf[5] == 'n') {
                    c = '1'; // arc tangent is encrypted with 1
                    i += 6;
                } else if (buf[3] == 't' && buf[4] == 'g') {
                    c = '1'; // arc tangent is encrypted with 1
                    i += 5;
                } else if (buf[3] == 'c' && buf[4] == 't' && buf[5] == 'g') {
                    c = '2'; // arc cotangent is encrypted with 1
                    i += 6;
                } else if (buf[3] == 's' && buf[4] == 'i' && buf[5] == 'n') {
                    c = '3'; // arc sine is encrypted with 1
                    i += 6;
                } else if (buf[3] == 'c' && buf[4] == 'o' && buf[5] == 's') {
                    c = '4'; // arc cosine is encrypted with 1
                    i += 6;
                } else {
                    c = 0;
                    i++;
                }
            else {
                c = 0;
                i++;
            }
            return c;
        case 'e': // Euler's number is encrypted with code 'e'
            i++;
            return c;
        default:
            i++;
            return 0;
    }
}

list *preprocessing(char *buf, int size, int &calc_flag, double &x0, char *possible_symbols) {
// creates a linear tree with nodes, which are operators or numbers
    // there are no symb 'p' and 'e' in list
    list *head = NULL, *node = NULL;
    int i = 0, bracket_balance = 0;
    while (buf[i] && buf[i] != ';') {
        if (buf[i] >= '0' && buf[i] <= '9') {
            preprocess_number(head, node, buf, possible_symbols, i);
        } else {
            char c = preprocess_symb(buf + i, i);
            switch(c) {
                case '(':
                    bracket_balance++;
                    break;
                case ')':
                    bracket_balance--;
                    break;
                case '-':
                    preprocess_minus(head, node, possible_symbols);
                    continue;
                case ' ':
                case '\n':
                    continue;
                default:;
            }
            int continue_flag = 0;
            if (node) {
                add_brackets_and_multipl(head, node, c, possible_symbols, continue_flag);
            }
            if (continue_flag) {
                continue;
            }
            if (c) {
                switch(c) {
                    case 'e':
                        add_in_list(head, node, 0, M_E);
                        break;
                    case 'p':
                        add_in_list(head, node, 0, M_PI);
                        break;
                    default:
                        add_in_list(head, node, c, 0);
                }
            }
            else {
                char string[20] = "unknown symbol ";
                write_mistake(string, c);
            }
        }
    }
    if (bracket_balance) {
        char string[35] = "bracket balance's been disturbed";
        write_mistake(string, 0xff);
    }
    if (buf[i] == ';') {
        read_point_for_calculating(calc_flag, i, size, buf, x0);
    }
    return head;
}

void add_brackets_and_multiply(list *&head,
                               list *&node,
                               char last_read_symbol,
                               char *possible_symbols,
                               int &continue_flag) {
    char symb = node->data.symb;
    if (last_read_symbol != '(') {
        if (search_in_possible_symbols(possible_symbols, symb, 6, 21)) {
            add_in_list(head, node, '(', 0);
            add_in_list(head, node, last_read_symbol, 0);
            add_in_list(head, node, ')', 0);
            continue_flag = 1;
            return;
        }
    }
    if ((last_read_symbol == '(' && (symb == 'x' || !symb))) {
        add_in_list(head, node, '*', 0);
        return;
    }
    if ((symb == ')' || !symb || symb == 'x') &&
            (search_in_possible_symbols(possible_symbols, last_read_symbol, 5, 24))) {
        add_in_list(head, node, '*', 0);
    }
}

void preprocess_number(list *&head, list *&node, char *buf, char *possible_symbols, int &i) {
    double num = get_number(buf + i, i);
    if (node && search_in_possible_symbols(possible_symbols, node->data.symb, 6, 21)) {
        add_in_list(head, node, '(', 0);
        add_in_list(head, node, 0, num);
        add_in_list(head, node, ')', 0);
    } else {
        if (node && (node->data.symb == ')' || node->data.symb == 'x'))
            add_in_list(head, node, '*', 0);
        add_in_list(head, node, 0, num);
    }
}

void preprocess_minus(list *&head, list *&node, char *possible_symbols) {
    int flag = !node || search_in_possible_symbols(possible_symbols, node->data.symb, 1, 21);
    if (flag) {
        add_in_list(head, node, 0, -1);
        add_in_list(head, node, '*', 0);
    } else
        add_in_list(head, node, '-', 0);
}

void read_point_for_calculating(int &calc_flag, int &i, int size, char *buf, double &x0) {
    calc_flag = 1;
    buf[i++] = 0;
    int pos = -1; // saves the position of beginning of a number
    for (; i < size; i++)
        if (buf[i] >= '0' && buf[i] <= '9') {
            pos = i;
            break;
        }
    if (pos < 0) {
        char string[50] = "point for calculating wasn't found";
        write_mistake(string, 0xff);
    }
    x0 = get_number(buf + i, i);
}

double get_number(char *string, int &curr_pos) { // gets number from a string where are other symbols
    int i = 0;
    while(string[i] >= '0' && string[i] <= '9') {
        i++;
    }
    char tmp = string[i];
    string[i] = 0;
    double res = atof(string);
    string[i] = tmp;
    curr_pos += i;
    return res;
}

list *read_expression(double &x0, int &calc_flag, char *possible_symbols) { // need to be tested on big numbers
    FILE *input = fopen("/home/michael/ClionProjects/C++/differentiator/input.txt", "r");
    fseek(input, 0, SEEK_END);
    int count = (int) ftell(input);
    fseek(input, 0, SEEK_SET);
    char *buf = (char *) calloc((size_t) count + 100, 1); // +100 because I don't know how much memory text file needs
    fread(buf, 1, (size_t) count, input);
    fclose(input);
    list *head = NULL;
    if(buf[0]) {
        head = preprocessing(buf, count + 100, calc_flag, x0, possible_symbols);
    }
    free(buf);
    return head;
}

void add_in_list(list *&head, list *&node, char symb, double num) {
    if(head) {
        list *next = node->next;
        node->next = (list *) malloc(sizeof(list));
        node = node->next;
        node->next = next;
    } else {
        head = node = (list *) calloc(sizeof(list), 1);
    }
    node->data.symb = symb;
    node->data.num = num;
}

int search_in_possible_symbols(char *possible_symbols, char symb, int begin, int end) {
    for (int j = begin; j <= end; j++) {
        if (symb == possible_symbols[j]) {
            return 1;
        }
    }
    return 0;
}

void write_mistake(char *string, char c) {
    FILE *output = fopen("/home/michael/ClionProjects/C++/differentiator/output.txt", "w");
    fprintf(output, "%s", string);
    if (c != 0xff) {
        fprintf(output, "%c", c);
    }
    fprintf(output, "\n");
    fclose(output);
    printf("mistake\n");
    abort();
}

void delete_head(list *&head) { // it's called only in 1 function and there is checked that head != NULL
    list *prev = head;
    head = head->next;
    free(prev);
}

void checking_flags(int &test_flag, int argv, char **argc) {
    for (int i = 0; i < argv; i++) {
        if (!strcmp(argc[i], "-t")) {
            test_flag = 1;
            return;
        }
    }
}
