/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_ERROR_H
#define _DX_PAL_ERROR_H


#ifdef __cplusplus
extern "C"
{
#endif

#define DX_PAL_BASE_ERROR                0x0F000000

/* Memory error returns */
#define DX_PAL_MEM_BUF1_GREATER          DX_PAL_BASE_ERROR + 0x01UL
#define DX_PAL_MEM_BUF2_GREATER          DX_PAL_BASE_ERROR + 0x02UL

/* Semaphore error returns */
#define DX_PAL_SEM_CREATE_FAILED         DX_PAL_BASE_ERROR + 0x03UL
#define DX_PAL_SEM_DELETE_FAILED         DX_PAL_BASE_ERROR + 0x04UL
#define DX_PAL_SEM_WAIT_TIMEOUT          DX_PAL_BASE_ERROR + 0x05UL
#define DX_PAL_SEM_WAIT_FAILED           DX_PAL_BASE_ERROR + 0x06UL
#define DX_PAL_SEM_RELEASE_FAILED        DX_PAL_BASE_ERROR + 0x07UL

#ifdef __cplusplus
}
#endif

#endif


