INCLUDE_DIR=./include
SOURCE_DIR=./src
BUILD_DIR=./build


BUILD_LIST=$(shell ./src/mk.sh $(PWD)/src src)


CC = i686-elf-gcc
AS = i686-elf-as


# -O for optimization, -fomit-frame-pointer for omiting frame pointer, -finline-functions for inlining available funcs
# -nostdinc and -fno-builtin to note we aren't using standard c lib functions, -I to say headers are in ./include
# -c to tell the compiler to only compile and not link
C_FLAGS=-Wall -Wextra -O -g -std=gnu99 -ffreestanding -I$(INCLUDE_DIR) -c
LINK_FLAGS=-Wall -Wextra -O -g -ffreestanding -nostdlib -I$(INCLUDE_DIR)


all: $(BUILD_DIR)/kernel

$(BUILD_DIR)/kernel: $(BUILD_LIST)
	$(CC) $(LINK_FLAGS) -std=gnu99 -T $(INCLUDE_DIR)/linker.ld -o $@ $^

clean:
	rm $(BUILD_DIR)/kernel
