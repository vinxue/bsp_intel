/*
 * Copyright (C) 2015 Intel Corporation
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

#include <cutils/log.h>
#include "MPU9150Accelerometer.hpp"

struct sensor_t const MPU9150Accelerometer::kSensorDescription = {
  name: "MPU9150/9250 Accelerometer",
  vendor: "Unknown",
  version: 1,
  handle: Sensor::Type::kMPU9150Accelerometer,
  type: SENSOR_TYPE_ACCELEROMETER,
  maxRange: 1000.0f,
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
};

MPU9150Accelerometer::MPU9150Accelerometer(int bus, int address, int magAddress, bool enableAk8975)
  : MPU9150 (bus, address, magAddress, enableAk8975) {
  handle = Sensor::Type::kMPU9150Accelerometer;
  type = SENSOR_TYPE_ACCELEROMETER;
}

MPU9150Accelerometer::~MPU9150Accelerometer() {}

int MPU9150Accelerometer::pollEvents(sensors_event_t* data, int count) {
  update();
  getAccelerometer(&data->acceleration.x, &data->acceleration.y, &data->acceleration.z);
  return 1;
}

int MPU9150Accelerometer::activate(int handle, int enabled) {
  if (enabled == 1) {
    if (init() != true) {
      ALOGE("%s: Failed to initialize sensor error", __func__);
      return -1;
    }

    /* start the acquisition thread */
    return activateAcquisitionThread(handle, enabled);
  } else if (enabled == 0) {
    /* stop the acquisition thread */
    return activateAcquisitionThread(handle, enabled);
  }

  return -1;
}
