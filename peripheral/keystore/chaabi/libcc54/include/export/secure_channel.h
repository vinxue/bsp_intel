/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef SECURE_CHANNEL_H
#define SECURE_CHANNEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * The Type is used to free the data buffer that has been created for returning data from SEP
 */
#define SECURE_CH_DATA_FREE_RETURN_DATA   0

/*!
 * \brief secure_channel_callback
 * This function provides an interface between Modem Manager and Chaabi to facilitate the use of
 * flashless modems.
 * \param type [IN/OUT] The type of data that is being passed.  It is updated on return to be
 *  consumed by the caller
 * \param length [IN/OUT] The lenght of the data in bytes. Updated when the called party wants
 * to return data
 * \param data [IN/OUT] The data that is being passed.  If data is being returned this will
 * be updated to contain the new data.  In the case where data is returned the caller *MUST*
 * send a SECURE_CH_DATA_FREE_RETURN_DATA message once they have used the data, to enable
 * this structure to be freed correctly.  Failure to do so *will* result in a memory leak.
 * \returns 0 on SUCCESS, any other value on error.
 */
uint32_t secure_channel_callback(uint32_t *type, uint32_t *length, uint8_t **data);

#ifdef __cplusplus
}
#endif

#endif // SECURE_CHANNEL_H
