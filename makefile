
#Written by Luan Cardoso dos Santos. May 2019 - Luxembourg
#
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <https://www.gnu.org/licenses/>.



#
# ARM make file variables
#


# System directory code path
SYSTEM_DIR := ./ARMsources

CC := arm-none-eabi-gcc
OBJDUMP := arm-none-eabi-objdump
OBJCOPY := arm-none-eabi-objcopy
BOSSAC := bossac

ARM_SERIAL_TERMINAL := $(SCRYPT_DIR)/arm_serial_terminal.py

PORT := ttyACM0 #Usual arduino due UART port
DEVICE := /dev/$(PORT)

#define the GCC optimization flag here.
OFLAG := -O0

DEF := -DOFLAG=$(OFLAG)

#	-g3 -ggdb
CFLAGS := $(OFLAG) \
	-save-temps \
	-std=gnu99 \
	-mcpu=cortex-m3 \
	-D__SAM3X8E__ \
	-march=armv7-m \
	-mthumb \
	-fdata-sections \
	-ffunction-sections \
	-fstack-usage \
	-Wcast-align \
	-Wsign-compare \
	-Waggregate-return \
	-Wunused \
	-Wfatal-errors \
	-Wa,-mimplicit-it=thumb \
	-Wa,-EL \
	-fno-exceptions \
	-I$(SYSTEM_DIR)/include

LDFLAGS := \
	-mcpu=cortex-m3 \
	-march=armv7-m \
	-mthumb \
	-T$(SYSTEM_DIR)/flash.ld \
	-L$(SYSTEM_DIR) \
	-Wl,--gc-sections \
	-Wl,--relax \
	-Wl,--entry=Reset_Handler

LDLIBS := -lsam3x

OBJDUMPFLAGS := -dSt

#put here the extra objects you wanna compile, like util.o
OBJS =

INCLUDES = -I./ -I./ARMsources/include

all: main.elf

main.bin : main.elf
	$(OBJCOPY) -O binary $< $@

.PHONY : upload
upload : main.bin
	@# Communicate with the board @1200 Bd resets everything
	@stty -F $(DEVICE) cs8 1200 hupcl
	@# Use bossac to load program in flash
	@$(BOSSAC) --port=$(PORT) -U false -e -w -b $< -R #-d -i -v

main.elf : $(OBJS) main.o
	$(CC) $(LDFLAGS) $(INCLUDES) $(OBJS) $(DEF) main.o $(LDLIBS) -o $@

#rule for generating any .o via a .c file
%.o : %.c
	$(CC) -c $(CFLAGS) $(DEF) $< $(INCLUDES) -o $@
	$(OBJDUMP) -dSt $@ > $@.lst

# Run the program stored in the flash memory of the board. S
# Note that the binary should be uploaded first.
.PHONY : run
run :
	@./$(ARM_SERIAL_TERMINAL)

#carefull if you use .s files. I recomend using .S
clean :
	@echo $(DELIMITER) Begin cleaning: $(CIPHERNAME) $(DELIMITER)
	rm -f *~
	rm -f *.elf
	rm -f *.bin
	rm -f *.lst
	rm -f *.o
	rm -f *.s
	rm -f *.su
	rm -f *.i
