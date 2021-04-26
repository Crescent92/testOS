
#include<printf.h>

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        cprintf("%s ", argv[i]);
    }
    cprintf("\n");
}