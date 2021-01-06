#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  uint8_t flag = 0;
  int i = 0, j = 0, k, res = 0;
  int tempInt;
  char tempStr[32];
  char *tstr;
  while (*(fmt + i) != '\0')
  {
    if (!flag && *(fmt + i) != '%') {
      _putc(*(fmt + i));
      j++;
    }
    else if (!flag && *(fmt + i) == '%') flag = 1;
    else if (flag) {
      switch(*(fmt + i)) {
        case 'd' :
          res++;      
          tempInt = va_arg(ap, int);
          i2a(tempInt, tempStr);
          k = 0;
          while(tempStr[k] != '\0'){
            _putc(tempStr[k]);
            j++;k++;
          }
          flag = 0;
          break;
        case 's' :
          res++;
          flag = 0;
          tstr = va_arg(ap, char *);
          k = 0;
          while(tstr[k] != '\0'){
            _putc(tstr[k]);
            j++;k++;
          }

          break;
        default:
          res = -1;
          break;
      }
    }
    i++;
  }


  va_end(ap);
  return res;
}

void i2a(int n, char *dst) {
  int start = 0, total = 0, temp = n;
  int i;

  if (n < 0) {
    start = 1;
    dst[0] = '-';
    n = 0 - n;
  }

  while(temp != 0) {
    temp /= 10;
    total++;
  }
  
  temp = n;
  *(dst + start + total) = '\0';
  for (i = total - 1; i >= 0; i--) {
    *(dst + start + i) = temp % 10 + '0';
    
    temp /= 10;
  }
  
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  uint8_t flag = 0;
  int i = 0, j = 0, k, res = 0;
  int tempInt;
  char tempStr[32];
  char *tstr;
  while (*(fmt + i) != '\0')
  {
    if (!flag && *(fmt + i) != '%') {
      *(out + j) = *(fmt + i);
      j++;
    }
    else if (!flag && *(fmt + i) == '%') flag = 1;
    else if (flag) {
      switch(*(fmt + i)) {
        case 'd' :
          res++;      
          tempInt = va_arg(ap, int);
          i2a(tempInt, tempStr);
          k = 0;
          while((*(out + j) = tempStr[k]) != '\0'){
            j++;k++;
          }
          flag = 0;
          break;
        case 's' :
          res++;
          flag = 0;
          tstr = va_arg(ap, char *);
          k = 0;
          while((*(out + j) = tstr[k]) != '\0'){
            j++;k++;
          }

          break;
        default:
          res = -1;
          break;
      }
    }
    i++;
  }
  *(out + j) = '\0';
  
  return res;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int res = vsprintf(out, fmt, va);
  va_end(va);
  return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
