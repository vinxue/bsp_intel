/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_FIPS_H
#define CRYS_FIPS_H

#ifndef CRYS_NO_FIPS_SUPPORT

#include "dx_pal_types.h"
#include "CRYS_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /** @file
   *  \brief Return FIPS mode state
   *
   *  \author nogad
   */

/************************ Defines ******************************/
#define DX_CRYS_FIPS_MODE_STATE_OFF   0x0
#define DX_CRYS_FIPS_MODE_STATE       0x1
#define DX_CRYS_FIPS_MODE_ERROR_STATE 0x2

/************************ Structs  ******************************/


/************************ Public Functions **********************/

/**
 * @brief This function retrieves the FIPS mode (ON or OFF)
 *
 * @param[out] FipsMode - holds the returned FIPS mode
 *
 * @return CRYSError_t - On success the function returns the value CRYS_OK, and on failure a non-ZERO error.
 *
 */
 CIMPORT_C CRYSError_t  CRYS_FIPS_GetFipsMode (DxUint8_t *FipsMode);

 /**
 * @brief This function sets the FIPS mode (always to ON). If the FIPS mode is set to Error
 *        the function will not change it.
 *
 * @param[in] void
 *
 * @return void
 *
 */
 CIMPORT_C CRYSError_t CRYS_FIPS_SetFipsMode ( void );

#ifdef __cplusplus
}
#endif

#endif

#endif


