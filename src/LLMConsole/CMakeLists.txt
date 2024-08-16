project(LLMConsole LANGUAGES CXX C)

file(GLOB_RECURSE LLMCONSOLE_SRC_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE LLMCONSOLE_HEADER_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.hpp" "${PROJECT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE LLMCONSOLE_UI_FILES "${PROJECT_SOURCE_DIR}/*.ui")

add_library(${PROJECT_NAME}
    STATIC
        ${LLMCONSOLE_SRC_FILES}
        ${LLMCONSOLE_HEADER_FILES}
        ${LLMCONSOLE_UI_FILES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        Qt5::Core 
        Qt5::Gui 
        Qt5::Widgets
        Qt5::Network
        core
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        extern::openai
)


install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION
        ${PROJECT_NAME}
    LIBRARY DESTINATION
        ${PROJECT_NAME}
)

