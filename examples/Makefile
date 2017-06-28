#
# Copyright (c) 2017 Animal Creek Technologies, Inc.
#
# Released under the MIT license.
# SPDX-License-Identifier: MIT
#

#
# Set 'Q' to '@' to quiet the output; otherwise, leave unset
#

CFLAGS:=-g

PROGRAMS = irq_mon

irq_mon_OBJS = irq_mon.o ../gpio.o ../libfile/file.o

PHONY := all
all: $(PROGRAMS)

define PROGRAM_template =
 -include $$($(1)_OBJS:.o=.d)
 -include $$($(1)_LIBS:.o=.d)
 $(1): $$($(1)_OBJS) $$($(1)_LIBS)
 ALL_INTERMEDIATE += $$($(1)_OBJS) $$($(1)_LIBS)
 ALL_INTERMEDIATE += $$($(1)_OBJS:.o=.d) $$($(1)_LIBS:.o=.d)
endef

$(foreach prog,$(PROGRAMS),$(eval $(call PROGRAM_template,$(prog))))

$(PROGRAMS):
	@echo "  LINK   $(patsubst $(TOPDIR)/%/$<,%,$(realpath $<))/$@"
	$(Q)$(LINK.o) $^ -o $@ $(LDLIBS)

%.d: %.c
	@set -e; \
	rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o: %.c
	@echo "  CC    $(subst $(TOPDIR)/, ,$(realpath $<))"
	$(Q)$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

PHONY += clean
clean:
	@echo "  CLEAN   $(subst $(TOPDIR)/, ,$(CURDIR))"
	$(Q)$(RM) $(irq_mon_OBJS) $(a_OBJS)
	$(Q)$(RM) $(PROGRAMS) $(ALL_INTERMEDIATE)

.PHONY: $(PHONY)
