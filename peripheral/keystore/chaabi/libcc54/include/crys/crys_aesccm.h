/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_AESCCM_H
#define CRYS_AESCCM_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "dx_pal_types.h"
#include "crys_error.h"
#include "crys_defs.h"
#include "crys_aes.h"


#ifdef __cplusplus
extern "C"
{
#endif

  /** @file
   *  \brief This file contains all of the enums and definitions that are used for the
   *         CRYS AESCCM APIs, as well as the APIs themselves.
   *
   *  \version CRYS_AESCCM.h#1:incl:13
   *  \author Levin R.
   */

/************************ Defines ******************************/

#ifdef DX_CC_SEP
#define CRYS_AESCCM_USER_CTX_SIZE_IN_WORDS 33
#else
#define CRYS_AESCCM_USER_CTX_SIZE_IN_WORDS 288
#endif

/**
@brief - a definition describing the low level Engine type ( SW , Hardware , Etc )
*/

/* key and key buffer sizes definitions */
#define CRYS_AESCCM_KeySize_t  CRYS_AES_KeySize_t
#define CRYS_AESCCM_KEY_SIZE_WORDS           8

/* nonce and AESCCM-MAC sizes definitions */
#define CRYS_AESCCM_NONCE_MIN_SIZE_BYTES     7
#define CRYS_AESCCM_NONCE_MAX_SIZE_BYTES    13
#define CRYS_AESCCM_MAC_MIN_SIZE_BYTES       4
#define CRYS_AESCCM_MAC_MAX_SIZE_BYTES      16

/* AES CCM encrypt-decrypt mode */
#define CRYS_AESCCM_Decrypt   CRYS_AES_Decrypt
#define CRYS_AESCCM_Encrypt   CRYS_AES_Encrypt


/************************ Typedefs  ****************************/

/* Defines the AES_CCM key buffer */
typedef DxUint8_t CRYS_AESCCM_Key_t[CRYS_AESCCM_KEY_SIZE_WORDS * sizeof(DxUint32_t)];
typedef DxUint8_t CRYS_AESCCM_Mac_Res_t[CRYS_AES_BLOCK_SIZE_IN_BYTES];


/******************* Context Structure  ***********************/
/* The user's context structure - the argument type that will be passed by the user
   to the APIs called by him */
typedef struct CRYS_AESCCM_UserContext_t
{
/* Allocated buffer must be double the size of actual context
* + 1 word for offset management */
   DxUint32_t  buff[CRYS_AESCCM_USER_CTX_SIZE_IN_WORDS];
}CRYS_AESCCM_UserContext_t;

/************************ Public Variables **********************/


/************************ Public Functions **********************/


/****************************************************************************************************/
/********                       AESCCM  FUNCTIONS                                              ******/
/****************************************************************************************************/

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
CIMPORT_C CRYSError_t  CRYS_AESCCM_Init(
					CRYS_AESCCM_UserContext_t   *ContextID_ptr,
					CRYS_AES_EncryptMode_t       EncrDecrMode,
					CRYS_AESCCM_Key_t            CCM_Key,    /*AESCCM key*/
					CRYS_AESCCM_KeySize_t        KeySizeId,  /* key size ID*/
					DxUint32_t                   AdataSize,  /*full size of additional data*/
					DxUint32_t                   TextSize,  /*full size of text data*/
					DxUint8_t                   *N_ptr,      /*Nonce*/
					DxUint8_t                    SizeOfN,    /*size of N buffer*/
					DxUint8_t                    SizeOfT );  /*size of CCM-MAC (T) */



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
CIMPORT_C CRYSError_t  CRYS_AESCCM_BlockAdata(
					CRYS_AESCCM_UserContext_t    *ContextID_ptr,
					DxUint8_t                    *DataIn_ptr,
					DxUint32_t                    DataInSize );


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
CIMPORT_C CRYSError_t  CRYS_AESCCM_BlockTextData(
						CRYS_AESCCM_UserContext_t    *ContextID_ptr,
						DxUint8_t                    *DataIn_ptr,
						DxUint32_t                    DataInSize,
						DxUint8_t                    *DataOut_ptr );

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
CIMPORT_C CRYSError_t  CRYS_AESCCM_Finish(CRYS_AESCCM_UserContext_t    *ContextID_ptr,
					  DxUint8_t                    *DataIn_ptr,
					  DxUint32_t                    DataInSize,
					  DxUint8_t                    *DataOut_ptr,
					  CRYS_AESCCM_Mac_Res_t         MacRes,
					  DxUint8_t                    *SizeOfT);


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
 * @param[in] N_ptr	    - A pointer to Nonce - unique value assigned to all data passed into CCM.
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
				   CRYS_AES_EncryptMode_t       EncrDecrMode,   /*CCM encrypt-decrypt mode*/
				   CRYS_AESCCM_Key_t            CCM_Key,        /*AES-CCM key*/
				   CRYS_AESCCM_KeySize_t        KeySizeId,      /*Key size ID*/
				   DxUint8_t                   *N_ptr,          /*Nonce*/
				   DxUint8_t                    SizeOfN,        /*size of N buffer*/
				   DxUint8_t                   *ADataIn_ptr,    /*input data pointer*/
				   DxUint32_t                   ADataInSize,    /*input data size*/
				   DxUint8_t                   *TextDataIn_ptr, /*input data pointer*/
				   DxUint32_t                   TextDataInSize, /*input data size*/
				   DxUint8_t                   *TextDataOut_ptr,/*output data pointer*/
				   DxUint8_t                    SizeOfT,      	/*size of CCM-MAC (T) */
				   CRYS_AESCCM_Mac_Res_t	Mac_Res	);


#ifdef __cplusplus
}
#endif

#endif /*#ifndef CRYS_AESCCM_H*/

