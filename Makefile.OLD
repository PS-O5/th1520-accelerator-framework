# --- Toolchain ---
CC      = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE    = $(CROSS_COMPILE)size

# --- Project Configuration ---
BUILD_DIR = build
TARGET    = $(BUILD_DIR)/firmware.elf

# --- Architecture Flags for C910 (RV64 + Vector 0.7.1) ---
# Note: Adjust -march depending on exact toolchain version support for v0.7.1
ARCH_FLAGS = -march=rv64gcv0p7 -mabi=lp64d -mcmodel=medany

# --- Source Directories ---
FREERTOS_DIR = freertos/FreeRTOS-Kernel
PORT_DIR     = $(FREERTOS_DIR)/portable/GCC/RISC-V
PLATFORM_DIR = platform/c910
APP_DIR      = app

# --- Includes ---
INCLUDES  = -I$(APP_DIR)
INCLUDES += -I$(PLATFORM_DIR)
INCLUDES += -I$(FREERTOS_DIR)/include
INCLUDES += -I$(PORT_DIR)
INCLUDES += -I$(PORT_DIR)/chip_specific_extensions/RV64I_CLINT_no_extensions

# --- Source Files ---
# Kernel
SRCS += $(FREERTOS_DIR)/tasks.c
SRCS += $(FREERTOS_DIR)/queue.c
SRCS += $(FREERTOS_DIR)/list.c
SRCS += $(FREERTOS_DIR)/portable/MemMang/heap_4.c
SRCS += $(PORT_DIR)/port.c

# Platform
SRCS += $(PLATFORM_DIR)/startup.S
SRCS += $(PLATFORM_DIR)/irq.c
SRCS += $(PLATFORM_DIR)/timer.c
SRCS += $(PLATFORM_DIR)/uart.c
SRCS += $(PORT_DIR)/portASM.S
SRCS += $(PLATFORM_DIR)/vtimer.c

# App
SRCS += $(APP_DIR)/main.c

# --- Compiler Flags ---
CFLAGS  = $(ARCH_FLAGS) $(INCLUDES) -O2 -g -Wall -Wextra
CFLAGS += -DportasmHANDLE_INTERRUPT=external_interrupt_handler
CFLAGS += -ffunction-sections -fdata-sections

# --- Linker Flags ---
LDFLAGS = $(ARCH_FLAGS) -T $(PLATFORM_DIR)/linker.ld
LDFLAGS += -Wl,--gc-sections -nostartfiles -Wl,-Map=$(BUILD_DIR)/firmware.map

# --- Build Rules ---
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.S.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
