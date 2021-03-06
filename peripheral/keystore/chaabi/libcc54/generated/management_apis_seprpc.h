/* Generated file - DO NOT EDIT! */
/* Generated by utils/src/sep_rpc_gen/sep_rpc_gen.py@2451 at 2012-09-20 11:57:19.110527 */
/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/* \file management_apis_seprpc.h
 * SeP-RPC header file for DxManagement API */

#ifndef __MANAGEMENT_APIS_SEPRPC_H__
#define __MANAGEMENT_APIS_SEPRPC_H__

#define SEPRPC_AGENT_ID_DxManagement 6

#include "dx_pal_types.h"
#include "dx_management.h"
#include "sep_rpc.h"
#include "dx_pal_compiler.h"


/************ DX_MNG_GetLcs ***************/
#define SEPRPC_FUNC_ID_DX_MNG_GetLcs 1
/*
DxUint32_t DX_MNG_GetLcs(
	DxUint32_t *lcsVal)
*/

#define SEPRPC_MEMREF_NUM_DX_MNG_GetLcs 0
#if SEPRPC_MEMREF_NUM_DX_MNG_GetLcs > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_DX_MNG_GetLcs is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_DX_MNG_GetLcsParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Output */
	/*le32*/DxUint32_t __lcsVal;
	DxUint8_t __lcsVal_null_flag;
	DxUint32_t _funcRetCode;
} SepRpc_DX_MNG_GetLcsParams_s;



/************ DX_MNG_GetFwVersion ***************/
#define SEPRPC_FUNC_ID_DX_MNG_GetFwVersion 2
/*
DxUint32_t DX_MNG_GetFwVersion(
	DxUint32_t *fwVer,
	DxUint32_t *romVer)
*/

#define SEPRPC_MEMREF_NUM_DX_MNG_GetFwVersion 0
#if SEPRPC_MEMREF_NUM_DX_MNG_GetFwVersion > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_DX_MNG_GetFwVersion is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_DX_MNG_GetFwVersionParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Output */
	/*le32*/DxUint32_t __fwVer;
	DxUint8_t __fwVer_null_flag;
	/*le32*/DxUint32_t __romVer;
	DxUint8_t __romVer_null_flag;
	DxUint32_t _funcRetCode;
} SepRpc_DX_MNG_GetFwVersionParams_s;


#endif /*__MANAGEMENT_APIS_SEPRPC_H__*/
