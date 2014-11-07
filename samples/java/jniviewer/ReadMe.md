OCCT JniViewer sample for Android {#samples_java_android_occt}
================== 

This sample demonstrates simple way of using OCCT libraries in Android application written using Java.

The connection between Java and OCCT (C++) level is provided by proxy library, libTKJniSample.so, written in C++ with exported JNI methods of Java class OcctJniRenderer.
The proxy library contains single C++ class OcctJni_Viewer encapsulating OCCT viewer and providing functionality to manipulate this viewer
and to import OCCT shapes from several supported formats of CAD files (IGES, STEP, BREP).

This sample demonstrates indirect method of wrapping C++ to Java using manually created proxy library.
Alternative method is available, wrapping individual OCCT classes to Java equivalents so that their full API is available to Java user
and the code can be programmed on Java level similarly to C++ one.
See description of OCCT Java Wrapper in Advanced Samples and Tools on OCCT web site at 
http://www.opencascade.org/support/products/advsamples

Run Eclipse from ADT (Android Developer Tools) for building the sample. To import sample project perform
~~~~
  File -> Import... -> Android -> Existing Android code into Workspace
~~~~
and specify this directory. The project re-build will be started immediately right after importation if "Build automatically" option is turned on (default in Eclipse).  
Proxy library compilation and packaging is performed by NDK build script, called by "C++ Builder" configured within Eclipse project.
The path to "ndk-build" tool from Android NDK (Native Development Kit) should be specified in Eclipse project properties:
~~~~
  Project -> Properties -> Builders -> C++ Builder -> Edit -> Location
~~~~

Now paths to OCCT C++ libraries and additional components should be specified in "jni/Android.mk" file:
~~~~
OCCT_ROOT := $(LOCAL_PATH)/../../../..

FREETYPE_INC  := $(OCCT_ROOT)/../freetype/include/freetype2
FREETYPE_LIBS := $(OCCT_ROOT)/../freetype/libs

FREEIMAGE_INC  := $(OCCT_ROOT)/../FreeImage/include
FREEIMAGE_LIBS := $(OCCT_ROOT)/../FreeImage/libs

OCCT_INC  := $(OCCT_ROOT)/inc
OCCT_LIBS := $(OCCT_ROOT)/and/libs
~~~~
The list of extra components (Freetype, FreeImage) depends on OCCT configuration.
Variable $(TARGET_ARCH_ABI) is used within this script to refer to active architecture.
E.g. for 32-bit ARM build (see variable *APP_ABI* in "jni/Application.mk")
the folder *OCCT_LIBS* should contain sub-folder "armeabi-v7a" with OCCT libraries.

FreeImage is optional and does not required for this sample, however you should include all extra libraries used for OCCT building
and load the explicitly from Java code within OcctJniActivity::loadNatives() method, including toolkits from OCCT itself in proper order:
~~~~
    if (!loadLibVerbose ("TKernel", aLoaded, aFailed)
     || !loadLibVerbose ("TKMath",  aLoaded, aFailed)
     || !loadLibVerbose ("TKG2d",   aLoaded, aFailed)
~~~~
Note that C++ STL library is not part of Android system.
Thus application must package this library as well as extra component.
"gnustl_shared" STL implementation is expected within this sample.

After successful build, the application can be packaged to Android:
- Deploy and run application on connected device or emulator directly from Eclipse using adb interface by menu items "Run" and "Debug". This would sign package with debug certificate.
- Prepare signed end-user package using wizard File -> Export -> Android -> Export Android Application.
