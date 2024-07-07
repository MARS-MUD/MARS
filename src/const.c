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


#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"


/* List of color codes */
const char colorcode_list[] = ".rgObpcwzRGYBPCWX";

/* Direction stuff */
const char *const dir_desc[] =
{
    "to the north",
    "to the east",
    "to the south",
    "to the west",
    "upwards",
    "downwards",
    "to the northwest",
    "to the northeast",
    "to the southwest",
    "to the southeast"
};

char *	const	dir_from[] =
{
    "the south", "the west", "the north", "the east", "below", "above",
    "the southeast", "the southwest", "the northeast", "the northwest"
};

char *	const	dir_letter[] =
{
    "N", "E", "S", "W", "U", "D", "Nw", "Ne", "Sw", "Se"
};

char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down", "northwest", "northeast",
    "southwest", "southeast"
};

int	const	dir_next	[MAX_DIR][2] =
{
    {	DIR_NORTHWEST,	DIR_NORTHEAST	},
    {	DIR_NORTHEAST,	DIR_SOUTHEAST	},
    {	DIR_SOUTHEAST,	DIR_SOUTHWEST	},
    {	DIR_SOUTHWEST,	DIR_NORTHWEST	},
    {	DIR_DOWN,	DIR_DOWN	},
    {	DIR_UP,		DIR_UP		},
    {	DIR_WEST,	DIR_NORTH	},
    {	DIR_NORTH,	DIR_EAST	},
    {	DIR_SOUTH,	DIR_WEST	},
    {	DIR_EAST,	DIR_SOUTH	}
};

const	int	dir_remap	[]	=
{
    DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST, DIR_UP, DIR_DOWN,
    DIR_NORTHWEST, DIR_NORTHEAST, DIR_SOUTHWEST, DIR_SOUTHEAST
};

const	int	rev_dir		[]	=
{
    DIR_SOUTH,		/* opposite of north */
    DIR_WEST,		/* opposite of east */
    DIR_NORTH,		/* opposite of south */
    DIR_EAST,		/* opposite of west */
    DIR_DOWN,		/* opposite of up */
    DIR_UP,		/* opposite of down */
    DIR_SOUTHEAST,	/* opposite of northwest */
    DIR_SOUTHWEST,	/* opposite of northeast */
    DIR_NORTHEAST,	/* Opposite of southwest */
    DIR_NORTHWEST	/* opposite of southeast */
};


/* Gender stuff */
const char * const he_she  [] = { "it",  "he",  "she" };
const char * const him_her [] = { "it",  "him", "her" };
const char * const his_her [] = { "its", "his", "her" };

/* Calendar stuff */
char *	const	day_name	[] =
{
    "the Moon", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun", "the Bull"
};

char *	const	month_name	[] =
{
    "Winter", "the Frost Giant", "the Spring", "Nature",
    "the Sun", "the Heat", "the Grand Struggle", "the Shadows",
    "Futility", "the Dragon",
    "the Battle", "the Dark Shades", 
    "the Long Shadows", "the Ancient Darkness", "the Great Evil",
    "the Winter Wolf", "the Old Forces",
};

char *	const	ordinal		[] =
{
    "zeroeth",	"first", "second", "third", "fourth", "fifth",
    "sixth",	"seventh", "eigth", "ninth", "tenth"
};

char *	const	week_name	[] =
{
    "Darkness", "Tears", "Toil", "the Dragon", "the Shadows",
    "the Battle", "Atonement"
};


/* weapon selection table */
const	struct	weapon_type	weapon_table	[]	=
{
   { "sword",		OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SWORD,		&gsn_sword	   },
   { "mace",		OBJ_VNUM_SCHOOL_MACE,	WEAPON_MACE,		&gsn_mace 	   },
   { "dagger",		OBJ_VNUM_SCHOOL_DAGGER,	WEAPON_DAGGER,		&gsn_dagger	   },
   { "axe",		OBJ_VNUM_SCHOOL_AXE,	WEAPON_AXE,		&gsn_axe	   },
   { "spear",		OBJ_VNUM_SCHOOL_STAFF,	WEAPON_SPEAR,		&gsn_spear	   },
   { "flail",		OBJ_VNUM_SCHOOL_FLAIL,	WEAPON_FLAIL,		&gsn_flail	   },
   { "whip",		OBJ_VNUM_SCHOOL_WHIP,	WEAPON_WHIP,		&gsn_whip	   },
   { "polearm",		OBJ_VNUM_SCHOOL_POLEARM,WEAPON_POLEARM,		&gsn_polearm	   },
   { "hammer",		OBJ_VNUM_SCHOOL_HAMMER, WEAPON_HAMMER,  	&gsn_hammer	   },
   { "club",		OBJ_VNUM_SCHOOL_MACE,	WEAPON_CLUB,		&gsn_club	   },
   { "longsword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_LONGSWORD,	&gsn_longsword	   },
   { "staff",		OBJ_VNUM_SCHOOL_STAFF,	WEAPON_STAFF,		&gsn_staff	   },
   { "stiletto",	OBJ_VNUM_SCHOOL_DAGGER, WEAPON_STILETTO,	&gsn_stiletto	   },
   { "scimitar",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SCIMITAR,	&gsn_scimitar	   },
   { "short_sword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SHORTSWORD,	&gsn_shortsword   },
   { "bastard_sword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_BASTARDSWORD,	&gsn_bastardsword },
   { "twohand_sword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_TWOHANDSWORD,	&gsn_twohandsword },
   { "dirk",		OBJ_VNUM_SCHOOL_DAGGER,	WEAPON_DIRK,		&gsn_dirk	   },
   { "lance",		OBJ_VNUM_SCHOOL_STAFF,	WEAPON_LANCE,		&gsn_lance	   },
   { "great_mace",	OBJ_VNUM_SCHOOL_MACE,	WEAPON_GREATMACE,	&gsn_greatmace	   },
   { "great_axe",	OBJ_VNUM_SCHOOL_AXE,	WEAPON_GREATAXE,	&gsn_greataxe	   },
   { "horse_mace",	OBJ_VNUM_SCHOOL_MACE,	WEAPON_HORSEMACE,	&gsn_horsemace	   },
   { "horse_flail",	OBJ_VNUM_SCHOOL_FLAIL,	WEAPON_HORSEFLAIL,	&gsn_horseflail   },
   { "bow",		OBJ_VNUM_SCHOOL_MACE,	WEAPON_BOW,		&gsn_bow	   },
   { "rapier",		OBJ_VNUM_SCHOOL_SWORD,	WEAPON_RAPIER,		&gsn_rapier	   },
   { "sabre",		OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SABRE,		&gsn_sabre	   },

   { "broadsword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_BROADSWORD,	&gsn_broadsword	   },
   { "crossbow",	OBJ_VNUM_SCHOOL_MACE,	WEAPON_CROSSBOW,	&gsn_crossbow	   },
   { NULL,	0,				0,	NULL		}
};


/*
 * Info channel table 
 */
const	struct wiznet_type	info_table	[] =
{
    {	"on",		INFO_ON,	1	},
    {	"deaths",	INFO_DEATHS,	1	},
    {	"levels",	INFO_LEVELS,	1	},
    {	"logins",	INFO_LOGINS,	1	},
    {	NULL,		0,		0	}
};


/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
    {	"on",		WIZ_ON,		IM },
    {	"auction",	WIZ_AUCTION,	IM },
    {	"bugs",		WIZ_BUG,	L8 },
    {	"bldbugs",	WIZ_BUILDBUG,	L8 },
    {	"builds",	WIZ_BUILD,	L8 },
    {	"clans",	WIZ_CLANS,	L7 },
    {	"combat",	WIZ_COMBAT,	IM },
    {	"damage",	WIZ_DAMAGE,	L7 },
    {	"deaths",	WIZ_DEATHS,	IM },
    {	"flags",	WIZ_FLAGS,	L5 },
    {	"levels",	WIZ_LEVELS,	IM },
    {	"links",	WIZ_LINKS,	L7 },
    {	"loads",	WIZ_LOAD,	L6 },
    {	"logins",	WIZ_LOGINS,	IM },
    {	"memory",	WIZ_MEMORY,	ML },
    {	"mobdeaths",	WIZ_MOBDEATHS,	L4 },
    {	"newbies",	WIZ_NEWBIE,	IM },
    {	"penalties",	WIZ_PENALTIES,	L5 },
    {	"prefix",	WIZ_PREFIX,	IM },
    {	"resets",	WIZ_RESETS,	L8 },
    {	"restore",	WIZ_RESTORE,	L2 },
    {	"saccing",	WIZ_SACCING,	L5 },
    {	"secure",	WIZ_SECURE,	L1 },
    {	"sites",	WIZ_SITES,	L4 },
    {	"snoops",	WIZ_SNOOPS,	L2 },
    {	"spam",		WIZ_SPAM,	L7 },
    {	"switches",	WIZ_SWITCHES,	L2 },
    {	"ticks",	WIZ_TICKS,	IM },
    {	NULL,		0,		0  }
};

/* attack table  -- not very organized :( */
const 	struct attack_type	attack_table	[MAX_DAMAGE_MESSAGE]	=
{
    { 	"none",		"hit",		-1		},  /*  0 */
    {	"slice",	"slice", 	DAM_SLASH	},	
    {   "stab",		"stab",		DAM_PIERCE	},
    {	"slash",	"slash",	DAM_SLASH	},
    {	"whip",		"whip",		DAM_SLASH	},
    {   "claw",		"claw",		DAM_SLASH	},  /*  5 */
    {	"blast",	"blast",	DAM_BASH	},
    {   "pound",	"pound",	DAM_BASH	},
    {	"crush",	"crush",	DAM_BASH	},
    {   "grep",		"grep",		DAM_SLASH	},
    {	"bite",		"bite",		DAM_PIERCE	},  /* 10 */
    {   "pierce",	"pierce",	DAM_PIERCE	},
    {   "suction",	"suction",	DAM_BASH	},
    {	"beating",	"beating",	DAM_BASH	},
    {   "digestion",	"digestion",	DAM_ACID	},
    {	"charge",	"charge",	DAM_BASH	},  /* 15 */
    { 	"slap",		"slap",		DAM_BASH	},
    {	"punch",	"punch",	DAM_BASH	},
    {	"wrath",	"wrath",	DAM_ENERGY	},
    {	"magic",	"magic",	DAM_ENERGY	},
    {   "divine",	"divine power",	DAM_HOLY	},  /* 20 */
    {	"cleave",	"cleave",	DAM_SLASH	},
    {	"scratch",	"scratch",	DAM_PIERCE	},
    {   "peck",		"peck",		DAM_PIERCE	},
    {   "peckb",	"peck",		DAM_BASH	},
    {   "chop",		"chop",		DAM_SLASH	},  /* 25 */
    {   "sting",	"sting",	DAM_PIERCE	},
    {   "smash",	 "smash",	DAM_BASH	},
    {   "shbite",	"shocking bite",DAM_LIGHTNING	},
    {	"flbite",	"flaming bite", DAM_FIRE	},
    {	"frbite",	"freezing bite", DAM_COLD	},  /* 30 */
    {	"acbite",	"acidic bite", 	DAM_ACID	},
    {	"chomp",	"chomp",	DAM_PIERCE	},
    {  	"drain",	"life drain",	DAM_NEGATIVE	},
    {   "thrust",	"thrust",	DAM_PIERCE	},
    {   "slime",	"slime",	DAM_ACID	},
    {	"shock",	"shock",	DAM_LIGHTNING	},
    {   "thwack",	"thwack",	DAM_BASH	},
    {   "flame",	"flame",	DAM_FIRE	},
    {   "chill",	"chill",	DAM_COLD	},
    {   NULL,		NULL,		0		}
};

/* race table */
struct	race_type	race_table	[]		=
{
/*
    {
	name,		who_name,	pc_race?,
	act bits,	aff_by bits,	off bits,
	imm,		res,		vuln,
	form,		parts,
	max_align,	min_align,	max_ethos, min_ethos, size,		race_value,
	weight_min,	weight_max,	points,	{ class multipliers },
	{ bonus skills },
	{ base stats },		{ max stats },
	{ country list }
    },
*/
    { "unique",	"uniq",	FALSE, 0, {{0}}, {{0}}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, { },
       { "" }, { 13, 13, 13, 13, 13 }, { 18, 18, 18, 18, 18 },
       { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE }
    },

    {
	"bat",		"Bat",		FALSE,
	0,		{{T|Z}},	{{F|H}},  /* OFF_DODGE|OFF_FAST */
	0,		0,		VULN_LIGHT,
	A|G|V,		A|C|D|E|F|H|J|K|P,
	1000,	-1000, 1000,	-1000,	SIZE_TINY,	&race_bat,
	5, 120,		0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"bear",		"Bear",		FALSE,
	0,		{{0}},		{{D|E|O}}, /* OFF_CRUSH|OFF_DISARM|OFF_BERSERK */
	0,		RES_BASH|RES_COLD,	0,
	A|G|V,		A|B|C|D|E|F|H|J|K|U|V,
	1000,	-1000, 1000,	-1000,	SIZE_LARGE,	&race_bear,
	80000, 2400000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"cat",		"Cat",		FALSE,
	0,		{{Z}},		{{F|H}},  /* OFF_FAST|OFF_DODGE */
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|U|V,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_cat,
	800, 3200,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"centaur",	"Centr",	FALSE,
	0,		{{Z}},		{{0}},
	0,		RES_BASH|RES_DROWNING,	VULN_BASH,
 	A|B|G|O,		A|C|K,
 	1000,	-1000, 1000,	-1000,	SIZE_TINY,	&race_centaur,
	1, 10,		0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"centipede",	"Cntpd",	FALSE,
	0,		{{Z}},		{{0}},
	0,		RES_PIERCE|RES_COLD,	VULN_BASH,
 	A|B|G|O,		A|C|K,
 	1000,	-1000, 1000,	-1000,	SIZE_TINY,	&race_centipede,
	1, 10,		0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    { 
	"chimera",	"Chmra",	FALSE,
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_chimera,
	16000, 40000,	0,	{ },
   	{ "" },
 	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    { 
	"demon",	"Demon",	FALSE, 
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q,
	-350,	-1000,-350,	-1000,	SIZE_MEDIUM,	&race_demon,
	16000, 40000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"dog",		"Dog",		FALSE,
	0,		{{0}},		{{H,0,0,0}},	/* OFF_FAST */
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|U|V,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_dog,
	800, 8000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"doll",		"Doll",		FALSE,
	0,		{{0}},		{{0}},
	IMM_COLD|IMM_POISON|IMM_HOLY|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE
	|IMM_DROWNING,	RES_BASH|RES_LIGHT,
	VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
	E|J|M|cc,	A|B|C|G|H|K,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_doll,
	800, 8000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    { 
	"dragonborn",	"Dgnbn",	TRUE, 
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q|W,
	-350,	-1000,-350,	-1000,	SIZE_MEDIUM,	&race_dragonborn,
	16000, 40000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"drow",		"Drow",		TRUE,
	0,		{{J}},		{{0}},
	0,		RES_CHARM,	VULN_IRON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	-350,	-1000,-350,	-1000,	SIZE_MEDIUM,	&race_drow,
	16000, 24000,	5,	{ }, 
	{ "sneak", "hide" },
	{ 12, 14, 13, 15, 11 },	{ 16, 20, 18, 21, 15 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE }
    },

    {
	"dwarf",	"Dwarf",	TRUE,
	0,		{{J}},		{{0}},
	0,		RES_POISON|RES_DISEASE, VULN_DROWNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_dwarf,
	16000, 32000,	8,	{ },
	{ "berserk" },
	{ 14, 12, 14, 10, 15 },	{ 20, 16, 19, 14, 21 },
	{ FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE }
    },

    {
	"elf",		"Elf",		TRUE,
	0,		{{J}},		{{0}},
	0,		RES_CHARM,	VULN_IRON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_elf,
	16000, 24000,	5,	{ }, 
	{ "sneak", "hide" },
	{ 12, 14, 13, 15, 11 },	{ 16, 20, 18, 21, 15 },
	{ FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE }
    },

    {
	"felis",	"Felis",	FALSE,
	0,		{{0}},		{{F|R}},   /* OFF_DODGE|ASSIST_RACE */
	IMM_CHARM,	RES_COLD,	VULN_FIRE,
	A|B|G|V,	A|C|D|E|F|H|J|K|Q|V,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_felis,
	800, 8000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE }
    },		

    {
	"fido",		"Fido",		FALSE,
	0,		{{0}},		{{F|R}},   /* OFF_DODGE|ASSIST_RACE */
	0,		0,			VULN_MAGIC,
	A|B|G|V,	A|C|D|E|F|H|J|K|Q|V,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_fido,
	800, 8000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },		

    {
	"fish",		"Fish",		FALSE,
	0,		{{0}},		{{0}},
	IMM_DROWNING,		RES_FIRE,		VULN_LIGHTNING,
	A|B|G|V,	A|C|D|E|F|H|J|K|Q|V,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_fish,
	800, 8000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },		

    {
	"fox",		"Fox",		FALSE,
	0,		{{Z}},		{{H|F}}, /* OFF_FAST|OFF_DODGE */
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|V,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_fox,
	800, 2400,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"giant",	"Giant",	FALSE,
	0,		{{0}},		{{0}},
	0,		RES_FIRE|RES_COLD,	VULN_MENTAL|VULN_LIGHTNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_HUGE,	&race_giant,
	48000, 160000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    { 
	"gnome",	"Gnome",	TRUE, 
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_gnome,
	16000, 24000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"goblin",	"Gobln",	FALSE,
	0,		{{J}},		{{0}},
	0,		RES_DISEASE,	VULN_MAGIC,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	349,	-1000,349,	-1000,	SIZE_MEDIUM,	&race_goblin,
	16000, 24000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE }
    },

    {
	"halfelf",	"HlfLf",	TRUE,
	0,		{{J}},		{{0}},
	0,		RES_CHARM,	VULN_IRON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_halfelf,
	16000, 32000,	5,	{ }, 
	{ "sneak", "hide" },
	{ 12, 14, 13, 15, 11 },	{ 16, 20, 18, 21, 15 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"halfling",	"Hflng",	TRUE,
	0,		{{J}},		{{0}},
	0,		RES_POISON|RES_DISEASE, VULN_DROWNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-349, 1000, -349,	SIZE_MEDIUM,	&race_halfling,
	4000, 12000,	8,	{ },
	{ "berserk" },
	{ 14, 12, 14, 10, 15 },	{ 20, 16, 19, 14, 21 },
	{ FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, TRUE }
    },

    {
	"halforc",	"Hforc",	TRUE,
	0,		{{J}},		{{0}},
	0,		RES_BASH|RES_NEGATIVE,	VULN_PIERCE|VULN_LIGHT,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_halfelf,
	16000, 32000,	5,	{ }, 
	{ "sneak", "hide" },
	{ 12, 14, 13, 15, 11 },	{ 16, 20, 18, 21, 15 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"highelf",	"Hielf",	FALSE,
	0,		{{J}},		{{0}},
	0,		RES_CHARM,	VULN_IRON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_highelf,
	16000, 24000,	5,	{ },
	{ "sneak", "hide" },
	{ 12, 14, 13, 15, 11 },	{ 16, 20, 18, 21, 15 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"hobgoblin",	"Hbgob",	FALSE,
	0,		{{J}},		{{0}},
	0,		RES_DISEASE|RES_POISON,	0,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_hobgoblin,
	16000, 24000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"horse",	"Horse",	FALSE,
	0,		{{0}},		{{H}},	/* OFF_FAST */
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|U|V,
	1000,	-1000, 1000,	-1000,	SIZE_LARGE,	&race_horse,
	160000, 320000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    { 
	"human",	"Human",	TRUE, 
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_human,
	16000, 40000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    { 
	"insect",	"Insct",	FALSE, 
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|G|O,	A|C|D|E|F|H|M|P|Q|U|V,
	1000,	-1000, 1000,	-1000,	SIZE_TINY,	&race_insect,
	1, 160,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"kobold",	"Kbold",	FALSE,
	0,		{{J}},		{{0}},
	0,		RES_POISON,	VULN_MAGIC,
	A|B|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_kobold,
	8000, 12000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"lizard",	"Liz",		FALSE,
	0,		{{0}},		{{0}},
	0,		RES_POISON,	VULN_COLD,
	A|G|X|cc,	A|C|D|E|F|H|K|Q|V,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_lizard,
	80, 1600,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
        "minotaur",	"Mino",	FALSE,
        0,		{{0}},		{{0}},
        0,		0,		0,
        A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q,
        349,	-1000,349,	-1000,	SIZE_LARGE,	&race_minotaur,
	48000, 160000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE }
    },


    {
	"ogre",		"Ogre",		FALSE,
	0,		{{0}},		{{0}},
	0,		RES_FIRE|RES_COLD,	VULN_MENTAL|VULN_LIGHTNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_HUGE,	&race_ogre,
	80000, 160000,	6,	{ },
	{ "bash", "fast healing" },
	{ 16, 11, 13, 11, 14 },	{ 22, 15, 18, 15, 20 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"orc",		"Orc",		FALSE,
	0,		{{J}},		{{0}},
	0,		RES_DISEASE,	VULN_LIGHT,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	-350,	-1000,-350,	-1000,	SIZE_MEDIUM,	&race_orc,
	16000, 24000,	0,	{ },
	{""},
	{ 13, 13, 13, 13, 13 }, { 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"pig",		"Pig",		FALSE,
	0,		{{0}},		{{0}},
	0,		0,		0,
	A|G|V,	 	A|C|D|E|F|H|J|K|Q,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_pig,
	4000, 16000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 }
    },	

    { 
	"pictsie",	"Pctse",	FALSE, 
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_pixie,
	800, 1600,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"rabbit",	"Rabit",	FALSE,
	0,		{{0}},		{{F|H}}, /*OFF_DODGE|OFF_FAST */
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_rabbit,
	800, 1600,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    { 
	"satyr",	"Satyr",	FALSE, 
	0,		{{0}}, 		{{0}},
	IMM_FIRE, 	RES_NEGATIVE,	VULN_BASH|VULN_COLD,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_satyr,
	16000, 40000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"school monster", "Schol",	FALSE,
	ACT_NOALIGN,	{{0}},		{{0}},
	IMM_CHARM|IMM_SUMMON,	0,		VULN_MAGIC,
	A|M|V,		A|B|C|D|E|F|H|J|K|Q|U,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_school_monster,
	1600, 8000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 }
    },	

    { 
	"shadow",	"Shadw",	FALSE, 
	0,		{{0}}, 		{{0}},
	IMM_NEGATIVE,	RES_MENTAL,	VULN_LIGHT|VULN_HOLY,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_shadow,
	16000, 40000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE }
    },

    {
	"snake",	"Snake",	FALSE,
	0,		{{0}},		{{0}},
	0,		RES_POISON,	VULN_COLD,
	A|G|X|Y|cc,	A|D|E|F|K|L|Q|V|X,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_snake,
	160, 1600,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"song bird",	"Sbird",		FALSE,
	0,		{{T}},		{{F|H}}, /* OFF_FAST|OFF_DODGE */
	0,		0,		0,
	A|G|W,		A|C|D|E|F|H|K|P,
	1000,	-1000, 1000,	-1000,	SIZE_TINY,	&race_songbird,
	40, 320,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    { 
	"sprite",	"Sprte",	FALSE, 
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-349, 1000, -349,	SIZE_MEDIUM,	&race_sprite,
	3200, 8000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

     { 
	"tiefling",	"Tflng",	TRUE, 
	0,		{{0}}, 		{{0}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q|W,
	-350,	-1000,-350,	-1000,	SIZE_MEDIUM,	&race_tiefling,
	16000, 40000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"titan",	"Titan",	FALSE,
	0,		{{0}},		{{0}},
	IMM_CHARM,	RES_FIRE|RES_COLD,	VULN_LIGHTNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_HUGE,	&race_titan,
	48000, 160000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"troll",	"Troll",	FALSE,
	0,		{{F|J|cc}},	{{D}}, /* OFF_BERSERK */
 	0,	RES_CHARM|RES_BASH,	VULN_FIRE|VULN_ACID,
	A|B|H|M|V,		A|B|C|D|E|F|G|H|I|J|K|U|V,
	1000,	-1000, 1000,	-1000,	SIZE_LARGE,		&race_troll,
	40000, 80000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE }
    },

    { 
	"undead",	"Unded",	FALSE,
	O,		{{0}}, 		{{ACT_UNDEAD}},
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K,
	1000,	-1000, 1000,	-1000,	SIZE_MEDIUM,	&race_undead,
	16000, 32000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"water fowl",	"Wbird",	FALSE,
	0,		{{T|bb}},	{{0}},
	0,		RES_DROWNING,		0,
	A|G|W,		A|C|D|E|F|H|K|P,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_waterfowl,
	320, 800,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"wolf",		"Wolf",		FALSE,
	0,		{{Z}},		{{H|F}}, /* OFF_FAST|OFF_DODGE */
	0,		0,		0,	
	A|G|V,		A|C|D|E|F|J|K|Q|V,
	1000,	-1000, 1000,	-1000,	SIZE_SMALL,	&race_wolf,
	4000, 12000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"wyvern",	"Wyvrn",	FALSE,
	0,		{{D|F|T}},	{{C|F|H}}, /* OFF_BASH|OFF_FAST|OFF_DODGE */
	IMM_POISON,	0,	VULN_LIGHT,
	A|B|G|Z,		A|C|D|E|F|H|J|K|Q|V|X,
	1000,	-1000, 1000,	-1000,	SIZE_HUGE,		&race_wyvern,
	80000, 320000,	0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },
      
    { 
        "zombie",       "Zombe",        FALSE,
        O,              {{0}},          {{ACT_UNDEAD}},
        0,              0,              0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K,
        1000,   -1000, 1000, -1000, SIZE_MEDIUM,    &race_zombie,
        16000, 32000,   0,      { },
        { "" },
        { 13, 13, 13, 13, 13 }, { 18, 18, 18, 18, 18 },
        { FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },

    {
	"unique",	"Uniq",		FALSE,
	0,		{{0}},		{{0}},
	0,		0,		0,		
	0,		0,
	1000,	-1000, 1000,	-1000,	0,		NULL,
	0, 0,		0,	{ },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },
	{ FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE }
    },


    {
	NULL, "", 0, 0, {{0}}, {{0}}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL,
    }
};


/*
 * Class table.
 */
const	struct	class_type	class_table	[MAX_CLASS]	=
{
     {
        "cleric",	"Clr",	"clerics",	STAT_WIS, OBJ_VNUM_SCHOOL_MACE,
        { 0, 0 },  75,  100, 10,  5,  11, CLASS_FMANA,
        "cleric basics", "cleric default", &class_cleric,
        TIER_TWO,	1000,	-1000, 1000,	-1000,	BASE_CLERIC
    },

    {
	"druid",        "Drd",  "druids", STAT_CON,  OBJ_VNUM_SCHOOL_SWORD,
	{ 0, 0 },  75,  100,  -50,  12, 16, CLASS_NOFLAGS,
	"druid basics", "druid default",	&class_druid,
	TIER_ONE,	1000,	-349,	BASE_FIGHTER
    },
   
    {
        "fighter",     "Ftr",  "fighters",    STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
        { 0, 0 },  75,  100, 10,  7,  10, CLASS_NOFLAGS,
        "fighter basics", "fighter default", &class_fighter,
        TIER_TWO,       1000, -1000,    BASE_FIGHTER
    },

     {
	"mage",		"Mag",  "mages",	STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 0, 0 },  75,  100, 30,  4,  7, CLASS_FMANA,
	"mage basics", "mage default",	&class_mage,
	TIER_ONE,	1000,	-1000, 1000,	-1000,	BASE_MAGE
    },

     {
	"rogue",         "Rog",  "rogues",	STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 0, 0 },  75,  100,  -20,  6, 11, CLASS_NOFLAGS,
	"rogue basics", "rogue default",	&class_rogue,
	TIER_ONE,	349,	-1000,349,	-1000,	BASE_ROGUE
    },

};


const struct armor_val_type armor_values [] =
{
    {	0,	1	},
    {	1,	1	},
    {	2,	1	},
    {	3,	1	},
    {	4,	1	},
    {	5,	1	},
    {	6,	1	},
    {	7,	1	},
    {	8,	1	},
    {	9,	2	},
    {	10,	2	},
    {	11,	2	},
    {	12,	2	},
    {	13,	2	},
    {	14,	2	},
    {	15,	2	},
    {	16,	2	},
    {	17,	3	},
    {	18,	3	},
    {	19,	3	},
    {	20,	3	},
    {	21,	3	},
    {	22,	3	},
    {	23,	3	},
    {	24,	3	},
    {	25,	4	},
    {	26,	4	},
    {	27,	4	},
    {	28,	4	},
    {	29,	4	},
    {	30,	4	},
    {	31,	4	},
    {	32,	4	},
    {	33,	4	},
    {	34,	4	},
    {	35,	4	},
    {	36,	4	},
    {	37,	5	},
    {	38,	5	},
    {	39,	5	},
    {	40,	5	},
    {	41,	5	},
    {	42,	5	},
    {	43,	5	},
    {	44,	5	},
    {	45,	6	},
    {	46,	6	},
    {	47,	6	},
    {	48,	6	},
    {	49,	6	},
    {	50,	6	},
    {	51,	6	},
    {	52,	6	},
    {	53,	7	},
    {	54,	7	},
    {	55,	7	},
    {	56,	7	},
    {	57,	7	},
    {	58,	7	},
    {	59,	7	},
    {	60,	7	},
    {	61,	8	},
    {	62,	8	},
    {	63,	8	},
    {	64,	8	},
    {	65,	8	},
    {	66,	8	},
    {	67,	8	},
    {	68,	8	},
    {	69,	9	},
    {	70,	9	},
    {	71,	9	},
    {	72,	9	},
    {	73,	9	},
    {	74,	9	},
    {	75,	9	},
    {	76,	9	},
    {	77,	10	},
    {	78,	10	},
    {	79,	10	},
    {	80,	10	},
    {	81,	10	},
    {	82,	10	},
    {	83,	10	},
    {	84,	10	},
    {	85,	11	},
    {	86,	11	},
    {	87,	11	},
    {	88,	11	},
    {	89,	11	},
    {	90,	11	},
    {	91,	11	},
    {	92,	11	},
    {	93,	12	},
    {	94,	12	},
    {	95,	12	},
    {	96,	12	},
    {	97,	12	},
    {	98,	12	},
    {	99,	12	},
    {	100,	12	},
    {	101,	13	},
    {	102,	13	},
    {	103,	13	},
    {	104,	13	},
    {	105,	13	},
    {	106,	13	},
    {	107,	13	},
    {	108,	13	},
    {	109,	13	},
    {	110,	13	},
    {	111,	14	},
    {	112,	14	},
    {	113,	14	},
    {	114,	14	},
    {	115,	14	},
    {	116,	14	},
    {	117,	14	},
    {	118,	14	},
    {	119,	14	},
    {	120,	14	},
    {	121,	15	},
    {	122,	15	},
    {	123,	15	},
    {	124,	15	},
    {	125,	15	},
    {	126,	15	},
    {	127,	15	},
    {	128,	15	},
    {	129,	15	},
    {	130,	15	},
    {	131,	16	},
    {	132,	16	},
    {	133,	16	},
    {	134,	16	},
    {	135,	16	},
    {	136,	16	},
    {	137,	16	},
    {	138,	16	},
    {	139,	16	},
    {	140,	16	},
    {	141,	17	},
    {	142,	17	},
    {	143,	17	},
    {	144,	17	},
    {	145,	17	},
    {	146,	17	},
    {	147,	17	},
    {	148,	17	},
    {	149,	17	},
    {	150,	17	},
    {	151,	18	},
    {	152,	18	},
    {	153,	18	},
    {	154,	18	},
    {	155,	18	},
    {	156,	18	},
    {	157,	18	},
    {	158,	18	},
    {	159,	18	},
    {	160,	18	},
    {	161,	19	},
    {	162,	19	},
    {	163,	19	},
    {	164,	19	},
    {	165,	19	},
    {	166,	19	},
    {	167,	19	},
    {	168,	19	},
    {	169,	19	},
    {	170,	19	},
    {	171,	20	},
    {	172,	20	},
    {	173,	20	},
    {	174,	20	},
    {	175,	20	},
    {	176,	20	},
    {	177,	20	},
    {	178,	20	},
    {	179,	20	},
    {	180,	20	},
    {	181,	20	},
    {	182,	20	},
    {	183,	20	},
    {	184,	20	},
    {	185,	20	},
    {	186,	20	},
    {	187,	20	},
    {	188,	20	},
    {	189,	20	},
    {	190,	20	},
    {	191,	22	},
    {	192,	22	},
    {	193,	22	},
    {	194,	22	},
    {	195,	22	},
    {	196,	22	},
    {	197,	22	},
    {	198,	22	},
    {	199,	22	},
    {	200,	22	},
    {	201,	22	},
    {	202,	22	},
    {	203,	22	},
    {	204,	22	},
    {	205,	22	},
    {	206,	22	},
    {	207,	22	},
    {	208,	22	},
    {	209,	22	},
    {	210,	22	},
    {	211,	23	},
    {	212,	23	},
    {	213,	23	},
    {	214,	23	},
    {	215,	23	},
    {	216,	23	},
    {	217,	23	},
    {	218,	23	},
    {	219,	23	},
    {	220,	23	},
    {	221,	23	},
    {	222,	23	},
    {	223,	23	},
    {	224,	23	},
    {	225,	23	},
    {	226,	23	},
    {	227,	23	},
    {	228,	23	},
    {	229,	23	},
    {	230,	23	},
    {	231,	24	},
    {	232,	24	},
    {	233,	24	},
    {	234,	24	},
    {	235,	24	},
    {	236,	24	},
    {	237,	24	},
    {	238,	24	},
    {	239,	24	},
    {	240,	24	},
    {	241,	24	},
    {	242,	24	},
    {	243,	24	},
    {	244,	24	},
    {	245,	24	},
    {	246,	24	},
    {	247,	24	},
    {	248,	24	},
    {	249,	24	},
    {	250,	24	},
    {	251,	25	},
    {	252,	25	},
    {	253,	25	},
    {	254,	25	},
    {	255,	25	},
    {	256,	25	},
    {	257,	25	},
    {	258,	25	},
    {	259,	25	},
    {	260,	25	},
    {	261,	25	},
    {	262,	25	},
    {	263,	25	},
    {	264,	25	},
    {	265,	25	},
    {	266,	25	},
    {	267,	25	},
    {	268,	25	},
    {	269,	25	},
    {	270,	25	},
    {	271,	26	},
    {	272,	26	},
    {	273,	26	},
    {	274,	26	},
    {	275,	26	},
    {	276,	26	},
    {	277,	26	},
    {	278,	26	},
    {	279,	26	},
    {	280,	26	},
    {	281,	26	},
    {	282,	26	},
    {	283,	26	},
    {	284,	26	},
    {	285,	26	},
    {	286,	26	},
    {	287,	26	},
    {	288,	26	},
    {	289,	26	},
    {	290,	26	},
    {	291,	27	},
    {	292,	27	},
    {	293,	27	},
    {	294,	27	},
    {	295,	27	},
    {	296,	27	},
    {	297,	27	},
    {	298,	27	},
    {	299,	27	},
    {	300,	27	},
    {	301,	27	},
    { 302,  27    },
    { 302,  27    },
    { 303,  27    },
    { 304,  27    },
    { 305,  27    },
    { 306,  27    },
    { 307,  27    },
    { 308,  27    },
    { 309,  27    },
    { 310,  27    },
    {	311,	27	},
    {	312,	27	},
    {	313,	27	},
    {	314,	27	},
    {	315,	27	},
    {	316,	27	},
    {	317,	27	},
    {	318,	27	},
    {	319,	27	},
    {	320,	27	},
    {	321,	28	},
    {	322,	28	},
    {	323,	28	},
    {	324,	28	},
    {	325,	28	},
    {	326,	28	},
    {	327,	28	},
    {	328,	28	},
    {	329,	28	},
    {	330,	28	},
    {	331,	29	},
    {	332,	29	},
    {	333,	29	},
    {	334,	29	},
    {	335,	29	},
    {	336,	29	},
    {	337,	29	},
    {	338,	29	},
    {	339,	29	},
    {	340,	29	},
    {	341,	29	},
    {	342,	29	},
    {	343,	29	},
    {	344,	29	},
    {	345,	29	},
    {	346,	29	},
    {	347,	29	},
    {	348,	29	},
    {	349,	29	},
    {	350,	29	},
    {	351,	29	},
    {	352,	29	},
    {	353,	29	},
    {	354,	29	},
    {	355,	29	},
    {	356,	29	},
    {	357,	29	},
    {	358,	29	},
    {	359,	29	},
    {	360,	29	},
    {	361,	29	},
    {	362,	29	},
    {	363,	29	},
    {	364,	29	},
    {	365,	30	},
    {	366,	30	},
    {	367,	30	},
    {	368,	30	},
    {	369,	30	},
    {	370,	30	},
    {	371,	30	},
    {	372,	30	},
    {	373,	30	},
    {	374,	30	},
    {	375,	30	},
    {	376,	30	},
    {	377,	30	},
    {	378,	30	},
    {	379,	30	},
    {	380,	30	},
    {	381,	30	},
    {	382,	30	},
    {	383,	30	},
    {	384,	30	},
    {	385,	30	},
    {	386,	30	},
    {	387,	30	},
    {	388,	30	},
    {	389,	30	},
    {	390,	30	},
    {	391,	30	},
    {	392,	30	},
    {	393,	30	},
    {	394,	30	},
    {	395,	30	},
    {	396,	30	},
    {	397,	30	},
    {	398,	30	},
    {	399,	30	},
    {	400,	30	},
    {   401,    30      },

    {	0,	0	},
};


/*   This is the rounded form of the level chart, as done
     by Russ Taylor in his ROM 2.4 documentation, and the
     additions (Levels 60-100) done by Belgarath ca. 1996.
     ----------------------------------------------------*/
const struct hitdice_type hitdice_table [] =
/*   Level        Hit Dice      DamageDice      ManaDice          AC Hitroll
     -----       ----------     ---------       --------        ----   -- */
{
    {	 0,	{ 1, 4,    5},	{ 1, 3, 0},	{1,1, 94},	 200,	0	},
    {	 1,	{ 1, 4,   10},	{ 1, 4, 0},	{1,1, 99},	 175,	0	},
    {	 2,	{ 1, 4,   15},	{ 1, 5, 0},	{1,1,104},	 150,	0	},
    {	 3,	{ 1, 4,   20},	{ 1, 6, 0},	{1,1,109},	 100,	0	},
    {	 4,	{ 1, 4,   25},	{ 1, 5, 1},	{1,1,114},	 90,	0	},
    {	 5,	{ 1, 4,   30},	{ 2, 4, 1},	{1,1,124},	 80,	0	},
    {	 6,	{ 2, 4,   50},	{ 2, 4, 1},	{1,1,129},	 70,	1	},
    {	 7,	{ 2, 4,   75},	{ 1, 8, 1},	{1,1,134},	  60,	1	},
    {	 8,	{ 2, 5,   85},	{ 1, 7, 2},	{1,1,139},	  45,	1	},
    {	 9,	{ 2, 6,  100},	{ 1, 8, 1},	{1,1,144},	  35,	1	},
    {	10,	{ 2, 6,  121},	{ 1, 8, 2},	{1,1,149},	  20,	1	},
    {	11,	{ 2, 6,  130},	{ 1, 9, 2},	{1,1,154},	  18,	1	},
    {	12,	{ 2, 6,  145},	{ 1,10, 2},	{1,1,159},	  16,	2	},
    {	13,	{ 2, 6,  160},	{ 2, 5, 3},	{1,1,164},	  15,	2	},
    {	14,	{ 2, 6,  180},	{ 1,12, 2},	{1,1,169},	  12,	2	},
    {	15,	{ 3, 5,  200},	{ 2, 6, 3},	{1,1,174},	  10,	2	},
    {	16,	{ 3, 6,  233},	{ 2, 6, 4},	{1,1,179},	   7,	2	},
    {	17,	{ 3, 7,  254},	{ 2, 7, 5},	{1,1,184},	   5,	2	},
    {	18,	{ 3, 9,  265},	{ 2, 7, 4},	{1,1,189},	   4,	3	},
    {	19,	{ 3, 9,  280},	{ 2, 7, 4},	{1,1,194},	   2,	3	},
    {	20,	{ 3, 9,  308},	{ 2, 7, 5},	{1,1,199},	   0,	3	},
    {	21,	{ 4, 8,  330},	{ 4, 4, 5},	{1,1,204},	  -3,	3	},
    {	22,	{ 4, 9,  360},	{ 4, 4, 6},	{1,1,209},	  -6,	3	},
    {	23,	{ 5, 8,  380},	{ 2,10, 3},	{1,1,214},	  -9,	3	},
    {	24,	{ 5, 7,  420},	{ 2,11, 4},	{1,1,219},	  -12,	4	},
    {	25,	{ 5, 6,  450},	{ 3, 8, 6},	{1,1,224},	  -15,	4	},
    {	26,	{ 5, 6,  550},	{ 3, 7, 7},	{1,1,229},	  -18,  4	},
    {	27,	{ 5, 6,  600},	{ 3, 8, 7},	{1,1,234},	  -21,  4	},
    {	28,	{ 5, 6,  650},	{ 3, 8, 8},	{1,1,239},	  -24,  4	},
    {	29,	{ 5, 6,  725},	{ 3, 8, 8},	{1,1,244},	  -27,  4	},
    {	30,	{ 5, 7,  750},	{ 3, 9, 8},	{1,1,249},	  -30,  5	},
    {   31,	{ 3,13,  800}, { 3, 9, 8},     {1,1,249},	  -33,  5	},
    {   32,	{ 3,12,  850}, { 4, 9, 0},     {1,1,249},	  -36,  5	},
    {   33,	{ 6, 9,  900}, { 4, 9, 0},     {1,1,249},	  -39,  5	},
    {   34,	{10,10,  900}, { 4, 9, 1},     {1,1,249},	  -42,  5	},
    {   35,	{ 9,10, 1000}, { 4, 9, 1},     {1,1,249},	  -45,  5	},
    {   36,	{ 7, 9, 1100}, { 6, 6, 2},     {1,1,249},	  -48,  6	},
    {   37,	{ 6, 8, 1200}, { 6, 6, 2},     {1,1,249},	  -51,  6	},
    {   38,	{ 7,12, 1250}, { 6, 6, 3},     {1,1,249},	  -54,  6	}, 
    {   39,	{ 5,10, 1400}, { 6, 6, 3},     {1,1,249},	  -57,  6	},
    {   40,	{ 5,10, 1450},	{ 5,8,0},	{1,1,249},	  -60,  6	},
    {   41,	{ 7,11, 1500},	{ 5,8,0},	{1,1,249},	  -63,  6	}, 
    {   42,	{ 7,11, 1600},	{ 5,8,1},	{1,1,249},	  -66, 7	}, 
    {   43,	{ 7,11, 1750},	{ 6,7,0},	{1,1,249},	  -69, 7	}, 
    {   44, { 6,10, 1800},    { 6,7,0},   {1,1,249},    -71, 7    },
    {   45,	{ 5,10, 1850},	{ 6,7,0},	{1,1,249},	  -72, 7	}, 
    {   46,	{ 7,11, 1900},	{ 6,7,1},	{1,1,249},	  -75, 7	}, 
    {   47,	{ 7,11, 2000},	{ 6,7,1},	{1,1,249},	  -78, 7	}, 
    {   48,	{10,10,2050},  { 6,7,1},	{1,1,249},	  -81, 7	}, 
    {   49,	{7,11,2150},	{ 6,7,2},	{1,1,249},	  -84, 8	}, 
    {   50,	{5,10,2250},	{ 5,9,0},	{1,1,249},	  -87, 8	}, 
    {   51,	{7,10,2300},	{ 5,9,0},	{1,1,249},	  -90, 8	}, 
    {   52,	{7,10,2370},	{ 5,9,1},	{1,1,249},	  -93, 8	}, 
    {   53,	{6,10,2450},	{ 5,9,1},	{1,1,249},	  -96, 8	}, 
    {   54,	{8,10,2500},	{ 5,9,2},	{1,1,249},	  -99, 8	}, 
    {   55,	{5,10,2650},	{ 5,9,2},	{1,1,249},	  -102, 9	}, 
    {   56,	{7,10,2650},	{ 6,8,0},	{1,1,249},	  -105, 9	}, 
    {   57,	{9,10,2700},	{ 6,8,0},	{1,1,249},	  -108, 9	}, 
    {   58,	{9,10,2770},	{ 7,7,0},	{1,1,249},	  -111, 9	}, 
    {   59,	{8,10,2850},	{ 7,7,0},	{1,1,249},	  -114, 9	}, 
    {   60,	{10,10,2900},	{ 5,10,0},	{1,1,249},	  -117, 9	}, 
    {   61,	{10,10,3000},	{ 7,7,1 },	{1,1,249},	  -120, 10	}, 
    {   62,	{10,10,3100},	{ 7,7,2 },	{1,1,249},	  -123, 10	}, 
    {   63,	{10,10,3200},	{ 5,10,1},	{1,1,249},	  -126, 10	}, 
    {   64,	{10,10,3300},	{ 5,10,2},	{1,1,249},	  -129, 10	}, 
    {   65,	{10,10,3400},	{ 7,7,3 },	{1,1,249},	  -132, 10	}, 
    {   66,	{10,10,3500},	{ 5,10,3},	{1,1,249},	  -135, 10	}, 
    {   67,	{10,10,3600},	{ 7,7,4 },	{1,1,249},	  -138, 11	},
    {   68,	{10,10,3700},	{ 6,9,0 },	{1,1,249},	  -141, 11	}, 
    {   69,	{10,10,3800},	{ 6,9,0 },	{1,1,249},	  -144, 11	}, 
    {   70,	{10,10,3950},	{ 5,11,0},	{1,1,249},	  -147, 11	}, 
    {   71,	{10,10,4100},	{ 6,9,1 },	{1,1,249},	  -150, 11	}, 
    {   72,	{10,10,4250},	{ 5,11,1},	{1,1,249},	  -153, 11	}, 
    {   73,	{10,10,4400},	{ 6,9,2 },	{1,1,249},	  -156, 12	}, 
    {   74,	{10,10,4550},	{5,11,2 },	{1,1,249},	  -159, 12	}, 
    {   75,	{10,10,4700},	{ 6,9,3 },	{1,1,249},	  -162, 12	}, 
    {   76,	{10,10,4850},	{ 5,11,3},	{1,1,249},	  -165, 12	}, 
    {   77,	{10,10,5000},	{ 6,9,4 },	{1,1,249},	  -168, 12	}, 
    {   78,	{10,10,5150},	{ 5,11,4},	{1,1,249},	  -172, 12	}, 
    {   79,	{ 7,10,5300},	{ 6,9,5 },	{1,1,249},	  -175, 13	}, 
    {   80,	{10,10,5400},	{ 6,10,0},	{1,1,249},	  -178, 13	}, 
    {   81,	{10,10,5550},	{ 6,10,0},	{1,1,249},	  -181, 13	}, 
    {   82,	{10,10,5700},	{ 6,10,1},	{1,1,249},	  -184, 13	},
    {   83,	{10,10,5850},	{ 6,10,1},	{1,1,249},	  -187, 13	}, 
    {   84,	{10,10,6000},	{ 6,10,2},	{1,1,249},	  -190, 13	}, 
    {   85,	{10,10,6150},	{ 6,10,2},	{1,1,249},	  -193, 14	}, 
    {   86,	{10,10,6300},	{ 7,9,0 },	{1,1,249},	  -196, 14	}, 
    {   87,	{10,10,6450},	{ 7,9,0 },	{1,1,249},	  -199, 14	}, 
    {   88,	{10,10,6600},	{ 8,8,0 },	{1,1,249},	  -202, 14	}, 
    {   89,	{10,10,6750},	{ 8,8,0 },	{1,1,249},	  -205, 14	}, 
    {   90,	{10,10,6900},	{ 8,8,1 },	{1,1,249},	  -208, 14	},   
    {   91,	{10,10,7050},	{ 8,8,1 },	{1,1,249},	  -211, 15	}, 
    {   92,	{10,10,7200},	{ 6,11,0},	{1,1,249},	  -214, 15	}, 
    {   93,	{10,10,7350},	{ 8,8,2 },	{1,1,249},	  -217, 15	}, 
    {   94,	{10,10,7500},	{ 6,11,1},	{1,1,249},	  -220, 15	}, 
    {   95,	{10,10,7650},	{ 6,11,1},	{1,1,249},	  -223, 15	}, 
    {   96,	{10,10,7800},	{ 6,11,2},	{1,1,249},	  -226, 15	}, 
    {   97,	{10,10,7050},	{ 6,11,2},	{1,1,249},	  -229, 16	}, 
    {   98,	{10,10,8100},	{ 6,11,3},	{1,1,249},	  -232, 16	}, 
    {   99,	{10,10,8250},	{ 6,11,4},	{1,1,249},	  -235, 16	}, 
    {	100,	{10,10,8400},	{ 7,10,0},	{1,1,249},	  -238, 16	}, 
    {	101,	{10,10,8550},	{ 7,10,0},	{1,1,249},	  -241, 16	},
    {	102,	{10,10,8700},	{ 7,10,1},	{1,1,249},	  -244, 16	}, 
    {	103,	{10,10,8850},	{ 7,10,1},	{1,1,249},	  -247, 17	}, 
    {	104,	{10,10,9000},	{ 8,9,0 },	{1,1,249},	  -250, 17	}, 
    {	105,	{10,10,9150},	{ 8,9,0 },	{1,1,249},	  -253, 17	}, 
    {	106,	{10,10,9300},	{ 8,9,1 },	{1,1,249},	  -256, 17	}, 
    {	107,	{10,10,9450},	{ 8,9,1 },	{1,1,249},	  -259, 17	}, 
    {	108,	{10,10,9600},	{ 8,9,2 },	{1,1,249},	  -262, 17	}, 
    {	109,	{10,10,9750},	{ 8,9,2 },	{1,1,249},	  -265, 17	}, 
    {	110,	{10,10,9900},	{ 5,15,0},	{1,1,249},	  -268, 18	}, 
    {	111,	{10,10,9950},	{ 5,15,0},	{1,1,249},	  -271, 18	}, 
    {	112,	{10,10,10100},	{ 7,10,6},	{1,1,249},	  -274, 18	}, 
    {	113,	{10,10,10250},	{ 7,10,6},	{1,1,249},	  -277, 18	}, 
    {	114,	{10,10,10400},	{ 7,11,0},	{1,1,249},	  -280, 18	}, 
    {	115,	{10,10,10550},	{ 7,11,0},	{1,1,249},	  -283, 18	}, 
    {	116,	{10,10,10700},	{ 7,11,1},	{1,1,249},	  -286, 19	}, 
    {	117,	{10,10,10850},	{ 7,11,1},	{1,1,249},	  -289, 19	}, 
    {	118,	{10,10,11000},	{ 7,11,2},	{1,1,249},	  -292, 19	}, 
    {	119,	{10,10,11250},	{ 7,11,2},	{1,1,249},	  -295, 19	},
    {	120,	{10,15,11350},	{ 8,10,0},	{1,1,249},	  -298, 19	},
    {	121,	{10,15,11475},	{ 8,10,0},	{1,1,249},	  -301, 19	},
    {	122,	{10,15,11600},	{ 9,9,0 },	{1,1,249},	  -304, 20	},
    {	123,	{10,15,11725},	{ 9,9,0 },	{1,1,249},	  -307, 20	},
    {	124,	{10,15,11850},	{ 9,9,1 },	{1,1,249},	  -310, 20	},
    {	125,	{10,15,12100},	{ 9,9,2 },	{1,1,249},	  -313, 20	},
    { 126,  {10,15,12175},    { 9,9,2 },  {1,1,249},    -313, 20  },
    {	127,	{10,15,12225},	{ 9,9,2 },	{1,1,249},	  -316, 20	}, 
    {	128,	{10,15,12350},	{ 9,9,3 },	{1,1,249},	  -319, 20	}, 
    {	129,	{10,15,12450},	{ 9,9,3 },	{1,1,249},	  -321, 21	}, 
    {	130,	{10,15,12550},	{ 8,10,5},	{1,1,249},	  -324, 21	}, 
    {	131,	{10,15,12700},	{ 9,9,4 },	{1,1,249},	  -327, 21	}, 
    {	132,	{10,15,12750},	{ 9,9,5 },	{1,1,249},	  -330, 21	}, 
    {	133,	{10,15,12900},	{ 9,9,5 },	{1,1,249},	  -333, 21	}, 
    {	134,	{10,15,13050},	{ 7,12,3},	{1,1,249},	  -336, 21	}, 
    {	135,	{10,15,13100},	{ 7,12,3},	{1,1,249},	  -339, 22	}, 
    {	136,	{10,15,13350},	{ 8,11,0},	{1,1,249},	  -342, 22	}, 
    {	137,	{10,15,13500},	{ 8,11,0},	{1,1,249},	  -345, 22	}, 
    {	138,	{10,15,13650},	{ 8,11,1},	{1,1,249},	  -348, 22	}, 
    {	139,	{10,15,13800},	{ 8,11,1},	{1,1,249},	  -351, 22	}, 
    {	140,	{10,15,14350},	{ 9,10,0},	{1,1,249},	  -354, 22	}, 
    {	141,	{10,15,14100},	{ 10,9,0},	{1,1,249},	  -357, 23	}, 
    {	142,	{10,15,14400},	{ 9,10,1},	{1,1,249},	  -360, 23	}, 
    { 143,  {10,15,14475},    { 9,10,1},  {1,1,249},    -363, 23  },
    {	144,	{10,15,14550},	{ 7,13,1},	{1,1,249},	  -363, 23	}, 
    {	145,	{10,15,14700},	{ 7,13,1},	{1,1,249},	  -366, 23	}, 
    {	146,	{10,15,14850},	{ 7,13,2},	{1,1,249},	  -369, 23	}, 
    {	147,	{10,15,15000},	{ 7,13,2},	{1,1,249},	  -372, 23	}, 
    {	148,	{10,15,15150},	{ 9,10,4},	{1,1,249},	  -375, 24	}, 
    {	149,	{10,15,15300},	{ 7,13,3},	{1,1,249},	  -378, 24	}, 
    {	150,	{10,15,15450},	{ 10,9,5},	{1,1,249},	  -381, 24	}, 
    {	151,	{10,15,15575},	{ 9,10,5},	{1,1,249},	  -384, 24	}, 
    {	152,	{10,15,15700},	{ 8,12,0},	{1,1,249},	  -387, 24	}, 
    {	153,	{10,15,15825},	{ 8,12,0},	{1,1,249},	  -390, 24	}, 
    {	154,	{10,15,15950},	{ 8,12,1},	{1,1,249},	  -393, 25	}, 
    {	155,	{10,15,16075},	{ 8,12,1},	{1,1,249},	  -396, 25	}, 
    {	156,	{10,15,16200},	{ 8,12,2},	{1,1,249},	  -399, 25	},
    {	157,	{10,15,16325},	{ 8,12,2},	{1,1,249},	  -402, 25	}, 
    {	158,	{10,15,16450},	{ 9,11,0},	{1,1,249},	  -405, 25	}, 
    {	159,	{10,15,16575},	{ 10,10,0},	{1,1,249},	  -408, 25	}, 
    {	160,	{10,15,16700},	{ 10,10,0},	{1,1,249},	  -411, 26	}, 
    {	161,	{10,15,16950},	{ 10,10,1},	{1,1,249},	  -414, 26	},
    { 162,  {10,15,16950},    { 10,10,1}, {1,1,249},    -414, 26  },
    {	163,	{10,15,17075},	{ 10,10,2},	{1,1,249},	  -417, 26	}, 
    {	164,	{10,15,17200},	{ 10,10,2},	{1,1,249},	  -420, 26	},
    {	165,	{10,15,17325},	{ 10,10,3},	{1,1,249},	  -423, 26	}, 
    {	166,	{10,15,17450},	{ 10,10,3},	{1,1,249},	  -426, 26	}, 
    {	167,	{10,15,17575},	{ 10,10,4},	{1,1,249},	  -429, 27	}, 
    {	168,	{10,15,17700},	{ 10,10,4},	{1,1,249},	  -432, 27	}, 
    {	169,	{10,15,17825},	{ 10,10,5},	{1,1,249},	  -435, 27	}, 
    {	170,	{10,15,17950},	{ 10,10,5},	{1,1,249},	  -438, 27	}, 
    {	171,	{10,15,18050},	{ 10,10,6},	{1,1,249},	  -441, 27	}, 
    {	172,	{10,15,18150},	{ 10,10,6},	{1,1,249},	  -444, 27	}, 
    {	173,	{10,15,18250},	{ 10,10,7},	{1,1,249},	  -447, 28	}, 
    {	174,	{10,15,18350},	{ 10,10,7},	{1,1,249},	  -450, 28	}, 
    {	175,	{10,15,18450},	{ 10,10,8},	{1,1,249},	  -453, 28	}, 
    {	176,	{10,15,18550},	{ 10,10,8},	{1,1,249},	  -456, 28	}, 
    {	177,	{10,15,18650},	{ 10,10,9},	{1,1,249},	  -459, 28	}, 
    {	178,	{10,15,18750},	{ 10,10,9},	{1,1,249},	  -462, 28	}, 
    {	179,	{10,15,18850},	{ 10,11,0},	{1,1,249},	  -465, 29	}, 
    {	180,	{10,15,18950},	{ 10,11,0},	{1,1,249},	  -468, 29	}, 
    {	181,	{10,15,19000},	{ 10,11,1},	{1,1,249},	  -471, 29	}, 
    {	182,	{10,15,19075},	{ 10,11,1},	{1,1,249},	  -474, 29	}, 
    {	183,	{10,15,19150},	{ 10,11,2},	{1,1,249},	  -477, 29	}, 
    {	184,	{10,15,19225},	{ 10,11,2},	{1,1,249},	  -480, 29	}, 
    {	185,	{10,15,19350},	{ 10,11,3},	{1,1,249},	  -483, 30	}, 
    {	186,	{10,15,19450},	{ 10,11,3},	{1,1,249},	  -486, 30	}, 
    {	187,	{10,15,19450},	{ 10,11,4},	{1,1,249},	  -489, 30	}, 
    {	188,	{10,15,19525},	{ 10,11,4},	{1,1,249},	  -492, 30	}, 
    {	189,	{10,15,19600},	{ 10,11,5},	{1,1,249},	  -495, 30	}, 
    {	190,	{10,15,19675},	{ 10,11,5},	{1,1,249},	  -498, 30	}, 
    {	191,	{10,15,19750},	{ 10,11,6},	{1,1,249},	  -501, 31	}, 
    {	192,	{10,15,19825},	{ 10,11,6},	{1,1,249},	  -504, 31	}, 
    {	193,	{10,15,19900},	{ 10,11,7},	{1,1,249},	  -507, 31	}, 
    {	194,	{10,15,19975},	{ 10,11,7},	{1,1,249},	  -510, 31	}, 
    {	195,	{10,15,20050},	{ 10,11,8},	{1,1,249},	  -513, 31	}, 
    {	196,	{10,15,20125},	{ 10,11,8},	{1,1,249},	  -516, 31	}, 
    {	197,	{10,15,20200},	{ 10,11,9},	{1,1,249},	  -519, 32	}, 
    {	198,	{10,15,20275},	{ 10,11,9},	{1,1,249},	  -522, 32	}, 
    {	199,	{10,15,20350},	{ 10,10,20},	{1,1,249},	  -525, 32	}, 
    {	200,	{10,20,20375},	{ 10,10,20},	{1,1,249},	  -528, 32	}, 
    {	201,	{10,20,20400},	{ 10,10,22},	{1,1,249},	  -531, 32	}, 
    {	202,	{10,20,20575},	{ 10,10,22},	{1,1,249},	  -534, 32	}, 
    {	203,	{10,20,20650},	{ 10,10,24},	{1,1,249},	  -537, 33	}, 
    {	204,	{10,20,20775},	{ 10,10,24},	{1,1,249},	  -540, 33	}, 
    {	205,	{10,20,20900},	{ 10,10,26},	{1,1,249},	  -543, 33	}, 
    {	206,	{10,20,21025},	{ 10,10,26},	{1,1,249},	  -546, 33	}, 
    {	207,	{10,20,21150},	{ 10,10,28},	{1,1,249},	  -549, 33	}, 
    {	208,	{10,20,21225},	{ 10,10,28},	{1,1,249},	  -552, 33	}, 
    {	209,	{10,20,21350},	{ 13,10,0},	{1,1,249},	  -555, 34	}, 
    {	210,	{10,20,21475},	{ 13,10,0},	{1,1,249},	  -558, 34	}, 
    {	211,	{10,20,21600},	{ 11,12,0},	{1,1,249},	  -561, 34	}, 
    {	212,	{10,20,21725},	{ 11,12,0},	{1,1,249},	  -564, 34	},  
    {	213,	{10,20,21875},	{ 11,12,2},	{1,1,249},	  -567, 34	}, 
    {	214,	{10,20,22000},	{ 11,12,2},	{1,1,249},	  -570, 34	}, 
    {	215,	{10,20,22125},	{ 11,12,4},	{1,1,249},	  -573, 35	}, 
    {	216,	{10,20,22250},	{ 11,12,4},	{1,1,249},	  -576, 35	},
    {	217,	{10,20,22375},	{ 11,12,6},	{1,1,249},	  -579, 35	}, 
    {	218,	{10,20,22500},	{ 11,12,6},	{1,1,249},	  -582, 35	}, 
    {	219,	{10,20,22625},	{ 10,14,0},	{1,1,249},	  -585, 35	}, 
    {	220,	{10,20,22750},	{ 10,14,0},	{1,1,249},	  -588, 35	}, 
    {	221,	{10,20,22875},	{ 10,14,2},	{1,1,249},	  -591, 36	}, 
    {	222,	{10,20,23000},	{ 10,14,2},	{1,1,249},	  -594, 36	}, 
    {	223,	{10,20,23125},	{ 12,12,0},	{1,1,249},	  -597, 36	}, 
    {	224,	{10,20,23250},	{ 12,12,0},	{1,1,249},	  -600, 36	}, 
    {	225,	{10,20,23375},	{ 10,14,6},	{1,1,249},	  -602, 36	}, 
    {	226,	{10,20,23500},	{ 10,14,6},	{1,1,249},	  -604, 36	}, 
    {	227,	{10,20,23625},	{ 10,14,8},	{1,1,249},	  -606, 37	}, 
    {	228,	{10,20,23550},	{ 10,14,8},	{1,1,249},	  -608, 37	}, 
    {	229,	{10,20,23875},	{ 10,15,0},	{1,1,249},	  -610, 37	}, 
    {	230,	{10,20,24000},	{ 10,15,0},	{1,1,249},	  -612, 37	}, 
    {	231,	{10,20,24125},	{ 10,15,2},	{1,1,249},	  -614, 37	}, 
    {	232,	{10,20,24250},	{ 10,15,2},	{1,1,249},	  -616, 37	}, 
    {	233,	{10,20,24375},	{ 11,14,0},	{1,1,249},	  -618, 38	}, 
    {	234,	{10,20,24400},	{ 11,14,0},	{1,1,249},	  -620, 38	}, 
    {	235,	{10,20,24525},	{ 11,14,2},	{1,1,249},	  -622, 38	}, 
    {	236,	{10,20,24650},	{ 11,14,2},	{1,1,249},	  -624, 38	}, 
    {	237,	{10,20,24775},	{ 11,14,4},	{1,1,249},	  -626, 38	},
    {	238,	{10,20,24900},	{ 11,14,4},	{1,1,249},	  -628, 38	},
    {	239,	{10,20,25025},	{ 11,14,6},	{1,1,249},	  -630, 39	}, 
    {	240,	{10,20,25150},	{ 11,14,6},	{1,1,249},	  -632, 39	}, 
    {	241,	{10,20,25275},	{ 11,14,8},	{1,1,249},	  -634, 39	}, 
    {	242,	{10,20,25400},	{ 10,16,2},	{1,1,249},	  -636, 39	}, 
    {	243,	{10,20,25525},	{ 10,16,4},	{1,1,249},	  -638, 39	}, 
    {	244,	{10,20,25650},	{ 10,16,4},	{1,1,249},	  -640, 39	}, 
    {	245,	{10,20,25775},	{ 10,16,6},	{1,1,249},	  -642, 40	}, 
    {	246,	{10,20,25800},	{ 10,16,6},	{1,1,249},	  -644, 40	}, 
    {	247,	{10,20,25900},	{ 10,16,8},	{1,1,249},	  -646, 40	}, 
    {	248,	{10,20,26000},	{ 10,16,8},	{1,1,249},	  -648, 40	}, 
    {	249,	{10,20,26100},	{ 10,17,0},	{1,1,249},	  -650, 40	}, 
    {	250,	{10,20,26200},	{ 10,17,0},	{1,1,249},	  -652, 40	},
    {	251,	{15,15,26300},	{ 10,17,2},	{1,1,249},	  -654, 41	},
    {	252,	{15,15,26425},	{ 10,17,2},	{1,1,249},	  -656, 41	}, 
    {	253,	{15,15,26550},	{ 10,17,4},	{1,1,249},	  -658, 41	}, 
    {	254,	{15,15,26775},	{ 10,17,4},	{1,1,249},	  -660, 41	}, 
    {	255,	{15,15,26795},	{ 10,17,4},	{1,1,249},	  -661, 41	},
    {	256,	{15,15,26800},	{ 10,17,6},	{1,1,249},	  -662, 41	}, 
    {	257,	{15,15,26925},	{ 10,17,6},	{1,1,249},	  -664, 41	}, 
    {	258,	{15,15,27150},	{ 10,17,8},	{1,1,249},	  -666, 42	}, 
    {	259,	{15,15,27175},	{ 10,15,28},	{1,1,249},	  -668, 42	}, 
    {	260,	{15,15,27300},	{ 10,15,30},	{1,1,249},	  -670, 42	},
    {	261,	{15,15,27425},	{ 10,15,30},	{1,1,249},	  -672, 42	},
    {	262,	{15,15,27550},	{ 10,15,32},	{1,1,249},	  -674, 42	}, 
    {	263,	{15,15,27675},	{ 10,18,32},	{1,1,249},	  -676, 42	}, 
    {	264,	{15,15,27800},	{ 10,15,34},	{1,1,249},	  -678, 43	}, 
    {	265,	{15,15,27925},	{ 10,15,34},	{1,1,249},	  -680, 43	}, 
    {	266,	{15,15,28050},	{ 10,15,36},	{1,1,249},	  -682, 43	}, 
    {	267,	{15,15,28175},	{ 10,15,36},	{1,1,249},	  -684, 43	}, 
    {	268,	{15,15,28300},	{ 10,15,38},	{1,1,249},	  -686, 43	}, 
    {	269,	{15,15,28425},	{ 10,15,38},	{1,1,249},	  -688, 43	}, 
    {	270,	{15,15,28550},	{ 10,15,39},	{1,1,249},	  -689, 43	}, 
    {	271,	{15,15,28675},	{ 10,15,40},	{1,1,249},	  -690, 44	}, 
    {	272,	{15,15,29800},	{ 10,15,41},	{1,1,249},	  -692, 44	}, 
    {	273,	{15,15,28925},	{ 10,15,41},	{1,1,249},	  -694, 44	}, 
    {	274,	{15,15,29050},	{ 10,15,42},	{1,1,249},	  -696, 44	}, 
    {	275,	{15,15,29175},	{ 10,15,42},	{1,1,249},	  -698, 44	}, 
    {	276,	{15,15,29300},	{ 10,15,43},	{1,1,249},	  -700, 44	}, 
    {	277,	{15,15,29425},	{ 10,15,43},	{1,1,249},	  -702, 45	}, 
    {	278,	{15,15,29550},	{ 10,15,44},	{1,1,249},	  -704, 45	}, 
    {	279,	{15,15,29675},	{ 10,15,44},	{1,1,249},	  -706, 45	},
    {	280,	{15,15,29800},	{ 10,15,45},	{1,1,249},	  -708, 45	}, 
    {	281,	{15,15,29925},	{ 10,15,45},	{1,1,249},	  -710, 45	}, 
    {	282,	{15,15,29050},	{ 10,15,46},	{1,1,249},	  -712, 45	}, 
    {	283,	{15,15,30175},	{ 10,15,46},	{1,1,249},	  -714, 46	}, 
    {	284,	{15,15,30300},	{ 10,15,47},	{1,1,249},	  -716, 46	}, 
    {	285,	{15,15,30425},	{ 10,15,47},	{1,1,249},	  -718, 46	}, 
    {	286,	{15,15,30550},	{ 10,15,48},	{1,1,249},	  -720, 46	}, 
    {	287,	{15,15,30675},	{ 10,15,49},	{1,1,249},	  -722, 46	}, 
    {	288,	{15,15,30800},	{ 10,15,50},	{1,1,249},	  -724, 46	}, 
    {	289,	{15,15,20925},	{ 14,14,5},	{1,1,249},	  -726, 47	}, 
    {	290,	{15,15,31050},	{ 14,14,5},	{1,1,249},	  -728, 47	}, 
    {	291,	{15,15,31175},	{ 14,14,6},	{1,1,249},	  -730, 47	}, 
    {	292,	{15,15,31300},	{ 14,14,6},	{1,1,249},	  -732, 47	}, 
    {	293,	{15,15,31425},	{ 14,14,7},	{1,1,249},	  -734, 47	}, 
    {	294,	{15,15,31550},	{ 14,14,7},	{1,1,249},	  -736, 47	}, 
    {	295,	{15,15,31675},	{ 14,14,8},	{1,1,249},	  -738, 48	}, 
    {	296,	{15,15,31800},	{ 14,14,9},	{1,1,249},	  -740, 48	}, 
    {	297,	{15,15,31925},	{ 14,14,10},	{1,1,249},	  -742, 48	}, 
    {	298,	{15,15,32050},	{ 14,14,10},	{1,1,249},	  -744, 48	}, 
    {	299,	{15,15,32175},	{ 14,14,10},	{1,1,249},	  -746, 48	}, 
    {	300,	{15,15,32225},	{ 14,14,11},	{1,1,249},	  -748, 48	},
    {	301,	{20,20,32250},	{ 14,14,12},	{1,1,249},	  -750, 49	},
    {	302,	{20,20,32375},	{ 14,14,12},	{1,1,249},	  -752, 49	}, 
    {	303,	{20,20,32500},	{ 14,14,13},	{1,1,249},	  -754, 49	}, 
    {	304,	{20,20,32635},	{ 14,14,14},	{1,1,249},	  -756, 49	}, 
    {	305,	{20,20,32800},	{ 14,14,15},	{1,1,249},	  -758, 49	}, 
    {	306,	{20,20,32950},	{ 14,14,16},	{1,1,249},	  -760, 49	}, 
    {	307,	{20,20,33100},	{ 14,14,17},	{1,1,249},	  -762, 50	}, 
    {	308,	{20,20,33250},	{ 14,14,18},	{1,1,249},	  -764, 50	}, 
    {	309,	{20,20,33400},	{ 14,14,19},	{1,1,249},	  -766, 50	}, 
    {	310,	{20,20,33550},	{ 14,14,20},	{1,1,249},	  -768, 50	}, 
    {	311,	{20,20,33700},	{ 14,14,21},	{1,1,249},	  -770, 50	}, 
    {	312,	{20,20,33850},	{ 14,14,22},	{1,1,249},	  -772, 50	}, 
    {	313,	{20,20,34000},	{ 14,14,23},	{1,1,249},	  -774, 51	}, 
    {	314,	{20,20,34300},	{ 14,14,25},	{1,1,249},	  -776, 51	},
    {	315,	{20,20,34300},	{ 14,14,25},    {1,1,249},	  -778, 51	},
    {	316,	{20,20,34450},	{ 14,14,26},	{1,1,249},	  -780, 51	}, 
    {	317,	{20,20,34600},	{ 14,14,27},	{1,1,249},	  -782, 51	}, 
    {	318,	{20,20,34750},	{ 14,14,28},	{1,1,249},	  -784, 51	}, 
    {	319,	{20,20,35050},	{ 15,15,1 },	{1,1,249},	  -786, 52	},
    {	320,	{20,20,35050},	{ 15,15,1},     {1,1,249},	  -788,	52	},
    {	321,	{20,20,35200},	{ 15,15,2},	{1,1,249},	  -790, 52	},
    {	322,	{20,20,35350},	{ 15,15,3},	{1,1,249},	  -792, 52	},
    {	323,	{20,20,35500},	{ 15,15,4},	{1,1,249},	  -794, 52	}, 
    {	324,	{20,20,35650},	{ 15,15,5},	{1,1,249},	  -796, 52	}, 
    {	325,	{20,20,35800},	{ 15,15,6},	{1,1,249},	  -798, 53	}, 
    {	326,	{20,20,35950},	{ 15,15,7},	{1,1,249},	  -800, 53	}, 
    {	327,	{20,20,36100},	{ 15,15,8},	{1,1,249},	  -802, 53	}, 
    {	328,	{20,20,36250},	{ 15,15,9},	{1,1,249},	  -804, 53	}, 
    {	329,	{20,20,36400},	{ 15,15,10},	{1,1,249},	  -806, 53	}, 
    {	330,	{20,20,36550},	{ 15,15,11},	{1,1,249},	  -808, 53	}, 
    {	331,	{20,20,36700},	{ 15,15,12},	{1,1,249},	  -810, 54	}, 
    {	332,	{20,20,36850},	{ 15,15,13},	{1,1,249},	  -812, 54	}, 
    {	333,	{20,20,37000},	{ 15,15,14},	{1,1,249},	  -814, 54	}, 
    {	334,	{20,20,37150},	{ 15,15,15},	{1,1,249},	  -816, 54	}, 
    {	335,	{20,20,37300},	{ 15,15,16},	{1,1,249},	  -818, 54	}, 
    {	336,	{20,20,37450},	{ 15,15,17},	{1,1,249},	  -820, 54	},
    {	337,	{20,20,37600},	{ 15,15,18},	{1,1,249},	  -822, 55	},
    {	338,	{20,20,37750},	{ 15,15,19},	{1,1,249},	  -824, 55	}, 
    {	339,	{20,20,37900},	{ 15,15,20},	{1,1,249},	  -826, 55	}, 
    {	340,	{20,20,38050},	{ 15,15,21},	{1,1,249},	  -828, 55	}, 
    {	341,	{20,20,38200},	{ 15,15,22},	{1,1,249},	  -830, 55	}, 
    {	342,	{20,20,38350},	{ 15,15,23},	{1,1,249},	  -832, 55	}, 
    {	343,	{20,20,38500},	{ 15,15,24},	{1,1,249},	  -834, 56	}, 
    {	344,	{20,20,38650},	{ 15,15,25},	{1,1,249},	  -836, 56	}, 
    {	345,	{20,20,38800},	{ 15,15,26},	{1,1,249},	  -838, 56	}, 
    {	346,	{20,20,38950},	{ 15,15,27},	{1,1,249},	  -840, 56	}, 
    {	347,	{20,20,39100},	{ 15,15,28},	{1,1,249},	  -842, 56	}, 
    {	348,	{20,20,39250},	{ 15,15,29},	{1,1,249},	  -844, 56	}, 
    {	349,	{20,20,39400},	{ 15,15,30},	{1,1,249},	  -846, 57	}, 
    {	350,	{20,20,39550},	{ 15,15,40},	{1,1,249},	  -848, 57	},
    {	351,	{20,20,39700},	{ 10,10,157},	{1,1,249},	  -850, 57	}, 
    {	352,	{20,20,39850},	{ 10,10,158},	{1,1,249},	  -852, 57	},
    {	353,	{20,20,40000},	{ 10,10,159},	{1,1,249},	  -854, 57	},
    {	354,	{20,20,40150},	{ 10,10,160},	{1,1,249},	  -856, 57	}, 
    {	355,	{20,20,40300},	{ 10,10,161},	{1,1,249},	  -858, 58	}, 
    {	356,	{20,20,40450},	{ 10,10,162},	{1,1,249},	  -860, 58	}, 
    {	357,	{20,20,40600},	{ 10,10,163},	{1,1,249},	  -862, 58	}, 
    {	358,	{20,20,40750},	{ 10,10,164},	{1,1,249},	  -864, 58	}, 
    {	359,	{20,20,40900},	{ 10,10,165},	{1,1,249},	  -866, 58	}, 
    {	360,	{20,20,41050},	{ 10,10,166},	{1,1,249},	  -868, 58	},
    {	361,	{20,20,41200},	{ 10,10,167},	{1,1,249},	  -870, 59	},
    {	362,	{20,20,41350},	{ 10,10,168},	{1,1,249},	  -872, 59	}, 
    {	363,	{20,20,41500},	{ 10,10,169},	{1,1,249},	  -874, 59	}, 
    {	364,	{20,20,41650},	{ 10,10,170},	{1,1,249},	  -876, 59	}, 
    {	365,	{20,20,41800},	{ 15,15,51},	{1,1,249},	  -878, 59	}, 
    {	366,	{20,20,41950},	{ 15,15,52},	{1,1,249},	  -880, 59	}, 
    {	367,	{20,20,42100},	{ 15,15,53},	{1,1,249},	  -882, 60	}, 
    {	368,	{20,20,42250},	{ 15,15,54},	{1,1,249},	  -884, 60	}, 
    {	369,	{20,20,42400},	{ 15,15,55},	{1,1,249},	  -886, 60	}, 
    {	370,	{20,20,42550},	{ 15,15,56},	{1,1,249},	  -888, 60	}, 
    {	371,	{20,20,42700},	{ 15,15,57},	{1,1,249},	  -890, 60	}, 
    {	372,	{20,20,42850},	{ 15,15,58},	{1,1,249},	  -892, 60	}, 
    {	373,	{20,20,43000},	{ 15,15,59},	{1,1,249},	  -894, 61	}, 
    {	374,	{20,20,43150},	{ 15,15,60},	{1,1,249},	  -896, 61	}, 
    {	375,	{20,20,43300},	{ 15,15,61},	{1,1,249},	  -898, 61	}, 
    {	376,	{20,20,43450},	{ 15,15,62},	{1,1,249},	  -890, 61	}, 
    {	377,	{20,20,43600},	{ 15,15,63},	{1,1,249},	  -892, 61	}, 
    {	378,	{20,20,43750},	{ 15,15,64},	{1,1,249},	  -894, 61	}, 
    {	379,	{20,20,43900},	{ 15,15,65},	{1,1,249},	  -896, 62	}, 
    {	380,	{20,20,44050},	{ 15,15,66},	{1,1,249},	  -898, 62	},
    {	381,	{20,20,44200},	{ 15,15,67},	{1,1,249},	  -890, 62	},
    {	382,	{20,20,44350},	{ 15,15,68},	{1,1,249},	  -892, 62	}, 
    {	383,	{20,20,44400},	{ 15,15,69},	{1,1,249},	  -894, 62	}, 
    {	384,	{20,20,44550},	{ 15,15,70},	{1,1,249},	  -896, 62	}, 
    {	385,	{20,20,44675},	{ 15,15,71},	{1,1,249},	  -897, 62	},
    {	386,	{20,20,44850},	{ 15,15,72},	{1,1,249},	  -898, 63	}, 
    {	387,	{20,20,45000},	{ 15,15,73},	{1,1,249},	  -890, 63	}, 
    {	388,	{20,20,45150},	{ 15,15,74},	{1,1,249},	  -892, 63	}, 
    {	389,	{20,20,45300},	{ 15,15,75},	{1,1,249},	  -894, 63	}, 
    {	390,	{20,20,45450},	{ 15,15,76},	{1,1,249},	  -896, 63	}, 
    {	391,	{20,20,45600},	{ 15,15,77},	{1,1,249},	  -898, 63	}, 
    {	392,	{20,20,45750},	{ 15,15,78},	{1,1,249},	  -900, 64	}, 
    {	393,	{20,20,45900},	{ 15,15,79},	{1,1,249},	  -902, 64	},
    {	394,	{20,20,46200},	{ 15,15,81},	{1,1,249},	  -904, 64	}, 
    {	395,	{20,20,46200},	{ 15,15,81},    {1,1,249},	  -906, 64	},
    {	396,	{20,20,46350},	{ 15,15,82},	{1,1,249},	  -908, 64	}, 
    {	397,	{20,20,46500},	{ 15,15,83},	{1,1,249},	  -910, 64	},
    {	398,	{20,20,46650},	{ 15,15,84},	{1,1,249},	  -912, 65	}, 
    {	399,	{20,20,46800},	{ 15,15,85},	{1,1,249},	  -914, 65	}, 
    {	400,	{20,20,47100},	{ 15,15,86},	{1,1,249},	  -916, 65	},
    {	401,	{20,20,47500},	{ 15,15,90},	{1,1,249},	  -918, 65	},

    {	0,	{0,0,0},	{0,0,0},	{0,0,0},	0,	0	}
};


const	struct	weapon_val_type	weapon_values[] =
{
    /*	Class			#dice	#sides	#bonus	dam	*/
    /*				V1	V2	V5	V3	*/
    {	WEAPON_EXOTIC,		1,	6,	2,	0	},
    {	WEAPON_SWORD,		1,	6,	2,	2	},
    {	WEAPON_DAGGER,		1,	4,	2,	11	},
    {	WEAPON_SPEAR,		1,	8,	3,	2	},
    {	WEAPON_MACE,		1,	6,	2,	7	},
    {	WEAPON_AXE,		1,	6,	3,	25	},
    {	WEAPON_FLAIL,		1,	8,	2,	4	},
    {	WEAPON_WHIP,		1,	2,	2,	4	},
    {	WEAPON_POLEARM,		1,	10,	4,	27	},
    {	WEAPON_HAMMER,		1,	8,	3,	7	},
    {	WEAPON_LONGSWORD,	1,	8,	2,	3	},
    {	WEAPON_STAFF,		1,	6,	2,	7	},
    {	WEAPON_STILETTO,	1,	4,	2,	11	},
    {	WEAPON_CLUB,		1,	6,	2,	7	},
    {	WEAPON_SCIMITAR,	2,	4,	2,	1	},
    {	WEAPON_SHORTSWORD,	1,	6,	2,	2	},
    {	WEAPON_BASTARDSWORD,	1,	10,	2,	2	},
    {	WEAPON_TWOHANDSWORD,	1,	6,	2,	3	},
    {	WEAPON_DIRK,		1,	4,	2,	11	},
    {	WEAPON_LANCE,		1,	8,	3,	15	},
    {	WEAPON_GREATMACE,	1,	8,	2,	27	},
    {	WEAPON_GREATAXE,	1,	12,	3,	21	},
    {	WEAPON_HORSEMACE,	1,	8,	2,	7	},
    {	WEAPON_HORSEFLAIL,	1,	10,	2,	4	},
    {	WEAPON_BOW,		1,	8,	3,	11	},
    {	WEAPON_RAPIER,		1,	6,	2,	1	},
    {	WEAPON_SABRE,		1,	6,	2,	3	},

    {	WEAPON_BROADSWORD,	1,	10,	2,	3	},
    {	WEAPON_CROSSBOW,	1,	8,	3,	11	},
    {	WEAPON_NONE,		0,	0,	0,	0	}
};


/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
    { -5, -4,   0,    0 },  /* 0  */
    { -5, -4,   3,  160 },  /* 1  */
    { -3, -2,   3,  320 },
    { -3, -1,  10,  480 },  /* 3  */
    { -2, -1,  25,  640 },
    { -2, -1,  55,  800 },  /* 5  */
    { -1,  0,  80,  960 },
    { -1,  0,  90, 1120 },
    {  0,  0, 100, 1280 },
    {  0,  0, 100, 1440 },
    {  0,  0, 115, 1600 }, /* 10  */
    {  0,  0, 115, 1760 },
    {  0,  0, 130, 1920 },
    {  0,  0, 130, 2080 }, /* 13  */
    {  0,  1, 140, 2240 },
    {  1,  1, 150, 2400 }, /* 15  */
    {  1,  2, 165, 2560 },
    {  2,  3, 180, 3520 },
    {  2,  3, 200, 4000 }, /* 18  */
    {  3,  4, 225, 4800 },
    {  3,  5, 250, 5600 }, /* 20  */
    {  4,  6, 300, 6400 },
    {  4,  6, 350, 7200 },
    {  5,  7, 400, 8000 },
    {  5,  8, 450, 8800 },
    {  6,  9, 500, 9600 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 80 },
    { 85 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 1 },	/* 10 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 2 },	/* 15 */
    { 2 },
    { 2 },
    { 3 },	/* 18 */
    { 3 },
    { 3 },	/* 20 */
    { 3 },
    { 4 },
    { 4 },
    { 4 },
    { 5 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};


const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};


/*
 * Kingdom table
 */
const   struct  kingdom_type    kingdom_table[] =
{
    {   KINGDOM_NONE,        'w',    906,      0,   "none",           "Outlying Areas"          },
    {   KINGDOM_BLACKMERE,   'Y',    906,    917,   "blackmere",      "Blackmere - Human"       },
    {   KINGDOM_EVERVALE,    'g',    906,    917,   "evervale",        "Evervale - Elven"       },
    {   KINGDOM_WINTERGARDE, 'G',    906,    917,  "wintergarde",     "Wintergarde - Dwarven"  },
    {   KINGDOM_DRUUNA,      'R',    906,    917,  "druuna",          "Druuna - Orcish"        },
    {   KINGDOM_AMBERFALL,   'W',    906,    917,   "amberfall",       "Amberfall - Tiefling"      },
    {   KINGDOM_TINKERWICK,  'B',    906,    917,  "tinkerwick",      "Tinkerwick - Gnomish"   },
    {   KINGDOM_GHOSTWALLOW, 'r',    906,    917,   "ghostwallow",     "Ghostwallow - Drow"     },
    {   KINGDOM_NONE,   ' ',    0,      0,      NULL,           NULL                            }
};

/*
 * Landmass names
 */
const	struct	landmass_name_types	land_name_table[] =
{
    {	LAND_NONE,	"a different plane of existence"	},
    {	LAND_OVERLAND,	"the world beyond"		},
    {	LAND_GRAVMERE,	"the continent of Gravmere"	},
    {	LAND_TINKERTON,	"the island of Tinkerton"	},
    {	LAND_DARK_SANDS,	"the Dark Sands"	},
    {	LAND_BLACK_ISLE,	"the Black Isle"	}
};


/*
 * Landmass table
 */
const	struct	landmass_type	landmass_table[] =
{
    {	LAND_GRAVMERE,	8,	122,	314,	224,	FALSE	},
    {	LAND_TINKERTON,	436,	75,	691,	191,	FALSE	},
    {	LAND_DARK_SANDS,	705,	84,	1006,	189,	FALSE	},
    {	LAND_BLACK_ISLE,	361,	361,	531,	495,	FALSE	},
    {	LAND_OVERLAND,	0,	0,	0,	0,	FALSE	},
    {	LAND_NONE,	0,	0,	0,	0,	FALSE	}
};


/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[]	=
{
/*    name			color	proof, full, thirst, food, ssize */
    { "water",			"clear",	{   0, 0, 10, 0, 16 }	},
    { "beer",			"amber",	{  12, 1,  8, 1, 12 }	},
    { "red wine",		"burgundy",	{  30, 1,  8, 1,  5 }	},
    { "ale",			"brown",	{  15, 1,  8, 1, 12 }	},
    { "dark ale",		"dark",		{  16, 1,  8, 1, 12 }	},

    { "whisky",			"golden",	{ 120, 1,  5, 0,  2 }	},
    { "lemonade",		"pink",		{   0, 1,  9, 2, 12 }	},
    { "firebreather",		"boiling",	{ 190, 0,  4, 0,  2 }	},
    { "local specialty",	"clear",	{ 151, 1,  3, 0,  2 }	},
    { "slime mold juice",	"green",	{   0, 2, -8, 1,  2 }	},

    { "milk",			"white",	{   0, 2,  9, 3, 12 }	},
    { "tea",			"tan",		{   0, 1,  8, 0,  6 }	},
    { "coffee",			"black",	{   0, 1,  8, 0,  6 }	},
    { "blood",			"red",		{   0, 2, -1, 2,  6 }	},
    { "salt water",		"clear",	{   0, 1, -2, 0,  1 }	},

    { "grog",			"pale brown",	{  12, 1,  9, 1, 12 }	}, 
    { "root beer",		"brown",	{   0, 2,  9, 2, 12 }   },
    { "elvish wine",		"green",	{  35, 2,  8, 1,  5 }   },
    { "white wine",		"golden",	{  28, 1,  8, 1,  5 }   },
    { "champagne",		"golden",	{  32, 1,  8, 1,  5 }   },

    { "mead",			"honey-colored",{  34, 2,  8, 2, 12 }   },
    { "rose wine",		"pink",		{  26, 1,  8, 1,  5 }	},
    { "benedictine wine",	"burgundy",	{  40, 1,  8, 1,  5 }   },
    { "vodka",			"clear",	{ 130, 1,  5, 0,  2 }   },
    { "cranberry juice",	"red",		{   0, 1,  9, 2, 12 }	},

    { "orange juice",		"orange",	{   0, 2,  9, 3, 12 }   }, 
    { "absinthe",		"green",	{ 200, 1,  4, 0,  2 }	},
    { "brandy",			"golden",	{  80, 1,  5, 0,  4 }	},
    { "aquavit",		"clear",	{ 140, 1,  5, 0,  2 }	},
    { "schnapps",		"clear",	{  90, 1,  5, 0,  2 }   },

    { "icewine",		"purple",	{  50, 2,  6, 1,  5 }	},
    { "amontillado",		"burgundy",	{  35, 2,  8, 1,  5 }	},
    { "sherry",			"red",		{  38, 2,  7, 1,  5 }   },	
    { "framboise",		"red",		{  50, 1,  7, 1,  5 }   },
    { "rum",			"amber",	{ 151, 1,  4, 0,  2 }	},

    { "cordial",		"clear",	{ 100, 1,  5, 0,  2 }   },
    { "wine",			"rose",		{  26, 1,  8, 1,  5 }	},
    { "brandywine",		"maroon",	{  75, 1,  8, 1,  4 }	},
    { "hotcoco",                "dark",         {  20, 1, 20, 0, 10 }   },
    { "honey",                  "golden",       {  0, 1, 20, 20,10 }   },
    { NULL,			NULL,		{   0, 0,  0, 0,  0 }	}
};
char *const tick_table[] =
{
  "TICK!\n\r",
  "A bell tolls from somewhere in the distance...\n\r",
  "You feel as if someone is watching you.\n\r",
  "What was that?!\n\r",
  "Fog encircles your feet for a moment and then vanishes as quickly as it appeared.\n\r",
  "You grow momentarily dizzy.\n\r",
  "Water drips from somewhere in the distance.\n\r",
  "Your bag feels heavy on your shoulder.\n\r",
  "It feels as if bugs are crawling on your skin for a moment, and then stops.\n\r",
  "An ominous rumble vibrates the ground and then stops.\n\r",
  "The ground rumbles slightly.\n\r",
  "You notice something moving out of the corner of your eye.\n\r",            
  "It suddenly seems eerily quiet.\n\r",
  "Someone vanishes!\n\r",
  "Shadows flicker at the corner of your vision.\n\r",
  "Look out behind you!\n\r",
  "`XA Thief says '`GDon't ask me! It was there a second ago!`W'\n\r",          
  "A Bell tolls somewhere in the distance.\n\r",
  "You hear whispering from somewhere nearby.\n\r",
  "You feel reality ripple around you.\n\r",

  
   NULL
};

const	struct	quest_type	quest_table	[ ] =
{
/*    vnum    qp cost	*/
    {	200,	10000    },
    {	201,	500    },
    {	202,	500    },
    {	203,	1000    },
    {	204,	250    },
    {	205,	250    },
    {   206,    1000    },
    {   207,    100    },
    {   208,    500    },
    {   209,    500    },
    {   210,    250    },
    {   211,    1000    },
    {   212,    1000    },
    {   213,    1000    },
    {   214,    1000    },
    {   215,    750    },
    {   216,    1000    },
    {   217,    500    },
    {   218,    1000    },
    {   219,    5000    },
    {   220,    200    },
    {   221,    500    },
    {   222,    1000    },
    {   223,    10000    },
    {   224,    10000    },
    {   225,    10000    },
    {   226,    10000    },
    {   227,    15000    },
    {   228,    15000    },
    {   234,    100000    },
    {   235,    10000    },
    {   236,    5000    },
    {   237,    2000    },
    {   238,    5000    },
    {	0,	0	}
};


struct sector_data_types	sector_data	[ ] =
{
/*  {	Sector type	Name		Desc		movement loss
	color index	RGB red		RGB green	RGB blue
	Elevation	Color char	Symbol		HTML color string
    }
*/
    {	SECT_INSIDE,	"inside",	"inside",	1,	AT_WHITE,
	0,		0x7F,		0x7F,		0x7F,
	ELEV_INSIDE,	'z',		'-',		"\"#7F7F7F\""
    },

    {	SECT_CITY,	"city",		"city",		2,	AT_WHITE,
	0,		0xFF,		0x00,		0x00,
	ELEV_CITY,	'w',		'O',		"\"#A0A0A0\""
    },

    {	SECT_FIELD,	"field",	"fields",	2,	AT_WHITE,
	0,		0x66,		0xFF,		0x00,
	ELEV_FIELD,	'G',		'\"',		"\"#66FF00\""
    },

    {	SECT_FOREST,	"forest",	"forest",	3,	AT_WHITE,
	0,		0x33,		0x99,		0x00,
	ELEV_FOREST,	'g',		'@',		"\"#339900\""
    },

    {	SECT_HILLS,	"hills",	"hills",	4,	AT_WHITE,
	0,		0xFF,		0xCC,		0x00,
	ELEV_HILLS,	'Y',		'm',		"\"#FFFF00\""
    },

    {	SECT_MOUNTAIN,	"mountain",	"mountains",	6,	AT_WHITE,
	0,		0xCC,		0xCC,		0xCC,
	ELEV_MOUNTAIN,	'W',		'^',		"\"#CCCCCC\""
    },

    {	SECT_WATER_SWIM, "swim",	"river",	4,	AT_WHITE,
	0,		0x00,		0x70,		0xE0,
	ELEV_WATER_SWIM, 'B',		'-',		"\"#0066CC\""
    },

    {	SECT_WATER_NOSWIM, "noswim",	"deep water",	1,	AT_WHITE,
	0,		0x00,		0x3F,		0xD0,
	ELEV_WATER_NOSWIM, 'B',		'w',		"\"#0066CC\""
    },

    {	SECT_UNDERWATER, "underwater",	"underwater",	6,	AT_WHITE,
	0,		0x00,		0x0F,		0xC0,
	ELEV_WATER_NOSWIM, 'B',		'v',		"\"#0066CC\""
    },

    {	SECT_AIR,	"air",		"air",		10,	AT_WHITE,
	0,		0x33,		0xFF,		0xFF,
	ELEV_AIR,	'C',		':',		"\"#33FFFF\""
    },

    {	SECT_DESERT,	"desert",	"the desert",	4,	AT_WHITE,
	0,		0xFF,		0xE0,		0x00,
	ELEV_DESERT,	'Y',		'\\',		"\"#FFCC00\""
    },

    {	SECT_SWAMP,	"swamp",	"the swamp",	6,	AT_WHITE,
	0,		0x33,		0x99,		0x99,
	ELEV_SWAMP,	'g',		'%',		"\"#339900\""
    },

    {	SECT_ROAD,	"road",		"road",		1,	AT_WHITE,
	0,		0x7F,		0x66,		0x00,
	ELEV_ROAD,	'O',		'+',		"\"#FF66CC\""
    },

    {	SECT_OCEAN,	"ocean",	"the ocean",	1,	AT_WHITE,
	0,		0x0F,		0x7F,		0x9F,
	ELEV_WATER_OCEAN, 'b',		'W',		"\"#0066CC\""
    },

    {	SECT_UNDERGROUND,"underground",	"underground",	6,	AT_WHITE,
	0,		0x00,		0x00,		0x00,
	ELEV_NONE,	'z',		' ',		"\"#000000\""
    },

    {	SECT_GRAVEYARD,	"graveyard",	"a graveyard",	4,	AT_WHITE,
	0,		0x3F,		0x3F,		0x3F,
	ELEV_NONE,	'w',		'+',		"\"#000000\""
    },

    {	SECT_TREECITY,	"treecity",	"treecity",	0,	AT_WHITE,
	0,		0xDF,		0x1F,		0x00,
	ELEV_NONE,	'z',		' ',		"\"#000000\""
    },

    {	SECT_ELFFOREST,	"forest",	"forest",	3,	AT_WHITE,
	0,		0x1F,		0x8F,		0x00,
	ELEV_NONE,	'z',		' ',		"\"#000000\""
    },

    {	SECT_JUNGLE,	"jungle",	"jungle",	12,	AT_WHITE,
	0,		0x3F,		0x8F,		0x00,
	ELEV_NONE,	'z',		' ',		"\"#000000\""
    },

    {	SECT_ARCTIC,	"arctic",	"arctic",	4,	AT_WHITE,
	0,		0xFF,		0xFF,		0xFF,
	ELEV_NONE,	'W',		'=',		"\"#FFFFFF\""
    },

    {	SECT_BEACH,	"beach",	"beach",	2,	AT_WHITE,
	0,		0xFF,		0xE0,		0x3F,
	ELEV_NONE,	'Y',		'.',		"\"#000000\""
    },

    {
        SECT_BOILING,	"boiling",	"the boiling ocean", 1, AT_WHITE,
        0,		0x72,		0xEB,		0xFE,
        ELEV_NONE,	'C',		'W',		"\"#72EBFE\""
    },

    {	SECT_LAKE, 	"lake",		"lake",	4,	AT_WHITE,
	0,		0x00,		0x4F,		0xD4,
	ELEV_WATER_SWIM, 'B',		'-',		"\"#0066CC\""
    },

    {	SECT_RIVER,	"river",	"river",	4,	AT_WHITE,
	0,		0x00,		0x5F,		0xDC,
	ELEV_WATER_SWIM, 'B',		'-',		"\"#0066CC\""
    },

    {
	SECT_WASTELAND,	"wasteland",	"wastelands",	6,	AT_WHITE,
	0,		0x80,		0x00,		0x00,
	ELEV_WASTELAND,	'r',		'#',		"\"#800000\""
    },

    {	SECT_NONE,	NULL,		NULL,		0,	AT_WHITE,
	0,		0x00,		0x00,		0x00,
	ELEV_NONE,	'z',		' ',		"\"#000000\""
    }
};


const	struct	phys_attr_type	hair_color_table[] =
{
/*  {	"name",			         " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " }, */
    {	"auburn hair",		     "                  Elf Gnome Hlflf Hflng Hforc Human Tflng"},
    {	"bald head",		     "       Drow Dwarf Elf       Hlflf Hflng Hforc Human       " },
    {	"black hair",		     "       Drow Dwarf     Gnome             Hforc Human Tflng " },
    {	"blonde hair",		     "                  Elf Gnome Hlflf Hflng Hforc Human       " },
    {	"blue hair",		     "                  Elf                               Tflng " },
    {	"brown hair",	     	 "       Drow Dwarf Elf       Hlflf Hflng Hforc Human Tflng " },
    {	"golden hair",		     "                  Elf       Hlflf             Human       " },
    {	"green hair",		     "                  Elf                   Hforc             " },
    {	"grey hair",		     "       Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {	"mahogany hair",	     "            Dwarf Elf       Hlflf Hflng       Human Tflng " },
    {	"no hair",		         "       Drow                                         Tflng " },
    {	"orange hair",		     "                      Gnome       Hflng       Human Tflng " },
    {	"red hair",		         "            Dwarf     Gnome Hlflf Hflng Hforn Human Tflng " },
    {	"salt and pepper hair",	 "            Dwarf                 Hflng Hforc Human       " },
    {	"silver hair",		     "       Drow       Elf       Hlflf Hflng       Human       " },
    {	"strawberry blonde hair","                  Elf       Hlflf Hflng       Human       " },
    {	"yellow hair",		     "                      Gnome                               " },
    {	"white hair",		     "       Drow       Elf       Hlflf Hflng Hforc Human       " },
    {	"black scales",		     " Dgnbn                                                    " },
    {	"blue scales",		     " Dgnbn                                                    " },
    {	"brass scales",	         " Dgnbn                                                    " },
    {	"bronze scales",	     " Dgnbn                                                    " },
    {	"copper scales",	     " Dgnbn                                                    " },
    {	"gold scales",	       	 " Dgnbn                                                    " },
    {	"green scales",	       	 " Dgnbn                                                    " },
    {	"red scales",	       	 " Dgnbn                                                    " },
    {	"silver scales",	     " Dgnbn                                                    " },
    {	"white scales",	       	 " Dgnbn                                                    " },
    {   "no scales",		     "                                                          " },
    {	"creme fur",		     "                                                          " },
    {	"black fur",		     "                                                          " },
    {	"brown fur",		     "                                                          " },
    {	"grey fur",		         "                                                          " },
    {	"spotted fur",		     "                                                          " },
    {	"tiger-striped fur",	 "                                                          " },
    {	"auburn feathers",	     "                                                          " },
    {	"brown feathers",	     "                                                          " },
    {	"black feathers",	     "                                                          " },
    {	"white feathers"	     "                                                          " },
    {	"a crystal body",	     "                                                          " },
    {	"translucent layers",	 "                                                          " },
    { "rainbow-colored layers",  "                                                          " },
    {	"a crystal shell",	     "                                                          " },
    {	"",		"" }
};

const   struct  phys_attr_type  hair_adj_table[] =
{
/*  {    "adj",                  " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " }, */
    {"bright", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"curly", "  Dwarf Gnome Hlflf Hflng Hforc Human "},
    {"dark", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"dirty", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"dull", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"dyed", " Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"fine", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"greasy", "Dgnbn Dwarf Gnome Hlflf Hflng Hforc Human " },
    {"greying", "Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"light", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"natural", "Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"shiny", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"spiked", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"straight", " Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"thick", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"thinning", " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"unkempt", " Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"wavy", " Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"wild", " Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng " },
    {"", ""}			
};

const	struct	phys_attr_type	eye_color_table[] =
{
/*  {	"name",		"Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " }, */
    {	"amber",	"Dgnbn      Dwarf Elf Gnome Hlflf Hflng Hforc Human        " },
    {	"aqua",		"Dgnbn            Elf Gnome Hlflf                          " },
    {	"azure",	"Dgnbn            Elf Gnome Hlflf Hflng       Human        " },
    {	"black",	"Dgnbn Drow                                         Tflng  " },
    {	"brown",	"           Dwarf Elf Gnome Hlflf Hflng Hforc Human        " },
    {	"blue",		"           Dwarf                 Hflng Hforc Human        " },
    {	"emerald",	"Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human        " },
    {	"gold",		"Dgnbn            Elf       Hlflf                   Tflng  " },
    {	"green",	"Dgnbn      Dwarf Elf Gnome Hlflf Hflng Hforc Human        " },
    {	"grey",		"      Drow Dwarf           Hlflf       Hforc Human        " },
    {	"hazel",	"           Dwarf Elf Gnome Hlflf Hflng Hforc Human        " },
    {	"jade",		"Dgnbn                                  Hforc              " },
    {	"mercury",	"Dgnbn                                                     " },
    {	"orange",	"Dgnbn                                                     " },
    {	"red",		"Dgnbn Drow                             Hforc       Tflng  " },
    {	"reflective",	"                                                      " },
    {	"silver",	"      Drow                                         Tflng  " },
    {	"violet",	"      Drow       Elf      Hlflf                           " },
    {	"white",	"                                                   Tflng  " },
    {	"yellow",	"Dgnbn                                                     " },
    {	"colorless",	"                                                      " },
    {	NULL,		NULL	}
};


const	struct	phys_attr_type	eye_adj_table[] =
{
/*  {	"name",		    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " }, */
    {	"alert",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"beady",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"curious",   	" Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"dark",	        " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"dead",	        " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"deep",		    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"dull",	        " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"fiery",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"glaring",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"glassy",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"glazed",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"straight",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"glinting",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"icy",	        " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"light",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"mischevious",	" Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"mysterious",	" Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"narrowed",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"old",	        " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"piercing",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "sleepy",       " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	"smoky",	    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "smoldering",   " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "soulful",      " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "sparkling",    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "twinkling",    " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "unseeing",     " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "vibrant",      " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "wild",         " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "wise",         " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {   "young",        " Dgnbn Drow Dwarf Elf Gnome Hlflf Hflng Hforc Human Tflng  " },
    {	NULL,		NULL	}
};



int	wear_order	[] =
{
    WEAR_HEAD,
    WEAR_EAR_L,
    WEAR_EAR_R,
    WEAR_EYES,
    WEAR_FACE,
    WEAR_ABOUT,
    WEAR_NECK_1,
    WEAR_NECK_2,
    WEAR_SHOULDER,
    WEAR_CPATCH,
    WEAR_RPATCH,
    WEAR_ARMS,
    WEAR_HANDS,
    WEAR_WRIST_L,
    WEAR_WRIST_R,
    WEAR_FINGER_L,
    WEAR_FINGER_R,
    WEAR_BODY,
    WEAR_WAIST,
    WEAR_LEGS,
    WEAR_ANKLE_L,
    WEAR_ANKLE_R,
    WEAR_FEET,
    WEAR_SHIELD,
    WEAR_WIELD,
    WEAR_DUAL,
    WEAR_LIGHT,
    WEAR_HOLD,
    WEAR_FLOAT,
    WEAR_TAIL,
    WEAR_HORNS,
};


struct  group_type      group_table     [MAX_GROUP]     =
{
    {
	"rom basics",		{ },
	{ "club", "recall", "swimming", "skin", "butcher" }
    },

    {
	"cleric basics",
	{ 0, -1, -1, -1, -1 },
	{ "dagger" }
    },

   {
	"druid basics",
	{ -1, 0, -1, -1, -1  },
	{ "dagger" }
    },

    {
        "fighter basics",
        {  -1, -1, 0, -1, -1  },
        { "haggle", "kick"}
    },

     {
	"mage basics",
	{ -1, -1, -1, 0, -1  },
	{ "dagger" }
    },

    {
	"rogue basics",
	{ -1, -1, -1, -1,  0},
	{ "dagger", "steal" }
    },

    {
	"cleric default",
	{ 40, -1, -1, -1, -1  },
         { "mace", "hammer", "meditation", "divine domain",  "scrolls",
          "staves", "wands" }

    },

     {
	"druid default",
	{ -1, 40, -1, -1, -1  },
	{ "dirk", "hammer", "longsword", "mace", "rapier", "saber", 
	  "shortsword", "staff", "whip",  
	  "meditation", "scrolls", "staves", "wands", "triage", "herbal lore"}
    },

    {
	"fighter default",
	{ -1, -1, 40, -1, -1  },
	{ "axe", "dodge", "hammer", "hand to hand", "defensive fighting" }
    },

     {
	"mage default",
	{ -1, -1, -1, 40, -1  },
        { "dagger", "phase", "meditation", "scrolls", "evocation"
          "staves", "wands", "spellcraft", "lore" }

    },

     {
	"rogue default",
	{ -1, -1, -1, -1, 40  },
	{ "dagger", "stilleto", "rapier", "larceny"}
    },

    {
	"weaponsmaster",
	{ 40, 40, 40, 40, 20},
	{ "axe","bastard sword", "bow", "broadsword", "club", "dagger", "dirk", "flail", "great axe",
          "great mace", "hammer", "horse flail", "horse mace", "lance", "longsword", "mace", "polearm", 
          "rapier", "sabre", "scimitar", "shortsword", "spear","staff", "stilettor", "sword", "two-handed sword",
	  "whip" }
    },



 /*
  * Fighter Skills
  */

  {
	"defensive fighting",
	{ -1, -1, 10, -1, -1  },
	{"charge", "counter", "disarm", "dodge", "fast healing", "glance", "parry", "rescue", "shield block" }
    },

    {
	"offensive fighting",
	{ -1, -1, 10, -1, -1  },
	{"bash", "berserk", "dual", "enhanced damage", "kick", "second attack", "third attack", "fourth attack", 
	"fifth attack", "sharpen", "whirlwind" }
    },

 /*
  * Rogue skills
  */

    {
	"assassination",
	{ -1, -1, -1, -1, 10  },
	{"assassinate", "envenom", "hide", "sneak" }
    },

    {
	"dirty fighting",
	{ -1, -1, -1, -1, 10  },
	{"backstab", "circle", "dirt kicking", "kick", "trip", "blackjack" }
    },

    {
	"larceny",
	{ -1, -1, -1, -1, 10  },
	{"appraise", "disarm", "haggle", "peek", "pick lock", "steal", "scroll", "staves", "wand" }
    },

    {
	"rogue combat",
	{ -1, -1, -1, -1, 10  },
	{"counter", "dodge", "dual", "enhanced damage", "fast healing", "parry", "second attack",
	"third attack", "fourth attack" }
    },


/*                                                                                                                                     
 * Cleric Domains                                                                                                                      
 */                                                                                                                                    
                                                                                                                                       
{                                                                                                                                      
 "divine domain",                                                                                                                       
 { 10, -1, -1, -1, -1},                                                                                                                
 { "aid", "aura of peace", "bless", "calm", "celestial lightning",                                                                     
 "holy word", "ray of truth", "remove curse", "resurrect" }                                                                            
 },                                                                                                                                    
                                                                                                                                       
{
 "healing domain",
 { 10, -1, -1, -1, -1},
 { "cure blindness", "cure critical", "cure disease", "cure drunk",                                                                   
 "cure light", "cure poison", "cure serious", "full heal", "heal",                                                                    
 "healing hands", "mana", "mass healing", "refresh"}
 },
                                                                                                                                       

 {               
 "protection domain",       
 { 10, -1, -1, -1, -1},                                                              
 { "protection good", "protection evil", "sanctuary"}                                
 },                                                                        


 {                                                                                                                                     
 "strength domain",                                                                                                                     
 { 10, -1, -1, -1, -1},
 { "barrage", "bears endurance", "bulls strength", "cats grace", "fox cunning",              
   "frenzy", "owls wisdom", "strike true"}     
 },
                                                                                                                                       
 {                                                                                                                                    
 "death domain",
 { 10, -1, -1, -1, -1},
 { "cause critical", "cause light", "cause serious", "harm" }                                        
 },                                                                                                                                   
                                                                                                                                       
/*                                                                                                                                     
 * Druid Spells                                                                                                                        
 */                                                                                                                                    
                                                                                                                                       
{                                                                                                                                      
 "nature magic",                                                                                                                        
 { -1, 20, -1, -1, -1},                                                                                                                
 { "acid geyser", "animal summoning", "astral walk", "barkskin",                                                                       
   "bear claws", "bear fat", "call lightning", "chain lightning",                                                                      
   "control weather", "curse of nature", "dragonscales", "earthquake",                                                                 
   "faerie fire", "faerie fog", "goodberry", "hurricane", "infravision",
   "natural armor", "summon greater wolf", "summon hawk", "summon tiger",
   "summon wolf", "telekinetic force" }
 },                                                                                  

/*                                                                         
 * Mage Schools
 */
                                                                                                                                       
{                                                                                                                                      
 "abjuration",          
 { -1, -1, -1, 10, -1},                                                                      
 {"armor", "cancellation", "dispel evil", "dispel good", "dispel magic",
  "flame shield", "forceshield", "shield", "spellmantle", "static shield" }
 },

 {             
 "conjuration",         
 { -1, -1, -1, 10, -1},                                                                              
 {"acid breath", "fire breath", "frost breath", "gas breath", "lightning breath",                                                     
  "blade barrier", "continual light", "create food", "create rose", "create spring",                                                  
  "create water", "dancing boots", "disruption", "floating disc", "gate", "nexus",
  "portal", "summon", "summon fire elemental", "summon greater golem", "teleport",                                                    
  "word of recall" }
 },                                                                                                                                   
                                                                                                                                       
 {                                                                                                                                    
 "divination",                                                                                                                         
 { -1, -1, -1, 10, -1},
 { "detect auras", "detect evil", "detect life", "detect good", "detect hidden",                                                       
   "detect invis", "detect magic", "detect poison", "farsight", "identify",                                                            
   "know alignment", "locate object", "scry", "truesight" }                                                                            
 },                                                                                                                                    
                                                                                                                                       
                                                                                                                                       
 {                                                                                                                                     
 "enchantment",                                                                                                                         
 { -1, -1, -1, 10, -1},                                                                                                                
 {"charm person", "enchant armor", "enchant weapon", "fireproof", "recharge", "sleep" }                                                
 },                                                                                                                                    
                                                                                                                                       
{                                                                                                                                      
 "evocation",                                                                                                                           
 { -1, -1, -1, 10, -1},
 { "acid arrow", "acid blast", "acid rain", "burning hands", "caustic eruption", "chill touch",
   "colour spray", "cone of cold", "corrosive burst", "electrocute", "fireball",
   "flamestrike", "ion blast", "lightning bolt", "magic missile", "project force",
   "rushing fist", "shocking grasp" }
 },

                                                                                                                                      
 {                                                                                                                                    
 "illusion",                                                                                                                           
 { -1, -1, -1, 10, -1},
 {"blindness", "dancing lights", "daze", "deafen", "invis", "mass invis", "sleep"                                                      
 "ventriloquate" }                                                                                                                     
 },                                                                                                                                    
                                                                                                                                       
 {                                                                                                                                    
 "necromancy",                                                                                                                         
 { -1, -1, -1, 10, -1},
 {"animate", "atrophy", "banshee blast", "black mantle", "curse", "damnation",
 "darkness shield", "dark ritual", "demonfire", "energy drain", "ghost shield",
 "hellfire", "plague", "poison", "wail of the banshee" }
 },                                                                                                                                    
                                                                                                                                       
 {                                                                                                                                     
 "transmutation",                                                                                                                       
 { -1, -1, -1, 10, -1},                                                                                                                
 {"change sex", "enlarge", "fly", "giant strength", "haste", "heat metal", "pass door",                                                
 "shrink", "slow", "stone skin", "weaken" }                                                                                            
 },                            

    {	NULL,		{},	{} }
};

