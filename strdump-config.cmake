# Library configuration file used by dezlibndent projects
# via find_package() built-in directive in "config" mode.

if(NOT DEFINED STRDUMP_FOUND)

  # Locate library headers.
  FIND_PATH(STRDUMP_INCLUDE_DIRS 
    NAMES strdump.h
    PATHS ${STRDUMP_DIR}
    NO_DEFAULT_PATH
  )

  # Common name for exported library targets.
  SET(STRDUMP_LIBRARIES
    strdump
    CACHE INTERNAL "strdump library" FORCE
  )

  # Usual "required" et. al. directive logic.
  INCLUDE(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
    strdump DEFAULT_MSG
    STRDUMP_INCLUDE_DIRS
    STRDUMP_LIBRARIES
  )

  # Add targets to dezlibndent project.
  add_subdirectory(
    ${STRDUMP_DIR}
    ${CMAKE_BINARY_DIR}/strdump
  )

  SET(STRDUMP_LIBRARY ${STRDUMP_LIBRARIES})
  SET(STRDUMP_INCLUDE_DIR ${STRDUMP_INCLUDE_DIRS})
endif()
