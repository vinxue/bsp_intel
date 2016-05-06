/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_HMAC_ERROR_H
#define CRYS_HMAC_ERROR_H

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
   *  Object % crys_hash_error.h    : %
   *  State           :  %state%
   *  Creation date   :  Sun Nov 21 11:07:08 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module containes the definitions of the CRYS AES errors.
   *
   *  \version crys_aes_error.h#1:incl:1
   *  \author adams
   */




/************************ Defines ******************************/

/* The CRYS HASH module errors */
#define CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR     (CRYS_HMAC_MODULE_ERROR_BASE + 0x0UL)
#define CRYS_HMAC_ILLEGAL_OPERATION_MODE_ERROR           (CRYS_HMAC_MODULE_ERROR_BASE + 0x1UL)
#define CRYS_HMAC_USER_CONTEXT_CORRUPTED_ERROR           (CRYS_HMAC_MODULE_ERROR_BASE + 0x2UL)
#define CRYS_HMAC_DATA_IN_POINTER_INVALID_ERROR          (CRYS_HMAC_MODULE_ERROR_BASE + 0x3UL)
#define CRYS_HMAC_DATA_SIZE_ILLEGAL                      (CRYS_HMAC_MODULE_ERROR_BASE + 0x4UL)
#define CRYS_HMAC_INVALID_RESULT_BUFFER_POINTER_ERROR    (CRYS_HMAC_MODULE_ERROR_BASE + 0x5UL)
#define CRYS_HMAC_INVALID_KEY_POINTER_ERROR              (CRYS_HMAC_MODULE_ERROR_BASE + 0x6UL)
#define CRYS_HMAC_UNVALID_KEY_SIZE_ERROR                 (CRYS_HMAC_MODULE_ERROR_BASE + 0x7UL)
#define CRYS_HMAC_LAST_BLOCK_ALREADY_PROCESSED_ERROR	 (CRYS_HMAC_MODULE_ERROR_BASE + 0xBUL)
#define CRYS_HMAC_ILLEGAL_PARAMS_ERROR 			 (CRYS_HMAC_MODULE_ERROR_BASE + 0xCUL)

#define CRYS_HMAC_IS_NOT_SUPPORTED                       (CRYS_HMAC_MODULE_ERROR_BASE + 0xFUL)



/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif


