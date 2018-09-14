cmake_minimum_required(VERSION 3.4.0)

project(gateway)

message("gateway...")

find_package(Boost REQUIRED COMPONENTS thread)

if(!Boost_FOUND)
    message(FATAL_ERROR "Boost NOT FOUND")
endif()

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MqttGatewayImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GtwItemFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DigitalGtwItem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnalogGtwItem.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    Boost::boost
    Boost::thread
    common
    parsers
    )
