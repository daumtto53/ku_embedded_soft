#! /bin/bash

sudo insmod ku_ipc.ko
dmesg | tail --lines=5
sudo sh ./ku_ipc_mknod.sh
