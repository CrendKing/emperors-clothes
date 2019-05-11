del EmperorsClothes-x64.zip
cd src\x64\Release\
7z a ..\..\..\EmperorsClothes-x64.zip emperors_clothes.ax ..\..\..\README.md ..\..\..\LICENSE ..\..\..\install.bat ..\..\..\uninstall.bat
cd ..\..\..\

del EmperorsClothes-x86.zip
cd src\Win32\Release\
7z a ..\..\..\EmperorsClothes-x86.zip emperors_clothes.ax ..\..\..\README.md ..\..\..\LICENSE ..\..\..\install.bat ..\..\..\uninstall.bat
cd ..\..\..\
