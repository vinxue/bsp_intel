/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_STRING_INT_H
#define _DX_PAL_STRING_INT_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
/**
* @brief File Description:
*        This file conatins wrapper functions for string manipulation.
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/**
 * @brief A wrapper function for strlen functionality. The function returns the size of a given string
 *
 */
#define  _DX_PAL_StrLen     strlen

/**
 * @brief A wrapper function for strstr functionality. The functions find a string in a string and
 *        return a pointer to it.
 *
 */
#define _DX_PAL_FindStr     strstr

/**
 * @brief A wrapper function for strchr functionality. The function finds a char in a given string.
 *
 */
#define _DX_PAL_StrChr      strchr

/**
 * @brief A wrapper function for strrchr functionality. The function finds a char inside a string
 *        (from the end) and returns a pointer to it
 *
 */
#define _DX_PAL_StrRChr     strrchr

/**
 * @brief A wrapper for strncpy functionality. The function copies a string.
 *
 */
#define _DX_PAL_StrNCopy    strncpy


#ifdef __cplusplus
}
#endif

#endif


