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
 
Configure project for building sample:

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
* In Ant executable specify path to ant.bat file located in Apache Ant bin directory

Make sure that "Android for armeabi-v7a" kit has been detected
~~~~
Tools->Options->Build & Run
~~~~ 

The paths to OCCT and 3rdparty libraries are specified in "OCCT.pri" file:

the paths to the headers:
~~~~
INCLUDEPATH += /occt/inc /3rdparty/include
DEPENDPATH  += /occt/inc /3rdparty/include
~~~~

the libraries location:
~~~~
LIBS += -L/occt/libs/armeabi-v7a
~~~~

OCCT resources (Shaders, SHMessage, StdResource, TObj, UnitsAPI and XSMessage folder) should be copied to androidqt_dir/android/assets/opencascade/shared/ directory. Current sample requires at least Shaders folder.

Select build configuration: Debug or Release and click Build->Build Project "AndroidQt" or (Ctrl + B).
After successful build the application can be deployed to device or emulator.
