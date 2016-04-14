/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#include "tee_client_api.h"
#include "dx_cclib.h"
#include <string.h>

#include "hwk_crypto.h"

#define HWK_APP_UUID "INTEL HWK 000001"

enum hwk_command {
    HWK_DBG = 0,
    HWK_CREATE = 1,
    HWK_UNSEAL,
    HWK_SEAL,
    HWK_DISABLE,
    HWK_ENABLE,
    HWK_RESEAL,
    HWK_DESTROY,
    HWK_CRYPTO,
    HWK_DBG_SS_WIPE,
};


int hwk_ss_wipe(void)
{
    TEEC_Context context;
    TEEC_Session session;
    static const uint8_t destination[] = HWK_APP_UUID;
    TEEC_Result ret;
    uint32_t returnOrigin = 0;


    ret = DX_CclibInit();
    if (TEEC_SUCCESS != ret)
        return ret;

    ret = TEEC_InitializeContext(NULL, &context);
    if (TEEC_SUCCESS != ret)
        goto CclibFini;

    ret = TEEC_OpenSession(&context, &session, (TEEC_UUID*)&destination,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &returnOrigin);
    if (TEEC_SUCCESS != ret )
        goto FinalizeContext;

    ret = TEEC_InvokeCommand(&session, HWK_DBG_SS_WIPE, NULL, &returnOrigin);

CloseSession:
    TEEC_CloseSession (&session);
FinalizeContext:
    TEEC_FinalizeContext(&context);
CclibFini:
    DX_CclibFini();

    return ret;
}

int hwk_dbg(void *buf, uint32_t len)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    static const uint8_t destination[] = HWK_APP_UUID;
    TEEC_Result ret;
    uint32_t returnOrigin = 0;
    TEEC_SharedMemory smDumpBuffer;


    ret = DX_CclibInit();
    if (TEEC_SUCCESS != ret)
        return ret;

    ret = TEEC_InitializeContext(NULL, &context);
    if (TEEC_SUCCESS != ret)
        goto CclibFini;

    ret = TEEC_OpenSession(&context, &session, (TEEC_UUID*)&destination,
            TEEC_LOGIN_PUBLIC, NULL, NULL, &returnOrigin);
    if (TEEC_SUCCESS != ret )
        goto FinalizeContext;

    smDumpBuffer.buffer = (void *)buf;
    smDumpBuffer.size = len;
    smDumpBuffer.flags = TEEC_MEM_OUTPUT;

    ret = TEEC_RegisterSharedMemory(&context, &smDumpBuffer);
    if (TEEC_SUCCESS != ret)
        goto CloseSession;


    memset((void *)&operation, 0, sizeof(operation));
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_MEMREF_WHOLE);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 1, TEEC_VALUE_INPUT);
    operation.params[0].memref.parent = &smDumpBuffer;
    operation.params[1].value.a = len;

    ret = TEEC_InvokeCommand(&session, HWK_DBG, &operation, &returnOrigin);

    TEEC_ReleaseSharedMemory(&smDumpBuffer);

CloseSession:
    TEEC_CloseSession (&session);
FinalizeContext:
    TEEC_FinalizeContext(&context);
CclibFini:
    DX_CclibFini();

    return ret;
}

static int hwk_keyid_ops(int op, uint32_t key_id)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    static const uint8_t destination[] = HWK_APP_UUID;
    TEEC_Result ret;
    uint32_t returnOrigin = 0;

    ret = DX_CclibInit();
    if (TEEC_SUCCESS != ret)
        return ret;

    ret = TEEC_InitializeContext(NULL, &context);
    if (TEEC_SUCCESS != ret )
        goto CclibFini;

    ret = TEEC_OpenSession(&context, &session, (TEEC_UUID*)&destination,
            TEEC_LOGIN_PUBLIC, NULL, NULL, &returnOrigin);
    if (TEEC_SUCCESS != ret)
        goto FinalizeContext;

    memset((void *)&operation, 0, sizeof(operation));
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_VALUE_INPUT);
    operation.params[0].value.a = key_id;

    ret = TEEC_InvokeCommand(&session, op, &operation, &returnOrigin);

CloseSession:
    TEEC_CloseSession(&session);
FinalizeContext:
    TEEC_FinalizeContext(&context);
CclibFini:
    DX_CclibFini();

    return ret;
}

int hwk_seal(uint32_t key_id)
{
    return hwk_keyid_ops(HWK_SEAL, key_id);
}

int hwk_disable(uint32_t key_id)
{
    return hwk_keyid_ops(HWK_DISABLE, key_id);
}

int hwk_enable(uint32_t key_id)
{
    return hwk_keyid_ops(HWK_ENABLE, key_id);
}

int hwk_destroy(uint32_t key_id)
{
    return hwk_keyid_ops(HWK_DESTROY, key_id);
}

static int hwk_keyid_pass_ops(int op, uint32_t key_id, const char *pwd, int pwd_len)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    static const uint8_t destination[] = HWK_APP_UUID;
    TEEC_Result ret;
    uint32_t returnOrigin = 0;
    TEEC_SharedMemory smPwd;


    ret = DX_CclibInit();
    if (TEEC_SUCCESS != ret)
        return ret;

    ret = TEEC_InitializeContext(NULL, &context);
    if (TEEC_SUCCESS != ret )
        goto CclibFini;

    ret = TEEC_OpenSession(&context, &session, (TEEC_UUID*)&destination,
            TEEC_LOGIN_PUBLIC, NULL, NULL, &returnOrigin);
    if (TEEC_SUCCESS != ret)
        goto FinalizeContext;

    smPwd.buffer = (void *)pwd;
    smPwd.size = pwd_len;
    smPwd.flags = TEEC_MEM_INPUT;

    ret = TEEC_RegisterSharedMemory(&context, &smPwd);
    if (TEEC_SUCCESS != ret)
        goto CloseSession;

    memset((void *)&operation, 0, sizeof(operation));
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_VALUE_INPUT);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 1, TEEC_MEMREF_WHOLE);
    operation.params[0].value.a = key_id;
    operation.params[0].value.b = pwd_len;
    operation.params[1].memref.parent = &smPwd;


    ret = TEEC_InvokeCommand(&session, op, &operation, &returnOrigin);

    TEEC_ReleaseSharedMemory(&smPwd);

CloseSession:
    TEEC_CloseSession(&session);
FinalizeContext:
    TEEC_FinalizeContext(&context);
CclibFini:
    DX_CclibFini();

    return ret;
}

int hwk_create(uint32_t key_id, const char *pwd, int pwd_len)
{
    return hwk_keyid_pass_ops(HWK_CREATE, key_id, pwd, pwd_len);
}

int hwk_unseal(uint32_t key_id, const char *pwd, int pwd_len)
{
    return hwk_keyid_pass_ops(HWK_UNSEAL, key_id, pwd, pwd_len);
}

int hwk_reseal(uint32_t key_id, const char *pwd, int pwd_len)
{
    return hwk_keyid_pass_ops(HWK_RESEAL, key_id, pwd, pwd_len);
}

int hwk_crypto(uint32_t key_id, hwk_crypto_op_t op, const char *iv,
               const char *src, char *dst, int len)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    static const uint8_t destination[] = HWK_APP_UUID;
    TEEC_Result ret;
    uint32_t returnOrigin = 0;
    TEEC_SharedMemory smIv, smSrc, smDst;

    ret = DX_CclibInit();
    if (TEEC_SUCCESS != ret)
        return ret;

    ret = TEEC_InitializeContext(NULL, &context);
    if (TEEC_SUCCESS != ret )
        goto CclibFini;

    ret = TEEC_OpenSession(&context, &session, (TEEC_UUID*)&destination,
            TEEC_LOGIN_PUBLIC, NULL, NULL, &returnOrigin);
    if (TEEC_SUCCESS != ret)
        goto FinalizeContext;

    smIv.buffer = (void *)iv;
    smIv.size = 16;
    smIv.flags = TEEC_MEM_INPUT;
    ret = TEEC_RegisterSharedMemory(&context, &smIv);
    if (TEEC_SUCCESS != ret)
        goto CloseSession;

    smSrc.buffer = (void *)src;
    smSrc.size = len;
    smSrc.flags = TEEC_MEM_INPUT;
    ret = TEEC_RegisterSharedMemory(&context, &smSrc);
    if (TEEC_SUCCESS != ret)
        goto ReleaseSmIv;

    smDst.buffer = (void *)dst;
    smDst.size = len;
    smDst.flags = TEEC_MEM_OUTPUT;
    ret = TEEC_RegisterSharedMemory(&context, &smDst);
    if (TEEC_SUCCESS != ret)
        goto ReleaseSmSrc;

    memset((void *)&operation, 0, sizeof(operation));
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_VALUE_INPUT);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 1, TEEC_MEMREF_WHOLE);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 2, TEEC_MEMREF_WHOLE);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 3, TEEC_MEMREF_WHOLE);
    operation.params[0].value.a = key_id;
    operation.params[0].value.b = len | (op << 16);
    operation.params[1].memref.parent = &smIv;
    operation.params[2].memref.parent = &smSrc;
    operation.params[3].memref.parent = &smDst;

    ret = TEEC_InvokeCommand(&session, HWK_CRYPTO, &operation, &returnOrigin);

    TEEC_ReleaseSharedMemory(&smDst);
ReleaseSmSrc:
    TEEC_ReleaseSharedMemory(&smSrc);
ReleaseSmIv:
    TEEC_ReleaseSharedMemory(&smIv);
CloseSession:
    TEEC_CloseSession(&session);
FinalizeContext:
    TEEC_FinalizeContext(&context);
CclibFini:
    DX_CclibFini();

    return ret;
}
