PSP_EBOOT_TITLE = pspMame

#OPT_DEFS += -DLINK_ROT=1

# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_armedf\
    ,driver_momoko\
    ,driver_baddudes\
    ,driver_avspirit\
    ,driver_citycon"


COREDEFS += -DTINY_POINTER="&driver_armedf\
    ,&driver_momoko\
    ,&driver_baddudes\
    ,&driver_avspirit\
    ,&driver_citycon"

# uses these CPUs
CPUS+=M6809@
CPUS+=Z80@
CPUS+=HD63701@
CPUS+=M6803@

CPUS+=M68000@
CPUS+=M6502@
CPUS+=H6280@
CPUS+=M6809E@

# uses these SOUNDs
SOUNDS+=YM2610@
SOUNDS+=YM2203@
SOUNDS+=YM2612@
SOUNDS+=YM2610B@

SOUNDS+=YM3812@
SOUNDS+=DAC@
SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
SOUNDS+=AY8910@


DRVLIBS = \
	$(OBJ)/vidhrdw/megasys1.o $(OBJ)/drivers/megasys1.o \
	$(OBJ)/vidhrdw/armedf.o $(OBJ)/drivers/armedf.o \
	$(OBJ)/vidhrdw/momoko.o $(OBJ)/drivers/momoko.o \
		$(OBJ)/vidhrdw/lkage.o \
	$(OBJ)/machine/dec0.o $(OBJ)/vidhrdw/dec0.o $(OBJ)/drivers/dec0.o \
	$(OBJ)/vidhrdw/citycon.o $(OBJ)/drivers/citycon.o \


# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
