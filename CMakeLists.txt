cmake_minimum_required(VERSION 3.0)
project(Tama)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-fsanitize=address
    -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op
    -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel
    -Wstrict-overflow=5 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option
)
add_link_options(-fsanitize=address)
add_executable(Tama
    main.cxx
    tama.cxx
    cpu.cxx
    opcode_handlers.cxx
    decode.cxx
    io.cxx
)
