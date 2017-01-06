/*
 * Copyright (c) 2015-2017 Animal Creek Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
