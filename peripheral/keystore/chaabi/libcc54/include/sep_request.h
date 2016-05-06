/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _SEP_REQUEST_H_
#define _SEP_REQUEST_H_
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif
#include "dx_bitops.h"

#define DX_SEP_REQUEST_GPR_IDX 3

#define DX_SEP_REQUEST_4KB_MASK 0xFFF
#define DX_SEP_REQUEST_MIN_BUF_SIZE (4*1024)
#define DX_SEP_REQUEST_MAX_BUF_SIZE (32*1024)

/* Protocol error codes */
#define DX_SEP_REQUEST_SUCCESS 0x00
#define DX_SEP_REQUEST_OUT_OF_SYNC_ERR 0x01
#define DX_SEP_REQUEST_INVALID_REQ_SIZE_ERR 0x02
#define DX_SEP_REQUEST_INVALID_AGENT_ID_ERR 0x03

/* Sep Request GPR3 format (Sep to Host) */
#define DX_SEP_REQUEST_AGENT_ID_BIT_OFFSET 0
#define DX_SEP_REQUEST_AGENT_ID_BIT_SIZE 8
#define DX_SEP_REQUEST_COUNTER_BIT_OFFSET 8
#define DX_SEP_REQUEST_COUNTER_BIT_SIZE 8
#define DX_SEP_REQUEST_REQ_LEN_BIT_OFFSET 16
#define DX_SEP_REQUEST_REQ_LEN_BIT_SIZE 16

/* Sep Request GPR3 format (Host to Sep) */
#define DX_SEP_REQUEST_RETURN_CODE_BIT_OFFSET 0
#define DX_SEP_REQUEST_RETURN_CODE_BIT_SIZE 8
/* #define DX_SEP_REQUEST_COUNTER_BIT_OFFSET 8 */
/* #define DX_SEP_REQUEST_COUNTER_BIT_SIZE 8 */
#define DX_SEP_REQUEST_RESP_LEN_BIT_OFFSET 16
#define DX_SEP_REQUEST_RESP_LEN_BIT_SIZE 16

/* Get/Set macros */
#define SEP_REQUEST_GET_AGENT_ID(gpr) BITFIELD_GET(                           \
	(gpr), DX_SEP_REQUEST_AGENT_ID_BIT_OFFSET,                            \
	DX_SEP_REQUEST_AGENT_ID_BIT_SIZE)
#define SEP_REQUEST_SET_AGENT_ID(gpr, val) BITFIELD_SET(                      \
	(gpr), DX_SEP_REQUEST_AGENT_ID_BIT_OFFSET,                            \
	DX_SEP_REQUEST_AGENT_ID_BIT_SIZE, (val))
#define SEP_REQUEST_GET_RETURN_CODE(gpr) BITFIELD_GET(                        \
	(gpr), DX_SEP_REQUEST_RETURN_CODE_BIT_OFFSET,                         \
	DX_SEP_REQUEST_RETURN_CODE_BIT_SIZE)
#define SEP_REQUEST_SET_RETURN_CODE(gpr, val) BITFIELD_SET(                   \
	(gpr), DX_SEP_REQUEST_RETURN_CODE_BIT_OFFSET,                         \
	DX_SEP_REQUEST_RETURN_CODE_BIT_SIZE, (val))
#define SEP_REQUEST_GET_COUNTER(gpr) BITFIELD_GET(                            \
	(gpr), DX_SEP_REQUEST_COUNTER_BIT_OFFSET,                             \
	DX_SEP_REQUEST_COUNTER_BIT_SIZE)
#define SEP_REQUEST_SET_COUNTER(gpr, val) BITFIELD_SET(                       \
	(gpr), DX_SEP_REQUEST_COUNTER_BIT_OFFSET,                             \
	DX_SEP_REQUEST_COUNTER_BIT_SIZE, (val))
#define SEP_REQUEST_GET_REQ_LEN(gpr) BITFIELD_GET(                            \
	(gpr), DX_SEP_REQUEST_REQ_LEN_BIT_OFFSET,                             \
	DX_SEP_REQUEST_REQ_LEN_BIT_SIZE)
#define SEP_REQUEST_SET_REQ_LEN(gpr, val) BITFIELD_SET(                       \
	(gpr), DX_SEP_REQUEST_REQ_LEN_BIT_OFFSET,                             \
	DX_SEP_REQUEST_REQ_LEN_BIT_SIZE, (val))
#define SEP_REQUEST_GET_RESP_LEN(gpr) BITFIELD_GET(                           \
	(gpr), DX_SEP_REQUEST_RESP_LEN_BIT_OFFSET,                            \
	DX_SEP_REQUEST_RESP_LEN_BIT_SIZE)
#define SEP_REQUEST_SET_RESP_LEN(gpr, val) BITFIELD_SET(                      \
	(gpr), DX_SEP_REQUEST_RESP_LEN_BIT_OFFSET,                            \
	DX_SEP_REQUEST_RESP_LEN_BIT_SIZE, (val))

#endif /*_SEP_REQUEST_H_*/
