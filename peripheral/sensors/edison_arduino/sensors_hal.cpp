/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "sensors_hal_edison"

#include <cutils/log.h>

#include "sensors_hal.h"

#include <errno.h>
#include <string.h>

SensorContext::SensorContext(const hw_module_t* module) {
  memset(&device, 0, sizeof(device));

  device.common.tag = HARDWARE_DEVICE_TAG;
  device.common.version = SENSORS_DEVICE_API_VERSION_1_0;
  device.common.module = const_cast<hw_module_t*>(module);
  device.common.close = CloseWrapper;
  device.activate = ActivateWrapper;
  device.setDelay = SetDelayWrapper;
  device.poll = PollEventsWrapper;
  device.batch = BatchWrapper;
  device.flush = FlushWrapper;

  for (int i = 0; i < AvailableSensors::kNumSensors; i++) {
    activatedMap[i] = false;
  }

  // NOTE: INSTANCES MUST FOLLOW THE ORDER IN AvailableSensors
  try {
    sensors[AvailableSensors::kMPU9150Accelerometer] = new MPU9150Accelerometer(
        6, MPU9150_DEFAULT_I2C_ADDR, AK8975_DEFAULT_I2C_ADDR, false);
  } catch (const std::runtime_error& e) {
    ALOGE("%s: Failed to instantiate MPU9150/9250 sensor. keep running",
        __func__);
  }

  try {
    sensors[AvailableSensors::kMMA7660Accelerometer] = new MMA7660Accelerometer(
        6, MMA7660_DEFAULT_I2C_ADDR);
  } catch (const std::runtime_error& e) {
    ALOGE("%s: Failed to instantiate MPU9150/9250 sensor. keep running",
        __func__);
  }
  // NEW sensors: instantiate object here
}

SensorContext::~SensorContext() {
  for (int i = 0; i < AvailableSensors::kNumSensors; i++) {
    if (sensors[i]) delete sensors[i];
  }
}

int SensorContext::activate(int handle, int enabled) {
  int ret = 0;

  if (enabled != 0 && enabled != 1) {
    ALOGE("%s: Invaled parameter", __func__);
    return -EINVAL;
  }

  try {
    ret = sensors[handle]->activate(handle, enabled);
    if (ret == 0) {
      activatedMap[handle] = enabled == 0 ? false : true;
    }
    return ret;
  } catch (const std::runtime_error& e) {
    ALOGE("%s: Failed to activate sensor(s). keep running", __func__);
    return -1;
  }
}

int SensorContext::setDelay(int handle, int64_t ns) {
  return sensors[handle]->setDelay(handle, ns);
}

int SensorContext::pollEvents(sensors_event_t* data, int count) {
  int evt = 0;
  int j = 0;
  // Assumption: each sensor supports polling and always returns some data;
  // Read one event out of each sensor if buffer is big enough.
  // If there is an error happens, stop reading (refer to assumption)
  for (int i = 0; i < AvailableSensors::kNumSensors; i++) {

    if (activatedMap[i] == false) {
      continue;
    }
    try {
      evt = sensors[i]->pollEvents(&data[j], 1);
    } catch (const std::runtime_error& e) {
      ALOGE("%s: Failed to pull sensor, stop polling", __func__);
      break;
    }
    if (evt == 1) {
      // set handle
      data[j].sensor = i;
      j++;
    } else
      // We asked only one event, anything else is wrong.
      break;

    if (j >= count)
      break;
  }

  /*
   * According to the documentation, the poll function should never return a zero.
   * If we, however, return -EIO or -1 at this point, badness ensues.
   */
  return j;
}

int SensorContext::batch(int handle, int flags,
                         int64_t period_ns, int64_t timeout) {
  return sensors[handle]->batch(handle, flags, period_ns, timeout);
}

int SensorContext::flush(int handle) {
  return sensors[handle]->flush(handle);
}

int SensorContext::CloseWrapper(hw_device_t* dev) {
  SensorContext* sensor_context = reinterpret_cast<SensorContext*>(dev);
  if (sensor_context) {
    delete sensor_context;
  }
  return 0;
}

int SensorContext::ActivateWrapper(sensors_poll_device_t* dev,
                                   int handle, int enabled) {
  return reinterpret_cast<SensorContext*>(dev)->activate(handle, enabled);
}

int SensorContext::SetDelayWrapper(sensors_poll_device_t* dev,
                                   int handle, int64_t ns) {
  return reinterpret_cast<SensorContext*>(dev)->setDelay(handle, ns);
}

int SensorContext::PollEventsWrapper(sensors_poll_device_t* dev,
                                     sensors_event_t* data, int count) {
  return reinterpret_cast<SensorContext*>(dev)->pollEvents(data, count);
}

int SensorContext::BatchWrapper(sensors_poll_device_1_t* dev, int handle,
                                int flags, int64_t period_ns, int64_t timeout) {
  return reinterpret_cast<SensorContext*>(dev)->batch(handle, flags, period_ns,
                                                      timeout);
}

int SensorContext::FlushWrapper(sensors_poll_device_1_t* dev,
                                int handle) {
  return reinterpret_cast<SensorContext*>(dev)->flush(handle);
}

static int open_sensors(const struct hw_module_t* module,
                        const char* id, struct hw_device_t** device) {
  SensorContext* ctx = new SensorContext(module);
  *device = &ctx->device.common;

  return 0;
}

static struct hw_module_methods_t sensors_module_methods = {
  open: open_sensors,
};

static struct sensor_t kSensorList[] = {
  { name: "MPU9150/9250 Accelerometer",
    vendor: "Unknown",
    version: 1,
    handle: SensorContext::AvailableSensors::kMPU9150Accelerometer,
    type: SENSOR_TYPE_ACCELEROMETER,
    maxRange: static_cast<float>(MPU9150Accelerometer::kMaxRange),
    resolution: 100.0f,
    power: 0.0f,
    minDelay: 10000,
    fifoReservedEventCount: 0,
    fifoMaxEventCount: 0,
    SENSOR_STRING_TYPE_ACCELEROMETER,
    requiredPermission: "",
    maxDelay: 0,
    flags: SENSOR_FLAG_CONTINUOUS_MODE,
    reserved: {},
  },

  { name: "MMA7660 Accelerometer",
    vendor: "Unknown",
    version: 1,
    handle: SensorContext::AvailableSensors::kMMA7660Accelerometer,
    type: SENSOR_TYPE_ACCELEROMETER,
    maxRange: static_cast<float>(MMA7660Accelerometer::kMaxRange),
    resolution: 100.0f,
    power: 0.0f,
    minDelay: 10000,
    fifoReservedEventCount: 0,
    fifoMaxEventCount: 0,
    SENSOR_STRING_TYPE_ACCELEROMETER,
    requiredPermission: "",
    maxDelay: 0,
    flags: SENSOR_FLAG_CONTINUOUS_MODE,
    reserved: {},
  },
  // NEW sensors: put info here
};

static int get_sensors_list(struct sensors_module_t* module,
                            struct sensor_t const** list) {
  if (!list) return 0;
  *list = kSensorList;
  return sizeof(kSensorList) / sizeof(kSensorList[0]);
}

struct sensors_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: SENSORS_HARDWARE_MODULE_ID,
        name: "Edison Sensor HAL",
        author: "Intel",
        methods: &sensors_module_methods,
        dso: NULL,
        reserved: {0},
    },
    get_sensors_list: get_sensors_list,
    set_operation_mode: NULL
};
