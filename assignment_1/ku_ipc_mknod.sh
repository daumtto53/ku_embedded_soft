#! /bin/bash

MODULE="ku_ipc_dev"

MAJOR=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)
mknod /dev/$MODULE c $MAJOR 0
sudo chmod 777 /dev/$MODULE
