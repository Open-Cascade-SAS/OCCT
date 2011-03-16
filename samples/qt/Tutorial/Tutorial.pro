TEMPLATE = app
CONFIG += debug_and_release qt

TARGET = Tutorial

HEADERS = src/*.h
SOURCES = src/*.cxx

INCLUDEPATH = $(CASROOT) $(CASROOT)/inc $(CASROOT)/src/WOKTclLib $(QTDIR)/include/QtCore \
              $(QTDIR)/include/QtGui $(QTDIR)/include

TS_FILES += ./src/Common-icon.ts \
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
}

win32 {
    CONFIG(debug, debug|release) {
	DEFINES += _DEBUG
	DESTDIR = ./win32/bind
	OBJECTS_DIR = ./win32/objd
	MOC_DIR = ./win32/srcd
	LIBS = -L$(CASROOT)/win32/libd
    } else {
	DEFINES += NDEBUG
	DESTDIR = ./win32/bin
	OBJECTS_DIR = ./win32/obj
	MOC_DIR = ./win32/src
	LIBS = -L$(CASROOT)/win32/lib
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
lrelease.input = TS_FILES
lrelease.clean = ./res/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease
