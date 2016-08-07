# Micro Disk

A graphical interface to create micro file system disk images.

Micro file system disk images are made for embedded projects with very limited available resources. The image is usually written to a SD card. This SD card is accessed from the MCU and can be read with minimal effort.

All files are written as sequential blocks, so a file can be read as continuous stream without bothering about allocation tables and similar.

There is an initial minimal file directory which can be easily read into memory to access the files.

All details are here:
https://luckyresistor.me/applications/micro-disk/

## License

The Micro Disk application is licensed under the GNU General Public License, version 3. See the LICENSE file for details.