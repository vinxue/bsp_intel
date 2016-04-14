/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#include "dx_pal_types.h"
#include "dx_pal_log.h"

void DX_PAL_LogInit(void)
{
	/*nothing to do*/
}

void DX_PAL_LogLevelSet(DxUint32_t setLevel)
{
	DX_PAL_logLevel = setLevel;
}


