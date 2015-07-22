SET PATH=C:\Qt\Qt5.4.1\5.4\msvc2013_64\bin;%PATH%
SET INCLUDE=C:\icu\include\;%INCLUDE%
SET LIB=C:\icu\lib\;%LIB%

rem needed: put this script in build dir
windeployqt.exe MangaLib.exe

pause
