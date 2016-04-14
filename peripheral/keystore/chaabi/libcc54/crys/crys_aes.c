/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

/************* Include Files ****************/
#include <stdio.h>
#include <unistd.h>
//#include "error.h"
#include "crys_aes_error.h"
#include "crys_aes.h"
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
static CRYSError_t DX_AesDiErr2CrysErr(DxDI_RetCode_t diRc, DxUint32_t errorInfo)
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
			return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;
		case DXDI_ERROR_UNSUP:
			return CRYS_AES_IS_NOT_SUPPORTED;
		case DXDI_ERROR_INVAL_MODE:
			return CRYS_AES_ILLEGAL_OPERATION_MODE_ERROR;
		case DXDI_ERROR_INVAL_DIRECTION:
			return CRYS_AES_ILLEGAL_PARAMS_ERROR;
		case DXDI_ERROR_INVAL_KEY_SIZE:
			return CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
		case DXDI_ERROR_INVAL_DIN_PTR:
			return CRYS_AES_DATA_IN_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DOUT_PTR:
			return CRYS_AES_DATA_OUT_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DATA_SIZE:
			return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
		case DXDI_ERROR_DIN_DOUT_OVERLAP:
			return CRYS_AES_DATA_OUT_DATA_IN_OVERLAP_ERROR;
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
static DxUint32_t GetContextSize(CRYS_AES_OperationMode_t mode)
{
	/* Sizes are cached to avoid IOCTL on each size query */
	/* Uninitialized - to be initialized on first invocation */
	static DxUint32_t aes_ecb_ctx_size = UINT32_MAX;
	static DxUint32_t aes_cbc_ctx_size = UINT32_MAX;
	static DxUint32_t aes_mac_ctx_size = UINT32_MAX;
	static DxUint32_t aes_ctr_ctx_size = UINT32_MAX;
	static DxUint32_t aes_xcbc_mac_ctx_size = UINT32_MAX;
	static DxUint32_t aes_cmac_ctx_size = UINT32_MAX;
	static DxUint32_t aes_xts_ctx_size = UINT32_MAX;

	/* For each mode we check if size is already "cached" */
	/* DxDI is queried only if size is not cached already */
	switch (mode) {
		case CRYS_AES_ECB_mode:
			if (aes_ecb_ctx_size == UINT32_MAX)
				aes_ecb_ctx_size =
				DxDI_GetSymCipherCtxSize(DXDI_SYMCIPHER_AES_ECB);
			return aes_ecb_ctx_size;

		case CRYS_AES_CBC_mode:
			if (aes_cbc_ctx_size == UINT32_MAX)
				aes_cbc_ctx_size =
				DxDI_GetSymCipherCtxSize(DXDI_SYMCIPHER_AES_CBC);
			return aes_cbc_ctx_size;

		case CRYS_AES_MAC_mode:
			if (aes_mac_ctx_size == UINT32_MAX)
				aes_mac_ctx_size =
				DxDI_GetMacCtxSize(DXDI_MAC_AES_MAC);
			return aes_mac_ctx_size;

		case CRYS_AES_CTR_mode:
			if (aes_ctr_ctx_size == UINT32_MAX)
				aes_ctr_ctx_size =
				DxDI_GetSymCipherCtxSize(DXDI_SYMCIPHER_AES_CTR);
			return aes_ctr_ctx_size;

		case CRYS_AES_XCBC_MAC_mode:
			if (aes_xcbc_mac_ctx_size == UINT32_MAX)
				aes_xcbc_mac_ctx_size =
				DxDI_GetMacCtxSize(DXDI_MAC_AES_XCBC_MAC);
			return aes_xcbc_mac_ctx_size;

		case CRYS_AES_CMAC_mode:
			if (aes_cmac_ctx_size == UINT32_MAX)
				aes_cmac_ctx_size =
				DxDI_GetMacCtxSize(DXDI_MAC_AES_CMAC);
			return aes_cmac_ctx_size;

		case CRYS_AES_XTS_mode:
			if (aes_xts_ctx_size == UINT32_MAX)
				aes_xts_ctx_size =
				DxDI_GetSymCipherCtxSize(DXDI_SYMCIPHER_AES_XTS);
			return aes_xts_ctx_size;

		default:
			return 0;
	}
}

static CRYSError_t InitAesParams(CRYS_AESUserContext_t    *ContextID_ptr,
				 CRYS_AES_IvCounter_t     IVCounter_ptr,
				 CRYS_AES_Key_t           Key_ptr,
				 CRYS_AES_KeySize_t       KeySizeID,
				 CRYS_AES_EncryptMode_t   EncryptDecryptFlag,
				 CRYS_AES_OperationMode_t OperationMode,
				 DxUint8_t                *Aes_Props,
				 DxUint32_t               **InternalCnxtPtr)
{
	DxUint32_t realContextSize;
	DxUint32_t   keySizeInBytes;
	struct dxdi_sym_cipher_props *cipher_props = DX_NULL;
	struct dxdi_mac_props *mac_props = DX_NULL;


	/* check if the operation mode is legal */
	if (OperationMode >= CRYS_AES_NumOfModes)
		return  CRYS_AES_ILLEGAL_OPERATION_MODE_ERROR;

	realContextSize = GetContextSize(OperationMode);
	if (realContextSize == 0)
		return CRYS_AES_ILLEGAL_OPERATION_MODE_ERROR;

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* Get pointer within the buffer that can accomodate context without
	   crossing a page */
	*InternalCnxtPtr = DX_InitUserCtxLocation(ContextID_ptr,
						  sizeof(CRYS_AESUserContext_t),
						  realContextSize);
	if (*InternalCnxtPtr == 0)
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;


	/* if the operation mode selected is CBC, MAC, CTR or XTS then check the validity of
	   the IV counter pointer (note: on XTS mode it is the Tweak pointer) */
	if ((OperationMode == CRYS_AES_CBC_mode  ||
	     OperationMode == CRYS_AES_MAC_mode  ||
	     OperationMode == CRYS_AES_CTR_mode  ||
	     OperationMode == CRYS_AES_XTS_mode) &&
	    IVCounter_ptr == DX_NULL) {
		return  CRYS_AES_INVALID_IV_OR_TWEAK_PTR_ERROR;
	}

	/* check the Encrypt / Decrypt flag validity */
	if (EncryptDecryptFlag >= CRYS_AES_EncryptNumOfOptions)
		return  CRYS_AES_INVALID_ENCRYPT_MODE_ERROR;

	/* in MAC,XCBC,CMAC,CTR modes enable only encrypt mode  */
	if ((OperationMode == CRYS_AES_XCBC_MAC_mode || OperationMode == CRYS_AES_CMAC_mode ||
	     OperationMode == CRYS_AES_MAC_mode) &&
	    EncryptDecryptFlag != CRYS_AES_Encrypt)
		return  CRYS_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE;


	switch (KeySizeID) {
		case CRYS_AES_Key128BitSize:
			keySizeInBytes = 16;
			break;
		case CRYS_AES_Key192BitSize:
			keySizeInBytes = 24;
			break;
		case CRYS_AES_Key256BitSize:
			keySizeInBytes = 32;
			break;
		case CRYS_AES_Key512BitSize:
			keySizeInBytes = 64;
			break;
		default:
			return CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
			/* TODO: Secret key support */
	}

	if (OperationMode == CRYS_AES_XTS_mode ||
	    OperationMode == CRYS_AES_ECB_mode ||
	    OperationMode == CRYS_AES_CBC_mode ||
	    OperationMode == CRYS_AES_CTR_mode) {

		cipher_props = (struct dxdi_sym_cipher_props *)Aes_Props;
		cipher_props->direction = EncryptDecryptFlag == CRYS_AES_Encrypt ?
					  DXDI_CDIR_ENC : DXDI_CDIR_DEC;
		cipher_props->key_size = keySizeInBytes;
		DX_PAL_MemCopy(cipher_props->key,Key_ptr,keySizeInBytes);
	} else {
		mac_props = (struct dxdi_mac_props *)Aes_Props;
		mac_props->key_size = keySizeInBytes;
		DX_PAL_MemCopy(mac_props->key,Key_ptr,keySizeInBytes);
	}

	switch (OperationMode) {
		case CRYS_AES_ECB_mode:
			cipher_props->cipher_type = DXDI_SYMCIPHER_AES_ECB;
			break;
		case CRYS_AES_CBC_mode:
			cipher_props->cipher_type = DXDI_SYMCIPHER_AES_CBC;
			DX_PAL_MemCopy(cipher_props->alg_specific.aes_cbc.iv,IVCounter_ptr,CRYS_AES_IV_COUNTER_SIZE_IN_BYTES);
			break;
		case CRYS_AES_MAC_mode:
			mac_props->mac_type = DXDI_MAC_AES_MAC;
			DX_PAL_MemCopy(mac_props->alg_specific.aes_mac.iv,IVCounter_ptr,CRYS_AES_IV_COUNTER_SIZE_IN_BYTES);
			break;
		case CRYS_AES_CTR_mode:
			cipher_props->cipher_type = DXDI_SYMCIPHER_AES_CTR;
			DX_PAL_MemCopy(cipher_props->alg_specific.aes_ctr.cntr,IVCounter_ptr,CRYS_AES_IV_COUNTER_SIZE_IN_BYTES);
			break;
		case CRYS_AES_XCBC_MAC_mode:
			if (KeySizeID != CRYS_AES_Key128BitSize)
				return  CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
			mac_props->mac_type = DXDI_MAC_AES_XCBC_MAC;
			break;
		case CRYS_AES_CMAC_mode:
			mac_props->mac_type = DXDI_MAC_AES_CMAC;
			break;
		case CRYS_AES_XTS_mode:
			if ((KeySizeID != CRYS_AES_Key256BitSize) &&
			    (KeySizeID != CRYS_AES_Key512BitSize))
				return  CRYS_AES_ILLEGAL_KEY_SIZE_ERROR;
			cipher_props->cipher_type = DXDI_SYMCIPHER_AES_XTS;
			DX_PAL_MemCopy(cipher_props->alg_specific.aes_xts.init_tweak,IVCounter_ptr,CRYS_AES_IV_COUNTER_SIZE_IN_BYTES);
			/* CRYS has no information on XTS data unit size.
			   Setting this field to 0 tells the driver to set
			   the data_unit_size based on size of first data unit */
			cipher_props->alg_specific.aes_xts.data_unit_size = 0;
			break;
		default:
			return CRYS_AES_INVALID_ENCRYPT_MODE_ERROR;
	}

	return CRYS_OK;
}

/****************************************************************************************************/

/************************ Public Functions ******************************/


/****************************************************************************************************/
/**
 * @brief This function is used to initialize the AES machine.
 *        In order to operate the AES machine the first function that should be
 *        called is this function.
 *
 *        The actual macros that will be used by the users are:
 *
 *        CRYS_AES_Init - initializing with a key from the user.
 *        CRYS_AES_InitSecretKey - initializing with the secret key.
 *
 *        The function executes the following major steps:
 *
 *        1. Validates all of the inputs of the function. If one of the received
 *           parameters is not valid it shall return an error:
 *
 *           - verifying that the context pointer is not DX_NULL (*ContextID_ptr).
 *           - verifying the pointer of the IV counter is not DX_NULL
 *             if one of the modes that is selected are CBC , MAC or CTR.
 *           - verifying that the pointer to the key buffer is not DX_NULL
 *             if the secret key is not enabled.
 *           - verifying the values of the key size is valid ( 0- 2 ).
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
 *      NOTE: In XCBC and CMAC modes: IVCounter_ptr = DX_NULL, EncryptDecrypt mode = Encrypt.
 *                                    The parameters checking in
 *                                    these modes is performed accordingly.
 *
 * @param[in] ContextID_ptr - A pointer to the AES context buffer that is allocated by the user
 *                            and is used for the AES machine operation.
 * @param[in] IVCounter_ptr - A buffer containing an initial value: IV, Counter or Tweak according
 *                            to operation mode:
 *                            - on ECB, XCBC, CMAC mode this parameter is not used and may be NULL,
 *                            - on CBC and MAC modes it contains the IV value,
 *                            - on CTR mode it contains the init counter,
 *                            - on XTS mode it contains the initial tweak value - 128-bit consecutive number
 *                              of data unit (in little endian).
 * @param[in] Key_ptr    -  A pointer to the user's key buffer (set to NULL for secret key utilization).
 * @param[in] KeySizeID  -  An enum parameter, defines size of used key (128, 192, 256, 512 bits):
 *                          On XCBC mode allowed 128 bit size only, on XTS - 256 or 512 bit, on other modes <= 256 bit.
 * @param[in] EncryptDecryptFlag - A flag specifying whether the AES should perform an Encrypt operation (0)
 *                                 or a Decrypt operation (1). In XCBC and CMAC modes it must be Encrypt.
 * @param[in] OperationMode - The operation mode: ECB, CBC, MAC, CTR, XCBC (PRF and 96), CMAC.
 *
 * @return CRYSError_t - On success the value CRYS_OK is returned, and on failure - a value from crys_aes_error.h
 */

CEXPORT_C CRYSError_t  CRYS_AES_Init( CRYS_AESUserContext_t    *ContextID_ptr,
				     CRYS_AES_IvCounter_t     IVCounter_ptr,
				     CRYS_AES_Key_t           Key_ptr,
				     CRYS_AES_KeySize_t       KeySizeID,
				     CRYS_AES_EncryptMode_t   EncryptDecryptFlag,
				     CRYS_AES_OperationMode_t OperationMode)
{
#ifndef CRYS_NO_AES_SUPPORT
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t   crysRc;
	DxUint32_t *contextPtr;
    struct dxdi_sym_cipher_props aes_cipher_props = {0};
    struct dxdi_mac_props aes_mac_props = {0};

	/* FUNCTION LOGIC */

	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */



	if (OperationMode == CRYS_AES_XTS_mode ||
	    OperationMode == CRYS_AES_ECB_mode ||
	    OperationMode == CRYS_AES_CBC_mode ||
	    OperationMode == CRYS_AES_CTR_mode) {

		crysRc = InitAesParams(ContextID_ptr,
				       IVCounter_ptr,
				       Key_ptr,
				       KeySizeID,
				       EncryptDecryptFlag,
				       OperationMode,
				       (DxUint8_t*)&aes_cipher_props,
				       &contextPtr);
		if (crysRc != CRYS_OK)
			return crysRc;

		/* Initialize context for given symmetric cipher configuration*/
		diRc =  DxDI_SymCipherInit(contextPtr,
					   &aes_cipher_props,
					   &errorInfo);
	} else {
		crysRc = InitAesParams(ContextID_ptr,
				       IVCounter_ptr,
				       Key_ptr,
				       KeySizeID,
				       EncryptDecryptFlag,
				       OperationMode,
				       (DxUint8_t*)&aes_mac_props,
				       &contextPtr);

		if (crysRc != CRYS_OK)
			return crysRc;

		diRc =  DxDI_MacInit(contextPtr,
				     &aes_mac_props,
				     &errorInfo);
	}
	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesDiErr2CrysErr);

#else
	return CRYS_AES_IS_NOT_SUPPORTED

#endif /* !CRYS_NO_AES_SUPPORT */

}/* END OF CRYS_AES_Init */


/****************************************************************************************************/
/**
 * @brief This function is used to operate a block of data on the SW or on AES machine.
 *        This function should be called after the appropriate CRYS AES init function
 *        (according to used AES operation mode).
 *
 *        The function executes the following major steps:
 *
 *        1.Checks the validity of all inputs of the function.
 *          If one of the received parameters is not valid it shall return an error.
 *
 *          The major checkers that are run over the received parameters:
 *          - verifying that the context pointer is not DX_NULL (*ContextID_ptr).
 *          - verifying the pointer of the data_in buffer is not DX_NULL.
 *          - in all modes besides XCBC and CMAC:
 *              verifying that the pointer to the data_out buffer is not DX_NULL,
 *              verifying that the DataInSize is not 0 and is a multiple of 16 bytes.
 *
 *        2.Decrypts the received context to the working context after
 *          capturing the working context by calling the CRYS_CCM_GetContext() call.
 *        3.Executes the AES operation on the software or hardware by calling the
 *          low level AES function LLF_AES_Block.
 *        4.Encrypts the working context and stores it to the users received context.
 *          Releases the working context by calling the CRYS_CCM_ReleaseContext call.
 *        5.Exits the handler with the OK code.
 *
 *
 * @param[in] ContextID_ptr - A pointer to the AES context buffer allocated by the user that
 *                            is used for the AES machine operation. This should be the same context that was
 *                            used on the previous call of this session.
 *
 * @param[in] DataIn_ptr - A pointer to the buffer of the input data to the AES. The pointer does
 *                         not need to be aligned.
 *
 * @param[in] DataInSize - A size of the input data must be multiple of 16 bytes and not 0,
 *                         on all modes. Note last chunk (block) of data must be processed by
 *                         CRYS_AES_Finish function and not by CRYS_AES_Block function;
 *
 * @param[out] DataOut_ptr - A pointer to the buffer of the output data from the AES. The pointer  does not
 *                             need to be aligned.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_aes_error.h
 *
 *
 */
CRYSError_t  CRYS_AES_Block( CRYS_AESUserContext_t	*ContextID_ptr,
			     DxUint8_t			*DataIn_ptr,
			     DxUint32_t			DataInSize,
			     DxUint8_t			*DataOut_ptr )
{
#ifndef CRYS_NO_AES_SUPPORT

	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;


	/* Checking the parameters validity */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* DO NOT validate data size is a block multiple because XTS allows non block multiples */

	/* Size zero is not a valid block operation */
	if (DataInSize == 0)
		return CRYS_AES_DATA_IN_SIZE_ILLEGAL;

	/* No need to validate buffers overlap, it is already done in the driver */

	/* Get pointer within the buffer that can accomodate context without
	    crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);

	if (contextPtr == 0)
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;

	diRc = DxDI_ProcessDataBlock(contextPtr,
				     DXDI_DATA_TYPE_TEXT,
				     DataIn_ptr,
				     DataOut_ptr,
				     DataInSize,
				     &errorInfo);


	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesDiErr2CrysErr);

#else
	return CRYS_AES_IS_NOT_SUPPORTED
#endif

}/* END OF _DX_AES_Block */


/****************************************************************************************************/
/**
 * @brief This function is used as finish operation on all AES modes.
 *
 *        The function must be called after AES_Block operations (or instead) for last chunck
 *        of data with size > 0.
 *
 *        The function performs all operations, including specific operations for last blocks of
 *        data on some modes (XCBC, CMAC, MAC) and puts out the result. After all operations
 *        the function cleanes the secure sensitive data from context.
 *
 *        1. Checks the validation of all of the inputs of the function.
 *           If one of the received parameters is not valid it shall return an error.
 *        2. Decrypts the received context to the working context  by calling the
 *           CRYS_CCM_GetContext function.
 *        3. Calls the LLF_AES_Finish function.
 *        4. Outputs the result and cleans working context.
 *        5. Exits
 *
 *
 * @param[in] ContextID_ptr - A pointer to the AES context buffer allocated by the user that
 *                            should be the same context that was used on the previous call
 *                            of this session.
 * @param[in] DataIn_ptr    - A pointer to the buffer of the input data to the AES. The pointer does
 *                            not need to be aligned. On CSI input mode the pointer must be equal to
 *                            value (0xFFFFFFFC | DataInAlignment).
 * @param[in] DataInSize    - A size of the input data must be:  DataInSize >= minimalSize, where:
 *                            minimalSize =
 *                                  -  1 byte for CTR, XCBC, CMAC mode;
 *                                  - 16 bytes for other modes.
 *                            On ECB,CBC and MAC modes the data size must be also a multiple of 16 bytes.
 * @param[out] DataOut_ptr  - A pointer to the output buffer. The pointer  does not need to be aligned.
 *                            On CSI output mode the pointer must be equal to value
 *                            (0xFFFFFFFC | DataOutAlignment). On some modes (MAC,XCBC,CMAC,XTS)
 *                            CSI output is not allowed. Temporarily is not allowed, that both the
 *                            Input and the output are on CSI mode simultaneously.
 *                            The size of the output buffer must be not less than:
 *                                - 16 bytes for MAC, XCBC, CMAC modes;
 *                                - DataInSize for ECB,CBC,CTR,XTS modes.
 *
 * @return CRYSError_t    - On success CRYS_OK is returned, on failure - a value defined in crys_aes_error.h.
 *
 */


CEXPORT_C CRYSError_t  CRYS_AES_Finish(CRYS_AESUserContext_t   *ContextID_ptr,
				       DxUint8_t               *DataIn_ptr,
				       DxUint32_t               DataInSize,
				       DxUint8_t               *DataOut_ptr )
{
#ifndef CRYS_NO_AES_SUPPORT

	/* FUNCTION DECLARATIONS */

	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	DxUint8_t macSize;
	DxUint32_t *contextPtr;

	/*-----------------------
	   FUNCTION LOGIC
	------------------------*/

	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* No need to validate buffers overlap, it is already done in the driver */

	/* Get pointer within the buffer that can accomodate context without
	   crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);
	if (contextPtr == 0)
		return CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* ............. call the CRYS_AES specific functions ................. */
	/* -------------------------------------------------------------------- */
	diRc =  DxDI_ProcessLastDataBlock(contextPtr,
					  DataIn_ptr,
					  DataOut_ptr,
					  DataInSize,
					  DataOut_ptr,
					  &macSize,
					  &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesDiErr2CrysErr);

#else
	return CRYS_AES_IS_NOT_SUPPORTED
#endif
}/* END OF CRYS_AES_Finish */

/****************************************************************************************************/
/**
 * @brief This function is used to operate the AES machine in one integrated operation.
 *
 *        The actual macros that will be used by the users are:
 *
 *        CRYS_AES_ - Operating the integrated AES with a key from the user.
 *
 * @param[in,out] IVCounter_ptr - this parameter is the buffer of the IV or counters on mode CTR.
 *                          In ECB mode this parameter has no use.
 *                          In CBC and MAC modes this parameter should containe the IV values.
 *                          in CTR mode this value shuld contain the init counter.
 *
 * @param[in] Key_ptr - a pointer to the users key buffer.
 *
 * @param[in] KeySize - The size of the KEY used on the AES : 128 , 196 or 256 bits as defined in the enum.
 *
 * @param[in] EncryptDecryptFlag - This flag determains if the AES shall perform an Encrypt operation [0] or a
 *                           Decrypt operation [1].
 *
 * @param[in] OperationMode - The operation mode : ECB , CBC , MAC or CTR.
 *
 * @param[in] DataIn_ptr - The pointer to the buffer of the input data to the AES.
 *                   The pointer's value does not need to be word-aligned.
 *
 * @param[in] DataInSize -  Size of the input data in bytes.
 *                          DataInSize must to be > 0 on all modes excluding AES-CMAC.
 *                          In addition:
 *	                        - on ECB,CBC,MAC modes - the size is a multiple of 16 bytes;
 *	                        - on XTS - the size must be not less than 16 bytes.
 *
 * @param[in,out] DataOut_ptr - The pointer to the buffer of the output data from the AES.
 *                        The pointer's value does not need to be word-aligned.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* crys_aes_error.h
 */
CEXPORT_C CRYSError_t  CRYS_AES( CRYS_AES_IvCounter_t       IVCounter_ptr,
				CRYS_AES_Key_t             Key_ptr,
				CRYS_AES_KeySize_t         KeySize,
				CRYS_AES_EncryptMode_t     EncryptDecryptFlag,
				CRYS_AES_OperationMode_t   OperationMode ,
				DxUint8_t                  *DataIn_ptr,
				DxUint32_t                 DataInSize,
				DxUint8_t                  *DataOut_ptr )
{
#ifndef CRYS_NO_AES_SUPPORT

	/* FUNCTION DECLARATIONS */

	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t   crysRc;
	CRYS_AESUserContext_t ContextID;
	DxUint32_t *contextPtr;

    struct dxdi_sym_cipher_props aes_cipher_props = {0};
    struct dxdi_mac_props aes_mac_props = {0};

	/*-----------------------
	   FUNCTION LOGIC
	------------------------*/

	/* check, that data size is multiple of 16 bytes on relevant modes */
	if (((DataInSize % CRYS_AES_BLOCK_SIZE_IN_BYTES) != 0) &&
	    ((OperationMode == CRYS_AES_ECB_mode) ||
	     (OperationMode == CRYS_AES_CBC_mode) ||
	     (OperationMode == CRYS_AES_MAC_mode))) {
		return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
	}


	/* check the minimum data size according to mode */
	if ((OperationMode == CRYS_AES_XTS_mode) && (DataInSize < 2*CRYS_AES_BLOCK_SIZE_IN_BYTES)) {
			return CRYS_AES_DATA_IN_SIZE_ILLEGAL;
	}

	/* No need to validate buffers overlap, it is already done in the driver */

	if (OperationMode == CRYS_AES_XTS_mode ||
	    OperationMode == CRYS_AES_ECB_mode ||
	    OperationMode == CRYS_AES_CBC_mode ||
	    OperationMode == CRYS_AES_CTR_mode) {
		crysRc = InitAesParams(&ContextID,
				       IVCounter_ptr,
				       Key_ptr,
				       KeySize,
				       EncryptDecryptFlag,
				       OperationMode,
				       (DxUint8_t*)&aes_cipher_props,
				       &contextPtr);

		if (crysRc != CRYS_OK)
			return crysRc;

		/* Initialize context for given symmetric cipher configuration */
		diRc =  DxDI_SymCipherProcess(contextPtr,
					      &aes_cipher_props,
					      DataIn_ptr,
					      DataOut_ptr,
					      DataInSize,
					      &errorInfo);
	} else {
		crysRc = InitAesParams(&ContextID,
				       IVCounter_ptr,
				       Key_ptr,
				       KeySize,
				       EncryptDecryptFlag,
				       OperationMode,
				       (DxUint8_t*)&aes_mac_props,
				       &contextPtr);

		if (crysRc != CRYS_OK)
			return crysRc;
		diRc =  DxDI_MacProcess(contextPtr,
					&aes_mac_props,
					DataIn_ptr,
					DataInSize,
					DataOut_ptr,
					&errorInfo);
	}

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesDiErr2CrysErr);

#else
	return CRYS_AES_IS_NOT_SUPPORTED
#endif
}/* END OF _DX_AES */

/**************************************************************************
 *	              CRYS_AES_SetIv function                                *
 **************************************************************************/
/**
   @brief  The CRYS_AES_SetIv function puts a new initial vector into
   an existing context.

   Inputs:  New IV vector

   Outputs: Result

   @param[in/out] ContextID_ptr - A pointer to the AES context buffer that is allocated by the user
                                  and is used for the AES machine operation.

   @return CRYSError_t - CRYS_OK, or error message
			 CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR

*/
CIMPORT_C  CRYSError_t CRYS_AES_SetIv(CRYS_AESUserContext_t   *ContextID_ptr,
				                       DxUint8_t               *iv_ptr)
{
        DxDI_RetCode_t  diRc;
        DxUint32_t *contextPtr = DX_GetUserCtxLocation(ContextID_ptr);

        diRc = DxDI_SetInitialVector(contextPtr,
               iv_ptr);

        return DX_CRYS_RETURN_ERROR(diRc, 0, DX_AesDiErr2CrysErr);
}



/**************************************************************************
 *	              CRYS_AES_GetIv function                                *
 **************************************************************************/
/**
   @brief  The CRYS_AES_GetIv function retrieves the initial vector from
   the context.

   Inputs:  IV vector buffer

   Outputs: Result

   @param[in/out] ContextID_ptr - A pointer to the AES context buffer that is allocated by the user
                                  and is used for the AES machine operation.

   @return CRYSError_t - CRYS_OK, or error message
			 CRYS_AES_INVALID_USER_CONTEXT_POINTER_ERROR

*/
CIMPORT_C  CRYSError_t CRYS_AES_GetIv(CRYS_AESUserContext_t   *ContextID_ptr,
				                       DxUint8_t               *iv_ptr)
{
        DxDI_RetCode_t  diRc;
        DxUint32_t *contextPtr = DX_GetUserCtxLocation(ContextID_ptr);

        diRc = DxDI_GetInitialVector(contextPtr,
               iv_ptr);

        return DX_CRYS_RETURN_ERROR(diRc, 0, DX_AesDiErr2CrysErr);
}



