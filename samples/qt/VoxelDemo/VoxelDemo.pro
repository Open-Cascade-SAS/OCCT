TEMPLATE = app
CONFIG += debug_and_release qt opengl

TARGET = VoxelDemo

SAMPLESROOT = $$(CASROOT)/samples/qt

HEADERS   = $${SAMPLESROOT}/voxeldemo/inc/*.h

SOURCES   = $${SAMPLESROOT}/voxeldemo/src/*.cxx \
            $${SAMPLESROOT}/voxeldemo/src/*.cpp


INCLUDEPATH += $$quote($${SAMPLESROOT}/voxeldemo/inc)

DEFINES = 

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
    DEFINES += OCC_CONVERT_SIGNALS QT_NO_STL
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
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1800) {
		DESTDIR = ./win32/vc12/bind
		OBJECTS_DIR = ./win32/vc12/objd
		MOC_DIR = ./win32/vc12/srcd
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
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1800) {
		DESTDIR = ./win64/vc12/bind
		OBJECTS_DIR = ./win64/vc12/objd
		MOC_DIR = ./win64/vc12/srcd
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
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1800) {
		DESTDIR = ./win32/vc12/bin
		OBJECTS_DIR = ./win32/vc12/obj
		MOC_DIR = ./win32/vc12/src
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
	    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1800) {
		DESTDIR = ./win64/vc12/bin
		OBJECTS_DIR = ./win64/vc12/obj
		MOC_DIR = ./win64/vc12/src
	    }
	}
    }
    DEFINES +=WNT WIN32 NO_COMMONSAMPLE_EXPORTS NO_IESAMPLE_EXPORTS
}

LIBS += -lTKernel -lTKMath -lTKService -lTKV3d \
        -lTKBRep -lTKGeomBase -lTKTopAlgo -lTKPrim \
        -lTKBool -lTKVoxel -lTKOpenGl \

QMAKE_CXXFLAGS            = -Zc:wchar_t 
