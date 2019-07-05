/*
    mdport.c - Machine Dependent Code for Porting Unix/Curses games
			   on Nintendo Wii

    Copyright (C) 2019 Nebiun
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name(s) of the author(s) nor the names of other contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <MLlib.h>
#include "rogue_ttf.h"
#include "extern.h"
#include "rogue_str.h"
#include "wcurses.h"

#define X_START 40
#define Y_START 40

typedef struct {
	GXColor color;
	char attr;
	char ch;
} wiiChar_t;

typedef struct {
	wiiChar_t *chars;
	ushort flag;
} wiiLine_t;

typedef struct {
	wiiLine_t *screen;
	short y,x;
	short lines;
	short cols;
} wiiScreen_t;

static wiiScreen_t wiiScreen;

static ML_Font font;
static ML_TextInfo infos;
static int fontSize = -1;
static int foreground_color = C_GREEN;
static int background_color = C_BLACK;

static GXColor _convertColor(int color)
{
	static u_char cc[4];
	static int last = -1;

	if(last != color) {
		register int i, newcolor;

		last = color;
		newcolor = color;
		for(i=0; i<4; i++) {
			cc[i] = newcolor & 0xff;
			newcolor >>= 8;
		}
	}
	return (GXColor){cc[0], cc[1], cc[2], cc[3]};
}

void md_putstr_at(int y, int x, char *s, int color)
{
	int i, len;
	GXColor new_color;

	if(wiiScreen.screen != NULL) {
		len = strlen(s);
		new_color = _convertColor(color);

		for(i=0; i<len; i++) {
			wiiChar_t *p = &wiiScreen.screen[y].chars[x+i];

			if( (s[i] >= ' ') && (s[i] <= '~') )
				p->ch = s[i];
			else
				p->ch = 'X';
			p->color = new_color;
		}
	}
}

void md_putchar(int c)
{

}

void md_refresh(void)
{
	GXColor old_color;
	int l, c;

	if(wiiScreen.screen != NULL) {
		old_color = font.color;
		for(l=0; l<wiiScreen.lines; l++) {
			for(c=0; c<wiiScreen.cols; c++) {
				wiiChar_t *p = &wiiScreen.screen[l].chars[c];
				char s[2];

				font.color = p->color;
				s[0] = p->ch;
				s[1] = '\0';
				ML_DrawText(&font, X_START + c * infos.width, Y_START + l * infos.height, "%s", s);
				font.color = old_color;
			}
		}
	}
	ML_Refresh();
}

int md_getchar_at(int y, int x)
{
	int l, c;
	wiiChar_t *p;
	
	l = (y - Y_START + infos.height - 1)/infos.height;
	c = (x - X_START + infos.width - 1)/infos.width;
	if((l < wiiScreen.lines) && (c < wiiScreen.cols)) {
		p = &wiiScreen.screen[l].chars[c];
#if 0
		char buffino[12];
		snprintf(buffino,sizeof(buffino),"%2d %2d %3d", l,c, p->ch);
		md_putstr_at(LINES - 1, 70, buffino, C_WHITE);
#endif	
		return p->ch;
	}
	return '\0';
}

void md_frame_destroy(wiiScreen_t *frame)
{
	if(frame->screen != NULL) {
		int l;

		for(l = 0; l<frame->lines; l++) {
			if(frame->screen[l].chars != NULL) {
				free(frame->screen[l].chars);
			}
		}
		free(frame->screen);
		frame->screen = NULL;
	}
}

void md_screen_destroy(void)
{
	if(wiiScreen.screen != NULL) {

		md_frame_destroy(&wiiScreen);
	}
}

int md_screen_restore(void *frame)
{
	wiiScreen_t *curframe;
	int x, y;

	if(frame == NULL)
		return -1;
	curframe = (wiiScreen_t *)frame;
	if(curframe->screen != NULL) {
		for(y=0; y<curframe->lines; y++) {
			for(x=0; x<curframe->cols; x++) {
				wiiScreen.screen[y+curframe->y].chars[x+curframe->x] = curframe->screen[y].chars[x];
			}
		}
		md_frame_destroy(curframe);
		free(curframe);
	}
	return 0;
}

static int md_frame_create(wiiScreen_t *frame, int lines, int cols)
{
	int l, sz;

	sz = lines * sizeof(wiiLine_t);
	frame->screen = malloc(sz);
	if(frame->screen == NULL) {
		return -1;
	}
	frame->lines = lines;
	memset(frame->screen, 0, sz);

	sz = cols * sizeof(wiiChar_t);
	for(l = 0; l<lines; l++) {
		frame->screen[l].chars = malloc(sz);
		if(frame->screen[l].chars == NULL) {
			md_frame_destroy(frame);
			return -1;
		}
		frame->screen[l].flag = 0;
		memset(frame->screen[l].chars, 0, sz);
	}
	frame->cols = cols;
	frame->y = 0;
	frame->x = 0;
	return 0;
}

void *md_screen_clone(int l, int c, int lines, int cols)
{
	wiiScreen_t *newframe;
	int x, y;

	newframe = malloc(sizeof(wiiScreen_t));
	if(newframe == NULL)
		return NULL;
	memset(newframe, 0, sizeof(wiiScreen_t));
	if(md_frame_create(newframe, lines, cols) == -1) {
		free(newframe);
		return NULL;
	}
	for(y=0; y<newframe->lines; y++) {
		for(x=0; x<newframe->cols; x++) {
			newframe->screen[y].chars[x] = wiiScreen.screen[y+l].chars[x+c];
		}
	}
	newframe->y = l;
	newframe->x = c;
	return (void *)newframe;
}

int md_screen_create(int lines, int cols)
{
	if(wiiScreen.screen == NULL) {
		if(md_frame_create(&wiiScreen, lines, cols) == -1)
			return -1;
	}

	return 0;
}

void md_setcolors(int background, int foreground)
{
	if(background != C_CURRENT) {
		background_color = background;
	}
	if(foreground != C_CURRENT) {
		foreground_color = foreground;
	}

	ML_SetBackgroundColor(_convertColor(background_color));
	font.color = _convertColor(foreground_color);	
}

void md_getcolors(int *background, int *foreground)
{
	if(background != NULL) {
		*background = background_color;
	}
	if(foreground != NULL) {
		*foreground = foreground_color;
	}
}

void md_init(void)
{
	char s[2] = {'@', '\0'};

	ML_Init();
	ML_InitFAT();
	ML_EnableTextureAntiAliasing();

	if(fontSize == -1) {
		ML_LoadFontFromBuffer(&font, rogue_ttf, rogue_ttf_size, 16);
		font.ftKerningEnabled = 0;
		infos = ML_GetTextInfo(&font, "%s", s);
	}
	md_setcolors(C_BLACK, C_GREEN);
}

void md_onsignal_default(void)
{
	return;
}

void md_onsignal_exit(void)
{
}

void md_onsignal_autosave(void)
{
}

int md_hasclreol(void)
{
	return FALSE;
}

int md_unlink_open_file(const char *file, FILE *inf)
{
	return(unlink(file));
}

int md_unlink(const char *file)
{
	return(unlink(file));
}

int md_chmod(const char *filename, int mode)
{
	return( chmod(filename, mode) );
}

int md_getpid(void)
{
	return 77;
}

char *md_gethomedir(void)
{
#ifndef SD_HOMEDIR
	static char homedir[] = "sd:/apps/rogueWii";
#else
	static char homedir[] = SD_HOMEDIR;
#endif
	return(homedir);
}

void md_sleep(int s)
{
	sleep(s);
}

char *md_getshell(void)
{
	return NULL;
}

int md_shellescape(void)
{
	return(0);
}

int directory_exists(char *dirname)
{
    struct stat sb;

    if (stat(dirname, &sb) == 0) /* path exists */
        return (sb.st_mode & S_IFDIR);

    return(0);
}

extern char *xcrypt(char *key, char *salt);

char *md_crypt(char *key, char *salt)
{
    return key;
}

char *md_getpass(char *prompt)
{
    return NULL;
}

int md_erasechar(void)
{
	return -1;
}

int md_killchar(void)
{
	return -1;
}

int md_dsuspchar(void)
{
	return -1;
}

int md_setdsuspchar(int c)
{
	return 0;
}

int md_suspchar(void)
{
	return 0;
}

int md_setsuspchar(int c)
{
	return 0;
}

void md_ignoreallsignals(void)
{
}

void md_tstphold(void)
{
}

void md_tstpresume(void)
{
}

void md_tstpsignal(void)
{
}

/*
 * Procedures, defines and structures for rogueWii
 *
 * Copyright (C) 2019 Nebiun
 * All rights reserved.
 *
 * Use:
 *              rtn = md_menu(attr,table,numelem)
 *              int rtn;                - command code of selected option
 *              MENU_ATTR_t *attr;      - menu attibutes
 *              MENU table[];           - options
 *              int numelem;            - number of element in table
 */

int md_readchar_flags = 0;
/* define, macro e typedef valide per il file           */
#define BUFLEN  128     /* dimensione buffer temporaneo */

#define HLINE_WIN       2       /* numero di colonne occupate dal bordo */
#define HLINE_SPACE     2       /* numero di colonne (spazi) aggiuntive */
#define HLINE_KEY       3       /* numero di colonne (key) aggiuntive */
#define VLINE_WIN       2       /* numero di linee occupate dal bordo */
#define VLINE_TITLE     2       /* numero di linee occupate dal titolo */

static char **win_builder(int flags, int ncol, int szcol)
{
	static char *lines[5] = { NULL, NULL, NULL, NULL, NULL };
	static const char *special[] = { "---+","-+++","----+"," |||", " || " };
	static const char *g_special[] = { "qlkw","qtun","qmjv"," xxx", " xx " };
	const char **s;
	int sz, l;

	sz = ncol * szcol;
	if(flags & MENU_SHOW_WIN)
		sz += HLINE_KEY;

	if(flags & CHR_GRAPHVT100_MODE)
		s = g_special;
	else
		s = special;

	for(l=0; l<sizeof(lines)/sizeof(*lines); l++) {
		int c;

		if(lines[l] != NULL) {
			free(lines[l]);
		}
		lines[l] = malloc(sz);
		if(lines[l] == NULL)
			return NULL;

		memset(lines[l],s[l][0], sz-1);
		lines[l][0] = s[l][1];
		lines[l][sz-2] = s[l][2];
		if(!(flags & MENU_SHOW_TITLE) || ((l != 0) && (l != 4))) {
			char vv;

			vv = ((flags & MENU_SHOW_TITLE) && (l == 1)) ? s[0][3] : s[l][3];
			for(c=1; c<ncol; c++) {
				lines[l][c * (szcol+1)] = vv;
			}
		}
		lines[l][sz-1] = '\0';
	}
#if 0
	for(l=0; l<5; l++)
		printf("%s\n",lines[l]);
	sleep(5);
#endif
	return lines;
}

int md_menu(MENU_ATTR_t *attr, MENU_t table[], int numelem)
{
	register int i;
	register int lin,col, fix;
	int y,x,yy,xx;
	int szcol, sztot;
	int optnew, optcur;
	int optlin;
	int rtcode;
	int go;
	int flags;
	void *user = NULL;

	if((attr == NULL) || (table == NULL) || (numelem <= 0)) {
		errno = EINVAL;
		return -1;
	}
	if((attr->input == NULL) || (attr->output == NULL)) {
		errno = ENOSYS;
		return -1;
	}
	if((attr->pre_menu != NULL) && (attr->post_menu == NULL)) {
		errno = EPERM;
		return -1;
	}

	if(attr->get_charattrs)
		flags = attr->get_charattrs() & CHR_MODE_MASK;
	else
		flags = 0;
	flags |= (attr->flags & MENU_FLAGS_MASK);

	/* calcola il numero di linee e       */
	/* (eventualmente) colonne necessarie */
	fix = 0;
	col = 1;
//	md_debug_printf("FLAGS = 0x%x",attr->flags);
	if(attr->flags & MENU_SHOW_TITLE) {
		if(attr->title == NULL) {
			errno = EINVAL;
			return -1;
		}
		fix += VLINE_TITLE;
	}
	if(attr->flags & MENU_SHOW_WIN)
		fix += VLINE_WIN;

	lin = fix + numelem;
	if(lin > attr->screen_maxlin) {
		int newlin;

		do {
			col++;
			newlin = fix + numelem/col + ((numelem%col) ? 1 : 0);
		} while(newlin > attr->screen_maxlin);
		lin = newlin;
	}

	/* calcolo dimensione minima colonna */
	/* e verifico di aver spazio per le  */
	/* colonne necessarie.               */
	/* E  intanto controllo che ci siano */
	/* opzioni disponibili.              */
	if(attr->flags & MENU_SHOW_TITLE) {
		szcol = strlen(attr->title);
		if(attr->flags & MENU_SHOW_WIN)
			szcol += HLINE_SPACE;
	}
	else
		szcol = 0;
	optnew = -1;
	for(i=0;i<numelem;i++) {
		if(szcol<strlen(table[i].entry))
			szcol = strlen(table[i].entry);
		if(((optnew == -1) && (table[i].attri & OPT_DSP)) ||
		   ((table[i].attri & OPT_DFL) && !(table[optnew].attri & OPT_DFL))) {
			optnew = i;
		}
	}
	if(optnew == -1) {
		errno = EINVAL;
		return -1;
	}
	/* Spazio prima e dopo l'opzione */
	szcol += HLINE_SPACE;

	if(attr->flags & MENU_SHOW_KEY) {
		szcol += HLINE_KEY;
	}

	sztot = szcol * col;

	/* Spazio tra le colonne */
	sztot += col - 1;
	if(attr->flags & MENU_SHOW_WIN)
		sztot += HLINE_WIN;

	if(sztot > attr->screen_maxcol) {
		errno = EFBIG;
		return -1;
	}

	/* Coordinate 0,0 del menu */
	y = 1 + (attr->screen_maxlin - lin)/2;
	x = 1 + (attr->screen_maxcol - sztot)/2;

//	logDebug("menu %d %d w %d h %d\n", y, x, lin, sztot);
	if(attr->pre_menu != NULL) {
		if( (user = attr->pre_menu(y, x, lin, sztot)) == NULL) {
			return -1;
		}
	}

	if(attr->flags & MENU_SHOW_WIN) {
		char **win;
		int reqmode, l;

		win = win_builder(flags, col, szcol);
		if(win == NULL) {
			return -1;
		}
	//	logDebug("graph len %d\n",strlen(win[0]));
		reqmode = flags & (CHR_GRAPHVT100_MODE|CHR_BOLD_MODE);
		if(attr->set_charattrs != NULL)
			attr->set_charattrs(reqmode,-1);
		l = y;
		attr->output(l++,x,attr->color,"%s",win[0]);
		if(attr->flags & MENU_SHOW_TITLE) {
			attr->output(l++,x,attr->color,"%s",win[4]);
			attr->output(l++,x,attr->color,"%s",win[1]);
		}
		for(i=0; i<lin-fix; i++) {
			attr->output(l++,x,attr->color,"%s",win[3]);
		}
		attr->output(l++,x,attr->color,"%s",win[2]);
		if(attr->set_charattrs != NULL)
			attr->set_charattrs(CHR_RESET_MODE,-1);
	}

	if(attr->flags & MENU_SHOW_WIN) {
		yy = y + 1;
		xx = x + 1;
	}
	else {
		yy = y;
		xx = x;
	}

	if(attr->flags & MENU_SHOW_TITLE) {
		attr->output(yy, xx, attr->color," %s", attr->title);
		yy += VLINE_TITLE;
	}
	optlin = lin - fix;
	for(i=0; i<numelem; i++) {
		if(attr->flags & MENU_SHOW_KEY) {
			attr->output(yy+(i%optlin),xx+((i/optlin)*(szcol+1)),
				attr->color,
				" %c) %s", table[i].code, table[i].entry);
		}
		else {
			attr->output(yy+(i%optlin),xx+((i/optlin)*(szcol+1)),
				attr->color,
				" %s", table[i].entry);
		}
	}

	optcur = optnew;
	go = 0;
	do {
		if(attr->flags & MENU_SHOW_KEY) {
			attr->output(yy+(optnew%optlin),xx+((optnew/optlin)*(szcol+1)),
				(attr->alt_color != -1) ? attr->alt_color : attr->color,
				"%c%c) %s", (attr->flags & MENU_SHOW_CHOICE) ? '>' : ' ',
				table[optnew].code, table[optnew].entry);
		}
		else {
			attr->output(yy+(optnew%optlin),xx+((optnew/optlin)*(szcol+1)),
				(attr->alt_color != -1) ? attr->alt_color : attr->color,
				"%c%s", (attr->flags & MENU_SHOW_CHOICE) ? '>' : ' ',
				table[optnew].entry);
		}
		rtcode = attr->input();
		if(rtcode == attr->up_keyval) {
			while(optnew>=0) {
				optnew--;
				if(optnew<0)
					optnew = numelem-1;
				if(table[optnew].attri & OPT_DSP)
					break;
			}
		}
		else if(rtcode == attr->down_keyval) {
			while(optnew<numelem) {
				optnew++;
				if(optnew>=numelem)
					optnew = 0;
				if(table[optnew].attri & OPT_DSP)
					break;
			}
		}
		else if(rtcode == attr->confirm_keyval) {
			go = 1;
		}
		else if(rtcode == attr->abort_keyval) {
			go = -1;
		}
		if(optnew != optcur) {
			if(attr->flags & MENU_SHOW_KEY) {
				attr->output(yy+(optcur%optlin),xx+((optcur/optlin)*(szcol+1)),
					attr->color,
					" %c) %s", table[optcur].code, table[optcur].entry);
			}
			else {
				attr->output(yy+(optcur%optlin),xx+((optcur/optlin)*(szcol+1)),
					attr->color,
					" %s", table[optcur].entry);
			}
			optcur = optnew;
		}
	} while(go == 0);

	if((user != NULL) && (attr->post_menu != NULL)) {
		if(attr->post_menu(user) == -1) {
			return -1;
		}
	}
	if(go == -1) {
		errno = EAGAIN;
		return -1;
	}

	return table[optcur].code;
}

int md_menu_input(void)
{
	int ch = '\0';

	md_readchar_flags = 1;
	ch = md_readchar();
	md_readchar_flags = 0;

	return ch;
}

int md_menu_output(int y, int x, int color, const char *fmt, ...)
{
	char line[128];
	va_list ap;

	va_start(ap,fmt);
	vsnprintf(line,sizeof(line),fmt,ap);
	va_end(ap);

	md_putstr_at(y, x, line, color);

	return 0;
}

static int md_get_altcolor(int color)
{
	int b, f, c;
	
	md_getcolors(&b,&f);
	
	switch(b) {
	case C_BLACK:
		c = (color == C_WHITE) ? C_CYAN : C_WHITE;
		break;
	case C_RED:
		c = (color == C_WHITE) ? C_YELLOW : C_WHITE;
		break;
	case C_GREEN:
		c = (color == C_BLUE) ? C_BLACK : C_BLUE;
		break;
	case C_YELLOW:
		c = (color == C_BLACK) ? C_RED : C_BLACK;
		break;
	case C_BLUE:
		c = (color == C_WHITE) ? C_CYAN : C_WHITE;
		break;		
	case C_MAGENTA:
		c = (color == C_WHITE) ? C_BLUE : C_WHITE;
		break;
	case C_CYAN:
		c = (color == C_BLUE) ? C_BLACK : C_BLUE;
		break;		
	case C_WHITE:
		c = (color == C_BLACK) ? C_BLUE : C_BLACK;
		break;
	case C_GREY:
		c = (color == C_BLUE) ? C_BLACK : C_BLUE;
		break;
	default:
		c = f;
		break;
	}
	return c;
}

int md_stdmenu(MENU_t table[], const char *title, int numelem)
{
	MENU_ATTR_t attr = {
		md_menu_input,		// input
		md_menu_output,		// output
		md_screen_clone,	// pre
		md_screen_restore,	// post
		NULL,				// get attrs
		NULL,				// set attrs
		NULL,				// title
		C_CURRENT,			// color
		C_CURRENT,			// alternative color
		0,					// max cols
		0,					// max lines
		MENU_SHOW_WIN|MENU_SHOW_CHOICE,	// flags
		RC_KEY_UP,			// up
		RC_KEY_DOWN,		// down
		0,					// left
		0,					// right
		RC_KEY_CONFIRM,		// confirm
		RC_KEY_ABORT		// abort
	};

	attr.screen_maxlin = wiiScreen.lines;
	attr.screen_maxcol = wiiScreen.cols;
	attr.title = title;
	if(attr.title != NULL)
		attr.flags |= MENU_SHOW_TITLE;
	md_getcolors(NULL,&attr.color);
	attr.alt_color = md_get_altcolor(attr.color);
	return md_menu(&attr, table, numelem);
}

int md_cmdmenu(void)
{
	static MENU_t commands[] = {
		{RC_KEY_PICKUP, OPT_DSP, "Pick-up something"},
		{RC_KEY_FIGHT, OPT_DSP, "Fight until someone dies"},
		{RC_KEY_MOVE, OPT_DSP, "Move without picking up"},
		{RC_KEY_ZAP, OPT_DSP, "Zap with a staff or wand"},
		{RC_KEY_TRAP, OPT_DSP, "Identify a trap"},
		{RC_KEY_INVENTORY, OPT_DSP, "Inventory"},
		{RC_KEY_QUAFF, OPT_DSP, "Quaff a potion"},
		{RC_KEY_READ, OPT_DSP, "Read a scroll"},
		{RC_KEY_EAT, OPT_DSP, "Eat food"},
		{RC_KEY_WIELD, OPT_DSP, "Wield a weapon"},
		{RC_KEY_WEAR, OPT_DSP, "Wear armor"},
		{RC_KEY_TAKEOFF, OPT_DSP, "Take off armor"},
		{RC_KEY_PUTON, OPT_DSP, "Put on a ring"},
		{RC_KEY_REMOVE, OPT_DSP, "Remove a ring"},
		{RC_KEY_DROP, OPT_DSP, "Drop an object"},
		{RC_KEY_RENAME, OPT_DSP, "Rename an object"},
		{RC_KEY_DISPLAY, OPT_DSP, "Print out discovered object"},
		{RC_KEY_SAVE, OPT_DSP, "Save the game"}
	};
	
	return md_stdmenu(commands, NULL, sizeof(commands)/sizeof(*commands));
}

int md_popup(const char *str[], int n)
{
	MENU_ATTR_t attr = {
		md_menu_input,		// input
		md_menu_output,		// output
		md_screen_clone,	// pre
		md_screen_restore,	// post
		NULL,				// get attrs
		NULL,				// set attrs
		NULL,				// title
		C_CURRENT,			// color
		C_CURRENT,			// alternative color
		0,					// max cols
		0,					// max lines
		MENU_SHOW_WIN,		// flags
		0,					// up
		0,					// down
		0,					// left
		0,					// right
		RC_KEY_CONFIRM,		// confirm
		0					// abort
	};
	MENU_t *list;
	int i, ch;
	
	list = malloc(sizeof(MENU_t) * (n + 1));
	if(list == NULL)
		return -1;

	for(i=0; i<n; i++) {
		list[i].code = RC_KEY_NONE;
		list[i].attri = OPT_NDS;
		list[i].entry = str[i];
	}
	list[n].code = RC_KEY_CONTINUE;
	list[n].attri = OPT_DSP|OPT_DFL;
	list[n].entry = RS_PRESS_KEY_TO_CONTINUE;
	
	attr.screen_maxlin = wiiScreen.lines;
	attr.screen_maxcol = wiiScreen.cols;
	attr.title = NULL;
	md_getcolors(NULL,&attr.color);
	attr.alt_color = md_get_altcolor(attr.color);
	ch = md_menu(&attr, list, n+1);
	free(list);
	return ch;
}

void md_help(void)
{
	const char *str[] = {
		"Remenber: hold the Wiimote horizontally",
		"",
		"Use UP and DOWN to move vertically",
		"Use UP+LEFT, UP+RIGHT,",
		"    DOWN+LEFT, DOWN+RIGHT to move diagonally",
		"Use A and an above combination to run",
		"Use B and an above combination to run",
  		"    (little difference, try it)",
		"Use 2 when asked or to select a menu item",
		"Use 1 for the commands menu",
		"Use + to search for traps or secret doors",
		"Use + and A to climb down a staircase",
		"Use + and B to do nothing (relax)",
		"Use - to throw something",
		"Use - and A to climb up a staircase",
		"Use - and B to see this help",
		""
	};
	
	md_popup(str, sizeof(str)/sizeof(*str));
}

#define MD_NONE		0
#define MD_CTRL		01
#define MD_SHIFT	02

static int _md_readchar(void)
{
	int _ch_flags = MD_NONE;
	int ch = '\0';

	// direzioni
	// 2 = shift
	// 1 = ctrl
	// home
	// + = on/off pointer
	// - =
	// a = space
	// b = menu
	md_refresh();
	if(Wiimote[0].Held.Home) {
		ch = RC_KEY_ABORT;
	}
	else {
		if(md_readchar_flags == 0) {
			if(Wiimote[0].Held.A) {
				_ch_flags |= MD_CTRL;
			}
			if(Wiimote[0].Held.B) {
				_ch_flags |= MD_SHIFT;
			}
			if(Wiimote[0].Held.Up && Wiimote[0].Held.Right) {
				if(_ch_flags & MD_CTRL)
					ch = CHR_CTRL(RC_KEY_UPANDLEFT);
				else if(_ch_flags & MD_SHIFT)
					ch = CHR_UP(RC_KEY_UPANDLEFT);
				else
					ch = RC_KEY_UPANDLEFT;
			}
			else if(Wiimote[0].Held.Down && Wiimote[0].Held.Right) {
				if(_ch_flags & MD_CTRL)
					ch = CHR_CTRL(RC_KEY_UPANDRIGHT);
				else if(_ch_flags & MD_SHIFT)
					ch = CHR_UP(RC_KEY_UPANDRIGHT);
				else
					ch = RC_KEY_UPANDRIGHT;
			}
			if(Wiimote[0].Held.Up && Wiimote[0].Held.Left) {
				if(_ch_flags & MD_CTRL)
					ch = CHR_CTRL(RC_KEY_DOWNANDLEFT);
				else if(_ch_flags & MD_SHIFT)
					ch = CHR_UP(RC_KEY_DOWNANDLEFT);
				else
					ch = RC_KEY_DOWNANDLEFT;
			}
			else if(Wiimote[0].Held.Down && Wiimote[0].Held.Left) {
				if(_ch_flags & MD_CTRL)
					ch = CHR_CTRL(RC_KEY_DOWNANDRIGHT);
				else if(_ch_flags & MD_SHIFT)
					ch = CHR_UP(RC_KEY_DOWNANDRIGHT);
				else
					ch = RC_KEY_DOWNANDRIGHT;
			}
		}

		if(ch == '\0') {
			if(Wiimote[0].Held.Right) {
				if(_ch_flags & MD_CTRL)
					ch = CHR_CTRL(RC_KEY_UP);
				else if(_ch_flags & MD_SHIFT)
					ch = CHR_UP(RC_KEY_UP);
				else
					ch = RC_KEY_UP;
			}
			else if(Wiimote[0].Held.Left) {
				if(_ch_flags & MD_CTRL)
					ch = CHR_CTRL(RC_KEY_DOWN);
				else if(_ch_flags & MD_SHIFT)
					ch = CHR_UP(RC_KEY_DOWN);
				else
					ch = RC_KEY_DOWN;
			}
			else if(Wiimote[0].Held.Up) {
				if(_ch_flags & MD_CTRL)
					ch = CHR_CTRL(RC_KEY_LEFT);
				else if(_ch_flags & MD_SHIFT)
					ch = CHR_UP(RC_KEY_LEFT);
				else
					ch = RC_KEY_LEFT;
			}
			else if(Wiimote[0].Held.Down) {
				if(_ch_flags & MD_CTRL)
					ch = CHR_CTRL(RC_KEY_RIGHT);
				else if(_ch_flags & MD_SHIFT)
					ch = CHR_UP(RC_KEY_RIGHT);
				else
					ch = RC_KEY_RIGHT;
			}
		}

		if(Wiimote[0].Held.Two) {
			ch = RC_KEY_CONTINUE;
		}

		if(md_readchar_flags == 0) {
			if(Wiimote[0].Held.One) {
				if( (ch = md_cmdmenu()) == -1) {
					ch = RC_KEY_NONE;
				}
			}
			if(Wiimote[0].Held.Plus) {
				if(_ch_flags & MD_SHIFT)
					ch = RC_KEY_CLIMBDOWN;	// Climb down a staircase to the next level.
				else if(_ch_flags & MD_CTRL)
					ch = RC_KEY_REST;	// Rest.
				else
					ch = RC_KEY_SEARCH;	// Search for traps and secret doors.
			}
			if(Wiimote[0].Held.Minus) {
				if(_ch_flags & MD_SHIFT)
					ch = RC_KEY_CLIMBUP;	// Climb up
				else if(_ch_flags & MD_CTRL)
					md_help();
				else {
					ch = RC_KEY_THROW;     // Throw something
				}
			}
		}
	}
	
	return ch;
}

int md_readchar(void)
{
	int ch;

	do {
		ch = _md_readchar();
	} while(ch == '\0');
	
	while(_md_readchar() == ch);	// wait release
	
	return ch;
}

void md_debug_printf(const char *fmt, ...)
{
	va_list args;
	char debug_buf[2048];

	va_start(args, fmt);
	vsnprintf(debug_buf,sizeof(debug_buf)-1,fmt,args);
	va_end(args);

	while(!Wiimote[0].Held.A) {
		ML_DrawText(&font, X_START + 1 * infos.width, Y_START + 1 * infos.height, "%s", debug_buf);
		ML_Refresh();
	}
	while(Wiimote[0].Held.A)
		ML_Refresh();
}

int md_getnstr(GETSTR_t *attr, char *str, int max)
{
	int c;
	int i = 0, go = 0;
	int rtcode;
	int u = -1, l = -1;

	if(attr->range != NULL) {
		u = 0;
		l = strlen(attr->range);
		c = attr->range[0];
	}
	else {
		c = '*';
	}

	i = 1;
	do {
		str[i-1] = c;
		str[i] = '\0';
		attr->output(attr->y,attr->x,attr->color,"%s",str);

		rtcode = attr->input();
		if(rtcode == attr->up_keyval) {
			if(u != -1) {
				u++;
				if(u >= l)
					u = 0;
				c = attr->range[u];
			}
			else {
				c++;
				while(!isprint(c)) {
					c++;
					if(c > 0xff)
						c = 0;
				}
			}
		}
		else if(rtcode == attr->down_keyval) {
			if(u != -1) {
				u--;
				if(u < 0)
					u = l-1;
				c = attr->range[u];
			}
			else {
				c--;
				while(!isprint(c)) {
					c--;
					if(c < 0)
						c = 0xff;
				}
			}
		}
		else if(rtcode == attr->left_keyval) {
			if(i > 1) {
				attr->output(attr->y,attr->x,attr->color,"%*s",i,"");
				i--;
				c = str[i];
			}
		}
		else if(rtcode == attr->right_keyval) {
			if(i < max) {
				str[i] = c;
				i++;
			}
		}
		else if(rtcode == attr->confirm_keyval) {
			go = 1;
		}
		else if(rtcode == attr->abort_keyval) {
			i = 0;
			go = -1;
		}
	} while(!go);

	str[i] = '\0';
	return i;
}

void md_exit(int val)
{
	ML_Exit();
}

