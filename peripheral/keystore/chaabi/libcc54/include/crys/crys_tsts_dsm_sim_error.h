/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef CRYS_TST_DSM_SIM_ERROR_H
#define CRYS_TST_DSM_SIM_ERROR_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "crys_error.h"

#ifdef __cplusplus
extern "C"
{
#endif



/* INTERNAL debug error module on the CRYS layer base address - 0x00F00700 */

/************************ Defines ******************************/

/*AES DSM Project errors*/
#define CRYS_AES_DSM_TEST_INDEX_NOT_SUPPORTED                    (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x01UL)
#define CRYS_AES_DSM_TEST_EXPECTED_RESULT_COMPARE_FAILED         (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x02UL)

/*DES DSM Project errors*/
#define CRYS_DES_DSM_TEST_INDEX_NOT_SUPPORTED                    (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x10UL)
#define CRYS_DES_DSM_TEST_EXPECTED_RESULT_COMPARE_FAILED         (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x11UL)

/*HASH DSM Project errors*/
#define CRYS_HASH_DSM_TEST_INDEX_NOT_SUPPORTED                   (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x20UL)
#define CRYS_HASH_DSM_TEST_EXPECTED_RESULT_COMPARE_FAILED        (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x21UL)

/*RSA DSM Errors*/
#define CRYS_PKI_PRIM_DSM_TEST_EXPECTED_RESULT_COMPARE_FAILED    (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x30UL)
#define CRYS_RSA_PRIM_TST_DSM_CRT_DATA_NOT_VALID                 (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x31UL)
#define CRYS_RSA_DSM_TEST_EXPECTED_RESULT_COMPARE_FAILED         (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x32UL)
#define CRYS_RSA_PRIM_DSM_TEST_INDEX_NOT_SUPPORTED               (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x33UL)
#define CRYS_RSA_DSM_TEST_CRT_512_NOT_SUPPORTED                  (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x34UL)
#define CRYS_RSA_DSM_TEST_EXPECTED_RESULT_COMPARE_FAILED_1       (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x35UL)
#define CRYS_RSA_DSM_TEST_EXPECTED_RESULT_COMPARE_FAILED_2       (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x36UL)
#define CRYS_RSA_DSM_TEST_EXPECTED_RESULT_COMPARE_FAILED_3       (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x37UL)

/*DSM general errors*/
#define CRYS_DSM_TEST_TIMEOUT_ERROR                              (CRYS_INTERNAL_MODULE_ERROR_BASE + 0x50UL)

#ifdef __cplusplus
}
#endif

#endif


