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

DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F10X_LD_VL -DHSE_VALUE=8000000

MCU = cortex-m4
MCFLAGS = -mcpu=$(MCU) -mthumb -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork
STM32_INCLUDES = -I STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc/ \
	-I STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/ \
	-I STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport/ \
	-I FreeRTOS/include
	
OPTIMIZE       = -Os

CFLAGS	= $(MCFLAGS)  $(OPTIMIZE)  $(DEFS) -I./ -I./ $(STM32_INCLUDES)  -Wl,-T,stm32_flash.ld -std=c99
CXXFLAGS = $(MCFLAGS)  $(OPTIMIZE)  $(DEFS) -IuSTL/ -I./ -I./ $(STM32_INCLUDES)  -Wl,-T,stm32_flash.ld -std=c++0x -nostdinc++
AFLAGS	= $(MCFLAGS) 
#-mapcs-float use float regs. small increase in code size

$(TARGET): $(EXECUTABLE)
	$(CP) -O binary $^ $@

OBJS = main.o \
	stm32f10x_it.o system_stm32f10x.o \
	startup_stm32f10x_ld_vl.o \
	FreeRTOS/list.o FreeRTOS/port.o FreeRTOS/queue.o FreeRTOS/tasks.o FreeRTOS/timers.o FreeRTOS/heap_2.o \
	stm32f10x_gpio.o stm32f10x_rcc.o \
	misc.o
#	stm32f10x_exti.o stm32f10x_usart.o stm32f10x_i2c.o stm32f10x_spi.o \
#	stm32f10x_adc.o stm32f10x_dma.o stm32f10x_flash.o stm32f10x_tim.o \
#	stm32f10x_dac.o stm32f10x_sdio.o stm32f10x_fsmc.o \


$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ 

flash: $(TARGET)
	sudo openocd -f ~/Embedded/busblaster.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "init" -c "reset halt" -c "flash write_image erase main.bin 0x08000000" -c "reset" -c "exit" || true
	#sudo openocd -f ~/Embedded/busblaster.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "init" -c "reset halt" -c "mt_flash main.elf" -c "reset" -c "exit" || true

clean:
	rm -f Startup.lst  $(OBJS) $(TARGET)  $(TARGET).lst $(TARGET).out  $(TARGET).hex  $(TARGET).map \
	 $(TARGET).dmp  $(TARGET).elf

all: $(TARGET)
