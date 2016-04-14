LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libkeymaster
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := src/sep_keymaster.c
LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/include \
  external/openssl/include
LOCAL_CFLAGS += -DDX_CC_HOST -DDX_CC54_SUPPORTED
LOCAL_SHARED_LIBRARIES := libc libdx_cc7 libcrypto libcutils

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_C_INCLUDES)
LOCAL_COPY_HEADERS_TO := libsepkeymaster
LOCAL_COPY_HEADERS += include/sep_keymaster.h

include $(BUILD_SHARED_LIBRARY)
