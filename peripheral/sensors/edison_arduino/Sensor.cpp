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
#include <hardware/sensors.h>
#include <errno.h>
#include "Sensor.hpp"

/* maximum delay: 1000ms */
const int64_t kMaxDelay = 1000000000;

Sensor::Sensor() : acquisitionThread(nullptr), handle(-1), type(-1), delay(kMaxDelay) {}

Sensor::~Sensor() {
  if (acquisitionThread != nullptr) {
    delete acquisitionThread;
    acquisitionThread = nullptr;
  }
}

int Sensor::activate(int handle, int enabled) { return 0; }

bool Sensor::readOneEvent(sensors_event_t *event) {
  int rc;

  if (acquisitionThread == nullptr) {
    ALOGE("%s: sensor %d doesn't have an acquisition thread", __func__, handle);
    return false;
  }

  /* read one event from the pipe read endpoint */
  rc = read(acquisitionThread->getReadPipeFd(), event, sizeof(sensors_event_t));
  if (rc != sizeof(sensors_event_t)) {
    return false;
  }

  return true;
}

int Sensor::activateAcquisitionThread(int handle, int enabled) {
  if (enabled) {
    /* create/init acquisition thread if necessary */
    if (acquisitionThread == nullptr) {
      acquisitionThread = new AcquisitionThread(this);
      if (!acquisitionThread->init()) {
        ALOGE("%s: Cannot initialize acquisition thread for sensor %d",
            __func__, handle);
        delete acquisitionThread;
        acquisitionThread = nullptr;
        return -1;
      }
    } else {
      ALOGE("%s: Sensor %d is already enabled", __func__, handle);
    }
  } else {
    /* free acquisition thread resources */
    if (acquisitionThread != nullptr) {
      delete acquisitionThread;
      acquisitionThread = nullptr;
    } else {
      ALOGE("%s: Sensor %d is already disabled", __func__, handle);
    }
  }
  return 0;
}

int Sensor::setDelay(int handle, int64_t requestedDelay) {
  if (requestedDelay < 0) {
    return -EINVAL;
  }

  if (requestedDelay > kMaxDelay) {
    requestedDelay = kMaxDelay;
  }

  this->delay = requestedDelay;
  /* wake up thread to immediately change the delay */
  if (acquisitionThread != nullptr) {
    return acquisitionThread->wakeup();
  }

  return 0;
}

int Sensor::batch(int handle, int flags,
                      int64_t period_ns, int64_t timeout) {
  /* batching mode is not supported; call setDelay */
  return setDelay(handle, period_ns);
}

int Sensor::flush(int handle) {
  if (acquisitionThread == nullptr) {
    return -EINVAL;
  }

  /* batching mode is not supported; generate META_DATA_FLUSH_COMPLETE */
  if (acquisitionThread->generateFlushCompleteEvent()) {
    return 0;
  }

  return -1;
}
