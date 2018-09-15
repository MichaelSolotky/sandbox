#include <stdlib.h>
#include <stdio.h>

void print_allocation(const int n, int *arr) {
  for (int i = 0 ; i < n; ++i) {
    printf("%d", arr[i]);
  }
  puts("");
}

void generate_all_allocations(const int n) {
  // initialization
  int *arr = malloc(n * sizeof(n));
  for (int i = 0; i < n; ++i) {
    arr[i] = 1;
  }
  print_allocation(n, arr);

  int finish_flag = 0;
  while (1) {
    for (int i = n - 1; i >= 0; --i) {
      if (arr[i] < n) {
        ++arr[i];
        for (int j = i + 1; i < n; ++i) {
          arr[j] = 1;
        }
        print_allocation(n, arr);
        break;
      } else if (i == 0) {
        finish_flag = 1;
        break;
      }
    }
    if (finish_flag) {
      break;
    }
  }
  free(arr);
}

int main(void) {
  generate_all_allocations(1);
}
