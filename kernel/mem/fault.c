#include "fault.h"




void panic_impl(const char* msg, const char* file, uint32 line)
{
    printf("KPANIC %s at %s:%d\n", msg, file, line);
    for(;;)
        ;
}
