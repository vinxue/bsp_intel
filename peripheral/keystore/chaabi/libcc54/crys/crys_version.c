/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

  /*
   *  Object %CRYS_COMMON_version.c    : %
   *  State           :  %state%
   *  Creation date   :  Mon Nov 22 10:22:57 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This file containes all of the common variables and function needed to operate
   *         all of the CRYS functionality.
   *
   *  \version CRYS_COMMON_version.c.#1:csrc:1
   *  \author adams
   */



/************* Include Files ****************/

/* .................. CRYS level includes ............. */

#include "DX_VOS_Mem.h"
#include "CRYS_version.h"
#include "SEPDriver.h"
#include "crys_host_op_code.h"
#include "error.h"


/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************* Private function prototype ****************/


/************************ Public Functions ******************************/

/**
 * @brief This Api returnes the CRYS version.
 *
 * The version containes the following:
 *
 * component string - a string describing the nature of the release.
 * release type : 'D' - development , 'A' - alpha (passed to the Q.A) ,
 *                'B' - beta , 'R' - release , after Q.A testing.
 *
 * major , minor , sub , internal - the release digits.
 *
 * each component : CRYS , LLF machines receives this database.
 *
 * @param[in] version_ptr - a pointer to the version structure.
 *
 */

void  CRYS_GetVersion(CRYS_Version_t *version_ptr)
{
  /* offset into SRAM */
  DxUint32_t    sramOffset;

  /* read param */
  DxUint32_t    messageParam[2];

  /* max length */
  DxUint32_t    maxLength;

  /* error */
  DxError_t     Error;

  /*------------------------------
      CODE
  ----------------------------------*/

  /* ............... Check input parameter ................... */

  if(version_ptr == (CRYS_Version_t *)DX_NULL)
  {
    return;
  }

   /* lock access to the SEP */
   Error = SEPDriver_Lock();

   if(Error != DX_OK)
   {
       goto end_function;
   }

  /*----------------------------
    start sending message to SEP
  -----------------------------*/
  sramOffset = 0;

  /* start the message */
  SEPDriver_StartMessage(&sramOffset);

  /* prepare message */
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_GET_VERSION_OP_CODE;

  /* send opcode */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam,
                            sizeof(DxUint32_t),
                            sizeof(DxUint32_t),
                            &sramOffset,
                            DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* end message */
  SEPDriver_EndMessage(sramOffset);

  /* wait for the response */
  Error = SEPDriver_POLL_FOR_REPONSE();
  if(Error != DX_OK)
  {
    goto end_function_unlock;
  }

  /*-------------------
    start reading message from the SEP
  ---------------------*/

  /* start the message */
  Error = SEPDriver_StartIncomingMessage(&sramOffset);
  if(Error)
  {
    goto end_function_unlock;
  }

  /* read opcode + status  */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam,
                            sizeof(DxUint32_t) * 2,
                            sizeof(DxUint32_t) * 2,
                            &sramOffset ,
                            DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* check the opcode */
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_GET_VERSION_OP_CODE)
  {
    Error = DX_WRONG_OPCODE_FROM_SEP_ERR;
    goto end_function_unlock;
  }

  /* check status */
  if(messageParam[1] != DX_OK)
  {
    Error = messageParam[1];
    goto end_function_unlock;
  }

  /* read the version */
  maxLength = ((sizeof(CRYS_Version_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater((DxUint32_t)version_ptr,
                            sizeof(CRYS_Version_t),
                            maxLength,
                            &sramOffset,
                            DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* ...................... end of function ................................ */

end_function_unlock:

  /* lock access to the SEP */
  SEPDriver_Unlock();

end_function:

	return;
}/* END OF CRYS_GetVersion */

