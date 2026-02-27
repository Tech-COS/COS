set(KERNEL_OUTPUT_DIR ${OUTPUT_DIR}/src/bin)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

add_executable(hello_world.bin
        hello_world.asm
)

add_executable(echo.bin
        echo.asm
)

set_source_files_properties(hello_world.asm PROPERTIES LANGUAGE ASM_NASM)
set_source_files_properties(echo.asm PROPERTIES LANGUAGE ASM_NASM)

install(TARGETS hello_world.bin DESTINATION ${KERNEL_OUTPUT_DIR})
install(TARGETS echo.bin DESTINATION ${KERNEL_OUTPUT_DIR})
