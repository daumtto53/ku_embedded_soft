sudo insmod simple_kthread.ko;
dmesg | tail --lines=10;
sudo rmmod simple_kthread;
