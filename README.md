# Micro Disk

A graphical interface to create micro file system disk images.

Micro file system disk images are made for embedded projects with very limited available resources. The image is usually written to a SD card. This SD card is accessed from the MCU and can be read with minimal effort.

All files are written as sequential blocks, so a file can be read as continuous stream without bothering about allocation tables and similar.

There is an initial minimal file directory which can be easily read into memory to access the files.

All details are here:
https://luckyresistor.me/applications/micro-disk/

## Copyright

(c)2017 by Lucky Resistor

## License

The Micro Disk application is licensed under the GNU General Public License, version 3. See the LICENSE file for details.

## Precompiled Binaries

- You find setups for macOS and Windows here: https://luckyresistor.me/applications/micro-disk/
- For Linux: Please compile the project for your distribution. 

## Requirements

- Download **Qt Creator** from http://qt.io/ and install Qt version 5.7 for your platform.
- **Nullsoft Installer** (NSIS) version 3.0 for the Setup from https://sourceforge.net/projects/nsis/

## How to Create a Windows Setup

- Compile a release version of Micro Disk.
- Create a `Setup` subfolder in the project folder.
- Search the `MicroDisk.exe` and copy it into the `Setup` folder.
- Open a command line.
- `cd` to the `Setup` folder.
- Add your Qt `bin` folder to `PATH`. E.g.: `PATH=%PATH%;c:\qt\5.7\mingw53_32\bin`
- Start `windeployqt MicroDisk.exe`. This will copy all required DLL and other required files to the `Setup` directory.
- Try to start `MicroDisk.exe` from this folder. Add any missing DLLs to the `Setup` folder. Usually `libstdc++-6.dll`, `libwinpthread-1.dll` and `libgcc_s_dw2-1.dll` are missing. Copy the required DLL until the executable starts from this directory.
- Now start the `NSIS` program and compile the `MicroDisk.nsi` script.

## How to Create a macOS Setup

- Compile a release version of Micro Disk.
- Create a `Setup` subfolder in the project folder.
- Search the `MicroDisk.app` and copy it into the `Setup` folder.
- Open a terminal.
- `cd` to the `Setup` folder.
- Run `macdeployqt` with `MicroDisk.app` and `-dmg` as argument. E.g. `~/Qt/5.7/clang_64/bin/macdeployqt MicroDisk.app -dmg`
- This will automatically do the magic, copy all required frameworks into the `.app` and create a DMG file with the application.

## How to contribute

Please contact me in advance.
