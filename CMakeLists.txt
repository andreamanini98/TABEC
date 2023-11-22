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
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inputTiles)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/executables)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputs)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputs/outputFiles)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputs/outputFilesForChecking)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputs/outputPDFs)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputs/outputDOTs)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputs/logs)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/outputs/logs/outputFilesForCheckingLogs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/executables)

set(SOURCES src/converter.cpp src/checker.cpp src/grapher.cpp src/constructor.cpp)
foreach (source IN LISTS SOURCES)
    get_filename_component(executable_name ${source} NAME_WE)
    add_executable(${executable_name} ${source})
    target_include_directories(${executable_name} PUBLIC headers)
    target_link_libraries(${executable_name} nlohmann_json::nlohmann_json)
endforeach ()

execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/scriptsForChecks/gt2C.sh)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/scriptsForChecks/lt2CCycle.sh)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/scriptsForChecks/lt2CScale.sh)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/scriptsForChecks/tCheckerLiveness.sh)
