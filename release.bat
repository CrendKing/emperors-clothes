del EmperorsClothes_x64.zip
D:\Application\7-Zip\7z a EmperorsClothes_x64.zip README.md LICENSE install.bat uninstall.bat
copy /y EmperorsClothes_x64.zip EmperorsClothes_x86.zip

cd emperors_clothes\x64\Release\
D:\Application\7-Zip\7z a ..\..\..\EmperorsClothes_x64.zip emperors_clothes.ax

cd ..\..\..\emperors_clothes\Win32\Release\
D:\Application\7-Zip\7z a ..\..\..\EmperorsClothes_x86.zip emperors_clothes.ax
