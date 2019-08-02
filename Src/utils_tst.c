// Ф-ции для тестирования работы экрана

#include "utils_tst.h"

/* Using FSMC A16 as RS */
#define GDISP_REG              (*((volatile uint8_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM              (*((volatile uint8_t *) 0x60040000)) /* RS = 1 */

// ини библиотеки
void LCDReadID(uint32_t *pid)
{
  uint8_t * pucval = (uint8_t *)pid;
  
  GDISP_REG = 0x04;
  
  *pucval = GDISP_RAM;    // dummy read  
  *pucval++ = GDISP_RAM;
  *pucval++ = GDISP_RAM;
  *pucval++ = GDISP_RAM;
}

void LCDReadStatus(uint32_t *pstatus)
{
  uint8_t * pucval = (uint8_t *)pstatus;
  
  GDISP_REG = 0x09;
  
  *pucval = GDISP_RAM;    // dummy read    
  *pucval++ = GDISP_RAM;
  *pucval++ = GDISP_RAM;
  *pucval++ = GDISP_RAM;
  *pucval++ = GDISP_RAM;
}

void LCDReadPower(uint8_t *ppwr)
{
  uint8_t * pucval = (uint8_t *)ppwr;
  
  GDISP_REG = 0x0a;
  
  *pucval = GDISP_RAM;    // dummy read    
  *pucval = GDISP_RAM;
}

void LCDReadRAM(uint16_t *buff, uint16_t cnt)
{
  uint8_t * pucval = (uint8_t *)buff;
  
  GDISP_REG = 0x2E;       // cmd
  *pucval = GDISP_RAM;    // dummy read    
  
  while (cnt--)
  {
    *pucval++ = GDISP_RAM;
    *pucval++ = GDISP_RAM;
  }
}
