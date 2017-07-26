project(TInspectorEXE)

set (USE_QT ON)
set (EXECUTABLE_PROJECT ON)

OCCT_INCLUDE_CMAKE_FILE_STANDALONE ("adm/cmake/occt_toolkit_standalone")

unset (USE_QT)
unset (EXECUTABLE_PROJECT)

configure_file(${CMAKE_SOURCE_DIR}/adm/templates/${PROJECT_NAME}.vcxproj.user.in 
               ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user @ONLY)

