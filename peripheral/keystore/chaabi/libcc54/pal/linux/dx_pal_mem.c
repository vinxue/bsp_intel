/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/************* Include Files ****************/
#include "dx_pal_types.h"
#include "dx_pal_error.h"
#include "dx_pal_mem.h"
#include "dx_pal_memint.h"


/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************************ Private Functions ******************************/


/************************ Public Functions ******************************/

/**
 * @brief This function purpose is to perform secured memory comparison between two given
 *        buffers according to given size. The function will compare each byte till aSize
 *        number of bytes was compared even if the bytes are different.
 *        The function should be used to avoid security timing attacks.
 *
 *
 * @param[in] aTarget - The target buffer to compare
 * @param[in] aSource - The Source buffer to compare to
 * @param[in] aSize - Number of bytes to compare
 *
 * @return The function will return DX_SUCCESS in case of success, else errors from
 *         DX_PAL_Error.h will be returned.
 */
DxError_t DX_PAL_SecMemCmp(	const DxUint8_t* aTarget,
	                  	      const DxUint8_t* aSource,
		                        DxUint32_t  aSize		)
{
  /* internal index */
  DxUint32_t i = 0;

  /* error return */
  DxUint32_t error = DX_SUCCESS;

  /*------------------
      CODE
  -------------------*/

  /* Go over aTarget till aSize is reached (even if its not equal) */
  for (i = 0; i < aSize; i++)
  {
    if (aTarget[i] != aSource[i])
    {
      if (error != DX_SUCCESS)
        continue;
      else
      {
        if (aTarget[i] < aSource[i])
          error = DX_PAL_MEM_BUF2_GREATER;
        else
          error = DX_PAL_MEM_BUF1_GREATER;
      }
    }
  }

  return error;
}/* End of DX_PAL_SecMemCmp */

