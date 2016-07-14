/* Generated file - DO NOT EDIT! */
/* Generated by utils/src/sep_rpc_gen/sep_rpc_gen.py@2451 at 2012-09-20 12:33:41.580095 */
/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

/* \file crys_dh_seprpc_stub.c
 * SeP-RPC host wrappers/stubs implementation for CrysDh API */

#include <string.h>
#include "crys_dh.h"
#include "crys_dh_error.h"
#include "crys_dh_kg.h"
#include "crys_rsa_prim.h"
#include "sep_rpc.h"
#include "driver_interface.h"
#include "crys_dh_seprpc.h"


/************ _DX_DH_GeneratePubPrv ***************/
CRYSError_t _DX_DH_GeneratePubPrv(
	DxUint8_t Generator_ptr[],
	DxUint16_t GeneratorSize,
	DxUint8_t Prime_ptr[],
	DxUint16_t PrimeSize,
	DxUint16_t L,
	DxUint8_t Q_ptr[],
	DxUint16_t QSize,
	CRYS_DH_OpMode_t DH_mode,
	CRYS_DHUserPubKey_t *tmpPubKey_ptr,
	CRYS_DHPrimeData_t *tmpPrimeData_ptr,
	DxUint8_t ClientPrvKey_ptr[],
	DxUint16_t *ClientPrvKeySize_ptr,
	DxUint8_t ClientPub1_ptr[],
	DxUint16_t *ClientPubSize_ptr)
{
	DxDI_RetCode_t diRc;
	seprpc_retcode_t rpcRc;
	struct SepRpc__DX_DH_GeneratePubPrvParams params;

	/* Verify input parameters and copy into params. struct. */
	if (Generator_ptr == NULL) {
		params.Generator_ptr_null_flag = 1;
	} else {
		/* Verify array size of Generator_ptr */
		SEP_RPC_ASSERT((GeneratorSize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.Generator_ptr, Generator_ptr, sizeof(DxUint8_t) * (GeneratorSize));
		params.Generator_ptr_null_flag = 0;
	}

	params.GeneratorSize = cpu_to_le16(GeneratorSize);
	if (Prime_ptr == NULL) {
		params.Prime_ptr_null_flag = 1;
	} else {
		/* Verify array size of Prime_ptr */
		SEP_RPC_ASSERT((PrimeSize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.Prime_ptr, Prime_ptr, sizeof(DxUint8_t) * (PrimeSize));
		params.Prime_ptr_null_flag = 0;
	}

	params.PrimeSize = cpu_to_le16(PrimeSize);
	params.L = cpu_to_le16(L);
	if (Q_ptr == NULL) {
		params.Q_ptr_null_flag = 1;
	} else {
		/* Verify array size of Q_ptr */
		SEP_RPC_ASSERT((QSize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.Q_ptr, Q_ptr, sizeof(DxUint8_t) * (QSize));
		params.Q_ptr_null_flag = 0;
	}

	params.QSize = cpu_to_le16(QSize);
	params.DH_mode = cpu_to_le32(DH_mode);
	if (tmpPubKey_ptr == NULL) {
		params.__tmpPubKey_ptr_null_flag = 1;
	} else {
		params.__tmpPubKey_ptr = *(tmpPubKey_ptr);
		params.__tmpPubKey_ptr_null_flag = 0;
	}

	params.tmpPrimeData_ptr_size = cpu_to_le32((sizeof(CRYS_DHPrimeData_t)));
	if (ClientPrvKeySize_ptr == NULL) {
		params.__ClientPrvKeySize_ptr_null_flag = 1;
	} else {
		params.__ClientPrvKeySize_ptr = cpu_to_le16(*(ClientPrvKeySize_ptr));
		params.__ClientPrvKeySize_ptr_null_flag = 0;
	}

	if (ClientPubSize_ptr == NULL) {
		params.__ClientPubSize_ptr_null_flag = 1;
	} else {
		params.__ClientPubSize_ptr = cpu_to_le16(*(ClientPubSize_ptr));
		params.__ClientPubSize_ptr_null_flag = 0;
	}

	if (ClientPrvKey_ptr == NULL) {
		params.ClientPrvKey_ptr_null_flag = 1;
    } else if (ClientPrvKeySize_ptr != NULL) {
		/* Verify array size of ClientPrvKey_ptr */
		SEP_RPC_ASSERT((*ClientPrvKeySize_ptr) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.ClientPrvKey_ptr_null_flag = 0;
	}

	if (ClientPub1_ptr == NULL) {
		params.ClientPub1_ptr_null_flag = 1;
    } else if (ClientPubSize_ptr != NULL) {
		/* Verify array size of ClientPub1_ptr */
		SEP_RPC_ASSERT((*ClientPubSize_ptr) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.ClientPub1_ptr_null_flag = 0;
	}


	params.num_of_memrefs = cpu_to_le32(SEPRPC_MEMREF_NUM__DX_DH_GeneratePubPrv);

	diRc = DxDI_SepRpcCall(SEPRPC_AGENT_ID_CrysDh, SEPRPC_FUNC_ID__DX_DH_GeneratePubPrv,
		NULL, sizeof(params), (struct seprpc_params*)&params, &rpcRc);

	SEP_RPC_ASSERT(diRc == DXDI_RET_OK, CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR);
	if (rpcRc != SEPRPC_RET_OK) {
		SEP_RPC_LOG("RPC of _DX_DH_GeneratePubPrv failed with RPC error code %d\n", rpcRc);
		switch (rpcRc) {
		case SEPRPC_RET_EINVAL:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR;
		case SEPRPC_RET_ENORSC:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_C_ERROR;
		default:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR;
		}
	}

	/* Copy back output parameters */
	if (ClientPrvKeySize_ptr == NULL) {
		params.__ClientPrvKeySize_ptr_null_flag = 1;
	} else {
		*(ClientPrvKeySize_ptr) = le16_to_cpu(params.__ClientPrvKeySize_ptr);
		params.__ClientPrvKeySize_ptr_null_flag = 0;
	}

	if (ClientPubSize_ptr == NULL) {
		params.__ClientPubSize_ptr_null_flag = 1;
	} else {
		*(ClientPubSize_ptr) = le16_to_cpu(params.__ClientPubSize_ptr);
		params.__ClientPubSize_ptr_null_flag = 0;
	}

	if (ClientPrvKey_ptr == NULL) {
		params.ClientPrvKey_ptr_null_flag = 1;
    } else if (ClientPrvKeySize_ptr != NULL && (*ClientPrvKeySize_ptr <= (sizeof(params.ClientPrvKey_ptr) / sizeof(params.ClientPrvKey_ptr[0])))) {
		memcpy(ClientPrvKey_ptr, params.ClientPrvKey_ptr, sizeof(DxUint8_t) * (*ClientPrvKeySize_ptr));
		params.ClientPrvKey_ptr_null_flag = 0;
	}

	if (ClientPub1_ptr == NULL) {
		params.ClientPub1_ptr_null_flag = 1;
    } else  if (ClientPubSize_ptr != NULL && (*ClientPubSize_ptr <= (sizeof(params.ClientPub1_ptr) / sizeof(params.ClientPub1_ptr[0])))) {
		memcpy(ClientPub1_ptr, params.ClientPub1_ptr, sizeof(DxUint8_t) * (*ClientPubSize_ptr));
		params.ClientPub1_ptr_null_flag = 0;
	}


	return le32_to_cpu(params._funcRetCode);
} /* _DX_DH_GeneratePubPrv */



/************ CRYS_DH_GetSecretKey ***************/
CRYSError_t CRYS_DH_GetSecretKey(
	DxUint8_t ClientPrvKey_ptr[],
	DxUint16_t ClientPrvKeySize,
	DxUint8_t ServerPubKey_ptr[],
	DxUint16_t ServerPubKeySize,
	DxUint8_t Prime_ptr[],
	DxUint16_t PrimeSize,
	CRYS_DHUserPubKey_t *tmpPubKey_ptr,
	CRYS_DHPrimeData_t *tmpPrimeData_ptr,
	DxUint8_t SecretKey_ptr[],
	DxUint16_t *SecretKeySize_ptr)
{
	DxDI_RetCode_t diRc;
	seprpc_retcode_t rpcRc;
	struct SepRpc_CRYS_DH_GetSecretKeyParams params;

	/* Verify input parameters and copy into params. struct. */
	if (ClientPrvKey_ptr == NULL) {
		params.ClientPrvKey_ptr_null_flag = 1;
	} else {
		/* Verify array size of ClientPrvKey_ptr */
		SEP_RPC_ASSERT((ClientPrvKeySize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.ClientPrvKey_ptr, ClientPrvKey_ptr, sizeof(DxUint8_t) * (ClientPrvKeySize));
		params.ClientPrvKey_ptr_null_flag = 0;
	}

	params.ClientPrvKeySize = cpu_to_le16(ClientPrvKeySize);
	if (ServerPubKey_ptr == NULL) {
		params.ServerPubKey_ptr_null_flag = 1;
	} else {
		/* Verify array size of ServerPubKey_ptr */
		SEP_RPC_ASSERT((ServerPubKeySize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.ServerPubKey_ptr, ServerPubKey_ptr, sizeof(DxUint8_t) * (ServerPubKeySize));
		params.ServerPubKey_ptr_null_flag = 0;
	}

	params.ServerPubKeySize = cpu_to_le16(ServerPubKeySize);
	if (Prime_ptr == NULL) {
		params.Prime_ptr_null_flag = 1;
	} else {
		/* Verify array size of Prime_ptr */
		SEP_RPC_ASSERT((PrimeSize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.Prime_ptr, Prime_ptr, sizeof(DxUint8_t) * (PrimeSize));
		params.Prime_ptr_null_flag = 0;
	}

	params.PrimeSize = cpu_to_le16(PrimeSize);
	if (tmpPubKey_ptr == NULL) {
		params.__tmpPubKey_ptr_null_flag = 1;
	} else {
		params.__tmpPubKey_ptr = *(tmpPubKey_ptr);
		params.__tmpPubKey_ptr_null_flag = 0;
	}

	params.tmpPrimeData_ptr_size = cpu_to_le32((sizeof(CRYS_DHPrimeData_t)));
	if (SecretKeySize_ptr == NULL) {
		params.__SecretKeySize_ptr_null_flag = 1;
	} else {
		params.__SecretKeySize_ptr = cpu_to_le16(*(SecretKeySize_ptr));
		params.__SecretKeySize_ptr_null_flag = 0;
	}

	if (SecretKey_ptr == NULL) {
		params.SecretKey_ptr_null_flag = 1;
    } else if (SecretKeySize_ptr != NULL) {
		/* Verify array size of SecretKey_ptr */
		SEP_RPC_ASSERT((*SecretKeySize_ptr) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.SecretKey_ptr_null_flag = 0;
	}


	params.num_of_memrefs = cpu_to_le32(SEPRPC_MEMREF_NUM_CRYS_DH_GetSecretKey);

	diRc = DxDI_SepRpcCall(SEPRPC_AGENT_ID_CrysDh, SEPRPC_FUNC_ID_CRYS_DH_GetSecretKey,
		NULL, sizeof(params), (struct seprpc_params*)&params, &rpcRc);

	SEP_RPC_ASSERT(diRc == DXDI_RET_OK, CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR);
	if (rpcRc != SEPRPC_RET_OK) {
		SEP_RPC_LOG("RPC of CRYS_DH_GetSecretKey failed with RPC error code %d\n", rpcRc);
		switch (rpcRc) {
		case SEPRPC_RET_EINVAL:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR;
		case SEPRPC_RET_ENORSC:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_C_ERROR;
		default:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR;
		}
	}

	/* Copy back output parameters */
	if (SecretKeySize_ptr == NULL) {
		params.__SecretKeySize_ptr_null_flag = 1;
	} else {
		*(SecretKeySize_ptr) = le16_to_cpu(params.__SecretKeySize_ptr);
		params.__SecretKeySize_ptr_null_flag = 0;
	}

	if (SecretKey_ptr == NULL) {
		params.SecretKey_ptr_null_flag = 1;
    } else if (SecretKeySize_ptr != NULL && (*SecretKeySize_ptr <= (sizeof(params.SecretKey_ptr) / sizeof(params.SecretKey_ptr[0])))) {
		memcpy(SecretKey_ptr, params.SecretKey_ptr, sizeof(DxUint8_t) * (*SecretKeySize_ptr));
		params.SecretKey_ptr_null_flag = 0;
	}


	return le32_to_cpu(params._funcRetCode);
} /* CRYS_DH_GetSecretKey */



/************ CRYS_DH_CheckPubKey ***************/
CRYSError_t CRYS_DH_CheckPubKey(
	DxUint8_t modP_ptr[],
	DxUint32_t modPsizeBytes,
	DxUint8_t orderQ_ptr[],
	DxUint32_t orderQsizeBytes,
	DxUint8_t pubKey_ptr[],
	DxUint32_t pubKeySizeBytes,
	CRYS_DH_Temp_t *tempBuff_ptr)
{
	DxDI_RetCode_t diRc;
	seprpc_retcode_t rpcRc;
	struct SepRpc_CRYS_DH_CheckPubKeyParams params;

	/* Verify input parameters and copy into params. struct. */
	if (modP_ptr == NULL) {
		params.modP_ptr_null_flag = 1;
	} else {
		/* Verify array size of modP_ptr */
		SEP_RPC_ASSERT((modPsizeBytes) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.modP_ptr, modP_ptr, sizeof(DxUint8_t) * (modPsizeBytes));
		params.modP_ptr_null_flag = 0;
	}

	params.modPsizeBytes = cpu_to_le32(modPsizeBytes);
	if (orderQ_ptr == NULL) {
		params.orderQ_ptr_null_flag = 1;
	} else {
		/* Verify array size of orderQ_ptr */
		SEP_RPC_ASSERT((orderQsizeBytes) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.orderQ_ptr, orderQ_ptr, sizeof(DxUint8_t) * (orderQsizeBytes));
		params.orderQ_ptr_null_flag = 0;
	}

	params.orderQsizeBytes = cpu_to_le32(orderQsizeBytes);
	if (pubKey_ptr == NULL) {
		params.pubKey_ptr_null_flag = 1;
	} else {
		/* Verify array size of pubKey_ptr */
		SEP_RPC_ASSERT((pubKeySizeBytes) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.pubKey_ptr, pubKey_ptr, sizeof(DxUint8_t) * (pubKeySizeBytes));
		params.pubKey_ptr_null_flag = 0;
	}

	params.pubKeySizeBytes = cpu_to_le32(pubKeySizeBytes);
	params.tempBuff_ptr_size = cpu_to_le32((sizeof(CRYS_DH_ExpTemp_t)));

	params.num_of_memrefs = cpu_to_le32(SEPRPC_MEMREF_NUM_CRYS_DH_CheckPubKey);

	diRc = DxDI_SepRpcCall(SEPRPC_AGENT_ID_CrysDh, SEPRPC_FUNC_ID_CRYS_DH_CheckPubKey,
		NULL, sizeof(params), (struct seprpc_params*)&params, &rpcRc);

	SEP_RPC_ASSERT(diRc == DXDI_RET_OK, CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR);
	if (rpcRc != SEPRPC_RET_OK) {
		SEP_RPC_LOG("RPC of CRYS_DH_CheckPubKey failed with RPC error code %d\n", rpcRc);
		switch (rpcRc) {
		case SEPRPC_RET_EINVAL:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR;
		case SEPRPC_RET_ENORSC:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_C_ERROR;
		default:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR;
		}
	}

	return le32_to_cpu(params._funcRetCode);
} /* CRYS_DH_CheckPubKey */



/************ CRYS_DH_X942_GetSecretData ***************/
CRYSError_t CRYS_DH_X942_GetSecretData(
	DxUint8_t ClientPrvKey_ptr[],
	DxUint16_t ClientPrvKeySize,
	DxUint8_t ServerPubKey_ptr[],
	DxUint16_t ServerPubKeySize,
	DxUint8_t Prime_ptr[],
	DxUint16_t PrimeSize,
	CRYS_DH_OtherInfo_t *otherInfo_ptr,
	CRYS_DH_HASH_OpMode_t hashMode,
	CRYS_DH_DerivationFunc_Mode DerivFunc_mode,
	CRYS_DH_Temp_t *tmpBuff_ptr,
	DxUint8_t SecretKeyingData_ptr[],
	DxUint16_t SecretKeyingDataSize)
{
	DxDI_RetCode_t diRc;
	seprpc_retcode_t rpcRc;
	struct SepRpc_CRYS_DH_X942_GetSecretDataParams params;

	/* Verify input parameters and copy into params. struct. */
	if (ClientPrvKey_ptr == NULL) {
		params.ClientPrvKey_ptr_null_flag = 1;
	} else {
		/* Verify array size of ClientPrvKey_ptr */
		SEP_RPC_ASSERT((ClientPrvKeySize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.ClientPrvKey_ptr, ClientPrvKey_ptr, sizeof(DxUint8_t) * (ClientPrvKeySize));
		params.ClientPrvKey_ptr_null_flag = 0;
	}

	params.ClientPrvKeySize = cpu_to_le16(ClientPrvKeySize);
	if (ServerPubKey_ptr == NULL) {
		params.ServerPubKey_ptr_null_flag = 1;
	} else {
		/* Verify array size of ServerPubKey_ptr */
		SEP_RPC_ASSERT((ServerPubKeySize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.ServerPubKey_ptr, ServerPubKey_ptr, sizeof(DxUint8_t) * (ServerPubKeySize));
		params.ServerPubKey_ptr_null_flag = 0;
	}

	params.ServerPubKeySize = cpu_to_le16(ServerPubKeySize);
	if (Prime_ptr == NULL) {
		params.Prime_ptr_null_flag = 1;
	} else {
		/* Verify array size of Prime_ptr */
		SEP_RPC_ASSERT((PrimeSize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.Prime_ptr, Prime_ptr, sizeof(DxUint8_t) * (PrimeSize));
		params.Prime_ptr_null_flag = 0;
	}

	params.PrimeSize = cpu_to_le16(PrimeSize);
	if (otherInfo_ptr == NULL) {
		params.__otherInfo_ptr_null_flag = 1;
	} else {
		/* Verify array size of AlgorithmID */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfAlgorithmID) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.AlgorithmID, otherInfo_ptr->AlgorithmID, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfAlgorithmID));
		params.__otherInfo_ptr.SizeOfAlgorithmID = cpu_to_le32(otherInfo_ptr->SizeOfAlgorithmID);
		/* Verify array size of PartyUInfo */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfPartyUInfo) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.PartyUInfo, otherInfo_ptr->PartyUInfo, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfPartyUInfo));
		params.__otherInfo_ptr.SizeOfPartyUInfo = cpu_to_le32(otherInfo_ptr->SizeOfPartyUInfo);
		/* Verify array size of PartyVInfo */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfPartyVInfo) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.PartyVInfo, otherInfo_ptr->PartyVInfo, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfPartyVInfo));
		params.__otherInfo_ptr.SizeOfPartyVInfo = cpu_to_le32(otherInfo_ptr->SizeOfPartyVInfo);
		/* Verify array size of SuppPrivInfo */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfSuppPrivInfo) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.SuppPrivInfo, otherInfo_ptr->SuppPrivInfo, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfSuppPrivInfo));
		params.__otherInfo_ptr.SizeOfSuppPrivInfo = cpu_to_le32(otherInfo_ptr->SizeOfSuppPrivInfo);
		/* Verify array size of SuppPubInfo */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfSuppPubInfo) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.SuppPubInfo, otherInfo_ptr->SuppPubInfo, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfSuppPubInfo));
		params.__otherInfo_ptr.SizeOfSuppPubInfo = cpu_to_le32(otherInfo_ptr->SizeOfSuppPubInfo);
		params.__otherInfo_ptr_null_flag = 0;
	}

	params.hashMode = cpu_to_le32(hashMode);
	params.DerivFunc_mode = cpu_to_le32(DerivFunc_mode);
	params.tmpBuff_ptr_size = cpu_to_le32((sizeof(CRYS_DHPrimeData_t)));
	params.SecretKeyingDataSize = cpu_to_le16(SecretKeyingDataSize);
	if (SecretKeyingData_ptr == NULL) {
		params.SecretKeyingData_ptr_null_flag = 1;
	} else {
		/* Verify array size of SecretKeyingData_ptr */
		SEP_RPC_ASSERT((SecretKeyingDataSize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.SecretKeyingData_ptr_null_flag = 0;
	}


	params.num_of_memrefs = cpu_to_le32(SEPRPC_MEMREF_NUM_CRYS_DH_X942_GetSecretData);

	diRc = DxDI_SepRpcCall(SEPRPC_AGENT_ID_CrysDh, SEPRPC_FUNC_ID_CRYS_DH_X942_GetSecretData,
		NULL, sizeof(params), (struct seprpc_params*)&params, &rpcRc);

	SEP_RPC_ASSERT(diRc == DXDI_RET_OK, CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR);
	if (rpcRc != SEPRPC_RET_OK) {
		SEP_RPC_LOG("RPC of CRYS_DH_X942_GetSecretData failed with RPC error code %d\n", rpcRc);
		switch (rpcRc) {
		case SEPRPC_RET_EINVAL:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR;
		case SEPRPC_RET_ENORSC:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_C_ERROR;
		default:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR;
		}
	}

	/* Copy back output parameters */
	if (SecretKeyingData_ptr == NULL) {
		params.SecretKeyingData_ptr_null_flag = 1;
    } else if (SecretKeyingDataSize <= (sizeof(params.SecretKeyingData_ptr) / sizeof(params.SecretKeyingData_ptr[0]))) {
		memcpy(SecretKeyingData_ptr, params.SecretKeyingData_ptr, sizeof(DxUint8_t) * (SecretKeyingDataSize));
		params.SecretKeyingData_ptr_null_flag = 0;
	}


	return le32_to_cpu(params._funcRetCode);
} /* CRYS_DH_X942_GetSecretData */



/************ CRYS_DH_X942_HybridGetSecretData ***************/
CRYSError_t CRYS_DH_X942_HybridGetSecretData(
	DxUint8_t ClientPrvKey_ptr1[],
	DxUint16_t ClientPrvKeySize1,
	DxUint8_t ClientPrvKey_ptr2[],
	DxUint16_t ClientPrvKeySize2,
	DxUint8_t ServerPubKey_ptr1[],
	DxUint16_t ServerPubKeySize1,
	DxUint8_t ServerPubKey_ptr2[],
	DxUint16_t ServerPubKeySize2,
	DxUint8_t Prime_ptr[],
	DxUint16_t PrimeSize,
	CRYS_DH_OtherInfo_t *otherInfo_ptr,
	CRYS_DH_HASH_OpMode_t hashMode,
	CRYS_DH_DerivationFunc_Mode DerivFunc_mode,
	CRYS_DH_HybrTemp_t *tmpDhHybr_ptr,
	DxUint8_t SecretKeyingData_ptr[],
	DxUint16_t SecretKeyingDataSize)
{
	DxDI_RetCode_t diRc;
	seprpc_retcode_t rpcRc;
	struct SepRpc_CRYS_DH_X942_HybridGetSecretDataParams params;

	/* Verify input parameters and copy into params. struct. */
	if (ClientPrvKey_ptr1 == NULL) {
		params.ClientPrvKey_ptr1_null_flag = 1;
	} else {
		/* Verify array size of ClientPrvKey_ptr1 */
		SEP_RPC_ASSERT((ClientPrvKeySize1) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.ClientPrvKey_ptr1, ClientPrvKey_ptr1, sizeof(DxUint8_t) * (ClientPrvKeySize1));
		params.ClientPrvKey_ptr1_null_flag = 0;
	}

	params.ClientPrvKeySize1 = cpu_to_le16(ClientPrvKeySize1);
	if (ClientPrvKey_ptr2 == NULL) {
		params.ClientPrvKey_ptr2_null_flag = 1;
	} else {
		/* Verify array size of ClientPrvKey_ptr2 */
		SEP_RPC_ASSERT((ClientPrvKeySize2) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.ClientPrvKey_ptr2, ClientPrvKey_ptr2, sizeof(DxUint8_t) * (ClientPrvKeySize2));
		params.ClientPrvKey_ptr2_null_flag = 0;
	}

	params.ClientPrvKeySize2 = cpu_to_le16(ClientPrvKeySize2);
	if (ServerPubKey_ptr1 == NULL) {
		params.ServerPubKey_ptr1_null_flag = 1;
	} else {
		/* Verify array size of ServerPubKey_ptr1 */
		SEP_RPC_ASSERT((ServerPubKeySize1) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.ServerPubKey_ptr1, ServerPubKey_ptr1, sizeof(DxUint8_t) * (ServerPubKeySize1));
		params.ServerPubKey_ptr1_null_flag = 0;
	}

	params.ServerPubKeySize1 = cpu_to_le16(ServerPubKeySize1);
	if (ServerPubKey_ptr2 == NULL) {
		params.ServerPubKey_ptr2_null_flag = 1;
	} else {
		/* Verify array size of ServerPubKey_ptr2 */
		SEP_RPC_ASSERT((ServerPubKeySize2) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.ServerPubKey_ptr2, ServerPubKey_ptr2, sizeof(DxUint8_t) * (ServerPubKeySize2));
		params.ServerPubKey_ptr2_null_flag = 0;
	}

	params.ServerPubKeySize2 = cpu_to_le16(ServerPubKeySize2);
	if (Prime_ptr == NULL) {
		params.Prime_ptr_null_flag = 1;
	} else {
		/* Verify array size of Prime_ptr */
		SEP_RPC_ASSERT((PrimeSize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.Prime_ptr, Prime_ptr, sizeof(DxUint8_t) * (PrimeSize));
		params.Prime_ptr_null_flag = 0;
	}

	params.PrimeSize = cpu_to_le16(PrimeSize);
	if (otherInfo_ptr == NULL) {
		params.__otherInfo_ptr_null_flag = 1;
	} else {
		/* Verify array size of AlgorithmID */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfAlgorithmID) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.AlgorithmID, otherInfo_ptr->AlgorithmID, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfAlgorithmID));
		params.__otherInfo_ptr.SizeOfAlgorithmID = cpu_to_le32(otherInfo_ptr->SizeOfAlgorithmID);
		/* Verify array size of PartyUInfo */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfPartyUInfo) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.PartyUInfo, otherInfo_ptr->PartyUInfo, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfPartyUInfo));
		params.__otherInfo_ptr.SizeOfPartyUInfo = cpu_to_le32(otherInfo_ptr->SizeOfPartyUInfo);
		/* Verify array size of PartyVInfo */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfPartyVInfo) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.PartyVInfo, otherInfo_ptr->PartyVInfo, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfPartyVInfo));
		params.__otherInfo_ptr.SizeOfPartyVInfo = cpu_to_le32(otherInfo_ptr->SizeOfPartyVInfo);
		/* Verify array size of SuppPrivInfo */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfSuppPrivInfo) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.SuppPrivInfo, otherInfo_ptr->SuppPrivInfo, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfSuppPrivInfo));
		params.__otherInfo_ptr.SizeOfSuppPrivInfo = cpu_to_le32(otherInfo_ptr->SizeOfSuppPrivInfo);
		/* Verify array size of SuppPubInfo */
		SEP_RPC_ASSERT((otherInfo_ptr->SizeOfSuppPubInfo) <= (CRYS_DH_MAX_SIZE_OF_OTHER_INFO_ENTRY), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.__otherInfo_ptr.SuppPubInfo, otherInfo_ptr->SuppPubInfo, sizeof(DxUint8_t) * (otherInfo_ptr->SizeOfSuppPubInfo));
		params.__otherInfo_ptr.SizeOfSuppPubInfo = cpu_to_le32(otherInfo_ptr->SizeOfSuppPubInfo);
		params.__otherInfo_ptr_null_flag = 0;
	}

	params.hashMode = cpu_to_le32(hashMode);
	params.DerivFunc_mode = cpu_to_le32(DerivFunc_mode);
	params.tmpDhHybr_ptr_size = cpu_to_le32((sizeof(CRYS_DHPrimeData_t)));
	params.SecretKeyingDataSize = cpu_to_le16(SecretKeyingDataSize);
	if (SecretKeyingData_ptr == NULL) {
		params.SecretKeyingData_ptr_null_flag = 1;
	} else {
		/* Verify array size of SecretKeyingData_ptr */
		SEP_RPC_ASSERT((SecretKeyingDataSize) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.SecretKeyingData_ptr_null_flag = 0;
	}


	params.num_of_memrefs = cpu_to_le32(SEPRPC_MEMREF_NUM_CRYS_DH_X942_HybridGetSecretData);

	diRc = DxDI_SepRpcCall(SEPRPC_AGENT_ID_CrysDh, SEPRPC_FUNC_ID_CRYS_DH_X942_HybridGetSecretData,
		NULL, sizeof(params), (struct seprpc_params*)&params, &rpcRc);

	SEP_RPC_ASSERT(diRc == DXDI_RET_OK, CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR);
	if (rpcRc != SEPRPC_RET_OK) {
		SEP_RPC_LOG("RPC of CRYS_DH_X942_HybridGetSecretData failed with RPC error code %d\n", rpcRc);
		switch (rpcRc) {
		case SEPRPC_RET_EINVAL:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR;
		case SEPRPC_RET_ENORSC:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_C_ERROR;
		default:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR;
		}
	}

	/* Copy back output parameters */
	if (SecretKeyingData_ptr == NULL) {
		params.SecretKeyingData_ptr_null_flag = 1;
    } else if (SecretKeyingDataSize <= (sizeof(params.SecretKeyingData_ptr) / sizeof(params.SecretKeyingData_ptr[0]))) {
		memcpy(SecretKeyingData_ptr, params.SecretKeyingData_ptr, sizeof(DxUint8_t) * (SecretKeyingDataSize));
		params.SecretKeyingData_ptr_null_flag = 0;
	}


	return le32_to_cpu(params._funcRetCode);
} /* CRYS_DH_X942_HybridGetSecretData */



/************ CRYS_DH_CreateDomainParams ***************/
CRYSError_t CRYS_DH_CreateDomainParams(
	DxUint32_t modPsizeBits,
	DxUint32_t orderQsizeBits,
	DxUint32_t seedSizeBits,
	DxUint8_t modP_ptr[],
	DxUint8_t orderQ_ptr[],
	DxUint8_t generatorG_ptr[],
	DxUint32_t *generGsizeBytes_ptr,
	DxUint8_t factorJ_ptr[],
	DxUint32_t *JsizeBytes_ptr,
	DxUint8_t seedS_ptr[],
	DxInt8_t generateSeed,
	DxUint32_t *pgenCounter_ptr,
	CRYS_DHKGData_t *DHKGbuff_ptr)
{
	DxDI_RetCode_t diRc;
	seprpc_retcode_t rpcRc;
	struct SepRpc_CRYS_DH_CreateDomainParamsParams params;

	/* Verify input parameters and copy into params. struct. */
	params.modPsizeBits = cpu_to_le32(modPsizeBits);
	params.orderQsizeBits = cpu_to_le32(orderQsizeBits);
	params.seedSizeBits = cpu_to_le32(seedSizeBits);
	params.generateSeed = generateSeed;
	params.DHKGbuff_ptr_size = cpu_to_le32((sizeof(CRYS_DHKGData_t)));
	if (generGsizeBytes_ptr == NULL) {
		params.__generGsizeBytes_ptr_null_flag = 1;
	} else {
		params.__generGsizeBytes_ptr = cpu_to_le32(*(generGsizeBytes_ptr));
		params.__generGsizeBytes_ptr_null_flag = 0;
	}

	if (JsizeBytes_ptr == NULL) {
		params.__JsizeBytes_ptr_null_flag = 1;
	} else {
		params.__JsizeBytes_ptr = cpu_to_le32(*(JsizeBytes_ptr));
		params.__JsizeBytes_ptr_null_flag = 0;
	}

	if (seedS_ptr == NULL) {
		params.seedS_ptr_null_flag = 1;
	} else {
		/* Verify array size of seedS_ptr */
		SEP_RPC_ASSERT(((seedSizeBits+7)/8) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.seedS_ptr, seedS_ptr, sizeof(DxUint8_t) * ((seedSizeBits+7)/8));
		params.seedS_ptr_null_flag = 0;
	}

	if (modP_ptr == NULL) {
		params.modP_ptr_null_flag = 1;
	} else {
		/* Verify array size of modP_ptr */
		SEP_RPC_ASSERT(((modPsizeBits+7)/8) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.modP_ptr_null_flag = 0;
	}

	if (orderQ_ptr == NULL) {
		params.orderQ_ptr_null_flag = 1;
	} else {
		/* Verify array size of orderQ_ptr */
		SEP_RPC_ASSERT(((orderQsizeBits+7)/8) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.orderQ_ptr_null_flag = 0;
	}

	if (generatorG_ptr == NULL) {
		params.generatorG_ptr_null_flag = 1;
    } else if (generGsizeBytes_ptr != NULL) {
		/* Verify array size of generatorG_ptr */
		SEP_RPC_ASSERT((*generGsizeBytes_ptr) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.generatorG_ptr_null_flag = 0;
	}

	if (factorJ_ptr == NULL) {
		params.factorJ_ptr_null_flag = 1;
    } else if (JsizeBytes_ptr != NULL) {
		/* Verify array size of factorJ_ptr */
		SEP_RPC_ASSERT((*JsizeBytes_ptr) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		params.factorJ_ptr_null_flag = 0;
	}

	if (pgenCounter_ptr == NULL) {
		params.__pgenCounter_ptr_null_flag = 1;
	} else {
		params.__pgenCounter_ptr_null_flag = 0;
	}


	params.num_of_memrefs = cpu_to_le32(SEPRPC_MEMREF_NUM_CRYS_DH_CreateDomainParams);

	diRc = DxDI_SepRpcCall(SEPRPC_AGENT_ID_CrysDh, SEPRPC_FUNC_ID_CRYS_DH_CreateDomainParams,
		NULL, sizeof(params), (struct seprpc_params*)&params, &rpcRc);

	SEP_RPC_ASSERT(diRc == DXDI_RET_OK, CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR);
	if (rpcRc != SEPRPC_RET_OK) {
		SEP_RPC_LOG("RPC of CRYS_DH_CreateDomainParams failed with RPC error code %d\n", rpcRc);
		switch (rpcRc) {
		case SEPRPC_RET_EINVAL:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR;
		case SEPRPC_RET_ENORSC:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_C_ERROR;
		default:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR;
		}
	}

	/* Copy back output parameters */
	if (generGsizeBytes_ptr == NULL) {
		params.__generGsizeBytes_ptr_null_flag = 1;
	} else {
		*(generGsizeBytes_ptr) = le32_to_cpu(params.__generGsizeBytes_ptr);
		params.__generGsizeBytes_ptr_null_flag = 0;
	}

	if (JsizeBytes_ptr == NULL) {
		params.__JsizeBytes_ptr_null_flag = 1;
	} else {
		*(JsizeBytes_ptr) = le32_to_cpu(params.__JsizeBytes_ptr);
		params.__JsizeBytes_ptr_null_flag = 0;
	}

	if (seedS_ptr == NULL) {
		params.seedS_ptr_null_flag = 1;
	} else {
		memcpy(seedS_ptr, params.seedS_ptr, sizeof(DxUint8_t) * ((seedSizeBits+7)/8));
		params.seedS_ptr_null_flag = 0;
	}

	if (modP_ptr == NULL) {
		params.modP_ptr_null_flag = 1;
	} else {
		memcpy(modP_ptr, params.modP_ptr, sizeof(DxUint8_t) * ((modPsizeBits+7)/8));
		params.modP_ptr_null_flag = 0;
	}

	if (orderQ_ptr == NULL) {
		params.orderQ_ptr_null_flag = 1;
	} else {
		memcpy(orderQ_ptr, params.orderQ_ptr, sizeof(DxUint8_t) * ((orderQsizeBits+7)/8));
		params.orderQ_ptr_null_flag = 0;
	}

	if (generatorG_ptr == NULL) {
		params.generatorG_ptr_null_flag = 1;
    } else if (generGsizeBytes_ptr != NULL) {
		memcpy(generatorG_ptr, params.generatorG_ptr, sizeof(DxUint8_t) * (*generGsizeBytes_ptr));
		params.generatorG_ptr_null_flag = 0;
	}

	if (factorJ_ptr == NULL) {
		params.factorJ_ptr_null_flag = 1;
    } else if (JsizeBytes_ptr != NULL) {
		memcpy(factorJ_ptr, params.factorJ_ptr, sizeof(DxUint8_t) * (*JsizeBytes_ptr));
		params.factorJ_ptr_null_flag = 0;
	}

	if (pgenCounter_ptr == NULL) {
		params.__pgenCounter_ptr_null_flag = 1;
	} else {
		*(pgenCounter_ptr) = le32_to_cpu(params.__pgenCounter_ptr);
		params.__pgenCounter_ptr_null_flag = 0;
	}


	return le32_to_cpu(params._funcRetCode);
} /* CRYS_DH_CreateDomainParams */



/************ CRYS_DH_CheckDomainParams ***************/
CRYSError_t CRYS_DH_CheckDomainParams(
	DxUint8_t modP_ptr[],
	DxUint32_t modPsizeBytes,
	DxUint8_t orderQ_ptr[],
	DxUint32_t orderQsizeBytes,
	DxUint8_t generatorG_ptr[],
	DxUint32_t generatorSizeBytes,
	DxUint8_t seedS_ptr[],
	DxUint32_t seedSizeBits,
	DxUint32_t pgenCounter,
	CRYS_DHKG_CheckTemp_t *checkTempBuff_ptr)
{
	DxDI_RetCode_t diRc;
	seprpc_retcode_t rpcRc;
	struct SepRpc_CRYS_DH_CheckDomainParamsParams params;

	/* Verify input parameters and copy into params. struct. */
	if (modP_ptr == NULL) {
		params.modP_ptr_null_flag = 1;
	} else {
		/* Verify array size of modP_ptr */
		SEP_RPC_ASSERT((modPsizeBytes) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.modP_ptr, modP_ptr, sizeof(DxUint8_t) * (modPsizeBytes));
		params.modP_ptr_null_flag = 0;
	}

	params.modPsizeBytes = cpu_to_le32(modPsizeBytes);
	if (orderQ_ptr == NULL) {
		params.orderQ_ptr_null_flag = 1;
	} else {
		/* Verify array size of orderQ_ptr */
		SEP_RPC_ASSERT((orderQsizeBytes) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.orderQ_ptr, orderQ_ptr, sizeof(DxUint8_t) * (orderQsizeBytes));
		params.orderQ_ptr_null_flag = 0;
	}

	params.orderQsizeBytes = cpu_to_le32(orderQsizeBytes);
	if (generatorG_ptr == NULL) {
		params.generatorG_ptr_null_flag = 1;
	} else {
		/* Verify array size of generatorG_ptr */
		SEP_RPC_ASSERT((generatorSizeBytes) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.generatorG_ptr, generatorG_ptr, sizeof(DxUint8_t) * (generatorSizeBytes));
		params.generatorG_ptr_null_flag = 0;
	}

	params.generatorSizeBytes = cpu_to_le32(generatorSizeBytes);
	if (seedS_ptr == NULL) {
		params.seedS_ptr_null_flag = 1;
	} else {
		/* Verify array size of seedS_ptr */
		SEP_RPC_ASSERT(((seedSizeBits+7)/8) <= (CRYS_DH_MAX_MOD_SIZE_IN_BYTES), CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR);
		memcpy(params.seedS_ptr, seedS_ptr, sizeof(DxUint8_t) * ((seedSizeBits+7)/8));
		params.seedS_ptr_null_flag = 0;
	}

	params.seedSizeBits = cpu_to_le32(seedSizeBits);
	params.pgenCounter = cpu_to_le32(pgenCounter);
	params.checkTempBuff_ptr_size = cpu_to_le32((sizeof(CRYS_DHKG_CheckTemp_t)));

	params.num_of_memrefs = cpu_to_le32(SEPRPC_MEMREF_NUM_CRYS_DH_CheckDomainParams);

	diRc = DxDI_SepRpcCall(SEPRPC_AGENT_ID_CrysDh, SEPRPC_FUNC_ID_CRYS_DH_CheckDomainParams,
		NULL, sizeof(params), (struct seprpc_params*)&params, &rpcRc);

	SEP_RPC_ASSERT(diRc == DXDI_RET_OK, CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR);
	if (rpcRc != SEPRPC_RET_OK) {
		SEP_RPC_LOG("RPC of CRYS_DH_CheckDomainParams failed with RPC error code %d\n", rpcRc);
		switch (rpcRc) {
		case SEPRPC_RET_EINVAL:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_B_ERROR;
		case SEPRPC_RET_ENORSC:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_C_ERROR;
		default:
			return CRYS_DH_HOST_MSG_GENERAL_RPC_A_ERROR;
		}
	}

	return le32_to_cpu(params._funcRetCode);
} /* CRYS_DH_CheckDomainParams */
