#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

// �������������� ������ � ������
char* utoa_fast_div(uint32_t val, char* buff);

// ������� �����, ��������������� ���-�� �����, � ������
// ������� "hhh:mm"
char* mintoa_hm(uint32_t val, char* buff);

#endif
