capsd 
=====

Among Vim users there are two destinct schools of how to handle the Caps Lock
key: Remap it to escape or remap it to control.  The aim of capsd is to not
make these schools mutually exclusive: Tapping Caps Lock will result in an
Escape press and holding Caps Lock while pressing another key results in
Control being held down.  

capsd is implemented as a Linux userspace driver and makes use of uinput
and the input subsystem.

Requirements 
------------

* A Linux distro with the uinput kernel module. This module is included by
  default in all kernels from somewhere around version 2.6.30. One of the files
  /dev/uinput or /dev/input/uinput should exist if the uinput module exists and
  is correctly loaded.
* Systemd or Upstart if capsd should run as a service. Most common Linux
  distributions use one of these.

Installation
------------

Simply run the script _install.sh_ with root permissions. You are done. 

The script will:
- Install capsd
- Start capsd as a system service
- Make sure that capsd starts every time the system boots

