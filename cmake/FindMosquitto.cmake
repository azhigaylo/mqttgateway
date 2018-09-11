# - Try to find mosquittopp library
# -------------------------
# Once done this will define

find_package(PkgConfig QUIET)

if (PkgConfig_FOUND)
    pkg_check_modules(_MOSQUITTO_PP mosquittopp QUIET)

    find_path(mosquittopp_INCLUDE_DIR
        mosquittopp.h
        ${_MOSQUITTO_PP_INCLUDEDIR}
        )

    find_library(mosquittopp_LIBRARY
        NAMES mosquittopp
        PATHS ${_MOSQUITTO_PP_LIBRARY_DIRS}
        )
        

    message("mosquittopp LIBRARY = ${mosquittopp_LIBRARY}")
    message("mosquittopp INCLUDE_DIR =  ${mosquittopp_INCLUDE_DIR}")

else(PkgConfig_FOUND)
    if (_MOSQUITTO_PP_FIND_REQUIRED)
        message("Couldn't find mosquittopp library. pkg-config required but NOT installed.")
    endif(_MOSQUITTO_PP_FIND_REQUIRED)
endif(PkgConfig_FOUND)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(mosquittopp
    FOUND_VAR mosquittopp_FOUND
    REQUIRED_VARS mosquittopp_LIBRARY mosquittopp_INCLUDE_DIR
    )

mark_as_advanced(
    mosquittopp_INCLUDE_DIR
    mosquittopp_LIBRARY
    )
