/*
 * main.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "rogue.h"

extern short party_room;

int saved_uid= -1;
int true_uid= -1;

void
turn_into_games()
{
#ifdef __EMSCRIPTEN__
	/* No setuid() in the browser. */
	return;
#else
	if(setuid(saved_uid)==-1)
	{
		perror("setuid(restore)");
		clean_up("");
	}
#endif /* __EMSCRIPTEN__ */
}

void
turn_into_user()
{
#ifdef __EMSCRIPTEN__
	/* No setuid() in the browser. */
	return;
#else
	if(setuid(true_uid)==-1)
	{
		perror("setuid(restore)");
		clean_up("");
	}
#endif /* __EMSCRIPTEN__ */
}

int
main(argc, argv)
int argc;
char *argv[];
{
#ifndef __EMSCRIPTEN__
	/* Save the setuid we have got, then turn back into the player */
	saved_uid=geteuid();
	setuid(true_uid=getuid());
#endif /* __EMSCRIPTEN__ */

	if (init(argc, argv)) {		/* restored game */
		goto PL;
	}

	for (;;) {
		clear_level();
		make_level();
		put_objects();
		put_stairs();
		add_traps();
		put_mons();
		put_player(party_room);
		print_stats(STAT_ALL);
PL:		
		play_level();
		free_stuff(&level_objects);
		free_stuff(&level_monsters);
	}
}
