## Setting up pspMAME

pspMAME is a straight port of MAME 0.97 (released for PC in 2005). 

Out of the commonly available romsets, the ones closest to this version are AdvanceMAME (0.106) and MAME 2003 (0.78).

### .dat file

A .dat file is an XML-formatted file containing the list of all the ROMS each version of MAME is compatible with.

![Screenshot](https://raw.githubusercontent.com/pierrelouys/pspMAME097-repuken2/conservative/docs/psp-guide/datfile-view.webp)

**[Link to the MAME 0.97 .dat file](https://github.com/pierrelouys/pspMAME097-repuken2/releases/download/0.0.2/MAME.0.97.datfile.zip).**

You can use this file with [RomCenter](https://www.romcenter.com/) (or any other ROM-managing app of your choice).

### RomCenter

Unpack the .dat file and drag it onto the RomCenter window. Select `Create the database`. Once the database has been created, you will be greeted by a list of the 5,600+ games supported by MAME 0.97 (left), and the individual ROMs required by the selected game (right).

![Screenshot](https://raw.githubusercontent.com/pierrelouys/pspMAME097-repuken2/conservative/docs/psp-guide/RomCenter-overview.webp)

Drag the folder with your ROMs on the RomCenter window. The games that appear in green are good. The ones in yellow require a fix (they might have the wrong file name, for instance), and the ones in red are just not compatible with MAME 0.97 (they might be intended for earlier/later versions of MAME).

![Screenshot](https://raw.githubusercontent.com/pierrelouys/pspMAME097-repuken2/conservative/docs/psp-guide/Romcenter-rom-view.webp)

### Copy to your PSP

Once you are done with RomCenter, simply copy pspMAME and the green ROMs to your memory stick:

`[unit letter]:\PSP\GAME\pspMAME\roms`

### History files

Just like the PC version of MAME 0.97, pspMAME supports `history.dat` files. These are text files including historical details and trivia for each of the games. 

![Screenshot](https://raw.githubusercontent.com/pierrelouys/pspMAME097-repuken2/conservative/docs/psp-guide/altbeast-history.webp)

The history.dat file available from [arcade-history.com](https://www.arcade-history.com/) is too large to be loaded on a PSP. Instead, smaller files are available under `pspMAME\hist` that include only the subset of games included in each of the builds. 

## Common issues

#### Required files are missing

![Screenshot](https://raw.githubusercontent.com/pierrelouys/pspMAME097-repuken2/conservative/docs/psp-guide/error-notfound.webp)

Not just the zip files, but the ROMs inside them, need to match the specifications of the MAME 0.97 .dat file precisely. Drag the offending ROM on RomCenter and see the suggested fixes.

#### Unable to allocate / Out of memory

![Screenshot](https://raw.githubusercontent.com/pierrelouys/pspMAME097-repuken2/conservative/docs/psp-guide/out-of-mem.webp)

![Screenshot](https://raw.githubusercontent.com/pierrelouys/pspMAME097-repuken2/conservative/docs/psp-guide/unable-allocate.webp)

![Screenshot](https://raw.githubusercontent.com/pierrelouys/pspMAME097-repuken2/conservative/docs/psp-guide/unable-allocate-2.webp)

pspMAME does not implement any form of caching. Because of that, you will run out of memory pretty quickly even on a PSP slim when trying to load larger ROMs. 

Similar messages might also appear for smaller ROMs in case of memory leaks. In this case, try exiting and reloading pspMAME to free the memory up again.

## Compiling

MAME 0.97 supports 5,661 ROMs. Compile it with that many drivers, and it will crash the moment you try to launch it. Use dedicated makefiles instead.

 1. `make all`
 2. move all the `.a` libraries from `obj/mame` to `obj/drivers`
 3. `make TARGET=Nichibutsu` - varies according to the name of the makefile in `/makes`.

## Changes

- settings are now saved in `pspmame.cfg`;
- save states work again (shortcut must be defined in the OSD that appears by pressing L while in the game);
- button configurations saved in a `[rom name].cfg` file (but see above);
- `SHOW FPS` in the settings now works;
- `LIMIT SPEED` now works;
- UI now in (proper) English;
- there is finally an option to quit in the OSD menu;
- Lua launcher borrowed from Brujito's Multi Emulator;

### Issues

- all scaling options other than x1, auto-stretch and rotate are broken;
- [SOLVED] files `default.cfg` and `[rom name].cfg` must exist in the `cfg` folder, or the emulator will get stuck while loading a rom (error: bad file descriptor);
- [SOLVED] a `sta` folder must exist, or game states won't be saved;
- [SOLVED] nvram files are not created (they still work if copied into the `nvram` folder);

## Credits

- [TMK](http://tmk2000.blog41.fc2.com/): Japanese code who first ported MAME 0.97 to PSP back in 2005.
- [PSP Development Kindergarten](https://ameblo.jp/pspdevblog/) (ＰＳＰ開発幼稚園): the Japanese coders who managed to speed up TMK's port enough to make it playable. This version of pspMAME is based entirely on their work.
- Brujito: the code for the launcher is taken from his [Multi Emulators](https://archive.org/details/multi-emulators-v-2.7z).
