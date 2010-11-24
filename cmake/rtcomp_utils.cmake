macro(LIST_TO_STRING _string _list)
    set(${_string})
    foreach(_item ${_list})
        set(${_string} "${${_string}} ${_item}")
    endforeach(_item)
endmacro(LIST_TO_STRING)


macro(FILTER_LIST _list _pattern _output)
    set(${_output})
    foreach(_item ${_list})
        if("${_item}" MATCHES ${_pattern})
            set(${_output} ${${_output}} ${_item})
        endif("${_item}" MATCHES ${_pattern})
    endforeach(_item)
endmacro(FILTER_LIST)


###############################################################################
# This macro processes a list of arguments into separate lists based on
# keywords found in the argument stream. For example:
# BUILDBLAG (misc_arg INCLUDEDIRS /usr/include LIBDIRS /usr/local/lib
#            LINKFLAGS -lthatawesomelib CFLAGS -DUSEAWESOMELIB SOURCES blag.c)
# Any other args found at the start of the stream will go into the variable
# specified in _other_args. Typically, you would take arguments to your macro
# as normal, then pass ${ARGN} to this macro to parse the dynamic-length
# arguments (so if ${_otherArgs} comes back non-empty, you've ignored something
# or the user has passed in some arguments without a keyword).
macro(PROCESS_ARGUMENTS _sources_args _include_dirs_args _lib_dirs_args _link_libs_args _link_flags_args _cflags_args _idl_args _other_args)
    set(${_sources_args})
    set(${_include_dirs_args})
    set(${_lib_dirs_args})
    set(${_link_libs_args})
    set(${_link_flags_args})
    set(${_cflags_args})
    set(${_idl_args})
    set(${_other_args})
    set(_current_dest ${_other_args})
    foreach(_arg ${ARGN})
        if(_arg STREQUAL "SOURCES")
            set(_current_dest ${_sources_args})
        elseif(_arg STREQUAL "INCLUDEDIRS")
            set(_current_dest ${_include_dirs_args})
        elseif(_arg STREQUAL "LIBDIRS")
            set(_current_dest ${_lib_dirs_args})
        elseif(_arg STREQUAL "LINKLIBS")
            set(_current_dest ${_link_libs_args})
        elseif(_arg STREQUAL "LINKFLAGS")
            set(_current_dest ${_link_flags_args})
        elseif(_arg STREQUAL "CFLAGS")
            set(_current_dest ${_cflags_args})
        elseif(_arg STREQUAL "IDL")
            set(_current_dest ${_idl_args})
        else(_arg STREQUAL "SOURCES")
            list(APPEND ${_current_dest} ${_arg})
        endif(_arg STREQUAL "SOURCES")
    endforeach(_arg)
endmacro(PROCESS_ARGUMENTS)


macro(IDL_OUTPUTS _idl _dir _result)
    set(${_result} ${_dir}/${_idl}.hh ${_dir}/${_idl}SK.cc
        ${_dir}/${_idl}DynSK.cc)
endmacro(IDL_OUTPUTS)


macro(COMPILE_INTF_IDL _idl _idl_dir)
    execute_process(COMMAND rtm-config --idlc OUTPUT_VARIABLE _idl_compiler
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT _idl_compiler)
        message(FATAL_ERROR "Could not find IDL compiler.")
    endif(NOT _idl_compiler)
    execute_process(COMMAND rtm-config --idlflags OUTPUT_VARIABLE _idlc_flags
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    separate_arguments(_idlc_flags)
    execute_process(COMMAND rtm-config --prefix OUTPUT_VARIABLE _rtm_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(RTM_IDL_DIR ${_rtm_prefix}/include/rtm/idl CACHE STRING
        "Directory containing the OpenRTM-aist IDL files.")
    set(_idl_srcs_var _${_idl}_SRCS)
    IDL_OUTPUTS(${_idl} ${PROJECT_BINARY_DIR}/idl ${_idl_srcs_var})
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/idl)
    add_custom_command(OUTPUT ${${_idl_srcs_var}} COMMAND ${_idl_compiler}
        ${_idlc_flags} -I${RTM_IDL_DIR} ${_idl_dir}/${_idl}.idl
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/idl
        DEPENDS ${_idl_dir}/${_idl}.idl
        COMMENT "Compiling ${_idl}.idl" VERBATIM)
    set(_idl_srcs ${_idl_srcs} ${${_idl_srcs_var}})
endmacro(COMPILE_INTF_IDL)


macro(COMPILE_IDL_FILES _idl_dir)
    set(_idl_srcs)
    foreach(idl ${ARGN})
        COMPILE_INTF_IDL(${idl} ${_idl_dir})
    endforeach(idl)
endmacro(COMPILE_IDL_FILES)


macro(INSTALL_IDL_FILES _idl_dir _dest_dir)
    foreach(idl ${ARGN})
        install(FILES ${_idl_dir}/${idl}.idl DESTINATION ${_dest_dir}
            COMPONENT IDL)
    endforeach(idl)
endmacro(INSTALL_IDL_FILES)


include(FindPkgConfig)
macro(GET_PKG_CONFIG_INFO _pkg _required)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(${_pkg}_PKG ${_required} ${_pkg})
        if(${_pkg}_PKG_CFLAGS_OTHER)
            LIST_TO_STRING(${_pkg}_CFLAGS "${${_pkg}_PKG_CFLAGS_OTHER}")
        else(${_pkg}_PKG_CFLAGS_OTHER)
            set(${_pkg}_CFLAGS "")
        endif(${_pkg}_PKG_CFLAGS_OTHER)
        set(${_pkg}_INCLUDE_DIRS ${${_pkg}_PKG_INCLUDE_DIRS})
        set(${_pkg}_LINK_LIBS ${${_pkg}_PKG_LIBRARIES})
        set(${_pkg}_LIBRARY_DIRS ${${_pkg}_PKG_LIBRARY_DIRS})
        if(${_pkg}_PKG_LDFLAGS_OTHER)
            LIST_TO_STRING(${_pkg}_LINK_FLAGS ${${_pkg}_PKG_LDFLAGS_OTHER})
        else(${_pkg}_PKG_LDFLAGS_OTHER)
            set(${_pkg}_LINK_FLAGS "")
        endif(${_pkg}_PKG_LDFLAGS_OTHER)
    else(PKG_CONFIG_FOUND)
        message(STATUS "Could not find pkg-config.")
        message(STATUS
            "You will need to set the following variables manually:")
        message(STATUS "${_pkg}_INCLUDE_DIRS ${_pkg}_CFLAGS_OTHER ${_pkg}_LINK_LIBS ${_pkg}_LIBRARY_DIRS ${_pkg}_LINK_FLAGS")
    endif(PKG_CONFIG_FOUND)
endmacro(GET_PKG_CONFIG_INFO)


macro(GET_OS_INFO)
    if(WIN32)
        message(FATAL_ERROR "This component does not support Windows.")
    endif(WIN32)
    string(REGEX MATCH "Linux" OS_IS_LINUX ${CMAKE_SYSTEM_NAME})
    if(OS_IS_LINUX)
        if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
            set(RTC_LIB_INSTALL_DIR "lib64")
        else(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
            set(RTC_LIB_INSTALL_DIR "lib")
        endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    endif(OS_IS_LINUX)
    set(RTC_INCLUDE_INSTALL_DIR
        "include/${PROJECT_NAME_LOWER}-${RTC_MAJOR_VERSION}.${RTC_MINOR_VERSION}")
endmacro(GET_OS_INFO)


macro(DISSECT_VERSION)
    # Find version components
    string(REGEX REPLACE "^([0-9]+).*" "\\1"
        RTC_MAJOR_VERSION "${RTC_VERSION}")
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1"
        RTC_MINOR_VERSION "${RTC_VERSION}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1"
        RTC_REVISION_VERSION ${RTC_VERSION})
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1"
        RTC_CANDIDATE_VERSION ${RTC_VERSION})
endmacro(DISSECT_VERSION)

