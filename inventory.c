/*
 * inventory.c
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

#define swap_string(x,y) {t = x; x = y; y = t;}

boolean is_wood[WANDS];

char *wand_materials[WAND_MATERIALS] = {
	"steel ",
	"bronze ",
	"gold ",
	"silver ",
	"copper ",
	"nickel ",
	"cobalt ",
	"tin ",
	"iron ",
	"magnesium ",
	"chrome ",
	"carbon ",
	"platinum ",
	"silicon ",
	"titanium ",

	"teak ",
	"oak ",
	"cherry ",
	"birch ",
	"pine ",
	"cedar ",
	"redwood ",
	"balsa ",
	"ivory ",
	"walnut ",
	"maple ",
	"mahogany ",
	"elm ",
	"palm ",
	"wooden "
};

char *gems[GEMS] = {
	"diamond ",
	"stibotantalite ",
	"lapi-lazuli ",
	"ruby ",
	"emerald ",
	"sapphire ",
	"amethyst ",
	"quartz ",
	"tiger-eye ",
	"opal ",
	"agate ",
	"turquoise ",
	"pearl ",
	"garnet "
};

char *syllables[MAXSYLLABLES] = {
	"blech ",
	"foo ",
	"barf ",
	"rech ",
	"bar ",
	"blech ",
	"quo ",
	"bloto ",
	"woh ",
	"caca ",
	"blorp ",
	"erp ",
	"festr ",
	"rot ",
	"slie ",
	"snorf ",
	"iky ",
	"yuky ",
	"ooze ",
	"ah ",
	"bahl ",
	"zep ",
	"druhl ",
	"flem ",
	"behil ",
	"arek ",
	"mep ",
	"zihr ",
	"grit ",
	"kona ",
	"kini ",
	"ichi ",
	"niah ",
	"ogr ",
	"ooh ",
	"ighr ",
	"coph ",
	"swerr ",
	"mihln ",
	"poxi "
};

extern boolean wizard;

void
inventory(pack, mask)
object *pack;
unsigned short mask;
{
	object *obj;
	short i = 0, j, maxlen = 0, n;
	char descs[MAX_PACK_COUNT+1][DCOLS];
	short row, col;

	obj = pack->next_object;

	if (!obj) {
		message("your pack is empty", 0);
		return;
	}
	while (obj) {
		if (obj->what_is & mask) {
			descs[i][0] = ' ';
			descs[i][1] = obj->ichar;
			descs[i][2] = ((obj->what_is & ARMOR) && obj->is_protected)
				? '}' : ')';
			descs[i][3] = ' ';
			get_desc(obj, descs[i]+4);
			if ((n = strlen(descs[i])) > maxlen) {
				maxlen = n;
			}
		i++;
		}
		obj = obj->next_object;
	}
	(void) strcpy(descs[i++], " --press space to continue--");
	if (maxlen < 27) maxlen = 27;
	col = DCOLS - (maxlen + 2);

	for (row = 0; ((row < i) && (row < DROWS)); row++) {
		if (row > 0) {
			for (j = col; j < DCOLS; j++) {
				descs[row-1][j-col] = (char) mvinch(row, j);
			}
			descs[row-1][j-col] = 0;
		}
		mvaddstr(row, col, descs[row]);
		clrtoeol();
	}
	refresh();
	wait_for_ack();

	move(0, 0);
	clrtoeol();

	for (j = 1; ((j < i)&&(j< DROWS)); j++) {
		mvaddstr(j, col, descs[j-1]);
	}
}

void
mix_colors()
{
	short i, j, k;
	char t[128];

	for (i = 0; i <= 32; i++) {
		j = get_rand(0, (POTIONS - 1));
		while ((k = get_rand(0, (POTIONS - 1))) == j);
		strcpy(t,id_potions[j].title);
		strcpy(id_potions[j].title,id_potions[k].title);
		strcpy(id_potions[k].title,t);
	}
}

void
make_scroll_titles()
{
	short i, j, n;
	short sylls, s;

	for (i = 0; i < SCROLLS; i++) {
		sylls = get_rand(2, 5);
/*		id_scrolls[i].title=(char *)malloc(128);
		if(id_scrolls[i].title==(char *)0)
			clean_up("Panic: no memory.",0);*/
		(void) strcpy(id_scrolls[i].title, "'");

		for (j = 0; j < sylls; j++) {
			s = get_rand(1, (MAXSYLLABLES-1));
			(void) strcat(id_scrolls[i].title, syllables[s]);
		}
		n = strlen(id_scrolls[i].title);
		strcpy(id_scrolls[i].title+(n-1), "' ");
	}
}

void
get_desc(obj, desc)
object *obj;
char *desc;
{
	char *item_name;
	struct id *id_table;
	char more_info[32];
	short i;

	if (obj->what_is == AMULET) {
		(void) strcpy(desc, "the amulet of Yendor ");
		return;
	}
	item_name = name_of(obj);

	if (obj->what_is == GOLD) {
		sprintf(desc, "%d pieces of gold", obj->quantity);
		return;
	}

	if (obj->what_is != ARMOR) {
		if (obj->quantity == 1) {
			(void) strcpy(desc, "a ");
		} else {
			sprintf(desc, "%d ", obj->quantity);
		}
	}
	if (obj->what_is == FOOD) {
		if (obj->which_kind == RATION) {
			if (obj->quantity > 1) {
				sprintf(desc, "%d rations of ", obj->quantity);
			} else {
				(void) strcpy(desc, "some ");
			}
		} else {
			(void) strcpy(desc, "a ");
		}
		(void) strcat(desc, item_name);
		goto ANA;
	}
	id_table = get_id_table(obj);

	if (wizard) {
		goto ID;
	}
	if (obj->what_is & (WEAPON | ARMOR | WAND | RING)) {
		goto CHECK;
	}

	switch(id_table[obj->which_kind].id_status) {
	case UNIDENTIFIED:
CHECK:
		switch(obj->what_is) {
		case SCROLL:
			(void) strcat(desc, item_name);
			(void) strcat(desc, "entitled: ");
			(void) strcat(desc, id_table[obj->which_kind].title);
			break;
		case POTION:
			(void) strcat(desc, id_table[obj->which_kind].title);
			(void) strcat(desc, item_name);
			break;
		case WAND:
		case RING:
			if (obj->identified ||
			(id_table[obj->which_kind].id_status == IDENTIFIED)) {
				goto ID;
			}
			if (id_table[obj->which_kind].id_status == CALLED) {
				goto CALL;
			}
			(void) strcat(desc, id_table[obj->which_kind].title);
			(void) strcat(desc, item_name);
			break;
		case ARMOR:
			if (obj->identified) {
				goto ID;
			}
			(void) strcpy(desc, id_table[obj->which_kind].title);
			break;
		case WEAPON:
			if (obj->identified) {
				goto ID;
			}
			(void) strcat(desc, name_of(obj));
			break;
		}
		break;
	case CALLED:
CALL:	switch(obj->what_is) {
		case SCROLL:
		case POTION:
		case WAND:
		case RING:
			(void) strcat(desc, item_name);
			(void) strcat(desc, "called ");
			(void) strcat(desc, id_table[obj->which_kind].title);
			break;
		}
		break;
	case IDENTIFIED:
ID:		switch(obj->what_is) {
		case SCROLL:
		case POTION:
			(void) strcat(desc, item_name);
			(void) strcat(desc, id_table[obj->which_kind].real);
			break;
		case RING:
			if (wizard || obj->identified) {
				if ((obj->which_kind == DEXTERITY) ||
					(obj->which_kind == ADD_STRENGTH)) {
					sprintf(more_info, "%s%d ", ((obj->class > 0) ? "+" : ""),
						obj->class);
					(void) strcat(desc, more_info);
				}
			}
			(void) strcat(desc, item_name);
			(void) strcat(desc, id_table[obj->which_kind].real);
			break;
		case WAND:
			(void) strcat(desc, item_name);
			(void) strcat(desc, id_table[obj->which_kind].real);
			if (wizard || obj->identified) {
				sprintf(more_info, "[%d]", obj->class);
				(void) strcat(desc, more_info);
			}
			break;
		case ARMOR:
			sprintf(desc, "%s%d ", ((obj->d_enchant >= 0) ? "+" : ""),
			obj->d_enchant);
			(void) strcat(desc, id_table[obj->which_kind].title);
			sprintf(more_info, "[%d] ", get_armor_class(obj));
			(void) strcat(desc, more_info);
			break;
		case WEAPON:
			sprintf(desc+strlen(desc), "%s%d,%s%d ",
			((obj->hit_enchant >= 0) ? "+" : ""),
			obj->hit_enchant,
			((obj->d_enchant >= 0) ? "+" : ""),
			obj->d_enchant);
			(void) strcat(desc, name_of(obj));
			break;
		}
		break;
	}
ANA:
	if (!strncmp(desc, "a ", 2)) {
		if (is_vowel(desc[2])) {
			for (i = strlen(desc) + 1; i > 1; i--) {
				desc[i] = desc[i-1];
			}
			desc[1] = 'n';
		}
	}
	if (obj->in_use_flags & BEING_WIELDED) {
		(void) strcat(desc, "in hand");
	} else if (obj->in_use_flags & BEING_WORN) {
		(void) strcat(desc, "being worn");
	} else if (obj->in_use_flags & ON_LEFT_HAND) {
		(void) strcat(desc, "on left hand");
	} else if (obj->in_use_flags & ON_RIGHT_HAND) {
		(void) strcat(desc, "on right hand");
	}
}

void
get_wand_and_ring_materials()
{
	short i, j;
	boolean used[WAND_MATERIALS];

	for (i = 0; i < WAND_MATERIALS; i++) {
		used[i] = 0;
	}
	for (i = 0; i < WANDS; i++) {
		do {
			j = get_rand(0, WAND_MATERIALS-1);
		} while (used[j]);
		used[j] = 1;
/*		id_wands[i].title=(char *)malloc(128);*/
		(void) strcpy(id_wands[i].title, wand_materials[j]);
		is_wood[i] = (j > MAX_METAL);
	}
	for (i = 0; i < GEMS; i++) {
		used[i] = 0;
	}
	for (i = 0; i < RINGS; i++) {
		do {
			j = get_rand(0, GEMS-1);
		} while (used[j]);
		used[j] = 1;
/*		id_rings[i].title=(char *)malloc(128);*/
		(void) strcpy(id_rings[i].title, gems[j]);
	}
}

void
single_inv(ichar)
short ichar;
{
	short ch;
	char desc[DCOLS];
	object *obj;

	ch = ichar ? ichar : pack_letter("inventory what?", ALL_OBJECTS);

	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
		message("no such item.", 0);
		return;
	}
	desc[0] = ch;
	desc[1] = ((obj->what_is & ARMOR) && obj->is_protected) ? '}' : ')';
	desc[2] = ' ';
	desc[3] = 0;
	get_desc(obj, desc+3);
	message(desc, 0);
}

struct id *
get_id_table(obj)
object *obj;
{
	switch(obj->what_is) {
	case SCROLL:
		return(id_scrolls);
	case POTION:
		return(id_potions);
	case WAND:
		return(id_wands);
	case RING:
		return(id_rings);
	case WEAPON:
		return(id_weapons);
	case ARMOR:
		return(id_armors);
	}
	return((struct id *) 0);
}

void
inv_armor_weapon(is_weapon)
boolean is_weapon;
{
	if (is_weapon) {
		if (rogue.weapon) {
			single_inv(rogue.weapon->ichar);
		} else {
			message("not wielding anything", 0);
		}
	} else {
		if (rogue.armor) {
			single_inv(rogue.armor->ichar);
		} else {
			message("not wearing anything", 0);
		}
	}
}
