cmake_minimum_required(VERSION 3.5...3.26)

project(pybind1_Qt_Console LANGUAGES CXX C)

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


find_package(pybind11 REQUIRED)  # or `add_subdirectory(pybind11)`

find_package(Qt5 REQUIRED COMPONENTS Core Network Widgets LinguistTools Gui OpenGL Concurrent)

file(GLOB_RECURSE EMBEDDING_SRC_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/pythonEmbedding/*.cpp")
file(GLOB_RECURSE EMBEDDING_HEADER_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/pythonEmbedding/*.hpp" "${PROJECT_SOURCE_DIR}/pythonEmbedding/*.h")
file(GLOB_RECURSE EMBEDDING_UI_FILES "${PROJECT_SOURCE_DIR}/pythonEmbedding/*.ui")

add_library(pythonEmbedding
    STATIC
        ${EMBEDDING_SRC_FILES}
        ${EMBEDDING_HEADER_FILES}
        ${EMBEDDING_UI_FILES}
)

target_include_directories(pythonEmbedding
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/pythonEmbedding
)

target_link_libraries(pythonEmbedding
    PRIVATE 
        Qt5::Core 
        Qt5::Gui 
        Qt5::Widgets
        Qt5::Network
        pybind11::embed
)


file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE UI_FILES "${PROJECT_SOURCE_DIR}/src/*.ui")

qt5_add_resources(GUI_RESOURCES "${PROJECT_SOURCE_DIR}/src/resource/resources.qrc")

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${GUI_RESOURCES}
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/commands
        ${PROJECT_SOURCE_DIR}/src/model
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        Qt5::Core 
        Qt5::Gui 
        Qt5::Widgets
        Qt5::Network
        pybind11::embed
        pythonEmbedding
)

# execute windeployqt.exe
if(WIN32)
    set(WINDEPLOYQT_EXECUTABLE "${Qt5_BIN_DIR}/windeployqt.exe")
  
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/windeployqt"
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations --no-angle --dir "${PROJECT_BINARY_DIR}/windeployqt" "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_BINARY_DIR}/windeployqt" "${CUSTOMIZE_RUNTIME_OUTPUT_DIRECTORY}"
    )
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION
        ${PROJECT_NAME}
    LIBRARY DESTINATION
        ${PROJECT_NAME}
)

install(
    DIRECTORY
        "${PROJECT_BINARY_DIR}/windeployqt/"
    DESTINATION
        ${PROJECT_NAME}
)