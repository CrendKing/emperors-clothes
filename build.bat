set sdk_version=10.0.17134.0

mkdir dep
cd dep
git init
git remote add -f origin https://github.com/microsoft/Windows-classic-samples.git
git config core.sparseCheckout true
echo /Samples/Win7Samples/multimedia/directshow/baseclasses > .git/info/sparse-checkout
git reset --hard origin/master
cd ..
move dep\Samples\Win7Samples\multimedia\directshow\baseclasses .

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

devenv.exe /Upgrade baseclasses\baseclasses.sln

MSBuild.exe /p:Configuration=Release_MBCS;Platform=x64;WindowsTargetPlatformVersion=%sdk_version% /m /nologo baseclasses\baseclasses.sln
MSBuild.exe /p:Configuration=Release;Platform=x64;WindowsTargetPlatformVersion=%sdk_version% /m /nologo %~dp0src\emperors_clothes.sln

MSBuild.exe /p:Configuration=Release_MBCS;Platform=Win32;WindowsTargetPlatformVersion=%sdk_version% /m /nologo baseclasses\baseclasses.sln
MSBuild.exe /p:Configuration=Release;Platform=x86;WindowsTargetPlatformVersion=%sdk_version% /m /nologo %~dp0src\emperors_clothes.sln
