/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_FIPS_ERROR_H
#define CRYS_FIPS_ERROR_H

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
   *  Object % CRYS_FIPS_error.h    : %
   *  State           :  %state%
   *  Creation date   :  25 Sept.2008
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module contains the definitions of the CRYS AESCCM errors.
   *
   *  \version CRYS_AESCCM_error.h#1:incl:1
   *  \author R.Levin
   */

/************************ Defines ******************************/

/* The CRYS FIPS module errors.
   CRYS_FIPS_MODULE_ERROR_BASE = 0x00F01700 */
#define CRYS_FIPS_INVALID_POINTER_ERROR     (CRYS_FIPS_MODULE_ERROR_BASE + 0x00UL)
#define CRYS_FIPS_INVALID_INPUT             (CRYS_FIPS_MODULE_ERROR_BASE + 0x01UL)

#define CRYS_FIPS_IS_NOT_SUPPORTED          (CRYS_FIPS_MODULE_ERROR_BASE + 0xFFUL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif



