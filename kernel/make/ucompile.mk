# Copyright 2016 The Fuchsia Authors
# Copyright (c) 2008-2015 Travis Geiselbrecht
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

# create a separate list of objects per source type
MODULE_CSRCS := $(filter %.c,$(MODULE_SRCS))
MODULE_CCSRCS := $(filter %.cc,$(MODULE_SRCS))
MODULE_CPPSRCS := $(filter %.cpp,$(MODULE_SRCS))
MODULE_ASMSRCS := $(filter %.S,$(MODULE_SRCS))
MODULE_ASMSRCS2 := $(filter %.s,$(MODULE_SRCS))

MODULE_COBJS := $(call TOMODULEDIR,$(patsubst %.c,%.c.o,$(MODULE_CSRCS)))
MODULE_CCOBJS := $(call TOMODULEDIR,$(patsubst %.cc,%.cc.o,$(MODULE_CCSRCS)))
MODULE_CPPOBJS := $(call TOMODULEDIR,$(patsubst %.cpp,%.cpp.o,$(MODULE_CPPSRCS)))
MODULE_ASMOBJS := $(call TOMODULEDIR,$(patsubst %.S,%.S.o,$(MODULE_ASMSRCS)))
MODULE_ASMOBJS2 := $(call TOMODULEDIR,$(patsubst %.s,%.s.o,$(MODULE_ASMSRCS2)))

MODULE_OBJS := $(MODULE_COBJS) $(MODULE_CPPOBJS) $(MODULE_CCOBJS) $(MODULE_ASMOBJS) $(MODULE_ASMOBJS2)

#$(info MODULE_SRCS = $(MODULE_SRCS))
#$(info MODULE_CSRCS = $(MODULE_CSRCS))
#$(info MODULE_CCSRCS = $(MODULE_CCSRCS))
#$(info MODULE_CPPSRCS = $(MODULE_CPPSRCS))
#$(info MODULE_ASMSRCS = $(MODULE_ASMSRCS))
#$(info MODULE_ASMSRCS2 = $(MODULE_ASMSRCS2))

#$(info MODULE_OBJS = $(MODULE_OBJS))
#$(info MODULE_COBJS = $(MODULE_COBJS))
#$(info MODULE_CCOBJS = $(MODULE_CCOBJS))
#$(info MODULE_CPPOBJS = $(MODULE_CPPOBJS))
#$(info MODULE_ASMOBJS = $(MODULE_ASMOBJS))
#$(info MODULE_ASMOBJS2 = $(MODULE_ASMOBJS2))

$(MODULE_OBJS): MODULE_OPTFLAGS:=$(MODULE_OPTFLAGS)
$(MODULE_OBJS): MODULE_COMPILEFLAGS:=$(MODULE_COMPILEFLAGS)
$(MODULE_OBJS): MODULE_CFLAGS:=$(MODULE_CFLAGS)
$(MODULE_OBJS): MODULE_CPPFLAGS:=$(MODULE_CPPFLAGS)
$(MODULE_OBJS): MODULE_ASMFLAGS:=$(MODULE_ASMFLAGS)
$(MODULE_OBJS): MODULE_SRCDEPS:=$(MODULE_SRCDEPS)

$(MODULE_COBJS): $(MODULE_BUILDDIR)/%.c.o: %.c $(MODULE_SRCDEPS)
	@$(MKDIR)
	@echo compiling $<
	$(NOECHO)$(CC) $(GLOBAL_OPTFLAGS) $(MODULE_OPTFLAGS) $(GLOBAL_COMPILEFLAGS) $(USER_COMPILEFLAGS) $(ARCH_COMPILEFLAGS) $(MODULE_COMPILEFLAGS) $(GLOBAL_CFLAGS) $(USER_CFLAGS) $(ARCH_CFLAGS) $(MODULE_CFLAGS) $(GLOBAL_INCLUDES) -c $< -MD -MP -MT $@ -MF $(@:%o=%d) -o $@

$(MODULE_CCOBJS): $(MODULE_BUILDDIR)/%.cc.o: %.cc $(MODULE_SRCDEPS)
	@$(MKDIR)
	@echo compiling $<
	$(NOECHO)$(CC) $(GLOBAL_OPTFLAGS) $(MODULE_OPTFLAGS) $(GLOBAL_COMPILEFLAGS) $(USER_COMPILEFLAGS) $(ARCH_COMPILEFLAGS) $(MODULE_COMPILEFLAGS) $(GLOBAL_CPPFLAGS) $(USER_CPPFLAGS) $(ARCH_CPPFLAGS) $(MODULE_CPPFLAGS) $(GLOBAL_INCLUDES) -c $< -MD -MP -MT $@ -MF $(@:%o=%d) -o $@

$(MODULE_CPPOBJS): $(MODULE_BUILDDIR)/%.cpp.o: %.cpp $(MODULE_SRCDEPS)
	@$(MKDIR)
	@echo compiling $<
	$(NOECHO)$(CC) $(GLOBAL_OPTFLAGS) $(MODULE_OPTFLAGS) $(GLOBAL_COMPILEFLAGS) $(USER_COMPILEFLAGS) $(ARCH_COMPILEFLAGS) $(MODULE_COMPILEFLAGS) $(GLOBAL_CPPFLAGS) $(USER_CPPFLAGS) $(ARCH_CPPFLAGS) $(MODULE_CPPFLAGS) $(GLOBAL_INCLUDES) -c $< -MD -MP -MT $@ -MF $(@:%o=%d) -o $@

$(MODULE_ASMOBJS): $(MODULE_BUILDDIR)/%.S.o: %.S $(MODULE_SRCDEPS)
	@$(MKDIR)
	@echo compiling $<
	$(NOECHO)$(CC) $(GLOBAL_OPTFLAGS) $(MODULE_OPTFLAGS) $(GLOBAL_COMPILEFLAGS) $(USER_COMPILEFLAGS) $(ARCH_COMPILEFLAGS) $(MODULE_COMPILEFLAGS) $(GLOBAL_ASMFLAGS) $(USER_ASMFLAGS) $(ARCH_ASMFLAGS) $(MODULE_ASMFLAGS) $(GLOBAL_INCLUDES) -c $< -MD -MP -MT $@ -MF $(@:%o=%d) -o $@

$(MODULE_ASMOBJS2): $(MODULE_BUILDDIR)/%.s.o: %.s $(MODULE_SRCDEPS)
	@$(MKDIR)
	@echo compiling $<
	$(NOECHO)$(CC) $(GLOBAL_OPTFLAGS) $(MODULE_OPTFLAGS) $(GLOBAL_COMPILEFLAGS) $(USER_COMPILEFLAGS) $(ARCH_COMPILEFLAGS) $(MODULE_COMPILEFLAGS) $(GLOBAL_ASMFLAGS) $(USER_ASMFLAGS) $(ARCH_ASMFLAGS) $(MODULE_ASMFLAGS) $(GLOBAL_INCLUDES) -c $< -MD -MP -MT $@ -MF $(@:%o=%d) -o $@

# clear some variables we set here
MODULE_CSRCS :=
MODULE_CCSRCS :=
MODULE_CPPSRCS :=
MODULE_ASMSRCS :=
MODULE_ASMSRCS2 :=
MODULE_COBJS :=
MODULE_CCOBJS :=
MODULE_CPPOBJS :=
MODULE_ASMOBJS :=
MODULE_ASMOBJS2 :=

# MODULE_OBJS is passed back
#MODULE_OBJS :=

