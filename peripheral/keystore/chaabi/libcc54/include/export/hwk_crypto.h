/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef HWK_CRYPTO_H
#define HWK_CRYPTO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Create a new key
 *
 * Creates a new hardware key, encrypts it with the given password,
 * associates it with the given id and saves it the permanent secure
 * storage.
 *
 * At this point the key is not ready to use, the user must call
 * hwk_unseal() to decrypt the key and make it available for encryption
 * operations.
 *
 * \param key_id [IN] Identifier for the key.
 *
 * \param pwd [IN] The password used to encrypt the key.
 *
 * \param pwd_len [IN] The password length.
 *
 * \returns 0 on SUCCESS, any other value on error.
 */
int hwk_create(uint32_t key_id, const char *pwd, int pwd_len);

/*!
 * \brief Unseal the key
 *
 * Retrieves the encrypted key from secure storage and makes it ready
 * for the crypto operations by decrypting the key with the given
 * password.
 *
 * \param key_id [IN] Identifier for the key.
 *
 * \param pwd [IN] The password used to encrypt the key.
 *
 * \param pwd_len [IN] The password length.
 *
 * \returns 0 on SUCCESS, any other value on error.
 */
int hwk_unseal(uint32_t key_id, const char *pwd, int pwd_len);


/*!
 * \brief Seal the key
 *
 * Removes the plain text key from memory and sets the key state to
 * not ready. The encrypted key remains in secure storage. All
 * subsequent crypto operations will fail.
 *
 * \param key_id [IN] Identifier for the key.
 *
 * \returns 0 on SUCCESS, any other value on error.
 */
int hwk_seal(uint32_t key_id);

/*!
 * \brief Disable the key
 *
 * Seals the key and sets the key status to disabled. All subsequent
 * crypto and key operations will fail (except hwk_destroy).
 *
 * \param key_id [IN] Identifier for the key.
 *
 * \returns 0 on SUCCESS, any other value on error.
 */
int hwk_disable(uint32_t key_id);

/*!
 * \brief Enable the key
 *
 * Enables the key making it usable again. It does not sets the key
 * status to ready, hwk_unseal() must be called to make the key ready
 * for crypto operations.
 *
 * \param key_id [IN] Identifier for the key.
 *
 * \returns 0 on SUCCESS, any other value on error.
 */
int hwk_enable(uint32_t key_id);

/*!
 * \brief Seal the key
 *
 * Seals the key and permanently removes the encrypted key from secure
 * storage. Data encrypted with this key will be permanently lost.
 *
 * \param key_id [IN] Identifier for the key.
 *
 * \returns 0 on SUCCESS, any other value on error.
 */
int hwk_destroy(uint32_t key_id);


/*!
 * \brief Reseal the key
 *
 * Encrypts the key with a new password and saves it in secure
 * storage. The key must be in ready state for this operation to
 * succeed.
 *
 * \param key_id [IN] Identifier for the key.
 *
 * \param pwd [IN] The password used to encrypt the key.
 *
 * \param pwd_len [IN] The password length.
 *
 * \returns 0 on SUCCESS, any other value on error.
 */
int hwk_reseal(uint32_t key_id, const char *pwd, int pwd_len);

typedef enum hwk_crypto_op {
    HWK_CBC_AES_256_DECRYPT = 0,
    HWK_CBC_AES_256_ENCRYPT,
} hwk_crypto_op_t;

/*!
 * \brief Perform a crypto operation
 *
 * Performs a crypto operation. The key must be in ready state.
 *
 * \param key_id [IN] Identifier for the key.
 *
 * \param key_id [IN] Crypto operation to be performed. So far only
 * CBC_AES_256_DECRYPT and CBC_AES_256_ENCRYPT are supported
 *
 *
 * \param iv [IN] pointer to 16 bytes IV
 *
 * \param src [IN] pointer to buffer from where the data is read for
 * processing
 *
 * \param dst [IN] pointer to buffer where process data will be stored
 *
 * \param len [IN] size of the buffer to be processed
 *
 * \returns 0 on SUCCESS, any other value on error.
 */
int hwk_crypto(uint32_t key_id, hwk_crypto_op_t op, const char *iv,
               const char *src, char *dst, int len);

#define HWK_ERR_BAD_KEY_ID      0xdeadf101
#define HWK_ERR_BAD_KEY_STATE   0xdeadf102
#define HWK_ERR_SS_CORRUPTED    0xdeadf103
#define HWK_ERR_TOO_MANY_KEYS   0xdeadf104
#define HWK_ERR_BAD_CRYPTO_OP   0xdeadf105
#define HWK_ERR_INVALID_CMD     0xdeadf106
#define HWK_ERR_BAD_PWD         0xdeadf107
#define HWK_INTERNAL_ERROR      0xdeadf108
#define HWK_ERR_KEY_EXISTS      0xdeadf109

#ifdef __cplusplus
}
#endif

#endif // HWK_CRYPTO_H
