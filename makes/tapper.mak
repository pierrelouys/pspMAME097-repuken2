PSP_EBOOT_TITLE = pspMame

#OPT_DEFS += -DLINK_ROT=1

# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_tapper"

COREDEFS += -DTINY_POINTER="&driver_tapper"

# uses these CPUs
CPUS+=M6809_ORIG
CPUS+=Z80_ORIG
CPUS+=HD63701_ORIG
CPUS+=M6803

CPUS+=M68000_ORIG
CPUS+=M6502_ORIG
CPUS+=H6280
CPUS+=M6808_ORIG

# uses these SOUNDs
SOUNDS+=YM2610_ORIG
SOUNDS+=YM2203_ORIG
SOUNDS+=YM2612_ORIG
SOUNDS+=YM2610B_ORIG

SOUNDS+=YM3812_ORIG
SOUNDS+=DAC
SOUNDS+=OKIM6295
SOUNDS+=YM2151_ORIG
SOUNDS+=AY8910
SOUNDS+=DAC
SOUNDS+=TMS5220
SOUNDS+=YM2151_ORIG
SOUNDS+=HC55516
SOUNDS+=OKIM6295


OBJS =	$(OBJ)/machine/mcr.o $(OBJ)/sndhrdw/mcr.o \
	$(OBJ)/machine/6821pia.o \
	$(OBJ)/machine/z80fmly.o \
    $(OBJ)/drivers/mcr3.o $(OBJ)/vidhrdw/mcr3.o \
	$(OBJ)/machine/williams.o $(OBJ)/vidhrdw/williams.o $(OBJ)/sndhrdw/williams.o $(OBJ)/drivers/williams.o \
	$(OBJ)/machine/ticket.o \
	$(OBJ)/vidhrdw/mcr68.o $(OBJ)/drivers/mcr68.o \
	$(OBJ)/vidhrdw/res_net.o \
        

#$(OBJ)/machine/ticket.o \ SHOULD LEAVE        
		
#$(OBJ)/vidhrdw/taito_f3.o $(OBJ)/sndhrdw/taito_f3.o $(OBJ)/drivers/taito_f3.o \
#DRVLIBS = \
#	$(OBJ)/konami.a \
	
#$(OBJ)/konami.a: \
#$(OBJ)/vidhrdw/gberet.o $(OBJ)/drivers/gberet.o \

# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
