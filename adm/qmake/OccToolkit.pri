# This is a project template file defining an OCCT Toolkit.
# This project should be included with predefined OCC_TOOLKIT_NAME variable.

TEMPLATE = lib
!exists(../../src/$$OCC_TOOLKIT_NAME/PACKAGES) {
  TEMPLATE = app
  CONFIG += console
}
win32 {
  # do not append version to DLL name
  CONFIG += skip_target_version_ext
}

include(OccCppConfig.pri)

aSrcRoot = $$OccGitRoot/src
aHxxRoot = $$OccGitRoot/inc
INCLUDEPATH += $$aHxxRoot

# CSF variables
HAVE_FREETYPE  { CSF_FREETYPE = -lfreetype }
CSF_TclLibs   = -ltcl8.6
CSF_TclTkLibs = -ltk8.6
HAVE_FREEIMAGE { CSF_FreeImagePlus = -lfreeimage } else:win32 { CSF_FreeImagePlus = -lwindowscodecs -lole32 }
HAVE_FFMPEG    { CSF_FFmpeg = -lavcodec -lavformat -lswscale -lavutil }
HAVE_TBB       { CSF_TBB = -ltbb -ltbbmalloc }
HAVE_ZLIB      { CSF_ZLIB = -lzlib }
HAVE_LIBLZMA   { CSF_LIBLZMA = -lliblzma }
HAVE_DRACO     { CSF_Draco = -ldraco }
win32 {
  CSF_kernel32   = -lkernel32
  CSF_advapi32   = -ladvapi32
  CSF_gdi32      = -lgdi32
  CSF_user32     = -luser32 -lcomdlg32
  CSF_shell32    = -lShell32
  CSF_opengl32   = -lopengl32
  CSF_wsock32    = -lwsock32
  CSF_netapi32   = -lnetapi32
  CSF_OpenGlLibs = -lopengl32
  CSF_OpenGlesLibs = -llibEGL -llibGLESv2
  CSF_psapi      = -lPsapi
  CSF_winmm      = -lwinmm
  CSF_d3d9       = -ld3d9
  CSF_TclLibs    = -ltcl86
  CSF_TclTkLibs  = -ltk86
  CSF_TBB =
} else:mac {
  CSF_dl         = -ldl
  CSF_objc       = -lobjc
  CSF_OpenGlLibs = -framework OpenGL
  CSF_OpenGlesLibs = -framework OpenGLES
  iphoneos {
    CSF_Appkit     = -framework UIKit
  } else {
    CSF_Appkit     = -framework AppKit
  }
  CSF_IOKit      = -framework IOKit
  CSF_TclLibs    = -framework Tcl
  CSF_TclTkLibs  = -framework Tk
} else {
  CSF_dl = -ldl
  CSF_ThreadLibs = -lpthread -lrt
  CSF_OpenGlesLibs = -lEGL -lGLESv2
  CSF_TclTkLibs  = -ltk8.6
  HAVE_XLIB {
    CSF_OpenGlLibs = -lGL
    CSF_XwLibs     = -lX11
  } else {
    CSF_OpenGlLibs = -lGL -lEGL
  }
  HAVE_FREETYPE  { CSF_fontconfig = -lfontconfig }
}

for (aCfgIter, CONFIG) {
  aRes = $$find(aCfgIter, "^HAVE_")
  !equals(aCfgIter, "HAVE_GLES2") {
    count(aRes, 1) {
      DEFINES += $$aCfgIter
    }
  }
}

# Define the list of standard OCCT file extensions
aHxxRegex = ^.*\.(hxx|h|lxx|gxx)$
aPxxRegex = ^.*\.(pxx)$
aCxxRegex = ^.*\.(cxx|cpp|c)$
mac { aCxxRegex = ^.*\.(cxx|cpp|c|m|mm)$ }

# Auxiliary function for probing file extension
defineTest (occCheckExtension) {
  aProbe = $$find(1, "$$2")
  count(aProbe, 1) { return(true) } else { return(false) }
}

# Auxiliary function for probing compilable files
defineTest (occIsCxxFile) { occCheckExtension ($$1, $$aCxxRegex) { return(true) } else { return(false) } }

# Auxiliary function for probing header files
defineTest (occIsHxxFile) { occCheckExtension ($$1, $$aHxxRegex) { return(true) } else { return(false) } }

aTkFiles     = $$cat($$aSrcRoot/$$OCC_TOOLKIT_NAME/FILES,     lines)
aTkFiles += CMakeLists.txt
aPackages    = $$cat($$aSrcRoot/$$OCC_TOOLKIT_NAME/PACKAGES,  lines)
anExternLibs = $$cat($$aSrcRoot/$$OCC_TOOLKIT_NAME/EXTERNLIB, lines)

for (aTkFileIter, aTkFiles) { OTHER_FILES += $$aSrcRoot/$$OCC_TOOLKIT_NAME/$$aTkFileIter }
for (anExternLib, anExternLibs) {
  hasCsf = $$find(anExternLib, CSF_)
  count(hasCsf, 1) {
    aList = $$split($$anExternLib, "\n")
    LIBS += $$aList
    equals(anExternLib, "CSF_OpenGlLibs") {
      DEFINES += "HAVE_OPENGL"
    }
    equals(anExternLib, "CSF_OpenGlesLibs") {
      DEFINES += "HAVE_GLES2"
    }
  } else {
    LIBS += -l$$anExternLib
  }
}

# Iterate over Packages and add compilable files into this project
isEmpty (aPackages) { aPackages = $$OCC_TOOLKIT_NAME }
for (aPackage, aPackages) {
  aPackageFolder = $$aSrcRoot/$$OCC_TOOLKIT_NAME/$$aPackage
  aPackageFiles = $$cat($$aSrcRoot/$$aPackage/FILES, lines)
  for (aFileIter, aPackageFiles) {
    occIsCxxFile($$aFileIter) {
      SOURCES += $$aSrcRoot/$$aPackage/$$aFileIter
    }
  }
}

!win32 {
  aVerList = $$split(VERSION, ".")
  aVerMaj = $$member(aVerList, 0)
  aVerMin = $$member(aVerList, 1)
  aVerMic = $$member(aVerList, 2)

  equals(TEMPLATE, app) {
    QMAKE_CLEAN += $$DESTDIR/$${TARGET}
  } else {
    mac {
      # override qmake soname versionong logic
      QMAKE_LFLAGS_SONAME =
      QMAKE_LFLAGS += -Wl,-soname=lib$${TARGET}.dylib.$${aVerMaj}.$${aVerMin}

      # extend clean with versioned .dylib files
      QMAKE_CLEAN += $$DESTDIR/lib$${TARGET}.dylib
      QMAKE_CLEAN += $$DESTDIR/lib$${TARGET}.$${aVerMaj}.dylib
      QMAKE_CLEAN += $$DESTDIR/lib$${TARGET}.$${aVerMaj}.$${aVerMin}.dylib
      QMAKE_CLEAN += $$DESTDIR/lib$${TARGET}.$${aVerMaj}.$${aVerMin}.$${aVerMic}.dylib
    } else {
      # override qmake soname versionong logic
      QMAKE_LFLAGS_SONAME =
      QMAKE_LFLAGS += -Wl,-soname=lib$${TARGET}.so.$${aVerMaj}.$${aVerMin}

      # extend clean with versioned .so files
      QMAKE_CLEAN += $$DESTDIR/lib$${TARGET}.so
      QMAKE_CLEAN += $$DESTDIR/lib$${TARGET}.so.$${aVerMaj}
      QMAKE_CLEAN += $$DESTDIR/lib$${TARGET}.so.$${aVerMaj}.$${aVerMin}
      QMAKE_CLEAN += $$DESTDIR/lib$${TARGET}.so.$${aVerMaj}.$${aVerMin}.$${aVerMic}
    }
  }
}
