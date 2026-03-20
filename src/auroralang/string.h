#ifndef LIB_STRING_H
#define LIB_STRING_H

#include <stddef.h>
#include <stdarg.h>

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
size_t strlen(const char *s);
char *strchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
char *itoa(int value, char *str, int base);
int atoi(const char *str);
char *strtok(char *str, const char *delim);

#define kstrlen strlen
#define kstrcpy strcpy
#define kstrncpy strncpy
#define kstrcmp strcmp
#define kstrncmp strncmp
#define kstrchr strchr
#define kmemset memset
#define kmemcpy memcpy
#define kstrcat strcat
#define kitoa itoa
#define katoi atoi
#define kstrtok strtok

int ksnprintf(char *buf, size_t n, const char *fmt, ...);

#endif // LIB_STRING_H