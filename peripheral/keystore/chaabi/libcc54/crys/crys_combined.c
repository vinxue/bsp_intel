/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

/** @file
 *  \brief This file contains functions that are used for the
 *         CRYS combined and tunneling modes operations processing on SEP
 *         controlled by the HOST.
 *
 */

/************* Include Files ****************/

/* .............. CRYS level includes ................. */

#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "crys_combined_error.h"
#include "crys_combined.h"
#include "driver_interface.h"
#include "crys_context_relocation.h"
#include "sep_ctx.h"
//#include "error.h"


/************* Private functions prototypes ****************/

/*!
 * Convert from DxDI return code to CRYS return code
 *
 * \param diRc:		Driver Interface returned error.
 * \param errorInfo:	Additional error information.
 *
 */
static CRYSError_t DX_CombinedDiErr2CrysErr(DxDI_RetCode_t diRc, DxUint32_t errorInfo)
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
		case DXDI_ERROR_UNSUP:
			return CRYS_COMBINED_IS_NOT_SUPPORTED;
		case DXDI_ERROR_INVAL_MODE:
			return CRYS_COMBINED_ILLEGAL_OPERATION_MODE_ERROR;
		case DXDI_ERROR_INVAL_DIRECTION:
			return CRYS_COMBINED_ILLEGAL_PARAMS_ERROR;
		case DXDI_ERROR_INVAL_KEY_SIZE:
			return CRYS_COMBINED_ILLEGAL_PARAMS_ERROR;
		case DXDI_ERROR_INVAL_DIN_PTR:
			return CRYS_COMBINED_DATA_IN_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DOUT_PTR:
			return CRYS_COMBINED_DATA_OUT_POINTER_INVALID_ERROR;
		case DXDI_ERROR_INVAL_DATA_SIZE:
			return CRYS_COMBINED_DATA_IN_SIZE_ILLEGAL;
		case DXDI_ERROR_DIN_DOUT_OVERLAP:
			return CRYS_COMBINED_DATA_OUT_DATA_IN_OVERLAP_ERROR;
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
	/* check that there is no overlapping between the data input and data output buffers,
	   except the inplace case that is legal */
	if (DataIn_ptr != DataOut_ptr) {
		/* checking the case that the input buffer is in a higher address than the output buffer */
		if (DataIn_ptr > DataOut_ptr) {
			if (DataOut_ptr + DataInSize > DataIn_ptr)
				return CRYS_COMBINED_DATA_OUT_DATA_IN_OVERLAP_ERROR;
		}

		/* checking the case that the output buffer is in a higher address than the input buffer */
		else {
			if (DataIn_ptr + DataInSize > DataOut_ptr)
				return CRYS_COMBINED_DATA_OUT_DATA_IN_OVERLAP_ERROR;
		}
	}

	return CRYS_OK;
}


/*!
 * This function feeds in the combined configuration scheme into the driver
 * interface data entity as user requested. It also retains each given context
 * in a non crossing page.
 *
 * \param pConfig The user configuration scheme.
 * \param props The driver interface data entity.
 *
 * \return CRYSError_t On success the value CRYS_OK is returned,
 * 			and on failure - a value from crys_combined_error.h
 */
static CRYSError_t BoxConfigScheme(
	CrysCombinedConfig_t *pConfig,
	struct dxdi_combined_props *props)
{
	int cfgNode;

	/* clear the dxdi data structure */
	memset(props, 0, sizeof(struct dxdi_combined_props));

	for (cfgNode = 0; cfgNode < CRYS_COMBINED_MAX_NODES; cfgNode++) {
		DxUint32_t *contextPtr = NULL;

		/* Get pointer within the buffer that can accomodate context without
		    crossing a page */
		if (pConfig->node[cfgNode].pContext != NULL) {
			contextPtr = DX_GetUserCtxLocation(pConfig->node[cfgNode].pContext);
			if (contextPtr == NULL) {
				return CRYS_COMBINED_INVALID_USER_SUB_CONTEXT_POINTER_ERROR;
			}
		}
		/* set the context pointer into the dxdi properties */
		props->node_props[cfgNode].context = contextPtr;
		/* set the engine input into the dxdi properties */
		props->node_props[cfgNode].eng_input = pConfig->node[cfgNode].engineSrc;
	}

	return CRYS_OK;
}

/*************************** Public Functions ***************************/

/*!
 * Clears the configuration nodes.
 *
 * \param pConfig A pointer to the configuration scheme array
 */
CIMPORT_C void CrysCombinedConfigInit(CrysCombinedConfig_t *pConfig)
{
	DX_PAL_MemSetZero(pConfig, sizeof(CrysCombinedConfig_t));
}

/************************************************************************/
/*!
 * This function is used to initilize the combined (tunneling) operations.
 *
 * The function initializes user's combined mode configuration structure,
 * used during current process sequence (Init-Block-Finish).
 *
 * Before processing the combined mode and calling this function the user must
 * initialize all needed sub operations contexts (AES, AES-MAC, HASH) included into
 * this combined operation and perform the following:
 *
 *      - allocate combined mode configuration structure of type CrysCombinedConfig_t;
 *        The structure containing MAX_NUM_NODES (see define) nodes of type CrysCombinedNodeConfig_t,
 *	  each of them contains pointer to sub operation context and input data source
 *        indication according to DMA data flow in configuration, as follows:
 *	     { {pCtxt1; src1;},  {pCtxt2; src2}, .... {NULL; srcForDout} }, where:
 *              - pCtxtX - pointer to context of current sub operation X or NULL for indication
 *                of DOUT (DMA output) or end node in the configuration;
 *              - srcX is number of previous engine (sub operation), which output is used
 *                as input to engine (or DOUT) X;
 *      - allocate CRYS contexts for each sub operation (AES...HASH) included into
 *        combined configuration - maximum count of operations is MAX_NUM_NODES-1 (one node - for
 *        DMA autput or indication of end of the configuration);
 *      - call appropriate CRYS Init functions for each sub operation (CRYS_AES_Init or
 *	  CRYS_HASH_Init) according to combined mode and given operations parameters;
 *	- initialize (clean) the configuration structure by calling macro CrysCombinedConfigInit();
 *      - set all needed configuration nodes by calling macro CrysCombinedConfigSet() for each sub
 *	  operation involved into combined mode; the input source indication for each engine
 *        should be set according to engines order defined by the user in the configuration
 *	  structure.
 *	- set indication of ending (last) node and DMA output source by calling the same macro
 *        CrysCombinedConfigSet() with appropriate input parameters:
 *	  	- context pointer ctxPtrX = NULL;
 *	  	- indication (type: CrysCombinedEngineSource_e) of engine, which output
 *                should be transferred to DMA DOUT; if DMA output is not used there,
 *                then srcX = INPUT_NULL;
 *
 *  Then the user may call this function to initialize the allocated combined
 *  user context.
 *
 *
 * \param pConfig A pointer to the Configuration Nodes array (NodesConfig).
 * 			This array represents the user combined scheme.
 *
 * \return CIMPORT_C CRYSError_t On success the value CRYS_OK is returned,
 * 			and on failure - a value from crys_combined_error.h
 */
CEXPORT_C CRYSError_t CRYS_Combined_Init(CrysCombinedConfig_t *pConfig)
{

#ifndef CRYS_NO_COMBINED_SUPPORT

	DxDI_RetCode_t  diRc;
	CRYSError_t crysRc;
	DxUint32_t errorInfo;
	struct dxdi_combined_props props;

	if(pConfig == DX_NULL) {
		return CRYS_COMBINED_INVALID_NODES_CONFIG_POINTER_ERROR;
	}

	/* accomodate configuration scheme in dxdi entity */
	crysRc = BoxConfigScheme(pConfig, &props);
	if (crysRc != CRYS_OK)
		return crysRc;

	diRc = DxDI_CombinedInit(&props, &errorInfo);

	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_CombinedDiErr2CrysErr);
#else
	return CRYS_COMBINED_IS_NOT_SUPPORTED;

#endif /* !CRYS_NO_COMBINED_SUPPORT */
}


/***************************************************************************/
/*!
 * This function is used to process block of data in the combined (tunneling) mode.
 *
 * \param pConfig A pointer to the Configuration Nodes array.
 * 			This array represents the user combined scheme.
 * \param cipherOffset Relevant in cases where the authenticated  data resides in
 *      		a different offset from the cipher data.
 *      		Note: currently an error returned for any value other than zero.
 * \param pDataIn A pointer to a block of input data ready for processing.
 * \param dataInSize The size of the input data.
 * \param pDataOut A pointer to output data. Could be the same as input data
 *      		pointer (for inplace operations) or NULL if there is
 *      		only authentication for output.
 *
 * \return CIMPORT_C CRYSError_t On success the value CRYS_OK is returned,
 * 			and on failure - a value from crys_combined_error.h
 */
CEXPORT_C CRYSError_t CRYS_Combined_Process(
					CrysCombinedConfig_t *pConfig,
					DxUint32_t cipherOffset,
					DxUint8_t *pDataIn,
					DxUint32_t dataInSize,
					DxUint8_t *pDataOut)
{
#ifndef CRYS_NO_COMBINED_SUPPORT

	struct dxdi_combined_props props;
	DxDI_RetCode_t diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;

	/* check the users Configuration structure pointer */
	if (pConfig == DX_NULL) {
		return CRYS_COMBINED_INVALID_NODES_CONFIG_POINTER_ERROR;
	}
	if (pDataIn == DX_NULL) {
		return CRYS_COMBINED_DATA_IN_POINTER_INVALID_ERROR;
	}
	if (cipherOffset != 0) {
		return CRYS_COMBINED_CIPHER_OFFSET_ILLEGAL;
	}
	/* validate data size */
	if (dataInSize == 0) {
		return CRYS_COMBINED_DATA_IN_SIZE_ILLEGAL;
	}
	/* if DMA output is needed then validate buffers overflow */
	if (pDataOut != DX_NULL) {
		crysRc = DataPtrValidation(pDataIn,
					   dataInSize,
					   pDataOut);
		if (crysRc != CRYS_OK) {
			return crysRc;
		}
	}

	/* accomodate configuration scheme in dxdi entity */
	crysRc = BoxConfigScheme(pConfig, &props);
	if (crysRc != CRYS_OK)
		return crysRc;

	diRc = DxDI_CombinedProcessDblk(
				&props,
				pDataIn,
				dataInSize,
				pDataOut,
				&errorInfo);

	/* check errors */
	return DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_CombinedDiErr2CrysErr);
#else
	return CRYS_COMBINED_IS_NOT_SUPPORTED;
#endif
}


/***************************************************************************/
/*!
 * This function is used to finish the combined or tunneling operations
 * It releases all used contexts (including sub operation ones).
 *
 * \param pConfig A pointer to the Configuration Nodes array.
 * 		This array represents the user given sequence of combined operations.
 * \param cipherOffset Relevant in cases where the authenticated  data resides in
 *      		a different offset from the cipher data.
 *      		Note: currently an error returned for any value other than zero.
 * \param pDataIn A pointer on a block of input data ready for processing.
 * \param dataInSize The size of the input data.
 * \param pDataOut A pointer to output data. Could be the same as input data pointer
 *      		(for inplace operations) or NULL if there is only
 *      		authentication result for output.
 * \param pAuthData A pointer to authentication or digest result output.
 * \param pAuthDataSize A pointer to size of user given output buffer [in]
 * 			and actual size [out] of authentication or digest result .
 *
 * \return CIMPORT_C CRYSError_t On success the value CRYS_OK is returned,
 * 			and on failure - a value from crys_combined_error.h
 */
CEXPORT_C CRYSError_t CRYS_Combined_Finish(
					CrysCombinedConfig_t *pConfig,
					DxUint32_t  cipherOffset,
					DxUint8_t  *pDataIn,
					DxUint32_t  dataInSize,
					DxUint8_t  *pDataOut,
					DxUint8_t  *pAuthData,
					DxUint32_t *pAuthDataSize)
{
#ifndef CRYS_NO_CONBINED_SUPPORT

	struct dxdi_combined_props props;
	DxDI_RetCode_t diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;
	DxUint32_t actualAuthDataSize;

	/* check the users Configuration structure pointer */
	if (pConfig == DX_NULL) {
		return CRYS_COMBINED_INVALID_NODES_CONFIG_POINTER_ERROR;
	}
	if (cipherOffset != 0) {
		return CRYS_COMBINED_CIPHER_OFFSET_ILLEGAL;
	}
	/* if dataInSize > 0 then validate buffers overflow */
	if ((dataInSize > 0) && (pDataOut != DX_NULL)) {
		crysRc = DataPtrValidation(pDataIn,
					   dataInSize,
					   pDataOut);
		if (crysRc != CRYS_OK) {
			return crysRc;
		}
	}

	/* accomodate configuration scheme in dxdi entity */
	crysRc = BoxConfigScheme(pConfig, &props);
	if (crysRc != CRYS_OK)
		return crysRc;

	actualAuthDataSize = *pAuthDataSize;

	diRc =  DxDI_CombinedFinalize(
				&props,
				pDataIn,
				dataInSize,
				pDataOut,
				pAuthData,
				&actualAuthDataSize,
				&errorInfo);

	/* check errors */
	crysRc = DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_CombinedDiErr2CrysErr);

	if (crysRc != CRYS_OK)
		goto End;

	/* check the size of the buffer for authentication data and set its actual size */
	if (pAuthData != DX_NULL){
		if (*pAuthDataSize < actualAuthDataSize) {
			*pAuthDataSize = actualAuthDataSize;
			crysRc = CRYS_COMBINED_DATA_AUTH_BUFFER_SIZE_INVALID_ERROR;
		} else {
			*pAuthDataSize = actualAuthDataSize;
		}
	}

End:
	return crysRc;
#else
	return CRYS_COMBINED_IS_NOT_SUPPORTED;
#endif
}

/***************************************************************************/
/*!
 * This function is used to perform the combined or tunneling operations with
 * one function call.
 *
 * \param pConfig A pointer to the Configuration Nodes array.
 * 		This array represents the user given combined operation.
 * \param cipherOffset Relevant in cases where the authenticated  data resides in
 *      		a different offset from the cipher data.
 *      		Note: currently an error returned for any value other than zero.
 * \param pDataIn A pointer on a block of input data ready for processing.
 * \param dataInSize The size of the input data.
 * \param pDataOut A pointer to output data. Could be the same as input data pointer
 *      		(for inplace operations) or NULL if there is only
 *      		authentication result for output.
 * \param pAuthData A pointer to authentication or digest result output.
 * \param pAuthDataSize A pointer to size of user given output buffer [in]
 * 			and actual size [out] of authentication or digest result .
 *
 * \return CIMPORT_C CRYSError_t On success the value CRYS_OK is returned,
 * 			and on failure - a value from crys_combined_error.h
 */
CEXPORT_C CRYSError_t CRYS_Combined(
				CrysCombinedConfig_t *pConfig,
				DxUint32_t  cipherOffset,
				DxUint8_t  *pDataIn,
				DxUint32_t  dataInSize,
				DxUint8_t  *pDataOut,
				DxUint8_t  *pAuthData,
				DxUint32_t *pAuthDataSize)
{
#ifndef CRYS_NO_CONBINED_SUPPORT
	struct dxdi_combined_props props;
	DxDI_RetCode_t diRc;
	DxUint32_t errorInfo;
	CRYSError_t crysRc;
	DxUint32_t actualAuthDataSize;

	/* check the users Configuration structure pointer */
	if (pConfig == DX_NULL) {
		return CRYS_COMBINED_INVALID_NODES_CONFIG_POINTER_ERROR;
	}
	if ((pDataIn == DX_NULL) && (dataInSize > 0)) {
		return CRYS_COMBINED_DATA_IN_POINTER_INVALID_ERROR;
	}
	if (cipherOffset != 0) {
		return CRYS_COMBINED_CIPHER_OFFSET_ILLEGAL;
	}
	/* if dataInSize > 0 then validate buffers overflow */
	if (pDataOut != DX_NULL) {
		crysRc = DataPtrValidation(pDataIn,
					   dataInSize,
					   pDataOut);
		if (crysRc != CRYS_OK) {
			return crysRc;
		}
	}

	/* accomodate configuration scheme in dxdi entity */
	crysRc = BoxConfigScheme(pConfig, &props);
	if (crysRc != CRYS_OK)
		return crysRc;

	actualAuthDataSize = *pAuthDataSize;

	diRc =  DxDI_Combined(
			&props,
			pDataIn,
			dataInSize,
			pDataOut,
			pAuthData,
			&actualAuthDataSize,
			&errorInfo);

	/* check errors */
	crysRc = DX_CRYS_RETURN_ERROR(diRc, errorInfo, DX_CombinedDiErr2CrysErr);
	if (crysRc != CRYS_OK) {
		goto End;
	}

	/* check size of the buffer for authentication data and set its actual size */
	if (pAuthData != DX_NULL) {
		if (*pAuthDataSize < actualAuthDataSize) {
			*pAuthDataSize = actualAuthDataSize;
			crysRc = CRYS_COMBINED_DATA_AUTH_BUFFER_SIZE_INVALID_ERROR;
		} else {
			*pAuthDataSize = actualAuthDataSize;
		}
	}
End:
	return crysRc;
#else
	return CRYS_COMBINED_IS_NOT_SUPPORTED;
#endif
}

