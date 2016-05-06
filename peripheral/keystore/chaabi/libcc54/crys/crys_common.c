/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_API

/*****************************************************************************
  \file CRYS_common.c   						     *
  CRYS infrastructure functions common to all the modules (on host)          *
*****************************************************************************/
#include "driver_interface.h"
#include "crys_error.h"

/*!
 * Convert from DxDI return code to CRYS return code
 *
 * \param diRc
 * \param sepRetCode
 */
CRYSError_t DX_DiErr2CrysErr(DxDI_RetCode_t diRc, DxUint32_t sepRetCode)
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

		return diRc;
	case DXDI_RET_ESEP:
		/* Pass through SeP/CRYS error code */
		return sepRetCode;
	default:
		/* ??? */
		return diRc;
	}
}
