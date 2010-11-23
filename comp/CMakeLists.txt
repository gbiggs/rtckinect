set(comp_headers rtc.h
                 service_impl.h)

install(FILES ${comp_headers} DESTINATION ${RTC_INCLUDE_INSTALL_DIR}/comp
    COMPONENT headers)

