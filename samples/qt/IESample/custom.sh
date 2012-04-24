#!/bin/sh

#Define CASROOT and QTDIR variables in order to generate Makefile files by qmake

export CASROOT=""

export QTDIR=""

if test "${CASROOT}" == ""; then
   echo "Environment variable \"CASROOT\" not defined."
   exit 1
fi

if test "${QTDIR}" == ""; then
   echo "Environment variable \"QTDIR\" not defined."
   exit 1
fi

export PATH=${QTDIR}/bin:${PATH}