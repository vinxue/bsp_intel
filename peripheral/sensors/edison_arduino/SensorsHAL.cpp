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

#include <string.h>
#include <cutils/log.h>
#include <stdexcept>
#include <errno.h>
#include <sys/epoll.h>
#include "SensorsHAL.hpp"
#include "SensorFactory.hpp"

#define MAX_DEVICES  20

int gPollFd;

SensorContext::SensorContext(const hw_module_t* module) {
  memset(&device, 0, sizeof(device));

  device.common.tag = HARDWARE_DEVICE_TAG;
  device.common.version = SENSORS_DEVICE_API_VERSION_1_0;
  device.common.module = const_cast<hw_module_t*>(module);
  device.common.close = CloseWrapper;
  device.activate = ActivateWrapper;
  device.setDelay = SetDelayWrapper;
  device.poll = PollEventsWrapper;
  device.batch = BatchWrapper;
  device.flush = FlushWrapper;

  memset(sensors, 0, sizeof(Sensor *) * Sensor::Type::kNumTypes);
}

SensorContext::~SensorContext() {
  for (int i = 0; i < Sensor::Type::kNumTypes; i++) {
    if (sensors[i]) {
      delete sensors[i];
      sensors[i] = nullptr;
    }
  }
}

int SensorContext::activate(int handle, int enabled) {
  int rc = 0;

  if (enabled != 0 && enabled != 1) {
    ALOGE("%s: Invalid parameter", __func__);
    return -EINVAL;
  }

  if (handle < 0 || handle >= Sensor::Type::kNumTypes) {
    return -EINVAL;
  }

  try {
    if (enabled) {
      if (sensors[handle] == nullptr) {
        sensors[handle] = SensorFactory::createSensor(static_cast<Sensor::Type>(handle));
        if (sensors[handle] == nullptr) {
          return -1;
        }
        rc = sensors[handle]->activate(handle, enabled);
        if (rc != 0) {
          goto delete_sensor;
        }
      } else {
        return 0;
      }
    } else {
      if (sensors[handle] != nullptr) {
        rc = sensors[handle]->activate(handle, enabled);
        delete sensors[handle];
        sensors[handle] = nullptr;
      } else {
        return 0;
      }
    }

    return rc;
  } catch (const std::exception& e) {
    /* The upper layer doesn't expect exceptions. Catch them all. */
    ALOGE("%s: Failed to %s sensor %d. Error message: %s.",
        __func__, enabled ? "activate" : "deactivate", handle, e.what());
  }

delete_sensor:
  if (sensors[handle] != nullptr) {
    delete sensors[handle];
    sensors[handle] = nullptr;
  }

  return -1;
}

int SensorContext::setDelay(int handle, int64_t ns) {
  if (handle < 0 || handle >= Sensor::Type::kNumTypes) {
    return -EINVAL;
  }

  if (sensors[handle] == nullptr) {
    ALOGE("%s: cannot set delay. sensor %d is not activated", __func__, handle);
    return -EINVAL;
  }

  return sensors[handle]->setDelay(handle, ns);
}

int SensorContext::pollEvents(sensors_event_t* data, int count) {
  int nfds, i;
  struct epoll_event ev[MAX_DEVICES];
  int returned_events = 0;

  /* return only when at least one event is available */
  while(true) {
    nfds = epoll_wait(gPollFd, ev, MAX_DEVICES, -1);
    for(i = 0; i < nfds && returned_events < count; i++) {
      if (ev[i].events == EPOLLIN) {

        if(sensors[ev[i].data.u32]->readOneEvent(data + returned_events)) {
          returned_events++;
        }
      }
    }
    if (returned_events > 0) {
      return returned_events;
    }
  }
}

int SensorContext::batch(int handle, int flags,
                         int64_t period_ns, int64_t timeout) {
  if (handle < 0 || handle >= Sensor::Type::kNumTypes) {
    return -EINVAL;
  }

  if (sensors[handle] == nullptr) {
    ALOGE("%s: cannot set delay. sensor %d is not activated", __func__, handle);
    return -EINVAL;
  }

  return sensors[handle]->batch(handle, flags, period_ns, timeout);
}

int SensorContext::flush(int handle) {
  if (handle < 0 || handle >= Sensor::Type::kNumTypes) {
    return -EINVAL;
  }

  if (sensors[handle] == nullptr) {
    ALOGE("%s: cannot set delay. sensor %d is not activated", __func__, handle);
    return -EINVAL;
  }

  /* flush doesn't apply to one-shot sensors */
  if (SensorDescriptionFactory::areFlagsSet(
      handle, SENSOR_FLAG_ONE_SHOT_MODE))
    return -EINVAL;

  return sensors[handle]->flush(handle);
}

int SensorContext::CloseWrapper(hw_device_t* dev) {
  SensorContext* sensorContext = reinterpret_cast<SensorContext*>(dev);
  int rc;

  rc = close(gPollFd);
  if (rc != 0) {
    ALOGE("Cannot close poll file descriptor");
  }

  if (sensorContext != nullptr) {
    delete sensorContext;
  }

  return 0;
}

int SensorContext::ActivateWrapper(sensors_poll_device_t* dev,
                                   int handle, int enabled) {
  return reinterpret_cast<SensorContext*>(dev)->activate(handle, enabled);
}

int SensorContext::SetDelayWrapper(sensors_poll_device_t* dev,
                                   int handle, int64_t ns) {
  return reinterpret_cast<SensorContext*>(dev)->setDelay(handle, ns);
}

int SensorContext::PollEventsWrapper(sensors_poll_device_t* dev,
                                     sensors_event_t* data, int count) {
  return reinterpret_cast<SensorContext*>(dev)->pollEvents(data, count);
}

int SensorContext::BatchWrapper(sensors_poll_device_1_t* dev, int handle,
                                int flags, int64_t period_ns, int64_t timeout) {
  return reinterpret_cast<SensorContext*>(dev)->batch(handle, flags, period_ns,
                                                      timeout);
}

int SensorContext::FlushWrapper(sensors_poll_device_1_t* dev,
                                int handle) {
  return reinterpret_cast<SensorContext*>(dev)->flush(handle);
}

static int open_sensors(const struct hw_module_t* module,
                        const char* id, struct hw_device_t** device) {
  SensorContext* ctx;

  ctx = new SensorContext(module);
  *device = &ctx->device.common;

  /* create the epoll fd used to register the incoming fds */
  gPollFd = epoll_create(MAX_DEVICES);
  if (gPollFd == -1) {
    ALOGE("%s: Failed to create epoll", __func__);
    return -1;
  }

  return 0;
}

static struct hw_module_methods_t sensors_module_methods = {
  open: open_sensors,
};

static int get_sensors_list(struct sensors_module_t* module,
                            struct sensor_t const** list) {
  if (!list) return 0;
  *list = SensorDescriptionFactory::getDescriptions();
  return Sensor::Type::kNumTypes;
}

struct sensors_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: SENSORS_HARDWARE_MODULE_ID,
        name: "Edison Sensor HAL",
        author: "Intel",
        methods: &sensors_module_methods,
        dso: nullptr,
        reserved: {0},
    },
    get_sensors_list: get_sensors_list,
    set_operation_mode: nullptr
};
