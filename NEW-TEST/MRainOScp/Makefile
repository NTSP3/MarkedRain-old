# --- Compilers --- #
CASM=i686-elf-as    # Assembler that comes with gcc
ASM=nasm            # Assembler for other asm
CC=i686-elf-gcc     # C/C++ Compiler
LD=i686-elf-ld      # Linker

# -- Virtualizer -- #
VM=qemu-system-i386 # Used to test iso image

# ----- Files ----- #
DIR_KERNEL = $(SRC_DIR)/kernel
SRC_KERNEL_C = $(wildcard $(DIR_KERNEL)/*.c)
SRC_KERNEL_C += $(wildcard $(DIR_KERNEL)/*/*.c)
OBJ_KERNEL_C = $(addsuffix .o, $(subst src/,$(OBJ_DIR), $(SRC_KERNEL_C)))

# -- Directories -- #
# Source directory
SRC_DIR=src
# Object directory
OBJ_DIR=bin/raw
# ISO Image directory
BIN_DIR=bin
# ISO File name
FI_NAME=ostest
# Grub include directory
BT_CONF=bin/conf

# Declare as sections #
.PHONY: all clean run runs iso dirs bootloader kernel

all: notice dirs iso 

# ------------------ #
#       Notice       #
# ------------------ #
notice:
	@echo ""
	@echo " MarkedRain OS makefile compilation"
	@echo " ----------------------------------"
	@echo ""

# ------------------ #
#     Disk Image     #
# ------------------ #
iso: $(BIN_DIR)/$(FI_NAME)

$(BIN_DIR)/$(FI_NAME): bootloader kernel
	$(CC) -T $(SRC_DIR)/configs/krnl_link.ld -o $(OBJ_DIR)/kernel.bin -ffreestanding -O2 -nostdlib $(OBJ_DIR)/kbp1_boot.o $(OBJ_DIR)/kbp2_kernel.o -lgcc
	cp $(OBJ_DIR)/kernel.bin $(BT_CONF)/mrain/system/kernel.bin
	cp $(SRC_DIR)/configs/grub.cfg $(BT_CONF)/boot/grub/grub.cfg
	grub-mkrescue -o $(BIN_DIR)/$(FI_NAME).iso $(BT_CONF)
	@echo ""
	@echo " You have built this iso file. Use 'make run' to run it, or use 'make runs' to skip to build and test."
	@echo ""

# ------------------ #
#      Make dir      #
# ------------------ #
dirs:
	mkdir -p $(BIN_DIR) $(OBJ_DIR) $(BT_CONF) $(BT_CONF)/boot $(BT_CONF)/boot/grub
	mkdir -p $(BT_CONF)/mrain $(BT_CONF)/mrain/system

# ------------------ #
#     Bootloader     #
# ------------------ #
bootloader: $(OBJ_DIR)/kbp1_boot.o

$(OBJ_DIR)/kbp1_boot.o:
	$(CASM) $(SRC_DIR)/boot/boot.s -o $(OBJ_DIR)/kbp1_boot.o

# ------------------ #
#       Kernel       #
# ------------------ #
kernel: $(OBJ_DIR)/kbp2_kernel.o

$(OBJ_DIR)/kbp2_kernel.o:
	$(CC) -c $(SRC_DIR)/kernel/kernel.c -o $(OBJ_DIR)/kbp2_kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I $(DIR_KERNEL) -I $(DIR_LIBC)

# ------------------ #
#   Virtualization   #
# ------------------ #

run:
	$(VM) -cdrom $(BIN_DIR)/$(FI_NAME).iso

# ------------------ #
#      V & ISOs      #
# ------------------ #

runs: all run

# ------------------ #
#       Cleans       #
# ------------------ #

clean:
	rm -rf $(BIN_DIR)
