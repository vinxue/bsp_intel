/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef __DX_CCLIB_H__
#define __DX_CCLIB_H__

#include <stdint.h>

typedef enum {
	DX_CCLIB_RET_OK = 0,
	DX_CCLIB_RET_ENODEV,        /* Device not opened or does not exist */
	DX_CCLIB_RET_EINTERNAL,     /* Internal driver error (check system log) */
	DX_CCLIB_RET_ENOTSUP,       /* Unsupported function/option */
	DX_CCLIB_RET_ENOPERM,       /* Not enough permissions for request */
	DX_CCLIB_RET_EINVAL,        /* Invalid parameters */
	DX_CCLIB_RET_HW_Q_INIT,
	DX_CCLIB_RET_COMPLETION,
	DX_CCLIB_RET_HAL,
	DX_CCLIB_RESERVE32B = 0x7FFFFFFFL
} DX_CclibRetCode_t;

#define DXDI_DEV_PRIO0 "/dev/dx_sep_q0"
#define DXDI_DEV_PRIO1 "/dev/dx_sep_q1"

/*!
 * Init OS driver context
 * This function must be invoked before any other CC library API function
 * but for the functions which deal with library initialization properties:
 * DX_CclibSetPriority, DX_CclibGetPriority, DX_CclibGetMaxPriority
 *
 * \return DxCclib_RetCode
 */
DX_CclibRetCode_t DX_CclibInit(void);

/*!
 * Finalize OS driver context - release associated resources
 */
void DX_CclibFini(void);

/*!
 * Set requested CryptoCell priority queue
 * This function must be invoked before DX_CclibInit
 *
 * \param priority Requested priority queue
 *
 * \return DxCclib_RetCode
 */
DX_CclibRetCode_t DX_CclibSetPriority(uint32_t priority);

/*!
 * Get selected priority
 * (as set by DX_CclibSetPriority or default if it was not invoked)
 *
 *
 * \return DX_CclibDevPriority
 */
uint32_t DX_CclibGetPriority(void);

/*!
 * Get the highest priority supported
 *
 * \return DX_CclibDevPriority
 */
uint32_t DX_CclibGetMaxPriority(void);



#endif /*__DX_CCLIB_H__*/

