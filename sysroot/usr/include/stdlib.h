#ifndef _STDLIB_H
#define _STDLIB_H
#include "sys/types.h"
#include "common.h"

void *malloc(unsigned int size);
void free(void *ptr);
void abort();
int atexit(void (*)(void));
char *getenv(const char *);

#endif