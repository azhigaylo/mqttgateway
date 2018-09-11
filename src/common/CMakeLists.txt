cmake_minimum_required(VERSION 3.1)

project(common LANGUAGES CXX)

message("common...")

find_package(AutomotiveDlt 2.15)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/slog.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${AutomotiveDlt_INCLUDE_DIR}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    ${AutomotiveDlt_LIBRARY}
    )
