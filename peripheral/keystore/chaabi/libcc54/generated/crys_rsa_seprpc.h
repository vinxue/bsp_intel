/* Generated file - DO NOT EDIT! */
/* Generated by utils/src/sep_rpc_gen/sep_rpc_gen.py@2451 at 2012-09-20 11:56:54.312055 */
/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/* \file crys_rsa_seprpc.h
 * SeP-RPC header file for CrysRsa API */

#ifndef __CRYS_RSA_SEPRPC_H__
#define __CRYS_RSA_SEPRPC_H__

#define SEPRPC_AGENT_ID_CrysRsa 2

#include "crys_host_rpc_config.h"
#include "crys_rsa_error.h"
#include "crys_rsa_prim.h"
#include "crys_rsa_schemes.h"
#include "crys_rsa_build.h"
#include "crys_rsa_kg.h"
#include "crys_rsa_extend.h"
#include "sep_rpc.h"
#include "dx_pal_compiler.h"


/************ CRYS_RSA_PRIM_Encrypt ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_PRIM_Encrypt 1
/*
CRYSError_t CRYS_RSA_PRIM_Encrypt(
	CRYS_RSAUserPubKey_t *UserPubKey_ptr,
	CRYS_RSAPrimeData_t *PrimeData_ptr,
	DxUint8_t Data_ptr[],
	DxUint16_t DataSize,
	DxUint8_t Output_ptr[])
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_Encrypt 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_Encrypt > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_Encrypt is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_PRIM_EncryptParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPubKey_t __UserPubKey_ptr;
	DxUint8_t __UserPubKey_ptr_null_flag;
	DxUint32_t  PrimeData_ptr_size;
	DxUint8_t Data_ptr[4*CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
	DxUint8_t Data_ptr_null_flag;
	/*le16*/DxUint16_t DataSize;
	/* Output */
	DxUint8_t Output_ptr[4*CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
	DxUint8_t Output_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_PRIM_EncryptParams_s;



/************ CRYS_RSA_PRIM_Decrypt ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_PRIM_Decrypt 2
/*
CRYSError_t CRYS_RSA_PRIM_Decrypt(
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSAPrimeData_t *PrimeData_ptr,
	DxUint8_t Data_ptr[],
	DxUint16_t DataSize,
	DxUint8_t Output_ptr[])
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_Decrypt 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_Decrypt > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_Decrypt is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_PRIM_DecryptParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	DxUint32_t  PrimeData_ptr_size;
	DxUint8_t Data_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Data_ptr_null_flag;
	/*le16*/DxUint16_t DataSize;
	/* Output */
	DxUint8_t Output_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Output_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_PRIM_DecryptParams_s;



/************ _DX_RSA_SCHEMES_Encrypt ***************/
#define SEPRPC_FUNC_ID__DX_RSA_SCHEMES_Encrypt 3
/*
CRYSError_t _DX_RSA_SCHEMES_Encrypt(
	CRYS_RSAUserPubKey_t *UserPubKey_ptr,
	CRYS_RSAPrimeData_t *PrimeData_ptr,
	CRYS_RSA_HASH_OpMode_t hashFunc,
	DxUint8_t L[],
	DxUint16_t Llen,
	CRYS_PKCS1_MGF_t MGF,
	DxUint8_t DataIn_ptr[],
	DxUint16_t DataInSize,
	DxUint8_t Output_ptr[],
	CRYS_PKCS1_version PKCS1_ver)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_SCHEMES_Encrypt 0
#if SEPRPC_MEMREF_NUM__DX_RSA_SCHEMES_Encrypt > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_SCHEMES_Encrypt is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc__DX_RSA_SCHEMES_EncryptParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPubKey_t __UserPubKey_ptr;
	DxUint8_t __UserPubKey_ptr_null_flag;
	DxUint32_t  PrimeData_ptr_size;
	/*le32*/CRYS_RSA_HASH_OpMode_t hashFunc;
	DxUint8_t L[2048];
	DxUint8_t L_null_flag;
	/*le16*/DxUint16_t Llen;
	/*le32*/CRYS_PKCS1_MGF_t MGF;
	DxUint8_t DataIn_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t DataIn_ptr_null_flag;
	/*le16*/DxUint16_t DataInSize;
	/*le32*/CRYS_PKCS1_version PKCS1_ver;
	/* Output */
	DxUint8_t Output_ptr[4*CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
	DxUint8_t Output_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_SCHEMES_EncryptParams_s;



/************ _DX_RSA_SCHEMES_Decrypt ***************/
#define SEPRPC_FUNC_ID__DX_RSA_SCHEMES_Decrypt 4
/*
CRYSError_t _DX_RSA_SCHEMES_Decrypt(
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSAPrimeData_t *PrimeData_ptr,
	CRYS_RSA_HASH_OpMode_t hashFunc,
	DxUint8_t L[],
	DxUint16_t Llen,
	CRYS_PKCS1_MGF_t MGF,
	DxUint8_t DataIn_ptr[],
	DxUint16_t DataInSize,
	DxUint8_t Output_ptr[],
	DxUint16_t *OutputSize_ptr,
	CRYS_PKCS1_version PKCS1_ver)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_SCHEMES_Decrypt 0
#if SEPRPC_MEMREF_NUM__DX_RSA_SCHEMES_Decrypt > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_SCHEMES_Decrypt is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc__DX_RSA_SCHEMES_DecryptParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	DxUint32_t  PrimeData_ptr_size;
	/*le32*/CRYS_RSA_HASH_OpMode_t hashFunc;
	DxUint8_t L[2048];
	DxUint8_t L_null_flag;
	/*le16*/DxUint16_t Llen;
	/*le32*/CRYS_PKCS1_MGF_t MGF;
	DxUint8_t DataIn_ptr[4*CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
	DxUint8_t DataIn_ptr_null_flag;
	/*le16*/DxUint16_t DataInSize;
	/*le32*/CRYS_PKCS1_version PKCS1_ver;
	/* Input/Output */
	/*le16*/DxUint16_t __OutputSize_ptr;
	DxUint8_t __OutputSize_ptr_null_flag;
	/* Output */
	DxUint8_t Output_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Output_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_SCHEMES_DecryptParams_s;



/************ _DX_RSA_SignInit ***************/
#define SEPRPC_FUNC_ID__DX_RSA_SignInit 5
/*
CRYSError_t _DX_RSA_SignInit(
	CRYS_RSAPrivUserContext_t *UserContext_ptr,
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSA_HASH_OpMode_t hashFunc,
	CRYS_PKCS1_MGF_t MGF,
	DxUint16_t SaltLen,
	CRYS_PKCS1_version PKCS1_ver)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_SignInit 0
#if SEPRPC_MEMREF_NUM__DX_RSA_SignInit > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_SignInit is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc__DX_RSA_SignInitParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	/*le32*/CRYS_RSA_HASH_OpMode_t hashFunc;
	/*le32*/CRYS_PKCS1_MGF_t MGF;
	/*le16*/DxUint16_t SaltLen;
	/*le32*/CRYS_PKCS1_version PKCS1_ver;
	/* Input/Output */
	CRYS_RSAPrivUserContext_t __UserContext_ptr;
	DxUint8_t __UserContext_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_SignInitParams_s;



/************ _DX_RSA_SignFinish ***************/
#define SEPRPC_FUNC_ID__DX_RSA_SignFinish 6
/*
CRYSError_t _DX_RSA_SignFinish(
	CRYS_RSAPrivUserContext_t *UserContext_ptr,
	DxUint8_t Output_ptr[],
	DxUint16_t *OutputSize_ptr)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_SignFinish 0
#if SEPRPC_MEMREF_NUM__DX_RSA_SignFinish > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_SignFinish is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc__DX_RSA_SignFinishParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input/Output */
	CRYS_RSAPrivUserContext_t __UserContext_ptr;
	DxUint8_t __UserContext_ptr_null_flag;
	/*le16*/DxUint16_t __OutputSize_ptr;
	DxUint8_t __OutputSize_ptr_null_flag;
	/* Output */
	DxUint8_t Output_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Output_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_SignFinishParams_s;



/************ _DX_RSA_SignUpdate ***************/
#define SEPRPC_FUNC_ID__DX_RSA_SignUpdate 7
/*
CRYSError_t _DX_RSA_SignUpdate(
	CRYS_RSAPrivUserContext_t *UserContext_ptr,
	DxUint8_t *DataIn_ptr,
	DxUint32_t DataInSize)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_SignUpdate 1
#if SEPRPC_MEMREF_NUM__DX_RSA_SignUpdate > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_SignUpdate is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif
#define SEPRPC_MEMREF_IDX_DataIn_ptr 0

typedef struct SepRpc__DX_RSA_SignUpdateParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	struct seprpc_memref memref[SEPRPC_MEMREF_NUM__DX_RSA_SignUpdate];
	/* Input */
	/*le32*/DxUint32_t DataInSize;
	/* Input/Output */
	CRYS_RSAPrivUserContext_t __UserContext_ptr;
	DxUint8_t __UserContext_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_SignUpdateParams_s;



/************ _DX_RSA_Sign ***************/
#define SEPRPC_FUNC_ID__DX_RSA_Sign 8
/*
CRYSError_t _DX_RSA_Sign(
	CRYS_RSAPrivUserContext_t *UserContext_ptr,
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSA_HASH_OpMode_t hashFunc,
	CRYS_PKCS1_MGF_t MGF,
	DxUint16_t SaltLen,
	DxUint8_t *DataIn_ptr,
	DxUint32_t DataInSize,
	DxUint8_t Output_ptr[],
	DxUint16_t *OutputSize_ptr,
	CRYS_PKCS1_version PKCS1_ver)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_Sign 1
#if SEPRPC_MEMREF_NUM__DX_RSA_Sign > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_Sign is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif
#define SEPRPC_MEMREF_IDX_DataIn_ptr 0

typedef struct SepRpc__DX_RSA_SignParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	struct seprpc_memref memref[SEPRPC_MEMREF_NUM__DX_RSA_Sign];
	/* Input */
	DxUint32_t  UserContext_ptr_size;
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	/*le32*/CRYS_RSA_HASH_OpMode_t hashFunc;
	/*le32*/CRYS_PKCS1_MGF_t MGF;
	/*le16*/DxUint16_t SaltLen;
	/*le32*/DxUint32_t DataInSize;
	/*le32*/CRYS_PKCS1_version PKCS1_ver;
	/* Input/Output */
	/*le16*/DxUint16_t __OutputSize_ptr;
	DxUint8_t __OutputSize_ptr_null_flag;
	/* Output */
	DxUint8_t Output_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Output_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_SignParams_s;



/************ _DX_RSA_VerifyInit ***************/
#define SEPRPC_FUNC_ID__DX_RSA_VerifyInit 9
/*
CRYSError_t _DX_RSA_VerifyInit(
	CRYS_RSAPubUserContext_t *UserContext_ptr,
	CRYS_RSAUserPubKey_t *UserPubKey_ptr,
	CRYS_RSA_HASH_OpMode_t hashFunc,
	CRYS_PKCS1_MGF_t MGF,
	DxUint16_t SaltLen,
	CRYS_PKCS1_version PKCS1_ver)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_VerifyInit 0
#if SEPRPC_MEMREF_NUM__DX_RSA_VerifyInit > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_VerifyInit is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc__DX_RSA_VerifyInitParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPubKey_t __UserPubKey_ptr;
	DxUint8_t __UserPubKey_ptr_null_flag;
	/*le32*/CRYS_RSA_HASH_OpMode_t hashFunc;
	/*le32*/CRYS_PKCS1_MGF_t MGF;
	/*le16*/DxUint16_t SaltLen;
	/*le32*/CRYS_PKCS1_version PKCS1_ver;
	/* Output */
	CRYS_RSAPubUserContext_t __UserContext_ptr;
	DxUint8_t __UserContext_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_VerifyInitParams_s;



/************ _DX_RSA_VerifyUpdate ***************/
#define SEPRPC_FUNC_ID__DX_RSA_VerifyUpdate 10
/*
CRYSError_t _DX_RSA_VerifyUpdate(
	CRYS_RSAPubUserContext_t *UserContext_ptr,
	DxUint8_t *DataIn_ptr,
	DxUint32_t DataInSize)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_VerifyUpdate 1
#if SEPRPC_MEMREF_NUM__DX_RSA_VerifyUpdate > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_VerifyUpdate is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif
#define SEPRPC_MEMREF_IDX_DataIn_ptr 0

typedef struct SepRpc__DX_RSA_VerifyUpdateParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	struct seprpc_memref memref[SEPRPC_MEMREF_NUM__DX_RSA_VerifyUpdate];
	/* Input */
	/*le32*/DxUint32_t DataInSize;
	/* Input/Output */
	CRYS_RSAPubUserContext_t __UserContext_ptr;
	DxUint8_t __UserContext_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_VerifyUpdateParams_s;



/************ _DX_RSA_VerifyFinish ***************/
#define SEPRPC_FUNC_ID__DX_RSA_VerifyFinish 11
/*
CRYSError_t _DX_RSA_VerifyFinish(
	CRYS_RSAPubUserContext_t *UserContext_ptr,
	DxUint8_t Sig_ptr[])
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_VerifyFinish 0
#if SEPRPC_MEMREF_NUM__DX_RSA_VerifyFinish > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_VerifyFinish is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc__DX_RSA_VerifyFinishParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t Sig_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Sig_ptr_null_flag;
	/* Input/Output */
	CRYS_RSAPubUserContext_t __UserContext_ptr;
	DxUint8_t __UserContext_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_VerifyFinishParams_s;



/************ _DX_RSA_Verify ***************/
#define SEPRPC_FUNC_ID__DX_RSA_Verify 12
/*
CRYSError_t _DX_RSA_Verify(
	CRYS_RSAPubUserContext_t *UserContext_ptr,
	CRYS_RSAUserPubKey_t *UserPubKey_ptr,
	CRYS_RSA_HASH_OpMode_t hashFunc,
	CRYS_PKCS1_MGF_t MGF,
	DxUint16_t SaltLen,
	DxUint8_t *DataIn_ptr,
	DxUint32_t DataInSize,
	DxUint8_t Sig_ptr[],
	CRYS_PKCS1_version PKCS1_ver)
*/

#define SEPRPC_MEMREF_NUM__DX_RSA_Verify 1
#if SEPRPC_MEMREF_NUM__DX_RSA_Verify > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_RSA_Verify is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif
#define SEPRPC_MEMREF_IDX_DataIn_ptr 0

typedef struct SepRpc__DX_RSA_VerifyParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	struct seprpc_memref memref[SEPRPC_MEMREF_NUM__DX_RSA_Verify];
	/* Input */
	DxUint32_t  UserContext_ptr_size;
	CRYS_RSAUserPubKey_t __UserPubKey_ptr;
	DxUint8_t __UserPubKey_ptr_null_flag;
	/*le32*/CRYS_RSA_HASH_OpMode_t hashFunc;
	/*le32*/CRYS_PKCS1_MGF_t MGF;
	/*le16*/DxUint16_t SaltLen;
	/*le32*/DxUint32_t DataInSize;
	DxUint8_t Sig_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Sig_ptr_null_flag;
	/*le32*/CRYS_PKCS1_version PKCS1_ver;
	CRYSError_t _funcRetCode;
} SepRpc__DX_RSA_VerifyParams_s;



/************ CRYS_RSA_Build_PubKey ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_Build_PubKey 13
/*
CRYSError_t CRYS_RSA_Build_PubKey(
	CRYS_RSAUserPubKey_t *UserPubKey_ptr,
	DxUint8_t Exponent_ptr[],
	DxUint16_t ExponentSize,
	DxUint8_t Modulus_ptr[],
	DxUint16_t ModulusSize)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PubKey 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PubKey > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PubKey is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_Build_PubKeyParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t Exponent_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Exponent_ptr_null_flag;
	/*le16*/DxUint16_t ExponentSize;
	DxUint8_t Modulus_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Modulus_ptr_null_flag;
	/*le16*/DxUint16_t ModulusSize;
	/* Output */
	CRYS_RSAUserPubKey_t __UserPubKey_ptr;
	DxUint8_t __UserPubKey_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_Build_PubKeyParams_s;



/************ CRYS_RSA_Build_PrivKey ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_Build_PrivKey 14
/*
CRYSError_t CRYS_RSA_Build_PrivKey(
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	DxUint8_t PrivExponent_ptr[],
	DxUint16_t PrivExponentSize,
	DxUint8_t PubExponent_ptr[],
	DxUint16_t PubExponentSize,
	DxUint8_t Modulus_ptr[],
	DxUint16_t ModulusSize)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PrivKey 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PrivKey > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PrivKey is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_Build_PrivKeyParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t PrivExponent_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t PrivExponent_ptr_null_flag;
	/*le16*/DxUint16_t PrivExponentSize;
	DxUint8_t PubExponent_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t PubExponent_ptr_null_flag;
	/*le16*/DxUint16_t PubExponentSize;
	DxUint8_t Modulus_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Modulus_ptr_null_flag;
	/*le16*/DxUint16_t ModulusSize;
	/* Output */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_Build_PrivKeyParams_s;



/************ CRYS_RSA_Build_PrivKeyCRT ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_Build_PrivKeyCRT 15
/*
CRYSError_t CRYS_RSA_Build_PrivKeyCRT(
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	DxUint8_t P_ptr[],
	DxUint16_t PSize,
	DxUint8_t Q_ptr[],
	DxUint16_t QSize,
	DxUint8_t dP_ptr[],
	DxUint16_t dPSize,
	DxUint8_t dQ_ptr[],
	DxUint16_t dQSize,
	DxUint8_t qInv_ptr[],
	DxUint16_t qInvSize)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PrivKeyCRT 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PrivKeyCRT > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_Build_PrivKeyCRT is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_Build_PrivKeyCRTParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t P_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t P_ptr_null_flag;
	/*le16*/DxUint16_t PSize;
	DxUint8_t Q_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t Q_ptr_null_flag;
	/*le16*/DxUint16_t QSize;
	DxUint8_t dP_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t dP_ptr_null_flag;
	/*le16*/DxUint16_t dPSize;
	DxUint8_t dQ_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t dQ_ptr_null_flag;
	/*le16*/DxUint16_t dQSize;
	DxUint8_t qInv_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t qInv_ptr_null_flag;
	/*le16*/DxUint16_t qInvSize;
	/* Output */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_Build_PrivKeyCRTParams_s;



/************ CRYS_RSA_KG_GenerateKeyPair ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_KG_GenerateKeyPair 16
/*
CRYSError_t CRYS_RSA_KG_GenerateKeyPair(
	DxUint8_t pubExp_ptr[],
	DxUint16_t pubExpSizeInBytes,
	DxUint32_t keySize,
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSAUserPubKey_t *userPubKey_ptr,
	CRYS_RSAKGData_t *keyGenData_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GenerateKeyPair 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GenerateKeyPair > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GenerateKeyPair is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_KG_GenerateKeyPairParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t pubExp_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t pubExp_ptr_null_flag;
	/*le16*/DxUint16_t pubExpSizeInBytes;
	/*le32*/DxUint32_t keySize;
	DxUint32_t  keyGenData_ptr_size;
	/* Output */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	CRYS_RSAUserPubKey_t __userPubKey_ptr;
	DxUint8_t __userPubKey_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_KG_GenerateKeyPairParams_s;



/************ CRYS_RSA_KG_GenerateKeyPairCRT ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_KG_GenerateKeyPairCRT 17
/*
CRYSError_t CRYS_RSA_KG_GenerateKeyPairCRT(
	DxUint8_t pubExp_ptr[],
	DxUint16_t pubExpSizeInBytes,
	DxUint32_t keySize,
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSAUserPubKey_t *userPubKey_ptr,
	CRYS_RSAKGData_t *keyGenData_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GenerateKeyPairCRT 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GenerateKeyPairCRT > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GenerateKeyPairCRT is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_KG_GenerateKeyPairCRTParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t pubExp_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4*2+1];
	DxUint8_t pubExp_ptr_null_flag;
	/*le16*/DxUint16_t pubExpSizeInBytes;
	/*le32*/DxUint32_t keySize;
	DxUint32_t  keyGenData_ptr_size;
	/* Output */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	CRYS_RSAUserPubKey_t __userPubKey_ptr;
	DxUint8_t __userPubKey_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_KG_GenerateKeyPairCRTParams_s;



/************ CRYS_RSA_Get_PubKey ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_Get_PubKey 18
/*
CRYSError_t CRYS_RSA_Get_PubKey(
	CRYS_RSAUserPubKey_t *UserPubKey_ptr,
	DxUint8_t Exponent_ptr[],
	DxUint16_t *ExponentSize_ptr,
	DxUint8_t Modulus_ptr[],
	DxUint16_t *ModulusSize_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_Get_PubKey 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_Get_PubKey > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_Get_PubKey is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_Get_PubKeyParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPubKey_t __UserPubKey_ptr;
	DxUint8_t __UserPubKey_ptr_null_flag;
	/* Input/Output */
	/*le16*/DxUint16_t __ExponentSize_ptr;
	DxUint8_t __ExponentSize_ptr_null_flag;
	/*le16*/DxUint16_t __ModulusSize_ptr;
	DxUint8_t __ModulusSize_ptr_null_flag;
	/* Output */
	DxUint8_t Exponent_ptr[ CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4-1];
	DxUint8_t Exponent_ptr_null_flag;
	DxUint8_t Modulus_ptr[ CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4-1];
	DxUint8_t Modulus_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_Get_PubKeyParams_s;



/************ CRYS_RSA_Get_ModSizeFromPubKey ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_Get_ModSizeFromPubKey 19
/*
CRYSError_t CRYS_RSA_Get_ModSizeFromPubKey(
	CRYS_RSAUserPubKey_t *userPubKey_ptr,
	DxUint16_t *ModulusSize_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_Get_ModSizeFromPubKey 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_Get_ModSizeFromPubKey > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_Get_ModSizeFromPubKey is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_Get_ModSizeFromPubKeyParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPubKey_t __userPubKey_ptr;
	DxUint8_t __userPubKey_ptr_null_flag;
	/* Output */
	/*le16*/DxUint16_t __ModulusSize_ptr;
	DxUint8_t __ModulusSize_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_Get_ModSizeFromPubKeyParams_s;



/************ CRYS_RSA_PRIM_DecryptExactSize ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_PRIM_DecryptExactSize 20
/*
CRYSError_t CRYS_RSA_PRIM_DecryptExactSize(
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSAPrimeData_t *PrimeData_ptr,
	DxUint8_t DataIn_ptr[],
	DxUint8_t Output_ptr[],
	DxUint16_t DataOutSize)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_DecryptExactSize 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_DecryptExactSize > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_PRIM_DecryptExactSize is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_PRIM_DecryptExactSizeParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	DxUint32_t  PrimeData_ptr_size;
	DxUint8_t DataIn_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t DataIn_ptr_null_flag;
	/*le16*/DxUint16_t DataOutSize;
	/* Output */
	DxUint8_t Output_ptr[4*CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
	DxUint8_t Output_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_PRIM_DecryptExactSizeParams_s;



/************ CRYS_RSA_InvModWord ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_InvModWord 21
/*
CRYSError_t CRYS_RSA_InvModWord(
	DxUint32_t aVal,
	DxUint32_t modVal,
	DxUint32_t *invMod_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_InvModWord 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_InvModWord > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_InvModWord is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_InvModWordParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	/*le32*/DxUint32_t aVal;
	/*le32*/DxUint32_t modVal;
	/* Output */
	/*le32*/DxUint32_t __invMod_ptr;
	DxUint8_t __invMod_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_InvModWordParams_s;



/************ CRYS_RSA_KG_GeneratePQprimes ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_KG_GeneratePQprimes 22
/*
CRYSError_t CRYS_RSA_KG_GeneratePQprimes(
	DxUint8_t pubExp_ptr[],
	DxUint16_t pubExpSizeInBytes,
	DxUint32_t primeSizeInBits,
	DxUint32_t testsCount,
	DxUint32_t p_ptr[],
	DxUint32_t q_ptr[],
	CRYS_RSAKGData_t *keyGenData_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GeneratePQprimes 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GeneratePQprimes > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_KG_GeneratePQprimes is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_KG_GeneratePQprimesParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t pubExp_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4*2+1];
	DxUint8_t pubExp_ptr_null_flag;
	/*le16*/DxUint16_t pubExpSizeInBytes;
	/*le32*/DxUint32_t primeSizeInBits;
	/*le32*/DxUint32_t testsCount;
	CRYS_RSAKGData_t __keyGenData_ptr;
	DxUint8_t __keyGenData_ptr_null_flag;
	/* Output */
	/*le32*/DxUint32_t p_ptr[(CRYS_PKA_MAX_KEY_GENERATION_SIZE_BITS/32)/2];
	DxUint8_t p_ptr_null_flag;
	/*le32*/DxUint32_t q_ptr[(CRYS_PKA_MAX_KEY_GENERATION_SIZE_BITS/32)/2];
	DxUint8_t q_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_KG_GeneratePQprimesParams_s;



/************ CRYS_RSA_KG_CalculateKeyPairFromPQ ***************/
#define SEPRPC_FUNC_ID_CRYS_RSA_KG_CalculateKeyPairFromPQ 23
/*
CRYSError_t CRYS_RSA_KG_CalculateKeyPairFromPQ(
	DxUint8_t pubExp_ptr[],
	DxUint16_t pubExpSizeInBytes,
	DxUint32_t p_ptr[],
	DxUint32_t q_ptr[],
	DxUint16_t primeSizeInBits,
	CRYS_RSA_DecryptionMode_t privKeyMode,
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSAUserPubKey_t *userPubKey_ptr,
	CRYS_RSAKGData_t *keyGenData_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_RSA_KG_CalculateKeyPairFromPQ 0
#if SEPRPC_MEMREF_NUM_CRYS_RSA_KG_CalculateKeyPairFromPQ > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_RSA_KG_CalculateKeyPairFromPQ is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_RSA_KG_CalculateKeyPairFromPQParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t pubExp_ptr[ CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t pubExp_ptr_null_flag;
	/*le16*/DxUint16_t pubExpSizeInBytes;
	/*le32*/DxUint32_t p_ptr[(CRYS_PKA_MAX_KEY_GENERATION_SIZE_BITS/32)/2];
	DxUint8_t p_ptr_null_flag;
	/*le32*/DxUint32_t q_ptr[(CRYS_PKA_MAX_KEY_GENERATION_SIZE_BITS/32)/2];
	DxUint8_t q_ptr_null_flag;
	/*le16*/DxUint16_t primeSizeInBits;
	/*le32*/CRYS_RSA_DecryptionMode_t privKeyMode;
	CRYS_RSAKGData_t __keyGenData_ptr;
	DxUint8_t __keyGenData_ptr_null_flag;
	/* Output */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	CRYS_RSAUserPubKey_t __userPubKey_ptr;
	DxUint8_t __userPubKey_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_RSA_KG_CalculateKeyPairFromPQParams_s;



/************ DX_RSA_KG_ExtendGenerateKeyPair ***************/
#define SEPRPC_FUNC_ID_DX_RSA_KG_ExtendGenerateKeyPair 24
/*
CRYSError_t DX_RSA_KG_ExtendGenerateKeyPair(
	DxUint8_t pubExp_ptr[],
	DxUint16_t pubExpSizeInBytes,
	DxUint32_t keySizeBits,
	CRYS_RSA_DecryptionMode_t privKeyMode,
	CRYS_RSAUserPrivKey_t *UserPrivKey_ptr,
	CRYS_RSAUserPubKey_t *userPubKey_ptr,
	CRYS_RSAKGData_t *keyGenData_ptr)
*/

#define SEPRPC_MEMREF_NUM_DX_RSA_KG_ExtendGenerateKeyPair 0
#if SEPRPC_MEMREF_NUM_DX_RSA_KG_ExtendGenerateKeyPair > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_DX_RSA_KG_ExtendGenerateKeyPair is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_DX_RSA_KG_ExtendGenerateKeyPairParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t pubExp_ptr[CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*4];
	DxUint8_t pubExp_ptr_null_flag;
	/*le16*/DxUint16_t pubExpSizeInBytes;
	/*le32*/DxUint32_t keySizeBits;
	/*le32*/CRYS_RSA_DecryptionMode_t privKeyMode;
	CRYS_RSAKGData_t __keyGenData_ptr;
	DxUint8_t __keyGenData_ptr_null_flag;
	/* Output */
	CRYS_RSAUserPrivKey_t __UserPrivKey_ptr;
	DxUint8_t __UserPrivKey_ptr_null_flag;
	CRYS_RSAUserPubKey_t __userPubKey_ptr;
	DxUint8_t __userPubKey_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_DX_RSA_KG_ExtendGenerateKeyPairParams_s;


#endif /*__CRYS_RSA_SEPRPC_H__*/
