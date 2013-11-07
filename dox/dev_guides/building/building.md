Building OCCT from sources {#dev_guides__building}
=========

In order to build OCCT libraries from these sources for use in your program, 
you need to:

1. Make sure you have all required third-party libraries installed (check 
   software requirements in \ref OCCT_OVW_SECTION_5 "Overview").

   See the following documents for short guide to installation of 
   third-party libraries on different platforms:
   - \subpage dev_guides__building_3rdparty_windows
   - \subpage dev_guides__building_3rdparty_linux
   - \subpage dev_guides__building_3rdparty_osx


2. If you use bare OCCT sources from Git repository or made some changes affecting
   CDL files or dependencies of OCCT toolkits, you need to update header files generated
   from \ref dev_guides__cdl "CDL", and regenerate build scripts for your environment using WOK.
   See \subpage dev_guides__building__wok for details.

   Skip to step 3 if you use complete source package (e.g. official OCCT 
   release) without changes in CDL.

3. Build using your preferred build tool.
   - \subpage dev_guides__building__automake "Building on Linux with Autotools"
   - \subpage dev_guides__building__cmake "Building with CMake (cross-platform)"
   - \subpage dev_guides__building__code_blocks "Building on Mac OS X with Code::Blocks"
   - \subpage dev_guides__building__msvc "Building on Windows with MS Visual Studio"
   - \subpage dev_guides__building__xcode "Building on Mac OS X with Xcode"

The current version of OCCT can be consulted in the file src/Standard/Standard_Version.hxx