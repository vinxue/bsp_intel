/*************************************************************************
** Copyright (C) 2013-2016 Intel Corporation. All rights reserved.          **
**                                                                     **
** Permission is hereby granted, free of charge, to any person         **
** obtaining a copy of this software and associated documentation      **
** files (the "Software"), to deal in the Software without             **
** restriction, including without limitation the rights to use, copy,  **
** modify, merge, publish, distribute, sublicense, and/or sell copies  **
** of the Software, and to permit persons to whom the Software is      **
** furnished to do so, subject to the following conditions:            **
**                                                                     **
** The above copyright notice and this permission notice shall be      **
** included in all copies or substantial portions of the Software.     **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,     **
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF  **
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND               **
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS **
** BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN  **
** ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN   **
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    **
** SOFTWARE.                                                           **
*************************************************************************/

#include "sep_keymaster.h"
#include "fw_interface.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tee_client_api.h"
#include "dx_cclib.h"

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>

#include <cutils/log.h>

static const uint8_t keystore_app_uuid[] = "INTELKEYSTOREAPP";
#define TEE_INVALID_PARAMETER     0xCAB15002
#define TEE_INVALID_SIZE          0xCAB15003
#define PUBKEY_BUF_SZ             1024

/*!
 * \brief send_to_sep Package a buffer for delivery to sep and invoke a command and
 * return the result of the invoked command
 * \param cmd_id The id of the command the is to be executed in sep FW
 * \param in_buf The input buffer that is to be passed to sep
 * \param in_buf_size the size of the buffer
 * \param out_buf The buffer in which to store the response
 * \param out_buf_size [IN/OUT] [IN] the size available in out_buf, [OUT] The actual size of out_buf
 * \return SEP_KEYMASTER_SUCCESS on success
 */
static int32_t send_to_sep(uint32_t cmd_id, uint8_t *in_buf, uint32_t in_buf_size,
                           uint8_t *out_buf, uint32_t *out_buf_size)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result ret;
    TEEC_SharedMemory input;
    TEEC_SharedMemory output;
    uint32_t retOrigin = 0;
    TEEC_UUID applet_uuid;

    memset((void *)&operation, 0, sizeof(operation));
    memset((void *)&input, 0, sizeof(input));
    memset((void *)&output, 0, sizeof(output));
    memset((void *)&context, 0, sizeof(context));
    memcpy(&applet_uuid, &keystore_app_uuid, 16);

    ret = DX_CclibInit();
    if (ret != TEEC_SUCCESS)
    {
        ret = SEP_KEYMASTER_CC_INIT_FAILURE;
        goto FinLib;
    }

    ret = TEEC_InitializeContext(NULL, &context);
    if (ret != TEEC_SUCCESS)
    {
        ret = SEP_KEYMASTER_CC_INIT_FAILURE;
        goto FinContext;
    }

    ret = TEEC_OpenSession(&context, &session, &applet_uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &retOrigin);
    if (ret != TEEC_SUCCESS)
    {
        ret = SEP_KEYMASTER_CC_INIT_FAILURE;
        goto FinSession;
    }

    // Map the input buffer and register it as shared memory
    input.size = in_buf_size;
    input.flags = TEEC_MEM_INPUT;
    input.buffer = in_buf;

    ret = TEEC_RegisterSharedMemory(&context, &input);
    if (ret != TEEC_SUCCESS)
    {
        ret = SEP_KEYMASTER_CC_INIT_FAILURE;
        goto FinInput;
    }

    //update the output shared memory and register it as a shared buffer
    output.size = *out_buf_size;
    output.flags = TEEC_MEM_OUTPUT;
    output.buffer = out_buf;

    ret = TEEC_RegisterSharedMemory(&context, &output);
    if (ret != TEEC_SUCCESS)
    {
        ret = SEP_KEYMASTER_CC_INIT_FAILURE;
        goto FinOutput;
    }

    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_VALUE_INPUT); // size of input buffer
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 1, TEEC_MEMREF_WHOLE); // the input buffer
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 2, TEEC_VALUE_INOUT); // the size needed for output
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 3, TEEC_MEMREF_WHOLE); // the output buffer

    operation.params[0].value.a = in_buf_size;
    operation.params[1].memref.parent = &input;
    operation.params[2].value.a = *out_buf_size;
    operation.params[3].memref.parent = &output;

    ret = TEEC_InvokeCommand(&session, cmd_id, &operation, &retOrigin);
    if (ret != TEEC_SUCCESS)
    {
        if (ret == TEE_INVALID_PARAMETER)
            ret = SEP_KEYMASTER_BAD_PARAMETER;
        else if (ret == TEE_INVALID_SIZE)
            ret = SEP_KEYMASTER_RSP_BUFFER_TOO_SMALL;
        else
            ret = SEP_KEYMASTER_FAILURE;
        goto FinOutput;
    }

    // Ensure to tell the caller the actual size of the response
    *out_buf_size = operation.params[2].value.a;

FinOutput:
    TEEC_ReleaseSharedMemory(&output);

FinInput:
    TEEC_ReleaseSharedMemory(&input);

FinSession:
    TEEC_CloseSession(&session);

FinContext:
    TEEC_FinalizeContext(&context);

FinLib:
    DX_CclibFini();

    return ret;
}

/*!
 * \brief generate_import_key
 * Handle requests to generate or import a key.  the function takes the raw command and returns
 * a properly formatted response message.
 * \param cb_layout The raw message received from the caller, intrepreted as struct raw_cmd_layout
 * \param res_layout The raw response buffer, intrepreted as struct raw_resp_layout
 * \param rsp_length [IN/OUT] [IN] the size available in the response buf, [OUT] actual size uzed
 * \return sep_keymaster_return_t, depending on status
 */
static sep_keymaster_return_t generate_import_key(struct raw_cmd_layout *cb_layout,
                                                  struct raw_resp_layout *res_layout,
                                                  uint32_t *rsp_length)
{
    struct keymaster_import_gen_request *imp_gen;
    sep_keymaster_return_t ret;

    if (cb_layout->cmd_length < sizeof(struct keymaster_import_gen_request))
        return SEP_KEYMASTER_BAD_PARAMETER;

    imp_gen = (struct keymaster_import_gen_request *)cb_layout->buf;

    if (imp_gen->key_type != KEY_TYPE_RSA)
        return SEP_KEYMASTER_BAD_PARAMETER;

    ret = send_to_sep(cb_layout->cmd_id, imp_gen->key_data,
                      cb_layout->cmd_length - sizeof(int32_t), //Just pass the key material to sep
                      res_layout->buf,
                      rsp_length);

    return ret;
}

sep_keymaster_return_t sep_keymaster_send_cmd(const uint8_t * const cmd_buffer,
                                              uint32_t cmd_length,
                                              uint8_t * const rsp_buffer,
                                              uint32_t * const rsp_length)
{
    int32_t ret = SEP_KEYMASTER_SUCCESS;
    struct raw_cmd_layout *cb_layout;
    struct raw_resp_layout *res_layout;
    uint32_t in_out_resp_size;

    if (cmd_buffer == NULL || cmd_length < sizeof(struct raw_cmd_layout) ||
            cmd_length > INTEL_KEYMASTER_MAX_MESSAGE_SIZE ||
            rsp_buffer == NULL || rsp_length == NULL ||
            *rsp_length < sizeof(struct raw_resp_layout))
        return SEP_KEYMASTER_BAD_PARAMETER;

    cb_layout = (struct raw_cmd_layout *)cmd_buffer;
    res_layout = (struct raw_resp_layout *)rsp_buffer;
    in_out_resp_size = *rsp_length - sizeof(struct raw_resp_layout);

    switch (cb_layout->cmd_id)
    {
        case KEYMASTER_CMD_GENERATE_KEYPAIR:
        case KEYMASTER_CMD_GENERATE_KEYPAIR_GKEK:
        case KEYMASTER_CMD_IMPORT_KEYPAIR:
            ret = generate_import_key(cb_layout, res_layout, &in_out_resp_size);
            break;
        case KEYMASTER_CMD_GET_KEYPAIR_PUBLIC:
        case KEYMASTER_CMD_GET_KEYPAIR_PUBLIC_GKEK:
        case KEYMASTER_CMD_DELETE_ALL:
        case KEYMASTER_CMD_SIGN_DATA:
        case KEYMASTER_CMD_VERIFY_DATA:
            ret = send_to_sep(cb_layout->cmd_id, cb_layout->buf, cb_layout->cmd_length,
                              res_layout->buf, &in_out_resp_size);
            break;
        case KEYMASTER_CMD_DELETE_KEYPAIR:
            // we do not have to do anything for an individual key
            ret = SEP_KEYMASTER_SUCCESS;
            in_out_resp_size = 0;
            break;
        default:
            ret = SEP_KEYMASTER_BAD_PARAMETER;
            break;
    }

    res_layout->resp_id = cb_layout->cmd_id | KEYMASTER_RSP_FLAG;
    // the size of the result field is included response length
    res_layout->resp_length = in_out_resp_size + sizeof(uint32_t);
    res_layout->result_code = ret;
    // the overall response is the data buffer + the default headers
    *rsp_length = in_out_resp_size + sizeof(struct raw_resp_layout);

    return ret;
}

static sep_keymaster_return_t sep_keymaster_unwrap_pubkey(const uint8_t *keyBlob,
                                                          const size_t keyBlobLength,
                                                          uint32_t *pubkey_len,
                                                          uint8_t **pubkey_ptr)
{
    struct raw_cmd_layout *cmd_buf;
    uint8_t cmd_buf_raw[INTEL_KEYMASTER_MAX_MESSAGE_SIZE];
    struct raw_resp_layout *rsp_buf;
    uint8_t rsp_buf_raw[INTEL_KEYMASTER_MAX_MESSAGE_SIZE];
    struct length_data_generic *key_blob;
    uint32_t cmd_len, rsp_len;
    sep_keymaster_return_t sep_ret;
    struct keymaster_import_gen_request *public_key;
    struct keymaster_get_keypair_data *rsa_public_key;
    uint32_t rsp_result_and_data_length;
    BIO *out = NULL;
    RSA *rsa = NULL;
    EVP_PKEY *pkey = NULL;
    char buf[256] = {0};

    if ((keyBlob == NULL) || (keyBlobLength == 0))
    {
        return SEP_KEYMASTER_BAD_PARAMETER;
    }

    cmd_buf = (struct raw_cmd_layout *)cmd_buf_raw;
    rsp_buf = (struct raw_resp_layout *)rsp_buf_raw;

    cmd_buf->cmd_id = KEYMASTER_CMD_GET_KEYPAIR_PUBLIC_GKEK;
    cmd_buf->cmd_length = keyBlobLength + sizeof(struct length_data_generic);
    cmd_len = cmd_buf->cmd_length + sizeof(struct raw_resp_layout);

    if (cmd_len > INTEL_KEYMASTER_MAX_MESSAGE_SIZE)
    {
        ALOGE("keyBlob is too big, keyBlobLength = %u", keyBlobLength);
        return SEP_KEYMASTER_CMD_BUFFER_TOO_BIG;
    }

    key_blob = (struct length_data_generic *)cmd_buf->buf;
    key_blob->length = keyBlobLength;
    memcpy(key_blob->data, keyBlob, keyBlobLength);

    rsp_len = INTEL_KEYMASTER_MAX_MESSAGE_SIZE;

    sep_ret = sep_keymaster_send_cmd((uint8_t *)cmd_buf, cmd_len, (uint8_t *)rsp_buf, &rsp_len);
    if (sep_ret != SEP_KEYMASTER_SUCCESS)
    {
        ALOGE("sep_keymaster_send_cmd() returned error %d", sep_ret);
        return SEP_KEYMASTER_FAILURE;
    }

    // response should contain at least rsp_id, rsp_result_and_data_length and result
    if (rsp_len < offsetof(struct raw_resp_layout, buf))
    {
        ALOGE("response is too short, rsp_len = %u", rsp_len);
        return SEP_KEYMASTER_RSP_BUFFER_TOO_SMALL;
    }

    // rsp_result_and_data_length should include at least the result field
    if (rsp_buf->resp_length < sizeof(sep_keymaster_return_t))
    {
        ALOGE("rsp_result_and_data_length too small, rsp_result_and_data_length = %u",
              rsp_buf->resp_length);
        return SEP_KEYMASTER_RSP_BUFFER_TOO_SMALL;
    }

    // the result must indicate success
    if (rsp_buf->result_code != SEP_KEYMASTER_SUCCESS)
    {
        ALOGE("firmware returned result is not successful, result = 0x%x", rsp_buf->result_code);
        return SEP_KEYMASTER_FAILURE;
    }

    public_key = (struct keymaster_import_gen_request *)rsp_buf->buf;

    if (public_key->key_type != KEY_TYPE_RSA)
    {
        ALOGE("key type other than RSA is not yet supported");
        return SEP_KEYMASTER_FAILURE;
    }

    rsa_public_key = (struct keymaster_get_keypair_data *)public_key->key_data;

    rsp_result_and_data_length = rsa_public_key->modulus_size +
                                 rsa_public_key->pub_exp_size +
                                 offsetof(struct keymaster_get_keypair_data, buffer) +
                                 offsetof(struct keymaster_import_gen_request, key_data) +
                                 sizeof(sep_keymaster_return_t);
    if (rsp_result_and_data_length != rsp_buf->resp_length)
    {
        ALOGE("rsp_result_and_data_length %u does not match rsp_result_and_data_length %u",
              rsp_result_and_data_length, rsp_buf->resp_length);
        return SEP_KEYMASTER_FAILURE;
    }

    if (rsp_buf->resp_length + offsetof(struct raw_resp_layout, result_code) != rsp_len)
    {
        ALOGE("rsp_result_and_data_length %u is inconsistent with rsp_len %u",
              rsp_buf->resp_length, rsp_len);
        return SEP_KEYMASTER_FAILURE;
    }

    // convert to PEM encoded format
    rsa = RSA_new();
    if (rsa == NULL)
    {
        ALOGE("Memory allocation for rsa failed");
        return SEP_KEYMASTER_FAILURE;
    }

    rsa->n = BN_bin2bn(rsa_public_key->buffer, rsa_public_key->modulus_size, NULL);
    if (rsa->n == NULL)
    {
        ALOGE("Couldn't create modulus from public key byte array");
        sep_ret = SEP_KEYMASTER_FAILURE;
        goto out;
    }

    rsa->e = BN_bin2bn(rsa_public_key->buffer + rsa_public_key->modulus_size,
                       rsa_public_key->pub_exp_size, NULL);
    if (rsa->e == NULL)
    {
        ALOGE("Couldn't create public exponent from public key byte array");
        sep_ret = SEP_KEYMASTER_FAILURE;
        goto out;
    }

    pkey = EVP_PKEY_new();
    if (pkey == NULL)
    {
        ALOGE("Memory allocation for pkey failed");
        sep_ret = SEP_KEYMASTER_FAILURE;
        goto out;
    }

    if (!EVP_PKEY_assign_RSA(pkey, rsa))
    {
        ALOGE("Couldn't assign rsa to pkey\n");
        sep_ret = SEP_KEYMASTER_FAILURE;
        goto out;
    }

    out = BIO_new(BIO_s_mem());
    if (out == NULL)
    {
        ALOGE("Couldn't create a new bio\n");
        sep_ret = SEP_KEYMASTER_FAILURE;
        goto out;
    }

    if (PEM_write_bio_PUBKEY(out, pkey) != 1)
    {
        ALOGE("Couldn't write pkey to bio\n");
        sep_ret = SEP_KEYMASTER_FAILURE;
        goto out;
    }

    *pubkey_ptr = calloc(PUBKEY_BUF_SZ, 1);
    if (*pubkey_ptr == NULL)
    {
        ALOGE("Could not assign menory\n");
        sep_ret = SEP_KEYMASTER_FAILURE;
        goto out;
    }

    size_t available = PUBKEY_BUF_SZ;

    // While there is Data in BIO, read line by line. Public cannot be larger that Private.
    while (BIO_gets(out, buf, sizeof(buf)) > 0)
    {
        if (available >= strlen(buf))
        {
            strcat((char *)*pubkey_ptr, buf);
            available -= strlen(buf);
        }
        else
        {
            ALOGE("Insufficient space to public key\n");
            sep_ret = SEP_KEYMASTER_FAILURE;
            goto out;
        }
    }

    *pubkey_len = strlen((const char *)*pubkey_ptr);

out:
    BIO_free_all(out);
    EVP_PKEY_free(pkey);
    return 0;
}

sep_keymaster_return_t tee_token_gen_shared_rsa(size_t *pub_key_size,
                                                uint8_t **pub_key,
                                                size_t *prv_key_size,
                                                uint8_t **prv_key)
{
    struct raw_cmd_layout *cmd_layout;
    struct keymaster_import_gen_request *req_body;
    struct keymaster_rsa_keygen_params *key_config;
    struct raw_resp_layout *resp;
    struct length_data_generic *blob_load;
    uint32_t ret;
    int32_t req_size = sizeof(struct raw_cmd_layout) +
                       sizeof(struct keymaster_import_gen_request) +
                       sizeof(struct keymaster_rsa_keygen_params);

    uint8_t req_buff[req_size];

    //create the overall request structure
    cmd_layout = (struct raw_cmd_layout *)req_buff;
    cmd_layout->cmd_id = KEYMASTER_CMD_GENERATE_KEYPAIR_GKEK;
    cmd_layout->cmd_length = sizeof(struct keymaster_import_gen_request) +
                             sizeof(struct keymaster_rsa_keygen_params);

    //define the type of request
    req_body = (struct keymaster_import_gen_request *)cmd_layout->buf;
    req_body->key_type = KEY_TYPE_RSA;

    // define the properties of the key that we are going to generate
    key_config = (struct keymaster_rsa_keygen_params *)req_body->key_data;
    key_config->modulus_size = 2048;
    key_config->public_exponent = 0x10001;

    *pub_key_size = 2048;
    *pub_key = malloc(*pub_key_size);
    if (*pub_key == NULL)
    {
        ALOGE("failed to malloc public key\n");
        return SEP_KEYMASTER_FAILURE;
    }

    *prv_key_size = INTEL_KEYMASTER_MAX_MESSAGE_SIZE;
    *prv_key = malloc(*prv_key_size);
    if (*prv_key == NULL)
    {
        ALOGE("Allocating key blob failed\n");
        return SEP_KEYMASTER_FAILURE;
    }

    ret = sep_keymaster_send_cmd((uint8_t *)cmd_layout, req_size, *prv_key, prv_key_size);
    if (ret != SEP_KEYMASTER_SUCCESS)
    {
        ALOGE("sep_keymaster_send_cmd failed 0x%08X\n", ret);
        return SEP_KEYMASTER_FAILURE;
    }

    // remove the response transport headers from the keyblob so we point to the actual key blob
    resp = (struct raw_resp_layout *)*prv_key;
    blob_load = (struct length_data_generic *)resp->buf;

    *prv_key_size -= sizeof(struct raw_resp_layout) + sizeof(struct length_data_generic);
    memmove(*prv_key, blob_load->data, *prv_key_size);

    return sep_keymaster_unwrap_pubkey(*prv_key, *prv_key_size, pub_key_size, pub_key);
}
