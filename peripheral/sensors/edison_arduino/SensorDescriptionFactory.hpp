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

#ifndef SENSOR_DESCRIPTION_FACTORY_HPP
#define SENSOR_DESCRIPTION_FACTORY_HPP

#include <hardware/sensors.h>
#include "Sensor.hpp"

/**
 * Instantiable Factory design pattern class to get sensor descriptions
 *
 * It supports retrieving and checking sensor descriptions parameters
 */
class SensorDescriptionFactory {
  public:

    /**
     * SensorDescriptionFactory constructor
    */
    SensorDescriptionFactory();

    /**
     * get all available sensor descriptions
     * @return array of sensor description data structures
     */
    struct sensor_t const * getDescriptions();

    /**
     * check if a sensor description has certain flags set
     * @param handle sensor type identifier
     * @param flags flags to check
     * @return true if flags are set, false otherwise
     */
    bool areFlagsSet(int handle, uint32_t flags);

  private:

    /*
     * Retrieve a sensor description of a certain type
     * @param type sensor type
     * @return pointer to a description data structure
     */
    static struct sensor_t const * getDescription(Sensor::Type type);

    struct sensor_t descriptions[Sensor::Type::kNumTypes];
};

#endif  // SENSOR_DESCRIPTION_FACTORY_HPP
