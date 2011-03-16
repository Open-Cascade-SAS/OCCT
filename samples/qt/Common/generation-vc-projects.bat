@ECHO OFF

call env.bat

IF NOT DEFINED QTDIR GOTO ERR_QTDIR

SET PATH=%QTDIR%\bin;%PATH%

SET TMP_CASROOT=%CASROOT%
SET CASROOT=

qmake -tp vc -r IESample.pro

SET CASROOT=%TMP_CASROOT%
GOTO END

:ERR_QTDIR
ECHO Environment variable "QTDIR" has not been defined. Abort.
GOTO END

:END
