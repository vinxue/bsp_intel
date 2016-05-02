/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/

#ifndef DX_PAL_TYPES_PLAT_H
#define DX_PAL_TYPES_PLAT_H
/* Host specific types for standard (ISO-C99) compilant platforms */

#include <stdint.h>
#include <stddef.h>

#define DX_NULL NULL

typedef unsigned int            DxUint_t;
typedef uint8_t                 DxUint8_t;
typedef uint16_t                DxUint16_t;
typedef uint32_t                DxUint32_t;
typedef uint64_t                DxUint64_t;

typedef int                   	DxInt_t;
typedef int8_t                  DxInt8_t;
typedef int16_t                 DxInt16_t;
typedef int32_t                 DxInt32_t;
typedef int64_t	                DxInt64_t;

typedef char                    DxChar_t;
typedef short                   DxWideChar_t;

/* Define macros for host to SeP endianess conversion (for host wrappers) */
#include <endian.h>
#include <byteswap.h>
#if __BYTE_ORDER == __BIG_ENDIAN
#define cpu_to_le16(x) bswap_16(x)
#define le16_to_cpu(x) bswap_16(x)
#define cpu_to_le32(x) bswap_32(x)
#define le32_to_cpu(x) bswap_32(x)
#else /*__LITTLE_ENDIAN*/
#define cpu_to_le16(x) x
#define le16_to_cpu(x) x
#define cpu_to_le32(x) x
#define le32_to_cpu(x) x
#endif /*__BYTE_ORDER*/

#endif /*DX_PAL_TYPES_PLAT_H*/
