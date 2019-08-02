#ifndef __MYMAIN_H
#define __MYMAIN_H

#include <stdint.h>
#include "cmsis_os.h"
#include "main.h"
#include "gfx.h"

#include "tm_stm32_button.h"
#include "pins.h"

#define ECG_LEADs_CNT   10

typedef enum {
  BTN_UNDEF,
  BTN_DOWN,
  BTN_PUSH_DOWN,
  BTN_SET,
  BTN_SET_LONG,
  BTN_UP,
  BTN_PUSH_UP,
  BTN_USER
} ButtonPushType;

typedef enum {
  SIG_OFF = -1,
  SIG_UNDEF,
  SIG_ON
} SignalStateType;

typedef enum {
  ECG_ERR,
  ECG_NORMAL
} Ecg_Quality_Type;

typedef struct EcgArrType_t
{
  uint8_t cnt;
  int8_t arr[255];
} EcgArrType;

typedef struct EcgLeadResultType_t
{
  char lead_name[4];    // for 4byte asjust
  Ecg_Quality_Type ecg_quality;
  EcgArrType ecg;
} EcgLeadResultType;

typedef enum {
  MODE_HOLTER,
  MODE_EVENT
} EcgModeType;

/*typedef enum {
  "’олтер",
  "—обытийный"
} EcgModeDescrType;*/

/*typedef struct FullNameType_t
{
  char* p_first_name;
  char* p_second_name;
  char* p_middle_name;
} FullNameType;*/

typedef enum {
  F_SMPL_250Hz = 250,
  F_SMPL_500Hz = 500,
  F_SMPL_1000Hz = 1000,
  F_SMPL_2000Hz = 2000
} Fsmpl_Type;

typedef void(*pxFunctionHandler) (uint16_t val);

typedef struct GlobValType_t
{
  //SemaphoreHandle_t lock;
  SignalStateType bluetooth_state;
  uint8_t accum_percent;

  //FullNameType full_name;
  char* full_name;
  
  pxFunctionHandler CbEcgUpdateHandler; // Cb Func after ecg_lead[] update
  
  SignalStateType measure_state;  
  uint32_t    measure_time_set;   // 
  uint32_t    measure_time_cur;
  EcgModeType ecg_mode;
  SignalStateType pacemaker_state;
  Fsmpl_Type smpl_rate;
  EcgLeadResultType ecg_lead[ECG_LEADs_CNT];
} GlobValType;

void MyInit(void);
void MyMainThread(void);

#endif
