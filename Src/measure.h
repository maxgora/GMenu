#ifndef __MEASURE_H
#define __MEASURE_H

#include <stdint.h>
#include "cmsis_os.h"

typedef void(*pxMeasureCbUpdateHandler) (uint32_t time_sec);

void MeasureStart(void* pval);
void MeasureStop(void* pval);
uint8_t MeasureIsRun(void);

void MeasureRegisterUpdateCb(pxMeasureCbUpdateHandler pfunc);
void MeasureRegisterStopCb(pxMeasureCbUpdateHandler pfunc);

//void MeasureDeRegisterCb(pxMeasureCbUpdateHandler);

#endif
