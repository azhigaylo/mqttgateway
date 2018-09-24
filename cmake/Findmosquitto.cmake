# - Try to find mosquitto library
# -------------------------
# Once done this will define

find_package(PkgConfig QUIET)

if (PkgConfig_FOUND)
    pkg_check_modules(_MOSQUITTO mosquitto QUIET)

    find_path(mosquitto_INCLUDE_DIR
        mosquitto.h
        ${_MOSQUITTO_INCLUDE_DIR}
        )

    find_library(mosquitto_LIBRARY
        NAMES libmosquitto mosquitto
        PATHS ${_MOSQUITTO_LIBRARY_DIRS}
        )

    message("mosquitto LIBRARY = ${mosquitto_LIBRARY}")
    message("mosquitto INCLUDE_DIR =  ${mosquitto_INCLUDE_DIR}")

else(PkgConfig_FOUND)
    if (_MOSQUITTO_FIND_REQUIRED)
        message("pkg-config required but NOT installed.")
    endif(_MOSQUITTO_FIND_REQUIRED)
endif(PkgConfig_FOUND)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(mosquitto
    FOUND_VAR mosquitto_FOUND
    REQUIRED_VARS mosquitto_INCLUDE_DIR mosquitto_LIBRARY
    )

mark_as_advanced(
    mosquitto_INCLUDE_DIR
    mosquitto_LIBRARY
    )
