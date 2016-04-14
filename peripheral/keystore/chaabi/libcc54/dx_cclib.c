/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CCLIB

#define _MAIN_C_ /*declare PAL's globals*/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include "dx_pal_types.h"
#include "dx_pal_log.h"
#include "driver_interface.h"
#include "dx_cclib.h"
#include "dx_pal_types.h"

/*** Cclib state variables ***/
static uint32_t devPriority = DXDI_PRIO_MIN;


/*!
 * Convert errno to DxDI_RetCode
 *
 * \param err The errno value
 *
 * \return DxDI_RetCode
 */
static DX_CclibRetCode_t ConvErr2RetCode(int err, const char* errContext)
{
	if (err != 0) {
		DX_PAL_LOG_ERR("%s: (%d)\n", errContext, err);
	}
	switch (err) {
	case 0:
		return DX_CCLIB_RET_OK;
	case DXDI_RET_ENODEV:
		return DX_CCLIB_RET_ENODEV;
	case DXDI_RET_ENOTSUP:
		return DX_CCLIB_RET_ENOTSUP;
	case DXDI_RET_EINVAL:
		return DX_CCLIB_RET_EINVAL;
	case DXDI_RET_ENOPERM:
		return DX_CCLIB_RET_ENOPERM;
	case DXDI_RET_EINTERNAL:
		return DX_CCLIB_RET_EINTERNAL;
	default:
		return DX_CCLIB_RET_EINTERNAL;
	}
}

/*!
 * Init OS driver
 *
 *
 * \param
 *
 * \return DxCclib_RetCode
 */
DX_CclibRetCode_t DX_CclibInit()
{
	DxDI_RetCode_t rc;

	DX_PAL_LogInit();

	rc = DxDI_Open(devPriority);
	return ConvErr2RetCode(rc, "DxCclib_Init: Open OS driver ");
}


/*!
 * Finalize OS driver context - release associated resources
 */
void DX_CclibFini(void)
{
	(void)DxDI_Close();
}


/*!
 * Set requested CryptoCell priority queue
 * This function must be invoked before DX_CclibInit
 *
 * \param priority Requested priority queue
 *
 * \return DxCclib_RetCode
 */
DX_CclibRetCode_t DX_CclibSetPriority(uint32_t priority)
{
	if (DxDI_IsOpen()) {
		DX_PAL_LOG_DEBUG("DxCclib_SetPriority: Devise is open.\n");
		return DX_CCLIB_RET_ENOPERM;
	}
    if (priority > DXDI_PRIO_MAX){
		DX_PAL_LOG_DEBUG("DxCclib_SetPriority: Ilegal Priority\n");
		return DX_CCLIB_RET_ENOTSUP;
	}
	devPriority = (DxDI_DevPriority_t)priority;
	return DX_CCLIB_RET_OK;
}

/*!
 * Get selected priority
 * (as set by DX_CclibSetPriority or default if it was not invoked)
 *
 *
 * \return DX_CclibDevPriority
 */
uint32_t DX_CclibGetPriority(void)
{
	DX_PAL_LOG_DEBUG("DxCclib_GetPriority: priority (%d)\n",devPriority);
	return devPriority;
}


/*!
 * Get the highest priority supported
 *
 * \return DX_CclibDevPriority
 */
uint32_t DX_CclibGetMaxPriority(void)
{
	return DXDI_PRIO_MAX;
	/* TODO: Return actually max. priority as reported by the driver */
}
