#! /bin/bash

MODULE="ku_ipc_dev"
MAJOR=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)
sudo mknod /dev/$MODULE c $MAJOR 0
