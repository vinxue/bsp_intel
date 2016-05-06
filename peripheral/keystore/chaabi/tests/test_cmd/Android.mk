LOCAL_PATH:= $(call my-dir)

ifeq ($(INTEL_CHAABI_KEYSTORE_SOURCE_BUILD),true)

include $(CLEAR_VARS)
LOCAL_MODULE := test_send_cmd
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := test_send.c

LOCAL_STATIC_LIBRARIES := libc libdx_cc7_static

LOCAL_FORCE_STATIC_EXECUTABLE := true
include $(BUILD_EXECUTABLE)

endif
