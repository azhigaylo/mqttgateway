# - Try to find automotive-dlt library
# -------------------------
# Once done this will define
#
# AutomotiveDlt_FOUND - system has automotive-dlt library
# AutomotiveDlt_VERSION - automotive-dlt library version
# AutomotiveDlt_INCLUDE_DIR - automotive-dlt include directory
# AutomotiveDlt_LIBRARY - automotive-dlt library

find_package(PkgConfig QUIET)

if (PkgConfig_FOUND)
    pkg_check_modules(_AUTOMOTIVE_DLT automotive-dlt REQUIRED)

    find_path(AutomotiveDlt_INCLUDE_DIR
        dlt/dlt.h
        ${_AUTOMOTIVE_DLT_INCLUDEDIR}
        NO_DEFAULT_PATH
        )

    find_library(AutomotiveDlt_LIBRARY
        NAMES dlt
        PATHS ${_AUTOMOTIVE_DLT_LIBRARY_DIRS}
        NO_DEFAULT_PATH  
        )

    if (_AUTOMOTIVE_DLT_VERSION)
        set(AutomotiveDlt_VERSION ${_AUTOMOTIVE_DLT_VERSION})
    else(_AUTOMOTIVE_DLT_VERSION)
        set(AutomotiveDlt_VERSION "")
        message("Couldn't determine automotive-dlt version.")
    endif(_AUTOMOTIVE_DLT_VERSION)
else(PkgConfig_FOUND)
    if (AutomotiveDlt_FIND_REQUIRED)
        message("Couldn't find automotive-dlt library. pkg-config required but NOT installed.")
    endif(AutomotiveDlt_FIND_REQUIRED)
endif(PkgConfig_FOUND)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(AutomotiveDlt
    FOUND_VAR AutomotiveDlt_FOUND
    REQUIRED_VARS AutomotiveDlt_LIBRARY AutomotiveDlt_INCLUDE_DIR
    VERSION_VAR AutomotiveDlt_VERSION
    )

mark_as_advanced(
    AutomotiveDlt_INCLUDE_DIR
    AutomotiveDlt_LIBRARY
    AutomotiveDlt_VERSION
    )
