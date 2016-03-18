TEMPLATE = app
CONFIG += debug_and_release qt

TARGET = Tutorial

SAMPLESROOT = $$quote($$(CSF_OCCTSamplesPath)/qt)

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
INCLUDEPATH += $$quote($$(CSF_OCCTIncludePath))

OCCT_DEFINES = $$(CSF_DEFINES)

DEFINES = $$split(OCCT_DEFINES, ;)

unix {
    UNAME = $$system(uname -s)
    LIBLIST = $$(LD_LIBRARY_PATH)
    LIBPATHS = $$split(LIBLIST,":")
    for(lib, LIBPATHS):LIBS += -L$${lib}

    CONFIG(debug, debug|release) {
        DESTDIR = ./$$UNAME/bind
        OBJECTS_DIR = ./$$UNAME/objd
        MOC_DIR = ./$$UNAME/mocd
    } else {
        DESTDIR = ./$$UNAME/bin
        OBJECTS_DIR = ./$$UNAME/obj
        MOC_DIR = ./$$UNAME/moc
    }

    MACOSX_USE_GLX = $$(MACOSX_USE_GLX)

    !macx | equals(MACOSX_USE_GLX, true): INCLUDEPATH += $$QMAKE_INCDIR_X11 $$QMAKE_INCDIR_OPENGL $$QMAKE_INCDIR_THREAD
    equals(MACOSX_USE_GLX, true): DEFINES += MACOSX_USE_GLX
    DEFINES += OCC_CONVERT_SIGNALS QT_NO_STL
    !macx | equals(MACOSX_USE_GLX, true): LIBS += -L$$QMAKE_LIBDIR_X11 $$QMAKE_LIBS_X11 -L$$QMAKE_LIBDIR_OPENGL $$QMAKE_LIBS_OPENGL $$QMAKE_LIBS_THREAD
    LIBS += -lfreeimageplus
    LIBS += -ltbb -ltbbmalloc
    QMAKE_CXXFLAGS += -std=gnu++11
}

win32 {
    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG
        DESTDIR = ./win$(ARCH)/$(VCVER)/bind
        OBJECTS_DIR = ./win$(ARCH)/$(VCVER)/objd
        MOC_DIR = ./win$(ARCH)/$(VCVER)/mocd
    } else {
        DEFINES += NDEBUG
        DESTDIR = ./win$(ARCH)/$(VCVER)/bin
        OBJECTS_DIR = ./win$(ARCH)/$(VCVER)/obj
        MOC_DIR = ./win$(ARCH)/$(VCVER)/moc
    }
    LIBS = -L$$(QTDIR)/lib;$$(CSF_OCCTLibPath)
    DEFINES += NO_COMMONSAMPLE_EXPORTS NO_IESAMPLE_EXPORTS
}

LIBS += -lTKernel -lTKMath -lTKService -lTKV3d -lTKOpenGl \
        -lTKBRep -lTKIGES -lTKSTL -lTKVRML -lTKSTEP -lTKSTEPAttr -lTKSTEP209 \
        -lTKSTEPBase -lTKGeomBase -lTKGeomAlgo -lTKG3d -lTKG2d \
        -lTKXSBase -lTKShHealing -lTKHLR -lTKTopAlgo -lTKMesh -lTKPrim \
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
#QMAKE_CXXFLAGS += /wd4996

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
} 