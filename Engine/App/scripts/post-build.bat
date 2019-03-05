:: x86 / x64
set target=%1

:: Debug / Release
set configuration=%2

set dest=%3

:: Set variables
set target-conf=%target%\%configuration%

set fileDirectory=%~dp0
set engineLibs="%fileDirectory%..\..\lib\%target%\Win\%configuration%\"

mkdir %dest%

:: Copy Engine lib
xcopy /Y /D /S %engineLibs%\*.dll %dest%
xcopy /Y /D /S %engineLibs%\*.pdb %dest%

