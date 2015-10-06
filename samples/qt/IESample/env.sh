#!/bin/bash

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ -e "custom.sh" ]; then
   source "custom.sh";
fi

if [ -e "${aSamplePath}/../../../env.sh" ]; then
   source "${aSamplePath}/../../../env.sh";
fi

if test "${QT_DIR}" == ""; then
   echo "Environment variable \"QT_DIR\" not defined. Define it in \"custom.sh\" script."
   exit 1
fi

host=`uname -s`
export STATION=$host
export RES_DIR=${aSamplePath}/${STATION}/res

export PATH=${QT_DIR}/bin:${PATH}
