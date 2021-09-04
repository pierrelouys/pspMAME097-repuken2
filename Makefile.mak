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
	$(SRC)/zlib \
	$(SRC)/expat

#------------------------------------------------------------------------------
# Linker Flags
#------------------------------------------------------------------------------

LIBDIR =
LDFLAGS      = -lpspgum_vfpu -lpspvfpu

#------------------------------------------------------------------------------
# Library
#------------------------------------------------------------------------------

ZLIB = $(OBJ)/libz.a
EXPAT = $(OBJ)/libexpat.a

#------------------------------------------------------------------------------
# Include Make Files
#------------------------------------------------------------------------------

CPUOBJS   = 
SOUNDOBJS = $(OBJ)/sndintrf.o $(OBJ_SOUND)/streams.o $(OBJ)/sound/flt_vol.o $(OBJ)/sound/flt_rc.o \

# include the various .mak files
include ./makes/$(TARGET).mak
include $(SRC)/rules.mak
include $(SRC)/$(OSD)_sound/_psp_sound.mak

# only PSP specific output files and rules
OSOBJS = \
	$(OBJ)/psp/psp.o \
	$(OBJ)/psp/osd_cycles.o \
	$(OBJ)/psp/video.o \
	$(OBJ)/psp/psp_font.o \
	$(OBJ)/psp/sound.o \
	$(OBJ)/psp/input.o \
	$(OBJ)/psp/ticker.o \
	$(OBJ)/psp/config.o \
	$(OBJ)/psp/fileio.o \
	$(OBJ)/psp/pspmain.o \
	$(OBJ)/psp/psp_video.o \
	$(OBJ)/psp/menu.o \
	$(OBJ)/psp/psp_data.o \

# the core object files (without target specific objects;
# those are added in the target.mak files)
COREOBJS += \
	$(OBJ)/mame.o \

## [tilemap]標準TILEMAP、使わないなら１(自動設定)。(基本は１)
ifneq ($(filter NO_TILEMAP,$(PSP_EXTENSION_OPTS)),)
	OPT_DEFS += -DLINK_TILEMAP=0
else
	OPT_DEFS += -DLINK_TILEMAP=1
	COREOBJS += $(OBJ)/tilemap.o 
endif

COREOBJS += \
	$(OBJ)/version.o \
    $(OBJ)/drawgfx.o \
	$(OBJ)/common.o \
	$(OBJ)/usrintrf.o \
	$(OBJ)/ui_text.o \
	$(OBJ)/cpuintrf.o \
	$(OBJ)/cpuexec.o \
	$(OBJ)/cpuint.o \
	$(OBJ)/memory.o \
	$(OBJ)/timer.o \
	$(OBJ)/palette.o \
	$(OBJ)/input.o \
	$(OBJ)/inptport.o \
	$(OBJ)/config.o \
	$(OBJ_VIDEO)/generic.o \
	$(OBJ)/vidhrdw/vector.o \
	$(OBJ)/machine/eeprom.o \
	$(OBJ)/profiler.o \
	$(OBJ)/unzip.o \
	$(OBJ)/audit.o \
	$(OBJ)/fileio.o \
	$(OBJ)/state.o \
	$(OBJ)/datafile.o \
    $(OBJ)/hiscore.o \
	$(OBJ)/png.o \
	$(OBJ)/hash.o \
	$(OBJ)/sha1.o \
	$(OBJ)/chd.o \
	$(OBJ)/md5.o \
	$(OBJ)/sound/wavwrite.o \
	$(OBJ)/harddisk.o	

## [state]ダミー（互換性向上の為）
ifneq ($(filter STATE,$(PSP_EXTENSION_OPTS)),)
	OPT_DEFS += -DLINK_STATE=1
	COREOBJS += $(OBJ)/state.o 
else
	OPT_DEFS += -DLINK_STATE=0
endif

## [common eeprom]標準EEP-ROM
ifneq ($(filter EEPROM,$(PSP_EXTENSION_OPTS)),)
	OPT_DEFS += -DLINK_EEPROM=1
	OPT_DEFS += -DLINK_NVRAM=1
	COREOBJS += $(OBJ)/machine/eeprom.o 
else
	OPT_DEFS += -DLINK_EEPROM=0
endif

## [cheat]ダミー（互換性向上の為）
OPT_DEFS += -DLINK_CHEAT=0

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

ifeq ($(PSPSDK),)
MD = -mkdir
RM = -rm
else
MD = -mkdir.exe
RM = -rm.exe
endif

#------------------------------------------------------------------------------
# PSPSDK settings
#------------------------------------------------------------------------------

## [専用 icon指定]の場合
ifneq ($(filter ICON,$(PSP_EXTENSION_OPTS)),)
	PSP_EBOOT_ICON = icon/$(TARGET).png
endif

## タイトルが無い場合に設定
ifeq ($(PSP_EBOOT_TITLE),)
PSP_EBOOT_TITLE = "MAME 0.97 $(TARGET)"
endif

## 配布ソース(zip size)が大きくなりすぎるので、暫定的にアイコン共通
PSP_EBOOT_ICON = ICON0.PNG

EXTRA_TARGETS = maked_directry EBOOT.PBP # copy_bak_pbp
EXTRA_CLEAN = pspclean

#------------------------------------------------------------------------------
# Configurations
#------------------------------------------------------------------------------

#SPRITE_OLD = 1

#↓カーネルモードにしたいなら#を外してコンパイルしてね。
#KERNEL_MODE = 1


ifdef KERNEL_MODE
CDEFS += -DKERNEL_MODE=1
endif

#------------------------------------------------------------------------------
# Library
#------------------------------------------------------------------------------

USE_PSPSDK_LIBC = 1

LIBS = -lm -lc -lpspaudio -lpspgu -lpsppower -lpsprtc

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
	$(OBJ)/zlib \
	$(OBJ)/expat


CDEFS += $(CPUDEFS) $(SOUNDDEFS) $(COREDEFS) $(DRVDEFS) $(OPT_DEFS)
OBJS  += $(CPUOBJS) $(SOUNDOBJS) $(COREOBJS) $(DRVLIBS) $(OSOBJS) $(ZLIB) $(EXPAT)

#------------------------------------------------------------------------------
# Include build.mak for PSPSDK
#------------------------------------------------------------------------------
# 註： ここは  pspsdk の都合上、「後」にインクルード。

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

#------------------------------------------------------------------------------
# Make Rules
#------------------------------------------------------------------------------

$(OBJ)/libexpat.a: $(OBJ)/expat/xmlparse.o $(OBJ)/expat/xmlrole.o $(OBJ)/expat/xmltok.o

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

$(OBJ)/expat/%.o: $(SRC)/expat/%.c
	@echo Compiling Expat $<...
	@$(CC) $(CDEFS) -O1 -c $< -o $@

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

