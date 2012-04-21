TEMPLATE = app
CONFIG += debug_and_release qt

TARGET = IESample

HEADERS = src/*.h
SOURCES = src/*.cxx

INCLUDES = $$(CSF_OPT_INC)
PATHS = $$split(INCLUDES,";")
for(path, PATHS):INCLUDEPATH += $${path}

TS_FILES = ./src/Common-icon.ts \
           ./src/Common-string.ts \
           ./src/Interface-string.ts

DEFINES = CSFDB

unix {
    CONFIG(debug, debug|release) {
	DESTDIR = ./Linux/bind
	OBJECTS_DIR = ./Linux/objd
	MOC_DIR = ./Linux/srcd
    } else {
	DESTDIR = ./Linux/bin
	OBJECTS_DIR = ./Linux/obj
	MOC_DIR = ./Linux/src
    }
    INCLUDEPATH += $$QMAKE_INCDIR_X11 $$QMAKE_INCDIR_OPENGL $$QMAKE_INCDIR_THREAD
    DEFINES += LIN LININTEL OCC_CONVERT_SIGNALS HAVE_CONFIG_H HAVE_WOK_CONFIG_H QT_NO_STL
    LIBS = -L$(CASROOT)/Linux/lib -L$$QMAKE_LIBDIR_X11 $$QMAKE_LIBS_X11 -L$$QMAKE_LIBDIR_OPENGL $$QMAKE_LIBS_OPENGL $$QMAKE_LIBS_THREAD
    FREEIMAGE_DIR = $$(FREEIMAGEDIR)
    exists($$FREEIMAGE_DIR) {
	LIBS += -L$(FREEIMAGEDIR)/lib -lfreeimageplus
    }
    TBB_LIB = $$(TBBLIB)
    exists($$TBB_LIB) {
	LIBS += -L$(TBBLIB) -ltbb -ltbbmalloc
    }
}

win32 {
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
	}
    }
    DEFINES +=WNT WIN32 NO_COMMONSAMPLE_EXPORTS NO_IESAMPLE_EXPORTS
}

LIBS += TKernel.lib PTKernel.lib TKMath.lib TKService.lib TKV3d.lib TKV2d.lib \
        TKBRep.lib TKIGES.lib TKSTL.lib TKVRML.lib TKSTEP.lib TKSTEPAttr.lib TKSTEP209.lib \
        TKSTEPBase.lib TKShapeSchema.lib TKGeomBase.lib TKGeomAlgo.lib TKG3d.lib TKG2d.lib \
        TKXSBase.lib TKPShape.lib TKShHealing.lib TKHLR.lib TKTopAlgo.lib TKMesh.lib TKPrim.lib \
        TKCDF.lib TKBool.lib TKBO.lib TKFillet.lib TKOffset.lib \

lrelease.name = LRELASE ${QMAKE_FILE_IN}
lrelease.commands = lrelease ${QMAKE_FILE_IN} -qm ./res/${QMAKE_FILE_BASE}.qm
lrelease.output = ./res/${QMAKE_FILE_BASE}.qm
lrelease.input = TS_FILES
lrelease.clean = ./res/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease
