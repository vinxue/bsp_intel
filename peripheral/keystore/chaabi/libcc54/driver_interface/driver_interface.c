/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_DRIVER_INTERFACE

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include "dx_pal_types.h"
#include "dx_pal_compiler.h"
#include "driver_interface.h"

#define DXDI_LOG(level, format,...) syslog(level, "%s: " format, __func__, ##__VA_ARGS__)
#define DXDI_LOG_ON(cond, format, ...) \
	if (cond)  DXDI_LOG(format, ##__VA_ARGS__)
#define DXDI_LOG_ERR(format,...) DXDI_LOG(LOG_ERR, format, ##__VA_ARGS__)
#ifdef DEBUG
#define SYSLOG_OPTIONS (LOG_CONS | LOG_NDELAY | LOG_PID | LOG_PERROR)
#define DXDI_LOG_DEBUG(format,...) DXDI_LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#else
#define SYSLOG_OPTIONS (LOG_CONS | LOG_NDELAY | LOG_PID)
#define DXDI_LOG_DEBUG(format,...) do {} while (0)
#endif
#define DXDI_LOG_ERR_ON(cond, format, ...) if (cond) DXDI_LOG_ERR(format, ##__VA_ARGS__)
#define DXDI_LOG_DEBUG_ON(cond, format, ...) if (cond) DXDI_LOG_DEBUG(format, ##__VA_ARGS__)


/*** DI state variables ***/
static int diFileH = -1;
static int rpcFileH = -1; /* Dedicated file handle for SeP RPC - must be on priority 0 */
static DxUint32_t abiVerMajor;
static DxUint32_t abiVerMinor;

/*!
 * Convert errno to DxDI_RetCode
 *
 * \param err The errno value
 *
 * \return DxDI_RetCode
 */
static DxDI_RetCode_t Errno2RetCode(int err, const char* errContext)
{
	DXDI_LOG_DEBUG_ON(err != 0,
		"DxDI::%s: %s (%d)\n", errContext, strerror(err), err);

	switch (err) {
	case 0:
		return DXDI_RET_OK;
	case ENODEV:
	case ENOENT:
		return DXDI_RET_ENODEV;
	case ENOTTY:
	case ENOSYS:
		return DXDI_RET_ENOTSUP;
	case EINVAL:
		return DXDI_RET_EINVAL;
	case EFAULT:
	case EPERM:
	case EACCES:
		return DXDI_RET_ENOPERM;
	case ENOMEM:
	case EAGAIN:
		return DXDI_RET_ENORSC;
	case EIO:
		return DXDI_RET_EHW;
	case EBUG:
		return DXDI_RET_EINTERNAL;
	default:
		return DXDI_RET_EINTERNAL;
	}
}

/*!
 * Wrapper for the common IOCTL invocation in DxDI where return code is
 * DxDI_RetCode_t (and not some value, e.g., context size)
 *
 * \param cmdName String to describe IOCTL command for error messages
 * \param cmd IOCTL command code
 * \param params Pointer to parameters structure (arg)
 * \param retCodeSrc Pointer in params where retCode is returned
 * \param retCodeDst Pointer where to return returned code
 *
 * \return DxDI_RetCode_t
 */
static DxDI_RetCode_t doIoctl(int fileH,
	const char *cmdName, int cmd, void *params,
	uint32_t *errInfoSrc, uint32_t *errInfoDst)
{
	int rc, err;

	if (fileH < 0)  return DXDI_RET_ENODEV;

	rc = ioctl(fileH, cmd, params);
	if (rc < 0) {
		if (rc == -1)
			err = errno; /* Record errno */
		else /* Proprietary errors are returned as is in rc */
			err = -rc;
	} else {
		err = rc; /* supposed to be 0, but just in case not take rc */
	}

	/* Set the additional error info */
	if ((errInfoDst != NULL) && (errInfoSrc != NULL))
		*errInfoDst = *errInfoSrc;

	/* In case of SeP operation, fetch the SeP return code */
	if ((err == 0) && (errInfoDst != NULL)) {
		if (*errInfoDst != 0) {
			DXDI_LOG_ERR("DxDI::%s: DXDI_RET_ESEP (0x%08X)\n",
				cmdName, *errInfoDst);
			return DXDI_RET_ESEP;
		}
	}

	return Errno2RetCode(err, cmdName);
}

/*!
 * Open the given device node
 *
 *
 * \param devPriority Device priority (queue)
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_Open(DxDI_DevPriority_t devPriority)
{
	int errno_save;
	DxDI_RetCode_t rc;
	char *devName;

	/* Use syslog */
	openlog("DxDI", SYSLOG_OPTIONS, LOG_USER);

	/* Open device file if not already opened */
	if (diFileH >= 0) { /* already opened */
		return DXDI_RET_OK;
	}
	switch (devPriority) {
	case   DXDI_PRIO0:
		devName = DXDI_DEVNAME_PRIO0;
		break;
	case   DXDI_PRIO1:
		devName = DXDI_DEVNAME_PRIO1;
		break;
	default:
		DXDI_LOG_ERR("Invalid device priority: %d\n", devPriority);
		return DXDI_RET_EINVAL;
	}

	diFileH = open(devName, O_SYNC);
	if (diFileH < 0) {
		errno_save = errno; /* Save errno of "open" before logging */
		DXDI_LOG_ERR("Failed opening devName=%s (errno=%d)\n", devName, errno_save);
		return Errno2RetCode(errno_save, "open");
	}
	(void)fcntl(diFileH, F_SETFD, FD_CLOEXEC);
	/* Get version info. */
	if (ioctl(diFileH, DXDI_IOC_GET_VER_MAJOR, &abiVerMajor) < 0) {
		rc = Errno2RetCode(errno, "GET_VER_MAJOR"); goto OpenErr;
	}
	if (abiVerMajor != DXDI_VER_MAJOR) {
		rc = DXDI_RET_ENOTSUP; goto OpenErr;
	}
	if (ioctl(diFileH, DXDI_IOC_GET_VER_MINOR, &abiVerMinor) < 0) {
		rc = Errno2RetCode(errno, "GET_VER_MINOR"); goto OpenErr;
	}

	/* Open a dedicated device file for SeP RPC - if priority is not 0 */
	if (devPriority == DXDI_PRIO0) {
		rpcFileH = diFileH;
	} else {
		rpcFileH = open(DXDI_DEVNAME_PRIO0, O_SYNC);
		if (rpcFileH < 0) {
			errno_save = errno; /* Save errno of "open" before logging */
			DXDI_LOG_ERR("Failed opening devName=%s (errno=%d)\n", DXDI_DEVNAME_PRIO0, errno_save);
			rc = Errno2RetCode(errno_save, "open(rpc)");
			goto OpenErr;
		}
		(void)fcntl(rpcFileH, F_SETFD, FD_CLOEXEC);
	}
	return DXDI_RET_OK;

OpenErr:
	close(diFileH);
	diFileH = -1;
	return rc;
}

/*!
 * Check if the given device node is open
 *
 *
 * \param
 *
 * \return TRUE/FALSE
 */
uint8_t DxDI_IsOpen(void)
{
    if (diFileH >= 0)  /* already opened */
		return 1;
	else
		return 0;
}


/*!
 * Close the driver interface (file)
 *
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_Close(void)
{
	if (diFileH < 0)  return DXDI_RET_ENODEV;

	if (rpcFileH != diFileH) {
		close(rpcFileH);
	}
	rpcFileH = -1;
	close(diFileH);
	diFileH = -1;
	closelog();
	return DXDI_RET_OK;
}

/*!
 * Get ABI version number
 *
 * \param verMajor Returned major version number
 * \param verMinor Returned minor version number
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_GetAbiVersion(uint32_t *verMajor, uint32_t *verMinor)
{
	if (diFileH < 0)  return DXDI_RET_ENODEV;

	if (verMajor != NULL) *verMajor = abiVerMajor;
    if (verMinor != NULL) *verMinor = abiVerMinor;
	return DXDI_RET_OK;
}

/*!
 * Get context size of given symmetric cipher
 *
 * \param cipherType The queried cipher type
 *
 * \return uint32_t The context buffer size required
 */
uint32_t DxDI_GetSymCipherCtxSize(enum dxdi_sym_cipher_type cipherType)
{
	struct dxdi_get_sym_cipher_ctx_size_params params;
	int err = 0, rc;

	if (diFileH < 0)  return 0;

	params.sym_cipher_type = cipherType;
	rc = ioctl(diFileH, DXDI_IOC_GET_SYMCIPHER_CTX_SIZE, &params);
	if (rc < 0) {
		if (rc == -1)
			err = errno; /* Record errno */
		else /* Proprietary errors are returned as is in rc */
			err = -rc;
		DXDI_LOG_ERR("ioctl failed (rc=%d,errno=%d)\n", rc, errno);
	}

	return err ? 0 : params.ctx_size;
}


/*!
 * Get context size of given MAC algorithm
 *
 * \param aeType The queries Auth. Enc. algorithm
 *
 * \return uint32_t The context buffer size required
 */
uint32_t DxDI_GetAuthEncCtxSize(enum dxdi_auth_enc_type aeType)
{
	struct dxdi_get_auth_enc_ctx_size_params params;
	int err = 0, rc;

	if (diFileH < 0)  return 0;

	params.ae_type = aeType;
	rc = ioctl(diFileH, DXDI_IOC_GET_AUTH_ENC_CTX_SIZE, &params);
	if (rc < 0) {
		if (rc == -1)
			err = errno; /* Record errno */
		else /* Proprietary errors are returned as is in rc */
			err = -rc;
		DXDI_LOG_ERR("ioctl failed (rc=%d,errno=%d)\n", rc, errno);
	}

	return err ? 0 : params.ctx_size;
}


/*!
 * Get context size of given MAC algorithm
 *
 * \param macType
 *
 * \return uint32_t The context buffer size required
 */
uint32_t DxDI_GetMacCtxSize(enum dxdi_mac_type macType)
{
	struct dxdi_get_mac_ctx_size_params params;
	int err = 0, rc;

	if (diFileH < 0)  return 0;

	params.mac_type = macType;
	rc = ioctl(diFileH, DXDI_IOC_GET_MAC_CTX_SIZE, &params);
	if (rc < 0) {
		if (rc == -1)
			err = errno; /* Record errno */
		else /* Proprietary errors are returned as is in rc */
			err = -rc;
		DXDI_LOG_ERR("ioctl failed (rc=%d,errno=%d)\n", rc, errno);
	}


	return err ? 0 : params.ctx_size;
}

/*!
 * Get context size of given hash type
 *
 * \param hashType The queried hash type
 *
 * \return uint32_t The context buffer size required
 */
uint32_t DxDI_GetHashCtxSize(enum dxdi_hash_type hashType)
{
	struct dxdi_get_mac_ctx_size_params params;
	int err = 0, rc;

	if (diFileH < 0)  return 0;

	params.mac_type = hashType;
	rc = ioctl(diFileH, DXDI_IOC_GET_HASH_CTX_SIZE, &params);
	if (rc < 0) {
		if (rc == -1)
			err = errno; /* Record errno */
		else /* Proprietary errors are returned as is in rc */
			err = -rc;
		DXDI_LOG_ERR("ioctl failed (rc=%d,errno=%d)\n", rc, errno);
	}

	return err ? 0 : params.ctx_size;
}

/*!
 * Initialize context for given symmetric cipher configuration
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props The cipher configuration properties
 * \param errorInfo Returned SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_SymCipherInit(
	uint32_t *contextBuf,
	struct dxdi_sym_cipher_props *props,
	uint32_t *errorInfo
)
{
	struct dxdi_sym_cipher_init_params params;

	params.context_buf = contextBuf;
	memcpy(&(params.props), props, sizeof(struct dxdi_sym_cipher_props));

	return doIoctl(diFileH, "SYMCIPHER_INIT", DXDI_IOC_SYMCIPHER_INIT,
		&params, &params.error_info, errorInfo);
}

/*!
 * Initialize context for Authenticated-Encryption configuration
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props Auth. Enc. algorithm configuration properties
 * \param errorInfo SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_AuthEncInit(
	uint32_t *contextBuf,
	struct dxdi_auth_enc_props *props,
	uint32_t *errorInfo
)
{
	struct dxdi_auth_enc_init_params params;

	params.context_buf = contextBuf;
	memcpy(&(params.props), props, sizeof(struct dxdi_auth_enc_props));

	return doIoctl(diFileH, "AUTH_ENC_INIT", DXDI_IOC_AUTH_ENC_INIT,
		&params, &params.error_info, errorInfo);
}

/*!
 * Initialize context for MAC configuration
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props MAC algorithm configuration properties
 * \param errorInfo SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_MacInit(
	uint32_t *contextBuf,
	struct dxdi_mac_props *props,
	uint32_t *errorInfo
)
{
	struct dxdi_mac_init_params params;

	params.context_buf = contextBuf;
	memcpy(&(params.props), props, sizeof(struct dxdi_mac_props));

	return doIoctl(diFileH, "MAC_INIT", DXDI_IOC_MAC_INIT, &params,
		&params.error_info, errorInfo);
}

/*!
 * Initialize context for given hash type
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param hashType The hash algorithm type
 * \param errorInfo SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_HashInit(
	uint32_t *contextBuf,
	enum dxdi_hash_type hashType,
	uint32_t *errorInfo
)
{
	struct dxdi_hash_init_params params;

	params.context_buf = contextBuf;
	params.hash_type = hashType;

	return doIoctl(diFileH, "HASH_INIT", DXDI_IOC_HASH_INIT, &params,
		&params.error_info, errorInfo);
}


/*!
 * Process given data block in given context
 *
 * \param contextBuf A pointer to a context buffer
 * \param dataBlockType Indicator of special data type (primarily Adata@CCM)
 * \param dataIn User pointer of input data
 * \param dataOut User pointer of output data buffer (when applicable)
 * \param dataInSize Size (in octets) of input data
 * \param errorInfo SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_ProcessDataBlock(
	uint32_t *contextBuf,
	enum dxdi_data_block_type dataBlockType,
	uint8_t *dataIn,
	uint8_t *dataOut,
	uint32_t dataInSize,
	uint32_t *errorInfo
)
{
	struct dxdi_process_dblk_params params;

	params.context_buf = contextBuf;
	params.data_block_type = dataBlockType;
	params.data_in = dataIn;
	params.data_out = dataOut;
	params.data_in_size = dataInSize;

	return doIoctl(diFileH, "PROC_DBLK", DXDI_IOC_PROC_DBLK, &params,
		&params.error_info, errorInfo);
}

/*!
 * Sets initial vector
 *
 * \param contextBuf A pointer to a context buffer
 * \param iv_ptr Source IV buffer
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_SetInitialVector(
	uint32_t *contextBuf,
	uint8_t *iv_ptr)
{
        struct dxdi_aes_iv_params params;

        memcpy(params.iv_ptr, iv_ptr, DXDI_AES_IV_SIZE);
        params.context_buf = contextBuf;

        return doIoctl(diFileH, "SET_IV", DXDI_IOC_SET_IV, &params,
                NULL, NULL);
}

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
	uint8_t *iv_ptr)
{
        struct dxdi_aes_iv_params params;
        int rc;

        params.context_buf = contextBuf;

        rc = doIoctl(diFileH, "GET_IV", DXDI_IOC_GET_IV, &params,
                NULL, NULL);

        if (rc == DXDI_RET_OK)
                memcpy(iv_ptr, params.iv_ptr, DXDI_AES_IV_SIZE);

        return rc;
}

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
 * \param errorInfo SeP return code
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
	uint32_t *errorInfo
)
{
	struct dxdi_fin_process_params params;
	DxDI_RetCode_t rc;

	params.context_buf = contextBuf;
	params.data_in = dataIn;
	params.data_out = dataOut;
	params.data_in_size = dataInSize;

	rc = doIoctl(diFileH, "FIN_PROC", DXDI_IOC_FIN_PROC, &params,
		&params.error_info, errorInfo);
	if (rc == DXDI_RET_OK) {
		/* Successful SeP operation: Get results */
		if (digestOrMacSize != NULL)
			*digestOrMacSize = params.digest_or_mac_size;
        if ((params.digest_or_mac_size > 0) && (digestOrMacSize != NULL)) /* Get digest/MAC results */
            memcpy(digestOrMac, &(params.digest_or_mac), *digestOrMacSize);
	}

	return rc;
}

/*!
 * Process data with given symmetric-cipher configuration (init.+proc.+fin.)
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props The cipher configuration properties
 * \param dataIn User pointer of input data
 * \param dataOut User pointer of output data buffer (when applicable)
 * \param dataInSize Size (in octets) of input data
 * \param errorInfo Returned SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_SymCipherProcess(
	uint32_t *contextBuf,
	struct dxdi_sym_cipher_props *props,
	uint8_t *dataIn,
	uint8_t *dataOut,
	uint32_t dataInSize,
	uint32_t *errorInfo
)
{
	struct dxdi_sym_cipher_proc_params params;

	params.context_buf = contextBuf;
	memcpy(&(params.props), props, sizeof(struct dxdi_sym_cipher_props));
	params.data_in = dataIn;
	params.data_out = dataOut;
	params.data_in_size = dataInSize;
	return doIoctl(diFileH, "SYMCIPHER_PROC", DXDI_IOC_SYMCIPHER_PROC,
		&params, &params.error_info, errorInfo);
}

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
 * \param errorInfo SeP return code
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
	uint32_t *errorInfo
)
{
	struct dxdi_auth_enc_proc_params params;
	DxDI_RetCode_t rc;

	params.context_buf = contextBuf;
    memcpy(&(params.props), props, sizeof(struct dxdi_auth_enc_props));
	params.adata = aData;
	params.text_data = textDataIn;
	params.data_out = dataOut;
	rc = doIoctl(diFileH, "SYM_AUTH_ENC_PROC", DXDI_IOC_AUTH_ENC_PROC,
		&params, &params.error_info, errorInfo);

	if (rc == DXDI_RET_OK) {
		/* Successful SeP operation: Get results */
		memcpy(tag, params.tag, props->tag_size);
	}

	return rc;
}

/*!
 * Process data with given MAC configuration (init.+proc.+fin.)
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param props MAC algorithm configuration properties
 * \param dataIn User pointer of input data
 * \param dataInSize Size (in octets) of input data
 * \param mac User pointer to return MAC into (of required MAC size)
 * \param errorInfo SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_MacProcess(
	uint32_t *contextBuf,
	struct dxdi_mac_props *props,
	uint8_t *dataIn,
	uint32_t dataInSize,
	uint8_t *mac,
	uint32_t *errorInfo
)
{
	struct dxdi_mac_proc_params params;
	DxDI_RetCode_t rc;

	if ((props == NULL) || (mac == NULL)) {
		DXDI_LOG_ERR("DxDI_MacProcess: props/mac parameter missing\n");
		return DXDI_RET_EINVAL;
	}

	params.context_buf = contextBuf;
	memcpy(&(params.props), props, sizeof(struct dxdi_mac_props));
	params.data_in = dataIn;
	params.data_in_size = dataInSize;

	rc = doIoctl(diFileH, "MAC_PROC", DXDI_IOC_MAC_PROC, &params,
		&params.error_info, errorInfo);

	if (rc == DXDI_RET_OK) {
		/* Successful SeP operation: Get results */
		if ((params.mac_size > 0) &&
		    (params.mac_size <= DXDI_DIGEST_SIZE_MAX)) {
			memcpy(mac, &(params.mac), params.mac_size);
			DXDI_LOG_DEBUG("MAC size is %d B!\n", params.mac_size);
		} else {
			DXDI_LOG_ERR("Invalid MAC size %d B!\n", params.mac_size);
			return DXDI_RET_EINTERNAL;
		}
	}

	return rc;
}

/*!
 * Process data with given hash type (init.+proc.+fin.)
 *
 * \param contextBuf A pointer to a context buffer (phys. contig.)
 * \param HashType The hash algorithm type
 * \param dataIn User pointer of input data
 * \param dataInSize Size (in octets) of input data
 * \param digest User pointer to return digest into (of required digest size)
 * \param errorInfo SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_HashProcess(
	uint32_t *contextBuf,
	enum dxdi_hash_type hashType,
	uint8_t *dataIn,
	uint32_t dataInSize,
	uint8_t *digest,
	uint32_t *errorInfo
)
{
	struct dxdi_hash_proc_params params;
	DxDI_RetCode_t rc;

	if (digest == NULL) {
		DXDI_LOG_ERR("DxDI_HashProcess: digest parameter missing\n");
		return DXDI_RET_EINVAL;
	}

	params.context_buf = contextBuf;
	params.hash_type = hashType;
	params.data_in = dataIn;
	params.data_in_size = dataInSize;

	rc = doIoctl(diFileH, "HASH_PROC", DXDI_IOC_HASH_PROC, &params,
		&params.error_info, errorInfo);
	if (rc == DXDI_RET_OK) {
		/* Successful SeP operation: Get results */
		if ((params.digest_size > 0) &&
		    (params.digest_size <= DXDI_DIGEST_SIZE_MAX)) {
			memcpy(digest, &(params.digest), params.digest_size);
			DXDI_LOG_DEBUG("Digest size is %d B\n", params.digest_size);
		} else {
			DXDI_LOG_ERR("Invalid digest size %d B!\n", params.digest_size);
			return DXDI_RET_EINTERNAL;
		}
	}

	return rc;
}


/************************************************************************/
/***                  COMBINED MODE PROCESSING  		       **/
/************************************************************************/

/*!
 * Initialization of Combined mode according to given configuration
 *
 * \param props The cipher configuration properties
 * \param errorInfo Returned SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_CombinedInit(
	struct dxdi_combined_props *props,
	uint32_t *errorInfo
)
{
	struct dxdi_combined_init_params params;

	memcpy(&(params.props), props, sizeof(struct dxdi_combined_props));

	return doIoctl(diFileH, "COMBINED_INIT", DXDI_IOC_COMBINED_INIT,
		&params, &params.error_info, errorInfo);
}


/*!
 * Process given data block in Combined mode according to configuration.
 *
 * \param props The cipher configuration properties
 * \param dataIn User pointer of input data
 * \param dataOut User pointer of output data buffer (when applicable)
 * \param dataInSize Size (in octets) of input data
 * \param errorInfo SeP return code
 *
 * \return DxDI_RetCode
 */
DxDI_RetCode_t DxDI_CombinedProcessDblk(
	struct dxdi_combined_props *props,
	uint8_t  *pDataIn,
	uint32_t  dataInSize,
	uint8_t  *pDataOut,
	uint32_t *errorInfo
)
{
	struct dxdi_combined_proc_dblk_params params;

	params.data_in = pDataIn;
	params.data_out = pDataOut;
	params.data_in_size = dataInSize;

	memcpy(&(params.props), props, sizeof(struct dxdi_combined_props));

	return doIoctl(diFileH, "COMBINED_PROC_DBLK", DXDI_IOC_COMBINED_PROC_DBLK, &params,
		&params.error_info, errorInfo);
}

/*!
 * Finalize processing of last data with given Combined mode configuration
 *
 * \param props The cipher configuration properties
 * \param pDataIn User pointer of input data
 * \param pDataOut User pointer of output data buffer (when applicable)
 * \param dataInSize Size (in octets) of input data
 * \param macOrDigest User pointer to return MAC or HASH (size according to used
 * 			MAC or HASH)
 * \param macOrDigestSize A pointer to size (in octets) of MAC or HASH output
 * \param errorInfo SeP return code
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
	uint32_t *errorInfo
)
{
	struct dxdi_combined_proc_params params;
	DxDI_RetCode_t rc;

	params.data_in = pDataIn;
	params.data_out = pDataOut;
	params.data_in_size = dataInSize;
	params.auth_data_size = *pAuthDataSize;

	memcpy(&(params.props), props, sizeof(struct dxdi_combined_props));

	rc = doIoctl(diFileH, "COMBINED_FIN", DXDI_IOC_COMBINED_PROC_FIN, &params,
		&params.error_info, errorInfo);

	if (rc == DXDI_RET_OK && pAuthData != 0) {
		/* Successful SeP operation: Get results */
		if ((*pAuthDataSize > 0) &&
		    (params.auth_data_size <= DXDI_DIGEST_SIZE_MAX) &&
		    (params.auth_data_size <= *pAuthDataSize)) {
			memcpy(pAuthData, params.auth_data, params.auth_data_size);
			DXDI_LOG_DEBUG("MAC/HASH size is %d B\n", params.auth_data_size);
			*pAuthDataSize = params.auth_data_size;
		} else {
			DXDI_LOG_ERR("Invalid MAC/HASH size %d B!\n", params.auth_data_size);
			*pAuthDataSize = params.auth_data_size;
			return DXDI_RET_EINTERNAL;
		}
	}

	return rc;
}

/*!
 * Pprocesses the data with given Combined mode configuration as one integrated
 * operation (init+fin.)
 *
 * \param props The cipher configuration properties
 * \param combined configuration properties
 * \param pDataIn User pointer of input data
 * \param pDataOut User pointer of output data buffer (when applicable)
 * \param dataInSize Size (in octets) of input data
 * \param pAuthData User pointer to return Authentication data (size according
 *      		to used MAC or HASH)
 * \param pAuthDataSize A pointer to size (in octets) of Authentication data
 * 			output
 * \param errorInfo SeP return code
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
	uint32_t *errorInfo
)
{
	struct dxdi_combined_proc_params params;
	DxDI_RetCode_t rc;

	params.data_in = pDataIn;
	params.data_out = pDataOut;
	params.data_in_size = dataInSize;
	params.auth_data_size = *pAuthDataSize;

	memcpy(&(params.props), props, sizeof(struct dxdi_combined_props));

	rc = doIoctl(diFileH, "COMBINED", DXDI_IOC_COMBINED_PROC, &params,
		&params.error_info, errorInfo);

	if (rc == DXDI_RET_OK && pAuthData != 0) {
		/* Successful SeP operation: Get results */
		if ((*pAuthDataSize > 0) &&
		    (params.auth_data_size <= DXDI_DIGEST_SIZE_MAX) &&
		    (params.auth_data_size <= *pAuthDataSize)) {
			memcpy(pAuthData, params.auth_data, params.auth_data_size);
			DXDI_LOG_DEBUG("MAC/HASH size is %d B\n", params.auth_data_size);
			*pAuthDataSize = params.auth_data_size;
		} else {
			DXDI_LOG_ERR("Invalid MAC/HASH size %d B!\n", params.auth_data_size);
			*pAuthDataSize = params.auth_data_size;
			return DXDI_RET_EINTERNAL;
		}
	}

	return rc;
}


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
	seprpc_retcode_t *errorInfo
)
{
	struct dxdi_sep_rpc_params params;
	const unsigned int numOfMemRefs = le32_to_cpu(rpcParams->num_of_memrefs);

	if (numOfMemRefs > SEP_RPC_MAX_MEMREF_PER_FUNC)
		return DXDI_RET_EINVAL;

	params.agent_id = agentId;
	params.func_id = funcId;
	if (numOfMemRefs > 0)
		memcpy(&(params.mem_refs), memRefs, sizeof(struct dxdi_memref) * numOfMemRefs);
	params.rpc_params_size = rpcParamsSize;
	params.rpc_params = rpcParams;

	return doIoctl(rpcFileH, "SEP_RPC", DXDI_IOC_SEP_RPC,
		&params, &params.error_info, errorInfo);
}

/*!
 * Get the file handle of the given SeP Applet context
 *
 * \param contextP
 *
 * \return int The file handle (-1 on error)
 */
static inline int GetSepAppContextFile(DxDI_SepAppContext_t *contextP)
{
	if (contextP == NULL) {
		return -1;
	}
	return *((int*)contextP);
}

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
)
{
	DxDI_RetCode_t rc;
	int devFileH;
	struct dxdi_register_mem4dma_params params;

	if ((contextP == NULL) || (memRefIdP == NULL) || (memRef == NULL)) {
		DXDI_LOG_ERR("Got NULL pointers for parameters\n");
		return DXDI_RET_EINVAL;
	}

	devFileH = GetSepAppContextFile(contextP);
	if (devFileH < 0) { /* Default if no context is provided - RPC use */
		devFileH = rpcFileH;
	}

	/* copy input parameter */
	memcpy(&params.memref, memRef, sizeof(struct dxdi_memref));

	rc = doIoctl(devFileH, "REGISTER_MEM4DMA", DXDI_IOC_REGISTER_MEM4DMA, &params, NULL, NULL);

	*memRefIdP = params.memref_id;
	return rc;
}

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
)
{
	return DXDI_RET_ENOTSUP; /* Not supported, yet */
}

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
)
{
	int devFileH;
	DxDI_RetCode_t rc;
	struct dxdi_free_mem4dma_params params;

	devFileH = GetSepAppContextFile(contextP);
	if (devFileH < 0) { /* Default if no context is provided - RPC use */
		devFileH = rpcFileH;
	}

	/* copy input parameter */
	params.memref_id = memRefId;

	rc = doIoctl(devFileH, "FREE_MEM4DMA", DXDI_IOC_FREE_MEM4DMA, &params, NULL, NULL);

	return rc;
}

/*!
 * Allocate a context for SeP Applet communication
 *
 * \param contextP A reference to a context container buffer (to be initialized with context info.)
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_SepAppContextAlloc(const char *name, DxDI_SepAppContext_t *contextP)
{
	int newFileH;
	int errno_save;

	if (contextP == NULL) {
		DXDI_LOG_ERR_ON(contextP == NULL, "Invoked with contextP=NULL\n");
		return DXDI_RET_EINVAL;
	}

	/* SeP Applet operations are always processed in SW queue #0 */
	if (name == NULL)
		newFileH = open(DXDI_DEVNAME_PRIO0, O_SYNC);
	else
		newFileH = open(name, O_SYNC);

	if (newFileH < 0) {
		errno_save = errno; /* Save errno of "open" before logging */
		DXDI_LOG_ERR("Failed opening devName=%s (errno=%d)\n", DXDI_DEVNAME_PRIO0, errno_save);
		return Errno2RetCode(errno_save, "open");
	}
	(void)fcntl(newFileH, F_SETFD, FD_CLOEXEC);

	/* Save file handle in the context container */
	*((int*)contextP) = newFileH;
	return DXDI_RET_OK;
}

/*!
 * Free resources associated with given SeP Applet communication context
 *
 * \param contextP A reference to a context data buffer initialized with DxDI_SepAppContextAlloc
 */
void DxDI_SepAppContextFree(DxDI_SepAppContext_t *contextP)
{
	int contextFileH = GetSepAppContextFile(contextP);

	if (contextFileH < 0) {
		DXDI_LOG_ERR("Invoked for closed file.");
	} else {
		close(contextFileH);
		*((int*)contextP) = -1;
	}
}

/*!
 * Copy "by value" output parameters from IOCTL arg buffer back to user buffer
 *
 * \param ioctl The IOCTL copy of the parameters
 * \param user The user copy of the parameters
 */
static void copyBackAppValParams(struct dxdi_sepapp_params *ioctl, struct dxdi_sepapp_params *user)
{
	int i;

	for (i = 0; i < SEP_APP_PARAMS_MAX; i++) {
		if ((user->params_types[i] == DXDI_SEPAPP_PARAM_VAL) &&
		    (user->params[i].val.copy_dir & DXDI_DATA_FROM_DEVICE)) {
			user->params[i].val.data[0] = ioctl->params[i].val.data[0];
			user->params[i].val.data[1] = ioctl->params[i].val.data[1];
		}
	}
}

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
 * \param sepRetOrigin Encoding of the source of the errorInfo in case of error.
 * \param errorInfo Indication of error event in SeP. 0 on success.
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
	uint32_t *errorInfo
)
{
	int contextFileH = GetSepAppContextFile(contextP);
	DxDI_RetCode_t rc;
	struct dxdi_sepapp_session_open_params params;

	if (contextFileH < 0) {
		DXDI_LOG_ERR("Invalid contextP\n");
		return DXDI_RET_EINVAL;
	}
	/* Check NULL pointers */
	if ((contextP == NULL) || (appletUuid == NULL) || (sessionIdP == NULL) || (errorInfo == NULL)) {
		DXDI_LOG_ERR("Got NULL pointers for required parameters\n");
		return DXDI_RET_EINVAL;
	}
	if (!(authMethod & DXDI_SEPAPP_AUTH_METHOD_PRIV_MASK)) {
		/* Handle non-private authentication methods */
		/* Currently only "PUBLIC" method is supported (nothing to do...) */
		if (authMethod != DXDI_SEPAPP_AUTH_METHOD_PUBLIC)
			DXDI_LOG_ERR("Authentication method 0x%08X is not supported.\n",
				authMethod);
	}

	if (sepAppAuthData != NULL) {
		/* Verify that given memory references are */
		memcpy(&params.app_auth_data, sepAppAuthData, sizeof(struct dxdi_sepapp_params));
	} else {/* Make all user parametes to be NULL */
		memset(&params.app_auth_data, 0, sizeof(struct dxdi_sepapp_params));
	}

	memcpy(&params.app_uuid, appletUuid, sizeof(dxdi_sepapp_uuid_t));
	params.auth_method = authMethod;
	/* Ignore auth_data - infrastructure-aware auth. methods are not supported, yet */
	params.sep_ret_origin = DXDI_SEP_MODULE_HOST_DRIVER; /* default if driver does not change */

	rc = doIoctl(contextFileH, "SEPAPP_SESSION_OPEN", DXDI_IOC_SEPAPP_SESSION_OPEN,
		&params, &params.error_info, errorInfo);

	if (rc == DXDI_RET_OK)
		/* Save new session info. */
		*sessionIdP = params.session_id;
	else
		*sessionIdP = DXDI_SEPAPP_SESSION_INVALID;

	if ((rc == DXDI_RET_OK) ||
	    ((rc == DXDI_RET_ESEP) && (params.sep_ret_origin == DXDI_SEP_MODULE_APP))) {
		/* Success or failure in Applet layer */
		if (sepAppAuthData != NULL) /* copy back output values */
			copyBackAppValParams(&params.app_auth_data, sepAppAuthData);
		if (sepRetOrigin != NULL)
			*sepRetOrigin = DXDI_SEP_MODULE_APP;

	} else { /* Driver Failure or SeP failure not in the Applet layer */
		if (sepRetOrigin != NULL)
			*sepRetOrigin = params.sep_ret_origin;
	}

	return rc;
}

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
	dxdi_sepapp_session_id_t sessionId)
{
	int contextFileH = GetSepAppContextFile(contextP);
	DxDI_RetCode_t rc;
	struct dxdi_sepapp_session_close_params params;

	if (contextFileH < 0) { /* This denotes closed session */
		DXDI_LOG_ERR("Given session is not opened!\n");
		return DXDI_RET_EINVAL;
	}

	params.session_id = sessionId;
	rc = doIoctl(contextFileH, "SEPAPP_SESSION_CLOSE", DXDI_IOC_SEPAPP_SESSION_CLOSE,
		&params, NULL, NULL);

	return rc;
}

/*!
 * Dispatch a command to a SeP Applet over given session
 *
 * \param contextP A reference to a context data buffer initialized with DxDI_SepAppContextAlloc
 * \param sessionId A session ID as return from DxDI_SepAppSessionOpen
 * \param commandId An applet specific command ID
 * \param commandParams Parameters for given command
 * \param sepRetOrigin Encoding of the source of the errorInfo in case of error.
 * \param errorInfo Indication of error event in SeP. 0 on success.
 *
 * \return DxDI_RetCode_t
 */
DxDI_RetCode_t DxDI_SepAppCommandInvoke(
	DxDI_SepAppContext_t *contextP,
	dxdi_sepapp_session_id_t sessionId,
	uint32_t commandId,
	struct dxdi_sepapp_params *commandParams,
	enum dxdi_sep_module *sepRetOrigin,
	uint32_t *errorInfo)
{
	int contextFileH = GetSepAppContextFile(contextP);
	DxDI_RetCode_t rc;
	struct dxdi_sepapp_command_invoke_params params;

	if (contextFileH < 0) { /* This denotes closed session */
		DXDI_LOG_ERR("Given session is not opened!\n");
		return DXDI_RET_EINVAL;
	}

	params.session_id = sessionId;
	params.command_id = commandId;
	if (commandParams != NULL)
		memcpy(&params.command_params, commandParams, sizeof(struct dxdi_sepapp_params));
	else /* NULLify all user parameters */
		memset(&params.command_params, 0, sizeof(struct dxdi_sepapp_params));
	params.sep_ret_origin = DXDI_SEP_MODULE_HOST_DRIVER; /* default if driver does not change */
	rc = doIoctl(contextFileH, "SEPAPP_COMMAND_INVOKE", DXDI_IOC_SEPAPP_COMMAND_INVOKE,
		&params, &params.error_info, errorInfo);

	if ((rc == DXDI_RET_OK) ||
	    ((rc == DXDI_RET_ESEP) && (params.sep_ret_origin == DXDI_SEP_MODULE_APP))) {
		/* Success or failure in Applet layer */
		if (commandParams != NULL) /* copy back output values */
			copyBackAppValParams(&params.command_params, commandParams);
		if (sepRetOrigin != NULL)
			*sepRetOrigin = DXDI_SEP_MODULE_APP;
	} else { /* Driver Failure or SeP failure not in the Applet layer */
		if (sepRetOrigin != NULL)
			*sepRetOrigin = params.sep_ret_origin;
	}

	return rc;
}

