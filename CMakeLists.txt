cmake_minimum_required(VERSION 3.5...3.26)

project(example LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)  # or `add_subdirectory(pybind11)`

find_package(Qt5 REQUIRED COMPONENTS Core Network Widgets LinguistTools Gui OpenGL Concurrent)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/shonTACommon/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE UI_FILES "${PROJECT_SOURCE_DIR}/src/*.ui")

add_executable(example
    ${SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
)

target_include_directories(${CMAKE_PROJECT_NAME} 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/commands
        ${PROJECT_SOURCE_DIR}/src/model
        ${PROJECT_SOURCE_DIR}/src/pybind11Embedding
)


target_link_libraries(${CMAKE_PROJECT_NAME} 
    PRIVATE 
        Qt5::Core 
        Qt5::Gui 
        Qt5::Widgets
        Qt5::Network
        pybind11::embed
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