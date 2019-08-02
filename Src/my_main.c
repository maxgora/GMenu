#include "my_main.h"
#include "utils_tst.h"
#include "utils.h"
#include "screen_mng.h"
//#include "ecg.h"

// элемент для уведомления о нажатии кнопки
QueueHandle_t xQueueButtons;

// глобальные переменные
GlobValType GV = {
  SIG_ON,       // bluetooth on
  50,           // accum 50%
  //{"Иванов", "Иван", "Иванович"},
  "Иванов Иван Иванович",
  NULL,
  SIG_OFF,      // measure off
  630,         // заданное время измерения в мин
  0,            // текущее время измерения в мин
  MODE_EVENT,   //MODE_HOLTER,
  SIG_ON,       // pacemaker on
  F_SMPL_1000Hz,
  { {"C1",ECG_ERR,{0}},
    {"C2",ECG_ERR,{0}},
    {"C3",ECG_ERR,{0}},
    {"C4",ECG_ERR,{0}},
    {"C5",ECG_ERR,{0}},
    {"C6",ECG_ERR,{0}},
    {"LA",ECG_ERR,{0}},
    {"LL",ECG_ERR,{0}},
    {"RA",ECG_ERR,{0}},
    {"RL",ECG_ERR,{0}} }
};

// управление экраном
void taskDisplayPool(void * argument)
{
  uint8_t button_event;
  
  while(1)
  {
    xQueueReceive( xQueueButtons, &button_event, portMAX_DELAY);
    
    ScreenPool(button_event);
    
    /*switch ( button_event )
    {
      case PUSH_UP:
        break;      
      case PUSH_SET:
        break;
      case PUSH_DOWN:
        break;
      case PUSH_USER:
        break;
      
      default:
        break;
    }*/   
  }
}

// мигаю
void taskLedSwitch(void * argument)
{ 
  while(1)
  {
    LED_GREEN_SWITCH();
    osDelay(500);
  }
}

// опрос кнопок
void taskBtnsPool(void * argument)
{ 
  for(;;)
  {
    TM_BUTTON_Update();
    osDelay(5);
  }
}
// функции, вызываемые при нажатии кнопок
void CbPushButtonUp(TM_BUTTON_t* bs, TM_BUTTON_PressType_t pt)
{
  ButtonPushType btn; 
  if (pt == TM_BUTTON_PressType_Normal)
  {   
    btn = (uint8_t)BTN_UP;
    xQueueSend( xQueueButtons,  &btn, 0);
  }
  else if (pt == TM_BUTTON_PressType_OnPressed)
  {
    btn = BTN_PUSH_UP;
    xQueueSend( xQueueButtons,  &btn, 0);
  }
}
void CbPushButtonSet(TM_BUTTON_t* bs, TM_BUTTON_PressType_t pt)
{
  ButtonPushType btn; 
  if (pt == TM_BUTTON_PressType_Normal)
  {    
    btn = BTN_SET;
    xQueueSend( xQueueButtons,  &btn, 0);
  }
  else if (pt == TM_BUTTON_PressType_Long)
  {    
    btn = BTN_SET_LONG;
    xQueueSend( xQueueButtons,  &btn, 0);
  }
}
void CbPushButtonDown(TM_BUTTON_t* bs, TM_BUTTON_PressType_t pt)
{
  ButtonPushType btn; 
  if (pt == TM_BUTTON_PressType_Normal)
  { 
    btn = BTN_DOWN;
    xQueueSend( xQueueButtons,  &btn, 0);
  }
  else if (pt == TM_BUTTON_PressType_OnPressed)
  {
    btn = BTN_PUSH_DOWN;
    xQueueSend( xQueueButtons,  &btn, 0);
  }
}
void CbPushButtonUser(TM_BUTTON_t* bs, TM_BUTTON_PressType_t pt)
{
  ButtonPushType btn;   
  btn = BTN_SET;
  xQueueSend( xQueueButtons,  &btn, 0);
}


/*
Инициализация оболочки.
(должна вызываться из отдельного потока, т.е. после vTaskStartScheduler)
*/
void MyInit(void)
{
  TM_BUTTON_t *pbutton;
  
  gfxInit();
  
  ScreenInit();
  
  //EcgInit();

  // очередь нажатий кнопок 
  xQueueButtons = xQueueCreate( 1, sizeof( ButtonPushType ) );
  while(xQueueButtons == NULL) ;  
  
  // кнопки
  
  pbutton = TM_BUTTON_Init(nBTN_UP_GPIO_Port, nBTN_UP_Pin, 0, CbPushButtonUp);
  while(pbutton == NULL) ;  // for debug
  TM_BUTTON_SetPressTime(pbutton, 50, 1500);

  pbutton = TM_BUTTON_Init(nBTN_DOWN_GPIO_Port, nBTN_DOWN_Pin, 0, CbPushButtonDown);
  while(pbutton == NULL) ;
  TM_BUTTON_SetPressTime(pbutton, 50, 1500);

  pbutton = TM_BUTTON_Init(nBTN_SET_GPIO_Port, nBTN_SET_Pin, 0, CbPushButtonSet);
  while(pbutton == NULL) ;
  TM_BUTTON_SetPressTime(pbutton, 50, 1500);

  pbutton = TM_BUTTON_Init(nBTN_USER_GPIO_Port, nBTN_USER_Pin, 0, CbPushButtonUser);
  while(pbutton == NULL) ;
  TM_BUTTON_SetPressTime(pbutton, 50, 1500);
  
  // задачи
  
  xTaskCreate( taskBtnsPool, "BtnsPool", 256, NULL, 1, NULL);
  xTaskCreate( taskLedSwitch, "LedSwitch", 128, NULL, 1, NULL);
  xTaskCreate( taskDisplayPool, "DisplayPool", 2048, NULL, 1, NULL); 
}
