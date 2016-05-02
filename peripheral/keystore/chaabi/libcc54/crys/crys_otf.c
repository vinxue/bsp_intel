/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

   /*
   *  Object %CRYS_OTF.c    : %
   *  State           :  %state%
   *  Creation date   :  Wed July 18 17:25:55 2007
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief this function contain the OTF functions.
   *
   *  \version CRYS_OTF.c#1:csrc:1
   *  \author yakovg
   */



/************* Include Files ****************/

/* .............. CRYS level includes ................. */

#include "DX_VOS_Mem.h"
#include "CRYS_COMMON.h"
#include "CRYS_COMMON_math.h"
#include "PLAT_SystemDep.h"
#include "CRYS_OTF.h"
#include "CRYS_OTF_error.h"
#include "crys_des.h"
#include "error.h"
#include "crys_host_op_code.h"
#include "SEPDriver.h"



/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Private Functions ******************************/


/************************ Public Functions ******************************/


/**************************************************************************************************
 * @brief CRYS_OTF_InitOperation - This function sets the OTF context with the
 * 								   required operation scenario, mode of operation, keys and Ivs.
 *
 * @param[in] OTFContext_ptr - Pointer to the OTF context.
 *
 * @param[in] CRYS_OTFEngCfg - The mode of the engine to configure the Crypto Flashfor the process.
 *							   The engine modes are defined at the CRYS_OTFEngCfg_t type.
 *
 * @param[in] CRYS_OTFKeysInfo_ptr - A pointer to a structure contains the Key and IV.
 *
 * @param[in] CRYS_OTFEngMode_ptr  - Pointer to CRYS_OTFEngMode_t array.
 *									 Each cell in the array indicates on the mode of operation of
 *									 this stage in the path from DIN to DOUT/CPU.
 *									 The mode is the mode to set the Crypto Flash hardware operation.
 *								     The operation mode are defined in the CRYS_OTFEngMode_t type.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 *
 **************************************************************************************************/

CEXPORT_C CRYSError_t CRYS_OTF_InitOperation (CRYS_OTFUserContext_t*  ContextID_ptr,
                                              CRYS_OTFEngCfg_t        OTFEngCfg,
                                              CRYS_OTFUserKeysInfo_t* OTFUserKeysInfo_ptr,
                                              CRYS_OTFEngMode_t*      OTFEngMode_ptr)
{
  /* The return error identifiers */
  CRYSError_t Error;

  /* offset into SRAM */
  DxUint32_t   sramOffset;

  /* read param */
  DxUint32_t   messageParam[9];

  /* max length */
  DxUint32_t   maxLength;


  /*----------------------------------------
      CODE
  --------------------------------------------*/

  /* initializing the Error to O.K */
  Error = CRYS_OK;


  #ifndef CRYS_NO_OTF_SUPPORT

  /* ............... checking the parameters validity ................... */
  /* -------------------------------------------------------------------- */


  /* check input parameters */
  if( ContextID_ptr == DX_NULL )
  {
    Error = CRYS_OTF_INVALID_USER_CONTEXT_POINTER_ERROR;
    goto end_function;
  }

  if( OTFUserKeysInfo_ptr == DX_NULL )
  {
    Error = CRYS_OTF_INVALID_USER_KEYS_POINTER_ERROR;
    goto end_function;
  }

 	if( OTFEngMode_ptr == DX_NULL )
 	{
 	  Error = CRYS_OTF_INVALID_ENG_MODE_POINTER_ERROR;
 	  goto end_function;
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
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_OTF_INIT_OPERATION_OP_CODE;
  messageParam[1] = OTFEngCfg;
  messageParam[2] = OTFEngMode_ptr[0];
  messageParam[3] = OTFEngMode_ptr[1];
  messageParam[4] = OTFEngMode_ptr[2];
  messageParam[5] = OTFUserKeysInfo_ptr->Key0Size;
  messageParam[6] = OTFUserKeysInfo_ptr->Iv0Size;
  messageParam[7] = OTFUserKeysInfo_ptr->Key1Size;
  messageParam[8] = OTFUserKeysInfo_ptr->Iv1Size;

  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam,
                                    sizeof(DxUint32_t) * 9,
                                    sizeof(DxUint32_t) * 9,
                                    &sramOffset,
                                    DX_FALSE);
  if(Error != DX_OK)
  {
    goto end_function_unlock;
  }

  /* send key 0 */
  Error = SEPDriver_WriteParamater((DxUint32_t)&OTFUserKeysInfo_ptr->Key0[0] ,
                                    CRYS_OTF_MAX_KEY_SIZE_IN_BYTES,
                                    CRYS_OTF_MAX_KEY_SIZE_IN_BYTES,
                                    &sramOffset ,
                                    DX_FALSE);
  if(Error != DX_OK)
  {
    goto end_function_unlock;
  }
  /* send iv 0 */
  Error = SEPDriver_WriteParamater((DxUint32_t)&OTFUserKeysInfo_ptr->Iv0[0] ,
                                    CRYS_OTF_MAX_IV_SIZE_IN_BYTES,
                                    CRYS_OTF_MAX_IV_SIZE_IN_BYTES,
                                    &sramOffset ,
                                    DX_FALSE);
  if(Error != DX_OK)
  {
    goto end_function_unlock;
  }

  /* send key 1 */
  Error = SEPDriver_WriteParamater((DxUint32_t)&OTFUserKeysInfo_ptr->Key1[0] ,
                                    CRYS_OTF_MAX_KEY_SIZE_IN_BYTES,
                                    CRYS_OTF_MAX_KEY_SIZE_IN_BYTES,
                                    &sramOffset ,
                                    DX_FALSE);
  if(Error != DX_OK)
  {
    goto end_function_unlock;
  }
  /* send iv 1 */
  Error = SEPDriver_WriteParamater((DxUint32_t)&OTFUserKeysInfo_ptr->Iv1[0] ,
                                    CRYS_OTF_MAX_IV_SIZE_IN_BYTES,
                                    CRYS_OTF_MAX_IV_SIZE_IN_BYTES,
                                    &sramOffset ,
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_OTF_INIT_OPERATION_OP_CODE)
  {
    Error = DX_WRONG_OPCODE_FROM_SEP_ERR;
    goto end_function_unlock;
  }

  /* check status */
  if(messageParam[1] != CRYS_OK)
  {
    Error = messageParam[1];
    goto end_function_unlock;
  }

  /* read the context */
  maxLength = ((sizeof(CRYS_OTFUserContext_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater((DxUint32_t)ContextID_ptr,
                                  sizeof(CRYS_OTFUserContext_t),
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

   return Error;

#endif /* !CRYS_NO_OTF_SUPPORT */

} /* end CRYS_OTF_InitOperation function */


/**************************************************************************************************
 * @brief CRYS_OTF_SuspendOperation - This function stop the hw operation started in the Init operation.
 *												  The function store the last state to enable the user
 *												  to resume to this state exactly by the CRYS_OTF_ResumeOperation
 * @param[in] OTFContext_ptr - Pointer to the OTF context.
 *
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 *
 **************************************************************************************************/
CEXPORT_C CRYSError_t CRYS_OTF_SuspendOperation (CRYS_OTFUserContext_t *ContextID_ptr)
{
  /* The return error identifiers */
  CRYSError_t Error;

  /* offset into SRAM */
  DxUint32_t   sramOffset;

  /* read param */
  DxUint32_t   messageParam[2];

  /* max length */
  DxUint32_t   maxLength;


  /*---------------------------
      CODE
  --------------------------------*/

  /* initializing the Error to O.K */
  Error = CRYS_OK;


  #ifndef CRYS_NO_OTF_SUPPORT

  /* ............... checking the parameters validity ................... */
  /* -------------------------------------------------------------------- */

  /* if the users context ID pointer is DX_NULL return an error */
  if( ContextID_ptr == DX_NULL )
  {
    Error = CRYS_OTF_INVALID_USER_CONTEXT_POINTER_ERROR;
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
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_OTF_SUSPEND_OPERATION_OP_CODE;

  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam,
                                    sizeof(DxUint32_t),
                                    sizeof(DxUint32_t),
                                    &sramOffset,
                                    DX_FALSE);
  if(Error != DX_OK)
  {
    goto end_function_unlock;
  }

  /* send context */
  maxLength = ((sizeof(CRYS_OTFUserContext_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)ContextID_ptr ,
                                    sizeof(CRYS_OTFUserContext_t),
                                    maxLength,
                                    &sramOffset ,
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_OTF_SUSPEND_OPERATION_OP_CODE)
  {
    Error = DX_WRONG_OPCODE_FROM_SEP_ERR;
    goto end_function_unlock;
  }

  /* check status */
  if(messageParam[1] != CRYS_OK)
  {
    Error = messageParam[1];
    goto end_function_unlock;
  }

  /* read the context */
  maxLength = ((sizeof(CRYS_OTFUserContext_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater((DxUint32_t)ContextID_ptr,
                                  sizeof(CRYS_OTFUserContext_t),
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

  return Error;

#endif /* !CRYS_NO_OTF_SUPPORT */

} /* end CRYS_OTF_SuspendOperation function */



/**************************************************************************************************
 * @brief CRYS_OTF_ResumeOperation - This function restart the hw operation stopped by the Rsume operation.
 *												 The function load to the HW the last state stored in the given context
 *												  and continue the operation exactly from the point it stopped in the Suspend function.
 * @param[in] OTFContext_ptr - Pointer to the OTF user context.
 *
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 *
 **************************************************************************************************/
CEXPORT_C CRYSError_t CRYS_OTF_ResumeOperation (CRYS_OTFUserContext_t *ContextID_ptr)
{
  /* The return error identifiers */
  CRYSError_t Error;

  /* offset into SRAM */
  DxUint32_t   sramOffset;

  /* read param */
  DxUint32_t   messageParam[2];

  /* max length */
  DxUint32_t   maxLength;


  /*---------------------------
      CODE
  --------------------------------*/

  /* initializing the Error to O.K */
  Error = CRYS_OK;


#ifndef CRYS_NO_OTF_SUPPORT


  /* ............... checking the parameters validity ................... */
  /* -------------------------------------------------------------------- */

  /* if the users context ID pointer is DX_NULL return an error */
  if( ContextID_ptr == DX_NULL )
  {
    Error = CRYS_OTF_INVALID_USER_CONTEXT_POINTER_ERROR;
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
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_OTF_RESUME_OPERATION_OP_CODE;

  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam,
                                    sizeof(DxUint32_t),
                                    sizeof(DxUint32_t),
                                    &sramOffset,
                                    DX_FALSE);
  if(Error != DX_OK)
  {
    goto end_function_unlock;
  }

  /* send context */
  maxLength = ((sizeof(CRYS_OTFUserContext_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)ContextID_ptr ,
                                    sizeof(CRYS_OTFUserContext_t),
                                    maxLength,
                                    &sramOffset ,
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_OTF_RESUME_OPERATION_OP_CODE)
  {
    Error = DX_WRONG_OPCODE_FROM_SEP_ERR;
    goto end_function_unlock;
  }

  /* check status */
  if(messageParam[1] != CRYS_OK)
  {
    Error = messageParam[1];
    goto end_function_unlock;
  }

  /* read the context */
  maxLength = ((sizeof(CRYS_OTFUserContext_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater((DxUint32_t)ContextID_ptr,
                                  sizeof(CRYS_OTFUserContext_t),
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

  return Error;

#endif

}


/**************************************************************************************************
 * @brief CRYS_OTF_EndOperation - This function stop the hw operation.
 *												  The function read the last data to the user
 *
 * @param[in] OTFContext_ptr - Pointer to the OTF context.
 *
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 *
 **************************************************************************************************/
CEXPORT_C CRYSError_t CRYS_OTF_EndOperation(CRYS_OTFUserContext_t*    ContextID_ptr,
              			 												CRYS_HASH_Result_t        HashResultBuff,
              			 											 	CRYS_AES_MAX_MAC_RESULT_t MacResult)
{

  /* The return error identifiers */
  CRYSError_t   Error;

  /* offset into SRAM */
  DxUint32_t    sramOffset;

  /* read param */
  DxUint32_t    messageParam[2];

  /* max length */
  DxUint32_t    maxLength;

  /*------------------------------
      CODE
  --------------------------------*/

  /* initializing the Error to O.K */
  Error = CRYS_OK;


  #ifndef CRYS_NO_OTF_SUPPORT


  /* ............... checking the parameters validity ................... */
  /* -------------------------------------------------------------------- */


  /* if the users context ID pointer is DX_NULL return an error */
  if( ContextID_ptr == DX_NULL )
  {
    Error = CRYS_OTF_INVALID_USER_CONTEXT_POINTER_ERROR;
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
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_OTF_END_OPERATION_OP_CODE;

  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam,
                                    sizeof(DxUint32_t),
                                    sizeof(DxUint32_t),
                                    &sramOffset,
                                    DX_FALSE);
  if(Error != DX_OK)
  {
    goto end_function_unlock;
  }

  /* send context */
  maxLength = ((sizeof(CRYS_OTFUserContext_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)ContextID_ptr ,
                                    sizeof(CRYS_OTFUserContext_t),
                                    maxLength,
                                    &sramOffset ,
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_OTF_END_OPERATION_OP_CODE)
  {
    Error = DX_WRONG_OPCODE_FROM_SEP_ERR;
    goto end_function_unlock;
  }

  /* check status */
  if(messageParam[1] != CRYS_OK)
  {
    Error = messageParam[1];
    goto end_function_unlock;
  }

  switch( ((OTFContext_t*)ContextID_ptr->context_buff)->OTFEngCfg)
  {
    case OTF_DIN_TO_HASH:
		case OTF_DIN_TO_HASH_AND_DOUT:
		case OTF_DIN_TO_AES_TO_HASH:
		case OTF_DIN_TO_AES_AND_HASH_TO_DOUT:
		case OTF_DIN_TO_AES_TO_HASH_AND_DOUT:
		case OTF_DIN_TO_DES_TO_HASH:
		case OTF_DIN_TO_DES_AND_HASH_TO_DOUT:
		case OTF_DIN_TO_DES_TO_HASH_AND_DOUT:
		  /* read the hash result */
      maxLength = ((sizeof(CRYS_HASH_Result_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
      Error = SEPDriver_ReadParamater((DxUint32_t)HashResultBuff,
                                      sizeof(CRYS_HASH_Result_t),
                                      maxLength,
                                      &sramOffset,
                                      DX_FALSE);
      if(Error != DX_OK)
      {
        goto end_function_unlock;
      }
      break;

    default:
      /* read the mac result */
      maxLength = ((sizeof(CRYS_AES_MAX_MAC_RESULT_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
      Error = SEPDriver_ReadParamater((DxUint32_t)MacResult,
                                      sizeof(CRYS_AES_MAX_MAC_RESULT_t),
                                      maxLength,
                                      &sramOffset,
                                      DX_FALSE);
      if(Error != DX_OK)
      {
        goto end_function_unlock;
      }

  }

  /* ...................... end of function ................................ */

end_function_unlock:

  /* lock access to the SEP */
  SEPDriver_Unlock();


end_function:

  return Error;

#endif /* !CRYS_NO_OTF_SUPPORT */

}
