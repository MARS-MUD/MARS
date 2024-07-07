CC      = gcc
DEBUG	= -ggdb
NOCRYPT =
OPT	= -O
PROF    =
WARN	= -Wall -Wno-uninitialized -Wno-missing-prototypes -Wstrict-prototypes \
	  -Wmissing-declarations -Wredundant-decls -Wno-unused-but-set-variable \
	  -Wno-unused-value -Wno-char-subscripts -Wno-unused-variable -Wno-pointer-to-int-cast \
	  -Wno-int-to-pointer-cast -fcommon
C_FLAGS = $(WARN) $(OPT) $(PROF) $(DEBUG) $(NOCRYPT) -D_GNU_SOURCE
L_FLAGS = $(DEBUG) $(PROF)
LIBS	= -lcrypt -lgd -lmysqlclient

BINARY	= mars
C_FILES = act_clan.c act_comm.c act_info.c act_misc.c act_move.c \
	  act_obj.c act_skills.c act_wiz.c act_wiz2.c act_wiz3.c \
          alias.c ban.c boards.c comm.c const.c db.c db2.c debug.c \
	  effects.c event.c fight.c fight2.c flags.c \
	  globals.c gsn.c \
	  handler.c healer.c interp.c lookup.c \
	  magic.c magic2.c \
	  mob_cmds.c multi.c nanny.c \
	  olc.c olc_area.c olc_board.c olc_clan.c olc_cmd.c olc_dream.c \
	  olc_forge.c olc_grp.c olc_help.c olc_mob.c olc_obj.c olc_prog.c \
	  olc_proj.c olc_race.c olc_room.c olc_save.c olc_skill.c \
	  olc_social.c \
	  overland.c progs.c quest.c rand_str.c \
	  recycle.c save.c scan.c skills.c skilltable.c special.c sql.c \
	  string.c tables.c track.c update.c igen.c igen_table.c whoweb.c

O_FILES := $(patsubst %.c,o/%.o,$(C_FILES))

all:
	make -s $(BINARY)

$(BINARY): $(O_FILES)
	rm -f $(BINARY)
	@echo "  Linking $@..."
	$(CC) $(L_FLAGS) -o $(BINARY) $(O_FILES) $(LIBS)
	@echo Build complete.

o/%.o: %.c
	echo "  Compiling $<..."
	$(CC) -c -fPIE $(C_FLAGS) $< -o $@

clean:
	rm -f $(O_FILES) $(BINARY)
