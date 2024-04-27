find_package(Doxygen) 
if(Doxygen_FOUND)
    doxygen_add_docs(
        marvin_docs 
        ${MARVIN_HEADERS}
        USE_STAMP_FILE
    )
endif()