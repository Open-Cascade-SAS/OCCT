#!/bin/bash

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ -e "${aSamplePath}/env.sh" ]; then source "${aSamplePath}/env.sh"; fi
cd $aSamplePath

aSystem=`uname -s`
if [ "$aSystem" == "Darwin" ] && [ "$MACOSX_USE_GLX" != "true" ]; then
  if [ "${CASDEB}" == "d" ]; then
    BIN_DIR="${aSamplePath}/build/Debug/Tutorial.app/Contents/MacOS"
  else
    BIN_DIR="${aSamplePath}/build/Release/Tutorial.app/Contents/MacOS"
  fi
else
  BIN_DIR="${aSamplePath}/${STATION}/bin${CASDEB}"
fi

CSF_ResourcesDefaults="${RES_DIR}"
CSF_TutorialResourcesDefaults="${RES_DIR}"

PATH="${BIN_DIR}:${PATH}"

export CSF_TutorialResourcesDefaults CSF_ResourcesDefaults
export PATH

if test ! -r "${BIN_DIR}/Tutorial"; then
  echo "Executable \"${BIN_DIR}/Tutorial\" not found."
  if [ "$aSystem" == "Darwin" ] && [ "$MACOSX_USE_GLX" != "true" ]; then
    echo "Probably you don't compile the application. Build it with Xcode."
  else
    echo "Probably you don't compile the application. Execute \"make\"."
  fi
  exit 1
fi

${BIN_DIR}/Tutorial
