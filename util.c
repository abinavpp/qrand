#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "util.h"

void err_exit(int status, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	_exit(status);
}	

/* all allocation functions are fatal on failure */

void *umalloc(size_t size)
{
	void *alloc;

	alloc = malloc(size);

	if (!alloc) {
		err_exit(EXIT_FAILURE, "malloc failed!");
	}
	return alloc;
}

/* returns zeroed allocation of size bytes */
void *zalloc(size_t size)
{
	void *alloc;

	alloc = calloc(size, 1);

	if (!alloc) {
		err_exit(EXIT_FAILURE, "malloc failed!");
	}
	return alloc;
}

void *stralloc(const char *str)
{
	void *alloc;
	size_t len = strlen(str);

	alloc = umalloc(len + 1); /* +1 for '\0' */

	return alloc;
}

/* null flag decides whether to append '\0' or not */
void ustrcpy(char *dst, const char *src, int len, char null_flag)
{
	strncpy(dst, src, len);
	if (null_flag) {
		dst[len] = '\0';
	}
}

/* returns hash value of a string of n bytes modulo mod */
long hash_simfn(const void *bytes, size_t n, size_t mod)
{
	int base = UCHAR_MAX + 1;
	unsigned long hash = 0;
	char *chars = (char *)bytes;

	for (size_t i=0; i<n; i++) {
		hash = (base*hash + chars[i]) % mod;
	}

	return hash;
}
