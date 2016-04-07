/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef __TEE_TOKEN_IF_H
#define __TEE_TOKEN_IF_H

#define PROCESS_TOKEN_REQUEST 0x70c3

#define TEE_TOKEN_FLAGS_INTENT 1
#define TEE_TOKEN_VERSION (0 << 8)

#define SPID_RAW_TAG 0x44495053
#define SNU_RAW_TAG  0x00554e53
#define FRU_RAW_TAG  0x00555246

#define TEE_TOKEN_MAX_PAYLOAD_SIZE_UNPROTECTED 4096
#define TOKEN_PSID_LENGTH 16
#define TOKEN_NONCE_LENGTH 16
#define TOKEN_MAC_LENGTH 16
#define TEE_TOKEN_VRL_SIZE 728
#define TEE_TOKEN_WVDRM_TOKEN_DG 31
enum
{
    SPID_DGID = 1,
    SNU_DGID = 5,
    FRU_DGID
};

struct SPID_RAW
{
    uint32_t tag;
    uint16_t id[6];
};
struct SNU_RAW
{
    uint32_t tag;
    uint8_t serialnumber[32];
};
struct FRU_RAW
{
    uint32_t tag;
    uint8_t fru_value[10];
};

union raw_token
{
    struct SPID_RAW spid;
    struct SNU_RAW  snu;
    struct FRU_RAW  fru;
};

// Data Group Header
struct tee_token_datagroup_info
{
    uint32_t DATA;      // magic value 'DATA'
    uint32_t id;
    uint32_t size;      // datagroup size in dwords (includes header)
    uint32_t version;
    uint32_t subgroups; // number of subgroups
    uint32_t flags;
    uint32_t reserved;  // total 28 bytes
};

// Sub Group Header
struct tee_token_subgroup_info
{
    uint32_t SUBG;        // magic value 'SUBG'
    uint32_t id;
    uint32_t size;        // subgroup size in dwords (including header)
    uint32_t cryptid[4];  // 16 bytes
    uint32_t itemcount;   // number of items
    uint32_t flags;       // total 36 bytes
};

// Item Header
struct tee_token_item_info
{
    uint32_t ITEM;        // magic value 'ITEM'
    uint32_t id;
    uint32_t size;        // size of item + header in dwords
    uint8_t AC_DATA;      // could use AC:4, DATA:4
    uint8_t padding;      // number of added bytes (to next uint32_t boundary)
    uint8_t reserved[2];  // Total 16 bytes
};

struct tee_token_lifetime
{
    uint32_t timestamp;
    uint16_t lifetime;      // unit = hours
    uint16_t token_size;
    uint8_t  flags;
    uint8_t  token_type;
    uint8_t  subtype;
    uint8_t  reserved;
    uint32_t reserved2;     // total 16 bytes
};

struct tee_token_info
{
    uint32_t formatVersion;
    uint32_t tokenID;
    uint8_t  PSID[TOKEN_PSID_LENGTH];      // Part Specific ID
    uint8_t  tsMAC[TOKEN_MAC_LENGTH];      // Time stamp MAC
    uint8_t  ltnonce[TOKEN_NONCE_LENGTH];  // Token Lifetime Nonce
    struct tee_token_lifetime lifetime;    // total = 72 bytes
};

struct tee_token_ctx
{
    uint32_t *base_address;
    uint32_t current_length;
    uint32_t allocated_length;
};

/*!
 *  \brief tee_token_unprotected_start    creates a context for online token generation
 *  \param ctx                            user area for context
 *  \param dg_id                          data group id of the token to write to
 *  \param flags                          reserved
 */
int32_t tee_token_unprotected_start(struct tee_token_ctx *ctx, uint32_t dg_id, uint32_t flags);

/*!
 *  \brief tee_token_unprotected_end      writes the token to secure storage unprovisioned
 *  \param ctx                            user area for context
 *  \param flags                          reserved
 */
int32_t tee_token_unprotected_end(struct tee_token_ctx *ctx, uint32_t flags);

/*!
 *  \brief tee_token_unprotected_write    appends or replaces a token item to the context
 *  \param ctx                            user area for context
 *  \param sg_id                          subgroup id of the token item
 *  \param item_id                        token item id
 *  \param buf                            token item data
 *  \param size                           payload size of the token item
 *  \param flags                          reserved
 */
int32_t tee_token_unprotected_write(struct tee_token_ctx *ctx, uint32_t sg_id,
                                    uint32_t item_id, const uint8_t *buf, size_t size, uint32_t flags);

/*!
 *  \brief tee_token_write    writes a raw buffer as a token
 */
int32_t tee_token_write_tmp(uint8_t *buf, size_t size, uint32_t flags);

/*!
 *  \brief tee_token_write    writes a (formatted) raw buffer as a token
 */
int32_t tee_token_write(uint8_t *buf, size_t size, uint32_t flags);

/*!
 *  \brief tee_token_remove      removes a token specified by dg_id
 *  \param dg_id                 data group ID
 *  \flags                       reserved
 *  \note                        for unsigned tokens only
 */
int32_t tee_token_remove(uint32_t dg_id, uint32_t flags);

/*!
 *  \brief tee_token_validate    validates a specific token
 *  \flags                       reserved
 *  \param dg_id                 data group ID
 */
int32_t tee_token_validate(uint32_t dg_id, uint32_t flags);

/*!
 *  \brief tee_token_validate_all  validates a specific token
 *  \flags                       reserved
 *  \param dg_id                 data group ID
 */
int32_t tee_token_validate_all(uint32_t flags);

/*!
 *  \brief tee_token_update_start   marks beginning of intent token transaction
 *  \flags                       reserved
 */
int32_t tee_token_update_start(uint32_t flags);

/*!
 *  \brief tee_token_update_cancel   cancels intent token transaction
 *  \flags                       reserved
 */
int32_t tee_token_update_cancel(uint32_t flags);

/*!
 *  \brief tee_token_update_end  ends intent token transaction
 *  \flags                       reserved
 */
int32_t tee_token_update_end(uint32_t flags);

/*!
 *  \brief tee_token_unformatted_write   writes a specific token item
 *  \param dg_id                 data group ID
 *  \param sg_id                 subgroup ID
 *  \param item_id               item ID to read
 *  \param data_buf              source data
 *  \param buf_size              number of bytes to write
 *  \flags                       intent state
 */
int32_t tee_token_unformatted_write(uint32_t dg_id, uint32_t sg_id, uint32_t item_id,
                                    uint8_t *data_buf, size_t size, uint32_t flags);

/*!
 *  \brief tee_token_item_read   reads a token item
 *  \param dg_id                 data group ID
 *  \param sg_id                 subgroup ID
 *  \param item_id               item ID to read
 *  \param offset                Byte offset from the beginning of Item Data
 *  \param data_buf              destination of read
 *  \param buf_size              number of bytes to read
 *  \param flags                 intent state
 */
int32_t tee_token_item_read(uint32_t dg_id, uint32_t sg_id, uint32_t item_id,
            size_t offset, uint8_t *data_buf, size_t buf_size, uint32_t flags);

/*!
 *  \brief tee_token_item_size_get   returns the byte size of a token item
 *  \param dg_id                 data group ID
 *  \param sg_id                 subgroup ID
 *  \param item_id               item ID to read
 *  \param item_size             pointer to token item size
 *  \param flags                 intent state
 */
int32_t tee_token_item_size_get(uint32_t dg_id, uint32_t sg_id, uint32_t item_id,
                                size_t *item_size, uint32_t flags);

/*!
 *  \brief tee_token_item_info_get   reads token item header
 *  \param dg_id                 data group ID
 *  \param sg_id                 subgroup ID
 *  \param item_id               item ID to read
 *  \param info                  pointer to token item header
 *  \param flags                 intent state
 */
int32_t tee_token_item_info_get(uint32_t dg_id, uint32_t sg_id, uint32_t item_id,
                                struct tee_token_item_info *info, uint32_t flags);
/*!
 *  \brief tee_token_itemids_get reads a list of available item ids
 *  \param dg_id                 data group ID
 *  \param sg_id                 subgroup ID
 *  \param items                 pointer to list (allocated by callee)
 *  \param count                 number of ids (populated by callee)
 *  \param flags                 intent state
 */
int32_t tee_token_itemids_get(uint32_t dg_id, uint32_t sg_id, uint32_t **items,
                             size_t *count, uint32_t flags);

/*!
 *  \brief tee_token_sgids_get   reads a list of available subgroup ids
 *  \param dg_id                 data group ID
 *  \param items                 pointer to list (allocated by callee)
 *  \param count                 number of ids (populated by callee)
 *  \param flags                 intent state
 */
int32_t tee_token_sgids_get(uint32_t dg_id, uint32_t **items, size_t *count, uint32_t flags);

/*!
 *  \brief tee_token_dgids_get   reads a list of available datagroup ids
 *  \param items                 pointer to list (allocated by callee)
 *  \param count                 number of ids (populated by callee)
 *  \param flags                 intent state
 */
int32_t tee_token_dgids_get(uint32_t **items, size_t *count, uint32_t flags);

/*!
 *  \brief tee_token_sgroup_info_get    reads subgroup header
 *  \param dg_id                 data group ID
 *  \param sg_id                 subgroup ID
 *  \param info                  pointer to datagroup header
 *  \param flags                 intent state
 */
int32_t tee_token_sgroup_info_get(uint32_t dg_id, uint32_t sg_id,
                                  struct tee_token_subgroup_info *info,
                                  uint32_t flags);

/*!
 *  \brief tee_token_dgroup_info_get reads datagroup header
 *  \param dg_id                 data group ID
 *  \param info                  pointer to datagroup header
 *  \param flags                 intent state
 */
int32_t tee_token_dgroup_info_get(uint32_t dg, struct tee_token_datagroup_info *info,
                                  uint32_t flags);

/*!
 *  \brief tee_token_info_get    reads token header
 *  \param dg_id                 data group ID
 *  \param info                  pointer to token header
 *  \param flags                 intent state
 */
int32_t tee_token_info_get(uint32_t dg_id, struct tee_token_info *info, uint32_t flags);

/*!
 *  \brief tee_token_read        reads a raw token
 *  \param dg_id                 data group ID
 *  \param data_buf              pointer to raw token
 *  \param data_size             number of bytes allocated
 *  \param flags                 intent state
 */
int32_t tee_token_read(uint32_t dg_id, uint8_t *data_buf, uint32_t data_size, uint32_t flags);

/*!
 *  \brief tee_token_count_get   receives the number of tokens
 *  \param flags                 intent state / container (SECURE_STORAGE) etc.
 */
int32_t tee_token_count_get(size_t *count, uint32_t flags);

/*!
 *  \brief tee_partid_get        reads part specific id
 *  \param partid_buf            result buffer
 */
int32_t tee_partid_get(uint8_t partid_buf[TOKEN_PSID_LENGTH]);

/*!
 *  \brief tee_token_minver_get  reads minimum version of a token from TVT
 *  \param dg_id                 data group ID
 *  \param min_ver               pointer to result
 *  \param flags                 unspecified
 */
int32_t tee_token_minver_get(uint32_t dg_id, uint32_t *min_ver, uint32_t flags);

/*!
 *  \brief tee_token_lifetimedata_get    gets device specific lifetime data
 *  \param timestamp             pointer for retrieving Token Lifetime Timestamp
 *  \param nonce                 pointer for retrieving Token Lifetime Nonce
 *  \param mac                   pointer for retrieving Token Timestamp MAC
 */
int32_t tee_token_lifetimedata_get(uint32_t *timestamp,
                                   uint8_t nonce[TOKEN_NONCE_LENGTH],
                                   uint8_t mac[TOKEN_MAC_LENGTH]);
#endif /* end of __TEE_TOKEN_IF_H */
