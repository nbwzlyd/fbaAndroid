// FB Alpha Gyruss driver module
// Based on MAME driver by Mirko Buffoni, Michael Cuddy, and Nicola Salmoria
//
// Massive overhaul by dink on Feb. 9, 2015

#include "tiles_generic.h"
#include "z80_intf.h"
#include "m6809_intf.h"
#include "i8039.h"
#include "driver.h"
#include "flt_rc.h"
#include "dac.h"
extern "C" {
#include "ay8910.h"
}

static UINT8 *AllMem;
static UINT8 *MemEnd;
static UINT8 *AllRam;
static UINT8 *RamEnd;
static UINT8 *DrvZ80ROM0;
static UINT8 *DrvZ80ROM1;
static UINT8 *DrvM6809ROM;
static UINT8 *DrvM6809DecROM;
static UINT8 *DrvI8039ROM;
static UINT8 *DrvGfxROM0;
static UINT8 *DrvGfxROM1;
static UINT8 *DrvGfxROM2;
static UINT8 *DrvColPROM;
static UINT8 *DrvSprRAM;
static UINT8 *DrvColRAM;
static UINT8 *DrvVidRAM;
static UINT8 *DrvZ80RAM0;
static UINT8 *DrvZ80RAM1;
static UINT8 *DrvShareRAM;
static UINT8 *DrvM6809RAM;
static UINT32 *DrvPalette;
static UINT32 *Palette;
static UINT8 DrvRecalc;

static INT16 *pAY8910Buffer[15];

static UINT8 DrvJoy1[8];
static UINT8 DrvJoy2[8];
static UINT8 DrvJoy3[8];
static UINT8 DrvDips[3];
static UINT8 DrvInputs[3];
static UINT8 DrvReset;

static UINT8 *soundlatch;
static UINT8 *soundlatch2;
static UINT8 *flipscreen;
static UINT8 *interrupt_enable0;
static UINT8 *interrupt_enable1;

static INT32 scanline;

static struct BurnInputInfo GyrussInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 coin"	},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy1 + 3,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 left"	},
	{"P1 Right",	BIT_DIGITAL,	DrvJoy2 + 1,	"p1 right"	},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy2 + 4,	"p1 fire 1"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 coin"	},
	{"P2 Start",	BIT_DIGITAL,	DrvJoy1 + 4,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy3 + 2,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy3 + 0,	"p2 left"	},
	{"P2 Right",	BIT_DIGITAL,	DrvJoy3 + 1,	"p2 right"	},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy3 + 4,	"p2 fire 1"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	    "reset"		},
	{"Service",		BIT_DIGITAL,	DrvJoy1 + 2,	"service"	},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
	{"Dip C",		BIT_DIPSWITCH,	DrvDips + 2,	"dip"		},
};

STDINPUTINFO(Gyruss)

static struct BurnDIPInfo GyrussDIPList[]=
{
	// Default Values
	{0x10, 0xff, 0xff, 0xff, NULL				},
	{0x11, 0xff, 0xff, 0x3b, NULL				},
	{0x12, 0xff, 0xff, 0x00, NULL				},

	{0   , 0xfe, 0   ,    16, "Coin A"			},
	{0x10, 0x01, 0x0f, 0x02, "4 Coins 1 Credits "		},
	{0x10, 0x01, 0x0f, 0x05, "3 Coins 1 Credits "		},
	{0x10, 0x01, 0x0f, 0x08, "2 Coins 1 Credits "		},
	{0x10, 0x01, 0x0f, 0x04, "3 Coins 2 Credits "		},
	{0x10, 0x01, 0x0f, 0x01, "4 Coins 3 Credits "		},
	{0x10, 0x01, 0x0f, 0x0f, "1 Coin  1 Credits "		},
	{0x10, 0x01, 0x0f, 0x03, "3 Coins 4 Credits "		},
	{0x10, 0x01, 0x0f, 0x07, "2 Coins 3 Credits "		},
	{0x10, 0x01, 0x0f, 0x0e, "1 Coin  2 Credits "		},
	{0x10, 0x01, 0x0f, 0x06, "2 Coins 5 Credits "		},
	{0x10, 0x01, 0x0f, 0x0d, "1 Coin  3 Credits "		},
	{0x10, 0x01, 0x0f, 0x0c, "1 Coin  4 Credits "		},
	{0x10, 0x01, 0x0f, 0x0b, "1 Coin  5 Credits "		},
	{0x10, 0x01, 0x0f, 0x0a, "1 Coin  6 Credits "		},
	{0x10, 0x01, 0x0f, 0x09, "1 Coin  7 Credits "		},
	{0x10, 0x01, 0x0f, 0x00, "Free Play"			},

	{0   , 0xfe, 0   ,    16, "Coin B"			},
	{0x10, 0x01, 0xf0, 0x20, "4 Coins 1 Credits "		},
	{0x10, 0x01, 0xf0, 0x50, "3 Coins 1 Credits "		},
	{0x10, 0x01, 0xf0, 0x80, "2 Coins 1 Credits "		},
	{0x10, 0x01, 0xf0, 0x40, "3 Coins 2 Credits "		},
	{0x10, 0x01, 0xf0, 0x10, "4 Coins 3 Credits "		},
	{0x10, 0x01, 0xf0, 0xf0, "1 Coin  1 Credits "		},
	{0x10, 0x01, 0xf0, 0x30, "3 Coins 4 Credits "		},
	{0x10, 0x01, 0xf0, 0x70, "2 Coins 3 Credits "		},
	{0x10, 0x01, 0xf0, 0xe0, "1 Coin  2 Credits "		},
	{0x10, 0x01, 0xf0, 0x60, "2 Coins 5 Credits "		},
	{0x10, 0x01, 0xf0, 0xd0, "1 Coin  3 Credits "		},
	{0x10, 0x01, 0xf0, 0xc0, "1 Coin  4 Credits "		},
	{0x10, 0x01, 0xf0, 0xb0, "1 Coin  5 Credits "		},
	{0x10, 0x01, 0xf0, 0xa0, "1 Coin  6 Credits "		},
	{0x10, 0x01, 0xf0, 0x90, "1 Coin  7 Credits "		},
	{0x10, 0x01, 0xf0, 0x00, "Free Play"			},

	{0   , 0xfe, 0   ,    4, "Lives"			},
	{0x11, 0x01, 0x03, 0x03, "3"				},
	{0x11, 0x01, 0x03, 0x02, "4"				},
	{0x11, 0x01, 0x03, 0x01, "5"				},
	{0x11, 0x01, 0x03, 0x00, "255 (Cheat)"			},

	{0   , 0xfe, 0   ,    2, "Cabinet"			},
	{0x11, 0x01, 0x04, 0x00, "Upright"			},
	{0x11, 0x01, 0x04, 0x04, "Cocktail"			},

	{0   , 0xfe, 0   ,    2, "Bonus Life"			},
	{0x11, 0x01, 0x08, 0x08, "30k 90k 60k+"			},
	{0x11, 0x01, 0x08, 0x00, "40k 110k 70k+"		},

	{0   , 0xfe, 0   ,    8, "Difficulty"			},
	{0x11, 0x01, 0x70, 0x70, "1 (Easiest)"			},
	{0x11, 0x01, 0x70, 0x60, "2"				},
	{0x11, 0x01, 0x70, 0x50, "3"				},
	{0x11, 0x01, 0x70, 0x40, "4"				},
	{0x11, 0x01, 0x70, 0x30, "5 (Average)"			},
	{0x11, 0x01, 0x70, 0x20, "6"				},
	{0x11, 0x01, 0x70, 0x10, "7"				},
	{0x11, 0x01, 0x70, 0x00, "8 (Hardest)"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x11, 0x01, 0x80, 0x80, "Off"				},
	{0x11, 0x01, 0x80, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Demo Music"			},
	{0x12, 0x01, 0x01, 0x01, "Off"				},
	{0x12, 0x01, 0x01, 0x00, "On"				},
};

STDDIPINFO(Gyruss)

static struct BurnDIPInfo GyrussceDIPList[]=
{
	{0x10, 0xff, 0xff, 0xff, NULL				},
	{0x11, 0xff, 0xff, 0x3b, NULL				},
	{0x12, 0xff, 0xff, 0x20, NULL				},

	{0   , 0xfe, 0   ,    16, "Coin A"			},
	{0x10, 0x01, 0x0f, 0x02, "4 Coins 1 Credits "		},
	{0x10, 0x01, 0x0f, 0x05, "3 Coins 1 Credits "		},
	{0x10, 0x01, 0x0f, 0x08, "2 Coins 1 Credits "		},
	{0x10, 0x01, 0x0f, 0x04, "3 Coins 2 Credits "		},
	{0x10, 0x01, 0x0f, 0x01, "4 Coins 3 Credits "		},
	{0x10, 0x01, 0x0f, 0x0f, "1 Coin  1 Credits "		},
	{0x10, 0x01, 0x0f, 0x03, "3 Coins 4 Credits "		},
	{0x10, 0x01, 0x0f, 0x07, "2 Coins 3 Credits "		},
	{0x10, 0x01, 0x0f, 0x0e, "1 Coin  2 Credits "		},
	{0x10, 0x01, 0x0f, 0x06, "2 Coins 5 Credits "		},
	{0x10, 0x01, 0x0f, 0x0d, "1 Coin  3 Credits "		},
	{0x10, 0x01, 0x0f, 0x0c, "1 Coin  4 Credits "		},
	{0x10, 0x01, 0x0f, 0x0b, "1 Coin  5 Credits "		},
	{0x10, 0x01, 0x0f, 0x0a, "1 Coin  6 Credits "		},
	{0x10, 0x01, 0x0f, 0x09, "1 Coin  7 Credits "		},
	{0x10, 0x01, 0x0f, 0x00, "Free Play"			},

	{0   , 0xfe, 0   ,    16, "Coin B"			},
	{0x10, 0x01, 0xf0, 0x20, "4 Coins 1 Credits "		},
	{0x10, 0x01, 0xf0, 0x50, "3 Coins 1 Credits "		},
	{0x10, 0x01, 0xf0, 0x80, "2 Coins 1 Credits "		},
	{0x10, 0x01, 0xf0, 0x40, "3 Coins 2 Credits "		},
	{0x10, 0x01, 0xf0, 0x10, "4 Coins 3 Credits "		},
	{0x10, 0x01, 0xf0, 0xf0, "1 Coin  1 Credits "		},
	{0x10, 0x01, 0xf0, 0x30, "3 Coins 4 Credits "		},
	{0x10, 0x01, 0xf0, 0x70, "2 Coins 3 Credits "		},
	{0x10, 0x01, 0xf0, 0xe0, "1 Coin  2 Credits "		},
	{0x10, 0x01, 0xf0, 0x60, "2 Coins 5 Credits "		},
	{0x10, 0x01, 0xf0, 0xd0, "1 Coin  3 Credits "		},
	{0x10, 0x01, 0xf0, 0xc0, "1 Coin  4 Credits "		},
	{0x10, 0x01, 0xf0, 0xb0, "1 Coin  5 Credits "		},
	{0x10, 0x01, 0xf0, 0xa0, "1 Coin  6 Credits "		},
	{0x10, 0x01, 0xf0, 0x90, "1 Coin  7 Credits "		},
	{0x10, 0x01, 0xf0, 0x00, "Free Play"			},

	{0   , 0xfe, 0   ,    4, "Lives"			},
	{0x11, 0x01, 0x03, 0x03, "3"				},
	{0x11, 0x01, 0x03, 0x02, "4"				},
	{0x11, 0x01, 0x03, 0x01, "5"				},
	{0x11, 0x01, 0x03, 0x00, "255 (Cheat)"			},

	{0   , 0xfe, 0   ,    2, "Cabinet"			},
	{0x11, 0x01, 0x04, 0x00, "Upright"			},
	{0x11, 0x01, 0x04, 0x04, "Cocktail"			},

	{0   , 0xfe, 0   ,    0, "Bonus Life"			},

	{0   , 0xfe, 0   ,    2, "Bonus Life"			},
	{0x11, 0x01, 0x08, 0x08, "50k 120k 70k+"		},
	{0x11, 0x01, 0x08, 0x00, "60k 140k 80k+"		},

	{0   , 0xfe, 0   ,    8, "Difficulty"			},
	{0x11, 0x01, 0x70, 0x70, "1 (Easiest)"			},
	{0x11, 0x01, 0x70, 0x60, "2"				},
	{0x11, 0x01, 0x70, 0x50, "3"				},
	{0x11, 0x01, 0x70, 0x40, "4"				},
	{0x11, 0x01, 0x70, 0x30, "5 (Average)"			},
	{0x11, 0x01, 0x70, 0x20, "6"				},
	{0x11, 0x01, 0x70, 0x10, "7"				},
	{0x11, 0x01, 0x70, 0x00, "8 (Hardest)"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x11, 0x01, 0x80, 0x80, "Off"				},
	{0x11, 0x01, 0x80, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Demo Music"			},
	{0x12, 0x01, 0x01, 0x01, "Off"				},
	{0x12, 0x01, 0x01, 0x00, "On"				},
};

STDDIPINFO(Gyrussce)

void __fastcall gyruss_main_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0xc000: // watchdog
		return;

		case 0xc080:
			ZetClose();
			ZetOpen(1);
			ZetSetVector(0xff);
			ZetSetIRQLine(0, CPU_IRQSTATUS_ACK);
			ZetClose();
			ZetOpen(0);
		return;

		case 0xc100:
			*soundlatch = data;
		return;

		case 0xc180:
			*interrupt_enable0 = data & 1;
			if (!*interrupt_enable0) {
				ZetSetIRQLine(Z80_INPUT_LINE_NMI, CPU_IRQSTATUS_NONE);
			}
		return;

		case 0xc185:
			*flipscreen = data & 1;
		return;
	}
}

UINT8 __fastcall gyruss_main_read(UINT16 address)
{
	switch (address)
	{
		case 0xc000:
			return DrvDips[1];

		case 0xc080:
			return DrvInputs[0];

		case 0xc0a0:
			return DrvInputs[1];

		case 0xc0c0:
			return DrvInputs[2];

		case 0xc0e0:
			return DrvDips[0];

		case 0xc100:
			return DrvDips[2];
	}

	return 0;
}

void gyruss_sub_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0x2000:
			*interrupt_enable1 = data & 1;
			if (!*interrupt_enable1) {
				M6809SetIRQLine(0, CPU_IRQSTATUS_NONE);
			}
		return;
	}
}

UINT8 gyruss_sub_read(UINT16 address)
{
	switch (address)
	{
		case 0x0000:
			return (scanline/* & 0x7f*/);
	}

	return 0;
}

UINT8 __fastcall gyruss_sound0_read(UINT16 address)
{
	switch (address)
	{
		case 0x8000:
			ZetSetIRQLine(0, CPU_IRQSTATUS_NONE);
			return *soundlatch;
	}

	return 0;
}

static void filter_write(INT32 num, UINT8 d)
{
	INT32 C = 0;
	if (d & 2) C += 220000;	/* 220000pF = 0.220uF */
	if (d & 1) C +=  47000;	/*  47000pF = 0.047uF */

	filter_rc_set_RC(num, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(C));
}

void AY8910_0_portBwrite(UINT32 /*port*/, UINT32 data)
{
	if (ZetGetActive() == -1) return;

	for (INT32 i = 0; i < 3; i++)
	{
		/* low bit: 47000pF = 0.047uF */
		/* high bit: 220000pF = 0.22uF */

		filter_write(i, data & 3);
		data >>= 2;
	}
}

void AY8910_1_portBwrite(UINT32 /*port*/, UINT32 data)
{
	if (ZetGetActive() == -1) return;

	for (INT32 i = 0; i < 3; i++)
	{
		/* low bit: 47000pF = 0.047uF */
		/* high bit: 220000pF = 0.22uF */

		filter_write(i + 3, data & 3);
		data >>= 2;
	}
}

UINT8 __fastcall gyruss_sound0_in(UINT16 port)
{
	switch (port & 0xff)
	{
		case 0x01:
			return AY8910Read(0);

		case 0x05:
			return AY8910Read(1);

		case 0x09:
			return AY8910Read(2);

		case 0x0d:
			return AY8910Read(3);

		case 0x11:
			return AY8910Read(4);
	}

	return 0;
}

void __fastcall gyruss_sound0_out(UINT16 port, UINT8 data)
{
	switch (port & 0xff)
	{
		case 0x00:
			AY8910Write(0, 0, data);
		return;

		case 0x02:
			AY8910Write(0, 1, data);
		return;

	    case 0x04:
			AY8910Write(1, 0, data);
		return;

		case 0x06:
			AY8910Write(1, 1, data);
		return;

		case 0x08:
			AY8910Write(2, 0, data);
		return;

		case 0x0a:
			AY8910Write(2, 1, data);
		return;

		case 0x0c:
			AY8910Write(3, 0, data);
		return;

		case 0x0e:
			AY8910Write(3, 1, data);
		return;

		case 0x10:
			AY8910Write(4, 0, data);
		return;

		case 0x12:
			AY8910Write(4, 1, data);
		return;

		case 0x14:
			I8039SetIrqState(1);
		return;

		case 0x18:
			*soundlatch2 = data;
		return;
	}
}

UINT8 AY8910_3_portA(UINT32)
{
	static const INT32 gyruss_timer[10] =
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x09, 0x0a, 0x0b, 0x0a, 0x0d
	};

	return gyruss_timer[(ZetTotalCycles() / 1024) % 10];
}

static UINT8 __fastcall gyruss_i8039_read(UINT32 address)
{
	return DrvI8039ROM[address & 0x0fff];
}

static void __fastcall gyruss_i8039_write_port(UINT32 port, UINT8 data)
{
	switch (port & 0x1ff)
	{
		case I8039_p1:
			DACWrite(0, data);
		return;

		case I8039_p2:
			I8039SetIrqState(0);
		return;
	}
}

static UINT8 __fastcall gyruss_i8039_read_port(UINT32 port)
{
	if ((port & 0x1ff) < 0x100) {
		return *soundlatch2;
	}

	return 0;
}


static INT32 DrvSyncDAC()
{
	return (INT32)(float)(nBurnSoundLen * (ZetTotalCycles() / (3579545.0000 / (nBurnFPS / 100.0000))));
}


static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvZ80ROM0		= Next; Next += 0x010000;
	DrvZ80ROM1		= Next; Next += 0x010000;

	DrvM6809ROM		= Next; Next += 0x010000;
	DrvM6809DecROM	= Next; Next += 0x010000;

	DrvI8039ROM		= Next; Next += 0x001000;

	DrvGfxROM0		= Next; Next += 0x010000;
	DrvGfxROM1		= Next; Next += 0x010000;
	DrvGfxROM2		= Next; Next += 0x008000;

	DrvColPROM		= Next; Next += 0x000300;

	Palette			= (UINT32*)Next; Next += 0x0140 * sizeof(UINT32);
	DrvPalette		= (UINT32*)Next; Next += 0x0140 * sizeof(UINT32);

	pAY8910Buffer[ 0]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 1]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 2]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 3]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 4]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 5]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 6]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 7]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 8]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[ 9]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[10]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[11]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[12]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[13]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[14]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);

	AllRam			= Next;

	flipscreen		= Next; Next += 0x000001;
	soundlatch		= Next; Next += 0x000001;
	soundlatch2		= Next; Next += 0x000001;

	interrupt_enable0 	= Next; Next += 0x000001;
	interrupt_enable1	= Next; Next += 0x000001;

	DrvShareRAM		= Next; Next += 0x000800;
	DrvColRAM		= Next; Next += 0x000400;
	DrvVidRAM		= Next; Next += 0x000400;
	DrvZ80RAM0		= Next; Next += 0x001000;
	DrvZ80RAM1		= Next; Next += 0x000400;

	DrvM6809RAM		= Next;	Next += 0x000800;

	DrvSprRAM		= DrvM6809RAM + 0x000040;

	RamEnd			= Next;

	MemEnd			= Next;

	return 0;
}

static INT32 DrvGfxDecode()
{
	INT32 Plane[4] = { 0x20004, 0x20000, 4, 0 };
	INT32 XOffs[8] = { STEP4(0,1), STEP4(64, 1) };
	INT32 YOffs[16] = { STEP8(0, 8), STEP8(256, 8) };

	UINT8 *tmp = (UINT8*)BurnMalloc(0x10000);
	if (tmp == NULL) {
		return 1;
	}

	memcpy (tmp, DrvGfxROM0, 0x8000);

	GfxDecode(0x0200, 4, 8, 16, Plane    , XOffs, YOffs, 0x200, tmp       , DrvGfxROM0);
	GfxDecode(0x0200, 4, 8, 16, Plane    , XOffs, YOffs, 0x200, tmp + 0x10, DrvGfxROM1);

	memcpy (tmp, DrvGfxROM2, 0x2000);

	GfxDecode(0x0200, 2,  8,  8, Plane + 2, XOffs, YOffs, 0x080, tmp      , DrvGfxROM2);

	BurnFree (tmp);

	return 0;
}

static INT32 DrvPaletteInit()
{
	UINT8 *color_prom = DrvColPROM;
	UINT32 *tmp = (UINT32*)BurnMalloc(32 * sizeof(UINT32));
	if (tmp == NULL) {
		return 1;
	}
	
	for (INT32 i = 0; i < 0x20; i++)
	{
		INT32 bit0, bit1, bit2;
		UINT8 r, g, b;

		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = (INT32)(double((bit0 * 33 + bit1 * 70 + bit2 * 151)+0.5));

		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = (INT32)(double((bit0 * 33 + bit1 * 70 + bit2 * 151)+0.5));

		bit0 = (color_prom[i] >> 6) & 0x01;
		bit1 = (color_prom[i] >> 7) & 0x01;
		b = (INT32)(double((bit0 * 78 + bit1 * 151)+0.5));

		tmp[i] = (r << 16) | (g << 8) | b;
	}

	color_prom += 32;

	for (INT32 i = 0; i < 0x140; i++) {
		UINT8 ctabentry = color_prom[i] & 0x0f;
		Palette[i] = tmp[ctabentry | ((i >> 4) & 0x10)];
	}

	BurnFree (tmp);
	return 0;
}

static void gyrussDecode()
{
	for (INT32 i = 0xe000; i < 0x10000; i++)
	{
		UINT8 xor1 = 0;

		if ( i & 0x02) xor1 |= 0x80;
		else           xor1 |= 0x20;
		if ( i & 0x08) xor1 |= 0x08;
		else           xor1 |= 0x02;

		DrvM6809DecROM[i] = DrvM6809ROM[i] ^ xor1;
	}
}

static INT32 DrvDoReset()
{
	DrvReset = 0;

	memset (AllRam, 0, RamEnd - AllRam);

	ZetOpen(0);
	ZetReset();
	ZetClose();

	M6809Open(0);
	M6809Reset();
	M6809Close();

	ZetOpen(1);
	ZetReset();
	ZetClose();

	I8039Reset();

	DACReset();

	AY8910Reset(0);
	AY8910Reset(1);
	AY8910Reset(2);
	AY8910Reset(3);
	AY8910Reset(4);

	return 0;
}

static INT32 DrvInit()
{
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x0000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x2000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x4000,  2, 1)) return 1;

		if (BurnLoadRom(DrvM6809ROM + 0xe000, 3, 1)) return 1;

		if (BurnLoadRom(DrvZ80ROM1 + 0x0000,  4, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM1 + 0x2000,  5, 1)) return 1;

		if (BurnLoadRom(DrvI8039ROM + 0x0000, 6, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM0 + 0x0000,  7, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM0 + 0x2000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM0 + 0x4000,  9, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM0 + 0x6000, 10, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM2 + 0x0000, 11, 1)) return 1;

		if (BurnLoadRom(DrvColPROM + 0x0000, 12, 1)) return 1;
		if (BurnLoadRom(DrvColPROM + 0x0020, 13, 1)) return 1;
		if (BurnLoadRom(DrvColPROM + 0x0120, 14, 1)) return 1;

		gyrussDecode();
		DrvGfxDecode();
		DrvPaletteInit();
	}

	ZetInit(0);
	ZetOpen(0);
	ZetMapMemory(DrvZ80ROM0, 0x0000, 0x7fff, MAP_ROM);
	ZetMapMemory(DrvColRAM, 0x8000, 0x83ff, MAP_RAM);
	ZetMapMemory(DrvVidRAM, 0x8400, 0x87ff, MAP_RAM);
	ZetMapMemory(DrvZ80RAM0, 0x9000, 0x9fff, MAP_RAM);
	ZetMapMemory(DrvShareRAM, 0xa000, 0xa7ff, MAP_RAM);

	ZetSetReadHandler(gyruss_main_read);
	ZetSetWriteHandler(gyruss_main_write);
	ZetClose();

	M6809Init(1);
	M6809Open(0);
	M6809MapMemory(DrvM6809RAM, 0x4000, 0x47ff, MAP_RAM);
	M6809MapMemory(DrvShareRAM,	0x6000, 0x67ff, MAP_RAM);
	M6809MapMemory(DrvM6809ROM + 0xe000, 0xe000, 0xffff, MAP_READ);
	M6809MapMemory(DrvM6809DecROM + 0xe000,	0xe000, 0xffff, MAP_FETCH);
	M6809SetReadHandler(gyruss_sub_read);
	M6809SetWriteHandler(gyruss_sub_write);
	M6809Close();

	ZetInit(1);
	ZetOpen(1);
	ZetMapMemory(DrvZ80ROM1, 0x0000, 0x5fff, MAP_ROM);
	ZetMapMemory(DrvZ80RAM1, 0x6000, 0x63ff, MAP_RAM);
	ZetSetReadHandler(gyruss_sound0_read);
	ZetSetOutHandler(gyruss_sound0_out);
	ZetSetInHandler(gyruss_sound0_in);
	ZetClose();

	I8039Init(NULL);
	I8039SetProgramReadHandler(gyruss_i8039_read);
	I8039SetCPUOpReadHandler(gyruss_i8039_read);
	I8039SetCPUOpReadArgHandler(gyruss_i8039_read);
	I8039SetIOReadHandler(gyruss_i8039_read_port);
	I8039SetIOWriteHandler(gyruss_i8039_write_port);

	DACInit(0, 0, 1, DrvSyncDAC);
	DACSetRoute(0, 0.15, BURN_SND_ROUTE_BOTH);

	AY8910Init(0, 1789750, nBurnSoundRate, NULL, NULL, NULL, &AY8910_0_portBwrite);
	AY8910Init(1, 1789750, nBurnSoundRate, NULL, NULL, NULL, &AY8910_1_portBwrite);
	AY8910Init(2, 1789750, nBurnSoundRate, AY8910_3_portA, NULL, NULL, NULL);
	AY8910Init(3, 1789750, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(4, 1789750, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910SetAllRoutes(0, 0.25, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(1, 0.25, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(2, 0.25, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(3, 0.25, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(4, 0.25, BURN_SND_ROUTE_BOTH);

	filter_rc_init(0, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(0), 0);
	filter_rc_init(1, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(0), 1);
	filter_rc_init(2, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(0), 1);
	filter_rc_init(3, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(0), 1);
	filter_rc_init(4, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(0), 1);
	filter_rc_init(5, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(0), 1);
	filter_rc_init(6, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(0), 1); // master out l
	filter_rc_init(7, FLT_RC_LOWPASS, 1000, 5100, 0, CAP_P(0), 1); // master out r

	filter_rc_set_src_gain(0, 0.70);
	filter_rc_set_src_gain(1, 0.60);
	filter_rc_set_src_gain(2, 0.45);
	filter_rc_set_src_gain(3, 0.70);
	filter_rc_set_src_gain(4, 0.60);
	filter_rc_set_src_gain(5, 0.45);
	filter_rc_set_src_gain(6, 1.00);
	filter_rc_set_src_gain(7, 1.00);

	filter_rc_set_route(0, 0.25, FLT_RC_PANNEDLEFT);
	filter_rc_set_route(1, 0.25, FLT_RC_PANNEDLEFT);
	filter_rc_set_route(2, 0.25, FLT_RC_PANNEDLEFT);
	filter_rc_set_route(3, 0.25, FLT_RC_PANNEDRIGHT);
	filter_rc_set_route(4, 0.25, FLT_RC_PANNEDRIGHT);
	filter_rc_set_route(5, 0.25, FLT_RC_PANNEDRIGHT);
	filter_rc_set_route(6, 0.25, BURN_SND_ROUTE_LEFT ); // master out l
	filter_rc_set_route(7, 0.25, BURN_SND_ROUTE_RIGHT); // master out r

	GenericTilesInit();

	DrvDoReset();

	return 0;
}

static INT32 DrvExit()
{
	GenericTilesExit();
	ZetExit();
	M6809Exit();

	I8039Exit();

	DACExit();

	AY8910Exit(0);
	AY8910Exit(1);
	AY8910Exit(2);
	AY8910Exit(3);
	AY8910Exit(4);
	filter_rc_exit();

	BurnFree (AllMem);

	return 0;
}

static void draw_background(INT32 transp)
{
	for (INT32 offs = 0x40; offs < 0x3c0; offs++)
	{
		INT32 sx    = (offs & 0x1f) << 3;
		INT32 sy    = (offs >> 5) << 3;

		INT32 attr  = DrvColRAM[offs];
		INT32 code  = ((attr & 0x20) << 3) | DrvVidRAM[offs];
		INT32 color =   attr & 0x0f;
		INT32 flipx =  (attr >> 6) & 1;
		INT32 flipy =   attr >> 7;

		INT32 group = ((~DrvColRAM[offs] >> 4) & 1);
		if (transp && group == 0) continue;

		if (*flipscreen) {
			flipx ^= 1;
			flipy ^= 1;
			sx ^= 0xf8;
			sy ^= 0xf8;
		}

		sy -= 16;

		if (flipy) {
			if (flipx) {
				Render8x8Tile_Mask_FlipXY(pTransDraw, code, sx, sy, color, 2, transp, 0x100, DrvGfxROM2);
			} else {
				Render8x8Tile_Mask_FlipY(pTransDraw, code, sx, sy, color, 2, transp, 0x100, DrvGfxROM2);
			}
		} else {
			if (flipx) {
				Render8x8Tile_Mask_FlipX(pTransDraw, code, sx, sy, color, 2, transp, 0x100, DrvGfxROM2);
			} else {
				Render8x8Tile_Mask(pTransDraw, code, sx, sy, color, 2, transp, 0x100, DrvGfxROM2);
			}
		}
	}
}

static void draw_8x16_tile_line(INT32 sx, INT32 sy, INT32 color, UINT8 *gfx_base, INT32 code, INT32 flipx, INT32 flipy, INT32 line)
{
	line -= 16;

	if (line < 0 || line >= nScreenHeight) return;

	INT32 y = line - sy; // right?
	if (y < 0 || y >= 16) return;

	if (flipy) flipy = 0x78;

	UINT8 *src = gfx_base + (code * 0x80) + ((y << 3) ^ flipy);
	color <<= 4;

	if (flipx) flipx = 0x07;

	for (INT32 x = 0; x < 8; x++, sx++)
	{
		INT32 pxl = src[x ^ flipx];

		if (sx < 8 || sx >= (nScreenWidth - 8) || !pxl) continue;

		pTransDraw[(line * nScreenWidth) + sx] = pxl | color;
	}
}

static void draw_sprites(INT32 line)
{
	for (INT32 offs = 0xbc; offs >= 0; offs -= 4)
	{
		INT32 sx = DrvSprRAM[offs];
		INT32 sy = (241 - DrvSprRAM[offs + 3]);

		if (sy <= (line-16) || sy >= (line+16)) continue;

		INT32 bank =   DrvSprRAM[offs + 1] & 0x01;
		INT32 code = ((DrvSprRAM[offs + 2] & 0x20) << 2) | (DrvSprRAM[offs + 1] >> 1);
		INT32 color =  DrvSprRAM[offs + 2] & 0x0f;
		INT32 flipx = ~DrvSprRAM[offs + 2] & 0x40;
		INT32 flipy =  DrvSprRAM[offs + 2] & 0x80;

		UINT8 *gfx_base = bank ? DrvGfxROM1 : DrvGfxROM0;

		draw_8x16_tile_line(sx, sy-16, color, gfx_base, code, flipx, flipy, line);
	}
}

static INT32 DrvDraw()
{
	if (DrvRecalc) {
		for (INT32 i = 0; i < 0x140; i++) {
			INT32 rgb = Palette[i];
			DrvPalette[i] = BurnHighCol(rgb >> 16, rgb >> 8, rgb, 0);
		}
		DrvRecalc = 0;
	}

	BurnTransferClear();

	draw_background(0);

	return 0;
}

static INT32 DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	{
		memset (DrvInputs, 0xff, 3);
		for (INT32 i = 0; i < 5; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
 			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
		}
	}

	ZetNewFrame();
	I8039NewFrame();

	INT32 nCyclesSegment;
	INT32 nInterleave = 256;
	INT32 nCyclesTotal[4] = { 3072000 / 60, 2000000 / 60, 3579545 / 60, 8000000 / 15 / 60 };
	INT32 nCyclesDone[4] = { 0, 0, 0, 0 };
	INT32 nSoundBufferPos = 0;

	DrvDraw();
	memset (pBurnSoundOut,0,nBurnSoundLen*4);

	for (INT32 i = 0; i < nInterleave; i++)
	{
		INT32 nNext;
		scanline = i + 6;

		ZetOpen(0);
		nNext = (i + 1) * nCyclesTotal[0] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[0];
		nCyclesDone[0] += ZetRun(nCyclesSegment);
		if (i == (nInterleave - 248) && *interrupt_enable0) {
			ZetSetIRQLine(Z80_INPUT_LINE_NMI, CPU_IRQSTATUS_ACK);
		}
		ZetClose();
		
		M6809Open(0);
		nNext = (i + 1) * nCyclesTotal[1] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[1];
		nCyclesDone[1] += M6809Run(nCyclesSegment);
		if (i == (nInterleave - 248) && *interrupt_enable1) {
			M6809SetIRQLine(0, CPU_IRQSTATUS_ACK);
		}
		M6809Close();

		ZetOpen(1);
		nNext = (i + 1) * nCyclesTotal[2] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[2];
		nCyclesDone[2] += ZetRun(nCyclesSegment);

		nNext = (nCyclesTotal[3] * (i + 1)) / nInterleave;
		nCyclesDone[3] += I8039Run(nNext - nCyclesDone[3]);
		ZetClose(); // after I8039Run() for DrvSyncDAC()!

		if (scanline >= 16 && scanline < 240) {
			draw_sprites(scanline);
		}
		// Render Sound Segment
		if (pBurnSoundOut) {
			INT32 nSegmentLength = nBurnSoundLen / nInterleave;
			INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
			nSoundBufferPos += nSegmentLength;

			filter_rc_update(0, pAY8910Buffer[0], pSoundBuf, nSegmentLength);
			filter_rc_update(1, pAY8910Buffer[1], pSoundBuf, nSegmentLength);
			filter_rc_update(2, pAY8910Buffer[2], pSoundBuf, nSegmentLength);
			filter_rc_update(3, pAY8910Buffer[3], pSoundBuf, nSegmentLength);
			filter_rc_update(4, pAY8910Buffer[4], pSoundBuf, nSegmentLength);
			filter_rc_update(5, pAY8910Buffer[5], pSoundBuf, nSegmentLength);
		    filter_rc_update(6, pAY8910Buffer[6], pSoundBuf, nSegmentLength);
			filter_rc_update(7, pAY8910Buffer[7], pSoundBuf, nSegmentLength);
			filter_rc_update(6, pAY8910Buffer[8], pSoundBuf, nSegmentLength);
			filter_rc_update(7, pAY8910Buffer[9], pSoundBuf, nSegmentLength);
			filter_rc_update(6, pAY8910Buffer[10], pSoundBuf, nSegmentLength);
			filter_rc_update(7, pAY8910Buffer[11], pSoundBuf, nSegmentLength);
			filter_rc_update(6, pAY8910Buffer[12], pSoundBuf, nSegmentLength);
			filter_rc_update(7, pAY8910Buffer[13], pSoundBuf, nSegmentLength);
			filter_rc_update(6, pAY8910Buffer[14], pSoundBuf, nSegmentLength);
		}
	}

	// Make sure the buffer is entirely filled.
	if (pBurnSoundOut) {
		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		if (nSegmentLength) {
			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);

			filter_rc_update(0, pAY8910Buffer[0], pSoundBuf, nSegmentLength);
			filter_rc_update(1, pAY8910Buffer[1], pSoundBuf, nSegmentLength);
			filter_rc_update(2, pAY8910Buffer[2], pSoundBuf, nSegmentLength);
			filter_rc_update(3, pAY8910Buffer[3], pSoundBuf, nSegmentLength);
			filter_rc_update(4, pAY8910Buffer[4], pSoundBuf, nSegmentLength);
			filter_rc_update(5, pAY8910Buffer[5], pSoundBuf, nSegmentLength);
		    filter_rc_update(6, pAY8910Buffer[6], pSoundBuf, nSegmentLength);
			filter_rc_update(7, pAY8910Buffer[7], pSoundBuf, nSegmentLength);
			filter_rc_update(6, pAY8910Buffer[8], pSoundBuf, nSegmentLength);
			filter_rc_update(7, pAY8910Buffer[9], pSoundBuf, nSegmentLength);
			filter_rc_update(6, pAY8910Buffer[10], pSoundBuf, nSegmentLength);
			filter_rc_update(7, pAY8910Buffer[11], pSoundBuf, nSegmentLength);
			filter_rc_update(6, pAY8910Buffer[12], pSoundBuf, nSegmentLength);
			filter_rc_update(7, pAY8910Buffer[13], pSoundBuf, nSegmentLength);
			filter_rc_update(6, pAY8910Buffer[14], pSoundBuf, nSegmentLength);
		}
		DACUpdate(pBurnSoundOut, nBurnSoundLen);
	}

	if (pBurnDraw) {
		BurnTransferCopy(DrvPalette);
	}

	return 0;
}

static INT32 DrvScan(INT32 nAction,INT32 *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {
		*pnMin = 0x029704;
	}

	if (nAction & ACB_VOLATILE) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = AllRam;
		ba.nLen	  = RamEnd - AllRam;
		ba.szName = "All RAM";
		BurnAcb(&ba);
	}

	if (nAction & ACB_DRIVER_DATA) {
		ZetScan(nAction);
		M6809Scan(nAction);
		I8039Scan(nAction,pnMin);

		AY8910Scan(nAction, pnMin);
		DACScan(nAction, pnMin);
	}

	return 0;
}


// Gyruss (Konami)

static struct BurnRomInfo gyrussRomDesc[] = {
	{ "gyrussk.1",		0x2000, 0xc673b43d, 1 | BRF_PRG | BRF_ESS }, //  0 Main Z80 Code
	{ "gyrussk.2",		0x2000, 0xa4ec03e4, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "gyrussk.3",		0x2000, 0x27454a98, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "gyrussk.9",		0x2000, 0x822bf27e, 2 | BRF_PRG | BRF_ESS }, //  3 Sub M6809 Code

	{ "gyrussk.1a",		0x2000, 0xf4ae1c17, 3 | BRF_PRG | BRF_ESS }, //  4 Audio Z80 Code
	{ "gyrussk.2a",		0x2000, 0xba498115, 3 | BRF_PRG | BRF_ESS }, //  5

	{ "gyrussk.3a",		0x1000, 0x3f9b5dea, 4 | BRF_PRG | BRF_ESS }, //  6 Audio i8039

	{ "gyrussk.6",		0x2000, 0xc949db10, 5 | BRF_GRA },           //  7 Sprites
	{ "gyrussk.5",		0x2000, 0x4f22411a, 5 | BRF_GRA },           //  8
	{ "gyrussk.8",		0x2000, 0x47cd1fbc, 5 | BRF_GRA },           //  9
	{ "gyrussk.7",		0x2000, 0x8e8d388c, 5 | BRF_GRA },           // 10

	{ "gyrussk.4",		0x2000, 0x27d8329b, 6 | BRF_GRA },           // 11 Background Tiles

	{ "gyrussk.pr3",	0x0020, 0x98782db3, 7 | BRF_GRA },           // 12 Color Proms
	{ "gyrussk.pr1",	0x0100, 0x7ed057de, 7 | BRF_GRA },           // 13
	{ "gyrussk.pr2",	0x0100, 0xde823a81, 7 | BRF_GRA },           // 14
};

STD_ROM_PICK(gyruss)
STD_ROM_FN(gyruss)

struct BurnDriver BurnDrvGyruss = {
	"gyruss", NULL, NULL, NULL, "1983",
	"Gyruss (Konami)\0", NULL, "Konami", "GX347",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL | BDF_ORIENTATION_FLIPPED, 2, HARDWARE_PREFIX_KONAMI, GBF_SHOOT, 0,
	NULL, gyrussRomInfo, gyrussRomName, NULL, NULL, GyrussInputInfo, GyrussDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x140,
	224, 256, 3, 4
};


// Gyruss (Centuri)

static struct BurnRomInfo gyrussceRomDesc[] = {
	{ "gya-1.bin",		0x2000, 0x85f8b7c2, 1 | BRF_PRG | BRF_ESS }, //  0 Main Z80 Code
	{ "gya-2.bin",		0x2000, 0x1e1a970f, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "gya-3.bin",		0x2000, 0xf6dbb33b, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "gyrussk.9",		0x2000, 0x822bf27e, 2 | BRF_PRG | BRF_ESS }, //  3 Sub M6809 Code

	{ "gyrussk.1a",		0x2000, 0xf4ae1c17, 3 | BRF_PRG | BRF_ESS }, //  4 Audio Z80 Code
	{ "gyrussk.2a",		0x2000, 0xba498115, 3 | BRF_PRG | BRF_ESS }, //  5

	{ "gyrussk.3a",		0x1000, 0x3f9b5dea, 4 | BRF_PRG | BRF_ESS }, //  6 Audio i8039

	{ "gyrussk.6",		0x2000, 0xc949db10, 5 | BRF_GRA },           //  7 Sprites
	{ "gyrussk.5",		0x2000, 0x4f22411a, 5 | BRF_GRA },           //  8
	{ "gyrussk.8",		0x2000, 0x47cd1fbc, 5 | BRF_GRA },           //  9
	{ "gyrussk.7",		0x2000, 0x8e8d388c, 5 | BRF_GRA },           // 10

	{ "gyrussk.4",		0x2000, 0x27d8329b, 6 | BRF_GRA },           // 11 Background Tiles

	{ "gyrussk.pr3",	0x0020, 0x98782db3, 7 | BRF_GRA },           // 12 Color Proms
	{ "gyrussk.pr1",	0x0100, 0x7ed057de, 7 | BRF_GRA },           // 13
	{ "gyrussk.pr2",	0x0100, 0xde823a81, 7 | BRF_GRA },           // 14
};

STD_ROM_PICK(gyrussce)
STD_ROM_FN(gyrussce)

struct BurnDriver BurnDrvGyrussce = {
	"gyrussce", "gyruss", NULL, NULL, "1983",
	"Gyruss (Centuri)\0", NULL, "Konami (Centuri license)", "GX347",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_ORIENTATION_FLIPPED, 2, HARDWARE_PREFIX_KONAMI, GBF_SHOOT, 0,
	NULL, gyrussceRomInfo, gyrussceRomName, NULL, NULL, GyrussInputInfo, GyrussceDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x140,
	224, 256, 3, 4
};


// Gyruss (bootleg)

static struct BurnRomInfo gyrussbRomDesc[] = {
	{ "1.bin",			0x2000, 0x6bc21c10, 1 | BRF_PRG | BRF_ESS }, //  0 Main Z80 Code
	{ "gyrussk.2",		0x2000, 0xa4ec03e4, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "gyrussk.3",		0x2000, 0x27454a98, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "gyrussk.9",		0x2000, 0x822bf27e, 2 | BRF_PRG | BRF_ESS }, //  3 Sub M6809 Code

	{ "gyrussk.1a",		0x2000, 0xf4ae1c17, 3 | BRF_PRG | BRF_ESS }, //  4 Audio Z80 Code
	{ "gyrussk.2a",		0x2000, 0xba498115, 3 | BRF_PRG | BRF_ESS }, //  5

	{ "gyrussk.3a",		0x1000, 0x3f9b5dea, 4 | BRF_PRG | BRF_ESS }, //  6 Audio i8039

	{ "gyrussk.6",		0x2000, 0xc949db10, 5 | BRF_GRA },           //  7 Sprites
	{ "gyrussk.5",		0x2000, 0x4f22411a, 5 | BRF_GRA },           //  8
	{ "gyrussk.8",		0x2000, 0x47cd1fbc, 5 | BRF_GRA },           //  9
	{ "gyrussk.7",		0x2000, 0x8e8d388c, 5 | BRF_GRA },           // 10

	{ "gyrussk.4",		0x2000, 0x27d8329b, 6 | BRF_GRA },           // 11 Background Tiles

	{ "gyrussk.pr3",	0x0020, 0x98782db3, 7 | BRF_GRA },           // 12 Color Proms
	{ "gyrussk.pr1",	0x0100, 0x7ed057de, 7 | BRF_GRA },           // 13
	{ "gyrussk.pr2",	0x0100, 0xde823a81, 7 | BRF_GRA },           // 14
};

STD_ROM_PICK(gyrussb)
STD_ROM_FN(gyrussb)

struct BurnDriver BurnDrvGyrussb = {
	"gyrussb", "gyruss", NULL, NULL, "1983",
	"Gyruss (bootleg)\0", NULL, "bootleg", "GX347",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_ORIENTATION_FLIPPED, 2, HARDWARE_PREFIX_KONAMI, GBF_SHOOT, 0,
	NULL, gyrussbRomInfo, gyrussbRomName, NULL, NULL, GyrussInputInfo, GyrussDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x140,
	224, 256, 3, 4
};


// Venus

static struct BurnRomInfo venusRomDesc[] = {
	{ "r1",			0x2000, 0xd030abb1, 1 | BRF_PRG | BRF_ESS }, //  0 Main Z80 Code
	{ "r2",			0x2000, 0xdbf65d4d, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "r3",			0x2000, 0xdb246fcd, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "gyrussk.9",		0x2000, 0x822bf27e, 2 | BRF_PRG | BRF_ESS }, //  3 Sub M6809 Code

	{ "gyrussk.1a",		0x2000, 0xf4ae1c17, 3 | BRF_PRG | BRF_ESS }, //  4 Audio Z80 Code
	{ "gyrussk.2a",		0x2000, 0xba498115, 3 | BRF_PRG | BRF_ESS }, //  5

	{ "gyrussk.3a",		0x1000, 0x3f9b5dea, 4 | BRF_PRG | BRF_ESS }, //  6 Audio i8039

	{ "gyrussk.6",		0x2000, 0xc949db10, 5 | BRF_GRA },           //  7 Sprites
	{ "gyrussk.5",		0x2000, 0x4f22411a, 5 | BRF_GRA },           //  8
	{ "gyrussk.8",		0x2000, 0x47cd1fbc, 5 | BRF_GRA },           //  9
	{ "gyrussk.7",		0x2000, 0x8e8d388c, 5 | BRF_GRA },           // 10

	{ "gyrussk.4",		0x2000, 0x27d8329b, 6 | BRF_GRA },           // 11 Background Tiles

	{ "gyrussk.pr3",	0x0020, 0x98782db3, 7 | BRF_GRA },           // 12 Color Proms
	{ "gyrussk.pr1",	0x0100, 0x7ed057de, 7 | BRF_GRA },           // 13
	{ "gyrussk.pr2",	0x0100, 0xde823a81, 7 | BRF_GRA },           // 14
};

STD_ROM_PICK(venus)
STD_ROM_FN(venus)

struct BurnDriver BurnDrvVenus = {
	"venus", "gyruss", NULL, NULL, "1983",
	"Venus\0", NULL, "bootleg", "GX347",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG | BDF_ORIENTATION_VERTICAL | BDF_ORIENTATION_FLIPPED, 2, HARDWARE_PREFIX_KONAMI, GBF_SHOOT, 0,
	NULL, venusRomInfo, venusRomName, NULL, NULL, GyrussInputInfo, GyrussDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x140,
	224, 256, 3, 4
};

