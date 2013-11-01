Building OCCT Libraries {#dev_guides__building}
=========

The source package of the Open CASCADE Technology including the source files of samples
and tools and the set of building procedures is available for self-dependent preparation
binary files on UNIX and Windows platforms. 

In order to build OCCT libraries from these sources for use in your program, 
you need to:

1. Install the required third-party libraries.

   Follow the instructions provided in the documents titled "Building 3rd party
   products for OCCT" on http://dev.opencascade.org/?q=home/resources for
   choice of the needed libraries, their installation and building.

2. If you use OCCT sources from Git repository or do come changes affecting
   CDL files or dependencies of OCCT toolkit, update header files generated 
   from CDL, and regenerate build scripts for your environment using WOK.
   See \subpage dev_guides__building__wok "WOK" for details.

   Skip to step 3 if you use complete source package (e.g. official OCCT 
   release) without changes in CDL.

3. Build using your preferred build tool.
   - \subpage dev_guides__building__automake "Building on Linux with Autotools"
   - \subpage dev_guides__building__cmake "Building with CMake (cross-platform)"
   - \subpage dev_guides__building__code_blocks "Building on Mac OS X with Code::Blocks"
   - \subpage dev_guides__building__msvc "Building on Windows with MS Visual Studio 2005-2012"
   - \subpage dev_guides__building__xcode "Building on Mac OS X with Xcode"

The current version of OCCT can be consulted in the file src/Standard/Standard_Version.hxx