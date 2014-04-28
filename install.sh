#!/bin/bash

APPNAME=smartcaps

detect_init_daemon() {
  if [[ $(which systemctl > /dev/null 2>&1) -eq 0 ]]; then
    SERVICE_DAEMON="systemd"
  fi
  # elif [[ $(which initctl > /dev/null 2>&1) -eq 0 ]]; then
  #   SERVICE_DAEMON="upstart"
}

install_service() {
  if [[ $SERVICE_DAEMON == "systemd" ]]; then
    cp etc/systemd/$APPNAME.service /etc/systemd/system/
    systemctl enable $APPNAME
  fi
}

start_service() {
  if [[ $SERVICE_DAEMON == "systemd" ]]; then
    systemctl start $APPNAME
  fi
}

# # # # #
# Main  #
# # # # #

if [[ $(whoami) != "root" ]]; then 
  echo "Must be root in order to install." 
  exit 1
fi

echo -n "Detecting service daemon..." 
detect_init_daemon
if [[ -z $SERVICE_DAEMON ]]; then
  echo "Aborted. Could not find systemd or upstart."
  exit 1
else 
  echo "Done. Found: $SERVICE_DAEMON"
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
