/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/* This file contains the definitions of the OTP data that the SEP copies
into the SRAM during the first boot process */


#ifndef _SEP_SRAM_MAP_
#define _SEP_SRAM_MAP_

#define DX_FIRST_OEM_KEY_OFFSET_IN_SRAM         0x0
#define DX_SECOND_OEM_KEY_OFFSET_IN_SRAM        0x4
#define DX_THIRD_OEM_KEY_OFFSET_IN_SRAM         0x8
#define DX_LCS_OFFSET_IN_SRAM                   0xC
#define DX_MISC_OFFSET_IN_SRAM                  0xD
#define DX_CC_INIT_MSG_OFFSET_IN_SRAM		0x100
#define DX_PKA_MEMORY_OFFSET_IN_SRAM		0x200

#endif /*_GEN_SRAM_MAP_*/
