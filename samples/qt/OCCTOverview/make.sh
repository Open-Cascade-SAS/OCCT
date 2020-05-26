#!/bin/bash

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ -e "${aSamplePath}/env.sh" ]; then source "${aSamplePath}/env.sh" $*; fi

cd $aSamplePath
qmake OCCTOverview.pro
if [ "$(uname -s)" != "Darwin" ] || [ "$MACOSX_USE_GLX" == "true" ]; then
  aNbJobs="$(getconf _NPROCESSORS_ONLN)"
  if [ "${CASDEB}" == "d" ]; then
    make -j $aNbJobs debug
  else
    make -j $aNbJobs release
  fi
fi
