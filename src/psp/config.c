
/* PSP2007 (osd_config_fuantei_070807.c) */

/*
 * Configuration routines.
 *
 */

#include "driver.h"
#include <ctype.h>

#include "rc.h"

extern SETTING setting;
extern int keyorientation;

float get_config_float(const char *section, const char *name, float def);

extern int blitMode;

//============================================================
//	LOCAL VARIABLES
//============================================================

//#if (1==PSP_USE_SYS_LOG)
static FILE *logfile;
static const char *s_errorlog="error.log";
static int b_flusherrorlog=0;
static int b_closeerrorlog=0;
//#endif // (1==PSP_USE_SYS_LOG)

static char *dos_basename(char *filename);
static char *dos_dirname(char *filename);
static char *dos_strip_extension(char *filename);

/* from video.c */
extern /*int*/UINT8 wait_vsync;
extern /*int*/UINT8 limit_max_speed;
extern int throttle;

extern int frameskip,autoframeskip;
extern int gfx_skiplines;
extern int gfx_skipcolumns;
extern int gfx_mode;

#if (1==LINK_BIOS)
static const char *s_bios;
#endif //(1==LINK_BIOS)

//#if (1==PSP_USE_SYS_LOG)
static int maxlogsize;
static int curlogsize;
static int errorlog;
//#endif

static int ignorecfg;

/* from sound.c */
int enable_sound;
extern int soundcard;
extern int usestereo;
extern int sampleratedetect;
extern int attenuation;

/* from datafile.c */
// const char *history_filename,*mameinfo_filename;
const char *history_filename;

char *rompath_extra;

// #define HISTRY_NAME "HISTORY.DAT"

/* from video.c for blit rotation */
extern int video_flipx;
extern int video_flipy;
extern int video_swapxy;

static int mame_argc;
static char **mame_argv;
static int game;
static char srcfile[10];

void set_config_string(const char *section, const char *name, const char *val) { }
void set_config_int(const char *section, const char *name, int val) { }
void set_config_float(const char *section, const char *name, float val) { }
void set_config_id(const char *section, const char *name, int val) { }

/*
 * gets some boolean config value.
 * 0 = false, >0 = true, <0 = auto
 * the shortcut can only be used on the commandline
 */
static int get_bool( const char *section, const char *option, const char *shortcut, int res/*def*/ )
{
	/* also take numerical values instead of "yes", "no" and "auto" */
	if		(/*stricmp(yesnoauto, "no"  ) == 0*/(/*def*/res == 0)) res = 0;
	else if (/*stricmp(yesnoauto, "yes" ) == 0*/(/*def*/res  > 0)) res = 1;
	else /*if (stricmp(yesnoauto, "auto") == 0(// def // res < 0))*/ res = -1;
	/* check the commandline */
	int i;
	for (i = 1; i < mame_argc; i++)
	{
		if( mame_argv[ i ][ 0 ] == '-' )
		{
			/* look for "-option" / "-shortcut" */
			if( stricmp( &mame_argv[ i ][ 1 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 1 ], shortcut ) == 0 ) )
			{
				if( i + 1 < mame_argc )
				{
					if( stricmp( mame_argv[ i + 1 ], "no" ) == 0 )
					{
						i++;
						res = 0;
					}
					else if( stricmp( mame_argv[ i + 1 ], "yes" ) == 0 )
					{
						i++;
						res = 1;
					}
					else if( stricmp( mame_argv[ i + 1 ], "auto" ) == 0 )
					{
						i++;
						res = -1;
					}
					else if( isdigit( mame_argv[ i + 1 ][ 0 ] ) )
					{
						i++;
						res = atoi( mame_argv[ i ] );
					}
					else
					{
						res = 1;
					}
				}
				else
				{
					res = 1;
				}
			}
			else if( strnicmp( &mame_argv[ i ][ 1 ], "no", 2 ) == 0 &&
				( stricmp( &mame_argv[ i ][ 3 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 3 ], shortcut ) == 0 ) ) )
			{
				res = 0;
			}
			else if( strnicmp( &mame_argv[ i ][ 1 ], "auto", 4 ) == 0 &&
				( stricmp( &mame_argv[ i ][ 5 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 5 ], shortcut ) == 0 ) ) )
			{
				res = -1;
			}
		}
	}
	return res;
}

int get_config_int(const char *section, const char *name, int def) {
	return (def);
}


static int get_int( const char *section, const char *option, const char *shortcut, int def )
{
	int res,i;

	res = def;

	if (!ignorecfg)
	{
		/* if the option does not exist, create it */
		if (get_config_int (section, option, -777) == -777)
			set_config_int (section, option, def);

		/* look into mame.cfg, [section] */
		res = get_config_int (section, option, def);

		if( game >= 0 )
		{
			/* look into mame.cfg, [srcfile] */
			res = get_config_int (srcfile, option, res);

			/* look into mame.cfg, [gamename] */
			res = get_config_int ((char *)drivers[game]->name, option, res);
		}
	}

	/* get it from the commandline */
	for (i = 1; i < mame_argc; i++)
	{
		if( mame_argv[ i ][ 0 ] == '-' )
		{
			if( stricmp( &mame_argv[ i ][ 1 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 1 ], shortcut ) == 0 ) )
			{
				if( i + 1 < mame_argc && isdigit( mame_argv[ i + 1 ][ 0 ] ) )
				{
					i++;
					res = atoi( mame_argv[ i ] );
				}
				else
				{
					res = def;
				}
			}
			else if( strnicmp( &mame_argv[ i ][ 1 ], "no", 2 ) == 0 &&
				( stricmp( &mame_argv[ i ][ 3 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 3 ], shortcut ) == 0 ) ) )
			{
				res = 0;
			}
		}
	}
	return res;
}

static float get_float( const char *section, const char *option, const char *shortcut, float def, float lower, float upper )
{
	int i;
	float res;

	res = def;

	if (!ignorecfg)
	{
		/* if the option does not exist, create it */
		if (get_config_float (section, option, 9999.0) == 9999.0)
			set_config_float (section, option, def);

		/* look into mame.cfg, [section] */
		res = get_config_float (section, option, def);

		if( game >= 0 )
		{
			/* look into mame.cfg, [srcfile] */
			res = get_config_float (srcfile, option, res);

			/* look into mame.cfg, [gamename] */
			res = get_config_float ((char *)drivers[game]->name, option, res);
		}
	}

	/* get it from the commandline */
	for (i = 1; i < mame_argc; i++)
	{
		if( mame_argv[ i ][ 0 ] == '-' )
		{
			if( stricmp( &mame_argv[ i ][ 1 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 1 ], shortcut ) == 0 ) )
			{
				if( i + 1 < mame_argc && isdigit( mame_argv[ i + 1 ][ 0 ] ) )
				{
					i++;
					res = atof( mame_argv[ i ] );
				}
				else
				{
					res = upper;
				}
			}
			else if( strnicmp( &mame_argv[ i ][ 1 ], "no", 2 ) == 0 &&
				( stricmp( &mame_argv[ i ][ 3 ], option ) == 0 || 
				( shortcut && stricmp( &mame_argv[ i ][ 3 ], shortcut ) == 0 ) ) )
			{
				res = lower;
			}
		}
	}
	if( res < lower )
	{
		return lower;
	}
	else if( res > upper )
	{
		return upper;
	}
	return res;
}


static const char *get_string( const char *section, const char *option, const char *shortcut, const char *def, const char *no, const char *yes, const char *s_auto )
{
	const char *res;
	int i;

	res = def;

	/* get it from the commandline */
	for (i = 1; i < mame_argc; i++)
	{
		if( mame_argv[ i ][ 0 ] == '-' )
		{
			if( stricmp( &mame_argv[ i ][ 1 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 1 ], shortcut ) == 0 ) )
			{
				i++;
				if ( i < mame_argc && mame_argv[ i ][ 0 ] != '-' )
				{
					res = mame_argv[ i ];
				}
				else if( yes != NULL && no != NULL && stricmp( res, no ) == 0 )
				{
					res = yes;
				}
			}
			else if( no != NULL && strnicmp( &mame_argv[ i ][ 1 ], "no", 2 ) == 0 &&
				( stricmp( &mame_argv[ i ][ 3 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 3 ], shortcut ) == 0 ) ) )
			{
				res = no;
			}
			else if( s_auto != NULL && strnicmp( &mame_argv[ i ][ 1 ], "auto", 4 ) == 0 &&
				( stricmp( &mame_argv[ i ][ 5 ], option ) == 0 ||
				( shortcut && stricmp( &mame_argv[ i ][ 5 ], shortcut ) == 0 ) ) )
			{
				res = s_auto;
			}
		}
	}
	return res;
}

extern struct rc_option fileio_opts[];

static void get_fileio_opts( void )
{
	int i;
	int s;
	const char *name;
	const char *shortname;
	const char *dest;

	struct
	{
		const char *name;
		const char *shortname;
	} shortcuts[] =
	{
		{ "inipath", NULL },
		{ "cfg_directory", "cfg" },
		{ "nvram_directory", "nvram" },
		{ "memcard_directory", "memcard" },
		{ "state_directory", "sta" },
		{ "history_file", NULL },
		{ NULL, NULL }
	};

	i = 0;
	while( fileio_opts[ i ].name != NULL )
	{
		if( fileio_opts[ i ].type == rc_string )
		{
			*( (char **)fileio_opts[ i ].dest ) = NULL;

			name = fileio_opts[ i ].name;
			shortname = fileio_opts[ i ].shortname;

			s = 0;
			while( shortcuts[ s ].name != NULL )
			{
				if( strcmp( shortcuts[ s ].name, name ) == 0 )
				{
					name = shortcuts[ s ].shortname;
					shortname = shortcuts[ s ].name;
					break;
				}
				s++;
			}

			if( name != NULL )
			{
				dest = get_string( "directory", name, shortname, fileio_opts[ i ].deflt, "", NULL, NULL );
				if( dest != NULL )
				{
					*( (char **)fileio_opts[ i ].dest ) = malloc( strlen( dest ) + 1 );
					if( *( (char **)fileio_opts[ i ].dest ) == NULL )
					{
						osd_die( "config.c: out of memory\n" );
						//exit(1);
					}
					strcpy( *( (char **)fileio_opts[ i ].dest ), dest );
					if( fileio_opts[ i ].func != NULL )
					{
						( *fileio_opts[ i ].func )( &fileio_opts[ i ], NULL, 0 );
					}
				}
			}
		}
		i++;
	}
}

static int help = 0;
struct rc_option frontend_opts[] = {
	{ "Frontend Related", NULL,	rc_seperator, NULL, NULL, 0, 0,	NULL, NULL },
	{ "help", "h", rc_set_int, &help, NULL, 1, 0, NULL, "show help message" },
	{ NULL, NULL, rc_end, NULL, NULL, 0, 0, NULL, NULL }
};
static void get_rc_opts( struct rc_option *p_rcopt )
{
	int i;
	int j;

	i = 0;
	while( p_rcopt[ i ].name != NULL )
	{
		if( p_rcopt[ i ].type == rc_set_int ||
			p_rcopt[ i ].type == rc_bool )
		{
			if( p_rcopt[ i ].deflt != NULL )
			{
				*( (int *)p_rcopt[ i ].dest ) = atoi( p_rcopt[ i ].deflt );
			}
			else
			{
				*( (int *)p_rcopt[ i ].dest ) = 0;
			}
		}
		i++;
	}

	i = 0;
	while( p_rcopt[ i ].name != NULL )
	{
		for( j = 1; j < mame_argc; j++ )
		{
			if( mame_argv[ j ][ 0 ] == '-' )
			{
				if( stricmp( &mame_argv[ j ][ 1 ], p_rcopt[ i ].name ) == 0 ||
					( p_rcopt[ i ].shortname != NULL && stricmp( &mame_argv[ j ][ 1 ], p_rcopt[ i ].shortname ) == 0 ) )
				{
					if( p_rcopt[ i ].type == rc_set_int )
					{
						*( (int *)p_rcopt[ i ].dest ) = p_rcopt[ i ].min;
					}
					else if( p_rcopt[ i ].type == rc_bool )
					{
						*( (int *)p_rcopt[ i ].dest ) = 1;
					}
					else if( p_rcopt[ i ].type == rc_int )
					{
						j++;
						*( (int *)p_rcopt[ i ].dest ) = atoi( mame_argv[ j ] );
						if( p_rcopt[ i ].func != NULL )
						{
							( *p_rcopt[ i ].func )( &p_rcopt[ i ], mame_argv[ j ], 0 );
						}
					}
					else if( p_rcopt[ i ].type == rc_string )
					{
						j++;
						*( (char **)p_rcopt[ i ].dest ) = malloc( strlen( mame_argv[ j ] ) + 1 );
						if( *( (char **)p_rcopt[ i ].dest ) == NULL )
						{
							osd_die( "config.c: out of memory\n" );
							//exit(1);
						}
						strcpy( *( (char **)p_rcopt[ i ].dest ), mame_argv[ j ] );
						if( p_rcopt[ i ].func != NULL )
						{
							( *p_rcopt[ i ].func )( &p_rcopt[ i ], mame_argv[ j ], 0 );
						}
					}
				}
				else if( strnicmp( &mame_argv[ j ][ 1 ], "no", 2 ) == 0 &&
					( stricmp( &mame_argv[ j ][ 3 ], p_rcopt[ i ].name ) == 0 ||
					( p_rcopt[ i ].shortname != NULL && stricmp( &mame_argv[ j ][ 3 ], p_rcopt[ i ].shortname ) == 0 ) ) )
				{
					if( p_rcopt[ i ].type == rc_bool )
					{
						*( (int *)p_rcopt[ i ].dest ) = 0;
					}
				}
			}
		}
		i++;
	}
}

struct rc_option config_opts[] =
{
	{ NULL, NULL, rc_link, frontend_opts, NULL, 0, 0, NULL, NULL },
	{ NULL, NULL, rc_link, fileio_opts, NULL, 0, 0, NULL, NULL },

	/* options supported by the mame core */
	/* video */
	{ "brightness", "bright", rc_float, &options.brightness, "1.0", 0.5, 2.0, NULL, "brightness correction"},
	{ "pause_brightness", NULL, rc_float, &options.pause_bright, "0.65", 0.5, 2.0, NULL, "additional pause brightness"},

	// name, shortname, type, dest, deflt, min, max, func, help
	/* misc */
	{ "skip_disclaimer", NULL, rc_bool, &options.skip_disclaimer, "0", 0, 0, NULL, "skip displaying the disclaimer screen" },
	{ "skip_gameinfo", NULL, rc_bool, &options.skip_gameinfo, "0", 0, 0, NULL, "skip displaying the game info screen" },
	{ "skip_validitychecks", NULL, rc_bool, &options.skip_validitychecks, "1", 0, 0, NULL, "skip doing the code validity checks" },
	{ NULL,	NULL, rc_end, NULL, NULL, 0, 0,	NULL, NULL }	
};

static void parse_cmdline( int argc, char **argv, int game_index )
{
	int orientation;

    char tmp[64];
    sprintf(tmp, "hist/%s-history.dat", BUILD_NAME);
    history_filename = tmp;

	struct InternalMachineDriver/*machine_config*/ drv;

	game = game_index;

	get_fileio_opts();

	/* read graphic configuration */
	options.use_samples			= get_bool( "config", "samples", NULL, 1 );
	/* sound configuration */
	soundcard					= get_int( "config", "soundcard",  NULL, -1 );
	enable_sound				= get_bool( "config", "sound", NULL, 1 );
	options.samplerate			= get_int( "config", "samplerate", "sr", 5512 );
	usestereo					= get_bool( "config", "stereo", NULL, 1 );
	attenuation 				= get_int( "config", "volume", "vol", 0 );
	sampleratedetect			= get_bool( "config", "sampleratedetect", NULL, 1 );

	/* misc configuration */
	history_filename			= get_string( "config", "historyfile", "history_file", history_filename, "", NULL, NULL );

	/* process language configuration */
	options.pause_bright		= get_float( "config", "pause_brightness", NULL, 0.65, 0.5, 2.0 );
	options.skip_disclaimer		= get_bool( "config", "skip_disclaimer", NULL, 1 );
	options.skip_gameinfo		= get_bool( "config", "skip_gameinfo", NULL, 0 );
	options.skip_validitychecks	= get_bool( "config", "skip_validitychecks", NULL, 1 );

#if (1==LINK_BIOS)
	s_bios						= get_string( "config", "bios", NULL, "default", NULL, NULL, NULL );
#endif //(1==LINK_BIOS)

//#if (0!=LINK_ROT)
//#if (1000!=LINK_ROT)
	if (SCR_ROT==setting.screensize)
	{
		keyorientation =1;
	}
	else
//#endif// (1000!=LINK_ROT)
//#endif// (0!=LINK_ROT)
	{
		keyorientation =0;
	}

    setBlitMode(setting.screensize);

	wait_vsync      = setting.vsync_ON_OFF;
	ui_show_fps_set(setting.show_fps_ON_OFF);
	throttle = setting.limit_ON_OFF;

	enable_sound =soundcard;//	soundcard =*/setting.sound_ON_OFF;

	if(0==(setting.sound_rate))	{options.samplerate = 0;	}
	else{options.samplerate = 44100/(setting.sound_rate);	}

	if(0 ==setting.frameskip)
	{
		frameskip = 0;
		autoframeskip = 1;
	}
	else
	{
		frameskip = setting.frameskip-1;
		autoframeskip = 0;
	}

		expand_machine_driver( drivers[ game ]->drv, &drv );

	/* first start with the game's built in orientation */
	orientation = drivers[ game_index ]->flags & ORIENTATION_MASK;
	options.ui_orientation = orientation;

	if( options.ui_orientation & ORIENTATION_SWAP_XY )
	{
		/* if only one of the components is inverted, switch them */
		if( ( options.ui_orientation & ROT180 ) == ORIENTATION_FLIP_X ||
			( options.ui_orientation & ROT180 ) == ORIENTATION_FLIP_Y )
		{
			options.ui_orientation ^= ROT180;
		}
	}

	/* rotate right */
	if( 1 == /*config_ror*/keyorientation )
	{
		/* if only one of the components is inverted, switch them */
		if( ( orientation & ROT180 ) == ORIENTATION_FLIP_X ||
			( orientation & ROT180 ) == ORIENTATION_FLIP_Y )
		{
			orientation ^= ROT180;
		}
		orientation ^= ROT90;
	}

	video_flipx = ( ( orientation & ORIENTATION_FLIP_X ) != 0 );
	video_flipy = ( ( orientation & ORIENTATION_FLIP_Y ) != 0 );
	video_swapxy = ( ( orientation & ORIENTATION_SWAP_XY ) != 0 );

	#if (1==LINK_BIOS)
	options.bios = malloc( strlen( s_bios ) + 1 );
	strcpy( options.bios, s_bios );
	#endif //(1==LINK_BIOS)
}

//============================================================
//	mame_set_to_options_samplerate
//============================================================

extern void sr_set(int snd_rate);//"src/sound/namco86"
void mame_set_to_options_samplerate(void)
{
	/* process sound options */
	sr_set(setting.sound_rate);

	if (0==setting.sound_rate)
	{
		options.samplerate =     0;
	}
	else
	{
		options.samplerate = 44100/(setting.sound_rate);
	}
}


//============================================================
//	cli_frontend_init
//============================================================

int cli_frontend_init( int argc, char **argv )
{
	char *gamename = argv[1];
	int i;
	int game_index;

	mame_argc = argc;
	mame_argv = argv;

	game_index = -1;

	/* clear all core options */
	memset(&options,0,sizeof(options));

	if (errorlog)
	{
		logfile = fopen(s_errorlog, "w"/*"wa"*/);
		if (logfile == NULL)
		{
			psp_printf_bbb("failed to open '%s'\n", s_errorlog);
		}
		curlogsize = 0;
	}

	{
		get_fileio_opts();
		get_rc_opts( frontend_opts );

		/* since the cpuintrf structure is filled dynamically now, we have to init first */
		cpuintrf_init();
		#ifndef PSP_DIRECT_STREAM
		/* 専用サウンド時には含まない． */
		sndintrf_init();
		#endif // PSP_DIRECT_STREAM
	}

	rompath_extra = dos_dirname( gamename );

	if( rompath_extra && strlen( rompath_extra ) == 0 )
	{
		free( rompath_extra );
		rompath_extra = NULL;
	}

	gamename = dos_basename( gamename );
	gamename = dos_strip_extension( gamename );

	/* If not playing back a new .inp file */
	if( game_index == -1 )
	{
		i = 0;
		while( drivers[ i ] != NULL )
		{
			if( stricmp( gamename, drivers[i]->name ) == 0 )
			{
				game_index = i;
				break;
			}
			i++;
		}
	}
 
	/* we give up. print a few approximate matches */
	if (game_index == -1)
	{
		return -1;
	}

	/* parse generic (os-independent) options */
	parse_cmdline( argc, argv, game_index );

	mame_set_to_options_samplerate();

	/* no sound is indicated by a 0 samplerate */
	if (SND_OFF==setting.sound_rate) options.samplerate = 0;

	return game_index;
}

//============================================================
//	cli_frontend_exit
//============================================================

void cli_frontend_exit(void)
{
	if (logfile)
	{
		fclose(logfile);
		logfile = NULL;
	}
}


//============================================================
//	logerror
//============================================================
//#if (1==PSP_USE_SYS_LOG)
void CLIB_DECL logerror(const char *text, ...)
{
	va_list arg;
	va_start(arg,text);

	if (errorlog)
	{
		if (logfile)
		{
			curlogsize += vfprintf(logfile, text, arg);
			if (maxlogsize > 0)
			{
				if (curlogsize > maxlogsize * 1024)
				{
					osd_close_display();
					osd_exit();
					cli_frontend_exit();
					sceKernelExitGame();
					//exit(1);
				}
			}
			if (b_flusherrorlog)
			{
				fflush(logfile);
			}
			if (b_closeerrorlog)
			{
				fclose(logfile);
				logfile = fopen(s_errorlog, "a");
			}
		}
	}
	va_end(arg);
}
//#endif

//============================================================
//	osd_printf
//============================================================

void CLIB_DECL osd_printf(const char *text, ...)
{
	char buf[128];

	va_list va;
	va_start(va, text);
	vsprintf(buf, text, va);
	va_end(va);

	psp_printf_bbb(buf);
	Confirm_Control();
}

//============================================================
//	dos_basename
//============================================================

static char *dos_basename(char *filename)
{
	char *c;

	// NULL begets NULL
	if (!filename)
		return NULL;

	// start at the end and return when we hit a slash or colon
	for (c = filename + strlen(filename) - 1; c >= filename; c--)
		if (*c == '\\' || *c == '/' || *c == ':')
			return c + 1;

	// otherwise, return the whole thing
	return filename;
}



//============================================================
//	dos_dirname
//============================================================

static char *dos_dirname(char *filename)
{
	char *s_dirname;
	char *c;

	// NULL begets NULL
	if (!filename)
		return NULL;

	// allocate space for it
	s_dirname = malloc(strlen(filename) + 1);
	if (!s_dirname)
	{
		osd_die( "error: malloc failed in dos_dirname\n");
		return NULL;
	}

	// copy in the name
	strcpy(s_dirname, filename);

	// search backward for a slash or a colon
	for (c = s_dirname + strlen(s_dirname) - 1; c >= s_dirname; c--)
		if (*c == '\\' || *c == '/' || *c == ':')
		{
			// found it: NULL terminate and return
			*(c + 1) = 0;
			return s_dirname;
		}

	// otherwise, return an empty string
	s_dirname[0] = 0;
	return s_dirname;
}



//============================================================
//	dos_strip_extension
//============================================================

static char *dos_strip_extension(char *filename)
{
	char *newname;
	char *c;

	// NULL begets NULL
	if (!filename)
		return NULL;

	// allocate space for it
	newname = malloc(strlen(filename) + 1);
	if (!newname)
	{
		osd_die( "error: malloc failed in osd_newname\n");
		return NULL;
	}

	// copy in the name
	strcpy(newname, filename);

	// search backward for a period, failing if we hit a slash or a colon
	for (c = newname + strlen(newname) - 1; c >= newname; c--)
	{
		// if we hit a period, NULL terminate and break
		if (*c == '.')
		{
			*c = 0;
			break;
		}

		// if we hit a slash or colon just stop
		if (*c == '\\' || *c == '/' || *c == ':')
			break;
	}

	return newname;
}


float get_config_float(const char *section, const char *name, float def) {
	return (def);
}
