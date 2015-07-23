#!/bin/bash

export TARGET="xcd"

source ./env.sh "$1" "$TARGET"

open -a Xcode ./adm/mac/xcd/OCCT.xcworkspace
