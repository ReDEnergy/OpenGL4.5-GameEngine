set target=%1
set configuration=%2
set library-conf="Release"

set target-conf=%target%\%configuration%

set fileDirectory=%~dp0
set libs="%fileDirectory%..\libs"
set dest=%fileDirectory%..\export
set src=%fileDirectory%..\Source
set lib="%fileDirectory%..\lib"

mkdir %dest%
mkdir %dest%\%target%
mkdir %dest%\include\Engine

:: Copy Engine lib
xcopy /Y /D /S "%src%\*.h" %dest%\include\Engine
xcopy /Y /D /S "%lib%\%target%\%configuration%\*.dll" %dest%\%target%\%configuration%\
xcopy /Y /D /S "%lib%\%target%\%configuration%\*.lib" %dest%\%target%\%configuration%\
xcopy /Y /D /S "%lib%\%target%\%configuration%\*.pdb" %dest%\%target%\%configuration%\

:: Copy Engine dependencies
xcopy /Y /D /S "%libs%\GL\%target%\%library-conf%\*.dll" %dest%
xcopy /Y /D /S "%libs%\GLFW\%target%\%library-conf%\*.dll" %dest%
xcopy /Y /D /S "%libs%\assimp\%target%\%library-conf%\*.dll" %dest%
