project(TKMessageView)

set (TKMessageView_DISABLE_COTIRE OFF)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_tool)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_tool)

# This project depends from Qt.
# Qt 5.11.2 is incompatible with c++20.
# So, it should be compiled with c++11/c++14 language standard.
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
