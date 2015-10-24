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

#ifndef SENSORS_SENSORS_LIST_H
#define SENSORS_SENSORS_LIST_H

#include <stdint.h>

#include "mpu9150.h"
#include "mma7660.h"

using namespace upm;

struct sensors_event_t;

class SensorBase {
 public:
  SensorBase();
  virtual ~SensorBase();

  virtual int activate(int handle, int enabled);
  virtual int setDelay(int handle, int64_t ns);
  virtual int pollEvents(sensors_event_t* data, int count) = 0;
  virtual int batch(int handle, int flags, int64_t period_ns, int64_t timeout);
  virtual int flush(int handle);
};

class MPU9150Accelerometer : public SensorBase, public MPU9150 {
 public:
  static const int kMaxRange = 1000;

  MPU9150Accelerometer(int bus=MPU9150_I2C_BUS, int address=MPU9150_DEFAULT_I2C_ADDR,
      int magAddress=AK8975_DEFAULT_I2C_ADDR, bool enableAk8975=false);

  ~MPU9150Accelerometer() override;

  int pollEvents(sensors_event_t* data, int count) override;
  int activate(int handle, int enabled);
};

class MMA7660Accelerometer : public SensorBase, public MMA7660 {
 public:
  static const int kMaxRange = 1000;

  MMA7660Accelerometer(int bus, uint8_t address);
  ~MMA7660Accelerometer() override;

  int pollEvents(sensors_event_t* data, int count) override;
  int activate(int handle, int enabled);
};

// NEW sensors: classes here

#endif  // SENSORS_SENSORS_LIST_H
