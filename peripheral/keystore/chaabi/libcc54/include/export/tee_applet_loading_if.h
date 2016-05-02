/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef __TEE_APPLET_LOADING_IF_H
#define __TEE_APPLET_LOADING_IF_H

#include "tee_client_api.h"

/*!
 * \brief tee_load_applet
 * loads a static or dynamic applet
 * \param img_buf Applet in a buffer
 * \param img_size Size of the applet
 * \return 0 on success
 * \note only supports primary vrl and that refers to one applet
 */
int32_t tee_load_applet(uint8_t *img_buf, uint32_t img_size);

/*!
 *  \brief tee_unload_applet     unloads a dynamic applet
 *  \param applet_uuid           uuid of the applet to unload
 *  \note                        valid only for dynamic applets
 */
int32_t tee_unload_applet(TEEC_UUID *applet_uuid);

#endif /* end of __TEE_APPLET_LOADING_IF_H */
