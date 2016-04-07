/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_CC_DEFS_H_
#define _DX_CC_DEFS_H

#define DX_INT32_MAX 0x7FFFFFFFL

typedef enum {
	DX_USER_KEY = 0,
	DX_ROOT_KEY = 1,
	DX_PROVISIONING_KEY = 2,
	DX_XOR_HDCP_KEY = 3,
	DX_APPLET_KEY = 4,
	DX_SESSION_KEY = 5,
	DX_END_OF_KEYS = DX_INT32_MAX,
}DX_CRYPTO_KEY_TYPE_t;


#endif
