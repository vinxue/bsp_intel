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
#include "H3LIS331DLAccelerometer.hpp"
#include "SensorsHAL.hpp"
#include "SensorUtils.hpp"

struct sensor_t H3LIS331DLAccelerometer::sensorDescription = {
  .name = "H3LIS331DL Accelerometer",
  .vendor = "ST Microelectronics",
  .version = 1,
  .handle = -1,
  .type = SENSOR_TYPE_ACCELEROMETER,
  /* maxRange = 1.5g */
  .maxRange = 29.43f,
  .resolution = 0.46714286f,
  .power = 0.0198f,
  .minDelay = 10,
  .fifoReservedEventCount = 0,
  .fifoMaxEventCount = 0,
  .stringType = SENSOR_STRING_TYPE_ACCELEROMETER,
  .requiredPermission = "",
  .maxDelay = 1000,
  .flags = SENSOR_FLAG_CONTINUOUS_MODE,
  .reserved = {},
};

Sensor * H3LIS331DLAccelerometer::createSensor(int pollFd) {
  return new H3LIS331DLAccelerometer(pollFd, SensorUtils::getI2cBusNumber(),
                                     H3LIS331DL_DEFAULT_I2C_ADDR);
}

void H3LIS331DLAccelerometer::initModule() {
  SensorContext::addSensorModule(&sensorDescription, createSensor);
}

H3LIS331DLAccelerometer::H3LIS331DLAccelerometer(int pollFd, int bus, int address)
    : H3LIS331DL(bus, address), pollFd(pollFd) {
  handle = sensorDescription.handle;
  type = SENSOR_TYPE_ACCELEROMETER;
}

H3LIS331DLAccelerometer::~H3LIS331DLAccelerometer() {}

int H3LIS331DLAccelerometer::pollEvents(sensors_event_t* data, int count) {
  update();
  getAcceleration(&data->acceleration.x, &data->acceleration.y, &data->acceleration.z);
  return 1;
}

int H3LIS331DLAccelerometer::activate(int handle, int enabled) {
  if (enabled) {
    if(!init(DR_100_74, PM_NORMAL, FS_100)) {
      return -1;
    }
  }
  else {
    setPowerMode(PM_POWERDWN);
  }
  return activateAcquisitionThread(pollFd, handle, enabled);
}
