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



/* vals from db.c */
extern int		area_version;
extern bool fBootDb;
extern int		newmobs;
extern int		newobjs;
extern MOB_INDEX_DATA 	* mob_index_hash          [MAX_KEY_HASH];
extern OBJ_INDEX_DATA 	* obj_index_hash          [MAX_KEY_HASH];
extern ROOM_INDEX_DATA	* room_index_hash	  [MAX_KEY_HASH];
extern char		strArea[MAX_INPUT_LENGTH];
extern int  		top_affect;
extern int		top_ed; 
extern int		top_event;
extern int		top_event_used;
extern int		top_exit;
extern int		top_mob_index;
extern int		top_obj_index;
extern int		top_social;

void	area_renumber	args( ( void ) );
void	area_sort	args( ( AREA_DATA *pArea ) );
void	reset_area	args( ( AREA_DATA * pArea ) );

/* from db2.c */
extern int	social_count;

/* conversion from db.h */
void	convert_mob(MOB_INDEX_DATA *mob);
void	convert_obj(OBJ_INDEX_DATA *obj);

/* macro for flag swapping */
#define GET_UNSET(flag1,flag2)	(~(flag1)&((flag1)|(flag2)))

/* Magic number for memory allocation */
#define MAGIC_NUM 52571214

/* loaders in db2.c called from db.c */
void	load_areadata		args( ( FILE *fp ) );
void	load_badnames		args( ( void ) );
void	load_boards		args( ( void ) );
void	load_commands		args( ( void ) );
void	load_dreams		args( ( void ) );
void	load_greet_data		args( ( void ) );
void	load_groups		args( ( void ) );
void 	load_mobiles		args( ( FILE *fp ) );
void	load_most_players	args( ( void ) );
void 	load_objects		args( ( FILE *fp ) );
bitvector load_oreprogs		args( ( FILE *fp, OREPROG_DATA **proglist,
					int ptype ) );
void	load_projects		args( ( void ) );
void	load_races		args( ( void ) );
void	load_resets		args( ( FILE *fp ) );
void	load_sectdesc		args( ( FILE *fp ) );
void	load_skills		args( ( void ) );
void 	load_socials		args( ( void ) );
void	load_sql_data		args( ( void ) );
void	load_sysconfig		args( ( void ) );
void	load_travels		args( ( FILE *fp ) );
void	load_travel_state	args( ( void ) );
void	load_userlist		args( ( void ) );
void	load_vehicles		args( ( void ) );
void	load_vrooms		args( ( FILE *fp ) );

/* random strings in rand_str.h */
void	set_mob_strings		args( ( CHAR_DATA *ch ) );

