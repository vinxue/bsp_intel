/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "tee_applet_loading_if.h"
#include "tee_client_api.h"
#include "dx_cclib.h"

static const uint8_t primary_applet_uuid[] = {0x00, 0xDE, 0xFA, 0x01, 0xDE, 0xFA, 0x02, 0xDE,
                                              0xFA, 0x03, 0xDE, 0xFA, 0x04, 0xDE, 0xFA, 0xFF};

#define CMD_LOAD_APPLET     0x8000
#define CMD_UNLOAD_APPLET   0x8001

// tee_load_applet
int32_t tee_load_applet(uint8_t *img_buf, uint32_t img_size)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result ret;
    TEEC_SharedMemory input;
    uint32_t retOrigin = 0;

    memset((void *)&context, 0, sizeof(context));
    memset((void *)&session, 0, sizeof(session));
    memset((void *)&operation, 0, sizeof(operation));
    memset((void *)&input, 0, sizeof(input));

    ret = TEEC_InitializeContext(NULL, &context);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "load applet init failed: (%x)\n", ret);
        goto end;
    }

    ret = TEEC_OpenSession(&context, &session, (TEEC_UUID *)&primary_applet_uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &retOrigin);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "session open to primary failed: (%x)\n", ret);
        goto end;
    }

    input.size = img_size;
    input.flags = TEEC_MEM_INPUT;
    input.buffer = img_buf;

    ret = TEEC_RegisterSharedMemory(&context, &input);
    if (ret != TEEC_SUCCESS)
    {
        printf("TEEC_RegisterSharedMemory failed, (%x)\n", ret);
        goto end;
    }

    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_MEMREF_WHOLE);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 1, TEEC_VALUE_INPUT);

    operation.params[0].memref.parent = &input;
    operation.params[1].value.a = img_size;

    retOrigin = TEEC_ORIGIN_TRUSTED_APP;

    ret = TEEC_InvokeCommand(&session, (uint32_t)CMD_LOAD_APPLET, &operation, &retOrigin);
    if (ret != TEEC_SUCCESS)
    {
        printf("TEEC_InvokeCommand call failed (%x)\n", ret);
        goto end;
    }

end:
    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);

    return ret;
}

// tee_unload_applet
int32_t tee_unload_applet(TEEC_UUID *applet_uuid)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Result ret = 0;
    uint32_t retOrigin = 0;

    ret = TEEC_InitializeContext(NULL, &context);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "unload applet init failed: (%x)\n", ret);
        goto end;
    }

    ret = TEEC_OpenSession(&context, &session, (TEEC_UUID *)&primary_applet_uuid,
                              TEEC_LOGIN_PUBLIC, NULL, NULL, &retOrigin);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "session open to primary failed: (%x)\n", ret);
        goto end;
    }

    TEEC_Operation operation_unload;
    memset((void *)&operation_unload, 0, sizeof(TEEC_Operation));

    // for applet uuid
    TEEC_SharedMemory input_unloadable_applet_uuid;
    memset((void *)&input_unloadable_applet_uuid, 0, sizeof(TEEC_SharedMemory));

    input_unloadable_applet_uuid.size = sizeof(TEEC_UUID);
    input_unloadable_applet_uuid.flags = TEEC_MEM_INPUT;
    input_unloadable_applet_uuid.buffer = (uint8_t *)applet_uuid;

    ret = TEEC_RegisterSharedMemory(&context, &input_unloadable_applet_uuid);
    if (ret != TEEC_SUCCESS)
    {
        printf("TEEC_RegisterSharedMemory failed (%x)\n", ret);
        goto end;
    }

    TEEC_PARAM_TYPE_SET(operation_unload.paramTypes, 0, TEEC_MEMREF_WHOLE);
    operation_unload.params[0].memref.parent = &input_unloadable_applet_uuid;

    retOrigin = TEEC_ORIGIN_TRUSTED_APP;
    ret = TEEC_InvokeCommand(&session, (uint32_t)CMD_UNLOAD_APPLET, &operation_unload, &retOrigin);
    if (ret != TEEC_SUCCESS)
    {
        printf("TEEC_InvokeCommand call failed (%x)\n", ret);
        goto end;
    }

end:
    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);

    return ret;
}
