BUILD_DIR ?= build/
DEBUG_DIR ?= $(BUILD_DIR)/debug
ASM ?= nasm
CC ?= gcc
CXX ?= g++          # Added for C++ compilation
LD ?= ld
CFLAGS ?= -ffreestanding -m32 -Wall -Wextra
CXXFLAGS ?= -ffreestanding -m32 -Wall -Wextra  # C++ flags
ASMFLAGS ?= -f elf32
LDFLAGS ?= -T linker.ld -m elf_i386
MAPFILE := $(DEBUG_DIR)/stage2.map

# Find all C++ files in the cpp directory and convert them into .o files in the build directory
CPP_SRC := $(wildcard c/*.cpp)
CPP_OBJ := $(patsubst c/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SRC))

# Find all C files in the c directory and convert them into .o files in the build directory
C_SRC := $(wildcard c/*.c)
C_OBJ := $(patsubst c/%.c,$(BUILD_DIR)/%.o,$(C_SRC))

# Find all assembly files in the asm directory and convert them into .o files in the build directory
ASM_SRC := $(wildcard asm/*.asm)
ASM_OBJ := $(patsubst asm/%.asm,$(BUILD_DIR)/%.o,$(ASM_SRC))

.PHONY: all stage2 clean

all: stage2

# Link the stage2 binary from object files and generate a map file
stage2: $(BUILD_DIR)/stage2.bin $(MAPFILE)

$(BUILD_DIR)/stage2.bin: $(C_OBJ) $(CPP_OBJ) $(ASM_OBJ)
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(DEBUG_DIR)
	$(LD) $(LDFLAGS) -o $@ $(C_OBJ) $(CPP_OBJ) $(ASM_OBJ)
	$(LD) $(LDFLAGS) --Map=$(MAPFILE) -o $@ $(C_OBJ) $(CPP_OBJ) $(ASM_OBJ)

# Assemble the assembly files
$(BUILD_DIR)/%.o: asm/%.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile all .cpp files into .o files in the build directory
$(BUILD_DIR)/%.o: c/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile all .c files into .o files in the build directory
$(BUILD_DIR)/%.o: c/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build directory
clean:
	rm -rf $(BUILD_DIR)
