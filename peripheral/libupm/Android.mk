# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Still keep Wswitch and other warnings...
LOCAL_CPPFLAGS:= -std=c++11 -Wno-unused-parameter -Wno-error=non-virtual-dtor -fexceptions -I$(LOCAL_PATH)/src/nrf8001/
LOCAL_CPP_EXTENSION := .cxx
LOCAL_SHARED_LIBRARIES := libcutils libmraa

# NOTE: Build system cannot handle c++ files in different extension in a single
# module. We compile all cxx file (the majority) this time and don't compile
# cpp files to avoid renaming files until there is a need of these cpp files
# shows up.

LIBUPM_CXX_FILE_LIST := $(call all-named-files-under,*.cxx,src)

# groveloudness module requires a header file missed in imported tip of libupm
# filter it out...
LIBUPM_CXX_FILE_LIST := $(filter-out %groveloudness.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %hwxpxx.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %zwNode.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %ozw.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %t3311.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %sx1276.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %ssd1351.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %st7735.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %ili9341.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %gfx.cxx, $(LIBUPM_CXX_FILE_LIST))

# drivers dependent on the BACnet stack
LIBUPM_CXX_FILE_LIST := $(filter-out %bacnetmstp.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %e50hx.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %bacnetutil.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %tb7300.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %t8100.cxx, $(LIBUPM_CXX_FILE_LIST))

# drivers dependent on the modbus library
LIBUPM_CXX_FILE_LIST := $(filter-out %h803x.cxx, $(LIBUPM_CXX_FILE_LIST))

# drivers dependent on the jpeg library
LIBUPM_CXX_FILE_LIST := $(filter-out %vcap.cxx, $(LIBUPM_CXX_FILE_LIST))

# keep a single implementation of MraaUtils class
LIBUPM_CXX_FILE_LIST := $(filter-out %ds1808lc/mraa-utils.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %si7005/mraa-utils.cxx, $(LIBUPM_CXX_FILE_LIST))
LIBUPM_CXX_FILE_LIST := $(filter-out %hlg150h/mraa-utils.cxx, $(LIBUPM_CXX_FILE_LIST))

LOCAL_SRC_FILES := $(LIBUPM_CXX_FILE_LIST)
LOCAL_SRC_FILES += src/bmi160/bosch_bmi160.c

LOCAL_C_INCLUDES := $(sort $(dir $(wildcard $(LOCAL_PATH)/src/*/)))
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_C_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libupm

include $(BUILD_SHARED_LIBRARY)
