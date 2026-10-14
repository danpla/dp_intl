add_executable(
    tests

    flow.cpp
    main.cpp
    test_lang_match.cpp
    test_lang_tag_utils.cpp
    test_translator.cpp)

get_target_property(LIB_TYPE dp_intl TYPE)
if(LIB_TYPE STREQUAL "SHARED_LIBRARY")
    # We can't link with internal plural::Eval, because its symbols
    # are hidden in the shared library.
    # TODO: Work around this.
    message(
        WARNING
        "Tests for internal plural::Eval are disabled because "
        "dp_intl is built as a shared library.")
else()
    target_sources(tests PRIVATE test_plural.cpp)
    target_include_directories(tests PRIVATE ../src)
endif()

set(FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/files")
configure_file("files_dir.cpp.in" "files_dir.cpp" @ONLY)
target_sources(
    tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/files_dir.cpp")

set_target_properties(
    tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(tests PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(tests PRIVATE .)

target_link_libraries(tests dp_intl::dp_intl)
