#include <math.h>
#include <stdlib.h>
#include "ecg.h"

extern GlobValType GV;

// Инициализация графиков для отладки вывода
void EcgInit(void)
{
  int i, j;
  int shift0, shift1;
  
  srand(0x55);
  
  // lead[0] - экспонента
  GV.ecg_lead[0].ecg.cnt = 233;
  GV.ecg_lead[0].ecg_quality = ECG_NORMAL;
  for (i = 0; i < GV.ecg_lead[0].ecg.cnt; i++) 
    GV.ecg_lead[0].ecg.arr[i] = (int8_t)((exp((double)(-i)/32)) * 110);
  
  // lead[1] - псевдослучайное число
  GV.ecg_lead[1].ecg.cnt = 233;
  GV.ecg_lead[1].ecg_quality = ECG_NORMAL;
  for (i = 0; i < GV.ecg_lead[1].ecg.cnt; i++)
    GV.ecg_lead[1].ecg.arr[i] = (int8_t)rand() / 8;
  
  // остальные - комбинация предыдущих со сдвигом
  for (i = 2; i < ECG_LEADs_CNT; i++)
  {
    shift0 = 233 - (i-2)*12;
    shift1 = 233 - (i-2)*5;
    
    GV.ecg_lead[i].ecg.cnt = 233;
    GV.ecg_lead[i].ecg_quality = ECG_NORMAL; 
    
    for (j = 0; j < GV.ecg_lead[i].ecg.cnt; j++)
    {
      if (shift0 >= GV.ecg_lead[i].ecg.cnt)
        shift0 = 0;
      if (shift1 >= GV.ecg_lead[i].ecg.cnt)
        shift1 = 0; 
        
      GV.ecg_lead[i].ecg.arr[j] = GV.ecg_lead[0].ecg.arr[shift0] + GV.ecg_lead[1].ecg.arr[shift1];
      
      shift0++;
      shift1++;
    }
  }
  // один из отводов помечу как обрыв
  GV.ecg_lead[5].ecg_quality = ECG_ERR;
}
