// g++ main.cpp -o main -O2 -pipe -lm
#include <cstdio>
#include <cstdlib>
#define newline putc('\n', stdout)
#define test printf("test\n")

typedef struct node {
    int val;
    struct node *right, *down;
} node;

node *construction(int n, int m) {
    node *arr = (node *) calloc(n * m, sizeof(node));
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < m - 1; j++) {
            arr[i * m + j].right = arr + i * m + j + 1;
            arr[i * m + j].down = arr + (i + 1) * m + j;
        }
    }
    for (int i = 1; i < n - 2; i++) {
        for (int j = 1; j < m - 2; j++) {
            scanf("%d", arr + i * m + j); // write right in val
        }
    }
    return arr;
}

void find_positions(node *&up1, node *&up2, node *&dw1, 
    node *&dw2, node *&lf1, node *&lf2, node *&rg1, node *&rg2, 
    node *arr, int a, int b, int w, int m, int h) {
    if (a < b) {
        up1 = arr + b - 1;
        for (int i = 0; i < a - 1; i++) {
            up1 = up1->down;
        }
    } else {
        up1 = arr + (a - 1) * m;
        for (int i = 0; i < b - 1; i++) {
            up1 = up1->right;
        }
    }
    lf1 = up1->down;
    up1 = up1->right;
    lf2 = up2 = up1->down;
    if (h > b) {
        dw1 = arr + (a + h - 1) * m;
        for (int i = 0; i < b; i++) {
            dw1 = dw1->right;
        }
    } else {
        dw1 = up2;
        for(int i = 1; i < h; i++) {
            dw1 = dw1->down;
        }
    }
    dw2 = dw1->down;
    if (w > a) {
        rg1 = arr + b + w - 1;
        for (int i = 0; i < a; i++) {
            rg1 = rg1->down;
        }
    } else {
        rg1 = lf2;
        for (int i = 1; i < w; i++) {
            rg1 = rg1->right;
        }
    }
    rg2 = rg1->right;
}

void processing(node *arr, int q, int m) {
    int a, b, c, d, h, w;
    node *up_src_1, *up_src_2, *up_dst_1, *up_dst_2;
    node *lf_src_1, *lf_src_2, *lf_dst_1, *lf_dst_2;
    node *rg_src_1, *rg_src_2, *rg_dst_1, *rg_dst_2;
	node *dw_src_1, *dw_src_2, *dw_dst_1, *dw_dst_2;
    for (int i = 0; i < q; i++) {
        scanf("%d%d%d%d%d%d", &a, &b, &c, &d, &h, &w);
        find_positions(
            up_src_1, up_src_2, dw_src_1, dw_src_2, lf_src_1, lf_src_2, rg_src_1,
            rg_src_2, arr, a, b, w, m, h);
        find_positions(
            up_dst_1, up_dst_2, dw_dst_1, dw_dst_2, lf_dst_1, lf_dst_2, rg_dst_1,
            rg_dst_2, arr, c, d, w, m, h);
        for(int i = 0; i < h; i++) {
            lf_src_1->right = lf_dst_2;
            lf_dst_1->right = lf_src_2;
            rg_src_1->right = rg_dst_2;
            rg_dst_1->right = rg_src_2;
            lf_src_1 = lf_src_1->down, lf_src_2 = lf_src_2->down;
            lf_dst_1 = lf_dst_1->down, lf_dst_2 = lf_dst_2->down;
            rg_src_1 = rg_src_1->down, rg_src_2 = rg_src_2->down;
            rg_dst_1 = rg_dst_1->down, rg_dst_2 = rg_dst_2->down;
        }
        for(int i = 0; i < w; i++) {
            up_src_1->down = up_dst_2;
            up_dst_1->down = up_src_2;
            dw_src_1->down = dw_dst_2;
            dw_dst_1->down = dw_src_2;
            up_src_1 = up_src_1->right, up_src_2 = up_src_2->right;
            up_dst_1 = up_dst_1->right, up_dst_2 = up_dst_2->right;
            dw_src_1 = dw_src_1->right, dw_src_2 = dw_src_2->right;
            dw_dst_1 = dw_dst_1->right, dw_dst_2 = dw_dst_2->right;
        }
    }
}

void print(node *arr, int n, int m) {
    newline;
    node *start = arr[m].right; // &arr[1][1]
    node *tmp;
    for (int i = 1; i < n - 2; i++, start = start->down) {
        tmp = start;
        for (int j = 1; j < m - 2; j++, tmp = tmp->right) {
            printf("%d ", tmp->val);
        }
        newline;
    }
}

int main() {
    int n, m, q;
    scanf("%d%d%d", &n, &m, &q);
    m += 3, n += 3;
    node *arr = construction(n, m);
    processing(arr, q, m);
    print(arr, n, m);
    free(arr);
    return 0;
}
