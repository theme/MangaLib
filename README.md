MangaLib
========
is a soft to record comic archieve file md5 and your rating automatically.
so that you know you have read it the next time you meet the same file.

1. first time running, you need to specify a path for database, it only contains manga file name and hash value, it will not grow too big, I guess.
2. browse to a folder, all the *.zip, *.rar, *.cbz, *.cbr files' md5 will be calculated and recorded.
( too many or too large file will cause hanging reading disk... but this is needed to be done only once. )
3. you can rate directly for each file, your rating score will be recorded.

Shortcut
--------
1. Inside Explorer, Ctrl+L will focus address bar. ( just like most browser )
2. To hide tags panel: click at any column other than file name column.

Build
=====
Static linking - use msys2
--------------------------
follow guides at https://wiki.qt.io/MSYS2
Needed packages :
* mingw-w64-x86_64-qt5-static
* mingw-w64-x86_64-binutils
* mingw-w64-x86_64-gcc
* mingw-w64-x86_64-libwebp
* mingw-w64-x86_64-jasper

Dynamic linking - use Qt from qt.io
-----------------------------------
from http://www.qt.io/download-open-source/
