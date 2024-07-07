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

/***************************************************************************************\
[*]:::::::..       ...     .        :     .,-:::::  [*] ROMC: Rivers of MUD Continued [*]
[*];;;;``;;;;   .;;;;;;;.  ;;,.    ;;;  ,;;;'````'  [*]     All rights reserved       [*]
[*] [[[,/[[['  ,[[     \[[,[[[[, ,[[[[, [[[         [*]    Copyright(C) 2018-2021     [*]
[*] $$$$$$c    $$$,     $$$$$$$$$$$"$$$ $$$         [*]     Ro Black (Pyrasti)        [*]
[*] 888b "88bo,"888,_ _,88P888 Y88" 888o`88bo,__,o, [*]        ro@wr3tch.com          [*]
[*] MMMM   "W"   "YMMMMMP" MMM  M'  "MMM  "YUMMMMMP"[*]                               [*]
[-]---------------------------------------+-+-----------------------------------------[-]
[*]                 This MUD based on... Rivers of MUD Continued.                     [*]
[*]              ROGUE Version 2.1 Alpha by Kenneth Conley (Mendanbar)                [*]
[*]        Original DikuMUD by Hans Staerfeldt, Katja Nyboe, Tom Madsen, Michael      [*]
[*]          Seifert, and Sebastian Hammer. Original MERC 2.1 code by Hatchet,        [*]
[*]        Furey, and Kahn. Original ROM 2.4 copyright 1993-1998 by Russ Taylor.      [*]
[*]                             From: wr3tch.com:3033                                 [*]
\***************************************************************************************/
/***************************************************************************
 * This source file contains the tables used by the iGen function.         *
 *   -------------------------------------------------------------------   *
 * This code may be used freely within any non-commercial MUD, all I ask   *
 * is that these comments remain in tact and that you give me any feedback *
 * or bug reports you come up with.  Credit in a helpfile might be nice,   *
 * too.                             -- Midboss (eclipsing.souls@gmail.com) *
 ***************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include "merc.h"
#include "igen.h"

/* mod_type 
 * {
 * "prefix", "desc",
 * apply_loc, apply_type, 
 * apply_mod, apply_flag, 
 * chance,
 * weight mod, value mod, level mod,
 * extra flags,
 * spells 
 * */
const struct mod_type	prefix_table[] ={
	{
	 "", "not a real prefix",
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {256, 256, 256, 256, 256, 256, 256, 256, 256},
	 0, 0, {0, 0}, 0, {NULL, NULL}
	},

	{
	 "mastercrafted", "mastercrafted",
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {256, 256, 256, 256, 256, 256, 256, 256, 256},
	 0, 1500, {0, 0}, 0,
	 {NULL, NULL}
	},

	{
	 "rare", "rare",
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {215, 215, 215, 215, 215, 215, 215, 215, 215},
	 0, 500, {0, 0}, 0,
	 {NULL, NULL}
	},

{
	 "uncommon", "uncommon",
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {150, 150, 150, 150, 150, 150, 150, 150, 150},
	 0, 50, {0, 0}, 0,
	 {NULL, NULL}
	},
	{
	 "common", "common",
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {80, 80, 80, 80, 80, 80, 80, 80, 80},
	 0, 0, {0, 0}, 0,
	 {NULL, NULL}
	},
	{
	 "heavily-used", "heavily-used",
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {50, 50, 50, 50, 50, 50, 50, 50, 50},
	 0, -500, {0, 0}, 0,
	 {NULL, NULL}
	},

	{
	 "cursed", "cursed",
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {256, 256, 256, 256, 256, 128, 256, 256, 256},
	 1, -50, {-2, 2}, 0,
	 {"slow", NULL}
	},

	{
	 NULL, NULL,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {256, 256, 256, 256, 256, 256, 256, 256, 256},
	 0, 0, {0, 0}, 0, {NULL, NULL}
	}
};
/* random_type 
 * name, desc, value, weight,
 * type, wtype, 
 * damtype, level, rarity, wear,
 * apply_loc, apply_type,
 * apply_mod, apply_flag, unique?
 * */
const struct random_type random_table[] ={

	{
	 "null","not an object",0,0,
     RANDOM_TRINKET,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "necklace","necklace",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|C,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "shirt","shirt",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|D,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "helmet","helmet",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|E,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "breeches","breeches",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|F,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "boots","boots",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|G,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "gloves","gloves",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|H,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "sleeves","sleeves",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|I,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "armor","armor",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|K,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "belt","belt",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|L,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "bracer","bracer",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|M,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "ioun stone","ioun stone",0,0,
     RANDOM_TRINKET,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|Q,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "mask","mask",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|S,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "pauldrons","pauldrons",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|W,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "lenses","lenses",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|R,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "earring","earring",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|T,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "anklet","anklet",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|X,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
{
	 "tail armor","tail armor",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|Y,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
	{
	 "horn caps","horn caps",0,0,
     RANDOM_ARMOR,WEAPON_EXOTIC,
	 "slash",{0,20,0},80,A|Z,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},

	{
	 "ring", "ring", 150, 1,
     RANDOM_TRINKET, WEAPON_EXOTIC, "slash",
	 {0,20,0}, 156, A|B,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},

	{
	 "scroll", "scroll", 20, 1,
     RANDOM_SCROLL, WEAPON_EXOTIC, "slash",
	 {0,20,0}, 95, A,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
	{
	 "wand", "wand", 20, 1,
     RANDOM_WAND, WEAPON_EXOTIC, "slash",
	 {0,20,0}, 95, A,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
	{
	 "staff", "staff", 20, 1,
     RANDOM_STAFF, WEAPON_EXOTIC, "slash",
	 {0,20,0}, 95, A,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},

	{
	 "shield", "tower shield", 300, 1,
     RANDOM_SHIELD, WEAPON_EXOTIC, "slash",
	 {0,20,0}, 115, A|J,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},

	{
	 "potion", "potion", 60, 1,
     RANDOM_POTION, WEAPON_EXOTIC, "slash",
	 {0,20,0}, 76, A,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
	{
	 "pill", "pill", 60, 1,
     RANDOM_PILL, WEAPON_EXOTIC, "slash",
	 {0,20,0}, 76, A,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},

	{
	 "sword", "Sword", 2000, 4,
     RANDOM_WEAPON, WEAPON_SWORD, "slash",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, TO_WEAPON},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, WEAPON_SHARP}, FALSE
	},
{
	 "dagger", "Dagger", 10000, 4,
     RANDOM_WEAPON, WEAPON_DAGGER, "stab",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, TO_WEAPON},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, WEAPON_SHARP}, FALSE
	},
{
	 "spear", "Spear", 10000, 4,
     RANDOM_WEAPON, WEAPON_SPEAR, "thrust",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, TO_WEAPON},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, WEAPON_TWO_HANDS}, FALSE
	},
	{
	 "mace", "Mace", 10000, 4,
     RANDOM_WEAPON, WEAPON_MACE, "pound",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, -1},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, 0}, FALSE
	},
	{
	 "axe", "Axe", 10000, 4,
     RANDOM_WEAPON, WEAPON_AXE, "cleave",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, TO_WEAPON},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, WEAPON_SHARP}, FALSE
	},
	{
	 "flail", "Flail", 10000, 4,
     RANDOM_WEAPON, WEAPON_FLAIL, "crush",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, -1},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, 0}, FALSE
	},
	{
	 "whip", "Whip", 10000, 4,
     RANDOM_WEAPON, WEAPON_WHIP, "bite",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, -1},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, 0}, FALSE
	},
	{
	 "polearm", "Polearm", 10000, 4,
     RANDOM_WEAPON, WEAPON_POLEARM, "thrust",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, TO_WEAPON},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, WEAPON_TWO_HANDS}, FALSE
	},
	{
	 "dualblade", "Dualblade", 10000, 4,
     RANDOM_WEAPON, WEAPON_EXOTIC, "stab",
	 {0,20,0}, 141, A|N,
	 {TO_AFFECTS, TO_AFFECTS, TO_WEAPON},
	 {APPLY_DAMROLL, APPLY_HITROLL, APPLY_NONE},
	 {5, 5, 0}, {0, 0, WEAPON_SHARP}, FALSE
	},	

	{
	 NULL,NULL,0,0,
     RANDOM_TRINKET,WEAPON_EXOTIC,
	 "slash",{0,100,0},256,A,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0}, FALSE
	},
};

/* mod_type 
 * {
 * "prefix", "desc",
 * apply_loc, apply_type, 
 * apply_mod, apply_flag, 
 * chance,
 * weight mod, value mod, level mod,
 * extra flags,
 * spells 
 * */
const struct mod_type	suffix_table[] ={
	{
	 "", "not a real suffix",
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {256, 256, 256, 256, 256, 256, 256, 256, 256},
	 0, 0, {0, 0}, 0, {NULL, NULL}
	},

	{
	 "sanctuary", "of sanctuary",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {AFF_SANCTUARY, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_GLOW,
	 {"sanctuary", NULL}
	},

	{
	 "haste", "of haste",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_DEX, -1, -1},
	 {2, 0, 0}, {AFF_HASTE, 0, 0},
     {220, 195, 210, 170, 140, 140, 140, 150, 175},
	 0, 150, {0, 2}, 0,
	 {"haste", NULL}
	},

	{
	 "flight", "of flight",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"fly", NULL}
	},
	{
	 "natural armor", "of natural armor",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"armor", NULL}
	},
	{
	 "invisibility", "of invisibility",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {AFF_INVISIBLE, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"invis", NULL}
	},
	{
	 "truesight", "of truesight",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"truesight", NULL}
	},
	{
	 "regeneration", "of regeneration",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {AFF_REGENERATION, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"refresh", NULL}
	},
	{
	 "night sight", "of night sight",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {AFF_DARK_VISION, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"infrared", NULL}
	},

	{
	 "strength of bull", "of strength of the bull",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"bulls_strength", NULL}
	},
	{
	 "passing", "of passing",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {AFF_PASS_DOOR, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"pass door", NULL}
	},
	{
	 "protect good", "of protection from good",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {AFF_PROTECT_GOOD, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"protection good", NULL}
	},
	{
	 "protect evil", "of protection from evil",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {0, 0, 0}, {AFF_PROTECT_EVIL, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {"protection evil", NULL}
	},

        {
	 "damaging", "of damaging",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_DAMROLL, -1, -1},
	 {10, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_GLOW,
	 {NULL, NULL}
	},
	
	{
	 "lesser damaging", "of lesser damaging",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_DAMROLL, -1, -1},
	 {5, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "greater damaging", "of greater damaging",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_DAMROLL, -1, -1},
	 {20, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "lesser precision", "of lesser precision",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_HITROLL, -1, -1},
	 {5, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "precision", "of precision",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_HITROLL, -1, -1},
	 {10, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "greater precision", "of greater precision",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_HITROLL, -1, -1},
	 {20, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "lesser health", "of lesser health",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_HIT, -1, -1},
	 {5, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "lesser mana", "of lesser mana",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_MANA, -1, -1},
	 {5, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "health", "of health",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_HIT, -1, -1},
	 {10, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "mana", "of mana",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_MANA, -1, -1},
	 {10, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "greater health", "of greater health",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_HIT, -1, -1},
	 {20, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "greater mana", "of greater mana",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_MANA, -1, -1},
	 {20, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "lesser strength", "of lesser strength",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_STR, -1, -1},
	 {1, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	
	{
	 "lesser nimbleness", "of lesser nimbleness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_DEX, -1, -1},
	 {1, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_MAGIC,
	 {NULL, NULL}
	},
{
	 "lesser heartiness", "of lesser heartiness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_CON, -1, -1},
	 {1, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_MAGIC,
	 {NULL, NULL}
	},	
	{
	 "lesser brilliance", "of lesser brilliance",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_INT, -1, -1},
	 {1, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "lesser worldliness", "of lesser worldliness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_WIS, -1, -1},
	 {1, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "lesser striding", "of lesser striding",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_MOVE, -1, -1},
	 {5, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 0}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "strength", "of strength",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_STR, -1, -1},
	 {2, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	
	{
	 "nimbleness", "of nimbleness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_DEX, -1, -1},
	 {2, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_MAGIC,
	 {NULL, NULL}
	},
{
	 "heartiness", "of heartiness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_CON, -1, -1},
	 {2, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_MAGIC,
	 {NULL, NULL}
	},	
	{
	 "brilliance", "of brilliance",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_INT, -1, -1},
	 {2, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "worldliness", "of worldliness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_WIS, -1, -1},
	 {2, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "striding", "of striding",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_MOVE, -1, -1},
	 {10, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "greater strength", "of greater strength",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_STR, -1, -1},
	 {3, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	
	{
	 "greater nimbleness", "of greater nimbleness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_DEX, -1, -1},
	 {3, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {NULL, NULL}
	},
{
	 "greater heartiness", "of greater heartiness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_CON, -1, -1},
	 {3, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {NULL, NULL}
	},	
	{
	 "greater brilliance", "of greater brilliance",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_INT, -1, -1},
	 {3, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "greater worldliness", "of greater worldliness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_WIS, -1, -1},
	 {3, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 1}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "greater striding", "of greater striding",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_MOVE, -1, -1},
	 {20, 0, 0}, {0, 0, 0},
     {255, 215, 240, 200, 170, 170, 170, 190, 195},
	 0, 200, {0, 2}, ITEM_MAGIC,
	 {NULL, NULL}
	},
	{
	 "weakness", "of weakness",
	 {TO_AFFECTS, -1, -1},
	 {APPLY_SPELL_AFFECT, -1, -1},
	 {10, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, 200, {0, -2}, ITEM_GLOW,
	 {"slow", NULL}
	},
	{
	 "freezing", "weakness to freezing",
	 {TO_VULN, -1, -1},
	 {VULN_COLD, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	
	{
	 "burning", "weakness to burning",
	 {TO_VULN, -1, -1},
	 {VULN_FIRE, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	
	{
	 "acidity", "weakness to acidity",
	 {TO_VULN, -1, -1},
	 {VULN_ACID, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "poisoned", "weakness to poisoning",
	 {TO_VULN, -1, -1},
	 {VULN_POISON, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "negativity", "weakness to negativity",
	 {TO_VULN, -1, -1},
	 {VULN_NEGATIVE, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "celestial", "weakness to holy",
	 {TO_VULN, -1, -1},
	 {VULN_HOLY, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "energetic", "weakness to energy",
	 {TO_VULN, -1, -1},
	 {VULN_ENERGY, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "diseased", "weakness to plague",
	 {TO_VULN, -1, -1},
	 {VULN_DISEASE, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "mental", "weakness of mind",
	 {TO_VULN, -1, -1},
	 {VULN_MENTAL, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "shocking", "weakness to shocking",
	 {TO_VULN, -1, -1},
	 {VULN_LIGHTNING, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "sensitivity", "sensitivity to sound",
	 {TO_VULN, -1, -1},
	 {VULN_SOUND, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},
	{
	 "light-sensitivity", "sensitivity to light",
	 {TO_VULN, -1, -1},
	 {VULN_LIGHT, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {255, 255, 255, 255, 255, 255, 255, 255, 255},
	 0, -200, {0, -2}, ITEM_GLOW,
	 {NULL, NULL}
	},

	{
	 NULL, NULL,
	 {-1, -1, -1}, {-1, -1, -1},
	 {0, 0, 0}, {0, 0, 0},
     {256, 256, 256, 256, 256, 256, 256, 256, 256},
	 0, 0, {0, 0}, 0, {NULL, NULL}
	}
};
