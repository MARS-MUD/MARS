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
 * This source file contains the iGen random item generator itself.        *
 *   -------------------------------------------------------------------   *
 * This code may be used freely within any non-commercial MUD, all I ask   *
 * is that these comments remain in tact and that you give me any feedback *
 * or bug reports you come up with.  Credit in a helpfile might be nice,   *
 * too.                             -- Midboss (eclipsing.souls@gmail.com) *
 ***************************************************************************/
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include "merc.h"
#include "igen.h"
#include "interp.h"
#include "recycle.h"

void iGen (CHAR_DATA * ch, CHAR_DATA * mob, int level)
{
	int i, prefix = 0, item = 0, suffix = 0, cLevel;
    AFFECT_DATA *pAf;
	OBJ_DATA * obj;
	char buf[MSL];

	//Determine what item.
	for (i = 0; random_table[i].name != NULL; i++)
	{
		//Make sure it fits the level range.
		if (   level < random_table[i].level[0]
			|| level > random_table[i].level[1])
			continue;

		//Now check the rarity.
		if (number_range (0, 256) > random_table[i].rarity)
		{
			//If a rarer item already popped, skip.
			if (random_table[item].rarity > random_table[i].rarity && item != 0)
				continue;

			//If the items are the same rarity, 50/50 chance each.			
			else if (random_table[item].rarity == random_table[i].rarity
					 && number_percent () < 50)
				continue;

			//All is well, new item.
			else
				item = i;
		}
	}

	//No blank items!
	if (item == 0)
		return;

	//If the item can have a prefix/suffix...
	if (!random_table[item].unique)
	{
		//Check for prefixes...
		for (i = 0; prefix_table[i].name != NULL; i++)
		{
			//Check the rarity.
			if (number_range (0, 256) > prefix_table[i].chance[random_table[item].type])
			{
				//If a rarer prefix is already set, skip.
				if (prefix_table[prefix].chance[random_table[item].type] >
					prefix_table[i].chance[random_table[item].type] && prefix != 0)
					continue;

				//Just like items, if they're the same rarity, 50/50.
				else if (prefix_table[prefix].chance[random_table[item].type] ==
						 prefix_table[i].chance[random_table[item].type]
						 && number_percent () < 50)
					continue;

				//Set the new prefix.
				else
					prefix = i;
			}
		}

		//Check for suffixes...
		for (i = 0; suffix_table[i].name != NULL; i++)
		{
			//Check the rarity.
			if (number_range (0, 256) > suffix_table[i].chance[random_table[item].type])
			{
				//Just like befor, if a rarer suffix is already set, skip.
				if (suffix_table[suffix].chance[random_table[item].type] >
					suffix_table[i].chance[random_table[item].type] && suffix != 0)
					continue;

				//Again, if they're the same rarity, 50/50.
				else if (suffix_table[suffix].chance[random_table[item].type] ==
						 suffix_table[i].chance[random_table[item].type]
						 && number_percent () < 50)
					continue;

				//Set the new suffix.
				else
					suffix = i;
			}
		}
	}


	//Determine the actual level and create the dummy object.
	obj = create_object (get_obj_index (OBJ_VNUM_DUMMY), level);
	
	//Set the strings.
	free_string (obj->name);
	free_string (obj->short_descr);
	free_string (obj->description);


	/*
	 * Alright, this is all pretty standard stuff.  If you don't know how to
	 * work sprintf, you probably shouldn't be running a MUD.
	 */

	sprintf (buf, "%s%s%s%s%s",
				prefix != 0 ? prefix_table[prefix].name : "",
				prefix != 0 ? " " : "",
				random_table[item].name,
				suffix != 0 ? " " : "",
				suffix != 0 ? suffix_table[suffix].name : "");
	obj->name = str_dup (buf);

	sprintf (buf, "%s %s%s%s%s%s",
				random_table[item].unique ? "the" :
				prefix != 0 ? IS_VOWEL (prefix_table[prefix].desc[0]) ?
				"an" : "a" : IS_VOWEL (random_table[item].desc[0]) ?
				"an" : "a", prefix != 0 ? prefix_table[prefix].desc : "",
				prefix != 0 ? " " : "", random_table[item].desc,
				suffix != 0 ? " " : "",
				suffix != 0 ? suffix_table[suffix].desc : "");
	obj->short_descr = str_dup (buf);

	sprintf (buf, "%s %s%s%s%s%s lies here.",
				random_table[item].unique ? "The" :
				prefix != 0 ? IS_VOWEL (prefix_table[prefix].desc[0]) ?
				"An" : "A" : IS_VOWEL (random_table[item].desc[0]) ? 
				"An" : "A", prefix != 0 ? prefix_table[prefix].desc : "",
				prefix != 0 ? " " : "", random_table[item].desc,
				suffix != 0 ? " " : "",
				suffix != 0 ? suffix_table[suffix].desc : "");
	obj->description = str_dup (buf);


	//Now, copy the short desc to the full desc.
	obj->description = str_dup (obj->short_descr);

	//Set the basic values.
	obj->wear_flags = random_table[item].wear;
	obj->cost		= random_table[item].value;
	obj->weight		= random_table[item].weight;
	obj->level		= level;

	//Now to handle auto-set for the base item.

	//Grab the level.
	cLevel = level + random_table[item].level[2] +
			 prefix_table[prefix].level_mod[0] +
			 suffix_table[suffix].level_mod[0];

	switch (random_table[item].type)
	{
		default:
		case RANDOM_WEAPON:
			//Set the types.
			obj->item_type = ITEM_WEAPON;
			obj->value[0] = random_table[item].wtype;

			//Set the damtype, and make sure it's there.
			obj->value[3] = attack_lookup (random_table[item].dtype);

			if (obj->value[3] < 0)
				obj->value[3] = attack_lookup ("slash");


			//I borrow the math here from Blade of E's autoset snippet.
			obj->value[1] = (cLevel / 6) + 2;
 
            /*
		  	 * Check every size until the next size goes over the average.
			 * If you want to adjust the average damage, change the cLevel
			 * multiplication.
			 */
			for (obj->value[2] = obj->value[1] / 2;
				 (obj->value[1] * (obj->value[2] + 1)) / 2 < cLevel * 1.3;
				 obj->value[2]++) { }

			//Modify the dice based on weapon type.
			switch (obj->value[0])
			{
				default:
				case WEAPON_EXOTIC:
				case WEAPON_SWORD:
					break;
				case WEAPON_DAGGER:
					obj->value[1] = UMAX(1, obj->value[1] + 1);
					obj->value[2] = UMAX(2, obj->value[2] - 1);
				  break;
				case WEAPON_SPEAR:
				case WEAPON_POLEARM:
				case WEAPON_MACE:
					obj->value[2]++;
					break;
				case WEAPON_AXE:
					obj->value[2] += 2;
					break;
				case WEAPON_FLAIL:
				case WEAPON_WHIP:
					obj->value[1] = UMAX(1, obj->value[1] + 1);
					break;
			}

			//Randomize the values.
			obj->value[1] = number_range (obj->value[1] - 1, obj->value[1] + 1);
			obj->value[2] = number_range (obj->value[2] - 3, obj->value[2] + 2);
			break;


		case RANDOM_SHIELD:
		case RANDOM_ARMOR:
			//Set the item type.
			obj->item_type = ITEM_ARMOR;

			//Determine the ACs.
			obj->value[0] = number_range (cLevel / 3, cLevel / 2);
			obj->value[1] = number_range (cLevel / 3, cLevel / 2);
			obj->value[2] = number_range (cLevel / 3, cLevel / 2);
			obj->value[3] = number_range (cLevel / 3, cLevel / 2.2);

			//Shields are a bit less powerful.
			if (random_table[item].type == RANDOM_SHIELD)
			{
				obj->value[0] *= .85;
				obj->value[1] *= .85;
				obj->value[2] *= .85;
				obj->value[3] *= .85;
			}
			break;

		case RANDOM_POTION:
		case RANDOM_PILL:
		case RANDOM_SCROLL:
			//Determine the item type.
			if (random_table[item].type == RANDOM_POTION)
				obj->item_type = ITEM_POTION;
			else if (random_table[item].type == RANDOM_PILL)
				obj->item_type = ITEM_PILL;
			else
				obj->item_type = ITEM_SCROLL;

			obj->value[0] = cLevel;

			//Pick random spells.
			if (cLevel > 40)
			{
				//Just modify this to set what spells are available in what range.
				char *const spells[] = {
					"heal", "armor", "refresh", "bless", "stone skin", "cure blindness",
					"cure poison", "cure disease", "faerie fire", "cancellation",
					"infravision", "haste", "invisibility", "sanctuary", "fly", "teleport",
					"giant strength", "detect evil", "detect good", "detect magic",
					"detect hidden", "detect invis", "frenzy", "sleep"

				};
				//Always gets one spell, has chances at more.
				obj->value[1] = skill_lookup (spells[number_range (0, 23)]);
				if (number_percent () < 80)
					obj->value[2] = skill_lookup (spells[number_range (0, 23)]);
				if (number_percent () < 60)
					obj->value[3] = skill_lookup (spells[number_range (0, 23)]);
				if (number_percent () < 40)
					obj->value[4] = skill_lookup (spells[number_range (0, 23)]);
			}
			else if (cLevel > 25)
			{
				char *const spells[] = {
					"cure critical", "armor", "refresh", "bless", "shield", "stone skin",
					"cure blindness", "cure poison", "cure disease", "faerie fire",
					"cancellation", "infravision", "haste", "fly", "teleport",
					"detect evil", "detect good", "detect magic", "detect hidden", "sleep"
				};
				obj->value[1] = skill_lookup (spells[number_range (0, 19)]);
				if (number_percent () < 75)
					obj->value[2] = skill_lookup (spells[number_range (0, 19)]);
				if (number_percent () < 50)
					obj->value[3] = skill_lookup (spells[number_range (0, 19)]);
				if (number_percent () < 25)
					obj->value[4] = skill_lookup (spells[number_range (0, 19)]);
			}
			else if (cLevel > 10)
			{
				char *const spells[] = {
					"cure serious", "armor", "refresh", "bless", "shield",
					"cure blindness", "cure poison", "infravision", "faerie fire",
					"fly", "teleport", "detect evil", "detect good", "detect magic",
					"detect hidden", "sleep"
				};
				obj->value[1] = skill_lookup (spells[number_range (0, 15)]);
				if (number_percent () < 60)
					obj->value[2] = skill_lookup (spells[number_range (0, 15)]);
				if (number_percent () < 40)
					obj->value[3] = skill_lookup (spells[number_range (0, 15)]);
				if (number_percent () < 20)
					obj->value[4] = skill_lookup (spells[number_range (0, 15)]);
			}
			else
			{
				char *const spells[] = {
					"cure light", "armor", "refresh", "bless", "faerie fire", "fly",
					"teleport", "detect evil", "detect good", "detect magic", "sleep",
					"cure poison"
				};

				obj->value[1] = skill_lookup (spells[number_range (0, 11)]);
				if (number_percent () < 40)
					obj->value[2] = skill_lookup (spells[number_range (0, 11)]);
				if (number_percent () < 20)
					obj->value[3] = skill_lookup (spells[number_range (0, 11)]);
				if (number_percent () < 10)
					obj->value[4] = skill_lookup (spells[number_range (0, 11)]);
			}

			//Failsafes.
			if (obj->value[1] < 1)
				obj->value[1] = skill_lookup ("cure poison");
			if (obj->value[2] < 0)
				obj->value[2] = 0;
			if (obj->value[3] < 0)
				obj->value[3] = 0;
			if (obj->value[4] < 0)
				obj->value[4] = 0;
			break;
		case RANDOM_STAFF:
		case RANDOM_WAND:
			//Determine the item type.
			if (random_table[item].type == RANDOM_STAFF)
				obj->item_type = ITEM_STAFF;
			else
				obj->item_type = ITEM_WAND;

			obj->value[0] = cLevel;
			obj->value[1] = number_range (cLevel / 5, cLevel / 2);
			obj->value[2] = number_range (obj->value[1] * .8, obj->value[1]);

			//Again, random spells.
			if (cLevel > 40)
			{
				//Just modify this to set what spells are available in what range.
				char *const spells[] = {
					"holy word", "demonfire", "chain lightning", "harm",
					"dispel evil", "dispel good", "dispel magic", "mass healing"
				};

				obj->value[3] = skill_lookup (spells[number_range (0, 7)]);
			}
			else if (cLevel > 25)
			{
				char *const spells[] = {
					"fireball", "lightning bolt", "chain lightning", "energy drain",
					"colour spray", "cause critical", "dispel evil", "dispel good"
				};

				obj->value[3] = skill_lookup (spells[number_range (0, 7)]);
			}
			else if (cLevel > 10)
			{
				char *const spells[] = {
					"chill touch", "burning hands", "shocking grasp", "fireball",
					"lightning bolt", "energy drain", "colour spray", "cause serious"
				};

				obj->value[3] = skill_lookup (spells[number_range (0, 7)]);
			}
			else
			{
				char *const spells[] = {
					"magic missile", "chill touch", "burning hands", "shocking grasp",
					"cause light"
				};

				obj->value[3] = skill_lookup (spells[number_range (0, 4)]);
			}

			//Failsafe.
			if (obj->value[3] < 1)
				obj->value[3] = skill_lookup ("fireball");
			break;
		case RANDOM_TRINKET:
			//Set the item type.
			obj->item_type = ITEM_JEWELRY;


			//Five chances...
			for (i = 0; i < 5; i++)
			{
				//Gets less likely each time.
				if (number_range (1, 55) > i * 10)
				{
					//Add an affect.
					pAf = new_affect ();
					pAf->where = TO_AFFECTS;
					pAf->type = -1;
					pAf->duration = -1;
					pAf->bitvector = 0;
					pAf->level = cLevel;

					//Choose a random bonus.
					switch (number_range (1, 8))
					{
						case 1:
							switch (number_range (1, 5))
							{
								case 1: pAf->location = APPLY_STR; break;
								case 2: pAf->location = APPLY_DEX; break;
								case 3: pAf->location = APPLY_INT; break;
								case 4: pAf->location = APPLY_CON; break;
								case 5: pAf->location = APPLY_WIS; break;
							}
							pAf->modifier = 1;
							break;
						case 2:
							pAf->location = APPLY_AC;
							pAf->modifier = 0 - UMAX(1, cLevel / 4);
							break;
						case 3:
							pAf->location = APPLY_DAMROLL;
							pAf->modifier = UMAX(1, cLevel / 5);
							break;
						case 4:
							pAf->location = APPLY_HITROLL;
							pAf->modifier = UMAX(1, cLevel / 5);
							break;
						case 5:
							pAf->location = APPLY_SAVES;
							pAf->modifier = 0 - UMAX(1, cLevel / 5);
							break;
						case 6:
							pAf->location = APPLY_HIT;
							pAf->modifier = 0 - UMAX(5, cLevel * .9);
							break;
						case 7:
							pAf->location = APPLY_MOVE;
							pAf->modifier = 0 - UMAX(5, cLevel * .9);
							break;
						case 8:
							pAf->location = APPLY_MANA;
							pAf->modifier = 0 - UMAX(5, cLevel * .9);
							break;
					}
					pAf->next = obj->affected;
					obj->affected = pAf;
				}
			}
			break;
	}

	//Now handle the applies and modifiers from the item.
	for (i = 0; i < 3; i++)
	{
		if (random_table[item].app_loc[i] != -1)
		{
			//This hack allows adding weapon flags in stock QuickMUD.
			if (random_table[item].app_loc[i] == TO_WEAPON)
			{
				//If the item type is weapon, add the flag to it.
				if (obj->item_type == ITEM_WEAPON)
					obj->value[4] |= random_table[item].app_flag[i];

				//Now add the stats, if there were any.
				if (random_table[item].app_mod[i] != 0)
				{
					pAf = new_affect ();
					pAf->where = TO_AFFECTS;
					pAf->type = -1;
					pAf->duration = -1;
					pAf->bitvector = 0;
					pAf->level = cLevel;
					pAf->location = random_table[item].app_type[i];
					pAf->modifier = random_table[item].app_mod[i];

					if ((pAf->location == APPLY_AC || pAf->location == APPLY_SAVES)
						&& pAf->modifier > 0)
						pAf->modifier = 0 - pAf->modifier;

					pAf->next = obj->affected;
					obj->affected = pAf;
				}
			}
			//Else, just add the affect as usual.
			else
			{

				pAf = new_affect ();
				pAf->where = random_table[item].app_loc[i];
				pAf->type = -1;
				pAf->duration = -1;
				pAf->bitvector = random_table[item].app_flag[i];
				pAf->level = cLevel;
				pAf->location = random_table[item].app_type[i];
				pAf->modifier = random_table[item].app_mod[i];

				//Make sure they're actually BONUSES.
				if ((pAf->location == APPLY_AC || pAf->location == APPLY_SAVES)
					&& pAf->modifier > 0)
					pAf->modifier = 0 - pAf->modifier;

				pAf->next = obj->affected;
				obj->affected = pAf;
			}
		}
		//Increment the level.
		obj->level += random_table[item].level[2];
	}
	//Now, if there's a prefix, repeat the same thing with the prefix table entry.
	if (prefix != 0)
	{
		for (i = 0; i < 3; i++)
		{
			if (prefix_table[prefix].app_loc[i] != -1)
			{
				if (prefix_table[prefix].app_loc[i] == TO_WEAPON)
				{
					if (obj->item_type == ITEM_WEAPON)
						obj->value[4] |= prefix_table[prefix].app_flag[i];

					if (prefix_table[prefix].app_mod[i] != 0)
					{
						pAf = new_affect ();
						pAf->where = TO_AFFECTS;
						pAf->type = -1;
						pAf->duration = -1;
						pAf->bitvector = 0;
						pAf->level = cLevel;
						pAf->location = prefix_table[prefix].app_type[i];
						pAf->modifier = prefix_table[prefix].app_mod[i];

						if ((pAf->location == APPLY_AC || pAf->location == APPLY_SAVES)
							&& pAf->modifier > 0)
							pAf->modifier = 0 - pAf->modifier;

						pAf->next = obj->affected;
						obj->affected = pAf;
					}
				}
				else
				{
					pAf = new_affect ();
					pAf->where = prefix_table[prefix].app_loc[i];
					pAf->type = -1;
					pAf->duration = -1;
					pAf->bitvector = prefix_table[prefix].app_flag[i];
					pAf->level = cLevel;
					pAf->location = prefix_table[prefix].app_type[i];
					pAf->modifier = prefix_table[prefix].app_mod[i];

					if ((pAf->location == APPLY_AC || pAf->location == APPLY_SAVES)
						&& pAf->modifier > 0)
						pAf->modifier = 0 - pAf->modifier;

					pAf->next = obj->affected;
					obj->affected = pAf;
				}
			}
		}

		//Add up other bonuses.
		obj->level += prefix_table[prefix].level_mod[1];
		obj->extra_flags |= prefix_table[prefix].extra_flags;
		obj->weight += prefix_table[prefix].weight_mod;
		obj->cost += prefix_table[prefix].value_mod;

		//Check for forced spells on magical devices.
		switch (random_table[item].type)
		{
			default: break;
			case RANDOM_POTION:
			case RANDOM_PILL:
			case RANDOM_SCROLL:
				//Prefixes will affect the FIRST two slots.
				for (i = 0; i < 2; i++)
				{
					if (prefix_table[prefix].spells[i] != NULL)
						obj->value[i+1] = skill_lookup (prefix_table[prefix].spells[i]);
				}
				break;
			case RANDOM_STAFF:
			case RANDOM_WAND:
				if (prefix_table[prefix].spells[0] != NULL)
					obj->value[3] = skill_lookup (prefix_table[prefix].spells[0]);
				break;
		}
	}
	//Now, if there's a suffix, do it all yet again.
	if (suffix != 0)
	{
		for (i = 0; i < 3; i++)
		{
			if (suffix_table[suffix].app_loc[i] != -1)
			{
				if (suffix_table[suffix].app_loc[i] == TO_WEAPON)
				{
					if (obj->item_type == ITEM_WEAPON)
						obj->value[4] |= suffix_table[suffix].app_flag[i];

					if (suffix_table[suffix].app_mod[i] != 0)
					{
						pAf = new_affect ();
						pAf->where = TO_AFFECTS;
						pAf->type = -1;
						pAf->duration = -1;
						pAf->bitvector = 0;
						pAf->level = cLevel;
						pAf->location = suffix_table[suffix].app_type[i];
						pAf->modifier = suffix_table[suffix].app_mod[i];

						if ((pAf->location == APPLY_AC || pAf->location == APPLY_SAVES)
							&& pAf->modifier > 0)
							pAf->modifier = 0 - pAf->modifier;

						pAf->next = obj->affected;
						obj->affected = pAf;
					}
				}
				else
				{
					pAf = new_affect ();
					pAf->where = suffix_table[suffix].app_loc[i];
					pAf->type = -1;
					pAf->duration = -1;
					pAf->bitvector = suffix_table[suffix].app_flag[i];
					pAf->level = cLevel;
					pAf->location = suffix_table[suffix].app_type[i];
					pAf->modifier = suffix_table[suffix].app_mod[i];

					if ((pAf->location == APPLY_AC || pAf->location == APPLY_SAVES)
						&& pAf->modifier > 0)
						pAf->modifier = 0 - pAf->modifier;

					pAf->next = obj->affected;
					obj->affected = pAf;
				}
			}
		}
		obj->level += suffix_table[suffix].level_mod[1];
		obj->extra_flags |= suffix_table[suffix].extra_flags;
		obj->weight += suffix_table[suffix].weight_mod;
		obj->cost += suffix_table[suffix].value_mod;
	

		switch (random_table[item].type)
		{
			default: break;
			case RANDOM_POTION:
			case RANDOM_PILL:
			case RANDOM_SCROLL:
				//Suffixes will affect the SECOND two slots.
				for (i = 2; i < 4; i++)
				{
					if (suffix_table[suffix].spells[i-2] != NULL)
						obj->value[i+1] = skill_lookup (suffix_table[suffix].spells[i-2]);
				}
				break;
			case RANDOM_STAFF:
			case RANDOM_WAND:
				/*
				 * Wands and staves are an imperfect system for this, as they
				 * only contain one spell.  In the event of a suffix and prefix
				 * that both have magic, only the first spell of the suffix
				 * can be used.
				 */
				if (suffix_table[suffix].spells[0] != NULL)
					obj->value[3] = skill_lookup (suffix_table[suffix].spells[0]);
				break;
		}
	}

	//Now make sure the level and cost are within the limits.
	if (obj->cost < 1)
		obj->cost = 1;

	obj->level = URANGE (1, obj->level, 50);


	//The item is finished!  Now echo the drop to the recipient only.
	act ("$N dropped $p!", ch, obj, mob, TO_CHAR);

	//Make sure they can pick it up.  Ripped right out of get_obj.
    if (ch->carry_number + get_obj_number (obj) > can_carry_n (ch))
    {
        act ("$d: you can't carry that many items.",
             ch, NULL, obj->name, TO_CHAR);
		obj_to_room (obj, ch->in_room);
		return;
    }

    if ((!obj->in_obj || obj->in_obj->carried_by != ch)
        && (get_carry_weight (ch) + get_obj_weight (obj) > can_carry_w (ch)))
    {
        act ("$d: you can't carry that much weight.",
             ch, NULL, obj->name, TO_CHAR);
		obj_to_room (obj, ch->in_room);
		return;
    }

	//Fin.
	obj_to_char (obj, ch);
	return;
}
