/* Generated file - DO NOT EDIT! */
/* Generated by utils/src/sep_rpc_gen/sep_rpc_gen.py@2451 at 2012-09-20 12:33:41.575692 */
/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/* \file crys_dh_seprpc.h
 * SeP-RPC header file for CrysDh API */

#ifndef __CRYS_DH_SEPRPC_H__
#define __CRYS_DH_SEPRPC_H__

#define SEPRPC_AGENT_ID_CrysDh 9

#include "crys_dh.h"
#include "crys_dh_error.h"
#include "crys_dh_kg.h"
#include "crys_rsa_prim.h"
#include "sep_rpc.h"
#include "dx_pal_compiler.h"


/************ _DX_DH_GeneratePubPrv ***************/
#define SEPRPC_FUNC_ID__DX_DH_GeneratePubPrv 1
/*
CRYSError_t _DX_DH_GeneratePubPrv(
	DxUint8_t Generator_ptr[],
	DxUint16_t GeneratorSize,
	DxUint8_t Prime_ptr[],
	DxUint16_t PrimeSize,
	DxUint16_t L,
	DxUint8_t Q_ptr[],
	DxUint16_t QSize,
	CRYS_DH_OpMode_t DH_mode,
	CRYS_DHUserPubKey_t *tmpPubKey_ptr,
	CRYS_DHPrimeData_t *tmpPrimeData_ptr,
	DxUint8_t ClientPrvKey_ptr[],
	DxUint16_t *ClientPrvKeySize_ptr,
	DxUint8_t ClientPub1_ptr[],
	DxUint16_t *ClientPubSize_ptr)
*/

#define SEPRPC_MEMREF_NUM__DX_DH_GeneratePubPrv 0
#if SEPRPC_MEMREF_NUM__DX_DH_GeneratePubPrv > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_DH_GeneratePubPrv is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc__DX_DH_GeneratePubPrvParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t Generator_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t Generator_ptr_null_flag;
	/*le16*/DxUint16_t GeneratorSize;
	DxUint8_t Prime_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t Prime_ptr_null_flag;
	/*le16*/DxUint16_t PrimeSize;
	/*le16*/DxUint16_t L;
	DxUint8_t Q_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t Q_ptr_null_flag;
	/*le16*/DxUint16_t QSize;
	/*le32*/CRYS_DH_OpMode_t DH_mode;
	CRYS_DHUserPubKey_t __tmpPubKey_ptr;
	DxUint8_t __tmpPubKey_ptr_null_flag;
	DxUint32_t  tmpPrimeData_ptr_size;
	/* Input/Output */
	/*le16*/DxUint16_t __ClientPrvKeySize_ptr;
	DxUint8_t __ClientPrvKeySize_ptr_null_flag;
	/*le16*/DxUint16_t __ClientPubSize_ptr;
	DxUint8_t __ClientPubSize_ptr_null_flag;
	/* Output */
	DxUint8_t ClientPrvKey_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ClientPrvKey_ptr_null_flag;
	DxUint8_t ClientPub1_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ClientPub1_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_DH_GeneratePubPrvParams_s;



/************ CRYS_DH_GetSecretKey ***************/
#define SEPRPC_FUNC_ID_CRYS_DH_GetSecretKey 2
/*
CRYSError_t CRYS_DH_GetSecretKey(
	DxUint8_t ClientPrvKey_ptr[],
	DxUint16_t ClientPrvKeySize,
	DxUint8_t ServerPubKey_ptr[],
	DxUint16_t ServerPubKeySize,
	DxUint8_t Prime_ptr[],
	DxUint16_t PrimeSize,
	CRYS_DHUserPubKey_t *tmpPubKey_ptr,
	CRYS_DHPrimeData_t *tmpPrimeData_ptr,
	DxUint8_t SecretKey_ptr[],
	DxUint16_t *SecretKeySize_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_DH_GetSecretKey 0
#if SEPRPC_MEMREF_NUM_CRYS_DH_GetSecretKey > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_DH_GetSecretKey is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_DH_GetSecretKeyParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t ClientPrvKey_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ClientPrvKey_ptr_null_flag;
	/*le16*/DxUint16_t ClientPrvKeySize;
	DxUint8_t ServerPubKey_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ServerPubKey_ptr_null_flag;
	/*le16*/DxUint16_t ServerPubKeySize;
	DxUint8_t Prime_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t Prime_ptr_null_flag;
	/*le16*/DxUint16_t PrimeSize;
	CRYS_DHUserPubKey_t __tmpPubKey_ptr;
	DxUint8_t __tmpPubKey_ptr_null_flag;
	DxUint32_t  tmpPrimeData_ptr_size;
	/* Input/Output */
	/*le16*/DxUint16_t __SecretKeySize_ptr;
	DxUint8_t __SecretKeySize_ptr_null_flag;
	/* Output */
	DxUint8_t SecretKey_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t SecretKey_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_DH_GetSecretKeyParams_s;



/************ CRYS_DH_CheckPubKey ***************/
#define SEPRPC_FUNC_ID_CRYS_DH_CheckPubKey 3
/*
CRYSError_t CRYS_DH_CheckPubKey(
	DxUint8_t modP_ptr[],
	DxUint32_t modPsizeBytes,
	DxUint8_t orderQ_ptr[],
	DxUint32_t orderQsizeBytes,
	DxUint8_t pubKey_ptr[],
	DxUint32_t pubKeySizeBytes,
	CRYS_DH_Temp_t *tempBuff_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_DH_CheckPubKey 0
#if SEPRPC_MEMREF_NUM_CRYS_DH_CheckPubKey > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_DH_CheckPubKey is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_DH_CheckPubKeyParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t modP_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t modP_ptr_null_flag;
	/*le32*/DxUint32_t modPsizeBytes;
	DxUint8_t orderQ_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t orderQ_ptr_null_flag;
	/*le32*/DxUint32_t orderQsizeBytes;
	DxUint8_t pubKey_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t pubKey_ptr_null_flag;
	/*le32*/DxUint32_t pubKeySizeBytes;
	DxUint32_t  tempBuff_ptr_size;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_DH_CheckPubKeyParams_s;



/************ CRYS_DH_X942_GetSecretData ***************/
#define SEPRPC_FUNC_ID_CRYS_DH_X942_GetSecretData 4
/*
CRYSError_t CRYS_DH_X942_GetSecretData(
	DxUint8_t ClientPrvKey_ptr[],
	DxUint16_t ClientPrvKeySize,
	DxUint8_t ServerPubKey_ptr[],
	DxUint16_t ServerPubKeySize,
	DxUint8_t Prime_ptr[],
	DxUint16_t PrimeSize,
	CRYS_DH_OtherInfo_t *otherInfo_ptr,
	CRYS_DH_HASH_OpMode_t hashMode,
	CRYS_DH_DerivationFunc_Mode DerivFunc_mode,
	CRYS_DH_Temp_t *tmpBuff_ptr,
	DxUint8_t SecretKeyingData_ptr[],
	DxUint16_t SecretKeyingDataSize)
*/

#define SEPRPC_MEMREF_NUM_CRYS_DH_X942_GetSecretData 0
#if SEPRPC_MEMREF_NUM_CRYS_DH_X942_GetSecretData > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_DH_X942_GetSecretData is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_DH_X942_GetSecretDataParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t ClientPrvKey_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ClientPrvKey_ptr_null_flag;
	/*le16*/DxUint16_t ClientPrvKeySize;
	DxUint8_t ServerPubKey_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ServerPubKey_ptr_null_flag;
	/*le16*/DxUint16_t ServerPubKeySize;
	DxUint8_t Prime_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t Prime_ptr_null_flag;
	/*le16*/DxUint16_t PrimeSize;
	CRYS_DH_OtherInfo_t __otherInfo_ptr;
	DxUint8_t __otherInfo_ptr_null_flag;
	/*le32*/CRYS_DH_HASH_OpMode_t hashMode;
	/*le32*/CRYS_DH_DerivationFunc_Mode DerivFunc_mode;
	DxUint32_t  tmpBuff_ptr_size;
	/*le16*/DxUint16_t SecretKeyingDataSize;
	/* Output */
	DxUint8_t SecretKeyingData_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t SecretKeyingData_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_DH_X942_GetSecretDataParams_s;



/************ CRYS_DH_X942_HybridGetSecretData ***************/
#define SEPRPC_FUNC_ID_CRYS_DH_X942_HybridGetSecretData 5
/*
CRYSError_t CRYS_DH_X942_HybridGetSecretData(
	DxUint8_t ClientPrvKey_ptr1[],
	DxUint16_t ClientPrvKeySize1,
	DxUint8_t ClientPrvKey_ptr2[],
	DxUint16_t ClientPrvKeySize2,
	DxUint8_t ServerPubKey_ptr1[],
	DxUint16_t ServerPubKeySize1,
	DxUint8_t ServerPubKey_ptr2[],
	DxUint16_t ServerPubKeySize2,
	DxUint8_t Prime_ptr[],
	DxUint16_t PrimeSize,
	CRYS_DH_OtherInfo_t *otherInfo_ptr,
	CRYS_DH_HASH_OpMode_t hashMode,
	CRYS_DH_DerivationFunc_Mode DerivFunc_mode,
	CRYS_DH_HybrTemp_t *tmpDhHybr_ptr,
	DxUint8_t SecretKeyingData_ptr[],
	DxUint16_t SecretKeyingDataSize)
*/

#define SEPRPC_MEMREF_NUM_CRYS_DH_X942_HybridGetSecretData 0
#if SEPRPC_MEMREF_NUM_CRYS_DH_X942_HybridGetSecretData > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_DH_X942_HybridGetSecretData is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_DH_X942_HybridGetSecretDataParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t ClientPrvKey_ptr1[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ClientPrvKey_ptr1_null_flag;
	/*le16*/DxUint16_t ClientPrvKeySize1;
	DxUint8_t ClientPrvKey_ptr2[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ClientPrvKey_ptr2_null_flag;
	/*le16*/DxUint16_t ClientPrvKeySize2;
	DxUint8_t ServerPubKey_ptr1[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ServerPubKey_ptr1_null_flag;
	/*le16*/DxUint16_t ServerPubKeySize1;
	DxUint8_t ServerPubKey_ptr2[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t ServerPubKey_ptr2_null_flag;
	/*le16*/DxUint16_t ServerPubKeySize2;
	DxUint8_t Prime_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t Prime_ptr_null_flag;
	/*le16*/DxUint16_t PrimeSize;
	CRYS_DH_OtherInfo_t __otherInfo_ptr;
	DxUint8_t __otherInfo_ptr_null_flag;
	/*le32*/CRYS_DH_HASH_OpMode_t hashMode;
	/*le32*/CRYS_DH_DerivationFunc_Mode DerivFunc_mode;
	DxUint32_t  tmpDhHybr_ptr_size;
	/*le16*/DxUint16_t SecretKeyingDataSize;
	/* Output */
	DxUint8_t SecretKeyingData_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t SecretKeyingData_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_DH_X942_HybridGetSecretDataParams_s;



/************ CRYS_DH_CreateDomainParams ***************/
#define SEPRPC_FUNC_ID_CRYS_DH_CreateDomainParams 6
/*
CRYSError_t CRYS_DH_CreateDomainParams(
	DxUint32_t modPsizeBits,
	DxUint32_t orderQsizeBits,
	DxUint32_t seedSizeBits,
	DxUint8_t modP_ptr[],
	DxUint8_t orderQ_ptr[],
	DxUint8_t generatorG_ptr[],
	DxUint32_t *generGsizeBytes_ptr,
	DxUint8_t factorJ_ptr[],
	DxUint32_t *JsizeBytes_ptr,
	DxUint8_t seedS_ptr[],
	DxInt8_t generateSeed,
	DxUint32_t *pgenCounter_ptr,
	CRYS_DHKGData_t *DHKGbuff_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_DH_CreateDomainParams 0
#if SEPRPC_MEMREF_NUM_CRYS_DH_CreateDomainParams > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_DH_CreateDomainParams is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_DH_CreateDomainParamsParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	/*le32*/DxUint32_t modPsizeBits;
	/*le32*/DxUint32_t orderQsizeBits;
	/*le32*/DxUint32_t seedSizeBits;
	DxInt8_t generateSeed;
	DxUint32_t  DHKGbuff_ptr_size;
	/* Input/Output */
	/*le32*/DxUint32_t __generGsizeBytes_ptr;
	DxUint8_t __generGsizeBytes_ptr_null_flag;
	/*le32*/DxUint32_t __JsizeBytes_ptr;
	DxUint8_t __JsizeBytes_ptr_null_flag;
	DxUint8_t seedS_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t seedS_ptr_null_flag;
	/* Output */
	DxUint8_t modP_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t modP_ptr_null_flag;
	DxUint8_t orderQ_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t orderQ_ptr_null_flag;
	DxUint8_t generatorG_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t generatorG_ptr_null_flag;
	DxUint8_t factorJ_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t factorJ_ptr_null_flag;
	/*le32*/DxUint32_t __pgenCounter_ptr;
	DxUint8_t __pgenCounter_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_DH_CreateDomainParamsParams_s;



/************ CRYS_DH_CheckDomainParams ***************/
#define SEPRPC_FUNC_ID_CRYS_DH_CheckDomainParams 7
/*
CRYSError_t CRYS_DH_CheckDomainParams(
	DxUint8_t modP_ptr[],
	DxUint32_t modPsizeBytes,
	DxUint8_t orderQ_ptr[],
	DxUint32_t orderQsizeBytes,
	DxUint8_t generatorG_ptr[],
	DxUint32_t generatorSizeBytes,
	DxUint8_t seedS_ptr[],
	DxUint32_t seedSizeBits,
	DxUint32_t pgenCounter,
	CRYS_DHKG_CheckTemp_t *checkTempBuff_ptr)
*/

#define SEPRPC_MEMREF_NUM_CRYS_DH_CheckDomainParams 0
#if SEPRPC_MEMREF_NUM_CRYS_DH_CheckDomainParams > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_CRYS_DH_CheckDomainParams is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_CRYS_DH_CheckDomainParamsParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t modP_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t modP_ptr_null_flag;
	/*le32*/DxUint32_t modPsizeBytes;
	DxUint8_t orderQ_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t orderQ_ptr_null_flag;
	/*le32*/DxUint32_t orderQsizeBytes;
	DxUint8_t generatorG_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t generatorG_ptr_null_flag;
	/*le32*/DxUint32_t generatorSizeBytes;
	DxUint8_t seedS_ptr[CRYS_DH_MAX_MOD_SIZE_IN_BYTES];
	DxUint8_t seedS_ptr_null_flag;
	/*le32*/DxUint32_t seedSizeBits;
	/*le32*/DxUint32_t pgenCounter;
	DxUint32_t  checkTempBuff_ptr_size;
	CRYSError_t _funcRetCode;
} SepRpc_CRYS_DH_CheckDomainParamsParams_s;


#endif /*__CRYS_DH_SEPRPC_H__*/
