# (c) Copyright 2011-2016 Intel Corporation.
# This file is licensed under the terms provided in the file
# libcc54/LICENSE in this directory or a parent directory

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

local_src_files :=                          \
                dx_cclib.c                              \
                driver_interface/driver_interface.c     \
                crys/crys_context_relocation.c          \
                crys/crys_aes.c                         \
                crys/crys_hash.c                        \
                crys/crys_hmac.c                        \
                crys/crys_des.c                         \
                crys/crys_common.c                      \
                crys/crys_aesccm.c                      \
                crys/crys_combined.c                    \
                tee_client_api.c                        \
                tee_token_lib.c                         \
                tee_applet_loading_lib.c                \
                generated/applet_mng_seprpc_stub.c      \
                generated/crys_aes_seprpc_stub.c        \
                generated/crys_ecc_seprpc_stub.c        \
                generated/crys_rc4_seprpc_stub.c        \
                generated/crys_rnd_seprpc_stub.c        \
                generated/crys_rsa_seprpc_stub.c        \
                generated/management_apis_seprpc_stub.c \
                generated/crys_kdf_seprpc_stub.c        \
                generated/crys_dh_seprpc_stub.c         \
                generated/crys_self_test_seprpc_stub.c  \
                pal/linux/dx_pal_log.c                  \
                disk_enc.c                              \
                secure_channel/secure_channel.c

ifeq ($(INTEL_FEATURE_ARKHAM),true)
local_src_files += hwk_crypto.c
endif

local_c_includes := \
                $(LOCAL_PATH)/driver_interface      \
                $(LOCAL_PATH)/include               \
                $(LOCAL_PATH)/include/export        \
                $(LOCAL_PATH)/include/crys          \
                $(LOCAL_PATH)/include/generated     \
                $(LOCAL_PATH)/include/pal           \
                $(LOCAL_PATH)/include/pal/linux

local_export_c_include_dirs := $(LOCAL_PATH)/include/export

local_c_flags := -DDX_CC_HOST -DDX_CC54_SUPPORTED


#################################################
# Target dynamic library

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(local_src_files)
LOCAL_C_INCLUDES += $(local_c_includes)
LOCAL_CFLAGS += $(local_c_flags)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(local_export_c_include_dirs)
LOCAL_SHARED_LIBRARIES += libc
LOCAL_MODULE := libdx_cc7
LOCAL_MODULE_TAGS := optional
ifeq ($(INTEL_FEATURE_ARKHAM),true)
LOCAL_COPY_HEADERS += include/export/hwk_crypto.h
endif
LOCAL_COPY_HEADERS_TO := libdx_cc7
LOCAL_COPY_HEADERS += \
               include/export/dx_cclib.h \
               include/export/secure_channel.h \
               include/export/tee_client_api.h \
               include/export/tee_token_error.h \
               include/export/tee_token_if.h
include $(BUILD_SHARED_LIBRARY)

###############################################
# Target static library

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(local_src_files)
LOCAL_C_INCLUDES += $(local_c_includes)
LOCAL_CFLAGS += $(local_c_flags)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(local_export_c_include_dirs)
LOCAL_STATIC_LIBRARIES += libc
LOCAL_MODULE := libdx_cc7_static
LOCAL_MODULE_TAGS := optional
ifeq ($(INTEL_FEATURE_ARKHAM),true)
LOCAL_COPY_HEADERS += include/export/hwk_crypto.h
endif
LOCAL_COPY_HEADERS_TO := libdx_cc7_static
LOCAL_COPY_HEADERS += \
               include/export/dx_cclib.h \
               include/export/secure_channel.h \
               include/export/tee_client_api.h \
               include/export/tee_token_error.h \
               include/export/tee_token_if.h
include $(BUILD_STATIC_LIBRARY)
