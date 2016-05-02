/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_version_H
#define CRYS_version_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "dx_vos_basetypes.h"


#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Tue Mar 22 09:35:02 2005
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief Get CRYS version information, including underlying engines.
   *
   *  \version CRYS_version.h#1:incl:1
   *  \author adams
   */

/************************ Defines ******************************/

/* @brief the version structure definition */
typedef struct
{
   char compName[4];
   char type;
   DxUint32_t major;
   DxUint32_t minor;
   DxUint32_t sub;
   DxUint32_t internal;

}CRYS_ComponentVersion_t;

typedef struct
{
   CRYS_ComponentVersion_t CRYS_Version;
   CRYS_ComponentVersion_t LLF_AES_Version;
   CRYS_ComponentVersion_t LLF_HASH_Version;
   CRYS_ComponentVersion_t LLF_DES_Version;
   CRYS_ComponentVersion_t LLF_PKI_Version;
   CRYS_ComponentVersion_t LLF_RND_Version;
   CRYS_ComponentVersion_t LLF_ECPKI_Version;
   CRYS_ComponentVersion_t LLF_RC4_Version;

}CRYS_Version_t;

/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

/**
 * @brief This function returns the CRYS version.
 *
 * The version contains the following fields:
 *
 * component string - a string describing the nature of the release.
 * release type: 'D' - development, 'A' - alpha (passed to QA) ,
 *                'R' - release, after QA testing.
 *
 * major, minor, sub, internal - the release digits.
 *
 * each component: CRYS, LLF machines has this structure.
 *
 * @param[in] version_ptr - a pointer to the version structure.
 *
 */

 void  CRYS_GetVersion(CRYS_Version_t *version_ptr);

#ifdef __cplusplus
}
#endif

#endif


