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


