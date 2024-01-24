make-emu:
	$(MAKE) -C emulator

make-as:
	$(MAKE) -C assembler

run-emu:
	$(MAKE) -C emulator run

run-as:
	$(MAKE) -C assembler run

install-emu:
	$(MAKE) -C emulator install

install-as:
	$(MAKE) -C assembler install

install-all: install-emu install-as

clean:
	$(MAKE) -C emulator clean
	$(MAKE) -C assembler clean

CFILES = $(shell find . -name "*.c")
HFILES = $(shell find . -name "*.h")
format:
	clang-format -i $(CFILES) $(HFILES)

.PHONY: make-emu make-as run-emu run-as install-emu install-as install-all clean
