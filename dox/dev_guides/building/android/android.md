Building with CMake and ADT for Android {#occt_dev_guides__building_android}
===================

@tableofcontents

This file describes the steps to build OCCT libraries from a complete source package
with following tools:
  - Generation of eclipse project files
    - CMake v3.0+ http://www.cmake.org/cmake/resources/software.html
    - Cross-compilation toolchain for CMake https://github.com/taka-no-me/android-cmake
    - Android NDK rev.10+ https://developer.android.com/tools/sdk/ndk/index.html
    - Make: MinGW v4.82+ for Windows, GNU Make vXX for Linux. http://sourceforge.net/projects/mingw/files/
  - Building eclipse project files of OCCT
    - Android Developer Tools (ADT) v22+ https://developer.android.com/sdk/index.html

## Generation of eclipse project files
Run GUI tool provided by CMake: cmake-gui

### Tools configuration
  - Specify the root folder of OCCT (<i>$CASROOT</i>, which contains *CMakelists.txt* file) by clicking **Browse Source**.
  - Specify the location (build folder) for Cmake generated project files by clicking **Browse Build**.

@figure{/dev_guides/building/android/images/android_image001.png}

Click **Configure** button. It opens the window with a drop-down list of generators supported by CMake project. - 

Select "Eclipse CDT4 - MinGW MakeFiles" item from the list
  - Choose "Specify toolchain file for cross-compiling"
  - Click "Next"
@figure{/dev_guides/building/android/images/android_image002.png}
  - Specify the Toolchain file at next dialog by android.toolchain.cmake is contained by cross-compilation toolchain for CMake
  - Click "Finish"
@figure{/dev_guides/building/android/images/android_image003.png}

Add cache entry ANDROID_NDK - path (entry type is PATH) to the NDK folder ("Add Entry" button)
@figure{/dev_guides/building/android/images/android_image004.png}

if there is message "CMake Error: CMake was unable to find a build program corresponding to "MinGW Makefiles".  CMAKE_MAKE_PROGRAM is not set.  You probably need to select a different build tool."  
Define CMAKE_MAKE_PROGRAM variable with the file path to mingw32-make executable. 
@figure{/dev_guides/building/android/images/android_image005.png}

### OCCT Configuration

How to configure OCCT, see "OCCT Configuration" section of @ref occt_dev_guides__building_cmake "Building with CMake"
taking into account the specific configuration variables for android:
  - ANDROID_ABI = armeabi-v7a
  - ANDROID_NATIVE_API_LEVEL = 15
  - ANDROID_NDK_LAYOUT is equal to BUILD_CONFIGURATION variable 
  - CMAKE_ECLIPSE_VERSION is equal to installed eclipse version (e.g., 4.2)

Configure parallel building:
  - CMAKE_ECLIPSE_MAKE_ARGUMENTS = -jN where N = your numbers of CPU cores (threads)

@figure{/dev_guides/building/android/images/android_image006.png}

### Generation of eclipse projects files

Click **Generate** button and wait until the generation process is finished. 
Then the eclipse project files will appear in the build folder (e.g. <i> D:/android-build-eclipse-api-15 </i>).

## Building eclipse project files of OCCT

  - Open eclipse (downloaded ADT bundle contains it)
  @figure{/dev_guides/building/android/images/android_image007.png}
  - Import "Existing project into Workspace"
  @figure{/dev_guides/building/android/images/android_image008.png}
  @figure{/dev_guides/building/android/images/android_image009.png}
  - Build ALL
  @figure{/dev_guides/building/android/images/android_image010.png}
  
When the building process has finished, libraries are located in \<build folder\>/out (e.g. <i> D:/android-build-eclipse-api-15/out </i>).