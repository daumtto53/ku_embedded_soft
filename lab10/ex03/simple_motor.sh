sudo insmod simple_motor.ko;
dmesg | tail --lines=10;
sudo rmmod simple_motor;
