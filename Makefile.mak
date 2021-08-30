PSP_LARGE_MEMORY = 1
#BUILD_PRX = 1

ifeq ($(TARGET),)
TARGET = armedf
endif

# customize option
OPT_DEFS = -D$(TARGET)=1

#------------------------------------------------------------------------------
# Configurations
#------------------------------------------------------------------------------

OSD = psp

TARGETOS = psp

OPT_DEFS += -D$(OSD)=1

NAME = $(TARGET)$(TARGETOS)

# build the targets in different object dirs, since mess changes
# some structures and thus they can't be linked against each other.

SRC = src
OBJ = obj/$(NAME)

OBJ_CPU     = $(OBJ)/cpu
OBJ_AUDIO   = $(OBJ)/audio
OBJ_SOUND   = $(OBJ)/sound
OBJ_DRIVERS = $(OBJ)/drivers
OBJ_MACHINE = $(OBJ)/machine
OBJ_SNDHRDW = $(OBJ)/sndhrdw
OBJ_VIDEO   = $(OBJ)/vidhrdw


#------------------------------------------------------------------------------
# Compiler Defines
#------------------------------------------------------------------------------

CDEFS = \
	-DLSB_FIRST \
	-DINLINE="static __inline__" \
	-DPI=M_PI 

#------------------------------------------------------------------------------
# Compiler Flags
#------------------------------------------------------------------------------

ifdef PSPSDK_HEADER_FIX_WARNING
CFLAGS = -Wstrict-prototypes
else
CFLAGS = 
endif

CFLAGS += \
	-fomit-frame-pointer \
	-fno-strict-aliasing \
	-Wno-sign-compare \
	-Wunused \
	-Wpointer-arith \
	-Wundef \
	-Wformat \
	-Wwrite-strings \
	-Wdisabled-optimization \
	-Wbad-function-cast

#------------------------------------------------------------------------------
# File include path
#------------------------------------------------------------------------------

INCDIR = \
	$(SRC) \
	$(SRC)/includes \
	$(SRC)/debug \
	$(SRC)/$(OSD) \
	$(SRC)/zlib

#------------------------------------------------------------------------------
# Linker Flags
#------------------------------------------------------------------------------

LIBDIR =
LDFLAGS      = -lpspgum_vfpu -lpspvfpu

#------------------------------------------------------------------------------
# Library
#------------------------------------------------------------------------------

ZLIB = $(OBJ)/libz.a

#------------------------------------------------------------------------------
# Include Make Files
#------------------------------------------------------------------------------

CPUOBJS   = 
SOUNDOBJS = $(OBJ)/sndintrf.o $(OBJ_SOUND)/streams.o $(OBJ)/sound/flt_vol.o $(OBJ)/sound/flt_rc.o \

# include the various .mak files
include ./makes/$(TARGET).mak
include $(SRC)/_cpu.mak
include $(SRC)/_sound.mak
include $(SRC)/_emu.mak
include $(SRC)/$(OSD)/_$(OSD).mak

#------------------------------------------------------------------------------
# Object Directory
#------------------------------------------------------------------------------

OBJDIRS += \
	obj \
	$(OBJ) \
	$(OBJ)/$(OSD) \
	$(OBJ_CPU) \
	$(OBJ_AUDIO) \
	$(OBJ_SOUND) \
	$(OBJ_DRIVERS) \
	$(OBJ_MACHINE) \
	$(OBJ_VIDEO) \
    $(OBJ_SNDHRDW) \
	$(OBJ)/etc \
	$(OBJ)/zlib


CDEFS += $(CPUDEFS) $(SOUNDDEFS) $(COREDEFS) $(DRVDEFS) $(OPT_DEFS)
OBJS  += $(CPUOBJS) $(SOUNDOBJS) $(COREOBJS) $(DRVLIBS) $(OSOBJS) $(ZLIB)


#------------------------------------------------------------------------------
# Include build.mak for PSPSDK
#------------------------------------------------------------------------------
# 註： ここは  pspsdk の都合上、「後」にインクルード。

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

#------------------------------------------------------------------------------
# Make Rules
#------------------------------------------------------------------------------

$(OBJ)/libz.a: \
	$(OBJ)/zlib/adler32.o \
	$(OBJ)/zlib/crc32.o \
	$(OBJ)/zlib/inflate.o \
	$(OBJ)/zlib/inftrees.o \
	$(OBJ)/zlib/inffast.o \
	$(OBJ)/zlib/zutil.o

# zlib only sized optimized (-O1)
$(OBJ)/zlib/%.o: $(SRC)/zlib/%.c
	@echo Compiling Zlib $<...
	@$(CC) $(CDEFS) -O1 $(CFLAGS) -c $< -o $@

# speed optimized (-O3)
$(OBJ)/%.o: $(SRC)/%.c
	@echo Compiling $<...
	@$(CC) $(CDEFS) -O3 $(CFLAGS) -c $< -o $@

$(OBJ)/%.a:
	@echo Archiving $@...
	@$(AR) -r $@ $^

pspclean:
	@echo Remove all object files and directories.
	@$(RM) -f -rd $(OBJ)

maked_directry:
	@echo Making object tree for $(TARGET) ...
	@$(MD) -p $(subst //,\,$(sort $(OBJDIRS)))
	@$(RM) -f PARAM.SFO

delpbp:
	@echo Delete all pbp tree ...
	@$(RM) -f -rd pbp

DELTREE = @$(RM) -f -rd obj

rr:
	@echo Remove all temporaly files.
	@$(RM) -f PARAM.SFO
	@$(RM) -f *.elf
	@$(DELTREE)

lse:
	ls -al *.PBP

