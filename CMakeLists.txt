cmake_minimum_required(VERSION 3.4.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Contains definitions
include(MqttGtw)

project(${HOME_IO_GATEWAY} VERSION 1.0.0)

################################################################################

if (CMAKE_CROSSCOMPILING)
    message(STATUS "Cross compiling: TRUE")
    message(STATUS "Target: ${CMAKE_SYSTEM_NAME}")
    if (CMAKE_SYSTEM_PROCESSOR) 
        message(STATUS "Target system processor(or hardware): ${CMAKE_SYSTEM_PROCESSOR}")
    endif(CMAKE_SYSTEM_PROCESSOR)
    if (CMAKE_SYSTEM_VERSION)
        message(STATUS "Target OS version: ${CMAKE_SYSTEM_VERSION}")
    endif()
else()
    message(STATUS "Cross compiling: FALSE")
    message(STATUS "Target: Host 'Linux'")
endif()

################################################################################

# Include 'function(enable_cxx_compiler_flag_if_supported flag)'
include(EnableCxxCompilerFlag)

if(CMAKE_COMPILER_IS_GNUCXX)
    enable_cxx_compiler_flag_if_supported(-Werror=all)
    enable_cxx_compiler_flag_if_supported(-Werror=extra)
    enable_cxx_compiler_flag_if_supported(-Werror=shadow)
    enable_cxx_compiler_flag_if_supported(-Werror=cast-align)
    enable_cxx_compiler_flag_if_supported(-Werror=write-strings)
    enable_cxx_compiler_flag_if_supported(-Werror=inconsistent-missing-override)
else()
    message(FATAL_ERROR "not supported compiler '${CMAKE_CXX_COMPILER_ID}' ")
endif()

add_subdirectory(src)

