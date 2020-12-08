CROSS   ?= arm-none-eabi-
OBJS_BLINKY = blinky.o systick.o

###############################################################################

CC       = $(CROSS)gcc
LD       = $(CROSS)ld
OBJCOPY  = $(CROSS)objcopy

CFLAGS  += -O3 -Wall -g -std=gnu99
CFLAGS  += -DGD32F1X0 -mthumb -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd
CFLAGS  += $(ARCH_FLAGS) -Ilibopencm3/include/ $(EXTRA_CFLAGS)

LIBC     = $(shell $(CC) $(CFLAGS) --print-file-name=libc.a)
LIBGCC   = $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
LIBOPENCM3 = libopencm3/lib/libopencm3_gd32f1x0.a
OPENCM3_MK = lib/gd32/f1x0

# LDPATH is required for libopencm3 ld scripts to work.
LDPATH   = libopencm3/lib/
LDSCRIPT_F1X0X4 = libopencm3/lib/gd32/f1x0/gd32f1x0x4.ld
LDFLAGS += -L$(LDPATH) --gc-sections
LDFLAGS_F1X0X4 = -T$(LDSCRIPT_F1X0X4)
LDLIBS  += $(LIBOPENCM3) $(LIBC) $(LIBGCC)

all: $(LIBOPENCM3) all_elfs all_bins all_hexs

all_elfs: blinky_f1x0x4.elf
all_bins: blinky_f1x0x4.bin
all_hexs: blinky_f1x0x4.hex

blinky_f1x0x4.elf: $(LDSCRIPT_F1X0X4) $(OBJS_BLINKY) $(LIBOPENCM3)
	$(LD) -o $@ $(LDFLAGS) $(LDFLAGS_F1X0X4) $(OBJS_BLINKY) $(LDLIBS)

%.hex: %.elf
	$(OBJCOPY) -S -O ihex   $< $@

%.bin: %.elf
	$(OBJCOPY) -S -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

blinky.o: blinky.c systick.h
systick.o: systick.c systick.h

$(LIBOPENCM3):
	CFLAGS="$(CFLAGS)" ${MAKE} -C libopencm3 $(OPENCM3_MK) V=1

.PHONY: clean

clean:
	rm -f *.o *.elf *.bin *.hex *.map
