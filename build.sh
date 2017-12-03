#!/bin/bash
PROJECT="dirtyjtag"
PLATFORM="bluepill"
IMAGE="$PROJECT-$PLATFORM"
WORKDIR="$PWD/bin/$PLATFORM"
docker build -t $IMAGE -f Dockerfile.$PLATFORM .
docker run -v $WORKDIR:/mnt $PROJECT bash -c "cp -v /home/$PROJECT/code/src/*.bin /mnt"

PLATFORM="stlinkv2"
WORKDIR="$PWD/bin/$PLATFORM"
docker build --no-cache -t $IMAGE -f Dockerfile.$PLATFORM .
docker run -v $WORKDIR:/mnt $PROJECT bash -c "cp -v /home/$PROJECT/code/src/*.bin /mnt"
