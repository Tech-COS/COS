cmake_minimum_required(VERSION 3.10)
project(cos.iso)

set(CMAKE_C_STANDARD 11)

set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

enable_language(ASM_NASM)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker.ld")

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_LINKER /usr/bin/ld)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -mcmodel=large -mlarge-data-threshold=2147483647 -ffreestanding -mno-red-zone -nostdlib -Wall -Wextra -z noexecstack -z max-page-size=0x1000 -fPIC -fno-stack-protector")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T '${LINKER_SCRIPT}' -o '${OUTPUT_DIR}/cos.bin' -n -z max-page-size=0x1000 -nostdlib")

set(SOURCES_ASM
        src/boot/main.asm
        src/boot/main64.asm
        src/boot/idt.asm
        src/boot/header.asm
        src/boot/cpu_state_functions.asm
)

set_source_files_properties(
        src/boot/main.asm
        src/boot/main64.asm
        src/boot/idt.asm
        src/boot/header.asm
        src/boot/cpu_state_functions.asm
        PROPERTIES LANGUAGE ASM_NASM
)

set(SOURCES
        src/kernel/acpi.c
        src/kernel/asm.c
        src/kernel/gdt_handler.c
        src/kernel/idt.c
        src/kernel/interrupts.c
        src/kernel/main.c
        src/kernel/pci.c
        src/kernel/stack_protection.c
        src/kernel/graphics/bmp.c
        src/kernel/graphics/font.c
        src/kernel/graphics/screen.c
        src/kernel/lib/cos_time.c
        src/kernel/lib/memory_functions.c
        src/kernel/lib/printf.c
        src/kernel/lib/string.c
        src/kernel/term/command.c
        src/kernel/term/tty.c
        src/kernel/term/commands/cat.c
        src/kernel/term/commands/cd.c
        src/kernel/term/commands/clear.c
        src/kernel/term/commands/launch.c
        src/kernel/term/commands/ls.c
        src/kernel/term/commands/lspci.c
        src/kernel/term/commands/move.c
        src/kernel/term/commands/touch.c
)

include_directories(include)
include_directories(include/kernel)
include_directories(include/kernel/graphics)
include_directories(include/kernel/lib)
include_directories(include/kernel/term)

add_subdirectory(WindowsParser)
add_subdirectory(MemoryManagement)
add_subdirectory(FileSystem)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
file(COPY ${ASSETS_SOURCE_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

string(MAKE_C_IDENTIFIER "assets/font/lat9u.psf" psf_input_id)
set(psf_output_id "${OUTPUT_DIR}/${psf_input_id}.o")
add_custom_command(
        OUTPUT ${psf_output_id}
        COMMAND ${CMAKE_OBJCOPY} -O elf64-x86-64 -B i386 -I binary "assets/font/lat9u.psf" ${psf_output_id}
)

string(MAKE_C_IDENTIFIER "assets/COS_Logo.bmp" logo_input_id)
set(logo_output_id "${OUTPUT_DIR}/${logo_input_id}.o"
        src/kernel/handle_stack_translation.c
        LinuxParser/include/linux_parser.h
        LinuxParser/src/parse_cos.c
        src/bin/hello_world.cpp
        src/bin/hello_world_elf.c
        src/kernel/term/commands/ps.c
        src/bin/prog1.c
        FileSystem/test/src/test_superblock_fill.c)
add_custom_command(
        OUTPUT ${logo_output_id}
        COMMAND ${CMAKE_OBJCOPY} -O elf64-x86-64 -B i386 -I binary "assets/COS_Logo.bmp" ${logo_output_id}
)

add_executable(kernel ${psf_output_id} ${logo_output_id} ${SOURCES_ASM} ${SOURCES})

target_link_libraries(kernel WindowsParser MemoryManagement FileSystem)

add_custom_command(
        TARGET kernel
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy kernel "${CMAKE_SOURCE_DIR}/isodir/boot/cos.bin"
        COMMENT "Copying cos.bin to isodir"
)

add_custom_command(
        TARGET kernel
        POST_BUILD
        COMMAND grub-mkrescue -o "${CMAKE_SOURCE_DIR}/cos.iso" "${CMAKE_SOURCE_DIR}/isodir"
        COMMENT "Creating ISO image for kernel and modules"
)
