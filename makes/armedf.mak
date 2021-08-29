PSP_EBOOT_TITLE = pspMame Deathrash Vol 3

#OPT_DEFS += -DLINK_ROT=1

# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_armedf\
    ,driver_momoko"


COREDEFS += -DTINY_POINTER="&driver_armedf\
    ,&driver_momoko"

# uses these CPUs
CPUS+=M6809_ORIG
CPUS+=Z80_ORIG
CPUS+=HD63701_ORIG
CPUS+=M6803

CPUS+=M68000_ORIG

#M68000_ORIG
#CPUS+=TMS34010_ORIG__@NOT

# uses these SOUNDs
SOUNDS+=YM2610_ORIG
SOUNDS+=YM2203_ORIG
SOUNDS+=YM2612_ORIG
SOUNDS+=YM2610B_ORIG

SOUNDS+=YM3812_ORIG
SOUNDS+=DAC


DRVLIBS = \
	$(OBJ)/vidhrdw/armedf.o $(OBJ)/drivers/armedf.o \
	$(OBJ)/vidhrdw/momoko.o $(OBJ)/drivers/momoko.o \
		$(OBJ)/vidhrdw/lkage.o \
        

#$(OBJ)/machine/ticket.o \ SHOULD LEAVE        
		
#$(OBJ)/vidhrdw/taito_f3.o $(OBJ)/sndhrdw/taito_f3.o $(OBJ)/drivers/taito_f3.o \
#DRVLIBS = \
#	$(OBJ)/konami.a \
	
#$(OBJ)/konami.a: \
#$(OBJ)/vidhrdw/gberet.o $(OBJ)/drivers/gberet.o \

# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
