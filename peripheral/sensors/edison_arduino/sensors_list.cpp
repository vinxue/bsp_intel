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


#define LOG_TAG "sensors_list_edison"

#include <cutils/log.h>
#include "sensors_list.h"
#include <stdlib.h>
#include <hardware/sensors.h>

SensorBase::SensorBase() {}
SensorBase::~SensorBase() {}

int SensorBase::activate(int handle, int enabled) {
  return 0;
}

int SensorBase::setDelay(int handle, int64_t ns) {
  return 0;
}

int SensorBase::batch(int handle, int flags,
                      int64_t period_ns, int64_t timeout) {
  return 0;
}

int SensorBase::flush(int handle) {
  return 0;
}

// MPU9150/9250 accelerometer sensor implementation
MPU9150Accelerometer::MPU9150Accelerometer(int bus, int address, int magAddress, bool enableAk8975)
  : MPU9150 (bus, address, magAddress, enableAk8975){}

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
    return 0;
  } else if (enabled == 0) {
    // Do nothing. We will hold resource until close()
    return 0;
  }

  return -1;
}

MMA7660Accelerometer::MMA7660Accelerometer(int bus, uint8_t address) : MMA7660 (bus, address) {}

MMA7660Accelerometer::~MMA7660Accelerometer() {}

int MMA7660Accelerometer::pollEvents(sensors_event_t* data, int count) {
  getAcceleration(&data->acceleration.x, &data->acceleration.y, &data->acceleration.z);
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

    return 0;
  }  else if (enabled == 0) {
    // Do nothing. We will hold resource until close()
    // Maybe we want to put sensor to standby mode...
    return 0;
  }

  return -1;
}
// NEW sensors: implementation here
