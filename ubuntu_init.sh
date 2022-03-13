#!/bin/bash

sudo apt-get install build-essential linux-headers-$(uname -r)

sudo rm /var/lib/apt/lists/lock; sudo rm /var/cache/apt/archives/lock; sudo rm /var/lib/dpkg/lock*;

sudo dpkg --configure -a; sudo apt update;

sudo apt-get install build-essential linux-headers-$(uname -r);

sudo apt-get install libelf-dev

sudo apt-get install util-linux

sudo apt-get install git

sudo apt-get install 

# export git token & alias dmesg | tail --line=10
vim ~/.bashrc  
