#include "g_local.h"

#define VOTE_MAP	1
#define	VOTE_MODE	2

qboolean loc_CanSee (edict_t *targ, edict_t *inflictor);
void check_for_levelup(edict_t *ent);

//Function prototypes required for this .c file:
void GDS_LoadPlayer(edict_t *ent);
void OpenDOMJoinMenu (edict_t *ent);
int GetAbilityUpgradeCost(int index); // from upgrades.c
qboolean SavePlayer(edict_t* ent);	// from v_file_IO.c

void disableAbilities (edict_t *ent)
{
	int i;

	for (i=0; i<MAX_ABILITIES; ++i)
	{
		ent->myskills.abilities[i].disable = true;
		ent->myskills.abilities[i].hidden = false;
	}
}

void setHardMax(edict_t *ent, int index)
{
	switch(index)
	{
		//Skills that max at level 1
		case ID:
		case WORLD_RESIST:
		case BULLET_RESIST:
		case SHELL_RESIST:
		case ENERGY_RESIST:
		case PIERCING_RESIST:
		case SPLASH_RESIST:
		//case FREEZE_SPELL:
		case CLOAK:
		case CREATE_QUAD:
		case CREATE_INVIN:
		case BOOST_SPELL:
		case SUPER_SPEED:
		case ANTIGRAV:
		case WEAPON_KNOCK:
		case TELEPORT:
		case JETPACK:
		case SHIELD:
			ent->myskills.abilities[index].hard_max = 1; break;

			// Special cases for the non-general ability mode.

		case REGENERATION:
				if (ent->myskills.abilities[index].general_skill)
					ent->myskills.abilities[index].hard_max = 15;
				break;
		case STRENGTH:
		case RESISTANCE:
			if (ent->myskills.abilities[index].general_skill && 
				!(ent->myskills.class_num == CLASS_SOLDIER))
			{					
				ent->myskills.abilities[index].hard_max = 15;
				
			}
			else
			{
				ent->myskills.abilities[index].hard_max = 30;					
			}	
			break;			

		//Everything else
		default:
			if (GetAbilityUpgradeCost(index) < 2)
			{
				if (!generalabmode->value)
				{
					if (ent->myskills.class_num == CLASS_WEAPONMASTER)
					{
						ent->myskills.abilities[index].hard_max = ent->myskills.abilities[index].max_level * 2; break;
					}
					else
					{
						ent->myskills.abilities[index].hard_max = ent->myskills.abilities[index].max_level * 4; break;
					}
					
				}else
					ent->myskills.abilities[index].hard_max = ent->myskills.abilities[index].max_level * 1.5; break;
			}
			else 
				ent->myskills.abilities[index].hard_max = 1; break;
	}	
}

void enableAbility (edict_t *ent, int index, int level, int max_level, int general)
{
	ent->myskills.abilities[index].disable = false;

	// we can pass this function -1 if we don't want to alter these variables
	if (max_level != -1)
		ent->myskills.abilities[index].max_level = max_level;

	if (level != -1)
	{
		ent->myskills.abilities[index].level = level;
		ent->myskills.abilities[index].current_level = level;
	}

	ent->myskills.abilities[index].general_skill = general;
	setHardMax(ent, index);
}

// NOTE: abilities must match with getClassRuneStat() in v_utils.c
void setClassAbilities (edict_t *ent)
{
	switch (ent->myskills.class_num)
	{
	case CLASS_SOLDIER:// 78 points
		enableAbility(ent, STRENGTH, 0, 20, 0);
		enableAbility(ent, RESISTANCE, 0, 15, 0);
		enableAbility(ent, HA_PICKUP, 0, 15, 0);
		enableAbility(ent, NAPALM, 0, 15, 0);
		enableAbility(ent, SPIKE_GRENADE, 0, 15, 0);
		enableAbility(ent, EMP, 0, 15, 0);
		enableAbility(ent, MIRV, 0, 15, 0);
		enableAbility(ent, CREATE_QUAD, 0, 1, 0);
		enableAbility(ent, CREATE_INVIN, 0, 1, 0);

		enableAbility(ent, GRAPPLE_HOOK, 3, 3, 0);
		break;
	case CLASS_KNIGHT:// 70 points
		enableAbility(ent, ARMOR_UPGRADE, 0, 10, 0);
		enableAbility(ent, REGENERATION, 0, 15, 0);
		enableAbility(ent, POWER_SHIELD, 0, 15, 0);
		enableAbility(ent, ARMOR_REGEN, 0, 15, 0);
		enableAbility(ent, EXPLODING_ARMOR, 0, 15, 0);
		enableAbility(ent, BEAM, 0, 20, 0);
		enableAbility(ent, PLASMA_BOLT, 0, 15, 0);
		enableAbility(ent, SHIELD, 1, 1, 0);

		enableAbility(ent, BOOST_SPELL, 1, 1, 0);
		ent->myskills.respawn_weapon = 1;	//sword only
		break;
	case CLASS_VAMPIRE:// 80 points
		enableAbility(ent, VAMPIRE, 0, 15, 0);
		enableAbility(ent, GHOST, 0, 15, 0);
		enableAbility(ent, LIFE_DRAIN, 0, 15, 0);
		enableAbility(ent, FLESH_EATER, 0, 15, 0);
		enableAbility(ent, CORPSE_EXPLODE, 0, 15, 0);
		enableAbility(ent, MIND_ABSORB, 0, 15, 0);
		enableAbility(ent, AMMO_STEAL, 0, 15, 0);
		enableAbility(ent, CLOAK, 1, 10, 0);

		break;
	case CLASS_NECROMANCER:// 80 points
		enableAbility(ent, MONSTER_SUMMON, 0, 20, 0);
		enableAbility(ent, HELLSPAWN, 0, 20, 0);
		enableAbility(ent, PLAGUE, 0, 15, 0);
		enableAbility(ent, AMP_DAMAGE, 0, 15, 0);
		enableAbility(ent, CRIPPLE, 0, 15, 0);
		enableAbility(ent, CURSE, 0, 15, 0);
		enableAbility(ent, WEAKEN, 0, 15, 0);

		enableAbility(ent, CONVERSION, 0, 15, 0);
		enableAbility(ent, JETPACK, 1, 1, 0);
		break;
	case CLASS_ENGINEER:// 80 points
		enableAbility(ent, PROXY, 0, 15, 0);
		enableAbility(ent, BUILD_SENTRY, 0, 20, 0);
		enableAbility(ent, SUPPLY_STATION, 0, 15, 0);
		enableAbility(ent, BUILD_LASER, 0, 15, 0);
		enableAbility(ent, MAGMINE, 0, 20, 0);
		enableAbility(ent, CALTROPS, 0, 15, 0);
		enableAbility(ent, AUTOCANNON, 0, 15, 0);
		enableAbility(ent, DETECTOR, 0, 15, 0);
		enableAbility(ent, DECOY, 0, 15, 0);

		enableAbility(ent, ANTIGRAV, 1, 1, 0);
		//Hard Max Eng
		ent->myskills.abilities[BUILD_SENTRY].hard_max = 50;
		break;
	case CLASS_MAGE:// 80 points
		enableAbility(ent, MAGICBOLT, 0, 15, 0);
		enableAbility(ent, NOVA, 0, 15, 0);
		enableAbility(ent, BOMB_SPELL, 0, 15, 0);
		enableAbility(ent, FORCE_WALL, 0, 15, 0);
		enableAbility(ent, LIGHTNING, 0, 15, 0);
		enableAbility(ent, METEOR, 0, 15, 0);
		enableAbility(ent, FIREBALL, 0, 15, 0);
		enableAbility(ent, LIGHTNING_STORM, 0, 15, 0);

		enableAbility(ent, TELEPORT, 1, 1, 0);
		break;
	case CLASS_CLERIC:// 80 points
		enableAbility(ent, SALVATION, 0, 15, 0);
		enableAbility(ent, HOLY_FREEZE, 0, 15, 0);
		enableAbility(ent, HEALING, 0, 15, 0);
		enableAbility(ent, BLESS, 0, 15, 0);
		enableAbility(ent, YIN, 0, 15, 0);
		enableAbility(ent, YANG, 0, 15, 0);
		enableAbility(ent, HAMMER, 0, 15, 0);
		enableAbility(ent, DEFLECT, 0, 15, 0);
		enableAbility(ent, LOWER_RESIST, 0, 15, 0);

		enableAbility(ent, DOUBLE_JUMP, 1, 1, 0);
		break;
	case CLASS_POLTERGEIST:// 80 points
		enableAbility(ent, BERSERK, 1, 20, 0);
		enableAbility(ent, CACODEMON, 1, 20, 0);
		enableAbility(ent, BLOOD_SUCKER, 1, 20, 0);
		enableAbility(ent, BRAIN, 1, 20, 0);
		enableAbility(ent, FLYER, 1, 20, 0);
		enableAbility(ent, MUTANT, 1, 20, 0);
		enableAbility(ent, TANK, 1, 20, 0);
		enableAbility(ent, MEDIC, 1, 20, 0);
		enableAbility(ent, GHOST, 99, 99, 0);
		enableAbility(ent, MORPH_MASTERY, 1, 1, 0);

		// Specific Skills for Medics.
		/*enableAbility(ent, SPIKER, 0, 1, 0);		
		enableAbility(ent, GASSER, 0, 1, 0);*/
		enableAbility(ent, MONSTER_SUMMON, 0, 1, 0);

		// Hard Max for Specific Skills
		/*ent->myskills.abilities[SPIKER].hard_max = 1;
		ent->myskills.abilities[GASSER].hard_max = 1;*/
		ent->myskills.abilities[MONSTER_SUMMON].hard_max = 1;
		break;

	case CLASS_SHAMAN:// 75 points
		enableAbility(ent, FIRE_TOTEM, 0, 20, 0);
		enableAbility(ent, WATER_TOTEM, 0, 20, 0);
		enableAbility(ent, AIR_TOTEM, 0, 20, 0);
		enableAbility(ent, EARTH_TOTEM, 0, 20, 0);
		enableAbility(ent, DARK_TOTEM, 0, 20, 0);
		enableAbility(ent, NATURE_TOTEM, 0, 20, 0);
		enableAbility(ent, FURY, 0, 15, 0);
		enableAbility(ent, HASTE, 0, 5, 0); // lol oops
		enableAbility(ent, SUPER_SPEED, 1, 1, 0);
		enableAbility(ent, TOTEM_MASTERY, 1, 1, 0);//4.4
		break;
	case CLASS_ALIEN:// 80 points
		enableAbility(ent, SPIKER, 0, 15, 0);
		enableAbility(ent, OBSTACLE, 0, 15, 0);
		enableAbility(ent, GASSER, 0, 15, 0);
		enableAbility(ent, HEALER, 0, 15, 0);
		enableAbility(ent, SPORE, 0, 15, 0);
		enableAbility(ent, ACID, 0, 15, 0);
		enableAbility(ent, SPIKE, 0, 15, 0);
		enableAbility(ent, COCOON, 0, 15, 0);

		enableAbility(ent, BLACKHOLE, 1, 1, 0);
		break;
	case CLASS_KAMIKAZE:
		enableAbility(ent, SELFDESTRUCT, 0, 15, 0);
		enableAbility(ent, HA_PICKUP, 1, 15, 0);
		enableAbility(ent, PROXY, 0, 15, 0);
		enableAbility(ent, CREATE_INVIN, 0, 1, 0);
		enableAbility(ent, AMNESIA, 0, 15, 0);
		enableAbility(ent, MAGMINE, 0, 15, 0);
		enableAbility(ent, FLASH, 1, 1, 0);

		enableAbility(ent, BOOST_SPELL, 1, 1, 0);

		break;
	case CLASS_WEAPONMASTER:// 0 points
		break;
	}
}

void setGeneralAbilities (edict_t *ent)
{
	// general
	enableAbility(ent, VITALITY, 0, 10, 1);
	//enableAbility(ent, ID, 0, 1, 1); // vrxchile 2.0- id is no longer a skill.
	enableAbility(ent, MAX_AMMO, 0, 10, 1);
	enableAbility(ent, POWER_REGEN, 0, 8, 1);
	enableAbility(ent, WORLD_RESIST, 0, 1, 1);

	// ammo regen is hard capped at 5
	enableAbility(ent, AMMO_REGEN, 0, 10, 1);
	ent->myskills.abilities[AMMO_REGEN].hard_max = 5;

	// shared
	enableAbility(ent, REGENERATION, 0, 5, 1);
	enableAbility(ent, STRENGTH, 0, 5, 1);
	enableAbility(ent, HASTE, 0, 5, 1); // Haste is hard capped as well; It's pretty powerful right now.
	ent->myskills.abilities[HASTE].hard_max = 5; // besides there are no valid upgrades over 5.

	// resists
	enableAbility(ent, RESISTANCE, 0, 5, 1);
	enableAbility(ent, SHELL_RESIST, 0, 1, 1);
	enableAbility(ent, BULLET_RESIST, 0, 1, 1);
	enableAbility(ent, SPLASH_RESIST, 0, 1, 1);
	enableAbility(ent, PIERCING_RESIST, 0, 1, 1);
	enableAbility(ent, ENERGY_RESIST, 0, 1, 1);

	enableAbility(ent, SCANNER, 0, 1, 1);

	enableAbility(ent, GRAPPLE_HOOK, 0, 3, 1);
	enableAbility(ent, JETPACK, 0, 1, 1);
	enableAbility(ent, SUPER_SPEED, 0, 1, 1);
	enableAbility(ent, DOUBLE_JUMP, 0, 1, 1);
	enableAbility(ent, ANTIGRAV, 0, 1, 1);
	enableAbility(ent, BLACKHOLE, 0, 1, 1);
	enableAbility(ent, BOOST_SPELL, 0, 1, 1);
	enableAbility(ent, TELEPORT, 0, 1, 1);

	if (ent->myskills.class_num == CLASS_WEAPONMASTER || generalabmode->value) // vrxchile 2.0: WMs are the new APs.
	{
		enableAbility(ent, VAMPIRE, 0, 5, 1);
		enableAbility(ent, CLOAK, 0, 1, 1);
		enableAbility(ent, WEAPON_KNOCK, 0, 1, 1);
		enableAbility(ent, ARMOR_UPGRADE, 0, 5, 1);
		enableAbility(ent, AMMO_STEAL, 0, 5, 1);
		enableAbility(ent, SUPPLY_STATION, 0, 5, 1);
		//enableAbility(ent, FREEZE_SPELL, 0, 1, 1);
		enableAbility(ent, CREATE_QUAD, 0, 1, 1);
		enableAbility(ent, CREATE_INVIN, 0, 1, 1);
		enableAbility(ent, POWER_SHIELD, 0, 5, 1);
		enableAbility(ent, CORPSE_EXPLODE, 0, 5, 1);
		enableAbility(ent, GHOST, 0, 5, 1);
		enableAbility(ent, SALVATION, 0, 5, 1);
		enableAbility(ent, FORCE_WALL, 0, 5, 1);
		//	enableAbility(ent, AMMO_REGEN, 0, 5, 1);
		enableAbility(ent, BUILD_LASER, 0, 5, 1);
		enableAbility(ent, HA_PICKUP, 0, 5, 1);
		enableAbility(ent, BUILD_SENTRY, 0, 5, 1);
		enableAbility(ent, BLOOD_SUCKER, 0, 5, 1);
		enableAbility(ent, PROXY, 0, 5, 1);
		enableAbility(ent, MONSTER_SUMMON, 0, 5, 1);
		enableAbility(ent, ARMOR_REGEN, 0, 5, 1);
		enableAbility(ent, BOMB_SPELL, 0, 5, 1);
		enableAbility(ent, LIGHTNING, 0, 5, 1);
		enableAbility(ent, HOLY_FREEZE, 0, 5, 1);
		enableAbility(ent, CACODEMON, 0, 5, 1);
		enableAbility(ent, PLAGUE, 0, 5, 1);
		enableAbility(ent, FLESH_EATER, 0, 5, 1);
		enableAbility(ent, HELLSPAWN, 0, 5, 1);
		enableAbility(ent, BEAM, 0, 5, 1);
		enableAbility(ent, BRAIN, 0, 5, 1);
		enableAbility(ent, MAGMINE, 0, 5, 1);
		enableAbility(ent, CRIPPLE, 0, 5, 1);
		enableAbility(ent, MAGICBOLT, 0, 5, 1);
		enableAbility(ent, NOVA, 0, 5, 1);
		enableAbility(ent, EXPLODING_ARMOR, 0, 5, 1);
		enableAbility(ent, MIND_ABSORB, 0, 5, 1);
		enableAbility(ent, LIFE_DRAIN, 0, 5, 1);
		enableAbility(ent, AMP_DAMAGE, 0, 5, 1);
		enableAbility(ent, CURSE, 0, 5, 1);
		enableAbility(ent, BLESS, 0, 5, 1);
		enableAbility(ent, WEAKEN, 0, 5, 1);
		enableAbility(ent, HEALING, 0, 5, 1);
		//enableAbility(ent, AMMO_UPGRADE, 0, 5, 1);
		enableAbility(ent, YIN, 0, 5, 1);
		enableAbility(ent, YANG, 0, 5, 1);
		enableAbility(ent, FLYER, 0, 5, 1);
		enableAbility(ent, MUTANT, 0, 5, 1);
		enableAbility(ent, TANK, 0, 5, 1);
		enableAbility(ent, BERSERK, 0, 5, 1);
		enableAbility(ent, SPIKE, 0, 5, 1);
		enableAbility(ent, MORPH_MASTERY, 0, 1, 1);
		enableAbility(ent, NAPALM, 0, 5, 1);
		enableAbility(ent, MEDIC, 0, 5, 1);
		enableAbility(ent, METEOR, 0, 5, 1);
		enableAbility(ent, AUTOCANNON, 0, 5, 1);
		enableAbility(ent, HAMMER, 0, 5, 1);
		enableAbility(ent, SUPER_SPEED, 0, 5, 1);
		enableAbility(ent, FIRE_TOTEM, 0, 5, 1);
		enableAbility(ent, WATER_TOTEM, 0, 5, 1);
		enableAbility(ent, AIR_TOTEM, 0, 5, 1);
		enableAbility(ent, EARTH_TOTEM, 0, 5, 1);
		enableAbility(ent, DARK_TOTEM, 0, 5, 1);
		enableAbility(ent, NATURE_TOTEM, 0, 5, 1);
		enableAbility(ent, FURY, 0, 5, 1);
		enableAbility(ent, TOTEM_MASTERY, 0, 1, 1);
		enableAbility(ent, SHIELD, 0, 1, 1);
		enableAbility(ent, CALTROPS, 0, 5, 1);
		enableAbility(ent, SPIKE_GRENADE, 0, 5, 1);
		enableAbility(ent, DETECTOR, 0, 5, 1);
		enableAbility(ent, CONVERSION, 0, 5, 1);
		enableAbility(ent, DEFLECT, 0, 5, 1);
		enableAbility(ent, EMP, 0, 5, 1);
		enableAbility(ent, LOWER_RESIST, 0, 5, 1);
		enableAbility(ent, FIREBALL, 0, 5, 1);
		enableAbility(ent, PLASMA_BOLT, 0, 5, 1);
		enableAbility(ent, LIGHTNING_STORM, 0, 5, 1);
		enableAbility(ent, MIRV, 0, 5, 1);
		enableAbility(ent, SPIKER, 0, 5, 1);
		enableAbility(ent, OBSTACLE, 0, 5, 1);
		enableAbility(ent, GASSER, 0, 5, 1);
		enableAbility(ent, HEALER, 0, 5, 1);
		enableAbility(ent, SPORE, 0, 5, 1);
		enableAbility(ent, ACID, 0, 5, 1);
		enableAbility(ent, COCOON, 0, 5, 1);
		enableAbility(ent, SELFDESTRUCT, 0, 5, 1);
		enableAbility(ent, FLASH, 0, 1, 1);
	}
}

void ChaseCam(edict_t *ent)
{
	if (ent->client->menustorage.menu_active)
		closemenu(ent);

	if (ent->client->chase_target) 
	{
		ent->client->chase_target = NULL;
		ent->client->ps.gunindex = 0;
		return;
	}

	GetChaseTarget(ent);
}

int ClassNum(edict_t *ent, int team); //GHz
int HighestLevelPlayer(void); //GHz
int TotalPlayersInGame(void)
{
	edict_t *player;
	int i, total = 0;

	for (i = 1; i <= maxclients->value; i++){
		player = &g_edicts[i];

		if (!player->inuse)
			continue;
		if (!player->client)
			continue;
		if (player->solid == SOLID_NOT)
			continue;

		total++;
	}
//	gi.dprintf("found %d players in game\n", total);
	return total;
}

void player_touch (edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	V_Touch(self, other, plane, surf);
}

void StartGame (edict_t *ent)
{
	int		i;
	gitem_t *item=itemlist;

	ent->svflags &= ~SVF_NOCLIENT;
	ent->client->resp.spectator = false;
	ent->client->pers.spectator = false;
	ent->client->ps.stats[STAT_SPECTATOR] = 0;
	PutClientInServer (ent);

	ent->client->pers.combat_changed = ent->myskills.respawns;//4.5 set changed combat preferences to default

	if (invasion->value || pvm->value)  // vrxchile: default combat modes
	{ 
		ent->myskills.respawns = ent->client->pers.combat_changed = HOSTILE_MONSTERS;
	}else if (!invasion->value && !pvm->value && !ffa->value) // pvp mode
	{
		ent->myskills.respawns = ent->client->pers.combat_changed = HOSTILE_PLAYERS;
	}else // anything else?
		ent->myskills.respawns = ent->client->pers.combat_changed = HOSTILE_PLAYERS |  HOSTILE_MONSTERS;

	average_player_level = AveragePlayerLevel();
	ent->health = ent->myskills.current_health;

	if(savemethod->value == 1) // binary .vrx style saving
		for (i=0; i<game.num_items; i++, item++) // reload inventory.
			ent->client->pers.inventory[ITEM_INDEX(item)] = ent->myskills.inventory[ITEM_INDEX(item)];

	ent->client->pers.inventory[flag_index] = 0; // NO FLAG FOR YOU!!!
	ent->client->pers.inventory[red_flag_index] = 0;
	ent->client->pers.inventory[blue_flag_index] = 0;
	//4.2 remove techs
	ent->client->pers.inventory[resistance_index] = 0;
	ent->client->pers.inventory[strength_index] = 0;
	ent->client->pers.inventory[haste_index] = 0;
	ent->client->pers.inventory[regeneration_index] = 0;
	ent->touch = player_touch;
	modify_max(ent);

	Give_respawnitems(ent);
		
	// add a teleportation effect
	ent->s.event = EV_PLAYER_TELEPORT;
	// hold in place briefly
	ent->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
	ent->client->ps.pmove.pm_time = 14;

	if (ent->myskills.boss > 0)
	{
		gi.bprintf(PRINT_HIGH, "A level %d boss known as %s starts their reign.\n", 
			ent->myskills.boss, ent->client->pers.netname);
	}
	else if (IsNewbieBasher(ent) && !ptr->value && !domination->value)
	{
		gi.bprintf(PRINT_HIGH, "A level %d mini-boss known as %s begins their domination.\n", 
		ent->myskills.level, ent->client->pers.netname);
		gi.bprintf(PRINT_HIGH, "Kill %s, and every non-boss gets a reward!\n", ent->client->pers.netname);
		safe_cprintf(ent, PRINT_HIGH, "You are currently a mini-boss. You will receive no point loss, but cannot war.\n");
	}
	else
	{
		gi.bprintf(PRINT_HIGH, "%s starts their reign.\n", ent->client->pers.netname);
	}

	V_UpdatePlayerAbilities(ent);

	//Set the player's name
	strcpy(ent->myskills.player_name, ent->client->pers.netname);

	if (level.time < pregame_time->value && !trading->value) {
		gi.centerprintf(ent, "This map is currently in pre-game\nPlease warm up, upgrade and\naccess the Armory now\n");
		ent->s.effects |= EF_COLOR_SHELL;
		ent->s.renderfx |= (RF_SHELL_RED | RF_SHELL_GREEN | RF_SHELL_BLUE);
	}

	if (trading->value) // Red shell for trading mode.
	{
		gi.centerprintf(ent, "Welcome to trading mode\nBuy stuff and trade runes freely.\nVote for another mode to start playing.\n");
		ent->s.effects |= EF_COLOR_SHELL;
		ent->s.renderfx |= RF_SHELL_RED;
	}
	gi.sound(ent, CHAN_AUTO, gi.soundindex("misc/startup.wav"), 1, ATTN_NORM, 0);
	WriteToLogfile(ent, "Logged in.\n");

#ifndef GDS_NOMULTITHREADING
	ent->ThreadStatus = 0;
#endif

	SavePlayer(ent); // Do we need to?
}

void OpenCombatMenu (edict_t *ent, int lastline);
void combatmenu_handler (edict_t *ent, int option)
{
	switch (option)
	{
	case 1:
		// enable fighting players
		ent->client->pers.combat_changed |= HOSTILE_PLAYERS; break;
	case 2: 
		// enable fighting monsters
		ent->client->pers.combat_changed |= HOSTILE_MONSTERS; break;
	case -1:
		// if we are not hostile against monsters, switch it on
		if (!(ent->client->pers.combat_changed & HOSTILE_MONSTERS))
			ent->client->pers.combat_changed |= HOSTILE_MONSTERS;
		// disable fighting players
		ent->client->pers.combat_changed &= ~HOSTILE_PLAYERS;
		break;
	case -2: 
		// if we are not hostile against players, switch it on
		if (!(ent->client->pers.combat_changed & HOSTILE_PLAYERS))
			ent->client->pers.combat_changed |= HOSTILE_PLAYERS;
		// disable fighting monsters
		ent->client->pers.combat_changed &= ~HOSTILE_MONSTERS;
		break;
	case 3:
		closemenu(ent);

		// new character
		if (ent->client->resp.spectator)
		{
			StartGame(ent);
			SaveCharacter(ent);
		}
		else
		{
			if (level.time > pregame_time->value) // out of pregame
				gi.centerprintf(ent, "Combat preferences will be applied\nthe next time you respawn.");
			else
				gi.centerprintf(ent, "Combat preferences updated!");
		}
		return;
	}

	// update preferences immediately for spectators (new characters)
	if (ent->client->resp.spectator || level.time < pregame_time->value) // pregame no need to suicide
		ent->myskills.respawns = ent->client->pers.combat_changed;

	OpenCombatMenu(ent, ent->client->menustorage.currentline);
}

void OpenCombatMenu (edict_t *ent, int lastline)
{
	if ((V_IsPVP() || pvm->value || invasion->value) && ent->myskills.administrator < 9 && !ent->client->resp.spectator)
	{
		safe_cprintf(ent, PRINT_HIGH, "You're disallowed to modify combat settings on this mode.\n");
		return;
	}

	if (!ShowMenu(ent))
        return;
	clearmenu(ent);

	V_ValidateCombatPreferences(ent);

	if (!lastline)
		lastline = 13;

	//				    xxxxxxxxxxxxxxxxxxxxxxxxxxx (max length 27 chars)
	addlinetomenu(ent, "Combat Preferences", MENU_GREEN_CENTERED);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "Choose your preferences", 0);
	addlinetomenu(ent, "for combat below. Your ", 0);
	addlinetomenu(ent, "selection determines who", 0);
	addlinetomenu(ent, "your opponents will be.", 0);
	addlinetomenu(ent, "You can return to this menu", 0);
	addlinetomenu(ent, "at any time by typing", 0);
	addlinetomenu(ent, "'combat' at the console.", 0);
	addlinetomenu(ent, " ", 0);

	//if (ent->myskills.respawns & HOSTILE_PLAYERS)
	if (ent->client->pers.combat_changed & HOSTILE_PLAYERS)
		addlinetomenu(ent, "Fight Players:  Enabled", -1);
	else
		addlinetomenu(ent, "Fight Players:  Disabled", 1);
	//if (ent->myskills.respawns & HOSTILE_MONSTERS)
	if (ent->client->pers.combat_changed & HOSTILE_MONSTERS)
		addlinetomenu(ent, "Fight Monsters: Enabled", -2);
	else
		addlinetomenu(ent, "Fight Monsters: Disabled", 2);

	addlinetomenu(ent, "Save & Close", 3);

	setmenuhandler(ent, combatmenu_handler);
	ent->client->menustorage.currentline = lastline;
	ent->client->menustorage.menu_index = MENU_COMBAT_PREFERENCES;
	showmenu(ent);
}

qboolean CanJoinGame(edict_t *ent, int returned)
{
	switch(returned)
	{
	case -1:	//bad password
		safe_cprintf(ent, PRINT_HIGH, "Access denied. Incorrect password.\n");
		return false;
	case -2:	//below minimum level
		safe_cprintf(ent, PRINT_HIGH, "You have to be at least level %d to play on this server.\n", ((int)min_level->value));
		return false;
	case -3:	//above maximum level
		safe_cprintf(ent, PRINT_HIGH, "You have to be level %d or below to play here.\n", ((int)max_level->value));
		if (strcmp(reconnect_ip->string, "0") != 0)
		{
			safe_cprintf(ent, PRINT_HIGH, "You are being sent to an alternate server where you can play.\n");
			stuffcmd(ent, va("connect %s\n", reconnect_ip->string));
		}
		return false;
	case -4:	//invalid player name
		safe_cprintf(ent, PRINT_HIGH, "Your name must be greater than 2 characters long.\n");
		return false;
	case -5:	//playing too much
		safe_cprintf(ent, PRINT_HIGH, "Can't join: %d hour play-time limit reached.\n", MAX_HOURS);
		safe_cprintf(ent, PRINT_HIGH, "Please try a different character, or try again tommorow.\n");
		return false;
	case -6:	//newbie basher can't play
		safe_cprintf(ent, PRINT_HIGH, "Unable to join: The current maximum level is %d.\n", NEWBIE_BASHER_MAX);
		safe_cprintf(ent, PRINT_HIGH, "Please return at a later time, or try a different character.\n");
		gi.dprintf("INFO: %s exceeds maximum level allowed by server (level %d)!", ent->client->pers.netname, NEWBIE_BASHER_MAX);
		return false;
	case -7:	//boss can't play
		safe_cprintf(ent, PRINT_HIGH, "Unable to join: Bosses are not allowed unless the server is at least half capacity.\n");
		safe_cprintf(ent, PRINT_HIGH, "Please come back at a later time, or try a different character.\n");
		return false;
	case -8: // stupid name
		safe_cprintf(ent, PRINT_HIGH, "Unable to join with default name \"Player\" - Don't use this name. We won't be able to know who are you.\n");
		return false;
	default:	//passed every check
		return true;
	}
}

void OpenModeMenu(edict_t *ent)
{
	if (ptr->value)
	{
		OpenPTRJoinMenu(ent);
		return;
	}

	if (domination->value)
	{
		OpenDOMJoinMenu(ent);
		return;
	}

	if (ctf->value)
	{
		CTF_OpenJoinMenu(ent);
		return;
	}

	if (ent->myskills.class_num == 0)
	{
		OpenClassMenu(ent, 1); //GHz
		return;
	}

	StartGame(ent);
}

void JoinTheGame (edict_t *ent)
{
	int		returned;

	if (ent->client->menustorage.menu_active)
	{
		closemenu(ent);
		return;
	}

	if (savemethod->value < 2)
		returned = OpenConfigFile(ent);
	else
	{
		// We can't use this function.
		// We instead do it in somewhere else.
		gi.dprintf("Warning: JoinTheGame Called on MYSQL mode!\n");
		return; 
	}

	if (!CanJoinGame(ent, returned))
		return;

	OpenModeMenu(ent); // This implies the game will start.
}

qboolean StartClient(edict_t *ent)
{
	if (ent->client->pers.spectator && !level.intermissiontime) 
	{
		OpenJoinMenu(ent);
		return true;
	}
	return false;
}

void joinmenu_handler (edict_t *ent, int option)
{
	switch (option)
	{
	case 1:
		//If no GDS is running, join the game right away.
#ifndef NO_GDS
		if(savemethod->value < 2)
#endif
			JoinTheGame(ent);
#ifndef NO_GDS
		else
		{
			gi.centerprintf(ent, "You have been queued for Loading.\n Please wait.\n");
			V_GDS_Queue_Add(ent, GDS_LOAD);
		}
#endif
		break;
	case 2: 
		ChaseCam(ent); break;
	case 3: 
		closemenu(ent); break;
	}
}

void OpenJoinMenu (edict_t *ent)
{
	if (!ShowMenu(ent))
        return;
	clearmenu(ent);

	//				    xxxxxxxxxxxxxxxxxxxxxxxxxxx (max length 27 chars)

	addlinetomenu(ent, va("Vortex Indy v%s", VRX_VERSION), MENU_GREEN_CENTERED);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "", 0);
	addlinetomenu(ent, "Original design by Kombat03", 0);
	addlinetomenu(ent, "", 0);
	addlinetomenu(ent, "Programming by GHz, NewB,", 0);
	addlinetomenu(ent, "Kombat03, Chamooze, doomie", 0);
	addlinetomenu(ent, "", 0);
	addlinetomenu(ent, "Maintained by ky0ko", 0);
	addlinetomenu(ent, "with help from darkzard", 0);
	addlinetomenu(ent, "", 0);
	addlinetomenu(ent, "Start your reign", 1);
	addlinetomenu(ent, "Toggle chasecam", 2);
	addlinetomenu(ent, "Exit", 3);

	setmenuhandler(ent, joinmenu_handler);
	ent->client->menustorage.currentline = 12;
	showmenu(ent);
}

void myinfo_handler (edict_t *ent, int option)
{
	closemenu(ent);
}

void OpenMyinfoMenu (edict_t *ent)
{
	if (!ShowMenu(ent))
        return;
	clearmenu(ent);

	//				    xxxxxxxxxxxxxxxxxxxxxxxxxxx (max length 27 chars)
	addlinetomenu(ent, va("%s (%s)", ent->client->pers.netname, 
		GetClassString(ent->myskills.class_num)), MENU_GREEN_CENTERED);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, va("Level:        %d", ent->myskills.level), 0);
	addlinetomenu(ent, va("Experience:   %d", ent->myskills.experience), 0);
	addlinetomenu(ent, va("Next Level:   %d", (ent->myskills.next_level-ent->myskills.experience)+ent->myskills.nerfme), 0);
	addlinetomenu(ent, va("Credits:      %d", ent->myskills.credits), 0);
	if (ent->myskills.shots > 0)
		addlinetomenu(ent, va("Hit Percent:  %d%c", (int)(100*((float)ent->myskills.shots_hit/ent->myskills.shots)), '%'), 0);
	else
		addlinetomenu(ent, "Hit Percent:  --", 0);
	addlinetomenu(ent, va("Frags:        %d", ent->myskills.frags), 0);
	addlinetomenu(ent, va("Fragged:      %d", ent->myskills.fragged), 0);
	if (ent->myskills.fragged > 0)
		addlinetomenu(ent, va("Frag Percent: %d%c", (int)(100*((float)ent->myskills.frags/ent->myskills.fragged)), '%'), 0);
	else
		addlinetomenu(ent, "Frag Percent: --", 0);
	addlinetomenu(ent, va("Played Hrs:   %.1f", (float)ent->myskills.playingtime/3600), 0);
	addlinetomenu(ent, va("Respawns: %d", ent->myskills.weapon_respawns), 0);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "Exit", 1);

	setmenuhandler(ent, myinfo_handler);
		ent->client->menustorage.currentline = 13;
	showmenu(ent);
}

void respawnmenu_handler (edict_t *ent, int option)
{
	if (option == 99)
	{
		closemenu(ent);
		return;
	}

	ent->myskills.respawn_weapon = option;
}

char *GetRespawnString (edict_t *ent)
{
	switch (ent->myskills.respawn_weapon)
	{
	case 1: return "Sword";
	case 2: return "Shotgun";
	case 3: return "Super Shotgun";
	case 4: return "Machinegun";
	case 5: return "Chaingun";
	case 6: return "Grenade Launcher";
	case 7: return "Rocket Launcher";
	case 8: return "Hyperblaster";
	case 9: return "Railgun";
	case 10: return "BFG10k";
	case 11: return "Hand Grenades";
	case 12: return "20mm Cannon";
	case 13: return "Blaster";
	default: return "Unknown";
	}
}

void OpenRespawnWeapMenu(edict_t *ent)
{
	if (!ShowMenu(ent))
        return;

	if(ent->myskills.class_num == CLASS_KNIGHT)
	{
		safe_cprintf(ent, PRINT_HIGH, "Knights are restricted to a sabre.\n");
		return;
	}

	if(ent->myskills.class_num == CLASS_POLTERGEIST)
	{
		safe_cprintf(ent, PRINT_HIGH, "You can't pick a respawn weapon.\n");
		return;
	}

	clearmenu(ent);

	//				xxxxxxxxxxxxxxxxxxxxxxxxxxx (max length 27 chars)
	addlinetomenu(ent, "Pick a respawn weapon.", MENU_GREEN_CENTERED);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "Sword", 1);
	addlinetomenu(ent, "Shotgun", 2);
	addlinetomenu(ent, "Super Shotgun", 3);
	addlinetomenu(ent, "Machinegun", 4);
	addlinetomenu(ent, "Chaingun", 5);
	addlinetomenu(ent, "Hand Grenades", 11);
	addlinetomenu(ent, "Grenade Launcher", 6);
	addlinetomenu(ent, "Rocket Launcher", 7);
	addlinetomenu(ent, "Hyperblaster", 8);
	addlinetomenu(ent, "Railgun", 9);
	addlinetomenu(ent, "BFG10k", 10);
	addlinetomenu(ent, "20mm Cannon", 12);
	addlinetomenu(ent, "Blaster", 13);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, va("Respawn: %s", GetRespawnString(ent)), 0);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "Exit", 99);

	setmenuhandler(ent, respawnmenu_handler);
		ent->client->menustorage.currentline = 3;
	showmenu(ent);
}

void classmenu_handler (edict_t *ent, int option)
{
	int points_needed;
	int page_num = (option / 1000);
	int page_choice = (option % 1000);
	int i;

	if ((page_num == 1) && (page_choice == 1))
	{
		OpenJoinMenu(ent);
		return;
	}
	else if (page_num > 0)
	{
		if (page_choice == 2)	//next
            OpenClassMenu (ent, page_num+1);
		else if (page_choice == 1)	//prev
			OpenClassMenu (ent, page_num-1);
		return;
	}
	//don't cause an invalid class to be created (option 99 is checked as well)
	else if ((option > CLASS_MAX) || (option < 1))
	{
		closemenu(ent);
		return;
	}

	// assign new players a team if the start level is low
	// otherwise, deny them access until the next game
	if (ctf->value && (level.time > pregame_time->value))
	{
		if (start_level->value > 1)
		{
			safe_cprintf(ent, PRINT_HIGH, "Can't join in the middle of a CTF game.\n");
			return;
		}

		// assign a random team
		ent->teamnum = GetRandom(1, 2);
	}

	for (i = 0; i < start_level->value; ++i)
	{
		points_needed = start_nextlevel->value * pow(nextlevel_mult->value, i);
		if (points_needed > 50000)
			points_needed = 50000;
		ent->myskills.experience += points_needed;
	}

	ent->myskills.class_num = option;
	disableAbilities(ent);
	setGeneralAbilities(ent);
	setClassAbilities(ent);
	setTalents(ent);
	ent->myskills.weapon_respawns = 100;
	gi.dprintf("INFO: %s created a new %s!\n", ent->client->pers.netname, GetClassString(ent->myskills.class_num));
	WriteToLogfile(ent, va("%s created a %s.\n", ent->client->pers.netname, GetClassString(ent->myskills.class_num)));
	check_for_levelup(ent);
	modify_max(ent);
	Give_respawnweapon(ent, ent->myskills.respawn_weapon);
	Give_respawnitems(ent);
	//StartGame(ent);
	resetWeapons(ent);

	// FIXME: we should do a better job of selecting a team OR block them from joining (temp make non-spec to make savechar() happy)
	// we need to select a team if we're past pre-game time in CTF or Domination modes
	if ((ctf->value || domination->value) && (level.time > pregame_time->value))
		ent->teamnum = GetRandom(1, 2);

	OpenCombatMenu(ent, 0);
	//StartGame(ent);4.5
	//SaveCharacter(ent);4.5
}

void OpenClassMenu (edict_t *ent, int page_num)
{
	int i;

	if (!ShowMenu(ent))
        return;
	clearmenu(ent);

	//				xxxxxxxxxxxxxxxxxxxxxxxxxxx (max length 27 chars)
	addlinetomenu(ent, "Please select your", MENU_GREEN_CENTERED);
	addlinetomenu(ent, "character class:", MENU_GREEN_CENTERED);
	addlinetomenu(ent, " ", 0);

	for (i = ((page_num-1)*11); i < (page_num*11); ++i)
	{
		if (i < CLASS_MAX)
			addlinetomenu(ent, va("%s", GetClassString(i+1)), i+1);
		else addlinetomenu(ent, " ", 0);
	}

	addlinetomenu(ent, " ", 0);
	if (i < CLASS_MAX) addlinetomenu(ent, "Next", (page_num*1000)+2);
	addlinetomenu(ent, "Back", (page_num*1000)+1);
	addlinetomenu(ent, "Exit", 99);

	setmenuhandler(ent, classmenu_handler);
	if (CLASS_MAX > 11)
		ent->client->menustorage.currentline = 15;
	else ent->client->menustorage.currentline = 5 + i;
	showmenu(ent);
}

void masterpw_handler (edict_t *ent, int option)
{
	closemenu(ent);
}

void OpenMasterPasswordMenu (edict_t *ent)
{
	if (!ShowMenu(ent))
        return;
	clearmenu(ent);

	addlinetomenu(ent, "Master Password", MENU_GREEN_CENTERED);
	addlinetomenu(ent, " ", 0);

	if (strcmp(ent->myskills.email, ""))
	{
		addlinetomenu(ent, "A master password has", 0);
		addlinetomenu(ent, "already been set and can't", 0);
		addlinetomenu(ent, "be changed!", 0);
		addlinetomenu(ent, " ", 0);
		addlinetomenu(ent, " ", 0);
		addlinetomenu(ent, " ", 0);
	}
	else
	{
		// open prompt for password
		stuffcmd(ent, "messagemode\n");

		addlinetomenu(ent, "Please enter a master", 0);
		addlinetomenu(ent, "password above. If you", 0);
		addlinetomenu(ent, "forget your normal", 0);
		addlinetomenu(ent, "password, you can use", 0);
		addlinetomenu(ent, "this one to recover your", 0);
		addlinetomenu(ent, "character.", 0);
	}

	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "Close", 1);

	setmenuhandler(ent, masterpw_handler);
	ent->client->menustorage.currentline = 10;
	ent->client->menustorage.menu_index = MENU_MASTER_PASSWORD;
	showmenu(ent);
}

void generalmenu_handler (edict_t *ent, int option)
{
	switch (option)
	{
	case 1: OpenUpgradeMenu(ent); break;
	case 2: OpenWeaponUpgradeMenu(ent, 0); break;
	case 3: OpenTalentUpgradeMenu(ent, 0); break;
	case 4: OpenRespawnWeapMenu(ent); break;
	case 5: OpenMasterPasswordMenu(ent); break;
	case 6: OpenMyinfoMenu(ent); break;
	case 7: OpenArmoryMenu(ent); break;
	case 8: ShowInventoryMenu(ent, 0, false); break;
	case 9: ShowAllyMenu(ent); break;
	case 10: ShowTradeMenu(ent); break;
	case 11: ShowVoteModeMenu(ent); break;
	case 12: ShowHelpMenu(ent, 0); break;
	case 13: OpenCombatMenu(ent, 0); break;
	default: closemenu(ent);
	}
}

void OpenGeneralMenu (edict_t *ent)
{
	if (!ShowMenu(ent))
        return;
	clearmenu(ent);

		//				xxxxxxxxxxxxxxxxxxxxxxxxxxx (max length 27 chars)
	addlinetomenu(ent, "Welcome to Vortex!", MENU_GREEN_CENTERED);
	addlinetomenu(ent, "Please choose a sub-menu.", MENU_GREEN_CENTERED);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "Upgrade abilities", 1);
	if (ent->myskills.class_num != CLASS_POLTERGEIST)
		addlinetomenu(ent, "Upgrade weapons", 2);
	addlinetomenu(ent, "Upgrade talents", 3);
	addlinetomenu(ent, " ", 0);
	if (ent->myskills.class_num != CLASS_POLTERGEIST && 
		ent->myskills.class_num != CLASS_KNIGHT)
	addlinetomenu(ent, "Set respawn weapon", 4);
	addlinetomenu(ent, "Set master password", 5);
	addlinetomenu(ent, "Show character info", 6);
	addlinetomenu(ent, "Access the armory", 7);
	addlinetomenu(ent, "Access your items", 8);
	addlinetomenu(ent, "Form alliance", 9);
	addlinetomenu(ent, "Trade items", 10);
	addlinetomenu(ent, "Vote for map/mode", 11);
	addlinetomenu(ent, "Help", 12);
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "Close", 99);

	setmenuhandler(ent, generalmenu_handler);
	ent->client->menustorage.currentline = 18;
	showmenu(ent);
}

char *G_GetTruncatedIP (edict_t *player);

char *GetTeamString (edict_t *ent)
{
	if (ent->teamnum == 1)
		return "Red";
	if (ent->teamnum == 2)
		return "Blue";
	if (numAllies(ent) > 0)
		return "Allied";
	return "None";
}

char *GetStatusString (edict_t *ent)
{
	if (ent->health < 1)
		return "Dead";
	if (ent->flags & FL_CHATPROTECT)
		return "Chat Protect";
	if (ent->client->idle_frames > 300)
		return "Idle";
	return "Normal";
}
	
void OpenWhoisMenu (edict_t *ent)
{
	edict_t *player;

	if (!ShowMenu(ent))
        return;
	clearmenu(ent);

	if ((player = FindPlayerByName(gi.argv(1))) == NULL)
	{
		safe_cprintf(ent, PRINT_HIGH, "Couldn't find player \"%s\".\n", gi.argv(1));
		return;
	}

	if (G_IsSpectator(player))
		return;

	addlinetomenu(ent, "Whois Information", MENU_GREEN_CENTERED);
	addlinetomenu(ent, "", 0);

	// print name and class
	addlinetomenu(ent, va("%s (%s)", player->client->pers.netname, 
		GetClassString(player->myskills.class_num)), MENU_GREEN_CENTERED);

	// print IP address
	if (ent->myskills.administrator)
		addlinetomenu(ent, player->client->pers.current_ip, MENU_GREEN_CENTERED);
	else
		addlinetomenu(ent, G_GetTruncatedIP(player), MENU_GREEN_CENTERED);

	addlinetomenu(ent, "", 0);

	addlinetomenu(ent, va("Admin:        %s", player->myskills.administrator?"Yes":"No"), 0);
	addlinetomenu(ent, va("Owner:        %s", player->myskills.owner), 0);
	addlinetomenu(ent, va("Status:       %s", GetStatusString(player)), 0);
	addlinetomenu(ent, va("Team:         %s", GetTeamString(player)), 0);
	addlinetomenu(ent, va("Level:        %d", player->myskills.level), 0);
	addlinetomenu(ent, va("Experience:   %d", player->myskills.experience), 0);
	addlinetomenu(ent, va("Hit Percent:  %d%c", (int)(100*((float)player->myskills.shots_hit/player->myskills.shots)), '%'), 0);
	addlinetomenu(ent, va("Frag Percent: %d%c", (int)(100*((float)player->myskills.frags/player->myskills.fragged)), '%'), 0);
	addlinetomenu(ent, va("Played Hours: %.1f", (float)player->myskills.playingtime/3600), 0);


	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, "Exit", 1);

	setmenuhandler(ent, masterpw_handler);//FIXME: is this safe?
		ent->client->menustorage.currentline = 16;
	showmenu(ent);
}
