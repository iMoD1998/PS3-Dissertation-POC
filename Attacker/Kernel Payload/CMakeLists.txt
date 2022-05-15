set(CMAKE_TOOLCHAIN_FILE toolchain/toolchain.cmake)
project("PS3KernelPayload" C CXX ASM)

# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

add_link_options(
    -T${CMAKE_SOURCE_DIR}/linker/Payload.ld
    -nostartfiles -nostdlib -nodefaultlibs -Wl,--gc-sections,-static,--undefined=PayloadEntry
)

add_compile_options(
    -Os -g -fno-exceptions -fno-builtin -ffunction-sections -ffreestanding -nostdinc -Wno-int-to-pointer-cast
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

add_executable("${PROJECT_NAME}" "src/Init.S" "src/HookAsm.s" "src/Libc.cpp" "src/Hooks.cpp" "src/Hex.cpp" "src/UserMemory.cpp" "src/Exploit.cpp" "src/SpinLock.s" "src/Entry.cpp" "src/LV2/LV2.cpp" )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMENT Create payload.bin
    COMMAND ..\bin\llvm-objcopy.exe -O binary ${PROJECT_NAME} payload.bin
    COMMENT Create PS3HEN payload.
    COMMAND ..\bin\hengen.exe ..\hen_payloads\PS3HEN.BIN_DEX_484 payload.bin PS3HEN.p3t
    COMMENT Move payload to website directory.
    COMMAND move PS3HEN.p3t ..\..\Website\4.84\ )