find_package(Doxygen) 
if(Doxygen_FOUND)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_EXTRACT_LOCAL_METHODS YES)
    doxygen_add_docs(
        marvin_docs 
        ${MARVIN_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin_NamespaceDocs.h
        USE_STAMP_FILE
    )
endif()