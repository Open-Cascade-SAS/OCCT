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
https://www.opencascade.com/content/advanced-samples-and-tools

Install Android Studio 4.0+ and install building tools (check Tools -> SDK Manager):
- Android SDK (API level 21 or higher).
- Android SDK build tools.
- Android NDK r16 or higher (coming with CMake toolchain).
  Using NDK r18 or newer will require changing ANDROID_STL in project settings.
- CMake 3.10+.

Specify this folder location in Android Studio for opening project.
You might need re-entering Android SDK explicitly in File -> Project Structure -> SDK Location settings (SDK, NDK, JDK locations).

This sample expects OCCT to be already build - please refer to appropriate CMake building instructions in OCCT documentation.
The following variables should be added into file gradle.properties (see gradle.properties.template as template):
- `OCCT_ROOT` - path to OCCT installation folder.
- `FREETYPE_ROOT` - path to FreeType installation folder.

FreeImage is optional and does not required for this sample, however you should include all extra libraries used for OCCT building
and load the explicitly from Java code within OcctJniActivity::loadNatives() method, including toolkits from OCCT itself in proper order:
~~~~
    if (!loadLibVerbose ("TKernel", aLoaded, aFailed)
     || !loadLibVerbose ("TKMath",  aLoaded, aFailed)
     || !loadLibVerbose ("TKG2d",   aLoaded, aFailed)
~~~~
Note that C++ STL library is not part of Android system, and application must package this library as well as extra component ("gnustl_shared" by default - see also `ANDROID_STL`).

After successful build via Build -> Rebuild Project, the application can be packaged to Android:
- Deploy and run application on connected device or emulator directly from Android Studio using adb interface by menu items "Run" and "Debug". This would sign package with debug certificate.
- Prepare signed end-user package using wizard Build -> Generate signed APK.
