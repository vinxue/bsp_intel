/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

   /*
   *  Object %CRYS_SMLA_SCHEMES.c    : %
   *  State           :  %state%
   *  Creation date   :  Wed Aug 23 2006
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module contains the routines performing SMLA schemes operations.
   *
   *   This module implements algorithms of SMLA_KDF - key derivation,
   *   CMLA Key Wrap and Unwrap Schemes, CMLA RSA Encryption and Decryption Schemes.
   *   as they are described in "CMLA Technical Specification. Version: v1.0-05-12-21.
   *
   *   This material also contains confidential information which may not be disclosed
   *   to others without the prior written consent of CMLA, LLC.
   *
   *  \version CRYS_SMLA_SCHEMES.c#1:csrc:1
   *  \author  R.Levin.
   */



/************* Include Files ****************/

/* .............. CRYS level includes ................. */

#include "DX_VOS_Mem.h"
#include "crys_aes.h"
#include "crys_aes_error.h"
#include "CRYS_CMLA.h"
#include "error.h"
#include "crys_host_op_code.h"
#include "SEPDriver.h"

/************************ Defines *************************************/

/************************ Enums ***************************************/

/************************ Typedefs ************************************/

/************************ Global Data *********************************/


/************* Private function prototype *****************************/


/************************ Public Functions ****************************/


/********************************************************************************************
 *	              CRYS_CMLA_KDF function                                                    *
 ********************************************************************************************/
/**
   @brief  CMLA Key Derivation Function produce the Key Encrypting Key according to
   CMLA v1.0-051221.

   CRYS_CMLA_KDF operates on an octet string of length 128.
   The input splits in two blocks of equal length. A constant value is concatenated
   to the first block. Then SHA-1 is applied to the preceding result. The result
   and two integers derived from the second block are used in a modulus calculation
   to produce the Key Encrypting Key. The output shall be 16 bytes.

   Input:  x   - Seed value, an octet string of length 128.
   Output: KEK - Key Encryption Key, an octet string of length 16.

   Steps:
     1. Let x = x0 || x1, where each xi consists of 64 bytes octet for i=0,1.
     2. Let C=(0x)00 00 00 01 (4 bytes).
     3. Compute Y = SHA-1(x0|| C) (160 bits).
     4. Let A be the first 32 bytes and B be the last 32 bytes of x1, respectively .
     5. Regarding Y, A, and B as integers, derive key encryption key KEK taking
        the least significant 128 bits of A*Y+B mod p, where p = 2^192-2^64-1.
     6. Output KEK.

   @param[in]  SeedX  - Seed value, an octet string of length 128.
   @param[out] KEK    - Key Encryption Key, an octet string of length 16.
   @param[in]  TempBuff  - Temporary buffer of length 46*(32-bits words).

   @return CRYSError_t - CRYS_OK,
                         CRYS_CMLA_KDF_INVALID_SEED_PTR_ERROR
                         CRYS_CMLA_KDF_INVALID_KEK_PTR_ERROR
                         CRYS_CMLA_KDF_INVALID_TEMP_BUFF_PTR_ERROR

*/
CEXPORT_C  CRYSError_t CRYS_CMLA_KDF( CRYS_CMLA_SEED_t      SeedX,    /*in*/
                                      CRYS_CMLA_KEK_t       KEK,      /*out*/
                                      CRYS_CMLA_KDF_TEMP_t  TempBuff  /*in*/ )
{

  /* the return error identifier */
  CRYSError_t   Error;

  /* offset */
  DxUint32_t    sramOffset;

  /* read parameter */
  DxUint32_t    messageParam[2];

  /* max length */
  DxUint32_t    maxLength;

	/*---------------------------------
	    CODE
	---------------------------------*/

  #if( !( defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) || \
        defined(CRYS_NO_CMLA_SUPPORT) || defined(CRYS_NO_AES_SUPPORT) ) )

  Error = CRYS_OK;

  /******************  Check input parameters  *********************/

  if( SeedX == DX_NULL)
  {
    Error = CRYS_CMLA_KDF_INVALID_SEED_PTR_ERROR;
    goto end_function;
  }

  if( KEK == DX_NULL)
  {
    Error = CRYS_CMLA_KDF_INVALID_KEK_PTR_ERROR;
    goto end_function;
  }

  if( TempBuff == DX_NULL)
  {
    Error = CRYS_CMLA_KDF_INVALID_TEMP_BUFF_PTR_ERROR;
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

  /* prepare params */
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_KDF_OP_CODE;

  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t),
                           sizeof(DxUint32_t),
                           &sramOffset ,
                           DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send seedX */
  maxLength = ((sizeof(CRYS_CMLA_SEED_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)SeedX ,
                           sizeof(CRYS_CMLA_SEED_t),
                           maxLength,
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_KDF_OP_CODE)
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

  /* read the kek */
  maxLength = ((sizeof(CRYS_CMLA_KEK_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater((DxUint32_t)KEK,
                           sizeof(CRYS_CMLA_KEK_t),
                           maxLength,
                           &sramOffset,
                           DX_TRUE);
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

  #endif  /* CRYS_CMLA SUPPRTED */

} /* End of CRYS_CMLA_KDF */


/********************************************************************************************
 *	              CRYS_CMLA_Wrap function                                                   *
 ********************************************************************************************/
/**
   @brief  The CRYS_CMLA_Wrap primitive composing CRYS AES Key Wrap Algorithm
           and CMLA_DDT_Perm according to CMLA algorithm CMLA v1.0-051221.

			Input:  KEK    - Key Encryption Key, an octet string of length 16 bytes
			        Key      - Key to be wrapped, an octet string of length 32 bytes
			Output: WrapKey  - An octet string of length 40 bytes.

			Steps:
			1.  Let  Key = K0 || K1,  where each Ki consists of 16 bytes octet
			    for i = 0,1.
			2.  Apply _DX_CMLA_DDT_Perm to the first 8 bytes of Ki, keeping
			    the rest of Ki unchanged, to produce 16-byte octet string ki
			    for i=0,1.
			3.  Let k = k0 || k1.
			4.  Compute CRYS_AES_Wrap (KEK, k) to produce an octet
			    string   WrapKey of 40 bytes.
			5. Output WrapKey.

   @param[in]  KEK      - Key Encryption Key, an octet string of length 16 bytes.
   @param[in]  Key      - Plain key, an octet string of length 32 bytes.
   @param[out] WrapKey  - Wrapped Key, an octet string of length 40 bytes.

   @return CRYSError_t - CRYS_OK,
           CRYS_CMLA_WRAP_ILLEGAL_KEK_PTR_ERROR
           CRYS_CMLA_WRAP_ILLEGAL_KEY_PTR_ERROR
           CRYS_CMLA_WRAP_ILLEGAL_WRAPPED_KEY_PTR_ERROR

*/
CEXPORT_C  CRYSError_t  CRYS_CMLA_Wrap(CRYS_CMLA_KEK_t            KEK,	  /*in*/
                                       CRYS_CMLA_UNWRAPPED_KEY_t  Key,     /*in*/
                                       CRYS_CMLA_WRAPPED_KEY_t    WrapKey  /*out*/ )
{
  /* The return error identifier */
  CRYSError_t   Error;

  /* offset */
  DxUint32_t    sramOffset;

  /* read parameter */
  DxUint32_t    messageParam[2+(CRYS_CMLA_WRAPPED_KEY_SIZE_IN_BYTES/4)]; /*12 words*/

  /* max length */
  DxUint32_t    maxLength;

  /*-------------------------------
    CODE
  ----------------------------------*/

  #if( !(defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) || \
       defined(CRYS_NO_CMLA_SUPPORT) || defined(CRYS_NO_AES_SUPPORT) ))


  Error = CRYS_OK;

    /******************  Check input parameters  *******************/

  if( KEK == DX_NULL)
	{
	  Error = CRYS_CMLA_WRAP_ILLEGAL_KEK_PTR_ERROR;
	  goto end_function;
	}

  if( Key == DX_NULL)
  {
    Error = CRYS_CMLA_WRAP_ILLEGAL_KEY_PTR_ERROR;
	  goto end_function;
  }

  if( WrapKey == DX_NULL)
  {
    Error = CRYS_CMLA_WRAP_ILLEGAL_WRAPPED_KEY_PTR_ERROR;
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

  /* prepare params */
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_WRAP_OP_CODE;

  /* send params */
  Error = SEPDriver_WriteParamater(
                           (DxUint32_t)messageParam ,
                           sizeof(DxUint32_t),
                           sizeof(DxUint32_t),
                           &sramOffset ,
                           DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send kek */
  maxLength = ((sizeof(CRYS_CMLA_KEK_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  DX_VOS_FastMemCpy( messageParam, KEK, sizeof(CRYS_CMLA_KEK_t) );
  Error = SEPDriver_WriteParamater(
                            (DxUint32_t)messageParam,
                            sizeof(CRYS_CMLA_KEK_t),
                            maxLength,
                            &sramOffset ,
                            DX_TRUE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send unwrap key */
  maxLength = ((sizeof(CRYS_CMLA_UNWRAPPED_KEY_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  DX_VOS_FastMemCpy( messageParam, Key, sizeof(CRYS_CMLA_UNWRAPPED_KEY_t) );
  Error = SEPDriver_WriteParamater(
                           (DxUint32_t)messageParam ,
                            sizeof(CRYS_CMLA_UNWRAPPED_KEY_t),
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
  Error = SEPDriver_ReadParamater(
                          (DxUint32_t)messageParam ,
                          sizeof(DxUint32_t) * 2,
                          sizeof(DxUint32_t) * 2,
                          &sramOffset ,
                          DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* check the opcode */
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_WRAP_OP_CODE)
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

  /* read the wrapped key */
  maxLength = ((sizeof(CRYS_CMLA_WRAPPED_KEY_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater(
                          (DxUint32_t)messageParam ,
                          sizeof(CRYS_CMLA_WRAPPED_KEY_t),
                          maxLength,
                          &sramOffset,
                          DX_TRUE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* copy data into output */
  DX_VOS_FastMemCpy( WrapKey, messageParam, sizeof(CRYS_CMLA_WRAPPED_KEY_t) );

  /* ...................... end of function ................................ */

end_function_unlock:

  /* lock access to the SEP */
  SEPDriver_Unlock();


end_function:

  return Error;

  #endif  /* CRYS_CMLA supported */

} /* End of CRYS_CMLA_Wrap */


/********************************************************************************************
 *	              CRYS_CMLA_Unwrap function                                                   *
 ********************************************************************************************/
/**
   @brief  The CRYS_CMLA_Unwrap() function performs inverse CRYS_CMLA_Wrap transformation
           and implements the following algorithm [CMLA v1.0-051221]:

	Input:   KEK    - Key Encryption Key, an octet string of length 16
	         KWrap  - key to be unwrapped, an octet string of length 40

	Output:  K      - Unwrapped key an octet string of length 32.

	Steps:
		1. Compute CRYS_AES_Unwrap(KEK, KWrap) to produce an octet string K of length 32 octets.
		2. Let K = K0 || K1, where each Ki consists of 16 bytes for i = 0,1.
		3. Apply CRYS_CMLA_DDT_PermInv to the first 8 bytes of Ki, keeping the rest of  Ki unchanged, to produce 16-byte octet string Ki  for i = 0,1.
		4. Let K = K0 || K1.
		5. Output K.

   @param[in]  KEK      - Key Encryption Key, an octet string of length 16 bytes.
   @param[in]  WrapKey  - Wrapped key (Kwrap), an octet string of length 40 bytes.
   @param[out] Key      - Unwrapped key (K), an octet string of length 32 bytes.

   @return CRYSError_t - CRYS_OK,
           CRYS_CMLA_WRAP_ILLEGAL_KEK_PTR_ERROR
           CRYS_CMLA_WRAP_ILLEGAL_KEY_PTR_ERROR
           CRYS_CMLA_WRAP_ILLEGAL_WRAPPED_KEY_PTR_ERROR

*/
CEXPORT_C  CRYSError_t  CRYS_CMLA_Unwrap(CRYS_CMLA_KEK_t            KEK,	   /*in*/
                                         CRYS_CMLA_WRAPPED_KEY_t    WrapKey, /*in*/
                                         CRYS_CMLA_UNWRAPPED_KEY_t  Key      /*out*/ )
{

  /* The return error identifier */
  CRYSError_t   Error;

  /* offset */
  DxUint32_t    sramOffset;

  /* read parameter */
  DxUint32_t    messageParam[2];

  /* max length */
  DxUint32_t    maxLength;

  /*------------------------------
      CODE
  --------------------------------*/
  #if( !( defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) || \
        defined(CRYS_NO_CMLA_SUPPORT) || defined(CRYS_NO_AES_SUPPORT) ) )

  /******************  Check input parameters  *******************/

	Error = CRYS_OK;

	if( KEK == DX_NULL)
	{
	  Error = CRYS_CMLA_UNWRAP_ILLEGAL_KEK_PTR_ERROR;
	  goto end_function;
	}

	if( Key == DX_NULL)
	{
	  Error = CRYS_CMLA_UNWRAP_ILLEGAL_KEY_PTR_ERROR;
	  goto end_function;
	}

	if( WrapKey == DX_NULL)
	{
	  Error = CRYS_CMLA_UNWRAP_ILLEGAL_WRAPPED_KEY_PTR_ERROR;
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

  /*prepare params */
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_UNWRAP_OP_CODE;

  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t),
                           sizeof(DxUint32_t),
                           &sramOffset ,
                           DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send kek */
  maxLength = ((sizeof(CRYS_CMLA_KEK_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)KEK ,
                            sizeof(CRYS_CMLA_KEK_t) ,
                            maxLength,
                            &sramOffset ,
                            DX_TRUE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send wrapped key */
  maxLength = ((sizeof(CRYS_CMLA_WRAPPED_KEY_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)WrapKey ,
                            sizeof(CRYS_CMLA_WRAPPED_KEY_t) ,
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_UNWRAP_OP_CODE)
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

  /* read the key */
  maxLength = ((sizeof(CRYS_CMLA_UNWRAPPED_KEY_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater((DxUint32_t)Key,
                          sizeof(CRYS_CMLA_UNWRAPPED_KEY_t),
                          maxLength,
                          &sramOffset,
                          DX_TRUE);
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

  #endif   /*  if CRYS_CMLA not supported  */

} /* End of CRYS_CMLA_Unwrap */


/**************************************************************************
 *	              CRYS_CMLA_RSA_Encrypt function                          *
 **************************************************************************/
/**
   @brief  The CRYS_CMLA_RSA_Encrypt primitive implements the following algorithm
           [CMLA v1.0-051221]::

			Input:     PubKey - Device's RSA public key,
			           M      - Message to be encrypted, an octet string of length 128 bytes.
			Output:    C      - Cipher text, an octet string of length 128.

			Steps:
			1. Let M = M0 || M1, where each Mi consists of 64 bytes octet for i = 0,1.
			2. Apply CRYS_CMLA_DDT_Exp to the first 3 bytes of Mi, keeping the rest of Mi
			   unchanged, to produce octet string mi for i = 0,1.
			3. Let m = m0 || m1.
			4. If the integer representative of m is not strictly less than the RSA modulus,
			   output "integer too long" and stop.
			5. Encrypt m under device public key PubKey with CRYS_RSA_PRIM_Encrypt to produce
			   the octet string C of length 128:  C = RSA_Encrypt (pubKey, m).
			6. Output C.

   @param[in]  PublKey     - A pointer to to structure containing user RSA Public Key.
   @param[in]  Message	   - A pointer to message of length 128 bytes.
   @param[out] EncrMessage - A pointer to output encrypted message of length 128 bytes.
   @param[in]  TempBuf     - A pointer to structure containing temp buffers for CRYS_CMLA_RSA operation

   @return CRYSError_t - CRYS_OK, or error message
                         CRYS_CMLA_IS_NOT_SUPPORTED
                         CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_PUB_KEY_PTR_ERROR
                         CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_MESSAGE_PTR_ERROR
                         CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_ENCR_MESSAGE_PTR_ERROR
                         CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_TEMP_BUFF_PTR_ERROR
                         CRYS_CMLA_RSA_ENCRYPT_MESSAGE_INTEGER_TOO_LONG_ERROR

*/
CEXPORT_C  CRYSError_t  CRYS_CMLA_RSA_Encrypt(CRYS_RSAUserPubKey_t*     UserPublKey_ptr, /*in*/
                    					                CRYS_CMLA_RSA_MESSAGE_t   Message,         /*in*/
                    					                CRYS_CMLA_RSA_MESSAGE_t   EncrMessage,     /*out*/
                    					                CRYS_CMLA_RSA_TempBuff_t* TempBuff_ptr     /*in */)
{
  /* The return error identifier */
  CRYSError_t   Error;

  /* offset */
  DxUint32_t    sramOffset;

  /* read parameter */
  DxUint32_t    messageParam[2];

  /* max length */
  DxUint32_t    maxLength;


  #if( !(defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) || \
       defined(CRYS_NO_CMLA_SUPPORT) || defined(CRYS_NO_AES_SUPPORT) ) )


  /*----------------------------
      CODE
  -------------------------------*/

  Error = CRYS_OK;

  /******************  Check input parameters  *******************/

	if( UserPublKey_ptr == DX_NULL)
	{
	  Error = CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_PUB_KEY_PTR_ERROR;
	  goto end_function;
	}

	if( Message == DX_NULL)
	{
	  Error = CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_MESSAGE_PTR_ERROR;
	  goto end_function;
	}

	if( EncrMessage == DX_NULL)
	{
	  Error = CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_ENCR_MESSAGE_PTR_ERROR;
	  goto end_function;
	}

	if( TempBuff_ptr == DX_NULL)
	{
	  Error = CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_TEMP_BUFF_PTR_ERROR;
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

  /* prepare params */
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_RSA_ENCRYPT_OP_CODE;

  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t),
                           sizeof(DxUint32_t),
                           &sramOffset ,
                           DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send message */
  maxLength = ((sizeof(CRYS_CMLA_RSA_MESSAGE_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)Message ,
                           sizeof(CRYS_CMLA_RSA_MESSAGE_t) ,
                           maxLength,
                           &sramOffset ,
                           DX_TRUE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send public key */
  maxLength = ((sizeof(CRYS_RSAUserPubKey_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)UserPublKey_ptr ,
                            sizeof(CRYS_RSAUserPubKey_t) ,
                            maxLength,
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_RSA_ENCRYPT_OP_CODE)
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

  /* read the encrypted message */
  maxLength = ((sizeof(CRYS_CMLA_RSA_MESSAGE_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater((DxUint32_t)EncrMessage,
                           sizeof(CRYS_CMLA_RSA_MESSAGE_t),
                           maxLength,
                           &sramOffset,
                           DX_TRUE);
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

  #endif /*  if CRYS_CMLA not supported  */

} /* End of CRYS_CMLA_RSA_Encrypt */


/**************************************************************************
 *	              CRYS_CMLA_RSA_Decrypt function                          *
 **************************************************************************/
/**
   @brief  The CRYS_CMLA_RSA_Decrypt primitive implements the following algorithm
           [CMLA v1.0-051221]:

			Input:   PrivKey - Device's RSA private key,
			         C       - Cipher text, an octet string of length 128.
			Output:  M       - Encrypted message, an octet string of length 128.
			Steps:
				1. Decrypt C under device private key PrivKey with CRYS_RSA_PRIM_Decrypt
				   to produce the octet string m of length 128:
				                   m = RSA.DECRYPT (PrivKey, C).

				2. Let m = m0 || m1, where each mi consists of 64 bytes octet for i = 0,1.
				3. Apply CRYS_CMLA_DDT_ExpInv to the first 3 bytes of mi, keeping the
				   rest of mi unchanged, to produce octet string Mi for i = 0,1.
				4. Let M = M0 || M1.
				5. Output M - message plain text.

   @param[in]  PrivKey     - A pointer to to structure containing user RSA Private Key.
   @param[in]  EncrMessage - A pointer to input encrypted message of length 128 bytes.
   @param[out] Message     - A pointer to output decrypted message of length 128 bytes.
   @param[in]  TempBuf     - A pointer to structure containing temp buffers for CRYS_CMLA_RSA
                             operation.

   @return CRYSError_t - CRYS_OK, or error message
                         CRYS_CMLA_IS_NOT_SUPPORTED
                         CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_PRIV_KEY_PTR_ERROR
                         CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_MESSAGE_PTR_ERROR
                         CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_ENCR_MESSAGE_PTR_ERROR
                         CRYS_CMLA_RSA_ENCRYPT_ILLEGAL_TEMP_BUFF_PTR_ERROR
*/
CEXPORT_C  CRYSError_t  CRYS_CMLA_RSA_Decrypt(CRYS_RSAUserPrivKey_t*    UserPrivKey_ptr, /*in*/
                    					                CRYS_CMLA_RSA_MESSAGE_t   EncrMessage,      /*in*/
                    					                CRYS_CMLA_RSA_MESSAGE_t   Message,          /*out*/
                    					                CRYS_CMLA_RSA_TempBuff_t* TempBuff_ptr      /*in */)
{
  /* The return error identifier */
  CRYSError_t   Error;

  /* offset */
  DxUint32_t    sramOffset;

  /* read parameter */
  DxUint32_t    messageParam[2];

  /* max length */
  DxUint32_t    maxLength;

  #if( !(defined(CRYS_NO_HASH_SUPPORT) || defined(CRYS_NO_PKI_SUPPORT) || \
       defined(CRYS_NO_CMLA_SUPPORT) || defined(CRYS_NO_AES_SUPPORT) ))

  /*-------------------------------------
      CODE
  ---------------------------------*/

  Error = CRYS_OK;

  /******************  Check input parameters  *******************/

  if( UserPrivKey_ptr == DX_NULL)
  {
    Error = CRYS_CMLA_RSA_DECRYPT_ILLEGAL_PRIV_KEY_PTR_ERROR;
    goto end_function;
  }

  if( Message == DX_NULL)
  {
    Error = CRYS_CMLA_RSA_DECRYPT_ILLEGAL_ENCR_MESSAGE_PTR_ERROR;
    goto end_function;
  }

  if( EncrMessage == DX_NULL)
  {
    Error = CRYS_CMLA_RSA_DECRYPT_ILLEGAL_DECR_MESSAGE_PTR_ERROR;
    goto end_function;
  }

  if( TempBuff_ptr == DX_NULL)
  {
    Error = CRYS_CMLA_RSA_DECRYPT_ILLEGAL_TEMP_BUFF_PTR_ERROR;
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

  /* prepare params */
  messageParam[0] = DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_RSA_DECRYPT_OP_CODE;

  /* send params */
  Error = SEPDriver_WriteParamater((DxUint32_t)messageParam ,
                           sizeof(DxUint32_t),
                           sizeof(DxUint32_t),
                           &sramOffset ,
                           DX_FALSE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send message */
  maxLength = ((sizeof(CRYS_CMLA_RSA_MESSAGE_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)EncrMessage ,
                            sizeof(CRYS_CMLA_RSA_MESSAGE_t) ,
                            maxLength,
                            &sramOffset ,
                            DX_TRUE);
  if(Error != DX_OK)
  {
      goto end_function_unlock;
  }

  /* send private key */
  maxLength = ((sizeof(CRYS_RSAUserPrivKey_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_WriteParamater((DxUint32_t)UserPrivKey_ptr ,
                            sizeof(CRYS_RSAUserPrivKey_t) ,
                            maxLength,
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
  if(messageParam[0] != DX_SEP_HOST_SEP_PROTOCOL_HOST_CMLA_RSA_DECRYPT_OP_CODE)
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

  /* read the decrypted message */
  maxLength = ((sizeof(CRYS_CMLA_RSA_MESSAGE_t) + 3) / sizeof(DxUint32_t)) * sizeof(DxUint32_t);
  Error = SEPDriver_ReadParamater((DxUint32_t)Message,
                           sizeof(CRYS_CMLA_RSA_MESSAGE_t),
                           maxLength,
                           &sramOffset,
                           DX_TRUE);
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

  #endif	/*  if CRYS_CMLA not supported  */

} /* End of CRYS_CMLA_RSA_Decrypt */



