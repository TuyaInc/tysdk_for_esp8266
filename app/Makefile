#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#

# add user lib
COMPONENTS_eagle.app.v6 = \
	libuser.a

ifndef PDIR # {
GEN_IMAGES= eagle.app.v6.out
GEN_BINS= eagle.app.v6.bin
SPECIAL_MKTARGETS=$(APP_MKTARGETS)
SUBDIRS=    \
	tuya_user/$(APP_BIN_NAME)/user
endif # } PDIR

# add by nzy 20150804 set TOP_DIR
TOP_DIR = ..
sinclude $(TOP_DIR)/app/make_rule

INCLUDES := $(INCLUDES) -I $(PDIR)include
INCLUDES += $(TOP_DIR)/app/tuya_user/$(APP_BIN_NAME)/include $(TOP_DIR)/app/tuya_user/$(APP_BIN_NAME)/user
PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile

.PHONY: FORCE
FORCE:

