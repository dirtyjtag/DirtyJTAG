#!/bin/sh

git submodule init
git submodule update

make PLATFORM=bluepill
make PLATFORM=stlinkv2
