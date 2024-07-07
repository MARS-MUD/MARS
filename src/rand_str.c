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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdarg.h>
#include <unistd.h>

#include "merc.h"
#include "db.h"
#include "interp.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"


char *   const syl_short[]   =
{
    "id",   "ad",  "al",  "no", 
    "ma",   "ol",  "da",  "nod", 
    "edig", "eda", "an",  "nos",
    "as",   "bri", "bi",  "bos",
    "jha",  "vod", "red", "vla",
    "cae",  "nal", "aod", "elad",
    "si",   "aer", "zon", "za",
    "li",   "ila", "son", "pod",
    "od",   "az",   "il", "stra",
    "ae",   "ao",  "io",  "ia",
    NULL
};
#define SYL_SHORT_LEN	(sizeof(syl_short)/sizeof(syl_short[0]))

char *   const syl_long []   =
{
    "riad",  "kad",     "dred",   "drin",
    "vil",   "vian",    "son",    "zorr",
    "met",   "brod",    "dro",    "gadro",
    "vos",   "vad",     "dos",    "dros",
    "nol",   "ton",     "dral",   "dron",
    "van",   "driad",   "striad", "lin",
    "lad",   "iad",     "ipia",   "iana",
    "iasta", "iliadro", "ilion",  "ilia",
    "stras", "tas",     "ad",     "id",
    "no",    "nosti",   "osti",   "aerd",
    NULL
};
#define SYL_LONG_LEN	(sizeof(syl_long)/sizeof(syl_long[0]))

char *   const fem_syl_short[]   =
{
    "id",   "ad",  "al",  "no", 
    "ma",   "ol",  "da",  "nod", 
    "edig", "eda", "an",  "nos",
    "as",   "bri", "bi",  "bos",
    "jha",  "vod", "red", "vla",
    "cae",  "nal", "aod", "elad",
    "si",   "aer", "zon", "za",
    "li",   "ila", "son", "po",
    "od",   "az",   "il", "stra",
    "ae",   "ao",  "io",  "ia",
    NULL
};
#define FSYL_SHORT_LEN	(sizeof(fem_syl_short)/sizeof(fem_syl_short[0]))

char *   const fem_syl_long[]   =
{
    "riad",  "kad",     "dred",   "drin",
    "vil",   "vian",    "son",    "zorr",
    "met",   "brod",    "dro",    "gadro",
    "vos",   "vad",     "dos",    "dros",
    "nol",   "ton",     "dral",   "dron",
    "van",   "driad",   "striad", "lin",
    "lad",   "iad",     "ipia",   "iana",
    "iasta", "iliadro", "ilion",  "ilia",
    "stras", "tas",     "ad",     "id",
    "no",    "nosti",   "osti",   "aerd",
    NULL
};
#define FSYL_LONG_LEN	(sizeof(fem_syl_long)/sizeof(fem_syl_long[0]))


char *	const	hum_male_short[] =
{
    "rich",	"gray",	"vain",	"dour",
    "rod",	"boas",	"kith",	"rau",
    "garl",	"glan",	"karo",	"brel",
    "gald",	"enach","ulun",	"karn",
    "and",	"ilt",	"roth",	"falr",
    "morb",	"con",	"dun",	"mors",
    "arth",	"tol",	"rol",	"fal",
    "aro",	"gal",	"gair",	"phi",
    "am",	"alo",	"all",	"ol",
    "pen",	"hath",	"ira",	"bran",
    "crae",	"lath",	"lyn",	"hela",
    "ala",	"midi",	"wal",	"quan",
    NULL
};

char *	const	hum_male_long[] =
{
    "Mieg",	"ron",	"aire",	"kolk",
    "dar",	"sace",	"nion",	"arei",
    "leff",	"melk",	"lem",	"pir",
    "ham",	"mar",	"rulf",	"cester",
    "lader",	"stard","card",	"doch",
    "werth",	"yerd",	"vard",	"sulf",
    "ven",	"ster",	"pold",	"land",
    "ward",	"nath",	"bran",	"nal",
    "lach",	"dor",	"lian",	"ald",
    "gar",	"lan",	"stef",	"staf",
    "wyn",	"luld",	"bar",	"ain",
    "ann",	"thor",	"uld",	"lain",
    NULL
};

char *	const	hum_fem_short[] =
{
    "aral",	"ara",	"tew",	"tiel",
    "glae",	"ailt",	"aray",	"tirp",
    "bren",	"dwen",	"net",	"fay",
    "lin",	"eth",	"sal",	"ay",
    "mido",	"cira",	"hana",	"dyn",
    "lero",	"erwa",	"oen",	"ema",
    "luar",	"rery",	"fwen",	"miry",
    "lore",	"nyth",	"emwa",	"raen",
    "rion",	"alia",	"leon",	"awe",
    "feli",	"fati",	"meri",	"marg",
    "tri",	NULL
};

char *	const	hum_fem_long[] =
{
    "wen",	"tey",	"weth",	"yren",
    "ren",	"aiyt",	"ayth",	"yeth",
    "uren",	"werr",	"ann",	"yell",
    "ana",	"ber",	"arel",	"thyr",
    "aynn",	"well",	"zel",	"neth",
    "ona",	"ota",	"pinn",	"fey",
    "shara",	"nill",	"ill",	"arra",
    "ara",	"eya",	"neya",	"ilna",
    "ina",	"ayna",	"anfa",	"mara",
    "vinn",	"vina",	"ila",	"ial",
    "nial",	"zial",	"ziel",	"iel",
    "niel",	NULL
};

char *const elf_male_short[] =
{
    "raven",	"bell",	"bark",	"ash",
    "eow",	"hawk",	"quiet",	"forest",
    "birch",	"oak",	"aldar",	"leg",
    "deer",	"sodden",	"fal",	"faro",
    "anga",	"thri",	"thon",	"aelu",
    "agia",	"fore",	"aluh",	"anu",
    "fare",	"farl",	"kahl",	"ahl",
    "lhin",	"foro",	"adin",	"apul",
    "arel",	"arib",	"eran",	"seni",
    "arlo",	"ona",	"ocea",	"ehat",
    "lole",	"tari",	"acir",	NULL
};

char *const elf_male_long[] =
{
    "sparrow",	"crystal",	"birun",	"rihr",
    "nanh",	"tutial",	"haet",	"nores",
    "guel",	"idorn",	"gian",	"hiar",
    "mane",	"fox",	"cinder",	"vine",
    "star",	"buck",	"trat",	"lute",
    "tear",	"ling",	"lail",	"low",
    "path",	"eah",	"hail",	"arro",
    "nehl",	"both",	"diah",	"nais",
    "reot",	"cenl",	"teer",	"nean",
    "dieh",	"neot",	"cenl",	"raoh",
    NULL
};

char *const elf_fem_short[] =
{
    "sind",	"arph",	"lass",	"glir",
    "cene",	"loth",	"duw",	"mir",
    "elei",	"estel",	"galu",	"gala",
    "hith",	"malla",	"duli",	"aer",
    "taur",	"than",	"ara",	"mene",
    "cele",	"gwath",	"ithi",	"anor",
    "elen",	"lome",	"nen",	"vani",
    "aili",	"vany",	"alqu",	"ingol",
    "orel",	"hele",	"alas",	"laur",
    "eari",	"elna",	"silm",	"nenu",
    "vane",	"lind",	"melu",	"ango",
    NULL
};

char *const elf_fem_long[] =
{
    "dess",	"ness",	"wen",	"inn",
    "dwen",	"bess",	"tari",	"ari",
    "losse",	"elpe",	"anis",	"ewen",
    "indis",	"indus",	"aira",	"silme",
    "enis",	"anar",	"arne",	"dale",
    "reth",	"beth",	"ante",	"ciel",
    "alad",	"iril",	"bain",	"thel",
    "rind",	"laer",	"estel",	"fair",
    "agan",	"daur",	"fair",	"aura",
    "cani",	"tale",	"uile",	"angan",
    "mede",	"quen",	"alata",	"alta",
    NULL
};

char *const dwa_male_short[] =
{
    "dun",	"kos",	"bal",	"fer",
    "mas",	"nir",	"foi",	"fis",
    "ges",	"kom",	"hug",	"fas",
    "galf",	"gim",	"his",	"doim",
    "tag",	"mes",	"kus",	"voil",
    "tel",	"min",	"gora",	"nog",
    "gog",	"kek",	"bara",	"otho",
    "ing",	"morm",	"ror",	"kulm",
    "cof",	"dat",	"eon",	"fam",
    "gir",	"goy",	"hih",	"hur",
    "iod",	"khu",	"mar",	"mer",
    "moh",	"nin",	"nnu",	"red",
    "rha",	"rib",	"rif",	"rov",
    "sil",	"ter",	"vole",	"uto",
    NULL
};

char *const dwa_male_long[] =
{
    "aer",	"auk",	"dot",	"nara",
    "kam",	"liro",	"mvik",	"kam",
    "turo",	"tiro",	"irot",	"dir",
    "hot",	"da",	"koh",	"ore",
    "ter",	"est",	"tler",	"chop",
    "iest",	"vnim",	"mobo",	"refhu",
    "oora",	"rug",	"rack",	"rock",
    "lode",	"copp",	"rand",	"hunt",
    "rokil",	"kek",	"rdeh",	"tob",
    "carv",	"vohg",	"nam",	"refa",
    "vor",	"hehak",	"nehak",	"seek",
    "tite",	"mash",	"irat",	"ruko",
    "terde",	"rahar",	"cker",	"iere",
    NULL
};

char *const dwa_fem_short[] =
{
    "dun",	"kos",	"bal",	"fer",
    "mas",	"nir",	"foi",	"fis",
    "ges",	"kom",	"hug",	"fas",
    "galf",	"gim",	"his",	"doim",
    "tag",	"mes",	"kus",	"voil",
    "tel",	"min",	"gora",	"nog",
    "gog",	"kek",	"bara",	"otho",
    "ing",	"morm",	"ror",	"kulm",
    "cof",	"dat",	"eon",	"fam",
    "gir",	"goy",	"hih",	"hur",
    "iod",	"khu",	"mar",	"mer",
    "moh",	"nin",	"nnu",	"red",
    "rha",	"rib",	"rif",	"rov",
    "sil",	"ter",	"vole",	"uto",
    NULL
};

char *const dwa_fem_long[] =
{
    "aer",	"auk",	"dot",	"nara",
    "kam",	"liro",	"mvik",	"kam",
    "turo",	"tiro",	"irot",	"dir",
    "hot",	"da",	"koh",	"ore",
    "ter",	"est",	"tler",	"chop",
    "iest",	"vnim",	"mobo",	"refhu",
    "oora",	"rug",	"rack",	"rock",
    "lode",	"copp",	"rand",	"hunt",
    "rokil",	"kek",	"rdeh",	"tob",
    "carv",	"vohg",	"nam",	"refa",
    "vor",	"hehak",	"nehak",	"seek",
    "tite",	"mash",	"irat",	"ruko",
    "terde",	"rahar",	"cker",	"iere",
    NULL
};

char *const orc_male_short[] =
{
    "bank",	"but",	"dku",	"uxn",
    "dno",	"xut",	"gas",	"guz",
    "mon",	"ezt",	"mug",	"ezb",
    "ndog",	"nto",	"sud",	"nux",
    "azk",	"nxa",	"ags",	"nza",
    "tek",	"sno",	"okt",	"sork",
    "stob",	"sxu",	"utn",	"tanm",
    "tke",	"emd",	"toxn",	"tozu",
    "zubg",	"xeb",	"xuno",	"nokr",
    "xzo",	"xzog",	"zbu",	"bud",
    NULL
};

char *const orc_male_long[] =
{
    "Besd",	"Bkam",	"Bsoz",	"Gexs",
    "Kexodb",	"Korx",	"Koxand",	"Kxamon",
    "Mes",	"Mubaxz",	"Munazs",	"Nasodb",
    "Nbezax",	"Norb",	"Ntoxar",	"Nugd",
    "Nzatog",	"Nzukax",	"Sdoman",	"Senotz",
    "Skodex",	"Szonx",	"Tar",	"Xamest",
    "Znumok",	NULL
};

char *const orc_fem_short[] =
{
    "bank",	"but",	"dku",	"uxn",
    "dno",	"xut",	"gas",	"guz",
    "mon",	"ezt",	"mug",	"ezb",
    "ndog",	"nto",	"sud",	"nux",
    "azk",	"nxa",	"ags",	"nza",
    "tek",	"sno",	"okt",	"sork",
    "stob",	"sxu",	"utn",	"tanm",
    "tke",	"emd",	"toxn",	"tozu",
    "zubg",	"xeb",	"xuno",	"nokr",
    "xzo",	"xzog",	"zbu",	"bud",
    NULL
};

char *const orc_fem_long[] =
{
    "besd",	"bkam",	"bsoz",	"gexs",
    "kexodb",	"korx",	"koxand",	"kxamon",
    "mes",	"mubaxz",	"munazs",	"nasodb",
    "nbezax",	"norb",	"ntoxar",	"nugd",
    "nzatog",	"nzukax",	"sdoman",	"senotz",
    "skodex",	"szonx",	"tar",	"xamest",
    "znumok",	NULL
};

char *const gob_male_short[] =
{
    "big",	"howl",	"bone",	"tooth",
    "clay",	"gibber",	"dark",	"rant",
    "death",	"shred",	"demon",	"doom",
    "dread",	"break",	"evil",	"foul",
    "chain",	"eater",	"hack",	"bane",
    "great",	"ice",	"kill",	"iron",
    "maw",	"mad",	"beast",	"puke",
    "render",	"fart",	"shadow",	"stone",
    "belch",	"toe",	"gloom",	"snap",
    "tomb",	"thief",	"war",	"hand",
    "wild",	"yell",	"killer",	"murder",
    "scream",	"steel",	"iron",	"bane",
    "brain",	"mind",	"drool",	"claw",
    "fire",	"damn",	"fang",	"cut",
    NULL
};

char *const gob_male_long[] =
{
    "laugher",	"damner",	"killer",	"breaker",
    "belcher",	"basher",	"hoster",	"eater",
    "tracker",	"sucker",	"stinker",	"snapper",
    "ripper",	"chewer",	"nibbler",	"cleaver",
    "mucker",	"crazy",	"rotten",	"ass",
    "flattus",	"boner",	"gouger",	"yeller",
    "hider",	"stealer",	"snatcher blade",
    "rotter",	"maw",	"beast",	"drooler",
    "knocker",	"dweller",	"shadow",	"shredder",
    "fart",	"gut",	"ooze",	"puss",
    "howler",	"doom",	"cutter",	"wild",
    "mad",	"throat",	"raider",	"cavern",
    NULL
};

char *const gob_fem_short[] =
{
    "big",	"howl",	"bone",	"tooth",
    "clay",	"gibber",	"dark",	"rant",
    "death",	"shred",	"demon",	"doom",
    "dread",	"break",	"evil",	"foul",
    "chain",	"eater",	"hack",	"bane",
    "great",	"ice",	"kill",	"iron",
    "maw",	"mad",	"beast",	"puke",
    "render",	"fart",	"shadow",	"stone",
    "belch",	"toe",	"gloom",	"snap",
    "tomb",	"thief",	"war",	"hand",
    "wild",	"yell",	"killer",	"murder",
    "scream",	"steel",	"iron",	"bane",
    "brain",	"mind",	"drool",	"claw",
    "fire",	"damn",	"fang",	"cut",
    NULL
};

char *const gob_fem_long[] =
{
    "laugher",	"damner",	"killer",	"breaker",
    "belcher",	"basher",	"hoster",	"eater",
    "tracker",	"sucker",	"stinker",	"snapper",
    "ripper",	"chewer",	"nibbler",	"cleaver",
    "mucker",	"crazy",	"rotten",	"ass",
    "flattus",	"boner",	"gouger",	"yeller",
    "hider",	"stealer",	"snatcher blade",
    "rotter",	"maw",	"beast",	"drooler",
    "knocker",	"dweller",	"shadow",	"shredder",
    "fart",	"gut",	"ooze",	"puss",
    "howler",	"doom",	"cutter",	"wild",
    "mad",	"throat",	"raider",	"cavern",
    NULL
};


enum
{
    PASS_NAME,
    PASS_SHORT,
    PASS_LONG,
    PASS_DESC,
    PASS_MAX
};


static char *	fix_doubled_i	args( ( char *str ) );
static int	word_count	args( ( char *const *list ) );

/*
 * Remove occurrences of "ii"
 */
static char *
fix_doubled_i( char *str )
{
    char *p;

    for ( p = str; *p != '\0'; p++ )
    {
        while ( *p == 'i' && *(p+1) == 'i' )
            strcpy( p, p+1 );
    }
    return str;
}


void
set_mob_strings( CHAR_DATA *mob )
{
    MOB_INDEX_DATA *	pMobIndex;
    char *		i;
    char *		point;
    char *		str;
    char		buf[MAX_STRING_LENGTH];
    char		tmp[SHORT_STRING_LENGTH];
    char		m_str[SHORT_STRING_LENGTH];
    char		M_str[SHORT_STRING_LENGTH];
    char		n_str[SHORT_STRING_LENGTH];
    char		N_str[SHORT_STRING_LENGTH];
    char * const *	ms;
    char * const *	ml;
    char * const *	fs;
    char * const *	fl;
    int			pass;
    int			msl;
    int			mll;
    int			fsl;
    int			fll;

    if ( !IS_NPC( mob ) || ( pMobIndex = mob->pIndexData ) == NULL )
    {
        bugf( "Set_mob_strings: setting strings for non-NPC" );
        return;
    }

    if ( strchr( pMobIndex->player_name, '$' ) == NULL
    &&	 strchr( pMobIndex->short_descr, '$' ) == NULL
    &&	 strchr( pMobIndex->long_descr, '$')  == NULL
    &&	 strchr( pMobIndex->description, '$' ) == NULL )
    {
        mob->name	 = str_dup( pMobIndex->player_name );
        mob->short_descr = str_dup( pMobIndex->short_descr );
        mob->long_descr	 = str_dup( pMobIndex->long_descr );
        mob->description = str_dup( pMobIndex->description );
        return;
    }

    if ( mob->race == race_human )
    {
        ms = hum_male_short;	msl = word_count( ms );
        ml = hum_male_long;	mll = word_count( ml );
        fs = hum_fem_short;	fsl = word_count( fs );
        fl = hum_fem_long;	fll = word_count( fl );
    }
    else if ( mob->race == race_elf )
    {
        ms = elf_male_short;	msl = word_count( ms );
        ml = elf_male_long;	mll = word_count( ml );
        fs = elf_fem_short;	fsl = word_count( fs );
        fl = elf_fem_long;	fll = word_count( fl );
    }
    else if ( mob->race == race_dwarf )
    {
        ms = dwa_male_short;	msl = word_count( ms );
        ml = dwa_male_long;	mll = word_count( ml );
        fs = dwa_fem_short;	fsl = word_count( fs );
        fl = dwa_fem_long;	fll = word_count( fl );
    }
    else if ( mob->race == race_orc )
    {
        ms = orc_male_short;	msl = word_count( ms );
        ml = orc_male_long;	mll = word_count( ml );
        fs = orc_fem_short;	fsl = word_count( fs );
        fl = orc_fem_long;	fll = word_count( fl );
    }
    else if ( mob->race == race_goblin )
    {
        ms = gob_male_short;	msl = word_count( ms );
        ml = gob_male_long;	mll = word_count( ml );
        fs = gob_fem_short;	fsl = word_count( fs );
        fl = gob_fem_long;	fll = word_count( fl );
    }
    else
    {
        ms = syl_short;		msl = word_count( ms );
        ml = syl_long;		mll = word_count( ml );
        fs = fem_syl_short;	fsl = word_count( fs );
        fl = fem_syl_long;	fll = word_count( fl );
    }

    if ( mob->sex == SEX_FEMALE )
    {
        strcpy ( m_str, fs[number_range( 0, fsl )] );
        strcpy ( M_str, fl[number_range( 0, fll )] );
        sprintf( n_str, "%s%s",
                 capitalize( fs[number_range( 0, fsl )] ),
                 fl[number_range( 0, fll )] );
        sprintf( N_str, "%s%s%s",
                 capitalize( fs[number_range( 0, fsl )] ),
                 fs[number_range( 0, fsl )],
                 fl[number_range( 0, fll )] );
    }
    else
    {
        strcpy ( m_str, ms[number_range( 0, msl )] );
        strcpy ( M_str, ml[number_range( 0, mll )] );
        sprintf( n_str, "%s%s",
                 capitalize( ms[number_range( 0, msl )] ),
                 ml[number_range( 0, mll )] );
        sprintf( N_str, "%s%s%s",
                 capitalize( ms[number_range( 0, msl )] ),
                 ms[number_range( 0, msl )],
                 ml[number_range( 0, mll )] );
    }

    for ( pass = PASS_NAME; pass < PASS_MAX; pass++ )
    {
        switch( pass )
        {
            case PASS_NAME:	str = pMobIndex->player_name;	break;
            case PASS_SHORT:	str = pMobIndex->short_descr;	break;
            case PASS_LONG:	str = pMobIndex->long_descr;	break;
            case PASS_DESC:	str = pMobIndex->description;	break;
            default:
                bugf( "Set_mob_strings: logic error." );
                return;
        }
        point = buf;

        while ( *str != '\0' )
        {
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }

            str++;
            switch( *str )
            {
                default:
                    tmp[0] = '$';
                    tmp[1] = *str;
                    tmp[2] = '\0';
                    i = tmp;
                    break;
                case '\0': continue;
                case 'm' : i = m_str; break;
                case 'M' : i = M_str; break;
                case 'n' : i = n_str; break;
                case 'N' : i = N_str; break;
            }

            str++;
            while ( *i != '\0' )
                *point++ = *i++;
        }

        *point = '\0';

        fix_doubled_i( buf );

        switch( pass )
        {
            case PASS_NAME:	mob->name	 = str_dup( buf ); break;
            case PASS_SHORT:	mob->short_descr = str_dup( buf ); break;
            case PASS_LONG:	mob->long_descr  = str_dup( buf ); break;
            case PASS_DESC:	mob->description = str_dup( buf ); break;
        }

    }

    return;
}


static int
word_count( char *const *list )
{
    int			c;

    for ( c = 0; ; c++ )
        if ( IS_NULLSTR( list[c] ) )
            return c - 1;
    return 1;
}

