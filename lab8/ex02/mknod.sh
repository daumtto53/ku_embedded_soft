#! /bin/sh

MODULE="simple_sensor_dev"
MAJOR=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)
echo  MODULE=$MODULE, MAJOR=$MAJOR

mknod /dev/$MODULE c $MAJOR 0
