#!/bin/sh

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ -e "${aSamplePath}/env.sh" ]; then source "${aSamplePath}/env.sh"; fi
cd $aSamplePath
qmake IESample.pro
if test `uname -s` != "Darwin"; then
  make
fi
