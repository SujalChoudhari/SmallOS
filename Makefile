# Define the assembler to be used
ASM=nasm
CC = gcc
# Define the source and build directories
SRC_DIR=src
BUILD_DIR=build
TOOLS_DIR=tools

# Declare phony targets (targets that don't create files)
.PHONY: all floppy_image kernel bootloader clean always tools_fat


all: floppy_image tools_fat

# Target to create a floppy image
floppy_image: $(BUILD_DIR)/main_floppy.img

# Recipe to create the floppy image
$(BUILD_DIR)/main_floppy.img: bootloader kernel
	# Create an empty floppy image of 1.44MB (2880 sectors, 512 bytes each)
	dd if=/dev/zero of=$(BUILD_DIR)/main_floppy.img bs=512 count=2880
	
	# Format the floppy image with FAT12 file system, label it "NBOS"
	mkfs.fat -F 12 -n "NBOS" $(BUILD_DIR)/main_floppy.img
	
	# Write the bootloader binary to the floppy image (overwrite the start)
	dd if=$(BUILD_DIR)/bootloader.bin of=$(BUILD_DIR)/main_floppy.img conv=notrunc
	
	# Copy the kernel binary to the floppy image (root directory)
	mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/kernel.bin "::kernel.bin"
	mcopy -i $(BUILD_DIR)/main_floppy.img test.txt "::test.txt"

# Target to assemble the bootloader
bootloader: $(BUILD_DIR)/bootloader.bin

# Recipe to assemble the bootloader
$(BUILD_DIR)/bootloader.bin: always
	# Assemble the bootloader source file into a binary
	$(ASM) $(SRC_DIR)/bootloader/boot.asm -f bin -o $(BUILD_DIR)/bootloader.bin


# Target to assemble the kernel
kernel: $(BUILD_DIR)/kernel.bin

# Recipe to assemble the kernel
$(BUILD_DIR)/kernel.bin: always
	# Assemble the kernel source file into a binary
	$(ASM) $(SRC_DIR)/kernel/main.asm -f bin -o $(BUILD_DIR)/kernel.bin


# Tools
tools_fat: $(BUILD_DIR)/tools/fat
$(BUILD_DIR)/tools/fat: $(TOOLS_DIR)/fat/fat.c
	$(CC) -g -o $(BUILD_DIR)/tools/fat $(TOOLS_DIR)/fat/fat.c
	chmod +x $(BUILD_DIR)/tools/fat

# Target to ensure the build directory exists
always:
	# Create the build directory if it does not exist
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/tools


# Target to clean the build directory
clean:
	# Remove all files in the build directory
	rm -rf $(BUILD_DIR)/*