XAML (UWP) sample

This sample was created to check possibility of OCCT compilation on Universal Windows Platform (UWP).
Note that only FoundationClasses, ModelingAlgorithms and ModelingData modules can be built at the moment.

Building OCCT and XAML (UWP) sample using CMake (since CMake version 3.4.0):
  - Run CMake, select source and binary directories for OCCT
  - Press "Configure" button
  - Select generator for this project - "Visual Studio 14 2015"
  - Select radio button "Specify toolchain file for cross-compiling" and press button "Next"
  - Specify absolute path to the Toolchain file "OCCT/adm/templates/uwp.toolchain.config.cmake" and press button "Finish"
  - After first configuration specify 3RDPARTY_DIR, INSTALL_DIR
  - Turn ON BUILD_MODULE_Uwp checkbox.
  - Press "Generate" button
  - Build OCCT and XAML (UWP) sample from Visual Studio as usual.

Troubleshooting:
  If you have got an error like this (appears after running the sample from Visual Studio):
    "Error : DEP3321 : To deploy this application, your deployment target should be running Windows Universal Runtime version 10.0.10240.0 or higher. You currently are running version 10.0.10166.0."
  Go to the properties of uwp sample project and in tab "General", set minimum deployment target to the lowest one.