MAKE := make

all:
	$(MAKE) -f Makefile.stm32f1 PLATFORM=bluepill LOADER=noloader
	$(MAKE) -f Makefile.stm32f1 PLATFORM=baite LOADER=noloader
	$(MAKE) -f Makefile.stm32f1 PLATFORM=olimexstm32h103 LOADER=noloader
	$(MAKE) -f Makefile.stm32f1 PLATFORM=stlinkv2 LOADER=noloader
	$(MAKE) -f Makefile.stm32f1 PLATFORM=stlinkv2white LOADER=noloader
	$(MAKE) -f Makefile.stm32f1 PLATFORM=bluepill LOADER=loader2k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=baite LOADER=loader2k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=olimexstm32h103 LOADER=loader2k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=stlinkv2 LOADER=loader2k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=stlinkv2white LOADER=loader2k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=bluepill LOADER=loader4k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=baite LOADER=loader4k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=olimexstm32h103 LOADER=loader4k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=stlinkv2 LOADER=loader4k
	$(MAKE) -f Makefile.stm32f1 PLATFORM=stlinkv2white LOADER=loader4k

clean:
	$(MAKE) -f Makefile.stm32f1 clean

.PHONY: all clean
