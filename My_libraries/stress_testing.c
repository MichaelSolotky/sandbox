#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int comparisons_for_shell, exchanges_for_shell, comparisons_for_qsort, exchanges_for_qsort;

int increment(int *inc, int size) {// calculating an array inc with numbers determined by formula below.
// These numbers are steps - distances between elements of all subarrys. We are sorting these subarays with selection sort.
// We choose steps in an inverse order. On the last step (inc[0] == 1) we sort the whole array with selection sort.
    int s = -1, e1 = 1, e2 = 1, e3 = 1;
    do {// formula for inc: arg is even => inc = 9 * 2 ^ s - 9 * 2 ^ (s / 2) + 1,
        // arg is odd => inc = 8 * 2 ^ s - 6 * 2 ^ ((s + 1) / 2) + 1;
        if (++s % 2)
            inc[s] = 8 * e1 - 6 * e2 + 1;
        else {
            inc[s] = 9 * e1 - 9 * e3 + 1;
            e2 *= 2;
            e3 *= 2;
        }
        e1 *= 2;
    } while(3 * inc[s] <= size); // number of elements we have to calculate depends on size of array
    return s > 0 ? --s : 0;
}

void shell_sort(double *a, int size) {
    int inc, i, j, seq[40];
    int s;
    s = increment(seq, size);
    while (s >= 0) {
        inc = seq[s--];
        for(i = inc; i < size; i++) {
            double temp = a[i];
            for(j = i - inc; (j >= 0) && (a[j] > temp); j -= inc, comparisons_for_shell++) {
                exchanges_for_shell++;
                a[j + inc] = a[j];
            }
            exchanges_for_shell++;
            a[j + inc] = temp;
        }
    }
}

void Quicksort(double *a, int begin, int end) {
    int i = begin, j = end;
    double tmp, comp = a[(begin + end) / 2];
    do {
        comparisons_for_qsort++;
        while(a[i] < comp && i < end) {
            i++;
            comparisons_for_qsort++;
        }
        comparisons_for_qsort++;
        while(a[j] > comp && j > begin) {
            j--;
            comparisons_for_qsort++;
        }
        comparisons_for_qsort++;
        if(i <= j) {
            exchanges_for_qsort++;
            tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
            i++; j--;
        }
        comparisons_for_qsort++;
    } while(i <= j);
    if(j > begin)
        Quicksort(a, begin, j);
    if(i < end)
        Quicksort(a, i, end);
}

void quicksort(double *a, int size) {
    Quicksort(a, 0, size - 1);
}

void generate(double *a, double *b, int size) { // as we need 2 identical arrays, we send in a function 2 pointers
    double tmp_1, tmp_2, tmp_3;
    for(int j = 0; j < size; j++) {
        tmp_1 = rand();
        tmp_2 = rand();
        tmp_3 = rand();
        while(!tmp_2)
            tmp_2 = rand();
        b[j] = a[j] = tmp_1 / tmp_2 * tmp_3;
    }
}

int check_up(double *a, double *b, int size) {
    double tmp_max = a[0];
    int flag = 1;
    if(a[0] != b[0])
        return 0;
    else {
        for(int i = 1; i < size; i++) {
            if((a[i] < tmp_max) || (a[i] != b[i])) {
                flag = 0;
                break;
            }
            if(a[i] > tmp_max)
                tmp_max = a[i];
        }
        return flag;
    }
}

int testing(void) {
// It generates many times different arrays (every time 3 equal arrays generates, 2 for 2 types of sorting and 1
// to return a test in a case of mistake), checks-up 2 types of sorting and returns 0 if a mistake happened and 1 if all right.
// On the same data 2 types of sorting must return the same data and it must be sorted - the function checks-up that
// Before every sorting we save a test in a file "test.txt" and if we have an infinite cycle, we can extract that test
// from a file. If a mistake occurred and there isn't an infinite cycle program prints a message.
    const int n = 1000;
    const int size = 100;
    int i, j, k, flag = 1;
    double *a, *b, tmp_1, tmp_2, tmp_3;
    FILE *fo;
    for(i = 0; i < n; i++) {
        a = malloc(size * sizeof(double));
        b = malloc(size * sizeof(double));// one of these arrays has been kept to return in a file in a case of mistake
        fo = fopen("test.txt", "w");
        generate(a, b, size);
        for(k = 0; k < size; k++)
            fprintf(fo, "%lf ", a[k]);
        fclose(fo);
        shell_sort(a, size);
        quicksort(b, size);
        if(!check_up(a, b, size)) {
            printf("mistake, passed %d from 1000", i);
            free(a); free(b);
            return 0;
        }
        free(a); free(b);
    }
    return flag;
}

int main(void) {
    int i, j, size[4] = {10, 100, 1000, 10000};
    double *a, *b, tmp_1, tmp_2, tmp_3; // the same array must be copied to make sorting 2 times
    const int n = 4;
    srand((unsigned int) time(NULL));
    if(!testing())
        abort();
    for(i = 0; i < n; i++) {
        a = malloc(size[i] * sizeof(double));
        b = malloc(size[i] * sizeof(double));
        generate(a, b, size[i]);
        comparisons_for_shell = exchanges_for_shell = comparisons_for_qsort = exchanges_for_qsort = 0;
        shell_sort(a, size[i]);
        quicksort(b, size[i]);
        printf("size %d\ncomparisons_for_shell\t%d, ", size[i], comparisons_for_shell);
        printf("\texchanges_for_shell\t%d, ", exchanges_for_shell);
        printf("\tcomparisons_for_qsort\t%d, ", comparisons_for_qsort);
        printf("\texchanges_for_qsort\t%d\n", exchanges_for_qsort);
        free(a); free(b);
    }
    //printf("right!");
    return 0;
}