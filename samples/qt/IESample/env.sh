#!/bin/sh

if [ -e "${aSamplePath}/custom.sh" ]; then source "${aSamplePath}/custom.sh"; fi

if test "${CASROOT}" == ""; then
   echo "Environment variable \"CASROOT\" not defined. Define it in \"custom.sh\" script."
   exit 1
fi

if test "${QTDIR}" == ""; then
   echo "Environment variable \"QTDIR\" not defined. Define it in \"custom.sh\" script."
   exit 1
fi

if test `uname -s` == "Darwin" && test "${WOKHOME}" == ""; then
   echo "Environment variable \"WOKHOME\" not defined. Define it in \"custom.sh\" script."
   exit 1
fi

host=`uname -s`
export STATION=$host
export RES_DIR=${aSamplePath}/${STATION}/res

export PATH=${QTDIR}/bin:${PATH}

source $CASROOT/env.sh
export CSF_OPT_INC="${CASROOT}/inc:${WOKHOME}/lib:${CSF_OPT_INC}"
