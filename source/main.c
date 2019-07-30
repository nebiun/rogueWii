/*
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 *
 * @(#)main.c	4.22 (Berkeley) 02/05/99
 */

#include <stdlib.h>
#include <string.h>
#include "rogue.h"

static void
rndname(char *buf)
{
	const char *heroes[] = {
		"Conan", "Heracles", "Samson", "Hiras", "Phoebe",
		"Selene", "Rhea", "Iris", "Perseus", "Jason",
		"Theseus", "Ethel", "Mulan", "Arthur", "Horus",
		"Gaia", "Helen", "Hera", "Aife", "Liath"
	};
	int n;

	n = rnd(sizeof(heroes)/sizeof(*heroes));
	strcpy(buf,heroes[n]);
}

static void
cover(void)
{
	int l = 0;
	mvprintw(l++,21," ______");
	mvprintw(l++,21,"|   __ \\.-----.-----.--.--.-----. __ ");
	mvprintw(l++,21,"|      <|  _  |  _  |  |  |  -__||__|");
	mvprintw(l++,21,"|___|__||_____|___  |_____|_____||__|");
	mvprintw(l++,21,"              |_____|");
	mvprintw(l++,12," _______               __              __");
	mvprintw(l++,12,"|    ___|.--.--.-----.|  |.-----.----.|__|.-----.-----.");
	mvprintw(l++,12,"|    ___||_   _|  _  ||  ||  _  |   _||  ||     |  _  |");
	mvprintw(l++,12,"|_______||__.__|   __||__||_____|__|  |__||__|__|___  |");
	mvprintw(l++,12,"               |__|                             |_____|");
	mvprintw(l++,31," __   __ ");
	mvprintw(l++,31,"|  |_|  |--.-----.");
	mvprintw(l++,31,"|   _|     |  -__|");
	mvprintw(l++,31,"|____|__|__|_____|");
	l++;
	mvprintw(l++,15," _____");
	mvprintw(l++,15,"|     \\.--.--.-----.-----.-----.-----.-----.-----.");
	mvprintw(l++,15,"|  --  |  |  |     |  _  |  -__|  _  |     |__ --|");
	mvprintw(l++,15,"|_____/|_____|__|__|___  |_____|_____|__|__|_____|");
	mvprintw(l++,15,"                   |_____|");
	mvprintw(l++,18,"        ___      _____");
	mvprintw(l++,18,".-----.'  _|    |     \\.-----.-----.--------.");
	mvprintw(l++,18,"|  _  |   _|    |  --  |  _  |  _  |        |");
	mvprintw(l++,18,"|_____|__|      |_____/|_____|_____|__|__|__|");
	l += 2;
	mvprintw(l++,25,"WII version by Nebiun (c)2019");
	
	mvprintw(N_LINES - 1,N_COLS - 1 - strlen(RS_PRESS_KEY_TO_CONTINUE2), RS_PRESS_KEY_TO_CONTINUE2);
	
	refresh();
	wait_for(RC_KEY_CONTINUE);
	clear();
}

static void
intro(void)
{
	static int colors[] = {C_BLACK, C_RED, C_GREEN, C_YELLOW, C_BLUE, C_MAGENTA, C_CYAN, C_LIGHTGREY,
						   C_DARKGREY, C_ORANGERED, C_LIMEGREEN, C_GOLD, C_DEEPSKYBLUE, C_DEEPPINK,
						   C_DARKTURQUOISE, C_WHITE };
	int l;
	int ch;
	int f, b, a, nw;
	int fc = 0, fb = 0, fa = 0;
	
	md_getcolors(&b, &f);
	a = md_get_altcolor();
	while(colors[fc] != f)
		fc++;
	while(colors[fb] != b)
		fb++;
	while(colors[fa] != a)
		fa++;

	do {
		nw = 0;
		l = 0;
		wsetcurcolor(stdscr,colors[fa]);	
		mvprintw(l++,0,"I N T R O D U C T I O N");
		wsetcurcolor(stdscr,colors[fc]);	
		l++;
		mvprintw(l++,0,"You have just finished your years as a student at the local fighter’s guild.");
		mvprintw(l++,0,"After much practice and sweat you have finally completed your training and are");
		mvprintw(l++,0,"ready to embark upon a perilous adventure. As a test of your skills, the local");
		mvprintw(l++,0,"guildmasters have sent you into the Dungeons of Doom. Your task is to return");
		mvprintw(l++,0,"with the Amulet of Yendor.");
		mvprintw(l++,0,"Your reward for the completion of this task will be a full membership in the");
		mvprintw(l++,0,"local guild. In addition, you are allowed to keep all the loot you bring back");
		mvprintw(l++,0,"from the dungeons.");
		l++;
		mvprintw(l++,0,"In preparation for your journey, you are given an enchanted mace, a bow, and a");
		mvprintw(l++,0,"quiver of arrows taken from a dragon’s hoard in the far off Dark Mountains.");
		mvprintw(l++,0,"You are also outfitted with elf-crafted armor and given enough food to reach");
		mvprintw(l++,0,"the dungeons. You say goodbye to family and friends for what may be the last");
		mvprintw(l++,0,"time and head up the road.");
		l++;
		mvprintw(l++,0,"You set out on your way to the dungeons and after several days of uneventful");
		mvprintw(l++,0,"travel, you see the ancient ruins that mark the entrance to the Dungeons of");
		mvprintw(l++,0,"Doom. It is late at night, so you make camp at the entrance and spend the night"); 
		mvprintw(l++,0,"sleeping under the open skies. In the morning you gather your weapons, put on");
		mvprintw(l++,0,"your armor, eat what is almost your last food, and enter the dungeons.");
		
		mvprintw(N_LINES - 1,N_COLS - 1 - strlen(RS_PRESS_KEY_TO_CONTINUE2), RS_PRESS_KEY_TO_CONTINUE2);
		
		refresh();
		ch = md_readchar(2);
		if(ch == RC_KEY_UP) {
			fb++;
			if(fb >= sizeof(colors)/sizeof(*colors))
				fb = 0;
			nw++;
		}
		if(ch == RC_KEY_DOWN) {
			fb--;
			if(fb < 0)
				fb = sizeof(colors)/sizeof(*colors) - 1;
			nw++;
		}		
		if(ch == RC_KEY_LEFT) {
			fc++;
			if(fc >= sizeof(colors)/sizeof(*colors))
				fc = 0;
			nw++;
		}
		if(ch == RC_KEY_RIGHT) {
			fc--;
			if(fc < 0)
				fc = sizeof(colors)/sizeof(*colors) - 1;
			nw++;
		}		
		if(ch == RC_KEY_ABORT) {
			md_setcolors(b, f);
			fc = 0;
			while(colors[fc] != f)
				fc++;
			fb = 0;
			while(colors[fb] != b)
				fb++;
			nw++;
		}
		if(ch == RC_KEY_SEARCH) {
			fa++;
			if(fa >= sizeof(colors)/sizeof(*colors))
				fa = 0;
			stdscr->_altcolor = colors[fa];
		}
		if(ch == RC_KEY_THROW) {
			fa--;
			if(fa < 0)
				fa = sizeof(colors)/sizeof(*colors) -1;
			stdscr->_altcolor = colors[fa];
		}
		if(nw) {
			stdscr->_color = colors[fc];
			stdscr->_background = colors[fb];
			md_setcolors(colors[fb], colors[fc]);
			a = md_get_altcolor();
			fa = 0;
			while(colors[fa] != a)
				fa++;
			stdscr->_altcolor = colors[fa];
			stdscr->_curcolor = stdscr->_color;
		}
	} while(ch != RC_KEY_CONTINUE);
	clear();		
}

static void
infos(void)
{
	int l = 0;
	mvprintw(l++,0,"What do all those things on the screen mean?");
	l++;
	mvprintw(l++,0,"@   You, the adventurer                 -|  The walls of rooms");
	mvprintw(l++,0,"+   A door to/from a room               .   The floor of a room");
	mvprintw(l++,0,"#   Floor of a passage between rooms    *   A pile or pot of gold");
	mvprintw(l++,0,")   A weapon of some sort               ]   A piece of armor");
	mvprintw(l++,0,"!   A flask containing a magic potion   ?   A piece of paper (a magic scroll?)");
	mvprintw(l++,0,"=   A ring with magic properties        /   A magical staff or wand");
	mvprintw(l++,0,"^   A trap                              %%   A staircase to other levels");
	mvprintw(l++,0,":   A piece of food                     A-Z The inhabitants of the Dungeons");
	l++;
	mvprintw(l++,0,"On the bottom line");
	l++;
	mvprintw(l++,0,"Level: This number indicates how deep you have gone in the dungeon.");
	mvprintw(l++,0,"Gold:  The number of gold pieces you have find"); 
	mvprintw(l++,0,"Hp:    Your current and maximum health points.");
	mvprintw(l++,0,"       Indicate how much damage you can take before you die.");
	mvprintw(l++,0,"Str:   Your current strength and maximum ever strength.");
	mvprintw(l++,0,"Arm:   Your current armor protection.");
	mvprintw(l++,0,"Exp:   Your current experience level and experience points.");
	mvprintw(l++,0,"       As you do things, you gain experience points.");
	l++;
	mvprintw(l++,0,"Commands");
	l++;
	mvprintw(l++,0,"Press A and - in the game to see the help");
	mvprintw(N_LINES - 1,N_COLS - 1 - strlen(RS_PRESS_KEY_TO_CONTINUE2), RS_PRESS_KEY_TO_CONTINUE2);
	
	refresh();
	wait_for(RC_KEY_CONTINUE);
	clear();		
}

/*
 * main:
 *	The main program, of course
 */
int
main(int argc, char **argv)
{
	char lastname[MAXSTR] = {'\0'};
	char name[MAXSTR];
	int l, oldfont;
	const char *saved;

	md_init();

    /*
     * get home dir
     */
    strncpy(home, md_gethomedir(), MAXSTR);

    open_score();

	initscr();              /* Start up cursor package */

	while(TRUE) {
		char yesno[2];
		int rtn = TRUE;

		init_globals();
		clear();
		cover();
		intro();
		infos();

		oldfont = md_setfont(MD_FONT_BIG);
		l = 5;
		clear();
		mvprintw(l,0,"What's your name? ");
		refresh();
		if( getnstre(name, 16, NAME_CHARS) == ERR) {
			l++;
			if(lastname[0] != '\0')
				strcpy(name,lastname);
			else
				rndname(name);
			mvprintw(l,0,"Well, the Fate choose for you: %s\n",name);
			refresh();
			md_setfont(oldfont);
			mvprintw(N_LINES - 1, N_COLS - 1 - strlen(RS_PRESS_KEY_TO_CONTINUE2), RS_PRESS_KEY_TO_CONTINUE2);
			refresh();
			wait_for(RC_KEY_CONTINUE);
		}
		else {
			strcpy(lastname, name);
			md_setfont(oldfont);
		}
		clear();
		refresh();

		if( (saved = get_saved_name(name)) != NULL) {
			rtn = restore(saved);
		}
		else {
			strcpy(whoami, name);

			l++;
#ifdef MASTER
			if (wizard)
				mvprintw(l,0,"Hello %s, welcome to dungeon #%d", whoami, dnum);
			else
#endif
				mvprintw(l,0,"Hello %s, just a moment while I dig the dungeon...", whoami);
			refresh();

			init_probs();           /* Set up prob tables for objects */
			init_player();          /* Set up initial player stats */
			init_names();           /* Set up names of scrolls, traps, etc. */
			init_colors();          /* Set up colors of potions */
			init_stones();          /* Set up stone settings of rings */
			init_materials();       /* Set up materials of wands */
			setup();

			/*
			 * The screen must be at least NUMLINES x NUMCOLS
			 */
			if (LINES < NUMLINES || COLS < NUMCOLS)
			{
				printf("\nSorry, the screen must be at least %dx%d\n", NUMLINES, NUMCOLS);
				endwin();
				my_exit(1);
			}

			/*
			 * Set up windows
			 */
			hw = newwin(LINES, COLS, 0, 0);
			idlok(stdscr, TRUE);
			idlok(hw, TRUE);
#ifdef MASTER
			noscore = wizard;
#endif
			new_level();            /* Draw current level */
			/*
			 * Start up daemons and fuses
			 */
			start_daemon(runners, 0, AFTER);
			start_daemon(doctor, 0, AFTER);
			fuse(swander, 0, WANDERTIME, AFTER);
			start_daemon(stomach, 0, AFTER);
		}

		if(rtn == TRUE) {
			playing = TRUE;
			playit();

			/* Try again? */
			oldfont = md_setfont(MD_FONT_BIG);
			l = 5;
			clear();
			mvprintw(l,0,"Do you want to play again? ");
			refresh();
			if((getnstre(yesno, 1, "YN") == ERR) || (yesno[0] == 'N')) {
				l++;
				mvprintw(l,0,"Ok, bye bye!");
				refresh();
				endwin();
				delwin(stdscr);
				delwin(curscr);
				if (hw != NULL)
					delwin(hw);
				my_exit(0);
			}
			md_setfont(oldfont);
		}
	}

    return(0);
}

/*
 * fatal:
 *	Exit the program, printing a message.
 */

void
fatal(char *s)
{
    mvaddstr(LINES - 2, 0, s);
    refresh();
    endwin();
    my_exit(0);
}

/*
 * rnd:
 *	Pick a very random number.
 */
int
rnd(int range)
{
    return range == 0 ? 0 : abs((int) RN) % range;
}

/*
 * roll:
 *	Roll a number of dice
 */
int
roll(int number, int sides)
{
    int dtotal = 0;

    while (number--)
	dtotal += rnd(sides)+1;
    return dtotal;
}

/*
 * playit:
 *	The main loop of the program.  Loop until the game is over,
 *	refreshing things and looking at the proper times.
 */

void
playit()
{
//  char *opts;

    /*
     * set up defaults for slow terminals
     */

    if (baudrate() <= 1200)
    {
		terse = TRUE;
		jump = TRUE;
		see_floor = FALSE;
    }

    if (md_hasclreol())
		inv_type = INV_CLEAR;

    oldpos = hero;
    oldrp = roomin(&hero);
    while (playing)
		command();			/* Command execution */
}

/*
 * quit:
 *	Have player make certain, then exit.
 */

void
quit(int sig)
{
    int oy, ox;

    if (!q_comm)
		mpos = 0;
    getyx(curscr, oy, ox);
    msg("really quit? (press again)");
	if (readchar(0) == 'Q')
    {
		clear();
		mvprintw(LINES - 2, 0, "You quit with %d gold pieces", purse);
		move(LINES - 1, 0);
		refresh();
		score(purse, 1, 0);
		my_exit(0);
    }
    else
    {
		move(0, 0);
		clrtoeol();
		status();
		move(oy, ox);
		refresh();
		mpos = 0;
		count = 0;
		to_death = FALSE;
    }
}

/*
 * leave:
 *	Leave quickly, but curteously
 */

void
leave(int sig)
{
    if (!isendwin())
    {
		mvcur(0, COLS - 1, LINES - 1, 0);
		endwin();
    }

    my_exit(0);
}

/*
 * my_exit:
 *	Leave the process properly
 */

void
my_exit(int st)
{
    md_exit(st);
}

