/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_LOG_PLAT_H_
#define _DX_PAL_LOG_PLAT_H_

#include "dx_log_mask.h"

/************** PRINTF rules ******************/
#if defined(DEBUG)

#define PRINTF printf

#else /* Disable all prints */

#define PRINTF(...)  do {} while (0)

#endif

#endif /*_DX_PAL_LOG_PLAT_H_*/
