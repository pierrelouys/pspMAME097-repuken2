
[https://github.com/pierrelouys/pspMAME097-repuken2/compare/conservative...pierrelouys:main](https://github.com/pierrelouys/pspMAME097-repuken2/compare/conservative...pierrelouys:main)

### ISSUES

- all scaling options other than x1, auto-stretch and rotate are broken;
- files `default.cfg` and `[rom name].cfg` must exist in the `cfg` folder, or the emulator will get stuck while loading a rom (error: bad file descriptor);
- a `sta` folder must exist, or game states won't be saved;

### CHANGES

- settings are now saved in `pspmame.cfg`;
- save states work again (shortcut must be defined in the OSD that appears by pressing L while in the game);
- button configurations saved in a `[rom name].cfg` file (but see above);
- `SHOW FPS` in the settings now works;
- UI now in (proper) English;

