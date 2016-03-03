Building OCCT from sources {#occt_dev_guides__building}
=========

Before building OCCT, make sure to have all the required third-party libraries installed.
The list of required libraries depends on what OCCT modules will be used, and your preferences.
The typical minimum is **Freetype** (necessary for Visualization) and **Tcl/Tk** (for DRAW Test Harness).
See "Third-party libraries" section in \ref OCCT_OVW_SECTION_5 "Overview" for a full list.

On Windows, the easiest way to install third-party libraries is to download archive with pre-built binaries from http://www.opencascade.com/content/3rd-party-components.
On Linux and OS X, it is recommended to use the version installed in the system natively.

You can also build third-party libraries from their sources:
* \subpage occt_dev_guides__building_3rdparty_windows
* \subpage occt_dev_guides__building_3rdparty_linux
* \subpage occt_dev_guides__building_3rdparty_osx

Build OCCT using your preferred build tool.
* \subpage occt_dev_guides__building_cmake "Building with CMake (cross-platform)"
* \subpage occt_dev_guides__building_android "Building with CMake for Android (cross-platform)"
* \subpage occt_dev_guides__building_msvc "Building on Windows with MS Visual Studio projects"
* \subpage occt_dev_guides__building_code_blocks "Building on Mac OS X with Code::Blocks projects"
* \subpage occt_dev_guides__building_xcode "Building on Mac OS X with Xcode projects"

The current version of OCCT can be consulted in the file src/Standard/Standard_Version.hxx