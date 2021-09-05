#OPT_DEFS += -DLINK_ROT=360

# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_armedf\
    ,driver_momoko\
    ,driver_baddudes\
    ,driver_avspirit\
    ,driver_citycon\
    ,driver_ssriders\
    ,driver_blandia\
    ,driver_simpsons, driver_simps4pa, driver_simpsn2p, driver_simps2pa, driver_simps2pj"


COREDEFS += -DTINY_POINTER="&driver_armedf\
    ,&driver_momoko\
    ,&driver_baddudes\
    ,&driver_avspirit\
    ,&driver_citycon\
    ,&driver_ssriders\
    ,&driver_blandia\
    ,&driver_simpsons, &driver_simps4pa, &driver_simpsn2p, &driver_simps2pa, &driver_simps2pj"

# uses these CPUs
CPUS+=M6809@
CPUS+=Z80@
CPUS+=HD63701@
CPUS+=M6803@

CPUS+=M68000@
CPUS+=M6502@
CPUS+=H6280@
CPUS+=M6809E@
CPUS+=KONAMI@

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
SOUNDS+=K053260@
SOUNDS+=SAMPLES@
SOUNDS+=K007232@
SOUNDS+=K054539@
SOUNDS+=UPD7759@
SOUNDS+=X1_010@


DRVLIBS = \
    $(OBJ)/drivers/simpsons.o $(OBJ)/machine/simpsons.o $(OBJ)/vidhrdw/simpsons.o \
    	$(OBJ)/vidhrdw/konamiic.o \
	$(OBJ)/vidhrdw/seta.o $(OBJ)/drivers/seta.o \
	$(OBJ)/vidhrdw/megasys1.o $(OBJ)/drivers/megasys1.o \
	$(OBJ)/vidhrdw/armedf.o $(OBJ)/drivers/armedf.o \
	$(OBJ)/vidhrdw/momoko.o $(OBJ)/drivers/momoko.o \
		$(OBJ)/vidhrdw/lkage.o \
	$(OBJ)/machine/dec0.o $(OBJ)/vidhrdw/dec0.o $(OBJ)/drivers/dec0.o \
	$(OBJ)/vidhrdw/citycon.o $(OBJ)/drivers/citycon.o \
	$(OBJ)/vidhrdw/tmnt.o $(OBJ)/drivers/tmnt.o \
    	$(OBJ)/vidhrdw/konamiic.o \


# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
