cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(rtckinect)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

include(${PROJECT_SOURCE_DIR}/cmake/rtcomp_utils.cmake)

# Set version and OS-specific settings
set(RTC_VERSION 1.0.0 CACHE STRING "RTC:Kinect version")
DISSECT_VERSION()
GET_OS_INFO()

# Add an "uninstall" target
CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/cmake/uninstall_target.cmake.in"
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET (uninstall "${CMAKE_COMMAND}" -P
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake")

# Component source files
set(comp_srcs comp/rtc.cpp)
    #comp/service_impl.cpp)
set(standalone_srcs comp/standalone.cpp)
set(idls)

# Compile the built-in IDL files
COMPILE_IDL_FILES(${PROJECT_SOURCE_DIR}/idl ${idls})
FILTER_LIST("${_idl_srcs}" "hh$" _installable_idl_srcs)
install(FILES ${_installable_idl_srcs}
    DESTINATION ${RTC_INCLUDE_INSTALL_DIR}/idl COMPONENT headers)

# Get necessary dependency information
GET_PKG_CONFIG_INFO(openrtm-aist REQUIRED)
set(libfreenect_dir "/usr/local" CACHE PATH
    "Directory where libfreenect is installed.")
GET_PKG_CONFIG_INFO(libusb REQUIRED)

# Component target
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
if(openrtm-aist_INCLUDE_DIRS)
    include_directories(${openrtm-aist_INCLUDE_DIRS})
endif(openrtm-aist_INCLUDE_DIRS)
if(openrtm-aist_LIBRARY_DIRS)
    link_directories(${openrtm-aist_LIBRARY_DIRS})
endif(openrtm-aist_LIBRARY_DIRS)
if(openrtm-aist_CFLAGS)
    set_source_files_properties(${comp_srcs} ${proxy_srcs}
        PROPERTIES COMPILE_FLAGS "${openrtm-aist_CFLAGS}")
endif(openrtm-aist_CFLAGS)
include_directories(${libfreenect_dir}/include)
link_directories(${libfreenect_dir}/lib64 ${libfreenect_dir}/lib)
if(libusb_INCLUDE_DIRS)
    include_directories(${libusb_INCLUDE_DIRS})
endif(libusb_INCLUDE_DIRS)
if(libusb_LIBRARY_DIRS)
    link_directories(${libusb_LIBRARY_DIRS})
endif(libusb_LIBRARY_DIRS)
if(libusb_CFLAGS)
    set_source_files_properties(${comp_srcs} ${proxy_srcs}
        PROPERTIES COMPILE_FLAGS "${libusb_CFLAGS}")
endif(libusb_CFLAGS)
include_directories("/usr/include/libusb-1.0")

add_library(rtckinect SHARED ${comp_srcs} ${proxy_srcs} ${_idl_srcs})
add_executable(rtckinect_standalone ${standalone_srcs})

target_link_libraries(rtckinect_standalone rtckinect)
if(openrtm-aist_LINK_FLAGS)
    set_target_properties(rtckinect PROPERTIES
        LINK_FLAGS "${openrtm-aist_LINK_FLAGS}")
endif(openrtm-aist_LINK_FLAGS)
if(openrtm-aist_LINK_LIBS)
    target_link_libraries(rtckinect ${openrtm-aist_LINK_LIBS})
endif(openrtm-aist_LINK_LIBS)
if(libusb_LINK_FLAGS)
    set_target_properties(rtckinect PROPERTIES
        LINK_FLAGS "${libusb_LINK_FLAGS}")
endif(libusb_LINK_FLAGS)
if(libusb_LINK_LIBS)
    target_link_libraries(rtckinect ${libusb_LINK_LIBS})
endif(libusb_LINK_LIBS)
target_link_libraries(rtckinect_standalone freenect)

install(TARGETS rtckinect LIBRARY DESTINATION ${RTC_LIB_INSTALL_DIR}
    COMPONENT component)
install(TARGETS rtckinect_standalone RUNTIME DESTINATION bin
    COMPONENT component)

# Subdirectories
add_subdirectory(cmake)
add_subdirectory(comp)
#add_subdirectory(doc)
#add_subdirectory(examples)
#add_subdirectory(idl)

# Package creation
include(InstallRequiredSystemLibraries)
set(RTC_DESCRIPTION "RT-Component for the Kinect depth sensor.")
set(RTC_VENDOR "Geoffrey Biggs, AIST")
set(RTC_EXECUTABLES "rtckinect_standalone" "RTC:Kinect")
configure_file("${PROJECT_SOURCE_DIR}/cmake/cpack_options.cmake.in"
    "${PROJECT_BINARY_DIR}/cpack_options.cmake" @ONLY)
set(CPACK_PROJECT_CONFIG_FILE "${PROJECT_BINARY_DIR}/cpack_options.cmake")
#include(cpack)

