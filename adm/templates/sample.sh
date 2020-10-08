#!/bin/bash

if [ "$1" == "" ]; then
  echo Launch selected sample as follows:
  echo   sample.sh SampleName d
  echo or to use last sample build configuration:
  echo   sample.sh SampleName
  echo available samples:
  echo    FuncDemo
  echo    IESample
  echo    OCCTOverview
  echo    Tutorial
fi


aCurrentPath="$PWD"
aScriptPath=${BASH_SOURCE%/*}; if [ -d "${aScriptPath}" ]; then cd "$aScriptPath"; fi; aScriptPath="$PWD";

source "${aScriptPath}/env.sh" "$2"

if test "${QTDIR}" == ""; then
  if [ -d "$QTDIR%\qml" ];
    then export QML2_IMPORT_PATH="$QTDIR\qml";
  fi
fi

export "EXE_PATH=$CSF_OCCTBinPath/$1"

if [ ! -f "$EXE_PATH" ]; then
  echo "Executable \"$EXE_PATH\" not found."
  echo "Probably you didn't compile the application."
  exit 1
fi

export CSF_OCCTOverviewSampleCodePath="${CSF_OCCTSamplesPath}/OCCTOverview/code"

cd ${aCurrentPath}
"$EXE_PATH"
