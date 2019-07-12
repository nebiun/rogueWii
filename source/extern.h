/*
 * Defines for things used in mach_dep.c
 *
 * @(#)extern.h	4.35 (Berkeley) 02/05/99
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

/*
 * Don't change the constants, since they are used for sizes in many
 * places in the program.
 */

#ifndef _extern_h_
#define _extern_h_
#include <stdbool.h>
#include <stdlib.h>

#undef SIGTSTP

#define MAXSTR		1024	/* maximum length of strings */
#define MAXLINES	32	/* maximum number of screen lines used */
#define MAXCOLS		80	/* maximum number of screen columns used */

#ifndef N_LINES
#define N_LINES 	30  /* must be <= MAXLINES */
#endif
#ifndef N_COLS
#define N_COLS		80	/* must be <= MAXCOLS */
#endif

#define RN		(((seed = seed*11109+13849) >> 16) & 0xffff)
#ifdef CTRL
#undef CTRL
#endif

#include <stdio.h>
#define CTRL(c)		(c & 037)

#if !defined(TRUE) || !defined(FALSE)
#undef TRUE
#undef FALSE
#define TRUE	1
#define FALSE	0
#endif

/*
 * Now all the global variables
 */

extern bool	got_ltc, in_shell;
extern int	wizard;
extern char	fruit[], prbuf[], whoami[];
extern int orig_dsusp;
extern FILE	*scoreboard;

/*
 * Function types
 */

void    auto_save(int);
void	come_down();
void	doctor();
void	end_line();
void    endit(int sig);
void	fatal();
void	getltchars();
void	land();
void    leave(int);
void	my_exit();
void	nohaste();
void	playit();
void    playltchars(void);
void	print_disc(char);
void    quit(int);
void    resetltchars(void);
void	rollwand();
void	runners();
void	set_order();
void	sight();
void	stomach();
void	swander();
void	tstp(int ignored);
void	unconfuse();
void	unsee();
void	visuals();

char	add_line(char *fmt, char *arg);

char	*killname(char monst, bool doart);
char	*nothing(char type);
char	*type_name(int type);

#ifdef CHECKTIME
int	checkout();
#endif

int	md_chmod(const char *filename, int mode);
char	*md_crypt(char *key, char *salt);
int	md_dsuspchar();
int	md_erasechar();
char	*md_gethomedir();
char	*md_getusername();
int	md_getuid();
char	*md_getpass(char *prompt);
int	md_getpid();
char	*md_getrealname(int uid);
void	md_init();
int	md_killchar();
void	md_normaluser();
void	md_raw_standout();
void	md_raw_standend();
extern int	md_readchar(void);
int	md_setdsuspchar(int c);
int	md_shellescape();
void	md_sleep(int s);
int	md_suspchar();
int	md_hasclreol();
int	md_unlink(const char *file);
int	md_unlink_open_file(const char *file, FILE *inf);
void md_tstpsignal();
void md_tstphold();
void md_tstpresume();
void md_ignoreallsignals();
void md_onsignal_autosave();
void md_onsignal_exit();
void md_onsignal_default();
int md_issymlink(const char *sp);

/*
 * Procedures, defines and structures for rogueWii
 *
 * Copyright (C) 2019 Nebiun
 * All rights reserved.
 */
#define ROGUE_WII	1966
#define NAME_CHARS  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789#()+-. !@[]^_{}~"

#define MD_FONT_STD	0
#define MD_FONT_BIG 1

#define OPT_NDS 0       /* opzione non disponibile */
#define OPT_DSP 01      /* opzione disponibile */
#define OPT_DFL 02      /* opzione predefinita */

typedef struct {
	char    code;           /* codice di selezione (carattere) */
	char    attri;          /* OPT_DSP, OPT_NDS */
	const char *entry;      /* stringa opzione */
} MENU_t;

#define CHR_RESET_MODE          0x0000
#define CHR_GRAPHVT100_MODE     0x0001
#define CHR_BOLD_MODE           0x0002
#define CHR_REVERSE_MODE        0x0004
#define CHR_BLINK_MODE          0x0008
#define CHR_UNDERLINE_MODE      0x0010
#define CHR_COLOR_MODE          0x0020
#define CHR_MODE_MASK           0x00ff  // OR of previous values

#define MENU_SHOW_WIN           0x0100
#define MENU_SHOW_KEY           0x0200
#define MENU_SHOW_TITLE         0x0400
#define MENU_SHOW_CHOICE        0x0800
#define MENU_FLAGS_MASK         0xff00  // OR of previous values

typedef struct {
	int     (*input)(void);
	int     (*output)(int line, int col, int color, const char *fmt, ...);
	void   *(*pre_menu)(int y, int x, int lines, int cols);
	int     (*post_menu)(void *);
	int     (*get_charattrs)(void);
	void    (*set_charattrs)(int mask, int color);
	const char *title;
	int     color;
	int 	alt_color;
	int     screen_maxcol;
	int     screen_maxlin;
	int     flags;
	int     up_keyval;
	int     down_keyval;
	int     left_keyval;
	int     right_keyval;
	int     confirm_keyval;
	int     abort_keyval;
} MENU_ATTR_t;

typedef struct {
	int     (*input)(void);
	int     (*output)(int line, int col, int color, const char *fmt, ...);
	const char *range;
	int     y, x;
	int		color;
	int     up_keyval;
	int     down_keyval;
	int     left_keyval;
	int     right_keyval;
	int     confirm_keyval;
	int     abort_keyval;
} GETSTR_t;

extern int md_readchar_flags;
extern int md_screen_create(int lines, int cols);
extern void md_putstr_at(int y, int x, char *s, int color);
extern void md_debug_printf(const char *fmt, ...);
extern void md_refresh(void);
extern void md_exit(int val);
extern int md_stdmenu(MENU_t table[], const char *title, int numelem);
extern int md_menu_input(void);
extern int md_menu_output(int y, int x, int color, const char *fmt, ...);
extern int md_getnstr(GETSTR_t *attr, char *str, int n);
extern int logDebug(const char *fmt, ...);
extern void md_setcolors(int background, int foreground);
extern void md_getcolors(int *background, int *foreground);
extern int md_setfont(int f);
#endif
