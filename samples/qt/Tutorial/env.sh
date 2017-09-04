#!/bin/bash

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ -e "custom.sh" ]; then
  source "custom.sh" $*;
fi

if [ -e "${aSamplePath}/../../../env.sh" ]; then
  source "${aSamplePath}/../../../env.sh" $*;
fi

if [ "${QTDIR}" != "" ]; then
  export PATH=${QTDIR}/bin:${PATH}
else
  aQMakePath=`which qmake`
  echo "Environment variable \"QTDIR\" not defined.. Define it in \"custom.sh\" script."
  if [ -x "$aQMakePath" ]; then
    echo "qmake from PATH will be used instead."
  else
    exit 1
  fi
fi

host=`uname -s`
export STATION=$host
export RES_DIR=${aSamplePath}/${STATION}/res
