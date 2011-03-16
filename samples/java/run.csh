#!/bin/csh -f

setenv STATION `uname`

if (${STATION} == "IRIX64") setenv STATION IRIX

## CASCADE part

if (! ($?CASROOT)) then
    echo -n "Please define CASROOT to the folder containing OpenCascade '"'src'"', '"'drv'"' and '"'inc'"' folders. : "
    set res = $<
    setenv CASROOT ${res}
endif

if (! -d ${STATION}/obj) then 
    setenv JAVAHOME "${CASROOT}/../3rdparty/${STATION}/java"
endif

if ( !($?JAVAHOME) ) then
    echo -n "JAVAHOME not setted . Please define it : "
    set res = $<
    setenv JAVAHOME  ${res}
else
    setenv JDK ${JAVAHOME}
endif

## Common part
if (!($?LD_LIBRARY_PATH)) then
    setenv LD_LIBRARY_PATH ""
endif
cd java
setenv LD_LIBRARY_PATH "${cwd}:${cwd}/../${STATION}:${cwd}/../${STATION}/lib:${CASROOT}/${STATION}/lib:${LD_LIBRARY_PATH}"

## Start up
echo 'LD_LIBRARY_PATH = ' $LD_LIBRARY_PATH 
${JAVAHOME}/bin/java   -classpath .:${cwd}/../${STATION}/CASCADESamples.jar SamplesStarter



