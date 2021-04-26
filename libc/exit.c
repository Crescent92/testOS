#include "type.h"
#include "syscalls.h"

void exit (int32 exit_code) {
    sys_exit(exit_code);
}