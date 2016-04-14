/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _DX_PAL_MEMMAP_H
#define _DX_PAL_MEMMAP_H


#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief File Description:
*        This file conatins functions for memory mapping
*        None of the described functions will check the input parameters so the behavior
*        of the APIs in illegal parameters case is dependent on the operating system behavior.
*
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/**
 * @brief This function purpose is to return the base virtual address that maps the
 *        base physical address
 *
 *
 * @param[in] aPhysicalAddress - Starts physical address of the I/O range to be mapped.
 * @param[in] aMapSize - Number of bytes that were mapped
 * @param[out] aVirtualAddr - Pointer to the base virtual address to which the physical pages were mapped
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t DX_PAL_MemMap(	DxUint32_t PhysicalAddress,
	                  	    DxUint32_t aMapSize,
		                      DxUint32_t* aVirtualAddr	);

/**
 * @brief This function purpose is to Unmaps a specified address range previously mapped
 *        by DX_PAL_MemMap
 *
 *
 * @param[in] aVirtualAddr - Pointer to the base virtual address to which the physical
 *            pages were mapped
 * @param[in] aMapSize - Number of bytes that were mapped
 *
 * @return The return values will be according to operating system return values.
 */
DxError_t DX_PAL_MemUnMap(	DxUint32_t* aVirtualAddr,
	                  	      DxUint32_t aMapSize   );



#ifdef __cplusplus
}
#endif

#endif


