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

/***************************************************************************
 * This header file contains the structs and definitions for iGen.         *
 *   -------------------------------------------------------------------   *
 * This code may be used freely within any non-commercial MUD, all I ask   *
 * is that these comments remain in tact and that you give me any feedback *
 * or bug reports you come up with.  Credit in a helpfile might be nice,   *
 * too.                             -- Midboss (eclipsing.souls@gmail.com) *
 ***************************************************************************/
#define RANDOM_WEAPON	0
#define RANDOM_SHIELD	1
#define RANDOM_ARMOR	2
#define RANDOM_TRINKET	3
#define RANDOM_POTION	4
#define RANDOM_SCROLL	5
#define RANDOM_PILL		6
#define RANDOM_STAFF	7
#define RANDOM_WAND		8
#define MAX_RANDTYPE	9

//This macro returns true if the letter is a vowel.
#define IS_VOWEL(c) (   c == 'a' || c == 'A' || c == 'e' || c == 'E' \
                     || c == 'i' || c == 'I' || c == 'o' || c == 'O' \
					 || c == 'u' || c == 'U')

struct random_type
{
	char *		name;			//Base item name field.
	char *		desc;			//Base item descriptions.
	int			value;			//Cost of the item.
	sh_int		weight;			//Weight of the item.
	sh_int		type;			//Random type.
	sh_int		wtype;			//For weapons only.
	char *		dtype;			//For weapons only.
	sh_int		level[3];		//Valid level range and level bonus.
	sh_int		rarity;			//Drop chance.
	long		wear;			//Wear flags.
	sh_int		app_loc[3];		//The TO_* location for the apply.
	sh_int		app_type[3];	//The APPLY_* type for the apply.
	sh_int		app_mod[3];		//The modifier for the apply.
	int			app_flag[3];	//The bitvector for the apply.
	bool		unique;			//Can't have suffix or prefix?
};

struct mod_type
{
	char *		name;			//Name addendum.
	char *		desc;			//Short description addendum.
	sh_int		app_loc[3];		//The TO_* location for the apply.
	sh_int		app_type[3];	//The APPLY_* type for the apply.
	sh_int		app_mod[3];		//The modifier for the apply.
	int			app_flag[3];	//The bitvector for the apply.
	sh_int		chance[9];		//Chance of loading on each type.
	sh_int		weight_mod;		//Extra weight.
	int			value_mod;		//Extra price.
	sh_int		level_mod[2];	//Pre-Mod and After-Mod.
	long		extra_flags;	//Extra flags to be added.
	char *		spells[2];
};


extern const struct mod_type	prefix_table[];
extern const struct random_type random_table[];
extern const struct mod_type	suffix_table[];

void iGen (CHAR_DATA * ch, CHAR_DATA * mob, int level);
