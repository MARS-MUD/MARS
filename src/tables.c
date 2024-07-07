/******************************************************************************
*       *****   **    **           **             ***** ***        *******    *         
*    ******  ***** *****        *****          ******  * **      *       ***  *         
*   **   *  *  ***** *****     *  ***         **   *  *  **     *         **  *         
*  *    *  *   * **  * **         ***        *    *  *   **     **        *   *         
*      *  *    *     *           *  **           *  *    *       ***          *         
*     ** **    *     *           *  **          ** **   *       ** ***        *         
*     ** **    *     *          *    **         ** **  *         *** ***      *         
*     ** **    *     *          *    **         ** ****            *** ***    *         
*     ** **    *     *         *      **        ** **  ***           *** ***  *         
*     ** **    *     **        *********        ** **    **            ** *** *         
*     *  **    *     **       *        **       *  **    **             ** ** *         
*        *     *      **      *        **          *     **              * *  *         
*    ****      *      **     *****      **     ****      ***   ***        *   *         
*   *  *****           **   *   ****    ** *  *  ****    **   *  *********    *         
*  *     **                *     **      **  *    **     *   *     *****      *         
*  *                       *                 *               *                *         
*    **                      **                **              **             *         
*                                                                             *
*  MULTI                    ADVENTURER         ROLEPLAYING     SYSTEM         *
*  MARS 0.1b is copyright 2015-2024 Ro Black mars@wr3tch.org                  *
******************************************************************************/
/******************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,           *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.      *
 *                                                                            *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael             *
 *  Chastain, Michael Quan, and Mitchell Tse.                                 *
 *                                                                            *
 *  In order to use any part of this Merc Diku Mud, you must comply with      *
 *  both the original Diku license in 'license.doc' as well the Merc          *
 *  license in 'license.txt'.  In particular, you may not remove either of    *
 *  these copyright notices.                                                  *
 *                                                                            *
 *  Much time and thought has gone into this software and you are             *
 *  benefitting.  We hope that you share your changes too.  What goes         *
 *  around, comes around.                                                     *
 *****************************************************************************/

/******************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			                      *
*	ROM has been brought to you by the ROM consortium		                  *
*	    Russ Taylor (rtaylor@hypercube.org)				                      *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			                  *
*	    Brian Moore (zump@rom.org)					                          *
*	By using this code, you have agreed to follow the terms of the	          *
*	ROM license, in the file Rom24/doc/rom.license			                  *
******************************************************************************/
/* dwatch egate artifice fdmud code copyright 2002-2008 Bob Kircher	  */


#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"


/* For position.  See also position_types. */
const struct position_type position_table[] =
{
    {	"dead",			"dead"	},
    {	"mortally wounded",	"mort"	},
    {	"incapacitated",	"incap"	},
    {	"stunned",		"stun"	},
    {	"sleeping",		"sleep"	},
    {	"resting",		"rest"	},
    {   "sitting",		"sit"   },
    {	"fighting",		"fight"	},
    {	"standing",		"stand"	},
    {	NULL,			NULL	}
};


/*****************************************************************************
 Name:		flag_stat_table
 Purpose:	This table categorizes the tables following the lookup
 		functions below into stats and flags.  Flags can be toggled
 		but stats can only be assigned.  Update this table when a
 		new set of flags or types is installed.
 ****************************************************************************/
const struct flag_stat_type flag_stat_table[] =
{
    {	act_flags,		FALSE	},	/* bitvector	*/
    {	act2_flags,		FALSE	},
    {	affect_flags,		TRUE	},
    {	apply_types,		TRUE	},	/* value	*/
    {	area_flags,		FALSE	},
    {	automob_types,		TRUE	},
    {	book_flags,		FALSE	},
    {	class_flags,		FALSE	},
    {	cmd_flags,		FALSE	},
    {	color_types,		TRUE	},
    {	comm_flags,		FALSE	},
    {	container_flags,	FALSE	},
    {	continent_types,	TRUE	},
    {	death_types,		TRUE	},
    {	dream_flags,		FALSE	},
    {	dream_types,		TRUE	},
    {	eprog_types,		TRUE	},
    {	exit_flags,		FALSE	},
    {	extra_flags,		FALSE	},
    {	form_flags,		FALSE	},
    {	furniture_flags,	FALSE	},
    {	herb_flags,		FALSE	},
    {	imm_flags,		FALSE	},
    {	invoke_types,		TRUE	},
    {	item_types,		TRUE	},
    {	kingdom_types,		TRUE	},
    {	level_types,		TRUE	},
    {	log_types,		TRUE	},
    {	material_types,		TRUE	},
    {   mprog_types,            TRUE    },
    {	off_flags,		TRUE	},
    {	old_level_types,	TRUE	},
    {	oprog_types,		TRUE	},
    {	part_flags,		FALSE	},
    {	plr_flags,		FALSE	},
    {	portal_flags,		FALSE	},
    {	position_types,		TRUE	},
    {	priority_flags,		TRUE	},
    {	rdesc_types,		TRUE	},
    {	res_flags,		FALSE	},
    {	room_aff_flags,		TRUE	},
    {	room_flags,		FALSE	},
    {	rprog_types,		TRUE	},
    {	sector_types,		TRUE	},
    {	security_flags,		TRUE	},
    {	sex_types,		TRUE	},
    {	sheath_flags,		FALSE	},
    {	shield_types,		TRUE	},
    {	size_types,		TRUE	},
    {	spell_flags,		FALSE	},
    {	stat_types,		TRUE	},
    {	target_types,		TRUE	},
    {	vehicle_types,		TRUE	},
    {	vuln_flags,		FALSE	},
    {	weapon_flags,		TRUE	},
    {	weapon_types,		TRUE	},
    {	weapon_type_olc,	FALSE	},
    {	wear_flags,		FALSE	},
    {	wear_loc_strings,	TRUE	},
    {	wear_loc_types,		TRUE	},
    {	windspeed_types,	TRUE	},
    {	world_types,		TRUE	},

    {   0,			0	}
};


/* various flag tables */

const struct flag_type act_flags[] =
{
    {	"npc",		ACT_IS_NPC,	FALSE	},
    {	"aggressive",	ACT_AGGRESSIVE,	TRUE	},
    {	"artificer",	ACT_ARTIFICER,	TRUE	},
    {	"changer",	ACT_IS_CHANGER,	TRUE	},
    {	"gain",		ACT_GAIN,	TRUE	},
    {	"healer",	ACT_IS_HEALER,	TRUE	},
    {	"indoors",	ACT_INDOORS,	TRUE	},
    {	"mount",	ACT_MOUNT,	TRUE	},
    {	"noalign",	ACT_NOALIGN,	TRUE	},
    {	"nopurge",	ACT_NOPURGE,	TRUE	},
    {	"noquest",	ACT_NOQUEST,	TRUE	},
    {	"outdoors",	ACT_OUTDOORS,	TRUE	},
    {	"pet",		ACT_PET,	TRUE	},
    {	"practice",	ACT_PRACTICE,	TRUE	},
    {	"scavenger",	ACT_SCAVENGER,	TRUE	},
    {	"sentinel",	ACT_SENTINEL,	TRUE	},
    {	"stay_area",	ACT_STAY_AREA,	TRUE	},
    {	"tethered",	ACT_TETHERED,	FALSE	},
    {	"train",	ACT_TRAIN,	TRUE	},
    {	"undead",	ACT_UNDEAD,	TRUE	},
    {	"update_always",ACT_UPDATE_ALWAYS, TRUE	},
    {	"wimpy",	ACT_WIMPY,	TRUE	},
    {	NULL,			0,	FALSE	}
};

const struct flag_type act2_flags[] =
{
    {	"coder",		CODER,		FALSE	},
    {	"headbuilder",		HBUILDER,	FALSE	},
    {	"fmeter",		PLR_FMETER,	FALSE	},
    {	"peek",			PLR_PEEK,	FALSE	},
    {	"autochannel",		PLR_AUTOCHANNEL, FALSE	},
    {	"ploaded",		PLR_PLOADED,	FALSE	},
    {	"coding",		PLR_CODING,	FALSE	},
    {	"marking",		PLR_MARKING,	FALSE	},
    {	"rflags",		PLR_RFLAGS,	FALSE	},
    {	"autoopen",		PLR_AUTOOPEN,	FALSE	},
    {	"automove",		PLR_AUTOMOVE,	FALSE	},
    {	"pose",			PLR_POSE,	FALSE	},
    {	"nopunct",		PLR_NOPUNCT,	FALSE	},
    {	"outcast",		PLR_OUTCAST,	FALSE	},
    {	"nocopy",		PLR_NOCOPY,	FALSE	},
    {   "autotick",               PLR_AUTOTICK,     FALSE   },
    {	NULL,			0,		FALSE	}
};

const struct flag_type affect_flags[] =
{
    {	"berserk",		AFF_BERSERK,		TRUE	},
     {	"black mantle",		AFF_BLACK_MANTLE,		TRUE	},
    {	"blind",		AFF_BLIND,		TRUE	},
    {	"calm",			AFF_CALM,		TRUE	},
    {	"charm",		AFF_CHARM,		TRUE	},
    {	"curse",		AFF_CURSE,		TRUE	},
    {	"dancing_lights",	AFF_DANCING,		TRUE	},
    {	"dark_vision",		AFF_DARK_VISION,	TRUE	},
    {   "deathgrip",            AFF_DEATHGRIP,		TRUE	},
    {	"detect_good",		AFF_DETECT_GOOD,	TRUE	},
    {	"detect_evil",		AFF_DETECT_EVIL,	TRUE	},
    {	"detect_hidden",	AFF_DETECT_HIDDEN,	TRUE	},
    {	"detect_invis",		AFF_DETECT_INVIS,	TRUE	},
    {	"detect_magic",		AFF_DETECT_MAGIC,	TRUE	},
    {	"faerie_fire",		AFF_FAERIE_FIRE,	TRUE	},
    {	"flying",		AFF_FLYING,		TRUE	},
    {	"haste",		AFF_HASTE,		TRUE	},
    {	"hide",			AFF_HIDE,		TRUE	},
    {	"infrared",		AFF_INFRARED,		TRUE	},
    {	"invisible",		AFF_INVISIBLE,		TRUE	},
    {	"pass_door",		AFF_PASS_DOOR,		TRUE	},
    {	"peace",		AFF_PEACE,		TRUE	},
    {	"plague",		AFF_PLAGUE,		TRUE	},
    {	"poison",		AFF_POISON,		TRUE	},
    {	"protect_good",		AFF_PROTECT_GOOD,	TRUE	},
    {	"protect_evil",		AFF_PROTECT_EVIL,	TRUE	},
    {	"regeneration",		AFF_REGENERATION,	TRUE	},
    {	"sanctuary",		AFF_SANCTUARY,		TRUE	},
    {	"sleep",		AFF_SLEEP,		TRUE	},
    {	"slow",			AFF_SLOW,		TRUE	},
    {	"sneak",		AFF_SNEAK,		TRUE	},
    {	"swim",			AFF_SWIM,		TRUE	},
    {	"weaken",		AFF_WEAKEN,		TRUE	},
//  {	"flame shield",		AFF_FLAME_SHIELD,	TRUE	},
//  { 	"static shield",	AFF_STATIC_SHIELD,	TRUE	},
//  {	"force shield",		AFF_FORCE_SHIELD,	TRUE	},
    {	NULL,			0,			0	}
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_types[] =
{
    {	"none",			APPLY_NONE,		TRUE	},
    {	"strength",		APPLY_STR,		TRUE	},
    {	"dexterity",		APPLY_DEX,		TRUE	},
    {	"intelligence",		APPLY_INT,		TRUE	},
    {	"wisdom",		APPLY_WIS,		TRUE	},
    {	"constitution",		APPLY_CON,		TRUE	},
    {	"sex",			APPLY_SEX,		TRUE	},
    {	"class",		APPLY_CLASS,		TRUE	},
    {	"level",		APPLY_LEVEL,		TRUE	},
    {	"age",			APPLY_AGE,		TRUE	},
    {	"height",		APPLY_HEIGHT,		TRUE	},
    {	"weight",		APPLY_WEIGHT,		TRUE	},
    {	"size",			APPLY_SIZE,		FALSE	},
    {	"mana",			APPLY_MANA,		TRUE	},
    {	"hp",			APPLY_HIT,		TRUE	},
    {	"move",			APPLY_MOVE,		TRUE	},
    {	"wealth",		APPLY_WEALTH,		TRUE	},
    {	"experience",		APPLY_EXP,		TRUE	},
    {	"ac",			APPLY_AC,		TRUE	},
    {	"hitroll",		APPLY_HITROLL,		TRUE	},
    {	"damroll",		APPLY_DAMROLL,		TRUE	},
    {	"saves",		APPLY_SAVES,		TRUE	},
    {	"saving-para",		APPLY_SAVING_PARA,	TRUE	},
    {	"saving-rod",		APPLY_SAVING_ROD,	TRUE	},
    {	"saving-petri",		APPLY_SAVING_PETRI,	TRUE	},
    {	"saving-breath",	APPLY_SAVING_BREATH,	TRUE	},
    {	"saving-spell",		APPLY_SAVING_SPELL,	TRUE	},
    {	"spell-affect",		APPLY_SPELL_AFFECT,	TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type area_flags[] =
{
    {	"changed",	AREA_CHANGED,		FALSE	},
    {	"prototype",	AREA_PROTOTYPE,		TRUE	},
    {	"quiet",	AREA_QUIET,		TRUE	},
    {	"noquest",	AREA_NOQUEST,		TRUE	},
    {	"clan",		AREA_CLAN,		TRUE	},
    {	"mudschool",	AREA_MUDSCHOOL,		TRUE	},
    {	"freeze",	AREA_FREEZE,		TRUE	},
    {	"no_unfinished",AREA_NO_UNFINISHED,	TRUE	},
    {	"quest",	AREA_QUEST,		TRUE	},
    {	"travedit",	AREA_TRAVEDIT,		TRUE	},
    {	"arena",	AREA_ARENA,		TRUE	},
    {	"nolist",	AREA_NOLIST,		TRUE	},
    {   "nowrap",       AREA_NOWRAP,            TRUE    },
    {	NULL,		0,			0	}
};

const struct flag_type automob_types[] =
{
    {	"noset",	DICE_NOSET,		TRUE	},
    {	"easy",		DICE_EASY,		TRUE	},
    {	"medium",	DICE_MEDIUM,		TRUE	},
    {	"hard",		DICE_HARD,		TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type book_flags[] =
{
    {	"pickproof",	CONT_PICKPROOF,		TRUE	},
    {	"locked",	CONT_LOCKED,		TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type class_flags[] =
{
    {	"mage",		MCLASS_MAGE,		TRUE	},
    {	"cleric",	MCLASS_CLERIC,		TRUE	},
    {	"fighter",	MCLASS_FIGHTER,		TRUE	},
    {	"rogue",	MCLASS_ROGUE,		TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type color_types[] =
{
    {	"darkred",	AT_BLOOD,	TRUE	},
    {	"brightred",	AT_RED,		TRUE	},
    {	"darkgreen",	AT_DGREEN,	TRUE	},
    {	"brightgreen",	AT_GREEN,	TRUE	},
    {	"darkblue",	AT_DBLUE,	TRUE	},
    {	"lightblue",	AT_BLUE,	TRUE	},
    {	"orange",	AT_ORANGE,	TRUE	},
    {	"yellow",	AT_YELLOW,	TRUE	},
    {	"darkpurple",	AT_PURPLE,	TRUE	},
    {	"lightpurple",	AT_PINK,	TRUE	},
    {	"darkcyan",	AT_CYAN,	TRUE	},
    {	"lightcyan",	AT_LBLUE,	TRUE	},
    {	"darkgrey",	AT_DGREY,	TRUE	},
    {	"lightgrey",	AT_GREY,	TRUE	},
    {	"white",	AT_WHITE,	TRUE	},
    {	NULL,		0,		0	}
};

const struct flag_type cmd_flags[] =
{
    {	"both",			CMD_BOTH,		TRUE	},
    {	"clan",			CMD_CLAN,		TRUE	},
    {	"dead",			CMD_DEAD,		TRUE	},
    {	"disable",		CMD_DISABLED,		TRUE	},
    {	"exact",		CMD_EXACT,		TRUE	},
    {	"nolist",		CMD_NOLIST,		TRUE	},
    {	"mobonly",		CMD_MOB,		TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type comm_flags[] =
{
    {	"quiet",		COMM_QUIET,		TRUE	},
    {   "deaf",			COMM_DEAF,		TRUE	},

    {	"detail",		COMM_DETAIL,		TRUE	},
    {   "compact",		COMM_COMPACT,		TRUE	},
    {   "brief",		COMM_BRIEF,		TRUE	},
    {   "prompt",		COMM_PROMPT,		TRUE	},
    {   "combine",		COMM_COMBINE,		TRUE	},
    {   "telnet_ga",		COMM_TELNET_GA,		TRUE	},
    {   "show_affects",		COMM_SHOW_AFFECTS,	TRUE	},
    {   "nograts",		COMM_NOGRATS,		TRUE	},
    {	"vnums",		COMM_VNUMS,		TRUE	},
    {   "noemote",		COMM_NOEMOTE,		FALSE	},
    {   "noshout",		COMM_NOSHOUT,		FALSE	},
    {   "notell",		COMM_NOTELL,		FALSE	},
    {   "nochannels",		COMM_NOCHANNELS,	FALSE	},
    {   "snoop_proof",		COMM_SNOOP_PROOF,	FALSE	},
    {   "afk",			COMM_AFK,		TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type container_flags[] =
{
    {	"closeable",	CONT_CLOSEABLE,		TRUE	},
    {	"pickproof",	CONT_PICKPROOF,		TRUE	},
    {	"closed",	CONT_CLOSED,		TRUE	},
    {	"locked",	CONT_LOCKED,		TRUE	},
    {	"put_on",	CONT_PUT_ON,		TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type continent_types[] =
{
    {   "None",		LAND_NONE,	TRUE	},
    {   "Overland",	LAND_OVERLAND,	TRUE	},
    {   "Gravmere",	LAND_GRAVMERE,	TRUE	},
    {   "Tinkerton",	LAND_TINKERTON,	TRUE	},
    {   "Dark Sands",	LAND_DARK_SANDS,	TRUE	},
    {   "Black Isle",	LAND_BLACK_ISLE,	TRUE	},
    {   NULL,		0,		0	}
};

const struct flag_type death_types[] =
{
    {	"none",		DEATH_NONE,		TRUE	},
    {	"ask",		DEATH_ASK,		TRUE	},
    {	"confirm",	DEATH_CONFIRM,		TRUE	},
    {	"search",	DEATH_SEARCH,		TRUE	},
    {	"found",	DEATH_FOUND,		TRUE	},
    {	"resurrect",	DEATH_RESURRECT,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type dream_flags[] =
{
    {	"force",	DREAM_FORCE,		TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type dream_types[] =
{
    {	"normal",	DREAM_NORMAL,		TRUE	},
    {	"nightmare",	DREAM_NIGHTMARE,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type eprog_types[] =
{
    {   "error_prog",	EXIT_PROG_ERROR,	FALSE	},
    {   "close_prog",	EXIT_PROG_CLOSE,	TRUE	},
    {   "enter_prog",	EXIT_PROG_ENTER,	TRUE	},
    {   "exit_prog",	EXIT_PROG_EXIT,		TRUE	},
    {	"knock_prog",	EXIT_PROG_KNOCK,	TRUE	},
    {	"listen_prog",	EXIT_PROG_LISTEN,	TRUE	},
    {   "lock_prog",	EXIT_PROG_LOCK,		TRUE	},
    {   "look_prog",	EXIT_PROG_LOOK,		TRUE	},
    {   "open_prog",	EXIT_PROG_OPEN,		TRUE	},
    {   "pass_prog",	EXIT_PROG_PASS,		TRUE	},
    {   "pick_prog",	EXIT_PROG_PICK,		TRUE	},
    {   "scry_prog",	EXIT_PROG_SCRY,		TRUE	},
    {   "unlock_prog",	EXIT_PROG_UNLOCK,	TRUE	},
    {   NULL,		0,			0	}
};

const struct flag_type event_area_types[] =
{
    {	NULL,		0,			0	}
};

const struct flag_type event_char_types[] =
{
    {	"mpdelay",	EVENT_CHAR_MPDELAY,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type event_desc_types[] =
{
    {	"idle",		EVENT_DESC_IDLE,	TRUE	},
    {	"login",	EVENT_DESC_LOGIN,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type event_game_types[] =
{
    {	NULL,		0,			0	}
};

const struct flag_type event_obj_types[] =
{
    {	"mpdelay",	EVENT_OBJ_MPDELAY,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type event_room_types[] =
{
    {	"mpdelay",	EVENT_ROOM_MPDELAY,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type exit_flags[] =
{
    {   "door",			EX_ISDOOR,		TRUE    },
    {	"closed",		EX_CLOSED,		TRUE	},
    {	"locked",		EX_LOCKED,		TRUE	},
    {	"bashed",		EX_BASHED,		FALSE	},
    {	"bashproof",		EX_BASHPROOF,		TRUE	},
    {	"pickproof",		EX_PICKPROOF,		TRUE	},
    {	"nopass",		EX_NOPASS,		TRUE	},
    {	"hidden",		EX_HIDDEN,		TRUE	},
    {	"nomob",		EX_NO_MOB,		TRUE	},
    {	"secret",		EX_SECRET,		TRUE	},
    {	"noreset",		EX_NORESET,		TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type extra_flags[] =
{
    {	"glow",			ITEM_GLOW,		TRUE	},
    {	"hum",			ITEM_HUM,		TRUE	},
    {	"dark",			ITEM_DARK,		TRUE	},
    {	"hidden",		ITEM_HIDDEN,		TRUE	},
    {	"evil",			ITEM_EVIL,		TRUE	},
    {	"invis",		ITEM_INVIS,		TRUE	},
    {	"invis_day",		ITEM_INVIS_DAY,		TRUE	},
    {	"invis_night",		ITEM_INVIS_NIGHT,	TRUE	},
    {	"magic",		ITEM_MAGIC,		TRUE	},
    {	"nodrop",		ITEM_NODROP,		TRUE	},
    {	"bless",		ITEM_BLESS,		TRUE	},
    {	"anti-good",		ITEM_ANTI_GOOD,		TRUE	},
    {	"anti-evil",		ITEM_ANTI_EVIL,		TRUE	},
    {	"anti-neutral",		ITEM_ANTI_NEUTRAL,	TRUE	},
    {	"noremove",		ITEM_NOREMOVE,		TRUE	},
    {	"inventory",		ITEM_INVENTORY,		TRUE	},
    {	"nopurge",		ITEM_NOPURGE,		TRUE	},
    {	"rot_death",		ITEM_ROT_DEATH,		TRUE	},
    {	"vis_death",		ITEM_VIS_DEATH,		TRUE	},
    {	"nonmetal",		ITEM_NONMETAL,		TRUE	},
    {   "nolocate",             ITEM_NOLOCATE,	        TRUE    },
    {	"meltdrop",		ITEM_MELT_DROP,		TRUE	},
    {	"had_timer",		ITEM_HAD_TIMER,		FALSE	},
    {	"sell_extract",		ITEM_SELL_EXTRACT,	TRUE	},
    {	"burnproof",		ITEM_BURN_PROOF,	TRUE	},
    {	"nouncurse",		ITEM_NOUNCURSE,		TRUE	},
    {	"whistle",		ITEM_WHISTLE,		TRUE	},
    {	"aquest",		ITEM_QUESTOBJ,		TRUE	},
    {	"nolist",		ITEM_NOLIST,		TRUE	},
    {	"noidentify",		ITEM_NOIDENTIFY,	TRUE	},
    //{	"quest",		ITEM_QUESTOBJ,	TRUE	},
    
    {	NULL,			0,			0	}
};

const struct flag_type extra_flags2[] =
{

    {	"enone",		ITEM2_NONE,	TRUE	},
    
    {	NULL,			0,			0	}
};

const struct flag_type form_flags[] =
{
    {	"edible",		FORM_EDIBLE,		TRUE	},
    {	"poison",		FORM_POISON,		TRUE	},
    {	"magical",		FORM_MAGICAL,		TRUE	},
    {	"instant_decay",	FORM_INSTANT_DECAY,	TRUE	},
    {	"other",		FORM_OTHER,		TRUE	},
    {	"animal",		FORM_ANIMAL,		TRUE	},
    {	"sentient",		FORM_SENTIENT,		TRUE	},
    {	"undead",		FORM_UNDEAD,		TRUE	},
    {	"construct",		FORM_CONSTRUCT,		TRUE	},
    {	"mist",			FORM_MIST,		TRUE	},
    {	"intangible",		FORM_INTANGIBLE,	TRUE	},
    {	"biped",		FORM_BIPED,		TRUE	},
    {	"centaur",		FORM_CENTAUR,		TRUE	},
    {	"insect",		FORM_INSECT,		TRUE	},
    {	"spider",		FORM_SPIDER,		TRUE	},
    {	"crustacean",		FORM_CRUSTACEAN,	TRUE	},
    {	"worm",			FORM_WORM,		TRUE	},
    {	"blob",			FORM_BLOB,		TRUE	},
    {	"mammal",		FORM_MAMMAL,		TRUE	},
    {	"bird",			FORM_BIRD,		TRUE	},
    {	"reptile",		FORM_REPTILE,		TRUE	},
    {	"snake",		FORM_SNAKE,		TRUE	},
    {	"dragon",		FORM_DRAGON,		TRUE	},
    {	"amphibian",		FORM_AMPHIBIAN,		TRUE	},
    {	"fish",			FORM_FISH ,		TRUE	},
    {	"cold_blood",		FORM_COLD_BLOOD,	TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type furniture_flags[] =
{
    {	"stand_at",		STAND_AT,	TRUE	},
    {	"stand_on",		STAND_ON,	TRUE	},
    {	"stand_in",		STAND_IN,	TRUE	},
    {	"sit_at",		SIT_AT,		TRUE	},
    {	"sit_on",		SIT_ON,		TRUE	},
    {	"sit_in",		SIT_IN,		TRUE	},
    {	"rest_at",		REST_AT,	TRUE	},
    {	"rest_on",		REST_ON,	TRUE	},
    {	"rest_in",		REST_IN,	TRUE	},
    {	"sleep_at",		SLEEP_AT,	TRUE	},
    {	"sleep_on",		SLEEP_ON,	TRUE	},
    {	"sleep_in",		SLEEP_IN,	TRUE	},
    {	"put_at",		PUT_AT,		TRUE	},
    {	"put_on",		PUT_ON,		TRUE	},
    {	"put_in",		PUT_IN,		TRUE	},
    {	"put_inside",		PUT_INSIDE,	TRUE	},
    {	NULL,			0,		0	}
};

const struct flag_type herb_flags[] =
{
    {	"eaten",		POISONED_EATEN,	 TRUE	},
    {	"rubbed",		POISONED_RUBBED, TRUE	},
    {	"brewed",		POISONED_BREWED, TRUE	},
    {	NULL,			0,		 0	}
};

const struct flag_type imm_flags[] =
{
    {	"summon",		IMM_SUMMON,	TRUE	},
    {	"charm",		IMM_CHARM,	TRUE	},
    {	"magic",		IMM_MAGIC,	TRUE	},
    {	"weapon",		IMM_WEAPON,	TRUE	},
    {	"bash",			IMM_BASH,	TRUE	},
    {	"pierce",		IMM_PIERCE,	TRUE	},
    {	"slash",		IMM_SLASH,	TRUE	},
    {	"fire",			IMM_FIRE,	TRUE	},
    {	"cold",			IMM_COLD,	TRUE	},
    {	"lightning",		IMM_LIGHTNING,	TRUE	},
    {	"acid",			IMM_ACID,	TRUE	},
    {	"poison",		IMM_POISON,	TRUE	},
    {	"negative",		IMM_NEGATIVE,	TRUE	},
    {	"holy",			IMM_HOLY,	TRUE	},
    {	"energy",		IMM_ENERGY,	TRUE	},
    {	"mental",		IMM_MENTAL,	TRUE	},
    {	"disease",		IMM_DISEASE,	TRUE	},
    {	"drowning",		IMM_DROWNING,	TRUE	},
    {	"light",		IMM_LIGHT,	TRUE	},
    {	"sound",		IMM_SOUND,	TRUE	},
    {	"wood",			IMM_WOOD,	TRUE	},
    {	"silver",		IMM_SILVER,	TRUE	},
    {	"iron",			IMM_IRON,	TRUE	},
    {	NULL,			0,	0	}
};

const struct flag_type		invoke_types	[]	=
{
    {	"none",		INVOKE_NONE,	TRUE	},
    {	"mobile",	INVOKE_MOB,	TRUE	},
    {	"object",	INVOKE_OBJ,	TRUE	},
    {	"transfer",	INVOKE_TRANS,	TRUE	},
    {	"morph",	INVOKE_MORPH,	TRUE	},
    {	"spell",	INVOKE_SPELL,	TRUE	},
    {	NULL,		0,		0	}
};

/* item type list */
const struct flag_type		item_types	[]	=
{
    {	"light",	ITEM_LIGHT,	TRUE	},
    {	"scroll",	ITEM_SCROLL,	TRUE	},
    {	"wand",		ITEM_WAND,	TRUE	},
    {	"staff",	ITEM_STAFF,	TRUE	},
    {	"weapon",	ITEM_WEAPON,	TRUE	},
    {	"treasure",	ITEM_TREASURE,	TRUE	},
    {	"armor",	ITEM_ARMOR,	TRUE	},
    {	"potion",	ITEM_POTION,	TRUE	},
    {	"clothing",	ITEM_CLOTHING,	TRUE	},
    {	"furniture",	ITEM_FURNITURE,	TRUE	},
    {	"trash",	ITEM_TRASH,	TRUE	},
    {	"container",	ITEM_CONTAINER,	TRUE	},
    {	"drink",	ITEM_DRINK_CON, TRUE	},
    {	"key",		ITEM_KEY,	TRUE	},
    {	"food",		ITEM_FOOD,	TRUE	},
    {	"money",	ITEM_MONEY,	TRUE	},
    {	"boat",		ITEM_BOAT,	FALSE	},
    {	"npc_corpse",	ITEM_CORPSE_NPC,FALSE	},
    {	"pc_corpse",	ITEM_CORPSE_PC,	FALSE	},
    {	"fountain",	ITEM_FOUNTAIN,	TRUE	},
    {	"pill",		ITEM_PILL,	TRUE	},
/*  {	"protect",	ITEM_PROTECT,	TRUE	}, */
    {	"map",		ITEM_MAP,	TRUE	},
    {	"portal",	ITEM_PORTAL,	TRUE	},
    {	"warp_stone",	ITEM_WARP_STONE,TRUE	},
/*  {	"room_key",	ITEM_ROOM_KEY,	TRUE	}, */
    {	"gem",		ITEM_GEM,	TRUE	},
    {	"jewelry",	ITEM_JEWELRY,	TRUE	},
    {	"lens",		ITEM_LENS,	TRUE	},
    {	"questitem",	ITEM_QUESTITEM,	TRUE	},
    {	"herb",		ITEM_HERB,	TRUE	},
    {	"berry",	ITEM_BERRY,	FALSE	},
    {	"noteboard",	ITEM_NOTEBOARD,	TRUE	},
    {	"paper",	ITEM_PAPER,	TRUE	},
    {	"ink",		ITEM_INK,	TRUE	},
    {	"permkey",	ITEM_PERMKEY,	TRUE	},
    {	"quiver",	ITEM_QUIVER,	TRUE	},
    {	"arrow",	ITEM_ARROW,	FALSE	},
    {	"book",		ITEM_BOOK,	TRUE	},
    {	"sheath",	ITEM_SHEATH,	TRUE	},
    {   "token",    ITEM_TOKEN,     TRUE},
    {	NULL,		0,		0	}
};

const struct flag_type  kingdom_types[] =
{
    {   "Outlying",     KINGDOM_NONE,           TRUE    },
    {   "Blackmere",      KINGDOM_BLACKMERE,        TRUE    },
    {   "Evervale",       KINGDOM_EVERVALE,         TRUE    },
    {   "Wintergarde",      KINGDOM_WINTERGARDE,        TRUE    },
    {   "Druuna",  KINGDOM_DRUUNA,         TRUE    },
    {   "Amberfall",       KINGDOM_AMBERFALL,         TRUE    },
    {   "Tinkerwick",       KINGDOM_TINKERWICK,         TRUE    },
    {   "Ghostwallow",    KINGDOM_GHOSTWALLOW,       TRUE    },
    {   "none",         KINGDOM_NONE,           TRUE    },
    {   NULL,           0,                      0       }
};
const struct flag_type level_types[] =
{
    {	"seraphim",	SERAPHIM,	TRUE	},	/* Implementor */
    {	"cherubim",	CHERUBIM,	TRUE	},
    {	"throne",	THRONE,		TRUE	},
    {	"dominion",	DOMINION,	TRUE	},
    {	"powers",	POWERS,		TRUE	},
    {	"virtues",	VIRTUES,	TRUE	},
    {	"archangel",	ARCHANGEL,	TRUE	},
    {	"angel",	ANGEL,		TRUE	},
    {	"avatar",	AVATAR,		TRUE	},
    {	"visitor",	VISITOR,	TRUE	},
    {	"hero",		HERO,		TRUE	},
    {	NULL,		0,		0	}
};

const struct flag_type log_types[] =
{
    {	"always",	LOG_ALWAYS,	TRUE	},
    {	"args",		LOG_ARGS,	TRUE	},
    {	"build",	LOG_BUILD,	TRUE	},
    {	"never",	LOG_NEVER,	TRUE	},
    {	"normal",	LOG_NORMAL,	TRUE	},
    {	NULL,		0,		0	}
};

const struct flag_type material_types[] =
{
    {   "none",         0,	    	TRUE    },
    {	"amber",	MAT_AMBER,	TRUE	},
    {	"bloodstone",	MAT_BLOODSTONE,	TRUE	},
    {	"bone",		MAT_BONE,	TRUE	},
    {   "brass",	MAT_BRASS,	TRUE    },
    {	"bread",	MAT_BREAD,	TRUE	},
    {	"bronze",	MAT_BRONZE,	TRUE	},
    {	"charcoal",	MAT_CHARCOAL,	TRUE	},
    {	"chitin",	MAT_CHITIN,	TRUE	},
    {	"clay",		MAT_CLAY,	TRUE	},
    {	"cloth",	MAT_CLOTH,	TRUE	},
    {	"coal",		MAT_COAL,	TRUE	},
    {	"copper",	MAT_COPPER,	TRUE	},
    {	"crystal",	MAT_CRYSTAL,	TRUE	},
    {	"diamond",	MAT_DIAMOND,	TRUE	},
    {	"dirt",		MAT_DIRT,	TRUE	},
    {	"dragonscale",	MAT_DRAGONSCALE,TRUE	},
    {	"ebony",	MAT_EBONY,	TRUE	},
    {	"emerald",	MAT_EMERALD,	TRUE	},
    {   "energy",	MAT_ENERGY,	TRUE    },
    {	"feather",	MAT_FEATHER,	TRUE	},
    {	"fiber",	MAT_FIBER,	TRUE	},
    {	"fishscale",	MAT_FISHSCALE,	TRUE	},
    {	"flax",		MAT_FLAX,	TRUE	},
    {	"flesh",	MAT_FLESH,	TRUE	},
    {	"food",		MAT_FOOD,	TRUE	},
    {	"fur",		MAT_FUR,	TRUE	},
    {	"gemstone",	MAT_GEMSTONE,	TRUE	},
    {	"glass",	MAT_GLASS,	TRUE	},
    {	"gold",		MAT_GOLD,	TRUE	},
    {	"granite",	MAT_GRANITE,	TRUE	},
    {	"guano",	MAT_GUANO,	TRUE	},
    {	"hemp",		MAT_HEMP,	TRUE	},
    {	"iron",		MAT_IRON,	TRUE	},
    {	"ivory",	MAT_IVORY,	TRUE	},
    {	"jade",		MAT_JADE,	TRUE	},
    {	"lead",		MAT_LEAD,	TRUE	},
    {	"leather",	MAT_LEATHER,	TRUE	},
    {	"marble",	MAT_MARBLE,	TRUE	},
    {	"mercury",	MAT_MERCURY,	TRUE	},
    {	"obsidian",	MAT_OBSIDIAN,	TRUE	},
    {	"oil",		MAT_OIL,	TRUE	},
    {	"paper",	MAT_PAPER,	TRUE	},
    {	"parchment",	MAT_PARCHMENT,	TRUE	},
    {	"pearl",	MAT_PEARL,	TRUE	},
    {	"platinum",	MAT_PLATINUM,	TRUE	},
    {	"porcelain",	MAT_PORCELAIN,	TRUE	},
    {	"ruby",		MAT_RUBY,	TRUE	},
    {	"rust",		MAT_RUST,	TRUE	},
    {	"salt",		MAT_SALT,	TRUE	},
    {	"sand",		MAT_SAND,	TRUE	},
    {	"sapphire",	MAT_SAPPHIRE,	TRUE	},
    {	"shell",	MAT_SHELL,	TRUE	},
    {	"silk",		MAT_SILK,	TRUE	},
    {	"silver",	MAT_SILVER,	TRUE	},
    {	"sinew",	MAT_SINEW,	TRUE	},
    {	"skin",		MAT_SKIN,	TRUE	},
    {   "steel",        MAT_STEEL,      TRUE    },
    {   "stone",	MAT_STONE,	TRUE    },
    {	"straw",	MAT_STRAW,	TRUE	},
    {	"sulfur",	MAT_SULFUR,	TRUE	},
    {	"tin",		MAT_TIN,	TRUE	},
    {	"vegetable",	MAT_VEGETABLE,	TRUE	},
    {	"velvet",	MAT_VELVET,	TRUE	},
    {	"water",	MAT_WATER,	TRUE	},
    {	"wax",		MAT_WAX,	TRUE	},
    {	"wood",		MAT_WOOD,	TRUE	},
    {	"wool",		MAT_WOOL,	TRUE	},
    {	"zinc",		MAT_ZINC,	TRUE	},
    {   NULL,            0,             0       }
};

const struct flag_type mprog_types[] =
{
    {	"act_prog",		MP_ACT_PROG,		TRUE	},
    {	"all_greet_prog",	MP_ALL_GREET_PROG,	TRUE	},
    {	"bribe_prog",		MP_BRIBE_PROG,		TRUE	},
    {	"death_prog",		MP_DEATH_PROG,		TRUE	},
    {	"buy_prog",		MP_BUY_PROG,		TRUE	},
    {	"entry_prog",		MP_ENTRY_PROG,		TRUE	},
    {	"fight_prog",		MP_FIGHT_PROG,		TRUE	},
    {	"give_prog",		MP_GIVE_PROG,		TRUE	},
    {	"greet_prog",		MP_GREET_PROG,		TRUE	},
    {	"hitprcnt_prog",	MP_HITPRCNT_PROG,	TRUE	},
    {	"list_prog",		MP_LIST_PROG,		TRUE	},
    {	"rand_prog",		MP_RAND_PROG,		TRUE	},
    {	"sell_prog",		MP_SELL_PROG,		TRUE	},
    {	"speech_prog",		MP_SPEECH_PROG,		TRUE	},
    {	"tell_prog",		MP_TELL_PROG,		TRUE	},
    {	"time_prog",		MP_TIME_PROG,		TRUE	},
    {	"repop_prog",		MP_REPOP_PROG,		TRUE	},
    {	"whisper_prog",		MP_WHISPER_PROG,	TRUE	},
    {	NULL,			0,			FALSE	}
};

const struct flag_type off_flags[] =
{
    {	"area_attack",		OFF_AREA_ATTACK,	TRUE	},
    {	"backstab",		OFF_BACKSTAB,		TRUE	},
    {	"bash",			OFF_BASH,		TRUE	},
    {	"berserk",		OFF_BERSERK,		TRUE	},
    {	"disarm",		OFF_DISARM,		TRUE	},
    {	"dodge",		OFF_DODGE,		TRUE	},
    {	"fade",			OFF_FADE,		TRUE	},
    {	"fast",			OFF_FAST,		TRUE	},
    {	"kick",			OFF_KICK,		TRUE	},
    {	"dirt_kick",		OFF_KICK_DIRT,		TRUE	},
    {	"parry",		OFF_PARRY,		TRUE	},
    {	"rescue",		OFF_RESCUE,		TRUE	},
    {	"tail",			OFF_TAIL,		TRUE	},
    {	"trip",			OFF_TRIP,		TRUE	},
    {	"crush",		OFF_CRUSH,		TRUE	},
    {	"assist_all",		ASSIST_ALL,		TRUE	},
    {	"assist_align",		ASSIST_ALIGN,		TRUE	},
    {	"assist_race",		ASSIST_RACE,		TRUE	},
    {	"assist_players",	ASSIST_PLAYERS,		TRUE	},
    {	"assist_guard",		ASSIST_GUARD,		TRUE	},
    {	"assist_vnum",		ASSIST_VNUM,		TRUE	},
    {	NULL,			0,		0	}
};

const struct flag_type old_level_types[] =
{
    {	"implementor",	IMPLEMENTOR,	TRUE	},
    {	"creator",	CREATOR,	TRUE	},
    {	"supreme",	SUPREME,	TRUE	},
    {	"deity",	DEITY,		TRUE	},
    {	"god",		GOD,		TRUE	},
    {	"immortal",	IMMORTAL,	TRUE	},
    {	"demi",		DEMI,		TRUE	},
    {	"angel",	ANGEL,		TRUE	},
    {	"avatar",	AVATAR,		TRUE	},
    {	"visitor",	VISITOR,	TRUE	},
    {	"hero",		HERO,		TRUE	},
    {	NULL,		0,		0	}
};

const struct flag_type oprog_types[] =
{
    {   "error_prog",		OBJ_PROG_ERROR,		FALSE	},
    {   "buy_prog",		OBJ_PROG_BUY,		TRUE	},
    {   "cast_prog",		OBJ_PROG_CAST,		TRUE	},
    {   "cast_sn_prog",		OBJ_PROG_CAST_SN,	TRUE	},
    {   "close_prog",		OBJ_PROG_CLOSE,		TRUE	},
    {   "drop_prog",		OBJ_PROG_DROP,		TRUE	},
    {   "fill_prog",		OBJ_PROG_FILL,		TRUE	},
    {   "get_prog",		OBJ_PROG_GET,		TRUE	},
    {   "get_from_prog",	OBJ_PROG_GET_FROM,	TRUE	},
    {   "give_prog",		OBJ_PROG_GIVE,		TRUE	},
    {   "invoke_prog",		OBJ_PROG_INVOKE,	TRUE	},
    {   "join_prog",		OBJ_PROG_JOIN,		TRUE	},
    {   "lock_prog",		OBJ_PROG_LOCK,		TRUE	},
    {   "look_prog",		OBJ_PROG_LOOK,		TRUE	},
    {   "look_in_prog",		OBJ_PROG_LOOK_IN,	TRUE	},
    {   "open_prog",		OBJ_PROG_OPEN,		TRUE	},
    {   "pick_prog",		OBJ_PROG_PICK,		TRUE	},
    {   "put_prog",		OBJ_PROG_PUT,		TRUE	},
    {   "rand_prog",		OBJ_PROG_RANDOM,	TRUE	},
    {	"read_prog",		OBJ_PROG_READ,		TRUE	},
    {	"remove_prog",		OBJ_PROG_REMOVE,	TRUE	},
    {   "retrieve_prog",	OBJ_PROG_RETRIEVE,	TRUE	},
    {   "sell_prog",		OBJ_PROG_SELL,		TRUE	},
    {   "separate_prog",	OBJ_PROG_SEPARATE,	TRUE	},
    {   "store_prog",		OBJ_PROG_STORE,		TRUE	},
    {   "throw_prog",		OBJ_PROG_THROW,		TRUE	},
    {   "unlock_prog",		OBJ_PROG_UNLOCK,	TRUE	},
    {	"unuse_prog",		OBJ_PROG_UNUSE,		TRUE	},
    {   "use_prog",		OBJ_PROG_USE,		TRUE	},
    {   "wear_prog",		OBJ_PROG_WEAR,		TRUE	},
    {   NULL,			0,			0	}
};

const struct flag_type part_flags[] =
{
    {	"head",			PART_HEAD,		TRUE	},
    {	"arms",			PART_ARMS,		TRUE	},
    {	"legs",			PART_LEGS,		TRUE	},
    {	"heart",		PART_HEART,		TRUE	},
    {	"brains",		PART_BRAINS,		TRUE	},
    {	"guts",			PART_GUTS,		TRUE	},
    {	"hands",		PART_HANDS,		TRUE	},
    {	"feet",			PART_FEET,		TRUE	},
    {	"fingers",		PART_FINGERS,		TRUE	},
    {	"ear",			PART_EAR,		TRUE	},
    {	"eye",			PART_EYE,		TRUE	},
    {	"long_tongue",		PART_LONG_TONGUE,	TRUE	},
    {	"eyestalks",		PART_EYESTALKS,		TRUE	},
    {	"tentacles",		PART_TENTACLES,		TRUE	},
    {	"fins",			PART_FINS,		TRUE	},
    {	"wings",		PART_WINGS,		TRUE	},
    {	"tail",			PART_TAIL,		TRUE	},
    {	"claws",		PART_CLAWS,		TRUE	},
    {	"fangs",		PART_FANGS,		TRUE	},
    {	"horns",		PART_HORNS,		TRUE	},
    {	"scales",		PART_SCALES,		TRUE	},
    {	"tusks",		PART_TUSKS,		TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type plr_flags[] =
{
    {	"npc",			A,	FALSE	},
    {	"autoassist",		C,	FALSE	},
    {	"autoexit",		D,	FALSE	},
    {	"autoloot",		E,	FALSE	},
    {	"autosac",		F,	FALSE	},
    {	"autogold",		G,	FALSE	},
    {	"autosplit",		H,	FALSE	},
    {	"holylight",		N,	FALSE	},
    {	"can_loot",		P,	FALSE	},
    {	"nosummon",		Q,	FALSE	},
    {	"nofollow",		R,	FALSE	},
    {   "autodamagedealt",      S,      FALSE   },
    {	"permit",		U,	TRUE	},
    {	"log",			W,	FALSE	},
    {	"deny",			X,	FALSE	},
    {	"freeze",		Y,	FALSE	},
    {	"thief",		Z,	FALSE	},
    {	"killer",		aa,	FALSE	},
    {	"noemote",		bb,	FALSE	},
    {	"questor",		cc,	FALSE	},
    {   "war",                  dd,     TRUE    },
    {	NULL,			0,	0	}
};

const struct flag_type portal_flags[] =
{
    {	"buggy",	GATE_BUGGY,		TRUE	},
    {	"closeable",	GATE_CLOSEABLE,		TRUE	},
    {	"closed",	GATE_CLOSED,		TRUE	},
    {	"exit",		GATE_NORMAL_EXIT,	TRUE	},
    {	"gowith",	GATE_GOWITH,		TRUE	},
    {	"locked",	GATE_LOCKED,		TRUE	},
    {	"nocurse",	GATE_NOCURSE,		TRUE	},
    {	"opaque",	GATE_OPAQUE,		TRUE	},
    {	"pickproof",	GATE_PICKPROOF,		TRUE	},
    {	"random",	GATE_RANDOM,		TRUE	},
    {	"window",	GATE_WINDOW,		TRUE	},
    {   NULL,		0,		0	}
};

const struct flag_type position_types[] =
{
    {	"dead",		POS_DEAD,	FALSE	},
    {	"mortal",	POS_MORTAL,	FALSE	},
    {	"incap",	POS_INCAP,	FALSE	},
    {	"stunned",	POS_STUNNED,	FALSE	},
    {	"sleeping",	POS_SLEEPING,	TRUE	},
    {	"resting",	POS_RESTING,	TRUE	},
    {	"sitting",	POS_SITTING,	TRUE	},
    {	"fighting",	POS_FIGHTING,	FALSE	},
    {	"standing",	POS_STANDING,	TRUE	},
    {   NULL,		0,		0	}
};

const struct flag_type priority_flags[] =
{
    {	"Critical",		PRI_CRITICAL,		TRUE	},
    {	"High",			PRI_HIGH,		TRUE	},
    {	"Medium",		PRI_MEDIUM,		TRUE	},
    {	"Low",			PRI_LOW,		TRUE	},
    {	"",			0,			0	}
};

const struct flag_type rdesc_types[] =
{
    {	"desc",		DESC_DESC,	TRUE	},
    {	"morning",	DESC_MORN,	TRUE	},
    {	"evening",	DESC_EVEN,	TRUE	},
    {	"night",	DESC_NIGHT,	TRUE	},
    {	"",		0,		0	}
};

const struct flag_type res_flags[] =
{
    {	"summon",	RES_SUMMON,	TRUE	},
    {	"charm",	RES_CHARM,	TRUE	},
    {	"magic",	RES_MAGIC,	TRUE	},
    {	"weapon",	RES_WEAPON,	TRUE	},
    {	"bash",		RES_BASH,	TRUE	},
    {	"pierce",	RES_PIERCE,	TRUE	},
    {	"slash",	RES_SLASH,	TRUE	},
    {	"fire",		RES_FIRE,	TRUE	},
    {	"cold",		RES_COLD,	TRUE	},
    {	"lightning",	RES_LIGHTNING,	TRUE	},
    {	"acid",		RES_ACID,	TRUE	},
    {	"poison",	RES_POISON,	TRUE	},
    {	"negative",	RES_NEGATIVE,	TRUE	},
    {	"holy",		RES_HOLY,	TRUE	},
    {	"energy",	RES_ENERGY,	TRUE	},
    {	"mental",	RES_MENTAL,	TRUE	},
    {	"disease",	RES_DISEASE,	TRUE	},
    {	"drowning",	RES_DROWNING,	TRUE	},
    {	"light",	RES_LIGHT,	TRUE	},
    {	"sound",	RES_SOUND,	TRUE	},
    {	"wood",		RES_WOOD,	TRUE	},
    {	"silver",	RES_SILVER,	TRUE	},
    {	"iron",		RES_IRON,	TRUE	},
    {   NULL,		0,		0	}
};

const struct flag_type room_aff_flags[] =
{
    {	"none",		ROOM_AFF_NONE,		TRUE	},
    {	"blind",	ROOM_AFF_BLIND,		TRUE	},
    {	"curse",	ROOM_AFF_CURSE,		TRUE	},
    {	"plague",	ROOM_AFF_PLAGUE,	TRUE	},
    {	"poison",	ROOM_AFF_POISON,	TRUE	},
    {	"sleep",	ROOM_AFF_SLEEP,		TRUE	},
    {	"slow",		ROOM_AFF_SLOW,		TRUE	},
    {	"weaken",	ROOM_AFF_WEAKEN,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type room_flags[] =
{
    {	"bank",		ROOM_BANK,		TRUE	},
    {	"brawl",	ROOM_BRAWL,		TRUE	},
    {	"dark",		ROOM_DARK,		TRUE	},
    {	"gods_only",	ROOM_GODS_ONLY,		TRUE	},
    {	"heroes_only",	ROOM_HEROES_ONLY,	TRUE	},
    {	"imp_only",	ROOM_IMP_ONLY,		TRUE	},
    {	"indoors",	ROOM_INDOORS,		TRUE	},
    {	"law",		ROOM_LAW,		TRUE	},
    {	"newbies_only",	ROOM_NEWBIES_ONLY,	TRUE	},
    {	"no_floor",	ROOM_NO_FLOOR,		TRUE	},
    {	"no_format",	ROOM_NO_FORMAT,		TRUE	},
    {	"no_in",	ROOM_NO_IN,		TRUE	},
    {	"no_magic",	ROOM_NO_MAGIC,		TRUE	},
    {	"no_mob",	ROOM_NO_MOB,		TRUE	},
    {	"no_mount",	ROOM_NO_MOUNT,		TRUE	},
    {	"no_out",	ROOM_NO_OUT,		TRUE	},
    {	"no_recall",	ROOM_NO_RECALL,		TRUE	},
    {	"no_resetmsg",	ROOM_NO_RESETMSG,	TRUE	},
    {	"noscan",	ROOM_NOSCAN,		TRUE	},
    {	"nowhere",	ROOM_NOWHERE,		TRUE	},
    {	"pet_shop",	ROOM_PET_SHOP,		TRUE	},
    {	"private",	ROOM_PRIVATE,		TRUE	},
    {	"safe",		ROOM_SAFE,		TRUE	},
    {	"solitary",	ROOM_SOLITARY,		TRUE	},
    {	"silent",	ROOM_SILENT,		TRUE	},
    {	"sunlight",	ROOM_SUNLIGHT,		TRUE	},
    {	"unfinished",	ROOM_UNFINISHED,	TRUE	},
    {	"virtual",	ROOM_VIRTUAL,		FALSE	},
    {	NULL,		0,			0	}
};

const struct flag_type rprog_types[] =
{
    {   "error_prog",	ROOM_PROG_ERROR,	FALSE 	},
    {	"arrive_prog",	ROOM_PROG_ARRIVE,	TRUE	},
    {	"boot_prog",	ROOM_PROG_BOOT,		TRUE	},
    {   "cast_prog",	ROOM_PROG_CAST,		TRUE	},
    {   "cast_sn_prog",	ROOM_PROG_CAST_SN,	TRUE	},
    {   "death_prog",	ROOM_PROG_DEATH,	TRUE	},
    {	"depart_prog",	ROOM_PROG_DEPART,	TRUE	},
    {   "enter_prog",	ROOM_PROG_ENTER,	TRUE	},
    {   "exit_prog",	ROOM_PROG_EXIT,		TRUE	},
    {	"interpret_prog", ROOM_PROG_INTERP,	TRUE	},
    {	"listen_prog",	ROOM_PROG_LISTEN,	TRUE	},
    {	"look_prog",	ROOM_PROG_LOOK,		TRUE	},
    {   "pass_prog",	ROOM_PROG_PASS,		TRUE	},
    {	"pull_prog",	ROOM_PROG_PULL,		TRUE	},
    {	"push_prog",	ROOM_PROG_PUSH,		TRUE	},
    {   "rand_prog",	ROOM_PROG_RANDOM,	TRUE	},
    {   "rest_prog",	ROOM_PROG_REST,		TRUE	},
    {	"sit_prog",	ROOM_PROG_SIT,		TRUE	},
    {   "sleep_prog",	ROOM_PROG_SLEEP,	TRUE	},
    {	"smell_prog",	ROOM_PROG_SMELL,	TRUE	},
    {	"speech_prog",	ROOM_PROG_SPEECH,	TRUE	},
    {	"stand_prog",	ROOM_PROG_STAND,	TRUE	},
    {   "time_prog",	ROOM_PROG_TIME,		TRUE	},
    {   "wake_prog",	ROOM_PROG_WAKE,		TRUE	},
    {   NULL,		0,			0   	}
};

const struct flag_type sector_types[] =
{
    {	"inside",	SECT_INSIDE,		TRUE	},
    {	"city",		SECT_CITY,		TRUE	},
    {	"field",	SECT_FIELD,		TRUE	},
    {	"forest",	SECT_FOREST,		TRUE	},
    {	"hills",	SECT_HILLS,		TRUE	},
    {	"mountain",	SECT_MOUNTAIN,		TRUE	},
    {	"swim",		SECT_WATER_SWIM,	TRUE	},
    {	"noswim",	SECT_WATER_NOSWIM,	TRUE	},
    {	"underwater",	SECT_UNDERWATER,	TRUE	},
    {	"air",		SECT_AIR,		TRUE	},
    {	"desert",	SECT_DESERT,		TRUE	},
    {	"swamp",	SECT_SWAMP,		TRUE	},
    {	"road",		SECT_ROAD,		TRUE	},
    {	"ocean",	SECT_OCEAN,		TRUE	},
    {	"underground",	SECT_UNDERGROUND,	TRUE	},
    {	"graveyard",	SECT_GRAVEYARD,		TRUE	},
    {	"treecity",	SECT_TREECITY,		TRUE	},
    {	"elfforest",	SECT_ELFFOREST,		TRUE	},
    {	"jungle",	SECT_JUNGLE,		TRUE	},
    {	"arctic",	SECT_ARCTIC,		TRUE	},
    {	"beach",	SECT_BEACH,		TRUE	},
    {	"boiling",	SECT_BOILING,		TRUE	},
    {	"lake",		SECT_LAKE,		TRUE	},
    {	"river",	SECT_RIVER,		TRUE	},
    {	"wasteland",	SECT_WASTELAND,		TRUE	},
    {	NULL,		0,			0	}
};

      struct flag_type	security_flags[33] =
{
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	"",		0,			FALSE	},
    {	NULL,		0,			0	}
};

/* for sex */
const struct flag_type sex_types[] =
{
   {	"neutral",	SEX_NEUTRAL,	TRUE	},
   {	"male",		SEX_MALE,	TRUE	},
   {	"female",	SEX_FEMALE,	TRUE	},
   {	"either",	SEX_EITHER,	TRUE	},
   {	NULL,		0,		0	}
};

const struct flag_type sheath_flags[] =
{
    {	"conceals",	SHEATH_CONCEAL,	TRUE },
    {	NULL,		0,		0    }
};

const struct flag_type shield_types[] =
{
    {	"acid",			SHLD_ACID,		TRUE	},
     {	"black_mantle",		SHLD_BLACK_MANTLE,		TRUE	},
    {	"blade",		SHLD_BLADE,		TRUE	},
    {	"flame_shield",		SHLD_FLAME,		TRUE	},
    {	"force_shield",		SHLD_FORCE,		TRUE	},
    {	"ghost_shield",		SHLD_GHOST,		TRUE	},
    {	"holy_shield",		SHLD_HOLY,		TRUE	},
    {	"ice_shield",		SHLD_ICE,		TRUE	},
    {	"light_shield",		SHLD_LIGHT,		TRUE	},
    {	"protect_acid",		SHLD_PROTECT_ACID,	TRUE	},
    {	"protect_air",		SHLD_PROTECT_AIR,	TRUE	},
    {	"protect_cold",		SHLD_PROTECT_COLD,	TRUE	},
    {	"protect_earth",	SHLD_PROTECT_EARTH,	TRUE	},
    {	"protect_energy",	SHLD_PROTECT_ENERGY,	TRUE	},
    {	"protect_evil",		SHLD_PROTECT_EVIL,	TRUE	},
    {	"protect_fire",		SHLD_PROTECT_FIRE,	TRUE	},
    {	"protect_good",		SHLD_PROTECT_GOOD,	TRUE	},
    {	"protect_lightning",	SHLD_PROTECT_LIGHTNING,	TRUE	},
    {	"protect_water",	SHLD_PROTECT_WATER,	TRUE	},
    {	"sanctuary",		SHLD_SANCTUARY,		TRUE	},
    {	"sonic_shield",		SHLD_SONIC,		TRUE	},
    {	"static_shield",	SHLD_SONIC,		TRUE	},
    {	"unholy_shield",	SHLD_UNHOLY,		TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type size_types[] =
{
    {	"tiny",		SIZE_TINY,	TRUE	},
    {	"small",	SIZE_SMALL,	TRUE	},
    {	"medium",	SIZE_MEDIUM,	TRUE	},
    {	"large",	SIZE_LARGE,	TRUE	},
    {	"huge",		SIZE_HUGE,	TRUE	},
    {	"giant",	SIZE_GIANT,	TRUE	},
    {	"titanic",	SIZE_TITANIC,	TRUE	},
    {	NULL,		0,		0	},
};

const struct flag_type stat_types[] =
{
    {	"str",		STAT_STR,	TRUE	},
    {	"int",		STAT_INT,	TRUE	},
    {	"wis",		STAT_WIS,	TRUE	},
    {	"dex",		STAT_DEX,	TRUE	},
    {	"con",		STAT_CON,	TRUE	},
    {	NULL,		0,		0	},
};

const struct flag_type spell_flags[] =
{
    {	"nomob",	SPELL_NOMOB,	TRUE	},
    {	"nopc",		SPELL_NOPC,	TRUE	},
    {	"noinvoke",	SPELL_NOINVOKE,	TRUE	},
    {	"noscroll",	SPELL_NOSCROLL,	TRUE	},
    {	"nowand",	SPELL_NOWAND,	TRUE	},
    {	"nostaff",	SPELL_NOSTAFF,	TRUE	},
    {	"nopotion",	SPELL_NOPOTION,	TRUE	},
    {	"nopill",	SPELL_NOPILL,	TRUE	},
    {	"nolens",	SPELL_NOLENS,	TRUE	},
    {	"noherb",	SPELL_NOHERB,	TRUE	},
    {	NULL,		0,		0	},
};

const struct flag_type target_types[] =
{
    {	"ignore",	TAR_IGNORE,		TRUE	},
    {	"offensive",	TAR_CHAR_OFFENSIVE,	TRUE	},
    {	"defensive",	TAR_CHAR_DEFENSIVE,	TRUE	},
    {	"self",		TAR_CHAR_SELF,		TRUE	},
    {	"obj_inv",	TAR_OBJ_INV,		TRUE	},
    {	"obj_defensive",TAR_OBJ_CHAR_DEF,	TRUE	},
    {	"obj_offensive",TAR_OBJ_CHAR_OFF,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type vehicle_types[] =
{
    {	"none",		VEHICLE_NONE,	TRUE	},
    {	"air",		VEHICLE_AIR,	FALSE	},
    {	"amphibious",	VEHICLE_AMPHIB,	TRUE	},
    {	"land",		VEHICLE_LAND,	TRUE	},
    {	"sea",		VEHICLE_SEA,	TRUE	},
    {	NULL,		0,		0	}
};

const struct flag_type vuln_flags[] =
{
    {	"summon",	VULN_SUMMON,	TRUE	},
    {	"charm",	VULN_CHARM,	TRUE	},
    {	"magic",	VULN_MAGIC,	TRUE	},
    {	"weapon",	VULN_WEAPON,	TRUE	},
    {	"bash",		VULN_BASH,	TRUE	},
    {	"pierce",	VULN_PIERCE,	TRUE	},
    {	"slash",	VULN_SLASH,	TRUE	},
    {	"fire",		VULN_FIRE,	TRUE	},
    {	"cold",		VULN_COLD,	TRUE	},
    {	"lightning",	VULN_LIGHTNING,	TRUE	},
    {	"acid",		VULN_ACID,	TRUE	},
    {	"poison",	VULN_POISON,	TRUE	},
    {	"negative",	VULN_NEGATIVE,	TRUE	},
    {	"holy",		VULN_HOLY,	TRUE	},
    {	"energy",	VULN_ENERGY,	TRUE	},
    {	"mental",	VULN_MENTAL,	TRUE	},
    {	"disease",	VULN_DISEASE,	TRUE	},
    {	"drowning",	VULN_DROWNING,	TRUE	},
    {	"light",	VULN_LIGHT,	TRUE	},
    {	"sound",	VULN_SOUND,	TRUE	},
    {	"wood",		VULN_WOOD,	TRUE	},
    {	"silver",	VULN_SILVER,	TRUE	},
    {	"iron",		VULN_IRON,	TRUE	},
    {	NULL,		0,		0	}
};

const struct flag_type weapon_flags[] =
{
    {	"hit",		0,	TRUE	},
    {	"slice",	1,	TRUE	},
    {	"stab",		2,	TRUE	},
    {	"slash",	3,	TRUE	},
    {	"whip",		4,	TRUE	},
    {	"claw",		5,	TRUE	},
    {	"blast",	6,	TRUE	},
    {	"pound",	7,	TRUE	},
    {	"crush",	8,	TRUE	},
    {	"grep",		9,	TRUE	},
    {	"bite",		10,	TRUE	},
    {	"pierce",	11,	TRUE	},
    {	"suction",	12,	TRUE	},
    {	"beating",	13,	TRUE	},  /* ROM */
    {	"digestion",	14,	TRUE	},
    {	"charge",	15,	TRUE	},
    {	"slap",		16,	TRUE	},
    {	"punch",	17,     TRUE	},
    {	"wrath",	18,	TRUE	},
    {	"magic",	19,	TRUE	},
    {	"divine-power",	20,	TRUE	},
    {	"cleave",	21,	TRUE	},
    {	"scratch",	22,	TRUE	},
    {	"peck-pierce",  23,	TRUE	},
    {	"peck-bash",	24,	TRUE	},
    {	"chop",		25,	TRUE	},
    {	"sting",	26,	TRUE	},
    {	"smash",	27,	TRUE	},
    {	"shocking-bite", 28,	TRUE	},
    {	"flaming-bite",	 29,	TRUE	},
    {	"freezing-bite", 30,	TRUE    },
    {	"acidic-bite",	31,	TRUE	},
    {	"chomp",	32,	TRUE	},
    {	NULL,		0,	TRUE	}
};

const struct flag_type weapon_types[] =
{
    {	"exotic",	WEAPON_EXOTIC,		TRUE	},
    {	"sword",	WEAPON_SWORD,		FALSE	},
    {	"club",		WEAPON_CLUB,		TRUE	},
    {	"dagger",	WEAPON_DAGGER,		TRUE	},
    {	"spear",	WEAPON_SPEAR,		TRUE	},
    {	"mace",		WEAPON_MACE,		TRUE	},
    {	"axe",		WEAPON_AXE,		TRUE	},
    {	"flail",	WEAPON_FLAIL,		TRUE	},
    {	"whip",		WEAPON_WHIP,		TRUE	},
    {	"polearm",	WEAPON_POLEARM,		TRUE	},
    {	"hammer",	WEAPON_HAMMER,		TRUE	},
    {	"longsword",	WEAPON_LONGSWORD,	TRUE	},
    {	"staff",	WEAPON_STAFF,		TRUE	},
    {	"stiletto",	WEAPON_STILETTO,	TRUE	},
    {	"scimitar",	WEAPON_SCIMITAR,	TRUE	},
    {	"short_sword",	WEAPON_SHORTSWORD,	TRUE	},
    {	"bastard_sword",WEAPON_BASTARDSWORD,	TRUE	},
    {	"twohand_sword",WEAPON_TWOHANDSWORD,	TRUE	},
    {	"dirk",		WEAPON_DIRK,		TRUE	},
    {	"lance",	WEAPON_LANCE,		TRUE	},
    {	"great_mace",	WEAPON_GREATMACE,	TRUE	},
    {	"great_axe",	WEAPON_GREATAXE,	TRUE	},
    {	"horse_mace",	WEAPON_HORSEMACE,	TRUE	},
    {	"horse_flail",	WEAPON_HORSEFLAIL,	TRUE	},
    {	"bow",		WEAPON_BOW,		FALSE	},
    {	"rapier",	WEAPON_RAPIER,		TRUE	},
    {	"sabre",	WEAPON_SABRE,		TRUE	},
    {	"saber",	WEAPON_SABRE,		TRUE	},
    {	"broadsword",	WEAPON_BROADSWORD,	TRUE	},
    {	"crossbow",	WEAPON_CROSSBOW,	FALSE	},
    {	NULL,		0,			0	}
};

const struct flag_type weapon_type_olc[] =
{
    {	"flaming",	WEAPON_FLAMING,		TRUE	},
    {	"frost",	WEAPON_FROST,		TRUE	},
    {	"vampiric",	WEAPON_VAMPIRIC,	TRUE	},
    {	"sharp",	WEAPON_SHARP,		TRUE	},
    {	"vorpal",	WEAPON_VORPAL,		TRUE	},
    {	"two-hands",	WEAPON_TWO_HANDS,	TRUE	},
    {	"shocking",	WEAPON_SHOCKING,	TRUE	},
    {	"poisoned",	WEAPON_POISON,		TRUE	},
    {   "talking",  	WEAPON_TALKING,     	TRUE    },
    {   "talking_mace", WEAPON_TALKING_MACE,   	TRUE	},
    {	"gripped",	WEAPON_GRIPPED,		TRUE	},
    {	"pestilence",	WEAPON_PESTILENCE,	TRUE	},
    {	"powerleech",	WEAPON_POWERLEECH,	TRUE	},
    {	NULL,		0,			0	}
};

const struct flag_type wear_flags[] =
{
    {	"take",			ITEM_TAKE,		TRUE	},
    {	"finger",		ITEM_WEAR_FINGER,	TRUE	},
    {	"neck",			ITEM_WEAR_NECK,		TRUE	},
    {	"body",			ITEM_WEAR_BODY,		TRUE	},
    {	"head",			ITEM_WEAR_HEAD,		TRUE	},
    {	"legs",			ITEM_WEAR_LEGS,		TRUE	},
    {	"feet",			ITEM_WEAR_FEET,		TRUE	},
    {	"hands",		ITEM_WEAR_HANDS,	TRUE	},
    {	"arms",			ITEM_WEAR_ARMS,		TRUE	},
    {	"shield",		ITEM_WEAR_SHIELD,	TRUE	},
    {	"about",		ITEM_WEAR_ABOUT,	TRUE	},
    {	"waist",		ITEM_WEAR_WAIST,	TRUE	},
    {	"wrist",		ITEM_WEAR_WRIST,	TRUE	},
    {	"wield",		ITEM_WIELD,		TRUE	},
    {	"hold",			ITEM_HOLD,		TRUE	},
    {   "eyes",			ITEM_WEAR_CONTACT,      TRUE	},
    {	"face",			ITEM_WEAR_FACE,		TRUE	},
    {   "float",		ITEM_WEAR_FLOAT,        TRUE	},
    {	"ears",			ITEM_WEAR_EAR,		TRUE	},
    {	"cpatch",		ITEM_WEAR_CPATCH,	TRUE	},
    {	"rpatch",		ITEM_WEAR_RPATCH,	TRUE	},
    {	"shoulder",		ITEM_WEAR_SHOULDER,	TRUE	},
    {	"ankle",		ITEM_WEAR_ANKLE,	TRUE	},
    {	"tail", 		ITEM_WEAR_TAIL, 	TRUE	},
     {	"horns", 		ITEM_WEAR_HORNS, 	TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type wear_loc_strings[] =
{
    {	"in the inventory",	WEAR_NONE,	TRUE	},
    {	"as a light",		WEAR_LIGHT,	TRUE	},
    {	"on the left finger",	WEAR_FINGER_L,	TRUE	},
    {	"on the right finger",	WEAR_FINGER_R,	TRUE	},
    {	"around the neck (1)",	WEAR_NECK_1,	TRUE	},
    {	"around the neck (2)",	WEAR_NECK_2,	TRUE	},
    {	"on the body",		WEAR_BODY,	TRUE	},
    {	"over the head",	WEAR_HEAD,	TRUE	},
    {	"on the legs",		WEAR_LEGS,	TRUE	},
    {	"on the feet",		WEAR_FEET,	TRUE	},
    {	"on the hands",		WEAR_HANDS,	TRUE	},
    {	"on the arms",		WEAR_ARMS,	TRUE	},
    {	"as a shield",		WEAR_SHIELD,	TRUE	},
    {	"about the shoulders",	WEAR_ABOUT,	TRUE	},
    {	"around the waist",	WEAR_WAIST,	TRUE	},
    {	"on the left wrist",	WEAR_WRIST_L,	TRUE	},
    {	"on the right wrist",	WEAR_WRIST_R,	TRUE	},
    {	"wielded",		WEAR_WIELD,	TRUE	},
    {	"dual wielded",		WEAR_DUAL,	TRUE	},
    {	"held in the hands",	WEAR_HOLD,	TRUE	},
    {	"in the eyes",		WEAR_EYES,	TRUE	},
    {	"on the face",		WEAR_FACE,	TRUE	},
    {	"on the left ankle",	WEAR_ANKLE_L,	TRUE	},
    {	"on the right ankle",	WEAR_ANKLE_R,	TRUE	},
    {	"on the left ear",	WEAR_EAR_L,	TRUE	},
    {	"on the right ear",	WEAR_EAR_R,	TRUE	},
    {	"over the shoulder",	WEAR_SHOULDER,	TRUE	},
    {	"clan patch",		WEAR_CPATCH,	TRUE	},
    {	"religious symbol",	WEAR_RPATCH,	TRUE	},
    {   "on the tail",          WEAR_TAIL,      TRUE    },
    {   "on the horns",          WEAR_HORNS,      TRUE    },
    {	NULL,			0,		0	}
};

const struct flag_type wear_loc_types[] =
{
    {	"none",		WEAR_NONE,	TRUE	},
    {	"about",	WEAR_ABOUT,	TRUE	},
    {	"arms",		WEAR_ARMS,	TRUE	},
    {	"body",		WEAR_BODY,	TRUE	},
    {   "dual",         WEAR_DUAL,	TRUE    },
    {	"eyes",		WEAR_EYES,	TRUE	},
    {	"feet",		WEAR_FEET,	TRUE	},
    {	"lfinger",	WEAR_FINGER_L,	TRUE	},
    {	"rfinger",	WEAR_FINGER_R,	TRUE	},
    {	"float",	WEAR_FLOAT,	TRUE	},
    {	"hands",	WEAR_HANDS,	TRUE	},
    {	"head",		WEAR_HEAD,	TRUE	},
    {	"hold",		WEAR_HOLD,	TRUE	},
    {	"legs",		WEAR_LEGS,	TRUE	},
    {	"light",	WEAR_LIGHT,	TRUE	},
    {	"neck1",	WEAR_NECK_1,	TRUE	},
    {	"neck2",	WEAR_NECK_2,	TRUE	},
    {	"shield",	WEAR_SHIELD,	TRUE	},
    {	"waist",	WEAR_WAIST,	TRUE	},
    {	"wield",	WEAR_WIELD,	TRUE	},
    {	"lwrist",	WEAR_WRIST_L,	TRUE	},
    {	"rwrist",	WEAR_WRIST_R,	TRUE	},
    {	"lankle",	WEAR_ANKLE_L,	TRUE	},
    {	"rankle",	WEAR_ANKLE_R,	TRUE	},
    {	"lear",		WEAR_EAR_L,	TRUE	},
    {	"rear",		WEAR_EAR_R,	TRUE	},
    {	"shoulder",	WEAR_SHOULDER,	TRUE	},
    {	"face",		WEAR_FACE,	TRUE	},
    {	"cpatch",	WEAR_CPATCH,	TRUE	},
    {	"rpatch",	WEAR_RPATCH,	TRUE	},
    {	"tail",	        WEAR_TAIL,	TRUE	},
     {	"horns",	        WEAR_HORNS,	TRUE	},
    {	NULL,		0,		0	}
};


const struct flag_type windspeed_types[] =
{
    {	"calm",		WIND_CALM,	TRUE	},
    {	"zephyr",	WIND_ZEPHYR,	TRUE	},
    {	"light",	WIND_LIGHT,	TRUE	},
    {	"moderate",	WIND_MODERATE,	TRUE	},
    {	"strong",	WIND_STRONG,	TRUE	},
    {	"gale",		WIND_GALE,	TRUE	},
    {	"hurricane",	WIND_HURRICANE,	TRUE	},
    {	NULL,		0,		0	}
};


const struct flag_type world_types[] =
{
    {	"None",		WORLD_NONE,	TRUE	},
    {	"All",		WORLD_ALL,	TRUE	},
    {	NULL,		0,		0	}
};


