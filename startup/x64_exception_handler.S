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


/*
	   Handler's Stack
	    +-----------+
	    |           |
	    +-----------+
	    |    SS     |    + 40
	    +-----------+
	    |    RSP    |    + 32
	    +-----------+
	    |   RFLAGS  |    + 24
	    +-----------+
	    |    CS     |    + 16
	    +-----------+
	    |    RIP    |    + 8
	    +-----------+
	    |  ERR CODE |    + 0 ==>>  Stack pointer after transfer
	    +-----------+        to handler
	    |           |
	    +-----------+

*/

#include "x64_asm_common.h"

#define X64_EXCEPT_HANDLE_WEC_WRAP(name, inum) \
		 .global name; \
		 .type name, %function; \
		 .code64; \
		  name##_exception_handler: \
		  push    %rdi; \
		  mov     $inum,    %rdi; \




#define X64_EXCEPT_HANDLE_NEC_WRAP(name, inum)








