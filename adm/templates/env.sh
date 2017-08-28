#!/bin/bash

# go to the script directory
aScriptPath=${BASH_SOURCE%/*}; if [ -d "${aScriptPath}" ]; then cd "$aScriptPath"; fi; aScriptPath="$PWD";

# Reset values
export CASROOT="__CASROOT__"
export CASDEB=""
export TARGET="";
export HAVE_TBB="false";
export HAVE_OPENCL="false";
export HAVE_FREEIMAGE="false";
export HAVE_FFMPEG="false";
export HAVE_GL2PS="false";
export HAVE_VTK="false";
export HAVE_GLES2="false";
export HAVE_ZLIB="false";
export HAVE_LIBLZMA="false";
export MACOSX_USE_GLX="false";
export CSF_OPT_INC=""
export CSF_OPT_LIB32=""
export CSF_OPT_LIB64=""
export CSF_OPT_BIN32=""
export CSF_OPT_BIN64=""

# ----- Set local settings -----
if [ "${CASROOT}" != "" ] && [ -d "${aScriptPath}/${CASROOT}" ]; then
  export CASROOT="${aScriptPath}/${CASROOT}"
fi
if [ "${CASROOT}" == "" ]; then
  export CASROOT="${aScriptPath}"
fi
if [ -e "${CASROOT}/custom.sh" ]; then source "${CASROOT}/custom.sh"; fi

# Read script arguments
shopt -s nocasematch
for i in $*
do
  if [ "$i" == "d" ] || [ "$i" == "debug" ]; then
    export CASDEB="d"
  elif [ "$i" == "i" ] || [ "$i" == "relwithdeb" ]; then
    export CASDEB="i"
  elif [ "$i" == "cbp" ]; then
    export TARGET="cbp";
  elif [ "$i" == "xcd" ] || [ "$i" == "xcode" ]; then
    export TARGET="xcd";
  fi
done
shopt -u nocasematch

# ----- Setup Environment Variables -----
anArch=`uname -m`
if [ "$anArch" != "x86_64" ] && [ "$anArch" != "ia64" ]; then
  export ARCH="32";
else
  export ARCH="64";
fi

aSystem=`uname -s`
if [ "$aSystem" == "Darwin" ]; then
  export WOKSTATION="mac";
  export ARCH="64";
else
  export WOKSTATION="lin";
fi

export CASBIN=""
if [ "${TARGET}" == "cbp" ]; then
  export CASBIN="${WOKSTATION}/cbp"
elif [ "${TARGET}" == "xcd" ]; then
  export CASBIN="adm/mac/xcd/build"
fi

export CSF_OPT_INC="${CSF_OPT_INC}:${CASROOT}/inc"

if [ "${TARGET}" == "cbp" ]; then
  export CSF_OPT_LIB32D="${CSF_OPT_LIB32}:${CASROOT}/${CASBIN}/libd"
  export CSF_OPT_LIB64D="${CSF_OPT_LIB64}:${CASROOT}/${CASBIN}/libd"
  export CSF_OPT_LIB32="${CSF_OPT_LIB32}:${CASROOT}/${CASBIN}/lib"
  export CSF_OPT_LIB64="${CSF_OPT_LIB64}:${CASROOT}/${CASBIN}/lib"
  export CSF_OPT_LIB32I="${CSF_OPT_LIB32}:${CASROOT}/${CASBIN}/libi"
  export CSF_OPT_LIB64I="${CSF_OPT_LIB64}:${CASROOT}/${CASBIN}/libi"
elif [ "${TARGET}" == "xcd" ]; then
  export CSF_OPT_LIB32D="${CSF_OPT_LIB32}:${CASROOT}/${CASBIN}/Debug"
  export CSF_OPT_LIB64D="${CSF_OPT_LIB64}:${CASROOT}/${CASBIN}/Debug"
  export CSF_OPT_LIB32="${CSF_OPT_LIB32}:${CASROOT}/${CASBIN}/Release"
  export CSF_OPT_LIB64="${CSF_OPT_LIB64}:${CASROOT}/${CASBIN}/Release"
  export CSF_OPT_LIB32I="${CSF_OPT_LIB32}:${CASROOT}/${CASBIN}/RelWithDebInfo"
  export CSF_OPT_LIB64I="${CSF_OPT_LIB64}:${CASROOT}/${CASBIN}/RelWithDebInfo"
fi

export CSF_OPT_CMPL=""

# Optiona 3rd-parties should be enabled by HAVE macros
if [ "$HAVE_TBB"       == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_TBB"; fi
if [ "$HAVE_OPENCL"    == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_OPENCL"; fi
if [ "$HAVE_FREEIMAGE" == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_FREEIMAGE"; fi
if [ "$HAVE_FFMPEG"    == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_FFMPEG"; fi
if [ "$HAVE_GL2PS"     == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_GL2PS"; fi
if [ "$HAVE_GLES2"     == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_GLES2"; fi
if [ "$HAVE_VTK"       == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_VTK"; fi
if [ "$HAVE_ZLIB"      == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_ZLIB"; fi
if [ "$HAVE_LIBLZMA"   == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DHAVE_LIBLZMA"; fi
# Option to compile OCCT with X11 libs on Mac OS X
if [ "$MACOSX_USE_GLX" == "true" ]; then export CSF_OPT_CMPL="${CSF_OPT_CMPL} -DMACOSX_USE_GLX"; fi

# To split string into array
aDelimBack=$IFS
IFS=":"

# 3rd-parties additional include paths
set -- "$CSF_OPT_INC"
declare -a aPartiesIncs=($*)
for anItem in ${aPartiesIncs[*]}
do
  export CSF_OPT_CMPL="${CSF_OPT_CMPL} -I${anItem}";
done

# Append 3rd-parties to LD_LIBRARY_PATH
if [ "$ARCH" == "32" ]; then
  set -- "$CSF_OPT_LIB32"
  declare -a aPartiesLibs=($*)
  set -- "$CSF_OPT_LIB32D"
  declare -a aPartiesLibsDeb=($*)
  set -- "$CSF_OPT_LIB32I"
  declare -a aPartiesLibsRelWithDebInfo=($*)
else
  set -- "$CSF_OPT_LIB64"
  declare -a aPartiesLibs=($*)
  set -- "$CSF_OPT_LIB64D"
  declare -a aPartiesLibsDeb=($*)
  set -- "$CSF_OPT_LIB64I"
  declare -a aPartiesLibsRelWithDebInfo=($*)
fi

# Turn back value
IFS=$aDelimBack

OPT_LINKER_OPTIONS_DEB=""
for anItem in ${aPartiesLibsDeb[*]}
do
  OPT_LINKER_OPTIONS_DEB="${OPT_LINKER_OPTIONS_DEB} -L${anItem}"
done

OPT_LINKER_OPTIONS_REL_WITH_DEB_INFO=""
for anItem in ${aPartiesLibsRelWithDebInfo[*]}
do
  OPT_LINKER_OPTIONS_REL_WITH_DEB_INFO="${OPT_LINKER_OPTIONS_REL_WITH_DEB_INFO} -L${anItem}"
done

OPT_LINKER_OPTIONS=""
for anItem in ${aPartiesLibs[*]}
do
  if [ "${LD_LIBRARY_PATH}" == "" ]; then
    export LD_LIBRARY_PATH="${anItem}"
  else
    export LD_LIBRARY_PATH="${anItem}:${LD_LIBRARY_PATH}"
  fi
  OPT_LINKER_OPTIONS="${OPT_LINKER_OPTIONS} -L${anItem}"
done

if [ "$ARCH" == "64" ]; then
  export CSF_OPT_LNK64="$OPT_LINKER_OPTIONS"
  export CSF_OPT_LNK64D="$OPT_LINKER_OPTIONS_DEB"
  export CSF_OPT_LNK64I="$OPT_LINKER_OPTIONS_REL_WITH_DEB_INFO"
else
  export CSF_OPT_LNK32="$OPT_LINKER_OPTIONS"
  export CSF_OPT_LNK32D="$OPT_LINKER_OPTIONS_DEB"
  export CSF_OPT_LNK32I="$OPT_LINKER_OPTIONS_REL_WITH_DEB_INFO"
fi

# ----- Default paths to sub-folders (can be different in install env) -----
export CSF_OCCTIncludePath="${CSF_OCCTIncludePath:-$CASROOT/inc}"
export CSF_OCCTResourcePath="${CSF_OCCTResourcePath:-$CASROOT/src}"
export CSF_OCCTSamplesPath="${CSF_OCCTSamplesPath:-$CASROOT/samples}"
export CSF_OCCTDataPath="${CSF_OCCTDataPath:-$CASROOT/data}"
export CSF_OCCTTestsPath="${CSF_OCCTTestsPath:-$CASROOT/tests}"

if [ "${TARGET}" == "xcd" ]; then
  if [ "${CASDEB}" == "d" ]; then
    export CSF_OCCTBinPath="${CSF_OCCTBinPath:-$CASROOT/$CASBIN/Debug}"
  else
    export CSF_OCCTBinPath="${CSF_OCCTBinPath:-$CASROOT/$CASBIN/Release}"
  fi
  export CSF_OCCTLibPath="${CSF_OCCTLibPath:-$CSF_OCCTBinPath}"
else
  export CSF_OCCTBinPath="${CSF_OCCTBinPath:-$CASROOT/$CASBIN/bin$CASDEB}"
  export CSF_OCCTLibPath="${CSF_OCCTLibPath:-$CASROOT/$CASBIN/lib$CASDEB}"
fi

export PATH="${CSF_OCCTBinPath}:${PATH}"
export LD_LIBRARY_PATH="${CSF_OCCTLibPath}:${LD_LIBRARY_PATH}"
if [ "$WOKSTATION" == "mac" ]; then
  export DYLD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${DYLD_LIBRARY_PATH}"
fi

# Set envoronment variables used by OCCT
export CSF_LANGUAGE="us"
export MMGT_CLEAR="1"
export CSF_SHMessage="${CASROOT}/src/SHMessage"
export CSF_MDTVTexturesDirectory="${CASROOT}/src/Textures"
export CSF_ShadersDirectory="${CASROOT}/src/Shaders"
export CSF_XSMessage="${CASROOT}/src/XSMessage"
export CSF_TObjMessage="${CASROOT}/src/TObj"
export CSF_StandardDefaults="${CASROOT}/src/StdResource"
export CSF_PluginDefaults="${CASROOT}/src/StdResource"
export CSF_XCAFDefaults="${CASROOT}/src/StdResource"
export CSF_TObjDefaults="${CASROOT}/src/StdResource"
export CSF_StandardLiteDefaults="${CASROOT}/src/StdResource"
export CSF_IGESDefaults="${CASROOT}/src/XSTEPResource"
export CSF_STEPDefaults="${CASROOT}/src/XSTEPResource"
export CSF_XmlOcafResource="${CASROOT}/src/XmlOcafResource"
export CSF_MIGRATION_TYPES="${CASROOT}/src/StdResource/MigrationSheet.txt"

# Draw Harness special stuff
if [ -e "${CASROOT}/src/DrawResources" ]; then
  export DRAWHOME="${CASROOT}/src/DrawResources"
  export CSF_DrawPluginDefaults="${DRAWHOME}"
fi
