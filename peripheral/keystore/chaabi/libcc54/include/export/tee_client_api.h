/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/* \file tee_client_api.h
   The header file of the TEE Client API Specification Version 1.0
*/

#ifndef __TEE_CLIENT_API_H__
#define __TEE_CLIENT_API_H__

#include <stddef.h>
#include <stdint.h>

/**********************************************/
/* API Constants - TEEC API Spec. section 4.4 */
/**********************************************/

/*  4.4.1 */
#define TEEC_CONFIG_SHAREDMEM_MAX_SIZE 0xFFFFFFFF /* < 4GB */

/*  4.4.2 - Return codes */
#define TEEC_SUCCESS               0x00000000
#define TEEC_ERROR_GENERIC         0xFFFF0000
#define TEEC_ERROR_ACCESS_DENIED   0xFFFF0001
#define TEEC_ERROR_CANCEL          0xFFFF0002
#define TEEC_ERROR_ACCESS_CONFLICT 0xFFFF0003
#define TEEC_ERROR_EXCESS_DATA     0xFFFF0004
#define TEEC_ERROR_BAD_FORMAT      0xFFFF0005
#define TEEC_ERROR_BAD_PARAMETERS  0xFFFF0006
#define TEEC_ERROR_BAD_STATE       0xFFFF0007
#define TEEC_ERROR_ITEM_NOT_FOUND  0xFFFF0008
#define TEEC_ERROR_NOT_IMPLEMENTED 0xFFFF0009
#define TEEC_ERROR_NOT_SUPPORTED   0xFFFF000A
#define TEEC_ERROR_NO_DATA         0xFFFF000B
#define TEEC_ERROR_OUT_OF_MEMORY   0xFFFF000C
#define TEEC_ERROR_BUSY            0xFFFF000D
#define TEEC_ERROR_COMMUNICATION   0xFFFF000E
#define TEEC_ERROR_SECURITY        0xFFFF000F
#define TEEC_ERROR_SHORT_BUFFER    0xFFFF0010

/* 4.4.3 */
#define TEEC_ORIGIN_API            0x00000001
#define TEEC_ORIGIN_COMMS          0x00000002
#define TEEC_ORIGIN_TEE            0x00000003
#define TEEC_ORIGIN_TRUSTED_APP    0x00000004

/* 4.4.4 */
/* Table 4-4: API Shared Memory Control Flags */
#define TEEC_MEM_INPUT             0x00000001
#define TEEC_MEM_OUTPUT            0x00000002
/* Table 4-5: API Parameter Types */
#define TEEC_NONE                  0x0
#define TEEC_VALUE_INPUT           0x1
#define TEEC_VALUE_OUTPUT          0x2
#define TEEC_VALUE_INOUT           0x3
#define TEEC_MEMREF_TEMP_INPUT     0x5
#define TEEC_MEMREF_TEMP_OUTPUT    0x6
#define TEEC_MEMREF_TEMP_INOUT     0x7
#define TEEC_MEMREF_WHOLE          0xC
#define TEEC_MEMREF_PARTIAL_INPUT  0xD
#define TEEC_MEMREF_PARTIAL_OUTPUT 0xE
#define TEEC_MEMREF_PARTIAL_INOUT  0xF

/* 4.4.5 */
#define TEEC_LOGIN_PUBLIC            0x00000000
#define TEEC_LOGIN_USER              0x00000001
#define TEEC_LOGIN_GROUP             0x00000002
#define TEEC_LOGIN_APPLICATION       0x00000004
#define TEEC_LOGIN_USER_APPLICATION  0x00000005
#define TEEC_LOGIN_GROUP_APPLICATION 0x00000006


/* Macro to set/get parameter type from paramTypes word */
#define TEEC_PARAM_TYPE_SET(paramTypes, paramIdx, thisParamType) do { \
	((uint8_t*)&(paramTypes))[paramIdx] = thisParamType;             \
} while (0)
#define TEEC_PARAM_TYPE_GET(paramTypes, paramIdx) ((uint8_t*)&paramTypes)[paramIdx]

/**********************************************/
/* API Data Types - TEEC API Spec. section 4.3*/
/**********************************************/

/* 4.3.2 */
typedef uint32_t TEEC_Result;

/* 4.3.3 */
typedef struct {
    uint32_t timeLow;
    uint16_t timeMid;
    uint16_t timeHiAndVersion;
    uint8_t  clockSeqAndNode[8];
} TEEC_UUID;

/* 4.3.4 */
typedef struct TEEC_Context {
	uint32_t imp[1]; /* Opaque space for implementation specific data */
} TEEC_Context;

/* 4.3.5 */
typedef struct TEEC_Session {
	uint32_t imp[2]; /* Opaque space for implementation specific data */
} TEEC_Session;

/* 4.3.6 */
typedef struct TEEC_SharedMemory {
	void    *buffer;
	size_t   size;
	uint32_t flags;
	uint32_t imp[2]; /* Opaque space for implementation specific data */
} TEEC_SharedMemory;

/* 4.3.7 */
typedef struct {
	void  *buffer;
	size_t size;
} TEEC_TempMemoryReference;

/* 4.3.8 */
typedef struct {
	TEEC_SharedMemory *parent;
	size_t size;
	size_t offset;
} TEEC_RegisteredMemoryReference;

/* 4.3.9 */
typedef struct {
	uint32_t a;
	uint32_t b;
} TEEC_Value;

/* 4.3.10 */
typedef union {
	TEEC_TempMemoryReference       tmpref;
	TEEC_RegisteredMemoryReference memref;
	TEEC_Value                     value;
} TEEC_Parameter;

/* 4.3.11 */
typedef struct TEEC_Operation {
	uint32_t       started; /* Not used - cancellation is not supported in our implementation */
	uint32_t       paramTypes;
    TEEC_Parameter params[4];
	/* No implementation specific data */
} TEEC_Operation;


/**********************************************/
/* API functions - TEEC API Spec. section 4.5 */
/**********************************************/

/* 4.5.2 */
TEEC_Result TEEC_InitializeContext(const char *name, TEEC_Context *context);

/* 4.5.3 */
void TEEC_FinalizeContext(TEEC_Context *context);

/* 4.5.4 */
TEEC_Result  TEEC_RegisterSharedMemory(
	TEEC_Context*      context,
	TEEC_SharedMemory* sharedMem);

/* 4.5.5 */
TEEC_Result  TEEC_AllocateSharedMemory(
	TEEC_Context*      context,
	TEEC_SharedMemory* sharedMem);

/* 4.5.6 */
void  TEEC_ReleaseSharedMemory (
	TEEC_SharedMemory* sharedMem);

/* 4.5.7 */
TEEC_Result  TEEC_OpenSession (
	TEEC_Context    *context,
	TEEC_Session    *session,
	const TEEC_UUID *destination,
	uint32_t         connectionMethod,
	void            *connectionData,
	TEEC_Operation  *operation,
	uint32_t        *returnOrigin);

/* 4.5.8 */
void  TEEC_CloseSession (
	TEEC_Session* session);

/* 4.5.9 */
TEEC_Result  TEEC_InvokeCommand(
	TEEC_Session     *session,
	uint32_t          commandID,
	TEEC_Operation   *operation,
	uint32_t         *returnOrigin);

/* 4.5.10 - not supported in this implementation */
void  TEEC_RequestCancellation(
	TEEC_Operation  *operation);


#endif /*__TEE_CLIENT_API_H__*/
