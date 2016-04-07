/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef __TEE_TOKEN_ERROR_H__
#define __TEE_TOKEN_ERROR_H__

#define TEE_TOKEN_BASE_ERROR                 0xCAB1A200
#define TEE_TOKEN_NULL_PTR                   (TEE_TOKEN_BASE_ERROR + 0x01)
#define TEE_TOKEN_NOT_FOUND                  (TEE_TOKEN_BASE_ERROR + 0x02)
#define TEE_TOKEN_READ_ERROR                 (TEE_TOKEN_BASE_ERROR + 0x03)
#define TEE_TOKEN_INVALID_SIZE               (TEE_TOKEN_BASE_ERROR + 0x04)
#define TEE_TOKEN_INVALID_PARAM              (TEE_TOKEN_BASE_ERROR + 0x05)
#define TEE_TOKEN_MEMORY_ERROR               (TEE_TOKEN_BASE_ERROR + 0x06)
#define TEE_TOKEN_PSID_ERROR                 (TEE_TOKEN_BASE_ERROR + 0x07)
#define TEE_TOKEN_TRANSACTION_ERROR          (TEE_TOKEN_BASE_ERROR + 0x08)
#define TEE_TOKEN_VERIFY_ERROR               (TEE_TOKEN_BASE_ERROR + 0x09)
#define TEE_TOKEN_INVALID_TOKEN              (TEE_TOKEN_BASE_ERROR + 0x0a)
#define TEE_TOKEN_LIFETIME_ERROR             (TEE_TOKEN_BASE_ERROR + 0x0b)
#define TEE_TOKEN_VERSION_ERROR              (TEE_TOKEN_BASE_ERROR + 0x0c)
#define TEE_TOKEN_CALLBACK_ERROR             (TEE_TOKEN_BASE_ERROR + 0x0d)
#define TEE_TOKEN_DUPLICATE_TOKEN            (TEE_TOKEN_BASE_ERROR + 0x0e)
#define TEE_TOKEN_BINDING_ERROR              (TEE_TOKEN_BASE_ERROR + 0x0f)
#define TEE_TOKEN_INVALID_CONTAINER          (TEE_TOKEN_BASE_ERROR + 0x10)
// libcc-54 -specific errors
#define TEE_TOKEN_LIB_DX_INIT_ERROR          0xCAB1A220
#define TEE_TOKEN_LIB_INIT_CTX_ERROR         0xCAB1A221
#define TEE_TOKEN_LIB_SHARED_MEM_ERROR       0xCAB1A222
#endif // tee_token_error
