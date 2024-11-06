FROM alpine:3.14 as build-stage
MAINTAINER Benjamin Henrion <zoobab@gmail.com>
LABEL Description="DirtyJTAG firmware for STM32 Bluepill board" 

RUN apk add --no-cache make python3 gcc-arm-none-eabi newlib-arm-none-eabi
RUN ln -sf python3 /usr/bin/python

ADD . /dirtyjtag
WORKDIR /dirtyjtag

RUN make PLATFORM=bluepill
RUN make PLATFORM=stlinkv2
RUN make PLATFORM=stlinkv2dfu
RUN make PLATFORM=baite
RUN make PLATFORM=olimexstm32h103
RUN make PLATFORM=stlinkv2white

# Run the automated JTAG test script
RUN python3 scripts/automated_jtag_test.py

FROM scratch AS export-stage
COPY --from=build-stage /dirtyjtag/src/dirtyjtag.*.bin /
COPY --from=build-stage /dirtyjtag/src/dirtyjtag.*.elf /
