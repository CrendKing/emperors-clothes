del EmperorsClothes_x64.zip
7z a EmperorsClothes_x64.zip README.md LICENSE install.bat uninstall.bat
copy /y EmperorsClothes_x64.zip EmperorsClothes_x86.zip

cd src\x64\Release\
7z a ..\..\..\EmperorsClothes_x64.zip emperors_clothes.ax

cd ..\..\..\src\Win32\Release\
7z a ..\..\..\EmperorsClothes_x86.zip emperors_clothes.ax
