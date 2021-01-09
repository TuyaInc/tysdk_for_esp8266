ARCH ?= X86
ifeq ($(strip $(ARCH)),X86)
GCC ?= gcc
GXX ?= g++ 
STRIP ?= strip
AR ?= ar
RANLIB ?= ranlib
endif

OUTPUT ?= .
INSALL ?= .
TARGET_APP ?= package
USER_SW_VER ?= 1.0.0
ESP8266_1M ?= 1
SRCS = $(wildcard *.c)
OBJS = $(foreach src, $(SRCS), $(OUTPUT)/$(basename $(src)).o )

DEBUG = 0
CFLAGS += -Wall -DUSER_SW_VER=\"$(USER_SW_VER)\" -DDEBUG=$(DEBUG) -DESP8266_1M=$(ESP8266_1M)
CFLAGS += -I ../../../../../sdk/esp8266/tuya_include

.PHONY : all clean install

all:$(OUTPUT)/$(TARGET_APP)

$(OUTPUT)/%.o: %.c
	@mkdir -p $(OUTPUT)
	$(GCC) $(CFLAGS) -o $@ -c $< 
	
$(OUTPUT)/$(TARGET_APP):$(OBJS)
	$(GCC) $(CFLAGS) $< -o $@ 

clean:
	@rm -fr $(OUTPUT)/*.o
	@rm -fr $(OUTPUT)/$(TARGET_APP)

install: $(OUTPUT)/$(TARGET_APP)
	@mkdir -p $(INSALL)
	@cp -f $(OUTPUT)/$(TARGET_APP) $(INSALL)/$(TARGET_APP)