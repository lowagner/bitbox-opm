# DO NOT FORGET to define BITBOX environment variable ++ = RGB(255, 255, 0);

USE_SDCARD = 1      # allow use of SD card for io
#DEFINES += DEBUG_CHIPTUNE

NAME = onepunch
# font file needs to be first in order to be generated first:
AUTOGEN_SRC_FILES = font mugs levels characters # these are autogenerated source files
SRC_FILES = overlay player physics animation draw # these are not!
DEFINES += VGA_MODE=320
MUGS = opm_what opm_suave
CHARACTERS = opm henchman
LEVELS = rentbox

# hopefully do not need to modify beyond here
SRC_FILES += $(AUTOGEN_SRC_FILES)
GAME_C_FILES=$(SRC_FILES:%=src/%.c) src/main.c
GAME_H_FILES=$(SRC_FILES:%=src/%.h) src/common.h config.h

MUGS_PNG=$(MUGS:%=mugs/%.png)
MUGS_C_WITHOUT_SRC=$(MUGS:%=mugs/%.c)
MUGS_C=$(MUGS:%=src/mugs/%.c)

FRAMES_PNG=$(CHARACTERS:%=frames/%.png)
FRAMES_C_WITHOUT_SRC=$(CHARACTERS:%=frames/%.c)
FRAMES_C=$(CHARACTERS:%=src/frames/%.c)

LEVELS_C_WITHOUT_SRC=$(LEVELS:%=%.c)
LEVELS_C=$(LEVELS:%=src/%.c)

CHARACTERS_C_WITHOUT_SRC=$(CHARACTERS:%=%.c)
CHARACTERS_C=$(CHARACTERS:%=src/%.c)

# see this file for options
include $(BITBOX)/kernel/bitbox.mk

src/main.c: src/font.c src/levels.h src/characters.h src/switch_player.c src/switch_animation.c

src/player.c: $(FRAMES_PNG) src/mk_frames.py

src/font.c: src/mk_font.py
	python2 src/mk_font.py

src/mugs.c: Makefile $(MUGS_C) src/mugs.h
	@echo "// $@ autogenerated by Makefile" > $@
	@echo "#include <stdint.h>" >> $@
	@for m in $(MUGS_C_WITHOUT_SRC); do echo "#include \"$$m\"" >> $@; done

src/mugs.h: Makefile $(MUGS_PNG)
	@echo "// $@ autogenerated by Makefile" > $@
	@echo "#pragma once" >> $@
	@echo "#include <stdint.h>" >> $@
	@for m in $(MUGS); do echo "extern const uint16_t mug_$$m[27][27];" >> $@; done

src/mugs/%.c: mugs/%.png src/mk_mug.py
	@mkdir -p src/mugs
	@python2 src/mk_mug.py $< > $@

src/frames/%.c: frames/%.png src/mk_frames.py
	@mkdir -p src/frames
	@python2 src/mk_frames.py $< > $@

src/levels.c: Makefile $(LEVELS_C) src/levels.h
	@echo "// $@ autogenerated by Makefile" > $@
	@echo "#include \"common.h\"" >> $@
	@echo "#include \"characters.h\"" >> $@
	@echo "#include \"overlay.h\"" >> $@
	@echo "#include \"mugs.h\"" >> $@
	@echo "#include <math.h>" >> $@
	@for l in $(LEVELS_C_WITHOUT_SRC); do echo "#include \"$$l\"" >> $@; done

src/levels.h: Makefile $(LEVELS_C)
	@echo "// $@ autogenerated by Makefile" > $@
	@echo "#pragma once" >> $@
	@for l in $(LEVELS); do \
	 echo "void $${l}_init();" >> $@; \
	 echo "void $${l}_background();" >> $@; \
	done
	@echo "typedef enum {" >> $@
	@echo "  level_none=0," >> $@
	@for l in $(LEVELS); do \
	 echo "  level_$$l," >> $@; \
	done
	@echo "} Level;" >> $@

src/characters.c: Makefile $(CHARACTERS_C) src/characters.h $(FRAMES_C)
	@echo "// $@ autogenerated by Makefile" > $@
	@echo "#include \"physics.h\"" >> $@
	@echo "#include \"animation.h\"" >> $@
	@echo "#include \"characters.h\"" >> $@
	@echo "#include \"draw.h\"" >> $@
	@echo "#include <math.h>" >> $@
	@echo "#include <stdlib.h>" >> $@
	@for c in $(CHARACTERS_C_WITHOUT_SRC); do echo "#include \"$$c\"" >> $@; done
	@for c in $(FRAMES_C_WITHOUT_SRC); do echo "#include \"$$c\"" >> $@; done
	@echo "void character_set(int p, int character)" >> $@
	@echo "{" >> $@
	@echo "	switch (character)" >> $@
	@echo "	{" >> $@
	@for c in $(CHARACTERS); do \
	 echo "		case CHARACTER_$$c:" >> $@; \
	 echo "			players[p].all_frames = frames_$${c};" >> $@; \
	 echo "			for (int i=1; i<16; ++i) {" >> $@; \ 
	 echo "				quads[32*p+i].color = colors_$${c}[i];" >> $@; \
	 echo "				quads[32*p+i].edge_color = (quads[32*p+i].color&31710)>>1;" >> $@; \
	 echo "			}" >> $@; \
	 echo "			break;" >> $@; \
	done
	@echo "		default:" >> $@;
	@echo "			return message(\"invalid character for player \%d: \%d\", p, character);" >> $@;
	@echo "	}" >> $@
	@echo "	players[p].character = character;" >> $@
	@echo "}" >> $@

src/characters.h: Makefile $(CHARACTERS_C)
	@echo "// $@ autogenerated by Makefile" > $@
	@echo "#pragma once" >> $@
	@echo "#include \"player.h\"" >> $@
	@echo "#include \"animation.h\"" >> $@
	@echo "#include \"common.h\"" >> $@
	@for c in $(CHARACTERS); do \
	 echo "void $${c}_start_level(int p);" >> $@; \
	 echo "void $${c}_ground(int p, float dt);" >> $@; \
	 echo "void $${c}_air(int p, float dt);" >> $@; \
	 echo "void $${c}_projectile(int p, float dt);" >> $@; \
	 echo "void $${c}_AI(int p, float dt);" >> $@; \
	 echo "extern const animation_frame frames_$${c}[ANIM_COUNT];" >> $@; \
	 echo "extern const uint16_t colors_$${c}[16];" >> $@; \
	done
	@i=0; \
	for c in $(CHARACTERS); do \
	 echo "#define CHARACTER_$$c $$i" >> $@; \
	 i=$$((i+1)); \
	done
	@echo "void character_set(int p, int character);" >> $@

src/switch_player.c: Makefile
	@echo "// $@ autogenerated by Makefile" > $@
	@echo "static inline void switch_player_ground(int p, float dt)" >> $@
	@echo "{" >> $@
	@echo "	switch (players[p].character)" >> $@
	@echo "	{" >> $@
	@for c in $(CHARACTERS); do \
	 echo "		case CHARACTER_$$c:" >> $@; \
	 echo "			$${c}_ground(p, dt);" >> $@; \
	 echo "			$${c}_projectile(p, dt);" >> $@; \
	 echo "			break;" >> $@; \
	done
	@echo "	}" >> $@
	@echo "}" >> $@
	@echo "static inline void switch_player_air(int p, float dt)" >> $@
	@echo "{" >> $@
	@echo "	switch (players[p].character)" >> $@
	@echo "	{" >> $@
	@for c in $(CHARACTERS); do \
	 echo "		case CHARACTER_$$c:" >> $@; \
	 echo "			$${c}_air(p, dt);" >> $@; \
	 echo "			$${c}_projectile(p, dt);" >> $@; \
	 echo "			break;" >> $@; \
	done
	@echo "	}" >> $@
	@echo "}" >> $@
	@echo "static inline void switch_player_start_level(int p)" >> $@
	@echo "{" >> $@
	@echo "	switch (players[p].character)" >> $@
	@echo "	{" >> $@
	@for c in $(CHARACTERS); do \
	 echo "		case CHARACTER_$$c:" >> $@; \
	 echo "			$${c}_start_level(p);" >> $@; \
	 echo "			break;" >> $@; \
	done
	@echo "	}" >> $@
	@echo "}" >> $@
	@echo "static inline void switch_AI_frame(int p, float dt)" >> $@
	@echo "{" >> $@
	@echo "	switch (players[p].character)" >> $@
	@echo "	{" >> $@
	@for c in $(CHARACTERS); do \
	 echo "		case CHARACTER_$$c:" >> $@; \
	 echo "			$${c}_AI(p, dt);" >> $@; \
	 echo "			break;" >> $@; \
	done
	@echo "	}" >> $@
	@echo "}" >> $@

src/switch_animation.c: src/mk_animation.py
	python2 src/mk_animation.py

clean::
	for x in $(AUTOGEN_SRC_FILES); do rm -f src/$$x.c src/$$x.h; done;
	rm -f src/switch_player.c src/switch_animation.*
	rm -rf src/mugs src/frames

destroy:
	rm -f RECENT16.TXT *.*16

very-clean: clean destroy

backup: bak

.ONESHELL:
bak:
	dir=drafts/$$(date +%y%m%d)
	mkdir -p drafts
	mkdir -p $$dir
	mkdir -p $$dir/src
	mkdir -p $$dir/mugs
	cp *.h Makefile $$dir
	cp src/*.c src/*.h $$dir/src
	cp mugs/* $$dir/mugs


g: $(NAME)_sdl
	gdb $(NAME)_sdl
