#include "string.h"
#include <stdint.h>

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

char *strcat(char *dest, const char *src) {
    char *d = dest;
    while (*d) d++;
    while ((*d++ = *src++));
    return dest;
}

char *strchr(const char *s, int c) {
    while (*s != (char)c) {
        if (!*s++) return NULL;
    }
    return (char *)s;
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    if (n == (size_t)-1) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *strstr(const char *haystack, const char *needle) {
    size_t len = strlen(needle);
    if (len == 0) return (char*)haystack;
    while (*haystack) {
        if (strncmp(haystack, needle, len) == 0) {
            return (char*)haystack;
        }
        haystack++;
    }
    return NULL;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

char *itoa(int value, char *str, int base) {
    char *rc;
    char *ptr;
    char *low;
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    if (value < 0 && base == 10) {
        *ptr++ = '-';
    }
    low = ptr;
    int v = value;
    if (v < 0 && base == 10) v = -v;
    
    do {
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[v % base];
        v /= base;
    } while (v);
    
    *ptr-- = '\0';
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

int atoi(const char *str) {
    int res = 0; int sign = 1;
    if (*str == '-') { sign = -1; str++; }
    while (*str >= '0' && *str <= '9') { res = res * 10 + (*str - '0'); str++; }
    return res * sign;
}

char *strtok(char *str, const char *delim) {
    static char *p = NULL;
    if (str) p = str;
    else if (!p) return NULL;
    while (*p && strchr(delim, *p)) p++;
    if (!*p) return NULL;
    char *start = p;
    while (*p && !strchr(delim, *p)) p++;
    if (*p) { *p = '\0'; p++; } else p = NULL;
    return start;
}

int ksnprintf(char *buf, size_t n, const char *fmt, ...) {
    va_list args; va_start(args, fmt);
    size_t i = 0; const char *p = fmt;
    while (*p && i < n - 1) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                char *s = va_arg(args, char*);
                while (*s && i < n - 1) buf[i++] = *s++;
            }
        } else buf[i++] = *p;
        p++;
    }
    buf[i] = '\0'; va_end(args); return i;
}