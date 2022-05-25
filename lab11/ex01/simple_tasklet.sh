sudo insmod simple_tasklet.ko;
dmesg | tail --lines=10;
sudo rmmod simple_tasklet;
