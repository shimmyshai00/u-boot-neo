#!/bin/bash
export ROCKCHIP_TPL=/home/shimmy/Software/Source-Code-Builds/rkbin/bin/rk35/rk3588_ddr_lp4_2112MHz_lp5_2736MHz_v1.08.bin
export BL31=/home/shimmy/Software/Source-Code-Builds/rkbin/bin/rk35/rk3588_bl31_v1.27.elf
if [ ! -f ".config" ]; then
	make CROSS_COMPILE=aarch64-linux-gnu- evb-rk3588_defconfig
fi
make CROSS_COMPILE=aarch64-linux-gnu- menuconfig
make CROSS_COMPILE=aarch64-linux-gnu- -j 1
./tools/mkimage -n rk3588 -T rksd -d ../rkbin/bin/rk35/rk3588_ddr_lp4_2112MHz_lp5_2736MHz_v1.08.bin:./spl/u-boot-spl.bin idbloader.img
#/home/shimmy/Software/Source-Code-Builds/rkbin/tools/loaderimage --pack --uboot u-boot.bin uboot.img
