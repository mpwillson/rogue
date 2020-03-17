/*
 * hit.c
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
#include <stdio.h>
#include <string.h>
#include "rogue.h"

object *fight_monster = 0;
boolean detect_monster;
char hit_message[80] = "";

/* forward decls */
void rogue_damage();
int special_hit();
int get_hit_chance();
int get_weapon_damage();
int get_damage();
int mon_damage();
int to_hit();
int damage_for_strength();

extern short halluc, blind, cur_level;
extern short add_strength, ring_exp, r_rings;
extern boolean being_held, interrupted, wizard;

void
mon_hit(monster, other, flame)
register object *monster;
char *other;
boolean flame;
{
	short damage, hit_chance;
	char *mn;
	int minus;

	if (fight_monster && (monster != fight_monster)) {
		fight_monster = 0;
	}
	monster->trow = NO_ROOM;
	if (cur_level >= (AMULET_LEVEL * 2)) {
		hit_chance = 100;
	} else {
		hit_chance = monster->m_hit_chance;
		hit_chance -= (((2 * rogue.exp) + (2 * ring_exp)) - r_rings);
	}
	if (wizard) {
		hit_chance /= 2;
	}
	if (!fight_monster) {
		interrupted = 1;
	}
	mn = mon_name(monster);

	if (other) {
		hit_chance -= ((rogue.exp + ring_exp) - r_rings);
	}

	if (!rand_percent(hit_chance)) {
		if (!fight_monster) {
			sprintf(hit_message + strlen(hit_message),
			"the %s misses", (other ? other : mn));
			message(hit_message, 1);
			hit_message[0] = 0;
		}
		return;
	}
	if (!fight_monster) {
		sprintf(hit_message + strlen(hit_message), "the %s hit",
			(other ? other : mn));
		message(hit_message, 1);
		hit_message[0] = 0;
	}
	if (!(monster->m_flags & STATIONARY)) {
		damage = get_damage(monster->m_damage, 1);
		if (other) {
			if (flame) {
				if ((damage -= get_armor_class(rogue.armor)) < 0) {
					damage = 1;
				}
			}
		}
		if (cur_level >= (AMULET_LEVEL * 2)) {
			minus =  ((AMULET_LEVEL * 2) - cur_level);
		} else {
			minus = get_armor_class(rogue.armor) * 3.00;
			minus = minus/100 *  damage;
		}
		damage -= (short) minus;
	} else {
		damage = monster->stationary_damage++;
	}
	if (wizard) {
		damage /= 3;
	}
	if (damage > 0) {
		rogue_damage(damage, monster);
	}
	if (monster->m_flags & SPECIAL_HIT) {
		special_hit(monster);
	}
}

void
rogue_hit(monster, force_hit)
register object *monster;
boolean force_hit;
{
	short damage, hit_chance;

	if (monster) {
		if (check_imitator(monster)) {
			return;
		}
		hit_chance = force_hit ? 100 : get_hit_chance(rogue.weapon);

		if (wizard) {
			hit_chance *= 2;
		}
		if (!rand_percent(hit_chance)) {
			if (!fight_monster) {
				(void) strcpy(hit_message, "you miss  ");
			}
			goto RET;
		}
		damage = get_weapon_damage(rogue.weapon);
		if (wizard) {
			damage *= 3;
		}
		if (mon_damage(monster, damage)) {	/* still alive? */
			if (!fight_monster) {
				(void) strcpy(hit_message, "you hit  ");
			}
		}
RET:	check_gold_seeker(monster);
		wake_up(monster);
	}
}

void
rogue_damage(d, monster)
short d;
object *monster;
{
	if (d >= rogue.hp_current) {
		rogue.hp_current = 0;
		print_stats(STAT_HP);
		killed_by(monster, 0);
	}
	rogue.hp_current -= d;
	print_stats(STAT_HP);
}

int
get_damage(ds, r)
char *ds;
boolean r;
{
	register int i = 0, j, n, d, total = 0;

	while (ds[i]) {
		n = get_number(ds+i);
		while (ds[i++] != 'd') ;
		d = get_number(ds+i);
		while ((ds[i] != '/') && ds[i]) i++;

		for (j = 0; j < n; j++) {
			if (r) {
				total += get_rand(1, d);
			} else {
				total += d;
			}
		}
		if (ds[i] == '/') {
			i++;
		}
	}
	return(total);
}

int
get_w_damage(obj)
object *obj;
{
	char new_damage[12];
	register int to_hit, damage;
	register int i = 0;

	if ((!obj) || (obj->what_is != WEAPON)) {
		return(-1);
	}
	to_hit = get_number(obj->damage) + obj->hit_enchant;
	while (obj->damage[i++] != 'd') ;
	damage = get_number(obj->damage + i) + obj->d_enchant;

	sprintf(new_damage, "%dd%d", to_hit, damage);

	return(get_damage(new_damage, 1));
}

int
get_number(s)
register char *s;
{
	register int i = 0;
	register int total = 0;

	while ((s[i] >= '0') && (s[i] <= '9')) {
		total = (10 * total) + (s[i] - '0');
		i++;
	}
	return(total);
}

long
lget_number(s)
register char *s;
{
	register long i = 0;
	register long total = 0;

	while ((s[i] >= '0') && (s[i] <= '9')) {
		total = (10 * total) + (s[i] - '0');
		i++;
	}
	return(total);
}

int
to_hit(obj)
object *obj;
{
	if (!obj) {
		return(1);
	}
	return(get_number(obj->damage) + obj->hit_enchant);
}

int
damage_for_strength()
{
	short strength;

	strength = rogue.str_current + add_strength;

	if (strength <= 6) {
		return(strength-5);
	}
	if (strength <= 14) {
		return(1);
	}
	if (strength <= 17) {
		return(3);
	}
	if (strength <= 18) {
		return(4);
	}
	if (strength <= 20) {
		return(5);
	}
	if (strength <= 21) {
		return(6);
	}
	if (strength <= 30) {
		return(7);
	}
	return(8);
}

int
mon_damage(monster, damage)
object *monster;
int damage;
{
	char *mn;
	short row, col;

	monster->hp_to_kill -= damage;

	if (monster->hp_to_kill <= 0) {
		row = monster->row;
		col = monster->col;
		dungeon[row][col] &= ~MONSTER;
		mvaddch(row, col, (int) get_dungeon_char(row, col));

		fight_monster = 0;
		cough_up(monster);
		mn = mon_name(monster);
		sprintf(hit_message+strlen(hit_message), "defeated the %s", mn);
		message(hit_message, 1);
		hit_message[0] = 0;
		add_exp(monster->kill_exp, 1);
		take_from_pack(monster, &level_monsters);

		if (monster->m_flags & HOLDS) {
			being_held = 0;
		}
		free_object(monster);
		return(0);
	}
	return(1);
}

void
fight(to_the_death)
boolean to_the_death;
{
	char ch, c;
	short row, col;
	boolean first_miss = 1;
	short possible_damage;
	object *monster;

	while (!is_direction(ch = rgetchar())) {
		sound_bell();
		if (first_miss) {
			message("direction?", 0);
			first_miss = 0;
		}
	}
	check_message();
	if (ch == CANCEL) {
		return;
	}
	row = rogue.row; col = rogue.col;
	get_dir_rc(ch, &row, &col, 0);

	c = (char) mvinch(row, col);
	if (((c < 'A') || (c > 'Z')) ||
		(!can_move(rogue.row, rogue.col, row, col))) {
		message("I see no monster there", 0);
		return;
	}
	if (!(fight_monster = object_at(&level_monsters, row, col))) {
		return;
	}
	if (!(fight_monster->m_flags & STATIONARY)) {
		possible_damage = ((get_damage(fight_monster->m_damage, 0) * 2) / 3);
	} else {
		possible_damage = fight_monster->stationary_damage - 1;
	}
	while (fight_monster) {
		(void) one_move_rogue(ch, 0);
		if (((!to_the_death) && (rogue.hp_current <= possible_damage)) ||
			interrupted || (!(dungeon[row][col] & MONSTER))) {
			fight_monster = 0;
		} else {
			monster = object_at(&level_monsters, row, col);
			if (monster != fight_monster) {
				fight_monster = 0;
			}
		}
	}
}

void
get_dir_rc(dir, row, col, allow_off_screen)
short dir;
short *row, *col;
short allow_off_screen;
{
	switch(dir) {
	case 'h':
		if (allow_off_screen || (*col > 0)) {
			(*col)--;
		}
		break;
	case 'j':
		if (allow_off_screen || (*row < (DROWS-2))) {
			(*row)++;
		}
		break;
	case 'k':
		if (allow_off_screen || (*row > MIN_ROW)) {
			(*row)--;
		}
		break;
	case 'l':
		if (allow_off_screen || (*col < (DCOLS-1))) {
			(*col)++;
		}
		break;
	case 'y':
		if (allow_off_screen || ((*row > MIN_ROW) && (*col > 0))) {
			(*row)--;
			(*col)--;
		}
		break;
	case 'u':
		if (allow_off_screen || ((*row > MIN_ROW) && (*col < (DCOLS-1)))) {
			(*row)--;
			(*col)++;
		}
		break;
	case 'n':
		if (allow_off_screen || ((*row < (DROWS-2)) && (*col < (DCOLS-1)))) {
			(*row)++;
			(*col)++;
		}
		break;
	case 'b':
		if (allow_off_screen || ((*row < (DROWS-2)) && (*col > 0))) {
			(*row)++;
			(*col)--;
		}
		break;
	}
}

int
get_hit_chance(weapon)
object *weapon;
{
	short hit_chance;

	hit_chance = 40;
	hit_chance += 3 * to_hit(weapon);
	hit_chance += (((2 * rogue.exp) + (2 * ring_exp)) - r_rings);
	return(hit_chance);
}

int
get_weapon_damage(weapon)
object *weapon;
{
	short damage;

	damage = get_w_damage(weapon);
	damage += damage_for_strength();
	damage += ((((rogue.exp + ring_exp) - r_rings) + 1) / 2);
	return(damage);
}
