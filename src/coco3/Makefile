#
#  A makefile for gcc 6809
#  

CC = m6809-unknown-gcc
ASM = m6809-unknown-as
AR = m6809-unknown-ar
AS = $(ASM)
LINKER = m6809-unknown-ld
CFLAGS = -fno-builtin -Os -I../ -I./
LINKER_OPT = --oformat=raw -L$(FUZIX_DIR)/Library/libs -lc6809
LIBGCCDIR = $(dir $(shell $(CC) -print-libgcc-file-name))
LINKER_OPT += -L$(LIBGCCDIR) -lgcc
LINKER_OPT += --script=$(TARGET).link
ASM_OPT = -o

CSRC_MAIN = ../protocol.c ../plato.c ../terminal.c
CSRC_PLAT = config.c io.c prefs.c screen.c touch.c keyboard.c
CSRC = $(CSRC_MAIN) $(CSRC_PLAT)
ASRC = crt0.s drivewire.s
AOBJ = $(ASRC:.s=.o)
COBJ = $(CSRC:.c=.o)
OBJ = $(AOBJ) $(COBJ)

all: plato

$(AOBJ): $(ASRC)

$(COBJ): $(CSRC)

plato: $(AOBJ) $(COBJ)
	lwlink -L$(LIBGCCDIR) -lgcc -b -o plato.bin -s basic.link -m plato.map $(OBJ)

plato-disk: plato
	rm -f plato.dsk
	decb dskini plato.dsk
	decb copy -b -2 plato.bin plato.dsk,PLATO.BIN
	decb copy -a -0 -l AUTOEXEC.BAS plato.dsk,AUTOEXEC.BAS

clean:
	rm -f *.o *~ plato.bin plato.dsk plato.map

