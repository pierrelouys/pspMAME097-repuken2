PSP_EBOOT_TITLE = pspMame

#OPT_DEFS += -DLINK_ROT=1

# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_simpsons, driver_simps4pa, driver_simpsn2p, driver_simps2pa, driver_simps2pj"

COREDEFS += -DTINY_POINTER="&driver_simpsons, &driver_simps4pa, &driver_simpsn2p, &driver_simps2pa, &driver_simps2pj"

# uses these CPUs
CPUS+=M6809_ORIG
CPUS+=Z80_ORIG
CPUS+=HD63701_ORIG
CPUS+=M6803

CPUS+=KONAMI_ORIG

# uses these SOUNDs
SOUNDS+=YM2610_ORIG
SOUNDS+=YM2203_ORIG
SOUNDS+=YM2612_ORIG
SOUNDS+=YM2610B_ORIG

SOUNDS+=YM2151_ORIG
SOUNDS+=K053260_ORIG


OBJS =	$(OBJ)/drivers/simpsons.o $(OBJ)/machine/simpsons.o $(OBJ)/vidhrdw/simpsons.o \
    	$(OBJ)/vidhrdw/konamiic.o \
        

#$(OBJ)/machine/ticket.o \ SHOULD LEAVE        
		
#$(OBJ)/vidhrdw/taito_f3.o $(OBJ)/sndhrdw/taito_f3.o $(OBJ)/drivers/taito_f3.o \
#DRVLIBS = \
#	$(OBJ)/konami.a \
	
#$(OBJ)/konami.a: \
#$(OBJ)/vidhrdw/gberet.o $(OBJ)/drivers/gberet.o \

# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
