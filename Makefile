# --- Toolchain ---
# Ensure CROSS_COMPILE is set in environment or default it here
CROSS_COMPILE ?= riscv64-unknown-elf-
CC      = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE    = $(CROSS_COMPILE)size

# --- Project Configuration ---
BUILD_DIR = build
# Default application if none specified
APP       ?= hello_boot

# --- Architecture Flags for C910 (RV64 + Vector 0.7.1) ---
# Note: Adjust -march depending on exact toolchain version support for v0.7.1
# Xuantie gcc typically uses -mcpu=c910 which implies the correct march/mabi
ARCH_FLAGS = -march=rv64gc -mabi=lp64d -mcmodel=medany

# --- Directories ---
FREERTOS_DIR = freertos/FreeRTOS-Kernel
PORT_DIR     = $(FREERTOS_DIR)/portable/GCC/RISC-V
PLATFORM_DIR = platform/c910

# Dynamic App Directory based on selection
APP_SRC_DIR  = app/$(APP)

# Output definitions
TARGET_ELF = $(BUILD_DIR)/$(APP).elf
TARGET_BIN = $(BUILD_DIR)/$(APP).bin
TARGET_MAP = $(BUILD_DIR)/$(APP).map

# --- Includes ---
INCLUDES  = -I$(APP_SRC_DIR)
INCLUDES += -Iplatform
INCLUDES += -I$(PLATFORM_DIR)
INCLUDES += -I$(FREERTOS_DIR)/include
INCLUDES += -I$(PORT_DIR)
# Only include this if you actually use the chip-specific extensions header
# INCLUDES += -I$(PORT_DIR)/chip_specific_extensions/RV64I_CLINT_no_extensions

# --- Source Files ---

# 1. Platform & Drivers (Common to all apps)
#    We exclude startup.S here because we handle assembly separately below
SRCS_C += $(PLATFORM_DIR)/uart.c
# SRCS_C += $(PLATFORM_DIR)/system.c   <-- Add back when you implement system init
# SRCS_C += $(PLATFORM_DIR)/irq.c      <-- Add back when ready for interrupts
# SRCS_C += $(PLATFORM_DIR)/timer.c    <-- Add back when ready for FreeRTOS timer

# 2. FreeRTOS Kernel (Common)
#    (Uncomment these when you are ready to link the scheduler. 
#     For "Hello Boot", we usually keep it simple first.)
# SRCS_C += $(FREERTOS_DIR)/tasks.c
# SRCS_C += $(FREERTOS_DIR)/queue.c
# SRCS_C += $(FREERTOS_DIR)/list.c
# SRCS_C += $(FREERTOS_DIR)/portable/MemMang/heap_4.c
# SRCS_C += $(PORT_DIR)/port.c

# 3. Application Specific Source
SRCS_C += $(APP_SRC_DIR)/main.c

# 4. Assembly Sources
SRCS_ASM += $(PLATFORM_DIR)/startup.S
# SRCS_ASM += $(PORT_DIR)/portASM.S    <-- Add back for FreeRTOS

# --- Compiler Flags ---
CFLAGS  = $(ARCH_FLAGS) $(INCLUDES) -O2 -g -Wall -Wextra
CFLAGS += -ffunction-sections -fdata-sections
# CFLAGS += -DportasmHANDLE_INTERRUPT=external_interrupt_handler

# --- Linker Flags ---
LDFLAGS = $(ARCH_FLAGS) -T $(PLATFORM_DIR)/linker.ld
LDFLAGS += -Wl,--gc-sections -nostartfiles -Wl,-Map=$(TARGET_MAP)

# --- Object File Generation ---
OBJS  = $(SRCS_C:%.c=$(BUILD_DIR)/%.o)
OBJS += $(SRCS_ASM:%.S=$(BUILD_DIR)/%.o)

# --- Build Rules ---

.PHONY: all clean info

all: info $(TARGET_ELF) bin

# Print what we are building
info:
	@echo "========================================"
	@echo " Building APP: $(APP)"
	@echo " Source:       $(APP_SRC_DIR)"
	@echo " Target:       $(TARGET_ELF)"
	@echo "========================================"

$(TARGET_ELF): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

bin: $(TARGET_ELF)
	$(OBJCOPY) -O binary $(TARGET_ELF) $(TARGET_BIN)
	@echo "Binary generated: $(TARGET_BIN)"

# Compile C
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Assembly
$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
