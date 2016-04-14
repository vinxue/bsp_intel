/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/************* Include Files ****************/
#include "dx_pal_types.h"

#include "dx_pal_sem.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/



/*typedef struct
{
  DxUint32_t DX_PAL_SemId;

}_DX_PAL_Sem_t; */


/************************ Global Data ******************************/

/************************ Private Functions ******************************/

/************************ Public Functions ******************************/

/**
 * @brief This function purpose is to create a semaphore.
 *
 *
 * @param[out] aSemId - Pointer to created semaphor handle
 * @param[in] aInitialVal - Initial semaphore value
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t DX_PAL_SemCreate( DX_PAL_SEM *aSemId, DxUint32_t aInitialVal )
{

  *aSemId = (DX_PAL_SEM)1;

  return DX_SUCCESS;

}/* End of DX_PAL_SemCreate */

/**
 * @brief This function purpose is to delete a semaphore
 *
 *
 * @param[in] aSemId - Semaphore handle
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t DX_PAL_SemDelete( DX_PAL_SEM *aSemId )
{

  *aSemId = (DX_PAL_SEM)0;
  return DX_SUCCESS;

}/* End of DX_PAL_SemDelete */

/**
 * @brief This function purpose is to Wait for semaphore with aTimeOut. aTimeOut is
 *        specified in milliseconds.
 *
 *
 * @param[in] aSemId - Semaphore handle
 * @param[in] aTimeOut - timeout in mSec, or DX_INFINITE
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t DX_PAL_SemWait(DX_PAL_SEM aSemId, DxUint32_t aTimeOut)
{
  return DX_SUCCESS;
}/* End of DX_PAL_SemWait */

/**
 * @brief This function purpose is to signal the semaphore.
 *
 *
 * @param[in] aSemId - Semaphore handle
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t DX_PAL_SemGive(DX_PAL_SEM aSemId)
{
  return DX_SUCCESS;
}/* End of DX_PAL_SemGive */
