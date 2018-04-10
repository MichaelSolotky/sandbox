void comparison(FILE *f_1, FILE *f_2) {
    int line = 1, column = 1;
    char c_1, c_2;
    while((fscanf(f_1, "%c", &c_1) != EOF) && (fscanf(f_2, "%c", &c_2) != EOF)) {
        if(c_1 != c_2) {
            if(c_1 == '\n')
                printf("'\\n' != ");
            else
                printf("'%c' != ", c_1);
            if(c_2 == '\n')
                printf("'\\n'\nmistake in a line %d, column %d\n", line, column);
            else
                printf("'%c'\nmistake in a line %d, column %d\n", c_2, line, column);
            return 0;
        }
        if(c_1 == '\n') {
            line++;
            column = 1;
        }
        else
            column++;
    }
    if(fscanf(f_1, "%c", &c_1) != EOF) {
        printf("the second text is a subtext of the first\n");
        return 0;
    }
    if(fscanf(f_2, "%c", &c_2) != EOF) {
        printf("the first text is a subtext of the second\n");
        return 0;
    }
    printf("Equal!\n");
    return 0;
}
