/* Generated file - DO NOT EDIT! */
/* Generated by utils/src/sep_rpc_gen/sep_rpc_gen.py@2451 at 2012-09-20 12:33:37.092987 */
/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/* \file crys_kdf_seprpc.h
 * SeP-RPC header file for CrysKdf API */

#ifndef __CRYS_KDF_SEPRPC_H__
#define __CRYS_KDF_SEPRPC_H__

#define SEPRPC_AGENT_ID_CrysKdf 8

#include "crys_kdf.h"
#include "crys_kdf_error.h"
#include "sep_rpc.h"
#include "dx_pal_compiler.h"


/************ _DX_KDF_KeyDerivFunc ***************/
#define SEPRPC_FUNC_ID__DX_KDF_KeyDerivFunc 1
/*
CRYSError_t _DX_KDF_KeyDerivFunc(
	DxUint8_t ZZSecret_ptr[],
	DxUint32_t ZZSecretSize,
	CRYS_KDF_OtherInfo_t *OtherInfo_ptr,
	DxUint32_t KDFhashMode,
	DxUint32_t derivation_mode,
	DxUint8_t KeyingData_ptr[],
	DxUint32_t KeyingDataSizeBytes)
*/

#define SEPRPC_MEMREF_NUM__DX_KDF_KeyDerivFunc 0
#if SEPRPC_MEMREF_NUM__DX_KDF_KeyDerivFunc > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM__DX_KDF_KeyDerivFunc is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc__DX_KDF_KeyDerivFuncParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t ZZSecret_ptr[CRYS_KDF_MAX_SIZE_OF_SHARED_SECRET_VALUE];
	DxUint8_t ZZSecret_ptr_null_flag;
	/*le32*/DxUint32_t ZZSecretSize;
	CRYS_KDF_OtherInfo_t __OtherInfo_ptr;
	DxUint8_t __OtherInfo_ptr_null_flag;
	/*le32*/DxUint32_t KDFhashMode;
	/*le32*/DxUint32_t derivation_mode;
	/*le32*/DxUint32_t KeyingDataSizeBytes;
	/* Output */
	DxUint8_t KeyingData_ptr[CRYS_KDF_MAX_SIZE_OF_KEYING_DATA];
	DxUint8_t KeyingData_ptr_null_flag;
	CRYSError_t _funcRetCode;
} SepRpc__DX_KDF_KeyDerivFuncParams_s;


#endif /*__CRYS_KDF_SEPRPC_H__*/
