SET MSBUILD=C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe
SET PROJECT="..\Visual Studio\Engine.vcxproj"
SET CONFIGURATION="Release"
SET PLATFORM="x86"

%MSBUILD% %PROJECT% /property:Configuration=%CONFIGURATION% /property:Platform=%PLATFORM%
PAUSE