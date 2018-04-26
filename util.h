#ifndef	_UTILH
#define _UTILH

#include <unistd.h>
#include <stdio.h>

extern void *umalloc(size_t size);
extern void *zalloc(size_t size);
extern void *stralloc(const char *str);

extern void ustrcpy(char *dst, const char *src, int len, char null_flag);

extern void err_exit(int status, const char *fmt, ...);
extern long hash_simfn(const void *bytes, size_t n, size_t mod);

#endif
