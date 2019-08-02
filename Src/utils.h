#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

// преобразование целого в строку
char* utoa_fast_div(uint32_t val, char* buff);

// Перевод числа, представляющего кол-во минут, в строку
// формата "hhh:mm"
char* mintoa_hm(uint32_t val, char* buff);

#endif
