/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* ALL rights reserved.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*
* The License of this software follows LGPL v2.1, See the LICENSE for more details:
* https://opensource.org/licenses/LGPL-2.1
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef STARTUP_X64_ISR_HANDLER_H_
#define STARTUP_X64_ISR_HANDLER_H_

#include "../startup/x64_common.h"


typedef struct {
  void *param;
  void (*pfn_handler)(void*param);
} x64_irq_hanlder_t;


int8_t x64_irq_handler_register(uint8_t vector, void(*p_fun)(void), void* param);

int8_t x64_irq_handler_destroy(uint8_t vector);


#endif /* STARTUP_X64_ISR_HANDLER_H_ */


