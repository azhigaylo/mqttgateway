include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported FLAG)
    string(FIND "${CMAKE_CXX_FLAGS}" "${FLAG}" FLAG_ALREDY_SET)
    if(FLAG_ALREDY_SET EQUAL -1)
        set(FLAG_TO_TEST "test_${FLAG}_cxx_flag")
        # Remove '=' characters in variable name
        # to prevent interpreting variable by cmake as 'key=value'
        string(REPLACE "=" "_" FLAG_TO_TEST ${FLAG_TO_TEST})

        # test if not tested yet
        if(NOT DEFINED ${FLAG_TO_TEST})
            check_cxx_compiler_flag("${FLAG}" ${FLAG_TO_TEST})
            if (${FLAG_TO_TEST})
                set(${FLAG_TO_TEST} ${${FLAG_TO_TEST}} CACHE BOOL "Supported CXX compiler flag" FORCE)
            else()
                set(${FLAG_TO_TEST} ${${FLAG_TO_TEST}} CACHE BOOL "Unsupported CXX compiler flag" FORCE)
            endif()
        endif()

        # append to CMAKE_CXX_FLAGS if supported
        if(${FLAG_TO_TEST})
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}" PARENT_SCOPE)
        endif()
    endif()
endfunction()
