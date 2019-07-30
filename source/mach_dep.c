/*
 * Various installation dependent routines
 *
 * @(#)mach_dep.c	4.37 (Berkeley) 05/23/83
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

/*
 * The various tuneable defines are:
 *
 *	SCOREFILE	Where/if the score file should live.
 *	ALLSCORES	Score file is top ten scores, not top ten
 *			players.  This is only useful when only a few
 *			people will be playing; break;default the score file
 *			gets hogged by just a few people.
 *	NUMSCORES	Number of scores in the score file (default 10).
 *	NUMNAME		String version of NUMSCORES (first character
 *			should be capitalized) (default "Ten").
 *	CHECKTIME	How often/if it should check during the game
 *			for high load average.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "rogue.h"

#define ALLSCORES	1

# ifndef NUMSCORES
#	define	NUMSCORES	10
#	define	NUMNAME		"Ten"
# endif

unsigned int numscores = NUMSCORES;
char *Numname = NUMNAME;

# ifdef ALLSCORES
bool allscore = TRUE;
# else  /* ALLSCORES */
bool allscore = FALSE;
# endif /* ALLSCORES */

#ifdef CHECKTIME
static int num_checks;		/* times we've gone over in checkout() */
#endif /* CHECKTIME */

/*
 * open_score:
 *	Open up the score file for future use
 */

void
open_score()
{
	static char scorefile[MAXSTR] = {'\0'};
     /* 
      * We drop setgid privileges after opening the score file, so subsequent 
      * open()'s will fail.  Just reuse the earlier filehandle. 
      */

    if (scoreboard != NULL) { 
        rewind(scoreboard); 
        return; 
    } 

    if(scorefile[0] == '\0') {
		sprintf(scorefile,"%s/rogue.scr",md_gethomedir());
	}
	
	scoreboard = fopen(scorefile, "r+");

    if ((scoreboard == NULL) && (errno == ENOENT))
    {
    	scoreboard = fopen(scorefile, "w+");
        md_chmod(scorefile,0664);
    }

    if (scoreboard == NULL) { 
         fprintf(stderr, "Could not open %s for writing: %s\n", scorefile, strerror(errno)); 
         fflush(stderr); 
    } 
}

/*
 * setup:
 *	Get starting setup for all games
 */

void
setup()
{
#ifdef CHECKTIME
    int  checkout();

    md_start_checkout_timer(CHECKTIME*60);
    num_checks = 0;
#endif

    raw();				/* Raw mode */
    noecho();				/* Echo off */
    keypad(stdscr,1);
}

/*
 * start_score:
 *	Start the scoring sequence
 */

void
start_score()
{
#ifdef CHECKTIME
    md_stop_checkout_timer();
#endif
}

/* 	 	 
 * is_symlink: 	 	 
 *      See if the file has a symbolic link 	 	 
  */ 	 	 
bool 	 	 
is_symlink(const char *sp) 	 	 
{ 	 	 
    return FALSE;
} 

#ifdef CHECKTIME
/*
 * checkout:
 *	Check each CHECKTIME seconds to see if the load is too high
 */

checkout(int sig)
{
    static char *msgs[] = {
	"The load is too high to be playing.  Please leave in %0.1f minutes",
	"Please save your game.  You have %0.1f minutes",
	"Last warning.  You have %0.1f minutes to leave",
    };
    int checktime;

    if (too_much())
    {
	if (author())
	{
	    num_checks = 1;
	    chmsg("The load is rather high, O exaulted one");
	}
	else if (num_checks++ == 3)
	    fatal("Sorry.  You took too long.  You are dead\n");
	checktime = (CHECKTIME * 60) / num_checks;
	chmsg(msgs[num_checks - 1], ((double) checktime / 60.0));
    }
    else
    {
	if (num_checks)
	{
	    num_checks = 0;
	    chmsg("The load has dropped back down.  You have a reprieve");
	}
	checktime = (CHECKTIME * 60);
    }

	md_start_checkout_timer(checktime);
}

/*
 * chmsg:
 *  checkout()'s version of msg.  
 */
/* VARARGS1 */

chmsg(char *fmt, int arg)
{
	msg(fmt, arg);
}
#endif

/*
 * flush_type:
 *	Flush typeahead for traps, etc.
 */

void
flush_type()
{
    flushinp();
}
