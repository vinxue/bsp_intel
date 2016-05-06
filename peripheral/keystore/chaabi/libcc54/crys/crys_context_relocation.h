/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/*! \file crys_context_relocation.h
 * Handle relocation of crypto context in the context buffer given
 * by the user to assure it does not cross a page boundary
 */

#ifndef _CRYS_CONTEXT_RELOCATION_H_
#define _CRYS_CONTEXT_RELOCATION_H_

/*!
 * Initialize the context offset for a new buffer given to INIT phase
 *
 * \param bufferStart The address of the context buffer given by the user
 * \param bufferSize The size of the user buffer in bytes
 * \param contextSize The required size (in bytes) of the context
 *
 * \return The address of the context within the buffer
 */
void *DX_InitUserCtxLocation(void *bufferStart,
			     unsigned long bufferSize,
			     unsigned long contextSize);

/*!
 * Return the context address in the given buffer
 * If previous context offset is now crossing a page the context data
 * would be moved to a good location.
 *
 * \param bufferStart The address of the context buffer given by the user
 *
 * \return The address of the context within the buffer
 */
void *DX_GetUserCtxLocation(void *bufferStart);

#endif /*_CRYS_CONTEXT_RELOCATION_H_*/
