#include <string.h>
#include <stdio.h>
#include "driver.h"
#include "rc.h"
//#include "syscall.h"	//tmk
#include "keyboard.h"	//tmk

/* sceCtrl */
/* Index for the two analog directions */
#define CTRL_ANALOG_X   0
#define CTRL_ANALOG_Y   1

/* Button bit masks */
#define CTRL_SQUARE      0x8000
#define CTRL_TRIANGLE   0x1000
#define CTRL_CIRCLE      0x2000
#define CTRL_CROSS      0x4000
#define CTRL_UP         0x0010
#define CTRL_DOWN      0x0040
#define CTRL_LEFT      0x0080
#define CTRL_RIGHT      0x0020
#define CTRL_START      0x0008
#define CTRL_SELECT      0x0001
#define CTRL_LTRIGGER   0x0100
#define CTRL_RTRIGGER   0x0200
#define	CTRL_HOLD	0x00020000

/* Returned control data */
typedef struct _ctrl_data
{
	UINT32 frame;	// マイクロ秒？
	UINT32 buttons;	// ボタン情報
	UINT8  analog[2];	// 0:X 1:Y
	UINT8  dummy[6];
} ctrl_data_t;

ctrl_data_t psp_ctl;

char key[KEY_MAX];
char keytmp[PSP_KEYMAX];

static cycles_t		last_poll =0;

void readkeys( void);
int keyorientation;


int use_mouse;
int joystick;
int steadykey;

#define ELEMENTS(x)			(sizeof(x) / sizeof((x)[0]))

#define MAX_PATH			256

//============================================================
//	PARAMETERS
//============================================================

#define MAX_KEYS			256

#define MAX_JOY				256
#define MAX_AXES			MAX_JOYSTICK_AXIS * MAX_JOYSTICK_STICKS
//#define MAX_BUTTONS			32
//#define MAX_POV				4

// this will be filled in dynamically
static struct OSCodeInfo	codelist[MAX_KEYS+MAX_JOY];
static int					total_codes;

// additional key data
static INT8					oldkey[MAX_KEYS];
static INT8					currkey[MAX_KEYS];

/* Mouse button mask */
static struct
{
	int axis[ 2 ];
	int buttons;
} mouse_state[ 2 ];

// current joystick
static int calibration_target;

//============================================================
//	KEYBOARD/JOYSTICK LIST
//============================================================

// macros for building/mapping keyboard codes
#define KEYCODE(allegrokey, ascii)		((allegrokey) | ((ascii) << 16))
#define ALLEGROKEY(keycode)				((keycode) & 0xff)
#define ASCIICODE(keycode)			(((keycode) >> 16) & 0xff)

// macros for building/mapping joystick codes
#define JOYCODE(joy, type, index)	((index) | ((type) << 8) | ((joy) << 12) | 0x80000000)
#define JOYINDEX(joycode)			((joycode) & 0xff)
#define CODETYPE(joycode)			(((joycode) >> 8) & 0xf)
#define JOYNUM(joycode)				(((joycode) >> 12) & 0xf)

// macros for differentiating the two
#define IS_KEYBOARD_CODE(code)		(((code) & 0x80000000) == 0)
#define IS_JOYSTICK_CODE(code)		(((code) & 0x80000000) != 0)

// joystick types
//#define CODETYPE_KEYBOARD			0
#define CODETYPE_AXIS_NEG			1
#define CODETYPE_AXIS_POS			2
//#define CODETYPE_POV_UP				3
//#define CODETYPE_POV_DOWN			4
//#define CODETYPE_POV_LEFT			5
//#define CODETYPE_POV_RIGHT			6
#define CODETYPE_BUTTON				7
#define CODETYPE_JOYAXIS			8
#define CODETYPE_MOUSEAXIS			9
#define CODETYPE_MOUSEBUTTON		10
//#define CODETYPE_GUNAXIS			11

// master keyboard translation table
static struct
{
	os_code_t keycode;
	int key;
	int ascii;
} msdos_key_trans_table[] =
{
	// MAME key				allegro key			ascii
	{ KEYCODE_ESC, 			KEY_ESC,			27 },
	{ KEYCODE_1, 			KEY_1,				'1' },
	{ KEYCODE_2, 			KEY_2,				'2' },
	{ KEYCODE_3, 			KEY_3,				'3' },
	{ KEYCODE_4, 			KEY_4,				'4' },
	{ KEYCODE_5, 			KEY_5,				'5' },
	{ KEYCODE_6, 			KEY_6,				'6' },
	{ KEYCODE_7, 			KEY_7,				'7' },
	{ KEYCODE_8, 			KEY_8,				'8' },
	{ KEYCODE_9, 			KEY_9,				'9' },
	{ KEYCODE_0, 			KEY_0,				'0' },
	{ KEYCODE_MINUS, 		KEY_MINUS, 			'-' },
	{ KEYCODE_EQUALS, 		KEY_EQUALS,		 	'=' },
	{ KEYCODE_BACKSPACE,	KEY_BACKSPACE, 		8 },
	{ KEYCODE_TAB, 			KEY_TAB, 			9 },
	{ KEYCODE_Q, 			KEY_Q,				'Q' },
	{ KEYCODE_W, 			KEY_W,				'W' },
	{ KEYCODE_E, 			KEY_E,				'E' },
	{ KEYCODE_R, 			KEY_R,				'R' },
	{ KEYCODE_T, 			KEY_T,				'T' },
	{ KEYCODE_Y, 			KEY_Y,				'Y' },
	{ KEYCODE_U, 			KEY_U,				'U' },
	{ KEYCODE_I, 			KEY_I,				'I' },
	{ KEYCODE_O, 			KEY_O,				'O' },
	{ KEYCODE_P, 			KEY_P,				'P' },
	{ KEYCODE_OPENBRACE,	KEY_OPENBRACE, 		'[' },
	{ KEYCODE_CLOSEBRACE,	KEY_CLOSEBRACE, 	']' },
	{ KEYCODE_ENTER, 		KEY_ENTER,			13 },
	{ KEYCODE_LCONTROL, 	KEY_LCONTROL, 		0 },
	{ KEYCODE_A, 			KEY_A,				'A' },
	{ KEYCODE_S, 			KEY_S,				'S' },
	{ KEYCODE_D, 			KEY_D,				'D' },
	{ KEYCODE_F, 			KEY_F,				'F' },
	{ KEYCODE_G, 			KEY_G,				'G' },
	{ KEYCODE_H, 			KEY_H,				'H' },
	{ KEYCODE_J, 			KEY_J,				'J' },
	{ KEYCODE_K, 			KEY_K,				'K' },
	{ KEYCODE_L, 			KEY_L,				'L' },
	{ KEYCODE_COLON, 		KEY_COLON,			';' },
	{ KEYCODE_QUOTE, 		KEY_QUOTE,			'\'' },
	{ KEYCODE_TILDE, 		KEY_TILDE, 			'`' },
	{ KEYCODE_LSHIFT, 		KEY_LSHIFT, 		0 },
	{ KEYCODE_BACKSLASH,	KEY_BACKSLASH, 		'\\' },
	{ KEYCODE_Z, 			KEY_Z,				'Z' },
	{ KEYCODE_X, 			KEY_X,				'X' },
	{ KEYCODE_C, 			KEY_C,				'C' },
	{ KEYCODE_V, 			KEY_V,				'V' },
	{ KEYCODE_B, 			KEY_B,				'B' },
	{ KEYCODE_N, 			KEY_N,				'N' },
	{ KEYCODE_M, 			KEY_M,				'M' },
	{ KEYCODE_COMMA, 		KEY_COMMA,			',' },
	{ KEYCODE_STOP, 		KEY_STOP, 			'.' },
	{ KEYCODE_SLASH, 		KEY_SLASH, 			'/' },
	{ KEYCODE_RSHIFT, 		KEY_RSHIFT, 		0 },
	{ KEYCODE_ASTERISK, 	KEY_ASTERISK, 		'*' },
	{ KEYCODE_LALT, 		KEY_ALT, 			0 },
	{ KEYCODE_SPACE, 		KEY_SPACE, 			' ' },
	{ KEYCODE_CAPSLOCK, 	KEY_CAPSLOCK, 		0 },
	{ KEYCODE_F1, 			KEY_F1,				0 },
	{ KEYCODE_F2, 			KEY_F2,				0 },
	{ KEYCODE_F3, 			KEY_F3,				0 },
	{ KEYCODE_F4, 			KEY_F4,				0 },
	{ KEYCODE_F5, 			KEY_F5,				0 },
	{ KEYCODE_F6, 			KEY_F6,				0 },
	{ KEYCODE_F7, 			KEY_F7,				0 },
	{ KEYCODE_F8, 			KEY_F8,				0 },
	{ KEYCODE_F9, 			KEY_F9,				0 },
	{ KEYCODE_F10, 			KEY_F10,			0 },
	{ KEYCODE_NUMLOCK, 		KEY_NUMLOCK,		0 },
	{ KEYCODE_SCRLOCK, 		KEY_SCRLOCK,		0 },
	{ KEYCODE_7_PAD, 		KEY_7_PAD,			0 },
	{ KEYCODE_8_PAD, 		KEY_8_PAD,			0 },
	{ KEYCODE_9_PAD, 		KEY_9_PAD,			0 },
	{ KEYCODE_MINUS_PAD,	KEY_MINUS_PAD,		0 },
	{ KEYCODE_4_PAD, 		KEY_4_PAD,			0 },
	{ KEYCODE_5_PAD, 		KEY_5_PAD,			0 },
	{ KEYCODE_6_PAD, 		KEY_6_PAD,			0 },
	{ KEYCODE_PLUS_PAD, 	KEY_PLUS_PAD,		0 },
	{ KEYCODE_1_PAD, 		KEY_1_PAD,			0 },
	{ KEYCODE_2_PAD, 		KEY_2_PAD,			0 },
	{ KEYCODE_3_PAD, 		KEY_3_PAD,			0 },
	{ KEYCODE_0_PAD, 		KEY_0_PAD,			0 },
	{ KEYCODE_DEL_PAD, 		KEY_DEL_PAD,		0 },
	{ KEYCODE_F11, 			KEY_F11,			0 },
	{ KEYCODE_F12, 			KEY_F12,			0 },
//	{ KEYCODE_F13, 			KEY_F13,			0 },
//	{ KEYCODE_F14, 			KEY_F14,			0 },
//	{ KEYCODE_F15, 			KEY_F15,			0 },
	{ KEYCODE_ENTER_PAD,	KEY_ENTER_PAD,		0 },
	{ KEYCODE_RCONTROL, 	KEY_RCONTROL,		0 },
	{ KEYCODE_SLASH_PAD,	KEY_SLASH_PAD,		0 },
	{ KEYCODE_PRTSCR, 		KEY_PRTSCR, 		0 },
	{ KEYCODE_RALT, 		KEY_ALTGR,			0 },
	{ KEYCODE_HOME, 		KEY_HOME,			0 },
	{ KEYCODE_UP, 			KEY_UP,				0 },
	{ KEYCODE_PGUP, 		KEY_PGUP,			0 },
	{ KEYCODE_LEFT, 		KEY_LEFT,			0 },
	{ KEYCODE_RIGHT, 		KEY_RIGHT,			0 },
	{ KEYCODE_END, 			KEY_END,			0 },
	{ KEYCODE_DOWN, 		KEY_DOWN,			0 },
	{ KEYCODE_PGDN, 		KEY_PGDN,			0 },
	{ KEYCODE_INSERT, 		KEY_INSERT,			0 },
	{ KEYCODE_DEL, 			KEY_DEL,			0 },
	{ KEYCODE_LWIN, 		KEY_LWIN,			0 },
	{ KEYCODE_RWIN, 		KEY_RWIN,			0 },
	{ KEYCODE_MENU, 		KEY_MENU,			0 },
	{ -1 }
};



// master joystick translation table
static int joy_trans_table[][2] =
{
	// internal code					MAME code
	{ JOYCODE(0, CODETYPE_AXIS_NEG, 0),	JOYCODE_1_LEFT },
	{ JOYCODE(0, CODETYPE_AXIS_POS, 0),	JOYCODE_1_RIGHT },
	{ JOYCODE(0, CODETYPE_AXIS_NEG, 1),	JOYCODE_1_UP },
	{ JOYCODE(0, CODETYPE_AXIS_POS, 1),	JOYCODE_1_DOWN },
	{ JOYCODE(0, CODETYPE_BUTTON, 0),	JOYCODE_1_BUTTON1 },
	{ JOYCODE(0, CODETYPE_BUTTON, 1),	JOYCODE_1_BUTTON2 },
	{ JOYCODE(0, CODETYPE_BUTTON, 2),	JOYCODE_1_BUTTON3 },
	{ JOYCODE(0, CODETYPE_BUTTON, 3),	JOYCODE_1_BUTTON4 },
	{ JOYCODE(0, CODETYPE_BUTTON, 4),	JOYCODE_1_BUTTON5 },
	{ JOYCODE(0, CODETYPE_BUTTON, 5),	JOYCODE_1_BUTTON6 },
	{ JOYCODE(0, CODETYPE_BUTTON, 6),	JOYCODE_1_BUTTON7 },
	{ JOYCODE(0, CODETYPE_BUTTON, 7),	JOYCODE_1_BUTTON8 },
	{ JOYCODE(0, CODETYPE_BUTTON, 8),	JOYCODE_1_BUTTON9 },
	{ JOYCODE(0, CODETYPE_BUTTON, 9),	JOYCODE_1_BUTTON10 },
	{ JOYCODE(0, CODETYPE_JOYAXIS, 0),	JOYCODE_1_ANALOG_X },
	{ JOYCODE(0, CODETYPE_JOYAXIS, 1),	JOYCODE_1_ANALOG_Y },
	{ JOYCODE(0, CODETYPE_JOYAXIS, 2),	JOYCODE_1_ANALOG_Z },

/* TMK	{ JOYCODE(1, CODETYPE_AXIS_NEG, 0),	JOYCODE_2_LEFT },
	{ JOYCODE(1, CODETYPE_AXIS_POS, 0),	JOYCODE_2_RIGHT },
	{ JOYCODE(1, CODETYPE_AXIS_NEG, 1),	JOYCODE_2_UP },
	{ JOYCODE(1, CODETYPE_AXIS_POS, 1),	JOYCODE_2_DOWN },
	{ JOYCODE(1, CODETYPE_BUTTON, 0),	JOYCODE_2_BUTTON1 },
	{ JOYCODE(1, CODETYPE_BUTTON, 1),	JOYCODE_2_BUTTON2 },
	{ JOYCODE(1, CODETYPE_BUTTON, 2),	JOYCODE_2_BUTTON3 },
	{ JOYCODE(1, CODETYPE_BUTTON, 3),	JOYCODE_2_BUTTON4 },
	{ JOYCODE(1, CODETYPE_BUTTON, 4),	JOYCODE_2_BUTTON5 },
	{ JOYCODE(1, CODETYPE_BUTTON, 5),	JOYCODE_2_BUTTON6 },
	{ JOYCODE(1, CODETYPE_BUTTON, 6),	JOYCODE_2_BUTTON7 },
	{ JOYCODE(1, CODETYPE_BUTTON, 7),	JOYCODE_2_BUTTON8 },
	{ JOYCODE(1, CODETYPE_BUTTON, 8),	JOYCODE_2_BUTTON9 },
	{ JOYCODE(1, CODETYPE_BUTTON, 9),	JOYCODE_2_BUTTON10 },
	{ JOYCODE(1, CODETYPE_JOYAXIS, 0),	JOYCODE_2_ANALOG_X },
	{ JOYCODE(1, CODETYPE_JOYAXIS, 1),	JOYCODE_2_ANALOG_Y },
	{ JOYCODE(1, CODETYPE_JOYAXIS, 2),	JOYCODE_2_ANALOG_Z },

	{ JOYCODE(2, CODETYPE_AXIS_NEG, 0),	JOYCODE_3_LEFT },
	{ JOYCODE(2, CODETYPE_AXIS_POS, 0),	JOYCODE_3_RIGHT },
	{ JOYCODE(2, CODETYPE_AXIS_NEG, 1),	JOYCODE_3_UP },
	{ JOYCODE(2, CODETYPE_AXIS_POS, 1),	JOYCODE_3_DOWN },
	{ JOYCODE(2, CODETYPE_BUTTON, 0),	JOYCODE_3_BUTTON1 },
	{ JOYCODE(2, CODETYPE_BUTTON, 1),	JOYCODE_3_BUTTON2 },
	{ JOYCODE(2, CODETYPE_BUTTON, 2),	JOYCODE_3_BUTTON3 },
	{ JOYCODE(2, CODETYPE_BUTTON, 3),	JOYCODE_3_BUTTON4 },
	{ JOYCODE(2, CODETYPE_BUTTON, 4),	JOYCODE_3_BUTTON5 },
	{ JOYCODE(2, CODETYPE_BUTTON, 5),	JOYCODE_3_BUTTON6 },
	{ JOYCODE(2, CODETYPE_BUTTON, 6),	JOYCODE_3_BUTTON7 },
	{ JOYCODE(2, CODETYPE_BUTTON, 7),	JOYCODE_3_BUTTON8 },
	{ JOYCODE(2, CODETYPE_BUTTON, 8),	JOYCODE_3_BUTTON9 },
	{ JOYCODE(2, CODETYPE_BUTTON, 9),	JOYCODE_3_BUTTON10 },
	{ JOYCODE(2, CODETYPE_JOYAXIS, 0),	JOYCODE_3_ANALOG_X },
	{ JOYCODE(2, CODETYPE_JOYAXIS, 1),	JOYCODE_3_ANALOG_Y },
	{ JOYCODE(2, CODETYPE_JOYAXIS, 2),	JOYCODE_3_ANALOG_Z },

	{ JOYCODE(3, CODETYPE_AXIS_NEG, 0),	JOYCODE_4_LEFT },
	{ JOYCODE(3, CODETYPE_AXIS_POS, 0),	JOYCODE_4_RIGHT },
	{ JOYCODE(3, CODETYPE_AXIS_NEG, 1),	JOYCODE_4_UP },
	{ JOYCODE(3, CODETYPE_AXIS_POS, 1),	JOYCODE_4_DOWN },
	{ JOYCODE(3, CODETYPE_BUTTON, 0),	JOYCODE_4_BUTTON1 },
	{ JOYCODE(3, CODETYPE_BUTTON, 1),	JOYCODE_4_BUTTON2 },
	{ JOYCODE(3, CODETYPE_BUTTON, 2),	JOYCODE_4_BUTTON3 },
	{ JOYCODE(3, CODETYPE_BUTTON, 3),	JOYCODE_4_BUTTON4 },
	{ JOYCODE(3, CODETYPE_BUTTON, 4),	JOYCODE_4_BUTTON5 },
	{ JOYCODE(3, CODETYPE_BUTTON, 5),	JOYCODE_4_BUTTON6 },
	{ JOYCODE(3, CODETYPE_BUTTON, 6),	JOYCODE_4_BUTTON7 },
	{ JOYCODE(3, CODETYPE_BUTTON, 7),	JOYCODE_4_BUTTON8 },
	{ JOYCODE(3, CODETYPE_BUTTON, 8),	JOYCODE_4_BUTTON9 },
	{ JOYCODE(3, CODETYPE_BUTTON, 9),	JOYCODE_4_BUTTON10 },
	{ JOYCODE(3, CODETYPE_JOYAXIS, 0),	JOYCODE_4_ANALOG_X },
	{ JOYCODE(3, CODETYPE_JOYAXIS, 1),	JOYCODE_4_ANALOG_Y },
	{ JOYCODE(3, CODETYPE_JOYAXIS, 2),	JOYCODE_4_ANALOG_Z },

	{ JOYCODE(4, CODETYPE_AXIS_NEG, 0),	JOYCODE_5_LEFT },
	{ JOYCODE(4, CODETYPE_AXIS_POS, 0),	JOYCODE_5_RIGHT },
	{ JOYCODE(4, CODETYPE_AXIS_NEG, 1),	JOYCODE_5_UP },
	{ JOYCODE(4, CODETYPE_AXIS_POS, 1),	JOYCODE_5_DOWN },
	{ JOYCODE(4, CODETYPE_BUTTON, 0),	JOYCODE_5_BUTTON1 },
	{ JOYCODE(4, CODETYPE_BUTTON, 1),	JOYCODE_5_BUTTON2 },
	{ JOYCODE(4, CODETYPE_BUTTON, 2),	JOYCODE_5_BUTTON3 },
	{ JOYCODE(4, CODETYPE_BUTTON, 3),	JOYCODE_5_BUTTON4 },
	{ JOYCODE(4, CODETYPE_BUTTON, 4),	JOYCODE_5_BUTTON5 },
	{ JOYCODE(4, CODETYPE_BUTTON, 5),	JOYCODE_5_BUTTON6 },
	{ JOYCODE(4, CODETYPE_BUTTON, 6),	JOYCODE_5_BUTTON7 },
	{ JOYCODE(4, CODETYPE_BUTTON, 7),	JOYCODE_5_BUTTON8 },
	{ JOYCODE(4, CODETYPE_BUTTON, 8),	JOYCODE_5_BUTTON9 },
	{ JOYCODE(4, CODETYPE_BUTTON, 9),	JOYCODE_5_BUTTON10 },
	{ JOYCODE(4, CODETYPE_JOYAXIS, 0),	JOYCODE_5_ANALOG_X },
	{ JOYCODE(4, CODETYPE_JOYAXIS, 1), 	JOYCODE_5_ANALOG_Y },
	{ JOYCODE(4, CODETYPE_JOYAXIS, 2),	JOYCODE_5_ANALOG_Z },

	{ JOYCODE(5, CODETYPE_AXIS_NEG, 0),	JOYCODE_6_LEFT },
	{ JOYCODE(5, CODETYPE_AXIS_POS, 0),	JOYCODE_6_RIGHT },
	{ JOYCODE(5, CODETYPE_AXIS_NEG, 1),	JOYCODE_6_UP },
	{ JOYCODE(5, CODETYPE_AXIS_POS, 1),	JOYCODE_6_DOWN },
	{ JOYCODE(5, CODETYPE_BUTTON, 0),	JOYCODE_6_BUTTON1 },
	{ JOYCODE(5, CODETYPE_BUTTON, 1),	JOYCODE_6_BUTTON2 },
	{ JOYCODE(5, CODETYPE_BUTTON, 2),	JOYCODE_6_BUTTON3 },
	{ JOYCODE(5, CODETYPE_BUTTON, 3),	JOYCODE_6_BUTTON4 },
	{ JOYCODE(5, CODETYPE_BUTTON, 4),	JOYCODE_6_BUTTON5 },
	{ JOYCODE(5, CODETYPE_BUTTON, 5),	JOYCODE_6_BUTTON6 },
	{ JOYCODE(5, CODETYPE_BUTTON, 6),	JOYCODE_6_BUTTON7 },
	{ JOYCODE(5, CODETYPE_BUTTON, 7),	JOYCODE_6_BUTTON8 },
	{ JOYCODE(5, CODETYPE_BUTTON, 8),	JOYCODE_6_BUTTON9 },
	{ JOYCODE(5, CODETYPE_BUTTON, 9),	JOYCODE_6_BUTTON10 },
	{ JOYCODE(5, CODETYPE_JOYAXIS, 0),	JOYCODE_6_ANALOG_X },
	{ JOYCODE(5, CODETYPE_JOYAXIS, 1),	JOYCODE_6_ANALOG_Y },
	{ JOYCODE(5, CODETYPE_JOYAXIS, 2),	JOYCODE_6_ANALOG_Z },

	{ JOYCODE(6, CODETYPE_AXIS_NEG, 0),	JOYCODE_7_LEFT },
	{ JOYCODE(6, CODETYPE_AXIS_POS, 0),	JOYCODE_7_RIGHT },
	{ JOYCODE(6, CODETYPE_AXIS_NEG, 1),	JOYCODE_7_UP },
	{ JOYCODE(6, CODETYPE_AXIS_POS, 1),	JOYCODE_7_DOWN },
	{ JOYCODE(6, CODETYPE_BUTTON, 0),	JOYCODE_7_BUTTON1 },
	{ JOYCODE(6, CODETYPE_BUTTON, 1),	JOYCODE_7_BUTTON2 },
	{ JOYCODE(6, CODETYPE_BUTTON, 2),	JOYCODE_7_BUTTON3 },
	{ JOYCODE(6, CODETYPE_BUTTON, 3),	JOYCODE_7_BUTTON4 },
	{ JOYCODE(6, CODETYPE_BUTTON, 4),	JOYCODE_7_BUTTON5 },
	{ JOYCODE(6, CODETYPE_BUTTON, 5),	JOYCODE_7_BUTTON6 },
	{ JOYCODE(6, CODETYPE_BUTTON, 6),	JOYCODE_7_BUTTON7 },
	{ JOYCODE(6, CODETYPE_BUTTON, 7),	JOYCODE_7_BUTTON8 },
	{ JOYCODE(6, CODETYPE_BUTTON, 8),	JOYCODE_7_BUTTON9 },
	{ JOYCODE(6, CODETYPE_BUTTON, 9),	JOYCODE_7_BUTTON10 },
	{ JOYCODE(6, CODETYPE_JOYAXIS, 0),	JOYCODE_7_ANALOG_X },
	{ JOYCODE(6, CODETYPE_JOYAXIS, 1),	JOYCODE_7_ANALOG_Y },
	{ JOYCODE(6, CODETYPE_JOYAXIS, 2),	JOYCODE_7_ANALOG_Z },

	{ JOYCODE(7, CODETYPE_AXIS_NEG, 0),	JOYCODE_8_LEFT },
	{ JOYCODE(7, CODETYPE_AXIS_POS, 0),	JOYCODE_8_RIGHT },
	{ JOYCODE(7, CODETYPE_AXIS_NEG, 1),	JOYCODE_8_UP },
	{ JOYCODE(7, CODETYPE_AXIS_POS, 1),	JOYCODE_8_DOWN },
	{ JOYCODE(7, CODETYPE_BUTTON, 0),	JOYCODE_8_BUTTON1 },
	{ JOYCODE(7, CODETYPE_BUTTON, 1),	JOYCODE_8_BUTTON2 },
	{ JOYCODE(7, CODETYPE_BUTTON, 2),	JOYCODE_8_BUTTON3 },
	{ JOYCODE(7, CODETYPE_BUTTON, 3),	JOYCODE_8_BUTTON4 },
	{ JOYCODE(7, CODETYPE_BUTTON, 4),	JOYCODE_8_BUTTON5 },
	{ JOYCODE(7, CODETYPE_BUTTON, 5),	JOYCODE_8_BUTTON6 },
	{ JOYCODE(7, CODETYPE_BUTTON, 6),	JOYCODE_8_BUTTON7 },
	{ JOYCODE(7, CODETYPE_BUTTON, 7),	JOYCODE_8_BUTTON8 },
	{ JOYCODE(7, CODETYPE_BUTTON, 8),	JOYCODE_8_BUTTON9 },
	{ JOYCODE(7, CODETYPE_BUTTON, 9),	JOYCODE_8_BUTTON10 },
	{ JOYCODE(7, CODETYPE_JOYAXIS, 0),	JOYCODE_8_ANALOG_X },
	{ JOYCODE(7, CODETYPE_JOYAXIS, 1),	JOYCODE_8_ANALOG_Y },
	{ JOYCODE(7, CODETYPE_JOYAXIS, 2),	JOYCODE_8_ANALOG_Z },

	{ JOYCODE(0, CODETYPE_MOUSEBUTTON, 0), 	MOUSECODE_1_BUTTON1 },
	{ JOYCODE(0, CODETYPE_MOUSEBUTTON, 1), 	MOUSECODE_1_BUTTON2 },
	{ JOYCODE(0, CODETYPE_MOUSEBUTTON, 2), 	MOUSECODE_1_BUTTON3 },
	{ JOYCODE(0, CODETYPE_MOUSEBUTTON, 3), 	MOUSECODE_1_BUTTON4 },
	{ JOYCODE(0, CODETYPE_MOUSEAXIS, 0),	MOUSECODE_1_ANALOG_X },
	{ JOYCODE(0, CODETYPE_MOUSEAXIS, 1),	MOUSECODE_1_ANALOG_Y },
	{ JOYCODE(0, CODETYPE_MOUSEAXIS, 2),	MOUSECODE_1_ANALOG_Z },

	{ JOYCODE(1, CODETYPE_MOUSEBUTTON, 0), 	MOUSECODE_2_BUTTON1 },
	{ JOYCODE(1, CODETYPE_MOUSEBUTTON, 1), 	MOUSECODE_2_BUTTON2 },
	{ JOYCODE(1, CODETYPE_MOUSEBUTTON, 2), 	MOUSECODE_2_BUTTON3 },
	{ JOYCODE(1, CODETYPE_MOUSEBUTTON, 3), 	MOUSECODE_2_BUTTON4 },
	{ JOYCODE(1, CODETYPE_MOUSEAXIS, 0),	MOUSECODE_2_ANALOG_X },
	{ JOYCODE(1, CODETYPE_MOUSEAXIS, 1),	MOUSECODE_2_ANALOG_Y },
	{ JOYCODE(1, CODETYPE_MOUSEAXIS, 2),	MOUSECODE_2_ANALOG_Z },
TMK */
//	{ JOYCODE(0, CODETYPE_GUNAXIS, 0),		GUNCODE_1_ANALOG_X },
//	{ JOYCODE(0, CODETYPE_GUNAXIS, 1),		GUNCODE_1_ANALOG_Y },

//	{ JOYCODE(1, CODETYPE_GUNAXIS, 0),		GUNCODE_2_ANALOG_X },
//	{ JOYCODE(1, CODETYPE_GUNAXIS, 1),		GUNCODE_2_ANALOG_Y },
};


//============================================================
//	updatekeyboard
//============================================================

// since the keyboard controller is slow, it is not capable of reporting multiple
// key presses fast enough. We have to delay them in order not to lose special moves
// tied to simultaneous button presses.

static void updatekeyboard(void)
{
	int i;
	int changed;

	changed = 0;
	for (i = 0;i < KEY_MAX;i++)
	{
		if (key[i] != oldkey[i])
		{
			changed = 1;

			if (key[i] == 0 && currkey[i] == 0)
			{
				/* keypress was missed, turn it on for one frame */
				currkey[i] = -1;
			}
		}
	}

	/* if keyboard state is stable, copy it over */
	if (!changed)
	{
		for (i = 0;i < KEY_MAX;i++)
		{
			currkey[i] = key[i];
		}
	}

	for (i = 0;i < KEY_MAX;i++)
	{
		oldkey[i] = key[i];
	}
}


void poll_joysticks(void)
{
	updatekeyboard();
}


static const char *msdos_key_name( int key )
{
	switch( key )
	{
	case KEY_ESC: return "Escape";
//	case KEY_1: return "1";
	case KEY_1: return "START";
	case KEY_2: return "2";
	case KEY_3: return "3";
	case KEY_4: return "4";
//	case KEY_5: return "5";
	case KEY_5: return "SELECT";
	case KEY_6: return "6";
	case KEY_7: return "7";
	case KEY_8: return "8";
	case KEY_9: return "9";
	case KEY_0: return "0";
	case KEY_MINUS: return "-";
	case KEY_EQUALS: return "=";
	case KEY_BACKSPACE: return "Backspace";
//	case KEY_TAB: return "Tab";
	case KEY_TAB: return "LTRIGGER";
	case KEY_Q: return "Q";
	case KEY_W: return "W";
	case KEY_E: return "E";
	case KEY_R: return "R";
	case KEY_T: return "T";
	case KEY_Y: return "Y";
	case KEY_U: return "U";
	case KEY_I: return "I";
	case KEY_O: return "O";
//	case KEY_P: return "P";
	case KEY_P: return "RTRIGGER";
	case KEY_OPENBRACE: return "[";
	case KEY_CLOSEBRACE: return "]";
	case KEY_ENTER: return "Enter";
//	case KEY_LCONTROL: return "Left Ctrl";
	case KEY_LCONTROL: return "TRIANGLE";
	case KEY_A: return "A";
	case KEY_S: return "S";
	case KEY_D: return "D";
	case KEY_F: return "F";
	case KEY_G: return "G";
	case KEY_H: return "H";
	case KEY_J: return "J";
	case KEY_K: return "K";
	case KEY_L: return "L";
	case KEY_COLON: return ";";
	case KEY_QUOTE: return "'";
	case KEY_TILDE: return "`";
//	case KEY_LSHIFT: return "Left Shift";
	case KEY_LSHIFT: return "SQUARE";
	case KEY_BACKSLASH: return "\\";
	case KEY_Z: return "Z";
	case KEY_X: return "X";
	case KEY_C: return "C";
	case KEY_V: return "V";
	case KEY_B: return "B";
	case KEY_N: return "N";
	case KEY_M: return "M";
	case KEY_COMMA: return ",";
	case KEY_STOP: return ".";
	case KEY_SLASH: return "/";
	case KEY_RSHIFT: return "Right Shift";
	case KEY_ASTERISK: return "Numpad *";
//	case KEY_ALT: return "Left Alt";
	case KEY_ALT: return "CIRCLE";
//	case KEY_SPACE: return "Space";
	case KEY_SPACE: return "CROSS";
	case KEY_CAPSLOCK: return "CapsLock";
	case KEY_F1: return "F1";
	case KEY_F2: return "F2";
	case KEY_F3: return "F3";
	case KEY_F4: return "F4";
	case KEY_F5: return "F5";
	case KEY_F6: return "F6";
	case KEY_F7: return "F7";
	case KEY_F8: return "F8";
	case KEY_F9: return "F9";
	case KEY_F10: return "F10";
	case KEY_NUMLOCK: return "NumLock";
	case KEY_SCRLOCK: return "ScrollLock";
	case KEY_7_PAD: return "Numpad 7";
	case KEY_8_PAD: return "Numpad 8";
	case KEY_9_PAD: return "Numpad 9";
	case KEY_MINUS_PAD: return "Numpad -";
	case KEY_4_PAD: return "Numpad 4";
	case KEY_5_PAD: return "Numpad 5";
	case KEY_6_PAD: return "Numpad 6";
	case KEY_PLUS_PAD: return "Numpad +";
	case KEY_1_PAD: return "Numpad 1";
	case KEY_2_PAD: return "Numpad 2";
	case KEY_3_PAD: return "Numpad 3";
	case KEY_0_PAD: return "Numpad 0";
	case KEY_DEL_PAD: return "Numpad .";
	case KEY_F11: return "F11";
	case KEY_F12: return "F12";
//	case KEY_F13: return "F13";
//	case KEY_F14: return "F14";
//	case KEY_F15: return "F15";
	case KEY_ENTER_PAD: return "Numpad Enter";
	case KEY_RCONTROL: return "Right Ctrl";
	case KEY_SLASH_PAD: return "Numpad /";
	case KEY_PRTSCR: return "SysRq";
	case KEY_ALTGR: return "Right Alt";
	case KEY_HOME: return "Home";
	case KEY_UP: return "Up Arrow";
	case KEY_PGUP: return "PgUp";
	case KEY_LEFT: return "Left Arrow";
	case KEY_RIGHT: return "Right Arrow";
	case KEY_END: return "End";
	case KEY_DOWN: return "Down Arrow";
	case KEY_PGDN: return "PgDn";
	case KEY_INSERT: return "Insert";
	case KEY_DEL: return "Delete";
	case KEY_LWIN: return "Left Win";
	case KEY_RWIN: return "Right Win";
	case KEY_MENU: return "AppMenu";
	default: return NULL;
	}
}


//============================================================
//	is_key_pressed
//============================================================

static int is_key_pressed(os_code_t keycode)
{
	int allegrokey = ALLEGROKEY(keycode);

	cycles_t now_poll =sceKernelLibcClock();
	if (now_poll > (last_poll + (1000000/60)) || now_poll <last_poll)
	{
		readkeys();
		last_poll =sceKernelLibcClock();
	}

	/* if update_video_and_audio() is not being called yet, copy key array */
	if (!Machine->scrbitmap)
	{
		int i;

		for (i = 0;i < KEY_MAX;i++)
		{
			currkey[i] = key[i];
		}
	}

	if (allegrokey == KEY_PAUSE)
	{
		static int pressed;
		static int counter;
		int res;

		res = currkey[KEY_PAUSE] ^ pressed;
		if (res)
		{
			if (counter > 0)
			{
				if (--counter == 0)
				{
					pressed = currkey[KEY_PAUSE];
				}
			}
			else
			{
				counter = 10;
			}
		}

		return res;
	}

    if(steadykey)
	{
		return currkey[allegrokey];
	}
	else
	{
		return key[allegrokey];
	}
}



//============================================================
//	init_keycodes
//============================================================

static void init_keycodes(void)
{
	int key;

	// iterate over all possible keys
	for (key = 0; key < MAX_KEYS; key++)
	{
		const char *keyname = msdos_key_name( key );
		if( keyname != NULL )
		{
			// copy the name
			char *namecopy = malloc(strlen(keyname) + 1);
			if (namecopy)
			{
				input_code_t standardcode;
				os_code_t code;
				int entry;

				// find the table entry, if there is one
				for (entry = 0; msdos_key_trans_table[entry].keycode >= 0; entry++)
					if (msdos_key_trans_table[entry].key == key)
						break;

				// compute the code, which encodes allegro and ASCII codes
				code = KEYCODE(key, 0);
				standardcode = CODE_OTHER_DIGITAL;
				if (msdos_key_trans_table[entry].keycode >= 0)
				{
					code = KEYCODE(key, msdos_key_trans_table[entry].ascii);
					standardcode = msdos_key_trans_table[entry].keycode;
				}

				// fill in the key description
				codelist[total_codes].name = strcpy(namecopy, keyname);
				codelist[total_codes].oscode = code;
				codelist[total_codes].inputcode = standardcode;
				total_codes++;
			}
		}
	}
}


void shutdown_keyboard(void)
{
	int key;

	for (key = 0; key < total_codes; key++)
		free(codelist[key].name);
}
//============================================================
//	add_joylist_entry
//============================================================

static void add_joylist_entry(const char *name, os_code_t code, input_code_t standardcode)
{
	// copy the name
	char *namecopy = malloc(strlen(name) + 1);
	if (namecopy)
	{
		int entry;

		// find the table entry, if there is one
		for (entry = 0; entry < ELEMENTS(joy_trans_table); entry++)
			if (joy_trans_table[entry][0] == code)
				break;

		// fill in the joy description
		codelist[total_codes].name = strcpy(namecopy, name);
		codelist[total_codes].oscode = code;
		if (entry < ELEMENTS(joy_trans_table))
			standardcode = joy_trans_table[entry][1];
		codelist[total_codes].inputcode = standardcode;
		total_codes++;
	}
}



//============================================================
//	init_joycodes
//============================================================

static void init_joycodes(void)
{
	int stick;
	int axis;
	char tempname[MAX_PATH];

	for (stick = 0; stick < 1; stick++)
	{
		for (axis = 0; axis < 2; axis++)
		{
			{
				char name[ 128 ];
				if (0 ==axis)
					sprintf( name, "JOY horiz" );
				else
					sprintf( name, "JOY Verti" );
				// add analog axis
				sprintf(tempname, "J%d %s", stick + 1, name);
				add_joylist_entry(tempname, JOYCODE(stick, CODETYPE_JOYAXIS, axis), CODE_OTHER_ANALOG_ABSOLUTE);

				// add negative value
				sprintf(tempname, "J%d %s -", stick + 1, name);
				add_joylist_entry(tempname, JOYCODE(stick, CODETYPE_AXIS_NEG, axis), CODE_OTHER_DIGITAL);

				// add positive value
				sprintf(tempname, "J%d %s +", stick + 1, name);
				add_joylist_entry(tempname, JOYCODE(stick, CODETYPE_AXIS_POS, axis), CODE_OTHER_DIGITAL);
			}
		}
	}
}



//============================================================
//	get_joycode_value
//============================================================

static INT32 get_joycode_value(os_code_t joycode)
{
	int joyindex = JOYINDEX(joycode);
	int codetype = CODETYPE(joycode);
	int joynum = JOYNUM(joycode);
//	DWORD pov;

	// switch off the type
	switch (codetype)
	{
	case CODETYPE_MOUSEBUTTON:
		return ( mouse_state[ joynum ].buttons >> joyindex ) & 1;

	case CODETYPE_BUTTON:
		return 0;

	case CODETYPE_AXIS_POS:
	case CODETYPE_AXIS_NEG:
	{
//		{ JOYCODE(0, CODETYPE_AXIS_NEG, 0),	JOYCODE_1_LEFT },
//		{ JOYCODE(0, CODETYPE_AXIS_POS, 0),	JOYCODE_1_RIGHT },
//		{ JOYCODE(0, CODETYPE_AXIS_NEG, 1),	JOYCODE_1_UP },
//		{ JOYCODE(0, CODETYPE_AXIS_POS, 1),	JOYCODE_1_DOWN },
//
//		if (psp_ctl.analog[CTRL_ANALOG_Y] == 0xff)	downkey(KEY_2_PAD, &i);	// DOWN
//		if (psp_ctl.analog[CTRL_ANALOG_Y] == 0x00)	downkey(KEY_8_PAD, &i); // UP
//		if (psp_ctl.analog[CTRL_ANALOG_X] == 0x00)	downkey(KEY_4_PAD, &i); // LEFT
//		if (psp_ctl.analog[CTRL_ANALOG_X] == 0xff)	downkey(KEY_6_PAD, &i); // RIGHT
		if (0 ==joynum)
		{
			if (keyorientation) {
				if( codetype == CODETYPE_AXIS_POS )
					if (0 ==joyindex)
						return (0xE0 <=psp_ctl.analog[1]);
					else
						return (0x20 >=psp_ctl.analog[0]);
				else
					if (0 ==joyindex)
						return (0x20 >=psp_ctl.analog[1]);
					else
						return (0xE0 <=psp_ctl.analog[0]);
			} else {
				if( codetype == CODETYPE_AXIS_POS )
					return (0xE0 <=psp_ctl.analog[joyindex]);
				else
					return (0x20 >=psp_ctl.analog[joyindex]);
			}
		}
		return 0;
	}

	// anywhere from 0-45 (315) deg to 0+45 (45) deg
//	case CODETYPE_POV_UP:
//		return 0;

	// anywhere from 90-45 (45) deg to 90+45 (135) deg
//	case CODETYPE_POV_RIGHT:
//		return 0;

	// anywhere from 180-45 (135) deg to 180+45 (225) deg
//	case CODETYPE_POV_DOWN:
//		return 0;

	// anywhere from 270-45 (225) deg to 270+45 (315) deg
//	case CODETYPE_POV_LEFT:
//		return 0;

	// analog joystick axis
	case CODETYPE_JOYAXIS:
	{
//	{ JOYCODE(0, CODETYPE_JOYAXIS, 0),	JOYCODE_1_ANALOG_X },
//	{ JOYCODE(0, CODETYPE_JOYAXIS, 1),	JOYCODE_1_ANALOG_Y },

//		if (psp_ctl.analog[CTRL_ANALOG_Y] == 0xff)	downkey(KEY_2_PAD, &i);	// DOWN
//		if (psp_ctl.analog[CTRL_ANALOG_Y] == 0x00)	downkey(KEY_8_PAD, &i); // UP
//		if (psp_ctl.analog[CTRL_ANALOG_X] == 0x00)	downkey(KEY_4_PAD, &i); // LEFT
//		if (psp_ctl.analog[CTRL_ANALOG_X] == 0xff)	downkey(KEY_6_PAD, &i); // RIGHT

		if (0 ==joynum)
		{
			if (keyorientation) {
				if (0 ==joyindex) {
					INT32 val = psp_ctl.analog[1];
					INT32 top = 0xff;
					INT32 bottom = 0x00;

					val = (INT64)(val - bottom) * (INT64)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN) / (INT64)(top - bottom) + ANALOG_VALUE_MIN;
					if (val < ANALOG_VALUE_MIN) val = ANALOG_VALUE_MIN;
					if (val > ANALOG_VALUE_MAX) val = ANALOG_VALUE_MAX;

					return val;
				} else {
					INT32 val = psp_ctl.analog[0];
					INT32 top = 0x00;
					INT32 bottom = 0xff;

					val = (INT64)(val - bottom) * (INT64)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN) / (INT64)(top - bottom) + ANALOG_VALUE_MIN;
					if (val < ANALOG_VALUE_MIN) val = ANALOG_VALUE_MIN;
					if (val > ANALOG_VALUE_MAX) val = ANALOG_VALUE_MAX;

					return val;
				}
			} else {
//				INT32 val = joy[ joynum ].stick[ s ].axis[ a ].pos;
//				INT32 top = 128;
//				INT32 bottom = -128;
				INT32 val = psp_ctl.analog[joyindex];
				INT32 top = 0xff;
				INT32 bottom = 0x00;

				val = (INT64)(val - bottom) * (INT64)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN) / (INT64)(top - bottom) + ANALOG_VALUE_MIN;
				if (val < ANALOG_VALUE_MIN) val = ANALOG_VALUE_MIN;
				if (val > ANALOG_VALUE_MAX) val = ANALOG_VALUE_MAX;
				return val;
			}
		}
		return 0;
	}

	// analog mouse axis
	case CODETYPE_MOUSEAXIS:
		return mouse_state[ joynum ].axis[ joyindex ] * 512;

	// analog gun axis
//	case CODETYPE_GUNAXIS:
//		return 0;
	}
	return 0;
}



//============================================================
//	osd_is_code_pressed
//============================================================

INT32 osd_get_code_value(os_code_t code)
{
	if (IS_KEYBOARD_CODE(code))
		return is_key_pressed(code);
	else
		return get_joycode_value(code);
}



//============================================================
//	osd_get_code_list
//============================================================

const struct OSCodeInfo *osd_get_code_list(void)
{
	return codelist;
}

void osd_customize_inputport_list(struct InputPortDefinition *defaults)
{
}



//============================================================
//	osd_joystick_needs_calibration
//============================================================

int osd_joystick_needs_calibration(void)
{
	return 0;
}



//============================================================
//	osd_joystick_start_calibration
//============================================================

void osd_joystick_start_calibration(void)
{
	calibration_target = 0;
}



//============================================================
//	osd_joystick_calibrate_next
//============================================================

const char *osd_joystick_calibrate_next(void)
{
	return 0;
}



//============================================================
//	osd_joystick_calibrate
//============================================================

void osd_joystick_calibrate(void)
{
//TMK	calibrate_joystick (calibration_target);
}



//============================================================
//	osd_joystick_end_calibration
//============================================================

void osd_joystick_end_calibration(void)
{
//TMK	save_joystick_data(0);
}



void psp_init_input (void)
{
	int err;

	total_codes =0;

	// init the keyboard list
	init_keycodes();

	// init the joystick list
	init_joycodes();

	// terminate array
	memset(&codelist[total_codes], 0, sizeof(codelist[total_codes]));
}

void init_keyboard(void)
{
psp_init_input();
}



void psp_shutdown_input(void)
{

}

//extern int psp_exit;

void downkey(int ikey, int *idx);
void readkeys( void)
{
   	int i;
	static int cnter =0;

	i = 0;
	while ( i < PSP_KEYMAX )
	{
		key [(int)keytmp[i] ] = 0;
		i++;
	}

//	sceCtrlReadBufferPositive(&psp_ctl,1);
	sceCtrlPeekBufferPositive(&psp_ctl,1);

	i = 0;
	if ((psp_ctl.buttons & (CTRL_LTRIGGER | CTRL_RTRIGGER | CTRL_START))
		==(CTRL_LTRIGGER | CTRL_RTRIGGER | CTRL_START)) {
		psp_loop =1; //<-- VER
	}
//	} else {
		if (psp_ctl.buttons & CTRL_LTRIGGER)	downkey(KEY_TAB, &i);
		if (psp_ctl.buttons & CTRL_RTRIGGER)	downkey(KEY_P, &i);
		if (psp_ctl.buttons & CTRL_SELECT)		downkey(KEY_5, &i);
		if (psp_ctl.buttons & CTRL_START)		downkey(KEY_1, &i);

		if (keyorientation) {
			if (psp_ctl.buttons & CTRL_RIGHT)	downkey(KEY_UP, &i);
			if (psp_ctl.buttons & CTRL_LEFT)	downkey(KEY_DOWN, &i);
			if (psp_ctl.buttons & CTRL_UP)	downkey(KEY_LEFT, &i);
			if (psp_ctl.buttons & CTRL_DOWN)	downkey(KEY_RIGHT, &i);
		} else {
			if (psp_ctl.buttons & CTRL_UP)	downkey(KEY_UP, &i);
			if (psp_ctl.buttons & CTRL_DOWN)	downkey(KEY_DOWN, &i);
			if (psp_ctl.buttons & CTRL_LEFT)	downkey(KEY_LEFT, &i);
			if (psp_ctl.buttons & CTRL_RIGHT)	downkey(KEY_RIGHT, &i);
		}

		if (psp_ctl.buttons & CTRL_TRIANGLE)	downkey(KEY_LCONTROL, &i);
		if (psp_ctl.buttons & CTRL_CIRCLE)		downkey(KEY_ALT, &i);
		if (psp_ctl.buttons & CTRL_CROSS)		downkey(KEY_SPACE, &i);
		if (psp_ctl.buttons & CTRL_SQUARE)		downkey(KEY_LSHIFT, &i);
//	}

//tmkdbg
//	pgcLocate(0,1);
//	pgcPuthex8(psp_ctl.buttons);
//	pgcLocate(0,2);
//	pgcPuthex8(cnter);
//	cnter ++;
//	if (cnter >30000) cnter =0;
}

void downkey(int ikey, int *idx) {
	key[ikey] =1;
	keytmp[*idx] =ikey;
	*idx +=1;
}
