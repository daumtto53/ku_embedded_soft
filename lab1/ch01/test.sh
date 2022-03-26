#!/bin/bash

sudo insmod ../ex00/margin.ko
sudo rmmod  ../ex00/margin.ko
sudo insmod mod1/ch1_mod1_201512285.ko
sudo insmod mod2/ch1_mod2_201512285.ko

sudo rmmod mod2/ch1_mod2_201512285.ko
sudo rmmod mod1/ch1_mod1_201512285.ko

dmesg | tail --line=15

