라즈비안 설치

## 준비 과정

1. 라즈베리 파이 설치를 위해 microsd, usb 장착

2. host pc에서 SD Card 포맷.

3. win q + cmd + `diskpart` -> `list disk` -> 800MB disk -> `select disk N` -> `clean` -> `exit`

## SD 카드 포맷

1. https://raspberrypi.com/software/ 로 입장.

2. Raspberry pi imager 다운, 실행 , 설치

3. 라즈베리의 압축 풀고 바탕화면에 img 파일 획득.

4. imager의 user custom(사용자지정) -> image 파일 open -> sd card 선택

## 라즈베리 파이에 Micro SD 장착

 <- 노란 버튼 클릭 후 .. SD 카드 분리

## Host PC <-> Rasp 통신환경 구축



Case 1: WIFI w/ 공유기 관리자 권한

Case 2: WIFI

Case 3: LAN cable -> 실습실에서 사용할 것.



### case 1 :

집에 있는 공유기 관리자 페이지에 접근 가능?

1. shell

`win q + cmd +`

```
.bat

copy /Y /b / NUL ssh
copy /Y /b NUL wpa_supplicant.conf

: /Y, /b == 이진파일, yes
```

2. conf 파일 수정

`wpa_supplicant.conf`

```batch
ctrl_interface = DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
network={
    ssid="[공유기 WIFI 이름]"
    psk="[pssword]"

}
```

3. 라즈베리 파이 5핀에 연결

4. 집 공유기 ip주소 URL에 치고
   
   1. DHCP 클라이언트 목록 IP
   
   2. 가상머신 터미널에서

```shell
ssh pi@[1의 RaspPI IP]
...
...

# password = raspberry
```

         3. DHCP 설정 바꿔서 라즈베리 파이 IP주소 정적할당 가능.

### 공유기에 접근 불가능할 때.

case2, case 3.

### Case 2 - WIFI

- case2, case 3를 할 때 준비물
  
  - HDMI, 마우스, 키보드

WIFI 연결하고, `ifconfig`의 `Wlan0` 의 IP 확인

### Case 3 - 유선 랜

- 준비물 : USB 랜카드, 랜선

- 설정 -> 네트워크 및 인터넷 -> 어댑터 옵션 변경 -> 어댑터 확인 -> 속성 -> IPV4 -> 다음 IP주소 사용.

- 

- 터미널을 연다

- 의

```shell
sudo mousepad /etc/dhcpd.conf # 파일 열고
                                #Example static IP config 열의
                                #static IP_Address의 10.10.10.12/24 변경
sudo /etc/init.d/dhcpd restart  #ETHERENT
```

ubuntu shell -> 

```shell
ssh pi@10.10.10.12
#pw : raspberry


# host : ubuntu -> pi network
df -h  # disk free space

uname -a #pi kernel version
# CONFIRM PI VERSION   ### 4.19.97.v7+ ### 



#pw change
sudo passwd pi # changed

#keyboard layout-compatibility btw/ host & PI
sudo nano /etc/default/keyboard
# XXBLAYOUt="gb" -> "us"
sudo reboot
```

# 크로스 컴파일 : intel machine 기준

intel machine X86-64 file.  -> Cross Compile (ARM machine)

성능이 저조한 아키텍쳐 시스템 대신 성능이 높은 HOST PC에서 컴파일하여 완성된 실행 파일을 임베디드 시스템에 삽입만 하는 구조로 **작업의 효율성을 극대화**

* 네트워크 연결을 한 이유 : file transfer

## SCP 사용법 : 명령어로 파일 전송

* SCP -> 

## host PC에서의 cross compile 준비 :

```shell
sudo apt install - y libelf-dev build-essential pkg-config bison flex\
     bc git vim gcc-arm-linux-gnueabi ncurses-dev perl make gcc

# linux-rpi
tar xvf linux-rpi-4.19.97.update.tar.gz -C ~

cd ~/linux-rpi

make ARCH=arm CROSSCOMPILE=arm-linux-gnueabi- menuconfig
```

![](C:\Users\MINSOO\AppData\Roaming\marktext\images\2022-04-06-20-46-30-image.png)

```shell
make -j2 ARCH=arm CROSSCOMPILE=arm-lunux-gnueabi modules_prepare
```

### Compile 예제 :

원 Directory에서 Makefile 생성

```makefile
obj-m := hello.o
KERNELDIR := ~/linux-rpi/
ARM := ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD := $(shell pwd)

default :
    $(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) modules
clean :
    $(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) clean
```

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void)
{
    printk(KERN_NOTICE "Helloc world\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_NOTICE "BYE world\n");
    return 0;
}

module_init(hello_init);
module_exit(hello_exit);
```

`make` 실행 하고 `hello.ko` 전송

`scp hello.ko pi@10.10.10.12:~/`
