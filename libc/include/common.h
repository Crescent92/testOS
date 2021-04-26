#ifndef _COMMON_H
#define _COMMON_H

#include "type.h"

#define my_sizeof(type) (char *)(&type+1)-(char*)(&type)

void *memcpy(void *dst, const void *src, uint32 n);

int memcmp(const void *s1, const void *s2, uint32 n);

void * memset(void *s, int c, int n);

void * memmove(void *dst, const void *src, uint32 size);

int abs (int arg);

int atoi(char *p);

long atol(char *p);

void bzero(void *p, int len);

char *strchar(const char *s, int c);

int strcmp(char *str1, char *str2);

int strcpy(char *tar, const char *src); 

char* strcat(char* dest, const char* src);

uint32 strlen(const char *str);

char *strncat(char *s1, const char *s2, int n);

int strncmp(char *s1, char *s2, int n);

char *strncpy(char *s1, const char *s2, int n);

int strnlen (const char *s, uint32 len);

char* strchr(const char* str, int ch);

char *strrchr(const char *s, int c);

char *strstr(const char *cs, const char *ct);

char *uitoa(uint32 val, char *buf, int radix);

char *itoa(int val, char *buf, int base);


void *malloc(unsigned int size);
void free(void *ptr);
void abort();
int atexit(void (*)(void));
char *getenv(const char *);
void exit(int exit_code);
#endif
