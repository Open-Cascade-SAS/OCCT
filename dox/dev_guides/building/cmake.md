Building with CMake {#dev_guides__building__cmake}
===================

This file describes steps to build OCCT libraries from complete source package
with CMake. CMake is free software that can create GNU Makefiles, KDevelop, 
XCode, and Visual Studio project files. Version 2.6 or above of CMake is 
required.

If you are building OCCT from bare sources (as in Git repository), or do some 
changes affecting CDL files, you need to use WOK to re-generate header files
and build scripts / projects. See file \ref wok "WOK" for instructions.

Before building OCCT, you need to install required third-party libraries; see
instructions for your platform on Resources page at http://dev.opencascade.org

1.  Decide on location of build and install directories.
    
    The build directory is the one where intermediate files will be created 
    (projects / makefiles, objects, binaries).
    The install directory is the one where binaries will be installed after 
    build, along with header files and resources required for OCCT use in 
    applications.
    
    OCCT CMake scripts assume use of separate build and one install directories
    for each configuration (Debug or Release).
    
    It is recommended to separate build and install directories from OCCT 
    source directory, for example:
    
       /user/home/occt/ros - sources
       /user/home/tmp/occt-build-release - intermediate files (release)
       /user/home/occt-install-release - installed binaries (release)
    
2.  Run CMake indicating path to OCCT sources (ros subdirectory) and selected build directory. 
    It is recommended to use GUI tools provided by CMake: cmake-gui on Windows
    and Mac, ccmake on Linux.
    
    Example:
    
       Linux> cd /user/home/occt-install-release
       Linux> ccmake /user/home/occt/ros
    
3. Run Configure
    
    You will likely get CMake errors due to missing paths to 3rd-party 
    libraries. This is normal; proceed with configuration as follows.
    
4. Check parameters shown by CMake, set them in accordance with your 
    environment, and repeat Configure until it runs without error:
    
    - 3RDPARTY_DIR: path to directory whethe 3rd-party libraries are installed
      (for the cases when they are not in default locations, or on Windows)
    - 3RDPARTY_USE_\<library\>: select to use optional libraries
    - Other options in 3RDPARTY group can be used to fine-tune paths to 
      3rd-party libraries
    
    - BUILD_TYPE: configuration to build (Debug or Release)
    - BUILD_BITNESS: bitness (32 or 64)
    - BUILD_TOOLKITS: optional string containing list of toolkits to be built
      in addition to those included in completely built modueles
    - BUILD_\<module\>: select to build corresponding OCCT module
    
    - INSTALL_DIR: directory to install OCCT
    - INSTALL_\<library\>: select to copy corresponding 3rd-party library to OCCT
      install dir

5. Run Generate

    This will create makefiles or project files for your build system.

6. Build OCCT:

    - on Windows with MSVC: open solution OCCT.sln and build it, when build project INSTALL_ALL explicitly to have binaries and headers installed
    - on Linux with make files: run 'make install'

 
 