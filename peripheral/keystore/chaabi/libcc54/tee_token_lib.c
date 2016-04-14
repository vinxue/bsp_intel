/*******************************************************************
* (c) Copyright 2013-2016 Intel Corporation.                       *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "tee_token_if.h"
#include "tee_token_error.h"
#include "tee_client_api.h"
#include "dx_cclib.h"

// Default/Primary Applet UUID -- TODO: use Token Feature Applet
static const uint8_t applet_uuid[] = { 0x00, 0xDE, 0xFA, 0x01, 0xDE, 0xFA, 0x02,
0xDE, 0xFA, 0x03, 0xDE, 0xFA, 0x04, 0xDE, 0xFA, 0xFF };

// Header version is passed as part of the 'flags' parameter
#define TOKEN_HDR_VERSION 0
#define PRINT_ERRORS 0

enum operation
{
    TOKEN_OP_QUERY = 0,
    TOKEN_OP_WRITE = 1,
    TOKEN_OP_WRITE_TMP = 2,
    TOKEN_OP_GET_HWID = 3,
    TOKEN_OP_WRUNF = 4,     // unformatted write -- no integrity check
    TOKEN_OP_REMOVE = 5,
    TOKEN_OP_VALIDATE = 6,
    TOKEN_OP_VALIDATE_ALL = 7,
    TOKEN_OP_START = 8,
    TOKEN_OP_CANCEL = 9,
    TOKEN_OP_END = 10,
    TOKEN_OP_COUNT = 11,
    TOKEN_OP_DGIDS = 12,
    TOKEN_OP_LIFETIME = 13,
    TOKEN_OP_MINVERS = 14
};

struct query_s
{
    uint32_t datagroupID;
    uint32_t subgroupID;
    uint32_t itemID;
    int32_t  offset;
    uint32_t length;
    uint32_t flags;
};

static int pass_to_fw(TEEC_Operation *operation, TEEC_SharedMemory *buffer, uint32_t opcode)
{
    TEEC_Context context;
    TEEC_Session session;
    uint32_t dummy;

    TEEC_Result ret = DX_CclibInit();
    if (ret != TEEC_SUCCESS)
    {
#if PRINT_ERRORS > 0
        fprintf(stderr, "DX_CclibInit() FAILED, ret = %p (%d)\n", ret, ret);
#endif
        ret = TEE_TOKEN_LIB_DX_INIT_ERROR;
        goto fin0;
    }
    ret = TEEC_InitializeContext(NULL, &context);
    if (ret != TEEC_SUCCESS)
    {
#if PRINT_ERRORS > 0
        fprintf(stderr, "TEEC_InitializeContext() FAILED, ret = %p (%d)\n", ret, ret);
#endif
        ret = TEE_TOKEN_LIB_INIT_CTX_ERROR;
        goto fin1;
    }

    if (buffer != NULL)
    {
        ret = TEEC_RegisterSharedMemory(&context, buffer);
        if (ret != TEEC_SUCCESS)
        {
#if PRINT_ERRORS > 0
            fprintf(stderr, "TEEC_RegisterSharedMemory() FAILED, ret = %p (%d)\n", ret, ret);
#endif
            ret = TEE_TOKEN_LIB_SHARED_MEM_ERROR;
            goto fin2;
        }
    }

    ret = TEEC_OpenSession(&context, &session, (TEEC_UUID *)&applet_uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &dummy);

    if (ret != TEEC_SUCCESS)
    {
#if PRINT_ERRORS > 0
        fprintf(stderr, "TEEC_OpenSession() FAILED, ret = %p (%d)\n", ret, ret);
#endif
        goto fin2;
    }

    ret = TEEC_InvokeCommand(&session, PROCESS_TOKEN_REQUEST + opcode,
                             operation, &dummy);

    TEEC_CloseSession(&session);
fin2:
    if (buffer != NULL)
    {
        TEEC_ReleaseSharedMemory(buffer);
    }
    TEEC_FinalizeContext(&context);
fin1:
    DX_CclibFini();
fin0:
    return ret;
}

// passes 'struct query_s' and a dma handle to applet
static int tee_token_handle_data_query(struct query_s *q, uint8_t *buffer, uint32_t opcode)
{
    TEEC_SharedMemory inout = {
            .size = q->length,
            .buffer = buffer,
            .flags = (opcode == TOKEN_OP_WRITE || opcode == TOKEN_OP_WRITE_TMP)
                     ? TEEC_MEM_INPUT : TEEC_MEM_OUTPUT
    };
    TEEC_Operation operation;
    memset(&operation, 0, sizeof(operation));

    TEEC_PARAM_TYPE_SET(operation.paramTypes, 0, TEEC_VALUE_INPUT);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 1, TEEC_VALUE_INPUT);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 2, TEEC_VALUE_INPUT);
    TEEC_PARAM_TYPE_SET(operation.paramTypes, 3, TEEC_MEMREF_WHOLE);

    operation.params[0].value.a = q->datagroupID;
    operation.params[0].value.b = q->subgroupID;
    operation.params[1].value.a = q->itemID;
    operation.params[1].value.b = q->offset;
    operation.params[2].value.a = q->length;
    operation.params[2].value.b = q->flags | TEE_TOKEN_VERSION;
    operation.params[3].memref.parent = &inout;
    int ret = pass_to_fw(&operation, &inout, opcode);

    return ret;
}

//  Auxiliary function to wrap token calls without a local buffer
//  some API functions are tied to a specific (non-zero) dg_id
//  some have no parameters ('flags' parameter is currently ignored)

static int tee_token_pass_command(uint32_t command, uint32_t flags, ...)
{
    uint32_t dg_id = 0;
    uint8_t dummy_buffer[8];

    // two commands expect a parameter (dg_id)

    if (command == TOKEN_OP_REMOVE ||
        command == TOKEN_OP_VALIDATE)
    {
        va_list argp;
        va_start(argp, flags);

        dg_id = va_arg(argp, int);
        if (dg_id == 0)
        {
            return TEE_TOKEN_INVALID_PARAM;
        }
        va_end(argp);
    }

    struct query_s query = {
        .datagroupID = dg_id,
        .subgroupID = 0,
        .itemID = 0,
        .offset = 0,
        .length = sizeof(dummy_buffer),
        .flags = flags
    };

    return tee_token_handle_data_query(&query, dummy_buffer, command);
}

int32_t tee_token_remove(uint32_t dg_id, uint32_t flags)
{
    return tee_token_pass_command(TOKEN_OP_REMOVE, flags, dg_id);
}

int32_t tee_token_validate(uint32_t dg_id, uint32_t flags)
{
    return tee_token_pass_command(TOKEN_OP_VALIDATE, flags, dg_id);
}

int32_t tee_token_validate_all(uint32_t flags)
{
    return tee_token_pass_command(TOKEN_OP_VALIDATE_ALL, flags);
}

int32_t tee_token_update_start(uint32_t flags)
{
    return tee_token_pass_command(TOKEN_OP_START, flags);
}

int32_t tee_token_update_cancel(uint32_t flags)
{
    return tee_token_pass_command(TOKEN_OP_CANCEL, flags);
}

int32_t tee_token_update_end(uint32_t flags)
{
    return tee_token_pass_command(TOKEN_OP_END, flags);
}

int32_t tee_token_write(uint8_t *buf, size_t size, uint32_t flags)
{
    if (buf == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    struct query_s query = {
        .datagroupID = 0,
        .subgroupID = 0,
        .itemID = 0,
        .offset = 0,
        .length = size,
        .flags = flags
    };

    return tee_token_handle_data_query(&query, buf, TOKEN_OP_WRITE);
}

int32_t tee_token_count_get(size_t *count, uint32_t flags)
{
    if (count == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    uint32_t buf[2] = { 0, 0 };

    struct query_s query = {
        .datagroupID = 0,
        .subgroupID = 0,
        .itemID = 0,
        .offset = 0,
        .length = sizeof(buf),
        .flags = flags
    };
    int ret = tee_token_handle_data_query(&query, (uint8_t *)buf, TOKEN_OP_COUNT);
    if (ret == 0)
    {
        *count = buf[0];
    }
    return ret;
}

int32_t tee_token_minver_get(uint32_t dg_id, uint32_t *min_ver, uint32_t flags)
{
    if (min_ver == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    uint32_t buf[2] = { 0, 0 };

    struct query_s query = {
        .datagroupID = dg_id,
        .subgroupID = 0,
        .itemID = 0,
        .offset = 0,
        .length = sizeof(buf),
        .flags = flags
    };

    int ret = tee_token_handle_data_query(&query, (uint8_t *)buf, TOKEN_OP_MINVERS);
    if (ret == 0)
    {
        *min_ver = buf[0];
    }
    return ret;
}

int32_t tee_token_lifetimedata_get(uint32_t *timestamp, uint8_t nonce[TOKEN_NONCE_LENGTH],
                                   uint8_t mac[TOKEN_MAC_LENGTH])
{
    if (timestamp == NULL || nonce == NULL || mac == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    struct
    {
        uint32_t timestamp;
        uint8_t nonce[TOKEN_NONCE_LENGTH];
        uint8_t mac[TOKEN_MAC_LENGTH];
    } buf = { 0, { 0 }, { 0 } };

    struct query_s query = {
        .datagroupID = 0,
        .subgroupID = 0,
        .itemID = 0,
        .offset = 0,
        .length = sizeof(buf),
        .flags = 0
    };

    int ret = tee_token_handle_data_query(&query, (uint8_t *)&buf, TOKEN_OP_LIFETIME);
    if (ret == 0)
    {
        *timestamp = buf.timestamp;
        memcpy(nonce, buf.nonce, TOKEN_NONCE_LENGTH);
        memcpy(mac, buf.mac, TOKEN_MAC_LENGTH);
    }
    return ret;
}


int32_t tee_token_dgids_get(uint32_t **items, size_t *count, uint32_t flags)
{
    if (items == NULL || count == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    size_t dgids;
    int ret = tee_token_count_get(&dgids, flags);
    if (ret)
        return ret;

    uint32_t *buf = malloc(dgids * sizeof(uint32_t));
    if (buf == NULL)
        return TEE_TOKEN_MEMORY_ERROR;

    struct query_s query = {
        .datagroupID = 0,
        .subgroupID = 0,
        .itemID = 0,
        .offset = 0,
        .length = dgids * sizeof(uint32_t),
        .flags = flags
    };

    ret = tee_token_handle_data_query(&query, (uint8_t *)buf, TOKEN_OP_DGIDS);
    if (ret == 0)
    {
        *count = dgids;
        *items = buf;
    }
    return ret;
}


int32_t tee_token_write_tmp(uint8_t *buf, size_t size, uint32_t flags)
{
    if (buf == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    struct query_s query = {
        .datagroupID = 0,
        .subgroupID = 0,
        .itemID = 0,
        .offset = 0,
        .length = size,
        .flags = flags
    };

    return tee_token_handle_data_query(&query, buf, TOKEN_OP_WRITE_TMP);
}

int32_t tee_token_unformatted_write(uint32_t dg_id, uint32_t sg_id, uint32_t item_id,
                                    uint8_t *buf, size_t size, uint32_t flags)
{
    if (buf == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    if (dg_id == 0 || sg_id == 0 || item_id == 0 || size == 0)
    {
        return TEE_TOKEN_INVALID_PARAM;
    }

    struct query_s query = {
        .datagroupID = dg_id,
        .subgroupID = sg_id,
        .itemID = item_id,
        .offset = 0,
        .length = size,
        .flags = flags
    };

    return tee_token_handle_data_query(&query, buf, TOKEN_OP_WRUNF);
}


int32_t tee_token_item_read(uint32_t dg_id, uint32_t sg_id, uint32_t item_id,
                            size_t offset, uint8_t *data_buf, size_t buf_size,
                            uint32_t flags)
{
    if (data_buf == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    if (dg_id == 0 || sg_id == 0 || item_id == 0 ||
        buf_size == 0 || (int)offset < 0)
    {
        return TEE_TOKEN_INVALID_PARAM;
    }

    struct query_s query = {
        .datagroupID = dg_id,
        .subgroupID = sg_id,
        .itemID = item_id,
        .offset = offset,
        .length = buf_size,
        .flags = flags
    };

    return tee_token_handle_data_query(&query, data_buf, TOKEN_OP_QUERY);
}

int32_t tee_partid_get(uint8_t partid_buf[16])
{
    if (partid_buf == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    struct query_s query = { 0, 0, 0, 0, 16, 0 };

    return tee_token_handle_data_query(&query, partid_buf, TOKEN_OP_GET_HWID);
}

int32_t tee_token_item_info_get(uint32_t dg, uint32_t sg, uint32_t item,
                                struct tee_token_item_info *info, uint32_t flags)
{
    if (info == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    if (dg == 0 || sg == 0 || item == 0)
    {
        return TEE_TOKEN_INVALID_PARAM;
    }

    struct query_s query = {
        .datagroupID = dg,
        .subgroupID = sg,
        .itemID = item,
        .offset = -sizeof(struct tee_token_item_info),
        .length = sizeof(struct tee_token_item_info),
        .flags = flags
    };

    return tee_token_handle_data_query(&query, (uint8_t *)info, TOKEN_OP_QUERY);
}

int32_t tee_token_item_size_get(uint32_t dg, uint32_t sg, uint32_t item,
                                size_t *item_size, uint32_t flags)
{
    struct tee_token_item_info info =  { 0, 0, 0, 0, 0, { 0, 0 } };
    if (item_size == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }
    int32_t ret = tee_token_item_info_get(dg, sg, item, &info, flags);
    if (ret)
    {
        return ret;
    }
    // item_size is in bytes, while header.size is in dwords
    *item_size = (info.size << 2) - info.padding - sizeof(info);
    return 0;
}

int32_t tee_token_sgroup_info_get(uint32_t dg, uint32_t sg,
                                  struct tee_token_subgroup_info *info, uint32_t flags)
{
    if (info == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    if (dg == 0 || sg == 0)
    {
        return TEE_TOKEN_INVALID_PARAM;
    }

    struct query_s query = {
        .datagroupID = dg,
        .subgroupID = sg,
        .itemID = 0,
        .offset = -sizeof(struct tee_token_subgroup_info),
        .length = sizeof(struct tee_token_subgroup_info),
        .flags = flags
    };

    return tee_token_handle_data_query(&query, (uint8_t *)info, TOKEN_OP_QUERY);
}

int32_t tee_token_itemids_get(uint32_t dg, uint32_t sg, uint32_t **items,
                              size_t *count, uint32_t flags)
{
    struct tee_token_subgroup_info info = { 0, 0, 0, { 0, 0, 0, 0 }, 0, 0 };
    int ret = tee_token_sgroup_info_get(dg, sg, &info, flags);
    if (ret)
        return ret;

    uint32_t *buf = malloc(info.itemcount * sizeof(uint32_t));
    if (buf == NULL)
        return TEE_TOKEN_MEMORY_ERROR;

    struct query_s query = {
        .datagroupID = dg,
        .subgroupID = sg,
        .itemID = 0,            // any zero marks a 'list' operation
        .offset = 0,
        .length = info.itemcount * sizeof(uint32_t),
        .flags = flags
    };

    ret = tee_token_handle_data_query(&query, (uint8_t *)buf, TOKEN_OP_QUERY);
    if (ret == 0)
    {
        *count = info.itemcount;
        *items = buf;
    }
    return ret;
}


int32_t tee_token_dgroup_info_get(uint32_t dg, struct tee_token_datagroup_info *info,
                                  uint32_t flags)
{
    if (info == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    if (dg == 0)
    {
        return TEE_TOKEN_INVALID_PARAM;
    }

    struct query_s query = {
        .datagroupID = dg,
        .subgroupID = 0,
        .itemID = 0,
        .offset = -sizeof(struct tee_token_datagroup_info),
        .length = sizeof(struct tee_token_datagroup_info),
        .flags = flags
    };

    return tee_token_handle_data_query(&query, (uint8_t *)info, TOKEN_OP_QUERY);
}

int32_t tee_token_sgids_get(uint32_t dg, uint32_t **items, size_t *count, uint32_t flags)
{
    struct tee_token_datagroup_info info = { 0, 0, 0, 0, 0, 0, 0 };
    int ret = tee_token_dgroup_info_get(dg, &info, flags);
    if (ret)
        return ret;

    uint32_t *buf = malloc(info.subgroups * sizeof(uint32_t));
    if (buf == NULL)
        return TEE_TOKEN_MEMORY_ERROR;

    struct query_s query = {
        .datagroupID = dg,
        .subgroupID = 0,
        .itemID = 0,
        .offset = 0,
        .length = info.subgroups * sizeof(uint32_t),
        .flags = flags
    };

    ret = tee_token_handle_data_query(&query, (uint8_t *)buf, TOKEN_OP_QUERY);
    if (ret == 0)
    {
        *count = info.subgroups;
        *items = buf;
    }
    return ret;
}

int32_t tee_token_info_get(uint32_t dg, struct tee_token_info *info, uint32_t flags)
{
    if (info == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    if (dg == 0)
    {
        return TEE_TOKEN_INVALID_PARAM;
    }

    struct query_s query = {
        .datagroupID = dg,
        .subgroupID = 0,
        .itemID = 0,
        .offset = -sizeof(struct tee_token_datagroup_info) -
                  sizeof(struct tee_token_info),
        .length = sizeof(struct tee_token_info),
        .flags = flags
    };

    return tee_token_handle_data_query(&query, (uint8_t *)info, TOKEN_OP_QUERY);
}

int32_t tee_token_read(uint32_t dg, uint8_t *data_buf,
                       uint32_t data_size, uint32_t flags)
{
    if (data_buf == NULL)
    {
        return TEE_TOKEN_NULL_PTR;
    }

    if (dg == 0)
    {
        return TEE_TOKEN_INVALID_PARAM;
    }

    struct query_s query = {
        .datagroupID = dg,
        .subgroupID = 0,
        .itemID = 0,
        .offset = -sizeof(struct tee_token_datagroup_info) -
                  sizeof(struct tee_token_info),
        .length = data_size,
        .flags = flags
    };

    return tee_token_handle_data_query(&query, (uint8_t *)data_buf, TOKEN_OP_QUERY);
}

// Ctx will be overwritten
int32_t tee_token_unprotected_start(struct tee_token_ctx *ctx, uint32_t dg_id, uint32_t flags)
{
    if (ctx == NULL)
        return TEE_TOKEN_NULL_PTR;

    if (dg_id != TEE_TOKEN_WVDRM_TOKEN_DG)
        return TEE_TOKEN_INVALID_PARAM;

    uint32_t size = TEE_TOKEN_VRL_SIZE +
                    sizeof(struct tee_token_info) +
                    sizeof(struct tee_token_datagroup_info);
    uint32_t alloc_size = size + TEE_TOKEN_MAX_PAYLOAD_SIZE_UNPROTECTED;

    ctx->base_address = calloc(alloc_size, 1);
    ctx->current_length = size;
    ctx->allocated_length = alloc_size;

    if (ctx->base_address == NULL)
        return TEE_TOKEN_MEMORY_ERROR;

    struct tee_token_info *tkn_info = (void *)((uint8_t *)ctx->base_address + TEE_TOKEN_VRL_SIZE);
    struct tee_token_datagroup_info *dg_info = (void *)((uint8_t *)tkn_info +
                                               sizeof(struct tee_token_info));

    dg_info->DATA = 0x41544144; // "DATA"
    dg_info->id = dg_id;
    dg_info->flags = flags;
    dg_info->size = sizeof(struct tee_token_datagroup_info) / sizeof(uint32_t);
    tkn_info->lifetime.token_size = size / sizeof(uint32_t);
    return TEEC_SUCCESS;
}

int32_t tee_token_unprotected_end(struct tee_token_ctx *ctx, uint32_t flags)
{
    if (ctx == NULL || ctx->base_address == NULL)
        return TEE_TOKEN_NULL_PTR;

    int32_t ret = tee_token_write((uint8_t *)ctx->base_address, ctx->current_length, flags);
    free(ctx->base_address);
    ctx->base_address = NULL;
    return ret;
}

// Auxiliary function to cut a block of memory from token ctx
static int delete(void *begin, uint32_t size, struct tee_token_ctx *ctx)
{
    void *next_block = (uint8_t *)begin + size;
    void *end_of_data = (uint8_t *)ctx->base_address + ctx->current_length;
    uint32_t move_len = (uint32_t)end_of_data - (uint32_t)next_block;
    if (ctx->current_length < size)
        return TEE_TOKEN_MEMORY_ERROR;
    memmove(begin, (const void *)next_block, move_len);
    ctx->current_length -= size;
    return 0;
}

// Auxiliary function to insert a block of memory in middle of token ctx
static int insert(void *begin, uint32_t size, struct tee_token_ctx *ctx)
{
    void *next_block = (uint8_t *)begin + size;
    void *end_of_data = (uint8_t *)ctx->base_address + ctx->current_length;
    uint32_t move_len = (uint32_t)end_of_data - (uint32_t)begin;
    if (ctx->current_length + size > ctx->allocated_length)
        return TEE_TOKEN_MEMORY_ERROR;
    memmove(next_block, begin, move_len);
    ctx->current_length += size;
    return 0;
}

// Unprotected write creates a valid structured token by inserting headers
// and payload to token context. Update to existing item_id is possible
int32_t tee_token_unprotected_write(struct tee_token_ctx *ctx, uint32_t sg_id, uint32_t item_id,
                                    const uint8_t *buf, size_t size, uint32_t flags)
{
    if (ctx == NULL || ctx->base_address == NULL || buf == NULL)
        return TEE_TOKEN_NULL_PTR;
    if (size == 0)
        return TEE_TOKEN_INVALID_PARAM;

    struct tee_token_info *tkn_info = (struct tee_token_info *)
                                      ((uint8_t *)ctx->base_address + TEE_TOKEN_VRL_SIZE);
    struct tee_token_datagroup_info *dg_info = (struct tee_token_datagroup_info *)
                                             ((uint8_t *)tkn_info + sizeof(struct tee_token_info));
    struct tee_token_subgroup_info *sg_info = (struct tee_token_subgroup_info *)((uint8_t *)dg_info
                                               + sizeof(struct tee_token_datagroup_info));
    uint32_t i;
    // Find a subgroup or leave sg_info to point to the place where a new subgroup will be inserted
    for (i = 0; i < dg_info->subgroups; i++)
    {
        if (sg_info->id == sg_id)
            break;
        sg_info = (struct tee_token_subgroup_info *)
                  ((uint8_t *)sg_info + sg_info->size * sizeof(uint32_t));
    }

    if (i == dg_info->subgroups)
    {
        struct tee_token_subgroup_info new_sg = {
            .SUBG = 0x47425553, // "SUBG"
            .id = sg_id,
            .size = sizeof(struct tee_token_subgroup_info) / sizeof(uint32_t)
        };
        // make space for the new sg header (and copy it)
        if (insert(sg_info, sizeof(new_sg), ctx))
            return TEE_TOKEN_MEMORY_ERROR;
        memcpy(sg_info, (void *)&new_sg, sizeof(new_sg));
        dg_info->subgroups++;
        dg_info->size += sizeof(struct tee_token_subgroup_info) / sizeof(uint32_t);
        tkn_info->lifetime.token_size += sizeof(struct tee_token_subgroup_info) / sizeof(uint32_t);
    }

    struct tee_token_item_info *item = (struct tee_token_item_info *)
                                     ((uint8_t *)sg_info + sizeof(struct tee_token_subgroup_info));

    for (i = 0; i < sg_info->itemcount; i++)
    {
        if (item->id == item_id)
        {
            uint32_t itemsize = item->size * sizeof(uint32_t);
            delete(item, itemsize, ctx);
            sg_info->itemcount--;
            sg_info->size -= itemsize / sizeof(uint32_t);
            dg_info->size -= itemsize / sizeof(uint32_t);
            tkn_info->lifetime.token_size -= itemsize / sizeof(uint32_t);
            break;
        }
        // Skip to next (possible) location of item
        item = (struct tee_token_item_info *)((uint8_t *)item + item->size * sizeof(uint32_t));
    }

    uint32_t padding = -size & 3;
    uint32_t isize = sizeof(struct tee_token_item_info) + size + padding;

    struct tee_token_item_info new_item = {
        .ITEM = 0x4d455449,
        .id = item_id,
        .size = isize / sizeof(uint32_t),
        .padding = padding
    };
    if (insert(item, isize, ctx))
        return TEE_TOKEN_MEMORY_ERROR;

    memcpy(item, &new_item, sizeof(new_item));
    memcpy((uint8_t *)item + sizeof(new_item), buf, size);
    memset((uint8_t *)item + sizeof(new_item) + size, 0, padding);
    sg_info->itemcount++;
    sg_info->size += isize / sizeof(uint32_t);
    dg_info->size += isize / sizeof(uint32_t);
    tkn_info->lifetime.token_size += isize / sizeof(uint32_t);
    return TEEC_SUCCESS;
}

