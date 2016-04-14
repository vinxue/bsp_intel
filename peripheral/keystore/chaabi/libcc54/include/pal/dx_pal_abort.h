/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_ABORT_H
#define _DX_PAL_ABORT_H


#include "dx_pal_abort_plat.h"


/*!
 * Abort SeP execution by reporting aborting instruction in GPR6
 * (State is set to 0xBAD0BAD0 in GPR7)
 */
#define DX_PAL_Abort(msg) _DX_PAL_Abort(msg)

#endif


