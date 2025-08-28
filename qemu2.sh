#!/bin/sh
set -e
. ./build.sh

/cygdrive/e/Development/Tools/qemu/qemu-system-$(./target-triplet-to-arch.sh $HOST) -kernel kernel/chios.kernel