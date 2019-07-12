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
 *	MAXLOAD		What (if any) the maximum load average should be
 *			when people are playing.  Since it is divided
 *			by 10, to specify a load limit of 4.0, MAXLOAD
 *			should be "40".	 If defined, then
 *      LOADAV		Should it use it's own routine to get
 *		        the load average?
 *      NAMELIST	If so, where does the system namelist
 *		        hide?
 *	MAXUSERS	What (if any) the maximum user count should be
 *	                when people are playing.  If defined, then
 *      UCOUNT		Should it use it's own routine to count
 *		        users?
 *      UTMP		If so, where does the user list hide?
 *	CHECKTIME	How often/if it should check during the game
 *			for high load average.
 */

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "rogue.h"

#define ALLSCORES	1

#define NOOP(x) (x += 0)

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
 * init_check:
 *	Check out too see if it is proper to play the game now
 */

void
init_check()
{
}

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
#endif

#ifdef DUMP
    md_onsignal_autosave();
#else
    md_onsignal_default();
#endif

#ifdef CHECKTIME
    md_start_checkout_timer(CHECKTIME*60);
    num_checks = 0;
#endif

    raw();				/* Raw mode */
    noecho();				/* Echo off */
    keypad(stdscr,1);
    getltchars();			/* get the local tty chars */
}

/*
 * getltchars:
 *	Get the local tty chars for later use
 */

void
getltchars()
{
    got_ltc = TRUE;
    orig_dsusp = md_dsuspchar();
    md_setdsuspchar( md_suspchar() );
}

/* 
 * resetltchars: 
 *      Reset the local tty chars to original values. 
 */ 
void 
resetltchars(void) 
{ 
    if (got_ltc) {
        md_setdsuspchar(orig_dsusp);
    } 
} 
  
/* 
 * playltchars: 
 *      Set local tty chars to the values we use when playing. 
 */ 
void 
playltchars(void) 
{ 
    if (got_ltc) { 
        md_setdsuspchar( md_suspchar() );
    } 
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

#if defined(MAXLOAD) || defined(MAXUSERS)
/*
 * too_much:
 *	See if the system is being used too much for this game
 */
bool
too_much()
{
#ifdef MAXLOAD
    double avec[3];
#else
    int cnt;
#endif

#ifdef MAXLOAD
    md_loadav(avec);
    if (avec[1] > (MAXLOAD / 10.0))
	return TRUE;
#endif
#ifdef MAXUSERS
    if (ucount() > MAXUSERS)
	return TRUE;
#endif
    return FALSE;
}
#endif

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
 *	checkout()'s version of msg.  If we are in the middle of a
 *	shell, do a printf instead of a msg to a the refresh.
 */
/* VARARGS1 */

chmsg(char *fmt, int arg)
{
    if (!in_shell)
	msg(fmt, arg);
    else
    {
	printf(fmt, arg);
	putchar('\n');
	fflush(stdout);
    }
}
#endif

#ifdef UCOUNT
/*
 * ucount:
 *	count number of users on the system
 */
#include <utmp.h>

struct utmp buf;

int
ucount()
{
    struct utmp *up;
    FILE *utmp;
    int count;

    if ((utmp = fopen(UTMP, "r")) == NULL)
	return 0;

    up = &buf;
    count = 0;

    while (fread(up, 1, sizeof (*up), utmp) > 0)
	if (buf.ut_name[0] != '\0')
	    count++;
    fclose(utmp);
    return count;
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
