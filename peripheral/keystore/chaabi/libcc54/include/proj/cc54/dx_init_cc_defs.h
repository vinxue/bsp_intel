/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef __DX_INIT_CC_DEFS__H__
#define __DX_INIT_CC_DEFS__H__

/** @file dx_init_cc_defs.h
*  \brief definitions for the CC54 initialization API
*
*  \version
*  \author avis
*/

/* message token to sep */
/* CC_INIT definitions */
#define DX_CC_INIT_HEAD_MSG_TOKEN		0X544B2FBAUL

/*The below enumerator includes the offsets inside the CC_Init message*/
/*The last enum is the length of the message */
enum dx_cc_init_msg_offset {
	DX_CC_INIT_MSG_TOKEN_OFFSET,
	DX_CC_INIT_MSG_LENGTH_OFFSET,
	DX_CC_INIT_MSG_OP_CODE_OFFSET,
	DX_CC_INIT_MSG_FLAGS_OFFSET,
	DX_CC_INIT_MSG_RESIDENT_IMAGE_OFFSET,
	DX_CC_INIT_MSG_I_CACHE_IMAGE_OFFSET,
	DX_CC_INIT_MSG_I_CACHE_DEST_OFFSET,
	DX_CC_INIT_MSG_I_CACHE_SIZE_OFFSET,
	DX_CC_INIT_MSG_D_CACHE_ADDR_OFFSET,
	DX_CC_INIT_MSG_D_CACHE_SIZE_OFFSET,
	DX_CC_INIT_MSG_CC_INIT_EXT_ADDR_OFFSET,
	DX_CC_INIT_MSG_USER_CONFIG_OFFSET,
	DX_CC_INIT_MSG_VRL_ADDR_OFFSET,
	DX_CC_INIT_MSG_MAGIC_NUM_OFFSET,
	DX_CC_INIT_MSG_KEY_INDEX_OFFSET,
	DX_CC_INIT_MSG_KEY_HASH_0_OFFSET,
	DX_CC_INIT_MSG_KEY_HASH_1_OFFSET,
	DX_CC_INIT_MSG_KEY_HASH_2_OFFSET,
	DX_CC_INIT_MSG_KEY_HASH_3_OFFSET,
	DX_CC_INIT_MSG_CHECK_SUM_OFFSET,
	DX_CC_INIT_MSG_LENGTH
};

/* Set this value if key used in the VRL is to be verified against KEY_HASH
   fields in the CC_INIT message */
#define DX_CC_INIT_MSG_VRL_KEY_INDEX_INVALID 0xFFFFFFFF

enum dx_cc_init_msg_icache_size {
	DX_CC_INIT_MSG_ICACHE_SCR_DISABLE_SIZE,
	DX_CC_INIT_MSG_ICACHE_SCR_256K_SIZE,
	DX_CC_INIT_MSG_ICACHE_SCR_1M_SIZE,
	DX_CC_INIT_MSG_ICACHE_SCR_2M_SIZE,
	DX_CC_INIT_MSG_ICACHE_SCR_4M_SIZE,
	DX_CC_INIT_MSG_ICACHE_SCR_INVALID_SIZE
};
/* Icache sizes enum to log2 -
 * Map enum of dx_cc_init_msg_icache_size to log2(size)
 * (-1) for invalid value. */
#define DX_CC_ICACHE_SIZE_ENUM2LOG { -1, 18, 20, 21, 22, -1 }

#define DX_CC_INIT_D_CACHE_MIN_SIZE_LOG2 17 /* 128KB */
#define DX_CC_INIT_D_CACHE_MIN_SIZE (1 << DX_CC_INIT_D_CACHE_MIN_SIZE_LOG2)
#define DX_CC_INIT_D_CACHE_MAX_SIZE_LOG2 27 /* 128MB */
#define DX_CC_INIT_D_CACHE_MAX_SIZE (1 << DX_CC_INIT_D_CACHE_MAX_SIZE_LOG2)

/* Bit flags for the CC_Init flags word*/
/* The CC_Init resident address address is valid (it might be passed via VRL) */
#define DX_CC_INIT_FLAGS_RESIDENT_ADDR_FLAG		0x00000001
/* The CC_Init I$ address address is valid (it might be passed via VRL) */
#define DX_CC_INIT_FLAGS_I_CACHE_ADDR_FLAG		0x00000002
/* The CC_Init D$ address address is valid (First CC_Init does not config. D$)*/
#define DX_CC_INIT_FLAGS_D_CACHE_EXIST_FLAG		0x00000004
/* The CC_Init extension address is valid and should be used */
#define DX_CC_INIT_FLAGS_INIT_EXT_FLAG			0x00000008
/* The I$ (and applets) should be encrypted */
#define DX_CC_INIT_FLAGS_CACHE_ENC_FLAG			0x00000010
/* The I$ (and applets) should be scrambled */
#define DX_CC_INIT_FLAGS_CACHE_SCRAMBLE_FLAG		0x00000020
/* The I$ (and applets) should be copied to new address (Icache address) */
#define DX_CC_INIT_FLAGS_CACHE_COPY_FLAG		0x00000040
/* use the magic number in the CC_Init to verify the  VRL */
#define DX_CC_INIT_FLAGS_MAGIC_NUMBER_FLAG		0x00000080


#define DX_CC_INIT_FLAGS_CACHE_COPY_MASK_FLAG \
	(DX_CC_INIT_FLAGS_CACHE_ENC_FLAG | \
	DX_CC_INIT_FLAGS_CACHE_SCRAMBLE_FLAG | \
	DX_CC_INIT_FLAGS_CACHE_COPY_FLAG)

/*-------------------------------
  STRUCTURES
---------------------------------*/
struct dx_cc_def_applet_msg {
        uint32_t 			cc_flags;
	uint32_t 			icache_image_addr;
        uint32_t 			vrl_addr;
        uint32_t 			magic_num;
        uint32_t 			ver_key_index;
	uint32_t 			hashed_key_val[4];
};

/**
 * struct dx_cc_init_msg - used for passing the parameters to the CC_Init API.
 * The structure is converted in to the CC_Init message
 * @cc_flags:		Bits flags for different fields in the message
 * @res_image_addr:	resident image address in the HOST memory
 * @Icache_image_addr:	I$ image address in the HOST memeory
 * @Icache_addr:	I$ memory allocation in case the I$ is not placed
 *			(scramble or encrypted I$)
 * @Icache_size:	Icache size ( The totoal I$ for Dx image and all
 *			applets). The size is limited to: 256K,1M,2M and 4M.
 * @Dcache_addr:	D$ memory allocation in the HOST memory
 * @Dcache_size:	D$ memory allocation size
 * @init_ext_addr:	Address of the cc_Init extension message in the HOST
 * @vrl_addr:		The address of teh VRL in the HOST memory
 * @magic_num:		Requested VRL magic number
 * @ver_key_index:	The index of the verification key
 * @output_buff_addr:	buffer to the HOST memeory, where the secure boot
 *			process might write the results of the secure boot
 * @output_buff_size:	size of the out put buffer ( in bytes)
 * @Hashed_key_val:	the trunked hash value of teh verification key in case
 *			the OTP keys are not in use
*/

struct dx_cc_init_msg {
        uint32_t 			cc_flags;
        uint32_t 			res_image_addr;
	uint32_t 			icache_image_addr;
	uint32_t 			icache_addr;
        enum dx_cc_init_msg_icache_size	icache_size;
        uint32_t 			dcache_addr;
        uint32_t 			dcache_size;
        uint32_t 			init_ext_addr;
	uint32_t 			user_config;
        uint32_t 			vrl_addr;
        uint32_t 			magic_num;
        uint32_t 			ver_key_index;
	uint32_t 			hashed_key_val[4];
};

#endif /*__DX_INIT_CC_DEFS__H__*/
