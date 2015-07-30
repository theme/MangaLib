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
Either of the following ways will build the program: 
* (Dynamic linking) use Qt from http://www.qt.io/download-open-source/
* (Static linking) use Qt in msys2, follow guides at https://wiki.qt.io/MSYS2

(ubuntu 14.04) build error "cannot find -lGL"
refer to http://wiki.qt.io/Building_Qt_5_from_Git#Linux.2FX11
sudo apt-get install "^libxcb.*" libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev