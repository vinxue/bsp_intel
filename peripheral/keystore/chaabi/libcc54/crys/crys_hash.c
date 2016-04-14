/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

/** @file
 *  \brief A brief description of this module
 *
 *  \version CRYS_HASH.c#1:csrc:6
 *  \author adams
 */


/************* Include Files ****************/
#include "crys_hash_error.h"
#include "crys_hash.h"

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
static CRYSError_t DX_HashDiErr2CrysErr(DxDI_RetCode_t diRc, DxUint32_t errorInfo)
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
			return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;
		case DXDI_ERROR_UNSUP:
			return CRYS_HASH_IS_NOT_SUPPORTED;
		case DXDI_ERROR_INVAL_MODE:
			return CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR;
		case DXDI_ERROR_INVAL_DIN_PTR:
			return CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DATA_SIZE:
			return CRYS_HASH_DATA_SIZE_ILLEGAL;
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
	static DxUint32_t sha1_ctx_size = UINT32_MAX;
	static DxUint32_t sha224_ctx_size = UINT32_MAX;
	static DxUint32_t sha256_ctx_size = UINT32_MAX;
	static DxUint32_t sha384_ctx_size = UINT32_MAX;
	static DxUint32_t sha512_ctx_size = UINT32_MAX;
	static DxUint32_t md5_ctx_size = UINT32_MAX;

	/* For each mode we check if size is already "cached" */
	/* DxDI is queried only if size is not cached already */
	switch (mode) {
		case CRYS_HASH_SHA1_mode:
			if (sha1_ctx_size == UINT32_MAX)
				sha1_ctx_size = DxDI_GetHashCtxSize(DXDI_HASH_SHA1);
			return sha1_ctx_size;
		case CRYS_HASH_SHA224_mode:
			if (sha224_ctx_size == UINT32_MAX)
				sha224_ctx_size = DxDI_GetHashCtxSize(DXDI_HASH_SHA224);
			return sha224_ctx_size;
		case CRYS_HASH_SHA256_mode:
			if (sha256_ctx_size == UINT32_MAX)
				sha256_ctx_size = DxDI_GetHashCtxSize(DXDI_HASH_SHA256);
			return sha256_ctx_size;
		case CRYS_HASH_SHA384_mode:
			if (sha384_ctx_size == UINT32_MAX)
				sha384_ctx_size = DxDI_GetHashCtxSize(DXDI_HASH_SHA384);
			return sha384_ctx_size;
		case CRYS_HASH_SHA512_mode:
			if (sha512_ctx_size == UINT32_MAX)
				sha512_ctx_size = DxDI_GetHashCtxSize(DXDI_HASH_SHA512);
			return sha512_ctx_size;
		case CRYS_HASH_MD5_mode:
			if (md5_ctx_size == UINT32_MAX)
				md5_ctx_size = DxDI_GetHashCtxSize(DXDI_HASH_MD5);
			return md5_ctx_size;
		default:
			return 0;
	}
}

static CRYSError_t InitHashParams(CRYS_HASHUserContext_t*    ContextID_ptr,
				  CRYS_HASH_OperationMode_t  OperationMode,
				  enum dxdi_hash_type*       HashType,
				  DxUint32_t**               InternalCnxtPtr)
{
	unsigned long realContextSize;

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* check if the operation mode is legal */
	if (OperationMode >= CRYS_HASH_NumOfModes)
		return CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR;

	realContextSize = GetContextSize(OperationMode);
	if (realContextSize == 0)
		return CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR;

	/* Get pointer within the buffer that can accomodate context without
	       crossing a page */
	*InternalCnxtPtr = DX_InitUserCtxLocation(ContextID_ptr,
						  sizeof(*ContextID_ptr),
						  realContextSize);

	if (*InternalCnxtPtr == 0)
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;

	switch (OperationMode) {
		case CRYS_HASH_SHA1_mode:
			*HashType = DXDI_HASH_SHA1;
			break;
		case CRYS_HASH_SHA224_mode:
			*HashType = DXDI_HASH_SHA224;
			break;
		case CRYS_HASH_SHA256_mode:
			*HashType = DXDI_HASH_SHA256;
			break;
		case CRYS_HASH_SHA384_mode:
			*HashType = DXDI_HASH_SHA384;
			break;
		case CRYS_HASH_SHA512_mode:
			*HashType = DXDI_HASH_SHA512;
			break;
		case CRYS_HASH_MD5_mode:
			*HashType = DXDI_HASH_MD5;
			break;
		default:
			return CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR;
	}
	return CRYS_OK;

}
/************************ Public Functions ******************************/

/**
 * This function initializes the HASH machine on the CRYS level.
 *
 * This function allocates and initializes the HASH Context .
 * The function receives as input a pointer to store the context handle to HASH Context ,
 * it initializes the
 * HASH Context with the cryptographic attributes that are needed for
 * the HASH block operation ( initialize H's value for the HASH algorithm ).
 *
 * The function flow:
 *
 * 1) checking the validity of the arguments - returnes an error on an illegal argument case.
 * 2) Aquiring the working context from the CCM manager.
 * 3) Initializing the context with the parameters passed by the user and with the init values
 *    of the HASH.
 * 4) loading the user tag to the context.
 * 5) release the CCM context.
 *
 * @param[in] ContextID_ptr - a pointer to the HASH context buffer allocated by the user that
 *                       is used for the HASH machine operation.
 *
 * @param[in] OperationMode - The operation mode : MD5 or SHA1.
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *
 */
CEXPORT_C CRYSError_t CRYS_HASH_Init(CRYS_HASHUserContext_t*    ContextID_ptr,
				     CRYS_HASH_OperationMode_t  OperationMode)
{
	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;
	DxUint32_t *contextPtr;
	enum dxdi_hash_type hashType;

	/*---------------------------
	    CODE
	-----------------------------*/

#ifndef CRYS_NO_HASH_SUPPORT

	/* ............... checking the parameters validity and init ................... */
	/* -------------------------------------------------------------------- */

	crysRc = InitHashParams(ContextID_ptr,
				OperationMode,
				&hashType,
				&contextPtr);
	if (crysRc != CRYS_OK)
		return crysRc;

	diRc =  DxDI_HashInit(contextPtr,
			      hashType,
			      &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_HashDiErr2CrysErr);

#endif /*!CRYS_NO_HASH_SUPPORT*/

}/* END OF CRYS_HASH_Init */

/**
 * This function process a block of data via the HASH Hardware.
 * The function receives as input an handle to the  HASH Context , that was initialized before
 * by an CRYS_HASH_Init function or by other CRYS_HASH_Update function. \n
 * The function Sets the hardware with the last H's value that where stored in
 * the CRYS HASH context and then process the data block using the hardware
 * and in the end of the process stores in the HASH context the H's value
 * HASH Context with the cryptographic attributes that are needed for
 * the HASH block operation ( initialize H's value for the HASH algorithm ).
 * This function is used in cases not all the data is arrange in one
 * continues buffer . \n
 *
 * The function flow:
 *
 * 1) checking the parameters validty if there is an error the function shall exit with an error code.
 * 2) Aquiring the working context from the CCM manager.
 * 3) If there isnt enouth data in the previous update data buff in the context plus the received data
 *    load it to the context buffer and exit the function.
 * 4) fill the previous update data buffer to contain an entire block.
 * 5) Calling the hardware low level function to execute the update.
 * 6) fill the previous update data buffer with the data not processed at the end of the received data.
 * 7) release the CCM context.
 *
 * @param[in] ContextID_ptr - a pointer to the HASH context buffer allocated by the user that
 *                       is used for the HASH machine operation.
 *
 * @param DataIn_ptr a pointer to the buffer that stores the data to be
 *                       hashed .
 *
 * @param DataInSize  The size of the data to be hashed in bytes.
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *
 */

CEXPORT_C CRYSError_t CRYS_HASH_Update(CRYS_HASHUserContext_t*    ContextID_ptr,
				       DxUint8_t*                 DataIn_ptr,
				       DxUint32_t                 DataInSize )
{
	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;

	/*-----------------------------------
	    CODE
	--------------------------------------*/



#ifndef CRYS_NO_HASH_SUPPORT

	/* ............... checking the parameters validity ................... */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* if the data size is zero return CRYS_OK - we just do not need to process any data */
	if (DataInSize == 0)
		return CRYS_OK;

	/* if the users Data In pointer is illegal and the data size is larger then 0 return an error */
	if (DataIn_ptr == DX_NULL)
		return CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR;

	/* larger then 2^29 (to prevant an overflow on the transition to bits )
	   return error */
	if (DataInSize >= (1 << 29))
		return CRYS_HASH_DATA_SIZE_ILLEGAL;

	/* Get pointer within the buffer that can accomodate context without
	    crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);

	if (contextPtr == 0)
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;

	diRc = DxDI_ProcessDataBlock(contextPtr,
				     DXDI_DATA_TYPE_TEXT,
				     DataIn_ptr,
				     DX_NULL,
				     DataInSize,
				     &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_HashDiErr2CrysErr);

#endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_HASH_Update */

/**
 * This function finalize the hashing process of data block.
 * The function receives as input an handle to the HASH Context , that was initialized before
 * by an CRYS_HASH_Init function or by CRYS_HASH_Update function.
 * The function "adds" an header to the data block as the specific hash standard
 * specifics , then it loads the hardware and reads the final message digest.
 *
 *  the function flow:
 *
 * 1) checking the parameters validty if there is an error the function shall exit with an error code.
 * 2) Aquiring the working context from the CCM manager.
 * 3) Setting the padding buffer to load.
 * 4) Calling the hardware low level function to execute the finish.
 * 5) fill the previous update data buffer with the data not processed at the end of the received data.
 * 6) release the CCM context.
 *
 *  @param[in] ContextID_ptr - a pointer to the HASH context buffer allocated by the user that
 *                       is used for the HASH machine operation.
 *
 *  @retval HashResultBuff a pointer to the target buffer where the
 *                       HASE result stored in the context is loaded to.
 *
 *  @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 */

CEXPORT_C CRYSError_t CRYS_HASH_Finish( CRYS_HASHUserContext_t*   ContextID_ptr ,
					CRYS_HASH_Result_t        HashResultBuff )
{
	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;
	DxUint8_t digestOrMacSize;


	/*---------------------------------
	    CODE
	-----------------------------------*/


#ifndef CRYS_NO_HASH_SUPPORT

	/* ............... checking the parameters validity ................... */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* if the result buffer DX_NULL return an error */
	if (HashResultBuff == DX_NULL)
		return CRYS_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR;

	/* Get pointer within the buffer that can accomodate context without
	    crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);
	if (contextPtr == 0)
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;

	diRc =       DxDI_ProcessLastDataBlock(contextPtr,
					       DX_NULL,
					       DX_NULL,
					       0,
					       (DxUint8_t*)HashResultBuff,
					       &digestOrMacSize,
					       &errorInfo);


	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_HashDiErr2CrysErr);

#endif /* !CRYS_NO_HASH_SUPPORT*/

}/* END OF CRYS_HASH_Finish */

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
 * @param[in] ContextID_ptr - a pointer to the HASH context buffer allocated by the user that
 *                       is used for the HASH machine operation. this should be the same context that was
 *                       used on the previous call of this session.
 *
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_hash_error.h
 */
CEXPORT_C CRYSError_t  CRYS_HASH_Free(CRYS_HASHUserContext_t  *ContextID_ptr )
{
	/* The return error identifier */
	CRYSError_t Error;



	/*---------------------------
	    CODE
	-----------------------------*/

	/* ............... local initializations .............................. */

	Error = CRYS_OK;

#ifndef CRYS_NO_HASH_SUPPORT

	/* ............... checking the parameters validity ................... */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* .............. clearing the users context .......................... */
	DX_PAL_MemSet( ContextID_ptr , 0,sizeof(CRYS_HASHUserContext_t) );

	/* ................. end of function ..................................... */
	/* ----------------------------------------------------------------------- */

	return Error;

#endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_HASH_Free */

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
 * @param DataIn_ptr a pointer to the buffer that stores the data to be
 *                       hashed .
 *
 * @param DataInSize  The size of the data to be hashed in bytes.
 *
 * @retval HashResultBuff a pointer to the target buffer where the
 *                      HASE result stored in the context is loaded to.
 *
 * @return CRYSError_t on success the function returns CRYS_OK else non ZERO error.
 *
 */
CEXPORT_C CRYSError_t CRYS_HASH  ( CRYS_HASH_OperationMode_t    OperationMode,
				   DxUint8_t*                   DataIn_ptr,
				   DxUint32_t                   DataSize,
				   CRYS_HASH_Result_t           HashResultBuff )
{

	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;
	DxUint32_t *contextPtr;
	CRYS_HASHUserContext_t ContextID;
	enum dxdi_hash_type hashType;

#ifndef CRYS_NO_HASH_SUPPORT

	/* if the users Data In pointer is illegal and the data size is larger then 0 return an error */
	if ((DataIn_ptr == DX_NULL) &&
	    (DataSize != 0))
		return CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR;

	/* larger then 2^29 (to prevant an overflow on the transition to bits )
	   return error */
	if (DataSize >= (1 << 29))
		return CRYS_HASH_DATA_SIZE_ILLEGAL;

	/* if the result buffer DX_NULL return an error */
	if (HashResultBuff == DX_NULL)
		return CRYS_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR;

	crysRc = InitHashParams(&ContextID,
				OperationMode,
				&hashType,
				&contextPtr);
	if (crysRc != CRYS_OK)
		return crysRc;

	diRc = DxDI_HashProcess(contextPtr,
				hashType,
				DataIn_ptr,
				DataSize,
				(DxUint8_t*)HashResultBuff,
				&errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_HashDiErr2CrysErr);

#endif /* !CRYS_NO_HASH_SUPPORT */

}/* END OF CRYS_HASH */
