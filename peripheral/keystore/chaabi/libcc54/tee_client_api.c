/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CCLIB

/* \file tee_client_api.c
   Implementation of the TEE Client API Specification Version 1.0
*/

#include "dx_pal_log.h"
#include "applet_mgr_error.h"
#include "dx_pal_compiler.h"
#include "driver_interface.h"
#include "tee_client_api.h"


/* Structure for implementation-specific part of the TEEC data structures */
struct TEEC_SharedMemory_imp {
	DxDI_SepAppContext_t *contextP; /* Associated context */
	dxdi_memref_id_t memrefId;
} DX_PAL_COMPILER_TYPE_MAY_ALIAS;
struct TEEC_Session_imp {
	DxDI_SepAppContext_t *contextP; /* Associated context */
	dxdi_sepapp_session_id_t sessionId;
} DX_PAL_COMPILER_TYPE_MAY_ALIAS;

/* Verify imp[] part of TEEC API structures can accomodate DxDI objects */
DX_PAL_COMPILER_ASSERT(sizeof(DxDI_SepAppContext_t) <= DX_PAL_COMPILER_SIZEOF_STRUCT_MEMBER(TEEC_Context, imp),
	"TEEC_Context.imp[] is too small");
DX_PAL_COMPILER_ASSERT(sizeof(struct TEEC_Session_imp) <= DX_PAL_COMPILER_SIZEOF_STRUCT_MEMBER(TEEC_Session, imp),
	"TEEC_Session.imp[] is too small");
DX_PAL_COMPILER_ASSERT(sizeof(struct TEEC_SharedMemory_imp) <= DX_PAL_COMPILER_SIZEOF_STRUCT_MEMBER(TEEC_SharedMemory, imp),
	"TEEC_SharedMemory.imp[] is too small");

static TEEC_Result appletMgrErr2TeecRetCode(uint32_t appletMgrErr)
{
	switch (appletMgrErr) {
	case DX_APP_MNG_SESSION_TBL_IS_FULL_ERROR:
		return TEEC_ERROR_OUT_OF_MEMORY;
	case DX_APP_MNG_ILLEGAL_SESSION_ERROR:
	case DX_APP_MNG_ILLEGAL_APPLET_NUM_ERROR:
	case DX_APP_MNG_APPLET_NOT_FOUND_ERROR:
		return TEEC_ERROR_ITEM_NOT_FOUND;
	case DX_APP_MNG_ILLEGAL_PARAM_ERROR:
		return TEEC_ERROR_BAD_PARAMETERS;
	case DX_APP_MNG_ILLEGAL_OP_FOR_NONPRIV_APPLET_ERROR:
		return TEEC_ERROR_ACCESS_DENIED;
	case DX_APP_MNG_ILLEGAL_SLOT_NUM_ERROR:
		return TEEC_ERROR_ITEM_NOT_FOUND;
	case DX_APP_MNG_SRAM_APPLET_CALL_NONPRIV_APPLET_ERROR:
		return TEEC_ERROR_ACCESS_DENIED;
	default: /* All the other errors are not supposed to happen here*/
		DX_PAL_LOG_DEBUG("Invalid error from applet manager = 0x%08X\n", appletMgrErr);
	}
	return TEEC_ERROR_GENERIC;
}

/*!
 * Convert from DriverInterface return code to TEEC_Result
 *
 * \param diRc DriverInterface return code
 * \return TEEC_Result
 */
static TEEC_Result diRetCodeToTeecResult(DxDI_RetCode_t diRc, uint32_t retOrigin, uint32_t sepRetCode)
{
	switch (diRc) {
	case DXDI_RET_OK: return TEEC_SUCCESS;
	case DXDI_RET_ENODEV: return TEEC_ERROR_ITEM_NOT_FOUND;
	case DXDI_RET_EINTERNAL: return TEEC_ERROR_GENERIC;
	case DXDI_RET_ENOTSUP: return TEEC_ERROR_NOT_SUPPORTED;
	case DXDI_RET_ENOPERM: return TEEC_ERROR_ACCESS_DENIED;
	case DXDI_RET_EINVAL: return TEEC_ERROR_BAD_PARAMETERS;
	case DXDI_RET_ENORSC: return TEEC_ERROR_OUT_OF_MEMORY;
	case DXDI_RET_ESEP:
		if (retOrigin == TEEC_ORIGIN_TRUSTED_APP) /* Return code from the applet */
			return sepRetCode;
		else /* Return code from the applet manager */
			return appletMgrErr2TeecRetCode(sepRetCode);
	case DXDI_RET_EHW: return TEEC_ERROR_COMMUNICATION;
	default: return TEEC_ERROR_GENERIC;
	}
}

static uint32_t sepModuleToTeecOrigin(enum dxdi_sep_module sepModule)
{
	switch (sepModule) {
	case DXDI_SEP_MODULE_HOST_DRIVER: return TEEC_ORIGIN_API;
	case DXDI_SEP_MODULE_SW_QUEUE   : return TEEC_ORIGIN_COMMS;
	case DXDI_SEP_MODULE_APP_MGR    : return TEEC_ORIGIN_TEE;
	case DXDI_SEP_MODULE_APP        : return TEEC_ORIGIN_TRUSTED_APP;
	case DXDI_SEP_MODULE_RPC_AGENT  : return TEEC_ORIGIN_TEE; /* Not supposed to happen for SepApp stack */
	case DXDI_SEP_MODULE_SYM_CRYPTO : return TEEC_ORIGIN_TEE; /* Not supposed to happen for SepApp stack */
	default: return TEEC_ORIGIN_API;
	}
}

/*!
 * Convert from TEEC data direction flags to DxDI data direction
 * This functions is used for memory registration flags and for operation parameters direction
 * It assumes the directions within an operand are always holding at
 * lowest 2 bits the direction flags (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT)
 *
 * \param teecDataDirFlags
 *
 * \return dxdi_data_direction
 */
static enum dxdi_data_direction TeecDataDirToDxDiDataDir(uint32_t teecDataDirFlags)
{
	enum dxdi_data_direction diDir = DXDI_DATA_NULL;

	if (teecDataDirFlags & TEEC_MEM_INPUT)
		diDir = DXDI_DATA_TO_DEVICE;
	if (teecDataDirFlags & TEEC_MEM_OUTPUT)
		diDir |= DXDI_DATA_FROM_DEVICE;

	return diDir;
}

/*!
 * Convert from TEEC API operation type to DxDI parameters type
 *
 * \param The operation context (of the associated Session)
 * \param TeecOp
 * \param appParams DxDI format of the client parameters
 *
 * \return int 0 on success, EINVAL for invalid paramters
 */
static int TeecOperationToSepAppParams(DxDI_SepAppContext_t *context,
	TEEC_Operation *TeecOp, struct dxdi_sepapp_params *appParams)
{
	int i;
	uint8_t curParamType;
	struct TEEC_SharedMemory_imp *SharedMemDataP;

	for (i = 0; i < 4; i++) {

		appParams->params[i].val.copy_dir = DXDI_DATA_NULL; /* default */
		if (TeecOp == NULL) {
			curParamType = TEEC_NONE; /* Default for non-existing operation */
		} else {
			curParamType = TEEC_PARAM_TYPE_GET(TeecOp->paramTypes, i);
		}

		switch (curParamType) {
		case TEEC_NONE                 :
			appParams->params_types[i] = DXDI_SEPAPP_PARAM_NULL;
			break;

		case TEEC_VALUE_INPUT          :
			/*FALLTHROUGH*/
		case TEEC_VALUE_INOUT          :
			/* copy-in data by value */
			appParams->params[i].val.data[0] = TeecOp->params[i].value.a;
			appParams->params[i].val.data[1] = TeecOp->params[i].value.b;
			/*FALLTROUGH*/
		case TEEC_VALUE_OUTPUT         :
			appParams->params[i].val.copy_dir = TeecDataDirToDxDiDataDir(curParamType);
			appParams->params_types[i] = DXDI_SEPAPP_PARAM_VAL;
			break;

		case TEEC_MEMREF_TEMP_INPUT    :
		case TEEC_MEMREF_TEMP_OUTPUT   :
		case TEEC_MEMREF_TEMP_INOUT    :
			appParams->params[i].memref.ref_id = DXDI_MEMREF_ID_NULL;
			appParams->params[i].memref.start_or_offset = (unsigned long)TeecOp->params[i].tmpref.buffer;
			appParams->params[i].memref.size = TeecOp->params[i].tmpref.size;
			appParams->params[i].memref.dma_direction = TeecDataDirToDxDiDataDir(curParamType);
			appParams->params_types[i] = DXDI_SEPAPP_PARAM_MEMREF;
			break;

		case TEEC_MEMREF_WHOLE         :
			SharedMemDataP = (struct TEEC_SharedMemory_imp *)&TeecOp->params[i].memref.parent->imp[0];
			if (SharedMemDataP->contextP != context)
				return EINVAL;
			appParams->params[i].memref.ref_id = SharedMemDataP->memrefId;
			appParams->params[i].memref.start_or_offset = 0;
			/* Size and direction are taken from the parent (registered memory) */
			appParams->params[i].memref.size = TeecOp->params[i].memref.parent->size;
			appParams->params[i].memref.dma_direction = TeecDataDirToDxDiDataDir(TeecOp->params[i].memref.parent->flags);
			appParams->params_types[i] = DXDI_SEPAPP_PARAM_MEMREF;
			break;

		case TEEC_MEMREF_PARTIAL_INPUT :
		case TEEC_MEMREF_PARTIAL_OUTPUT:
		case TEEC_MEMREF_PARTIAL_INOUT :
			SharedMemDataP = (struct TEEC_SharedMemory_imp *)&TeecOp->params[i].memref.parent->imp[0];
			if (SharedMemDataP->contextP != context)
				return EINVAL;
			appParams->params[i].memref.ref_id = SharedMemDataP->memrefId;
			appParams->params[i].memref.start_or_offset = (unsigned long)TeecOp->params[i].memref.offset;
			appParams->params[i].memref.size = TeecOp->params[i].memref.size;
			appParams->params[i].memref.dma_direction = TeecDataDirToDxDiDataDir(curParamType);
			appParams->params_types[i] = DXDI_SEPAPP_PARAM_MEMREF;
			break;

		default: /* Invalid parameter type */
			return EINVAL;
		}
	}

	return 0;
}

/*!
 * Copy back to "VAL" parameters of type "OUT" or "INOUT"
 *
 * \param TeecOp
 * \param appParams
 */
static void CopyBackValParams(TEEC_Operation *TeecOp, struct dxdi_sepapp_params *appParams)
{
	int i;
	uint8_t curParamType;

	if (TeecOp == NULL)
		return;

	for (i = 0; i < 4; i++) {
		curParamType = TEEC_PARAM_TYPE_GET(TeecOp->paramTypes, i);
		if ((curParamType == TEEC_VALUE_OUTPUT) ||
		    (curParamType == TEEC_VALUE_INOUT)) {
			 TeecOp->params[i].value.a = appParams->params[i].val.data[0];
			 TeecOp->params[i].value.b = appParams->params[i].val.data[1];
		}
	}
}

/* 4.5.2 */
TEEC_Result TEEC_InitializeContext(const char *name, TEEC_Context *context)
{
	DxDI_RetCode_t diRc;

	if (context == NULL)
		return TEEC_ERROR_BAD_PARAMETERS;
	/* "name" is ignored - we have only one SeP in the system */
	diRc = DxDI_SepAppContextAlloc(name, (DxDI_SepAppContext_t *)context);

	return diRetCodeToTeecResult(diRc, TEEC_ORIGIN_API, 0);
}

/* 4.5.3 */
void TEEC_FinalizeContext(TEEC_Context *context)
{
	if (context == NULL)
		return;
	DxDI_SepAppContextFree((DxDI_SepAppContext_t *)context);
}

/* 4.5.4 */
TEEC_Result  TEEC_RegisterSharedMemory(
	TEEC_Context*      context,
	TEEC_SharedMemory* sharedMem)
{
	struct TEEC_SharedMemory_imp *impDataP = (struct TEEC_SharedMemory_imp *)&sharedMem->imp[0];
	DxDI_RetCode_t diRc;
	struct dxdi_memref memRef;

	if ((context == NULL) || (sharedMem == NULL))
		return TEEC_ERROR_BAD_PARAMETERS;

	memRef.ref_id = DXDI_MEMREF_ID_NULL;
	memRef.start_or_offset = (unsigned long)sharedMem->buffer;
	memRef.size = sharedMem->size;

	/* Convert TEE flags to DXDI flags */
	memRef.dma_direction = 0;
	if (sharedMem->flags & TEEC_MEM_INPUT)
		memRef.dma_direction = DXDI_DATA_TO_DEVICE;
	if (sharedMem->flags & TEEC_MEM_OUTPUT)
		memRef.dma_direction |= DXDI_DATA_FROM_DEVICE;

	diRc = DxDI_RegisterMemForDma((DxDI_SepAppContext_t *)context, &memRef, &impDataP->memrefId);
	impDataP->contextP = (DxDI_SepAppContext_t *)context;

	return diRetCodeToTeecResult(diRc, TEEC_ORIGIN_API, 0);
}

/* 4.5.5 */
TEEC_Result  TEEC_AllocateSharedMemory(
	TEEC_Context*      context,
	TEEC_SharedMemory* sharedMem)
{
	return TEEC_ERROR_NOT_IMPLEMENTED;
}

/* 4.5.6 */
void  TEEC_ReleaseSharedMemory (
	TEEC_SharedMemory* sharedMem)
{
	struct TEEC_SharedMemory_imp *impDataP = (struct TEEC_SharedMemory_imp *)&sharedMem->imp[0];

	if (sharedMem != NULL)
		(void)DxDI_FreeMemForDma(impDataP->contextP, impDataP->memrefId);
}

/* 4.5.7 */
TEEC_Result  TEEC_OpenSession (
	TEEC_Context    *context,
	TEEC_Session    *session,
	const TEEC_UUID *destination,
	uint32_t         connectionMethod,
	void            *connectionData,
	TEEC_Operation  *operation,
	uint32_t        *returnOrigin)
{
	struct TEEC_Session_imp *diSessionP = (struct TEEC_Session_imp *)&session->imp;
	enum dxdi_sep_module sepRetOrigin;
	uint32_t sepRetCode;
	DxDI_RetCode_t diRc;
	struct dxdi_sepapp_params sepAppParams;
	struct dxdi_sepapp_params *sepAppParamsP;

	if ((context == NULL) || (session == NULL)) {
		*returnOrigin = TEEC_ORIGIN_API;
		return TEEC_ERROR_BAD_PARAMETERS;
	}

	if (operation == NULL) {
		sepAppParamsP = NULL;
	} else {
		sepAppParamsP = &sepAppParams;
		if (TeecOperationToSepAppParams((DxDI_SepAppContext_t *)context,
		    operation, &sepAppParams) != 0) {
			*returnOrigin = TEEC_ORIGIN_API;
			return TEEC_ERROR_BAD_PARAMETERS;
		}
	}

	/* TODO: Map TEEC API login methods (TEEC_LOGIN_*) to DI's (DXDI_SEPAPP_AUTH_METHOD_*) */

	diSessionP->contextP = (DxDI_SepAppContext_t *)context;
	diRc = DxDI_SepAppSessionOpen(diSessionP->contextP,
		(dxdi_sepapp_uuid_t *)destination, connectionMethod, connectionData, sepAppParamsP,
		&diSessionP->sessionId, &sepRetOrigin, &sepRetCode);

	if (diRc == DXDI_RET_OK) {
		CopyBackValParams(operation, sepAppParamsP);
	}
	*returnOrigin = sepModuleToTeecOrigin(sepRetOrigin);

	return diRetCodeToTeecResult(diRc, *returnOrigin, sepRetCode);
}

/* 4.5.8 */
void  TEEC_CloseSession (
	TEEC_Session* session)
{
	struct TEEC_Session_imp *diSessionP = (struct TEEC_Session_imp *)&session->imp;

	if (session != NULL)
		(void)DxDI_SepAppSessionClose(diSessionP->contextP, diSessionP->sessionId);
}

/* 4.5.9 */
TEEC_Result  TEEC_InvokeCommand(
	TEEC_Session     *session,
	uint32_t          commandID,
	TEEC_Operation   *operation,
	uint32_t         *returnOrigin)
{
	struct TEEC_Session_imp *diSessionP = (struct TEEC_Session_imp *)&session->imp;
	enum dxdi_sep_module sepRetOrigin;
	uint32_t sepRetCode;
	DxDI_RetCode_t diRc;
	struct dxdi_sepapp_params sepAppParams;
	struct dxdi_sepapp_params *sepAppParamsP;

	if (session == NULL) {
		*returnOrigin = TEEC_ORIGIN_API;
		return TEEC_ERROR_BAD_PARAMETERS;
	}

	if (operation == NULL) {
		sepAppParamsP = NULL;
	} else {
		sepAppParamsP = &sepAppParams;
		if (TeecOperationToSepAppParams(diSessionP->contextP,
			    operation, &sepAppParams) != 0) {
			*returnOrigin = TEEC_ORIGIN_API;
			return TEEC_ERROR_BAD_PARAMETERS;
		}
	}

	diRc = DxDI_SepAppCommandInvoke(diSessionP->contextP, diSessionP->sessionId,
		commandID, sepAppParamsP,
		&sepRetOrigin, &sepRetCode);

	if (diRc == DXDI_RET_OK) {
		CopyBackValParams(operation, sepAppParamsP);
	}
	*returnOrigin = sepModuleToTeecOrigin(sepRetOrigin);

	return diRetCodeToTeecResult(diRc, *returnOrigin, sepRetCode);
}

/* 4.5.10 - not supported in this implementation */
void  TEEC_RequestCancellation(
	TEEC_Operation  *operation)
{
	/* Since this is function returns "void" we just ignore the request */
}


