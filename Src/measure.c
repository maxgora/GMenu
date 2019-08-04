/*
Шаблон ф-ций организации измерений
*/
#include <math.h>
#include "measure.h"
#include "my_main.h"
#include "hooks.h"

extern GlobValType GV;

// ф-ция вызываемая для уведомления интерфейса
static pxMeasureCbUpdateHandler cb_func_update = NULL;

static pxMeasureCbUpdateHandler cb_func_stop = NULL;

// очередь ф-ций                            
//static QueueHandle_t xQueueCbFunc = NULL;
// handle для задачи обновления экрана 
static TaskHandle_t xHandleMeasure = NULL;

/*
Задача имитации измерения
*/
static void taskMeasure(void * argument)
{ 
  srand(0xaa);
  int8_t rnd_val;
  uint8_t err_lead = 0;
  
  while(1)
  {
    osDelay(1000);
    
    GV.measure_time_cur_sec++;
    
    // имитация случайного обрыва отведения
    rnd_val = (int8_t)rand() ;
    if ((rnd_val & 0xF0) == 0xF0)
    {
      rnd_val &= 0x0F;
      if ( rnd_val > ECG_LEADs_CNT)
        rnd_val = ECG_LEADs_CNT;
      
      GV.ecg_lead[err_lead].ecg_quality = ECG_NORMAL;
      GV.ecg_lead[rnd_val].ecg_quality = ECG_ERR;
      
      // вызов ф-ции, зарегестрированной на смену состояния отведений
      if (GV.CbEcgUpdateHandler != NULL)
        GV.CbEcgUpdateHandler((1<<err_lead)|(1<<rnd_val));
      
      err_lead = rnd_val;
    }
    // вызов ф-ции обновления интерфейса
    if (cb_func_update != NULL)
      cb_func_update(GV.measure_time_cur_sec);
    
    if (GV.measure_time_cur_sec >= GV.measure_time_set*60)
      MeasureStop(NULL);
  }
}

void MeasureStart(void* pval)
{
  if (xHandleMeasure == NULL)
  {  
    GV.measure_time_cur_sec = 0;
    xTaskCreate( taskMeasure, "MeasureTask", 256, NULL, 1, &xHandleMeasure);
    MY_CHECK( xHandleMeasure ) ;
  }
}

void MeasureStop(void* pval)
{
  if (cb_func_stop != NULL)
    cb_func_stop(GV.measure_time_cur_sec);
  vTaskDelete( xHandleMeasure );
  xHandleMeasure = NULL;
}

uint8_t MeasureIsRun(void)
{
  if (xHandleMeasure) return 1;
  else return 0;
}

void MeasureRegisterUpdateCb(pxMeasureCbUpdateHandler pfunc)
{
  cb_func_update = pfunc;
}
void MeasureRegisterStopCb(pxMeasureCbUpdateHandler pfunc)
{
  cb_func_stop = pfunc;
}



/*BaseType_t MeasureRegisterCb(pxMeasureCbUpdateHandler cb_bunc)
{
  if (xQueueCbFunc == NULL)
  {
    xQueueCbFunc =  xQueueCreate( 4, sizeof( uint32_t ) );
    MY_CHECK(xQueueCbFunc);
  }
  BaseType_t size = uxQueueSpacesAvailable( xQueueCbFunc );
  if (size)
    xQueueSend( xQueueCbFunc,  cb_bunc, 0);
  return size;
}*/
