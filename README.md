# Rogue 9.8.6 - version 9.8.5 ported to modern compiler/OS

Date: 2020-03-04

Despite the version number referred to below, this source  was originally
packaged in a tar file with the version number 985.  Hence, this is
now rogue-9.8.6.

## Changes to 9.8.5 version

Ported to Debian Linux under WSL (GCC 8.3) and FreeBSD 12.x (clang 8.0.1)

* machdep.c changes required

  Added includes for <time.h> and <stdlib.h>

  md_gct: Remove declaration of 'struct timezone tzp';
          Second argument of call to gettimeofday() set to NULL

  md_gln: Change call to getlogin() to getpwuid(geteuid()) as
          getlogin() under WSL returns NULL

  md_malloc: Remove declaration of malloc()
  
* Debian ncurses library header, <curses.h>, defines the macro CURSES,
  which conflicts with define for internal rogue curses
  implementation, therefore define for internal version of curses
  renamed to RCURSES

* All #endif labels changed to comments

* Added declarations of many routines to "rogue.h" to eliminate
  compiler warnings

* Added return types to functions to eliminate compiler warnings

* Added <stdio.h> and/or <string.h> to source files to eliminate
  compiler warnings

* Added support for cursor arrow keys as directions, using libcurses

* Added support for cursor arrow keys as directions for in-built rogue
  curses code. Tested on FreeBSD 11.2

* Fixed bugs related to returning to original tty state on error exits

* Added macros to Makefile to handle use of in-built curses
  code. Consequence is that build requires GNU make.

Mark Willson

# ORIGINAL README FOLLOWS

This is an updated version of rogue 5.3-clone. It's a LINUX port which
has all of the posted patches added to it, and considerable numbers of
bug fixes. I've also added a help page and made it operate correctly
setuid.

Alan Cox [iiitac@pyr.swan.ac.uk]

## Original readme entries...
----
I've ported this to LINUX..It seems to work fine.  I mostly
POSIXized machdep.c and got rid of some floating point 
arithmetic.  (Not a terribly difficult effort...)  This 
is a first effort at dealing with the POSIX terminal I/O
so it may not be perfect.  

I didn't add the patches to the original source code
so I don't have the bug fixes added.  I used the original
machdep.c instead of the new "SYSV" version so don't know
if my effort was totally in vain. Heck I got to learn something
about the new terminal I/O model so it wasn't all in vain ;-)

Steve Wilson stevew@netcom.com

----
Yes, folks, this is the same old rogue 5.3-clone source that I've posted
here before.  So why do it again?  Well, I got many complaints/inquisitions
about whether this code was ported to a number of different UNIX/ULTRIX/VMS
systems.  The answer is still "no."  BUTTTT..., I would like to first port
it to as may of the different UNIX systems as I can, and go from there.

My problem is that I'm not particularly familiar with the ins and outs of
other UNIX systems besides the one I work on. (BSD 4.[23])  So what I've
done is set up the rogue source code so that all of the machine dependent
stuff is localized in one source file, machdep.c.  Enclosed in machdep.c
are all of the routines that I thought were even slightly machine-dependent.
Also enclosed in machdep.c is a copious set of comments describing what's
going on in there.  Now, most of these routines I believe will run most
UNIX systems without modification, but then again, some won't.
That's where I need some help.  I need folks who have access to other
UNIX systems to make modifications to machdep.c that will effectively
port the entire rogue program to their particular UNIX system.  Enough
instructions are included in machdep.c to do this.  The modified machdep.c
can be e-mailed to me, whereby I can incorporate the changes, and post
versions of machdep.c to comp.sources.games as it becomes more complete.

Does this sound remotely interesting to you?  Please give me a hand.

If you can't do it yourself, maybe you know someone who can.  A little
thumbing through your system documentation is all you probably need.

I am hoping all the system-dependencies are in machdep.c, but I suspect
there may be some 'C' constructs that various compilers out there don't
support or treat differently.  I need to know about these too.  I'm not
expecting much of this on UNIX systems, though.

The code runs through 'lint' fine on my system.  All identifiers are unique
within their first eight characters.  Function names are unique within
their first seven characters.  All file names are under 14 characters.


Prerequisites of the operating system:

	32-bit long integer. (at least)
	16-bit short integer. (at least)

	Standard I/O library providing the following:
		stdio.h (include file),
		getchar(),
		fopen(),
		fclose(),
		fread(),
		fwrite(),
		fgets(),
		printf(),
		sprintf(),
		fflush(),
		rewind().

	String library providing the following:
		strcpy(),
		strncpy(),
		strcmp(),
		strncmp().

	The curses/termlib libraries are desireable, but not strictly necessary.
	The rogue source can be compiled with -DCURSES.  This will enable a
	self-contained curses/termlib emulation package.  This package has some
	restrictions which are outined in the comments near the top of the
	enclosed source file curses.c.  This curses emulation package was put
	into the source for systems that may not have curses available, or have
	very buggy versions.  If your system has usable working curses/termlib
	libraries, it is suggested that you use them instead of the enclosed
	curses library.


Tim Stoehr
tims@zeus.TEK.COM
