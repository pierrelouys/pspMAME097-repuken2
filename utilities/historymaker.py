# -*- coding: utf-8 -*-
import re
import csv
import os
import time

# # # use me with the history.dat from https://www.arcade-history.com/ (old format)

build_name = "capcom"
        
hitlist = "driver_pc_1942,driver_1942,driver_1942a,driver_1942b,driver_1943kai,driver_1943j,driver_1943,driver_hatena,driver_lwingsjp,driver_avengers,driver_avenger2,driver_bionicc,driver_bionicc2,driver_blkdrgon,driver_blktiger,driver_blockj,driver_blocka,driver_block,driver_bbros,driver_cbasebal,driver_cworld,driver_pc_rrngr,driver_commandu,driver_commando,driver_dokaben,driver_exedexes,driver_f1dream,driver_gngt,driver_gng,driver_gnga,driver_mt_gng,driver_gunsmokj,driver_gunsmoku,driver_gunsmoka,driver_gunsmoke,driver_buraiken,driver_lastduel,driver_lstduela,driver_ledstorm,driver_lwings,driver_lwings2,driver_madgearj,driver_madgear,driver_makaimuc,driver_makaimug,driver_makaimur,driver_pc_mman3,driver_higemaru,driver_qsangoku,driver_qtono1,driver_rushcrsh,driver_savgbees,driver_sectionz,driver_sctionza,driver_commandj,driver_sidearjp,driver_sidearmr,driver_sidearms,driver_slipstrm,driver_sonson,driver_sonsonj,driver_sinvasn,driver_sfjp,driver_sfp,driver_sfus,driver_sf,driver_sftm110,driver_sftm111,driver_sftm,driver_sftmj,driver_trojanj,driver_srumbler,driver_srumblr2,driver_tigeroad,driver_topsecrt,driver_toramich,driver_pc_trjan,driver_trojanr,driver_trojan,driver_vulgusj,driver_vulgus,driver_vulgus2,driver_pc_ynoid"

abspath = os.path.abspath(__file__)
os.chdir(os.path.dirname(abspath))

output = ""
copied_drivers = []
    
hitlist_purified = hitlist.replace("driver_", "").replace(" ", "").split(",")

history = open("history.dat", "r", encoding='ansi')

regex = r'\$info=([^\n]+)([^]+?)\$end'
allmatches = re.findall(regex, history.read() )

for match in allmatches:
    drivers = (match[0])
    entry = (match[1])
    for hit in hitlist_purified:
        if hit in drivers:
            if drivers in copied_drivers:
                continue
            output = output + r'$info=' + drivers + entry + r'$end' + "\n\n"
            copied_drivers.append(drivers)

print(output)

outputdat = open(build_name + "-history.dat", "w", encoding="ansi")
outputdat.write(output)
outputdat.close
