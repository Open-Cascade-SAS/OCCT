#!/bin/bash

# Auxiliary script for semi-automated building of OCCT for iOS platform.
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
export aDraco=

# build stages to perform
export isStatic=1
export toCMake=1
export toClean=1
export toMake=1
export toInstall=1
export toPack=0
export toPackFat=0
export toDebug=0

export BUILD_ModelingData=ON
export BUILD_ModelingAlgorithms=ON
export BUILD_Visualization=ON
export BUILD_ApplicationFramework=ON
export BUILD_DataExchange=ON
export BUILD_DETools=OFF

export USE_FREETYPE=ON
export USE_FREEIMAGE=OFF
export USE_RAPIDJSON=OFF
export USE_DRACO=OFF

export IPHONEOS_DEPLOYMENT_TARGET=8.0
#export anAbiList="iPhoneOS|arm64 iPhoneSimulator|arm64 iPhoneSimulator|x86_64"
export anAbiList="iPhoneOS|arm64"

if [[ -f "${aScriptDir}/ios_custom.sh" ]]; then
  source "${aScriptDir}/ios_custom.sh"
fi

anOcctVerSuffix=`grep -e "#define OCC_VERSION_DEVELOPMENT" "$aCasSrc/src/Standard/Standard_Version.hxx" | awk '{print $3}' | xargs`
anOcctVersion=`grep -e "#define OCC_VERSION_COMPLETE" "$aCasSrc/src/Standard/Standard_Version.hxx" | awk '{print $3}' | xargs`
aGitBranch=`git symbolic-ref --short HEAD`

YEAR=$(date +"%Y")
MONTH=$(date +"%m")
DAY=$(date +"%d")
aRevision=-${YEAR}-${MONTH}-${DAY}
#aRevision=-${aGitBranch}

set -o pipefail

aBuildType="Release"
aBuildTypePrefix=
if [[ $toDebug == 1 ]]; then
  aBuildType="Debug"
  aBuildTypePrefix="-debug"
fi
aLibType="Shared"
if [[ $isStatic == 1 ]]; then
  aLibType="Static"
fi

function buildArch {
  anAbi=$1
  aPlatformSdk=$2

  aPlatformAndCompiler=${aPlatformSdk}-${anAbi}${aBuildTypePrefix}-clang

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

  # include some information about OCCT into archive
  echo \<pre\>> "${aWorkDir}/VERSION.html"
  git status >> "${aWorkDir}/VERSION.html"
  git log -n 100 >> "${aWorkDir}/VERSION.html"
  echo \</pre\>>> "${aWorkDir}/VERSION.html"

  pushd "$aWorkDir"

  aTimeZERO=$SECONDS

  function logDuration {
    if [[ $1 == 1 ]]; then
      aDur=$(($4 - $3))
      echo $2 time: $aDur sec>> "$aLogFile"
    fi
  }

  aSysRoot="/Applications/Xcode.app/Contents/Developer/Platforms/${aPlatformSdk}.platform/Developer/SDKs/${aPlatformSdk}.sdk"

  # (re)generate Make files
  if [[ $toCMake == 1 ]]; then
    echo Configuring OCCT for iOS...
    cmake -G "Unix Makefiles" \
  -D CMAKE_SYSTEM_NAME="iOS" \
  -D CMAKE_OSX_ARCHITECTURES:STRING="$anAbi" \
  -D CMAKE_OSX_DEPLOYMENT_TARGET:STRING="$IPHONEOS_DEPLOYMENT_TARGET" \
  -D CMAKE_OSX_SYSROOT:PATH="$aSysRoot" \
  -D CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS:BOOL="OFF" \
  -D CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS:BOOL="OFF" \
  -D CMAKE_BUILD_TYPE:STRING="$aBuildType" \
  -D BUILD_LIBRARY_TYPE:STRING="$aLibType" \
  -D INSTALL_DIR:PATH="$aDestDir" \
  -D INSTALL_DIR_INCLUDE:STRING="inc" \
  -D INSTALL_DIR_LIB:STRING="lib" \
  -D INSTALL_DIR_RESOURCE:STRING="src" \
  -D INSTALL_NAME_DIR:STRING="@executable_path/../Frameworks" \
  -D USE_FREETYPE:BOOL="$USE_FREETYPE" \
  -D 3RDPARTY_FREETYPE_DIR:PATH="$aFreeType" \
  -D 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2:FILEPATH="$aFreeType/include" \
  -D 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build:FILEPATH="$aFreeType/include" \
  -D 3RDPARTY_FREETYPE_LIBRARY_DIR:PATH="$aFreeType/lib" \
  -D USE_RAPIDJSON:BOOL="$USE_RAPIDJSON" \
  -D 3RDPARTY_RAPIDJSON_DIR:PATH="$aRapidJson" \
  -D 3RDPARTY_RAPIDJSON_INCLUDE_DIR:PATH="$aRapidJson/include" \
  -D USE_DRACO:BOOL="$USE_DRACO" \
  -D 3RDPARTY_DRACO_DIR:PATH="$aDraco" \
  -D 3RDPARTY_DRACO_INCLUDE_DIR:FILEPATH="$aDraco/include" \
  -D 3RDPARTY_DRACO_LIBRARY_DIR:PATH="$aDraco/lib" \
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
  -D BUILD_MODULE_DETools:BOOL="${BUILD_DETools}" \
  -D BUILD_MODULE_Draw:BOOL="OFF" \
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
    echo Building...
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
    echo Platform: ${aPlatformSdk} ABI: ${anAbi} Build: ${aBuildType} IPHONEOS_DEPLOYMENT_TARGET: ${IPHONEOS_DEPLOYMENT_TARGET} > "$aDestDir/build_target.txt"
  fi
  aTimeINSTALL=$SECONDS
  logDuration $toInstall "Install" $aTimeBUILD $aTimeINSTALL

  # create an archive
  if [[ $toPack == 1 ]]; then
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
}

for anArchIter in $anAbiList
do
  IFS="|" read -r aPlatform anArch <<< "$anArchIter"
  echo Platform: ${aPlatform} ABI: ${anArch} Build: ${aBuildType}
  buildArch $anArch $aPlatform
done

# create a FAT archive
if [[ $toPackFat == 1 ]]; then
  for aPlatIter in iPhoneOS iPhoneSimulator
  do
    aSuffixFat=${aPlatIter}${aBuildTypePrefix}-clang
    aFatDir="${aCasSrc}/${aBuildRoot}/${aSuffixFat}"

    # merge per-arch builds into fat builds
    hasPlatform=0
    for anArchIter in $anAbiList
    do
      IFS="|" read -r aPlatform anArch <<< "$anArchIter"
      if [[ $aPlatIter != ${aPlatform} ]]; then
        continue
      fi

      aSuffixThin=${aPlatform}-${anArch}${aBuildTypePrefix}-clang
      anArchDir="${aCasSrc}/${aBuildRoot}/${aSuffixThin}"
      if [[ $hasPlatform == 0 ]]; then
        hasPlatform=1
        echo Packing FAT archive for platform: ${aPlatform}
        rm -r -f "$aFatDir"
        mkdir -p "$aFatDir"
        rsync -r --exclude '*.a' "$anArchDir/" "$aFatDir"
        rm -f "$aFatDir/build_target.txt"
        for aLibIter in $anArchDir/lib/*.a; do
          aLibName=`basename $aLibIter`
          lipo "$anArchDir/lib/$aLibName" -output "$aFatDir/lib/$aLibName" -create
        done
      else
        for aLibIter in $aFatDir/lib/*.a; do
          aLibName=`basename $aLibIter`
          lipo "$aFatDir/lib/$aLibName" "$anArchDir/lib/$aLibName" -output "$aFatDir/lib/$aLibName" -create
          #lipo -info "$aFatDir/lib/$aLibName"
        done
      fi
      cat "$anArchDir/build_target.txt" >> "$aFatDir/build_target.txt"
    done

    # create an archive
    for anArchIter in $anAbiList
    do
      IFS="|" read -r aPlatform anArch <<< "$anArchIter"
      if [[ $aPlatIter != ${aPlatform} ]]; then
        continue
      fi

      anArchName=occt-${anOcctVersion}${anOcctVerSuffix}${aRevision}-${aSuffixFat}.tar.bz2
      echo Creating an archive ${aCasSrc}/${aBuildRoot}/${anArchName}...
      rm ${aFatDir}/../${anArchName} &>/dev/null
      pushd "$aFatDir"
      tar -jcf ${aFatDir}/../${anArchName} *
      popd
      break
    done
  done
fi
