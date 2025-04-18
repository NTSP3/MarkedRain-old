# -- Directories -- #
SRC_DIR=src#              # Source directory
BIN_DIR=$(ISO_DIR)/raw#   # Output directory for binary files
ISO_DIR=bin#              # Output directory for ISO
FI_NAME=mrain.iso#        # File name for ISO image
BT_CONF=$(ISO_DIR)/conf#  # Include directory for grub

# ----- Files ----- #
CHUNK_KBOOT = $(BIN_DIR)/kernel-pt1.o# The file with multiboot header 
CHUNK_KERNEL = $(BIN_DIR)/kernel-pt2.o# Kernel and shell
DIR_KERNEL = $(SRC_DIR)/kernel# Kernel directory
DIR_LIB = $(SRC_DIR)/lib# # Libraries
DIR_LIBC = $(DIR_LIB)/libc# C Libraries
DIR_LIBK = $(DIR_LIB)/libk# Kernel Libraries
SRC_KERNEL_C = $(wildcard $(DIR_KERNEL)/*.c)# Kernel rootdir search (C files)
SRC_KERNEL_C += $(wildcard $(DIR_KERNEL)/*/*.c)# Kernel subdir search (C files)
SRC_KERNEL_ASM = $(wildcard $(DIR_KERNEL)/*.asm)# Kernel rootdir search (ASM files)
SRC_KERNEL_ASM += $(wildcard $(DIR_KERNEL)/*/*.asm)# Kernel subdir search (ASM files)
OBJ_KERNEL_C = $(addsuffix .o, $(subst $(SRC_DIR)/,$(BIN_DIR)/, $(SRC_KERNEL_C)))# Make krnl C files with *.o extension
OBJ_KERNEL_ASM = $(addsuffix .o, $(subst $(SRC_DIR)/,$(BIN_DIR)/, $(SRC_KERNEL_ASM)))# Make krnl ASM files with *.o extension
DEP_KERNEL = $(OBJ_KERNEL_C:.o=.d)# Kernel dependencies?
SRC_LIB_C = $(shell find $(DIR_LIB) -name "*.c")# Library rootdir search (C files)
SRC_LIB_ASM = $(shell find $(DIR_LIB) -name "*.asm")# Library rootdir search (ASM files)
OBJ_LIB_C = $(addsuffix .o, $(subst $(SRC_DIR)/,$(BIN_DIR)/, $(SRC_LIB_C)))# Make lib C files with *.o extension
OBJ_LIB_ASM = $(addsuffix .o, $(subst $(SRC_DIR)/,$(BIN_DIR)/, $(SRC_LIB_ASM)))# Make lib ASM files with *.o extension
DEP_LIB = $(OBJ_LIB_C:.o=.d)# Library dependencies?

OBJ_DIRS = $(BIN_DIR)/kernel $(BIN_DIR)/lib $(BIN_DIR)/chunks# Object directories
OBJ_DIRS += $(dir $(OBJ_KERNEL_C))# Kernel C obj files to obj dir
OBJ_DIRS += $(dir $(OBJ_KERNEL_ASM))# Kernel ASM obj files to obj dir
OBJ_DIRS += $(dir $(OBJ_LIB_C))# Library C obj files to obj dir
OBJ_DIRS += $(dir $(OBJ_LIB_ASM))# Library ASM obj files to obj dir

# --- Compilers --- #
CASM = i686-elf-as        # Another Assembler
ASM = nasm#               # Assembler
CC = i686-elf-gcc#        # C/C++ Compiler
LD = i686-elf-ld#         # Linker

# -- Virtualizer -- #
VM = qemu-system-i386#    # Virtualization software

# ----- Flags ----- #
FLAGS_C = -Wall -Wextra -std=c99 -nostdinc -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -lgcc -D__is_kernel -m32 -MMD
FLAGS_RUN =

# ---- Section ---- #
.PHONY: all clean dirs runs run iso bootkrnl kernel

# ---- Autorun ---- #
all: notice dirs iso

# ----------------- #
#      Message      #
# ----------------- #
notice:
	@echo ""
	@echo " MarkedRain OS makefile compilation"
	@echo "--------------------------------------"
	@echo ""

# ----------------- #
#     ISO Image     #
# ----------------- #
iso: $(ISO_DIR)/$(FI_NAME)

$(ISO_DIR)/$(FI_NAME): bootkrnl kernel
	$(CC) -T $(SRC_DIR)/conf/gkrnl.ld -o $(BIN_DIR)/kernel.bin -ffreestanding -O2 -nostdlib $(BIN_DIR)/kernel-pt1.o $(BIN_DIR)/kernel-pt2.o -lgcc
	cp $(BIN_DIR)/kernel.bin $(BT_CONF)/mrain/system/kernel.bin
	cp $(SRC_DIR)/conf/grub.cfg $(BT_CONF)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_DIR)/$(FI_NAME) $(BT_CONF)
	@echo ""
	@echo " You have now built this iso file. Use 'make run' to run it, or use 'make runs' to skip to build and test."
	@echo " If you already use one of them, please ignore this message."
	@echo ""

# ----------------- #
#    Kernel-boot    #
# ----------------- #
bootkrnl: $(CHUNK_KBOOT)

$(CHUNK_KBOOT):
	$(CASM) $(SRC_DIR)/kboot/kboot.s -o $@

# ----------------- #
#    Kernel-main    #
# ----------------- #
kernel: $(CHUNK_KERNEL)

$(CHUNK_KERNEL): $(OBJ_KERNEL_ASM) $(OBJ_KERNEL_C) $(OBJ_LIB_ASM) $(OBJ_LIB_C)
	$(LD) -T $(SRC_DIR)/conf/kernel.ld -e main -r -o $@ $^

$(BIN_DIR)/kernel/%.c.o: $(SRC_DIR)/kernel/%.c
	$(CC) $(FLAGS_C) -c -I $(DIR_KERNEL) -I $(DIR_LIBC) -I $(DIR_LIBK) -o $@ $<

$(BIN_DIR)/kernel/%.asm.o: $(SRC_DIR)/kernel/%.asm
	$(ASM) -f elf -i $(DIR_KERNEL) -o $@ $<

# ----------------- #
#     Libraries     #
# ----------------- #
$(BIN_DIR)/lib/%.c.o: $(DIR_LIB)/%.c
	$(CC) $(FLAGS_C) -c -I $(DIR_LIBC) -I $(DIR_LIBK) -o $@ $<

$(BIN_DIR)/lib/%.asm.o: $(DIR_LIB)/%.asm
	$(ASM) -f elf -i $(DIR_LIBC) -i $(DIR_LIBK) -o $@ $<

# ----------------- #
#   Compile & Run   #
# ----------------- #
runs: all run

# ----------------- #
#  Run, no compile  #
# ----------------- #
run:
	$(VM) $(FLAGS_RUN) -drive format=raw,if=ide,index=0,file=$(ISO_DIR)/$(FI_NAME)

# ----------------- #
#  New Directories  #
# ----------------- #
dirs:
	@mkdir -p $(ISO_DIR) $(BIN_DIR) $(OBJ_DIRS) $(BT_CONF) $(BT_CONF)/boot $(BT_CONF)/boot/grub
	@mkdir -p $(BT_CONF)/mrain $(BT_CONF)/mrain/system

# ----------------- #
#       Clean       #
# ----------------- #
clean:
	@rm -rf $(BT_CONF) $(OBJ_DIRS) $(BIN_DIR) $(ISO_DIR)

# -- Depedencies -- #
-include $(DEP_STAGE2)
-include $(DEP_KERNEL)
