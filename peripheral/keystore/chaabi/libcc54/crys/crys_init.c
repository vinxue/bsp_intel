/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API


  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:25:55 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version CRYS_AES.c#1:csrc:6
   *  \author adams
   */



/************* Include Files ****************/

/* .............. CRYS level includes ................. */

#include "DX_VOS_Mem.h"
#include "DX_VOS_Memmap.h"
#include "DX_VOS_Sem.h"
#include "error.h"
#include "CRYS_init.h"
//#include "SystemDefs.h"
#include "SEPDriver.h"
/************************ Defines ******************************/



/************************ MACROS ******************************/

/* this macro is required to remove compilers warnings if the AES is not supported */

/************************ Global Data ******************************/



/************* Private function prototype ****************/


/************************ Public Functions ******************************/


/**
 * @brief This function initializes the CRYS SW.
 *
 *  The function calls the init functions of all of the modules in the CRYS.
 *
 * @param[in] - HwBaseAddress the hardware base address.
 * @param[in] - param_PTR - pointer to parameter for the CRYS_Init (the seed used to create the SK - relevant only on CRYS_SW).
 * @param[in] - param - parameter by value (the size of the seed in bytes when seed is in param_PTR)
 *
 * \note The last two arguments are only used in software implementations of CRYS.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

CIMPORT_C CRYSError_t  CRYS_Init(DxUint32_t CryptoCellBaseAddress , DxUint8_t *param_PTR , DxUint16_t param )
{
  /*-------------------
      CODE
  --------------------*/
  CryptoCellBaseAddress=CryptoCellBaseAddress;
  param_PTR=param_PTR;
  param=param;

  return CRYS_OK;
}


/**
 * @brief This function terminates the CRYS SW.
 *
 *  The function calls the terminate functions of all of the modules in the CRYS.
 *
 *
 * @return CRYSError_t - On success CRYS_OK is returned
 */

CIMPORT_C CRYSError_t  CRYS_Terminate()
{

  return CRYS_OK;
}
