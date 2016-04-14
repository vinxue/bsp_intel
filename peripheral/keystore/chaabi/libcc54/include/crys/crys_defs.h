/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef _1_CRYS_Defs_h_H
#define _1_CRYS_Defs_h_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */



#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Mon Jan 03 18:37:21 2005
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version crys_defs.h#1:incl:1
   *  \author ohads
   */


/*whether needed to export CRYS APIs for firmware testing*/
#ifdef CRYS_EXPORT_APIS_FOR_DLL
#define CEXPORT_C EXPORT_C
#define CIMPORT_C IMPORT_C
#else
#define CEXPORT_C
#define CIMPORT_C
#endif

/************************ Enums ********************************/

/* Defines the enum that is used for specifying whether or not to perform
 * a decrypt operation when performing the AES operation mode on the Context
 */


/* Defines the enum that is used for specifying whether or not to perform
 * a decrypt operation when performing the AES operation mode on the Context
 */
typedef enum
{
   AES_DECRYPT_CONTEXT = 0,
   AES_DONT_DECRYPT_CONTEXT = 1,
/*  AES_ENCRYPT_RELEASE_CONTEXT = 2*/

   CRYS_AES_CONTEXTS_flagLast = 0x7FFFFFFF,

}CRYS_AES_CONTEXTS_flag;
/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif
