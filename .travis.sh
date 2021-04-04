#!/bin/sh

set -eu

git submodule init
git submodule update

make
