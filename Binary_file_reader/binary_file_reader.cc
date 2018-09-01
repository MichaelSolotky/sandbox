#include <cstdlib>
#include <cstdio>

enum { BLOCK_SIZE = 65536 };

int main(int argc, const char **argv) {
    FILE *bin = fopen(argv[1], "rb");
    int *buf = (int *) malloc(BLOCK_SIZE * sizeof *buf);
    int read_bytes = fread(buf, sizeof *buf, BLOCK_SIZE, bin);
    while (read_bytes) {
        for (int i = 0; i < read_bytes; ++i)
            printf("%d\n", buf[i]);
        read_bytes = fread(buf, sizeof *buf, BLOCK_SIZE, bin);
    }
}
