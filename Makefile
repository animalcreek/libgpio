#
# Copyright (c) 2017 Animal Creek Technologies, Inc.
#
# Released under the MIT license.
# SPDX-License-Identifier: MIT
#

#
# Set 'Q' to '@' to quiet the output; otherwise, leave unset
#

MAKEFLAGS += --no-print-directory

TOPDIR ?= $(CURDIR)
Q ?= @

export TOPDIR Q

SUBDIRS := libfile examples

libgpio_OBJS = gpio.o

PHONY := all
all: init_submodules update_submodules $(libgpio_OBJS)
	@for i in $(SUBDIRS); do \
		$(MAKE) -C $$i; \
	done

PHONY += clean
clean:
	@echo "  CLEAN   $(subst $(TOPDIR)/, ,$(CURDIR))"
	$(Q)$(RM) $(libgpio_OBJS)
	@for i in $(SUBDIRS); do \
		$(MAKE) -C $$i clean; \
	done

PHONY += init_submodules
init_submodules:
	$(Q)git submodule init

PHONY += update_submodules
update_submodules:
	$(Q)git submodule update --remote

%.o: %.c
	@echo "  CC    $(subst $(TOPDIR)/, ,$(realpath $<))"
	$(Q)$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

.PHONY: $(PHONY)
