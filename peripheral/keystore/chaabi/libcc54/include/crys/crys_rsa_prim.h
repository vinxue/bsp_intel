/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_RSA_PRIM_H
#define CRYS_RSA_PRIM_H

#include "crys_rsa_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *  Object name     :  CRYS_RSA_PRIM.h
 */

/** @file
 * \brief This module defines the API that supports PKCS#1 v2.1 primitive functions
 *
 * \version
 * \author ronys
 */


/************************ Defines ******************************/

/**
@brief - A definition describing the low level Engine type (software, hardware, etc.)
*/

#define CRYS_RSA_PRIM_PKI_ENGINE_TYPE  LLF_PKI_ENGINE_TYPE

/**
@brief
CRYS_RSA_PRIM_Encrypt implements the RSAEP algorithm as defined in PKCS#1 v2.1 6.1.1

  @param[in] UserPubKey_ptr - A pointer to the public key data structure
  @param[in] PrimeData_ptr - A pointer to a structure containing internal buffers
  @param[in] Data_ptr - A pointer to the data to encrypt
  @param[in] DataSize - The size, in bytes, of the data to encrypt.
                        \note This value must be <= the size of the modulus.
  @param[out] Output_ptr - Pointer to the encrypted data.
                           The buffer must be at least PubKey_ptr->N.len bytes long
                           (that is, the size of the modulus, in bytes).

  @return CRYSError_t - CRYS_OK,
                        CRYS_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR,
                        CRYS_RSA_PUB_KEY_VALIDATION_TAG_ERROR,
                        CRYS_RSA_PRIM_DATA_STRUCT_POINTER_INVALID,
                        CRYS_RSA_DATA_POINTER_INVALID_ERROR,
                        CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR,
                        CRYS_RSA_INVALID_MESSAGE_BUFFER_SIZE,
                        CRYS_RSA_INVALID_MESSAGE_DATA_SIZE,
                        CRYS_RSA_INVALID_MESSAGE_VAL
 */

CIMPORT_C CRYSError_t CRYS_RSA_PRIM_Encrypt(CRYS_RSAUserPubKey_t *UserPubKey_ptr,
                                  CRYS_RSAPrimeData_t  *PrimeData_ptr,
                                  DxUint8_t              *Data_ptr,
                                  DxUint16_t              DataSize,
                                  DxUint8_t              *Output_ptr);


/**
@brief
CRYS_RSA_PRIM_Decrypt implements the RSADP algorithm as defined in PKCS#1 v2.1 6.1.2

  @param[in] UserPrivKey_ptr - Pointer to the private key data structure.
                           \note The representation (pair or quintuple)
                           and hence the algorithm (CRT or not-CRT)
                           is determined by the Private Key data structure -
                           using CRYS_Build_PrivKey or CRYS_Build_PrivKeyCRT
                           determines which algorithm will be used.

  @param[in] PrimeData_ptr - A pointer to a structure containing internal buffers
                             required for the RSA operation
  @param[in] Data_ptr - A pointer to the data to be decrypted
  @param[in] DataSize - The size, in bytes, of the data to decrypt.
                        \note Must be <= the size of the modulus.

  @param[out] Output_ptr - A pointer to the decrypted data.
                           The buffer must be at least PrivKey_ptr->N.len bytes long
                           (that is, the size of the modulus, in bytes).

  @return CRYSError_t - CRYS_OK,
                        CRYS_RSA_INVALID_PRIV_KEY_STRUCT_POINTER_ERROR,
                        CRYS_RSA_PRIM_DATA_STRUCT_POINTER_INVALID,
                        CRYS_RSA_PRIV_KEY_VALIDATION_TAG_ERROR,
                        CRYS_RSA_DATA_POINTER_INVALID_ERROR,
                        CRYS_RSA_INVALID_OUTPUT_POINTER_ERROR,
                        CRYS_RSA_INVALID_MESSAGE_DATA_SIZE,
                        CRYS_RSA_INVALID_MESSAGE_VAL
 */

CIMPORT_C CRYSError_t CRYS_RSA_PRIM_Decrypt(CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
                                  CRYS_RSAPrimeData_t   *PrimeData_ptr,
                                  DxUint8_t     *Data_ptr,
                                  DxUint16_t     DataSize,
                                  DxUint8_t     *Output_ptr);


/**
   \brief CRYS_RSA_PRIM_Sign implements the RSASP1 algorithm as defined in PKCS#1 v2.1 6.2.1

 \def CRYS_RSA_PRIM_Sign The signature primitive is identical to the decryption algorithm

*/
#define CRYS_RSA_PRIM_Sign CRYS_RSA_PRIM_Decrypt

/**
   \brief CRYS_RSA_PRIM_Verify implements the RSAVP1 algorithm as defined in PKCS#1 v2.1 6.2.2

 \def CRYS_RSA_PRIM_Verify The verification primitive is identical to the encryption algorithm

*/
#define CRYS_RSA_PRIM_Verify CRYS_RSA_PRIM_Encrypt

#ifdef __cplusplus
}
#endif

#endif
