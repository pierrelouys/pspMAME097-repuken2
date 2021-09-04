#OPT_DEFS += -DLINK_ROT=1

# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_simpsons, driver_simps4pa, driver_simpsn2p, driver_simps2pa, driver_simps2pj"

COREDEFS += -DTINY_POINTER="&driver_simpsons, &driver_simps4pa, &driver_simpsn2p, &driver_simps2pa, &driver_simps2pj"

# uses these CPUs
CPUS+=Z80@
CPUS+=KONAMI@

# uses these SOUNDs
SOUNDS+=YM2151@
SOUNDS+=K053260@


OBJS =	$(OBJ)/drivers/simpsons.o $(OBJ)/machine/simpsons.o $(OBJ)/vidhrdw/simpsons.o \
    	$(OBJ)/vidhrdw/konamiic.o \


# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
