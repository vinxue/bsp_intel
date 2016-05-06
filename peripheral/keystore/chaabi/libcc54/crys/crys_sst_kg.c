/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

  /*
   *  Object % CRYS_SST_KG.c    : %
   *  State           :  %state%
   *  Creation date   :  Thu Aug 23 11:29:00 2007
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This file contains the implementation of the Host side of the Host-Sep CRYS_SST key generation operations.
   *
   *  \version CRYS_SST.c#1:incl:13
   *  \author arield
   */

#include "error.h"
#include "crys_aes_error.h"
#include "crys_des_error.h"
/*#include "crys_hmac_error.h"
#include "CRYS_RSA_error.h"*/
#include "SEPDriver.h"
#include "crys_host_op_code.h"

#include "CRYS_SST.h"
#include "CRYS_SST_KG.h"
#include "CRYS_SST_error.h"

#ifdef SST_SEP_SIMULATOR_ENV
#include "crys_sst_native_duplicate_wrapper.h"
#endif
/************************ Defines ******************************/
#define CRYS_SST_MAX_HMAC_KEY_LEN_IN_BYTES			256
#define CRYS_SST_RSA_MAX_KEYSIZE_IN_BYTES    		256
/************************ Enums ********************************/
/************************ Typedefs  ****************************/
/************************ Context Structs  *********************/
/************************ Public Variables *********************/
/************************ Public Functions *********************/
/**
 * @brief The function creates AES key saves it in the SST and
 *        return the handle to it.
 *
 *
 * @param[in] TransactionId  the opened atransaction for the SST changes.
 *
 * @param[in] SessionId  the opened authenticator SessionId.
 *
 * @param[in] MasterAuthHandle - the master authenticator that has all of the
 *								access writes on the new inserted data including
 *								binding writes to additional authenticators.
 *
 * @param[in] DataType  user depend for internal SST use.
 *
 * @param[in] KeySize - type of AES key (128,192 or 256 bits).
 *
 * @param[in/out] KeyHandle_ptr - handle to SST entry.The user can enter an empty handle
 *								  and get a new handle, or enter handle and force the SST to use it
 *								  if possible.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned,
 *                        and on failure a value from crys_aes_error.h TBD
 */

CIMPORT_C CRYSError_t  CRYS_SST_KG_AES(  DxUint32_t			TransactionId,
									     SSTSessionId_t		SessionId,
										 SSTHandle_t		MasterAuthHandle,
			                             SSTDataType_t      DataType,
										 CRYS_AES_KeySize_t KeySize,
										 SSTHandle_t	   *KeyHandle_ptr,
										 DxUint8_t*			aWorkspace_ptr,
										 DxUint32_t		    aWorkspaceSizeInBytes)
{
  /* The return error identifier */
  CRYSError_t         Error;

  /* offset */
  DxUint32_t          sramOffset;

  /* read parameter */
  DxUint32_t          messageParam[9];

  /*----------------------
      CODE
  ------------------------*/
  /* eliminating compiler warnings: */
  aWorkspace_ptr = aWorkspace_ptr;
  aWorkspaceSizeInBytes = aWorkspaceSizeInBytes;

  Error = CRYS_OK;

  #ifndef CRYS_NO_AES_SUPPORT

  if( KeySize >= CRYS_AES_KeySizeNumOfOptions )
  {
    Error = CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
    goto end_function;
  }

  if (KeyHandle_ptr == DX_NULL)
  {
    Error = SST_RC_ERROR_INVALID_PARAM;
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
  messageParam[0] =	DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_AES_OP_CODE;
  messageParam[1] = TransactionId;
  messageParam[2] = SessionId;
  messageParam[3] = MasterAuthHandle.objDigest;
  messageParam[4] = MasterAuthHandle.objId;
  messageParam[5] = DataType;
  messageParam[6] = KeySize;
  messageParam[7] = KeyHandle_ptr->objDigest;
  messageParam[8] = KeyHandle_ptr->objId;


  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t) * 9,
                           sizeof(DxUint32_t) * 9,
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

  /* read opcode + status  */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* check the opcode */
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_AES_OP_CODE)
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

   /* read key handle */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

 KeyHandle_ptr->objDigest = messageParam[0];
 KeyHandle_ptr->objId = messageParam[1];

end_function_unlock:

  /* unlock access to the SEP */
  SEPDriver_Unlock();

end_function:

  return Error;

  #endif /* !CRYS_NO_AES_SUPPORT */

}/* END OF CRYS_SST_KG_AES */

/**
 * @brief The function creates DES key saves it in the SST and
 *        return the handle to it.
 *
 *
 * @param[in] TransactionId  the opened atransaction for the SST changes.
 *
 * @param[in] SessionId  the opened authenticator SessionId.
 *
 * @param[in] MasterAuthHandle - the master authenticator that has all of the
 *								access writes on the new inserted data including
 *								binding writes to additional authenticators.
 *
 * @param[in] DataType  user depand for internal SST use.
 *
 * @param[in,out] NumOfKeys - type of DES key (DES or 3DES).
 *
 * @param[in/out] KeyHandle_ptr - handle to SST entry.The user can enter an empty handle
 *								  and get a new handle, or enter handle and force the SST to use it
 *								  if possible.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned,
 *                        and on failure a value from crys_aes_error.h TBD
 */

CIMPORT_C CRYSError_t  CRYS_SST_KG_DES(  DxUint32_t			    TransactionId,
									     SSTSessionId_t		    SessionId,
										 SSTHandle_t		    MasterAuthHandle,
			                             SSTDataType_t          DataType,
										 CRYS_DES_NumOfKeys_t   NumOfKeys,
										 SSTHandle_t		    *KeyHandle_ptr,
										 DxUint8_t*			    aWorkspace_ptr,
										 DxUint32_t		        aWorkspaceSizeInBytes)
{
  /* The return error identifier */
  CRYSError_t         Error;

  /* offset */
  DxUint32_t          sramOffset;

  /* read parameter */
  DxUint32_t          messageParam[9];

  /*----------------------
      CODE
  ------------------------*/
  /* eliminating compiler warnings: */
  aWorkspace_ptr = aWorkspace_ptr;
  aWorkspaceSizeInBytes = aWorkspaceSizeInBytes;

  Error = CRYS_OK;

  #ifndef CRYS_NO_DES_SUPPORT

  if( NumOfKeys >= CRYS_DES_NumOfKeysOptions )
  {
    Error = CRYS_DES_ILLEGAL_NUM_OF_KEYS_ERROR;
    goto end_function;
  }

  if (KeyHandle_ptr == DX_NULL)
  {
    Error = SST_RC_ERROR_INVALID_PARAM;
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
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_DES_OP_CODE;
  messageParam[1] = TransactionId;
  messageParam[2] = SessionId;
  messageParam[3] = MasterAuthHandle.objDigest;
  messageParam[4] = MasterAuthHandle.objId;
  messageParam[5] = DataType;
  messageParam[6] = NumOfKeys;
  messageParam[7] = KeyHandle_ptr->objDigest;
  messageParam[8] = KeyHandle_ptr->objId;


  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t) * 9,
                           sizeof(DxUint32_t) * 9,
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

  /* read opcode + status  */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* check the opcode */
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_DES_OP_CODE)
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

   /* read key handle */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

 KeyHandle_ptr->objDigest = messageParam[0];
 KeyHandle_ptr->objId = messageParam[1];

end_function_unlock:

  /* unlock access to the SEP */
  SEPDriver_Unlock();

end_function:

  return Error;

  #endif /* !CRYS_NO_DES_SUPPORT */

}/* END OF CRYS_SST_KG_DES */


/**
 * @brief The function creates HMAC key saves it in the SST and
 *        return the handle to it.
 *
 *
 * @param[in] TransactionId  the opened transaction for the SST changes.
 *
 * @param[in] SessionId  the opened authenticator SessionId.
 *
 * @param[in] MasterAuthHandle - the master authenticator that has all of the
 *								access writes on the new inserted data including
 *								binding writes to additional authenticators.
 *
 * @param[in] DataType  user depend for internal SST use.
 *
 * @param[in] KeySize - HMAC key len in bytes.
 *
 * @param[in/out] KeyHandle_ptr - handle to SST entry.The user can enter an empty handle
 *								  and get a new handle, or enter handle and force the SST to use it
 *								  if possible.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned,
 *                        and on failure a value from crys_aes_error.h TDB
 */

CIMPORT_C CRYSError_t  CRYS_SST_KG_HMAC( DxUint32_t			TransactionId,
									     SSTSessionId_t		SessionId,
										 SSTHandle_t		MasterAuthHandle,
			                             SSTDataType_t      DataType,
										 DxUint32_t			KeySize,
										 SSTHandle_t		*KeyHandle_ptr,
										 DxUint8_t*			aWorkspace_ptr,
										 DxUint32_t		    aWorkspaceSizeInBytes)
{
  /* The return error identifier */
  CRYSError_t         Error;

  /* offset */
  DxUint32_t          sramOffset;

  /* read parameter */
  DxUint32_t          messageParam[9];

  /*----------------------
      CODE
  ------------------------*/
  /* eliminating compiler warnings: */
  aWorkspace_ptr = aWorkspace_ptr;
  aWorkspaceSizeInBytes = aWorkspaceSizeInBytes;

  Error = CRYS_OK;

  #ifndef CRYS_NO_HASH_SUPPORT

  if( KeySize > CRYS_SST_MAX_HMAC_KEY_LEN_IN_BYTES )
  {
    Error = CRYS_SST_MODULE_WRONG_HMAC_KEY_SIZE;
    goto end_function;
  }

  if (KeyHandle_ptr == DX_NULL)
  {
    Error = SST_RC_ERROR_INVALID_PARAM;
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
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_HMAC_OP_CODE;
  messageParam[1] = TransactionId;
  messageParam[2] = SessionId;
  messageParam[3] = MasterAuthHandle.objDigest;
  messageParam[4] = MasterAuthHandle.objId;
  messageParam[5] = DataType;
  messageParam[6] = KeySize;
  messageParam[7] = KeyHandle_ptr->objDigest;
  messageParam[8] = KeyHandle_ptr->objId;


  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t) * 9,
                           sizeof(DxUint32_t) * 9,
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

  /* read opcode + status  */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* check the opcode */
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_HMAC_OP_CODE)
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

   /* read key handle */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

 KeyHandle_ptr->objDigest = messageParam[0];
 KeyHandle_ptr->objId = messageParam[1];

end_function_unlock:

  /* unlock access to the SEP */
  SEPDriver_Unlock();

end_function:

  return Error;

  #endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_SST_KG_HMAC */


/**
 * @brief The function creates RSA key pair saves it in the SST and
 *        return the handle to it.
 *
 *
 * @param[in] TransactionId  the opened atransaction for the SST changes.
 *
 * @param[in] SessionId  the opened authenticator SessionId.
 *
 * @param[in] MasterAuthHandle - the master authenticator that has all of the
 *								access writes on the new inserted data including
 *								binding writes to additional authenticators.
 *
 * @param[in] DataType  user depand for internal SST use.
 *
 * @param[in/out] KeyHandle_ptr - handle to SST entry.The user can enter an empty handle
 *								  and get a new handle, or enter handle and force the SST to use it
 *								  if possible.
 *
 * @param[in] PubExp_ptr - public exponent
 *
 * @param[in] PubExpSizeInBytes - size of public exponent in bytes.
 *
 * @param[in] KeySize - size of the modulus.
 *
 * @param[in] KeyGenData_ptr - internal buffer for the key gen operation.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned,
 *                        and on failure a value from crys_aes_error.h TBD
 */

CIMPORT_C CRYSError_t CRYS_SST_RSA_KG_GenerateKeyPair(DxUint32_t					TransactionId,
													  SSTSessionId_t				SessionId,
													  SSTHandle_t					MasterAuthHandle,
													  SSTDataType_t					DataType,
													  SSTHandle_t					*KeyHandle_ptr,
													  DxUint8_t						*PubExp_ptr,
													  DxUint16_t					PubExpSizeInBytes,
													  DxUint32_t					KeySize,
													  CRYS_SST_RSA_keys_and_data_container_t	*KeyGenData_ptr)
{
  /* The return error identifier */
  CRYSError_t         Error;

  /* offset */
  DxUint32_t          sramOffset;

  /* read parameter */
  DxUint32_t          messageParam[10];

  /* max length (padding to words) */
  DxUint32_t          maxLength;

  /*----------------------
      CODE
  ------------------------*/
  /* eliminating compiler warnings: */
  KeyGenData_ptr = KeyGenData_ptr;

  Error = CRYS_OK;

  #ifndef CRYS_NO_PKI_SUPPORT

  if( (KeySize/8) > CRYS_SST_RSA_MAX_KEYSIZE_IN_BYTES )
  {
    Error = CRYS_SST_MODULE_WRONG_RSA_KEY_SIZE;
    goto end_function;
  }

  if (KeyHandle_ptr == DX_NULL)
  {
    Error = SST_RC_ERROR_INVALID_PARAM;
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
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_RSA_OP_CODE;
  messageParam[1] = TransactionId;
  messageParam[2] = SessionId;
  messageParam[3] = MasterAuthHandle.objDigest;
  messageParam[4] = MasterAuthHandle.objId;
  messageParam[5] = DataType;
  messageParam[6] = KeyHandle_ptr->objDigest;
  messageParam[7] = KeyHandle_ptr->objId;
  messageParam[8] = PubExpSizeInBytes;
  messageParam[9] = KeySize;


  /* send params */

  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t) * 10,
                           sizeof(DxUint32_t) * 10,
                           &sramOffset ,
                           DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }


  /* send exponent */
  maxLength = ((PubExpSizeInBytes + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)PubExp_ptr ,
                           PubExpSizeInBytes,
                           maxLength,
                           &sramOffset ,
                           DX_TRUE);
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

  /* read opcode + status  */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* check the opcode */
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_RSA_OP_CODE)
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

   /* read key handle */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

 KeyHandle_ptr->objDigest = messageParam[0];
 KeyHandle_ptr->objId = messageParam[1];

end_function_unlock:

  /* unlock access to the SEP */
  SEPDriver_Unlock();

end_function:

  return Error;

  #endif /* !CRYS_NO_PKI_SUPPORT */

}/* END OF CRYS_SST_RSA_KG_GenerateKeyPair */


/**
 * @brief The function creates RSA key pair with CRT format ,saves it in the SST and
 *        return the handle to it.
 *
 *
 * @param[in] TransactionId  the opened atransaction for the SST changes.
 *
 * @param[in] SessionId  the opened authenticator SessionId.
 *
 * @param[in] MasterAuthHandle - the master authenticator that has all of the
 *								access writes on the new inserted data including
 *								binding writes to additional authenticators.
 *
 * @param[in] DataType  user depand for internal SST use.
 *
 * @param[in/out] KeyHandle_ptr - handle to SST entry.The user can enter an empty handle
 *								  and get a new handle, or enter handle and force the SST to use it
 *								  if possible.
 *
 * @param[in] PubExp_ptr - public exponent
 *
 * @param[in] PubExpSizeInBytes - size of public exponent in bytes.
 *
 * @param[in] KeySize - size of the modulus.
 *
 * @param[in] KeyGenData_ptr - internal buffer for the key gen operation.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned,
 *                        and on failure a value from crys_aes_error.h TBD
 */

CIMPORT_C CRYSError_t CRYS_SST_RSA_KG_GenerateKeyPairCRT(DxUint32_t					TransactionId,
														 SSTSessionId_t				SessionId,
														 SSTHandle_t				MasterAuthHandle,
														 SSTDataType_t				DataType,
														 SSTHandle_t				*KeyHandle_ptr,
														 DxUint8_t					*PubExp_ptr,
														 DxUint16_t					PubExpSizeInBytes,
														 DxUint32_t					KeySize,
														 CRYS_SST_RSA_keys_and_data_container_t	*KeyGenData_ptr)
{
  /* The return error identifier */
  CRYSError_t         Error;

  /* offset */
  DxUint32_t          sramOffset;

  /* read parameter */
  DxUint32_t          messageParam[10];

  /* max length (padding to words) */
  DxUint32_t          maxLength;

  /*----------------------
      CODE
  ------------------------*/
  /* eliminating compiler warnings: */
  KeyGenData_ptr = KeyGenData_ptr;

  Error = CRYS_OK;

  #ifndef CRYS_NO_PKI_SUPPORT

  if( (KeySize/8) > CRYS_SST_RSA_MAX_KEYSIZE_IN_BYTES )
  {
    Error = CRYS_SST_MODULE_WRONG_RSA_KEY_SIZE;
    goto end_function;
  }

  if (KeyHandle_ptr == DX_NULL)
  {
    Error = SST_RC_ERROR_INVALID_PARAM;
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
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_RSA_CRT_OP_CODE;
  messageParam[1] = TransactionId;
  messageParam[2] = SessionId;
  messageParam[3] = MasterAuthHandle.objDigest;
  messageParam[4] = MasterAuthHandle.objId;
  messageParam[5] = DataType;
  messageParam[6] = KeyHandle_ptr->objDigest;
  messageParam[7] = KeyHandle_ptr->objId;
  messageParam[8] = PubExpSizeInBytes;
  messageParam[9] = KeySize;


  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t) * 10,
                           sizeof(DxUint32_t) * 10,
                           &sramOffset ,
                           DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }


  /* send exponent */
  maxLength = ((PubExpSizeInBytes + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)PubExp_ptr ,
                           PubExpSizeInBytes,
                           maxLength,
                           &sramOffset ,
                           DX_TRUE);
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

  /* read opcode + status  */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* check the opcode */
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_SST_KG_RSA_CRT_OP_CODE)
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

   /* read key handle */
  Error = SEPDriver_ReadParamater((DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

 KeyHandle_ptr->objDigest = messageParam[0];
 KeyHandle_ptr->objId = messageParam[1];

end_function_unlock:

  /* unlock access to the SEP */
  SEPDriver_Unlock();

end_function:

  return Error;

  #endif /* !CRYS_NO_PKI_SUPPORT */

}/* END OF CRYS_SST_RSA_KG_GenerateKeyPairCRT */

