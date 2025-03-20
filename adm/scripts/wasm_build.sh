#!/bin/bash

# Auxiliary script for semi-automated building of OCCT for WASM platform.
# wasm_custom.sh should be configured with paths to CMake, 3rd-parties and Emscripten SDK.
# FreeType should be specified as mandatory dependency.

export aScriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export aSrcRoot="${aScriptDir}/../.."
export aBuildRoot=work

export aNbJobs=${NUMBER_OF_PROCESSORS}

export toCMake=1
export toClean=0
export toMake=1
export toInstall=1

export BUILD_ModelingData=ON
export BUILD_ModelingAlgorithms=ON
export BUILD_Visualization=ON
export BUILD_ApplicationFramework=ON
export BUILD_DataExchange=ON

if [ -f "${aScriptDir}/wasm_custom.sh" ] ; then
  . "${aScriptDir}/wasm_custom.sh"
fi

. "${EMSDK_ROOT}/emsdk_env.sh"

export aToolchain="${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"

export aGitBranch=`git symbolic-ref --short HEAD`

echo "Compilation OCCT branch : $aGitBranch"

export aPlatformAndCompiler=wasm

export aWorkDir="${aSrcRoot}/${aBuildRoot}/${aPlatformAndCompiler}-make"
if [ ! -d "${aWorkDir}" ]; then
  mkdir -p "${aWorkDir}"
fi

export aDestDir="${aSrcRoot}/${aBuildRoot}/${aPlatformAndCompiler}"
if [ ! -d "${aDestDir}" ]; then
  mkdir -p "${aDestDir}"
fi

export aLogFile="${aSrcRoot}/${aBuildRoot}/build-${aPlatformAndCompiler}.log"
if [ -f "${aLogFile}" ]; then
  rm "${aLogFile}"
fi

echo Start building OCCT for ${aPlatformAndCompiler}
echo Start building OCCT for ${aPlatformAndCompiler}>> "${aLogFile}"

pushd "${aWorkDir}"
pwd
echo toCMake=${toCMake}
if [ "${toCMake}" = "1" ]; then

echo "Configuring OCCT for WASM..."
echo cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="${aToolchain}" \
-DCMAKE_BUILD_TYPE:STRING="Release" \
-DBUILD_LIBRARY_TYPE:STRING="Static" \
-DINSTALL_DIR:PATH="${aDestDir}" \
-DINSTALL_DIR_INCLUDE:STRING="inc" \
-DINSTALL_DIR_RESOURCE:STRING="src" \
-D3RDPARTY_FREETYPE_DIR:PATH="$aFreeType" \
-D3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2:FILEPATH="$aFreeType/include" \
-D3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build:FILEPATH="$aFreeType/include" \
-DBUILD_MODULE_FoundationClasses:BOOL="ON" \
-DBUILD_MODULE_ModelingData:BOOL="${BUILD_ModelingData}" \
-DBUILD_MODULE_ModelingAlgorithms:BOOL="${BUILD_ModelingAlgorithms}" \
-DBUILD_MODULE_Visualization:BOOL="${BUILD_Visualization}" \
-DBUILD_MODULE_ApplicationFramework:BOOL="${BUILD_ApplicationFramework}" \
-DBUILD_MODULE_DataExchange:BOOL="${BUILD_DataExchange}" \
-DBUILD_MODULE_Draw:BOOL="OFF" \
-DBUILD_DOC_Overview:BOOL="OFF" "${aSrcRoot}"

cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="${aToolchain}" \
-DCMAKE_BUILD_TYPE:STRING="Release" \
-DBUILD_LIBRARY_TYPE:STRING="Static" \
-DINSTALL_DIR:PATH="${aDestDir}" \
-DINSTALL_DIR_INCLUDE:STRING="inc" \
-DINSTALL_DIR_RESOURCE:STRING="src" \
-D3RDPARTY_FREETYPE_DIR:PATH="$aFreeType" \
-D3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2:FILEPATH="$aFreeType/include" \
-D3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build:FILEPATH="$aFreeType/include" \
-DBUILD_MODULE_FoundationClasses:BOOL="ON" \
-DBUILD_MODULE_ModelingData:BOOL="${BUILD_ModelingData}" \
-DBUILD_MODULE_ModelingAlgorithms:BOOL="${BUILD_ModelingAlgorithms}" \
-DBUILD_MODULE_Visualization:BOOL="${BUILD_Visualization}" \
-DBUILD_MODULE_ApplicationFramework:BOOL="${BUILD_ApplicationFramework}" \
-DBUILD_MODULE_DataExchange:BOOL="${BUILD_DataExchange}" \
-DBUILD_MODULE_Draw:BOOL="OFF" \
-DBUILD_DOC_Overview:BOOL="OFF" "${aSrcRoot}"

  if [ $? -ne 0 ]; then
    echo "Problem during configuration"
    popd
    exit 1
  fi

fi

if [ "${toClean}" = "1" ]; then
  make clean
fi

if [ "${toMake}" = "1" ]; then
  echo Building...
  make -j ${aNbJobs} 2>> "${aLogFile}"
  if [ $? -ne 0 ]; then
    echo "Problem during make operation"
    popd
    exit 1
  fi
  echo "${aLogFile}"
fi

if [ "${toInstall}" = "1" ]; then
  echo Installing into ${aDestDir}
  make install 2>> "${aLogFile}"
fi

popd
