project(Tama)
set(CMAKE_BUILD_TYPE Debug)
add_executable(Tama
    main.cxx
    tama.cxx
    cpu.cxx
    opcode_handlers.cxx
    decode.cxx
)
