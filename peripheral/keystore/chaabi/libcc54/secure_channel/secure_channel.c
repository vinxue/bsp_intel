/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#include "secure_channel.h"
#include "tee_client_api.h"
#include "dx_cclib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*!
 * \brief applet_uuid
 * ID of the Secure Channel applet that we are communicating with
 */
static const uint8_t applet_uuid[] =
        {'I', 'N', 'T', 'E', 'L', '_', 'S', 'E', 'C', 'C', 'H', 'A', 'N', 'N', 'E', 'L'};

/*!
 * \brief Prosses a request that is received from the modem.
 */
#define PROCESS_SEC_REQUEST               1

/*!
 * \brief After the data has been processed we must retrieve the response from SEP
 */
#define COLLECT_RESPONSE                  2

/*!
 * \brief Error code to define that we already have one client connected in a session.
 */
#define ERROR_ALREADY_IN_SESSION          1

/*!
 * \brief The requested command operation is not valid.
 */
#define ERROR_INVALID_COMMAND             2

/*!
 * \brief We receive this message type when we are instructed to free our internal buffer
 */
#define SECURE_CH_DATA_FREE_RETURN_DATA   0

/*!
 * \brief The SNUM value is sent on request for the certificates to be returned
 */
#define SECURE_CH_DATA_SNUM               1

/*!
 * \brief this is the 16 bit value that snum is understood by when received by the applet
 */
#define SNUM_ENMU_VALUE                   0x9999

/*!
  * \brief The Type Length header is 4 bytes 2 for each component
  */
#define TL_SIZE                           4

/*!
 * \brief out_buffer
 */
static uint8_t *out_buffer;

/*!
 * \brief preserveBuffer
 * We should try and keep the buffer allocated until we are requested to free it
 */
static bool preserveBuffer = false;

/*!
 * \brief reset_buffer
 *  Reset our internal output buffer after the data has been consumed.
 */
static void reset_buffer()
{
    if (preserveBuffer)
        return;

    free(out_buffer);
    out_buffer = NULL;
}

uint32_t secure_channel_callback(uint32_t *type, uint32_t *length, uint8_t **data)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result ret;
    TEEC_SharedMemory input;
    TEEC_SharedMemory output;
    uint32_t retOrigin = 0;
    uint32_t required_length = 0;
    uint8_t *snumBuf = 0;

    memset((void *)&operation, 0, sizeof(operation));
    memset((void *)&input, 0, sizeof(input));
    memset((void *)&output, 0, sizeof(output));

    if (type && *type == SECURE_CH_DATA_FREE_RETURN_DATA)
    {
        // This command tells us to clean up our previously allocated memory
        preserveBuffer = false;
        reset_buffer();
        fprintf(stderr, "Sec_Lib: Cleaned the internal buffer\n");
        return 0;
    }
    else if (preserveBuffer == true)
    {
        // We have not cleaned up the buffer from the previous time
        // so an error has occured.  If we continued a memory leak would occur.
        fprintf(stderr, "Sec_Lib: Internal buffer not cleaned before subsequent calls\n");
        return 100;
    }

    if (!type || !length || !data || *length == 0 || *data == NULL)
    {
        fprintf(stderr, "Sec_Lib: Invalid data passed as paramaters\n");
        return 101;
    }

    if (*type == SECURE_CH_DATA_SNUM)
    {
        // Turn the snum value into a TLV structure
        snumBuf = malloc(TL_SIZE + *length);
        if (!snumBuf)
        {
            fprintf(stderr, "Sec_Lib: No memory to hold SNUM TLV request\n");
            return 102;
        }

        *((uint16_t *)&(snumBuf)[0]) = SNUM_ENMU_VALUE;
        *((uint16_t *)&(snumBuf)[2]) = *length;
        memcpy(snumBuf + TL_SIZE, *data, *length);
        *data = (uint8_t *)snumBuf;
        *length += TL_SIZE; //the length of the array in bytes (4 bytes for TLV header)
    }

    ret = DX_CclibInit();
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "Sec_Lib: failed to initialize libcc54 interface (0x%x)\n", ret);
        goto FinLib;
    }

    ret = TEEC_InitializeContext(NULL, &context);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "Sec_Lib: Failed to Initialize context (0x%x)\n", ret);
        goto FinContext;
    }

    ret = TEEC_OpenSession(&context, &session,  (TEEC_UUID*)&applet_uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &retOrigin);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "Sec_Lib: Failed to open session (0x%x)\n", ret);
        goto FinSession;
    }

    // Map the input buffer and register it as shared memory
    input.size = *length;
    input.flags = TEEC_MEM_INPUT;
    input.buffer = *data;

    ret = TEEC_RegisterSharedMemory(&context, &input);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "Sec_lib: Failed to register shared mem {input} (0x%x)\n", ret);
        goto FinInput;
    }

    // Create an operation to be performed by SEP
    // TODO DX should provide the macro TEEC_PARAM_TYPES for portability
    // see 4.5.11 of spec
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_VALUE_INPUT); // size of input buffer
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 1, TEEC_MEMREF_WHOLE); // the input buffer
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 2, TEEC_VALUE_OUTPUT); // the size needed for output

    operation.params[0].value.a = *length;
    operation.params[1].memref.parent = &input;

    ret = TEEC_InvokeCommand(&session, PROCESS_SEC_REQUEST, &operation, &retOrigin);
    if (ret != TEEC_SUCCESS || operation.params[2].value.a == 0)
    {
        fprintf(stderr, "Sec_Lib: Failed to invoke 1st command (0x%x)\n", ret);
        goto FinInput; // either an error or there is no response
    }

    required_length = operation.params[2].value.a;

    //create a buffer to hold the response
    out_buffer = (uint8_t *) malloc(required_length * sizeof(uint8_t));
    if (out_buffer == NULL)
    {
        fprintf(stderr, "Sec_Lib: Failed to malloc space {output}\n");
        goto FinInput;
    }

    //update the output shared memory and register it as a shared buffer
    output.size = required_length;
    output.flags = TEEC_MEM_OUTPUT;
    output.buffer = out_buffer;

    ret = TEEC_RegisterSharedMemory(&context, &output);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "Sec_Lib: Failed to register shared mem {output} (0x%x)\n", ret);
        goto FinOutput;
    }

    // Create a second operation to be performed by SEP
    memset((void *)&operation, 0, sizeof(operation));
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_VALUE_INPUT); // size of output buffer
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 1, TEEC_MEMREF_WHOLE); // Output_buffer
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 2, TEEC_VALUE_OUTPUT); // Type of the response
    operation.params[0].value.a = required_length;
    operation.params[1].memref.parent = &output;

    // Call the functionality to collect the response from SEP
    ret = TEEC_InvokeCommand(&session, COLLECT_RESPONSE, &operation, &retOrigin);
    if (ret != TEEC_SUCCESS)
    {
        fprintf(stderr, "Sec_Lib: Failed to invoke 2nd command (0x%x)\n", ret);
        goto FinOutput;
    }

    //Update the outout so the response is visible to the caller
    *length = required_length;
    *data = out_buffer;
    *type = operation.params[2].value.a;
    preserveBuffer = true;

FinOutput:
    reset_buffer();
    TEEC_ReleaseSharedMemory(&output);

FinInput:
    TEEC_ReleaseSharedMemory(&input);

FinSession:
    TEEC_CloseSession(&session);

FinContext:
    TEEC_FinalizeContext(&context);

FinLib:
    DX_CclibFini();

    if (snumBuf)
        free(snumBuf);

    if (ret == TEEC_SUCCESS)
        fprintf(stderr, "Sec_Lib: Command (0x%x) completed SUCCESS\n", *type);

    return ret;
}
