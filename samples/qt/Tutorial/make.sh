#!/bin/sh

export aSamplePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ -e "${aSamplePath}/custom.sh" ]; then source "${aSamplePath}/env.sh"; fi
cd $aSamplePath
qmake Tutorial.pro
make
