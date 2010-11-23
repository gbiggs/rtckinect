set(doc_files manual.pdf
              manual_jp.pdf)

install(FILES ${doc_files}
    DESTINATION share/doc/${PROJECT_NAME_LOWER} COMPONENT doc)

