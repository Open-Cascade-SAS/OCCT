#!/bin/bash

# Helper script to run generation of CBP/XCode projects on Linux / OS X.
# Running it requires that Tcl should be in the PATH

anOldPath="$PATH"
anOldLd="$LD_LIBRARY_PATH"
anOldDyLd="$DYLD_LIBRARY_PATH"

# go to the script directory
aScriptPath=${BASH_SOURCE%/*}; if [ -d "${aScriptPath}" ]; then cd "$aScriptPath"; fi; aScriptPath="$PWD";

aSystem=`uname -s`
aTarget="$1"
anOpt2=$2
anOpt3=$3
anOpt4=$4
anOpt5=$5
if [ "$aTarget" == "" ]; then
  aTarget="cbp"
  if [ "$aSystem" == "Darwin" ]; then aTarget="xcd"; fi;
fi

if [ ! -e "${aScriptPath}/custom.sh" ]; then
  tclsh "${aScriptPath}/adm/genconf.tcl"
fi

if [ ! -e "${aScriptPath}/custom.sh" ]; then
  echo custom.sh is not created. Run the script again and generate custom.sh
  exit 1
fi
source "${aScriptPath}/custom.sh"

if [ -e "${aScriptPath}/env.sh" ]; then source "${aScriptPath}/env.sh"; fi

tclsh "${aScriptPath}/adm/start.tcl" genproj -path="${aScriptPath}" -target=${aTarget} $anOpt2 $anOpt3 $anOpt4 $anOpt5

export PATH="$anOldPath"
export LD_LIBRARY_PATH="$anOldLd"
export DYLD_LIBRARY_PATH="$anOldDyLd"
