/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_BYPASS_ERROR_H
#define CRYS_BYPASS_ERROR_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "crys_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object % crys_aes_error.h    : %
   *  State           :  %state%
   *  Creation date   :  Sun Nov 21 11:07:08 2004
   *  Last modified   :  %modify_time%
   */
/************************ Defines ******************************/
/* BYPASS module on the CRYS layer base address - 0x00F01800 */
#define CRYS_BYPASS_INVALID_INPUT_POINTER_ERROR          	(CRYS_BYPASS_MODULE_ERROR_BASE + 0x00UL)
#define CRYS_BYPASS_INVALID_OUTPUT_POINTER_ERROR         	(CRYS_BYPASS_MODULE_ERROR_BASE + 0x01UL)
#define CRYS_BYPASS_NOT_ALLIGNED_POINTER_ERROR           	(CRYS_BYPASS_MODULE_ERROR_BASE + 0x02UL)
#define CRYS_BYPASS_ILLEGAL_SIZE_ERROR                   	(CRYS_BYPASS_MODULE_ERROR_BASE + 0x03UL)
#define CRYS_BYPASS_ILLEGAL_MEMORY_AREA_ERROR            	(CRYS_BYPASS_MODULE_ERROR_BASE + 0x04UL)
#define CRYS_BYPASS_ILLEGAL_PARAMS_ERROR      			(CRYS_BYPASS_MODULE_ERROR_BASE + 0x05UL)
#define CRYS_BYPASS_IS_NOT_SUPPORTED      			(CRYS_BYPASS_MODULE_ERROR_BASE + 0x06UL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif


