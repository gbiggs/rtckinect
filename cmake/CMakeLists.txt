set(cmake_modules rtcomp_utils.cmake)

install(FILES ${cmake_modules} DESTINATION share/cmake/Modules
    COMPONENT cmakestuff)

set(PKG_DESC "RT-Component for the Kinect depth sensor.")
set(PKG_EXTERNAL_DEPS "openrtm-aist")
set(pkg_conf_file ${CMAKE_CURRENT_BINARY_DIR}/rtckinect.pc)
configure_file(rtcomp.pc.in ${pkg_conf_file} @ONLY)
install(FILES ${pkg_conf_file}
    DESTINATION ${RTC_LIB_INSTALL_DIR}/pkgconfig/ COMPONENT pkgconfig)

