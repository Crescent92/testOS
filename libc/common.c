#include "common.h"

void *memcpy(void *dst, const void *src, uint32 n) {

	if(n <= 0) {
	       return 0;
	}	       

	unsigned char * destination_ptr = (unsigned char *)dst;
	const unsigned char * source_ptr = (const unsigned char *)src;

	for(uint32 i = 0; i < n; i++) {
		destination_ptr[i] = source_ptr[i];
	}

	return destination_ptr;
}

int memcmp(const void *s1, const void *s2, uint32 n) {

	const unsigned char *c1;
	const unsigned char *c2;
	for(c1 = s1, c2 = s2; n > 0; n--, c1++, c2++) {
		if(*c1 != *c2) {
			return (((int)*c1) - ((int)*c2));
		}
	}

	return 0;
}

/*void * memset(void *s, int c, int n) {
	register int i;
	char *cp = (char *)s;

	for(i = 0; i < n;i++) {
		*cp = (unsigned char)c;
		cp++;
	}
	return s;
}*/

void *memset(void *s, int c, int n){
  char *m = (char *)s;

  for (int i = 0; i < n; i++)
    m[i] = (uint8)c;

  return s;
}


void * memmove(void *dstptr, const void *srcptr, uint32 size) {
	unsigned char *dst = (unsigned char *)dstptr;
	const unsigned char *src = (const unsigned char*) srcptr;
	if (dst < src) {
		for(uint32 i = 0; i < size; i++) {
			dst[i] = src[i];
		} 
	} else {
		for (uint32 i = size; i != 0; i--) {
		       dst[i-1] = src[i-1];
		}
	}
	return dstptr;
}	

int abs (int arg) {
	if (arg < 0) 
		arg = -arg;
	return (arg);
}
/*
int atoi(char *p) {
	int n = 0, f = 0;

	for(;; p++) {
		switch(*p) {
			case ' ':
			case '\t':
				continue;
			case '-':
				f++;
			case '+':
				p++;
		}
		break;
	}

	while(((*p) >= '0') && ((*p) <= '9')) {
		n = n * 10 + *p - '0';
		p++;
	}

	return (f ? -n : n);
}



long atol(char *p) {
        long n = 0;
        int  f = 0;

        for(;; p++) {
                switch(*p) {
                        case ' ':
                        case '\t':
                                continue;
                        case '-':
                                f++;
                        case '+':
                                p++;
                }
                break;
        }

        while(((*p) >= '0') && ((*p) <= '9')) {
                n = n * 10 + *p - '0';
                p++;
        }

        return (f ? -n : n);
}
*/

       		       
void bzero(void *p, int len) {
	int n;
	char *pch = (char *)p;

	if((n = len) <= 0) {
		return;
	}

	do {
		*pch = 0;
		pch++;
	} while(--n);
}


char *strchar(const char *s, int c) {
	for(; *s != '\0'; s++) {
		if(*s == ((const char)c)) {
			return ((char *)s);
		}
	}

	if ((const char)c == *s) {
		return (char*)s;
	}

	return 0;
}


int strcmp(char *str1, char *str2) {
	while(*str1 == *str2) {
		if(*str1 == '\0') {
			return 0;
		}
		str1++;
		str2++;
	}

	if(*str1 < *str2) {
		return -1;
	} else {
		return 1;
	}
}

int strcpy(char *tar, const char *src) {
	while((*tar++ = *src++) != '\0') {
		;
	}
	return 0;
}

uint32 strlen(const char* str) {
	uint32 len = 0;
	while (str[len])
		len++;
	return len;
}

char *strncat(char *s1, const char *s2, int n) {
	char *os1;

	os1 = s1;

	char * ptr = s1 + strlen(s1);

	while (*s2 != '\0' && n--) {
		*ptr++ = *s2++;  
	}

	*ptr = '\0';

	return os1;
}

int strncmp(char *s1, char *s2, int n) {
	while (--n >= 0 && *s1 == *s2++) {
		if (*s1++ == '\0') {
			return 0;
		}
	}
	return (n < 0 ? 0: *s1 - *--s2);
}

char *strncpy(char *s1, const char *s2, int n) {
	register int i;
	register char *os1;

	os1 = s1;
	for (i=0; i < n; i++) {
		if (((*s1++) = (*s2++)) == '\0') {
			while (++i < n) {
				*s1++ = '\0';
			}
			return os1;
		}
	}
	return os1;
}

int strnlen (const char *s, uint32 len) {
	uint32 n;
	n = 0;
	while (*s++ && n < len)
		n++;

	return n;
}

char *strrchr(const char *s, int c) {
	char *r = 0;
	for(; *s != '\0'; s++){
		if(*s == (const char)c) {
			r = (char *)s;
		}
	}

	if((const char)c == *s) {
		return (char*)s;
	}

	return r;
}

char *strstr(const char *cs, const char *ct) {
	char *cq;
	char *cr;

	for(; *cs != '\0'; cs++) {
		if (*cs == *ct) {
			cq = (char *)cs;
			cr = (char *)ct;
			while ((*cq != '\0') && (*cr != '\0')) {
				if (*cq != *cr) {
					break;
				}
				cq++;
				cr++;
			}
			if ((*cr) == '\0') {
				return (char*)cs;
			}
		}
	}
	return 0;
}



// Converts an integer to a string.
char *itoa(int val, char *buf, int base){
  static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  char temp;
  int sign,i = 0;

  // Sign
  if((sign = val) < 0)
    val = -val;

  // Convert
  do{
    buf[i++] = num[val % base];
  }while(val /= base);

  if(sign < 0)
    buf[i++] = '-';

  // End of the string
  buf = '\0';

  char *end = buf - 1;
  char *begin = buf;

  // Reverse string
  while (end > begin) {
    temp = *end;
    *end-- = *begin;
    *begin++ = temp;
  }

  return buf;
}

 // Converts an unsigned integer to a string.
char *uitoa(uint32 val, char *buf, int radix) {
  uint32 i = 0;
  uint32 start = i;
  uint32 temp = val;

  if(radix == 16){
    buf[i++] = '0';
    buf[i++] = 'x';
    start = i;
  }

  do{
    uint32 a = temp % radix;
    if(a < 10)
      buf[i++] = a + '0';
    else
      buf[i++] = a + 'a' - 10;
  }while(temp /= radix);

  char *s = buf + start;
  char *e = buf + (i - 1);

  while(s < e){
    char t = *s;
    *s = *e;
    *e = t;
    s++;
    e--;
  }

  buf[i] = 0;
  return buf;
}

char* strcat(char* dest, const char* src) {
	while (*dest != '\0') {
		dest++;
	}

	strcpy(dest, src);
	return dest;
}

char* strchr(const char* str, int ch) {
	do {
		if (*str == ch) {
			return ((char *)str);
		}
	} while (*str != '\0');

	return 0;
}