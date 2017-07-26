##

if(3RDPARTY_MACRO_ALREADY_INCLUDED)
  return()
endif()
set(3RDPARTY_MACRO_ALREADY_INCLUDED 1)


macro (THIRDPARTY_PRODUCT PRODUCT_NAME HEADER_NAME LIBRARY_CSF_NAME LIBRARY_NAME_DEBUG_SUFFIX)

  if (NOT DEFINED INSTALL_${PRODUCT_NAME} AND BUILD_SHARED_LIBS)
    set (INSTALL_${PRODUCT_NAME} OFF CACHE BOOL "${INSTALL_${PRODUCT_NAME}_DESCR}")
  endif()

  if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_DIR)
    set (3RDPARTY_${PRODUCT_NAME}_DIR "" CACHE PATH "The directory containing ${PRODUCT_NAME}")
  endif()

  # include occt macros. compiler_bitness, os_wiht_bit, compiler
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

  # specify product folder in connectin with 3RDPARTY_DIR
  if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
    #CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_DIR 3RDPARTY_${PRODUCT_NAME}_DIR PATH "The directory containing ${PRODUCT_NAME}")

    if (NOT 3RDPARTY_${PRODUCT_NAME}_DIR OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_DIR}")
      FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" ${PRODUCT_NAME} ${PRODUCT_NAME}_DIR_NAME)
      if (${PRODUCT_NAME}_DIR_NAME)
        set (3RDPARTY_${PRODUCT_NAME}_DIR "${3RDPARTY_DIR}/${${PRODUCT_NAME}_DIR_NAME}" CACHE PATH "The directory containing ${PRODUCT_NAME}" FORCE)
      endif()
    endif()
  else()
    #set (3RDPARTY_${PRODUCT_NAME}_DIR "" CACHE PATH "The directory containing ${PRODUCT_NAME}" FORCE)
  endif()

  if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR)
    set (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR           "" CACHE PATH "the path of ${HEADER_NAME}")
  endif()

  separate_arguments (${LIBRARY_CSF_NAME})
  foreach (LIBRARY_NAME ${${LIBRARY_CSF_NAME}})
    string (REPLACE "." "" LIBRARY_NAME "${LIBRARY_NAME}")
    if (BUILD_SHARED_LIBS)
      if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME} OR NOT 3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME} OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME}}")
        set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME}               "" CACHE FILEPATH "${PRODUCT_NAME} library \"${LIBRARY_NAME}\"" FORCE)
      endif()

      if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME})
        set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME}           "" CACHE PATH "The directory containing ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"")
      endif()

      if (WIN32)
        if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME} OR NOT 3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME} OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME}}")
          set (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME}                 "" CACHE FILEPATH "${PRODUCT_NAME} shared library \"${LIBRARY_NAME}\"" FORCE)
        endif()
      endif()

      if (WIN32)
        if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME})
          set (3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME}             "" CACHE PATH "The directory containing ${PRODUCT_NAME} shared library \"${LIBRARY_NAME}\"")
        endif()
      endif()
    endif()

    # check 3RDPARTY_${PRODUCT_NAME}_ paths for consistency with specified 3RDPARTY_${PRODUCT_NAME}_DIR
    if (3RDPARTY_${PRODUCT_NAME}_DIR AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_DIR}")
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_${PRODUCT_NAME}_DIR 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR PATH "the path to ${PRODUCT_NAME}")
      if (BUILD_SHARED_LIBS)
        CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_${PRODUCT_NAME}_DIR 3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME} FILEPATH "the path to ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"")

        if (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME} AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME}}")
          get_filename_component (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME} "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME}}" PATH)
          set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME} "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME}}" CACHE PATH "The directory containing ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"" FORCE)
        else()
          CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_${PRODUCT_NAME}_DIR 3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME} PATH "The directory containing ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"")
        endif()

        if (WIN32)
          CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_${PRODUCT_NAME}_DIR 3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME} FILEPATH "the path to ${PRODUCT_NAME} shared library \"${LIBRARY_NAME}\"")

          if (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME} AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME}}")
            get_filename_component (3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME} "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME}}" PATH)
            set (3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME} "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME}}" CACHE PATH "The directory containing ${PRODUCT_NAME} shared library \"${LIBRARY_NAME}\"" FORCE)
          else()
            CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_${PRODUCT_NAME}_DIR 3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME} PATH "The directory containing ${PRODUCT_NAME} shared library \"${LIBRARY_NAME}\"")
          endif()
        endif()
      endif()
    endif()
  endforeach()
  # header
  if (NOT 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR}")

    # set 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR as notfound, otherwise find_library can't assign a new value to 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR
    set (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR "3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "the path to ${HEADER_NAME}" FORCE)

    if (3RDPARTY_${PRODUCT_NAME}_DIR AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_DIR}")
      find_path (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR NAMES ${HEADER_NAME}
                                                      PATHS ${3RDPARTY_${PRODUCT_NAME}_DIR}
                                                      PATH_SUFFIXES include inc
                                                      CMAKE_FIND_ROOT_PATH_BOTH
                                                      NO_DEFAULT_PATH)
    else()
      find_path (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR NAMES ${HEADER_NAME}
                                                      PATH_SUFFIXES include inc
                                                      CMAKE_FIND_ROOT_PATH_BOTH)
    endif()
  endif()

  if (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR}")
    list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR)

    set (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR "" CACHE FILEPATH "The path to ${HEADER_NAME}" FORCE)
  endif()

  foreach (LIBRARY_NAME ${${LIBRARY_CSF_NAME}})
    string (REPLACE "." "" LIBRARY_NAME_SUFFIX "${LIBRARY_NAME}")
    if (BUILD_SHARED_LIBS)
      # library
      if (NOT 3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX} OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}}")
        set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .so .dylib .a)

        set (${PRODUCT_NAME}_PATH_SUFFIXES lib)
        if (WIN32)
          set (${PRODUCT_NAME}_PATH_SUFFIXES ${${PRODUCT_NAME}_PATH_SUFFIXES} win${COMPILER_BITNESS}/${COMPILER}/lib)
        endif()
        if (ANDROID)
          set (${PRODUCT_NAME}_PATH_SUFFIXES ${${PRODUCT_NAME}_PATH_SUFFIXES} libs/${ANDROID_ABI})
        endif()

        # set 3RDPARTY_${PRODUCT_NAME}_LIBRARY as notfound, otherwise find_library can't assign a new value to 3RDPARTY_${PRODUCT_NAME}_LIBRARY
        set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX} "3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}-NOTFOUND" CACHE FILEPATH "The path to ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"" FORCE)

        if (3RDPARTY_${PRODUCT_NAME}_DIR AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_DIR}")
          find_library (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX} NAMES ${LIBRARY_NAME}
                                                                                PATHS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME}}" "${3RDPARTY_${PRODUCT_NAME}_DIR}"
                                                                                PATH_SUFFIXES ${${PRODUCT_NAME}_PATH_SUFFIXES}
                                                                                CMAKE_FIND_ROOT_PATH_BOTH
                                                                                NO_DEFAULT_PATH)
          if ("${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}}" STREQUAL "3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}-NOTFOUND")
            # find directory recursive
            FIND_SUBDIRECTORY (${3RDPARTY_${PRODUCT_NAME}_DIR} "${${PRODUCT_NAME}_PATH_SUFFIXES}" SUBDIR_NAME)
            if (NOT "${SUBDIR_NAME}" STREQUAL "")
              find_library (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX} NAMES ${LIBRARY_NAME_SUFFIX}
                                                           PATHS "${SUBDIR_NAME}"
                                                           PATH_SUFFIXES ${${PRODUCT_NAME}_PATH_SUFFIXES}
                                                           CMAKE_FIND_ROOT_PATH_BOTH
                                                           NO_DEFAULT_PATH)
            endif()
          endif()
        else()
          find_library (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX} NAMES ${LIBRARY_NAME}
                                                                                PATH_SUFFIXES ${${PRODUCT_NAME}_PATH_SUFFIXES}
                                                                                CMAKE_FIND_ROOT_PATH_BOTH)
        endif()

        if (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX} AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}}")
          get_filename_component (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX} "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}}" PATH)
          set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX} "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX}}" CACHE PATH "The directory containing ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"" FORCE)
        else()
          set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX} "" CACHE PATH "The directory containing ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"" FORCE)
        endif()
      endif()

      if (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX} AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX}}")
        list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX}}")
      else()
        list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX})

        set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX} "" CACHE FILEPATH "The path to ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"" FORCE)
      endif()

      # shared library
      if (WIN32)
        if (NOT 3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX} OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}}")

          set (CMAKE_FIND_LIBRARY_SUFFIXES .dll)

          # set 3RDPARTY_${PRODUCT_NAME}_DLL as notfound, otherwise find_library can't assign a new value to 3RDPARTY_${PRODUCT_NAME}_DLL
          set (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX} "3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}-NOTFOUND" CACHE FILEPATH "The path to ${PRODUCT_NAME} shared library \"${LIBRARY_NAME}\"" FORCE)

          if (3RDPARTY_${PRODUCT_NAME}_DIR AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_DIR}")
            find_library (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}  NAMES ${LIBRARY_NAME}
                                                                               PATHS "${3RDPARTY_${PRODUCT_NAME}_DIR}"
                                                                               PATH_SUFFIXES bin  win${COMPILER_BITNESS}/${COMPILER}/bin
                                                                               NO_DEFAULT_PATH)
            if (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX} STREQUAL "3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}-NOTFOUND")
              # find directory recursive
              FIND_SUBDIRECTORY (${3RDPARTY_${PRODUCT_NAME}_DIR} bin SUBDIR_NAME)
              if (NOT "${SUBDIR_NAME}" STREQUAL "")
                find_library (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX} NAMES ${LIBRARY_NAME_SUFFIX}
                                                             PATHS "${SUBDIR_NAME}"
                                                             PATH_SUFFIXES bin
                                                             NO_DEFAULT_PATH)
              endif()
            endif()
          else()
            find_library (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX} NAMES ${LIBRARY_NAME} PATH_SUFFIXES bin)
          endif()

          if (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX} AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}}")
            get_filename_component (3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX} "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}}" PATH)
            set (3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX} "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX}}" CACHE PATH "The directory containing ${PRODUCT_NAME} library \"${LIBRARY_NAME}\"" FORCE)
          else()
            set (3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX} "" CACHE PATH "The directory containing ${PRODUCT_NAME} shared library \"${LIBRARY_NAME}\"" FORCE)

            set (3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX} "" CACHE FILEPATH "${PRODUCT_NAME} shared library \"${LIBRARY_NAME}\"" FORCE)
          endif()
        endif()

        if (3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX} OR EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX}}")
          list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX}}")
        else()
          list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX})
        endif()
      endif()

      if (WIN32)
        set (3RDPARTY_${PRODUCT_NAME}_DLL_DIRS "")
      else()
        set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIRS "")
      endif()

      foreach (LIBRARY_NAME ${${LIBRARY_CSF_NAME}})
        string (REPLACE "." "" LIBRARY_NAME_SUFFIX "${LIBRARY_NAME}")
        if (WIN32)
          set (3RDPARTY_${PRODUCT_NAME}_DLL_DIRS "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX}};${3RDPARTY_${PRODUCT_NAME}_DLL_DIRS}")
          set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIRS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX}}")
        else()
          set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIRS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX}}:${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIRS}")
        endif()
      endforeach()

      set (USED_3RDPARTY_${PRODUCT_NAME}_DIRS "")

      if (INSTALL_${PRODUCT_NAME})
        OCCT_MAKE_OS_WITH_BITNESS()
        OCCT_MAKE_COMPILER_SHORT_NAME()

        if (WIN32)
          if (SINGLE_GENERATOR)
            install (FILES "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}}" DESTINATION "${INSTALL_DIR_BIN}")
          else()
            install (FILES "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}}"
                     CONFIGURATIONS Release
                     DESTINATION "${INSTALL_DIR_BIN}")
            install (FILES "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}}"
                     CONFIGURATIONS RelWithDebInfo
                     DESTINATION "${INSTALL_DIR_BIN}i")
            install (FILES "${3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX}}"
                     CONFIGURATIONS Debug
                     DESTINATION "${INSTALL_DIR_BIN}d")
          endif()
        else()
          get_filename_component(ABS_PATH ${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}} REALPATH)

          if ("${PRODUCT_NAME}" STREQUAL "FREEIMAGE")
            get_filename_component(FREEIMLIB ${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}} NAME)

            if (SINGLE_GENERATOR)
              install (FILES "${ABS_PATH}" DESTINATION "${INSTALL_DIR_LIB}" RENAME ${FREEIMLIB}.3)
            else()
              install (FILES "${ABS_PATH}"
                       CONFIGURATIONS Release
                       DESTINATION "${INSTALL_DIR_LIB}"
                       RENAME ${FREEIMLIB}.3)
              install (FILES "${ABS_PATH}"
                       CONFIGURATIONS RelWithDebInfo
                       DESTINATION "${INSTALL_DIR_LIB}i"
                       RENAME ${FREEIMLIB}.3)
              install (FILES "${ABS_PATH}"
                       CONFIGURATIONS Debug
                       DESTINATION "${INSTALL_DIR_LIB}d"
                       RENAME ${FREEIMLIB}.3)
            endif()
          endif()

          if("${PRODUCT_NAME}" STREQUAL "GL2PS")
            get_filename_component(GL2PSLIB ${3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX}} NAME)

            if (SINGLE_GENERATOR)
              install (FILES "${ABS_PATH}" DESTINATION "${INSTALL_DIR_LIB}" RENAME ${GL2PSLIB}.1)
            else()
              install (FILES "${ABS_PATH}"
                       CONFIGURATIONS Release
                       DESTINATION "${INSTALL_DIR_LIB}"
                       RENAME ${GL2PSLIB}.1)
              install (FILES "${ABS_PATH}"
                       CONFIGURATIONS RelWithDebInfo
                       DESTINATION "${INSTALL_DIR_LIB}i"
                       RENAME ${GL2PSLIB}.1)
              install (FILES "${ABS_PATH}"
                       CONFIGURATIONS Debug
                       DESTINATION "${INSTALL_DIR_LIB}d"
                       RENAME ${GL2PSLIB}.1)
            endif()
          endif()
        endif()
      else()
        # the library directory for using by the executable
      foreach (LIBRARY_NAME ${${LIBRARY_CSF_NAME}})
        string (REPLACE "." "" LIBRARY_NAME_SUFFIX "${LIBRARY_NAME}")
          if (WIN32)
            set (USED_3RDPARTY_${PRODUCT_NAME}_DIRS "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR_${LIBRARY_NAME_SUFFIX}};${USED_3RDPARTY_${PRODUCT_NAME}_DIRS}")
          else()
            set (USED_3RDPARTY_${PRODUCT_NAME}_DIRS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_${LIBRARY_NAME_SUFFIX}}:${USED_3RDPARTY_${PRODUCT_NAME}_DIRS}")
          endif()
        endforeach()
      endif()

      mark_as_advanced (3RDPARTY_${PRODUCT_NAME}_LIBRARY_${LIBRARY_NAME_SUFFIX} 3RDPARTY_${PRODUCT_NAME}_DLL_${LIBRARY_NAME_SUFFIX})
    endif()
  endforeach()
endmacro()

macro (COMPLIANCE_PRODUCT_CONSISTENCY LIBNAME)
  if (3RDPARTY_${LIBNAME}_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_DIR}")
    # include dir
    set (DOES_PATH_CONTAIN FALSE)
    if (3RDPARTY_${LIBNAME}_INCLUDE_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_INCLUDE_DIR}")
      string (REGEX MATCH "${3RDPARTY_${LIBNAME}_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_INCLUDE_DIR}")
    endif()
    if (NOT DOES_PATH_CONTAIN)
      set (3RDPARTY_${LIBNAME}_INCLUDE_DIR "" CACHE FILEPATH "The directory containing headers of ${LIBNAME}" FORCE)
    endif()

    if (BUILD_SHARED_LIBS)
      # library dir
      set (DOES_PATH_CONTAIN FALSE)
      if (3RDPARTY_${LIBNAME}_LIBRARY_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}")
        string (REGEX MATCH "${3RDPARTY_${LIBNAME}_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}")
      endif()
      if (NOT DOES_PATH_CONTAIN)
        set (3RDPARTY_${LIBNAME}_LIBRARY_DIR "" CACHE FILEPATH "The directory containing ${LIBNAME} library" FORCE)
      endif()

      # shared library dir
      if (WIN32)
        set (DOES_PATH_CONTAIN FALSE)
        if (3RDPARTY_${LIBNAME}_DLL_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_DLL_DIR}")
          string (REGEX MATCH "${3RDPARTY_${LIBNAME}_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_DLL_DIR}")
        endif()
        if (NOT DOES_PATH_CONTAIN)
          set (3RDPARTY_${LIBNAME}_DLL_DIR "" CACHE FILEPATH "The directory containing ${LIBNAME} shared library" FORCE)
        endif()
      endif()
    endif()
  endif()

  if (BUILD_SHARED_LIBS)
    # check library
    set (DOES_PATH_CONTAIN FALSE)
    if (3RDPARTY_${LIBNAME}_LIBRARY_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}")
      if (3RDPARTY_${LIBNAME}_LIBRARY AND EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY}")
        string (REGEX MATCH "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_LIBRARY}")
      endif()
    endif()
    if (NOT DOES_PATH_CONTAIN)
      set (3RDPARTY_${LIBNAME}_LIBRARY "" CACHE FILEPATH "${LIBNAME} library" FORCE)
    endif()

    # check shared library
    if (WIN32)
      set (DOES_PATH_CONTAIN FALSE)
      if (3RDPARTY_${LIBNAME}_DLL_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_DLL_DIR}")
        if (3RDPARTY_${LIBNAME}_DLL AND EXISTS "${3RDPARTY_${LIBNAME}_DLL}")
          string (REGEX MATCH "${3RDPARTY_${LIBNAME}_DLL_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_DLL}")
        endif()
      endif()
      if (NOT DOES_PATH_CONTAIN)
        set (3RDPARTY_${LIBNAME}_DLL "" CACHE FILEPATH "${LIBNAME} shared library" FORCE)
      endif()
    endif()
  endif()
endmacro()
