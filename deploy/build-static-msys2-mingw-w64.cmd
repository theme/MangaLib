SET PATH=C:\msys64\usr\bin;C:\msys64\mingw64\bin;C:\msys64\mingw64\qt5-static\bin;%PATH%
SET QMAKESPEC=win32-g++

qmake "CONFIG+=release" ..\Mangalib.pro

mingw32-make

pause
