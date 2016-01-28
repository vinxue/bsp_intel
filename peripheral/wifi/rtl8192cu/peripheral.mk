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

# RTL8192CU WIFI Firmware
RTL8192CU_WIFI_FW_SRC := vendor/bsp/intel/peripheral/wifi/rtlwifi_firmware
RTL8192CU_WIFI_FW_DST := system/vendor/firmware/rtlwifi

PRODUCT_COPY_FILES += \
	$(RTL8192CU_WIFI_FW_SRC)/rtl8192cufw.bin:$(RTL8192CU_WIFI_FW_DST)/rtl8192cufw.bin     \
	$(RTL8192CU_WIFI_FW_SRC)/rtl8192cufw_A.bin:$(RTL8192CU_WIFI_FW_DST)/rtl8192cufw_A.bin \
	$(RTL8192CU_WIFI_FW_SRC)/rtl8192cufw_B.bin:$(RTL8192CU_WIFI_FW_DST)/rtl8192cufw_B.bin \
	$(RTL8192CU_WIFI_FW_SRC)/rtl8192cufw_TMSC.bin:$(RTL8192CU_WIFI_FW_DST)/rtl8192cufw_TMSC.bin

WIFI_DRIVER_HAL_MODULE := wifi_driver.$(TARGET_DEVICE)
WIFI_DRIVER_HAL_PERIPHERAL := rtl8192cu
