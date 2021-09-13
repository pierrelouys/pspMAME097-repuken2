#include <pspctrl.h>
#include <sys/stat.h>
#include "driver.h"

/* カラーメニューでのＸボタン（０：無効、１：有効） */
#define USE_COLOR_MENU_X_BUTTON 1

#define NOW_DATE   " "__DATE__" "
#define DEVELOPPER "TMK & ＰＳＰ開発幼稚園"
//∽∝∵∫∬開発幼稚園

#ifndef MENU_STR1
	#define MENU_STR1 "pspMAME 0.97" //開
#endif
#ifndef MENU_STR2
	#define MENU_STR2 " "//"もっと速くないと遊べん！"
#endif
#define APP_TITLE MENU_STR1 MENU_STR2 BUILD_NAME

// ----------------------------------------
static SceCtrlData/*ctrl_data_t*/ ctl;
// ----------------------------------------

static unsigned int is_PAD; // read pad raw data.

static u8 is_U,rs_U,ps_U,cs_U;	// U: Up
static u8 is_J,rs_J,ps_J,cs_J;	// J: Right (see your keyboard)
static u8 is_D,rs_D,ps_D,cs_D;	// D: Down
static u8 is_F,rs_F,ps_F,cs_F;	// F: Left	(see your keyboard)
static u8 is_L,rs_L,ps_L;//,cs_L;	// L: L-trigger
static u8 is_R,rs_R,ps_R;//,cs_R;	// R: R-trigger
static u8 is_A,rs_A,ps_A;//,cs_A;	// A: triangle
static u8 is_O,rs_O,ps_O;//,cs_O;	// O: Circle
static u8 is_X,rs_X,ps_X;//,cs_X;	// X: Cross
static u8 is_Q,rs_Q,ps_Q;//,cs_Q;	// Q: Square

static void pad_read(void)
{
static u8 ccc; //counter
u8 ttt; // work
	ccc++; //count up
	ccc&=1; // masked on/off
	sceCtrlPeekBufferPositive(&ctl,1);
UINT32 buttons;
	buttons=ctl.Buttons;

	/* pached the analog stick to digitily sense. */
	ttt=ctl.Ly;//analog[CTRL_ANALOG_Y];
		 if(ttt < 0x10) {		  buttons |= PSP_CTRL_UP;		/* UP HIGH */ }
	else if(ttt < 0x40) { if(ccc) buttons |= PSP_CTRL_UP;		/* UP LOW  */ }
	else if(ttt < 0xc0) { ; /* NONE */ }
	else if(ttt < 0xf0) { if(ccc) buttons |= PSP_CTRL_DOWN; 	/* DOWN LOW  */ }
	else				{		  buttons |= PSP_CTRL_DOWN; 	/* DOWN HIGH */ }

	ttt=ctl.Lx;//analog[CTRL_ANALOG_X];
		 if(ttt < 0x10) {		  buttons |= PSP_CTRL_LEFT; 	/* LEFT HIGH */ }
	else if(ttt < 0x40) { if(ccc) buttons |= PSP_CTRL_LEFT; 	/* LEFT LOW  */ }
	else if(ttt < 0xc0) { ; /* NONE */ }
	else if(ttt < 0xf0) { if(ccc) buttons |= PSP_CTRL_RIGHT;	/* RIGHT LOW  */ }
	else				{		  buttons |= PSP_CTRL_RIGHT;	/* RIGHT HIGH */ }

	/* before buttons sense */
//	rs_E=is_E;
//	rs_S=is_S;
	rs_L=is_L;
	rs_R=is_R;

	rs_U=is_U;
	rs_J=is_J;
	rs_D=is_D;
	rs_F=is_F;

	rs_A=is_A;
	rs_O=is_O;
	rs_X=is_X;
	rs_Q=is_Q;

	/* now buttons sense */
	is_PAD=buttons;
//	is_E=(is_PAD & PSP_CTRL_SELECT);
//	is_S=(is_PAD & PSP_CTRL_START);
	is_L=(is_PAD & PSP_CTRL_LTRIGGER)?1:0;
	is_R=(is_PAD & PSP_CTRL_RTRIGGER)?1:0;

	is_U=(is_PAD & PSP_CTRL_UP);
	is_J=(is_PAD & PSP_CTRL_RIGHT);
	is_D=(is_PAD & PSP_CTRL_DOWN);
	is_F=(is_PAD & PSP_CTRL_LEFT);

	is_A=(is_PAD & PSP_CTRL_TRIANGLE)?1:0;
	is_O=(is_PAD & PSP_CTRL_CIRCLE)?1:0;
	is_X=(is_PAD & PSP_CTRL_CROSS)?1:0;
	is_Q=(is_PAD & PSP_CTRL_SQUARE)?1:0;

	/* now pushed or pulled buttons(triggered check) */
//	ps_E=(is_E != rs_E);
//	ps_S=(is_S != rs_S);
	ps_L=(is_L != rs_L);
	ps_R=(is_R != rs_R);

	ps_U=(is_U != rs_U);
	ps_J=(is_J != rs_J);
	ps_D=(is_D != rs_D);
	ps_F=(is_F != rs_F);

	ps_A=(is_A != rs_A);
	ps_O=(is_O != rs_O);
	ps_X=(is_X != rs_X);
	ps_Q=(is_Q != rs_Q);

	/* key pressed time counter at a frame */ /* 要するにオートリピート */
	if(is_U) cs_U++;	if(ps_U) cs_U=0;
	if(is_D) cs_D++;	if(ps_D) cs_D=0;
	if(is_F) cs_F++;	if(ps_F) cs_F=0;
	if(is_J) cs_J++;	if(ps_J) cs_J=0;
//	if(is_E) cs_E++;	if(ps_E) cs_E=0;//disabled
}

/////////////////Filer
#define PATHLIST_H 20
static int dlist_num;
static int dlist_start;
int dlist_curpos;


extern SETTING setting;
extern UINT32 end_size;

void Get_DriverList(int mode) {
	int i;

	dlist_num = 0;

	for (i =0; drivers[i]; i++)
		dlist_num++;

	if (0 ==mode) // no check
	{
		for (i =0; i <dlist_num; i++)
			drv_idx[i] =i;
	} else if (1 ==mode) // filtered
	{
		dlist_num =Verify_Roms();
	}
	dlist_start  = 0;
	dlist_curpos = 0;
}

int Verify_Roms()
{
char  *paths[] ={ "roms", "ms0:/mamepsp/roms", 0 };
int i, j;
int idx =0;
struct stat st;
	for (i =0; drivers[i]; i++) {
		for (j =0; paths[j]; j++) {
			char path[40];
			sprintf(path, "%s/%s.zip", paths[j], drivers[i]->name);
			if (stat(path, &st) == 0) {
				drv_idx[idx ++] =i;
				break;
			}

			sprintf(path, "%s/%s", paths[j], drivers[i]->name);
			if (stat(path, &st) == 0) {
				if (S_ISDIR(st.st_mode)) {
					drv_idx[idx ++] =i;
					break;
				}
			}
		}
	}
	drv_idx[idx] =-1;
	return idx;
}

static void BitBlt(int x, int y, int w, int h, /*int mag,*/ const u16 *d)
{
	u16 *vptr, *src;
	int xx, yy;//, mx, my;
	const u16 *dst;

	vptr = psp_frame_addr(draw_frame, x, y);

	for (yy = 0; yy < h; yy++)
	{
		{
			src = vptr;
			dst = d;
			for (xx = 0; xx < w; xx++)
			{
				{
					*src++ = *dst;
				}
				dst++;
			}
			vptr += BUF_WIDTH;
		}
		d += w;
	}
}

int bBitmap;
u16 bgBitmap[480*272];
static void psp_frame(const char *msg0, const char *msg1)
{
	if (bBitmap)/* 壁紙 */
	{	BitBlt(0, 0, 480, 272, /*1,*/ bgBitmap);}
	else/* 壁紙なし */
	{	psp_fill_frame(draw_frame, CNVCOL15TO32(setting.color[0]));}
	psp_print(0, 0, setting.color[3], APP_TITLE);
	// メッセージなど
	if (msg0) psp_print(17, 14, setting.color[2], msg0);
	// 枠
#if 1
	/*pgDrawFrame*/psp_box_frame(draw_frame, 17-16, 25, 463+16, 248+10, setting.color[1]);
	/*pgDrawFrame*/psp_box_frame(draw_frame, 18-16, 26, 462+16, 247+10, setting.color[1]);
#else
	{
		const rectangle tbounds={	17-16,	463+16, 25, 248+10};
		psp_box_rect( draw_frame,	setting.color[1], &tbounds);
		tbounds.min_x++;	tbounds.max_x--;
		tbounds.min_y++;	tbounds.max_y--;
		psp_box_rect( draw_frame,	setting.color[1], &tbounds);
	}
#endif
	// 操作説明
	if(msg1) psp_print(17, 252+10, setting.color[2], msg1);
}


void Draw_All(void)
{                                                         //○：実行 Ｌ：設定メニュー △：エミュレータの終了
    // display title, year, manufacturer of game in title bar    
	char game_info[80];
    if (dlist_num < 1) 
    {
		sprintf(game_info, "No supported ROMs found");
    } else {    
		sprintf(game_info, "%s (%s, %s)", 
		drivers[(int)drv_idx[dlist_curpos]]->name, 
		drivers[(int)drv_idx[dlist_curpos]]->manufacturer, 
		drivers[(int)drv_idx[dlist_curpos]]->year);
    }
	psp_frame(game_info, "◯: run game L: settings △: exit");

#if (1==DEBUG_MAME_MEMORY_CHECK)
    // show available mem
    if (end_size > 0) 
        {
        char memInfo[80];
        sprintf(memInfo,"Free memory: %d MB",(end_size / (1024*1024)) );
    	psp_print(333, 27, setting.color[3], memInfo);
        }
#endif //(1==MEMORY_CHECK)

	int i;
	// ゲームリスト
	i = dlist_start;
	while (i < (dlist_start+PATHLIST_H))
	{
		if (i<dlist_num) {
		    int col;
			col = setting.color[(i==dlist_curpos)?3:2];

			psp_print(((4-3)*8)-2, ((i-dlist_start)+4)*10, col, drivers[(int)drv_idx[i]]->description);
		}
		i++;
	}
	v_sync();
	psp_flip_screen(/*1*/);
}


int Confirm_Control(void)
{
	while (psp_loop /*&& !pressed*/)
	{
		v_sync();
		pad_read();//key = Read_Key();

		if (ps_O && is_O)
		{
			return 1;
		}
		if (ps_X && is_X)
		{
			return 0;
		}
	}
	return 0;
}

int Control(void)
{
	int pressed = 0;

	while (psp_loop && !pressed)
	{
		pressed = 1;
		v_sync();
		pad_read();//key = Read_Key();
		if ((is_U)&&((6<cs_U)&&(0==(cs_U&7))||(ps_U)))
		{
			if (dlist_curpos > 0) { dlist_curpos--; 	}
			if (dlist_curpos < dlist_start) { dlist_start = dlist_curpos; }
		}
		else if ((is_D)&&((6<cs_D)&&(0==(cs_D&7))||(ps_D)))
		{
			if (dlist_curpos < (dlist_num-1)) { 	dlist_curpos++; 	}
			if (dlist_curpos >= (dlist_start+PATHLIST_H)) { dlist_start++; }
		}
		else if (ps_F && is_F)
		{
			dlist_curpos -= PATHLIST_H;
			if (dlist_curpos <	0)			{ dlist_curpos = 0; 		  }
			if (dlist_curpos < dlist_start) { dlist_start = dlist_curpos; }
		}
		else if (ps_J && is_J)
		{
			dlist_curpos += PATHLIST_H;
			if (dlist_curpos >= dlist_num) { dlist_curpos = dlist_num-1; }
			while (dlist_curpos >= (dlist_start+PATHLIST_H)) { dlist_start++; }
		}
		else if (ps_A && is_A)
		{
			//psp_frame("ALO! また遊んでね" DEVELOPPER, "×：やっぱキャンセル  ○：終了" );
			psp_frame(""," ×: cancel   ○: quit" );
			psp_print(40,120, setting.color[3], " Press ○ to quit");
			v_sync();
			psp_flip_screen(/*1*/);
			if (Confirm_Control()) psp_loop = 0;
		}
		else if (ps_O && is_O)
		{
			return 2;
		}
		else if (ps_L && is_L)
		{
			return 3;
		}
		else
		{
			pressed = 0;
		}
	}
	return 0;
}

/* REPEAT_TIME_W: オートリピートがかかるまでの時間 */
#define REPEAT_TIME_W 6

#define MENU2_Y_OFFS  (5-1-1)
#define MENU2_X_SHIFT (4-2)
#define MENU2_X_OFFS  (MENU2_X_SHIFT*8)
#define MENU2B_X_OFFS ((MENU2_X_SHIFT+10)*8)
#define MENU2C_X_OFFS ((MENU2_X_SHIFT-1)*8)


void Draw_Confirm(void)
{
	psp_frame(drivers[(int)drv_idx[dlist_curpos]]->name, "○: run   ×: cancel");
	psp_print(MENU2C_X_OFFS, ( 9*10), setting.color[3], drivers[(int)drv_idx[dlist_curpos]]->description);
	psp_print(MENU2B_X_OFFS, (12*10), setting.color[3], "Press ○ to run game");//"を実行します。"
	v_sync();
	psp_flip_screen(/*1*/);
}

//////////////////

SETTING setting;

void save_config(void)
{
	char CfgPath[MAX_PATH];
	char *p;

	strcpy(CfgPath, getCurDir());
	p = strrchr(CfgPath, '/')+1;
	strcpy(p, "pspmame.cfg");

	int fd;
	fd = sceIoOpen(CfgPath, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_TRUNC, 0777);
	if(fd>=0){
		sceIoWrite(fd, &setting, sizeof(setting));
		sceIoClose(fd);
	}
}

#define SET_ON	(1)
#define SET_OFF (0)

#define SET_FSKIP_07   (8)
#define SET_FSKIP_06   (7)
#define SET_FSKIP_05   (6)
#define SET_FSKIP_04   (5)
#define SET_FSKIP_03   (4)
#define SET_FSKIP_02   (3)
#define SET_FSKIP_01   (2)
#define SET_FSKIP_00   (1)
#define SET_FSKIP_AUTO (0)

#define PSP222MHz (0)
#define PSP266MHz (1)
#define PSP300MHz (2)
#define PSP333MHz (3)

/* set default to fskip00 */
#ifndef SET_DEFAULT_FSKIPS
	#define SET_DEFAULT_FSKIPS SET_FSKIP_00
#endif // SET_DEFAULT_FSKIPS
/* set default to off the video syncronization for draw. */
#define SET_DEFAULT_VSYNC_ON_OFF SET_ON

/* set default speed max limitage. */
#define SET_DEFAULT_LIMIT_ON_OFF SET_OFF

/* set default to 22050 */
#ifndef SET_DEFAULT_SOUND_FREQ
	#define SET_DEFAULT_SOUND_FREQ SND_22050
#endif // SET_DEFAULT_SOUND_FREQ

/* set default PSP cpu clock 333M[Hz] */
#ifndef SET_DEFAULT_PSP_CLOCK
	#define SET_DEFAULT_PSP_CLOCK PSP333MHz
#endif // SET_DEFAULT_PSP_CLOCK

#define SET_DEFAULT_SCREEN SCR_X1

void load_config(void)
{
	char CfgPath[MAX_PATH];
	char *p;

	memset(&setting, 0, sizeof(setting));

	strcpy(CfgPath, getCurDir());
	p = strrchr(CfgPath, '/')+1;
	strcpy(p, "pspmame.cfg");

	int fd;
	fd = sceIoOpen(CfgPath,PSP_O_RDONLY, 0777);
	if (fd >= 0)
	{
		sceIoRead(fd, &setting, sizeof(setting));
		sceIoClose(fd);
		/* check if ignore value, forced in range. */
		if(setting.screensize > (SCR_MAX-1)) setting.screensize = SET_DEFAULT_SCREEN;
		if(setting.frameskip > /*9*/60) 						setting.frameskip=/*0*/60;
		if (setting.sound_rate > (SND_MAX-1))					setting.sound_rate = (SND_MAX-1);
		setting.cpu_clock  &=3;
		if(setting.bgbright<0 || setting.bgbright>100)			setting.bgbright=100;
		if(!strcmp(setting.vercnf, CONFIG_VER))
			return;
	}

	strcpy(setting.vercnf, CONFIG_VER);
	setting.screensize		= SET_DEFAULT_SCREEN;
	setting.frameskip		= SET_DEFAULT_FSKIPS;
	setting.vsync_ON_OFF	= SET_DEFAULT_VSYNC_ON_OFF;
	setting.limit_ON_OFF	= SET_DEFAULT_LIMIT_ON_OFF;
	setting.show_fps_ON_OFF = SET_OFF;
	setting.sound_rate		= SET_DEFAULT_SOUND_FREQ;/*default sound rate 44100 */
	setting.rom_filter  	= SET_OFF;

	setting.color[0] = DEF_COLOR0;
	setting.color[1] = DEF_COLOR1;
	setting.color[2] = DEF_COLOR2;
	setting.color[3] = DEF_COLOR3;
	setting.bgbright = 100;/* default back bitmap brightness. */

	setting.cpu_clock = SET_DEFAULT_PSP_CLOCK;/* 3==psp speed 333[MHz] */
}

/*static*/ void load_menu_bg(void)
{
	char BgPath[MAX_PATH];
	char *p;

	strcpy(BgPath, getCurDir());
	p = strrchr(BgPath, '/')+1;
	strcpy(p, "MENU.BMP");

	int fd;
	fd = sceIoOpen(BgPath, PSP_O_RDONLY, 0777);
	if(fd>=0){
		#define my_BMP_HEADER_SIZE (0x36)
		#define my_BMP_FILE_SIZE (480*272*3+my_BMP_HEADER_SIZE)
		static u8 menu_bg_buf[my_BMP_FILE_SIZE];
		sceIoRead(fd, menu_bg_buf, my_BMP_FILE_SIZE);
		sceIoClose(fd);
		#undef my_BMP_FILE_SIZE

		u8 *menu_bg;
		u16 *vptr;
		menu_bg = menu_bg_buf + my_BMP_HEADER_SIZE;
		#undef my_BMP_HEADER_SIZE
		vptr=bgBitmap;
		u16 y;
		for(y=0; y<272; y++){
			u16 x;
			for(x=0; x<480; x++){
				*vptr= (((	/*b*/(*(menu_bg +((271-y)*480 +x)*3   )) &0xf8)<<7) |
						((	/*g*/(*(menu_bg +((271-y)*480 +x)*3 +1)) &0xf8)<<2) |
						(	/*r*/(*(menu_bg +((271-y)*480 +x)*3 +2))	   >>3));
				vptr+=/*2*/1;
			}
		}
		bBitmap = 1;
	}else{
		bBitmap = 0;
	}
}

// 半透明処理
static u16 rgbTransp(u16 fgRGB, u16 bgRGB, int alpha)
{
	alpha *= 256;
	alpha /= 100;

return(MAKECOL15(
/*R =*/ (((GETR15(fgRGB) * (alpha)) + (GETR15(bgRGB) * (256 - alpha))) >> 9)/*;*/
/*G =*/,(((GETG15(fgRGB) * (alpha)) + (GETG15(bgRGB) * (256 - alpha))) >> 9)/*;*/
/*B =*/,(((GETB15(fgRGB) * (alpha)) + (GETB15(bgRGB) * (256 - alpha))) >> 9)/*;*/
));

}

/*static*/ void bgbright_change(void)
{
	u16 *vptr,rgb;
	int i;

	vptr=bgBitmap;
	for(i=0; i<272*480; i++)
	{
		rgb = *vptr;
		*vptr = rgbTransp(rgb, 0x0000, setting.bgbright);
		vptr++;
	}
}

void psp_colorconfig(void)
{
	enum
	{
		COLOR0_R,		COLOR0_G,		COLOR0_B,
		COLOR1_R,		COLOR1_G,		COLOR1_B,
		COLOR2_R,		COLOR2_G,		COLOR2_B,
		COLOR3_R,		COLOR3_G,		COLOR3_B,
		BG_BRIGHT,		INIT,			EXIT,
		MENU2_MAX
	};
	char tmp[64];
	int color[4][3];
	int sel=0, y, i;
	u8 sel_count=0;
	u8 reload_bitmap = 0;

	memset(color, 0, sizeof(int)*4*3);
	for (i=0; i<4; i++)
	{
		color[i][0] = GETR15(setting.color[i]);
		color[i][1] = GETG15(setting.color[i]);
		color[i][2] = GETB15(setting.color[i]);
	}

	while (psp_loop)
	{
		v_sync();
		pad_read();//new_pad =Read_Key();
		if ( ((ps_J ||(REPEAT_TIME_W<cs_J)) && is_J) )
		{
			if (sel == BG_BRIGHT)
			{
				setting.bgbright++;
				if (setting.bgbright > 100) setting.bgbright = 0;
				reload_bitmap = bBitmap;
			}
			else if (sel >= COLOR0_R && sel <= COLOR3_B)
			{
				if (color[sel/3][sel%3] < 255/*31*/)
				{	color[sel/3][sel%3]++;}
			}
		}
		else if ( ((ps_F ||(REPEAT_TIME_W<cs_F)) && is_F) )
		{
			if (sel == BG_BRIGHT)
			{
				setting.bgbright--;
				if (setting.bgbright < 0) setting.bgbright = 100;
				reload_bitmap = bBitmap;
			}
			else if (sel >= COLOR0_R && sel <= COLOR3_B)
			{
				if (color[sel/3][sel%3] > 0)
				{	color[sel/3][sel%3]--;}
			}
		}
		else if ( ((ps_U ||(REPEAT_TIME_W<cs_U)) && is_U) )
		{
			sel--;
			if (sel < 0) sel = (MENU2_MAX-1);
		}
		else if ( ((ps_D ||(REPEAT_TIME_W<cs_D)) && is_D) )
		{
			sel++;
			if (sel > (MENU2_MAX-1)) sel = 0;
		}
		#if (1==USE_COLOR_MENU_X_BUTTON)
		else if( ps_X && is_X )
		{
			break;/*戻る*/
		}
		#endif //(1==USE_COLOR_MENU_X_BUTTON)
		else if( ps_O && is_O ){
			if (sel == EXIT){ break;}
			else if (sel == INIT)
			{
				color[0][0] = GETR15(DEF_COLOR0);
				color[0][1] = GETG15(DEF_COLOR0);
				color[0][2] = GETB15(DEF_COLOR0);
				color[1][0] = GETR15(DEF_COLOR1);
				color[1][1] = GETG15(DEF_COLOR1);
				color[1][2] = GETB15(DEF_COLOR1);
				color[2][0] = GETR15(DEF_COLOR2);
				color[2][1] = GETG15(DEF_COLOR2);
				color[2][2] = GETB15(DEF_COLOR2);
				color[3][0] = GETR15(DEF_COLOR3);
				color[3][1] = GETG15(DEF_COLOR3);
				color[3][2] = GETB15(DEF_COLOR3);
				setting.bgbright = 100;
				reload_bitmap = bBitmap;
			}
		}

		if (reload_bitmap)
		{
			load_menu_bg();
			bgbright_change();
			reload_bitmap = 0;
		}

		for (i = 0; i < 4; i++)
		{	setting.color[i] = MAKECOL15(color[i][0], color[i][1], color[i][2]);}

		sel_count++;
		sel_count &=15;

		if (sel >= COLOR0_R && sel <= BG_BRIGHT)
		{	psp_frame(NULL, "←→：change values" //"←→：値の変更"
			#if (1==USE_COLOR_MENU_X_BUTTON)
				"  ×：back to main menu" //"  ×：メインメニューに戻る"
			#endif //(1==USE_COLOR_MENU_X_BUTTON)
			);
		} else if (sel == INIT)
		{	psp_frame(NULL, "○：restore defaults" //"○：設定を初期化する"
			#if (1==USE_COLOR_MENU_X_BUTTON)
				"  ×：back to main menu" //"  ×：メインメニューに戻る"
			#endif //(1==USE_COLOR_MENU_X_BUTTON)
			);
		} else
		{	psp_frame(NULL, "○"
			#if (1==USE_COLOR_MENU_X_BUTTON)
				"×"
			#endif //(1==USE_COLOR_MENU_X_BUTTON)
				": back to main menu" //"：メインメニューに戻る"
			);
		}

#define MENU2_Y_SPACER 11
#define DDD (0.90)
		int color_rrr;
		int color_ggg;
		int color_bbb;
		int rr1,gg1,bb1;
		int rr2,gg2,bb2;
		rr1=GETR15(setting.color[3]);	rr2=rr1*DDD;
		gg1=GETG15(setting.color[3]);	gg2=gg1*DDD;
		bb1=GETB15(setting.color[3]);	bb2=bb1*DDD;
		color_rrr = MAKECOL15(rr1, gg2, bb2);
		color_ggg = MAKECOL15(rr2, gg1, bb2);
		color_bbb = MAKECOL15(rr2, gg2, bb1);
#undef DDD
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS	 )*(MENU2_Y_SPACER)), color_rrr,"COLOR0 R:");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+ 1)*(MENU2_Y_SPACER)), color_ggg,"COLOR0 G:");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+ 2)*(MENU2_Y_SPACER)), color_bbb,"COLOR0 B:");
		//3
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+ 4)*(MENU2_Y_SPACER)), color_rrr,"COLOR1 R:");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+ 5)*(MENU2_Y_SPACER)), color_ggg,"COLOR1 G:");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+ 6)*(MENU2_Y_SPACER)), color_bbb,"COLOR1 B:");
		//7
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+ 8)*(MENU2_Y_SPACER)), color_rrr,"COLOR2 R:");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+ 9)*(MENU2_Y_SPACER)), color_ggg,"COLOR2 G:");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+10)*(MENU2_Y_SPACER)), color_bbb,"COLOR2 B:");
		//11
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+12)*(MENU2_Y_SPACER)), color_rrr,"COLOR3 R:");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+13)*(MENU2_Y_SPACER)), color_ggg,"COLOR3 G:");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+14)*(MENU2_Y_SPACER)), color_bbb,"COLOR3 B:");
		//15
		sprintf(tmp, "BG BRIGHT:%3d%%", setting.bgbright);
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+16)*(MENU2_Y_SPACER)), setting.color[3],tmp);
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+17)*(MENU2_Y_SPACER)), setting.color[3],"Initialize");
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+18)*(MENU2_Y_SPACER)), setting.color[3],"Return to Main Menu");

		y=MENU2_Y_OFFS;
		for(i=0; i<12; i++, y++)
		{
			if(i!=0 && i%3==0) y++;
			//_itoa(color[i/3][i%3], tmp);
			sprintf(tmp, "%d", color[i/3][i%3]);
			psp_print(MENU2B_X_OFFS,y*(MENU2_Y_SPACER),setting.color[3],tmp);
		}

		if (sel_count < 12)
		{
			y=MENU2_Y_OFFS + sel;
			if(sel>=COLOR1_R) y++;
			if(sel>=COLOR2_R) y++;
			if(sel>=COLOR3_R) y++;
			if(sel>=BG_BRIGHT) y++;
			psp_print(MENU2C_X_OFFS,y*(MENU2_Y_SPACER),setting.color[3],">");
		}
		v_sync();
		psp_flip_screen(/*1*/);
	}
}

#define STR_DEFAULT " ""[default]"

const char *cpu_clocks[] = {
	"222MHz"
	#if (PSP222MHz==SET_DEFAULT_PSP_CLOCK)
	STR_DEFAULT
	#endif
	,"266MHz"
	#if (PSP266MHz==SET_DEFAULT_PSP_CLOCK)
	STR_DEFAULT
	#endif
	,"300MHz"
	#if (PSP300MHz==SET_DEFAULT_PSP_CLOCK)
	STR_DEFAULT
	#endif
	,"333MHz"
	#if (PSP333MHz==SET_DEFAULT_PSP_CLOCK)
	STR_DEFAULT
	#endif
};

#define MENU1_Y_OFFS (5-1-1)

void psp_credits(void)
{
	char tmp[64];

	while (psp_loop)
	{
		v_sync();
		pad_read();//new_pad =Read_Key();
		if( ps_X && is_X )
		{
			break;/*戻る*/
		}
		else if( ps_O && is_O )
	    { 
            break; 
        }

		psp_frame(NULL, "○"
		#if (1==USE_COLOR_MENU_X_BUTTON)
			"×"
		#endif //(1==USE_COLOR_MENU_X_BUTTON)
			": back to main menu" //"：メインメニューに戻る"
		);
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+1)*(MENU2_Y_SPACER)), setting.color[3], "CREDITS");

		sprintf(tmp, "%s version %s", MENU_STR1, CONFIG_VER);
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+5)*(MENU2_Y_SPACER)), setting.color[3],tmp);

		sprintf(tmp, "Compiled on %s", NOW_DATE);
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+7)*(MENU2_Y_SPACER)), setting.color[3],tmp);

		sprintf(tmp, "Original port by %s", DEVELOPPER);
		psp_print(MENU2_X_OFFS, ((MENU2_Y_OFFS+9)*(MENU2_Y_SPACER)), setting.color[3],tmp);

		v_sync();
		psp_flip_screen(/*1*/);
	}
}

extern void mame_set_to_options_samplerate(void);/* src/psp/config.c */
void psp_menu(void)
{
	enum
	{
		ITEM_SOUND_RATE, //0
//---space---
        ITEM_SCREEN_SIZE, //1
		ITEM_FRAME_SKIP,
		ITEM_VSYNC,
		ITEM_LIMIT_SPEED,
		ITEM_SHOW_FPS,
        ITEM_ROM_FILTER,
//---space---
		ITEM_CPU_CLOCK, //7
		ITEM_COLOR_CONFIG,
//---space---
		ITEM_CREDITS, //9
//---space---
		ITEM_CONTINUE, //10
//---end---
		ITEM_MENU1_MAX
	};

	char tmp[64];
	static u8 sel=0;
	u8 y;
	u8 sel_count=0;

	while (psp_loop)
	{
		v_sync();
		pad_read();//new_pad =Read_Key();
		if( ps_F && is_F )//atras
		{
			switch (sel)
			{
			case ITEM_SOUND_RATE:	if (setting.sound_rate < (SND_MAX-1)) setting.sound_rate++; break;
			case ITEM_FRAME_SKIP:	if (setting.frameskip  > 0) setting.frameskip--;	break;
			case ITEM_SCREEN_SIZE:	if (setting.screensize > 0) setting.screensize--; break;

			case ITEM_VSYNC:		setting.vsync_ON_OFF	= SET_OFF; break;
			case ITEM_LIMIT_SPEED:	setting.limit_ON_OFF	= SET_OFF; break;
			case ITEM_SHOW_FPS: 	setting.show_fps_ON_OFF = SET_OFF; break;
			case ITEM_ROM_FILTER:	setting.rom_filter  	= SET_OFF; Get_DriverList(setting.rom_filter); break;
			case ITEM_CPU_CLOCK:	if (setting.cpu_clock  > 0) setting.cpu_clock--;	break;
			}
		}
		else if( ps_J && is_J )//adelante
		{
			switch (sel)
			{
			case ITEM_SOUND_RATE:	if (setting.sound_rate >  0) setting.sound_rate--; break;
			case ITEM_FRAME_SKIP:	if (setting.frameskip  < (60+1)) setting.frameskip++;  break;
			case ITEM_SCREEN_SIZE:	if (setting.screensize < (SCR_MAX-1)) setting.screensize++; break;
			case ITEM_VSYNC:		setting.vsync_ON_OFF	= SET_ON; break;
			case ITEM_LIMIT_SPEED:	setting.limit_ON_OFF	= SET_ON; break;
			case ITEM_SHOW_FPS: 	setting.show_fps_ON_OFF = SET_ON; break;
			case ITEM_ROM_FILTER:	setting.rom_filter  	= SET_ON; Get_DriverList(setting.rom_filter); break;
			case ITEM_CPU_CLOCK:	if (setting.cpu_clock  <  3) setting.cpu_clock++;  break;
			}
		}
		else if ( ((ps_U ||(REPEAT_TIME_W<cs_U)) && is_U) )
		{
			sel--;
			if (sel > ITEM_MENU1_MAX) sel=(ITEM_MENU1_MAX-1);
		}
		else if ( ((ps_D ||(REPEAT_TIME_W<cs_D)) && is_D) )
		{
			sel++;
			if (sel > (ITEM_MENU1_MAX-1)) sel = 0;
		}
		else if( ps_O && is_O )
		{
			switch(sel)
			{
			case ITEM_SCREEN_SIZE:
                 break;

			case ITEM_COLOR_CONFIG:
				psp_colorconfig();
				break;

			case ITEM_CREDITS:
				psp_credits();
				break;

			case ITEM_CONTINUE: return;
			}
		}
		else if( ( ps_X && is_X ) || ( ps_L && is_L ) )
		{
			break;
		}

		{// sprintf(tmp, "selected item #%d",sel );
#if 0
/* 「説明」入れると「不安定」になるので「廃止」。 */
		const char *setumei[]={
			"OFFで、音なしです。",
			#if (0!=LINK_ROT)
			"画面の縦横です。([H]が横＝PSPは普通、[V]が縦＝PSPを回転)。",
			#endif //(0!=LINK_ROT)
			"フレームスキップです。00は「０フレームスキップ」、つまりフルフレーム描画。",
			"VSYNC入れると重い...orz",
			"デバッグ用FPS表\示。(表\示ルーチンが遅いので) 3%～5%程度遅くなるようです。",
			"そら333MHzに決まってます。",
			"メニューの「字の色」の設定です。",
			"ファイラー(ゲーム選択画面)に戻ります。",
			};
			strcpy(tmp, &setumei[sel][0] );
#endif
			if (sel < ITEM_COLOR_CONFIG)
				psp_frame(NULL/*tmp*/, "←→：change value  ×Ｌ：back to game selection"); //"←→：値の変更  ×Ｌ：ゲーム選択画面に戻る"
			else if ( (sel == ITEM_COLOR_CONFIG) || (sel == ITEM_CREDITS) )
				psp_frame(NULL/*tmp*/, "○：open  ×Ｌ：back to game selection"); //"○：カラー設定メニューに移動  ×Ｌ：ゲーム選択画面に戻る"
			else
				psp_frame(NULL/*tmp*/, "○×Ｌ：back to game selection"); //"○×Ｌ：ゲーム選択画面に戻る"
		}
#define MENU1_Y_SPACER 11
		mame_set_to_options_samplerate();
		if(SND_OFF==setting.sound_rate)
			{	sprintf(tmp, "SOUND:  OFF    ");}
		else {sprintf(tmp,"SOUND:   %5d[Hz]", options.samplerate);}
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_SOUND_RATE)*(MENU1_Y_SPACER)), setting.color[3],tmp);

{
static const char *scr_names[] = {
	"x1-normal",
	"Rotate",
};
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_SCREEN_SIZE+(1))*(MENU1_Y_SPACER)), setting.color[3], "SCREEN MODE:  %s", scr_names[setting.screensize]);
}

		if (0 ==setting.frameskip)
		{	sprintf(tmp, "FRAME SKIP:   Auto"
		#if (SET_DEFAULT_FSKIPS==SET_FSKIP_AUTO)
			STR_DEFAULT
		#endif
			);
		}
		else if (SET_DEFAULT_FSKIPS ==setting.frameskip)
		{	sprintf(tmp, "FRAME SKIP:   %02d" STR_DEFAULT, (setting.frameskip - 1));}
		else
		{	sprintf(tmp, "FRAME SKIP:   %02d", (setting.frameskip - 1));}
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_FRAME_SKIP+(1))*(MENU1_Y_SPACER)), setting.color[3],tmp);


		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_VSYNC+(1))*(MENU1_Y_SPACER)), 			setting.color[3], setting.vsync_ON_OFF
			 ? "VSYNC:        " "ON"
			#if (SET_ON==SET_DEFAULT_VSYNC_ON_OFF)
			STR_DEFAULT
			#endif
			 : "VSYNC:        " "OFF"
			#if (SET_OFF==SET_DEFAULT_VSYNC_ON_OFF)
			STR_DEFAULT
			#endif
		);

		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_LIMIT_SPEED +(1)	)*(MENU1_Y_SPACER)),	setting.color[3], setting.limit_ON_OFF		? "LIMIT SPEED:  " "ON" : "LIMIT SPEED:  " "OFF" STR_DEFAULT			);
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_SHOW_FPS	+(1)	)*(MENU1_Y_SPACER)),	setting.color[3], setting.show_fps_ON_OFF	? "SHOW FPS:     " "ON" 			: "SHOW FPS:     " "OFF" STR_DEFAULT);
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_ROM_FILTER	+(2)	)*(MENU1_Y_SPACER)),	setting.color[3], setting.rom_filter	? "ROM FILTER:   " "ON" 			: "ROM FILTER:   " "OFF" STR_DEFAULT);
		//7
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_CPU_CLOCK	+(2)	)*(MENU1_Y_SPACER)),	setting.color[3],"CPU CLOCK:    " "%s",cpu_clocks[setting.cpu_clock]);
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_COLOR_CONFIG+(2)	)*(MENU1_Y_SPACER)),	setting.color[3],"COLOR CONFIG");
		//10
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_CREDITS	    +(3)    )*(MENU1_Y_SPACER)),	setting.color[3],"CREDITS");
        //12
		psp_print(MENU2_X_OFFS, ((MENU1_Y_OFFS+ITEM_CONTINUE	+(4)    )*(MENU1_Y_SPACER)),	setting.color[3],"Go back to roms list");

		sel_count++;
		sel_count &=15;
		if(sel_count < 12)
		{
			y=MENU1_Y_OFFS + sel;
			if(sel > ITEM_SOUND_RATE)	{y++;}
			if(sel > ITEM_SHOW_FPS) 	{y++;}
			if(sel > ITEM_COLOR_CONFIG) {y++;}
			if(sel > ITEM_CREDITS) {y++;}

			psp_print(MENU2C_X_OFFS,y*(MENU1_Y_SPACER),setting.color[3], ">");
		}
		v_sync();
		psp_flip_screen(/*1*/);
	}

}
