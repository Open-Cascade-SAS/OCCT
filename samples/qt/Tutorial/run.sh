#!/bin/sh

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ -e "${aSamplePath}/custom.sh" ]; then source "${aSamplePath}/env.sh"; fi
cd $aSamplePath

host=`uname -s`
STATION=$host

RES_DIR="${aSamplePath}/res"
BIN_DIR="${aSamplePath}/${STATION}/bin"

CSF_ResourcesDefaults="${RES_DIR}"
CSF_TutorialResourcesDefaults="${RES_DIR}"

PATH="${BIN_DIR}:${PATH}"

export CSF_TutorialResourcesDefaults CSF_ResourcesDefaults
export PATH

if test ! -r "${BIN_DIR}/Tutorial"; then
   echo "Executable \"${BIN_DIR}/Tutorial\" not found."
   echo "Probably you don't compile the application. Execute \"make\"."
   exit 1
fi

${BIN_DIR}/Tutorial
