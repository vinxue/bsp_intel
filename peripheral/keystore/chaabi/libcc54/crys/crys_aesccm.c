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
#include "crys_aesccm_error.h"
#include "crys_aesccm.h"
#include "driver_interface.h"
#include "crys_context_relocation.h"
#include "sep_ctx.h"
#include "dx_pal_mem.h"
#include "dx_bitops.h"


/********************* Private functions  *******************************/

/*!
 * Convert from DxDI return code to CRYS return code
 *
 * \param diRc:		Driver Interface returned error.
 * \param errorInfo:	Additional error information.
 *
 */
static CRYSError_t DX_AesCcmDiErr2CrysErr(DxDI_RetCode_t diRc, DxUint32_t errorInfo)
{
	switch (diRc) {
	case DXDI_RET_OK:
		return CRYS_OK;
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
			return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;
		case DXDI_ERROR_UNSUP:
			return CRYS_AESCCM_IS_NOT_SUPPORTED;
		case DXDI_ERROR_INVAL_DIRECTION:
			return CRYS_AESCCM_INVALID_ENCRYPT_MODE_ERROR;
		case DXDI_ERROR_INVAL_KEY_SIZE:
			return CRYS_AESCCM_ILLEGAL_KEY_SIZE_ERROR;
		case DXDI_ERROR_INVAL_DIN_PTR:
			return CRYS_AESCCM_DATA_IN_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DOUT_PTR:
			return CRYS_AESCCM_DATA_OUT_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DATA_SIZE:
			return CRYS_AESCCM_DATA_IN_SIZE_ILLEGAL;
		case DXDI_ERROR_DIN_DOUT_OVERLAP:
			return CRYS_AESCCM_DATA_OUT_DATA_IN_OVERLAP_ERROR;
		case DXDI_ERROR_INVAL_NONCE_SIZE:
			return CRYS_AESCCM_ILLEGAL_NONCE_SIZE_ERROR;
		case DXDI_ERROR_INVAL_TAG_SIZE:
			return CRYS_AESCCM_ILLEGAL_TAG_SIZE_ERROR;
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

static CRYSError_t DataPtrValidation(DxUint8_t *DataIn_ptr,
				DxUint32_t DataInSize,
				DxUint8_t *DataOut_ptr)
{
	if (((DataIn_ptr < DataOut_ptr) &&
	    ((DataIn_ptr + DataInSize) > DataOut_ptr)) ||
	    ((DataOut_ptr < DataIn_ptr) &&
	    ((DataOut_ptr + DataInSize) > DataIn_ptr))) {
		return CRYS_AESCCM_DATA_OUT_DATA_IN_OVERLAP_ERROR;
	}

	return CRYS_OK;
}

static CRYSError_t  InitAesCcmParams(
	CRYS_AESCCM_UserContext_t   *ContextID_ptr,
	CRYS_AES_EncryptMode_t       EncrDecrMode,
	CRYS_AESCCM_Key_t            CCM_Key,
	CRYS_AESCCM_KeySize_t        KeySizeId,
	DxUint32_t                   AdataSize,
	DxUint32_t                   TextSize,
	DxUint8_t                    *N_ptr,
	DxUint8_t                    SizeOfN,
	DxUint8_t                    SizeOfT,
	struct dxdi_auth_enc_props   *AesCcmProps,
	DxUint32_t		     **InternalCnxtPtr)
{
	DxUint32_t realContextSize;
	DxUint32_t keySizeInBytes;
	DxUint8_t QFieldSize = 15 - SizeOfN;

	/* -------------------------------------------------------------------- */
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */

	/* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL )
	   return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* Check the Encrypt / Decrypt mode */
	if( EncrDecrMode >= CRYS_AES_EncryptNumOfOptions )
		return  CRYS_AESCCM_INVALID_ENCRYPT_MODE_ERROR;

	/* check key pointer */
	if ( CCM_Key == DX_NULL ) {
		return CRYS_AESCCM_ILLEGAL_PARAMETER_PTR_ERROR;
	}
	/* check Nonce pointer */
	if( N_ptr == DX_NULL )
	   return  CRYS_AESCCM_ILLEGAL_PARAMETER_PTR_ERROR;

	/* check the Q field size: according to our implementation QFieldSize <= 4*/
	if( QFieldSize < 2 || QFieldSize > 8 )
		return  CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR;

	/* Check that TextSize fits into Q field (i.e., there are enough bits) */
	if ((BITMASK(QFieldSize * 8) & TextSize) != TextSize)
		return CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR;

	/* check Nonce size. Note: QFieldSize + SizeOfN <= 15 */
	if( SizeOfN < 7 || SizeOfN > 15 - QFieldSize )
	   return  CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR;

	 /* check CCM MAC size: [4,6,8,10,12,14,16] */
	if( SizeOfT < 4   || SizeOfT > 16  || (SizeOfT & 1) != 0 )
		return  CRYS_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR;

	/* check Key size ID */
	if( KeySizeId > CRYS_AES_Key256BitSize )
		return  CRYS_AESCCM_ILLEGAL_KEY_SIZE_ERROR;

	 /* get context size */
	 realContextSize = DxDI_GetAuthEncCtxSize(DXDI_AUTHENC_AES_CCM);
	 if (realContextSize == 0)
		 return CRYS_AESCCM_USER_CONTEXT_CORRUPTED_ERROR;

	 /* Get pointer within the buffer that can accomodate context without
		crossing a page */
	 *InternalCnxtPtr = DX_InitUserCtxLocation(ContextID_ptr,
						   sizeof(CRYS_AESCCM_UserContext_t),
						   realContextSize);
	 if (*InternalCnxtPtr == 0)
		 return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	 switch (KeySizeId) {
	 case CRYS_AES_Key128BitSize:
		 keySizeInBytes = 16;
		 break;
	 case CRYS_AES_Key192BitSize:
		 keySizeInBytes = 24;
		 break;
	 case CRYS_AES_Key256BitSize:
		 keySizeInBytes = 32;
		 break;
	 default:
		 return CRYS_AESCCM_ILLEGAL_KEY_SIZE_ERROR;
	 }

	 /* init user params */
	 DX_PAL_MemSetZero(AesCcmProps, sizeof(struct dxdi_auth_enc_props));
	 AesCcmProps->direction = (EncrDecrMode == CRYS_AESCCM_Encrypt) ?
				 DXDI_CDIR_ENC : DXDI_CDIR_DEC;
	 AesCcmProps->key_size = keySizeInBytes;
	 DX_PAL_MemCopy(AesCcmProps->key, CCM_Key, keySizeInBytes);
	 AesCcmProps->ae_type = DXDI_AUTHENC_AES_CCM;
	 AesCcmProps->adata_size = AdataSize;
	 AesCcmProps->nonce_size = SizeOfN;
	 DX_PAL_MemCopy(AesCcmProps->nonce, N_ptr, SizeOfN);
	 AesCcmProps->tag_size = SizeOfT;
	 AesCcmProps->text_size = TextSize;

	 return CRYS_OK;
}

/************************ Public Functions ******************************/


/**
* @brief This function transfers the AESCCM_init function parameters from HOST to
*        SEP and backwards for operating AESCCM_init.
*
* @param[in] ContextID_ptr - A pointer to the AESCCM context buffer, that is allocated by the user
*                            and is used for the AESCCM operations.
* @param[in] EncrDecrMode  - Enumerator variable defining operation mode (0 - encrypt; 1 - decrypt).
* @param[in] CCM_Key       - A buffer, containing the AESCCM key passed by user (predefined size 128 bits).
* @param[in] KeySizeID     - An enum parameter, defines size of used key (128, 192, 256).
* @param[in] AdataSize     - Full size of additional data in bytes, which must be processed.
*                            Limitation in our implementation is: AdataSize < 2^32. If Adata is absent,
*                            then AdataSize = 0.
* @param[in] TextSize     - The full size of text data (in bytes), which must be processed by CCM.
*
* @param[in] N_ptr	       - A pointer to Nonce - unique value assigned to all data passed into CCM.
*                            Bytes order - big endian form (MSB is the first).
* @param[in] SizeOfN       - The size of the user passed Nonce (in bytes).
* 			     It is an element of {7,8,9,10,11,12,13}.
* @param[in] SizeOfT	   - Size of AESCCM MAC output T in bytes. Valid values: [4,6,8,10,12,14,16].
*
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure an error according to CRYS_AESCCM_error.h
*
*/
CEXPORT_C CRYSError_t  CRYS_AESCCM_Init(
	CRYS_AESCCM_UserContext_t   *ContextID_ptr,
	CRYS_AES_EncryptMode_t       EncrDecrMode,
	CRYS_AESCCM_Key_t            CCM_Key,
	CRYS_AESCCM_KeySize_t        KeySizeId,
	DxUint32_t                   AdataSize,
	DxUint32_t                   TextSize,
	DxUint8_t                   *N_ptr,
	DxUint8_t                    SizeOfN,
	DxUint8_t                    SizeOfT )
{
#if !(defined CRYS_NO_AESCCM_SUPPORT || defined CRYS_NO_AES_SUPPORT)

	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t   crysRc;
	DxUint32_t *contextPtr;
	struct dxdi_auth_enc_props aesccm_cipher_props;

	/* check & initialize user params */
	crysRc = InitAesCcmParams(ContextID_ptr, EncrDecrMode,
				CCM_Key, KeySizeId, AdataSize,
				TextSize, N_ptr, SizeOfN, SizeOfT,
				&aesccm_cipher_props, &contextPtr);
	if (crysRc != CRYS_OK)
		return crysRc;

	/* Initialize context for given symmetric cipher configuration*/
	diRc = DxDI_AuthEncInit(contextPtr, &aesccm_cipher_props, &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesCcmDiErr2CrysErr);

#else
   return CRYS_AESCCM_IS_NOT_SUPPORTED;
#endif /* !CRYS_NO_AESCCM_SUPPORT */
}

/**
* @brief This function transfers the CRYS_AESCCM_BlockAdata function parameters from HOST to
*        SEP and backwards for operating CRYS_AESCCM_BlockAdata on SEP.
*
* @param[in] ContextID_ptr - A pointer to the AESCCM context buffer allocated by the user that
*                            is used for the AESCCM machine operation. This should be the same
*                            context that was used on the previous call of this session.
* @param[in] DataIn_ptr - A pointer to the buffer of the input additional data.
*                         The pointer does not need to be aligned.
* @param[in] DataInSize   - A size of the additional data in bytes.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure an error according to
*                       CRYS_AESCCM_error.h
*   Restrictions:
*      1. The entire input data MUST pass in a single invocation.
*      2. Size of input data may have any size.
*
*/
CEXPORT_C CRYSError_t  CRYS_AESCCM_BlockAdata(
					CRYS_AESCCM_UserContext_t    *ContextID_ptr,
					DxUint8_t                    *DataIn_ptr,
					DxUint32_t                    DataInSize)
{
#if !(defined CRYS_NO_AESCCM_SUPPORT || defined CRYS_NO_AES_SUPPORT)

	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	DxUint32_t *contextPtr;

	/* -------------------------------------------------------------------- */
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */

	/* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL )
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* if the users Data In pointer is illegal return an error */
	if( DataIn_ptr == DX_NULL )
		return CRYS_AESCCM_DATA_IN_POINTER_INVALID_ERROR;

	/* if the data size is illegal return an error */
	if( DataInSize == 0 )
		return CRYS_AESCCM_DATA_IN_SIZE_ILLEGAL;

	/* Get pointer within the buffer that can accomodate context without
	crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);
	if (contextPtr == 0)
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	diRc = DxDI_ProcessDataBlock(contextPtr,
				DXDI_DATA_TYPE_ADATA,
				DataIn_ptr,
				DX_NULL,
				DataInSize,
				&errorInfo);


	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesCcmDiErr2CrysErr);
#else
	return CRYS_AESCCM_IS_NOT_SUPPORTED;
#endif /* !CRYS_NO_AESCCM_SUPPORT */
}


/**
* @brief This function transfers the CRYS_AESCCM_BlockTextData function parameters from HOST to
*        SEP and backwards for operating CRYS_AESCCM_BlockTextData on SEP.
*
* @param[in] ContextID_ptr - A pointer to the AESCCM context buffer allocated by the user that
*                            is used for the AES machine operation. This should be the same
*                            context that was used on the previous call of this session.
* @param[in] DataIn_ptr - A pointer to the buffer of the input data (plain or cipher text).
*                         The pointer does not need to be aligned.
* @param[in] DataInSize  - A size of the data block in bytes: must be multiple of 16 bytes and not 0.
*                          The block of data must be not a last block, that means:
*                            - on Encrypt mode: DataInSize < CCM_Context->RemainTextSize;
*                            - on Decrypt mode: DataInSize < CCM_Context->RemainTextSize - SizeOfT;
* @param[out] DataOut_ptr - A pointer to the output buffer (cipher or plain text).
*                          The pointer does not need to be aligned.
*                          Size of the output buffer must be not less, than DataInSize.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure a
*                       value MODULE_* CRYS_AESCCM_error.h
*   Notes:
*      1. Overlapping of the in-out buffers is not allowed, excluding the in placement case:
*         DataIn_ptr = DataOut_ptr.
*/
CEXPORT_C CRYSError_t  CRYS_AESCCM_BlockTextData(
						CRYS_AESCCM_UserContext_t    *ContextID_ptr,
						DxUint8_t                    *DataIn_ptr,
						DxUint32_t                    DataInSize,
						DxUint8_t                    *DataOut_ptr )
{
#if !(defined CRYS_NO_AESCCM_SUPPORT || defined CRYS_NO_AES_SUPPORT)

	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t   crysRc;
	DxUint32_t *contextPtr;

	/* ............... first checking the parameters ...................... */
	/* -------------------------------------------------------------------- */


	/* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL )
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* if the users Data In pointer is illegal return an error */
	if( DataIn_ptr == DX_NULL )
		return CRYS_AESCCM_DATA_IN_POINTER_INVALID_ERROR;

	/* if the Data In size is 0, return an error */
	if( DataInSize == 0 )
		return CRYS_AESCCM_DATA_IN_SIZE_ILLEGAL;

	/* if the users Data Out pointer is illegal return an error */
	if( DataOut_ptr == DX_NULL )
		return CRYS_AESCCM_DATA_OUT_POINTER_INVALID_ERROR;

	crysRc = DataPtrValidation(DataIn_ptr, DataInSize, DataOut_ptr);
	if (crysRc != CRYS_OK)
		return crysRc;

	/* Get pointer within the buffer that can accomodate context without
		crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);

	if (contextPtr == 0)
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	diRc = DxDI_ProcessDataBlock(contextPtr,
				 DXDI_DATA_TYPE_TEXT,
				 DataIn_ptr,
				 DataOut_ptr,
				 DataInSize,
				 &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesCcmDiErr2CrysErr);
#else
	return CRYS_AESCCM_IS_NOT_SUPPORTED;
#endif /* !CRYS_NO_AESCCM_SUPPORT */
}

/**
* @brief This function transfers the CRYS_AESCCM_BlockLastTextData function parameters from HOST to
*        SEP and backwards for operating CRYS_AESCCM_BlockLastTextData on SEP.
*
* @param[in] ContextID_ptr - A pointer to the AESCCM context buffer, allocated by the user,
*                          that is used for the AESCCM operations. This should be the same
*                          context that was used on the previous call of this session.
* @param[in] DataIn_ptr  - A pointer to the buffer of the input data (plain or cipher text).
*                          The pointer does not need to be aligned.
* @param[in] DataInSize  - A size of the data block in bytes. The size must be equal to remaining
*                          size value, saved in the context.
* @param[in] DataOut_ptr - A pointer to the output buffer (cipher or plain text). If
*                          user passes DataInSize 0 bytes the DataOut_ptr may be equal to NULL.
*                          The pointer does not need to be aligned.
* @param[in] MacRes -   A pointer to the Mac buffer.
* @param[out] SizeOfT - size of MAC in bytes as defined in CRYS_AESCCM_Init function.
*
* @return CRYSError_t - On success CRYS_OK is returned, on failure a
*                       value MODULE_* CRYS_AESCCM_error.h
*   Notes:
*      1. Overlapping of the in-out buffers is not allowed, excluding the in placement case:
*         DataIn_ptr = DataOut_ptr.
*/
CEXPORT_C CRYSError_t  CRYS_AESCCM_Finish(CRYS_AESCCM_UserContext_t    *ContextID_ptr,
					  DxUint8_t                    *DataIn_ptr,
					  DxUint32_t                    DataInSize,
					  DxUint8_t                    *DataOut_ptr,
					  CRYS_AESCCM_Mac_Res_t         MacRes,
					  DxUint8_t                    *SizeOfT)
{

#if !(defined CRYS_NO_AESCCM_SUPPORT || defined CRYS_NO_AES_SUPPORT)

	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t   crysRc;
	DxUint32_t *contextPtr;

	/* -------------------------------------------------------------------- */
	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */

	/* if the users context ID pointer is DX_NULL return an error */
	if( ContextID_ptr == DX_NULL )
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* if the users Data In pointer is illegal return an error */
	if( DataIn_ptr == DX_NULL && DataInSize != 0)
		return CRYS_AESCCM_DATA_IN_POINTER_INVALID_ERROR;

	/* if the users Data Out pointer is illegal return an error */
	if( DataOut_ptr == DX_NULL && DataInSize != 0)
		return CRYS_AESCCM_DATA_OUT_POINTER_INVALID_ERROR;


	/* ............... checking the parameters validity ................... */
	/* -------------------------------------------------------------------- */

	/* if the users context ID pointer is DX_NULL return an error */
	if (ContextID_ptr == DX_NULL)
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* check overlapping pointers */
	crysRc = DataPtrValidation(DataIn_ptr, DataInSize, DataOut_ptr);
	if (crysRc != CRYS_OK)
		return crysRc;

	/* Get pointer within the buffer that can accomodate context without
	crossing a page */
	contextPtr = DX_GetUserCtxLocation(ContextID_ptr);
	if (contextPtr == 0)
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	/* ............. call the CRYS_AES specific functions ................. */
	/* -------------------------------------------------------------------- */
	diRc =  DxDI_ProcessLastDataBlock(contextPtr,
					  DataIn_ptr,
					  DataOut_ptr,
					  DataInSize,
					  MacRes,
					  SizeOfT,
					  &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesCcmDiErr2CrysErr);
#else
	return CRYS_AESCCM_IS_NOT_SUPPORTED;
#endif /* !CRYS_NO_AESCCM_SUPPORT */
}

/**
 * @brief This function is used to perform the AES_CCM operation in one integrated process.
 *
 *        The function preforms CCM algorithm according to NIST 800-38C by call the CRYS_CCM
 *        Init, Block and Finish functions.
 *
 *        The input-output parameters of the function are the following:
 *
 * @param[in] EncrDecrMode  - Enumerator variable defining operation mode (0 - encrypt; 1 - decrypt).
 * @param[in] CCM_Key       - A buffer, containing the AESCCM key passed by user (predefined size 128 bits).
 * @param[in] KeySizeId     - An ID of AESCCM key size (according to 128, 192, or 256 bits size).
 * @param[in] N_ptr	        - A pointer to Nonce - unique value assigned to all data passed into CCM.
 *                            Bytes order - big endian form (MSB is the first).
 * @param[in] SizeOfN       - The size of the user passed Nonce (in bytes).
 * 			      It is an element of {7,8,9,10,11,12,13}.
 * @param[in] ADataIn_ptr    - A pointer to the additional data buffer. The pointer does
 *                             not need to be aligned.
 * @param[in] ADataInSize    - The size of the additional data in bytes;
 * @param[in] TextDataIn_ptr - A pointer to the input text data buffer (plain or cipher according to
 *                             encrypt-decrypt mode). The pointer does not need to be aligned.
 * @param[in] TextDataInSize - The size of the input text data in bytes:
 *                               - on encrypt mode: (2^32 - SizeOfT) > DataInSize >= 0;
 *                               - on Decrypt mode: 2^32 > DataInSize >= SizeOfT (SizeOfT from context).
 * @param[out] TextDataOut_ptr - The output text data pointer (cipher or plain text data).
 *
 * @param[in] SizeOfT	    - Size of AES-CCM MAC output T in bytes. Valid values: [4,6,8,10,12,14,16].
 *
 * @param[in/out] Mac_Res	    -  AES-CCM MAC input/output .
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a value defined in CRYS_AESCCM_error.h
 *
 */
CIMPORT_C CRYSError_t  CRYS_AESCCM(
	CRYS_AES_EncryptMode_t       EncrDecrMode,
	CRYS_AESCCM_Key_t            CCM_Key,
	CRYS_AESCCM_KeySize_t        KeySizeId,
	DxUint8_t                   *N_ptr,
	DxUint8_t                    SizeOfN,
	DxUint8_t                   *ADataIn_ptr,
	DxUint32_t                   ADataInSize,
	DxUint8_t                   *TextDataIn_ptr,
	DxUint32_t                   TextDataInSize,
	DxUint8_t                   *TextDataOut_ptr,
	DxUint8_t                    SizeOfT,
	CRYS_AESCCM_Mac_Res_t        MacRes)
{
#if !(defined CRYS_NO_AESCCM_SUPPORT || defined CRYS_NO_AES_SUPPORT)
	/* The return error identifier */
	DxDI_RetCode_t  diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;
	CRYS_AESCCM_UserContext_t ContextID;
	DxUint32_t *contextPtr;
	struct dxdi_auth_enc_props aesccm_cipher_props;


	/* check overlapping data pointers */
	crysRc = DataPtrValidation(TextDataIn_ptr,
				   TextDataInSize,
				   TextDataOut_ptr);
	if (crysRc != CRYS_OK)
		return crysRc;

	/* check & initialize user params */
	crysRc = InitAesCcmParams(&ContextID, EncrDecrMode,
				CCM_Key, KeySizeId, ADataInSize,
				TextDataInSize, N_ptr, SizeOfN, SizeOfT,
				&aesccm_cipher_props, &contextPtr);
	if (crysRc != CRYS_OK)
		return crysRc;

	/* Get pointer within the buffer that can accomodate context without
	crossing a page */
	contextPtr = DX_GetUserCtxLocation(&ContextID);
	if (contextPtr == 0)
		return CRYS_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR;

	diRc = DxDI_AuthEncProcess(contextPtr,
				&aesccm_cipher_props,
				ADataIn_ptr,
				TextDataIn_ptr,
				TextDataOut_ptr,
				MacRes,
				&errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_AesCcmDiErr2CrysErr);
#else
	return CRYS_AESCCM_IS_NOT_SUPPORTED;
#endif /* !CRYS_NO_AES_SUPPORT || !CRYS_NO_AESCCM_SUPPORT  */
}

