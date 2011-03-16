#!/bin/csh -f

setenv STATION `uname -s`                                                                                                                                   
if (${STATION} == "IRIX64") setenv STATION IRIX

setenv SAMPLE_ROOT `pwd`

cd $STATION

if ( ! -d obj ) then
    mkdir obj
endif

if ( ! -d classes ) then
    mkdir classes
endif
if ( !($?JAVAHOME) ) then
    echo -n "JAVAHOME not setted . Please define it : "
    set res = $<
    setenv JAVAHOME  ${res}
else
    setenv JDK ${JAVAHOME}
endif

${JAVAHOME}/bin/javac -d classes ${CASROOT}/src/jcas/*.java ${SAMPLE_ROOT}/java/CASCADESamplesJni/*.java ${SAMPLE_ROOT}/java/SampleGeometryJni/*.java ${SAMPLE_ROOT}/java/SamplesTopologyJni/*.java ${SAMPLE_ROOT}/java/SampleViewer3DJni/*.java ${SAMPLE_ROOT}/java/SampleAISBasicJni/*.java ${SAMPLE_ROOT}/java/SampleAISDisplayModeJni/*.java ${SAMPLE_ROOT}/java/SampleAISSelectJni/*.java ${SAMPLE_ROOT}/java/SampleDisplayAnimationJni/*.java ${SAMPLE_ROOT}/java/SampleImportExportJni/*.java ${SAMPLE_ROOT}/java/SampleHLRJni/*.java ${SAMPLE_ROOT}/java/util/*.java ${SAMPLE_ROOT}/java/util/x11/*.java ${SAMPLE_ROOT}/java/*.java

${JAVAHOME}/bin/jar -cf CASCADESamples.jar -C classes/ .

rm -R classes
