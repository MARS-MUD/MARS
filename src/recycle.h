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


/* externs */
extern char str_empty[1];
extern int mobile_count;


/* stuff for providing a crash-proof buffer */

#define MAX_BUF		16384
#define MAX_BUF_LIST 	10
#define BASE_BUF 	1024

/* valid states */
#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2

/* affect recycling */
#define AD AFFECT_DATA
AD	*new_affect args( (void) );
void	free_affect args( (AFFECT_DATA *af) );
#undef AD

/* alias recycling */
#define AD ALIAS_DATA
AD	*new_alias args( ( void ) );
void	free_alias args( ( ALIAS_DATA *alias ) );
#undef AD

/* area creation */
#define AD AREA_DATA
AD	*new_area	args( ( void ) );
#undef AD

/* avatar data */
AVATAR_DATA *	new_avatar_data		args( ( void ) );
void		free_avatar_data	args( ( AVATAR_DATA *ad ) );

/* ban data recycling */
#define BD BAN_DATA
BD	*new_ban	args( (void) );
void	free_ban	args( (BAN_DATA *ban) );
#undef BD

/* bank data reycling */
#define BD BANK_DATA
BD *	new_bank	args( ( void ) );
void	free_bank	args( ( BANK_DATA *bank ) );
#undef BD

/* boards */
#define BD BOARD_DATA
BD *	new_board	args( ( void ) );
void	free_board	args( ( BOARD_DATA *board ) );
#undef BD

/* board info */
#define BD BOARD_INFO
BD *	new_board_info	args( ( void ) );
void	free_board_info	args( ( BOARD_INFO *info ) );
#undef BD

/* character recyling */
#define CD CHAR_DATA
CD	*new_char	args( (void) );
void	free_char	args( (CHAR_DATA *ch) );
#undef CD

/* clan stuff */
#define CD CLAN_DATA
CD	*new_clan_data	args( ( void ) );
void	free_clan_data	args( ( CLAN_DATA *pClan ) );
#undef CD

/* char lists */
#define CL CLIST_DATA
CL	*new_clist_data	args( ( void ) );
void	free_clist_data	args( ( CLIST_DATA *clist ) );
#undef CL

/* descriptor recycling */
#define DD DESCRIPTOR_DATA
DD	*new_descriptor args( (void) );
void	free_descriptor args( (DESCRIPTOR_DATA *d) );
#undef DD

/* dream recycling */
DREAM_DATA *	new_dream	args( ( void ) );
DREAMLIST_DATA *new_dreamlist	args( ( void ) );
DREAMSEG_DATA *	new_dream_seg	args( ( void ) );
void		free_dream	args( ( DREAM_DATA *dream ) );
void		free_dream_seg	args( ( DREAMSEG_DATA *seg ) );
void		free_dreamlist	args( ( DREAMLIST_DATA *item ) );

/* events */
EVENT_DATA *	new_event	args( ( void ) );
void		free_event	args( ( EVENT_DATA *event ) );

/* exit recycling */
EXIT_DATA *new_exit args( ( void ) );
void	   free_exit args( ( EXIT_DATA *pExit ) );

/* extra descr recycling */
#define ED EXTRA_DESCR_DATA
ED	*new_extra_descr args( (void) );
void	free_extra_descr args( (EXTRA_DESCR_DATA *ed) );
#undef ED

/* char gen data recycling */
#define GD GEN_DATA
GD 	*new_gen_data args( (void) );
void	free_gen_data args( (GEN_DATA * gen) );
#undef GD

/* help recycling */
#define HD HELP_DATA
HD	*new_help	args( ( void ) );
void	free_help	args( ( HELP_DATA *pHelp ) );
#undef HD

IGNORE_DATA *	new_ignore_data		args ( ( void ) );
void		free_ignore_data	args ( ( IGNORE_DATA *pId ) );

MPROG_ACT_LIST *new_mpact_data		args( ( void ) );
void		free_mpact_data		args( ( MPROG_ACT_LIST * ) );

MPROG_DATA      *new_mprog_data         args ( ( void ) );
void            free_mprog_data         args ( ( MPROG_DATA *pMProg ) );

/* note recycling */
#define ND NOTE_DATA
ND	*new_note args( (void) );
void	free_note args( (NOTE_DATA *note) );
#undef ND

/* object recycling */
#define OD OBJ_DATA
OD	*new_obj args( (void) );
void	free_obj args( (OBJ_DATA *obj) );
#undef OD

/* object index recycling */
#define OID OBJ_INDEX_DATA
OID	*new_obj_index	args( ( void ) );
void	free_obj_index	args( ( OBJ_INDEX_DATA *pObjIndex ) );
#undef OID

/* o/r/eprog data recycling */
#define ORE OREPROG_DATA
ORE	*new_oreprog_data	args( ( void ) );
void	free_oreprog_data	args( ( OREPROG_DATA *pProg ) );
#undef ORE

/* overland stuff */
#define OV OVERLAND_DATA
OV	*new_overland		args( ( int x, int y ) );
void	free_overland		args( ( OVERLAND_DATA *ov ) );
#undef OV

/* pc data recycling */
#define PD PC_DATA
PD	*new_pcdata args( (void) );
void	free_pcdata args( (PC_DATA *pcdata) );
#undef PD

/* project recycling  */
#define PJ PROJECT_DATA
PJ	*new_project		args( ( void ) );
void	free_project		args( ( PROJECT_DATA *pProj ) );
#undef PJ

/* mob id and memory procedures */
#define MD MEM_DATA
long 	get_pc_id args( (void) );
long	get_mob_id args( (void) );
MD	*new_mem_data args( (void) );
void	free_mem_data args( ( MEM_DATA *memory) );
MD	*find_memory args( (MEM_DATA *memory, long id) );
#undef MD

/* mob_index */
MOB_INDEX_DATA *new_mob_index	args( ( void ) );
void		free_mob_index	args( ( MOB_INDEX_DATA *pMobIndex ) );

/* mob index list */
MILIST_DATA *	new_milist_data	args( ( void ) );
void		free_milist_data args(( MILIST_DATA *mob ) );

/* obj index list */
OILIST_DATA *	new_oilist_data	args( ( void ) );
void		free_oilist_data args(( OILIST_DATA *mob ) );

/* Reset recycling */
RESET_DATA *	new_reset_data	args( ( void ) );
void		free_reset_data	args( ( RESET_DATA *pReset ) );

/* Room index recycling */
ROOM_INDEX_DATA *new_room_index	args( ( void ) );
void		free_room_index	args( ( ROOM_INDEX_DATA *pRoomIndex ) );

/* Shop recycling */
SHOP_DATA *	new_shop	args( ( void ) );
void		free_shop	args( ( SHOP_DATA *pShop ) );

/* Social recycling */
SOCIAL_DATA *	new_social	args( ( void ) );
void		free_social	args( ( SOCIAL_DATA *pSocial ) );

/* String list recycling */
STRLIST_DATA *	new_strlist	args( ( void ) );
void		free_strlist	args( ( STRLIST_DATA *list ) );

/* Text list recycling */
TEXT_DATA *	new_textlist	args( ( void ) );
void		free_textlist	args( ( TEXT_DATA *list ) );

/* Travel struct recycling */
TRAVEL_DATA *	new_travel	args( ( void ) );
void		free_travel	args( ( TRAVEL_DATA *pTravel ) );

/* Userlist recycling */
USERLIST_DATA *	new_user_rec	args( ( void ) );
void		free_user_rec	args( ( USERLIST_DATA *pUser ) );

/* buffer procedures */

bool	add_buf args( (BUFFER *buffer, const char *string) );
bool	buf_printf args( ( BUFFER *pbuf, const char *fmt, ... ) __attribute__ ((format (printf, 2, 3 ))));
char	*buf_string args( (BUFFER *buffer) );
void	clear_buf args( (BUFFER *buffer) );
void	free_buf args( (BUFFER *buffer) );
BUFFER	*new_buf args( (void) );
BUFFER  *new_buf_size args( (int size) );
