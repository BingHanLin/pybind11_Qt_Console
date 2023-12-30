cmake_minimum_required(VERSION 3.5...3.26)

project(example LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)  # or `add_subdirectory(pybind11)`

find_package(Qt5 REQUIRED COMPONENTS Core Network Widgets LinguistTools Gui OpenGL Concurrent)

add_executable(example
    main.cpp
    mainWindow.cpp
    mainWindow.hpp
    commands.hpp
    pythonTerminal.cpp
    pythonTerminal.hpp
    pythonCommands.hpp
)

target_include_directories(${CMAKE_PROJECT_NAME} 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}
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