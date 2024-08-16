project(core LANGUAGES CXX C)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/commands/*.cpp" "${PROJECT_SOURCE_DIR}/model/*.cpp")

add_library(${PROJECT_NAME}
    STATIC
        ${SRC_FILES}
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        ${PROJECT_SOURCE_DIR}/commands
        ${PROJECT_SOURCE_DIR}/model
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt5::Core
        Qt5::Widgets 
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION
        ${PROJECT_NAME}
    LIBRARY DESTINATION
        ${PROJECT_NAME}
)