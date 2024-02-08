cmake_minimum_required(VERSION 3.5...3.26)

project(example LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
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
  message(" >> WINDEPLOYQT_EXECUTABLE")
  message(" WINDEPLOYQT_EXECUTABLE >> ${WINDEPLOYQT_EXECUTABLE}")
  
  add_custom_command(TARGET ${CMAKE_PROJECT_NAME}
  POST_BUILD
          COMMAND set PATH="${Qt5_BIN_DIR}"
          COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
)
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION
        ${PROJECT_NAME}
    LIBRARY DESTINATION
        ${PROJECT_NAME}
)