#include "utils.h"

typedef struct divmod10_t
{
  uint32_t quot;
  uint8_t rem;
} divmod10_t;

inline static divmod10_t divmodu10(uint32_t n)
{
  divmod10_t res;
  
  // * 0.8
  res.quot = n >> 1;
  res.quot += res.quot >> 1;
  res.quot += res.quot >> 4;
  res.quot += res.quot >> 8;
  res.quot += res.quot >> 16;
  uint32_t qq = res.quot;
  
  // /8
  res.quot >>= 3;
  
  // остаток
  res.rem = (uint8_t)(n - ((res.quot << 1) + (qq & ~7ul)));
  
  // коррекция
  if (res.rem > 9)
  {
    res.rem -= 10;
    res.quot++;
  }
  
  return res;
}

// Перевод числа в строку
char* utoa_fast_div(uint32_t val, char* buff)
{
  divmod10_t res;
  
  buff += 10; // str must be end ...
  *buff = 0;  // ... null
  
  do
  {
    res = divmodu10(val);
    *--buff = res.rem + '0';
    val = res.quot;
    
  } while ( val );
  
  return buff;
}
