/*
 * save and restore routines
 *
 * @(#)save.c	4.33 (Berkeley) 06/01/83
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include "rogue.h"
#include "score.h"

typedef struct stat STAT;

extern char version[], encstr[];

static STAT sbuf;

const char *get_saved_dir()
{
	static char saved_dir[64] = {'\0'};

	if(saved_dir[0] == '\0') {
		char *home;

		home = md_gethomedir();
		snprintf(saved_dir,sizeof(saved_dir),"%s/saved",home);
	}
	return saved_dir;
}

const char *
get_saved_name(const char *user)
{
	static char saved_file[128];
	DIR *pdir;
	struct dirent *pent;
	int nsaved = 0;
	char *rtn = NULL;

	pdir=opendir(get_saved_dir());
	if (!pdir){
		return NULL;
	}

	while ((pent=readdir(pdir))!=NULL) {
		if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
			continue;

		snprintf(saved_file,sizeof(saved_file),"%s/%s",get_saved_dir(),pent->d_name);
		if(is_for_user(saved_file, user) ==  TRUE) {
			nsaved++;
		}
	}
	if(nsaved > 0) {
		MENU_t *menu;
		int i, ch;

		rewinddir(pdir);
		menu = malloc(nsaved * sizeof(MENU_t));
		i = 0;
		while ((pent=readdir(pdir))!=NULL) {
			if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
				continue;
			snprintf(saved_file,sizeof(saved_file),"%s/%s",get_saved_dir(),pent->d_name);
			if(is_for_user(saved_file, user) ==  TRUE) {
				menu[i].attri = OPT_DSP;
				menu[i].code = 'a' + i;
				menu[i].entry = strdup(pent->d_name);
				i++;
			}
		}
		ch = md_stdmenu(menu, "Available saved files", nsaved);
		if(ch != -1) {
			snprintf(saved_file,sizeof(saved_file),"%s/%s",get_saved_dir(),menu[ch - 'a'].entry);
			rtn = saved_file;
		}
		/* freeing menu */
		for(i=0; i<nsaved; i++) {
			if(menu[i].entry != NULL) {
				free((void *)menu[i].entry);
			}
		}
		free((void *)menu);
	}
	closedir(pdir);

	return rtn;
}

/*
 * save_game:
 *	Implement the "save game" command
 */

void
save_game()
{
    FILE *savef;
    auto char buf[MAXSTR];
	char yesno[2];

    /*
     * get file name
     */
    mpos = 0;
over:
    if (file_name[0] != '\0')
    {
		msg("save file (%s)? ", file_name);
		if( getnstre(yesno, 1, "YN") == ERR)
		{
				msg("");
				return;
		}
		if (yesno[0] == 'Y')
		{
			addstr("Yes\n");
			refresh();
			strcpy(buf, file_name);
			goto gotfile;
		}
    }

    do
    {
		mpos = 0;
		msg("file name: ");
		buf[0] = '\0';
		if(getnstre(buf, sizeof(buf), NAME_CHARS) == ERR)
		{
quit_it:
			msg("");
			return;
		}
		sprintf(file_name,"%s/%s",get_saved_dir(),buf);
		mpos = 0;
gotfile:
		/*
		 * test to see if the file exists
		 */
		if (stat(file_name, &sbuf) >= 0)
		{
			msg("File exists. Do you wish to overwrite it?");
			mpos = 0;
			if(getnstre(yesno, 1, "YN") == ERR)
					goto quit_it;
			if (yesno[0] == 'Y')
					break;
			else if (yesno[0] == 'N')
				goto over;

			msg("file name: %s", file_name);
			md_unlink(file_name);
		}

		if ((savef = fopen(file_name, "w")) == NULL) {
			msg(strerror(errno));
		}
    } while (savef == NULL);

    save_file(savef);
    /* NOTREACHED */
}

/*
 * auto_save:
 *	Automatically save a file.  This is used if a HUP signal is
 *	recieved
 */

void
auto_save(int sig)
{
    FILE *savef;
    NOOP(sig);

    md_ignoreallsignals();
    if (file_name[0] != '\0' && ((savef = fopen(file_name, "w")) != NULL ||
	(md_unlink_open_file(file_name, savef) >= 0 && (savef = fopen(file_name, "w")) != NULL)))
	    save_file(savef);
    exit(0);
}

/*
 * save_file:
 *	Write the saved game on the file
 */

void
save_file(FILE *savef)
{
    char buf[80];
    mvcur(0, COLS - 1, LINES - 1, 0);
    putchar('\n');
    endwin();
    resetltchars();
    md_chmod(file_name, 0400);
    encwrite(version, strlen(version)+1, savef);
    sprintf(buf,"%d x %d\n", LINES, COLS);
    encwrite(buf,80,savef);
    rs_save_file(savef);
    fflush(savef);
    fclose(savef);
    exit(0);
}

/*
 * restore:
 *	Restore a saved game from a file with elaborate checks for file
 *	integrity from cheaters
 */
bool
restore(const char *file, char **envp)
{
    FILE *inf;
    int syml;
    auto char buf[MAXSTR];
    auto STAT sbuf2;
    int lines, cols;

	if (strcmp(file, "-r") == 0)
		file = file_name;

	md_tstphold();

	if ((inf = fopen(file,"r")) == NULL)
    {
		perror(file);
		return FALSE;
    }

	stat(file, &sbuf2);
    syml = is_symlink(file);
//	fflush(stdout);
    encread(buf, (unsigned) strlen(version) + 1, inf);
    if (strcmp(buf, version) != 0)
    {
		printf("Sorry, saved game is out of date.\n");
		return FALSE;
    }
    encread(buf,80,inf);
    sscanf(buf,"%d x %d\n", &lines, &cols);

    keypad(stdscr, 1);

    if (lines > LINES)
    {
        endwin();
        printf("Sorry, original game was played on a screen with %d lines.\n",lines);
        printf("Current screen only has %d lines. Unable to restore game\n",LINES);
        return(FALSE);
    }
    if (cols > COLS)
    {
        endwin();
        printf("Sorry, original game was played on a screen with %d columns.\n",cols);
        printf("Current screen only has %d columns. Unable to restore game\n",COLS);
        return(FALSE);
    }

    hw = newwin(LINES, COLS, 0, 0);
    setup();
    rs_restore_file(inf);
    /*
     * we do not close the file so that we will have a hold of the
     * inode for as long as possible
     */

    if (
#ifdef MASTER
	!wizard &&
#endif
        md_unlink_open_file(file, inf) < 0)
    {
		printf("Cannot unlink file\n");
		return FALSE;
    }
    mpos = 0;
/*    printw(0, 0, "%s: %s", file, ctime(&sbuf2.st_mtime)); */
/*
    printw("%s: %s", file, ctime(&sbuf2.st_mtime));
*/
    clearok(stdscr,TRUE);
    /*
     * defeat multiple restarting from the same place
     */
#ifdef MASTER
    if (!wizard)
#endif
	if (sbuf2.st_nlink != 1 || syml)
	{
	    endwin();
	    printf("\nCannot restore from a linked file\n");
	    return FALSE;
	}

    if (pstats.s_hpt <= 0)
    {
		endwin();
		printf("\n\"He's dead, Jim\"\n");
		return FALSE;
    }
	md_tstpresume();
    strcpy(file_name, file);
    clearok(curscr, TRUE);
    srand(md_getpid());
    msg("file name: %s", file);
    playit();
    /*NOTREACHED*/
    return(0);
}

/*
 * encwrite:
 *	Perform an encrypted write
 */

size_t
encwrite(char *start, size_t size, FILE *outf)
{
    char *e1, *e2, fb;
    int temp;
    extern char statlist[];
    size_t o_size = size;
    e1 = encstr;
    e2 = statlist;
    fb = 0;

    while(size)
    {
		if (putc(*start++ ^ *e1 ^ *e2 ^ fb, outf) == EOF)
            break;

		temp = *e1++;
		fb = fb + ((char) (temp * *e2++));
		if (*e1 == '\0')
			e1 = encstr;
		if (*e2 == '\0')
			e2 = statlist;
		size--;
    }

    return(o_size - size);
}

/*
 * encread:
 *	Perform an encrypted read
 */
size_t
encread(char *start, size_t size, FILE *inf)
{
    char *e1, *e2, fb;
    int temp;
    size_t read_size;
    extern char statlist[];

    fb = 0;

    if ((read_size = fread(start,1,size,inf)) == 0 || read_size == -1)
		return(read_size);

    e1 = encstr;
    e2 = statlist;

    while (size--)
    {
		*start++ ^= *e1 ^ *e2 ^ fb;
		temp = *e1++;
		fb = fb + (char)(temp * *e2++);
		if (*e1 == '\0')
			e1 = encstr;
		if (*e2 == '\0')
			e2 = statlist;
    }

    return(read_size);
}

static char scoreline[100];
/*
 * read_score
 *	Read in the score file
 */
void
rd_score(SCORE *top_ten)
{
    unsigned int i;

	if (scoreboard == NULL)
		return;

	rewind(scoreboard);

	for(i = 0; i < numscores; i++)
    {
        encread(top_ten[i].sc_name, MAXSTR, scoreboard);
        encread(scoreline, 100, scoreboard);
		sscanf(scoreline, " %d %u %hu %d %x \n",
			&top_ten[i].sc_score,
            &top_ten[i].sc_flags, &top_ten[i].sc_monster,
            &top_ten[i].sc_level, &top_ten[i].sc_time);
    }

	rewind(scoreboard);
}

/*
 * write_scrore
 *	Read in the score file
 */
void
wr_score(SCORE *top_ten)
{
    unsigned int i;

	if (scoreboard == NULL)
		return;

	rewind(scoreboard);

    for(i = 0; i < numscores; i++)
    {
          memset(scoreline,0,100);
          encwrite(top_ten[i].sc_name, MAXSTR, scoreboard);
		  sprintf(scoreline, " %d %u %hu %d %x \n",
			  top_ten[i].sc_score,
              top_ten[i].sc_flags, top_ten[i].sc_monster,
              top_ten[i].sc_level, top_ten[i].sc_time);
          encwrite(scoreline,100,scoreboard);
    }

	rewind(scoreboard);
}
