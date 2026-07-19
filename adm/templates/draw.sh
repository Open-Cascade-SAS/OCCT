#!/bin/bash

aCurrentPath="$PWD"
aScriptPath=${BASH_SOURCE%/*}; if [ -d "${aScriptPath}" ]; then cd "$aScriptPath"; fi; aScriptPath="$PWD";

source "${aScriptPath}/env.sh" "$1"

echo 'Hint: use "pload ALL" command to load standard commands'
cd ${aCurrentPath}
DRAWEXE
