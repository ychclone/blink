# Compilation (QMake Windows) 

1. Download Qt Open Source installer:
https://www.qt.io/download-open-source

2. Install Qt 6.4.2 (Mingw 64 bit and its tool)

3. Set Qt and mingw 64 binaries path

```
set QTDIR=C:\Qt6\6.4.2\mingw_64
set PATH=%PATH%;%QTDIR%\bin;C:\Qt6\Tools\mingw1120_64\bin
```

4. Download QScintilla 2.14.1 and install
https://riverbankcomputing.com/software/qscintilla/download

```
cd QScintilla_src-2.14.1/src
qmake qscintilla.pro
make
make install
```

5. cd blink
6. qmake blink.pro
7. mingw32-make

# Compilation (CMake Linux) 

1. Download Qt Open Source installer:
https://www.qt.io/download-open-source

2. Install Qt 6.4.2 and its shipped CMake

3. Link qmake to /usr/bin/qmake
```
sudo ln -s /home/ychclone/Qt6/6.4.2/gcc_64/bin/qmake /usr/bin/qmake
```

3. Download QScintilla 2.14.1 and install
https://riverbankcomputing.com/software/qscintilla/download

Set to static library for qscintilla
```
qscintilla.pro:
  CONFIG += staticlib

```

Compile and install QScintilla 
```
cd QScintilla_src-2.14.1/src
qmake qscintilla.pro
make
make install
```

4. Build in CMake

```
/home/ychclone/Qt6/Tools/CMake/bin/cmake .
```

or

```
cmake -DCMAKE_PREFIX_PATH="/home/ychclone/Qt6/6.4.2/gcc_64/lib/cmake" . 
```

# AppImage

1. Download linuxdeployqt-7-x86_64.AppImage:
https://github.com/probonopd/linuxdeployqt/releases

2. chmod u+x linuxdeployqt-7-x86_64.AppImage
3. ./linuxdeployqt-7-x86_64.AppImage blinkAppImage/usr/share/applications/blink.desktop -verbose=2 -appimage -extra-plugins=iconengines,platformthemes/libqgtk3.so

