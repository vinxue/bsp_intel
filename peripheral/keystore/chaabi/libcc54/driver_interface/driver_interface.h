/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef __DRIVER_INTERFACE_H__
#define __DRIVER_INTERFACE_H__

#include <stdint.h>
#include "dx_pal_types.h"
/* For unknown reason in user space (usr/include/asm/ioctl.h) the macro
   _IOWR is named _IORW. In order to use kernel ABI IOCTL defs. we map
   the kernel space macro to the user space name */
#define _IORW _IOWR
#include "dx_driver_abi.h"
#include "sep_rpc.h"

/* SeP driver device file names (per priority) */
#define DXDI_DEVNAME_PRIO0 "/dev/dx_sep_q0"
#define DXDI_DEVNAME_PRIO1 "/dev/dx_sep_q1"

#define DXDI_SEPAPP_AUTH_METHOD_PUBLIC 0 /* Public/Free access.. */
/* SeP Applet private authentication method (ignored by the driver and auth_data is invalid) */
#define DXDI_SEPAPP_AUTH_METHOD_PRIV_BIT 31
#define DXDI_SEPAPP_AUTH_METHOD_PRIV_MASK (1UL << DXDI_SEPAPP_AUTH_METHOD_PRIV_BIT)


typedef enum {
	DXDI_RET_OK        = 0,
	DXDI_RET_ENODEV,        /* Device not opened or does not exist */
	DXDI_RET_EINTERNAL,     /* Internal driver error (check system log) */
	DXDI_RET_ENOTSUP,       /* Unsupported function/option */
	DXDI_RET_ENOPERM,       /* Not enough permissions for request */
	DXDI_RET_EINVAL,        /* Invalid parameters */
	DXDI_RET_ENORSC,        /* No resources available (e.g., memory) */
	DXDI_RET_ESEP,          /* SeP operation error (see SeP error code) */
	DXDI_RET_EHW,           /* HW level error (e.g., Operation timeout) */
	DXDI_RESERVE32B = INT32_MAX
} DxDI_RetCode_t;

typedef enum {
	DXDI_PRIO_MIN = 0,
	DXDI_PRIO0    = DXDI_PRIO_MIN,
	DXDI_PRIO1,
	DXDI_PRIO_MAX = DXDI_PRIO1
} DxDI_DevPriority_t;

/* In current implementation this holds the device file handle */
typedef int DxDI_SepAppContext_t;

#define DX_OK DXDI_RET_OK

/*!
 * Open the given device node
 *
 *
 * \param devPriority Device priority (queue)
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_Open(DxDI_DevPriority_t devPriority);

/*!
 * Check if the given device node is open
 *
 *
 * \param
 *
 * \return true/false
 */
uint8_t DxDI_IsOpen(void);


/*!
 * Close the driver interface (file)
 *
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_Close(void);

/*!
 * Get ABI version number
 *
 * \param verMajor Returned major version number
 * \param verMinor Returned minor version number
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_GetAbiVersion(uint32_t *verMajor, uint32_t *verMinor);

/*!
 * Sets initial vector
 *
 * \param contextBuf A pointer to a context buffer
 * \param iv_ptr Source IV buffer
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_SetInitialVector(uint32_t *contextBuf, uint8_t *iv_ptr);

/*!
 * Get context size of given symmetric cipher
 *
 * \param cipherType The queried cipher type
 *
 * \return uint32_t The context buffer size required
 */
uint32_t DxDI_GetSymCipherCtxSize(enum dxdi_sym_cipher_type cipherType);


/*!
 * Get context size of given MAC algorithm
 *
 * \param aeType The queries Auth. Enc. algorithm
 *
 * \return uint32_t The context buffer size required
 */
uint32_t DxDI_GetAuthEncCtxSize(enum dxdi_auth_enc_type aeType);

/*!
 * Get context size of given MAC algorithm
 *
 * \param macType
 *
 * \return uint32_t The context buffer size required
 */
uint32_t DxDI_GetMacCtxSize(enum dxdi_mac_type macType);

/*!
 * Get context size of given hash type
 *
 * \param hashType The queried hash type
 *
 * \return uint32_t The context buffer size required
 */
uint32_t DxDI_GetHashCtxSize(enum dxdi_hash_type hashType);

/*!
 * Gets initial vector
 *
 * \param contextBuf A pointer to a context buffer
 * \param iv_ptr Destination IV buffer
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_GetInitialVector(
	uint32_t *contextBuf,
	uint8_t *iv_ptr);

/*!
 * Initialize context for given symmetric cipher configuration
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props The cipher configuration properties
 * \param sepRetCode Returned SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_SymCipherInit(
	uint32_t *contextBuf,
	struct dxdi_sym_cipher_props *props,
	uint32_t *sepRetCode
);


/*!
 * Initialize context for Authenticated-Encryption configuration
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props Auth. Enc. algorithm configuration properties
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_AuthEncInit(
	uint32_t *contextBuf,
	struct dxdi_auth_enc_props *props,
	uint32_t *sepRetCode
);

/*!
 * Initialize context for MAC configuration
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props MAC algorithm configuration properties
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_MacInit(
	uint32_t *contextBuf,
	struct dxdi_mac_props *props,
	uint32_t *sepRetCode
);

/*!
 * Initialize context for given hash type
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param hashType The hash algorithm type
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_HashInit(
	uint32_t *contextBuf,
	enum dxdi_hash_type hashType,
	uint32_t *sepRetCode
);

/*!
 * Process given data block in given context
 *
 * \param contextBuf A pointer to a context buffer
 * \param dataBlockType Indicator of special data type (primarily Adata@CCM)
 * \param dataIn User pointer of input data
 * \param dataOut User pointer of output data buffer (when applicable)
 * \param dataInSize Size (in octets) of input data
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_ProcessDataBlock(
	uint32_t *contextBuf,
	enum dxdi_data_block_type dataBlockType,
	uint8_t *dataIn,
	uint8_t *dataOut,
	uint32_t dataInSize,
	uint32_t *sepRetCode
);

/*!
 * Process last data block of processed stream and release resources
 * associated with given context.
 * Input data may be NULL in most symmetric-crypto algorithm. In that
 * case this function only deals with releasing resources.
 *
 * \param contextBuf A pointer to a context buffer
 * \param dataIn User pointer of input data
 * \param dataOut User pointer of output data buffer (when applicable)
 * \param dataInSize Size (in octets) of input data
 * \param digestOrMac Pointer for returning digest (hash) or MAC data.
 * \param digestOrMacSize Pointer for returning digest/MAC size (in octets)
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_ProcessLastDataBlock(
	uint32_t *contextBuf,
	uint8_t *dataIn,
	uint8_t *dataOut,
	uint32_t dataInSize,
	uint8_t *digestOrMac,
	uint8_t *digestOrMacSize,
	uint32_t *sepRetCode
);

/*!
 * Process data with given symmetric-cipher configuration (init.+proc.+fin.)
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props The cipher configuration properties
 * \param dataIn User pointer of input data
 * \param dataOut User pointer of output data buffer (when applicable)
 * \param dataInSize Size (in octets) of input data
 * \param sepRetCode Returned SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_SymCipherProcess(
	uint32_t *contextBuf,
	struct dxdi_sym_cipher_props *props,
	uint8_t *dataIn,
	uint8_t *dataOut,
	uint32_t dataInSize,
	uint32_t *sepRetCode
);


/*!
 * Process data with given Authenticated-Encryption configuration
 * (init.+proc.+fin.)
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props Auth. Enc. algorithm configuration properties
 * \param aData User pointer of additional/associated data (aDataSize octets)
 * \param textDataIn User pointer of input text data (textSize octets)
 * \param dataOut User pointer of output data buffer
 * \param tag The returned calculated authentication Tag/MAC (T)
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_AuthEncProcess(
	uint32_t *contextBuf,
	struct dxdi_auth_enc_props *props,
	uint8_t *aData,
	uint8_t *textDataIn,
	uint8_t *dataOut,
	uint8_t *tag,
	uint32_t *sepRetCode
);

/*!
 * Process data with given MAC configuration (init.+proc.+fin.)
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props MAC algorithm configuration properties
 * \param dataIn User pointer of input data
 * \param dataInSize Size (in octets) of input data
 * \param mac User pointer to return MAC into (of required MAC size)
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_MacProcess(
	uint32_t *contextBuf,
	struct dxdi_mac_props *props,
	uint8_t *dataIn,
	uint32_t dataInSize,
	uint8_t *mac,
	uint32_t *sepRetCode
);

/*!
 * Process data with given hash type (init.+proc.+fin.)
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param HashType The hash algorithm type
 * \param dataIn User pointer of input data
 * \param dataInSize Size (in octets) of input data
 * \param digest User pointer to return digest into (of required digest size)
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_HashProcess(
	uint32_t *contextBuf,
	enum dxdi_hash_type hashType,
	uint8_t *dataIn,
	uint32_t dataInSize,
	uint8_t *digest,
	uint32_t *sepRetCode
);

/************************************************************************/
/***                  COMBINED MODE PROCESSING  		       **/
/************************************************************************/

/*!
 * Initialization of Combined mode according to given configuration
 *
 * \param props The cipher configuration properties
 * \param sepRetCode Returned SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_CombinedInit(
	struct dxdi_combined_props *props,
	uint32_t *sepRetCode
);

/*!
 * Process given data block in Combined mode according to configuration.
 *
 * \param props The cipher configuration properties
 * \param dataIn User pointer of input data
 * \param dataInSize Size (in octets) of input data
 * \param dataOut User pointer of output data buffer (when applicable)
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_CombinedProcessDblk(
	struct dxdi_combined_props *props,
	uint8_t *dataIn,
	uint32_t dataInSize,
	uint8_t *dataOut,
	uint32_t *sepRetCode
);


/*!
 * Finalize processing of last data with given Combined mode configuration
 *
 * \param props The cipher configuration properties
 * \param combined configuration properties
 * \param pDataIn User pointer of input data
 * \param dataInSize Size (in octets) of input data
 * \param pDataOut User pointer of output data buffer (when applicable)
 * \param macOrDigest User pointer to return MAC or HASH (size according to used
 * 			MAC or HASH)
 * \param macOrDigestSize A pointer to size (in octets) of MAC or HASH output
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_CombinedFinalize(
	struct dxdi_combined_props *props,
	uint8_t  *pDataIn,
	uint32_t  dataInSize,
	uint8_t  *pDataOut,
	uint8_t  *pAuthData,
	uint32_t *pAuthDataSize,
	uint32_t *sepRetCode
);


/*!
 * Pprocesses the data with given Combined mode configuration as one integrated
 * operation (init+fin.)
 *
 * \param props The cipher configuration properties
 * \param combined configuration properties
 * \param pDataIn User pointer of input data
 * \param dataInSize Size (in octets) of input data
 * \param pDataOut User pointer of output data buffer (when applicable)
 * \param pAuthData User pointer to return Authentication data (size according
 *      		to used MAC or HASH)
 * \param pAuthDataSize A pointer to size (in octets) of Authentication data
 * 			output
 * \param sepRetCode SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_Combined(
	struct dxdi_combined_props *props,
	uint8_t  *pDataIn,
	uint32_t  dataInSize,
	uint8_t  *pDataOut,
	uint8_t  *pAuthData,
	uint32_t *pAuthDataSize,
	uint32_t *sepRetCode
);

/*!
 * Invoke SeP RPC
 *
 * \param agentId SeP RPC agent (API) ID
 * \param funcId Function ID
 * \param memRefs Array of user memory references to be mapped for DMA by the host driver
 * \param rpcParamsSize Size of rpcParams
 * \param rpcParams The RPC parameters structure (function specific)
 * \param Return code from SeP (Given in the SW descriptor from RPC infrastructure)
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_SepRpcCall(
	seprpc_agent_id_t agentId,
	seprpc_func_id_t funcId,
	struct dxdi_memref *memRefs,
	unsigned long rpcParamsSize,
	struct seprpc_params *rpcParams,
	seprpc_retcode_t *sepRetCode
);


/*!
 * Register a user memory buffer for future DMA operations
 *
 * \param contextP Associated SeP Applet context or NULL if to be used for SeP RPC
 * \param memRef The memory buffer to register
 * \param memRefIdP The returned registered memory reference ID
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_RegisterMemForDma(
	DxDI_SepAppContext_t *contextP,
	struct dxdi_memref *memRef,
	dxdi_memref_id_t *memRefIdP
);

/*!
 * Allocate DMA coherent memory
 *
 * \param contextP Associated SeP Applet context or NULL if to be used for SeP RPC
 * \param size Size in bytes of request memory buffer
 * \param memMapAddrP Returned pointer to mapping of allocated memory in this process address space
 * \param memRefIdP The returned registered memory reference ID
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_AllocMemForDma(
	DxDI_SepAppContext_t *contextP,
	unsigned long size,
	void **memMapAddrP,
	dxdi_memref_id_t *memRefIdP
);

/*!
 * Free registered user memory resources (deregister).
 *
 * \param contextP Associated SeP Applet context or NULL if to be used for SeP RPC
 * \param memRefId The registered memory reference ID
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_FreeMemForDma(
	DxDI_SepAppContext_t *contextP,
	dxdi_memref_id_t memRefId
);

/*!
 * Allocate a context for SeP Applet communication
 *
 * \param contextP A reference to a context container buffer (to be initialized with context info.)
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_SepAppContextAlloc(const char *name, DxDI_SepAppContext_t *contextP);

/*!
 * Free resources associated with given SeP Applet communication context
 *
 * \param contextP A reference to a context data buffer initialized with DxDI_SepAppContextAlloc
 */
void DxDI_SepAppContextFree(DxDI_SepAppContext_t *contextP);

/*!
 * Open a session with selected Applet
 *
 * \param contextP A reference to a context data buffer initialized with DxDI_SepAppContextAlloc
 * \param appletUuid Unique Applet ID
 * \param authMethod Session connection authentication method.
 *                   Any value with bit 31 cleared must one of DXDI_SEPAPP_AUTH_* constants.
 *                   Any value with bit 31 set is Applet-specific method with optional parameters in sepAppAuthData
 * \param authData A pointer to optional authentication parameter in case of authMethod from DXDI_SEPAPP_AUTH_*.
 * \param sepAppAuthData A set of SeP Applet parameters for Applet specific connection data (optional).
 * \param sessionIdP The returned new session ID (handle)
 * \param sepRetOrigin Encoding of the source of the sepRetCode in case of error.
 * \param sepRetCode Indication of error event in SeP. 0 on success.
 *
 * \return DxDI_RetCode_t DI return/error code
 */
DxDI_RetCode_t DxDI_SepAppSessionOpen(
	DxDI_SepAppContext_t *contextP,
	dxdi_sepapp_uuid_t *appletUuid,
	uint32_t authMethod,
	void *authData,
	struct dxdi_sepapp_params *sepAppAuthData,
	dxdi_sepapp_session_id_t *sessionIdP,
	enum dxdi_sep_module *sepRetOrigin,
	uint32_t *sepRetCode
);

/*!
 * Close a session with an Applet
 *
 * \param contextP A reference to a context data buffer initialized with DxDI_SepAppContextAlloc
 * \param sessionId A session ID as return from DxDI_SepAppSessionOpen
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_SepAppSessionClose(
	DxDI_SepAppContext_t *contextP,
	dxdi_sepapp_session_id_t sessionId);

/*!
 * Dispatch a command to a SeP Applet over given session
 *
 * \param contextP A reference to a context data buffer initialized with DxDI_SepAppContextAlloc
 * \param sessionId A session ID as return from DxDI_SepAppSessionOpen
 * \param commandId An applet specific command ID
 * \param commandParams Parameters for given command
 * \param sepRetOrigin Encoding of the source of the sepRetCode in case of error.
 * \param sepRetCode Indication of error event in SeP. 0 on success.
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_SepAppCommandInvoke(
	DxDI_SepAppContext_t *contextP,
	dxdi_sepapp_session_id_t sessionId,
	uint32_t commandId,
	struct dxdi_sepapp_params *commandParams,
	enum dxdi_sep_module *sepRetOrigin,
	uint32_t *sepRetCode);

#endif

