/*
 * rogue.h
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  This notice shall not be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#define boolean char

#define NOTHING		((unsigned short)     0)
#define OBJECT		((unsigned short)    01)
#define MONSTER		((unsigned short)    02)
#define STAIRS		((unsigned short)    04)
#define HORWALL		((unsigned short)   010)
#define VERTWALL	((unsigned short)   020)
#define DOOR		((unsigned short)   040)
#define FLOOR		((unsigned short)  0100)
#define TUNNEL		((unsigned short)  0200)
#define TRAP		((unsigned short)  0400)
#define HIDDEN		((unsigned short) 01000)

#define ARMOR		((unsigned short)   01)
#define WEAPON		((unsigned short)   02)
#define SCROLL		((unsigned short)   04)
#define POTION		((unsigned short)  010)
#define GOLD		((unsigned short)  020)
#define FOOD		((unsigned short)  040)
#define WAND		((unsigned short) 0100)
#define RING		((unsigned short) 0200)
#define AMULET		((unsigned short) 0400)
#define ALL_OBJECTS	((unsigned short) 0777)

#define LEATHER 0
#define RINGMAIL 1
#define SCALE 2
#define CHAIN 3
#define BANDED 4
#define SPLINT 5
#define PLATE 6
#define ARMORS 7

#define BOW 0
#define DART 1
#define ARROW 2
#define DAGGER 3
#define SHURIKEN 4
#define MACE 5
#define LONG_SWORD 6
#define TWO_HANDED_SWORD 7
#define WEAPONS 8

#define MAX_PACK_COUNT 24

#define PROTECT_ARMOR 0
#define HOLD_MONSTER 1
#define ENCH_WEAPON 2
#define ENCH_ARMOR 3
#define IDENTIFY 4
#define TELEPORT 5
#define SLEEP 6
#define SCARE_MONSTER 7
#define REMOVE_CURSE 8
#define CREATE_MONSTER 9
#define AGGRAVATE_MONSTER 10
#define MAGIC_MAPPING 11
#define SCROLLS 12

#define INCREASE_STRENGTH 0
#define RESTORE_STRENGTH 1
#define HEALING 2
#define EXTRA_HEALING 3
#define POISON 4
#define RAISE_LEVEL 5
#define BLINDNESS 6
#define HALLUCINATION 7
#define DETECT_MONSTER 8
#define DETECT_OBJECTS 9
#define CONFUSION 10
#define LEVITATION 11
#define HASTE_SELF 12
#define SEE_INVISIBLE 13
#define POTIONS 14

#define TELE_AWAY 0
#define SLOW_MONSTER 1
#define CONFUSE_MONSTER 2
#define INVISIBILITY 3
#define POLYMORPH 4
#define HASTE_MONSTER 5
#define PUT_TO_SLEEP 6
#define MAGIC_MISSILE 7
#define CANCELLATION 8
#define DO_NOTHING 9
#define WANDS 10

#define STEALTH 0
#define R_TELEPORT 1
#define REGENERATION 2
#define SLOW_DIGEST 3
#define ADD_STRENGTH 4
#define SUSTAIN_STRENGTH 5
#define DEXTERITY 6
#define ADORNMENT 7
#define R_SEE_INVISIBLE 8
#define MAINTAIN_ARMOR 9
#define SEARCHING 10
#define RINGS 11

#define RATION 0
#define FRUIT 1

#define NOT_USED		((unsigned short)   0)
#define BEING_WIELDED	((unsigned short)  01)
#define BEING_WORN		((unsigned short)  02)
#define ON_LEFT_HAND	((unsigned short)  04)
#define ON_RIGHT_HAND	((unsigned short) 010)
#define ON_EITHER_HAND	((unsigned short) 014)
#define BEING_USED		((unsigned short) 017)

#define NO_TRAP -1
#define TRAP_DOOR 0
#define BEAR_TRAP 1
#define TELE_TRAP 2
#define DART_TRAP 3
#define SLEEPING_GAS_TRAP 4
#define RUST_TRAP 5
#define TRAPS 6

#define STEALTH_FACTOR 3
#define R_TELE_PERCENT 8

#define UNIDENTIFIED ((unsigned short) 00)	/* MUST BE ZERO! */
#define IDENTIFIED ((unsigned short) 01)
#define CALLED ((unsigned short) 02)

#define DROWS 24
#define DCOLS 80
#define MAX_TITLE_LENGTH 30
#define MORE "-more-"
#define MAXSYLLABLES 40
#define MAX_METAL 14
#define WAND_MATERIALS 30
#define GEMS 14

#define GOLD_PERCENT 46

struct id {
	short value;
	char title[128];
	char real[128];
	unsigned short id_status;
};

/* The following #defines provide more meaningful names for some of the
 * struct object fields that are used for monsters.  This, since each monster
 * and object (scrolls, potions, etc) are represented by a struct object.
 * Ideally, this should be handled by some kind of union structure.
 */

#define m_damage damage
#define hp_to_kill quantity
#define m_char ichar
#define first_level is_protected
#define last_level is_cursed
#define m_hit_chance class
#define stationary_damage identified
#define drop_percent which_kind
#define trail_char d_enchant
#define slowed_toggle quiver
#define moves_confused hit_enchant
#define nap_length picked_up
#define disguise what_is
#define next_monster next_object

struct obj {				/* comment is monster meaning */
	unsigned long m_flags;	/* monster flags */
	char *damage;			/* damage it does */
	short quantity;			/* hit points to kill */
	short ichar;			/* 'A' is for aquatar */
	short kill_exp;			/* exp for killing it */
	short is_protected;		/* level starts */
	short is_cursed;		/* level ends */
	short class;			/* chance of hitting you */
	short identified;		/* 'F' damage, 1,2,3... */
	unsigned short which_kind; /* item carry/drop % */
	short o_row, o_col, o;	/* o is how many times stuck at o_row, o_col */
	short row, col;			/* current row, col */
	short d_enchant;		/* room char when detect_monster */
	short quiver;			/* monster slowed toggle */
	short trow, tcol;		/* target row, col */
	short hit_enchant;		/* how many moves is confused */
	unsigned short what_is;	/* imitator's charactor (?!%: */
	short picked_up;		/* sleep from wand of sleep */
	unsigned short in_use_flags;
	struct obj *next_object;	/* next monster */
};

typedef struct obj object;

#define INIT_HP 12

struct fight {
	object *armor;
	object *weapon;
	object *left_ring, *right_ring;
	short hp_current;
	short hp_max;
	short str_current;
	short str_max;
	object pack;
	long gold;
	short exp;
	long exp_points;
	short row, col;
	short fchar;
	short moves_left;
};

typedef struct fight fighter;

struct dr {
	short oth_room;
	short oth_row,
	      oth_col;
	short door_row,
		  door_col;
};

typedef struct dr door;

struct rm {
	char bottom_row, right_col, left_col, top_row;
	door doors[4];
	unsigned short is_room;
};

typedef struct rm room;

#define MAXROOMS 9
#define BIG_ROOM 10

#define NO_ROOM -1

#define PASSAGE -3		/* cur_room value */

#define AMULET_LEVEL 26

#define R_NOTHING	((unsigned short) 01)
#define R_ROOM		((unsigned short) 02)
#define R_MAZE		((unsigned short) 04)
#define R_DEADEND	((unsigned short) 010)
#define R_CROSS		((unsigned short) 020)

#define MAX_EXP_LEVEL 21
#define MAX_EXP 10000000L
#define MAX_GOLD 900000
#define MAX_ARMOR 99
#define MAX_HP 800
#define MAX_STRENGTH 99
#define LAST_DUNGEON 99

#define STAT_LEVEL 01
#define STAT_GOLD 02
#define STAT_HP 04
#define STAT_STRENGTH 010
#define STAT_ARMOR 020
#define STAT_EXP 040
#define STAT_HUNGER 0100
#define STAT_LABEL 0200
#define STAT_ALL 0377

#define PARTY_TIME 10	/* one party somewhere in each 10 level span */

#define MAX_TRAPS 10	/* maximum traps per level */

#define HIDE_PERCENT 12

struct tr {
	short trap_type;
	short trap_row, trap_col;
};

typedef struct tr trap;

extern fighter rogue;
extern room rooms[];
extern trap traps[];
extern unsigned short dungeon[DROWS][DCOLS];
extern object level_objects;

extern struct id id_scrolls[];
extern struct id id_potions[];
extern struct id id_wands[];
extern struct id id_rings[];
extern struct id id_weapons[];
extern struct id id_armors[];

extern object mon_tab[];
extern object level_monsters;

#define MONSTERS 26

#define HASTED					01L
#define SLOWED					02L
#define INVISIBLE				04L
#define ASLEEP				   010L
#define WAKENS				   020L
#define WANDERS				   040L
#define FLIES				  0100L
#define FLITS				  0200L
#define CAN_FLIT			  0400L		/* can, but usually doesn't, flit */
#define CONFUSED	 		 01000L
#define RUSTS				 02000L
#define HOLDS				 04000L
#define FREEZES				010000L
#define STEALS_GOLD			020000L
#define STEALS_ITEM			040000L
#define STINGS			   0100000L
#define DRAINS_LIFE		   0200000L
#define DROPS_LEVEL		   0400000L
#define SEEKS_GOLD		  01000000L
#define FREEZING_ROGUE	  02000000L
#define RUST_VANISHED	  04000000L
#define CONFUSES		 010000000L
#define IMITATES		 020000000L
#define FLAMES			 040000000L
#define STATIONARY		0100000000L		/* damage will be 1,2,3,... */
#define NAPPING			0200000000L		/* can't wake up for a while */
#define ALREADY_MOVED	0400000000L

#define SPECIAL_HIT		(RUSTS|HOLDS|FREEZES|STEALS_GOLD|STEALS_ITEM|STINGS|DRAINS_LIFE|DROPS_LEVEL)

#define WAKE_PERCENT 45
#define FLIT_PERCENT 33
#define PARTY_WAKE_PERCENT 75

#define HYPOTHERMIA 1
#define STARVATION 2
#define POISON_DART 3
#define QUIT 4
#define WIN 5

#define UP 0
#define UPRIGHT 1
#define RIGHT 2
#define RIGHTDOWN 3
#define DOWN 4
#define DOWNLEFT 5
#define LEFT 6
#define LEFTUP 7
#define DIRS 8

#define ROW1 7
#define ROW2 15

#define COL1 26
#define COL2 52

#define MOVED 0
#define MOVE_FAILED -1
#define STOPPED_ON_SOMETHING -2
#define CANCEL '\033'
#define LIST '*'

#define HUNGRY 300
#define WEAK 150
#define FAINT 20
#define STARVE 0

#define MIN_ROW 1

/* external routine declarations.
 */
char *mon_name();
char *get_ench_color();
char *name_of();
char *md_gln();
char *md_getenv();
char *md_malloc();
boolean is_direction();
boolean mon_sees();
boolean mask_pack();
boolean mask_room();
boolean is_digit();
boolean check_hunger();
boolean reg_move();
boolean md_df();
boolean has_been_touched();
object *add_to_pack();
object *alloc_object();
object *get_letter_object();
object *gr_monster();
object *get_thrown_at_monster();
object *get_zapped_monster();
object *check_duplicate();
object *gr_object();
object *object_at();
object *pick_up();
struct id *get_id_table();
unsigned short gr_what_is();
long rrandom();
long lget_number();
long xxx();

/* additional extern decls for cross module routines */

/* main.c */
void turn_into_games();
void turn_into_user();


/* init.c */
void onintr();
void byebye();
void error_save();
void clean_up();
int init();
void start_window();

/* random.c */
void srrandom();
int get_rand();
int rand_percent();
int coin_toss();

/* message.c */
void message();
void print_stats();
int rgetchar();
void sound_bell();
void check_message();
int r_index();
int get_input_line();
void remessage();

/* object.c */
int get_armor_class();
void free_object();
void get_food();
void put_objects();
void put_stairs();
void free_stuff();
void show_objects();
void new_object_for_wizard();

/* hit.c */
void mon_hit();
void get_dir_rc();
void rogue_hit();
int get_number();
void fight();
int get_hit_chance();
int get_weapon_damage();
int mon_damage();
int get_damage();

/* spec_hit.c */
int check_imitator();
void check_gold_seeker();
void cough_up();
int m_confuse();
int flame_broil();
int seek_gold();
int imitating();
void rust();

/* monster.c */
void wake_up();
void put_mons();
void mv_monster();
int mon_can_go();
int rogue_is_around();
void move_mon_to();
int rogue_can_see();
void wake_room();
void party_monsters();
void mv_aquatars();
void show_monsters();
int gmc_row_col();
void create_monster();
void aggravate();
int gr_obj_char();
int gmc();

/* score.c */
void killed_by();
void put_scores();
void quit();
int is_vowel();
void win();
void xxxx();

/* room.c */
int get_dungeon_char();
int is_all_connected();
void gr_row_col();
void dr_course();
int get_room_number();
void light_up_room();
void light_passage();
int gr_room();
void darken_room();
int party_objects();
int get_mask_char();
void draw_magic_map();
void visit_rooms();

/* level.c */
void add_exp();
void clear_level();
void make_level();
int drop_check();
int check_up();
void show_average_hp();
int hp_raise();

/* trap.c */
void add_traps();
void trap_player();
void search();
void id_trap();
void show_traps();

/* pack.c */
void take_from_pack();
void wait_for_ack();
void unwear();
void unwield();
int has_amulet();

/* move.c */
int can_move();
int one_move_rogue();
int is_passable();
void mv_mons();
void wanderer();
void rest();
void multiple_move_rogue();
void move_onto();

/* machdep.c */
void md_heed_signals();
int md_gseed();
void md_exit();
void md_control_keybord(); /* sic */
void md_ignore_signals();
void md_slurp();
int md_get_file_id();
void md_gct();
int md_link_count();
void md_gfmt();
void md_sleep();

/* *save.c */
void restore();
void save_into_file();
void save_game();

/* inventory.c */
void mix_colors();
void get_wand_and_ring_materials();
void make_scroll_titles();
void get_desc();
void inventory();
void inv_armor_weapon();
void single_inv();

/* ring.c */
void ring_stats();
void un_put_on();
void put_on_ring();
void remove_ring();
void inv_rings();
void do_put_on();

/* pack.c */
void do_wear();
void do_wield();
int pack_letter();
int pack_count();
void place_at();
void drop();
void take_off();
void wear();
void wield();
void call_it();
void kick_into_pack();

/* use.c */
void put_player();
void unhallucinate();
void hallucinate();
void unblind();
void unconfuse();
void vanish();
void eat();
void quaff();
void read_scroll();
void relight();
void take_a_nap();
void confuse();

/* play.c */
void play_level();
void tele();

/* throw.c */
void rand_around();
void throw();

/* instruct.c */
void Instructions();

/* zap.c */
void wizardize();
void zapp();
void zap_monster();

struct rogue_time {
	short year;		/* >= 1987 */
	short month;	/* 1 - 12 */
	short day;		/* 1 - 31 */
	short hour;		/* 0 - 23 */
	short minute;	/* 0 - 59 */
	short second;	/* 0 - 59 */
};

#ifdef RCURSES
#define FALSE 0
#define TRUE 1

struct _win_st {
	short _cury, _curx;
	short _maxy, _maxx;
};

typedef struct _win_st WINDOW;

extern int LINES, COLS;
extern WINDOW *curscr, *stdscr;
extern char *CL;

/* curses requires additional md support */
void md_cbreak_no_echo_nonl();
void md_tstp();
char *md_gdtcf();


/* curses function decls */
void initscr();
void endwin();
void move();
void mvaddstr();
void addstr();
void addch();
void mvaddch();
void refresh();
void wrefresh();
int mvinch();
void clear();
void clrtoeol();
void standout();
void standend();
int noecho();
int nonl();
int cbreak();
void clear_buffers();
void put_char_at();
void put_cursor();
void put_st_char();
void get_term_info();
boolean tc_tname();
void tc_gtdata();
void tc_gets();
void tc_gnum();
void tstp();
void tc_cmget();
void keypad();
int getch();

/* keyboard arrow key support */
#define KEY_UP 0403
#define KEY_DOWN 0402
#define KEY_RIGHT 0405
#define KEY_LEFT 0404

#endif /* RCURSES */