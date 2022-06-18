# OS Development Course Material

## Introduction
This repository contains materials I use for a private course delivered to friends. The codebase is intended for educational
purpose and may be somewhat dirty to keep it simple for beginners.

## Get Started
### Prerequisite
An x86 linux box, but mac (either intel or apple silicon) can be used too with a bit tweaks.

### Install tools
```
cd tools
./install_binutils.sh
./install_gcc.sh
./install_gdb.sh

export PATH=$PATH:$HOME/.local/cross/bin
```
See [lesson 01](doc/lesson-01.md) for more information, especially if you run on mac.

### Build & Run
```
make
make run
```
> You may want to adjust the qemu command to switch from curses ui to real GUI.

## Lesson Notes
Lesson notes are available in the [doc](doc/) directory. We conduct the course over online meeting so the notes are
not self-contained, however it can still point you to some useful information.
