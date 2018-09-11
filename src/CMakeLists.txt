cmake_minimum_required(VERSION 3.4.0)

find_package(Boost REQUIRED COMPONENTS thread system)

if(Boost_FOUND)
    message("Boost_FOUND !!!!")
    message("Boost_INCLUDE_DIRS = '${Boost_INCLUDE_DIRS}'")
    message("Boost_LIBRARIES = '${Boost_LIBRARIES}'")
else()
    message(FATAL_ERROR "Boost NOT FOUND")
endif()

add_subdirectory(common)
add_subdirectory(parsers)
add_subdirectory(dataclient)
add_subdirectory(mqttclient)

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/MqttGateway.cpp
    )

target_link_libraries(${PROJECT_NAME}
    Boost::boost
    Boost::thread
    Boost::system
    )
