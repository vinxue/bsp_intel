#
# Copyright 2015 The Android Open Source Project
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

LOCAL_PATH := hardware/bsp/intel/soc/edison

# x86 device.
TARGET_ARCH := x86
TARGET_ARCH_VARIANT := silvermont
TARGET_CPU_VARIANT :=
TARGET_CPU_ABI :=  x86
TARGET_CPU_ABI2 :=
TARGET_KERNEL_ARCH := i386

# Disable building kernel and bootloader until we really support them.
TARGET_NO_BOOTLOADER := false
TARGET_NO_KERNEL := false

# FIXME: Need to dynamically grab the serial number of the device from the hardware.
BOARD_KERNEL_CMDLINE := console=ttyMFD2 earlyprintk=ttyMFD2,keep androidboot.hardware=edison

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_FLASH_BLOCK_SIZE := 131072

PRODUCT_COPY_FILES += \
  $(LOCAL_PATH)/init.edison.rc:root/init.edison.rc \
  system/core/rootdir/init.usb.rc:root/init.usb.rc \
  system/core/rootdir/ueventd.rc:root/ueventd.rc \
  $(LOCAL_PATH)/ueventd.edison.rc:root/ueventd.edison.rc \

BOARD_SEPOLICY_DIRS += \
	$(LOCAL_PATH)/sepolicy \

TARGET_KERNEL_SRC := hardware/bsp/kernel/intel/edison-3.10
TARGET_KERNEL_DEFCONFIG := i386_brillo_edison_defconfig
TARGET_KERNEL_CONFIGS := $(realpath $(LOCAL_PATH)/soc.kconf)

# Bt product packages
DEVICE_PACKAGES += \
    bt_bcm

DEVICE_PACKAGES += \
    keystore.default

# Audio HAL
DEVICE_PACKAGES += \
    audio.usb.edison

# Audio HAL support
DEVICE_PACKAGES += \
    libstagefright_soft_mp3dec

# Boot control HAL package
DEVICE_PACKAGES += \
    bootctrl.edison

# lights HAL package
DEVICE_PACKAGES += \
    lights.edison

# Audio utilities. You may not need these for a product
DEVICE_PACKAGES += \
    tinyplay tinypcminfo tinymix tinycap

# Device partition table
DEVICE_PACKAGES += \
    gpt.bin

# Sensor HAL package
DEVICE_PACKAGES += \
    sensors.edison
