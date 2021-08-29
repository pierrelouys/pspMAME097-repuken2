PSP_EBOOT_TITLE = pspMame

#OPT_DEFS += -DLINK_ROT=1

# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_armedf\
    ,driver_momoko\
    ,driver_baddudes\
    ,driver_avspirit"


COREDEFS += -DTINY_POINTER="&driver_armedf\
    ,&driver_momoko\
    ,&driver_baddudes\
    ,&driver_avspirit"

# uses these CPUs
CPUS+=M6809_ORIG
CPUS+=Z80_ORIG
CPUS+=HD63701_ORIG
CPUS+=M6803

CPUS+=M68000_ORIG
CPUS+=M6502_ORIG
CPUS+=H6280

# uses these SOUNDs
SOUNDS+=YM2610_ORIG
SOUNDS+=YM2203_ORIG
SOUNDS+=YM2612_ORIG
SOUNDS+=YM2610B_ORIG

SOUNDS+=YM3812_ORIG
SOUNDS+=DAC
SOUNDS+=OKIM6295
SOUNDS+=YM2151_ORIG


DRVLIBS = \
	$(OBJ)/vidhrdw/megasys1.o $(OBJ)/drivers/megasys1.o \
	$(OBJ)/vidhrdw/armedf.o $(OBJ)/drivers/armedf.o \
	$(OBJ)/vidhrdw/momoko.o $(OBJ)/drivers/momoko.o \
		$(OBJ)/vidhrdw/lkage.o \
	$(OBJ)/machine/dec0.o $(OBJ)/vidhrdw/dec0.o $(OBJ)/drivers/dec0.o \
        

#$(OBJ)/machine/ticket.o \ SHOULD LEAVE        
		
#$(OBJ)/vidhrdw/taito_f3.o $(OBJ)/sndhrdw/taito_f3.o $(OBJ)/drivers/taito_f3.o \
#DRVLIBS = \
#	$(OBJ)/konami.a \
	
#$(OBJ)/konami.a: \
#$(OBJ)/vidhrdw/gberet.o $(OBJ)/drivers/gberet.o \

# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
