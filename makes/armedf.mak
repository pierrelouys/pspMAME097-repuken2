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
CPUS+=M6809@
CPUS+=Z80@
CPUS+=HD63701@
CPUS+=M6803@

CPUS+=M68000@
CPUS+=M6502@
CPUS+=H6280@

# uses these SOUNDs
SOUNDS+=YM2610@
SOUNDS+=YM2203@
SOUNDS+=YM2612@
SOUNDS+=YM2610B@

SOUNDS+=YM3812@
SOUNDS+=DAC@
SOUNDS+=OKIM6295@
SOUNDS+=YM2151@


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
