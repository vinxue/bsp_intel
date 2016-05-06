/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef  APPLET_MGR_ERROR_H
#define  APPLET_MGR_ERROR_H

#include "dx_pal_types.h"

/******************************************************************************
*				DEFINITIONS
******************************************************************************/
/* noga - define base error for app_mgr */

#define DX_APP_MNG_SLOT_IS_NOT_EMPTY_ERROR                              0x00000001
#define DX_APP_MNG_STATIC_LOAD_AFTER_DYNAMIC_NOT_ALLOWED_ERROR          0x00000002
#define DX_APP_MNG_SESSION_TBL_IS_FULL_ERROR                            0x00000003
#define DX_APP_MNG_ILLEGAL_SESSION_ERROR                                0x00000004
#define DX_APP_MNG_ILLEGAL_APPLET_NUM_ERROR                             0x00000005
#define DX_APP_MNG_EXCEED_ICACHE_SIZE_ERROR                             0x00000006
#define DX_APP_MNG_EXCEED_DCACHE_SIZE_ERROR                             0x00000007
#define DX_APP_MNG_APPLET_NOT_FOUND_ERROR                               0x00000008
#define DX_APP_MNG_OPEN_SESSIONS_EXIST_FOR_APPLET_ERROR                 0x00000009
#define DX_APP_MNG_ILLEGAL_VERIFICATION_KEY_INDEX_ERROR                 0x0000000A
#define DX_APP_MNG_VERIFICATION_FUNC_ALREADY_REGISTERED_ERROR           0x0000000B
#define DX_APP_MNG_ILLEGAL_PARAM_ERROR                                  0x0000000C
#define DX_APP_MNG_ILLEGAL_REPLACEABLE_APPLET_ID_ERROR                  0x0000000D
#define DX_APP_MNG_NO_CURRENT_ACTIVE_APPLET_ERROR                  	0x0000000E
#define DX_APP_MNG_ILLEGAL_OP_FOR_NONPRIV_APPLET_ERROR                	0x0000000F
#define DX_APP_MNG_ILLEGAL_SLOT_NUM_ERROR                		0x00000010
#define DX_APP_MNG_DUPLICATE_APPLET_ERROR                		0x00000011
#define DX_APP_MNG_SRAM_APPLET_CALL_NONPRIV_APPLET_ERROR                0x00000012
#define DX_APP_MNG_CALLBACK_FUNC_ALREADY_REGISTERED_ERROR		0x00000013
#define DX_APP_MNG_APPLET_PRIORITY_ERROR				0x00000014
#define DX_APP_MNG_CALLBACK_REGISTRATION_NOT_ALLOWED_ERROR		0x00000015
#define DX_APP_MNG_DEFAULT_APPLET_INIT_ERROR				0x00000016
#define DX_APP_MNG_APPLET_ALREADY_UNLOADED_ERROR			0x00000017
#define DX_APP_MNG_UNLOADING_RUNNING_APPLET_ERROR			0x00000018
#define DX_APP_MNG_NO_FREE_SEMAPHORE_ERROR				0x00000019
#define DX_APP_MNG_SEMAPHORE_LOCKED_ERROR				0x00000020

/******************************************************************************
*				MACROS
******************************************************************************/

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

#endif /*APPLET_MGR_ERROR_H*/

