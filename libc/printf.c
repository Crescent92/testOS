#include <printf.h>
#include <syscalls.h>


static void print_dec(unsigned int value, unsigned int wid, char *buf, int *ptr, int fill_z, int align_r, int prec)
{
    unsigned int n_w = 1;
    unsigned int i = 9;
    if (prec == -1) prec = 1;

    if (value == 0) {
        n_w = 0;
    } else if (value < 10UL) {
        n_w = 1;
    } else if (value < 100UL) {
        n_w = 2;
    } else if (value < 1000UL) {
        n_w = 3;
    } else if(value < 10000UL) {
        n_w = 4;
    } else if (value < 100000UL) {
        n_w = 5;
    } else if (value < 1000000UL) {
        n_w = 6;
    } else if (value < 10000000UL) {
        n_w = 7;
    } else if (value < 100000000UL) {
        n_w = 8;
    } else if (value < 1000000000UL) {
        n_w = 9;
    } else {
        n_w = 10;
    }

    if (n_w < (unsigned int)prec) n_w = prec;

    int printed = 0;
    if (align_r) {
        while (n_w + printed < wid) {
            buf[*ptr] = fill_z ? '0' : ' ';
            *ptr += 1;
            printed += 1;
        }
        i = n_w;
        while (i > 0) {
            unsigned int n = value / 10;
            int r = value % 10;
            buf[*ptr + i - 1] = r + '0';
            i--;
            value = n;
        }
        *ptr += n_w;
    } else {
        i = n_w;
        while (i > 0) {
            unsigned int n = value / 10;
            int r = value % 10;
            buf[*ptr + i - 1] = r + '0';
            i--;
            value = n;
            printed++;
        }
        *ptr += n_w;
        while (printed < (int)wid) {
            buf[*ptr] = fill_z ? '0' : ' ';
            *ptr += 1;
            printed += 1;
        }
    }
}

static void print_hex(unsigned int val, unsigned int wid, char *buf, int *ptr)
{
    int i = wid;
    if (i == 0) i = 8;

    unsigned int n_wid = 1;
    unsigned int j = 0x0f;
    while (val > j) {
        n_wid += 1;
        j *= 0x10;
        j += 0x0f;
    }

    while ( i > (int)n_wid) {
        buf[*ptr] = '0';
        *ptr += 1;
        i--;
    }

    i = (int)n_wid;
    while (i-- > 0) {
        buf[*ptr] = "0123456789abcdef"[(val >>(i*4))&0xf];
        *ptr += 1;
    }
}

static int _printf_h(char *buf, const char *fmt, va_list args) {
    int i = 0;
    char *s;
    char *buff = buf;
    uint32 arg_w = 0;
    int align = 1;
    int fill_zero = 0;
    int big = 0;
    int alt = 0;
    int always_sign = 0;
    int prec = -1;
    // uint32 count = 0;
    for (const char *f = fmt; *f; f++) {
        if (*f != '%') {
            *buff = *f;
            continue;
        }
        ++f;
        while (1) {
            if (*f == '-') {
                align = 0;
                ++f;
            } else if (*f == '#') {
                alt = 1;
                ++f;
            } else if (*f == '*') {
                arg_w = (char)va_arg(args, int);
                ++f;
            } else if (*f == '0') {
                fill_zero = 1;
                ++f;
            } else if (*f == '+') {
                always_sign = 1;
                ++f;
            } else {
                break;
            }
        }

        while (*f >= '0' && *f <= '9') {
            arg_w *= 10;
            arg_w += *f - '0';
            ++f;
        }

        if (*f == '.') {
            ++f;
            prec = 0;
            if (*f == '*') {
                prec = (int)va_arg(args, int);
                ++f;
            } else {
                while (*f >= '0' && *f <= '9') {
                    prec *= 10;
                    prec += *f - '0';
                    ++f;
                }
            }
        }

        if (*f == '1') {
            big = 1;
            ++f;
            if (*f == '1') {
                big = 2;
                ++f;
            }
        }

        if (*f == 'z') {
            big = 1;
            ++f;
        }

        switch (*f) {
            case 's':
                {
                    uint32 count = 0;
                    if(big) {
                        int *ws = (int *)va_arg(args, int *);
                        if (ws == NULL) {
                            ws = L"(null)";
                        }
                        uint32 count = 0;
                        while (*ws) {
                            *buff++ = *ws++;
                            count++;
                            if (arg_w && count == arg_w) break;
                        }

                    } else {
                        s = (char *)va_arg(args, char *);
                        if (s == NULL) {
                            s = "(null)";
                        }
                        if (prec >= 0) {
                            while (*s && prec > 0) {
                                *buff++ = *s++;
                                count++;
                                prec--;
                                if (arg_w && count == arg_w) break;
                            }
                        } else {
                            while (*s) {
                                *buff++ = *s++;
                                count++;
                                if (arg_w && count == arg_w) break;
                            }
                        }
                    }
                    while (count <arg_w) {
                        *buff++ = ' ';
                        count++;
                    }
                }
                break;
            case 'c':
                *buff++ = (char)va_arg(args, int);
                break;
            case 'p':
                if (!arg_w) {
                    arg_w = 8;
                }
            case 'x':
                if (alt) {
                    *buff++ = '0';
                    *buff++ = 'x';
                }
                i = buff - buf;
                if (big == 2) {
                    unsigned long long val = (unsigned long long)va_arg(args, unsigned long long);
                    if (val > 0xffffffff) {
                        print_hex(val >> 32, arg_w > 8 ? (arg_w - 8) : 0, buf, &i);
                    } 
                    print_hex((val & 0xffffffff), arg_w > 8 ? 8 : arg_w, buf, &i);
                } else {
                    print_hex((unsigned long)va_arg(args, unsigned long), arg_w, buf, &i);
                }
                buff = buf + i;
                break;
            case 'i':
            case 'd':
                i = buff - buf;
                {
                    long long val;
                    if (big == 2) {
                        val = (long long)va_arg(args, long long);
                    } else {
                        val = (long)va_arg(args, long);
                    }

                    if (val < 0) {
                        *buff++ = '-';
                        buf++;
                        val = -val;
                    } else if (always_sign) {
                        *buff++ = '+';
                        buff++;
                    }
                    print_dec(val, arg_w, buf, &i, fill_zero, align, prec);
                }
                buff = buf + i;
                break;
            case 'u':
                i = buff - buf;
                {
                    unsigned long long val;
                    if (big == 2) {
                        val = (unsigned long long)va_arg(args, unsigned long long);
                    } else {
                        val = (unsigned long long)va_arg(args, unsigned long);
                    }
                    print_dec(val, arg_w, buf, &i, fill_zero, align, prec);
                }
                buff = buf + i;
                break;
            case 'g':
            case 'f':
                {
                    double val = (double)va_arg(args, double);
                    i = buff - buf;
                    if (val < 0) {
                        *buff++ = '-';
                        buff++;
                        val = -val;
                    }
                    print_dec((long)val, arg_w, buf, &i, fill_zero, align, 1);
                    buff = buf + i;
                    i = buff - buf;
                    *buff++ = '.';
                    buf++;
                    for (int j = 0; j < ((prec > -1 && prec < 8) ? prec : 8); ++j) {
                        if ((int)(val * 100000.0) % 100000 == 0 && j != 0) break;
                        val *= 10.0;
                        print_dec((int)(val) % 10, 0, buf, &i, 0, 0, 1);
                    }
                    buff = buf + i;
                }
                break;
            case '%':
                *buff = '%';
                break;
            default:
                *buff++ = *f;
                break;
        }
    }

    *buff = '\0';
    return buff - buf;
}



int printf(const char* fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    _printf_h(buf, fmt, args);
    sys_print_str(buf, strlen(buf));
    va_end(args);
    return strlen(buf);
}


//---------------------------------------------------------------

/*
 * Space or zero padding and a field width are supported for the numeric
 * formats only.
 *
 * The special format %e takes an integer error code
 * and prints a string describing the error.
 * The integer may be positive or negative,
 * so that -E_NO_MEM and E_NO_MEM are equivalent.
 */

enum {
	// Kernel error codes -- keep in sync with list in lib/printfmt.c.
	E_UNSPECIFIED	= 1,	// Unspecified or unknown problem
	E_BAD_ENV	,	// Environment doesn't exist or otherwise
				// cannot be used in requested action
	E_INVAL		,	// Invalid parameter
	E_NO_MEM	,	// Request failed due to memory shortage
	E_NO_FREE_ENV	,	// Attempt to create a new environment beyond
				// the maximum allowed
	E_FAULT		,	// Memory fault

	E_IPC_NOT_RECV	,	// Attempt to send to env that is not recving
	E_EOF		,	// Unexpected end of file

	// File system error codes -- only seen in user-level
	E_NO_DISK	,	// No free space left on disk
	E_MAX_OPEN	,	// Too many files are open
	E_NOT_FOUND	, 	// File or block not found
	E_BAD_PATH	,	// Bad path
	E_FILE_EXISTS	,	// File already exists
	E_NOT_EXEC	,	// File not a valid executable
	E_NOT_SUPP	,	// Operation not supported

	E_NOSYS		,	// syscall not implemented

	E_NOT_READY ,

	MAXERROR
};
static const char * const error_string[MAXERROR] =
{
	[E_UNSPECIFIED]	= "unspecified error",
	[E_BAD_ENV]	= "bad environment",
	[E_INVAL]	= "invalid parameter",
	[E_NO_MEM]	= "out of memory",
	[E_NO_FREE_ENV]	= "out of environments",
	[E_FAULT]	= "segmentation fault",
	[E_IPC_NOT_RECV]= "env is not recving",
	[E_EOF]		= "unexpected end of file",
	[E_NO_DISK]	= "no free space on disk",
	[E_MAX_OPEN]	= "too many files are open",
	[E_NOT_FOUND]	= "file or block not found",
	[E_BAD_PATH]	= "invalid path",
	[E_FILE_EXISTS]	= "file already exists",
	[E_NOT_EXEC]	= "file is not a valid executable",
	[E_NOT_SUPP]	= "operation not supported",

	[E_NOSYS]		= "no such syscall",
};

/*
 * Print a number (base <= 16) in reverse order,
 * using specified putch function and associated pointer putdat.
 */
static void
printnum(void (*putch)(int, void*), void *putdat,
	 unsigned long long num, unsigned base, int width, int padc)
{
	// first recursively print all preceding (more significant) digits
	if (num >= base) {
		printnum(putch, putdat, num / base, base, width - 1, padc);
	} else {
		// print any needed pad characters before first digit
		while (--width > 0)
			putch(padc, putdat);
	}

	// then print this (the least significant) digit
	putch("0123456789abcdef"[num % base], putdat);
}

// Get an unsigned int of various possible sizes from a varargs list,
// depending on the lflag parameter.
static unsigned long long
getuint(va_list *ap, int lflag)
{
	if (lflag >= 2)
		return va_arg(*ap, unsigned long long);
	else if (lflag)
		return va_arg(*ap, unsigned long);
	else
		return va_arg(*ap, unsigned int);
}

// Same as getuint but signed - can't use getuint
// because of sign extension
static long long
getint(va_list *ap, int lflag)
{
	if (lflag >= 2)
		return va_arg(*ap, long long);
	else if (lflag)
		return va_arg(*ap, long);
	else
		return va_arg(*ap, int);
}


// Main function to format and print a string.
void printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...);

void
vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list ap)
{
	register const char *p;
	register int ch, err;
	unsigned long long num;
	int base, lflag, width, precision, altflag;
	char padc;

	while (1) {
		while ((ch = *(unsigned char *) fmt++) != '%') {
			if (ch == '\0')
				return;
			putch(ch, putdat);
		}

		// Process a %-escape sequence
		padc = ' ';
		width = -1;
		precision = -1;
		lflag = 0;
		altflag = 0;
	reswitch:
		switch (ch = *(unsigned char *) fmt++) {

		// flag to pad on the right
		case '-':
			padc = '-';
			goto reswitch;

		// flag to pad with 0's instead of spaces
		case '0':
			padc = '0';
			goto reswitch;

		// width field
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			for (precision = 0; ; ++fmt) {
				precision = precision * 10 + ch - '0';
				ch = *fmt;
				if (ch < '0' || ch > '9')
					break;
			}
			goto process_precision;

		case '*':
			precision = va_arg(ap, int);
			goto process_precision;

		case '.':
			if (width < 0)
				width = 0;
			goto reswitch;

		case '#':
			altflag = 1;
			goto reswitch;

		process_precision:
			if (width < 0)
				width = precision, precision = -1;
			goto reswitch;

		// long flag (doubled for long long)
		case 'l':
			lflag++;
			goto reswitch;

		// character
		case 'c':
			putch(va_arg(ap, int), putdat);
			break;

		// error message
		case 'e':
			err = va_arg(ap, int);
			if (err < 0)
				err = -err;
			if (err >= MAXERROR || (p = error_string[err]) == NULL)
				printfmt(putch, putdat, "error %d", err);
			else
				printfmt(putch, putdat, "%s", p);
			break;

		// string
		case 's':
			if ((p = va_arg(ap, char *)) == NULL)
				p = "(null)";
			if (width > 0 && padc != '-')
				for (width -= strnlen(p, precision); width > 0; width--)
					putch(padc, putdat);
			for (; (ch = *p++) != '\0' && (precision < 0 || --precision >= 0); width--)
				if (altflag && (ch < ' ' || ch > '~'))
					putch('?', putdat);
				else
					putch(ch, putdat);
			for (; width > 0; width--)
				putch(' ', putdat);
			break;

		// (signed) decimal
		case 'd':
			num = getint(&ap, lflag);
			if ((long long) num < 0) {
				putch('-', putdat);
				num = -(long long) num;
			}
			base = 10;
			goto number;

		// unsigned decimal
		case 'u':
			num = getuint(&ap, lflag);
			base = 10;
			goto number;

		// (unsigned) octal
		case 'o':
			num = getuint(&ap, lflag);
			base = 8;
			goto number;

		// pointer
		case 'p':
			putch('0', putdat);
			putch('x', putdat);
			num = (unsigned long long)
				(uint32) va_arg(ap, void *);
			base = 16;
			goto number;

		// (unsigned) hexadecimal
		case 'x':
			num = getuint(&ap, lflag);
			base = 16;
		number:
			printnum(putch, putdat, num, base, width, padc);
			break;

		// escaped '%' character
		case '%':
			putch(ch, putdat);
			break;

		// unrecognized escape sequence - just print it literally
		default:
			putch('%', putdat);
			for (fmt--; fmt[-1] != '%'; fmt--)
				/* do nothing */;
			break;
		}
	}
}

void
printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintfmt(putch, putdat, fmt, ap);
	va_end(ap);
}

struct sprintbuf {
	char *buf;
	char *ebuf;
	int cnt;
};

static void
sprintputch(int ch, struct sprintbuf *b)
{
	b->cnt++;
	if (b->buf < b->ebuf)
		*b->buf++ = ch;
}

int
vsnprintf(char *buf, int n, const char *fmt, va_list ap)
{
	struct sprintbuf b = {buf, buf+n-1, 0};

	if (buf == NULL || n < 1)
		return -E_INVAL;

	// print the string to the buffer
	vprintfmt((void*)sprintputch, &b, fmt, ap);

	// null terminate the buffer
	*b.buf = '\0';

	return b.cnt;
}

int
snprintf(char *buf, int n, const char *fmt, ...)
{
	va_list ap;
	int rc;

	va_start(ap, fmt);
	rc = vsnprintf(buf, n, fmt, ap);
	va_end(ap);

	return rc;
}


//---------------------------------------------------------------

struct printbuf {
	int idx;	// current buffer index
	int cnt;	// total bytes printed so far
	char buf[256];
};


static void
putch(int ch, struct printbuf *b)
{
	b->buf[b->idx++] = ch;
	if (b->idx == 256-1) {
		sys_cputs(b->buf, b->idx);
		b->idx = 0;
	}
	b->cnt++;
}


int
vcprintf(const char *fmt, va_list ap)
{
	struct printbuf b;

	b.idx = 0;
	b.cnt = 0;
	vprintfmt((void*)putch, &b, fmt, ap);
	sys_cputs(b.buf, b.idx);

	return b.cnt;
}

int
cprintf(const char *fmt, ...)
{
	va_list ap;
	int cnt;

	va_start(ap, fmt);
	cnt = vcprintf(fmt, ap);
	va_end(ap);

	return cnt;
}