# AuroraOS Makefile
# A more scalable and professional build system

CC = gcc
LD = ld
ASM = nasm
QEMU = qemu-system-x86_64

BUILD_DIR := build
SRC_DIR := src

# --- Source Files ---
# Automatically find all .c and .asm files
ASM_SOURCES := $(wildcard $(SRC_DIR)/**/*.asm)
C_SOURCES   := $(wildcard $(SRC_DIR)/**/*.c)

# --- Object Files ---
# Generate object file paths from source file paths
BOOTLOADER_BIN := $(patsubst $(SRC_DIR)/bootloader/%.asm, $(BUILD_DIR)/%.bin, $(filter %bootloader.asm, $(ASM_SOURCES)))
OBJECTS        := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

# --- Output Files ---
KERNEL_BIN := $(BUILD_DIR)/kernel.bin
IMAGE      := $(BUILD_DIR)/auroraos.img

# --- Build Flags ---
LINKER_SCRIPT := $(SRC_DIR)/kernel/kernel.ld
CFLAGS        := -ffreestanding -c -I$(SRC_DIR) -Wall -Wextra
LDFLAGS       := -T $(LINKER_SCRIPT)
ASMFLAGS      := -f bin

# =============================================================================
# Main Targets
# =============================================================================
all: $(IMAGE)

$(IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	@echo "==> Creating OS Image: $@"
	@cat $^ > $@

$(KERNEL_BIN): $(OBJECTS) $(LINKER_SCRIPT)
	@echo "==> Linking Kernel: $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

run: $(IMAGE)
	@echo "==> Booting AuroraOS in QEMU..."
	@$(QEMU) $<

clean:
	@echo "==> Cleaning build files..."
	@rm -rf $(BUILD_DIR)

.PHONY: all run clean

# =============================================================================
# Pattern Rules (The magic that makes this scalable)
# =============================================================================

# Rule to compile the bootloader
$(BUILD_DIR)/%.bin: $(SRC_DIR)/bootloader/%.asm
	@echo "==> Assembling: $<"
	@mkdir -p $(dir $@)
	@$(ASM) $(ASMFLAGS) $< -o $@

# Generic rule to compile any .c file from src/ to an .o file in build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "==> Compiling: $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@