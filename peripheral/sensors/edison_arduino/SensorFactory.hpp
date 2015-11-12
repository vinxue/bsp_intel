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

#ifndef SENSOR_FACTORY_HPP
#define SENSOR_FACTORY_HPP

#include "Sensor.hpp"

/**
 * SensorFactory implements the Factory design pattern to create sensors
 *
 * A sensor can be created by calling the createSensor method with
 * the sensor type as argument.
 */
class SensorFactory {
  public:

    /**
     * Create a sensor of a certain type
     * @param type sensor type
     * @return constructed sensor
     */
    static Sensor * createSensor(Sensor::Type type);
};

#endif  // SENSOR_FACTORY_HPP
