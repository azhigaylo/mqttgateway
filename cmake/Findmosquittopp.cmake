# - Try to find mosquitto library
# -------------------------
# Once done this will define

find_package(PkgConfig QUIET)

if (PkgConfig_FOUND)
    pkg_check_modules(_MOSQUITTO mosquittopp QUIET)

    find_path(mosquittopp_INCLUDE_DIR
        mosquittopp.h
        ${_MOSQUITTO_INCLUDE_DIR}
        )

    find_library(mosquittopp_LIBRARY
        NAMES mosquittopp
        PATHS ${_MOSQUITTO__LIBRARIES}
        )

    message("mosquitto c++ LIBRARY = ${mosquittopp_LIBRARY}")
    message("mosquitto c++ INCLUDE_DIR =  ${mosquittopp_INCLUDE_DIR}")

else(PkgConfig_FOUND)
    if (_MOSQUITTO_FIND_REQUIRED)
        message("pkg-config required but NOT installed.")
    endif(_MOSQUITTO_FIND_REQUIRED)
endif(PkgConfig_FOUND)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(mosquitto
    FOUND_VAR mosquitto_FOUND
    REQUIRED_VARS mosquittopp_INCLUDE_DIR mosquittopp_LIBRARY
    )

mark_as_advanced(
    mosquittopp_INCLUDE_DIR
    mosquittopp_LIBRARY
    )
