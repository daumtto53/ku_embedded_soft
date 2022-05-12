sudo insmod simple_speaker.ko;
dmesg | tail --lines=10;
sudo rmmod simple_speaker;
