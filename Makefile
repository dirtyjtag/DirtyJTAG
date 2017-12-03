PLATFORM = bluepill

OBJS := src/dirtyjtag.o src/jtag.o src/usb.o src/delay.o src/cmd.o

PREFIX ?= arm-none-eabi
TARGETS := stm32/f1
DEFS += -DSTM32F1
ARCH_FLAGS := -mthumb -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd
LD_SCRIPT := dirtyjtag.ld

UCMX_DIR := $(realpath unicore-mx)
UCMX_INCLUDE_DIR := $(UCMX_DIR)/include
UCMX_LIB_DIR := $(UCMX_DIR)/lib

CFLAGS = -g
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -fno-common -ffunction-sections -fdata-sections
CFLAGS += -std=gnu11
CFLAGS += -DPLATFORM='HW_$(PLATFORM)'

CPPFLAGS = -MD -g
CPPFLAGS += -Wall -Wundef
CPPFLAGS += -I$(UCMX_INCLUDE_DIR) $(DEFS)

LDFLAGS	+= --static -nostartfiles
LDFLAGS += -L"$(UCMX_LIB_DIR)"
LDFLAGS	+= -T$(LD_SCRIPT)
LDFLAGS	+= -Wl,-Map=$(*).map
LDFLAGS	+= -Wl,--gc-sections # Remove deadcode

LDLIBS	+= -lucmx_stm32f1
LDLIBS	+= -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

CC := $(PREFIX)-gcc
LD := $(PREFIX)-gcc
AR := $(PREFIX)-ar
AS := $(PREFIX)-as
OBJCOPY := $(PREFIX)-objcopy

all: dirtyjtag

clean: dirtyjtag-clean ucmx-clean

dirtyjtag: ucmx src/dirtyjtag.$(PLATFORM).elf src/dirtyjtag.$(PLATFORM).bin

dirtyjtag-clean:
	$(Q)$(RM) src/*.d src/*.o src/*.map src/*.bin src/*.elf

ucmx:
	$(Q)$(MAKE) -C $(UCMX_DIR)

ucmx-clean:
	$(Q)$(MAKE) -C $(UCMX_DIR) clean

%.bin: %.elf
	$(Q)$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.elf %.map: $(OBJS) $(LD_SCRIPT)
	$(Q)$(LD) $(LDFLAGS) $(ARCH_FLAGS) $(OBJS) $(LDLIBS) -o $(*).elf

%.o: %.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $@ -c $<

.PHONY: clean dirtyjtag dirtyjtag-clean ucmx ucmx-clean
