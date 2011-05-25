TEMPLATE = app
CONFIG += debug_and_release qt

TARGET = Tutorial

HEADERS = src/*.h
SOURCES = src/*.cxx

INCLUDEPATH = $(CASROOT) $(CASROOT)/inc $(CASROOT)/src/WOKTclLib $(QTDIR)/include/QtCore \
              $(QTDIR)/include/QtGui $(QTDIR)/include

TRANSLATIONS += ./src/Common-icon.ts \
                ./src/Common-string.ts \
                ./src/Tutorial-icon.ts \
		./src/Tutorial-string.ts

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
    DEFINES += LIN LININTEL OCC_CONVERT_SIGNALS HAVE_CONFIG_H HAVE_WOK_CONFIG_H
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
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1310) {
		DESTDIR = ./win32/vc7/bind
		OBJECTS_DIR = ./win32/vc7/objd
		MOC_DIR = ./win32/vc7/srcd
		LIBS = -L$(CASROOT)/win32/vc7/libd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1400) {
		DESTDIR = ./win32/vc8/bind
		OBJECTS_DIR = ./win32/vc8/objd
		MOC_DIR = ./win32/vc8/srcd
		LIBS = -L$(CASROOT)/win32/vc8/libd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1500) {
		DESTDIR = ./win32/vc9/bind
		OBJECTS_DIR = ./win32/vc9/objd
		MOC_DIR = ./win32/vc9/srcd
		LIBS = -L$(CASROOT)/win32/vc9/libd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
		DESTDIR = ./win32/vc10/bind
		OBJECTS_DIR = ./win32/vc10/objd
		MOC_DIR = ./win32/vc10/srcd
		LIBS = -L$(CASROOT)/win32/vc10/libd
	    }
	} else {
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1400) {
		DESTDIR = ./win64/vc8/bind
		OBJECTS_DIR = ./win64/vc8/objd
		MOC_DIR = ./win64/vc8/srcd
		LIBS = -L$(CASROOT)/win64/vc8/libd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1500) {
		DESTDIR = ./win64/vc9/bind
		OBJECTS_DIR = ./win64/vc9/objd
		MOC_DIR = ./win64/vc9/srcd
		LIBS = -L$(CASROOT)/win64/vc9/libd
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
		DESTDIR = ./win64/vc10/bind
		OBJECTS_DIR = ./win64/vc10/objd
		MOC_DIR = ./win64/vc10/srcd
		LIBS = -L$(CASROOT)/win64/vc10/libd
	    }
	}
    } else {
	DEFINES += NDEBUG
	!contains(QMAKE_HOST.arch, x86_64) {
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1310) {
		DESTDIR = ./win32/vc7/bin
		OBJECTS_DIR = ./win32/vc7/obj
		MOC_DIR = ./win32/vc7/src
		LIBS = -L$(CASROOT)/win32/vc7/lib
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1400) {
		DESTDIR = ./win32/vc8/bin
		OBJECTS_DIR = ./win32/vc8/obj
		MOC_DIR = ./win32/vc8/src
		LIBS = -L$(CASROOT)/win32/vc8/lib
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1500) {
		DESTDIR = ./win32/vc9/bin
		OBJECTS_DIR = ./win32/vc9/obj
		MOC_DIR = ./win32/vc9/src
		LIBS = -L$(CASROOT)/win32/vc9/lib
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
		DESTDIR = ./win32/vc10/bin
		OBJECTS_DIR = ./win32/vc10/obj
		MOC_DIR = ./win32/vc10/src
		LIBS = -L$(CASROOT)/win32/vc10/lib
	    }
	} else {
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1400) {
		DESTDIR = ./win64/vc8/bin
		OBJECTS_DIR = ./win64/vc8/obj
		MOC_DIR = ./win64/vc8/src
		LIBS = -L$(CASROOT)/win64/vc8/lib
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1500) {
		DESTDIR = ./win64/vc9/bin
		OBJECTS_DIR = ./win64/vc9/obj
		MOC_DIR = ./win64/vc9/src
		LIBS = -L$(CASROOT)/win64/vc9/lib
	    }
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
		DESTDIR = ./win64/vc10/bin
		OBJECTS_DIR = ./win64/vc10/obj
		MOC_DIR = ./win64/vc10/src
		LIBS = -L$(CASROOT)/win64/vc10/lib
	    }
	}
    }
    DEFINES +=WNT WIN32 NO_COMMONSAMPLE_EXPORTS NO_IESAMPLE_EXPORTS
}

LIBS += -lTKernel -lPTKernel -lTKMath -lTKService -lTKV3d -lTKV2d \
        -lTKBRep -lTKIGES -lTKSTL -lTKVRML -lTKSTEP -lTKSTEPAttr -lTKSTEP209 \
        -lTKSTEPBase -lTKShapeSchema -lTKGeomBase -lTKGeomAlgo -lTKG3d -lTKG2d \
        -lTKXSBase -lTKPShape -lTKShHealing -lTKHLR -lTKTopAlgo -lTKMesh -lTKPrim \
        -lTKCDF -lTKBool -lTKBO -lTKFillet -lTKOffset \
	-L$(QTDIR)/lib -lQtCore -lQtGui

lrelease.name = LRELASE ${QMAKE_FILE_IN}
lrelease.commands = $(QTDIR)/bin/lrelease ${QMAKE_FILE_IN} -qm ./res/${QMAKE_FILE_BASE}.qm
lrelease.output = ./res/${QMAKE_FILE_BASE}.qm
lrelease.input = TRANSLATIONS
lrelease.clean = ./res/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease
