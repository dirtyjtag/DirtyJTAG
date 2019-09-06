#!/bin/sh

set -eu

git submodule init
git submodule update

make PLATFORM=bluepill
make PLATFORM=stlinkv2
make PLATFORM=stlinkv2dfu
make PLATFORM=olimexstm32h103
