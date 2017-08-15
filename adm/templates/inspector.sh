#!/bin/bash

aCurrentPath="$PWD"
aScriptPath=${BASH_SOURCE%/*}; if [ -d "${aScriptPath}" ]; then cd "$aScriptPath"; fi; aScriptPath="$PWD";

source "${aScriptPath}/env.sh" "$1"

cd ${aCurrentPath}
TInspectorEXE
