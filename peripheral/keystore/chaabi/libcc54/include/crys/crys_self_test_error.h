/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_SELF_TEST_ERROR_H
#define CRYS_SELF_TEST_ERROR_H

/*
   *  Object % crys_self_test_error.h    : %
   *  State           :  %state%
   *  Creation date   :  07/03/ 2012 Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module containes the definitions of the crys "self test"
   *         errors.
   *
   *  \version crys_self_test_error.h#1:incl:1
   *  \author G.Koltun
  */
/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "crys_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/****************************************************************************************
* CRYS SELF TEST MODULE ERRORS 							*
***************************************************************************************/
#define CRYS_SELF_TEST_MSG_GENERAL_RPC_A_ERROR    (CRYS_SELF_TEST_MODULE_ERROR_BASE + 0UL)
#define CRYS_SELF_TEST_MSG_GENERAL_RPC_B_ERROR    (CRYS_SELF_TEST_MODULE_ERROR_BASE + 1UL)
#define CRYS_SELF_TEST_MSG_GENERAL_RPC_C_ERROR    (CRYS_SELF_TEST_MODULE_ERROR_BASE + 2UL)



















/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif


