project(gui LANGUAGES CXX C)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.hpp" "${PROJECT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE UI_FILES "${PROJECT_SOURCE_DIR}/*.ui")

qt5_add_resources(GUI_RESOURCES "${PROJECT_SOURCE_DIR}/resource/resources.qrc")

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${GUI_RESOURCES}
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        Qt5::Core 
        Qt5::Gui 
        Qt5::Widgets
        Qt5::Network
        pythonEmbedding
        LLMChat
        core
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