/*
 * Copyright (c) 2015-2017 Animal Creek Technologies, Inc.
 *
 * Released under the MIT license.
 * SPDX-License-Identifier: MIT
 */

/**
 * @file libgpio/gpio.c
 * @brief GPIO Helper Library
 * @author Mark Greer <mgreer@animalcreek.com>
 *
 * Set of routines to provide easy-to-use ways to perform common Linux(tm)
 * GPIO operations.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

#include "libfile/file.h"
#include "gpio.h"

/** @cond */
#define GPIO_PATH		"/sys/class/gpio/"
#define GPIO_EXPORT_PATH	GPIO_PATH "export"
#define GPIO_UNEXPORT_PATH	GPIO_PATH "unexport"

struct gpio_info {
	char		*gpio_num;
	int		fd;
	char		*path;
	char		*dir_path;
	char		*value_path;
	char		*edge_path;
	char		*active_low_path;
};
/** @endcond */

/**
 * @brief Get file descriptor associated with GPIO
 * @param [in] gpio_handle Handle returned by gpio_init()
 * @param [out] fdp Pointer to area where file descriptor is placed
 * @return 0 on success, negative errno on failure
 */
int gpio_get_fd(void *gpio_handle, int *fdp)
{
	struct gpio_info *gip = gpio_handle;

	if (!gip)
		return -EINVAL;

	*fdp = gip->fd;

	return 0;
}

/**
 * @brief Set GPIO polarity
 * @param [in] gpio_handle Handle returned by gpio_init()
 * @param [in] polarity Polarity to set GPIO to
 * @return 0 on success, negative errno on failure
 */
int gpio_set_polarity(void *gpio_handle, enum gpio_polarity polarity)
{
	struct gpio_info *gip = gpio_handle;
	char *polarity_str;

	if (!gip)
		return -EINVAL;

	if (polarity == GPIO_POLARITY_ACTIVE_LOW)
		polarity_str = "1";
	else if (polarity == GPIO_POLARITY_ACTIVE_HIGH)
		polarity_str = "0";
	else
		return -EINVAL;

	return file_owc(gip->active_low_path, polarity_str,
			strlen(polarity_str) + 1);
}

/**
 * @brief Set GPIO edge
 * @param [in] gpio_handle Handle returned by gpio_init()
 * @param [in] edge Edge to set GPIO to
 * @return 0 on success, negative errno on failure
 */
int gpio_set_edge(void *gpio_handle, enum gpio_edge edge)
{
	struct gpio_info *gip = gpio_handle;
	char *edge_str;

	if (!gip)
		return -EINVAL;

	if (edge == GPIO_EDGE_NONE)
		edge_str = "none";
	else if (edge == GPIO_EDGE_RISING)
		edge_str = "rising";
	else if (edge == GPIO_EDGE_FALLING)
		edge_str = "falling";
	else if (edge == GPIO_EDGE_BOTH)
		edge_str = "both";
	else
		return -EINVAL;

	return file_owc(gip->edge_path, edge_str, strlen(edge_str) + 1);
}

/**
 * @brief Set GPIO direction
 * @param [in] gpio_handle Handle returned by gpio_init()
 * @param [in] dir Direction to set GPIO to
 * @return 0 on success, negative errno on failure
 */
int gpio_set_direction(void *gpio_handle, enum gpio_direction dir)
{
	struct gpio_info *gip = gpio_handle;
	char *dir_str;

	if (!gip)
		return -EINVAL;

	if (dir == GPIO_DIRECTION_IN)
		dir_str = "in";
	else if (dir == GPIO_DIRECTION_OUT)
		dir_str = "out";
	else if (dir == GPIO_DIRECTION_OUT_LOW)
		dir_str = "low";
	else if (dir == GPIO_DIRECTION_OUT_HIGH)
		dir_str = "high";
	else
		return -EINVAL;

	return file_owc(gip->dir_path, dir_str, strlen(dir_str) + 1);
}

/** @cond */
static int gpio_rw(void *gpio_handle, char *cp, int write_flag)
{
	struct gpio_info *gip = gpio_handle;
	off_t offset;
	ssize_t ssize;

	if (!gip)
		return -EINVAL;

	offset = lseek(gip->fd, 0, SEEK_SET);
	if (offset > 0)
		return -EIO;
	else if (offset < 0)
		return -errno;

	if (write_flag)
		ssize = write(gip->fd, cp, 1);
	else
		ssize = read(gip->fd, cp, 1);

	if (ssize == 1)
		return 0;
	else if (ssize >= 0)
		return -EIO;
	else
		return -errno;
}
/** @endcond */

/**
 * @brief Get value of GPIO
 * @param [in] gpio_handle Handle returned by gpio_init()
 * @param [out] cp Pointer to where value is placed
 * @return 0 on success, negative errno on failure
 */
int gpio_read(void *gpio_handle, char *cp)
{
	return gpio_rw(gpio_handle, cp, 0);
}

/**
 * @brief Set value of GPIO
 * @param [in] gpio_handle Handle returned by gpio_init()
 * @param [in] c Value to set GPIO to
 * @return 0 on success, negative errno on failure
 */
int gpio_write(void *gpio_handle, char c)
{
	return gpio_rw(gpio_handle, &c, 1);
}

/** @cond */
static char *gpio_alloc_fullpath(char *begin, char *middle, char *end)
{
	char *s;
	size_t size;
	int ret;

	size = strlen(begin) + strlen(middle) + strlen(end) + 1;

	s = malloc(size);
	if (!s)
		return NULL;

	ret = snprintf(s, size, "%s%s%s", begin, middle, end);
	if ((ret < 0) || ((unsigned int)ret >= size)) {
		free(s);
		return NULL;
	}

	return s;
}
/** @endcond */

/**
 * @brief Initialize a handle that provides access to specified GPIO
 * @param [in] gpio_num Number of GPIO
 * @param [out] gpio_handlep Pointer to handle to use for GPIO operations
 * @return 0 on success, negative errno on failure
 */
int gpio_init(char *gpio_num, void **gpio_handlep)
{
	struct gpio_info *gip;
	size_t gpio_num_len;
	unsigned int i;
	int ret;

	if (!gpio_handlep)
		return -EINVAL;

	gpio_num_len = strlen(gpio_num);

	if (gpio_num_len > 10)
		return -EINVAL;

	for (i = 0; i < gpio_num_len; i++) {
		if (!isdigit(gpio_num[i]))
			return -EINVAL;
	}

	gip = malloc(sizeof(*gip));
	if (!gip)
		return -ENOMEM;

	gip->gpio_num = strdup(gpio_num);
	if (!gip->gpio_num) {
		ret = -errno;
		goto err_free_gip;
	}

	gip->path = gpio_alloc_fullpath(GPIO_PATH, "gpio", gpio_num);
	if (!gip->path) {
		ret = -ENOMEM;
		goto err_free_gpio_num;
	}

	if (file_is_present(gip->path)) {
		ret = -EBUSY;
		goto err_free_path;
	}

	gip->dir_path = gpio_alloc_fullpath(gip->path, "/", "direction");
	if (!gip->dir_path) {
		ret = -ENOMEM;
		goto err_free_path;
	}

	gip->value_path = gpio_alloc_fullpath(gip->path, "/", "value");
	if (!gip->value_path) {
		ret = -ENOMEM;
		goto err_free_dir_path;
	}

	gip->edge_path = gpio_alloc_fullpath(gip->path, "/", "edge");
	if (!gip->edge_path) {
		ret = -ENOMEM;
		goto err_free_value_path;
	}

	gip->active_low_path = gpio_alloc_fullpath(gip->path, "/",
						   "active_low");
	if (!gip->active_low_path) {
		ret = -ENOMEM;
		goto err_free_edge_path;
	}

	ret = file_owc(GPIO_EXPORT_PATH, gpio_num, gpio_num_len + 1);
	if (ret)
		goto err_free_active_low_path;

	gip->fd = open(gip->value_path, O_RDWR);
	if (gip->fd < 0) {
		ret = -errno;
		goto err_unexport;
	}

	*gpio_handlep = gip;

	return 0;

err_unexport:
	file_owc(GPIO_UNEXPORT_PATH, gpio_num, gpio_num_len + 1);
err_free_active_low_path:
	free(gip->active_low_path);
err_free_edge_path:
	free(gip->edge_path);
err_free_value_path:
	free(gip->value_path);
err_free_dir_path:
	free(gip->dir_path);
err_free_path:
	free(gip->path);
err_free_gpio_num:
	free(gip->gpio_num);
err_free_gip:
	free(gip);

	return ret;
}

/**
 * @brief Destroy resource allocated by gpio_init()
 * @param [in] gpio_handle Handle returned by gpio_init()
 * @return 0 on success, negative errno on failure
 */
int gpio_destroy(void *gpio_handle)
{
	struct gpio_info *gip = gpio_handle;

	if (!gip)
		return -EINVAL;

	close(gip->fd);
	file_owc(GPIO_UNEXPORT_PATH, gip->gpio_num, strlen(gip->gpio_num) + 1);
	free(gip->active_low_path);
	free(gip->edge_path);
	free(gip->value_path);
	free(gip->dir_path);
	free(gip->path);
	free(gip->gpio_num);
	free(gip);

	return 0;
}
