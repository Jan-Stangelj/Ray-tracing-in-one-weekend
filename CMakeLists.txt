cmake_minimum_required(VERSION 3.16)

project("rt" LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package(OpenMP REQUIRED)

set("GLFW_BUILD_DOCS" OFF CACHE BOOL "" FORCE)
set("GLFW_BUILD_TESTS" OFF CACHE BOOL "" FORCE)
set("GLFW_BUILD_EXAMPLES" OFF CACHE BOOL "" FORCE)
set("GLFW_INSTALL" OFF CACHE BOOL "" FORCE)
set("BUILD_SHARED_LIBS" OFF CACHE BOOL "" FORCE)

add_subdirectory("vendor/glm")
add_subdirectory("vendor/glfw")

file(GLOB_RECURSE "SOURCES" CONFIGURE_DEPENDS "src/*.cpp")
add_executable("rt" "${SOURCES}")

# Enable ALL warnings
if(MSVC)
  target_compile_options(rt PRIVATE /W4)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(rt PRIVATE -Wall -Wextra -Wpedantic -O3 -g -march=native)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(rt PRIVATE -Wall -Wextra -Wpedantic -O3 -g -march=native)
endif()

target_compile_definitions("rt" PUBLIC "GLFW_INCLUDE_NONE=1" "GLM_FORCE_INTRINSICS")

target_include_directories("rt" PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories("rt" PRIVATE ${PROJECT_SOURCE_DIR}/vendor/include)

target_link_libraries("rt" PRIVATE glm glfw OpenMP::OpenMP_CXX)