set target=%1
set platform=%2

set fileDirectory=%~dp0
set libs=%fileDirectory%..\..\..\libs
set dest=%fileDirectory%..\..\lib\%platform%

mkdir %dest%

:: Copy Engine dependencies
xcopy /Y /D /S "%libs%\assimp\%target%\%platform%\*.so" %dest%
xcopy /Y /D /S "%libs%\FreeType\%target%\%platform%\*.so" %dest%
xcopy /Y /D /S "%libs%\freetype-gl\%target%\%platform%\*.a" %dest%
