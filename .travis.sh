#!/bin/sh

git submodule init
git submodule update

make bin PLATFORM=bluepill
make bin PLATFORM=stlinkv2
