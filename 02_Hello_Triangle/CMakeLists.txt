cmake_minimum_required(VERSION 3.16)

project(hello_triagnle LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find GLFW (installed system-wide) and OpenGL.
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    OpenGL::GL
)

# On some Linux setups GLAD needs the dynamic loader library.
include(CheckLibraryExists)
check_library_exists(dl dlopen "" HAVE_LIBDL)
if(HAVE_LIBDL)
  target_link_libraries(${PROJECT_NAME} PRIVATE dl)
endif()
