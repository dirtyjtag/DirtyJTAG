FROM debian:jessie
MAINTAINER Benjamin Henrion <zoobab@gmail.com>
LABEL Description="DirtyJTAG firmware for STM32 Bluepill board" 

RUN DEBIAN_FRONTEND=noninteractive apt-get update -y -q
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y -q make gcc-arm-none-eabi binutils sudo python git-core

ENV user dirtyjtag
RUN useradd -d /home/$user -m -s /bin/bash $user
RUN echo "$user ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$user
RUN chmod 0440 /etc/sudoers.d/$user

USER $user
WORKDIR /home/$user
RUN mkdir -pv code
COPY . ./code/
RUN sudo chown $user.$user -R /home/$user/code
WORKDIR /home/$user/code/
RUN git submodule init
RUN git submodule update
RUN make PLATFORM=bluepill
RUN make PLATFORM=stlinkv2