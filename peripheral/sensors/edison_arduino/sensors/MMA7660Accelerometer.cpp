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
#include "MMA7660Accelerometer.hpp"

struct sensor_t const MMA7660Accelerometer::kSensorDescription = {
  name: "MMA7660 Accelerometer",
  vendor: "Unknown",
  version: 1,
  handle: Sensor::Type::kMMA7660Accelerometer,
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

MMA7660Accelerometer::MMA7660Accelerometer(int bus, uint8_t address) : MMA7660 (bus, address) {
  handle = Sensor::Type::kMMA7660Accelerometer;
  type = SENSOR_TYPE_ACCELEROMETER;
}

MMA7660Accelerometer::~MMA7660Accelerometer() {}

int MMA7660Accelerometer::pollEvents(sensors_event_t* data, int count) {
  getAcceleration(&data->acceleration.x, &data->acceleration.y, &data->acceleration.z);
  data->acceleration.x *= Sensor::kGravitationalAcceleration;
  data->acceleration.y *= Sensor::kGravitationalAcceleration;
  data->acceleration.z *= Sensor::kGravitationalAcceleration;
  return 1;
}

// MMA7660 accelerometer sensor implementation
int MMA7660Accelerometer::activate(int handle, int enabled) {
  setModeStandby();
  if (enabled == 1) {
    if (!setSampleRate(upm::MMA7660::AUTOSLEEP_64)) {
      ALOGE("%s: Failed to set sensor SampleRate", __func__);
      return -1;
    }

    setModeActive();

    /* start the acquisition thread */
    return activateAcquisitionThread(handle, enabled);
  }  else if (enabled == 0) {
    /* stop the acquisition thread */
    return activateAcquisitionThread(handle, enabled);
  }

  return -1;
}