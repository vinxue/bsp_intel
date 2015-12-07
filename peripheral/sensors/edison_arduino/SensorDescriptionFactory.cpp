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
#include <mraa.hpp>
#include "SensorDescriptionFactory.hpp"
#include "sensors/Sensors.hpp"

const int kTriStateAllGpioPin = 214;
const int kArduinoI2cBusNumber = 6;
const int kNonArduinoI2cBusNumber = 1;

bool SensorDescriptionFactory::initialized = false;
int SensorDescriptionFactory::i2cBusNumber = -1;
struct sensor_t SensorDescriptionFactory::descriptions[Sensor::Type::kNumTypes];

struct sensor_t const *
SensorDescriptionFactory::getDescription(Sensor::Type type) {
  switch(type) {
    case Sensor::Type::kMPU9150Accelerometer:
      return &MPU9150Accelerometer::kSensorDescription;
    case Sensor::Type::kMMA7660Accelerometer:
      return &MMA7660Accelerometer::kSensorDescription;
    case Sensor::Type::kGroveLight:
      return &GroveLight::kSensorDescription;
    default:
      ALOGE("%s: No %d sensor description available.", __func__, type);
  }

  return nullptr;
}

struct sensor_t const * SensorDescriptionFactory::getDescriptions() {
  init();

  return descriptions;
}

bool SensorDescriptionFactory::areFlagsSet(int handle, uint32_t flags) {
  if ((handle < 0) || (handle >= Sensor::Type::kNumTypes)) {
    return false;
  }

  init();

  return (descriptions[handle].flags & flags) == flags;
}

int SensorDescriptionFactory::getI2cBusNumber() {
  init();

  return i2cBusNumber;
}

void SensorDescriptionFactory::init() {
  if (!SensorDescriptionFactory::initialized) {
    mraa::Gpio *gpio = NULL;

    for (int i = 0; i < Sensor::Type::kNumTypes; i++) {
      descriptions[i] = *SensorDescriptionFactory::getDescription(
          static_cast<Sensor::Type>(i));
    }

    try {
      gpio = new mraa::Gpio(kTriStateAllGpioPin, true, true);
    } catch(...) {}

    if (gpio == NULL) {
      i2cBusNumber = kNonArduinoI2cBusNumber;
    } else {
      delete gpio;
      i2cBusNumber = kArduinoI2cBusNumber;
    }

    SensorDescriptionFactory::initialized = true;
  }
}
