Building OCCT from sources {#occt_dev_guides__building}
=========

In order to build OCCT libraries from these sources for use in your program, 
you need to:

1. Make sure you have all required third-party libraries installed (check 
   software requirements in \ref OCCT_OVW_SECTION_5 "Overview").

   See the following documents for short guide to installation of 
   third-party libraries on different platforms:
   - \subpage occt_dev_guides__building_3rdparty_windows
   - \subpage occt_dev_guides__building_3rdparty_linux
   - \subpage occt_dev_guides__building_3rdparty_osx


2. Build using your preferred build tool.
   - \subpage occt_dev_guides__building_automake "Building on Linux with Autotools"
   - \subpage occt_dev_guides__building_cmake "Building with CMake (cross-platform)"
   - \subpage occt_dev_guides__building_android "Building with CMake for Android (cross-platform)"
   - \subpage occt_dev_guides__building_code_blocks "Building on Mac OS X with Code::Blocks"
   - \subpage occt_dev_guides__building_msvc "Building on Windows with MS Visual Studio"
   - \subpage occt_dev_guides__building_xcode "Building on Mac OS X with Xcode"

The current version of OCCT can be consulted in the file src/Standard/Standard_Version.hxx