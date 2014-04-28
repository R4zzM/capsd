#!/bin/bash

APPNAME=smartcaps

detect_init_daemon() {
  $(which systemctl > /dev/null 2>&1)
  exitcode=$?
  if [[ $exitcode == 0 ]]; then
    INIT_DAEMON="systemd"
    return
  fi

  $(which initctl > /dev/null 2>&1)
  exitcode=$?
  if [[ $exitcode == 0 ]]; then
    INIT_DAEMON="upstart"
    return
  fi
}

install_service() {
  if [[ $INIT_DAEMON == "systemd" ]]; then
    cp etc/systemd/$APPNAME.service /etc/systemd/system/
    systemctl enable $APPNAME
  elif [[ $INIT_DAEMON == "upstart" ]]; then
    cp etc/upstart/$APPNAME.conf /etc/init/
  fi
}

start_service() {
  if [[ $INIT_DAEMON == "systemd" ]]; then
    systemctl start $APPNAME
  elif [[ $INIT_DAEMON == "upstart" ]]; then
    initctl start $APPNAME
  fi
}

# # # # #
# Main  #
# # # # #

if [[ $(whoami) != "root" ]]; then 
  echo "Must be root in order to install." 
  exit 1
fi

echo -n "Detecting init daemon..." 
detect_init_daemon
if [[ -z $INIT_DAEMON ]]; then
  echo "Aborted. Could not find systemd or upstart."
  exit 1
else 
  echo "Done. Using: $INIT_DAEMON"
fi

echo "Building..."
make clean
make

echo "Installing application..."
make install

echo "Installing service..."
install_service

echo "Starting service..."
start_service

echo "All done! $APPNAME is installed and running. Enjoy!"
exit 0
