project(extern LANGUAGES CXX C)


# openai
find_package(CURL REQUIRED)

add_library(openai
    INTERFACE
)

add_library(${PROJECT_NAME}::openai ALIAS openai)

target_include_directories(openai
    INTERFACE 
        ${PROJECT_SOURCE_DIR}/openai
)

target_link_libraries(openai
    INTERFACE
        CURL::libcurl
)

# pybind11
find_package(pybind11 REQUIRED)

add_library(pybind11_embed
    INTERFACE
)

add_library(${PROJECT_NAME}::pybind11_embed ALIAS pybind11_embed)


target_link_libraries(pybind11_embed
    INTERFACE
        pybind11::embed
)

