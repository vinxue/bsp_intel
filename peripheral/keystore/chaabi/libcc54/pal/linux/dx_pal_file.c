/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/************* Include Files ****************/
#include "stdio.h"
#include "dx_pal_types.h"
#include "dx_pal_error.h"
#include "dx_pal_file.h"
#include "dx_pal_fileint.h"

/************************ Defines ******************************/


/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/* File modes table is constant table that handles the translations from DX_PAL modes to
   NoOS modes */
const DX_PAL_FileModeStr_t DX_PAL_FileModeTable[] =
{
  {"r"}, {"r+"}, {"w"}, {"w+"}, {"a"}, {"a+"}, {"rb"}, {"r+b"}, {"wb"}, {"w+b"}, {"ab"}, {"a+b"}
};

/************************ Private Functions ******************************/


/************************ Public Functions ******************************/
/**
 * @brief This function purpose is to return the file size
 *
 *
 * @param[in] aFileHandle - The file handle
 * @param[out] aFileSize - The returned file size
 *
 * @return The function will return DX_SUCCESS in case of success, else errors from
 *         DX_PAL_Error.h will be returned.
 */
DxError_t DX_PAL_FGetFileSize(	DxFile_t aFileHandle, DxUint32_t *aFileSize  )
{
  /* position in file, size */
  DxUint32_t currPos = 0, fileSize = 0;

  /* error variable */
  DxError_t error = DX_SUCCESS;

  /*------------------
      CODE
  -------------------*/

  /* Get current position */
  currPos = DX_PAL_FTell(aFileHandle);

  /* Move pointer position to end fo file */
  error = DX_PAL_FSeek(aFileHandle, 0, DX_PAL_SEEK_END);
  if (error != DX_SUCCESS)
    return error;

  /* Get current position == size of file */
  fileSize = DX_PAL_FTell(aFileHandle);

  /* Return pointer to start position */
  error = DX_PAL_FSeek(aFileHandle, currPos, DX_PAL_SEEK_START);
  if (error != DX_SUCCESS)
    return error;

  *aFileSize = fileSize;

  return DX_SUCCESS;

}/* End of DX_PAL_FGetFileSize */


