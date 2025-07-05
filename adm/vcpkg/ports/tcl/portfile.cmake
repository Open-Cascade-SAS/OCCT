vcpkg_from_sourceforge(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO tcl/Tcl
    REF 8.6.16
    FILENAME tcl8.6.16-src.tar.gz
    SHA512 434c92f8181fb8dca6bc065b0f1f5078779086f19adf008818c90a3108596c63465ef43e9f3c1cfb3d4151a9de244d0bf0e6ee5b40e714b1ddca4a78eb43050b
    PATCHES
        0001-Support-Tk.patch
)

set(USE_TCL_TK OFF)

if ("tk" IN_LIST FEATURES)
  vcpkg_from_sourceforge(
      OUT_SOURCE_PATH TK_SOURCE_PATH
      REPO tcl/Tcl
      REF 8.6.16
      FILENAME tk8.6.16-src.tar.gz
      SHA512 b7d37bee25f826f156137a04859ac756c682f1dd155ec9629119dc3690509ce1b6e308e23b291f2debbc10f3b1650993fea66463e5445c505860a10acac901d0
  )

  # Copy TK to TCL package source path (SOURCE_PATH/extra/tk) if TK is used
  if (NOT EXISTS "${SOURCE_PATH}/extra/tk.8.6.16-src")
    file(MAKE_DIRECTORY "${SOURCE_PATH}/extra/tk.8.6.16-src")
    file(COPY ${TK_SOURCE_PATH}/ DESTINATION "${SOURCE_PATH}/extra/tk.8.6.16-src")
  endif()
  set (USE_TCL_TK ON)
endif()

# Copy TK to TCL package source path if TK is used

# Use Windows NMAKE build for MSVC, but Unix build for MinGW
if (VCPKG_TARGET_IS_WINDOWS AND NOT VCPKG_TARGET_IS_MINGW)
   if(VCPKG_TARGET_ARCHITECTURE MATCHES "x64")
        set(TCL_BUILD_MACHINE_STR MACHINE=AMD64)
        set(TCL_BUILD_ARCH_STR ARCH=AMD64)
    elseif(VCPKG_TARGET_ARCHITECTURE MATCHES "arm")
        set(TCL_BUILD_MACHINE_STR MACHINE=ARM64)
        set(TCL_BUILD_ARCH_STR ARCH=ARM64)
    elseif(VCPKG_TARGET_ARCHITECTURE MATCHES "x86")
        set(TCL_BUILD_MACHINE_STR MACHINE=IX86)
        set(TCL_BUILD_ARCH_STR ARCH=IX86)
    else()
        # Default fallback for unknown architectures
        set(TCL_BUILD_MACHINE_STR MACHINE=IX86)
        set(TCL_BUILD_ARCH_STR ARCH=IX86)
    endif()
    
    # Handle features
    set(TCL_BUILD_OPTS OPTS=)
    set(TCL_BUILD_STATS STATS=none)
    set(TCL_BUILD_CHECKS CHECKS=none)
    if (VCPKG_LIBRARY_LINKAGE STREQUAL static)
        set(TCL_BUILD_OPTS ${TCL_BUILD_OPTS},static,staticpkg)
    endif()
    if (VCPKG_CRT_LINKAGE STREQUAL dynamic)
        set(TCL_BUILD_OPTS ${TCL_BUILD_OPTS},msvcrt)
    endif()
    
    if ("thrdalloc" IN_LIST FEATURES)
        set(TCL_BUILD_OPTS ${TCL_BUILD_OPTS},thrdalloc)
    endif()
    if ("profile" IN_LIST FEATURES)
        set(TCL_BUILD_OPTS ${TCL_BUILD_OPTS},profile)
    endif()
    if ("unchecked" IN_LIST FEATURES)
        set(TCL_BUILD_OPTS ${TCL_BUILD_OPTS},unchecked)
    endif()
    if ("utfmax" IN_LIST FEATURES)
        set(TCL_BUILD_OPTS ${TCL_BUILD_OPTS},time64bit)
    endif()
    
    vcpkg_install_nmake(
        SOURCE_PATH ${SOURCE_PATH}
        PROJECT_SUBPATH win
        OPTIONS
            ${TCL_BUILD_MACHINE_STR}
            ${TCL_BUILD_ARCH_STR}
            ${TCL_BUILD_STATS}
            ${TCL_BUILD_CHECKS}
            TKDIR=../extra/tk.8.6.16-src
        OPTIONS_DEBUG
            ${TCL_BUILD_OPTS},symbols,pdbs
            INSTALLDIR=${CURRENT_PACKAGES_DIR}/debug
        OPTIONS_RELEASE
            ${TCL_BUILD_OPTS}
            INSTALLDIR=${CURRENT_PACKAGES_DIR}
    )

    if(VCPKG_LIBRARY_LINKAGE STREQUAL static)
        file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
    endif()

    file (REMOVE ${CURRENT_PACKAGES_DIR}/bin/zlib1.dll)
    file (REMOVE ${CURRENT_PACKAGES_DIR}/debug/bin/zlib1.dll)
      file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
else()
    set (TCL_PROJECT_SUBPATH unix)
    if (VCPKG_TARGET_IS_MINGW)
        set (TCL_PROJECT_SUBPATH win)
    endif()
    # file(REMOVE "${SOURCE_PATH}/${TCL_PROJECT_SUBPATH}/configure")
    # For MinGW and other Unix-like environments on Windows, use unix build path
    # MinGW can use either win/ (with MinGW-compatible Makefiles) or unix/ (with autotools)
    vcpkg_configure_make(
        SOURCE_PATH "${SOURCE_PATH}"
        PROJECT_SUBPATH ${TCL_PROJECT_SUBPATH}
        AUTOCONFIG
        OPTIONS
            TKDIR=${SOURCE_PATH}/extra/tk.8.6.16-src
    )

    vcpkg_install_make(
    )

    vcpkg_fixup_pkgconfig()

    if(VCPKG_LIBRARY_LINKAGE STREQUAL static)
        file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
    endif()
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")
endif()

file(REMOVE "${CURRENT_PACKAGES_DIR}/lib/tclConfig.sh" "${CURRENT_PACKAGES_DIR}/debug/lib/tclConfig.sh")

# Remove TK configuration files if TK was built
if(USE_TCL_TK)
    file(REMOVE "${CURRENT_PACKAGES_DIR}/lib/tkConfig.sh" "${CURRENT_PACKAGES_DIR}/debug/lib/tkConfig.sh")
endif()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/license.terms")
