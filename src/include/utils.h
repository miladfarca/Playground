#include <stdint.h>

#define XSTRINGER(V) #V
#define STRINGER(V) XSTRINGER(V)

int32_t rand();
int utils_itoa(int32_t value, char *sp, int radix);
int tolower(int chr);
int isdigit(int c);
int strcmp(const char *s1, const char *s2);
uint64_t strlen(const char *str);
void *memcpy(void *dest, const void *src, uint64_t len);
void *memset(void *s, int c, uint64_t len);
int strncmp(const char *s1, const char *s2, uint64_t n);
char *strpbrk(const char *s1, const char *s2);
char *strchr(char *str, int ch);
char *strtok(char *string, const char *delim);
char *strdup(const char *s1);
int abs(int i);
