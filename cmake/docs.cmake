find_package(Doxygen REQUIRED)

set(DOXYGEN_QUIET YES)
set(DOXYGEN_STRIP_FROM_PATH "build" "include")
set(DOXYGEN_STRIP_FROM_INC_PATH "include")
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)
set(DOXYGEN_SORT_MEMBER_DOCS NO)
set(DOXYGEN_JAVADOC_AUTOBRIEF YES)

set(DOXYGEN_SHOW_INCLUDE_FILES NO)
set(DOXYGEN_VERBATIM_HEADERS NO)
set(DOXYGEN_ALPHABETICAL_INDEX NO)
set(DOXYGEN_HTML_COLORSTYLE LIGHT)
set(DOXYGEN_HTML_COPY_CLIPBOARD NO)
set(DOXYGEN_HTML_CODE_FOLDING NO)
set(DOXYGEN_MARKDOWN_ID_STYLE GITHUB)
set(DOXYGEN_GENERATE_TREEVIEW NO)
set(DOXYGEN_SEARCHENGINE NO)
set(DOXYGEN_HAVE_DOT NO)
set(DOXYGEN_CLASS_GRAPH TEXT)

set(DOXYGEN_HTML_EXTRA_STYLESHEET
    "${CMAKE_CURRENT_LIST_DIR}/doxygen_extra.css")

set(DOXYGEN_LAYOUT_FILE
    "${CMAKE_CURRENT_LIST_DIR}/doxygen_layout.xml")

set(README_PATH "${CMAKE_CURRENT_SOURCE_DIR}/readme.md")
set(MAIN_PAGE_PATH "${CMAKE_CURRENT_BINARY_DIR}/doxygen-main-page.md")

add_custom_command(
    OUTPUT "${MAIN_PAGE_PATH}"
    COMMAND
        "${CMAKE_COMMAND}"
        -D "README_PATH=${README_PATH}"
        -D "MAIN_PAGE_PATH=${MAIN_PAGE_PATH}"
        -P "${CMAKE_CURRENT_LIST_DIR}/doxygen_gen_main_page.cmake"
    DEPENDS "${README_PATH}"
    VERBATIM)

add_custom_target(doxygen_main_page DEPENDS "${MAIN_PAGE_PATH}")

set(LICENSE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
set(LICENSE_PAGE_PATH
    "${CMAKE_CURRENT_BINARY_DIR}/doxygen-licnese-page.md")

add_custom_command(
    OUTPUT "${LICENSE_PAGE_PATH}"
    COMMAND
        "${CMAKE_COMMAND}"
        -D "LICENSE_PATH=${LICENSE_PATH}"
        -D "LICENSE_PAGE_PATH=${LICENSE_PAGE_PATH}"
        -P "${CMAKE_CURRENT_LIST_DIR}/doxygen_gen_license_page.cmake"
    DEPENDS "${LICENSE_PATH}"
    VERBATIM)

add_custom_target(doxygen_license_page DEPENDS "${LICENSE_PAGE_PATH}")

doxygen_add_docs(
    docs
    include/dp_intl/count_t.h
    include/dp_intl/error.h
    include/dp_intl/file_stream.h
    include/dp_intl/lang_match.h
    include/dp_intl/lang_tag_utils.h
    include/dp_intl/stream.h
    include/dp_intl/translator.h
    include/dp_intl/translator_plural_spec.h
    "${MAIN_PAGE_PATH}"
    "${LICENSE_PAGE_PATH}"
    ALL
    USE_STAMP_FILE)

add_dependencies(docs doxygen_main_page doxygen_license_page)
