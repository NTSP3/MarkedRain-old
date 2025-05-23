# make run -> Builds and launches the OS on Qemu
# Set DEBUG to 1 to enable debug mode, scripts/debug is executed in gdb

# Files
BIN = bin/os
FS = fs/fs
CHUNK_STAGE1 = obj/chunks/stage1
CHUNK_STAGE2 = obj/chunks/stage2
CHUNK_KERNEL = obj/chunks/kernel
DBG_STAGE2 = obj/stage2/stage2.elf
DBG_KERNEL = obj/kernel/kernel.elf
SYM_STAGE2 = $(DBG_STAGE2:.elf=.sym)
SYM_KERNEL = $(DBG_KERNEL:.elf=.sym)
DIR_STAGE1 = src/stage1
DIR_STAGE2 = src/stage2
DIR_KERNEL = src/kernel
DIR_LIB = src/lib
DIR_LIBC = $(DIR_LIB)/libc
DIR_LIBK = $(DIR_LIB)/libk
SRC_STAGE1 = $(wildcard $(DIR_STAGE1)/*)
SRC_STAGE2_C = $(wildcard $(DIR_STAGE2)/*.c)
SRC_STAGE2_ASM = $(wildcard $(DIR_STAGE2)/*.asm)
OBJ_STAGE2_C = $(addsuffix .o, $(subst src/,obj/, $(SRC_STAGE2_C)))
OBJ_STAGE2_ASM = $(addsuffix .o, $(subst src/,obj/, $(SRC_STAGE2_ASM)))
DEP_STAGE2 = $(OBJ_STAGE2_C:.o=.d)
SRC_KERNEL_C = $(wildcard $(DIR_KERNEL)/*.c)
SRC_KERNEL_C += $(wildcard $(DIR_KERNEL)/*/*.c)
SRC_KERNEL_ASM = $(wildcard $(DIR_KERNEL)/*.asm)
SRC_KERNEL_ASM += $(wildcard $(DIR_KERNEL)/*/*.asm)
OBJ_KERNEL_C = $(addsuffix .o, $(subst src/,obj/, $(SRC_KERNEL_C)))
OBJ_KERNEL_ASM = $(addsuffix .o, $(subst src/,obj/, $(SRC_KERNEL_ASM)))
DEP_KERNEL = $(OBJ_KERNEL_C:.o=.d)
SRC_LIB_C = $(shell find $(DIR_LIB) -name "*.c")
SRC_LIB_ASM = $(shell find $(DIR_LIB) -name "*.asm")
OBJ_LIB_C = $(addsuffix .o, $(subst src/,obj/, $(SRC_LIB_C)))
OBJ_LIB_ASM = $(addsuffix .o, $(subst src/,obj/, $(SRC_LIB_ASM)))
DEP_LIB = $(OBJ_LIB_C:.o=.d)
DBG_CMD = scripts/debug

OBJ_DIRS = obj/kernel obj/lib obj/stage2 obj/chunks
OBJ_DIRS += $(dir $(OBJ_KERNEL_C))
OBJ_DIRS += $(dir $(OBJ_KERNEL_ASM))
OBJ_DIRS += $(dir $(OBJ_LIB_C))
OBJ_DIRS += $(dir $(OBJ_LIB_ASM))

# Tools
TOOL_ASM = nasm
TOOL_VM = qemu-system-i386
TOOL_C = i686-elf-gcc
TOOL_LINK = i686-elf-ld
TOOL_DBG = gdb

# Flags
FLAGS_C = -Wall -Wextra -std=c99 -nostdinc -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -lgcc -D__is_kernel -m32 -MMD
FLAGS_RUN =
DEBUG ?= 0

ifeq ($(DEBUG), 1)
FLAGS_C += -ggdb
FLAGS_RUN += -s -S
endif

# Commands
CMD_EXEC_GDB = cd $(PWD) && ((cat $(DBG_CMD); cat) | $(TOOL_DBG) $(BIN)); exec bash


# --- Main --- #
all: mkdirs $(BIN)

$(BIN): $(CHUNK_STAGE1) $(CHUNK_STAGE2) $(CHUNK_KERNEL) $(FS)
	cat $(CHUNK_STAGE1) $(CHUNK_STAGE2) $(CHUNK_KERNEL) $(FS) > $@


# --- Stage 1 --- #
$(CHUNK_STAGE1): $(SRC_STAGE1)
	$(TOOL_ASM) -f bin -o $(CHUNK_STAGE1) -i $(DIR_STAGE1) $(DIR_STAGE1)/bootloader.asm


# --- Stage 2 --- #
$(CHUNK_STAGE2): $(OBJ_STAGE2_ASM) $(OBJ_STAGE2_C)
	$(TOOL_LINK) -T stage2.ld -e main --oformat binary -o $@ $^

obj/stage2/%.c.o: src/stage2/%.c
	$(TOOL_C) $(FLAGS_C) -c -I $(DIR_STAGE2) -o $@ $<

obj/stage2/%.asm.o: src/stage2/%.asm
	$(TOOL_ASM) -f elf -i $(DIR_STAGE2) -o $@ $<


# --- Kernel --- #
$(CHUNK_KERNEL): $(OBJ_KERNEL_ASM) $(OBJ_KERNEL_C) $(OBJ_LIB_ASM) $(OBJ_LIB_C)
ifeq ($(DEBUG), 1)
	$(TOOL_LINK) -T kernel.ld -e main -o $(DBG_KERNEL) $^

	objcopy --only-keep-debug $(DBG_KERNEL) $(SYM_KERNEL)
	objcopy --strip-debug $(DBG_KERNEL)
	objcopy -O binary $(DBG_KERNEL) $@
else
	$(TOOL_LINK) -T kernel.ld -e main --oformat binary -o $@ $^
endif

obj/kernel/%.c.o: src/kernel/%.c
	$(TOOL_C) $(FLAGS_C) -c -I $(DIR_KERNEL) -I $(DIR_LIBC) -I $(DIR_LIBK) -o $@ $<

obj/kernel/%.asm.o: src/kernel/%.asm
	$(TOOL_ASM) -f elf -i $(DIR_KERNEL) -o $@ $<


# --- Libs --- #
obj/lib/%.c.o: $(DIR_LIB)/%.c
	$(TOOL_C) $(FLAGS_C) -c -I $(DIR_LIBC) -I $(DIR_LIBK) -o $@ $<

obj/lib/%.asm.o: $(DIR_LIB)/%.asm
	$(TOOL_ASM) -f elf -i $(DIR_LIBC) -i $(DIR_LIBK) -o $@ $<


# --- Fs --- #
$(FS):
	@echo "--- Creating an empty file system named 'OS2020FS' (64 MiB) ---"
	dd if=/dev/zero of=$@ bs=512 count=128K
	mkfs.fat -F 32 -s 1 -n OS2020FS $@


# --- Utils --- #
run: all
ifeq ($(DEBUG), 1)
	xterm -hold -e 'bash -c "$(CMD_EXEC_GDB)"' &
endif

	$(TOOL_VM) $(FLAGS_RUN) -drive format=raw,if=ide,index=0,file=$(BIN)

# Used to avoid a compilation when programming FS
.PHONY: forcerun
forcerun:
	$(TOOL_VM) $(FLAGS_RUN) -drive format=raw,if=ide,index=0,file=$(BIN)


.PHONY: mkdirs
mkdirs:
	@echo DIRS $(OBJ_DIRS)
	mkdir -p bin obj fs $(OBJ_DIRS)

.PHONY: clean
clean:
	rm -rf obj bin

.PHONY: cleanfs
cleanfs: clean
	rm -rf fs

# --- Depedencies --- #
-include $(DEP_STAGE2)
-include $(DEP_KERNEL)
