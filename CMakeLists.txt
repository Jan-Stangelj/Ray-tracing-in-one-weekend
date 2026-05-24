cmake_minimum_required(VERSION 3.16)

project("rt" LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_subdirectory("vendor/glm")

file(GLOB_RECURSE "SOURCES" CONFIGURE_DEPENDS "src/*.cpp")
add_executable("rt" "${SOURCES}")

# Enable ALL warnings
if(MSVC)
  target_compile_options(rt PRIVATE /W4)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(rt PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(rt PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories("rt" PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries("rt" PRIVATE glm SFML::Graphics SFML::Window SFML::System)