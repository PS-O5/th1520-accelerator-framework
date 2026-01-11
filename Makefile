# --- Toolchain ---
CROSS_COMPILE ?= riscv64-unknown-elf-
CC      = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE    = $(CROSS_COMPILE)size

# --- Default App ---
APP ?= hello_boot

# --- Build Directory ---
# We put objects in build/APP_NAME to avoid conflicts
BUILD_ROOT = build
BUILD_DIR  = $(BUILD_ROOT)/$(APP)
TARGET_ELF = $(BUILD_DIR)/$(APP).elf
TARGET_BIN = $(BUILD_DIR)/$(APP).bin
TARGET_MAP = $(BUILD_DIR)/$(APP).map

# --- App Specific Configurations ---

# Config for: hello_boot
ifeq ($(APP),hello_boot)
    # Standard C910 (GC extensions)
    ARCH_FLAGS = -march=rv64gc -mabi=lp64d -mcmodel=medany
    EXTRA_CFLAGS = 
endif

# Config for: rvv_test
ifeq ($(APP),rvv_test)
    # Enable Vectors (v0p7) and define the macro for startup.S
    # Note: If -march=rv64gcv fails, use -march=rv64gcv0p7 ???<- need to confirm
    ARCH_FLAGS = -march=rv64gcv0p7 -mabi=lp64d -mcmodel=medany
    EXTRA_CFLAGS = -DENABLE_VECTORS
endif

# --- Directories ---
FREERTOS_DIR = freertos/FreeRTOS-Kernel
PORT_DIR     = $(FREERTOS_DIR)/portable/GCC/RISC-V
PLATFORM_DIR = platform/c910
APP_SRC_DIR  = app/$(APP)

# --- Includes ---
INCLUDES  = -I$(APP_SRC_DIR) -Iplatform -I$(PLATFORM_DIR)

# --- Source Files ---
# Common Platform Sources
SRCS_C   = $(PLATFORM_DIR)/uart.c
SRCS_ASM = $(PLATFORM_DIR)/startup.S

# App Specific Main
SRCS_C  += $(APP_SRC_DIR)/main.c

# --- Flags ---
CFLAGS  = $(ARCH_FLAGS) $(INCLUDES) -O2 -g -Wall -Wextra $(EXTRA_CFLAGS)
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS = $(ARCH_FLAGS) -T $(PLATFORM_DIR)/linker.ld
LDFLAGS += -Wl,--gc-sections -nostartfiles -Wl,-Map=$(TARGET_MAP)

# --- Objects ---
OBJS  = $(SRCS_C:%.c=$(BUILD_DIR)/%.o)
OBJS += $(SRCS_ASM:%.S=$(BUILD_DIR)/%.o)

# --- Convenience Targets ---
.PHONY: all clean hello_boot rvv_test

# Default 'make' builds the variable $(APP)
all: $(TARGET_BIN)

# Shortcut: 'make hello_boot'
hello_boot:
	@$(MAKE) --no-print-directory APP=hello_boot

# Shortcut: 'make rvv_test'
rvv_test:
	@$(MAKE) --no-print-directory APP=rvv_test

# --- Build Rules ---

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@
	@echo "-------------------------------------------------------"
	@echo " Build Success: $@"
	@echo "-------------------------------------------------------"

$(TARGET_ELF): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c $< -o $@

clean:
	rm -rf $(BUILD_ROOT)
