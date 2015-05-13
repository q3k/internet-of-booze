TARGET=main.bin
EXECUTABLE=main.elf

CC=arm-none-eabi-gcc
CXX=arm-none-eabi-g++
LD=arm-none-eabi-ld 
#LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
CP=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump

BIN=$(CP) -O ihex 

DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -DHSE_VALUE=8000000

MCU = cortex-m4
MCFLAGS = -mcpu=$(MCU) -mthumb -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork
STM32_INCLUDES = -I include/ -I include/cmsis/ -I include/freertos/ -I include/stdperiph/ -I include/project/
	
OPTIMIZE       = -Os

CFLAGS	= $(MCFLAGS)  $(OPTIMIZE)  $(DEFS) -I./ -I./ $(STM32_INCLUDES)  -Wl,-T,src/stm32_flash.ld -std=c99
AFLAGS	= $(MCFLAGS) 
#-mapcs-float use float regs. small increase in code size

$(TARGET): $(EXECUTABLE)
	$(CP) -O binary $^ $@

OBJS = src/stm32f10x_it.o src/startup_stm32f10x_md.o \
	src/project/main.o \
	src/cmsis/core_cm3.o src/cmsis/system_stm32f10x.o \
	src/stdperiph/misc.o src/stdperiph/stm32f10x_gpio.o src/stdperiph/stm32f10x_rcc.o \
	src/freertos/list.o src/freertos/port.o src/freertos/queue.o src/freertos/tasks.o src/freertos/timers.o \
	src/freertos/heap_2.o


$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ 

flash: $(TARGET)
	sudo openocd -f swd.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "init" -c "reset halt" -c "flash write_image erase main.bin 0x08000000" -c "reset" -c "exit" || true
	#sudo openocd -f ~/Embedded/busblaster.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "init" -c "reset halt" -c "mt_flash main.elf" -c "reset" -c "exit" || true

clean:
	rm -f Startup.lst  $(OBJS) $(TARGET)  $(TARGET).lst $(TARGET).out  $(TARGET).hex  $(TARGET).map \
	 $(TARGET).dmp  $(TARGET).elf

all: $(TARGET)
