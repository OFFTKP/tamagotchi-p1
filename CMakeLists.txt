cmake_minimum_required(VERSION 3.0)
project(Tama)
set(CMAKE_BUILD_TYPE Debug)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
add_compile_options(-fsanitize=address
    -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op
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
    timers.cxx
    io.cxx
    frontend.cxx
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)
target_include_directories(Tama PRIVATE ${GLEW_INCLUDE_DIRS})
target_link_libraries(Tama glfw  ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})