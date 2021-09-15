# a tiny compile is without Neogeo games
COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="driver_11beat, driver_arbalest, driver_atehate, driver_blandia, driver_blandiap, driver_blockcar, driver_cairblad, driver_calibr50, driver_daioh, driver_downtown, driver_downtowp, driver_drgnunit, driver_drifto94, driver_dynagear, driver_eaglshot, driver_eaglshta, driver_eightfrc, driver_extdwnhl, driver_gdfs, driver_grdians, driver_gundamex, driver_gundhara, driver_hanaawas, driver_hypreac2, driver_hypreact, driver_janjans1, driver_jjsquawk, driver_jsk, driver_kamenrid, driver_keithlcy, driver_kiwame, driver_koikois2, driver_krzybowl, driver_madshark, driver_mayjinsn, driver_mayjisn2, driver_meosism, driver_metafox, driver_mj4simai, driver_mjyuugi, driver_mjyuugia, driver_msgunda1, driver_msgundam, driver_mslider, driver_mtetrisc, driver_myangel, driver_myangel2, driver_neobattl, driver_nratechu, driver_oisipuzl, driver_pairlove, driver_penbros, driver_ponchin, driver_ponchina, driver_pzlbowl, driver_qzkklgy2, driver_qzkklogy, driver_renju, driver_rezon, driver_rezont, driver_ryorioh, driver_sokonuke, driver_speedatk, driver_speglsha, driver_speglsht, driver_srmp2, driver_srmp3, driver_srmp4, driver_srmp4o, driver_srmp5, driver_srmp6, driver_srmp7, driver_stg, driver_stmblade, driver_survarts, driver_survartu, driver_sxyreac2, driver_sxyreact, driver_thunderl, driver_tndrcade, driver_tndrcadj, driver_triplfun, driver_twineag2, driver_twineagl, driver_ultrax, driver_umanclub, driver_usclssic, driver_utoukond, driver_vasara, driver_vasara2, driver_vasara2a, driver_wiggie, driver_wits, driver_wrofaero, driver_zingzip, driver_zombraid"

COREDEFS += -DTINY_POINTER="&driver_11beat, &driver_arbalest, &driver_atehate, &driver_blandia, &driver_blandiap, &driver_blockcar, &driver_cairblad, &driver_calibr50, &driver_daioh, &driver_downtown, &driver_downtowp, &driver_drgnunit, &driver_drifto94, &driver_dynagear, &driver_eaglshot, &driver_eaglshta, &driver_eightfrc, &driver_extdwnhl, &driver_gdfs, &driver_grdians, &driver_gundamex, &driver_gundhara, &driver_hanaawas, &driver_hypreac2, &driver_hypreact, &driver_janjans1, &driver_jjsquawk, &driver_jsk, &driver_kamenrid, &driver_keithlcy, &driver_kiwame, &driver_koikois2, &driver_krzybowl, &driver_madshark, &driver_mayjinsn, &driver_mayjisn2, &driver_meosism, &driver_metafox, &driver_mj4simai, &driver_mjyuugi, &driver_mjyuugia, &driver_msgunda1, &driver_msgundam, &driver_mslider, &driver_mtetrisc, &driver_myangel, &driver_myangel2, &driver_neobattl, &driver_nratechu, &driver_oisipuzl, &driver_pairlove, &driver_penbros, &driver_ponchin, &driver_ponchina, &driver_pzlbowl, &driver_qzkklgy2, &driver_qzkklogy, &driver_renju, &driver_rezon, &driver_rezont, &driver_ryorioh, &driver_sokonuke, &driver_speedatk, &driver_speglsha, &driver_speglsht, &driver_srmp2, &driver_srmp3, &driver_srmp4, &driver_srmp4o, &driver_srmp5, &driver_srmp6, &driver_srmp7, &driver_stg, &driver_stmblade, &driver_survarts, &driver_survartu, &driver_sxyreac2, &driver_sxyreact, &driver_thunderl, &driver_tndrcade, &driver_tndrcadj, &driver_triplfun, &driver_twineag2, &driver_twineagl, &driver_ultrax, &driver_umanclub, &driver_usclssic, &driver_utoukond, &driver_vasara, &driver_vasara2, &driver_vasara2a, &driver_wiggie, &driver_wits, &driver_wrofaero, &driver_zingzip, &driver_zombraid"

# cpuintrf.c cpuintrf.h cheat.c audit.c memory.c info.c sndintrf.c sndintrf.h usrintrf.c psp/fronthlp.c

# uses these CPUs
CPUS+=Z80@
CPUS+=M68000@
CPUS+=M65C02@
CPUS+=V60@
CPUS+=V810@
CPUS+=M68000@

# uses these SOUNDs
SOUNDS+=YM2203@
SOUNDS+=YM3812@
SOUNDS+=X1_010@
SOUNDS+=YM3438@
SOUNDS+=OKIM6295@
SOUNDS+=ES5506@
SOUNDS+=MSM5205@
SOUNDS+=ST0016@



DRVLIBS = \
	obj/drivers/pacman.a obj/drivers/epos.a obj/drivers/nichibut.a \
	obj/drivers/phoenix.a obj/drivers/namco.a obj/drivers/univers.a obj/drivers/nintendo.a \
	obj/drivers/midw8080.a obj/drivers/meadows.a obj/drivers/cvs.a obj/drivers/midway.a \
	obj/drivers/irem.a obj/drivers/gottlieb.a obj/drivers/taito.a obj/drivers/toaplan.a obj/drivers/cave.a \
	obj/drivers/kyugo.a obj/drivers/williams.a obj/drivers/gremlin.a obj/drivers/vicdual.a \
	obj/drivers/capcom.a obj/drivers/itech.a obj/drivers/leland.a obj/drivers/sega.a \
	obj/drivers/dataeast.a obj/drivers/tehkan.a obj/drivers/konami.a \
	obj/drivers/exidy.a obj/drivers/atari.a obj/drivers/snk.a obj/drivers/alpha.a obj/drivers/technos.a \
	obj/drivers/stern.a obj/drivers/gameplan.a obj/drivers/zaccaria.a \
	obj/drivers/upl.a obj/drivers/nmk.a obj/drivers/cinemar.a obj/drivers/cinemav.a \
	obj/drivers/thepit.a obj/drivers/valadon.a obj/drivers/seibu.a obj/drivers/tad.a obj/drivers/jaleco.a \
	obj/drivers/vsystem.a obj/drivers/psikyo.a obj/drivers/orca.a obj/drivers/gaelco.a \
	obj/drivers/kaneko.a obj/drivers/seta.a obj/drivers/atlus.a \
	obj/drivers/sun.a obj/drivers/suna.a obj/drivers/dooyong.a obj/drivers/tong.a \
	obj/drivers/comad.a obj/drivers/playmark.a obj/drivers/pacific.a obj/drivers/tecfri.a \
	obj/drivers/metro.a obj/drivers/venture.a obj/drivers/yunsung.a obj/drivers/zilec.a \
	obj/drivers/fuuki.a obj/drivers/unico.a obj/drivers/dynax.a \
	obj/drivers/sigma.a obj/drivers/igs.a obj/drivers/ramtek.a obj/drivers/omori.a obj/drivers/tch.a \
	obj/drivers/usgames.a obj/drivers/sanritsu.a obj/drivers/rare.a \
	obj/drivers/alba.a obj/drivers/homedata.a obj/drivers/artmagic.a \
	obj/drivers/taiyo.a obj/drivers/edevices.a obj/drivers/other.a obj/drivers/excelent.a obj/drivers/nix.a \
	obj/drivers/arcadia.a obj/drivers/othertrv.a obj/drivers/other68k.a obj/drivers/otherz80.a \
	obj/drivers/neogeo.a $(OBJ)/shared.a \

$(OBJ)/shared.a: \
	$(OBJ)/machine/segacrpt.o \
	$(OBJ)/machine/tmp68301.o \
	$(OBJ)/machine/6532riot.o \
	$(OBJ)/machine/z80fmly.o \
	$(OBJ)/machine/6821pia.o \
	$(OBJ)/machine/8255ppi.o \
	$(OBJ)/machine/6522via.o \
	$(OBJ)/machine/7474.o \
	$(OBJ)/machine/idectrl.o \
	$(OBJ)/machine/ticket.o \
	$(OBJ)/machine/random.o \
	$(OBJ)/machine/adc083x.o \
	$(OBJ)/vidhrdw/crtc6845.o \
	$(OBJ)/vidhrdw/avgdvg.o \
	$(OBJ)/vidhrdw/poly.o \
	$(OBJ)/vidhrdw/tlc34076.o \
	$(OBJ)/vidhrdw/res_net.o \

# MAME specific core objs
COREOBJS += $(OBJ)/tiny.o $(OBJ)/cheat.o
