OCCT AndroidQt sample for Android {#samples_qml_android_occt}
================== 

This sample demonstrates a simple way of using OCCT libraries in Android application written using Qt/Qml.

The connection between Qt/Qml and OCCT (C++) level is provided by proxy library, libAndroidQt.so, written in C++.
The proxy library contains single C++ class AndroidQt encapsulating OCCT viewer and providing functionality to manipulate this viewer
and to import OCCT shapes from supported format of CAD file (BREP).

Requirements for building sample:
* Java Development Kit 1.7 or higher
* Qt 5.3 or higher
* Android SDK  from 2014.07.02 or newer
* Android NDK r9d or newer
* Apache Ant 1.9.4 or higher
* OCCT compiled under Android platform and placed in directories:
  * occt/libs/armeabi-v7a/\*.so and occt/inc/\*.hxx (libraries and include files of OCCT install)
  * android/assets/opencascade/shared/Shaders/\* (Shaders folder of OCCT install: /share/opencascade/resources/Shaders)
  * 3rdparty/include/freetype2/\*, 3rdparty/include/FreeImage.h and 3rdparty/libs/armeabi-v7a/libFreeImage.so and 3rdparty/libs/armeabi-v7a/libfreetype.so

It is also possible to to correct OCCT.pri file an get resources from another tree of directories.

When AndroidQt will be started, it may be helpful to have some default data files(BRep) on Device for opening in AndroidQt.
Copy these files into "android\assets\opencascade\shared" and it will be installed to device during compilation procedure.

Having prepared all these products, configure AndroidQt project for building sample:

In QtCreator, open AndroidQt.pro project-file:
~~~~
  File -> Open file or Project... 
~~~~

Specify Android configurations:
~~~~
Tools->Options->Android
~~~~ 
* In JDK location specify path to Java Development Kit
* In Android SDK location specify path to Android SDK
* In Android NDK location specify path to Android NDK
(During this location definition, warning is possible and OK:
 "Qt version for architecture mips is missing. To add the Qt version, select Options > Build & Run > Qt Versins.")
* In Ant executable specify path to ant.bat file located in Apache Ant bin directory

Make sure that "Android for armeabi-v7a" kit has been detected (present in the list).
~~~~
Tools->Options->Build & Run
~~~~ 

also or it can be checked or corrected in:
~~~~
Projects->Android for armeabi-v7a option should be checked
~~~~ 

Switch On device, connect it to PC and define it in Qt Creator:
~~~~
Projects->Manage Kits...->Devices->Device: Run on Android
~~~~                                                     
Check that "Current state" is "Ready to use" on this page.

~~~~
Projects->Build Settings->General: Shadow build is switched OFF
~~~~

Start configuration:

~~~~
Call Build -> Run qmake
~~~~
Check content of "Compile Output" view.

In order to perform qmake correctly, for example if you have the following error:
~~~~
Running steps for project AndroidQt...
Could not start process "<qt_dir>\android_armv7\bin\qmake.exe" <occt_dir>\samples\qt\AndroidQt\AndroidQt.pro -r -spec android-g++ "CONFIG+=debug" "CONFIG+=declarative_debug" "CONFIG+=qml_debug"
Error while building/deploying project AndroidQt (kit: Android for armeabi-v7a (GCC 4.9, Qt 5.3.2))
When executing step "qmake"
~~~~

~~~~
Projects->Build Settings->General: switch OFF Shadow build
Build->Build Project "Android Qt"
Build->Run
~~~~

Dialog to "Select Android Device" is shown. Select Compatible Device, Ok.
In case of any error, see log in "Application Output" view.

After successful build the application can be deployed to device or emulator.
