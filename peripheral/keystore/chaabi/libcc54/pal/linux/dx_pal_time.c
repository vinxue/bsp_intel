/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/************* Include Files ****************/
#include "dx_pal_types.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Private Functions ******************************/

/************************ Public Functions ******************************/

/**
 * @brief This function returns the number of seconds passed from 1/1/1970 00:00 UTC.
 *
 *
 * @param[in] void
 *
 * @return Number of seconds passed from 1/1/1970 00:00 UTC
 */

DxInt32_t DX_PAL_GetTime( void )
{
  /* No support for time utils in NoOS */
  return 0;
}

