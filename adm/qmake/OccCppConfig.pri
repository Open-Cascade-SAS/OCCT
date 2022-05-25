# This is project defines C++ compilation rules for building an OCCT Toolkit.

exists(custom.auto.pri) { include(custom.auto.pri) }
exists(custom.pri)      { include(custom.pri) }

# Disable some dummy Qt defaults
QT -= core gui
CONFIG -= qt app_bundle
CONFIG -= qml_debug
CONFIG -= debug_and_release

OccGitRoot = $$_PRO_FILE_PWD_/../../../..

# Define compilation flags
CONFIG += warn_on
QMAKE_CFLAGS_WARN_ON   = -Wall -Wextra
QMAKE_CXXFLAGS_WARN_ON = -Wall -Wextra
win32 {
  QMAKE_CFLAGS_WARN_ON   = -W4
  QMAKE_CXXFLAGS_WARN_ON = -W4
  QMAKE_CXXFLAGS_EXCEPTIONS_ON = /EHa
  QMAKE_CXXFLAGS_STL_ON = /EHa

  QMAKE_CXXFLAGS += -fp:precise
  #QMAKE_CXXFLAGS -= -Zc:throwingNew
  #QMAKE_CXXFLAGS -= -Zc:rvalueCast

  QMAKE_LFLAGS += -INCREMENTAL:NO

  CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -Od
    QMAKE_CXXFLAGS += -Ob1
  }

  DEFINES -= WIN32
  DEFINES -= WIN64
  DEFINES += _CRT_SECURE_NO_WARNINGS
  DEFINES += _CRT_NONSTDC_NO_DEPRECATE
  DEFINES += _SCL_SECURE_NO_WARNINGS
} else {
  CONFIG += c++11
  clang {
    QMAKE_CFLAGS_WARN_ON   += -Wshorten-64-to-32
    QMAKE_CXXFLAGS_WARN_ON += -Wshorten-64-to-32
  }
  QMAKE_CFLAGS   += -fexceptions
  QMAKE_CXXFLAGS += -fexceptions
  QMAKE_CXXFLAGS += -fvisibility=default
  DEFINES += OCC_CONVERT_SIGNALS
  mac {
    iphoneos {
      QMAKE_IOS_DEPLOYMENT_TARGET = 8.0
    } else {
      QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
    }
  } else:gcc {
    # ask linker to report missing library dependencies
    QMAKE_LFLAGS += -Wl,-z,defs
  }
}
!CONFIG(debug, debug|release) {
  # disable exceptions in Release builds
  DEFINES += No_Exception
  HAVE_RelWithDebInfo {
    win32 {
      CONFIG += force_debug_info
    }
  }
}

# Define output folder depending on compiler name
MY_BITNESS = 32

equals(QMAKE_TARGET.arch, x86_64) | equals(QMAKE_HOST.arch, x86_64) { MY_BITNESS = 64 }
equals(ANDROID_TARGET_ARCH, arm64-v8a) { MY_BITNESS = 64 }
has64Target = $$find(QMAKE_TARGET.arch, "x64")
count(has64Target, 1) { MY_BITNESS = 64 }

MY_PLATFORM = platform
CONFIG(iphonesimulator, iphoneos|iphonesimulator) { MY_PLATFORM = iphonesimulator
} else:CONFIG(iphoneos, iphoneos|iphonesimulator) { MY_PLATFORM = iphoneos
} else:android { MY_PLATFORM = android-$$ANDROID_TARGET_ARCH
} else:win32   { MY_PLATFORM = win$$MY_BITNESS
} else:mac     { MY_PLATFORM = mac
} else:linux   { MY_PLATFORM = lin
} else:unix    { MY_PLATFORM = unix
} else { warning (Unknown platform. "$$MY_PLATFORM" is used) }

MY_COMPILER = compiler
MY_VC_VER = 0
android-g++ {
  MY_COMPILER = gcc
} else:clang {
  MY_COMPILER = clang
} else:gcc {
  MY_COMPILER = gcc
} else:win32-msvc2010 {
  MY_COMPILER = vc10
  MY_VC_VER = 10
} else:win32-msvc2012 {
  MY_COMPILER = vc11
  MY_VC_VER = 11
} else:win32-msvc2013 {
  MY_COMPILER = vc12
  MY_VC_VER = 12
} else:win32-msvc2015 {
  MY_COMPILER = vc14
  MY_VC_VER = 14
} else:win32-msvc2017 {
  MY_COMPILER = vc14
  MY_VC_VER = 14
} else:win32-msvc {
  MY_COMPILER = vc14
  MY_VC_VER = 14
  aMsvcVer = $$(VisualStudioVersion)
  equals(aMsvcVer, 14.0){
    # VS2015, vc140
  } else:equals(aMsvcVer, 15.0){
    # VS2015, vc141
  } else:equals(aMsvcVer, 16.0){
    # VS2019, vc142
  } else:equals(aMsvcVer, 17.0){
    # VS2022, vc143
  } else {
    warning (Unknown msvc version. "$$MY_COMPILER" is used)
  }
} else {
  warning (Unknown compiler. "$$MY_COMPILER" is used)
}
MY_PLATFORM_AND_COMPILER = $$MY_PLATFORM/$$MY_COMPILER
#warning (The platform is "$$MY_PLATFORM"; bitness is "$$MY_BITNESS"; compiler is "$$MY_COMPILER")

CONFIG(debug, debug|release) { MY_BUILDTYPE = d }

DESTDIR = $$OccGitRoot/$${MY_PLATFORM_AND_COMPILER}/lib$${MY_BUILDTYPE}
win32 {
  DESTDIR  = $$OccGitRoot/win$${MY_BITNESS}/vc$${MY_VC_VER}/bin$${MY_BUILDTYPE}
  aLibDest = $$DESTDIR/../lib$${MY_BUILDTYPE}
  #DLLDESTDIR = $$DESTDIR/../bin$${MY_BUILDTYPE}

  # dummy target creating lib/libd folder
  occtkgen_libfolder.input  = $$_PRO_FILE_PWD_/../../OcctDummy.in
  occtkgen_libfolder.output = $$aLibDest/dummy.tmp
  occtkgen_libfolder.config = verbatim
  QMAKE_SUBSTITUTES += occtkgen_libfolder

  LIBS += -L$$aLibDest
  equals(TEMPLATE, lib) {
    QMAKE_CLEAN += $$DESTDIR/$${TARGET}.dll
    QMAKE_CLEAN += $$aLibDest/$${TARGET}.lib
    QMAKE_CLEAN += $$aLibDest/$${TARGET}.exp
  } else {
    QMAKE_CLEAN += $$DESTDIR/$${TARGET}.exe
  }
  QMAKE_CLEAN += $$DESTDIR/$${TARGET}.pdb
  QMAKE_LFLAGS += -PDB:"$$DESTDIR/$${TARGET}.pdb"
  QMAKE_LFLAGS += -IMPLIB:"$$aLibDest/$${TARGET}.lib"
} else {
  LIBS += -L$$DESTDIR
  equals(TEMPLATE, app) {
    DESTDIR = $$OccGitRoot/$${MY_PLATFORM_AND_COMPILER}/bin$${MY_BUILDTYPE}
  }
}

OBJECTS_DIR = $$DESTDIR/../obj$${MY_BUILDTYPE}/$${TARGET}
