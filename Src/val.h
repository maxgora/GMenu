#ifndef __VALs_H
#define __VALs_H

#include <stdint.h>

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define	FW_REV	0x0100


// флаги, первые 16bit доступны по команде диагностики
typedef __packed struct
{
  char power_low:1;     // bit0 - пониженное напряжение питания
  char power_critical:1;// bit1 - критически низкое напряжение питания
  char evnt_btn:1;      // bit2 - зафиксированно нажатие кнопки 
  char link:1;          // bit3 - состояние сигнала link от bt модуля
  char mbdev_exp:1;     // bit4 - состояние экспозиции утройства на ModBus
  char bit5:1;          // bit5 - 
  char bit6:1;          // bit6 - 
  char bit7:1;          // bit7
  char bit8:1;          // bit8
  char bit9:1;          // bit9 - 
  char bit10:1;         // bit10 - 
  char bit11:1;         // bit11 - 
  char bit12:1;         // bit12 - 
  char bit13:1;         // bit13 - 
  char bit14:1;         // bit14 - 
  char bit15:1;         // bit15 - 

  char first_run:1;     // bit16 - 
  char bit17:1;         // bit17 - 
  char bit18:1;         // bit18 -   
  char bit19:1;         // bit19 - 
  char bit20:1;         // bit20 - 
  char bit21:1;         // bit21 - 
  char bit22:1;         // bit22 - 
  char bit23:1;         // bit23 - 
  char bit24:1;         // bit24 - 
  char bit25:1;         // bit25 - 
  char bit26:1;         // bit26 - 
  char bit27:1;         // bit27 - 
  char bit28:1;         // bit28 - 
  char bit29:1;         // bit29 - 
  char bit30:1;         // bit30 - 
  char bit31:1;         // bit31 - 
} Bits32;

typedef	union {
  Bits32 bit;
  uint32_t word;
} CMN_FLG;

void ValueInit(void);

#endif
