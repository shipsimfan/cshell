#include <los.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    uint64_t* ptr = (uint64_t*)malloc(sizeof(uint64_t));

    *ptr = 0xABCDEF;

    printf("Hello World!\n%#lX\n", *ptr);

    ProcessID pid = execute("1:/LOS/CAT.APP");
    uint64_t status = wait_process(pid);

    printf("Cat exited with status %#lX\n", status);

    return 0x12345678;
}
