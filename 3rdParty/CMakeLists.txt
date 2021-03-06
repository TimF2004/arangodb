# -*- mode: CMAKE; -*-

# ------------------------------------------------------------------------------
# External Projects used by ArangoDB
# ------------------------------------------------------------------------------

include(ExternalProject)

remove_definitions("-DUSE_ENTERPRISE=1")

################################################################################
## V8 and ICU
################################################################################

option(USE_PRECOMPILED_V8 "use a precompiled V8" OFF)


set(V8_SUB_DIR "v5.7.492.77")
set(V8_VERSION
  "5.7.492.77"
  CACHE INTERNAL
  "${PROJECT_NAME}: Version"
  FORCE
  )
set(V8_VERSION ${V8_VERSION} PARENT_SCOPE)
if (USE_PRECOMPILED_V8)

  set(V8_DIR ${PROJECT_SOURCE_DIR}/build/3rdParty/V8/${V8_SUB_DIR})
  set(V8_SRC_DIR ${PROJECT_SOURCE_DIR}/3rdParty/V8/${V8_SUB_DIR})

  set(V8_PROC_ARCH "x64")
  set(V8_TARGET_ARCH "${V8_PROC_ARCH}.release")

  set(V8_TARGET_DIR "${V8_DIR}/${V8_TARGET_ARCH}/obj.target")
  set(V8_BIN_DIR "${V8_TARGET_DIR}/src/")
  set(ICU_BIN_DIR "${V8_TARGET_DIR}/third_party/icu")

  set(ICU_VERSION
    "54.1"
    CACHE INTERNAL
    "ICU: Version"
    FORCE
  )

  set(ICU_INCLUDE_DIR
    ${V8_SRC_DIR}/third_party/icu/source/common;${V8_SRC_DIR}/third_party/icu/source/i18n;${V8_SRC_DIR}/third_party/icu/source/io
    CACHE INTERNAL
    "ICU: include path"
    FORCE
  )

  add_library(libicui18n-static IMPORTED STATIC)
  set_target_properties(libicui18n-static PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ICU_INCLUDE_DIR}"
    IMPORTED_LOCATION "${ICU_BIN_DIR}/libicui18n.a"
  )

  add_library(libicuuc-static IMPORTED STATIC)
  set_target_properties(libicuuc-static PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ICU_INCLUDE_DIR}"
    IMPORTED_LOCATION "${ICU_BIN_DIR}/libicuuc.a"
  )

  set(ICU_LIBS
    libicui18n-static libicuuc-static
    CACHE INTERNAL
    "ICU: Libraries"
    FORCE
  )

  set(V8_INCLUDE_DIR
    ${V8_SRC_DIR};${V8_SRC_DIR}/include
    CACHE INTERNAL
    "${PROJECT_NAME}: include path"
    FORCE
  )

  set(V8_LIBS
    "${V8_BIN_DIR}/libv8_base.a;${V8_BIN_DIR}/libv8_libbase.a;${V8_BIN_DIR}/libv8_libplatform.a;${V8_BIN_DIR}/libv8_nosnapshot.a"
    CACHE INTERNAL
    "${PROJECT_NAME}: Libraries"
    FORCE
  )
else ()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/V8)
  set(ICU_DT "${ICU_DT}" PARENT_SCOPE)
  set(ICU_DT_DEST "${ICU_DT_DEST}" PARENT_SCOPE)
  set(ICU_LIBRARY_DIR "${ICU_LIBRARY_DIR}" PARENT_SCOPE)
endif ()

################################################################################
## IResearch
################################################################################

if (USE_IRESEARCH)
  add_library(boost-static IMPORTED STATIC)
  set_target_properties(boost-static PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/boost/1.62.0"
    IMPORTED_LOCATION "boost_locale;boost_system;boost_thread"
  )

  set(Boost_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/boost/1.62.0") # explicitly set since suppressing Boost search

  # IResearch on MacOS requires a complete build of ICU for encoding conversion
  if(APPLE)
    set(ICU_LIBS icu-static CACHE INTERNAL "ICU: Libraries" FORCE) # use complete ICU build
    set(ICU_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/V8/v5.7.492.77/third_party/icu")
  elseif(MSVC)
    add_library(icu-shared STATIC IMPORTED GLOBAL) # empty unused library to satisfy IResearch shared dependencies
    add_library(icu-static STATIC IMPORTED GLOBAL)
    set_property(TARGET icu-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ICU_INCLUDE_DIR}")
    set_property(TARGET icu-static PROPERTY IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/build/Debug/lib/icui18n.lib")
    set_property(TARGET icu-static PROPERTY IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/build/Release/lib/icui18n.lib")
    set_property(TARGET icu-static PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_BINARY_DIR}/build/Release/lib/icui18n.lib")
    set_property(TARGET icu-static PROPERTY IMPORTED_LOCATION_MINSIZEREL "${CMAKE_CURRENT_BINARY_DIR}/build/Release/lib/icui18n.lib")
    set_property(TARGET icu-static PROPERTY INTERFACE_LINK_LIBRARIES_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/build/Debug/lib/icuuc.lib")
    set_property(TARGET icu-static PROPERTY INTERFACE_LINK_LIBRARIES_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/build/Release/lib/icuuc.lib")
    set_property(TARGET icu-static PROPERTY INTERFACE_LINK_LIBRARIES_RELWITHDEBINFO "${CMAKE_CURRENT_BINARY_DIR}/build/Release/lib/icuuc.lib")
    set_property(TARGET icu-static PROPERTY INTERFACE_LINK_LIBRARIES_MINSIZEREL "${CMAKE_CURRENT_BINARY_DIR}/build/Release/lib/icuuc.lib")
    set(ICU_FOUND TRUE) # ICU built from source in 3rdParty directory
  else()
    add_library(icu-shared STATIC IMPORTED GLOBAL) # empty unused library to satisfy IResearch shared dependencies
    add_library(icu-static STATIC IMPORTED GLOBAL)
    set_property(TARGET icu-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ICU_INCLUDE_DIR}")
    set_property(TARGET icu-static PROPERTY IMPORTED_LOCATION_DEBUG "${ICU_LIBRARY_DIR}/libicui18n.a")
    set_property(TARGET icu-static PROPERTY IMPORTED_LOCATION_RELEASE "${ICU_LIBRARY_DIR}/libicui18n.a")
    set_property(TARGET icu-static PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${ICU_LIBRARY_DIR}/libicui18n.a")
    set_property(TARGET icu-static PROPERTY IMPORTED_LOCATION_MINSIZEREL "${ICU_LIBRARY_DIR}/libicui18n.a")
    set_property(TARGET icu-static PROPERTY INTERFACE_LINK_LIBRARIES_DEBUG "${ICU_LIBRARY_DIR}/libicuuc.a")
    set_property(TARGET icu-static PROPERTY INTERFACE_LINK_LIBRARIES_RELEASE "${ICU_LIBRARY_DIR}/libicuuc.a")
    set_property(TARGET icu-static PROPERTY INTERFACE_LINK_LIBRARIES_RELWITHDEBINFO "${ICU_LIBRARY_DIR}/libicuuc.a")
    set_property(TARGET icu-static PROPERTY INTERFACE_LINK_LIBRARIES_MINSIZEREL "${ICU_LIBRARY_DIR}/libicuuc.a")
    set(ICU_FOUND TRUE) # ICU built from source in 3rdParty directory
  endif()

  if(NOT PERL_FOUND)
    set(PERL_FOUND TRUE) # suppress error for Perl not-found

    # MSVC will execute ADD_CUSTOM_COMMAND even though OUTPUT is already present
    if(MSVC)
      set(PERL_EXECUTABLE echo)
    endif()
  endif()

  set(LZ4_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/lz4")
  set(SNOWBALL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/snowball")
  set(UNWIND_ROOT "invalid")

  if(MSVC)
    set(USE_IQL OFF CACHE BOOL "Use IQL" FORCE) # skip IQL on MSVC until IResearch upstream is updated
  endif()

  set(IRESEARCH_EXCLUDE_STATIC_THIRD_PARTY_LIBS TRUE) # disable linking in of 3rd party libraries automatically
  find_package(IResearch REQUIRED) # set IRESEARCH_BUILD_DIR

  set(CMAKE_MACOSX_RPATH ON) # suppress cmake warning (use same value as cmake default)
  set(CMAKE_MODULE_PATH_ORIGINAL ${CMAKE_MODULE_PATH}) # remember CMAKE_MODULE_PATH
  list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake" # cmake overrides (must be first)
    "${IRESEARCH_ROOT}/cmake" # to find iResearch dependencies
  )

  # cmake will seemingly randomly delete files from ${IRESEARCH_BUILD_DIR} during configuration
  # so must copy required files only after the cmake configuration phase is complete, not via file(COPY ...)
  # this command must _always_ be executed because the subsequent cmake configuration runs will once again delete files
  if(MSVC)
    add_custom_command(OUTPUT "${IRESEARCH_BUILD_DIR}/copy_iresearch.build" # non-existent file
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${IRESEARCH_ROOT}.build/" "${IRESEARCH_BUILD_DIR}/"
    )
  else()
    # 'cmake -E copy_directory' does not preserve timestamps, use 'cp' directly where avalable
    add_custom_command(OUTPUT "${IRESEARCH_BUILD_DIR}/copy_iresearch.build" # non-existent file
      COMMAND cp -rp "${IRESEARCH_ROOT}.build" "${IRESEARCH_BUILD_DIR}/"
    )
  endif()

  add_custom_target("copy_iresearch.build" ALL
    DEPENDS "${IRESEARCH_BUILD_DIR}/copy_iresearch.build"
  )

  add_subdirectory("${IRESEARCH_ROOT}" "${IRESEARCH_BUILD_DIR}" EXCLUDE_FROM_ALL) # do not build unused targets
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_ORIGINAL}) # restore CMAKE_MODULE_PATH
  unset(BUILD_SHARED_LIBS) # otherwise ZLib (below) does not build on win32
  unset(BUILD_SHARED_LIBS CACHE) # otherwise ZLib (below) does not build on win32

  # IResearch on MacOS requires a complete build of ICU for encoding conversion
  if(APPLE)
    # cmake does not build ICU without this (cmake does not inherit dependencies)
    add_dependencies(v8_build icu-build icudata-static icui18n-static icuuc-static)
  endif()

  # ensure the 'copy_iresearch.build' target is executed before start of build of the 'iresearch' target
  if(MSVC)
    add_dependencies(iresearch-static-scrt "copy_iresearch.build") # use static + staticCRT to match ArangoDB
  else()
    add_dependencies(iresearch-static "copy_iresearch.build") # use static to match ArangoDB
  endif()
endif()

################################################################################
## ZLIB
################################################################################

set(SKIP_INSTALL_ALL ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/zlib/zlib-1.2.11)

################################################################################
## cURL
################################################################################

set(ENABLE_MANUAL OFF CACHE type BOOL)
set(BUILD_CURL_EXE OFF CACHE type BOOL)
set(BUILD_TESTING OFF CACHE type BOOL)
set(CURL_STATICLIB ON CACHE type BOOL)
set(HTTP_ONLY ON CACHE type BOOL)
set(CMAKE_USE_LIBSSH2 OFF CACHE type BOOL)
set(CMAKE_USE_OPENSSL ON CACHE type BOOL)
# mop: super important...if this is off curl will not handle request timeouts < 1000ms
set(ENABLE_THREADED_RESOLVER ON CACHE type BOOL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/curl/curl-7.57.0)
set(CURL_CONFIG_HAS_BEEN_RUN_BEFORE true PARENT_SCOPE)

################################################################################
## SNAPPY
################################################################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/snappy/google-snappy-d53de18)

################################################################################
## JEMALLOC
################################################################################

if (USE_JEMALLOC)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/jemalloc)
  include_directories("${JEMALLOC_HOME}/include")
  link_directories("${JEMALLOC_HOME}/lib")
  set(SYS_LIBS ${SYS_LIBS} ${JEMALLOC_LIB})
endif ()

################################################################################
## BOOST
################################################################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/boost)
set(BOOST_VERSION ${BOOST_VERSION} PARENT_SCOPE)

################################################################################
## ROCKSDB
################################################################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rocksdb)
include_directories(${ROCKSDB_INCLUDE_DIR})

################################################################################
## LINK_DIRECTORIES
################################################################################

set(LINK_DIRECTORIES "${LINK_DIRECTORIES}" PARENT_SCOPE)

set(STARTER_SOURCE "${PROJECT_SOURCE_DIR}/3rdParty/arangodb-starter/")
set(STARTER_BUILD "${CMAKE_CURRENT_BINARY_DIR}/arangodb-starter/")
set(STARTER_BINARY "${STARTER_BUILD}/arangodb${CMAKE_EXECUTABLE_SUFFIX}")
if (EXISTS ${STARTER_SOURCE})
  find_program(GO_COMMAND "go")
  if (GO_COMMAND)
    MESSAGE("-- building arangodb starter.")
    ExternalProject_Add(arangodb_starter_build
      SOURCE_DIR
      ${STARTER_SOURCE}
      CONFIGURE_COMMAND
      ""
      BINARY_DIR
      ${STARTER_BUILD}
      BUILD_COMMAND
      $(MAKE) -f "${STARTER_SOURCE}Makefile" "NODOCKER=1" "BUILDDIR=${STARTER_BUILD}" "SCRIPTDIR=${STARTER_SOURCE}" local
      LOG_INSTALL 1
      INSTALL_DIR
      ${CMAKE_INSTALL_BINDIR}
      INSTALL_COMMAND
      ${CMAKE_COMMAND} -E copy ${STARTER_BINARY} ${CMAKE_INSTALL_BINDIR}
      )
    list(APPEND THIRDPARTY_BIN ${STARTER_BINARY})
    set(THIRDPARTY_BIN ${THIRDPARTY_BIN} PARENT_SCOPE)
  else()
    MESSAGE(WARNING "arangodb starter source present, but no go command to build it found.")
  endif()
endif()
