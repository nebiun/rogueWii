#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "extern.h"
#include "rogue_str.h"
#define global
#include "wcurses.h"

//#define WCURSES_DEBUG	1
#define WCURSES_CSPACE  ' '

int COLS = N_COLS;
int LINES = N_LINES;
int ESCDELAY;

static char lbuf[2048];

/*
 * The baudrate routine returns the output speed of the terminal
 */
int baudrate(void)
{
	return 9600;
}

/*
 * The nocbreak routine returns the terminal to normal (cooked) mode.
 */
int nocbreak(void)
{
	return OK;
}

/* The echo and noecho routines control whether characters typed by the
 * user are echoed by getch as they are typed.
 */
int noecho(void)
{
	return OK;
}

/*
 * The raw and noraw routines place the terminal into or out of raw mode.
 */
int raw(void)
{
	return OK;
}

/*
 * If clearok is called with TRUE as argument, the next call to wrefresh
 * with this window will clear the screen completely and redraw the entire
 * screen from scratch. This is useful when the contents of the screen
 * are uncertain, or in some cases for a more pleasing visual effect. If
 * the win argument to clearok is the global variable curscr, the next
 * call to wrefresh with any window causes the screen to be cleared and
 * repainted from scratch.
 */
int clearok(WINDOW *win, bool bf)
{
	if(win == NULL)
		return ERR;
	return OK;
}

/*
 * Calling delwin deletes the named window, freeing all memory associated with it
 * (it does not actually erase the window's screen image).
 * Subwindows must be deleted before the main window can be deleted.
 */
int delwin(WINDOW *win)
{
	int i;

	if(win != NULL) {
		if(win->_screen != NULL) {
			if(win->_flags == W_FULLWIN) {
				for(i=0; i< win->_nlines; i++) {
					if(win->_screen[i].line == NULL)
						break;
					free(win->_screen[i].line);
				}
			}
			free(win->_screen);
		}
		free(win);
		if(win == curscr)
			curscr = NULL;
		if(win == stdscr)
			stdscr = NULL;
	}
	return OK;
}

int endwin(void)
{
	return OK;
}

/*
 * The erasechar routine returns the user’s current erase character.
 */
char erasechar(void)
{
	return '\b';
}

/*
 * The flushinp routine throws away any typeahead that has been typed by
 * the user and has not yet been read by the program.
 */
int flushinp(void)
{
	return OK;
}

/*
 * The  halfdelay routine is used for half-delay mode, which is similar to
 * cbreak mode in that characters typed by the user are immediately
 * available to the program. However, after blocking for tenths tenths
 * of seconds, ERR is returned if nothing has been typed. The value of tenths
 * must be a number between 1 and 255.
 */
int halfdelay(int tenths)
{
	return OK;
}

/*
 * If idlok is called with TRUE as second argument, curses considers using
 * the hardware insert/delete line feature of terminals so equipped.
 * Calling idlok with FALSE as second argument disables use of line
 * insertion and deletion. This option should be enabled only if the
 * application needs insert/delete line, for example, for a screen editor.
 * It is disabled by default because insert/delete line tends to be visually
 * annoying when used in applications where it is not really needed. If
 * insert/delete line cannot be used, curses redraws the changed portions
 * of all lines.
 */
int idlok(WINDOW *win, bool bf)
{
	return OK;
}

/*
 * The isendwin routine returns TRUE if endwin has been called without any
 * subsequent calls to wrefresh, and FALSE otherwise.
 */
bool isendwin(void)
{
	return FALSE;
}

/*
 * The keypad option enables the keypad of the user's  terminal.
 * If enabled (bf is TRUE), the user can press a function key (such as an
 * arrow key) and wgetch returns a single value representing the function
 * key, as in KEY_LEFT. If disabled (bf is FALSE), curses does not treat
 * function keys specially and the program has to interpret the escape
 * sequences itself. If the keypad in the terminal can be turned on (made
 * to transmit) and off (made to work locally), turning on this option
 * causes the terminal keypad to be turned on when wgetch is called. The
 * default value for keypad is false.
 */
int keypad(WINDOW *win, bool bf)
{
	if(win == NULL)
		return ERR;

	return OK;
}

/*
 * The killchar routine returns the user's current line kill character.
 */
char killchar(void)
{
	return '\030';
}

/*
 * Normally, the hardware cursor is left at the location of the window
 * cursor being refreshed. The leaveok option allows the cursor to be
 * left wherever the update happens to leave it. It is useful for
 * applications where the cursor is not used, since it reduces the need for
 * cursor motions.
 */
int leaveok(WINDOW *win, bool bf)
{
	if(win == NULL)
		return ERR;

	return OK;
}

/*
 * Calling mvwin moves the window so that the upper left-hand corner is at
 * position (x, y). If the move would cause the window to be off the
 * screen, it is an error and the window is not moved.
 * Moving subwindows is allowed, but should be avoided.
 */
int mvwin(WINDOW *win, int y, int x)
{
	if((win == NULL) || (x < 0) || (y < 0))
		return ERR;

	win->_y = y;
	win->_x = x;
	win->_maxy = win->_y + win->_nlines - 1;
	win->_maxx = win->_x + win->_ncols -1;

	return OK;
}

/*
 * Calling newwin creates and returns a pointer to a new window with the
 * given number of lines and columns. The upper left-hand corner of the
 * window is at line begin_y, column begin_x. If either nlines or ncols
 * is zero, they default to LINES - begin_y and COLS - begin_x.
 * A new full-screen window is created by calling newwin(0,0,0,0).
 */
WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x)
{
	WINDOW *win;
	int i, sz;

	win = malloc(sizeof(WINDOW));
	if(win == NULL)
		return NULL;
	win->_y = begin_y;
	win->_x = begin_x;
	win->_nlines = (nlines == 0) ? (LINES - win->_y) : nlines;
	win->_ncols = (ncols == 0) ? (COLS - win->_x) : ncols;

	win->_maxy = win->_y + win->_nlines - 1;
	win->_maxx = win->_x + win->_ncols -1;
	win->_cury = 0;
	win->_curx = 0;
	win->_attrs = A_NORMAL;
	win->_flags = W_FULLWIN;
	sz = win->_nlines * sizeof(lineScreen_t);
	win->_screen = malloc(sz);
	if(win->_screen == NULL) {
		delwin(win);
		return NULL;
	}
	sz = win->_ncols * sizeof(charScreen_t);
	for(i=0; i<win->_nlines; i++) {
		lineScreen_t *p = &win->_screen[i];

		p->line = malloc(sz);
		if(p->line == NULL) {
			delwin(win);
			return NULL;
		}
		p->flags = 0;
	}
	md_getcolors((int *)&win->_background, (int *)&win->_color);
	win->_altcolor = md_get_altcolor();
	win->_curcolor = win->_color;
	return win;
}

/*
 * Calling subwin creates and returns a pointer to a new window with the
 * given number of lines, nlines, and columns, ncols. The window is at
 * position (begin_y, begin_x) on the screen. (This position is relative
 * to the screen, and not to the window orig.) The window is made in the
 * middle of the window orig, so that changes made to one window will
 * affect both windows. The subwindow shares memory with the window orig.
 * When using this routine, it is necessary to call touchwin or touchline
 * on orig before calling wrefresh on the subwindow.
 */
WINDOW *subwin(WINDOW *orig, int nlines, int ncols, int begin_y, int begin_x)
{
	WINDOW *win;
	int i, sz;

	if( ((nlines != 0) && (nlines > orig->_nlines)) ||
		((ncols != 0) && (ncols > orig->_ncols)) )
		return NULL;

	win = malloc(sizeof(WINDOW));
	if(win == NULL)
		return NULL;
	win->_y = begin_y;
	win->_x = begin_x;
	win->_nlines = (nlines == 0) ? orig->_nlines : nlines;
	win->_ncols = (ncols == 0) ? orig->_ncols : ncols;

	win->_maxy = win->_y + win->_nlines - 1;
	win->_maxx = win->_x + win->_ncols -1;
	win->_cury = 0;
	win->_curx = 0;
	win->_attrs = orig->_attrs;
	win->_flags = W_SUBWIN;
	sz = win->_nlines * sizeof(lineScreen_t);
	win->_screen = malloc(sz);
	if(win->_screen == NULL) {
		delwin(win);
		return NULL;
	}
	for(i=0; i<win->_nlines; i++) {
		memcpy(&win->_screen[i], &orig->_screen[i], sizeof(lineScreen_t));
	}
	win->_color = orig->_color;
	win->_background = orig->_background;
	win->_altcolor = orig->_altcolor;
	win->_curcolor = orig->_curcolor;
	return win;
}

/*
 * The unctrl routine returns a character string which is a printable
 * representation of the character c, ignoring attributes.
 * Control characters are displayed in the ^X notation.
 * Printing characters are displayed as is.
 */
char *unctrl(chtype c)
{
	static char str[3];

	if(c < ' ') {
		str[0] = '^';
		str[1] = c + 0x40;
		str[2] = '\0';
	}
	else {
		str[0] = c;
		str[1] = '\0';
	}
	return str;
}

/*
 * These routines move the cursor associated with the window to line y and
 * column x.
 * This routine does not move the physical cursor of the terminal until
 * refresh is called. The position specified is relative to the
 * upper left-hand corner of the window, which is (0,0).
 */
int wmove(WINDOW *win, int y, int x)
{
	if(win == NULL)
		return ERR;

	if( ((y < 0) || (y >= win->_nlines)) ||
	    ((x < 0) || (x >= win->_ncols)) )
		return ERR;

	win->_cury = y;
	win->_curx = x;

	return OK;
}

/*
 * The mvcur routine provides low-level cursor motion. It takes effect
 * immediately (rather than at the next refresh).
 */
int mvcur(int oldrow, int oldcol, int newrow, int newcol)
{
	if(stdscr == NULL)
		return ERR;
	return wmove(stdscr, newrow, newcol);
}

/*
 * The addch, waddch, mvaddch and mvwaddch routines put the character ch
 * into the given window at its current window position, which is then
 * advanced. They are analogous to putchar in stdio(3). If the advance
 * is at the right margin, the cursor automatically wraps to the beginning
 * of the next line. At the bottom of the current scrolling  region, if
 * scrollok is enabled, the scrolling region is scrolled up one line.
 */
int waddch(WINDOW *win, const chtype ch)
{
	charScreen_t *curchar;
	int y, x;

	if(win == NULL)
		return ERR;

	y = win->_cury;
	x = win->_curx;
	curchar = &win->_screen[y].line[x];

	if((ch == '\n') || (ch == '\r')) {
		if(y < win->_nlines-1) {
			y++;
			x = 0;
		}
	}
	else if(ch == '\b') {
		curchar->color = win->_background;
		curchar->value = ' ';
		win->_screen[y].flags = 1;
		x--;
	}
	else if(ch == '\t') {
		int i;
		for(i=0; i<8; i++) {
			waddch(win, ' ');
		}
	}
	else if((ch < ' ') || (ch > '~')) {
		waddch(win,'^');
	}
	else {
		curchar->color = win->_curcolor;
		curchar->value = ch;
		win->_screen[y].flags = 1;
		x++;
	}

	if(x < 0) {
		if(y > 0) {
			y--;
			x = win->_ncols - 1;
		}
		else {
			x = 0;
		}
	}
	else if(x >= win->_ncols) {
		if(y < win->_nlines-1) {
			y++;
			x = 0;
		}
		else {
			x = win->_ncols - 1;
		}
	}
	win->_cury = y;
	win->_curx = x;

	return OK;
}

/*
 * This routine write the characters of the (null-terminated) character
 * string str on the given window. It is similar to calling waddch once
 * for each character in the string. The n as the last argument write at
 * most n characters. If n is -1, then the entire string will be added,
 * up to the maximum number of characters that will fit on the line, or
 * until a terminating null is reached.
 */
int waddnstr(WINDOW *win, const char *str, int n)
{
	int c, l;

	if((win == NULL) || (str == NULL))
		return ERR;

	l = strlen(str);
	if((n >= 0) && (n < l))
		l = n;

	for(c=0; c<l; c++) {
		waddch(win, str[c]);
	}
	return OK;
}

/*
 * The printw, wprintw, mvprintw and mvwprintw routines are analogous to
 * printf [see printf(3)]. In effect, the string that would be output by
 * printf is output instead as though waddstr were used on the given window.
 */
int printw(const char *fmt, ...)
{
	va_list args;
	int n;

	if(stdscr == NULL)
		return ERR;

	va_start(args, fmt);
	n = vsnprintf(lbuf,sizeof(lbuf)-1,fmt,args);
	va_end(args);
	return waddnstr(stdscr, lbuf, n);
}

int wprintw(WINDOW *win, const char *fmt, ...)
{
	va_list args;
	int n;

	if(win == NULL)
		return ERR;

	va_start(args, fmt);
	n = vsnprintf(lbuf,sizeof(lbuf)-1,fmt,args);
	va_end(args);
	return waddnstr(win, lbuf, n);
}

int mvprintw(int y, int x, const char *fmt, ...)
{
	va_list args;
	int n;

	if(stdscr == NULL)
		return ERR;

	va_start(args, fmt);
	n = vsnprintf(lbuf,sizeof(lbuf)-1,fmt,args);
	va_end(args);

	wmove(stdscr, y, x);
	return waddnstr(stdscr, lbuf, n);
}

int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...)
{
	va_list args;
	int n;

	if(win == NULL)
		return ERR;

	va_start(args, fmt);
	n = vsnprintf(lbuf,sizeof(lbuf)-1,fmt,args);
	va_end(args);

	wmove(win, y, x);
	return waddnstr(win, lbuf, n);
}

/*
 * The werase routine copy blanks to every position in the window,
 * clearing the screen.
 */
int werase(WINDOW *win)
{
	int l,c;

	if(win == NULL)
		return ERR;

	for(l=0; l<win->_nlines; l++) {
		for(c=0; c<win->_ncols; c++) {
			charScreen_t *p = &win->_screen[l].line[c];
			p->value = WCURSES_CSPACE;
			p->color = win->_background;
		}
		win->_screen[l].flags = 1;
	}
	return OK;
}

/*
 * The wclear routines are like erase and werase, but they also call
 * clearok, so that the screen is cleared completely on the next call
 * to wrefresh for that window and repainted from scratch.
 */
int wclear(WINDOW *win)
{
	int rtn;

	if(win == NULL)
		return ERR;

	rtn = werase(win);
	clearok(win, TRUE);

	return rtn;
}

/*
 * The wclrtoeol routine erase the current line to the right
 * of the cursor, inclusive, to the end of the current line.
 */
int wclrtoeol(WINDOW *win)
{
	int c;

	if(win == NULL)
		return ERR;

	for(c=win->_curx; c<win->_ncols; c++) {
		charScreen_t *p = &win->_screen[win->_cury].line[c];
		p->value = WCURSES_CSPACE;
		p->color = win->_background;
	}
	win->_screen[win->_cury].flags = 1;
	return OK;
}

/*
 * The wgetch routines read a character from the window.
 */
int wgetch(WINDOW *win)
{
	if(win == NULL)
		return ERR;

	return md_readchar(0);
}

/*
 * These routines return the character, of type chtype, at the current
 * position in the named window. If any attributes are set for that
 * position, their values are ORed into the value returned.
 *
 */
chtype winch(WINDOW *win)
{
	charScreen_t *p;

	if(win == NULL)
		return ERR;

	p = &win->_screen[win->_cury].line[win->_curx];
	return p->value;
}

/*
 * The routine wrefresh copies the named window to the physical terminal
 * screen, taking into account what is already there to do optimizations.
 * Unless leaveok has been enabled, the physical cursor of the terminal is
 * left at the location of the cursor for that window.
 */
int wrefresh(WINDOW *win)
{
	register int l,c;

	if(win == NULL)
		return ERR;

	for(l=0; l<win->_nlines; l++) {
		if(win->_screen[l].flags == 0)
			continue;

		for(c=0; c<win->_ncols; c++) {
			charScreen_t *p = &win->_screen[l].line[c];
			md_putchar_at(win->_y + l, win->_x + c, p->value ,p->color);
		}
		win->_screen[l].flags = 0;
	}
	md_refresh();
	return 0;
}

/*
 * wgetnstr reads at most n characters, thus preventing a possible overflow
 * of the input buffer. Any attempt to enter more characters (other than
 * the terminating newline or carriage return) causes a beep.
 * Function keys also cause a beep and are ignored.
 */
int wgetnstre(WINDOW *win, char *str, int n, const char *range)
{
	GETSTR_t attr;

	memset(&attr,0,sizeof(attr));
	attr.input = md_menu_input;
	attr.output = md_menu_output;
	attr.range = range;
	attr.y = win->_cury;
	attr.x = win->_curx;
	attr.color = win->_color;
	attr.up_keyval = RC_KEY_UP;
	attr.down_keyval = RC_KEY_DOWN;
	attr.left_keyval = RC_KEY_LEFT;
	attr.right_keyval = RC_KEY_RIGHT;
	attr.confirm_keyval = RC_KEY_CONFIRM;
	attr.abort_keyval = RC_KEY_ABORT;

	if(md_getnstr(&attr, str, n) <= 0)
		return ERR;

	return OK;
}

int wgetnstr(WINDOW *win, char *str, int n)
{
	return wgetnstre(win, str, n, NULL);
}

/*
 * The wtouchln routine makes n lines in the window, starting at line y,
 * look as if they have (changed=1) or have not (changed=0) been changed
 * since the last call to wrefresh.
 */
int wtouchln(WINDOW *win, int y, int n, int changed)
{
	int l;

	if(win == NULL)
		return ERR;

	for(l=y; l<n; l++)
		win->_screen[l].flags = changed;

	return OK;
}

/*
* initscr is normally the first curses routine to call when initializing
* a program.
* The initscr code determines the terminal type and initializes all curses
* data structures. initscr also causes the first call to refresh to clear
* the screen. If errors occur, initscr writes an appropriate error message
* standard error and exits; otherwise, a pointer is returned to stdscr.
*/
WINDOW *initscr(void)
{
	if(stdscr == NULL) {
		stdscr = newwin(LINES, COLS, 0, 0);
		if(stdscr == NULL)
			return NULL;
		curscr = subwin(stdscr, LINES, COLS, 0, 0);
		if(curscr == NULL) {
			delwin(stdscr);
			return NULL;
		}
	}
	md_screen_create(LINES, COLS);
	werase(stdscr);
	wrefresh(stdscr);

	return stdscr;
}
