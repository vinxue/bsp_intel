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
#include "SensorDescriptionFactory.hpp"
#include "sensors/Sensors.hpp"

bool SensorDescriptionFactory::initialized = false;
struct sensor_t SensorDescriptionFactory::descriptions[Sensor::Type::kNumTypes];

struct sensor_t const *
SensorDescriptionFactory::getDescription(Sensor::Type type) {
  try {
    switch(type) {
      case Sensor::Type::kMPU9150Accelerometer:
        return &MPU9150Accelerometer::kSensorDescription;
        break;
      case Sensor::Type::kMMA7660Accelerometer:
        return &MMA7660Accelerometer::kSensorDescription;
        break;
      case Sensor::Type::kGroveLight:
        return &GroveLight::kSensorDescription;
        break;
      default:
        ALOGE("%s: No %d sensor description available.",__func__, type);
    }
  } catch (const std::runtime_error& e) {
    ALOGE("%s: Failed to get sensor %d description.",__func__, type);
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

void SensorDescriptionFactory::init() {
  if (!SensorDescriptionFactory::initialized) {
    for (int i = 0; i < Sensor::Type::kNumTypes; i++) {
      descriptions[i] = *SensorDescriptionFactory::getDescription(
          static_cast<Sensor::Type>(i));
    }
    SensorDescriptionFactory::initialized = true;
  }
}
