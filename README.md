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
* Systemd (if smartcaps should autostart)

Installation
------------
`make` to build  
`sudo make install`      

If you have a distro that does not use systemd the last step might fail as it
tries to copy systemd unit files into a directory that doesn't exist. This will
be remedied in the future.

