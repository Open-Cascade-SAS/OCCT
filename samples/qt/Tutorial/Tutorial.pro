TEMPLATE = app
CONFIG += debug_and_release qt
QT += qt3support

TARGET = Tutorial

SAMPLESROOT = $$(CASROOT)/samples/qt

HEADERS   = src/*.h \
            $${SAMPLESROOT}/Common/src/*.h \
            $${SAMPLESROOT}/Interface/src/*.h

SOURCES   = src/*.cxx \
            $${SAMPLESROOT}/Common/src/*.cxx \
            $${SAMPLESROOT}/Interface/src/*.cxx

TS_FILES  = $${SAMPLESROOT}/Common/src/Common-icon.ts \
            $${SAMPLESROOT}/Common/src/Common-string.ts \
            ./src/Tutorial-icon.ts \
            ./src/Tutorial-string.ts

RES_FILES = $${SAMPLESROOT}/Common/res/* \
            ./res/*

RES_DIR   = $$quote($$(RES_DIR))

INCLUDEPATH += $$quote($${SAMPLESROOT}/Common/src)
INCLUDEPATH += $$quote($${SAMPLESROOT}/Interface/src)

DEFINES = CSFDB

unix {
    UNAME = $$system(uname -s)
    INCLUDES = $$(CSF_OPT_INC)
    PATHS = $$split(INCLUDES,":")
    for(path, PATHS):INCLUDEPATH += $${path}
    LIBLIST = $$(LD_LIBRARY_PATH)
    LIBPATHS = $$split(LIBLIST,":")
    for(lib, LIBPATHS):LIBS += -L$${lib}

    CONFIG(debug, debug|release) {
	DESTDIR = ./$$UNAME/bind
	OBJECTS_DIR = ./$$UNAME/objd
	MOC_DIR = ./$$UNAME/srcd
    } else {
	DESTDIR = ./$$UNAME/bin
	OBJECTS_DIR = ./$$UNAME/obj
	MOC_DIR = ./$$UNAME/src
    }

    MACOSX_USE_GLX = $$(MACOSX_USE_GLX)

    !macx | equals(MACOSX_USE_GLX, true): INCLUDEPATH += $$QMAKE_INCDIR_X11 $$QMAKE_INCDIR_OPENGL $$QMAKE_INCDIR_THREAD
    !macx | equals(MACOSX_USE_GLX, true): DEFINES += LIN LININTEL
    equals(MACOSX_USE_GLX, true): DEFINES += MACOSX_USE_GLX
    DEFINES += OCC_CONVERT_SIGNALS HAVE_CONFIG_H HAVE_WOK_CONFIG_H QT_NO_STL
    !macx | equals(MACOSX_USE_GLX, true): LIBS += -L$$QMAKE_LIBDIR_X11 $$QMAKE_LIBS_X11 -L$$QMAKE_LIBDIR_OPENGL $$QMAKE_LIBS_OPENGL $$QMAKE_LIBS_THREAD
    LIBS += -lfreeimageplus
    LIBS += -ltbb -ltbbmalloc
}

win32 {
    INCLUDES = $$(CSF_OPT_INC)
    PATHS = $$split(INCLUDES,";")
    for(path, PATHS):INCLUDEPATH += $${path}

    CONFIG(debug, debug|release) {
	DEFINES += _DEBUG
	!contains(QMAKE_HOST.arch, x86_64) {
		LIBS = -L$(CSF_OPT_LIB32D)
		contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1310) {
		DESTDIR = ./win32/vc7/bind
		OBJECTS_DIR = ./win32/vc7/objd
		MOC_DIR = ./win32/vc7/srcd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1400) {
		DESTDIR = ./win32/vc8/bind
		OBJECTS_DIR = ./win32/vc8/objd
		MOC_DIR = ./win32/vc8/srcd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1500) {
		DESTDIR = ./win32/vc9/bind
		OBJECTS_DIR = ./win32/vc9/objd
		MOC_DIR = ./win32/vc9/srcd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
		DESTDIR = ./win32/vc10/bind
		OBJECTS_DIR = ./win32/vc10/objd
		MOC_DIR = ./win32/vc10/srcd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1700) {
		DESTDIR = ./win32/vc11/bind
		OBJECTS_DIR = ./win32/vc11/objd
		MOC_DIR = ./win32/vc11/srcd
	    }
	} else {
		LIBS = -L$(CSF_OPT_LIB64D)
		contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1400) {
		DESTDIR = ./win64/vc8/bind
		OBJECTS_DIR = ./win64/vc8/objd
		MOC_DIR = ./win64/vc8/srcd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1500) {
		DESTDIR = ./win64/vc9/bind
		OBJECTS_DIR = ./win64/vc9/objd
		MOC_DIR = ./win64/vc9/srcd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
		DESTDIR = ./win64/vc10/bind
		OBJECTS_DIR = ./win64/vc10/objd
		MOC_DIR = ./win64/vc10/srcd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1700) {
		DESTDIR = ./win64/vc11/bind
		OBJECTS_DIR = ./win64/vc11/objd
		MOC_DIR = ./win64/vc11/srcd
	    }
	}
    } else {
	DEFINES += NDEBUG
	!contains(QMAKE_HOST.arch, x86_64) {
		LIBS = -L$(CSF_OPT_LIB32)
		contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1310) {
		DESTDIR = ./win32/vc7/bin
		OBJECTS_DIR = ./win32/vc7/obj
		MOC_DIR = ./win32/vc7/src
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1400) {
		DESTDIR = ./win32/vc8/bin
		OBJECTS_DIR = ./win32/vc8/obj
		MOC_DIR = ./win32/vc8/src
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1500) {
		DESTDIR = ./win32/vc9/bin
		OBJECTS_DIR = ./win32/vc9/obj
		MOC_DIR = ./win32/vc9/src
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
		DESTDIR = ./win32/vc10/bin
		OBJECTS_DIR = ./win32/vc10/obj
		MOC_DIR = ./win32/vc10/src
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1700) {
		DESTDIR = ./win32/vc11/bin
		OBJECTS_DIR = ./win32/vc11/obj
		MOC_DIR = ./win32/vc11/src
	    }
	} else {
		LIBS = -L$(CSF_OPT_LIB64)
		contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1400) {
		DESTDIR = ./win64/vc8/bin
		OBJECTS_DIR = ./win64/vc8/obj
		MOC_DIR = ./win64/vc8/src
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1500) {
		DESTDIR = ./win64/vc9/bin
		OBJECTS_DIR = ./win64/vc9/obj
		MOC_DIR = ./win64/vc9/src
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
		DESTDIR = ./win64/vc10/bin
		OBJECTS_DIR = ./win64/vc10/obj
		MOC_DIR = ./win64/vc10/src
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1700) {
		DESTDIR = ./win64/vc11/bin
		OBJECTS_DIR = ./win64/vc11/obj
		MOC_DIR = ./win64/vc11/src
	    }
	}
    }
    DEFINES +=WNT WIN32 NO_COMMONSAMPLE_EXPORTS NO_IESAMPLE_EXPORTS
}

LIBS += -lTKernel -lPTKernel -lTKMath -lTKService -lTKV3d \
        -lTKBRep -lTKIGES -lTKSTL -lTKVRML -lTKSTEP -lTKSTEPAttr -lTKSTEP209 \
        -lTKSTEPBase -lTKShapeSchema -lTKGeomBase -lTKGeomAlgo -lTKG3d -lTKG2d \
        -lTKXSBase -lTKPShape -lTKShHealing -lTKHLR -lTKTopAlgo -lTKMesh -lTKPrim \
        -lTKCDF -lTKBool -lTKBO -lTKFillet -lTKOffset \

!exists($${RES_DIR}) {
    win32 {
        system(mkdir $${RES_DIR})
    } else {
        system(mkdir -p $${RES_DIR})
    }
}

lrelease.name = LRELEASE ${QMAKE_FILE_IN}
lrelease.commands = lrelease ${QMAKE_FILE_IN} -qm $${RES_DIR}/${QMAKE_FILE_BASE}.qm
lrelease.output = ${QMAKE_FILE_BASE}.qm
lrelease.input = TS_FILES
lrelease.clean = $${RES_DIR}/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

copy_res.name = Copy resource ${QMAKE_FILE_IN}
copy_res.output = ${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_res.clean = $${RES_DIR}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_res.input = RES_FILES
copy_res.CONFIG += no_link target_predeps
win32: copy_res.commands = type ${QMAKE_FILE_IN} > $${RES_DIR}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
unix:  copy_res.commands = cp -f ${QMAKE_FILE_IN} $${RES_DIR}
QMAKE_EXTRA_COMPILERS += copy_res

