set(idl_files stage_services.idl)

install(FILES ${idl_files} DESTINATION ${RTC_INCLUDE_INSTALL_DIR}/idl
    COMPONENT headers)

