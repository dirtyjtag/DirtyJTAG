#!/bin/bash
PROJECT="dirtyjtag"
docker build -t dirtyjtag -f Dockerfile .
docker run -v /mnt dirtyjtag bash -c "cp -v /home/$PROJECT/code/src/*.bin /mnt"
