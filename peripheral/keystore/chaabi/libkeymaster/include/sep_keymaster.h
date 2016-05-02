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

#ifndef __SEP_TEE_KEYMASTER_H__
#define __SEP_TEE_KEYMASTER_H__

#include <stdint.h>

typedef enum
{
    SEP_KEYMASTER_SUCCESS = 0,
    SEP_KEYMASTER_BAD_PARAMETER,
    SEP_KEYMASTER_CMD_BUFFER_TOO_BIG,
    SEP_KEYMASTER_RSP_BUFFER_TOO_SMALL,
    SEP_KEYMASTER_CC_INIT_FAILURE,
    SEP_KEYMASTER_FAILURE
} sep_keymaster_return_t;


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Send a keymaster command to sep, and waits for a response
 * \param cmd_buffer :          Pointer to command buffer
 * \param cmd_length :          Number of command bytes
 * \param rsp_buffer :          Pointer to response buffer
 * \param rsp_length [IN/OUT]:  [IN] sizeof response buffer, [OUT] actual size of response
 * \return SEP_KEYMASTER_CMD_SUCCESS on success
 */
sep_keymaster_return_t sep_keymaster_send_cmd(const uint8_t * const cmd_buffer,
                                              uint32_t cmd_length,
                                              uint8_t * const rsp_buffer,
                                              uint32_t * const rsp_length);

/*!
 * Generates public and private RSA keys and returns them and their sizes
 * \param pub_key_size [OUT]:   Size of the public key in DER format
 * \param pub_key [OUT]:        Pointer to the public key buffer, must be freed by caller!
 * \param prv_key_size [OUT]:   Size of the key blob containing encrypted private key.
 * \param prv_key [OUT]:        Pointer to the key blob buffer containing encrypted private key.
                                Must be freed by caller!
 * \return SEP_KEYMASTER_SUCCESS on success
 */
sep_keymaster_return_t tee_token_gen_shared_rsa(size_t *pub_key_size,
                                                uint8_t **pub_key,
                                                size_t *prv_key_size,
                                                uint8_t **prv_key);
#ifdef __cplusplus
}
#endif


#endif
