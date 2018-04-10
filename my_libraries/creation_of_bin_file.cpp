#include <iostream>

using namespace std;

int main() {
    FILE *fi = fopen("input.bin", "wb");
    int n;
    scanf("%d", &n);
    int *a = (int *) malloc(n * sizeof(int));
    for(int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }
    fwrite(a, sizeof(int), n, fi);
    fclose(fi);
    return 0;
}