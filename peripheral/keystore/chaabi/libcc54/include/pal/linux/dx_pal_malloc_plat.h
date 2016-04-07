/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_MEMALLOC_INT_H
#define _DX_PAL_MEMALLOC_INT_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
/**
* @brief File Description:
*        This file contains wrappers for memory operations APIs.
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/**
 * @brief A wrapper over malloc functionality. The function allocates a buffer according to given size
 *
 */
 #define _DX_PAL_MemMalloc    malloc


/**
 * @brief A wrapper over realloc functionality. The function allocates and copy a buffer
 *        according to size
 *
 */
#define _DX_PAL_MemRealloc    realloc


/**
 * @brief A wrapper over free functionality/ The function will free allocated memory.
 *
 */
#define _DX_PAL_MemFree       free



#ifdef __cplusplus
}
#endif

#endif


