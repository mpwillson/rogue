/*
 * curses.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#ifdef RCURSES

/* The following is a curses emulation package suitable for the rogue program
 * in which it is included.  No other suitability is claimed or suspected.
 * Only those routines currently needed by this rogue program are included.
 * This is being provided for those systems that don't have a suitable
 * curses package and want to run this rogue program.
 *
 * Compile the entire program with -DRCURSES to incorporate this package.
 *
 * The following is NOT supported:
 *   "%D", "%B", "%n", or "%>" inside a cursor motion (cm) termcap string.
 *   Terminals in which the cursor motion addresses the row differently from
 *       the column, as in ":cm=\E%2,%3" or ":cm=\EY%+x;%+y"
 *   Termcap database stored in the TERMCAP environ variable as returned
 *       from md_getenv().  Only the termcap file name can be stored there.
 *       See the comments for md_getenv() in machdep.c.
 *   Terminals without non-destructive backspace.  Backspace (^H) is used
 *       for cursor motion regardless of any termcap entries.
 *   The ":tc=" termcap entry is ignored.
 *
 * Suggestions:
 *   Use line-feed as your termcap "do" entry: ":do=^J", ":do=\012" or
 *      ":do=\n"  This will help cursor motion optimization.  If line-feed
 *      won't work, then a short escape sequence will do.  Same goes for "up"
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include "rogue.h"

#define BS 010
#define LF 012
#define CR 015
#define ESC '\033'
#define TAB '\011'

#define ST_MASK 0x80
#define BUFLEN 256

char terminal[DROWS][DCOLS];
char buffer[DROWS][DCOLS];
char *tc_file;

char cm_esc[16], cm_sep[16], cm_end[16];
boolean cm_reverse = 0;
boolean cm_two = 0;
boolean cm_three = 0;
boolean cm_char = 0;
short cm_inc = 0;

boolean screen_dirty;
boolean lines_dirty[DROWS];
boolean buf_stand_out = 0;
boolean term_stand_out = 0;

int LINES = DROWS, COLS = DCOLS;
WINDOW scr_buf;
WINDOW *curscr = &scr_buf;
WINDOW *stdscr;

char *CL = (char *) 0;
char *CM = (char *) 0;
char *UC = (char *) 0;
char *DO = (char *) 0;
char *VS = "";
char *VE = "";
char *TI = "";
char *TE = "";
char *SO = "";
char *SE = "";
/* arrow key support */
char *KU = "";
char *KD = "";
char *KL = "";
char *KR = "";

short cur_row, cur_col;

void
initscr()
{
	get_term_info();
	clear();
	printf("%s%s", TI, VS);
}

void
endwin()
{
	printf("%s%s", TE, VE);
	md_cbreak_no_echo_nonl(FALSE);
}

void
move(row, col)
short row, col;
{
	curscr->_cury = row;
	curscr->_curx = col;
	screen_dirty = 1;
}

void
mvaddstr(row, col, str)
short row, col;
char *str;
{
	move(row, col);
	addstr(str);
}

void
addstr(str)
char *str;
{
	while (*str) {
		addch((int) *str++);
	}
}

void
addch(ch)
register int ch;
{
	short row, col;

	row = curscr->_cury;
	col = curscr->_curx++;

	if (buf_stand_out) {
		ch |= ST_MASK;
	}
	buffer[row][col] = (char) ch;
	lines_dirty[row] = 1;
	screen_dirty = 1;
}

void
mvaddch(row, col, ch)
short row, col;
int ch;
{
	move(row, col);
	addch(ch);
}

void
refresh()
{
	register int i, j, line;
	short old_row, old_col, first_row;

	if (screen_dirty) {

		old_row = curscr->_cury;
		old_col = curscr->_curx;
		first_row = cur_row;

		for (i = 0; i < DROWS; i++) {
			line = (first_row + i) % DROWS;
			if (lines_dirty[line]) {
				for (j = 0; j < DCOLS; j++) {
					if (buffer[line][j] != terminal[line][j]) {
						put_char_at(line, j, buffer[line][j]);
					}
				}
				lines_dirty[line] = 0;
			}
		}
		put_cursor(old_row, old_col);
		screen_dirty = 0;
		fflush(stdout);
	}
}

void
wrefresh(scr)
WINDOW *scr;
{
	short i, col;

	printf("%s", CL);
	cur_row = cur_col = 0;

	for (i = 0; i < DROWS; i++) {
		col = 0;
		while (col < DCOLS) {
			while ((col < DCOLS) && (buffer[i][col] == ' ')) {
				col++;
			}
			if (col < DCOLS) {
				put_cursor(i, col);
			}
			while ((col < DCOLS) && (buffer[i][col] != ' ')) {
				put_st_char((int) buffer[i][col]);
				cur_col++;
				col++;
			}
		}
	}
	put_cursor(curscr->_cury, curscr->_curx);
	fflush(stdout);
	scr = scr;		/* make lint happy */
}

int
mvinch(row, col)
short row, col;
{
	move(row, col);
	return((int) buffer[row][col]);
}

void
clear()
{
	printf("%s", CL);
	fflush(stdout);
	cur_row = cur_col = 0;
	move(0, 0);
	clear_buffers();
}

void
clrtoeol()
{
	short row, col;

	row = curscr->_cury;

	for (col = curscr->_curx; col < DCOLS; col++) {
		buffer[row][col] = ' ';
	}
	lines_dirty[row] = 1;
}

void
standout()
{
	buf_stand_out = 1;
}

void
standend()
{
	buf_stand_out = 0;
}

void
clear_buffers()
{
	register int i, j;

	screen_dirty = 0;

	for (i = 0; i < DROWS; i++) {
		lines_dirty[i] = 0;
		for (j = 0; j < DCOLS; j++) {
			terminal[i][j] = ' ';
			buffer[i][j] = ' ';
		}
	}
}

void
put_char_at(row, col, ch)
register int row, col, ch;
{
	put_cursor(row, col);
	put_st_char(ch);
	terminal[row][col] = (char) ch;
	cur_col++;
}

void
put_cursor(row, col)
register int row, col;
{
	register int i, rdif, cdif;
	short ch, t;

	rdif = (row > cur_row) ? row - cur_row : cur_row - row;
	cdif = (col > cur_col) ? col - cur_col : cur_col - col;

	if (((row > cur_row) && DO) || ((cur_row > row) && UC)) {
		if ((rdif < 4) && (cdif < 4)) {
			for (i = 0; i < rdif; i++) {
				printf("%s", ((row < cur_row) ? UC : DO));
			}
			cur_row = row;
			if (col == cur_col) {
				return;
			}
		}
	}
	if (row == cur_row) {
		if (cdif <= 6) {
		for (i = 0; i < cdif; i++) {
				ch = (col < cur_col) ? BS :
						terminal[row][cur_col + i];
				put_st_char((int) ch);
			}
			cur_row = row;
			cur_col = col;
			return;
		}
	}
	cur_row = row;
	cur_col = col;

	row += cm_inc;
	col += cm_inc;

	if (cm_reverse) {
		t = row;
		row = col;
		col = t;
	}
	if (cm_two) {
		printf("%s%02d%s%02d%s", cm_esc, row, cm_sep, col, cm_end);
	} else if (cm_three) {
		printf("%s%03d%s%03d%s", cm_esc, row, cm_sep, col, cm_end);
	} else if (cm_char) {
		printf("%s%c%s%c%s", cm_esc, row, cm_sep, col, cm_end);
	} else {
		printf("%s%d%s%d%s", cm_esc, row, cm_sep, col, cm_end);
	}
}

void
put_st_char(ch)
register int ch;
{
	if ((ch & ST_MASK) && (!term_stand_out)) {
		ch &= ~ST_MASK;
		printf("%s%c", SO, ch);
		term_stand_out = 1;
	} else if ((!(ch & ST_MASK)) && term_stand_out) {
		printf("%s%c", SE, ch);
		term_stand_out = 0;
	} else {
		ch &= ~ST_MASK;
		putchar(ch);
	}
}

void
get_term_info()
{
	FILE *fp;
	char *term, *tcf;
	char buf[BUFLEN];

	if ((tcf = md_getenv("TERMCAP"))) {
		if (strlen(tcf) > 40) {
			clean_up("TERMCAP file name too long");
		}
		tc_file = tcf;
	} else {
		if (!(tc_file = md_gdtcf())) {
			clean_up("I need a termcap file");
		}
	}

	if (!(term = md_getenv("TERM"))) {
		clean_up("Cannot find TERM variable in environ");
	}
	if ((fp = fopen(tc_file, "r")) == NULL) {
		sprintf(buf, "Cannot open TERMCAP file: %s", tc_file);
		clean_up(buf);
	}

	if (!tc_tname(fp, term, buf)) {
		sprintf(buf, "Cannot find TERM type: %s in TERMCAP file: %s", term,
			tc_file);
		clean_up(buf);
	}
	tc_gtdata(fp, buf);
	fclose(fp);
}

boolean
tc_tname(fp, term, buf)
FILE *fp;
char *term;
char *buf;
{
	short i, j;
	boolean found = 0;
	char *fg;

	while (!found) {
		i = 0;
		fg = fgets(buf, BUFLEN, fp);
		if (fg != NULL) {
			if (	(buf[0] != '#') && (buf[0] != ' ') && (buf[0] != TAB) &&
					(buf[0] != CR) && (buf[0] != LF)) {
				while (buf[i] && (!found)) {
					j = 0;
					while (buf[i] == term[j]) {
						i++;
						j++;
					}
					if ((!term[j]) && ((buf[i] == '|') || (buf[i] == ':'))) {
						found = 1;
					} else {
						while (buf[i] && (buf[i] != '|') && (buf[i] != ':')) {
							i++;
						}
						if (buf[i]) {
							i++;
						}
					}
				}
			}
		} else {
			break;
		}
	}
	return(found);
}

void
tc_gtdata(fp, buf)
FILE *fp;
char *buf;
{
	short i;
	boolean first = 1;

	do {
		if (!first) {
			if ((buf[0] != TAB) && (buf[0] != ' ')) {
				break;
			}
		}
		first = 0;
		i = 0;
		while (buf[i]) {
			while (buf[i] && (buf[i] != ':')) {
				i++;
			}
			if (buf[i] == ':') {
				if (!strncmp(buf + i, ":cl=", 4)) {
					tc_gets(buf + i, &CL);
				} else if (!strncmp(buf + i, ":cm=", 4)) {
					tc_gets(buf + i, &CM);
				} else if (!strncmp(buf + i, ":up=", 4)) {
					tc_gets(buf + i, &UC);
				} else if (!strncmp(buf + i, ":do=", 4)) {
					tc_gets(buf + i, &DO);
				} else if (!strncmp(buf + i, ":vs=", 4)) {
					tc_gets(buf + i, &VS);
				} else if (!strncmp(buf + i, ":ve=", 4)) {
					tc_gets(buf + i, &VE);
				} else if (!strncmp(buf + i, ":ti=", 4)) {
					tc_gets(buf + i, &TI);
				} else if (!strncmp(buf + i, ":te=", 4)) {
					tc_gets(buf + i, &TE);
				} else if (!strncmp(buf + i, ":vs=", 4)) {
					tc_gets(buf + i, &VS);
				} else if (!strncmp(buf + i, ":ve=", 4)) {
					tc_gets(buf + i, &VE);
				} else if (!strncmp(buf + i, ":so=", 4)) {
					tc_gets(buf + i, &SO);
				} else if (!strncmp(buf + i, ":se=", 4)) {
					tc_gets(buf + i, &SE);
				} else if (!strncmp(buf + i, ":li#", 4)) {
					tc_gnum(buf + i, &LINES);
				} else if (!strncmp(buf + i, ":co#", 4)) {
					tc_gnum(buf + i, &COLS);
				} else if (!strncmp(buf + i, ":ku=", 4)) {
					tc_gets(buf + i, &KU);
				} else if (!strncmp(buf + i, ":kd=", 4)) {
					tc_gets(buf + i, &KD);
				} else if (!strncmp(buf + i, ":kr=", 4)) {
					tc_gets(buf + i, &KR);
				} else if (!strncmp(buf + i, ":kl=", 4)) {
					tc_gets(buf + i, &KL);
				}
				i++;
			}
		}
	} while (fgets(buf, BUFLEN, fp) != NULL);

	if ((!CM) || (!CL)) {
		clean_up("Terminal and termcap must have cm and cl");
	}
	tc_cmget();
}

void
tc_gets(ibuf, tcstr)
char *ibuf;
char **tcstr;
{
	short i, j, k, n;
	char obuf[BUFLEN];

	i = 4;
	j = 0;

	while (ibuf[i] && is_digit(ibuf[i])) {
		i++;
	}

	while (ibuf[i] && (ibuf[i] != ':')) {
		if (ibuf[i] == '\\') {
			i++;
			switch(ibuf[i]) {
			case 'E':
				obuf[j] = ESC;
				i++;
				break;
			case 'n':
				obuf[j] = LF;
				i++;
				break;
			case 'r':
				obuf[j] = CR;
				i++;
				break;
			case 'b':
				obuf[j] = BS;
				i++;
				break;
			case 't':
				obuf[j] = TAB;
				i++;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				n = 0;
				k = 0;
				while (k < 3 && ibuf[i] && is_digit(ibuf[i])) {
					n = (8 * n) + (ibuf[i] - '0');
					i++;
					k++;
				}
				obuf[j] = (char) n;
				break;
			default:
				obuf[j] = ibuf[i];
				i++;
			}
		} else if (ibuf[i] == '^') {
			obuf[j] = ibuf[i+1] - 64;
			i += 2;
		} else {
			obuf[j] = ibuf[i++];
		}
		j++;
	}
	obuf[j] = 0;
	if (!(*tcstr = md_malloc(j + 1))) {
		clean_up("cannot alloc() memory");
	}
	strncpy(*tcstr, obuf, j);
}

void
tc_gnum(ibuf, n)
char *ibuf;
int *n;
{
	short i;
	int r = 0;

	i = 4;

	while (is_digit(ibuf[i])) {
		r = (r * 10) + (ibuf[i] - '0');
		i++;
	}
	*n = r;
}

void
tstp()
{
	endwin();
	md_tstp();

	start_window();
	printf("%s%s", TI, VS);
	wrefresh(curscr);
	md_slurp();
}

void
tc_cmget()
{
	short i = 0, j = 0, rc_spec = 0;

	while (CM[i] && (CM[i] != '%') && (j < 15)) {
		cm_esc[j++] = CM[i++];
	}
	cm_esc[j] = 0;

	while (CM[i] && (rc_spec < 2)) {
		if (CM[i] == '%') {
			i++;
			switch(CM[i]) {
			case 'd':
				rc_spec++;
				break;
			case 'i':
				cm_inc = 1;
				break;
			case '2':
				cm_two = 1;
				rc_spec++;
				break;
			case '3':
				cm_three = 1;
				rc_spec++;
				break;
			case '.':
				cm_char = 1;
				rc_spec++;
				break;
			case 'r':
				cm_reverse = 1;
				break;
			case '+':
				i++;
				cm_inc = CM[i];
				cm_char = 1;
				rc_spec++;
				break;
			}
			i++;
		} else {
			j = 0;
			while (CM[i] && (CM[i] != '%')) {
				cm_sep[j++] = CM[i++];
			}
			cm_sep[j] = 0;
		}
	}

	j = 0;
	if (rc_spec == 2) {
		while (CM[i] && (j < 15)) {
			cm_end[j++] = CM[i++];
		}
	}
	cm_end[j] = 0;
}

/* noecho handles nonl and cbreak as well
 * endwin turns this off */
int
noecho()
{
    md_cbreak_no_echo_nonl(TRUE);
    return 0;
}

int
nonl()
{
    return 0;
}

int
cbreak()
{
    return 0;
}

static int arrow_key_seq_len = 0;
static char arrow_key_seq_intro = '\0';

/* setup for handling arrow key sequences */
void
keypad(win,bf)
WINDOW *win;
boolean bf;
{
    /* setup vars for getch (see below)
     * assume all arrow key seqs have same length and intro character */
    arrow_key_seq_len = strlen(KU); 
    if (arrow_key_seq_len > 0) arrow_key_seq_intro = *KU;
    return;
}

/* select is used to detect if typeahead characters exist, in an
 * effort to distinguish between arrow key sequences and accidently
 * hitting the sequence introducer key.
 */

int
typeahead()
{
    struct timeval timeout = {0,500}; /* seconds, milliseconds */
    fd_set readset;

    FD_ZERO(&readset);
    FD_SET(STDIN_FILENO,&readset);
    return select(1,&readset,NULL,NULL,&timeout);
}
    

/* emulate curses getch() function (a bit) to enable support of cursor keys
 * If using putty, check what the cursor keys are sending and that it
 * matches the termcap entry.  ESC[n~ selection fits with vt420.
 *
 * Uses unbuffered read(2) to match use of select(2) to detect typeahead.
 */
#define KSMAX 20

int
getch()
{
    int ch = 0;
    char keyseq[KSMAX+1];
    int idx = 0;
    
    (void) read(0,(void *) &ch,1);
    if (ch == arrow_key_seq_intro) {
        keyseq[idx++] = ch;
        while (typeahead() > 0 &&idx < arrow_key_seq_len && idx < KSMAX) {
            (void) read(0,keyseq+idx++,1);
        }
        keyseq[idx] = '\0';
        if (strncmp(keyseq,KU,arrow_key_seq_len)==0) {
            ch = KEY_UP;
        }
        else if (strncmp(keyseq,KD,arrow_key_seq_len)==0) {
            ch = KEY_DOWN;
        }
        else if (strncmp(keyseq,KL,arrow_key_seq_len)==0) {
            ch = KEY_LEFT;
        }
        else if (strncmp(keyseq,KR,arrow_key_seq_len)==0) {
            ch = KEY_RIGHT;
        }
    }
    return ch;
}
#endif /* RCURSES */
