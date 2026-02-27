########################
##
##  Created: Thu Jun 27 2024
##  File: Makefile
##
########################

# Compilers
BOOT_COMPILER = nasm
KERNEL_COMPILER = x86_64-elf-gcc
LINKER = x86_64-elf-ld
TEST_COMPILER = gcc
TEST_LINKER = ld
TESTING ?= 0

ifeq (${TESTING}, 1)
	KERNEL_COMPILER = ${TEST_COMPILER}
	LINKER = ${TEST_LINKER}
endif

# Check distro
UNAME_S := $(shell uname -s)
DISTRO := unknown

ifeq ($(UNAME_S), Linux)
    ifeq ($(wildcard /etc/fedora-release),/etc/fedora-release)
        DISTRO := Fedora
    else ifeq ($(wildcard /etc/arch-release),/etc/arch-release)
        DISTRO := ArchLinux
    else ifneq ("$(wildcard /etc/os-release)", "")
        DISTRO := $(shell . /etc/os-release && echo $$ID)
    endif
endif

# Choose grub command depends of the distro
ifeq ($(DISTRO), Fedora)
	ISO_COMPILER = grub2-mkrescue
else
    ISO_COMPILER = grub-mkrescue
endif

# Compiling properties
BUILD_DIR = ./build
BINARY_NAME = ${BUILD_DIR}/cos.bin
BINARY_FOLDER = ./src/bin
ISO_NAME = cos.iso
ISO_DIR = ./isodir
ISO_DIR_BOOT = ${ISO_DIR}/boot
ISO_DIR_GRUB = ${ISO_DIR_BOOT}/grub
ISO_FILESYSTEM_BIN = ${ISO_DIR}/bin

LINKER_FILE = ./linker.ld
LINKER_FLAGS = -n -z max-page-size=0x1000 -nostdlib
TEST_NAME = ./build/test_run

# Boot properties
SRC_BOOT_TYPE = .asm
SRC_BOOT_DIR = ./src
SRC_BIN_TYPE = .bin
SRC_BIN_DIR = ./src/bin
SRC_BOOT =\
	${SRC_BOOT_DIR}/boot/header.asm\
	${SRC_BOOT_DIR}/boot/main.asm\
	${SRC_BOOT_DIR}/boot/main64.asm\
	${SRC_BOOT_DIR}/boot/idt.asm\
	${SRC_BOOT_DIR}/boot/cpu_state_functions.asm\

BUILD_BOOT_PATH = ${BUILD_DIR}/boot
OBJ_BOOT = ${SRC_BOOT:${SRC_BOOT_DIR}/%${SRC_BOOT_TYPE}=${BUILD_BOOT_PATH}/%.o}

BUILD_BIN_PATH = ${BUILD_DIR}/bin
BIN_EXEC = ${SRC_BIN:${SRC_BIN_DIR}/%${SRC_BOOT_TYPE}=${BUILD_BIN_PATH}/%.bin}

ASSET_PATH = assets
SRC_FONT_TYPE = .psf
SRC_BMP_TYPE = .bmp
BUILD_FONT_PATH = ${BUILD_DIR}/${ASSET_PATH}/font
SRC_ASSET =\
	${ASSET_PATH}/font/lat9u.psf\
	${ASSET_PATH}/COS_Logo.bmp\

OBJ_ASSET = $(patsubst $(ASSET_PATH)/%$(SRC_FONT_TYPE),$(BUILD_FONT_PATH)/%.o,\
	$(filter %$(SRC_FONT_TYPE),$(SRC_ASSET))) \
	$(patsubst $(ASSET_PATH)/%${SRC_BMP_TYPE},$(BUILD_FONT_PATH)/%.o,\
	$(filter %.bmp,$(SRC_ASSET)))

# KERNEL
# Kernel properties
SRC_KERNEL_TYPE = .c
SRC_KERNEL_DIR = ./src/kernel
SRC_KERNEL =\
	${SRC_KERNEL_DIR}/gdt_handler.c\
	${SRC_KERNEL_DIR}/main.c\
	${SRC_KERNEL_DIR}/asm.c\
	${SRC_KERNEL_DIR}/idt.c\
	${SRC_KERNEL_DIR}/stack_protection.c\
	${SRC_KERNEL_DIR}/interrupts.c\
	${SRC_KERNEL_DIR}/lib/string.c\
	${SRC_KERNEL_DIR}/lib/printf.c\
	${SRC_KERNEL_DIR}/lib/cos_time.c\
	${SRC_KERNEL_DIR}/lib/memory_functions.c\
	${SRC_KERNEL_DIR}/graphics/font.c\
	${SRC_KERNEL_DIR}/graphics/screen.c\
	${SRC_KERNEL_DIR}/graphics/bmp.c\
	${SRC_KERNEL_DIR}/term/commands/clear.c\
	${SRC_KERNEL_DIR}/term/commands/launch.c\
	${SRC_KERNEL_DIR}/term/commands/ls.c\
	${SRC_KERNEL_DIR}/term/commands/cat.c\
	${SRC_KERNEL_DIR}/term/commands/move.c\
	${SRC_KERNEL_DIR}/term/commands/touch.c\
	${SRC_KERNEL_DIR}/term/commands/lspci.c\
	${SRC_KERNEL_DIR}/term/commands/pwd.c\
	${SRC_KERNEL_DIR}/term/commands/ps.c\
	${SRC_KERNEL_DIR}/term/tty.c\
	${SRC_KERNEL_DIR}/term/command.c\
	${SRC_KERNEL_DIR}/acpi.c\
	${SRC_KERNEL_DIR}/pci.c\

HEADERS_KERNEL =\
	-I./include/\
	-I./FileSystem/include\
	-I./MemoryManagement/include\
	-I./WindowsParser/include\

BUILD_KERNEL_PATH = ${BUILD_DIR}/kernel
OBJ_KERNEL = ${SRC_KERNEL:${SRC_KERNEL_DIR}/%${SRC_KERNEL_TYPE}=${BUILD_KERNEL_PATH}/%.o}
KERNEL_FLAGS = -m64 -mcmodel=large -mlarge-data-threshold=2147483647 -ffreestanding -mno-red-zone -nostdlib -g3 -Wall -Wextra -z noexecstack -z max-page-size=0x1000 -fPIC -MD ${HEADERS_KERNEL}  -pedantic -pedantic-errors

# Testing properties
SRC_TEST_TYPE = .c
SRC_TEST_DIR = ./test/src
SRC_TEST_BOOT =\
	${SRC_BOOT_DIR}/boot/cpu_state_functions.asm
SRC_TEST =\
	${SRC_TEST_DIR}/main.c\
	${SRC_TEST_DIR}/unity.c\
	${SRC_KERNEL_DIR}/term/tty.c\
	${SRC_KERNEL_DIR}/graphics/screen.c\
	${SRC_KERNEL_DIR}/graphics/font.c\
	${SRC_KERNEL_DIR}/asm.c\
	${SRC_KERNEL_DIR}/lib/string.c\
	${SRC_KERNEL_DIR}/lib/printf.c\
	${SRC_KERNEL_DIR}/lib/memory_functions.c\
	${SRC_KERNEL_DIR}/interrupts.c\
	${SRC_KERNEL_DIR}/lib/cos_time.c\


HEADER_TEST =\
	-I./test/include/\
	${HEADERS_KERNEL}\

BUILD_TEST_PATH = ${BUILD_DIR}/test
OBJ_TEST = ${SRC_TEST:${SRC_TEST_DIR}/%${SRC_TEST_TYPE}=${BUILD_TEST_PATH}/%.o}
OBJ_BOOT_TEST = ${SRC_TEST_BOOT:${SRC_BOOT_DIR}/%${SRC_BOOT_TYPE}=${BUILD_BOOT_PATH}/%.o}
TEST_FLAGS = -g3 -Wall -Wextra -Werror -MD ${HEADER_TEST}

# Submodules properties
FILESYSTEM_PATH = ./FileSystem
LIB_FILESYSTEM = ${FILESYSTEM_PATH}/libfilesystem.a
MEMORY_PATH = ./MemoryManagement
LIB_MEMORY = ${MEMORY_PATH}/libmemory.a
SCHEDULER_PATH = ./Scheduler
LIB_SCHEDULER = ${SCHEDULER_PATH}/libscheduler.a
WINPARSER_PATH = ./WindowsParser
LIB_WINDOWS = ${WINPARSER_PATH}/libwindowsparser.a
LINPARSER_PATH = ./LinuxParser
LIB_LINUX = ${LINPARSER_PATH}/liblinuxparser.a

all: ${ISO_NAME}

${LIB_FILESYSTEM}:
	$(MAKE) -C ${FILESYSTEM_PATH} lib

${LIB_MEMORY}:
	$(MAKE) -C ${MEMORY_PATH} -e COS_COMPILATION=1 lib

${LIB_SCHEDULER}:
	$(MAKE) -C ${SCHEDULER_PATH} -e COS_COMPILATION=1 lib

${LIB_WINDOWS}:
	$(MAKE) -C ${WINPARSER_PATH} -e IS_COS_COMPILATION=1 lib

${LIB_LINUX}:
	$(MAKE) -C ${LINPARSER_PATH} lib

${ISO_NAME}: ${OBJ_ASSET} ${OBJ_BOOT} ${OBJ_KERNEL} ${LIB_FILESYSTEM} ${LIB_MEMORY} ${LIB_SCHEDULER} ${LIB_WINDOWS} ${LIB_LINUX} linker
	@mkdir -p ${ISO_DIR}
	@mkdir -p ${ISO_DIR_BOOT}
	@mkdir -p ${ISO_FILESYSTEM_BIN}
	cp ${BINARY_NAME} ${ISO_DIR_BOOT}
	#cp ${BUILD_BIN_PATH}/*${SRC_BIN_TYPE} ${ISO_FILESYSTEM_BIN}
	${ISO_COMPILER} -o ${ISO_NAME} ${ISO_DIR}

${BUILD_FONT_PATH}/%.o: ${ASSET_PATH}/%${SRC_FONT_TYPE}
	@mkdir -p ${dir $@}
	objcopy -O elf64-x86-64 -B i386 -I binary $< $@

${BUILD_FONT_PATH}/%.o: ${ASSET_PATH}/%${SRC_BMP_TYPE}
	@mkdir -p ${dir $@}
	objcopy -O elf64-x86-64 -B i386 -I binary $< $@

${BUILD_BOOT_PATH}/%.o: ${SRC_BOOT_DIR}/%${SRC_BOOT_TYPE}
	@mkdir -p ${dir $@}
	${BOOT_COMPILER} -f elf64 $< -o $@

${BUILD_KERNEL_PATH}/%.o: ${SRC_KERNEL_DIR}/%${SRC_KERNEL_TYPE}
	@mkdir -p ${dir $@}
	${KERNEL_COMPILER} -c $< -o $@ ${KERNEL_FLAGS}

${TEST_BUILD_FONT_PATH}/%.o: ${ASSET_PATH}/%${SRC_FONT_TYPE}
	@mkdir -p ${dir $@}
	objcopy -O elf64-x86-64 -B i386 -I binary $< $@

${BUILD_TEST_PATH}/%.o: ${SRC_TEST_DIR}/%${SRC_TEST_TYPE}
	@mkdir -p ${dir $@}
	${TEST_COMPILER} -c $< -o $@ ${TEST_FLAGS}

test_run: ${OBJ_ASSET} ${OBJ_BOOT_TEST} ${OBJ_TEST} ${LIB_FILESYSTEM} ${LIB_MEMORY} ${LIB_WINDOWS} ${LIB_LINUX}
	${TEST_COMPILER} ${TEST_FLAGS} -o ${TEST_NAME} ${OBJ_ASSET} ${OBJ_BOOT_TEST} ${OBJ_TEST} ${LIB_FILESYSTEM} ${LIB_MEMORY} ${LIB_WINDOWS} ${LIB_LINUX}
	make -C ./MemoryManagement test_run
	cp ./build/test_run .
	chmod +x ./test_run
	chmod +x ./MemoryManagement/test_run

linker: ${OBJ_BOOT} ${OBJ_KERNEL} ${LIB_FILESYSTEM} ${LIB_MEMORY} ${LIB_SCHEDULER} ${LIB_WINDOWS} ${LIB_LINUX}
	${LINKER} ${LINKER_FLAGS} -T ${LINKER_FILE} -o ${BINARY_NAME} ${OBJ_BOOT} ${OBJ_ASSET} ${OBJ_KERNEL} ${LIB_MEMORY} ${LIB_SCHEDULER} ${LIB_WINDOWS} ${LIB_LINUX} ${LIB_FILESYSTEM}
clean:
	rm -rf ${BUILD_DIR}
	$(MAKE) -C ${FILESYSTEM_PATH} clean
	$(MAKE) -C ${MEMORY_PATH} clean
	$(MAKE) -C ${SCHEDULER_PATH} clean
	$(MAKE) -C ${WINPARSER_PATH} clean
	$(MAKE) -C ${LINPARSER_PATH} clean

fclean: clean
	rm -rf ${ISO_NAME}
	rm -rf ${TEST_NAME}
	rm -rf ${ISO_FILESYSTEM_BIN}
	$(MAKE) -C ${FILESYSTEM_PATH} fclean
	$(MAKE) -C ${MEMORY_PATH} fclean
	$(MAKE) -C ${SCHEDULER_PATH} fclean
	$(MAKE) -C ${WINPARSER_PATH} fclean
	$(MAKE) -C ${LINPARSER_PATH} fclean

re: fclean all

-include ${OBJ_KERNEL:%.o=%.d}
