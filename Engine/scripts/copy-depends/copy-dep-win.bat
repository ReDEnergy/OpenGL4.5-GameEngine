set target=%1
set configuration=%2
set library-conf="Release"

set target-conf=%target%\%configuration%

set fileDirectory=%~dp0
set libs="%fileDirectory%..\..\..\libs"
set dest=%fileDirectory%..\..\lib\%target%

mkdir %dest%

:: Copy Engine dependencies
xcopy /Y /D /S "%libs%\GL\%target%\%library-conf%\*.dll" %dest%
xcopy /Y /D /S "%libs%\GLFW\%target%\%library-conf%\*.dll" %dest%
xcopy /Y /D /S "%libs%\assimp\%target%\%library-conf%\*.dll" %dest%
xcopy /Y /D /S "%libs%\OpenGL\%target%\*.dll" %dest%