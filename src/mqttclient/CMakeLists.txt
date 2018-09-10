cmake_minimum_required(VERSION 3.4.0)

project(mqttclient LANGUAGES CXX)

message("mqttclient...")

find_package(Boost REQUIRED COMPONENTS thread system)

if(Boost_FOUND)
    message("Boost_FOUND..")
else()
    message(FATAL_ERROR "Boost NOT FOUND")
endif()

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MqttDataHandler.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    Boost::boost
    Boost::thread
    )
