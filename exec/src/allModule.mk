# @file module.mk
#
# The topmost subsystem (module) make include file. Uses the MODULES
# list to determine what files to continue the make with (the
# submodules themselves).
EXECUTABLE_MODULES := tools
MODULES :=


include $(patsubst %,$(SOURCE)/%/module.mk,$(MODULES))
include $(patsubst %,$(SOURCE)/%/executable.mk,$(EXECUTABLE_MODULES))
