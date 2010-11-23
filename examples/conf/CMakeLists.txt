set(comp_conf_in ${CMAKE_CURRENT_SOURCE_DIR}/comp.conf.in)
set(comp_conf ${CMAKE_CURRENT_BINARY_DIR}/comp.conf)
configure_file(${comp_conf_in} ${comp_conf})

set(example_conf_files rtc.conf
                       ${comp_conf})

install(FILES ${example_conf_files}
    DESTINATION share/${PROJECT_NAME_LOWER}/examples/conf COMPONENT examples)

