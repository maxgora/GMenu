#ifndef __UTILS_TST_H
#define __UTILS_TST_H

#include <stdint.h>

void LCDReadID(uint32_t *pid);
void LCDReadStatus(uint32_t *pstatus);
void LCDReadPower(uint8_t *ppwr);
void LCDReadRAM(uint16_t *buff, uint16_t cnt);


#endif
