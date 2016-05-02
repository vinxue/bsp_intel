/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef DX_PAL_TYPES_H
#define DX_PAL_TYPES_H

#include "dx_pal_types_plat.h"

typedef DxUint8_t               DxByte_t;

typedef enum {
    DX_FALSE = 0,
    DX_TRUE = 1
} EDxBool;

typedef DxUint32_t              DxBool_t;
typedef DxUint32_t              DxStatus;

#define DxError_t               DxStatus
#define DX_SUCCESS              0UL
#define DX_FAIL			1UL
#define DX_INFINITE             0xFFFFFFFF

#endif
