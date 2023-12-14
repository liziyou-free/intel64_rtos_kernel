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

#ifndef X64_PCIE_H_
#define X64_PCIE_H_

struct  config_header_type0{

};


struct  config_header_type1{

};


struct {

}pci_compatible_config_space_t;


/**
 * \brief Configuration Address Port At 0x0CF8
 *
 *  31  30        24 23    16 15     11 10       8 7       2 1  0
 *  --------------------------------------------------------------
 *  | En | Reserved |Bus Num | Dev Num | Func Num | Reg Idx | 00 |
 *  --------------------------------------------------------------
 */
#define CONFIG_ADDRESS_PORT        0x0CF8

#define CONFIG_DATA_PORT           0x0CFC

#endif /* X64_PCIE_H_ */












