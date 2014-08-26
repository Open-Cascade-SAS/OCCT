#
INCLUDEPATH += $$_PRO_FILE_PWD_/occt/inc $$_PRO_FILE_PWD_/3rdparty/include
DEPENDPATH  += $$_PRO_FILE_PWD_/occt/inc $$_PRO_FILE_PWD_/3rdparty/include

DEFINES += OCC_CONVERT_SIGNALS

QMAKE_CFLAGS   += -fexceptions -Wno-ignored-qualifiers
QMAKE_CXXFLAGS += -fexceptions -Wno-ignored-qualifiers

CONFIG(debug,debug|release) {
  DEFINES += DEB
}

occt_lib_subpath = libs/armeabi-v7a

occt_lib_path      = $$_PRO_FILE_PWD_/occt/$$occt_lib_subpath
3rdparty_lib_path  = $$_PRO_FILE_PWD_/3rdparty/$$occt_lib_subpath


LIBS += -L$$occt_lib_path \
        -lTKernel \
        -lTKMath \
        -lTKG2d \
        -lTKG3d \
        -lTKGeomBase \
        -lTKBRep \
        -lTKGeomAlgo \
        -lTKTopAlgo \
        -lTKShHealing \
        -lTKService \
        -lTKMesh \
        -lTKHLR \
        -lTKV3d \
        -lTKOpenGl \
        -lEGL

# IMPORTANT. load libraries in a proper order
ANDROID_EXTRA_LIBS =  $$3rdparty_lib_path/libfreeimage.so \
                      $$3rdparty_lib_path/libfreetype.so \
                      $$occt_lib_path/libTKernel.so \
                      $$occt_lib_path/libTKMath.so \
                      $$occt_lib_path/libTKG2d.so \
                      $$occt_lib_path/libTKG3d.so \
                      $$occt_lib_path/libTKGeomBase.so \
                      $$occt_lib_path/libTKBRep.so \
                      $$occt_lib_path/libTKGeomAlgo.so \
                      $$occt_lib_path/libTKTopAlgo.so \
                      $$occt_lib_path/libTKShHealing.so \
                      $$occt_lib_path/libTKService.so \
                      $$occt_lib_path/libTKMesh.so \
                      $$occt_lib_path/libTKHLR.so \
                      $$occt_lib_path/libTKV3d.so \
                      $$occt_lib_path/libTKOpenGl.so
