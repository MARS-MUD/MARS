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

struct clan_type
{
    char 	*name;
    char 	*who_name;
    sh_int 	hall;
    bool	independent; /* true for loners */
};

struct prog_type
{
    char *name;
    bitvector value;
};

struct position_type
{
    char *name;
    char *short_name;
};

struct sex_type
{
    char *name;
};

struct flag_stat_type
{
    const struct flag_type *structure;
    bool stat;
};


/* game tables */
extern	const	struct	position_type	position_table[];

/* flag stat table */
extern	const	struct	flag_stat_type	flag_stat_table[];

/* flag tables */
extern	const	struct	flag_type	act_flags[];
extern	const	struct	flag_type	act2_flags[];
extern	const	struct	flag_type	affect_flags[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	area_flags[];
extern	const	struct	flag_type	automob_types[];
extern	const	struct	flag_type	book_flags[];
extern	const	struct	flag_type	class_flags[];
extern	const	struct	flag_type	cmd_flags[];
extern	const	struct	flag_type	color_types[];
extern	const	struct	flag_type	comm_flags[];
extern	const	struct	flag_type	container_flags[];
extern	const	struct	flag_type	continent_types[];
extern	const	struct	flag_type	death_types[];
extern	const	struct	flag_type	dream_flags[];
extern	const	struct	flag_type	dream_types[];
extern	const	struct	flag_type	eprog_types[];
extern	const	struct	flag_type	event_area_types[];
extern	const	struct	flag_type	event_char_types[];
extern	const	struct	flag_type	event_desc_types[];
extern	const	struct	flag_type	event_game_types[];
extern	const	struct	flag_type	event_obj_types[];
extern	const	struct	flag_type	event_room_types[];
extern	const	struct	flag_type	exit_flags[];
extern	const	struct	flag_type	extra_flags[];
extern	const	struct	flag_type	extra_flags2[];
extern	const	struct	flag_type	form_flags[];
extern	const	struct	flag_type	furniture_flags[];
extern	const	struct	flag_type	herb_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	invoke_types[];
extern	const	struct	flag_type	item_types[];
extern	const	struct	flag_type	kingdom_types[];
extern	const	struct	flag_type	level_types[];
extern	const	struct	flag_type	log_types[];
extern	const	struct	flag_type	material_types[];
extern	const	struct	flag_type	mprog_types[];
extern	const	struct	flag_type	off_flags[];
extern	const	struct	flag_type	old_level_types[];
extern	const	struct	flag_type	oprog_types[];
extern	const	struct	flag_type	part_flags[];
extern	const	struct	flag_type	plr_flags[];
extern	const	struct	flag_type	portal_flags[];
extern	const	struct	flag_type	position_types[];
extern	const	struct	flag_type	priority_flags[];
extern	const	struct	flag_type	rdesc_types[];
extern	const	struct	flag_type	res_flags[];
extern	const	struct	flag_type	room_aff_flags[];
extern	const	struct	flag_type	room_flags[];
extern	const	struct	flag_type	rprog_types[];
extern	const	struct	flag_type	sector_types[];
extern		struct	flag_type	security_flags[33];
extern	const	struct	flag_type	sex_types[];
extern	const	struct	flag_type	sheath_flags[];
extern	const	struct	flag_type	shield_types[];
extern	const	struct	flag_type	size_types[];
extern	const	struct	flag_type	spell_flags[];
extern	const	struct	flag_type	stat_types[];
extern	const	struct	flag_type	target_types[];
extern	const	struct	flag_type	vehicle_types[];
extern	const	struct	flag_type	vuln_flags[];
extern	const	struct	flag_type	weapon_flags[];
extern	const	struct	flag_type	weapon_types[];
extern	const	struct	flag_type	weapon_type_olc[];
extern	const	struct	flag_type	wear_flags[];
extern	const	struct	flag_type	wear_loc_strings[];
extern	const	struct	flag_type	wear_loc_types[];
extern	const	struct	flag_type	windspeed_types[];
extern	const	struct	flag_type	world_types[];

