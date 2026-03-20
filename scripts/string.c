#include "string.h"

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *d = dest;
    size_t i = 0;
    while (i++ < n && (*d++ = *src++));
    while (i++ < n) *d++ = '\0';
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- && *s1 && (*s1 == *s2)) { s1++; s2++; }
    if (n == (size_t)-1) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
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

char *strstr(const char *haystack, const char *needle) {
    if (!*needle) return (char *)haystack;
    for (; *haystack; haystack++) {
        if (*haystack == *needle) {
            const char *h = haystack, *n = needle;
            while (*h && *n && *h == *n) { h++; n++; }
            if (!*n) return (char *)haystack;
        }
    }
    return NULL;
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

int atoi(const char *str) {
    int res = 0; int sign = 1;
    if (*str == '-') { sign = -1; str++; }
    while (*str >= '0' && *str <= '9') { res = res * 10 + (*str - '0'); str++; }
    return res * sign;
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
            // Minimal implementation for VFS paths
        } else buf[i++] = *p;
        p++;
    }
    buf[i] = '\0'; va_end(args); return i;
}