/*
 * File for the fun ends
 * Death or a total win
 *
 * @(#)rip.c	4.57 (Berkeley) 02/05/99
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include "rogue.h"
#include "score.h"

/*
 * score:
 *	Figure score and post it.
 *
 * flags:
 *	0	looser
 *	2	winner
 *	3	looser with amulet
 *
 */
/* VARARGS2 */

void
score(int amount, int flags, char monst)
{
    SCORE *scp;
    int i, l;
    SCORE *sc2;
    SCORE *top_ten, *endp;
	char buf[MAXSTR];
	int start_col = 4;
# ifdef MASTER
    int prflags = 0;
# endif
    static char *reason[] = {
	"killed",
	"quit",
	"A total winner",
	"killed with Amulet"
    };

    start_score();

    top_ten = (SCORE *) malloc(numscores * sizeof (SCORE));
    endp = &top_ten[numscores];
    for (scp = top_ten; scp < endp; scp++)
    {
		scp->sc_score = -1;
		for (i = 0; i < MAXSTR; i++)
			scp->sc_name[i] = (unsigned char) rnd(255);
		scp->sc_flags = RN;
		scp->sc_level = RN;
		scp->sc_monster = (unsigned short) RN;
    }
    signal(SIGINT, SIG_DFL);

#ifdef MASTER
    if (wizard)
		if (strcmp(prbuf, "names") == 0)
			prflags = 1;
		else if (strcmp(prbuf, "edit") == 0)
			prflags = 2;
#endif
    rd_score(top_ten);
    /*
     * Insert her in list if need be
     */
    sc2 = NULL;
    if (!noscore)
    {
		scp = top_ten;
		while(scp < endp) {
			if (amount > scp->sc_score)
				break;
			scp++;
		}
		if (scp < endp)
		{
			sc2 = endp - 1;
			while (sc2 > scp)
			{
				*sc2 = sc2[-1];
				sc2--;
			}
			scp->sc_score = amount;
			strncpy(scp->sc_name, whoami, sizeof(scp->sc_name)-1);
			scp->sc_name[sizeof(scp->sc_name)-1] = '\0';
			scp->sc_flags = flags;
			if (flags == 2)
				scp->sc_level = max_level;
			else
				scp->sc_level = level;
			scp->sc_monster = monst;
			sc2 = scp;
		}
    }

    /*
     * Print the list
     */
    l = 0;
	if (flags != -1)
		l++;
    mvprintw(l++,start_col,"Top %s %s:", Numname, allscore ? "Scores" : "Rogueists");
	l++;
    mvprintw(l++,start_col,"   Score Name");
	l++;
    for (scp = top_ten; scp < endp; scp++)
    {
		if (scp->sc_score >= 0)
		{
			snprintf(buf,sizeof(buf),"%2d %5d %s: %s on level %d", 
				(int) (scp - top_ten + 1),
				scp->sc_score, scp->sc_name, reason[scp->sc_flags],
				scp->sc_level);
			if (scp->sc_flags == 0 || scp->sc_flags == 3) {
				strcat(buf," by ");
				strcat(buf, killname((char) scp->sc_monster, TRUE));
			}
			strcat(buf,".");
            mvprintw(l++,start_col,"%s",buf);
		}
		else
			break;
    }
    /*
     * Update the list file
     */
    if (sc2 != NULL)
    {
		wr_score(top_ten);
    }
}

/*
 * death:
 *	Do something really fun when he dies
 */

void
death(char monst)
{
    char *killer;
    struct tm *lt;
    static time_t date;
    struct tm *localtime();
	int oldfont;
	int start_line = 1;

    signal(SIGINT, SIG_IGN);
    purse -= purse / 10;
    signal(SIGINT, leave);
    clear();
    killer = killname(monst, FALSE);
	oldfont = md_setfont(MD_FONT_BIG);
	time(&date);
	lt = localtime(&date);
	mvaddstr(start_line,0, "                       __________\n");
	addstr("                      /          \\\n");
	addstr("                     /    REST    \\\n");
	addstr("                    /      IN      \\\n");
	addstr("                   /     PEACE      \\\n");
	addstr("                  /                  \\\n");
	addstr("                  |                  |\n");
	addstr("                  |                  |\n");
	addstr("                  |   killed by a    |\n");
	addstr("                  |                  |\n");
	addstr("                  |       1980       |\n");
	addstr("                 *|     *  *  *      | *\n");
	addstr("         ________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______\n");

	mvaddstr(start_line + 9, center(killer), killer);
	if (monst == 's' || monst == 'h')
		mvaddch(start_line + 8, 32, ' ');
	else
		mvaddstr(start_line + 8, 33, vowelstr(killer));
	mvaddstr(start_line + 6, center(whoami), whoami);
	sprintf(prbuf, "%d Au", purse);
	move(start_line + 7, center(prbuf));
	addstr(prbuf);
	sprintf(prbuf, "%4d", 1900+lt->tm_year);
	mvaddstr(start_line + 10, 26, prbuf);
	refresh();
	md_setfont(oldfont);
    mvprintw(N_LINES - 1, N_COLS - 1 - strlen(RS_PRESS_KEY_TO_CONTINUE2), RS_PRESS_KEY_TO_CONTINUE);
	refresh();
	wait_for(RC_KEY_CONTINUE);
    clear();
	oldfont = md_setfont(MD_FONT_BIG);
	score(purse, amulet ? 3 : 0, monst);
	refresh();
	md_setfont(oldfont);
	mvprintw(N_LINES - 1, N_COLS - 1 - strlen(RS_PRESS_KEY_TO_CONTINUE2), RS_PRESS_KEY_TO_CONTINUE);
	refresh();
	wait_for(RC_KEY_CONTINUE);
	
	endwin();
	resetltchars();
	delwin(stdscr);
	delwin(curscr);
	if (hw != NULL)
		delwin(hw);
	my_exit(0);
}

/*
 * center:
 *	Return the index to center the given string
 */
int
center(char *str)
{
    return 28 - (((int)strlen(str) + 1) / 2);
}

/*
 * total_winner:
 *	Code for a winner
 */

void
total_winner()
{
    THING *obj;
    struct obj_info *op;
    int worth = 0;
    int oldpurse;

    clear();
    standout();
    addstr("                                                               \n");
    addstr("  @   @               @   @           @          @@@  @     @  \n");
    addstr("  @   @               @@ @@           @           @   @     @  \n");
    addstr("  @   @  @@@  @   @   @ @ @  @@@   @@@@  @@@      @  @@@    @  \n");
    addstr("   @@@@ @   @ @   @   @   @     @ @   @ @   @     @   @     @  \n");
    addstr("      @ @   @ @   @   @   @  @@@@ @   @ @@@@@     @   @     @  \n");
    addstr("  @   @ @   @ @  @@   @   @ @   @ @   @ @         @   @  @     \n");
    addstr("   @@@   @@@   @@ @   @   @  @@@@  @@@@  @@@     @@@   @@   @  \n");
    addstr("                                                               \n");
    addstr("     Congratulations, you have made it to the light of day!    \n");
    standend();
    addstr("\nYou have joined the elite ranks of those who have escaped the\n");
    addstr("Dungeons of Doom alive.  You journey home and sell all your loot at\n");
    addstr("a great profit and are admitted to the Fighters' Guild.\n");
    mvaddstr(LINES - 1, 0, RS_PRESS_KEY_TO_CONTINUE2);
    refresh();
    wait_for(RC_KEY_CONTINUE);
    clear();
    mvaddstr(0, 0, "   Worth  Item\n");
    oldpurse = purse;
    for (obj = pack; obj != NULL; obj = next(obj))
    {
		switch (obj->o_type)
		{
		case FOOD:
			worth = 2 * obj->o_count;
			break;
		case WEAPON:
			worth = weap_info[obj->o_which].oi_worth;
			worth *= 3 * (obj->o_hplus + obj->o_dplus) + obj->o_count;
			obj->o_flags |= ISKNOW;
			break;
		case ARMOR:
			worth = arm_info[obj->o_which].oi_worth;
			worth += (9 - obj->o_arm) * 100;
			worth += (10 * (a_class[obj->o_which] - obj->o_arm));
			obj->o_flags |= ISKNOW;
			break;
		case SCROLL:
			worth = scr_info[obj->o_which].oi_worth;
			worth *= obj->o_count;
			op = &scr_info[obj->o_which];
			if (!op->oi_know)
				worth /= 2;
			op->oi_know = TRUE;
			break;
		case POTION:
			worth = pot_info[obj->o_which].oi_worth;
			worth *= obj->o_count;
			op = &pot_info[obj->o_which];
			if (!op->oi_know)
				worth /= 2;
			op->oi_know = TRUE;
			break;
		case RING:
			op = &ring_info[obj->o_which];
			worth = op->oi_worth;
			if (obj->o_which == R_ADDSTR || obj->o_which == R_ADDDAM ||
				obj->o_which == R_PROTECT || obj->o_which == R_ADDHIT)
			{
				if (obj->o_arm > 0)
					worth += obj->o_arm * 100;
				else
					worth = 10;
			}
			if (!(obj->o_flags & ISKNOW))
				worth /= 2;
			obj->o_flags |= ISKNOW;
			op->oi_know = TRUE;
			break;
		case STICK:
			op = &ws_info[obj->o_which];
			worth = op->oi_worth;
			worth += 20 * obj->o_charges;
			if (!(obj->o_flags & ISKNOW))
				worth /= 2;
			obj->o_flags |= ISKNOW;
			op->oi_know = TRUE;
			break;
		case AMULET:
			worth = 1000;
			break;
		}
		if (worth < 0)
			worth = 0;
		printw("%c) %5d  %s\n", obj->o_packch, worth, inv_name(obj, FALSE));
		purse += worth;
    }
    printw("   %5d  Gold Pieces          ", oldpurse);
    refresh();
    score(purse, 2, ' ');
    my_exit(0);
}

/*
 * killname:
 *	Convert a code to a monster name
 */
char *
killname(char monst, bool doart)
{
    struct h_list *hp;
    char *sp;
    bool article;
    static struct h_list nlist[] = {
		{'a',	"arrow",		TRUE},
		{'b',	"bolt",			TRUE},
		{'d',	"dart",			TRUE},
		{'h',	"hypothermia",	FALSE},
		{'s',	"starvation",	FALSE},
		{'\0'}
    };

    if (isupper(monst))
    {
		sp = monsters[monst-'A'].m_name;
		article = TRUE;
    }
    else
    {
		sp = "Wally the Wonder Badger";
		article = FALSE;
		for (hp = nlist; hp->h_ch; hp++) 
		{
			if (hp->h_ch == monst)
			{
				sp = hp->h_desc;
				article = hp->h_print;
				break;
			}
		}
    }
    if (doart && article)
		sprintf(prbuf, "a%s ", vowelstr(sp));
    else
		prbuf[0] = '\0';
    strcat(prbuf, sp);
    return prbuf;
}

/*
 * death_monst:
 *	Return a monster appropriate for a random death.
 */
char
death_monst()
{
    static char poss[] =
    {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'h', 'd', 's',
	' '	/* This is provided to generate the "Wally the Wonder Badger"
		   message for killer */
    };

    return poss[rnd(sizeof poss / sizeof (char))];
}
