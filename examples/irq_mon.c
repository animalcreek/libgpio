/*
 * Copyright (c) 2017 Animal Creek Technologies, Inc.
 *
 * Released under the MIT license.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <sys/select.h>

#include "../gpio.h"

struct info {
	char			*gpio_num;
	enum gpio_polarity	polarity;
};

static bool shutdown;

static int print_usage(char *pgm_name)
{
	char *pgm_fullpath, *pgm_basename;

	pgm_fullpath = strdup(pgm_name);
	if (!pgm_fullpath)
		return -errno;

	pgm_basename = basename(pgm_fullpath);

	fprintf(stderr, "%s [-h | -l] <GPIO #>\n", pgm_basename);
	fprintf(stderr, "\t-h\t- set polarity of GPIO pin to high\n");
	fprintf(stderr, "\t-l\t- set polarity of GPIO pin to low\n");

	free(pgm_fullpath);

	return 0;
}

static int parse_cmdline(int argc, char *argv[], struct info *ip)
{
	char c;
	unsigned int hflg, lflg, err_flg;

	if (argc < 2)
		return -EINVAL;

	hflg = 0;
	lflg = 0;
	err_flg = 0;

	while ((c = getopt(argc, argv, "hl")) != (char)-1) {
		switch (c) {
		case 'h':
			hflg++;
			break;
		case 'l':
			lflg++;
			break;
		default:
			err_flg++;
		}
	}

	if (err_flg || (hflg && lflg) || (optind != (argc - 1)))
		return -EINVAL;

	if (hflg)
		ip->polarity = GPIO_POLARITY_ACTIVE_HIGH;
	else if (lflg)
		ip->polarity = GPIO_POLARITY_ACTIVE_LOW;
	else
		ip->polarity = GPIO_POLARITY_INVALID;

	ip->gpio_num = argv[optind];

	return 0;
}

static void sig_handler(int signum)
{
	shutdown = true;
}

static int set_sigact(void)
{
	struct sigaction sig_act;
	int ret;

	sig_act.sa_handler = sig_handler;
	sig_act.sa_flags = SA_RESETHAND;

	ret = sigaction(SIGINT, &sig_act, NULL);
	if (ret) {
		fprintf(stderr, "can't set SIGINT handler: %d (%s)\n", errno,
			strerror(errno));
		return -errno;
	}

	ret = sigaction(SIGUSR1, &sig_act, NULL);
	if (ret) {
		fprintf(stderr, "can't set SIGUSR1 handler: %d (%s)\n", errno,
			strerror(errno));
		return -errno;
	}

	ret = sigaction(SIGUSR2, &sig_act, NULL);
	if (ret) {
		fprintf(stderr, "can't set SIGUSR2 handler: %d (%s)\n", errno,
			strerror(errno));
		return -errno;
	}

	ret = sigaction(SIGTERM, &sig_act, NULL);
	if (ret) {
		fprintf(stderr, "can't set SIGTERM handler: %d (%s)\n", errno,
			strerror(errno));
		return -errno;
	}

	return 0;
}

static int wait_for_irqs(void *gpio_handle)
{
	fd_set exceptfds;
	int fd, ret;
	char c;

	ret = gpio_get_fd(gpio_handle, &fd);
	if (ret < 0) {
		fprintf(stderr, "can't get file descriptor: %d (%s)\n",
			-ret, strerror(-ret));
		return ret;
	}

	FD_ZERO(&exceptfds);
	FD_SET(fd, &exceptfds);

	do {
		ret = gpio_read(gpio_handle, &c);
		if (ret) {
			if (ret != EINTR)
				fprintf(stderr,
					"can't get read GPIO value: %d (%s)\n",
					-ret, strerror(-ret));

			return -ret;
		}

		ret = select(fd + 1, NULL, NULL, &exceptfds, NULL);
		if (ret < 0) {
			if (errno != EINTR)
				fprintf(stderr, "select() failed: %d (%s)\n",
					-errno, strerror(-errno));

			return -errno;
		} else if ((ret != 1) || !FD_ISSET(fd, &exceptfds)) {
			fprintf(stderr, "bogus return from select(): %d, %d\n",
				ret, errno);
			return -EIO;
		}

		printf("IRQ: %c\n", c);
	} while (!shutdown);

	return -EINTR;
}

int main(int argc, char *argv[])
{
	struct info *ip;
	void *gpio_handle;
	int ret, exit_val = EXIT_FAILURE;

	shutdown = false;

	ret = set_sigact();
	if (ret) {
		fprintf(stderr, "set_sigact() failed: %d (%s)\n", -ret,
			strerror(-ret));
		return EXIT_FAILURE;
	}

	ip = malloc(sizeof(*ip));
	if (!ip) {
		fprintf(stderr, "can't malloc info struct\n");
		return EXIT_FAILURE;
	}

	memset(ip, 0, sizeof(*ip));

	ret = parse_cmdline(argc, argv, ip);
	if (ret) {
		print_usage(argv[0]);
		goto free_ip;
	}

	ret = gpio_init(ip->gpio_num, &gpio_handle);
	if (ret) {
		fprintf(stderr, "gpio_init() failed: %d (%s)\n", -ret,
			strerror(-ret));
		goto free_ip;
	}

	ret = gpio_set_direction(gpio_handle, GPIO_DIRECTION_IN);
	if (ret) {
		fprintf(stderr, "gpio_set_direction() failed: %d (%s)\n", -ret,
			strerror(-ret));
		goto destroy_gpio;
	}

	if (ip->polarity != GPIO_POLARITY_INVALID) {
		ret = gpio_set_polarity(gpio_handle, ip->polarity);
		if (ret) {
			fprintf(stderr, "gpio_set_polarity() failed: %d (%s)\n",
				-ret, strerror(-ret));
			goto destroy_gpio;
		}
	}

	ret = gpio_set_edge(gpio_handle, GPIO_EDGE_BOTH);
	if (ret) {
		fprintf(stderr, "gpio_set_edge() failed: %d (%s)\n", -ret,
			strerror(-ret));
		goto destroy_gpio;
	}

	printf("Waiting for GPIO interrupts...\n");

	ret = wait_for_irqs(gpio_handle);
	if (ret)
		goto destroy_gpio;
	else
		exit_val = EXIT_SUCCESS;

destroy_gpio:
	gpio_destroy(gpio_handle);
free_ip:
	free(ip);

	return exit_val;
}
