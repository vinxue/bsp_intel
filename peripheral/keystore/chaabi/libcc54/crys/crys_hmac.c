/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

/** @file
 *  \brief A brief description of this module
 *
 *  \version CRYS_HMAC.c#1:csrc:4
 *  \author adams
 */


/************* Include Files ****************/
#include "crys_hmac_error.h"
#include "crys_hash.h"
#include "crys_hmac.h"
//#include "error.h"

#include <stdio.h>
#include <unistd.h>
//#include "error.h"

#include "driver_interface.h"
#include "crys_context_relocation.h"
#include "sep_ctx.h"
#include "dx_pal_mem.h"


/************* Private function prototype ****************/


/*!
 * Convert from DxDI return code to CRYS return code
 *
 * \param diRc:		Driver Interface returned error.
 * \param errorInfo:	Additional error information.
 *
 */
static CRYSError_t DX_HmacDiErr2CrysErr(DxDI_RetCode_t diRc, DxUint32_t errorInfo)
{
	switch (diRc) {
	case DXDI_RET_ENODEV:
	case DXDI_RET_EINTERNAL:
	case DXDI_RET_ENOTSUP:
	case DXDI_RET_ENOPERM:
	case DXDI_RET_EINVAL:
	case DXDI_RET_ENORSC:
	case DXDI_RET_EHW:
		/* DXDI errors are below CRYS_ERROR_BASE - let's reuse... */
		switch (errorInfo) {
		case DXDI_ERROR_BAD_CTX:
			return CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR;
		case DXDI_ERROR_UNSUP:
			return CRYS_HMAC_IS_NOT_SUPPORTED;
		case DXDI_ERROR_INVAL_MODE:
			return CRYS_HMAC_ILLEGAL_OPERATION_MODE_ERROR;
		case DXDI_ERROR_INVAL_KEY_SIZE:
			return CRYS_HMAC_UNVALID_KEY_SIZE_ERROR;
		case DXDI_ERROR_INVAL_DIN_PTR:
			return CRYS_HMAC_DATA_IN_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DATA_SIZE:
			return CRYS_HMAC_DATA_SIZE_ILLEGAL;
		case DXDI_ERROR_NO_RESOURCE:
			return CRYS_OUT_OF_RESOURCE_ERROR;
		case DXDI_ERROR_INTERNAL:
		case DXDI_ERROR_FATAL:
		default:
			return CRYS_FATAL_ERROR;
		}
	case DXDI_RET_ESEP:
		/* Pass through SeP error code */
		return errorInfo;
	default:
		/* ??? */
		return CRYS_FATAL_ERROR;
	}
}

/*!
 * Get the actual context size of given AES mode
 *
 * \param mode
 *
 * \return DxUint32_t Context size in bytes
 */
static DxUint32_t GetContextSize(CRYS_HASH_OperationMode_t mode)
{
	/* Sizes are cached to avoid IOCTL on each size query */
	/* Uninitialized - to be initialized on first invocation */
	static DxUint32_t hmac_ctx_size = UINT32_MAX;


	/* For each mode we check if size is already "cached" */
	/* DxDI is queried only if size is not cached already */
	if (hmac_ctx_size == UINT32_MAX)
		hmac_ctx_size = DxDI_GetMacCtxSize(DXDI_MAC_HMAC);

	return hmac_ctx_size;

}
static CRYSError_t InitHmacParams(CRYS_HMACUserContext_t*    ContextID_ptr,
				  CRYS_HASH_OperationMode_t  OperationMode,
				  DxUint8_t*                 key_ptr,
				  DxUint16_t                 keySize,
				  struct dxdi_mac_props*     macProps,
				  DxUint32_t**               InternalCnxtPtr)
{
	unsigned long realContextSize;
	/* the length of the hash digest */
	DxUint16_t hashDigestLen;
	/* the length of the hash block (max. K0 size) */
	DxUint16_t maxK0Len;
	/* the KEY hash result - relevant if the key is larger then 64 bytes */
	CRYS_HASH_Result_t keyHashResult = {0};
	CRYSError_t crysRc;

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* check if the key pointer is valid */
	if (key_ptr == DX_NULL)
		return CRYS_HMAC_INVALID_KEY_POINTER_ERROR;

	/* check if the key size is valid */
	if (keySize == 0)
		return CRYS_HMAC_UNVALID_KEY_SIZE_ERROR;


	realContextSize = GetContextSize(OperationMode);
	if (realContextSize == 0)
		return CRYS_HMAC_ILLEGAL_OPERATION_MODE_ERROR;

	/* Get pointer within the buffer that can accomodate context without
	       crossing a page */
	*InternalCnxtPtr = DX_InitUserCtxLocation(ContextID_ptr,
						  sizeof(*ContextID_ptr),
						  realContextSize);


	if (*InternalCnxtPtr == 0)
		return CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* Get key size limit */
	switch (OperationMode) {
		case CRYS_HASH_SHA1_mode:
		case CRYS_HASH_SHA224_mode:
		case CRYS_HASH_SHA256_mode:
			maxK0Len = CRYS_HMAC_KEY_SIZE_IN_BYTES;
			break;
		case CRYS_HASH_SHA384_mode:
		case CRYS_HASH_SHA512_mode:
			maxK0Len = CRYS_HMAC_SHA2_1024BIT_KEY_SIZE_IN_BYTES;
			break;
		default: return CRYS_HMAC_ILLEGAL_OPERATION_MODE_ERROR;
	}

	/* Digest key if too long */
	if (keySize > maxK0Len) {
		crysRc = CRYS_HASH(OperationMode,
				   key_ptr,
				   keySize,
				   keyHashResult);

		if (crysRc != CRYS_OK)
			return crysRc;

		/* Get key hash digest length in case key needs to be hashed into K0 */
		switch (OperationMode) {
			case CRYS_HASH_SHA1_mode: hashDigestLen = CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES; break;
			case CRYS_HASH_SHA224_mode: hashDigestLen = CRYS_HASH_SHA224_DIGEST_SIZE_IN_BYTES; break;
			case CRYS_HASH_SHA256_mode: hashDigestLen = CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES; break;
			case CRYS_HASH_SHA384_mode: hashDigestLen = CRYS_HASH_SHA384_DIGEST_SIZE_IN_BYTES; break;
			case CRYS_HASH_SHA512_mode: hashDigestLen = CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES; break;
			default: return CRYS_HMAC_ILLEGAL_OPERATION_MODE_ERROR;
		}

		DX_PAL_MemCopy(macProps->key,keyHashResult,hashDigestLen);
		macProps->key_size = hashDigestLen;
	} else {
		DX_PAL_MemCopy(macProps->key,key_ptr,keySize);
		macProps->key_size = keySize;
	}

	switch (OperationMode) {
		case CRYS_HASH_SHA1_mode:
			macProps->alg_specific.hmac.hash_type = DXDI_HASH_SHA1;
			break;
		case CRYS_HASH_SHA224_mode:
			macProps->alg_specific.hmac.hash_type = DXDI_HASH_SHA224;
			break;
		case CRYS_HASH_SHA256_mode:
			macProps->alg_specific.hmac.hash_type = DXDI_HASH_SHA256;
			break;
		case CRYS_HASH_SHA384_mode:
			macProps->alg_specific.hmac.hash_type = DXDI_HASH_SHA384;
			break;
		case CRYS_HASH_SHA512_mode:
			macProps->alg_specific.hmac.hash_type = DXDI_HASH_SHA512;
			break;
		case CRYS_HASH_MD5_mode:
			macProps->alg_specific.hmac.hash_type = DXDI_HASH_MD5;
			break;
		default:
			return CRYS_HMAC_ILLEGAL_OPERATION_MODE_ERROR;
	}
	macProps->mac_type = DXDI_MAC_HMAC;

	return CRYS_OK;
}
/************************ Public Functions ******************************/

/**
 * This function initializes the HMAC machine on the CRYS level.
 *
 * This function allocates and initializes the HMAC Context .
 * The function receives as input a pointer to store the context handle to HMAC Context.
 *
 * If the key is larger then 64 bytes it performs on it a HASH operation.
 * then the function executed a HASH_init session and processes a HASH update
 * on the Key XOR ipad and stores it on the context.
 *
 * the context.
 *
 * @param[in] ContextID_ptr - a pointer to the HMAC context buffer allocated by the user that
 *                       is used for the HMAC machine operation.
 *
 * @param[in] OperationMode - The operation mode : MD5 or SHA1.
 *
 * @param[in] key_ptr - The pointer to the users key buffer.
 *
 * @oaram[in] keySize - The size of the received key.
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *
 */
CEXPORT_C CRYSError_t CRYS_HMAC_Init(CRYS_HMACUserContext_t*    ContextID_ptr,
				     CRYS_HASH_OperationMode_t  OperationMode,
				     DxUint8_t*                 key_ptr,
				     DxUint16_t                 keySize )
{
	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	CRYSError_t crysRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;
	struct dxdi_mac_props mac_props = {0};


	/*--------------------
	    CODE
	----------------------*/

#ifndef CRYS_NO_HASH_SUPPORT

	/* ............... checking the parameters validity and init parameters................... */
	crysRc = InitHmacParams(ContextID_ptr,
				OperationMode,
				key_ptr,
				keySize,
				&mac_props,
				&contextPtr);
	if (crysRc != CRYS_OK)
		return crysRc;

	diRc =  DxDI_MacInit(contextPtr,
			     &mac_props,
			     &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_HmacDiErr2CrysErr);

#endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_HMAC_Init */

/**
 * This function process a HMAC block of data via the HASH Hardware.
 * The function receives as input an handle to the  HMAC Context , and operated the HASH update on the data
 * below.
 *
 * @param[in] ContextID_ptr - a pointer to the HMAC context buffer allocated by the user that
 *                       is used for the HMAC machine operation.
 *
 * @param DataIn_ptr a pointer to the buffer that stores the data to be
 *                       hashed .
 *
 * @param DataInSize  The size of the data to be hashed in bytes.
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *
 */

CEXPORT_C CRYSError_t CRYS_HMAC_Update(CRYS_HMACUserContext_t*    ContextID_ptr,
				       DxUint8_t*                 DataIn_ptr,
				       DxUint32_t                 DataInSize )
{

	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;
	/*----------------------------------
	    CODE
	-----------------------------------*/

#ifndef CRYS_NO_HASH_SUPPORT

	/* ............... checking the parameters validity ................... */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* if the data size is zero no need to execute an update , return CRYS_OK */
	if (DataInSize == 0)
		return CRYS_OK;

	/* if the users Data In pointer is illegal and the size is not 0 return an error */
	if (DataIn_ptr == DX_NULL)
		return CRYS_HMAC_DATA_IN_POINTER_INVALID_ERROR;

	/* Get pointer within the buffer that can accomodate context without
	    crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);

	if (contextPtr == 0)
		return CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR;

	diRc = DxDI_ProcessDataBlock(contextPtr,
				     DXDI_DATA_TYPE_TEXT,
				     DataIn_ptr,
				     DX_NULL,
				     DataInSize,
				     &errorInfo);


	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_HmacDiErr2CrysErr);

#endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_HMAC_Update */

/**
 * This function finalize the HMAC process of data block.
 * The function receives as input an handle to the HMAC Context , that was initialized before
 * by an CRYS_HMAC_Init function or by CRYS_HMAC_Update function.
 * The function finishes the HASH operation on the ipad and text then
 * executes a new hash operation with the key XOR opad and the previous HASH operation result.
 *
 *  @param[in] ContextID_ptr - a pointer to the HMAC context buffer allocated by the user that
 *                       is used for the HMAC machine operation.
 *
 *  @retval HmacResultBuff a pointer to the target buffer where the
 *                       HMAC result stored in the context is loaded to.
 *
 *  @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 */

CEXPORT_C CRYSError_t CRYS_HMAC_Finish( CRYS_HMACUserContext_t*   ContextID_ptr ,
					CRYS_HASH_Result_t        HmacResultBuff )
{

	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;
	DxUint8_t digestOrMacSize;

	/*------------------------------
	    CODE
	--------------------------------*/

#ifndef CRYS_NO_HASH_SUPPORT

	/* ............... checking the parameters validity ................... */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* if the result buffer DX_NULL return an error */
	if (HmacResultBuff == DX_NULL)
		return CRYS_HMAC_INVALID_RESULT_BUFFER_POINTER_ERROR;

	/* Get pointer within the buffer that can accomodate context without
	   crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);
	if (contextPtr == 0)
		return CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR;


	diRc =       DxDI_ProcessLastDataBlock(contextPtr,
					       DX_NULL,
					       DX_NULL,
					       0,
					       (DxUint8_t*)HmacResultBuff,
					       &digestOrMacSize,
					       &errorInfo);


	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_HmacDiErr2CrysErr);

#endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_HMAC_Finish */

/**
 * @brief This function is a service function that frees the context if the operation has
 *        failed.
 *
 *        The function executes the following major steps:
 *
 *        1. Checks the validity of all of the inputs of the function.
 *           If one of the received parameters is not valid it shall return an error.
 *
 *           The major checkers that are run over the received parameters:
 *           - verifying that the context pointer is not DX_NULL (*ContextID_ptr).
 *        2. Clearing the users context.
 *        3. Exit the handler with the OK code.
 *
 *
 * @param[in] ContextID_ptr - a pointer to the HMAC context buffer allocated by the user that
 *                       is used for the HMAC machine operation. this should be the same context that was
 *                       used on the previous call of this session.
 *
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_hmac_error.h
 */

CEXPORT_C CRYSError_t  CRYS_HMAC_Free(CRYS_HMACUserContext_t* ContextID_ptr )
{
	/* The return error identifier */
	CRYSError_t Error;

	/*--------------------
	    CODE
	-----------------------*/
	Error = CRYS_OK;



#ifndef CRYS_NO_HASH_SUPPORT

	/* ............... checking the parameters validity ................... */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_HMAC_INVALID_USER_CONTEXT_POINTER_ERROR;


	/* .............. clearing the users context .......................... */
	/* -------------------------------------------------------------------- */

	DX_PAL_MemSet( ContextID_ptr , 0 , sizeof(CRYS_HMACUserContext_t) );

	return Error;

#endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_HMAC_Free */

/**
 * This function provide HASH function to process one buffer of data.
 * The function allocates an internal HASH Context , it initializes the
 * HASH Context with the cryptographic attributes that are needed for
 * the HASH block operation ( initialize H's value for the HASH algorithm ).
 * Then the function loads the Hardware with the initializing values and after
 * that process the data block using the hardware to do hash .
 * At the end the function return the message digest of the data buffer .
 *
 *
 * @param[in] OperationMode - The operation mode : MD5 or SHA1.
 *
 * @param[in] key_ptr - The pointer to the users key buffer.
 *
 * @oaram[in] keySize - The size of the received key.
 *
 * @param DataIn_ptr a pointer to the buffer that stores the data to be
 *                       hashed .
 *
 * @param DataInSize  The size of the data to be hashed in bytes.
 *
 * @retval HashResultBuff a pointer to the target buffer where the
 *                      HMAC result stored in the context is loaded to.
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *
 */
CEXPORT_C CRYSError_t CRYS_HMAC  ( CRYS_HASH_OperationMode_t  OperationMode,
				   DxUint8_t*                 key_ptr,
				   DxUint16_t                 keySize,
				   DxUint8_t*                 DataIn_ptr,
				   DxUint32_t                 DataSize,
				   CRYS_HASH_Result_t         HmacResultBuff )
{
	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	CRYSError_t crysRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;
	CRYS_HMACUserContext_t contextID;
	struct dxdi_mac_props mac_props = {0};


	/*-----------------------------
	    CODE
	-------------------------------*/

	/* ............... local initializations .............................. */

	/* if the users Data In pointer is illegal and the size is not 0 return an error */
	if (DataIn_ptr == DX_NULL && DataSize)
		return CRYS_HMAC_DATA_IN_POINTER_INVALID_ERROR;


	/* larger then 2^29 (to prevant an overflow on the transition to bits )
	   return error */
	if (DataSize >= (1 << 29))
		return CRYS_HMAC_DATA_SIZE_ILLEGAL;

	/* if the result buffer DX_NULL return an error */
	if (HmacResultBuff == DX_NULL)
		return CRYS_HMAC_INVALID_RESULT_BUFFER_POINTER_ERROR;

#ifndef CRYS_NO_HASH_SUPPORT
	/* ............... checking the parameters validity and init parameters................... */
	crysRc = InitHmacParams(&contextID,
				OperationMode,
				key_ptr,
				keySize,
				&mac_props,
				&contextPtr);
	if (crysRc != CRYS_OK)
		return crysRc;

	diRc = DxDI_MacProcess(contextPtr,
			       &mac_props,
			       DataIn_ptr,
			       DataSize,
			       (DxUint8_t*) HmacResultBuff,
			       &errorInfo);


	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_HmacDiErr2CrysErr);
#endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_HMAC */

