call "%~dp0..\..\..\env.bat" %1 %2 %3

SET "SAMPLESROOT=%~dp0.."
SET "RES_DIR=%~dp0win%ARCH%\%VCVER%\res"
SET "CSF_ResourcesDefaults=%RES_DIR%"
SET "CSF_IEResourcesDefaults=%RES_DIR%"
