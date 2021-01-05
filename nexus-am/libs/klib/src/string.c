#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t n = 0;
  while (s[n++]);
  return (n - 1);
}

char *strcpy(char* dst,const char* src) {
  size_t i = 0;
  for(i = 0; src[i] != '\0'; i++) dst[i]=src[i];
  dst[i]='\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i]!= '\0'; i++) dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  char *str = dst;
  assert(dst!=NULL && src!=NULL);
  while(*dst!='\0') dst++;
  while((*dst++ = *src++) != '\0');
  return str;
}

int strcmp(const char* s1, const char* s2) {
  uint32_t i = 0;
  for (i = 0; s1[i] == s2[i]; i++) if(s1[i] == '\0') return 0;

  return ((signed char *)s1)[i] - ((signed char *)s2)[i];

}

int strncmp(const char* s1, const char* s2, size_t n) {
  uint32_t i = 0;
  for (i = 0; s1[i] == s2[i] && i < n; i++) if(s1[i] == '\0') return 0;

  return ((signed char *)s1)[i] - ((signed char *)s2)[i];
}

void* memset(void* v,int c,size_t n) {
  const uint8_t bytes = c;
  uint8_t *s = v;
  for (s = v; 0 < n; n--, s++)
	  *s = bytes;
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  const uint8_t *src = in;
  uint8_t *dst = out;
  while (n--) *dst++ = *src++;

  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if(!n) return 0;
  while(--n && *(uint8_t *)s1 == *(uint8_t *)s2) {
	s1 = (uint8_t *)s1 + 1;
	s2 = (uint8_t *)s2 + 1;
  }
  return (*((uint8_t *)s1) - *((uint8_t *)s2));
}

#endif
