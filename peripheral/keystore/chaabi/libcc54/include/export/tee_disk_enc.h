/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef TEE_DISK_ENC_H
#define TEE_DISK_ENC_H

#include <stdint.h>

/*!
 * \brief tee_create_disk_enc_key
 * Create a master key that is stored within the TEE and is protected from the REE
 * \param masterkey A key that will be used to index the actual encryption key in the TEE
 * \return 0 on success
 */
int32_t tee_create_disk_enc_key(uint8_t* masterkey);

/*!
 * \brief tee_verify_disk_enc_masterkey
 * Validate that the master key that is used as the index is the valid one as understood by the TEE
 * \param masterkey The key that is used as the index
 * \return 0 on success
 */
int32_t tee_verify_disk_enc_masterkey(uint8_t* masterkey);

#endif // TEE_DISK_ENC_H
