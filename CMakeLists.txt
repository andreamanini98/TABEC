cmake_minimum_required(VERSION 3.25)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${ProjectId})

add_compile_definitions("SOURCE_ROOT=${CMAKE_CURRENT_SOURCE_DIR}")

# How to set the variable during configuration time:
# cmake -DTCHECKER_BIN_PATH="/path/to/tChecker/bin"
set(TCHECKER_BIN_PATH "/" CACHE STRING "The path to the bin directory of tChecker")
add_compile_definitions("TCHECKER_BIN=${TCHECKER_BIN_PATH}")

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

set(CMAKE_CXX_STANDARD 17)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inputFiles)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputFiles)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputFilesForChecking)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${PROJECT_NAME} "main.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC headers)

target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/scriptsForChecks/gt2C.sh)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/scriptsForChecks/tCheckerLiveness.sh)
