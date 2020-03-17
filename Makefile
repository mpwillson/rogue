ROGUE_OBJS = curses.o hit.o init.o instruct.o inventory.o level.o machdep.o \
	main.o message.o monster.o move.o object.o pack.o play.o random.o ring.o \
    room.o save.o score.o spec_hit.o throw.o trap.o use.o zap.o

CC = cc

# Setting RCURSES to 1 will compile and link against the internal
# rogue version of curses.  If RCURSES is not set to 1, it is assumed
# libcurses is available.  On Linux this is a package likely called
# libncurses5-dev. On FreeBSD, ncurses-6.1.
RCURSES=0

DEBUG= #-g
CFLAGS = -c ${DEBUG}

ifeq (${RCURSES},1)
CFLAGS += -DRCURSES
else
LIBS = -lncurses
endif

rogue: $(ROGUE_OBJS)
	$(CC) $(ROGUE_OBJS)  ${LIBS} -o rogue

curses.o: curses.c rogue.h
	$(CC) $(CFLAGS) curses.c

hit.o: hit.c rogue.h
	$(CC) $(CFLAGS) hit.c

init.o: init.c rogue.h
	$(CC) $(CFLAGS) init.c

instruct.o: instruct.c rogue.h
	$(CC) $(CFLAGS) instruct.c

inventory.o: inventory.c rogue.h
	$(CC) $(CFLAGS) inventory.c

level.o: level.c rogue.h
	$(CC) $(CFLAGS) level.c

machdep.o: machdep.c rogue.h
	$(CC) $(CFLAGS) machdep.c

main.o: main.c rogue.h
	$(CC) $(CFLAGS) main.c

message.o: message.c rogue.h
	$(CC) $(CFLAGS) message.c

monster.o: monster.c rogue.h
	$(CC) $(CFLAGS) monster.c

move.o: move.c rogue.h
	$(CC) $(CFLAGS) move.c

object.o: object.c rogue.h
	$(CC) $(CFLAGS) object.c

pack.o: pack.c rogue.h
	$(CC) $(CFLAGS) pack.c

play.o: play.c rogue.h
	$(CC) $(CFLAGS) play.c

random.o: random.c
	$(CC) $(CFLAGS) random.c

ring.o: ring.c rogue.h
	$(CC) $(CFLAGS) ring.c

room.o: room.c rogue.h
	$(CC) $(CFLAGS) room.c

save.o: save.c rogue.h
	$(CC) $(CFLAGS) save.c

score.o: score.c rogue.h
	$(CC) $(CFLAGS) score.c

spec_hit.o: spec_hit.c rogue.h
	$(CC) $(CFLAGS) spec_hit.c

throw.o: throw.c rogue.h
	$(CC) $(CFLAGS) throw.c

trap.o: trap.c rogue.h
	$(CC) $(CFLAGS) trap.c

use.o: use.c rogue.h
	$(CC) $(CFLAGS) use.c

zap.o: zap.c rogue.h
	$(CC) $(CFLAGS) zap.c

clean:
	rm -f *.o rogue
