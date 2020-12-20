#!/bin/bash

# Auxiliary script for semi-automated building of OCCT for macOS platform.
# macos_custom.sh should be configured with paths to CMake and other 3rd-parties.
# FreeType should be specified as mandatory dependency.

aScriptDir=${BASH_SOURCE%/*}
if [ -d "$aScriptDir" ]; then cd "$aScriptDir"; fi
aScriptDir="$PWD"

aCasSrc=${aScriptDir}/../..
aNbJobs="$(getconf _NPROCESSORS_ONLN)"

export aBuildRoot=work

# paths to pre-built 3rd-parties
export aFreeType=
export aFreeImage=
export aRapidJson=

# build stages to perform
export toCMake=1
export toClean=1
export toMake=1
export toInstall=1
export toPack=0
export toDebug=0

export BUILD_ModelingData=ON
export BUILD_ModelingAlgorithms=ON
export BUILD_Visualization=ON
export BUILD_ApplicationFramework=ON
export BUILD_DataExchange=ON
export BUILD_Draw=ON

export USE_FREEIMAGE=ON
export USE_RAPIDJSON=OFF

export MACOSX_DEPLOYMENT_TARGET=10.10
#export anAbi=arm64
export anAbi=x86_64

if [[ -f "${aScriptDir}/macos_custom.sh" ]]; then
  source "${aScriptDir}/macos_custom.sh"
fi

aBuildType="Release"
aBuildTypePrefix=
if [[ $toDebug == 1 ]]; then
  aBuildType="Debug"
  aBuildTypePrefix="-debug"
fi
aPlatformAndCompiler=mac-${anAbi}${aBuildTypePrefix}-clang

aWorkDir="${aCasSrc}/${aBuildRoot}/${aPlatformAndCompiler}-make"
aDestDir="${aCasSrc}/${aBuildRoot}/${aPlatformAndCompiler}"
aLogFile="${aCasSrc}/${aBuildRoot}/build-${aPlatformAndCompiler}.log"

if [[ $toCMake == 1 ]] && [[ $toClean == 1 ]]; then
  rm -r -f "$aWorkDir"
  rm -r -f "$aDestDir"
fi
mkdir -p "$aWorkDir"
mkdir -p "$aDestDir"
rm -f "$aLogFile"

anOcctVerSuffix=`grep -e "#define OCC_VERSION_DEVELOPMENT" "$aCasSrc/src/Standard/Standard_Version.hxx" | awk '{print $3}' | xargs`
anOcctVersion=`grep -e "#define OCC_VERSION_COMPLETE" "$aCasSrc/src/Standard/Standard_Version.hxx" | awk '{print $3}' | xargs`
aGitBranch=`git symbolic-ref --short HEAD`

# include some information about OCCT into archive
echo \<pre\>> "${aWorkDir}/VERSION.html"
git status >> "${aWorkDir}/VERSION.html"
git log -n 100 >> "${aWorkDir}/VERSION.html"
echo \</pre\>>> "${aWorkDir}/VERSION.html"

pushd "$aWorkDir"

aTimeZERO=$SECONDS
set -o pipefail

function logDuration {
  if [[ $1 == 1 ]]; then
    aDur=$(($4 - $3))
    echo $2 time: $aDur sec>> "$aLogFile"
  fi
}

# (re)generate Make files
if [[ $toCMake == 1 ]]; then
  echo Configuring OCCT for macOS...
  cmake -G "Unix Makefiles" \
  -D CMAKE_BUILD_TYPE:STRING="$aBuildType" \
  -D BUILD_LIBRARY_TYPE:STRING="Shared" \
  -D CMAKE_OSX_ARCHITECTURES:STRING="$anAbi" \
  -D INSTALL_DIR:PATH="$aDestDir" \
  -D INSTALL_DIR_INCLUDE:STRING="inc" \
  -D INSTALL_DIR_LIB:STRING="lib" \
  -D INSTALL_DIR_RESOURCE:STRING="src" \
  -D INSTALL_NAME_DIR:STRING="@executable_path/../Frameworks" \
  -D 3RDPARTY_FREETYPE_DIR:PATH="$aFreeType" \
  -D 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2:FILEPATH="$aFreeType/include" \
  -D 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build:FILEPATH="$aFreeType/include" \
  -D 3RDPARTY_FREETYPE_LIBRARY_DIR:PATH="$aFreeType/lib" \
  -D 3RDPARTY_FREETYPE_LIBRARY:FILEPATH="$aFreeType/lib/libfreetype.dylib" \
  -D USE_RAPIDJSON:BOOL="$USE_RAPIDJSON" \
  -D 3RDPARTY_RAPIDJSON_DIR:PATH="$aRapidJson" \
  -D 3RDPARTY_RAPIDJSON_INCLUDE_DIR:PATH="$aRapidJson/include" \
  -D USE_FREEIMAGE:BOOL="$USE_FREEIMAGE" \
  -D 3RDPARTY_FREEIMAGE_DIR:PATH="$aFreeImage" \
  -D 3RDPARTY_FREEIMAGE_INCLUDE_DIR:FILEPATH="$aFreeImage/include" \
  -D 3RDPARTY_FREEIMAGE_LIBRARY_DIR:PATH="$aFreeImage/lib" \
  -D 3RDPARTY_FREEIMAGE_LIBRARY:FILEPATH="$aFreeImage/lib/libfreeimage.a" \
  -D BUILD_MODULE_FoundationClasses:BOOL="ON" \
  -D BUILD_MODULE_ModelingData:BOOL="${BUILD_ModelingData}" \
  -D BUILD_MODULE_ModelingAlgorithms:BOOL="${BUILD_ModelingAlgorithms}" \
  -D BUILD_MODULE_Visualization:BOOL="${BUILD_Visualization}" \
  -D BUILD_MODULE_ApplicationFramework:BOOL="${BUILD_ApplicationFramework}" \
  -D BUILD_MODULE_DataExchange:BOOL="${BUILD_DataExchange}" \
  -D BUILD_MODULE_Draw:BOOL="${BUILD_Draw}" \
  -D BUILD_DOC_Overview:BOOL="OFF" \
  "$aCasSrc" 2>&1 | tee -a "$aLogFile"
  aResult=$?; if [[ $aResult != 0 ]]; then exit $aResult; fi
fi
aTimeGEN=$SECONDS
logDuration $toCMake "Generation" $aTimeZERO $aTimeGEN

# clean up from previous build
if [[ $toClean == 1 ]]; then
  make clean
fi

# build the project
if [[ $toMake == 1 ]]; then
  echo Building OCCT...
  make -j $aNbJobs 2>&1 | tee -a "$aLogFile"
  aResult=$?; if [[ $aResult != 0 ]]; then exit $aResult; fi
fi
aTimeBUILD=$SECONDS
logDuration $toMake "Building"       $aTimeGEN  $aTimeBUILD
logDuration $toMake "Total building" $aTimeZERO $aTimeBUILD

# install the project
if [[ $toInstall == 1 ]]; then
  echo Installing OCCT into $aDestDir...
  make install 2>&1 | tee -a "$aLogFile"
  cp -f "$aWorkDir/VERSION.html" "$aDestDir/VERSION.html"
fi
aTimeINSTALL=$SECONDS
logDuration $toInstall "Install" $aTimeBUILD $aTimeINSTALL

# create an archive
if [[ $toPack == 1 ]]; then
  YEAR=$(date +"%Y")
  MONTH=$(date +"%m")
  DAY=$(date +"%d")
  aRevision=-${YEAR}-${MONTH}-${DAY}
  #aRevision=-${aGitBranch}

  anArchName=occt-${anOcctVersion}${anOcctVerSuffix}${aRevision}-${aPlatformAndCompiler}.tar.bz2
  echo Creating an archive ${aCasSrc}/${aBuildRoot}/${anArchName}...
  rm ${aDestDir}/../${anArchName} &>/dev/null
  pushd "$aDestDir"
  tar -jcf ${aDestDir}/../${anArchName} *
  popd
fi
aTimePACK=$SECONDS
logDuration $toPack "Packing archive" $aTimeINSTALL $aTimePACK

# finished
DURATION=$(($aTimePACK - $aTimeZERO))
echo Total time: $DURATION sec
logDuration 1 "Total" $aTimeZERO $aTimePACK

popd
