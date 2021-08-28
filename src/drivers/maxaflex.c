/******************************************************************************
    Exidy Max-A-Flex driver

    by Mariusz Wojcieszek

    Based on Atari 400/800 MESS Driver by Juergen Buchmueller

    TODO:
    - add DAC on supervisor board (AUDIO port)
    - add AUDMUTE port - muting all sounds on system
    - fix LCD digits display controlling
    - change lamps and time counter display to use artwork

******************************************************************************/

#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "includes/atari.h"
#include "cpu/m6805/m6805.h"
#include "sound/dac.h"
#include "sound/pokey.h"


/* Supervisor board emulation */

static unsigned char portA_in,portA_out,ddrA;
static unsigned char portB_in,portB_out,ddrB;
static unsigned char portC_in,portC_out,ddrC;
static unsigned char tdr,tcr;
static mame_timer *mcu_timer;
static signed char digitA, digitB, digitC;
static unsigned char lamps;

/* Port A:
    0   (in)  DSW
    1   (in)  DSW
    2   (in)  DSW
    3   (in)  DSW
    4   (in)  coin
    5   (in)  START button
    6   -
    7   (out) AUDIO
*/

READ8_HANDLER( mcu_portA_r )
{
	portA_in = readinputport(5) | (readinputport(4) << 4) | (readinputport(0) << 5);
	return (portA_in & ~ddrA) | (portA_out & ddrA);
}

WRITE8_HANDLER( mcu_portA_w )
{
	portA_out = data;
}

/* Port B:
    0   (out)   Select 7-segment display to control by writing port C
    1   (out)
    2   (out)   clear coin interupt
    3   (out)   STRKEY - line connected to keyboard input in 600XL, seems to be not used
    4   (out)   RES600 - reset 600
    5   (out)   AUDMUTE - mutes audio
    6   (out)   latch for lamps
    7   (out)   TOFF - enables/disables user controls
*/

READ8_HANDLER( mcu_portB_r )
{
	return (portB_in & ~ddrB) | (portB_out & ddrB);
}

WRITE8_HANDLER( mcu_portB_w )
{
	portB_out = data;
	if ( data & 0x4 )
	{
		cpunum_set_input_line( 1, M6805_IRQ_LINE, CLEAR_LINE );
	}
	/* latch for lamps */
	if ( (data & (1<<6)) == 0 )
	{
		lamps = portC_out & 0xf;
	}
	/* RES600 */
	if ( (data & (1<<4)) == 0 )
	{
		cpunum_set_input_line( 0, INPUT_LINE_RESET, ASSERT_LINE );
	}
	else
	{
		cpunum_set_input_line( 0, INPUT_LINE_RESET, CLEAR_LINE );
	}
	if ( (data & 0x3) == 0x3 )
	{
		/*digitA = digitB = digitC = -1;*/
	}
}

/* Port C:
    0   (out)   lamp COIN
    1   (out)   lamp PLAY
    2   (out)   lamp START
    3   (out)   lamp OVER */
READ8_HANDLER( mcu_portC_r )
{
	return (portC_in & ~ddrC) | (portC_out & ddrC);
}

WRITE8_HANDLER( mcu_portC_w )
{
	portC_out = data & 0x0f;

	/* displays */
	switch( portB_out & 0x3 )
	{
		case 0x0: digitC = portC_out; break;
		case 0x1: digitB = portC_out; break;
		case 0x2: digitA = portC_out; break;
		case 0x3: break;
	}

}

READ8_HANDLER( mcu_ddr_r )
{
	return 0xff;
}

WRITE8_HANDLER( mcu_portA_ddr_w )
{
	ddrA = data;
}

WRITE8_HANDLER( mcu_portB_ddr_w )
{
	ddrB = data;
}

WRITE8_HANDLER( mcu_portC_ddr_w )
{
	ddrC = data;
}

void mcu_timer_proc( int param )
{
	if ( --tdr == 0x00 )
	{
		if ( (tcr & 0x40) == 0 )
		{
			//timer interrupt!
			cpunum_set_input_line(1, M68705_INT_TIMER, PULSE_LINE );
		}
	}
}

/* Timer Data Reg */
READ8_HANDLER( mcu_tdr_r )
{
	return tdr;
}

WRITE8_HANDLER( mcu_tdr_w )
{
	tdr = data;
}

/* Timer control reg */
READ8_HANDLER( mcu_tcr_r )
{
	return tcr & ~0x08;
}

WRITE8_HANDLER( mcu_tcr_w )
{
	double period_in_hz;

	tcr = data;
	if ( (tcr & 0x40) == 0 )
	{
		if ( !(tcr & 0x20) )
		{
			/* internal clock / 4*/
			//period_in_hz = 1000000 / 4;
			period_in_hz = 3579545 / 4;
		}
		else
		{
			/* external clock */
			period_in_hz = 3579545;
		}
		if ( tcr & 0x07 )
		{
			/* use prescaler */
			period_in_hz /= (1 << (tcr & 0x7));
		}
		timer_adjust( mcu_timer, TIME_IN_HZ(period_in_hz), 0, TIME_IN_HZ(period_in_hz));
	}
}

static MACHINE_INIT(supervisor_board)
{
	portA_in = portA_out = ddrA	= 0;
	portB_in = portB_out = ddrB	= 0;
	portC_in = portC_out = ddrC	= 0;
	tdr = tcr = 0;
	digitA = digitB = digitC = -1;
	lamps = 0;
	mcu_timer = timer_alloc( mcu_timer_proc );
}

void supervisor_board_check_coin_input(void)
{
	if ( !readinputport(4) )
	{
		cpunum_set_input_line(1, M6805_IRQ_LINE, HOLD_LINE );
	}
}

static MACHINE_INIT(maxaflex)
{
	machine_init_supervisor_board();
	machine_init_a600xl();
}

VIDEO_UPDATE(maxaflex)
{
	static char text[100];

	video_update_atari(bitmap,cliprect);

	if ( digitA != -1 )
	{
		sprintf(text, "%d%d%d", digitA, digitB, digitC );
	}
	else if ( digitB != -1 )
	{
		sprintf(text, " %d%d", digitB, digitC );
	}
	else if ( digitC != -1 )
	{
		sprintf(text, "  %d", digitC );
	}
	ui_text(bitmap, text, 160, 216 );
	if (lamps & 0x01) ui_text(bitmap, "COIN", 20, 216);
	if (lamps & 0x02) ui_text(bitmap, "PLAY", 70, 216);
	if (lamps & 0x04) ui_text(bitmap, "START", 250, 216);
	if (lamps & 0x08) ui_text(bitmap, "OVER", 300, 216);
}

int atari_readinputport(int port)
{
	if ( port <= PORT_JOY_BUTTONS )
	{
		if ( portB_out & 0x80 )
		{
			return readinputport(port);
		}
		else
		{
			/* controls are disabled by mcu */
			return 0xff;
		}
	}
	else
	{
		return readinputport(port);
	}
}

static ADDRESS_MAP_START(a600xl_mem, ADDRESS_SPACE_PROGRAM, 8)
	AM_RANGE(0x0000, 0x3fff) AM_RAM
	AM_RANGE(0x5000, 0x57ff) AM_READWRITE(MRA8_BANK1, MWA8_BANK1)	/* self test */
	AM_RANGE(0x8000, 0xbfff) AM_ROM	/* game cartridge */
	AM_RANGE(0xc000, 0xcfff) AM_ROM /* OS */
	AM_RANGE(0xd000, 0xd0ff) AM_READWRITE(atari_gtia_r, atari_gtia_w)
	AM_RANGE(0xd100, 0xd1ff) AM_NOP
	AM_RANGE(0xd200, 0xd2ff) AM_READWRITE(pokey1_r, pokey1_w)
    AM_RANGE(0xd300, 0xd3ff) AM_READWRITE(atari_pia_r, atari_pia_w)
	AM_RANGE(0xd400, 0xd4ff) AM_READWRITE(atari_antic_r, atari_antic_w)
	AM_RANGE(0xd500, 0xd7ff) AM_NOP
	AM_RANGE(0xd800, 0xffff) AM_ROM /* OS */
ADDRESS_MAP_END

static ADDRESS_MAP_START( mcu_mem, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(11) )
	AM_RANGE(0x0000, 0x0000) AM_READ( mcu_portA_r ) AM_WRITE( mcu_portA_w )
	AM_RANGE(0x0001, 0x0001) AM_READ( mcu_portB_r ) AM_WRITE( mcu_portB_w )
	AM_RANGE(0x0002, 0x0002) AM_READ( mcu_portC_r ) AM_WRITE( mcu_portC_w )
	AM_RANGE(0x0004, 0x0004) AM_READ( mcu_ddr_r ) AM_WRITE( mcu_portA_ddr_w )
	AM_RANGE(0x0005, 0x0005) AM_READ( mcu_ddr_r ) AM_WRITE( mcu_portB_ddr_w )
	AM_RANGE(0x0006, 0x0006) AM_READ( mcu_ddr_r ) AM_WRITE( mcu_portC_ddr_w )
	AM_RANGE(0x0008, 0x0008) AM_READ( mcu_tdr_r ) AM_WRITE( mcu_tdr_w )
	AM_RANGE(0x0009, 0x0009) AM_READ( mcu_tcr_r ) AM_WRITE( mcu_tcr_w )
	AM_RANGE(0x0010, 0x007f) AM_RAM
	AM_RANGE(0x0080, 0x07ff) AM_ROM
ADDRESS_MAP_END


INPUT_PORTS_START( a600xl )

    PORT_START  /* IN0 console keys & switch settings */
	PORT_BIT(0x04, 0x04, IPT_KEYBOARD) PORT_NAME("Option") PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x02, 0x02, IPT_KEYBOARD) PORT_NAME("Select") PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x01, 0x01, IPT_START1 )

	PORT_START	/* IN1 digital joystick #1 + #2 (PIA port A) */
	PORT_BIT(0x01, 0x01, IPT_JOYSTICK_UP) PORT_PLAYER(1)
	PORT_BIT(0x02, 0x02, IPT_JOYSTICK_DOWN) PORT_PLAYER(1)
	PORT_BIT(0x04, 0x04, IPT_JOYSTICK_LEFT) PORT_PLAYER(1)
	PORT_BIT(0x08, 0x08, IPT_JOYSTICK_RIGHT) PORT_PLAYER(1)
	/* player #2 input is not connected */
	PORT_BIT(0x10, 0x10, IPT_JOYSTICK_UP) PORT_PLAYER(2)
	PORT_BIT(0x20, 0x20, IPT_JOYSTICK_DOWN) PORT_PLAYER(2)
	PORT_BIT(0x40, 0x40, IPT_JOYSTICK_LEFT) PORT_PLAYER(2)
	PORT_BIT(0x80, 0x80, IPT_JOYSTICK_RIGHT) PORT_PLAYER(2)

	PORT_START	/* IN2 digital joystick #3 + #4 (PIA port B) */
	/* not connected */
	PORT_BIT(0x01, 0x01, IPT_JOYSTICK_UP) PORT_PLAYER(3)
	PORT_BIT(0x02, 0x02, IPT_JOYSTICK_DOWN) PORT_PLAYER(3)
	PORT_BIT(0x04, 0x04, IPT_JOYSTICK_LEFT) PORT_PLAYER(3)
	PORT_BIT(0x08, 0x08, IPT_JOYSTICK_RIGHT) PORT_PLAYER(3)
	PORT_BIT(0x10, 0x10, IPT_JOYSTICK_UP) PORT_PLAYER(4)
	PORT_BIT(0x20, 0x20, IPT_JOYSTICK_DOWN) PORT_PLAYER(4)
	PORT_BIT(0x40, 0x40, IPT_JOYSTICK_LEFT) PORT_PLAYER(4)
	PORT_BIT(0x80, 0x80, IPT_JOYSTICK_RIGHT) PORT_PLAYER(4)

	PORT_START	/* IN3 digital joystick buttons (GTIA button bits) */
	PORT_BIT(0x01, 0x01, IPT_BUTTON1) PORT_PLAYER(1)
	PORT_BIT(0x02, 0x02, IPT_BUTTON1) PORT_PLAYER(2)
	PORT_BIT(0x04, 0x04, IPT_BUTTON1) PORT_PLAYER(3)
	PORT_BIT(0x08, 0x08, IPT_BUTTON1) PORT_PLAYER(4)
	PORT_BIT(0x10, 0x10, IPT_BUTTON2) PORT_PLAYER(1)
	PORT_BIT(0x20, 0x20, IPT_BUTTON2) PORT_PLAYER(2)
	PORT_BIT(0x40, 0x40, IPT_BUTTON2) PORT_PLAYER(3)
	PORT_BIT(0x80, 0x80, IPT_BUTTON2) PORT_PLAYER(4)

	/* Max-A-Flex specific ports */
	PORT_START	/* IN4 coin */
	PORT_BIT(0x1, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START	/* IN5 DSW */
	PORT_DIPNAME(0xf, 0x0, "Coin/Time" )
	PORT_DIPSETTING( 0x0, "30 sec" )
	PORT_DIPSETTING( 0x1, "60 sec" )
	PORT_DIPSETTING( 0x2, "90 sec" )
	PORT_DIPSETTING( 0x3, "120 sec" )
	PORT_DIPSETTING( 0x4, "150 sec" )
	PORT_DIPSETTING( 0x5, "180 sec" )
	PORT_DIPSETTING( 0x6, "210 sec" )
	PORT_DIPSETTING( 0x7, "240 sec" )
	PORT_DIPSETTING( 0x8, "270 sec" )
	PORT_DIPSETTING( 0x9, "300 sec" )
	PORT_DIPSETTING( 0xa, "330 sec" )
	PORT_DIPSETTING( 0xb, "360 sec" )
	PORT_DIPSETTING( 0xc, "390 sec" )
	PORT_DIPSETTING( 0xd, "420 sec" )
	PORT_DIPSETTING( 0xe, "450 sec" )
	PORT_DIPSETTING( 0xf, "480 sec" )

INPUT_PORTS_END

static UINT8 atari_palette[256*3] =
{
	/* Grey */
    0x00,0x00,0x00, 0x1c,0x1c,0x1c, 0x39,0x39,0x39, 0x59,0x59,0x59,
	0x79,0x79,0x79, 0x92,0x92,0x92, 0xab,0xab,0xab, 0xbc,0xbc,0xbc,
	0xcd,0xcd,0xcd, 0xd9,0xd9,0xd9, 0xe6,0xe6,0xe6, 0xec,0xec,0xec,
	0xf2,0xf2,0xf2, 0xf8,0xf8,0xf8, 0xff,0xff,0xff, 0xff,0xff,0xff,
	/* Gold */
    0x39,0x17,0x01, 0x5e,0x23,0x04, 0x83,0x30,0x08, 0xa5,0x47,0x16,
	0xc8,0x5f,0x24, 0xe3,0x78,0x20, 0xff,0x91,0x1d, 0xff,0xab,0x1d,
	0xff,0xc5,0x1d, 0xff,0xce,0x34, 0xff,0xd8,0x4c, 0xff,0xe6,0x51,
	0xff,0xf4,0x56, 0xff,0xf9,0x77, 0xff,0xff,0x98, 0xff,0xff,0x98,
	/* Orange */
    0x45,0x19,0x04, 0x72,0x1e,0x11, 0x9f,0x24,0x1e, 0xb3,0x3a,0x20,
	0xc8,0x51,0x22, 0xe3,0x69,0x20, 0xff,0x81,0x1e, 0xff,0x8c,0x25,
	0xff,0x98,0x2c, 0xff,0xae,0x38, 0xff,0xc5,0x45, 0xff,0xc5,0x59,
	0xff,0xc6,0x6d, 0xff,0xd5,0x87, 0xff,0xe4,0xa1, 0xff,0xe4,0xa1,
	/* Red-Orange */
    0x4a,0x17,0x04, 0x7e,0x1a,0x0d, 0xb2,0x1d,0x17, 0xc8,0x21,0x19,
	0xdf,0x25,0x1c, 0xec,0x3b,0x38, 0xfa,0x52,0x55, 0xfc,0x61,0x61,
	0xff,0x70,0x6e, 0xff,0x7f,0x7e, 0xff,0x8f,0x8f, 0xff,0x9d,0x9e,
	0xff,0xab,0xad, 0xff,0xb9,0xbd, 0xff,0xc7,0xce, 0xff,0xc7,0xce,
	/* Pink */
    0x05,0x05,0x68, 0x3b,0x13,0x6d, 0x71,0x22,0x72, 0x8b,0x2a,0x8c,
	0xa5,0x32,0xa6, 0xb9,0x38,0xba, 0xcd,0x3e,0xcf, 0xdb,0x47,0xdd,
	0xea,0x51,0xeb, 0xf4,0x5f,0xf5, 0xfe,0x6d,0xff, 0xfe,0x7a,0xfd,
	0xff,0x87,0xfb, 0xff,0x95,0xfd, 0xff,0xa4,0xff, 0xff,0xa4,0xff,
	/* Purple */
    0x28,0x04,0x79, 0x40,0x09,0x84, 0x59,0x0f,0x90, 0x70,0x24,0x9d,
	0x88,0x39,0xaa, 0xa4,0x41,0xc3, 0xc0,0x4a,0xdc, 0xd0,0x54,0xed,
	0xe0,0x5e,0xff, 0xe9,0x6d,0xff, 0xf2,0x7c,0xff, 0xf8,0x8a,0xff,
	0xff,0x98,0xff, 0xfe,0xa1,0xff, 0xfe,0xab,0xff, 0xfe,0xab,0xff,
	/* Purple-Blue */
    0x35,0x08,0x8a, 0x42,0x0a,0xad, 0x50,0x0c,0xd0, 0x64,0x28,0xd0,
	0x79,0x45,0xd0, 0x8d,0x4b,0xd4, 0xa2,0x51,0xd9, 0xb0,0x58,0xec,
	0xbe,0x60,0xff, 0xc5,0x6b,0xff, 0xcc,0x77,0xff, 0xd1,0x83,0xff,
	0xd7,0x90,0xff, 0xdb,0x9d,0xff, 0xdf,0xaa,0xff, 0xdf,0xaa,0xff,
	/* Blue 1 */
    0x05,0x1e,0x81, 0x06,0x26,0xa5, 0x08,0x2f,0xca, 0x26,0x3d,0xd4,
	0x44,0x4c,0xde, 0x4f,0x5a,0xee, 0x5a,0x68,0xff, 0x65,0x75,0xff,
	0x71,0x83,0xff, 0x80,0x91,0xff, 0x90,0xa0,0xff, 0x97,0xa9,0xff,
	0x9f,0xb2,0xff, 0xaf,0xbe,0xff, 0xc0,0xcb,0xff, 0xc0,0xcb,0xff,
	/* Blue 2 */
    0x0c,0x04,0x8b, 0x22,0x18,0xa0, 0x38,0x2d,0xb5, 0x48,0x3e,0xc7,
	0x58,0x4f,0xda, 0x61,0x59,0xec, 0x6b,0x64,0xff, 0x7a,0x74,0xff,
	0x8a,0x84,0xff, 0x91,0x8e,0xff, 0x99,0x98,0xff, 0xa5,0xa3,0xff,
	0xb1,0xae,0xff, 0xb8,0xb8,0xff, 0xc0,0xc2,0xff, 0xc0,0xc2,0xff,
	/* Light-Blue */
    0x1d,0x29,0x5a, 0x1d,0x38,0x76, 0x1d,0x48,0x92, 0x1c,0x5c,0xac,
	0x1c,0x71,0xc6, 0x32,0x86,0xcf, 0x48,0x9b,0xd9, 0x4e,0xa8,0xec,
	0x55,0xb6,0xff, 0x70,0xc7,0xff, 0x8c,0xd8,0xff, 0x93,0xdb,0xff,
	0x9b,0xdf,0xff, 0xaf,0xe4,0xff, 0xc3,0xe9,0xff, 0xc3,0xe9,0xff,
	/* Turquoise */
    0x2f,0x43,0x02, 0x39,0x52,0x02, 0x44,0x61,0x03, 0x41,0x7a,0x12,
	0x3e,0x94,0x21, 0x4a,0x9f,0x2e, 0x57,0xab,0x3b, 0x5c,0xbd,0x55,
	0x61,0xd0,0x70, 0x69,0xe2,0x7a, 0x72,0xf5,0x84, 0x7c,0xfa,0x8d,
	0x87,0xff,0x97, 0x9a,0xff,0xa6, 0xad,0xff,0xb6, 0xad,0xff,0xb6,
	/* Green-Blue */
    0x0a,0x41,0x08, 0x0d,0x54,0x0a, 0x10,0x68,0x0d, 0x13,0x7d,0x0f,
    0x16,0x92,0x12, 0x19,0xa5,0x14, 0x1c,0xb9,0x17, 0x1e,0xc9,0x19,
	0x21,0xd9,0x1b, 0x47,0xe4,0x2d, 0x6e,0xf0,0x40, 0x78,0xf7,0x4d,
	0x83,0xff,0x5b, 0x9a,0xff,0x7a, 0xb2,0xff,0x9a, 0xb2,0xff,0x9a,
	/* Green */
    0x04,0x41,0x0b, 0x05,0x53,0x0e, 0x06,0x66,0x11, 0x07,0x77,0x14,
	0x08,0x88,0x17, 0x09,0x9b,0x1a, 0x0b,0xaf,0x1d, 0x48,0xc4,0x1f,
	0x86,0xd9,0x22, 0x8f,0xe9,0x24, 0x99,0xf9,0x27, 0xa8,0xfc,0x41,
	0xb7,0xff,0x5b, 0xc9,0xff,0x6e, 0xdc,0xff,0x81, 0xdc,0xff,0x81,
	/* Yellow-Green */
    0x02,0x35,0x0f, 0x07,0x3f,0x15, 0x0c,0x4a,0x1c, 0x2d,0x5f,0x1e,
	0x4f,0x74,0x20, 0x59,0x83,0x24, 0x64,0x92,0x28, 0x82,0xa1,0x2e,
	0xa1,0xb0,0x34, 0xa9,0xc1,0x3a, 0xb2,0xd2,0x41, 0xc4,0xd9,0x45,
	0xd6,0xe1,0x49, 0xe4,0xf0,0x4e, 0xf2,0xff,0x53, 0xf2,0xff,0x53,
	/* Orange-Green */
    0x26,0x30,0x01, 0x24,0x38,0x03, 0x23,0x40,0x05, 0x51,0x54,0x1b,
	0x80,0x69,0x31, 0x97,0x81,0x35, 0xaf,0x99,0x3a, 0xc2,0xa7,0x3e,
	0xd5,0xb5,0x43, 0xdb,0xc0,0x3d, 0xe1,0xcb,0x38, 0xe2,0xd8,0x36,
	0xe3,0xe5,0x34, 0xef,0xf2,0x58, 0xfb,0xff,0x7d, 0xfb,0xff,0x7d,
	/* Light-Orange */
    0x40,0x1a,0x02, 0x58,0x1f,0x05, 0x70,0x24,0x08, 0x8d,0x3a,0x13,
	0xab,0x51,0x1f, 0xb5,0x64,0x27, 0xbf,0x77,0x30, 0xd0,0x85,0x3a,
	0xe1,0x93,0x44, 0xed,0xa0,0x4e, 0xf9,0xad,0x58, 0xfc,0xb7,0x5c,
	0xff,0xc1,0x60, 0xff,0xc6,0x71, 0xff,0xcb,0x83, 0xff,0xcb,0x83
};

static unsigned short atari_colortable[] =
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
	0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
	0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
	0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
	0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
	0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};

/* Initialise the palette */
static PALETTE_INIT( atari )
{
	palette_set_colors(0, atari_palette, sizeof(atari_palette) / 3);
	memcpy(colortable,atari_colortable,sizeof(atari_colortable));
}


static struct POKEYinterface pokey_interface = {
	{ 0 },
	0,
	0,0,
	atari_interrupt_cb
};

static MACHINE_DRIVER_START( a600xl )
	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", M6502, FREQ_17_EXACT)
	MDRV_CPU_PROGRAM_MAP(a600xl_mem, 0)
	MDRV_CPU_VBLANK_INT(a800xl_interrupt, TOTAL_LINES_60HZ)
	MDRV_VBLANK_DURATION(0)

	MDRV_CPU_ADD(M68705, 3579545)
	MDRV_CPU_PROGRAM_MAP(mcu_mem,0)
	MDRV_CPU_PERIODIC_INT( supervisor_board_check_coin_input,10 )

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES( VIDEO_TYPE_RASTER )
	MDRV_VISIBLE_AREA(MIN_X, MAX_X, MIN_Y, MAX_Y)
	MDRV_PALETTE_LENGTH(sizeof(atari_palette) / sizeof(atari_palette[0]) / 3)
	MDRV_COLORTABLE_LENGTH(sizeof(atari_colortable) / sizeof(atari_colortable[0]))
	MDRV_PALETTE_INIT(atari)
	MDRV_FRAMES_PER_SECOND(FRAME_RATE_60HZ)
	MDRV_SCREEN_SIZE(HWIDTH*8, TOTAL_LINES_60HZ)

	MDRV_VIDEO_START(atari)
	MDRV_VIDEO_UPDATE(maxaflex)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(POKEY, FREQ_17_EXACT)
	MDRV_SOUND_CONFIG(pokey_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_MACHINE_INIT( maxaflex )
MACHINE_DRIVER_END

ROM_START(maxaflex)
	ROM_REGION(0x10000,REGION_CPU1,0) /* 64K for the CPU */
    ROM_LOAD("atarixl.rom", 0xc000, 0x4000, CRC(1f9cd270) SHA1(ae4f523ba08b6fd59f3cae515a2b2410bbd98f55))

	ROM_REGION( 0x0800, REGION_CPU2, 0 )	/* 2k for the microcontroller */
	ROM_LOAD("maxaflex.uc",  0x0000, 0x0800, CRC(fe9cf53c) SHA1(4b02bc2f0c8a1eab799814fac82d5812c0160206))

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD("maxprom.prm", 0x0000, 0x0200, CRC(edf5c950) SHA1(9ad046ea41a61585dd8d2f2d4167a3cc39d2928f))	/* for simulating keystrokes ?*/
ROM_END

ROM_START(mf_bdash)
	ROM_REGION(0x10000,REGION_CPU1,0) /* 64K for the CPU */
	ROM_LOAD("bd-acs-1.rom",	0x8000, 0x2000, CRC(2b11750e) SHA1(43e9ae44eb1767621920bb94a4370ed602d81056))
	ROM_LOAD("bd-acs-2.rom",	0xa000, 0x2000, CRC(e9ea2658) SHA1(189ede7201ef122cf2b72fc847a896b9dbe007e5))
    ROM_LOAD("atarixl.rom",		0xc000, 0x4000, CRC(1f9cd270) SHA1(ae4f523ba08b6fd59f3cae515a2b2410bbd98f55))

	ROM_REGION( 0x0800, REGION_CPU2, 0 )	/* 2k for the microcontroller */
	ROM_LOAD("maxaflex.uc",  0x0000, 0x0800, CRC(fe9cf53c) SHA1(4b02bc2f0c8a1eab799814fac82d5812c0160206))

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD("maxprom.prm", 0x0000, 0x0200, CRC(edf5c950) SHA1(9ad046ea41a61585dd8d2f2d4167a3cc39d2928f))	/* for simulating keystrokes ?*/
ROM_END

ROM_START(mf_achas)
	ROM_REGION(0x10000,REGION_CPU1,0) /* 64K for the CPU */
	ROM_LOAD("ac.rom",			0x8000, 0x4000, CRC(18752991) SHA1(f508b89d2251c53d017cff6cb23b8e9880a0cc0b))
    ROM_LOAD("atarixl.rom",		0xc000, 0x4000, CRC(1f9cd270) SHA1(ae4f523ba08b6fd59f3cae515a2b2410bbd98f55))

	ROM_REGION( 0x0800, REGION_CPU2, 0 )	/* 2k for the microcontroller */
	ROM_LOAD("maxaflex.uc",  0x0000, 0x0800, CRC(fe9cf53c) SHA1(4b02bc2f0c8a1eab799814fac82d5812c0160206))

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD("maxprom.prm", 0x0000, 0x0200, CRC(edf5c950) SHA1(9ad046ea41a61585dd8d2f2d4167a3cc39d2928f))	/* for simulating keystrokes ?*/
ROM_END

ROM_START(mf_brist)
	ROM_REGION(0x10000,REGION_CPU1,0) /* 64K for the CPU */
	ROM_LOAD("brist.rom",		0x8000, 0x4000, CRC(4263d64d) SHA1(80a041bceb499e1466516488013aa4439b3db6f2))
    ROM_LOAD("atarixl.rom",		0xc000, 0x4000, CRC(1f9cd270) SHA1(ae4f523ba08b6fd59f3cae515a2b2410bbd98f55))

	ROM_REGION( 0x0800, REGION_CPU2, 0 )	/* 2k for the microcontroller */
	ROM_LOAD("maxaflex.uc",  0x0000, 0x0800, CRC(fe9cf53c) SHA1(4b02bc2f0c8a1eab799814fac82d5812c0160206))

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD("maxprom.prm", 0x0000, 0x0200, CRC(edf5c950) SHA1(9ad046ea41a61585dd8d2f2d4167a3cc39d2928f))	/* for simulating keystrokes ?*/
ROM_END

ROM_START(mf_flip)
	ROM_REGION(0x10000,REGION_CPU1,0) /* 64K for the CPU */
	ROM_LOAD("flipflop.rom",	0x8000, 0x4000, CRC(8ae057be) SHA1(ba26d6a3790ebdb754c1192b2c28f0fe93aca377))
    ROM_LOAD("atarixl.rom",		0xc000, 0x4000, CRC(1f9cd270) SHA1(ae4f523ba08b6fd59f3cae515a2b2410bbd98f55))

	ROM_REGION( 0x0800, REGION_CPU2, 0 )	/* 2k for the microcontroller */
	ROM_LOAD("maxaflex.uc",  0x0000, 0x0800, CRC(fe9cf53c) SHA1(4b02bc2f0c8a1eab799814fac82d5812c0160206))

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD("maxprom.prm", 0x0000, 0x0200, CRC(edf5c950) SHA1(9ad046ea41a61585dd8d2f2d4167a3cc39d2928f))	/* for simulating keystrokes ?*/
ROM_END

DRIVER_INIT( a600xl )
{
	memcpy( memory_region(REGION_CPU1) + 0x5000, memory_region(REGION_CPU1) + 0xd000, 0x800 );
}

GAMEX( 1984, maxaflex, 0,		  a600xl, a600xl, a600xl, ROT0, "Exidy", "Max-A-Flex", NOT_A_DRIVER )
GAME ( 1982, mf_achas, maxaflex,  a600xl, a600xl, a600xl, ROT0, "Exidy / First Star Software", "Astro Chase (Max-A-Flex)" )
GAME ( 1983, mf_brist, maxaflex,  a600xl, a600xl, a600xl, ROT0, "Exidy / First Star Software", "Bristles (Max-A-Flex)" )
GAME ( 1983, mf_flip,  maxaflex,  a600xl, a600xl, a600xl, ROT0, "Exidy / First Star Software", "Flip & Flop (Max-A-Flex)" )
GAME ( 1984, mf_bdash, maxaflex,  a600xl, a600xl, a600xl, ROT0, "Exidy / First Star Software", "Boulder Dash (Max-A-Flex)" )

