#
# Quake2 gamei386.real.so Makefile for Linux
#
# Jan '98 by Zoid <zoid@idsoftware.com>
#
# ELF only
#
# Edited April 28, 2020 by QwazyWabbit for Vortex-indy
#
# Requires GNU make
#

# this nice line comes from the linux kernel makefile
ARCH := $(shell uname -m | sed -e s/i.86/i386/ \
	-e s/sun4u/sparc64/ -e s/arm.*/arm/ \
	-e s/sa110/arm/ -e s/alpha/axp/)

# On 64-bit OS use the command: setarch i386 make all
# to obtain the 32-bit binary DLL on 64-bit Linux.

CC = gcc -std=c11 -Wparentheses

INCLUDE = -I../src
INCLUDE += -I../src/*

# on x64 machines do this preparation:
# sudo apt-get install ia32-libs
# sudo apt-get install libc6-dev-i386
# On Ubuntu 16.x use sudo apt install libc6-dev-i386
# this will let you build 32-bits on ia64 systems
#
# This is for native build
CFLAGS=-O3 -DARCH="$(ARCH)" -DSTDC_HEADERS
# This is for 32-bit build on 64-bit host
ifeq ($(ARCH),i386)
CFLAGS =-m32 -O3 -DARCH="$(ARCH)" -DSTDC_HEADERS -I/usr/include
endif

# use this when debugging
#CFLAGS=-g -Og -DDEBUG -DARCH="$(ARCH)" -Wall -pedantic

# flavors of Linux
ifeq ($(shell uname),Linux)
#SVNDEV := -D'SVN_REV="$(shell svnversion -n .)"'
#CFLAGS += $(SVNDEV)
CFLAGS += -DLINUX
LIBTOOL = ldd
endif

# OS X wants to be Linux and FreeBSD too.
ifeq ($(shell uname),Darwin)
#SVNDEV := -D'SVN_REV="$(shell svnversion -n .)"'
#CFLAGS += $(SVNDEV)
CFLAGS += -DLINUX
LIBTOOL = otool
endif

CFLAGS += -DREMOVE_RESPAWNS
CFLAGS += -DCMD_USEHASH
CFLAGS += -DALLOW_ADMIN
CFLAGS += -DQ2PRO_COMPATIBILITY
CFLAGS += -DNO_GDS 
#CFLAGS += -DGDS_NOMULTITHREADING
#CFLAGS += -DSQLITE_HOMEGROWN_RECURSIVE_MUTEX


SHLIBEXT=so
#set position independent code
SHLIBCFLAGS=-fPIC

DO_SHLIB_CC=$(CC) $(CFLAGS) $(INCLUDE) $(SHLIBCFLAGS) -o $@ -c $<

.c.o:
	$(DO_SHLIB_CC)

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

GAME_OBJS = \
	g_chase.o g_misc.o v_cmd.o g_target.o weapons.o g_func.o g_trigger.o grid.o \
	maplist.o v_maplist.o lasersight.o g_save.o g_lasers.o g_monster.o g_weapon.o \
	g_main.o g_flame.o gamemode/domination.o gamemode/tbi.o gamemode/pvb.o \
	gamemode/ctf.o gamemode/holywars.o gamemode/invasion.o v_items.o ally.o \
	ai/ai_movement.o ai/ai_main.o ai/ai_class_monster_default.o ai/ai_links.o \
	ai/ai_class_dmbot.o ai/ai_tools.o ai/ai_navigation.o ai/AStar.o ai/ai_weapons.o \
	ai/bot_common.o ai/ai_nodes.o ai/bot_spawn.o ai/ai_items.o ai/ai_dropnodes.o \
	g_combat.o menu/trade.o menu/Talents.o menu/weapon.o menu/menu.o menu/upgrades.o \
	menu/help.o menu/item.o teamplay.o armory.o g_cmds.o g_phys.o vote.o misc/ents.o \
	misc/file_output.o misc/hash_32a.o g_sword.o g_svcmds.o g_spawn.o g_utils.o q_shared.o \
	v_mysql_gds.o backpack.o sql/sqlite3.o misc_stuff.o damage.o v_utils.o abil/flying_skull.o \
	abil/totems.o abil/magicbolt.o abil/blessedhammer.o abil/magmine.o abil/napalm.o abil/scanner.o \
	abil/sentrygun.o abil/morph/playertomutant.o abil/morph/cacodemon.o abil/morph/playertoparasite.o \
	abil/morph/autoaim.o abil/morph/playertotank.o abil/morph/playertoberserk.o \
	abil/morph/playertoflyer.o abil/morph/brain.o abil/morph/playertomedic.o abil/holyground.o \
	abil/ammosteal.o abil/supplystation.o abil/antigrav.o abil/laser/common.o abil/laser/platform.o \
	abil/laser/laser.o abil/fireball.o abil/boost.o abil/spikegrenade.o abil/curses/healing.o \
	abil/curses/deflect.o abil/curses/ampdamage.o abil/curses/blessing.o abil/curses/lifedrain.o \
	abil/curses/common.o abil/curses/lowerresist.o abil/curses/weaken.o abil/curses/amnesia.o \
	abil/curses/curse.o abil/explodingarmor.o abil/conversion.o abil/plague.o abil/mirror.o \
	abil/meditate.o abil/wormhole.o abil/proxy.o abil/spike.o abil/jetpack.o abil/corpseexplode.o \
	abil/caltrops.o abil/hook.o abil/lightningstorm.o abil/meteor.o abil/purge.o abil/organic/healer.o \
	abil/organic/gasser.o abil/organic/spiker.o abil/organic/obstacle.o abil/organic/cocoon.o \
	abil/organic/common.o abil/chainlightning.o abil/corpseeater.o abil/Spirit.o abil/teleport.o \
	abil/detector.o abil/minisentry.o abil/bombspell.o abil/forcewall.o abil/auras.o abil/mindabsorb.o \
	abil/cloak.o abil/cripple.o abil/plasmabolt.o abil/autocannon.o abil/mjump.o abil/nova.o \
	abil/selfdestruct.o monster/boss_tank.o monster/drone_ai.o monster/drone_gunner.o \
	monster/drone_soldier.o monster/drone_supertank.o monster/drone_jorg.o monster/drone_medic.o \
	monster/drone_makron.o monster/m_flash.o monster/boss_general.o monster/drone_parasite.o \
	monster/drone_infantry.o monster/drone_tank.o monster/drone_berserk.o monster/drone_gladiator.o \
	monster/drone_mutant.o monster/drone_move.o monster/drone_bitch.o monster/drone_brain.o \
	monster/boss_makron.o monster/drone_decoy.o monster/drone_misc.o g_items.o \
	player/player_points.o player/weapon.o player/Player.o player/menu.o \
	player/view.o player/trail.o player/parasite.o player/client.o player/hud.o \
	v_file_IO.o



game$(ARCH).real.$(SHLIBEXT) : $(GAME_OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -shared -o $@ $(GAME_OBJS) -ldl -lm
	$(LIBTOOL) -r $@


#############################################################################
# MISC
#############################################################################

clean:
	/bin/rm -f $(GAME_OBJS)

depend:
	$(CC) -MM $(INCLUDE) $(GAME_OBJS:.o=.c)

depends:
	$(CC) $(CFLAGS) $(INCLUDE) -MM *.c > dependencies

all:
	make clean
	make depends
	make

-include dependencies

