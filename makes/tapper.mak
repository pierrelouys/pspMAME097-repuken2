#OPT_DEFS += -DLINK_ROT=360

# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_tapper"

COREDEFS += -DTINY_POINTER="&driver_tapper"

# uses these CPUs
CPUS+=M6808@
CPUS+=Z80@

# uses these SOUNDs
SOUNDS+=AY8910@
SOUNDS+=DAC@
SOUNDS+=TMS5220@
SOUNDS+=HC55516@
SOUNDS+=YM2151@
SOUNDS+=OKIM6295@


OBJS =	$(OBJ)/machine/mcr.o $(OBJ)/sndhrdw/mcr.o \
	$(OBJ)/machine/6821pia.o \
	$(OBJ)/machine/z80fmly.o \
    $(OBJ)/drivers/mcr3.o $(OBJ)/vidhrdw/mcr3.o \
	$(OBJ)/machine/williams.o $(OBJ)/vidhrdw/williams.o $(OBJ)/sndhrdw/williams.o $(OBJ)/drivers/williams.o \
	$(OBJ)/machine/ticket.o \
	$(OBJ)/vidhrdw/mcr68.o $(OBJ)/drivers/mcr68.o \
	$(OBJ)/vidhrdw/res_net.o \

# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
