
MAME 0.97 supports 5,661 ROMs. Compile it with that many drivers, and it will crash the moment you try to launch it. Use dedicated makefiles instead.

### COMPILING

 1. `make all`
 2. move all the `.a` libraries from `obj/mame` to `obj/drivers`
 3. `make TARGET=Nichibutsu` - varies according to the name of the makefile in `/makes`.

### ISSUES

- all scaling options other than x1, auto-stretch and rotate are broken;
- [SOLVED] files `default.cfg` and `[rom name].cfg` must exist in the `cfg` folder, or the emulator will get stuck while loading a rom (error: bad file descriptor);
- [SOLVED] a `sta` folder must exist, or game states won't be saved;
- [SOLVED] nvram files are not created (they still work if copied into the `nvram` folder);

### CHANGES

- settings are now saved in `pspmame.cfg`;
- save states work again (shortcut must be defined in the OSD that appears by pressing L while in the game);
- button configurations saved in a `[rom name].cfg` file (but see above);
- `SHOW FPS` in the settings now works;
- `LIMIT SPEED` now works;
- UI now in (proper) English;
- there is finally an option to quit in the OSD menu;
- Lua launcher borrowed from Brujito's Multi Emulator;
