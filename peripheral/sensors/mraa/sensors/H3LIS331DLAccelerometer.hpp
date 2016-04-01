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

#ifndef H3LIS331DL_ACCELEROMETER_HPP
#define H3LIS331DL_ACCELEROMETER_HPP

#include <hardware/sensors.h>
#include "Sensor.hpp"
#include "h3lis331dl.h"

struct sensors_event_t;

/**
 * H3LIS331DLAccelerometer exposes the H3LIS331DL accelerometer sensor
 *
 * Overrides the pollEvents & activate Sensor methods.
 */
class H3LIS331DLAccelerometer : public Sensor, public upm::H3LIS331DL {
  public:
    /**
     * H3LIS331DLAccelerometer constructor
     * @param pollFd poll file descriptor
     * @param bus number of the bus
     * @param address device address
     */
    H3LIS331DLAccelerometer(int pollFd, int bus=H3LIS331DL_I2C_BUS,
        int address=H3LIS331DL_DEFAULT_I2C_ADDR);

    /**
     * H3LIS331DLAccelerometer destructor
     */
    ~H3LIS331DLAccelerometer() override;

    /**
     * Poll for events
     * @param data where to store the events
     * @param count the number of events returned must be <= to the count
     * @return number of events returned in data on success and a negative error number otherwise
     */
    int pollEvents(sensors_event_t* data, int count) override;

    /**
     * Activate the sensor
     * @param handle sensor identifier
     * @param enabled 1 for enabling and 0 for disabling
     * @return 0 on success and a negative error number otherwise
     */
    int activate(int handle, int enabled);

  private:
    static Sensor * createSensor(int pollFd);
    static void initModule() __attribute__((constructor));

    int pollFd;
    static struct sensor_t sensorDescription;
};

#endif  // H3LIS331DL_ACCELEROMETER_HPP
