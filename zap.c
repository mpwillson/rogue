/*
 * zap.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#ifndef RCURSES
#include <curses.h>
#endif /* RCURSES */
#include <string.h>
#include "rogue.h"

boolean wizard = 0;

/* forward decls */
void tele_away();

extern boolean being_held, score_only, detect_monster;

void
zapp()
{
	short wch;
	boolean first_miss = 1;
	object *wand;
	short dir, row, col;
	object *monster;

	while (!is_direction(dir = rgetchar())) {
		sound_bell();
		if (first_miss) {
			message("direction? ", 0);
			first_miss = 0;
		}
	}
	check_message();
	if (dir == CANCEL) {
		return;
	}
	if ((wch = pack_letter("zap with what?", WAND)) == CANCEL) {
		return;
	}
	check_message();

	if (!(wand = get_letter_object(wch))) {
		message("no such item.", 0);
		return;
	}
	if (wand->what_is != WAND) {
		message("you can't zap with that", 0);
		return;
	}
	if (wand->class <= 0) {
		message("nothing happens", 0);
	} else {
		wand->class--;
		row = rogue.row; col = rogue.col;
		monster = get_zapped_monster(dir, &row, &col);
		if (monster) {
			wake_up(monster);
			zap_monster(monster, wand->which_kind);
			relight();
		}
	}
	(void) reg_move();
}

object *
get_zapped_monster(dir, row, col)
short dir;
short *row, *col;
{
	short orow, ocol;

	for (;;) {
		orow = *row; ocol = *col;
		get_dir_rc(dir, row, col, 0);
		if (((*row == orow) && (*col == ocol)) ||
		   (dungeon[*row][*col] & (HORWALL | VERTWALL)) ||
		   (dungeon[*row][*col] == NOTHING)) {
			return(0);
		}
		if (dungeon[*row][*col] & MONSTER) {
			if (!imitating(*row, *col)) {
				return(object_at(&level_monsters, *row, *col));
			}
		}
	}
}

void
zap_monster(monster, kind)
object *monster;
unsigned short kind;
{
	short row, col;
	object *nm;
	short tc;

	row = monster->row;
	col = monster->col;

	switch(kind) {
	case SLOW_MONSTER:
		if (monster->m_flags & HASTED) {
			monster->m_flags &= (~HASTED);
		} else {
			monster->slowed_toggle = 0;
			monster->m_flags |= SLOWED;
		}
		break;
	case HASTE_MONSTER:
		if (monster->m_flags & SLOWED) {
			monster->m_flags &= (~SLOWED);
		} else {
			monster->m_flags |= HASTED;
		}
		break;
	case TELE_AWAY:
		tele_away(monster);
		break;
	case CONFUSE_MONSTER:
		monster->m_flags |= CONFUSED;
		monster->moves_confused += get_rand(12, 22);
		break;
	case INVISIBILITY:
		monster->m_flags |= INVISIBLE;
		break;
	case POLYMORPH:
		if (monster->m_flags & HOLDS) {
			being_held = 0;
		}
		nm = monster->next_monster;
		tc = monster->trail_char;
		(void) gr_monster(monster, get_rand(0, MONSTERS-1));
		monster->row = row;
		monster->col = col;
		monster->next_monster = nm;
		monster->trail_char = tc;
		if (!(monster->m_flags & IMITATES)) {
			wake_up(monster);
		}
		break;
	case PUT_TO_SLEEP:
		monster->m_flags |= (ASLEEP | NAPPING);
		monster->nap_length = get_rand(3, 6);
		break;
	case MAGIC_MISSILE:
		rogue_hit(monster, 1);
		break;
	case CANCELLATION:
		if (monster->m_flags & HOLDS) {
			being_held = 0;
		}
		if (monster->m_flags & STEALS_ITEM) {
			monster->drop_percent = 0;
		}
		monster->m_flags &= (~(FLIES | FLITS | SPECIAL_HIT | INVISIBLE |
			FLAMES | IMITATES | CONFUSES | SEEKS_GOLD | HOLDS));
		break;
	case DO_NOTHING:
		message("nothing happens", 0);
		break;
	}
}

void
tele_away(monster)
object *monster;
{
    short row, col;

    if (monster->m_flags & HOLDS) {
        being_held = 0;
    }
    gr_row_col(&row, &col, (FLOOR | TUNNEL | STAIRS | OBJECT));
    mvaddch(monster->row, monster->col, monster->trail_char);
    dungeon[monster->row][monster->col] &= ~MONSTER;
    monster->row = row; monster->col = col;
    dungeon[row][col] |= MONSTER;
    monster->trail_char = (short) mvinch(row, col);
    if (detect_monster || rogue_can_see(row, col)) {
        mvaddch(row, col, gmc(monster));
    }
}

void
wizardize()
{
	char buf[100];

	if (wizard) {
		wizard = 0;
		message("not wizard anymore", 0);
	} else {
		if (get_input_line("wizard's password:", "", buf, "", 0, 0)) {
			(void) xxx(1);
			xxxx(buf, strlen(buf));
			if (!strncmp(buf, "\247\104\126\272\115\243\027", 7)) {
				wizard = 1;
				score_only = 1;
				message("Welcome, mighty wizard!", 0);
			} else {
				message("sorry", 0);
			}
		}
	}
}
