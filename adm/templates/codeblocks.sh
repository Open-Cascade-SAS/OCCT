#!/bin/bash

export TARGET="cbp"

source ./env.sh "$1" "$TARGET"

if [ -e "/Applications/CodeBlocks.app/Contents/MacOS/CodeBlocks" ]; then
  /Applications/CodeBlocks.app/Contents/MacOS/CodeBlocks ./adm/$WOKSTATION/cbp/OCCT.workspace
else
  codeblocks ./adm/$WOKSTATION/cbp/OCCT.workspace
fi
