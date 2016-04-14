/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

/** @file
 *  \brief A brief description of this module
 *
 *  \version CRYS_DES.c#1:csrc:6
 *  \author adams
 */


/************* Include Files ****************/
//#include "error.h"
#include "crys_des_error.h"
#include "crys_des.h"
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
static CRYSError_t DX_DesDiErr2CrysErr(DxDI_RetCode_t diRc, DxUint32_t errorInfo)
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
			return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;
		case DXDI_ERROR_UNSUP:
			return CRYS_DES_IS_NOT_SUPPORTED;
		case DXDI_ERROR_INVAL_MODE:
			return CRYS_DES_ILLEGAL_OPERATION_MODE_ERROR;
		case DXDI_ERROR_INVAL_DIRECTION:
			return CRYS_DES_INVALID_ENCRYPT_MODE_ERROR;
		case DXDI_ERROR_INVAL_KEY_SIZE:
			return CRYS_DES_ILLEGAL_PARAMS_ERROR;
		case DXDI_ERROR_INVAL_DIN_PTR:
			return CRYS_DES_DATA_IN_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DOUT_PTR:
			return CRYS_DES_DATA_OUT_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DATA_SIZE:
			return CRYS_DES_DATA_SIZE_ILLEGAL;
		case DXDI_ERROR_DIN_DOUT_OVERLAP:
			return CRYS_DES_DATA_OUT_DATA_IN_OVERLAP_ERROR;
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
 * Get the actual context size of given DES mode
 *
 * \param mode
 *
 * \return DxUint32_t Context size in bytes
 */
static DxUint32_t GetContextSize(CRYS_DES_OperationMode_t mode)
{
	/* Sizes are cached to avoid IOCTL on each size query */
	/* Uninitialized - to be initialized on first invocation */
	static DxUint32_t des_ecb_ctx_size = UINT32_MAX;
	static DxUint32_t des_cbc_ctx_size = UINT32_MAX;


	/* For each mode we check if size is already "cached" */
	/* DxDI is queried only if size is not cached already */

	switch (mode) {
		case CRYS_DES_ECB_mode:
			if (des_ecb_ctx_size == UINT32_MAX)
				des_ecb_ctx_size =
				DxDI_GetSymCipherCtxSize(DXDI_SYMCIPHER_DES_ECB);

			return des_ecb_ctx_size;

		case CRYS_DES_CBC_mode:
			if (des_cbc_ctx_size == UINT32_MAX)
				des_cbc_ctx_size =
				DxDI_GetSymCipherCtxSize(DXDI_SYMCIPHER_DES_CBC);

			return des_cbc_ctx_size;

		default:
			return 0;
	}
}

static CRYSError_t  InitDesParams( CRYS_DESUserContext_t*       ContextID_ptr,
				   CRYS_DES_Iv_t             IV_ptr,
				   CRYS_DES_Key_t*           Key_ptr,
				   CRYS_DES_NumOfKeys_t      NumOfKeys,
				   CRYS_DES_EncryptMode_t    EncryptDecryptFlag,
				   CRYS_DES_OperationMode_t  OperationMode,
				   DxUint32_t**              InternalCnxtPtr,
				   struct dxdi_sym_cipher_props*    DesProps)
{
	DxUint32_t realContextSize;

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;


	/* check if the operation mode is legal */
	if (OperationMode >= CRYS_DES_NumOfModes)
		return CRYS_DES_ILLEGAL_OPERATION_MODE_ERROR;


	/* if the operation mode selected is CBC then check the validity of
	       the IV counter pointer */
	if (OperationMode == CRYS_DES_CBC_mode && IV_ptr == DX_NULL)
		return CRYS_DES_INVALID_IV_PTR_ON_NON_ECB_MODE_ERROR;

	/* If the number of keys in invalid return an error */
	if (NumOfKeys >= CRYS_DES_NumOfKeysOptions || NumOfKeys == 0)
		return CRYS_DES_ILLEGAL_NUM_OF_KEYS_ERROR;


	/*check the valisity of the key pointer */
	if (Key_ptr == DX_NULL)
		return CRYS_DES_INVALID_KEY_POINTER_ERROR;


	/* Check the Encrypt / Decrypt flag validity */
	if (EncryptDecryptFlag >= CRYS_DES_EncryptNumOfOptions)
		return CRYS_DES_INVALID_ENCRYPT_MODE_ERROR;

	realContextSize = GetContextSize(OperationMode);
	if (realContextSize == 0)
		return CRYS_DES_ILLEGAL_OPERATION_MODE_ERROR;

	/* Get pointer within the buffer that can accomodate context without
	       crossing a page */
	*InternalCnxtPtr = DX_InitUserCtxLocation(ContextID_ptr,
						  sizeof(CRYS_DESUserContext_t),
						  realContextSize);
	if (*InternalCnxtPtr == 0)
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;

	if (OperationMode == CRYS_DES_ECB_mode)
		DesProps->cipher_type = DXDI_SYMCIPHER_DES_ECB;
	else {
		DesProps->cipher_type = DXDI_SYMCIPHER_DES_CBC;
		DX_PAL_MemCopy(DesProps->alg_specific.des_cbc.iv ,IV_ptr,CRYS_DES_IV_SIZE_IN_BYTES);
	}
	DesProps->direction = EncryptDecryptFlag == CRYS_DES_Encrypt ?
			      DXDI_CDIR_ENC : DXDI_CDIR_DEC;

	DesProps->key_size = CRYS_DES_KEY_SIZE_IN_BYTES*NumOfKeys;
	DX_PAL_MemCopy(DesProps->key ,Key_ptr,CRYS_DES_KEY_SIZE_IN_BYTES*NumOfKeys);

	return CRYS_OK;
}


static CRYSError_t DataValidation(DxUint8_t            *DataIn_ptr,
				  DxUint32_t            DataInSize,
				  DxUint8_t            *DataOut_ptr)
{
	/* if the users Data In pointer is illegal return an error */
	if (DataIn_ptr == DX_NULL)
		return CRYS_DES_DATA_IN_POINTER_INVALID_ERROR;

	/* if the users Data Out pointer is illegal return an error */
	if (DataOut_ptr == DX_NULL)
		return CRYS_DES_DATA_OUT_POINTER_INVALID_ERROR;


	/* if the data size is zero or not a multiple of 16 bytes return error */
	if (DataInSize % CRYS_DES_BLOCK_SIZE_IN_BYTES != 0)
		return CRYS_DES_DATA_SIZE_ILLEGAL;

	/* No need to validate buffers overlap, it is already done in the driver */

	return CRYS_OK;
}

/****************************************************************************************************/


/************************ Public Functions ******************************/

/* ------------------------------------------------------------
 * @brief This function is used to initialize the DES machine.
 *        In order to operate the DES machine the first function that should be
 *        called is this function.
 *
 *        The function executes the following major steps:
 *
 *        1. Validates all of the inputs of the function. If one of the received
 *           parameters is not valid it shall return an error:
 *
 *           - verifying that the context pointer is not DX_NULL (*ContextID_ptr).
 *           - verifying the pointer of the IV counter is not DX_NULL
 *             if the modes that is selected are CBC.
 *           - verifying that the pointer to the key buffer is not DX_NULL
 *           - verifying the values of the number of keys is valid ( 0- 2 ).
 *           - verifying the value of the operation mode is valid ( 0 - 2 )
 *        2. Decrypting the received context to the working context after capturing
 *           the working context by calling the CRYS_CCM_GetContext() call.
 *
 *        3. Initializing the working context by the following:
 *           - loading the keys.
 *           - loading the IV counters.
 *           - loading the control field
 *           - loading the operation mode.
 *           - loading the DecryptEncrypt flag and the key size.
 *           - loading the key size.
 *        4. Encrypting the information in the working context and storing
 *           it to the users received context. After then the working context is released.
 *           This state is operated by calling the CRYS_CCM_EncryptAndReleaseContext call.
 *        5. Exit the handler with the OK code.
 *
 *
 *
 * @param[in] ContextID_ptr - a pointer to the DES context buffer allocated by the user that
 *                       is used for the DES machine operation.
 *
 * @param[in,out] IV_ptr - this parameter is the buffer of the IV or counters on mode CTR.
 *                          On ECB mode this parameter has no use.
 *                          On CBC this parameter should containe the IV values.
 *
 * @param[in] Key_ptr -  a pointer to the users key buffer.
 *
 * @param[in] NumOfKeys - the number of keys used by the module ( 1 - 3 )
 *
 * @param[in] EncryptDecryptFlag - This flag determains if the DES shall perform an Encrypt operation [0] or a
 *                           Decrypt operation [1].
 *
 * @param[in] OperationMode - The operation mode : ECB or CBC.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_des_error.h
 */

CEXPORT_C CRYSError_t  CRYS_DES_Init( CRYS_DESUserContext_t*    ContextID_ptr,
				      CRYS_DES_Iv_t             IV_ptr,
				      CRYS_DES_Key_t*           Key_ptr,
				      CRYS_DES_NumOfKeys_t      NumOfKeys,
				      CRYS_DES_EncryptMode_t    EncryptDecryptFlag,
				      CRYS_DES_OperationMode_t  OperationMode )
{
	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;
	DxUint32_t *contextPtr;
	struct dxdi_sym_cipher_props des_cipher_props;

	/*-----------------------------
	    CODE
	--------------------------------*/

#ifndef CRYS_NO_DES_SUPPORT

	/* ............... checking the parameters validity and init des params................... */

	crysRc = InitDesParams(ContextID_ptr,
			       IV_ptr,
			       Key_ptr,
			       NumOfKeys,
			       EncryptDecryptFlag,
			       OperationMode,
			       &contextPtr,
			       &des_cipher_props);
	if (crysRc != CRYS_OK)
		return crysRc;


	/* Initialize context for given symmetric cipher configuration*/
	diRc =  DxDI_SymCipherInit(contextPtr,
				   &des_cipher_props,
				   &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_DesDiErr2CrysErr);

#endif /* CRYS_NO_DES_SUPPORT */

}/* END OF DES_Init */

/** ------------------------------------------------------------
 * @brief This function is used to operate a block on the DES machine.
 *        This function should be called after the CRYS_DES_Init function
 *        was called.
 *
 *        The function executes the following major steps:
 *
 *        1.Checks the validation of all of the inputs of the function.
 *          If one of the received parameters is not valid it shall return an error.
 *
 *          The major checkers that are run over the received parameters:
 *          - verifying that the context pointer is not DX_NULL (*ContextID_ptr).
 *          - verifying the pointer of the data_in buffer is not DX_NULL.
 *          - verifying that the pointer to the data_out buffer is not DX_NULL.
 *          - verifying the values of the data_in buffers size is not 0 and a multiple of 16 bytes.
 *
 *        2.Decrypting the received context to the working context after
 *          capturing the working context by calling the CRYS_CCM_GetContext() call.
 *
 *        3.executing the DES operation on the hardware by calling the
 *          low level DES function LLF_DES_Block.
 *        4.Encrypting the information in the working context and storing it
 *          to the users received context. After then the working context is released.
 *          This state is operated by calling the CRYS_CCM_ReleaseContext call.
 *        5.Exit the handler with the OK code.
 *
 *
 * @param[in] ContextID_ptr - a pointer to the DES context buffer allocated by the user that
 *                       is used for the DES machine operation. this should be the same context that was
 *                       used on the previous call of this session.
 *
 *
 * @param[in] DataIn_ptr - The pointer to the buffer of the input data to the DES. The pointer does
 *                   not need to be aligned.
 *
 * @param[in] DataInSize - The size of the input data(must be not 0 and must be multiple of 8 bytes).
 *
 * @param[in/out] DataOut_ptr - The pointer to the buffer of the output data from the DES. The pointer does not
 *                        need to be aligned to 32 bits.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_des_error.h
 */
CEXPORT_C CRYSError_t  CRYS_DES_Block( CRYS_DESUserContext_t*    ContextID_ptr,
				       DxUint8_t*                DataIn_ptr,
				       DxUint32_t                DataInSize,
				       DxUint8_t*                DataOut_ptr )
{
	/* The return error identifiers */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;
	DxUint32_t *contextPtr;

	/*-------------------
	  CODE
	----------------------*/


#ifndef CRYS_NO_DES_SUPPORT

	/* ............... checking the parameters validity ................... */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;

	crysRc = DataValidation(DataIn_ptr,
				DataInSize,
				DataOut_ptr);
	if (crysRc != CRYS_OK)
		return crysRc;


	/* Get pointer within the buffer that can accomodate context without
	    crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);

	if (contextPtr == 0)
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;

	diRc = DxDI_ProcessDataBlock(contextPtr,
				     DXDI_DATA_TYPE_TEXT,
				     DataIn_ptr,
				     DataOut_ptr,
				     DataInSize,
				     &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_DesDiErr2CrysErr);

#endif /* CRYS_NO_DES_SUPPORT */

}/* END OF CRYS_DES_Block */

/**
 * @brief This function is used to end the DES operation seesion.
 *        It is the last function called on the DES operation.
 *
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
 * @param[in] ContextID_ptr - a pointer to the DES context buffer allocated by the user that
 *                       is used for the DES machine operation. this should be the same context that was
 *                       used on the previous call of this session.
 *
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_des_error.h
 */
CEXPORT_C CRYSError_t  CRYS_DES_Free(CRYS_DESUserContext_t  *ContextID_ptr )
{


	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;

	/*-----------------------
	    CODE
	--------------------------*/

#ifndef CRYS_NO_DES_SUPPORT

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* Get pointer within the buffer that can accomodate context without
		 crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);
	if (contextPtr == 0)
		return CRYS_DES_INVALID_USER_CONTEXT_POINTER_ERROR;


	/* ............. call the CRYS_AES specific functions ................. */
	/* -------------------------------------------------------------------- */
	diRc =       DxDI_ProcessLastDataBlock(contextPtr,
					       DX_NULL,
					       DX_NULL,
					       0,
					       DX_NULL,
					       0,
					       &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_DesDiErr2CrysErr);

#endif /* CRYS_NO_DES_SUPPORT */

}/* END OF CRYS_DES_Free */

/**
 * @brief This function is used to operate the DES machine in one integrated operation.
 *
 *        The actual macros that will be used by the users are:
 *
 *
 * @param[in,out] IVCounter_ptr - this parameter is the buffer of the IV or counters on mode CTR.
 *                          On ECB mode this parameter has no use.
 *                          On CBC mode this parameter should containe the IV values.
 *
 * @param[in] Key_ptr - a pointer to the users key buffer.
 *
 * @param[in] KeySize - Thenumber of keys used by the DES as defined in the enum.
 *
 * @param[in] EncryptDecryptFlag - This flag determains if the DES shall perform an Encrypt operation [0] or a
 *                           Decrypt operation [1].
 *
 * @param[in] OperationMode - The operation mode : ECB or CBC.
 *
 * @param[in] DataIn_ptr - The pointer to the buffer of the input data to the DES.
 *                   The pointer does not need to be word-aligned.
 *
 * @param[in] DataInSize - The size of the input data (must be not 0 and must be multiple of 8 bytes).
 *
 * @param[in,out] DataOut_ptr - The pointer to the buffer of the output data from the DES.
 *                        The pointer does not need to be word-aligned.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_des_error.h
 */
CEXPORT_C CRYSError_t  CRYS_DES(CRYS_DES_Iv_t            IV_ptr,
				CRYS_DES_Key_t*          Key_ptr,
				CRYS_DES_NumOfKeys_t     NumOfKeys,
				CRYS_DES_EncryptMode_t   EncryptDecryptFlag,
				CRYS_DES_OperationMode_t OperationMode,
				DxUint8_t*               DataIn_ptr,		 /* in */
				DxUint32_t               DataInSize,		  /* in */
				DxUint8_t*               DataOut_ptr )		 /* in / out */
{
	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;
	CRYS_DESUserContext_t    ContextID;
	DxUint32_t *contextPtr;

	struct dxdi_sym_cipher_props des_cipher_props;

	/*--------------------------------
	    CODE
	---------------------------------*/

#ifndef CRYS_NO_DES_SUPPORT

	/* ............... checking the parameters validity and init des params................... */

	crysRc = InitDesParams(&ContextID,
			       IV_ptr,
			       Key_ptr,
			       NumOfKeys,
			       EncryptDecryptFlag,
			       OperationMode,
			       &contextPtr,
			       &des_cipher_props);
	if (crysRc != CRYS_OK)
		return crysRc;

	crysRc = DataValidation(DataIn_ptr,
				DataInSize,
				DataOut_ptr);
	if (crysRc != CRYS_OK)
		return crysRc;

	/* Initialize context for given symmetric cipher configuration*/
	diRc =  DxDI_SymCipherProcess(contextPtr,
				      &des_cipher_props,
				      DataIn_ptr,
				      DataOut_ptr,
				      DataInSize,
				      &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_DesDiErr2CrysErr);

#endif /* CRYS_NO_DES_SUPPORT */

}/* END OF CRYS_DES */

