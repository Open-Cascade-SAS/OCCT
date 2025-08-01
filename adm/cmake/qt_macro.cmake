#qt

macro (FIND_QT_PACKAGE PROJECT_LIBRARIES_DEBUG PROJECT_LIBRARIES_RELEASE PROJECT_INCLUDES)

  if ("${3RDPARTY_QT_DIR}" STREQUAL "")
    message (FATAL_ERROR "Empty Qt dir")
  endif()

  if (${Qt5_FOUND})
    #message (STATUS "Qt5 cmake configuration")

    set(PROJECT_INCLUDES "${Qt5Widgets_INCLUDE_DIRS}" "${Qt5Quick_INCLUDE_DIRS}" "${Qt5Xml_INCLUDE_DIRS}")
    set(PROJECT_LIBRARIES_DEBUG "${Qt5Widgets_LIBRARIES}" "${Qt5Quick_LIBRARIES}" "${Qt5Xml_LIBRARIES}")
    set(PROJECT_LIBRARIES_RELEASE "${Qt5Widgets_LIBRARIES}" "${Qt5Quick_LIBRARIES}" "${Qt5Xml_LIBRARIES}")

    # processing *.ts files to generate *.qm
    find_package(Qt5LinguistTools)
    get_target_property(QT_LRELEASE_EXECUTABLE Qt5::lrelease LOCATION)
    mark_as_advanced(QT_LRELEASE_EXECUTABLE)

    GET_FILENAME_COMPONENT(QT_BINARY_DIR ${QT_LRELEASE_EXECUTABLE} DIRECTORY)
    MARK_AS_ADVANCED(QT_BINARY_DIR)
  else()
    #message (STATUS "Qt4 cmake configuration")
    set(PROJECT_INCLUDES ${QT_INCLUDES})
    if (WIN32)
      set(PROJECT_LIBRARIES_DEBUG "${3RDPARTY_QT_DIR}/lib/QtCored4.lib;${3RDPARTY_QT_DIR}/lib/QtGuid4.lib;${3RDPARTY_QT_DIR}/lib/QtXmld4.lib")
      set(PROJECT_LIBRARIES_RELEASE "${3RDPARTY_QT_DIR}/lib/QtCore4.lib;${3RDPARTY_QT_DIR}/lib/QtGui4.lib;${3RDPARTY_QT_DIR}/lib/QtXml4.lib")
    else()
      set(PROJECT_LIBRARIES_DEBUG "${3RDPARTY_QT_DIR}/lib/libQtCore.so;${3RDPARTY_QT_DIR}/lib/libQtGui.so;${3RDPARTY_QT_DIR}/lib/libQtXml.so")
      set(PROJECT_LIBRARIES_RELEASE "${3RDPARTY_QT_DIR}/lib/libQtCore.so;${3RDPARTY_QT_DIR}/lib/libQtGui.so;${3RDPARTY_QT_DIR}/lib/libQtXml.so")
    endif(WIN32)
    find_program(QT_LRELEASE_EXECUTABLE lrelease)
  endif()
endmacro()


macro (FIND_AND_WRAP_MOC_FILES HEADER_FILES GENERATED_MOC_FILES)
  set (GENERATED_MOC_FILES "")
  foreach (FILE ${HEADER_FILES})
    # processing only files where Q_OBJECT exists
    file(STRINGS "${FILE}" LINES REGEX "Q_OBJECT")
    if(LINES)
      unset (MOC_FILE)
      if (${Qt5_FOUND})
        qt5_wrap_cpp(MOC_FILE ${FILE})
      else()
        qt4_wrap_cpp(MOC_FILE ${FILE})
      endif()
      #message (STATUS "... Info: next MOC file ${MOC_FILE}")
      list(APPEND ${GENERATED_MOC_FILES} ${MOC_FILE})
     endif(LINES)
  endforeach (FILE)

endmacro()

macro (FIND_AND_WRAP_RESOURCE_FILE RESOURCE_FILE_NAME RCC_FILES)
  if(EXISTS "${RESOURCE_FILE_NAME}")
    if (${Qt5_FOUND})
      qt5_add_resources(RCC_FILES "${RESOURCE_FILE_NAME}")
    else()
      qt4_add_resources(RCC_FILES "${RESOURCE_FILE_NAME}")
      # suppress some GCC warnings coming from source files generated from .qrc resources
      if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
      endif()
    endif()
  endif()
endmacro()

macro (FIND_AND_WRAP_TS_FILE RESOURCE_FILE_NAME TARGET_FOLDER QM_FILES)
  if(EXISTS "${RESOURCE_FILE_NAME}")
    if (${Qt5_FOUND})
      qt5_add_translation(QM_FILES "${RESOURCE_FILE_NAME}")
    else()
      qt4_add_translation(QM_FILES "${RESOURCE_FILE_NAME}")
    endif()
  endif()
endmacro()

macro (FIND_AND_INSTALL_QT_RESOURCES OCCT_PACKAGE RESOURCE_FILES)
  # Package name is now the sample name (e.g., "Tutorial")
  set(PACKAGE_NAME ${OCCT_PACKAGE})
  
  # Get files from CMAKE variables with proper location
  set(ALL_FILES ${OCCT_${PACKAGE_NAME}_FILES})
  set(FILES_LOCATION ${OCCT_${PACKAGE_NAME}_FILES_LOCATION})
  
  # Filter for .ts and .qrc files with full paths
  set(TS_FILES)
  set(QRC_FILES)
  foreach(FILE ${ALL_FILES})
    if(FILE MATCHES ".+[.]ts$")
      list(APPEND TS_FILES "${FILES_LOCATION}/${FILE}")
    endif()
    if(FILE MATCHES ".+[.]qrc$")
      list(APPEND QRC_FILES "${FILES_LOCATION}/${FILE}")
    endif()
  endforeach()

  string (FIND "${OCCT_PACKAGE}" "/" _index)
  if (_index GREATER -1)
    math (EXPR _index "${_index}")
    string (SUBSTRING "${OCCT_PACKAGE}" 0 ${_index} OCCT_PACKAGE_NAME)
  else()
    set (OCCT_PACKAGE_NAME "${OCCT_PACKAGE}")
  endif(_index GREATER -1)

  #message("QRC files are: ${QRC_FILES} in ${OCCT_PACKAGE}")
  foreach (QRC_FILE ${QRC_FILES})
    if (EXISTS ${QRC_FILE})
      FIND_AND_WRAP_RESOURCE_FILE(${QRC_FILE} RCC_FILES)
      list (APPEND ${RESOURCE_FILES} "${RCC_FILES}")
    endif()
  endforeach()

  #message("TS files are: ${TS_FILES} in ${OCCT_PACKAGE}")
  foreach (TS_FILE ${TS_FILES})
    FIND_AND_WRAP_TS_FILE(${TS_FILE} "${TARGET_FOLDER}/${CURRENT_MODULE}" QM_FILES)
    if (EXISTS ${TS_FILE})
      list (APPEND ${RESOURCE_FILES} "${TS_FILE}")
      list (APPEND ${RESOURCE_FILES} "${QM_FILES}")
    endif()
  endforeach()

  foreach (QM_FILE ${QM_FILES})
    INSTALL(FILES ${QM_FILE} DESTINATION "${INSTALL_DIR_RESOURCE}/samples")
    #message("install *.qm files (${QM_FILE}) to: ${INSTALL_DIR_RESOURCE}/samples")
  endforeach (QM_FILE ${QM_FILES})
endmacro()

