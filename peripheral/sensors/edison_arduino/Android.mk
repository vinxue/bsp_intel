# Copyright (C) 2015 Intel Corporation
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

LOCAL_CPPFLAGS:= -Wno-unused-parameter -Wno-error=non-virtual-dtor -fexceptions
LOCAL_CFLAGS += -DLOG_TAG=\"Sensors\" -Wno-unused-parameter
LOCAL_SHARED_LIBRARIES := libcutils libupm libmraa
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../libupm/src/mpu9150/
LOCAL_SRC_FILES := SensorsHAL.cpp Sensor.cpp AcquisitionThread.cpp Utils.cpp \
    SensorFactory.cpp \
    SensorDescriptionFactory.cpp \
    sensors/MPU9150Accelerometer.cpp \
    sensors/MMA7660Accelerometer.cpp \
    sensors/GroveLight.cpp
LOCAL_MODULE := sensors.edison
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
