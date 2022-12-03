#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    uint64_t size = 1024 * 1024 * 1024;
    int gb = atoi(argv[1]);
    printf("Allocating %d GB of memory\n", gb);
    char* ptrs[gb];
    for (int i = 0; i < gb; ++i) {
        ptrs[i] = (char*)malloc(size);
        for (int j = 0; j < size; ++j) {
            ptrs[i][j] = 43;
        }
    }
    while (1) {
        for (int i = 0; i < gb; ++i) {
            for (int j = 0; j < size; ++j) {
                if (ptrs[i][j] != 43) {
                    printf("Memory corruption detected\n");
                    return 1;
                }
            }
        }
    }
    printf("Allocated %d GB of memory\n", gb);
    puts("Press any key to continue...");
    getchar();
}
