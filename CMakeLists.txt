cmake_minimum_required(VERSION 3.5...3.26)

project(pybind11_Qt_Console LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CUSTOMIZE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CUSTOMIZE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CUSTOMIZE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive")

if (MSVC) # only need for multi config build system
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        message(STATUS OUTPUTCONFIG....${OUTPUTCONFIG})
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CUSTOMIZE_RUNTIME_OUTPUT_DIRECTORY})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CUSTOMIZE_LIBRARY_OUTPUT_DIRECTORY})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CUSTOMIZE_ARCHIVE_OUTPUT_DIRECTORY})
    endforeach()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CUSTOMIZE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CUSTOMIZE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CUSTOMIZE_ARCHIVE_OUTPUT_DIRECTORY})

# Generate clangd configuration(for developing enviroment only)
if(MSVC)
    file(WRITE ${PROJECT_SOURCE_DIR}/.clangd "CompileFlags:\n")
    file(APPEND ${PROJECT_SOURCE_DIR}/.clangd "    CompilationDatabase: ${CMAKE_BINARY_DIR}\n")
    file(APPEND ${PROJECT_SOURCE_DIR}/.clangd "    Add:\n")

    set(MSVC_STANDARD_INCLUDE_PATHS "$ENV{INCLUDE}")
    foreach(ONE_PATH ${MSVC_STANDARD_INCLUDE_PATHS})
        file(APPEND ${PROJECT_SOURCE_DIR}/.clangd "        - \"-I${ONE_PATH}\"\n")
    endforeach()
endif()




find_package(Qt5 REQUIRED COMPONENTS Core Network Widgets LinguistTools Gui OpenGL Concurrent)

add_subdirectory(extern)
add_subdirectory(src/core)
add_subdirectory(src/pythonEmbedding)
add_subdirectory(src/LLMConsole)
add_subdirectory(src/gui)