/*
 *	Instructions command for rogue clone. Alan Cox 1992
 */

#ifndef RCURSES
#include <curses.h>
#endif /* RCURSES */
#include "rogue.h"
#include <stdio.h>
#include <string.h>

static char *instructions="rogue.instr";

void Instructions()
{
	char buffer[DROWS+1][DCOLS+1];
	char buf[256];
	FILE *f;
	short row;
	int i,j;
	f=fopen(instructions,"r");
	if(f==NULL)
	{
		message("Help file not on line.",0);
		return;
	}
	for (row = 0; row < DROWS; row++) {
		for (j = 0; j < DCOLS; j++) {
			buffer[row][j] = (char) mvinch(row,j); 
		}
		buffer[row][j] = 0;
        /* When using -lcurses, these two lines cause stdscr to be
         * cleared, as the mvaddstr call moves the cursor to the next
         * row, so the clrtoeol call erases row+1. When we copy the
         * row on the next loop interation, it is empty.  The
         * current level cannot be redrawn. The inbuilt version of
         * mvaddstr does not advance the cursor.*/
		/* mvaddstr(row, 0, buffer[row]); 
           clrtoeol();      */
    }
    for(i=0;i<DROWS;i++)
	{
		move(i,0);
		clrtoeol();
	}
	refresh();
	for(i=0;i<DROWS;i++)
	{
		if(fgets(buf,250,f)==NULL)
			break;
		if(strchr(buf,'\n')!=NULL)
			*strchr(buf,'\n')=0;
		move(i,0);
		clrtoeol();
		mvaddstr(i,0,buf);
	}
    fclose(f);
	refresh();
	rgetchar();
	for(i=0;i<DROWS;i++)
	{
		move(i,0);
		clrtoeol();
	}
	refresh();
	for(i=0;i<DROWS;i++)
	{
		mvaddstr(i,0,buffer[i]);
	}
	refresh();
}

