LOCAL_PATH:= $(call my-dir)

ifeq ($(INTEL_CHAABI_KEYSTORE_SOURCE_BUILD),true)

include $(CLEAR_VARS)
LOCAL_MODULE := intel_keymaster_test
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := intel_keymaster_test.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src

LOCAL_CFLAGS = -Wall -Werror
LOCAL_SHARED_LIBRARIES := libcutils libkeymaster

include $(BUILD_EXECUTABLE)

endif
