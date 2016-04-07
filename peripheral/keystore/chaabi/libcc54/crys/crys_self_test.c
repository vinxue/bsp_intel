/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Wed Nov 17 17:10:59 2004
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version PLAT_SystemDep.c#1:csrc:2
   *  \author adams
   */



/************* Include Files ****************/

#include "CRYS_SELF_TEST.h"
#include "PLAT_SystemDep.h"
#include "CRYS_Defs.h"
#include "DX_VOS_Mem.h"
#include "CRYS_COMMON_error.h"
#include "CRYS_RND.h"
#include "error.h"
#include "crys_host_op_code.h"
#include "SEPDriver.h"



/* canceling the lint warning:
   Unusual pointer cast (incompatible indirect types) */
/*lint --e{785} */
/*lint --e{740} */

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/**
 * @brief This function executes the CRYS self test.
 *
 *
 *
 * @param[in] EnginesSelect - selecting the engines to activate according to the following values:
 *
 *                            CRYS_SELF_TEST_AES_BIT
 *                            CRYS_SELF_TEST_DES_BIT
 *                            CRYS_SELF_TEST_HASH_BIT
 *                            CRYS_SELF_TEST_HMAC_BIT
 *                            CRYS_SELF_TEST_RSA_ENCDEC_BIT
 *                            CRYS_SELF_TEST_RND_BIT
 *                            CRYS_SELF_TEST_ALL_BIT
 *
 * @param[in] EnginesTestResult_ptr - a bit field describing the test results of the engines.
 *                                    the bit field is the same as described above.
 *                                    on each bit : 0 - test passes , 1 - test failure.
 *
 * @param[in] TempData_ptr - a temp pointer of a temp buffer required for the self test.
 *
 * @return CRYSError_t - On success the function returns the value CRYS_OK,
 *			                 and on failure a non-ZERO error.

 */

CRYSError_t  CRYS_SelfTest(DxUint16_t           EnginesSelect,
                           DxUint16_t*          EnginesTestResult_ptr,
                           CRYS_SelfTestData_t* TempData_ptr )
{
  /* The return error identifier */
  CRYSError_t   Error;

  /* offset */
  DxUint32_t    sramOffset;

  /* word parameter */
  DxUint32_t    messageParam[2];

  /*---------------------------
      CODE
  -----------------------------*/

  TempData_ptr = TempData_ptr;

  /* initializing the Error to O.K */
  Error = CRYS_OK;


   /* lock access to the SEP */
   Error = SEPDriver_Lock();

   if(Error != DX_OK)
   {
       goto end_function_no_unlock;
   }

  /*----------------------------
      start sending message to SEP
  -----------------------------*/
  sramOffset = 0;

  /* start the message */
  SEPDriver_StartMessage(&sramOffset);

  /* prepare message  */
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_SELF_TEST_OP_CODE;
  messageParam[1] = EnginesSelect;

  /* send message */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam,
                           sizeof(DxUint32_t) * 2,
                           sizeof(DxUint32_t) * 2,
                           &sramOffset ,
                           DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

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
  if(Error != DX_OK)
  {
  	goto end_function_unlock;
  }

  /* read opcode + status */
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_SELF_TEST_OP_CODE)
  {
    Error = DX_WRONG_OPCODE_FROM_SEP_ERR;
    goto end_function_unlock;
  }

  /* check the status */
  if(messageParam[1] != CRYS_OK)
  {
    Error = messageParam[1];
    goto end_function_unlock;
  }

  /* read test status */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam,
                          sizeof(DxUint32_t),
                          sizeof(DxUint32_t),
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  *EnginesTestResult_ptr = (DxUint16_t)messageParam[0];

  /* ...................... end of function ................................ */

end_function_unlock:

  /* lock access to the SEP */
  SEPDriver_Unlock();

end_function_no_unlock:

  return Error;
}/* END OF CRYS_SelfTest function */


