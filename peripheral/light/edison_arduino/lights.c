/*
 * Copyright (C) 2008 The Android Open Source Project
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

/*
 * Based on htc/flounder/lights/lights.h
 */

#define LOG_TAG "lights"

#include <cutils/log.h>

#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>
#include <hardware/hardware.h>

#define UNUSED(x) (void)(x)

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
char const* const DS2_BRIGHTNESS = "/sys/class/leds/DS2_Green_LED/brightness";
char const* const DS2_ON_MS = "/sys/class/leds/DS2_Green_LED/delay_on";
char const* const DS2_OFF_MS = "/sys/class/leds/DS2_Green_LED/delay_off";

static int write_int(char const *path, int value)
{
	int fd;
	static int already_warned = -1;
	fd = open(path, O_RDWR);
	if (fd >= 0) {
		char buffer[20];
		int err = 0;
		int bytes = snprintf(buffer, 20, "%d\n", value);
		int amt = write(fd, buffer, bytes);
		if (amt == -1)
			err = errno;
		close(fd);
		return amt == -1 ? -err : 0;
	} else {
		if (already_warned == -1) {
			ALOGE("write_int failed to open %s\n", path);
			already_warned = 1;
		}
		return -errno;
	}
}


static int set_light_notifications(struct light_device_t* dev,
		struct light_state_t const* state)
{
	UNUSED(dev);
	int err;
	pthread_mutex_lock(&g_lock);
	ALOGV("set_light_notifications, flashMode:%x, color:%x", state->flashMode, state->color);
	if(state->flashMode) {
		if (0 == (err = write_int(DS2_OFF_MS, state->flashOffMS)))
			err = write_int(DS2_ON_MS, state->flashOnMS);
	} else {
		// The driver regards color as 0(off)/anything-else(on)
		err = write_int(DS2_BRIGHTNESS, state->color);
	}

	pthread_mutex_unlock(&g_lock);
	return err;
}

static int close_lights(struct light_device_t *dev)
{
	if (dev)
		free(dev);
	return 0;
}

/** Open a new instance of a lights device using name */
static int open_lights(const struct hw_module_t *module, char const *name,
		struct hw_device_t **device)
{
	struct light_device_t *dev = malloc(sizeof(struct light_device_t));
	int (*set_light) (struct light_device_t *dev,
			struct light_state_t const *state);

	pthread_t lighting_poll_thread;
	ALOGV("open lights");
	if (dev == NULL) {
		ALOGE("failed to allocate memory");
		return -1;
	}
	memset(dev, 0, sizeof(*dev));

	if (0 == strcmp(LIGHT_ID_NOTIFICATIONS, name))
		set_light = set_light_notifications;
	else
		return -EINVAL;

	pthread_mutex_init(&g_lock, NULL);

	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t *)module;
	dev->common.close = (int (*)(struct hw_device_t *))close_lights;
	dev->set_light = set_light;

	*device = (struct hw_device_t *)dev;

	return 0;
}

static struct hw_module_methods_t lights_methods =
{
	.open =  open_lights,
};

struct hw_module_t HAL_MODULE_INFO_SYM =
{
	.tag = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id = LIGHTS_HARDWARE_MODULE_ID,
	.name = "Edison lights module",
	.author = "Intel",
	.methods = &lights_methods,
};
