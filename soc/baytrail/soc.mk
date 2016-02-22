#
# Copyright 2016 The Android Open Source Project
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
#

LOCAL_PATH := hardware/bsp/intel/soc/baytrail

# x86 device.
TARGET_ARCH := x86
TARGET_ARCH_VARIANT := silvermont
TARGET_CPU_VARIANT :=
TARGET_CPU_ABI :=  x86
TARGET_CPU_ABI2 :=
TARGET_CPU_ABI_LIST_32_BIT := x86
TARGET_CPU_SMP := true
TARGET_KERNEL_ARCH := x86_64
TARGET_USES_64_BIT_BINDER := true

# Disable building kernel and bootloader until we really support them.
TARGET_NO_BOOTLOADER := false
TARGET_NO_KERNEL := false

BOARD_KERNEL_CMDLINE := androidboot.hardware=$(TARGET_DEVICE)
BOARD_KERNEL_CMDLINE += firmware_class.path=/system/vendor/firmware

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_FLASH_BLOCK_SIZE := 131072

PRODUCT_COPY_FILES += \
  $(LOCAL_PATH)/init.baytrail.rc:root/init.$(TARGET_DEVICE).rc \
  system/core/rootdir/ueventd.rc:root/ueventd.rc \
  $(LOCAL_PATH)/ueventd.baytrail.rc:root/ueventd.$(TARGET_DEVICE).rc \
  $(LOCAL_PATH)/watchdog.rc:$(TARGET_COPY_OUT_INITRCD)/watchdog.rc

USERFASTBOOT_EXTRAS := \
  $(LOCAL_PATH)/dhcpcd_eth0.rc \
  $(LOCAL_PATH)/watchdog.rc \
  $(LOCAL_PATH)/init.userfastboot.rc

TARGET_KERNEL_SRC := hardware/bsp/kernel/intel/minnowboard-3.14
TARGET_KERNEL_DEFCONFIG := x86_64_brillo_$(TARGET_DEVICE)_defconfig
TARGET_KERNEL_CONFIGS := $(realpath $(LOCAL_PATH)/soc.kconf)

TARGET_BOOTCTRL_HAL := intel

DEVICE_PACKAGES += \
    keystore.default

# Boot control HAL package
DEVICE_PACKAGES += \
    bootctrl.$(TARGET_DEVICE)
