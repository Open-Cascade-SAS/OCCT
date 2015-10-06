#!/bin/bash

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ -e "${aSamplePath}/env.sh" ]; then source "${aSamplePath}/env.sh"; fi
cd $aSamplePath
qmake IESample.pro
if [ "$(uname -s)" != "Darwin" ] || [ "$MACOSX_USE_GLX" == "true" ]; then
  if [ "${CASDEB}" == "d" ]; then
    make debug
  else
    make release
  fi
fi
