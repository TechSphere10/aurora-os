# AuroraOS Makefile

CC = gcc
LD = ld
ASM = nasm
QEMU = qemu-system-x86_64

BUILD_DIR = build
SRC_DIR = src

BOOTLOADER_SRC = $(SRC_DIR)/bootloader/bootloader.asm
KERNEL_SRC = $(SRC_DIR)/kernel/kernel.c
KEYBOARD_SRC = $(SRC_DIR)/kernel/keyboard.c
MEMORY_SRC = $(SRC_DIR)/kernel/memory.c
VFS_SRC = $(SRC_DIR)/kernel/vfs.c
SCHEDULER_SRC = $(SRC_DIR)/kernel/scheduler.c
DESKTOP_SRC = $(SRC_DIR)/kernel/desktop.c
SERVICES_SRC = $(SRC_DIR)/kernel/services.c
PACKAGES_SRC = $(SRC_DIR)/kernel/packages.c
SHELL_SRC = $(SRC_DIR)/shell/shell.c
AURORALANG_SRC = $(SRC_DIR)/auroralang/auroralang.c
AI_ASSISTANT_SRC = $(SRC_DIR)/auroralang/ai_assistant.c

BOOTLOADER_BIN = $(BUILD_DIR)/bootloader.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
KEYBOARD_OBJ = $(BUILD_DIR)/keyboard.o
MEMORY_OBJ = $(BUILD_DIR)/memory.o
VFS_OBJ = $(BUILD_DIR)/vfs.o
SCHEDULER_OBJ = $(BUILD_DIR)/scheduler.o
DESKTOP_OBJ = $(BUILD_DIR)/desktop.o
SERVICES_OBJ = $(BUILD_DIR)/services.o
PACKAGES_OBJ = $(BUILD_DIR)/packages.o
SHELL_OBJ = $(BUILD_DIR)/shell.o
AURORALANG_OBJ = $(BUILD_DIR)/auroralang.o
AI_ASSISTANT_OBJ = $(BUILD_DIR)/ai_assistant.o
IMAGE = $(BUILD_DIR)/auroraos.img

LINKER_SCRIPT = $(SRC_DIR)/kernel/kernel.ld

CFLAGS = -ffreestanding -c
LDFLAGS = -T $(LINKER_SCRIPT)

all: $(IMAGE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BOOTLOADER_BIN): $(BOOTLOADER_SRC) | $(BUILD_DIR)
	$(ASM) -f bin $< -o $@

$(KERNEL_OBJ): $(KERNEL_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(KEYBOARD_OBJ): $(KEYBOARD_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(MEMORY_OBJ): $(MEMORY_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(VFS_OBJ): $(VFS_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(SCHEDULER_OBJ): $(SCHEDULER_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(DESKTOP_OBJ): $(DESKTOP_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(SERVICES_OBJ): $(SERVICES_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(PACKAGES_OBJ): $(PACKAGES_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(SHELL_OBJ): $(SHELL_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(AURORALANG_OBJ): $(AURORALANG_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(AI_ASSISTANT_OBJ): $(AI_ASSISTANT_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_BIN): $(KERNEL_OBJ) $(KEYBOARD_OBJ) $(MEMORY_OBJ) $(VFS_OBJ) $(SCHEDULER_OBJ) $(DESKTOP_OBJ) $(SERVICES_OBJ) $(PACKAGES_OBJ) $(SHELL_OBJ) $(AURORALANG_OBJ) $(AI_ASSISTANT_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	cat $^ > $@

run: $(IMAGE)
	$(QEMU) $<

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean