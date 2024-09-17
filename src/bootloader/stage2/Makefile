BUILD_DIR?=build/
ASM?=nasm
ASMFLAGS?=-f obj
CC16?=/usr/bin/watcom/binl64/wcc
CFLAGS16?=-4 -d3 -s -wx -ms -zl -zq -za99
LD16?=/usr/bin/watcom/binl64/wlink

# Recursively find all C and ASM files in their respective directories
SOURCES_C=$(shell find c -name '*.c')
SOURCES_ASM=$(shell find asm -name '*.asm')

# Replace 'c/' or 'asm/' in the source path with 'build/stage2/c/' or 'build/stage2/asm/' for object file paths
OBJECTS_C=$(patsubst c/%.c, $(BUILD_DIR)/stage2/c/%.obj, $(SOURCES_C))
OBJECTS_ASM=$(patsubst asm/%.asm, $(BUILD_DIR)/stage2/asm/%.obj, $(SOURCES_ASM))

.PHONY: all stage2 clean always

all: stage2

stage2: $(BUILD_DIR)/stage2.bin

# Link the objects to produce the binary
$(BUILD_DIR)/stage2.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	$(LD16) NAME $(BUILD_DIR)/stage2.bin FILE \{ $(OBJECTS_ASM) $(OBJECTS_C) \} OPTION MAP=$(BUILD_DIR)/stage2.map @linker.lnk

# Rule to compile C source files into object files, creating directories as needed
$(BUILD_DIR)/stage2/c/%.obj: c/%.c always
	@mkdir -p $(dir $@)
	$(CC16) $(CFLAGS16) -fo=$@ $<

# Rule to assemble ASM source files into object files, creating directories as needed
$(BUILD_DIR)/stage2/asm/%.obj: asm/%.asm always
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

always:
	@mkdir -p $(BUILD_DIR)/stage2/c
	@mkdir -p $(BUILD_DIR)/stage2/asm

clean:
	rm -f $(BUILD_DIR)/stage2.bin
	rm -rf $(BUILD_DIR)/stage2/c
	rm -rf $(BUILD_DIR)/stage2/asm
