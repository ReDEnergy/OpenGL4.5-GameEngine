set fileDirectory=%~dp0
set dest=%fileDirectory%..\export
set src=%fileDirectory%..\Source
set lib="%fileDirectory%..\lib"

mkdir "%dest%"
mkdir "%dest%\include\Engine"

:: Copy Engine lib
xcopy /Y /D /S "%src%\*.h" "%dest%\include\Engine"
xcopy /Y /D /S "%lib%\*.lib" "%dest%\lib\"
xcopy /Y /D /S "%lib%\*.dll" "%dest%\lib\"
xcopy /Y /D /S "%lib%\*.pdb" "%dest%\lib\"
xcopy /Y /D /S "%lib%\*.so.*" "%dest%\lib\"

