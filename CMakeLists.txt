cmake_minimum_required(VERSION 3.20)

project(onecard-poker-solver VERSION 0.1.0 LANGUAGES CXX)

execute_process(
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/open_spiel
    COMMAND ./install.sh
    COMMAND_ERROR_IS_FATAL ANY
)

set(ENV{DEFAULT_OPTIONAL_DEPENDENCY} OFF)
set(ENV{OPEN_SPIEL_BUILD_WITH_PYTHON} OFF)
set(ENV{OPEN_SPIEL_ENABLE_JAX} OFF)
set(ENV{OPEN_SPIEL_ENABLE_PYTORCH} OFF)
set(ENV{OPEN_SPIEL_ENABLE_TENSORFLOW} OFF)

set(ENV{BUILD_SHARED_LIB} ON)
add_subdirectory(open_spiel/open_spiel)

add_executable(main src/main.cpp src/onecard_poker.cpp)
target_link_libraries(main PRIVATE open_spiel)
target_include_directories(main PRIVATE open_spiel open_spiel/open_spiel/abseil-cpp)
target_compile_options(main PRIVATE -Werror -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-deprecated-declarations)
target_compile_features(main PRIVATE cxx_std_26)

add_custom_command(TARGET main POST_BUILD COMMAND main)
