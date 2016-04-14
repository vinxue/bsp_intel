/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef  _DX_MANAGEMENT_H
#define  _DX_MANAGEMENT_H


/******************************************************************************
*                        	DEFINITIONS
******************************************************************************/

/* ROM CRC address  */
#define DX_ROM_CRC_ADDR			0x0400FFEC
#ifndef DX_FAILURE
	#define DX_FAILURE		1
#endif

/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/


/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*
 @Brief: This function returns the ROM CRC value
 @params:
	crcVal - returns the crc value
 @return:
	DX_SUCCESS

*/
DxUint32_t DX_MNG_GetRomCrc(DxUint32_t *crcVal);

/*
 @Brief: This function returns the ROM CRC value
 @params:
        lcsVal - returns the lcs value
 @return:
	DX_SUCCESS

*/
DxUint32_t DX_MNG_GetLcs(DxUint32_t *lcsVal);

/*
 @Brief: This function returns the fw and the rom versions
 @params:
        fwVer [out] - fw version
	romVer [out] - rom version
 @return:
	DX_SUCCESS

*/
DxUint32_t DX_MNG_GetFwVersion(DxUint32_t *fwVer, DxUint32_t *romVer);

#endif /*_DX_MANAGEMENT_H*/
