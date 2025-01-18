cmake_minimum_required(VERSION 3.31.0)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++-18")
set(CMAKE_C_COMPILER "/usr/bin/clang-18")
set(CMAKE_CXX_FLAGS "-stdlib=libc++ -v")
set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++ -lc++abi")

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project("module" C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/external/*.cpp")
# Add source to this project's executable.
add_executable (ThePinkPuma-vol2)
target_sources(ThePinkPuma-vol2
    PUBLIC
    ${ALL_SOURCES}
 )

add_library(glad "external/include/glad/glad.c" )

target_include_directories(ThePinkPuma-vol2 PRIVATE "${CMAKE_SOURCE_DIR}/external/include" "${CMAKE_SOURCE_DIR}/external/include/imgui/imgui")

file(GLOB_RECURSE MODULES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.ixx")
target_sources(ThePinkPuma-vol2
  PUBLIC
    FILE_SET all_my_modules TYPE CXX_MODULES FILES
    ${MODULES}
)

target_link_libraries(ThePinkPuma-vol2 glfw glad)

file(COPY "${CMAKE_SOURCE_DIR}/shaders" DESTINATION "${CMAKE_BINARY_DIR}")
