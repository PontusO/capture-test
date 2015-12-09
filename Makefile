CC=arm-none-eabi-gcc
RM=rm -rf
CFLAGS=-mthumb -mcpu=cortex-m0plus -Wall -c -g -Os -w -std=gnu99 -ffunction-sections \
		-fdata-sections -nostdlib --param max-inline-insns-single=500
LDFLAGS=-mthumb -mcpu=cortex-m0plus -Wall -Wl,--cref -Wl,--check-sections -Wl,--gc-sections \
		-Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align \
		-Wl,--warn-unresolved-symbols -Wl,-Map=Map.map
#BLD_EXTA_FLAGS=-D__SAMD21E18A__
BLD_EXTA_FLAGS=-D__SAMD11D14AM__
BUILD_PATH=build
INCLUDES=-I CMSIS/CMSIS/Include/ -I CMSIS/Device/ATMEL/ -I./drivers/ -I./utils/ \
		-I./utils/preprocessor/ -I./utils/interrupt 
SOURCES=main.c startup_samd21.c drivers/uart_driver.c utils/interrupt/interrupt_sam_nvic.c 
OBJECTS=$(addprefix $(BUILD_PATH)/, $(SOURCES:.c=.o))

NAME=samd11_capture_test
EXECUTABLE=$(NAME).bin

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -L$(BUILD_PATH) $(LDFLAGS) -Os -Wl,--gc-sections -save-temps  -Tsamd11d14am_flash.ld \
	-Wl,-Map,$(BUILD_PATH)/$(NAME).map --specs=nano.specs --specs=nosys.specs \
	-o $(BUILD_PATH)/$(NAME).elf $(OBJECTS) -Wl,--start-group -lm -Wl,--end-group
	arm-none-eabi-objcopy -O binary $(BUILD_PATH)/$(NAME).elf $@

$(BUILD_PATH)/%.o: %.c
	-@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(BLD_EXTA_FLAGS) $(INCLUDES) $< -o $@
	
clean:
	$(RM) $(EXECUTABLE) $(BUILD_PATH)
