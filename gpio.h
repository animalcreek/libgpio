/*
 * Copyright (c) 2015-2017 Animal Creek Technologies, Inc.
 *
 * Released under the MIT license.
 * SPDX-License-Identifier: MIT
 */

#ifndef __LIBGPIO_H_
#define __LIBGPIO_H_

/**
 * @file libgpio/gpio.h
 * @brief GPIO Helper Library API
 * @author Mark Greer <mgreer@animalcreek.com>
 *
 * Set of routines to provide easy-to-use ways to perform common Linux(tm)
 * GPIO operations.
 */

/**
 * @brief GPIO Polarity types
 *
 * Types used to specify the GPIO polarity.
 */
enum gpio_polarity {
	/** Invalid type (forces initialization) */
	GPIO_POLARITY_INVALID,
	/** Active low */
	GPIO_POLARITY_ACTIVE_LOW,
	/** Active high */
	GPIO_POLARITY_ACTIVE_HIGH,
};

/**
 * @brief GPIO Edge types
 *
 * Types used to specify the GPIO edge(s).
 */
enum gpio_edge {
	/** Invalid type (forces initialization) */
	GPIO_EDGE_INVALID,
	/** No edge */
	GPIO_EDGE_NONE,
	/** Rising edge */
	GPIO_EDGE_RISING,
	/** Falling edge */
	GPIO_EDGE_FALLING,
	/** Rising and falling edge */
	GPIO_EDGE_BOTH,
};

/**
 * @brief GPIO Direction types
 *
 * Types used to specify the GPIO direction.
 */
enum gpio_direction {
	/** Invalid type (forces initialization) */
	GPIO_DIRECTION_INVALID,
	/** Input */
	GPIO_DIRECTION_IN,
	/** Output (default) */
	GPIO_DIRECTION_OUT,
	/** Output with initial low value */
	GPIO_DIRECTION_OUT_LOW,
	/** Output with initial high value */
	GPIO_DIRECTION_OUT_HIGH,
};

int gpio_get_fd(void *gpio_handle, int *fdp);
int gpio_set_polarity(void *gpio_handle, enum gpio_polarity polarity);
int gpio_set_edge(void *gpio_handle, enum gpio_edge edge);
int gpio_set_direction(void *gpio_handle, enum gpio_direction dir);
int gpio_read(void *gpio_handle, char *cp);
int gpio_write(void *gpio_handle, char c);
int gpio_init(char *gpio_num, void **gpio_handlep);
int gpio_destroy(void *gpio_handle);

#endif
