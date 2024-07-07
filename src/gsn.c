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
#include <time.h>
#include "merc.h"

int		gsn_adrenaline;
int		gsn_assassinate;
int		gsn_atrophy;
int		gsn_aura_peace;
int		gsn_axe;
int		gsn_backstab;
int		gsn_bash;
int		gsn_bastardsword;
int		gsn_berserk;
int		gsn_black_mantle;
int		gsn_blade_barrier;
int		gsn_blindness;
int		gsn_bow;
int		gsn_broadsword;
int		gsn_charge;
int		gsn_charm_person;
int             gsn_circle;
int		gsn_club;
int		gsn_counter;
int		gsn_crossbow;
int		gsn_curse;
int		gsn_dagger;
int		gsn_dancing_lights;
int             gsn_deafen;
int		gsn_dirk;
int		gsn_dirt;
int		gsn_disarm;
int		gsn_dream;
int		gsn_dodge;
int		gsn_dual;
int		gsn_enhanced_damage;
int		gsn_enlarge;
int		gsn_envenom;
int		gsn_fast_healing;
int		gsn_fifth_attack;
int		gsn_flail;
int		gsn_fly;
int		gsn_fourth_attack;
int		gsn_glance;
int		gsn_greataxe;
int		gsn_greatmace;
int		gsn_haggle;
int		gsn_hammer;
int		gsn_hand_to_hand;
int		gsn_herbs;
int		gsn_hide;
int		gsn_horseflail;
int		gsn_horsemace;
int		gsn_invis;
int		gsn_kick;
int		gsn_lance;
int		gsn_longsword;
int		gsn_lore;
int		gsn_mace;
int		gsn_mass_invis;
int		gsn_meditation;
int		gsn_parry;
int		gsn_peek;
int		gsn_phase;
int		gsn_pick_lock;
int		gsn_plague;
int		gsn_poison;
int		gsn_polearm;
int		gsn_rapier;
int		gsn_recall;
int		gsn_rescue;
int		gsn_riding;
int		gsn_sabre;
int		gsn_sanctuary;
int		gsn_scimitar;
int		gsn_scrolls;
int		gsn_second_attack;
int		gsn_shield_block;
int		gsn_shortsword;
int		gsn_shrink;
int		gsn_sleep;
int		gsn_sneak;
int		gsn_spear;
int		gsn_spellcraft;
int		gsn_staff;
int		gsn_staves;
int		gsn_steal;
int		gsn_stiletto;
int		gsn_sword;
int             gsn_swimming;
int		gsn_third_attack;
int             gsn_track;
int		gsn_trip;
int		gsn_twohandsword;
int             gsn_wail;
int		gsn_wands;
int             gsn_warmount;
int		gsn_weariness;
int		gsn_whip;
int      gsn_sharpen;
int      gsn_butcher;
int      gsn_skin;
int      gsn_whirlwind;
int      gsn_blackjack;
int     gsn_acid_breath;
int     gsn_fire_breath;
int     gsn_frost_breath;
int     gsn_gas_breath;
int     gsn_lightning_breath;
int     gsn_breath;
int     gsn_engage;
int     gsn_purify;
