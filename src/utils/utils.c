#include "../include/hal.h"
#include "../include/memory.h"
#include "../include/system.h"
#include <stddef.h>
#include <stdint.h>

// https://stackoverflow.com/a/7603688
int32_t rand() {
  static int32_t lfsr = 0;
  static int bit = 0;
  if (!lfsr) {
    // get the current CMOS time to be used as seed.
    struct system_time time = system_get_time();
    lfsr = (time.hours << 16) | (time.minutes << 8) | (time.seconds);
  }
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
  return lfsr = (lfsr >> 1) | (bit << 15);
}

// https://stackoverflow.com/a/12386915
int utils_itoa(int32_t value, char *sp, int radix) {
  char tmp[16]; // be careful with the length of the buffer
  char *tp = tmp;
  int i;
  unsigned v;

  int sign = (radix == 10 && value < 0);
  if (sign)
    v = -value;
  else
    v = (unsigned)value;

  while (v || tp == tmp) {
    i = v % radix;
    v /= radix;
    if (i < 10)
      *tp++ = i + '0';
    else
      *tp++ = i + 'a' - 10;
  }

  int len = tp - tmp;

  if (sign) {
    *sp++ = '-';
    len++;
  }

  while (tp > tmp)
    *sp++ = *--tp;

  return len;
}

// https://stackoverflow.com/a/19301684
int tolower(int chr) { return (chr >= 'A' && chr <= 'Z') ? (chr + 32) : (chr); }

// https://stackoverflow.com/a/6737769
int isdigit(int c) {
  if ((c >= '0') && (c <= '9'))
    return 1;
  return 0;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

// https://stackoverflow.com/a/1733294
uint64_t strlen(const char *str) {
  const char *s;

  for (s = str; *s; ++s)
    ;
  return (s - str);
}

// https://github.com/gcc-mirror/gcc/blob/master/libiberty/memcmp.c
int memcmp(const void *str1, const void *str2, uint64_t count) {
  const unsigned char *s1 = (const unsigned char *)str1;
  const unsigned char *s2 = (const unsigned char *)str2;

  while (count-- > 0) {
    if (*s1++ != *s2++)
      return s1[-1] < s2[-1] ? -1 : 1;
  }
  return 0;
}

// https://stackoverflow.com/a/2490637
char *strcat(char *dest, const char *src) {
  char *rdest = dest;

  while (*dest)
    dest++;
  while (*dest++ = *src++)
    ;
  return rdest;
}

// https://code.woboq.org/gcc/libgcc/memcpy.c.html
void *memcpy(void *dest, const void *src, uint64_t len) {
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

int strncmp(const char *s1, const char *s2, uint64_t n) {
  while (n && *s1 && (*s1 == *s2)) {
    ++s1;
    ++s2;
    --n;
  }
  if (n == 0) {
    return 0;
  } else {
    return (*(unsigned char *)s1 - *(unsigned char *)s2);
  }
}

// https://opensource.apple.com/source/Libc/Libc-997.1.1/string/FreeBSD/strpbrk.c.auto.html
char *strpbrk(const char *s1, const char *s2) {
  const char *scanp;
  int c, sc;

  while ((c = *s1++) != 0) {
    for (scanp = s2; (sc = *scanp++) != '\0';)
      if (sc == c)
        return ((char *)(s1 - 1));
  }
  return (NULL);
}

// https://code.woboq.org/userspace/glibc/stdlib/abs.c.html
int abs(int i) { return i < 0 ? -i : i; }

// https://stackoverflow.com/a/41863857
char *strchr(char *str, int ch) {
  for (;; str++) {
    if (*str == ch)
      return str;
    if (!*str)
      return NULL;
  }
  return NULL;
}

// https://stackoverflow.com/a/18851921
void *memset(void *s, int c, uint64_t len) {
  unsigned char *dst = s;
  while (len > 0) {
    *dst = (unsigned char)c;
    dst++;
    len--;
  }
  return s;
}

// https://paperbun.org/implement-strtok-function-in-c-cpp/
char *strtok(char *string, const char *delim) {

  /* Static storage class is used to keep
   track of the string's index in each function call */
  static char *index;

  // Initial case, where user passes the actual string in strtok
  if (string != NULL) {
    index = string;
  } else {
    // further cases, where NULL is passed
    string = index;
  }

  // Final case where the index will be '\0'
  if (*index == '\0') {
    return NULL;
  }

  while (*index != '\0') {
    // Iterate over each delimeter and check if any delimiter matches to the
    // character
    for (int i = 0; delim[i] != '\0'; i++) {
      if (*index == delim[i]) {

        // We are not intrested in the following case where there is
        // no character available to print before delimiter.
        // This case occurs when two delimiters are side by side.
        if (index == string) {
          index++;
          string++;
        } else {
          *index = '\0';
          break;
        }
      }
    }

    // return the token
    if (*index == '\0') {
      index++;
      return string;
    }

    index++;
  }
  return string;
}

// https://stackoverflow.com/a/37134815
char *strdup(const char *s1) {
  char *str;
  size_t size = strlen(s1) + 1;

  str = memory_alloc(size);
  if (str) {
    memcpy(str, s1, size);
  }
  return str;
}
