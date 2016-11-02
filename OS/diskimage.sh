#!/bin/bash
fdisk -lu fishos.img        
sudo losetup -o 1048576 /dev/loop1 fishos.img
sudo mkdosfs -F32 -f 2 /dev/loop1
sudo mount /dev/loop1 /mnt
sudo cp kernel.bin /mnt
sudo umount /mnt/
#grub --device-map=/dev/null << EOF
#device (hd0) fishos.img 
#root (hd0,0)
#setup (hd0)
#quit
#EOF
sudo losetup -d /dev/loop1

