Smartcaps 
========= 

Among Vim users there are two destinct schools of how to handle the Caps Lock
key: Remap it to escape or remap it to control.  The aim of Smartcaps is to not
make these schools mutually exclusive: Tapping Caps Lock will result in an
Escape press and holding Caps Lock while pressing another key results in
Control being held down.  

Smartcaps is implemented as a Linux userspace driver and makes use of uinput
and the input subsystem.

Requirements 
------------
* A Linux distro with the uinput kernel module. This module is included by
  default in all kernels from somewhere around version 2.6.30. I don't remember
  exactly. One of the files /dev/uinput or /dev/input/uinput should exist if
  the uinput module exists and is correctly loaded.
* Systemd or Upstart if smartcaps should run as a service. Most Linux
  distributions use one of these.

Installation
------------
Simply run the script _install.sh_ with root permissions. You are done. 

The script will:
- Install smartcaps
- Start smartcaps as a system service
- Make sure that smartcaps starts every time the system boots

Known Bugs
----------
- The device file is only read once which results in that keyboards plugged into 
the USB-port after smartcaps was sterted will not be detected and thus 
the functionality of the thier caps lock key will not be altered. If a new
keyboard is plugged in the daemon has to be restarted.
