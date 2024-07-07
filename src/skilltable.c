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
#include "magic.h"
#include "interp.h"


/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 * NOTE: Slot numbers are obsolete and the slot field is now used for
 * the spell flags.
 */
// clr, drd, ftr, mag, rog
#define SLOT(n)		n

struct	skill_type	skill_table	[]	=
{

/*
 * Magic spells.
 */

    {
	"reserved",		{ },	{ },	{ },
	0,			TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	 0,	 0,	0,
	"",			"",		"",	""
    },

    {
	"acid blast",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"acid blast",		"!Acid Blast!",	"",	""
    },


    {
	"acid geyser",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_acid_geyser,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"acid geyser",		"!Acid Geyser!",	"",	""
    },
    {
	"acid rain",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_acid_rain,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	350,	12,	0,
	"acid rain",		"!acid rain!",	"",	""
    },

 {
	"acid arrow",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_acid_arrow,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	350,	12,	0,
	"acid arrow",		"!acid arrow!",	"",	""
    },

    {
        "adrenaline control", {-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_adrenaline,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
        &gsn_adrenaline,	SPELL_NONE,	30,	24,	0,
        "",			"Your rage ebbs.",
        "",			"$n's rage ebbs."
    },

    {
	"aid",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_aid,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"Your divine aid dissipates.",
	"",			"$n's divine aid dissipates."
    },

    {
   	 "animate dead", 	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
    	 spell_animate,      	TAR_OBJ_INV,            POS_STANDING,
   	 NULL,                  SPELL_NONE,     	100,    24,	0,
    	 "",                 	"!Animate Dead!",       "",	""
     },

    {
   	 "animal summoning", 	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
    	 spell_animal_summoning,TAR_IGNORE,		POS_STANDING,
   	 NULL,                  SPELL_NONE,	50,	24,	0,
    	 "",                 	"!Animal Control!",       "",	""
     },

{
   	 "astral walk", 	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
    	 spell_astral_walk,TAR_IGNORE,		POS_STANDING,
   	 NULL,                  SPELL_NONE,	50,	24,	0,
    	 "",                 	"!Astral Walk!",       "",	""
     },
    {
	"armor",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"You feel less armored.",
	"",			""
    },

    {
	"atrophy",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_atrophy,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_atrophy,		SPELL_NONE,	 25,	12,	0,
	"",			"The wasting away stops.",
	"",			"$n stops wasting away."
    },

    {
	"aura of peace",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_aura_peace,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_aura_peace,	SPELL_NONE,	100,	12,	0,
	"",			"The peace aura around your body fades away.",
	"",			"The peace aura around $n fades away."
    },

  {
	"bane",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_bane,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		SPELL_NONE,	20,	12,	0,
	"bane",		"The bane on your soul wears off.", 
	"$p is no longer fearful for their soul."
    },


    {
	"barkskin",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_barkskin,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	 15,	12,	0,
	"",			"Your skin is no longer woody.", 
	"",			"$n's skin is no longer woody."
    },

    {
	"barrage",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_barrage,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	 30,	8,	0,
	"burst of light",		"!Barrage!", 
	"",	""
    },
    {
	"banshee blast",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_banshee_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"banshee blast",	"!Banshee Blast!",	""
    },

    {
	"bear claws",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_bear_claws,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SPELL_NONE,	 60,	15,	0,
	"",			"!Bear Claw!", 
	"",			""
    },

    {
	"bear fat",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_bear_fat,		TAR_CHAR_SELF,	POS_STANDING,
	NULL,			SPELL_NONE,	 10,	11,	0,
	"",			"The winter warmth of the bear is gone.",
	"",			"The winter warmth of the bear leaves $n."
    },
    {
	"bears endurance",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_bears_endurance,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"You feel less resilient.",
	"",			"$n seems less resilient."
    },

    {
	"black mantle",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_black_mantle,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_black_mantle,		SPELL_NONE,	75,	12,	0,
	"",			"The dark aura around your body fades.",
	"",			"The dark aura around $n's body fades."
    },


    {
	"blade barrier",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_blade_barrier,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_blade_barrier,	SPELL_NONE,	40,	12,	0,
	"whirling blades",	"The whirling blades surrounding you vanish.",
	"",			"The whirling blades surrounding $n vanish."
    },

    {
	"bless",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_bless,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"You feel less righteous.", 
	"$p's holy aura fades.",	"$n's holy aura fades."
    },

    {
	"blindness",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		SPELL_NONE,	 5,	12,	0,
	"",			"You can see again.",	"",	""
    },
    {
	"bulls strength",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_bulls_strength,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"You no longer feel the strength of the bear.",
	"",			"$n looks weaker as the strength of the bear fades."
    },

    {
	"burning hands",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"burning hands",	"!Burning Hands!", 	"",	""
    },

    {
	"call lightning",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"lightning bolt",	"!Call Lightning!",	"",	""
    },

    {   "calm",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_calm,		TAR_IGNORE,		POS_FIGHTING,
	NULL,			SPELL_NONE,	30,	12,	0,
	"",			"You have lost your peace of mind.",
	"",	""
    },

    {
	"cancellation",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cancellation,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	""			"!cancellation!",	"",	""
    },
    {
	"cats grace",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cats_grace,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"You feel less graceful.",
	"",			"$n looks less graceful."
    },

    {
	"cause critical",	{ },	{ },	{ },
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"spell",		"!Cause Critical!",	"",	""
    },

    {
	"cause light",		{ },	{ },	{ },
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"spell",		"!Cause Light!",	""
    },

    {
	"cause serious",	{ },	{ },	{ },
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	17,	12,	0,
	"spell",		"!Cause Serious!",	"",	""
    },

{
	"caustic eruption",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_caustic_eruption,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	17,	12,	0,
	"spell",		"!Caustic Eruption!",	"",	""
    },
    {   
	"celestial lightning",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_celestial_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	25,	12,	0,
	"lightning",		"!Celestial Lightning!",	"",	""
    }, 
    {   
	"chain lightning",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	25,	12,	0,
	"lightning",		"!Chain Lightning!",	"",	""
    }, 

    {
	"change sex",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"Your body feels familiar again.",	""
    },

    {
	"charm person",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	SPELL_NONE,	 5,	12,	0,
	"",			"You feel more self-confident.",	""
    },

    {
	"chill touch",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"chilling touch",	"You feel less cold.",	""
    },

    {
	"colour spray",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"colour spray",		"!Colour Spray!",	""
    },

    {
	"cone of cold",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cone_of_cold,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	40,	6,	0,
	"cone of cold",		"!Cone of Cold!",	"",	""
    },

    {
	"continual light",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	 7,	12,	0,
	"",			"!Continual Light!",	""
    },

    {
	"control weather",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	25,	12,	0,
	"",			"!Control Weather!",	""
    },
    {   
	"corrosive burst",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_corrosive_burst,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	25,	12,	0,
	"corrosive burst",		"!Corrosive Burst!",	"",	""
    }, 

    {
	"create food",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"!Create Food!",	""
    },

    {
	"create rose",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_create_rose,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	30, 	12,	0,
	"",			"!Create Rose!",	""
    },  

    {
	"create spring",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"",			"!Create Spring!",	""
    },

    {
	"create water",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"!Create Water!",	""
    },

    {
	"cure blindness",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"!Cure Blindness!",	""
    },

    {
	"cure critical",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"",			"!Cure Critical!",	""
    },

    {
	"cure disease",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cure_disease,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"",			"!Cure Disease!",	""
    },

    {
	"cure drunk",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cure_drunk,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"",			"!Cure Drunk!",		""
    },

    {
	"cure light",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	10,	12,	0,
	"",			"!Cure Light!",		""
    },

    {
	"cure poison",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"!Cure Poison!",	""
    },

    {
	"cure serious",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"!Cure Serious!",	""
    },

    {
	"curse",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_curse,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_curse,		SPELL_NONE,	20,	12,	0,
	"curse",		"The curse wears off.", 
	"$p is no longer impure."
    },

    {
	"curse of nature",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_curse_of_nature,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	100,	12,	0,
	"curse of nature",	"The curse of Nature lifts from your soul.",
	"",			"The Curse of Nature lifts from $n."
    },
     {
	"damnation",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_damnation,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"damnation",		"!Damnation!",		""
    },	

    {
	"dancing boots",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_dancing_boots,	TAR_IGNORE,	POS_STANDING,
	NULL,			SPELL_NONE,	40,	15,	4,
	"",			"!Dancing boots!",
	"",			""
    },

    {
	"dancing lights",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_dancing_lights,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_dancing_lights,	SPELL_NONE,	50,	12,	4,
	"dancing lights", "The dancing lights about your body fade away.",
	"",		  "The dancing lights around $n fade away."
    },

    {
	"dark ritual",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_dark_ritual,	TAR_IGNORE,	POS_STANDING,
	NULL,			SPELL_NONE,	5,	12,	2,
	"",			"!dark ritual!",
	"",			""
    },

    {
	"darkness shield",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_darkness_shield,	TAR_IGNORE,	POS_STANDING,
	NULL,			SPELL_NONE,	40,	15,	4,
	"",			"!Darkness shield!",
	"",			""
    },
     {
	"daze",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_daze,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"daze",		"!Daze!",		""
    },	

    {
	"deafen",              {-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_deafen,           TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
	&gsn_deafen,            SPELL_NONE,     20,    12,      0,
	"",                     "Your hearing returns!", "$n can hear again."
    },

    {
	"demonfire",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_demonfire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"torments",		"!Demonfire!",		""
    },	

     {
	"detect auras",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_aurasight,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"You can no longer detect auras.",	
	""
    },

    {
	"detect evil",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"The red in your vision disappears.",	""
    },

    {
        "detect good",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_detect_good,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SPELL_NONE,        5,     12,	0,
        "",                     "The gold in your vision disappears.",	""
    },

    {
	"detect hidden",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"You feel less aware of your surroundings.",	
	""
    },

     {
	"detect life",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_life_detect,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"You can no longer sense the living.",	
	""
    },

    {
	"detect invis",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"You no longer see invisible objects.",
	""
    },

    {
	"detect magic",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"The detect magic wears off.",	""
    },

    {
	"detect poison",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"!Detect Poison!",	""
    },

    {
	"dispel evil",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"dispel evil",		"!Dispel Evil!",	""
    },

    {
        "dispel good",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_dispel_good,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_NONE,      15,     12,	0,
        "dispel good",          "!Dispel Good!",	""
    },

    {
	"dispel magic",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"!Dispel Magic!",	""
    },
    {
	"disruption",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_disruption,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"disruption",	"!Disruption!",	""
    },
    {
	"dragonscales",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_dragonscales,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	12,	18,	0,
	"",			"The dragon scales crack and turn to dust, leaving your skin smooth again.",
	"",			"$n's dragon scales turn to dust."
    },

    {
	"earthquake",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"earthquake",		"!Earthquake!",		""
    },
     {
	"electrocute",{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_electrocute,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"electrocute",	"!Electrocute!",	""
    },

    {
	"enchant armor",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_enchant_armor,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SPELL_NONE,	100,	24,	0,
	"",			"!Enchant Armor!",	""
    },

    {
	"enchant weapon",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SPELL_NONE,	100,	24,	0,
	"",			"!Enchant Weapon!",	""
    },

    {
	"energy drain",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	35,	12,	0,
	"energy drain",		"!Energy Drain!",	""
    },

    {
	"enlarge",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_enlarge,		TAR_CHAR_SELF,		POS_STANDING,
	&gsn_enlarge,		SPELL_NONE,	200,	24,	0,
	"",	"You return to your normal size.",
	"",	"$n returns to $s normal size."
    },

    {
	"faerie fire",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"faerie fire",		"The pink aura around you fades away.",
	""
    },

    {
	"faerie fog",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	12,	12,	0,
	"faerie fog",		"!Faerie Fog!",		""
    },

    {
	"farsight",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_farsight,		TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	36,	20,	0,
	"farsight",		"!Farsight!",		""
    },	

    {
	"fireball",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"fireball",		"!Fireball!",		""
    },

    {
	"fireproof",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_fireproof,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SPELL_NONE,	10,	12,	0,
	"",			"",	"$p's protective aura fades."
    },

    {
	"flame shield",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_flameshield,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	40,	12,	0,
	"flame shield",		"The flames surrounding you die away.",
	"",			"The flames surrounding $n die away."
    },

    {
	"force shield",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_forceshield,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	30,	12,	0,
	"force shield",		"The shield about you fades away.",
	"",			"$n's force shield fades away."
    },


    {
	"flamestrike",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"flamestrike",		"!Flamestrike!",		""
    },

    {
	"fly",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	10,	18,	0,
	"",			"You slowly float to the ground.",	""
    },

    {
	"floating disc",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_floating_disc,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	40,	24,	0,
	"",			"!Floating disc!",	""
    },

    {
	"foxs cunning",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_fox_cunning,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"You feel less cunning.",
	"",			"$n seems less cunning."
    },

    {
        "frenzy",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_frenzy,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   SPELL_NONE,      30,     24,	0,
        "",                     "Your rage ebbs.",	""
    },

     {
	"full heal",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_full_heal,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"",			"!Full Heal!",	""
    },

    {
        "deathgrip",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_RESTING,
        NULL,                   SPELL_NONE,       0,      12,	0,
        "",                     "The dark shroud leaves your hands.",   ""
    },

    {
	"gate",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_gate,		TAR_IGNORE,		POS_FIGHTING,
	NULL,			SPELL_NONE,	80,	12,	0,
	"",			"!Gate!",		""
    },

    {
        "ghost shield",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_ghost_shield,	TAR_CHAR_SELF,		POS_STANDING,
        NULL,			SPELL_NONE,	50,	12,	0,
        "",			"The ghosts surrounding you dissipate.",
        "",			"The ghosts surrounding $n dissipate."
    },

    {
	"giant strength",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"",			"You feel weaker.",	""
    },

    {
	"goodberry",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
    	spell_goodberry,	TAR_OBJ_INV,		POS_STANDING,
    	NULL,			SPELL_NONE,	25,	8,	0,
    	"",			"!GOODBERRY!",		""     
    },

    {
	"harm",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	35,	12,	0,
	"harm spell",		"!Harm!,",		""
    },

    {
	"haste",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_haste,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	30,	12,	0,
	"",			"You feel yourself slow down.",	""
    },

    {
	"heal",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	50,	12,	0,
	"",			"!Heal!",		""
    },

    {
	"healing hands",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_healing_hands,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	40,	12,	0,
	"",			"!Healing Hands!",	""
    },

    {
	"heat metal",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_heat_metal,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE, 	25,	18,	0,
	"spell",		"!Heat Metal!",		""
    },
     {
	"hellfire",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_hellfire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	35,	12,	0,
	"hellfire",		"!Hellfire!",	""
    },

    {
	"holy word",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_holy_word,	TAR_IGNORE,	POS_FIGHTING,
	NULL,			SPELL_NONE, 	200,	24,	0,
	"divine wrath",		"!Holy Word!",		""
    },
     {
	"hurricane",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_hurricane,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"hurricane",	"!Hurricane!",	""
    },

    {
	"identify",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SPELL_NONE,	12,	24,	0,
	"",			"!Identify!",		""
    },

    {
	"infravision",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	 5,	18,	0,
	"",			"You no longer see in the dark.",	""
    },

    {
	"invisibility",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_invis,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	&gsn_invis,		SPELL_NONE,	 5,	12,	0,
	"",			"You are no longer invisible.",		
	"$p fades into view."
    },
     {
	"ion blast",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_ion_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"ion blast",	"!Ion Blast!",	""
    },

    {
	"know alignment",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	 9,	12,	0,
	"",			"!Know Alignment!",	""
    },

    {
	"lightning bolt",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"lightning bolt",	"!Lightning Bolt!",	""
    },
    

    {
	"locate object",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	20,	18,	0,
	"",			"!Locate Object!",	""
    },

    {
	"magic missile",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"magic missile",	"!Magic Missile!",	""
    },

    {
	"mana",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_mana,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	0,	0,	0,
	"",			"!mana!",		""
    },

    {
	"mass healing",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_mass_healing,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	100,	36,	0,
	"",			"!Mass Healing!",	""
    },

    {
	"mass invis",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	SPELL_NONE,	20,	24,	0,
	"",			"You are no longer invisible.",		""
    },

    {
	"natural armor",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_natural_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	12,	18,	0,
	"",			"You are no longer naturally armored.",
	"",			"$n's natural armor subsides."
    },

    {
        "nexus",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,                   SPELL_NONE,       150,   36,	0,
        "",                     "!Nexus!",		""
    },

    {
	"owls wisdom",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_owls_wisdom,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"",			"Your wisdom fades.",
	"",			"$n's eyes look duller."
    },

    {
	"pass door",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"",			"You feel solid again.",	""
    },

    {
	"plague",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_plague,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_plague,		SPELL_NONE,	20,	12,	0,
	"sickness",		"Your sores vanish.",	""
    },

    {
	"poison",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_poison,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_poison,		SPELL_NONE,	10,	12,	0,
	"poison",		"You feel less sick.",	
	"The poison on $p dries up."
    },

    {
        "portal",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,                   SPELL_NONE,       100,     24,	0,
        "",                     "!Portal!",		""
    },

     {
	"project force",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_project_force,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"project force",		"!Project Force!",	"",	""
    },

    {
	"protection evil",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE, 	5,	12,	0,
	"",			"You feel less protected.",	""
    },

    {
        "protection good",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_protection_good,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SPELL_NONE,       5,     12,	0,
        "",                     "You feel less protected.",	""
    },

    {
        "ray of truth",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_ray_of_truth,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_NONE,      20,     12,	0,
        "ray of truth",         "!Ray of Truth!",	""
    },

    {
	"recharge",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_recharge,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SPELL_NONE,	60,	24,	0,
	"",			"!Recharge!",		""
    },

    {
	"refresh",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	12,	18,	0,
	"refresh",		"!Refresh!",		""
    },

    {
	"remove curse",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_remove_curse,	TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"!Remove Curse!",	"",	""
    },

    {
	"resurrect",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_resurrect,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	75,	12,	0,
	"",			"!Resurrect!",		"",	""
    },

{
	"rushing fist",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_rushing_fist,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"rushing fist",	"!Rushing Fist!",	""
    },
    {
	"sanctuary",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_sanctuary,		SPELL_NONE,	75,	12,	0,
	"",			"The white aura around your body fades.",
	"",			"The white aura around $n's body fades."
    },

    {
	"scry",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_scry,		TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	15,	8,	0,
	"",			"",
	"",			""
    },

    {
	"shield",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	12,	18,	0,
	"",			"Your force shield shimmers then fades away.",
	"",			"$n's force field shimmers and fades."
    },

    {
	"shocking grasp",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"shocking grasp",	"!Shocking Grasp!",	""
    },

    {
	"shrink",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_shrink,		TAR_CHAR_SELF,		POS_STANDING,
	&gsn_shrink,		SPELL_NONE,	200,	24,	0,
	"",	"You return to your normal size.",
	"",	"$n returns to $s normal size."
    },

    {
	"sleep",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		SPELL_NONE,	15,	12,	0,
	"",			"You feel less tired.",	""
    },

    {
        "slow",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_slow,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_NONE,      30,     12,	0,
        "",                     "You feel yourself speed up.",	""
    },

    {
	"spellmantle",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_spellmantle,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SPELL_NONE,	12,	18,	0,
	"",			"Your spellmantle fades away, leaving you vulnerable to magic.",
	"",			"$n's is more vulnerable to magic as their spellmantle fadss."
    },

    {
	"static shield",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_staticshield,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	35,	12,	0,
	"static shield",	"The static around you grounds out.",
	"",			"The static around $n grounds out."
    },

    {
	"stone skin",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	12,	18,	0,
	"",			"Your skin feels soft again.",
	"",			"$n's skin looks softer."
    },

     {
        "strike true",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_strike_true,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   SPELL_NONE,      30,     24,	0,
        "",                     "You feel less accurate.",	""
    },

    {
	"summon",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	50,	12,	0,
	"",			"!Summon!",		""
    },

{
	"preserve",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_preserve,		TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	50,	12,	0,
	"",			"!Preserve!",		""
    },

    {
        "summon greater golem",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_summon_ggolem,     TAR_IGNORE,             POS_STANDING,
        NULL,           SPELL_NONE,      250,            32,	0,
        "",     "You gained enough mana to summon more golems now.",""
    },

    {
 	"summon fire elemental",{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
 	spell_summon_fire_elemental,TAR_CHAR_SELF,	POS_FIGHTING,
 	NULL,			SPELL_NONE,	50,     12,	0,
  	"",			"!Fire Elemental!",	""     
    },

	    {
 	"summon air elemental",{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
 	spell_summon_air_elemental,TAR_CHAR_SELF,	POS_FIGHTING,
 	NULL,			SPELL_NONE,	50,     12,	0,
  	"",			"!Air Elemental!",	""     
    },

	    {
 	"summon water elemental",{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
 	spell_summon_water_elemental,TAR_CHAR_SELF,	POS_FIGHTING,
 	NULL,			SPELL_NONE,	50,     12,	0,
  	"",			"!Water Elemental!",	""     
    },

	    {
 	"summon earth elemental",{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
 	spell_summon_earth_elemental,TAR_CHAR_SELF,	POS_FIGHTING,
 	NULL,			SPELL_NONE,	50,     12,	0,
  	"",			"!Earth Elemental!",	""     
    },

    {
	"summon greater wolf", 	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_summon_greater_wolf,TAR_CHAR_SELF,	POS_FIGHTING,
	NULL,			SPELL_NONE,        50,     12,	0,
	"",                     "!Greater Wolf!",       ""
    },

{
	"find familiar", 	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_find_familiar,TAR_CHAR_SELF,	POS_FIGHTING,
	NULL,			SPELL_NONE,        50,     12,	0,
	"",                     "!Find Familiar!",       ""
    },

    {
	"summon hawk",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_summon_hawk,	TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,			SPELL_NONE,	65,	12,	0,
	"",			"!Hawk!",		""
    },

    {
    	"summon wolf", 		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
    	spell_summon_wolf,      TAR_CHAR_SELF,          POS_FIGHTING,
    	NULL,                   SPELL_NONE,	50,     12,	0,
    	"",                 	"!Wolf!",       	""
    },

    {
	"summon tiger",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_summon_tiger,	TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,			SPELL_NONE,	85,	12,	0,
	"",			"!Tiger!",		""
    },

    {
	"teleport",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		SPELL_NONE,	35,	12,	0,
	"",			"!Teleport!",		""
    },
    {
	"telekinetic force",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_telekinetic_force,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	15,	12,	0,
	"telekinetic force",	"!Telekinetic Force!",	""
    },

     {
	"truesight",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_truesight,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"You feel momentarily disoriented as your truesight fades.",	
	""
    },

    {
	"ventriloquate",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"!Ventriloquate!",	""
    },

    {
       "wail of the banshee",    {-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
       spell_wail_of_the_banshee, TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
       &gsn_wail,               SPELL_NONE,     20,     12,     0,
       "spell",                 "Your hearing returns.", "$n can hear again."
    },

    {
	"war mount",          {-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,            TAR_IGNORE,              POS_FIGHTING,
	&gsn_warmount,          SPELL_NONE,       0,      0,     0,
	"",                    "!War Mount!",           ""
    },

    {
	"weaken",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	20,	12,	0,
	"spell",		"You feel stronger.",	""
    },

    {
	"weariness",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_weariness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_weariness,		SPELL_NONE,	20,	12,	0,
	"spell",		"You don't feel nearly so weary.",	"$n perks up."
    },

    {
	"word of recall",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			SPELL_NONE,	 5,	12,	0,
	"",			"!Word of Recall!",	""
    },

/*
 * Dragon breath
 */
    {
	"acid breath",		{ },	{ },	{ },
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	100,	24,	0,
	"blast of acid",	"!Acid Breath!",	""
    },

    {
	"fire breath",		{ },	{ },	{ },
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	200,	24,	0,
	"blast of flame",	"The smoke leaves your eyes.",	""
    },

    {
	"frost breath",		{ },	{ },	{ },
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	125,	24,	0,
	"blast of frost",	"!Frost Breath!",	""
    },

    {
	"gas breath",		{ },	{ },	{ },
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SPELL_NONE,	175,	24,	0,
	"blast of gas",		"!Gas Breath!",		""
    },

    {
	"lightning breath",	{ },	{ },	{ },
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SPELL_NONE,	150,	24,	0,
	"blast of lightning",	"!Lightning Breath!",	""
    },

    {
        "channel",		{ },	{ },	{ },
        spell_channel,          TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   SPELL_NONE,        5,     12,     0,
        "",                     "You loose grip on the magic.",
        "",                     ""
    },

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",	{ },	{ },	{ },
        spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_NONE,      0,      12,	0,
        "general purpose ammo", "!General Purpose Ammo!",	""
    },

    {
        "high explosive",	{ },	{ },	{ },
        spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_NONE,      0,      12,	0,
        "high explosive ammo",  "!High Explosive Ammo!",	""
    },


/* combat and weapons skills */


    {
	"axe",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_axe,            	SPELL_NONE,       0,      0,	0,
        "",                     "!Axe!",		""
    },

    {
	"bastard sword",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_bastardsword,	SPELL_NONE,	0,	0,	0,
	"",			"!Bastardsword!",	""
    },

    {
	"bow",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_bow,		SPELL_NONE,	0,	0,	0,
	"",			"!Bow!",	""
    },

    {
	"broadsword",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_broadsword,	SPELL_NONE,	0,	0,	0,
	"",			"!Broadsword!",	""
    },

    {
	"club",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_club,		SPELL_NONE,	0,	0,	0,
        "",			"!Club!",		""
    },

    {
	"crossbow",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_crossbow,	SPELL_NONE,	0,	0,	0,
	"",			"!Crossbow!",	""
    },

    {
	"dagger",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_dagger,            SPELL_NONE,	0,	0,	0,
        "",			"!Dagger!",		""
    },

    {
	"dirk",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dirk,		SPELL_NONE,	0,	0,	0,
	"",			"!Dirk!",	""
    },

    {
	"flail",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_flail,            	SPELL_NONE,       0,      0,	0,
        "",                     "!Flail!",		""
    },

    {
	"great axe",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_greataxe,	SPELL_NONE,	0,	0,	0,
	"",			"!Greataxe!",	""
    },

    {
	"great mace",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_greatmace,	SPELL_NONE,	0,	0,	0,
	"",			"!Greatmace!",	""
    },

    {
	"hammer",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null, 		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hammer,		SPELL_NONE,	0,	0,	0,
	"",			"!Hammer!",		""
    },

    {
	"horse flail",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_horseflail,	SPELL_NONE,	0,	0,	0,
	"",			"!Horseflail!",	""
    },

    {
	"horse mace",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_horsemace,	SPELL_NONE,	0,	0,	0,
	"",			"!Horsemace!",	""
    },

    {
	"lance",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_lance,            	SPELL_NONE,	0,	0,	0,
	"",			"!Lance!",		""
    },

    {
	"longsword",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_longsword,         SPELL_NONE,       0,      0,	0,
        "",                     "!Longsword!",		""
    },

    {
	"mace",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_mace,            	SPELL_NONE,       0,      0,	0,
        "",                     "!Mace!",		""
    },

    {
	"polearm",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_polearm,           SPELL_NONE,       0,      0,	0,
        "",                     "!Polearm!",		""
    },

    {
	"rapier",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_rapier,            	SPELL_NONE,	0,	0,	0,
	"",			"!Rapier!",		""
    },

    {
	"sabre",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_sabre,            	SPELL_NONE,	0,	0,	0,
	"",			"!Sabre!",		""
    },

    {
	"scimitar",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_scimitar,		SPELL_NONE,	0,	0,	0,
	"",			"!Scimitar!",	""
    },

    {
	"shortsword",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shortsword,	SPELL_NONE,	0,	0,	0,
	"",			"!Shortsword!",	""
    },

    {
	"spear",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_spear,            	SPELL_NONE,       0,      0,	0,
        "",                     "!Spear!",		""
    },

    {
        "staff",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_staff,		SPELL_NONE,	0,      0,	0,
        "",                     "!Staff!",		""
    },

    {
        "stiletto",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_stiletto,		SPELL_NONE,	0,      0,	0,
        "",                     "!Stiletto!",		""
    },

    {
	"sword",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_sword,            	SPELL_NONE,	0,	0,	0,
	"",			"!sword!",		""
    },

    {
	"two-handed sword",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_twohandsword,	SPELL_NONE,	0,	0,	0,
	"",			"!Twohandsword!",	""
    },


    {
	"whip",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_whip,		SPELL_NONE,	0,	0,	0,
	"",			"!Whip!",	""
    },

    {
        "assassinate",             {-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_assassinate,          SPELL_NONE,        0,     24,   0,
        "assassinate",             "!Assassinate!",           ""
    },

    {
        "backstab",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_backstab,          SPELL_NONE,        0,     24,	0,
        "backstab",             "!Backstab!",		""
    },

    {
	"bash",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_bash,            	SPELL_NONE,       0,      24,	0,
        "bash",                 "!Bash!",		""
    },
        {
	"blackjack",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_blackjack,            	SPELL_NONE,       0,      24,	0,
        "blackjack",                 "!Blackjack!",		""
    },
        {
	"whirlwind",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_whirlwind,            	SPELL_NONE,       0,      24,	0,
        "whirlwind",                 "!Whirlwind!",		""
    },

    {
	"berserk",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,        	SPELL_NONE,       0,      24,	0,
        "",                     "You feel your pulse slow down.",	""
    },

    {
        "charge",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_charge,		SPELL_NONE,	0,	24,	0,
        "charge",		"!Charge!",		""
    },

    {
        "circle",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_circle,		SPELL_NONE,	0,	24,	0,
        "circle",		"!Circle!",		""
    },

    {
        "counter",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_counter,           SPELL_NONE,       0,      0,  0,
        "counterattack",        "!Counter!",   ""
    },

    {
	"dirt kicking",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dirt,		SPELL_NONE,	0,	24,	0,
	"kicked dirt",		"You rub the dirt out of your eyes.",	""
    },

    {
        "disarm",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_disarm,            SPELL_NONE,        0,     24,	0,
        "",                     "!Disarm!",		""
    },

    {
        "dodge",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dodge,             SPELL_NONE,        0,     0,	0,
        "",                     "!Dodge!",		""
    },

    {
	"dream",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_dream,		SPELL_NONE,	0,	3,	0,
	"",			"The effects of the dream fade.", ""
    },

    {
	"dual",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_dual,		SPELL_NONE,	0,	3,	0,
	"",			"!dual",		""
    },
    
	{
        "engage",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_engage,            SPELL_NONE,        0,     24,	0,
        "",                     "!Engage!",		""
    },

    {
        "enhanced damage",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_enhanced_damage,   SPELL_NONE,        0,     0,	0,
        "",                     "!Enhanced Damage!",	""
    },

    {
	"envenom",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,	  	POS_RESTING,
	&gsn_envenom,		SPELL_NONE,	0,	36,	0,
	"",			"!Envenom!",		""
    },

    {
	"hand to hand",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hand_to_hand,	SPELL_NONE,	0,	0,	0,
	"",			"!Hand to Hand!",	""
    },

    {
        "kick",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_kick,              SPELL_NONE,        0,     12,	0,
        "kick",                 "!Kick!",		""
    },

    {
        "parry",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_parry,             SPELL_NONE,        0,     0,	0,
        "",                     "!Parry!",		""
    },

    {
        "phase",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_phase,             SPELL_NONE,       0,      0,	0,
        "",                     "!Phase!",   ""
    },


    {
        "rescue",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_rescue,            SPELL_NONE,        0,     12,	0,
        "",                     "!Rescue!",		""
    },

    {
	"trip",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_trip,		SPELL_NONE,	0,	24,	0,
	"trip",			"!Trip!",		""
    },

    {
        "second attack",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_second_attack,     SPELL_NONE,        0,     0,	0,
        "",                     "!Second Attack!",	""
    },

    {
	"shield block",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shield_block,	SPELL_NONE,	0,	0,	0,
	"",			"!Shield!",		""
    },

    {
        "third attack",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_third_attack,      SPELL_NONE,        0,     0,	0,
        "",                     "!Third Attack!",	""
    },

    {	"fourth attack",	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fourth_attack,	SPELL_NONE,	0,	0,	0,
	"",			"!Fourth Attack!",	""
    },

    {   "fifth attack", 	{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_fifth_attack,     SPELL_NONE,       0,      0,      0,
        "",                     "!Fifth Attack!",      ""
    },


/* non-combat skills */

    { 
	"fast healing",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_fast_healing,	SPELL_NONE,	0,	0,	0,
	"",			"!Fast Healing!",	""
    },

    {	"glance",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_glance,		SPELL_NONE,	0,	0,	0,
	"",			"!Glance!",		""
    },

    {
	"haggle",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_haggle,		SPELL_NONE,	0,	0,	0,
	"",			"!Haggle!",		""
    },

    {
	"hide",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		SPELL_NONE,	 0,	12,	0,
	"",			"!Hide!",		""
    },

    {
	"lore",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_lore,		SPELL_NONE,	0,	36,	0,
	"",			"!Lore!",		""
    },

    {
	"meditation",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_meditation,	SPELL_NONE,	0,	0,	0,
	"",			"Meditation",		""
    },

    {
	"peek",			{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		SPELL_NONE,	 0,	 0,	0,
	"",			"!Peek!",		""
    },

    {
	"pick lock",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		SPELL_NONE,	 0,	12,	0,
	"",			"!Pick!",		""
    },

    {
	"riding",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_riding,		SPELL_NONE,	0,	4,	0,
	"",			"!Riding!",	""
    },
    
    {
	"butcher",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_butcher,		SPELL_NONE,	0,	4,	0,
	"",			"!Butcher!",	""
    },
    {
	"skin",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_skin,		SPELL_NONE,	0,	4,	0,
	"",			"!Skin!",	""
    },
    {
	"sharpen",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sharpen,		SPELL_NONE,	0,	4,	0,
	"",			"!Sharpen!",	""
    },

    {
	"sneak",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		SPELL_NONE,	 0,	12,	0,
	"",			"You no longer feel stealthy.",	""
    },

    {
	"steal",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		SPELL_NONE,	 0,	24,	0,
	"",			"!Steal!",		""
    },

    {
	"scrolls",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scrolls,		SPELL_NONE,	0,	24,	0,
	"",			"!Scrolls!",		""
    },

    {
	"spellcraft",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_spellcraft,	SPELL_NONE,	0,	12,	0,
	"",			"!Spellcraft!",		"",	""
    },

    {
	"staves",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_staves,		SPELL_NONE,	0,	12,	0,
	"",			"!Staves!",		""
    },

     {
	"swimming",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_swimming,		SPELL_NONE,	0,	0,	12,
	"",			"!Swimming!",		""
    },

  { 
	"track",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_track,	        SPELL_NONE,	0,	0,	10,
	"",			"!Track!",		"",	""
    },
    {
	"triage",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	NULL,			SPELL_NONE,	0,	0,	0,
	"",			"!Triage!",		"",	""
    },
    

    {
	"wands",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_wands,		SPELL_NONE,	0,	12,	0,
	"",			"!Wands!",		""
    },

    {
	"herbal lore",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_herbs,		SPELL_NONE,	0,	12,	0,
	"",			"",			""
    },
    {
	"purify",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_purify,		SPELL_NONE,	0,	12,	0,
	"",			"!Purify!",		""
    },
    {
	"recall",		{-1,-1,-1,-1,-1 },	{0,0,0,0,0},	{ },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_recall,		SPELL_NONE,	0,	12,	0,
	"",			"!Recall!",		""
    },


    /* end of table */
    {
	NULL,			{ },	{ },	{ },
	NULL,			0,			0,
	NULL,			0,	0,	0,	0,
	NULL,			NULL,			NULL
    }

};

