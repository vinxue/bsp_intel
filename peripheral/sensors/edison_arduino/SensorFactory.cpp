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
#include "SensorFactory.hpp"
#include "sensors/Sensors.hpp"

Sensor * SensorFactory::createSensor(Sensor::Type type) {
  try {
    switch(type) {
      case Sensor::Type::kMPU9150Accelerometer:
        return new MPU9150Accelerometer(
            6, MPU9150_DEFAULT_I2C_ADDR, AK8975_DEFAULT_I2C_ADDR, false);
        break;
      case Sensor::Type::kMMA7660Accelerometer:
        return new MMA7660Accelerometer(6, MMA7660_DEFAULT_I2C_ADDR);
        break;
      case Sensor::Type::kGroveLight:
        return new GroveLight(0);
        break;
      default:
        ALOGE("%s: No %d sensor available.",__func__, type);
    }
  } catch (const std::runtime_error& e) {
    ALOGE("%s: Failed to instantiate sensor %d.",__func__, type);
  }
  return nullptr;
}
