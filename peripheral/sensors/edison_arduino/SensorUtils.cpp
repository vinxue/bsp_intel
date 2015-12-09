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

#include <mraa.hpp>
#include "SensorUtils.hpp"

const int kTriStateAllGpioPin = 214;
const int kArduinoI2cBusNumber = 6;
const int kNonArduinoI2cBusNumber = 1;

bool SensorUtils::initialized = false;
int SensorUtils::i2cBusNumber = -1;

int SensorUtils::getI2cBusNumber() {
  init();

  return i2cBusNumber;
}

void SensorUtils::init() {
  if (!SensorUtils::initialized) {
    mraa::Gpio *gpio = nullptr;

    try {
      gpio = new mraa::Gpio(kTriStateAllGpioPin, true, true);
    } catch(...) {}

    if (gpio == nullptr) {
      i2cBusNumber = kNonArduinoI2cBusNumber;
    } else {
      delete gpio;
      i2cBusNumber = kArduinoI2cBusNumber;
    }

    SensorUtils::initialized = true;
  }
}
