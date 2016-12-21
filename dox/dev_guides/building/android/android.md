Building with CMake for Android {#occt_dev_guides__building_android}
===================

@tableofcontents

This article describes the steps to build OCCT libraries for Android from a complete source package
with GNU make (makefiles) on Windows 7 and Ubuntu 15.10.

The steps on Windows and Ubuntu are similar. There is the only one difference: makefiles are built with mingw32-make
on Windows and native GNU make on Ubuntu.

Required tools (download and install if it is required):
  - CMake v3.7+ http://www.cmake.org/cmake/resources/software.html
  - Android NDK rev.10+ https://developer.android.com/tools/sdk/ndk/index.html
  - GNU Make: MinGW v4.82+ for Windows (http://sourceforge.net/projects/mingw/files/), GNU Make 4.0 for Ubuntu. 

## Prerequisites

In toolchain file <i>$CASROOT/adm/templates/android.toolchain.config.cmake</i>:

  - Set CMAKE_ANDROID_NDK variable equal to your Android NDK path.
  - Set CMAKE_ANDROID_STL_TYPE variable to specify which C++ standard library to use.

The default value of CMAKE_ANDROID_STL_TYPE is <i>gnustl_shared</i> (GNU libstdc++ Shared)

@figure{/dev_guides/building/android/images/android_image001.png}

## Generation of makefiles using CMake GUI tool
Run GUI tool provided by CMake: cmake-gui

### Tools configuration
  - Specify the root folder of OCCT (<i>$CASROOT</i>, which contains *CMakelists.txt* file) by clicking **Browse Source**.
  - Specify the location (build folder) for Cmake generated project files by clicking **Browse Build**.

@figure{/dev_guides/building/android/images/android_image002.png}

Click **Configure** button. It opens the window with a drop-down list of generators supported by CMake project.

Select "MinGW Makefiles" item from the list
  - Choose "Specify toolchain file for cross-compiling"
  - Click "Next"

@figure{/dev_guides/building/android/images/android_image003.png}

  - Specify a toolchain file at the next dialog by <i>android.toolchain.config.cmake</i> . It is contained by cross-compilation toolchain for CMake
  - Click "Finish"
@figure{/dev_guides/building/android/images/android_image004.png}

If on Windows the message is appeared: "CMake Error: CMake was unable to find a build program corresponding to "MinGW Makefiles"
CMAKE_MAKE_PROGRAM is not set.  You probably need to select a different build tool.",
specify **CMAKE_MAKE_PROGRAM** to mingw32-make executable.
@figure{/dev_guides/building/android/images/android_image005.png}

### OCCT Configuration

How to configure OCCT, see "OCCT Configuration" section of @ref occt_dev_guides__building_cmake "Building with CMake"

### Generation of makefiles

Click **Generate** button and wait until the generation process is finished. 
Then makefiles will appear in the build folder (e.g. <i> D:/occt/build-android </i>).

## Generation of makefiles using CMake from the command line

Alternatively one may specify the values without a toolchain file:

> cmake -G "MinGW Makefiles" -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=D:/DevTools/android-ndk-r13b -DCMAKE_ANDROID_STL_TYPE=gnustl_shared -DCMAKE_SYSTEM_VERSION=15 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_MAKE_PROGRAM=D:/DevTools/MinGW/bin/mingw32-make.exe -D3RDPARTY_DIR=D:/occt-3rdparty D:/occt

@figure{/dev_guides/building/android/images/android_image006.png}

## Building makefiles of OCCT

Open console and go to the build folder. Type "mingw32-make" (Windows) or "make" (Ubuntu) to start build process.

> mingw32-make
or
> make


Parallel building can be started with using **"-jN"** argument of "mingw32-make/make", where N is the number of building threads.

> mingw32-make -j4
or
> make -j4

## Install built OCCT libraries

Type "mingw32-make/make" with argument "install" to place the libraries 
to the install folder (see "OCCT Configuration" section of @ref occt_dev_guides__building_cmake "Building with CMake")

> mingw32-make install
or
> make install
