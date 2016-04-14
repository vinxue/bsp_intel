/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/***************************************************************************
 *  This file provides the applet load interface        *
 ***************************************************************************/

#ifndef __DX_APPLET_MNG_LOAD_H__
#define __DX_APPLET_MNG_LOAD_H__

#include "dx_pal_types.h"

/******************************************************************************
*				DEFINITIONS
******************************************************************************/

/* Applet UUID MAX size according to TEEC defintions */
#define APPLET_UUID_MAX_SIZE_WORDS  4

/* Applet's unique identifier */
typedef DxUint8_t DX_SepAppUuid_t[APPLET_UUID_MAX_SIZE_WORDS*sizeof(DxUint32_t)];


/******************************************************************************
*				FUNCTION PROTOTYPES
******************************************************************************/

/*
 @Brief: This function is used to load applet to the sep.
         The function will do the following steps :
        1. verify the applet's vrl
	    2. load the applet to its designated location
        3. update the Icache and Dcache
        4. add the applet's data to the applet's manager DB

 @params:
    vrlAddr [in] - vrl address in host memory
    magicNum [in] - a magic number in case the magic number to validate the vrl with is not the default magic number
    verKeyIndex [in] - Index of the RSA public key used to verify the VRL (0-2).
    flags [in] - bit flags for the imageAddress parameter:
        - origin of applet Image address (if set to 1, the address in the host ram is taken from appletImageAddr) - DX_CC_INIT_FLAGS_I_CACHE_ADDR_FLAG
        - magic number - from user or default (if set to 1 - use user's magic number) - DX_CC_INIT_FLAGS_MAGIC_NUMBER_FLAG
    appletImageAddr [in] - image address in the host memory


*/
DxUint32_t DX_CC_LoadApplet(DxUint32_t vrlAddr,
                            DxUint32_t magicNum,
                            DxUint32_t verKeyIndex,
                            DxUint32_t flags,
                            DxUint32_t appletImageAddr,
			    DxUint32_t userParam);

/*
 @Brief: This function is used to unload the replaceable applet
         The function will do the following steps :
        1. find the applet's slot according to uuid
        2. verify there are no open sessions for that applet
        3. Call the applet terminate function
        3. zero the Dcache
        4. remove the entry from the applets info table

 @params:
 	uuid [in] - applet's identifier


*/
DxUint32_t DX_CC_UnloadApplet(DX_SepAppUuid_t uuid);


#endif /*__DX_APPLET_MNG_LOAD_H__*/
