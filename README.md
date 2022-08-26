virgo
=====

[原版](https://github.com/papplampe/virgo)已多年没有维护，此版本为C++版

Virtual Desktop Manager for Windows

[原版下载](https://github.com/papplampe/virgo/releases/download/v.1.4.2/virgo.zip)

Features:
- resource friendly, exe is <15kb on disk and uses <1mb memory while running
- 4 virtual desktops (more if you change a constant and recompile the code)
- shows only a tray icon with the number of the desktop you are on
- you can pin the window that makes it shows on every window

Hotkeys:

        ALT + 1..4             -> changes to desktop 1..4
        CTRL + 1..4            -> moves active window to desktop 1..4
        ALT + CTRL + SHIFT + Q -> exits the program
        ALT + CTRL + SHIFT + S -> starts/stops handling of other hotkeys
        ALT + CTRL + SHIFT + P -> pin active window (makes it always visible)

the nerds can build it with

        git clone https://github.com/papplampe/virgo.git
        cd virgo
        make

If you do not have gcc/make installed you can change that doing following

1. go to http://msys2.github.io/ and install it according to the instructions there
2. open msys2 shell and install mingw-w64-i686-gcc and mingw-w64-i686-make using pacman
3. duplicate C:\msys32\mingw32\bin\mingw32-make.exe and name it make.exe
