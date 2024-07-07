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

/*  globals.c  $Revision: 1.4 $  $Date: 2010/07/17 00:26:48 $  */
/* dwatch egate artifice fdmud thud code copyright 2002-2008 Bob Kircher */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"

/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    merc_down;		/* Shutdown			*/
bool		    wizlock;		/* Game is wizlocked		*/
bool		    newlock;		/* Game is newlocked		*/
time_t		    current_time;	/* time of this pulse */	


AREA_DATA *		area_first;
AREA_DATA *		area_last;

CHAR_DATA *		auc_bid;
int			auc_cost;
int			auc_count = -1;
CHAR_DATA *		auc_held;
OBJ_DATA *		auc_obj;

char *			badname_list;
BOARD_DATA *		board_first;
BOARD_DATA *		board_last;
char			boot_file	[SHORT_STRING_LENGTH];
time_t			boot_time;
char			bug_buf		[2*MAX_INPUT_LENGTH];
CHAR_DATA *		char_list;

bool			changed_boards;
bool			changed_clan;
bool			changed_command;
bool			changed_dream;
bool			changed_group;
bool			changed_project;
bool			changed_race;
bool			changed_secgroup;
bool			changed_skill;
bool			changed_social;
bool			changed_system;

CLAN_DATA		*clan_first;

int			class_cleric;
int			class_fighter;
int			class_druid;
int			class_mage;
int			class_rogue;

int			control;		/* control socket */

DREAM_DATA *		dream_last;
DREAM_DATA *		dream_list;

int			forcetick;

int			greet_size;		/* Size of one row in greet table */
bool *			greet_table;

HELP_DATA *		help_first;
char			help_greeting[MAX_STRING_LENGTH];
HELP_DATA *		help_last;

int                     hiring[2];              /* Hiring builders/coders? */

int			hour_sunrise;
int			hour_sunset;

struct	last_cmd_buf	lcb[LCB_SIZE];
int			lci = LCB_SIZE - 1;

char			log_buf		[2*MAX_INPUT_LENGTH];

int			map_mem;

bool			MOBtrigger;

int			object_count;
OBJ_DATA *		object_list;

int			port;			/* game port */
PROJECT_DATA *		project_last;
PROJECT_DATA *		project_list;

int			race_bat;
int			race_bear;
int			race_cat;
int			race_centaur;
int			race_centipede;
int			race_chimera;
int			race_demon;
int			race_dog;
int			race_doll;
int			race_dragon;
int			race_dragonborn;
int			race_drow;
int			race_dwarf;
int			race_elf;
int			race_felis;
int			race_fido;
int			race_fish;
int			race_fox;
int			race_giant;
int			race_gnome;
int			race_goblin;
int			race_halfelf;
int			race_halfling;
int			race_halforc;
int			race_highelf;
int			race_hobgoblin;
int			race_horse;
int			race_human;
int			race_insect;
int			race_kobold;
int			race_lizard;
int			race_minotaur;
int			race_ogre;
int			race_orc;
int			race_pig;
int			race_pixie;
int			race_rabbit;
int			race_satyr;
int			race_school_monster;
int			race_shadow;
int			race_snake;
int			race_songbird;
int			race_sprite;
int			race_tiefling;
int			race_titan;
int			race_troll;
int			race_undead;
int			race_unique;
int			race_vampire;
int			race_waterfowl;
int			race_wolf;
int			race_wyvern;
int			race_zombie;


struct	rebuild_data	rebuild;
int			recursion_counter;

int			most_on;
int			most_yesterday;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

SOCIAL_DATA *		social_first;
SOCIAL_DATA *		social_last;

char *			sql_db;
bool			sql_flag;
char *			sql_host;
char *			sql_passwd;
int			sql_port;
char *			sql_socket;
char *			sql_user;

time_t			startup_time;
char			str_boot_time[MAX_INPUT_LENGTH];
int			sys_pkill;
bool			sys_webwho;

TIME_INFO_DATA		time_info;
int			today;

int			top_alias;
int			top_area;
int			top_avatar;
int			top_ban;
int			top_bank;
int			top_board;
int			top_board_info;
int			top_buffer;
int			top_char;
int			top_clan;
int			top_clist;
int			top_desc;
int			top_dream;
int			top_dream_seg;
int			top_dreamlist;
int			top_event;
int			top_event_used;
int			top_exit;
int			top_help;
int			top_ignore;
int			top_milist;
int			top_mob_index;
int			top_mpact;
int			top_mprog;
int			top_note;
int			top_obj_index;
int			top_oilist;
int			top_oreprog;
int			top_overland;
int			top_pcdata;
int			top_project;
int			top_race;
int			top_reset;
int			top_room;
int			top_shop;
int			top_skill;
int			top_social;
int			top_textlist;
int			top_travel;
int			top_userlist;
int			top_vum_dream;
int			top_vnum_mob;
int			top_vnum_obj;
int			top_vnum_room;
int			top_vroom;

USERLIST_DATA *		user_first;
USERLIST_DATA *		user_last;

bool			vehicle_moved;

WEATHER_DATA		weather_info;
char			whoborder_imm[MAX_INPUT_LENGTH];
char			whoborder_mort[MAX_INPUT_LENGTH];
int			whosock;		/* web who socket */
