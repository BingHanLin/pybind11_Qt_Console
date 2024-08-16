project(pythonEmbedding LANGUAGES CXX C)

file(GLOB_RECURSE EMBEDDING_SRC_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/commands/*.cpp" "${PROJECT_SOURCE_DIR}/console/*.cpp")
file(GLOB_RECURSE EMBEDDING_HEADER_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/commands/*.hpp" "${PROJECT_SOURCE_DIR}/commands/*.h" "${PROJECT_SOURCE_DIR}/console/*.h" "${PROJECT_SOURCE_DIR}/console/*.h")
file(GLOB_RECURSE EMBEDDING_UI_FILES "${PROJECT_SOURCE_DIR}/commands/*.ui" "${PROJECT_SOURCE_DIR}/console/*.ui")

add_library(${PROJECT_NAME}
    STATIC
        ${EMBEDDING_SRC_FILES}
        ${EMBEDDING_HEADER_FILES}
        ${EMBEDDING_UI_FILES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/commands
        ${PROJECT_SOURCE_DIR}/console
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
        extern::pybind11_embed
)


install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION
        ${PROJECT_NAME}
    LIBRARY DESTINATION
        ${PROJECT_NAME}
)
