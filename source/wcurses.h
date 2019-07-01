#ifndef _wcurses_h_
#define _wcurses_h_
#include <stdint.h>
#include <stdarg.h>
#include <MLlib.h>

#define C_CURRENT	-1
#define C_BLACK		0x00000000
#define C_RED		0x00000080
#define C_GREEN		0x0080ff00
#define C_YELLOW	0x0000ffd7
#define C_BLUE		0x00800000
#define C_MAGENTA	0x00800080
#define C_CYAN		0x00ffff00
#define C_WHITE		0x00ffffff
#define C_GREY		0x00808080

#ifndef global
#define global extern
#endif

#define OK      0
#define ERR     -1
#define TRUE    1
#define FALSE   0

typedef uint32_t chtype;
typedef chtype   attr_t;

typedef struct {
	wchar_t	value;
	u_int color;
} charScreen_t;

typedef struct {
	charScreen_t *line;
	u_int flags;
} lineScreen_t;

typedef struct {
	u_short _nlines;
	u_short _ncols;
	u_short _cury;              /* current y cursor position */
	u_short _curx;              /* current x cursor position */
	/* window location and size */
	u_short _y, _x;
	u_short _maxy, _maxx;       /* maximums of x and y, NOT window size */
	u_short _flags;
	attr_t  _attrs;         /* current attribute for non-space character */
	u_int _color;
	u_int _background;
	lineScreen_t *_screen;
	char *_tmpLine;
} WINDOW;

typedef struct {


} SCREEN;

typedef struct {

} TERMINAL;

#define W_FULLWIN		0x0
#define W_SUBWIN		0x1

#define A_NORMAL        0x0
#define A_ATTRIBUTES    0xff00
#define A_CHARTEXT      0xff
#define A_STANDOUT      (1 << 16)

global int COLS;
global int LINES;
global int ESCDELAY;

global WINDOW *curscr;
global WINDOW *stdscr;
global TERMINAL *cur_term;

extern int baudrate(void);
extern int clearok(WINDOW *win, bool bf);
extern int delwin(WINDOW *win);
extern int endwin(void);
extern char erasechar(void);
extern int flushinp(void);
extern int halfdelay(int tenths);
extern int idlok(WINDOW *win, bool bf);
extern WINDOW *initscr(void);
extern bool isendwin(void);
extern int keypad(WINDOW *win, bool bf);
extern char killchar(void);
extern int leaveok(WINDOW *win, bool bf);
extern int mvcur(int oldrow, int oldcol, int newrow, int newcol);
extern int mvprintw(int y, int x, const char *fmt, ...);
extern int mvwin(WINDOW *win, int y, int x);
extern int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...);
extern WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
extern int nocbreak(void);
extern int noecho(void);
extern int printw(const char *fmt, ...);
extern int raw(void);
extern WINDOW *subwin(WINDOW *orig, int nlines, int ncols, int begin_y, int begin_x);
extern char *unctrl(chtype c);
extern int waddch(WINDOW *win, const chtype ch);
extern int waddnstr(WINDOW *win, const char *str, int n);
extern int wclear(WINDOW *win);
extern int wclrtoeol(WINDOW *win);
extern int werase(WINDOW *win);
extern int wgetch(WINDOW *win);
extern int wgetnstr(WINDOW *win, char *str, int n);
extern int wgetnstre(WINDOW *win, char *str, int n, const char *range);
extern chtype winch(WINDOW *win);
extern int wmove(WINDOW *win, int y, int x);
extern int wprintw(WINDOW *win, const char *fmt, ...);
extern int wrefresh(WINDOW *win);
extern int wtouchln(WINDOW *win, int y, int n, int changed);

/* pseudo-routine */
#define mvwaddch(win,y,x,ch)            (wmove(win,y,x) == ERR ? ERR : waddch(win,ch))
#define getcurx(win)                    ((win) ? (win)->_curx : ERR)
#define getcury(win)                    ((win) ? (win)->_cury : ERR)
#define getmaxy(win)                    ((win) ? ((win)->_maxy + 1) : ERR)
#define getmaxx(win)                    ((win) ? ((win)->_maxx + 1) : ERR)
#define waddstr(win,str)                waddnstr(win,str,-1)
#define touchwin(win)                   wtouchln((win), 0, getmaxy(win), 1)
#define refresh()                       wrefresh(stdscr)
#define mvaddch(y,x,ch)                 mvwaddch(stdscr,y,x,ch)
#define getyx(win,y,x)                  (y = getcury(win), x = getcurx(win))
#define mvwinch(win,y,x)                (wmove(win,y,x) == ERR ? ERR : winch(win))
#define addstr(str)                     waddnstr(stdscr,str,-1)
#define addch(ch)                       waddch(stdscr,ch)
#define wstandend(win)                  (wattrset(win,A_NORMAL))
#define standend()                      wstandend(stdscr)
#define wattrset(win,at)                ((win)->_attrs = (at))
#define wstandout(win)                  (wattrset(win,A_STANDOUT))
#define standout()                      wstandout(stdscr)
#define inch()                          winch(stdscr)
#define move(y,x)                       wmove(stdscr,y,x)
#define mvwaddstr(win,y,x,str)          (wmove(win,y,x) == ERR ? ERR : waddnstr(win,str,-1))
#define mvaddstr(y,x,str)               mvwaddstr(stdscr,y,x,str)
#define clear()                         wclear(stdscr)
#define clrtoeol()                      wclrtoeol(stdscr)
#define mvinch(y,x)                     mvwinch(stdscr,y,x)
#define getnstr(str,n)					wgetnstr(stdscr, str, n)
#define getnstre(str, n, range)			wgetnstre(stdscr, str, n, range)

#endif
