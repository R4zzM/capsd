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
* A Linux distro with a kernel version > 2.6.34
* Systemd (if smartcaps should run as a service)

Installation
------------
Simply run the script _install.sh_ with root permissions. You are done. 

The script will:
- Install smartcaps
- Add smartcaps as a system service to start on boot
- Start smartcaps
