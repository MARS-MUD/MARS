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



/*
 * Global vars
 */
extern	char *	target_name;

/* Support functions in magic.c */
bool	check_dispel	args( ( int dis_level, CHAR_DATA *victim, int sn ) );
bool	check_spellcraft args(( CHAR_DATA *ch, int sn ) );
bool	saves_dispel	args( ( int dis_level, int spell_level, int duration) );
int	spellcraft_dam	args( ( int num, int die ) );

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_acid_rain		);
DECLARE_SPELL_FUN(	spell_adrenaline	);
DECLARE_SPELL_FUN(	spell_aid		);
DECLARE_SPELL_FUN(	spell_animal_summoning	);
DECLARE_SPELL_FUN(      spell_animate           );
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_atrophy		);
DECLARE_SPELL_FUN(	spell_aura_peace	);
DECLARE_SPELL_FUN(	spell_barkskin		);
DECLARE_SPELL_FUN(	spell_barrage		);
DECLARE_SPELL_FUN(	spell_bear_claws	);
DECLARE_SPELL_FUN(	spell_bear_fat		);
DECLARE_SPELL_FUN(	spell_blade_barrier	);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(      spell_calm		);
DECLARE_SPELL_FUN(      spell_cancellation	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(      spell_chain_lightning   );
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(	spell_cone_of_cold	);
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_rose	);
DECLARE_SPELL_FUN(	spell_create_spring	);
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(      spell_cure_disease	);
DECLARE_SPELL_FUN(	spell_cure_drunk	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_curse		);
DECLARE_SPELL_FUN(	spell_curse_of_nature	);
DECLARE_SPELL_FUN(	spell_dancing_boots	);
DECLARE_SPELL_FUN(	spell_dancing_lights	);
DECLARE_SPELL_FUN(	spell_darkness_shield	);
DECLARE_SPELL_FUN(	spell_dark_ritual	);
DECLARE_SPELL_FUN(      spell_deafen            );
DECLARE_SPELL_FUN(      spell_demonfire		);
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_good	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(      spell_dispel_good       );
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_armor	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_enlarge		);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_farsight		);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_fireproof		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(  	spell_flameshield	);
DECLARE_SPELL_FUN(	spell_floating_disc	);
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(  	spell_forceshield	);
DECLARE_SPELL_FUN(      spell_frenzy		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_ghost_shield	);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(	spell_goodberry		);
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(      spell_haste		);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_healing_hands	);
DECLARE_SPELL_FUN(	spell_heat_metal	);
DECLARE_SPELL_FUN(      spell_holy_word		);
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(	spell_mana		);
DECLARE_SPELL_FUN(      spell_mass_healing	);
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(	spell_nexus		);
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(      spell_plague		);
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_portal		);
DECLARE_SPELL_FUN(	spell_preserve	);
DECLARE_SPELL_FUN(	spell_protection_evil	);
DECLARE_SPELL_FUN(	spell_protection_good	);
DECLARE_SPELL_FUN(	spell_ray_of_truth	);
DECLARE_SPELL_FUN(	spell_recharge		);
DECLARE_SPELL_FUN(	spell_refresh		);
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_resurrect		);
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(	spell_scry		);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_shrink		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(	spell_slow		);
DECLARE_SPELL_FUN(  	spell_staticshield	);
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(      spell_summon_fire_elemental );
DECLARE_SPELL_FUN(      spell_summon_air_elemental );
DECLARE_SPELL_FUN(      spell_summon_water_elemental );
DECLARE_SPELL_FUN(      spell_summon_earth_elemental );
DECLARE_SPELL_FUN(      spell_summon_greater_wolf );
DECLARE_SPELL_FUN(      spell_find_familiar );
DECLARE_SPELL_FUN(	spell_summon_ggolem	);
DECLARE_SPELL_FUN(	spell_summon_hawk	);
DECLARE_SPELL_FUN(      spell_summon_tiger      );
DECLARE_SPELL_FUN(      spell_summon_wolf       );
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(      spell_wail_of_the_banshee );
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_weariness		);
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);
DECLARE_SPELL_FUN(	spell_general_purpose	);
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(	spell_channel		);
DECLARE_SPELL_FUN(	spell_shadowstep        );
DECLARE_SPELL_FUN(	spell_hellfire		);
DECLARE_SPELL_FUN(	spell_acid_arrow	);
DECLARE_SPELL_FUN(	spell_banshee_blast	);
DECLARE_SPELL_FUN(	spell_disruption	);
DECLARE_SPELL_FUN(	spell_caustic_eruption	);
DECLARE_SPELL_FUN(	spell_acid_geyser	);
DECLARE_SPELL_FUN(	spell_corrosive_burst	);
DECLARE_SPELL_FUN(	spell_daze		);
DECLARE_SPELL_FUN(	spell_damnation		);
DECLARE_SPELL_FUN(	spell_rushing_fist	);
DECLARE_SPELL_FUN(	spell_telekinetic_force	);
DECLARE_SPELL_FUN(	spell_ion_blast		);
DECLARE_SPELL_FUN(	spell_celestial_lightning );
DECLARE_SPELL_FUN(	spell_electrocute	);
DECLARE_SPELL_FUN(	spell_hurricane		);
DECLARE_SPELL_FUN(      spell_shadow_bolt       );
DECLARE_SPELL_FUN(      spell_astral_walk       );
DECLARE_SPELL_FUN(      spell_black_mantle      );
DECLARE_SPELL_FUN(      spell_full_heal      );
DECLARE_SPELL_FUN(      spell_truesight      );
DECLARE_SPELL_FUN(      spell_life_detect      );
DECLARE_SPELL_FUN(      spell_aurasight      );
DECLARE_SPELL_FUN(      spell_project_force      );
DECLARE_SPELL_FUN(      spell_strike_true      );
DECLARE_SPELL_FUN(      spell_natural_armor      );
DECLARE_SPELL_FUN(      spell_dragonscales      );
DECLARE_SPELL_FUN(      spell_spellmantle      );
DECLARE_SPELL_FUN(      spell_bears_endurance      );
DECLARE_SPELL_FUN(      spell_bulls_strength      );
DECLARE_SPELL_FUN(      spell_cats_grace      );
DECLARE_SPELL_FUN(      spell_fox_cunning      );
DECLARE_SPELL_FUN(      spell_owls_wisdom      );
DECLARE_SPELL_FUN(      spell_bane             );
