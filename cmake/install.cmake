configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/install_prefix_check.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/install_prefix_check.cmake"
    @ONLY)
install(
    SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/install_prefix_check.cmake")

include(GNUInstallDirs)

target_include_directories(
    dp_intl
    INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(
    TARGETS dp_intl
    EXPORT dp_intl_targets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(
    DIRECTORY include/dp_intl
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(
    FILES changes.txt license.txt readme.md
    DESTINATION "${CMAKE_INSTALL_DOCDIR}")

set(DP_INTL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/dp_intl")
install(
    EXPORT dp_intl_targets
    NAMESPACE dp_intl::
    DESTINATION "${DP_INTL_CMAKE_DIR}")

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/package_config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/dp_intl-config.cmake"
    INSTALL_DESTINATION "${DP_INTL_CMAKE_DIR}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/dp_intl-config-version.cmake"
    COMPATIBILITY SameMajorVersion)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/dp_intl-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dp_intl-config-version.cmake"
    DESTINATION "${DP_INTL_CMAKE_DIR}")

export(
    EXPORT dp_intl_targets
    NAMESPACE dp_intl::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/dp_intl_targets.cmake")

# We use get_property() instead of get_target_property() because the
# latter sets the variable to <variable>-NOTFOUND instead of an empty
# string and therefore requires an extra step for cleanup.
get_property(
    PKG_CONFIG_DEFS
    TARGET dp_intl
    PROPERTY INTERFACE_COMPILE_DEFINITIONS)
list(TRANSFORM PKG_CONFIG_DEFS PREPEND "-D")

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/pkg-config.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/dp_intl.pc"
    @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/dp_intl.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
