#ifndef _STDIO_H
#define _STDIO_H

#include "sys/types.h"
#include "stdarg.h"
struct __FILE
{
	int fd;
	int flags;
	int pos;
	char *read_ptr, *read_base, *read_end;
	char *write_ptr, *write_base, *write_end;
	int blksize;
};
typedef struct __FILE FILE;

#define SEEK_SET 0

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int fflush(FILE *stream);
int fprintf(FILE *stream, const char *format, ...);

int fclose(FILE *stream);
FILE *fopen(const char *filename, const char *mode);
size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream);
int fseek(FILE *stream, long int offset, int whence);
long int ftell(FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream);
void setbuf(FILE *stream, char *buf);
int vfprintf(FILE*, const char*, va_list);

#endif