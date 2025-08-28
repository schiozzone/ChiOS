#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/chios.kernel isodir/boot/chios.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "chios" {
	multiboot /boot/chios.kernel
}
EOF
grub-mkrescue -o chios.iso isodir
