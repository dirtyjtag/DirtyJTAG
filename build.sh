#!/bin/bash
set -e
PROJECT="dirtyjtag"
docker build -t dirtyjtag -f Dockerfile .
docker run -v $PWD:/mnt dirtyjtag bash -c "cp -v /home/$PROJECT/code/src/*.bin /mnt"
