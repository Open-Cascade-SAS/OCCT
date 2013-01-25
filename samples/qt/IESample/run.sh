#!/bin/sh

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ -e "${aSamplePath}/env.sh" ]; then source "${aSamplePath}/env.sh"; fi
cd $aSamplePath

host=`uname -s`
STATION=$host

RES_DIR="${aSamplePath}/res"

aSystem=`uname -s`
if [ "$aSystem" == "Darwin" ]; then
  if [ "${CASDEB}" == "d" ]; then
    BIN_DIR="${aSamplePath}/build/Debug/IESample.app/Contents/MacOS"
  else
    BIN_DIR="${aSamplePath}/build/Release/IESample.app/Contents/MacOS"
  fi
else
  BIN_DIR="${aSamplePath}/${STATION}/bin${CASDEB}"
fi

CSF_ResourcesDefaults="${RES_DIR}"
CSF_IEResourcesDefaults="${RES_DIR}"

PATH="${BIN_DIR}:${PATH}"

export CSF_IEResourcesDefaults CSF_ResourcesDefaults
export PATH

if test ! -r "${BIN_DIR}/IESample"; then
  echo "Executable \"${BIN_DIR}/IESample\" not found."
  if [ "$aSystem" == "Darwin" ]; then
    echo "Probably you don't compile the application. Build it with Xcode."
  else
    echo "Probably you don't compile the application. Execute \"make\"."
  fi
  exit 1
fi

${BIN_DIR}/IESample
