/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_MEM_INT_H
#define _DX_PAL_MEM_INT_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
/**
* @brief File Description:
*        This file conatins the implementation for memory operations APIs.
*        The functions implementations are generally just wrappers to different operating system calls.
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/**
 * @brief A wrapper over memcmp functionality. The function compares two given buffers
 *        according to size.
 */
#define _DX_PAL_MemCmp        memcmp

/**
 * @brief A wrapper over memmove functionality, the function copies from one
 *        buffer to another according to given size
 *
 */
#define _DX_PAL_MemCopy       memmove

/**
 * @brief A wrapper over memset functionality, the function sets a buffer with given value
 *        according to size
 *
 */
#define _DX_PAL_MemSet(aTarget, aChar, aSize)        memset(aTarget, aChar, aSize)

/**
 * @brief A wrapper over memset functionality, the function sets a buffer with zeroes
 *        according to size
 *
 */
#define _DX_PAL_MemSetZero(aTarget, aSize)    _DX_PAL_MemSet(aTarget,0x00, aSize)
		                  	


#ifdef __cplusplus
}
#endif

#endif


