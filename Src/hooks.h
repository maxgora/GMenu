#ifndef __HOOKS_H
#define __HOOKS_H

#define NEED_FATAL_ERR_ASSERT   1   // Разрешает "экран смерти" с выводом ошибки

#include <stdint.h>
#include "cmsis_os.h"

void vApplicationStackOverflowHook( xTaskHandle *pxTsk, signed portCHAR *pcTskNm );
void vApplicationMallocFailedHook( void );
void vAssertCalled( unsigned long ulLine, const char * const pcFileName );
void vMyFatalErrAssert( char *file, uint32_t line );

#ifdef  NEED_FATAL_ERR_ASSERT
  #define MY_CHECK(expr) ((expr) ? (void)0U : vMyFatalErrAssert((char *)__FILE__, __LINE__))
#else
  #define MY_FATAL_ERR_ASSERT(expr) ((void)0U)
#endif

#endif
