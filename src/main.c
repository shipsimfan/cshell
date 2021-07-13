#include <los.h>

int main() {
    console_write_str("Hello world!\n");

    ProcessID pid = execute("1:/LOS/CAT.APP");
    wait_process(pid);

    console_write_str("Cat exited!\n");

    return 0;
}
