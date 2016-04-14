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

#ifndef __TEE_SEP_FW_INTERFACE_KEYSTORE_H__
#define __TEE_SEP_FW_INTERFACE_KEYSTORE_H__

#include <stdint.h>

#define SYM_KEY_SIZE_BYTES   32
#define HMAC_SIZE_BYTES      32
#define IVV_SIZE_BYTES       16
#define BLOB_LAYOUT_VERSION   1
#define INTEL_KEYMASTER_MAX_MESSAGE_SIZE    4000

enum keymaster_key_type
{
    KEY_TYPE_RSA = 1,
    KEY_TYPE_DSA,
    KEY_TYPE_EC
};

/*!
 * Parameters needed to generate an RSA key.
 */
struct keymaster_rsa_keygen_params
{
    uint32_t modulus_size;
    uint32_t reserved;
    uint64_t public_exponent;
};

/*!
 * \brief The keymaster_import_key_data struct
 * Paramaters that are required to import a key pair
 */
struct keymaster_import_key_data
{
    uint32_t modulus_size;     /*!< The size of the modulus in bytes */
    uint32_t pub_exp_size;     /*!< The size of the public exponent size */
    uint32_t priv_exp_size;    /*!< The size of the private exponent size */
    uint32_t buffer[];         /*!< A concatination of modulus, public and priv exponents */
};

/*!
 * \brief The keymaster_import_gen_request struct
 * This struct defines the layout of a key import and key generate request as received from the
 * higherlevel keystore
 */
struct keymaster_import_gen_request
{
   enum keymaster_key_type  key_type;    /*!< The type of the key material that follows */
   uint8_t                  key_data[];  /*!< RSA keygen = struct keymaster_rsa_keygen_params
                                              RSA import_key = struct keymaster_import_key_data */
};

struct keymaster_get_keypair_data
{
    uint32_t modulus_size;
    uint32_t pub_exp_size;
    uint8_t  buffer[];
};

/*!
 * \brief The keymaster_signing_cmd_data struct
 * Structure passed from userspace containing the data needed for signing
 */
struct keymaster_signing_cmd_data
{
    uint32_t key_blob_length;  /*!< The length of the key blob */
    uint32_t data_length;      /*!< The length of the data to be signed */
    uint32_t buffer[];         /*!< A concatination of key blob and data block to be signed */
};

/*!
 * \brief The length_data_generic struct
 * A generic structure to hold the length of a piece of data and the data itself
 */
struct length_data_generic
{
    uint32_t length;   /*!< The length of the data */
    uint8_t data[];    /*!< The generic data */
};

enum intel_keymaster_cmd
{
    KEYMASTER_CMD_GENERATE_KEYPAIR = 1,
    KEYMASTER_CMD_IMPORT_KEYPAIR,
    KEYMASTER_CMD_GET_KEYPAIR_PUBLIC,
    KEYMASTER_CMD_DELETE_KEYPAIR,
    KEYMASTER_CMD_DELETE_ALL,
    KEYMASTER_CMD_SIGN_DATA,
    KEYMASTER_CMD_VERIFY_DATA,
    KEYMASTER_CMD_GENERATE_KEYPAIR_GKEK = 101,
    KEYMASTER_CMD_GET_KEYPAIR_PUBLIC_GKEK,
    KEYMASTER_RSP_FLAG = 0x80000000 // responses are flagged with cmd_id with msb changed to 1
};

/*!
 * \brief The key_blob_layout struct
 */
struct key_blob_layout
{
    uint8_t  magic[4];                    /*!< "KBLB" */
    uint32_t version;                     /*!< Blob layout version */
    uint8_t  hmac[HMAC_SIZE_BYTES];       /*!< HMAC calculated over the key blob */
    uint8_t  wrapKey[SYM_KEY_SIZE_BYTES]; /*!< Wrapping key encrypted with KEK */
    uint8_t  iv[IVV_SIZE_BYTES];          /*!< IV for the wrapping key */
    uint32_t key_type;                    /*!< The type of key contained in the blob (RSA) */
    uint32_t exponent_size;               /*!< The size of the exponent buffer in bytes*/
    uint32_t modulus_size;                /*!< The size of the modulus buffer in bytes */
    uint32_t priv_key_size;               /*!< The size of the private key buffer in bytes */
    uint32_t key_data[];                  /*!< Exponent, modulus, and encrypted private key */
};

/*!
 * \brief The raw_cmd_layout struct the layout of the command buffer that is received from the HAL
 */
struct raw_cmd_layout
{
    uint32_t cmd_id;        /*!< The command to be executed */
    uint32_t cmd_length;    /*!< The lenght of the buffer that follows */
    uint8_t buf[];          /*!< The main body of the command */
};

struct raw_resp_layout
{
    uint32_t resp_id;        /*!< The result id of the executed command */
    uint32_t resp_length;    /*!< The lenght of the buffer that follows */
    uint32_t result_code;    /*!< The result status of the command */
    uint8_t buf[];           /*!< The response data of the call */
};

#endif
