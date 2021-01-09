#  copyright (c) 2010 Espressif System
#
ifndef PDIR

endif

THISDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ifdef TARGET_APP_PATH
$(eval SDK_DIR:=$(realpath $(THISDIR)/../..))
endif
ifdef SDK_TARGET_APP_PATH
$(eval SDK_DIR:=$(realpath $(THISDIR)))
endif

TOOLCHAIN:=$(SDK_DIR)/platforms/$(TARGET_PLATFORM)/bin
ifeq ($(COMPILE), xcc)
    AR = xt-ar
	CC = xt-xcc
	NM = xt-nm
	CPP = xt-xt++
	OBJCOPY = xt-objcopy
	OBJDUMP = xt-objdump
else
	AR = $(TOOLCHAIN)/xtensa-lx106-elf-ar
	CC = $(TOOLCHAIN)/xtensa-lx106-elf-gcc
	NM = $(TOOLCHAIN)/xtensa-lx106-elf-nm
	CPP = $(TOOLCHAIN)/xtensa-lx106-elf-g++
	OBJCOPY = $(TOOLCHAIN)/xtensa-lx106-elf-objcopy
	OBJDUMP = $(TOOLCHAIN)/xtensa-lx106-elf-objdump
endif

BOOT?=none
APP?=0
SPI_SPEED?=40
SPI_MODE?=QIO
SPI_SIZE_MAP?=0

speed_index?=0
#crystalfreq 40->0 26->1 24->2
crystalfreq?=1
disable_cfg?=0
SDK_FLASH_SIZE=
flash_bin_suffix=
ifeq ($(ESP8266_1M), 1)
    SDK_FLASH_SIZE=8Mbit
    flash_bin_suffix=_8M
else
    SDK_FLASH_SIZE=16Mbit
    flash_bin_suffix=_16M
endif

crystal_bin_suffix=
ifeq ($(CRYSTAL_FREQ), 40M)
    crystalfreq=0
	crystal_bin_suffix=_40M
endif

ifeq ($(CRYSTAL_FREQ), 26M)
    crystalfreq=1
	crystal_bin_suffix=_26M
endif

ifeq ($(CRYSTAL_FREQ), 24M)
    crystalfreq=2
	crystal_bin_suffix=_24M
endif

ifeq ($(BOOT), new)
    boot = new
else
    ifeq ($(BOOT), old)
        boot = old
    else
        boot = none
    endif
endif

ifeq ($(APP), 1)
    app = 1
else
    ifeq ($(APP), 2)
        app = 2
    else
        app = 0
    endif
endif

ifeq ($(SPI_SPEED), 26.7)
    freqdiv = 1
	speed_index = 1
else
    ifeq ($(SPI_SPEED), 20)
        freqdiv = 2
        speed_index = 2
    else
        ifeq ($(SPI_SPEED), 80)
            freqdiv = 15
            speed_index = 3
        else
            freqdiv = 0
            speed_index = 0
        endif
    endif
endif


ifeq ($(SPI_MODE), QOUT)
    mode = 1
else
    ifeq ($(SPI_MODE), DIO)
        mode = 2
    else
        ifeq ($(SPI_MODE), DOUT)
            mode = 3
        else
            mode = 0
        endif
    endif
endif

addr = 0x01000

ifeq ($(SPI_SIZE_MAP), 1)
  size_map = 1
  flash = 256
else
  ifeq ($(SPI_SIZE_MAP), 2)
    size_map = 2
    flash = 1024
    ifeq ($(app), 2)
      addr = 0x81000
    endif
  else
    ifeq ($(SPI_SIZE_MAP), 3)
      size_map = 3
      flash = 2048
      ifeq ($(app), 2)
        addr = 0x81000
      endif
    else
      ifeq ($(SPI_SIZE_MAP), 4)
        size_map = 4
        flash = 4096
        ifeq ($(app), 2)
          addr = 0x81000
        endif
      else
        ifeq ($(SPI_SIZE_MAP), 5)
          size_map = 5
          flash = 2048
          ifeq ($(app), 2)
            addr = 0x101000
          endif
        else
          ifeq ($(SPI_SIZE_MAP), 6)
            size_map = 6
            flash = 4096
            ifeq ($(app), 2)
              addr = 0x101000
            endif
          else
            size_map = 0
            flash = 512
            ifeq ($(app), 2)
              addr = 0x41000
            endif
          endif
        endif
      endif
    endif
  endif
endif

LD_FILE = $(LDDIR)/eagle.app.v6.ld

ifneq ($(boot), none)
ifneq ($(app),0)
    ifeq ($(size_map), 6)
      LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).2048.ld
    else
      ifeq ($(size_map), 5)
        LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).2048.ld
      else
        ifeq ($(size_map), 4)
          LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).1024.app$(app).ld
        else
          ifeq ($(size_map), 3)
            LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).1024.app$(app).ld
          else
            ifeq ($(size_map), 2)
              LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).1024.app$(app).ld
            else
              ifeq ($(size_map), 0)
                LD_FILE = $(LDDIR)/eagle.app.v6.$(boot).512.app$(app).ld
              endif
            endif
	      endif
	    endif
	  endif
	endif
	#BIN_NAME ?= user$(app).$(flash).$(boot).$(size_map)
	#BIN_NAME ?= $(APP_BIN_NAME)$(app).$(flash).$(boot).$(size_map)
	#BIN_NAME ?= $(APP_BIN_NAME)\($(APP)\)_$(USER_SW_VER)
	BIN_NAME ?= $(APP_BIN_NAME)\($(APP)\)_$(USER_SW_VER)
endif
else
    app = 0
endif

CSRCS ?= $(wildcard *.c)
CPPSRCS ?= $(wildcard *.cpp)
ASRCs ?= $(wildcard *.s)
ASRCS ?= $(wildcard *.S)
SUBDIRS ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))

ifeq ($(TOP_DIR),)
ODIR := .output
else
ODIR := $(TOP_DIR)/.output
endif
OBJODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/obj

OBJS := $(CSRCS:%.c=$(OBJODIR)/%.o) \
        $(CPPSRCS:%.cpp=$(OBJODIR)/%.o) \
        $(ASRCs:%.s=$(OBJODIR)/%.o) \
        $(ASRCS:%.S=$(OBJODIR)/%.o)

DEPS := $(CSRCS:%.c=$(OBJODIR)/%.d) \
        $(CPPSRCS:%.cpp=$(OBJODIR)/%.d) \
        $(ASRCs:%.s=$(OBJODIR)/%.d) \
        $(ASRCS:%.S=$(OBJODIR)/%.d)

LIBODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/lib
OLIBS := $(GEN_LIBS:%=$(LIBODIR)/%)

IMAGEODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/image
OIMAGES := $(GEN_IMAGES:%=$(IMAGEODIR)/%)

BINODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/bin
OBINS := $(GEN_BINS:%=$(BINODIR)/%)

CCFLAGS += 			\
	-g			\
	-Wpointer-arith		\
	-Wundef			\
	-Werror			\
	-Wl,-EL			\
	-fno-inline-functions	\
	-nostdlib       \
	-mlongcalls	\
	-mtext-section-literals \
	-ffunction-sections \
	-fdata-sections	\
	-fno-builtin-printf	\
	-fno-jump-tables
#	-Wall			

CFLAGS = $(CCFLAGS) $(DEFINES) $(EXTRA_CCFLAGS) $(INCLUDES)
DFLAGS = $(CCFLAGS) $(DDEFINES) $(EXTRA_CCFLAGS) $(INCLUDES)


#############################################################
# Functions
#

define ShortcutRule
$(1): .subdirs $(2)/$(1)
endef

define MakeLibrary
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(LIBODIR)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OBJODIR)/$$(notdir $$(obj)))
$$(LIBODIR)/$(1).a: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1)) $$(DEPENDS_$(1))
	@mkdir -p $$(LIBODIR)
	$$(if $$(filter %.a,$$?),mkdir -p $$(EXTRACT_DIR)_$(1))
	$$(if $$(filter %.a,$$?),cd $$(EXTRACT_DIR)_$(1); $$(foreach lib,$$(filter %.a,$$?),$$(AR) xo $$(UP_EXTRACT_DIR)/$$(lib);))
	$$(AR) ru $$@ $$(filter %.o,$$?) $$(if $$(filter %.a,$$?),$$(EXTRACT_DIR)_$(1)/*.o)
	$$(if $$(filter %.a,$$?),$$(RM) -r $$(EXTRACT_DIR)_$(1))
endef

define MakeImage
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(LIBODIR)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OBJODIR)/$$(notdir $$(obj)))
$$(IMAGEODIR)/$(1).out: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1)) $$(DEPENDS_$(1))
	@mkdir -p $$(IMAGEODIR)
	$$(CC) $$(LDFLAGS) $$(if $$(LINKFLAGS_$(1)),$$(LINKFLAGS_$(1)),$$(LINKFLAGS_DEFAULT) $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1))) -o $$@ 
endef

$(BINODIR)/%.bin: $(IMAGEODIR)/%.out
	@mkdir -p $(BINODIR)
	
ifeq ($(APP), 0)
	@$(RM) -r ../bin/eagle.S ../bin/eagle.dump
	@$(OBJDUMP) -x -s $< > ../bin/eagle.dump
	@$(OBJDUMP) -S $< > ../bin/eagle.S
else
	@mkdir -p ../bin/upgrade
	@$(RM) -r ../bin/upgrade/$(BIN_NAME).S ../bin/upgrade/$(BIN_NAME).dump
	@$(OBJDUMP) -x -s $< > ../bin/upgrade/$(BIN_NAME).dump
	@$(OBJDUMP) -S $< > ../bin/upgrade/$(BIN_NAME).S
endif

	@$(OBJCOPY) --only-section .text -O binary $< eagle.app.v6.text.bin
	@$(OBJCOPY) --only-section .data -O binary $< eagle.app.v6.data.bin
	@$(OBJCOPY) --only-section .rodata -O binary $< eagle.app.v6.rodata.bin
	@$(OBJCOPY) --only-section .irom0.text -O binary $< eagle.app.v6.irom0text.bin

	@echo ""
	@echo "!!!"
	
ifeq ($(app), 0)
	@python ../tools/gen_appbin.py $< 0 $(mode) $(freqdiv) $(size_map)
	@mv eagle.app.flash.bin ../bin/eagle.flash.bin
	@mv eagle.app.v6.irom0text.bin ../bin/eagle.irom0text.bin
	@rm eagle.app.v6.*
	@echo "No boot needed."
	@echo "Generate eagle.flash.bin and eagle.irom0text.bin successully in folder bin."
	@echo "eagle.flash.bin-------->0x00000"
	@echo "eagle.irom0text.bin---->0x40000"
else
    ifneq ($(boot), new)
		@python ../tools/gen_appbin.py $< 1 $(mode) $(freqdiv) $(size_map)
		@echo "Support boot_v1.1 and +"
    else
		@python ../tools/gen_appbin.py $< 2 $(mode) $(freqdiv) $(size_map)

    	ifeq ($(size_map), 6)
		@echo "Support boot_v1.4 and +"
        else
            ifeq ($(size_map), 5)
		@echo "Support boot_v1.4 and +"
            else
		@echo "Support boot_v1.2 and +"
            endif
        endif
    endif

	@mv eagle.app.flash.bin ../bin/upgrade/$(BIN_NAME).bin
	@rm eagle.app.v6.*
	@echo "Generate $(BIN_NAME).bin successully in folder bin/upgrade."
	@echo "boot.bin------------>0x00000"
	@echo "$(BIN_NAME).bin--->$(addr)"
endif

ifeq ($(app), 2)
	@echo "package start..."
	@cd ../bin/upgrade/;\
		./package $(APP_BIN_NAME)\(1\)_$(USER_SW_VER).bin \$(APP_BIN_NAME)\(2\)_$(USER_SW_VER).bin $(APP_BIN_NAME)_ug_$(USER_SW_VER).bin;\
	if [ $$? = "0" ];\
	then \
		echo "Generate $(APP_BIN_NAME)_ug_$(USER_SW_VER).bin package in folder bin/upgrade success"; \
	else \
		echo "package error"; \
		exit 1; \
	fi
#以下使用python自动打包合成生产固件，APP_BIN_NAME/USER_SW_VER/target_file
	-rm -rf  ../tools/combine_bin.pyc
	@echo "build QIO"
	python ../tools/makecombine.py ../bin/boot_v1.7.bin    ../bin/upgrade/$(APP_BIN_NAME)\(1\)_$(USER_SW_VER).bin  ../bin/esp_init_data_default.bin  ../bin/blank.bin   ../bin/upgrade/$(APP_BIN_NAME)$(flash_bin_suffix)_QIO_TLS_$(USER_SW_VER).bin 0 $(speed_index) $(size_map) $(crystalfreq) $(disable_cfg)
	@echo "build DOUT"
	python ../tools/makecombine.py ../bin/boot_v1.7.bin    ../bin/upgrade/$(APP_BIN_NAME)\(1\)_$(USER_SW_VER).bin  ../bin/esp_init_data_default.bin  ../bin/blank.bin   ../bin/upgrade/$(APP_BIN_NAME)$(flash_bin_suffix)_DOUT_TLS_$(USER_SW_VER).bin 3 $(speed_index) $(size_map) $(crystalfreq) $(disable_cfg)


	mkdir -p ../bin/upgrade/$(APP_BIN_NAME)/$(USER_SW_VER)
	mv -f  ../bin/upgrade/$(APP_BIN_NAME)\(1\)_$(USER_SW_VER).bin  ../bin/upgrade/$(APP_BIN_NAME)/$(USER_SW_VER)/$(APP_BIN_NAME)$(flash_bin_suffix)_UA_TLS_$(USER_SW_VER).bin
	mv -f  ../bin/upgrade/$(APP_BIN_NAME)_ug_$(USER_SW_VER).bin  ../bin/upgrade/$(APP_BIN_NAME)/$(USER_SW_VER)/$(APP_BIN_NAME)$(flash_bin_suffix)_UG_TLS_$(USER_SW_VER).bin	
	mv -f  ../bin/upgrade/$(APP_BIN_NAME)$(flash_bin_suffix)_QIO_TLS_$(USER_SW_VER).bin  ../bin/upgrade/$(APP_BIN_NAME)/$(USER_SW_VER)/
	mv -f  ../bin/upgrade/$(APP_BIN_NAME)$(flash_bin_suffix)_DOUT_TLS_$(USER_SW_VER).bin  ../bin/upgrade/$(APP_BIN_NAME)/$(USER_SW_VER)/

	@echo "target file"
	mkdir -p $(CI_PACKAGE_PATH)
	cp -f  ../bin/upgrade/$(APP_BIN_NAME)/$(USER_SW_VER)/* $(CI_PACKAGE_PATH)


endif

	@echo "!!!"

#############################################################
# Rules base
# Should be done in top-level makefile only
#

all:	.subdirs $(OBJS) $(OLIBS) $(OIMAGES) $(OBINS) $(SPECIAL_MKTARGETS)

clean:
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clean;)
	$(RM) -r $(ODIR)/$(TARGET)/$(FLAVOR)

clobber: $(SPECIAL_CLOBBER)
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clobber;)
	$(RM) -r $(ODIR)

.subdirs:
	set -e; $(foreach d, $(SUBDIRS), $(MAKE) -C $(d);)

#.subdirs:
#	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d))

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
ifdef DEPS
sinclude $(DEPS)
endif
endif
endif

$(OBJODIR)/%.o: %.c
	@mkdir -p $(dir $@);
	$(CC) $(if $(findstring $<,$(DSRCS)),$(DFLAGS),$(CFLAGS)) $(COPTS_$(*F)) -o $@ -c $<

$(OBJODIR)/%.d: %.c
	@mkdir -p $(dir $@);
	@echo DEPEND: $(CC) -M $(CFLAGS) $<
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
$(OBJODIR)/%.o: %.cpp
	@mkdir -p $(dir $@);
	$(CPP) $(if $(findstring $<,$(DSRCS)),$(DFLAGS),$(CFLAGS)) $(COPTS_$(*F)) -o $@ -c $<

$(OBJODIR)/%.d: %.cpp
	@mkdir -p $(dir $@);
	@echo DEPEND: $(CPP) -M $(CFLAGS) $<
	@set -e; rm -f $@; \
	$(CPP) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJODIR)/%.o: %.s
	@mkdir -p $(dir $@);
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJODIR)/%.d: %.s
	@mkdir -p $(dir $@); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJODIR)/%.o: %.S
	@mkdir -p $(dir $@);
	$(CC) $(CFLAGS) -D__ASSEMBLER__ -o $@ -c $<

$(OBJODIR)/%.d: %.S
	@mkdir -p $(dir $@); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(foreach lib,$(GEN_LIBS),$(eval $(call ShortcutRule,$(lib),$(LIBODIR))))

$(foreach image,$(GEN_IMAGES),$(eval $(call ShortcutRule,$(image),$(IMAGEODIR))))

$(foreach bin,$(GEN_BINS),$(eval $(call ShortcutRule,$(bin),$(BINODIR))))

$(foreach lib,$(GEN_LIBS),$(eval $(call MakeLibrary,$(basename $(lib)))))

$(foreach image,$(GEN_IMAGES),$(eval $(call MakeImage,$(basename $(image)))))

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

INCLUDES := $(INCLUDES) 
INCLUDES += -I $(PDIR)sdk/esp8266/fac_include/lwip
INCLUDES += -I $(PDIR)sdk/esp8266/fac_include/lwip/ipv4
INCLUDES += -I $(PDIR)sdk/esp8266/fac_include/lwip/ipv6
INCLUDES += -I $(PDIR)sdk/esp8266/fac_include/espressif
INCLUDES += -I $(PDIR)sdk/esp8266/fac_include
INCLUDES += -I $(PDIR)sdk/esp8266/tuya_include/include/
INCLUDES += -I $(PDIR)sdk/esp8266/tuya_include

PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile