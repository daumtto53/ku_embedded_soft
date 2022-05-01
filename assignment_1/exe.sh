#! /bin/bash

sudo insmod ku_ipc.ko

sudo sh ./ku_ipc_mknod.sh

echo ""
echo "######## KU_IPC_LIB START #########"
./ku_ipc_lib_snd
echo "######## KU_IPC_LIB END #########"

echo ""
echo "######## KU_IPC_LIB START #########"
# ./ku_ipc_lib_rcv
echo "######## KU_IPC_LIB END #########"
echo ""

sudo rmmod ku_ipc
echo ""
echo "######## KERNEL #########"
dmesg | tail --lines=10
echo "######## KERNEL END #########"
echo ""
echo "Delete /dev/ku_ipc_dev"
sudo rm /dev/ku_ipc_dev
