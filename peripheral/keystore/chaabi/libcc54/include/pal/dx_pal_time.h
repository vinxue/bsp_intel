/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_TIME_H
#define _DX_PAL_TIME_H


#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief File Description:
*        This file conatins functions for time handling. The functions implementations
*        are generally just wrappers to different operating system calls.
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/**
 * @brief This function returns the number of seconds passed from 1/1/1970 00:00 UTC.
 *
 *
 * @param[in] void
 *
 * @return Number of seconds passed from 1/1/1970 00:00 UTC
 */
DxInt32_t DX_PAL_GetTime( void );



#ifdef __cplusplus
}
#endif

#endif


