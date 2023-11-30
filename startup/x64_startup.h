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

#ifndef STARTUP_X64_STARTUP_H_
#define STARTUP_X64_STARTUP_H_


/* Set Global descriptors table number */
#define X86_GDT_NUM  256

/* Set Global descriptors table number */
#define X86_IDT_NUM  256


/* Can not be modified! */
#define X64_FLAT_SEGMENT_BASE           0x00000
#define X64_FLAT_SEGMENT_LIMIT          0xFFFFF

/* Can not be modified! */
#define X64_INTERRUPT_IST_INDEX         0x01
#define X64_EXCEPTION_IST_INDEX         0x02

/* Can not be modified! */
#define X64_GDT_CODE32_INDEX            0x01
#define X64_GDT_DATA_INDEX              0x02
#define X64_GDT_CODE64_INDEX            0x03
#define X64_GDT_INT_TRAP_GATE_INDEX     0x03
#define X64_GDT_TSS_INDEX               0x04


#define print_func_name() \
    x86_serial_send_str(0x3f8, __FILE__); \
    x86_serial_send_str(0x3f8, "    "); \
    x86_serial_send_str(0x3f8, __FUNCTION__); \
    x86_serial_send_str(0x3f8, "\r\n"); \

#endif /* STARTUP_X64_STARTUP_H_ */



