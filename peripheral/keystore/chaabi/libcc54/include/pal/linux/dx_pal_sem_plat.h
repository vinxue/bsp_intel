/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_SEM_INT__H
#define _DX_PAL_SEM_INT__H



#ifdef __cplusplus
extern "C"
{
#endif
#include <semaphore.h>
/**
* @brief File Description:
*        This file contains functions for resource management (semaphor operations).
*        The functions implementations are generally just wrappers to different operating system calls.
*        None of the described functions will check the input parameters so the behavior
*        of the APIs in illegal parameters case is dependent on the operating system behavior.
*
*/

typedef sem_t DX_PAL_SEM;



/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/**
 * @brief This function purpose is to create a semaphore.
 *
 *
 * @param[out] aSemId - Pointer to created semaphor handle
 * @param[in] aInitialVal - Initial semaphore value
 *
 * @return The return values will be according to operating system return values.
 */
#define _DX_PAL_SemCreate(aSemId, aInitialVal) sem_init(aSemId, 0, aInitialVal)

/**
 * @brief This function purpose is to delete a semaphore
 *
 *
 * @param[in] aSemId - Semaphore handle
 *
 * @return The return values will be according to operating system return values.
 */
#define _DX_PAL_SemDelete(aSemId) sem_destroy (aSemId)
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
#define _DX_PAL_SemWait(aSemId, aTimeOut) sem_wait(aSemId)
/**
 * @brief This function purpose is to signal the semaphore.
 *
 *
 * @param[in] aSemId - Semaphore handle
 *
 * @return The return values will be according to operating system return values.
 */
#define _DX_PAL_SemGive(aSemId) sem_post(aSemId)




#ifdef __cplusplus
}
#endif

#endif


