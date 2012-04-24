#!/bin/sh

if [ -e "${aSamplePath}/custom.sh" ]; then source "${aSamplePath}/custom.sh"; fi
source $CASROOT/env.sh
export CSF_OPT_INC="${CSF_OPT_INC}${CASROOT}/inc:"
