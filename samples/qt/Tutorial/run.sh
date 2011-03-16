#!/bin/sh

host=`uname -s`

PROG_DIR=`dirname $0`
ROOT_DIR=`(cd $PROG_DIR; pwd)`

if test ! -d "$host/obj"; then
    QTDIR="$CASROOT/../3rdparty/$host/qt"
    QT_LD_PATH="$CASROOT/../3rdparty/$host/qt"
    CASCADE_LD_PATH="$CASROOT/$host/lib"
    LD_LIBRARY_PATH="${CASCADE_LD_PATH}:${QT_LD_PATH}:${LD_LIBRARY_PATH}"
fi
STATION=$host

if test "${CASROOT+set}" != "set"; then
   echo "Environment variable \"CASROOT\" not defined."
   CASROOT="$CASCADE_ROOT"
   if test -n "$CASROOT"; then
      echo "Try to use CASROOT=$CASROOT"
   else
      exit 1
   fi
fi

if test ! -d "$CASROOT"; then
   echo "CasCade root directory \"$CASROOT\" not found."
   exit 1
fi

if test "${QTDIR+set}" != "set"; then
   echo "Environment variable \"QTDIR\" not defined."
   exit 1
fi

occ_opengl_lib=libTKOpenGl.so

RES_DIR="${ROOT_DIR}/res"
BIN_DIR="${ROOT_DIR}/${STATION}/bin"

CSF_GraphicShr="${CASROOT}/${STATION}/lib/${occ_opengl_lib}"
CSF_UnitsLexicon="${CASROOT}/src/UnitsAPI/Lexi_Expr.dat"
CSF_UnitsDefinition="${CASROOT}/src/UnitsAPI/Units.dat"
CSF_MDTVFontDirectory="${CASROOT}/src/FontMFT"
CSF_MDTVTexturesDirectory="${CASROOT}/src/Textures"

CSF_ResourcesDefaults="${RES_DIR}"
CSF_TuturialResourcesDefaults="${RES_DIR}"

PATH="${BIN_DIR}:${PATH}"

export CSF_GraphicShr CSF_UnitsLexicon CSF_UnitsDefinition
export CSF_MDTVFontDirectory CSF_MDTVTexturesDirectory
export CSF_TuturialResourcesDefaults CSF_ResourcesDefaults
export PATH LD_LIBRARY_PATH

if test ! -r "${BIN_DIR}/Tutorial"; then
   echo "Executable \"${BIN_DIR}/Tutorial\" not found."
   echo "Probably you don't compile the application. Execute \"make\"."
   exit 1
fi

${BIN_DIR}/Tutorial

