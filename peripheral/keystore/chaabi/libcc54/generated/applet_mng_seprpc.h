/* Generated file - DO NOT EDIT! */
/* Generated by utils/src/sep_rpc_gen/sep_rpc_gen.py@2451 at 2012-09-20 11:55:35.521996 */
/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/* \file applet_mng_seprpc.h
 * SeP-RPC header file for AppletMngLoad API */

#ifndef __APPLET_MNG_SEPRPC_H__
#define __APPLET_MNG_SEPRPC_H__

#define SEPRPC_AGENT_ID_AppletMngLoad 3

#include "dx_applet_mng_load.h"
#include "applet_mgr_error.h"
#include "sep_rpc.h"
#include "dx_pal_compiler.h"


/************ DX_CC_LoadApplet ***************/
#define SEPRPC_FUNC_ID_DX_CC_LoadApplet 1
/*
DxUint32_t DX_CC_LoadApplet(
	DxUint32_t vrlAddr,
	DxUint32_t magicNum,
	DxUint32_t verKeyIndex,
	DxUint32_t flags,
	DxUint32_t appletImageAddr,
	DxUint32_t userParam)
*/

#define SEPRPC_MEMREF_NUM_DX_CC_LoadApplet 0
#if SEPRPC_MEMREF_NUM_DX_CC_LoadApplet > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_DX_CC_LoadApplet is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_DX_CC_LoadAppletParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	/*le32*/DxUint32_t vrlAddr;
	/*le32*/DxUint32_t magicNum;
	/*le32*/DxUint32_t verKeyIndex;
	/*le32*/DxUint32_t flags;
	/*le32*/DxUint32_t appletImageAddr;
	/*le32*/DxUint32_t userParam;
	DxUint32_t _funcRetCode;
} SepRpc_DX_CC_LoadAppletParams_s;



/************ DX_CC_UnloadApplet ***************/
#define SEPRPC_FUNC_ID_DX_CC_UnloadApplet 2
/*
DxUint32_t DX_CC_UnloadApplet(
	DxUint8_t uuid[])
*/

#define SEPRPC_MEMREF_NUM_DX_CC_UnloadApplet 0
#if SEPRPC_MEMREF_NUM_DX_CC_UnloadApplet > SEP_RPC_MAX_MEMREF_PER_FUNC
#error SEPRPC_MEMREF_NUM_DX_CC_UnloadApplet is more than SEP_RPC_MAX_MEMREF_PER_FUNC
#endif

typedef struct SepRpc_DX_CC_UnloadAppletParams {
	uint32_t num_of_memrefs; /* Number of elements in the memRef array */
	/* Input */
	DxUint8_t uuid[sizeof(DX_SepAppUuid_t)];
	DxUint8_t uuid_null_flag;
	DxUint32_t _funcRetCode;
} SepRpc_DX_CC_UnloadAppletParams_s;


#endif /*__APPLET_MNG_SEPRPC_H__*/
