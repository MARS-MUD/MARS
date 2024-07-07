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
 * This is a header file for all the OLC files.  Feel free to copy it into
 * merc.h if you wish.  Many of these routines may be handy elsewhere in
 * the code.  -Jason Dinkel
 */



/*
 * The version info.  Please use this info when reporting bugs.
 * It is displayed in the game by typing 'version' while editing.
 * Do not remove these from the code - by request of Jason Dinkel
 */
#define VHEADER "This OLC is based on "
#define VERSION	"ILAB Online Creation [Beta 1.1]"
#define AUTHOR	"By Jason(jdinkel@mines.colorado.edu)"
#define DATE	"(May. 15, 1995)"
#define CREDITS "Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"


/*
 * Result status from read_desc_file()
 */
#define SUCCESS		0
#define DESC_BAD	1
#define DESC_MISSING	2

/*
 * Area_wide set/clear/toggle flags
 */
#define FLAG_SET	0
#define FLAG_CLEAR	1
#define FLAG_TOGGLE	2

/*
 * New typedefs.
 */
typedef	bool OLC_FUN		args( ( CHAR_DATA *ch, char *argument ) );
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun

/*
 * Interpreter Prototypes
 */
void	aedit		args( ( CHAR_DATA *ch, char *argument ) );
void	bedit		args( ( CHAR_DATA *ch, char *argument ) );
void	cedit		args( ( CHAR_DATA *ch, char *argument ) );
void	cmdedit		args( ( CHAR_DATA *ch, char *argument ) );
void	dreamedit	args( ( CHAR_DATA *ch, char *argument ) );
void	epedit		args( ( CHAR_DATA *ch, char *argument ) );
void	gedit		args( ( CHAR_DATA *ch, char *argument ) );
void	hedit		args( ( CHAR_DATA *ch, char *argument ) );
void	medit		args( ( CHAR_DATA *ch, char *argument ) );
void	mpedit		args( ( CHAR_DATA *ch, char *argument ) );
void	oedit		args( ( CHAR_DATA *ch, char *argument ) );
void	opedit		args( ( CHAR_DATA *ch, char *argument ) );
void	pedit		args( ( CHAR_DATA *ch, char *argument ) );
void	racedit		args( ( CHAR_DATA *ch, char *argument ) );
void    redit		args( ( CHAR_DATA *ch, char *argument ) );
void	rename_obj	args( ( CHAR_DATA *ch, char *argument ) );
void	rpedit		args( ( CHAR_DATA *ch, char *argument ) );
void	secedit		args( ( CHAR_DATA *ch, char *argument ) );
void	sedit		args( ( CHAR_DATA *ch, char *argument ) );
void	skedit		args( ( CHAR_DATA *ch, char *argument ) );
void    travedit	args( ( CHAR_DATA *ch, char *argument ) );
void    tpedit		args( ( CHAR_DATA *ch, char *argument ) );
void	forge_obj	args( ( CHAR_DATA *ch, OBJ_DATA *to_forge ) );
void	forge_object	args( ( CHAR_DATA *ch, char *argument ) );

/*
 * OLC Constants
 */
#define MAX_MOB	1		/* Default maximum number for resetting mobs */



/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type
{
    char * const	name;
    OLC_FUN *		olc_fun;
};



/*
 * Structure for an OLC editor startup command.
 */
struct	editor_cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
};



/*
 * Utils.
 */
bool		can_aedit_all	args( ( CHAR_DATA *ch, AREA_DATA *pArea ) );
AREA_DATA *get_area_data	args( ( int vnum ) );
AREA_DATA *get_area_edit	args( ( CHAR_DATA *ch ) );
AREA_DATA *get_vnum_area	args ( ( int vnum ) );
bool exec_command		args( ( CHAR_DATA *ch, char *argument ) );
void instaroom			args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom ) );
int read_desc_file		args ( ( CHAR_DATA *ch, char *buf,
					 char *filename, char *fext,
					 bool fOpen ) );
bool set_mob_dice		args( ( MOB_INDEX_DATA *pMob, int difficulty ) );
bool set_obj_stats		args( ( OBJ_INDEX_DATA *pObj ) );
void show_olc_cmds		args( ( CHAR_DATA *ch,
					const struct olc_cmd_type *olc_table ) );
void olcautosave args (( void ));

/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type	aedit_table[];
extern const struct olc_cmd_type	bedit_table[];
extern const struct olc_cmd_type	cedit_table[];
extern const struct olc_cmd_type	cmdedit_table[];
extern const struct olc_cmd_type	dreamedit_table[];
extern const struct olc_cmd_type	epedit_table[];
extern const struct olc_cmd_type	gedit_table[];
extern const struct olc_cmd_type	hedit_table[];
extern const struct olc_cmd_type	medit_table[];
extern const struct olc_cmd_type	mpedit_table[];
extern const struct olc_cmd_type	oedit_table[];
extern const struct olc_cmd_type	opedit_table[];
extern const struct olc_cmd_type	pedit_table[];
extern const struct olc_cmd_type	racedit_table[];
extern const struct olc_cmd_type	redit_table[];
extern const struct olc_cmd_type	rename_obj_table[];
extern const struct olc_cmd_type	rpedit_table[];
extern const struct olc_cmd_type	secedit_table[];
extern const struct olc_cmd_type	sedit_table[];
extern const struct olc_cmd_type	skedit_table[];
extern const struct olc_cmd_type	travedit_table[];
extern const struct olc_cmd_type	forge_obj_table[];

extern const struct olc_cmd_type	aedit_ship_table[];
extern const struct olc_cmd_type	iedit_table[];

/*
 * General Functions
 */
bool show_commands		args ( ( CHAR_DATA *ch, char *argument ) );
bool show_help			args ( ( CHAR_DATA *ch, char *argument ) );
bool edit_done			args ( ( CHAR_DATA *ch ) );
bool show_version		args ( ( CHAR_DATA *ch, char *argument ) );


/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN( aedit_show		);
DECLARE_OLC_FUN( aedit_age		);
DECLARE_OLC_FUN( aedit_allset		);
DECLARE_OLC_FUN( aedit_builder		);
DECLARE_OLC_FUN( aedit_circle		);
DECLARE_OLC_FUN( aedit_color		);
DECLARE_OLC_FUN( aedit_create		);
DECLARE_OLC_FUN( aedit_created		);
DECLARE_OLC_FUN( aedit_continent	);
DECLARE_OLC_FUN( aedit_credits		);
DECLARE_OLC_FUN( aedit_exitsize		);
DECLARE_OLC_FUN( aedit_fast_repop	);
DECLARE_OLC_FUN( aedit_file		);
DECLARE_OLC_FUN( aedit_housing		);
DECLARE_OLC_FUN( aedit_island		);
DECLARE_OLC_FUN( aedit_kingdom		);
DECLARE_OLC_FUN( aedit_level		);
DECLARE_OLC_FUN( aedit_line		);
DECLARE_OLC_FUN( aedit_mark		);
DECLARE_OLC_FUN( aedit_name		);
DECLARE_OLC_FUN( aedit_norecall		);
DECLARE_OLC_FUN( aedit_noquest          );
DECLARE_OLC_FUN( aedit_overland		);
DECLARE_OLC_FUN( aedit_prototype        );
DECLARE_OLC_FUN( aedit_recall		);
DECLARE_OLC_FUN( aedit_reset		);
DECLARE_OLC_FUN( aedit_resetmsg		);
DECLARE_OLC_FUN( aedit_rooms		);
DECLARE_OLC_FUN( aedit_security		);
DECLARE_OLC_FUN( aedit_setmobs		);
DECLARE_OLC_FUN( aedit_setobjs		);
DECLARE_OLC_FUN( aedit_stamp		);
DECLARE_OLC_FUN( aedit_travel		);
DECLARE_OLC_FUN( aedit_vnum		);
DECLARE_OLC_FUN( aedit_lvnum		);
DECLARE_OLC_FUN( aedit_uvnum		);
DECLARE_OLC_FUN( aedit_world		);
DECLARE_OLC_FUN( aedit_past             );
DECLARE_OLC_FUN( aedit_present          );
DECLARE_OLC_FUN( aedit_future           );

/*
 * Boards Editor Prototypes
 */
DECLARE_OLC_FUN( bedit_create		);
DECLARE_OLC_FUN( bedit_desc		);
DECLARE_OLC_FUN( bedit_expire		);
DECLARE_OLC_FUN( bedit_read		);
DECLARE_OLC_FUN( bedit_rename		);
DECLARE_OLC_FUN( bedit_show		);
DECLARE_OLC_FUN( bedit_write		);

/*
 * Clan Editor Prototypes
 */  
DECLARE_OLC_FUN( cedit_box		);
DECLARE_OLC_FUN( cedit_clan		);
DECLARE_OLC_FUN( cedit_create           );
DECLARE_OLC_FUN( cedit_deadly		);
DECLARE_OLC_FUN( cedit_desc             );
DECLARE_OLC_FUN( cedit_ftitle		);
DECLARE_OLC_FUN( cedit_induct		);
DECLARE_OLC_FUN( cedit_leader           );
DECLARE_OLC_FUN( cedit_losses		);
DECLARE_OLC_FUN( cedit_members          );
DECLARE_OLC_FUN( cedit_morgue		);
DECLARE_OLC_FUN( cedit_mtitle		);
DECLARE_OLC_FUN( cedit_name             );
DECLARE_OLC_FUN( cedit_outcast		);
DECLARE_OLC_FUN( cedit_peaceful		);
DECLARE_OLC_FUN( cedit_recall           );
DECLARE_OLC_FUN( cedit_show             );
DECLARE_OLC_FUN( cedit_title		);
DECLARE_OLC_FUN( cedit_type		);
DECLARE_OLC_FUN( cedit_wins		);
DECLARE_OLC_FUN( cedit_withdraw		);
DECLARE_OLC_FUN( cedit_object           );


/*
 * Command Editor Prototypes
 */
DECLARE_OLC_FUN( cmdedit_add		);
DECLARE_OLC_FUN( cmdedit_remove		);
DECLARE_OLC_FUN( cmdedit_show		);


/*
 * Dream Editor prototypes
 */
DECLARE_OLC_FUN( dreamedit_add		);
DECLARE_OLC_FUN( dreamedit_addaffect	);
DECLARE_OLC_FUN( dreamedit_author	);
DECLARE_OLC_FUN( dreamedit_class	);
DECLARE_OLC_FUN( dreamedit_create	);
DECLARE_OLC_FUN( dreamedit_delaffect	);
DECLARE_OLC_FUN( dreamedit_delete	);
DECLARE_OLC_FUN( dreamedit_edit		);
DECLARE_OLC_FUN( dreamedit_insert	);
DECLARE_OLC_FUN( dreamedit_race		);
DECLARE_OLC_FUN( dreamedit_show		);
DECLARE_OLC_FUN( dreamedit_title	);


/*
 * Exit Prog Editor prototypes
 */
DECLARE_OLC_FUN( epedit_show		);


/*
 * Skill Group Editor prototypes
 */
DECLARE_OLC_FUN( gedit_add		);
DECLARE_OLC_FUN( gedit_drop		);
DECLARE_OLC_FUN( gedit_show		);


/*
 * Help Editor Prototypes
 */
DECLARE_OLC_FUN( hedit_area		);
DECLARE_OLC_FUN( hedit_delet		);
DECLARE_OLC_FUN( hedit_delete		);
DECLARE_OLC_FUN( hedit_desc		);
DECLARE_OLC_FUN( hedit_edlevel		);
DECLARE_OLC_FUN( hedit_import		);
DECLARE_OLC_FUN( hedit_level		);
DECLARE_OLC_FUN( hedit_name		);
DECLARE_OLC_FUN( hedit_show		);


/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN( medit_ac		);
DECLARE_OLC_FUN( medit_align		);
DECLARE_OLC_FUN( medit_automob		);
DECLARE_OLC_FUN( medit_clan		);
DECLARE_OLC_FUN( medit_create		);
DECLARE_OLC_FUN( medit_damage		);
DECLARE_OLC_FUN( medit_damtype		);
DECLARE_OLC_FUN( medit_delete		);
DECLARE_OLC_FUN( medit_desc		);
DECLARE_OLC_FUN(  medit_ethos);
DECLARE_OLC_FUN( medit_form		);
DECLARE_OLC_FUN( medit_group		);
DECLARE_OLC_FUN( medit_hitdice		);
DECLARE_OLC_FUN( medit_hitroll		);
DECLARE_OLC_FUN( medit_immune		);
DECLARE_OLC_FUN( medit_level		);
DECLARE_OLC_FUN( medit_long		);
DECLARE_OLC_FUN( medit_mana		);
DECLARE_OLC_FUN( medit_mpedit           );
DECLARE_OLC_FUN( medit_mplist           );
DECLARE_OLC_FUN( medit_mpremove         );
DECLARE_OLC_FUN( medit_mpswap		);
DECLARE_OLC_FUN( medit_name		);
DECLARE_OLC_FUN( medit_offensive	);
DECLARE_OLC_FUN( medit_parts		);
DECLARE_OLC_FUN( medit_position		);
DECLARE_OLC_FUN( medit_resistant	);
DECLARE_OLC_FUN( medit_shop		);
DECLARE_OLC_FUN( medit_short		);
DECLARE_OLC_FUN( medit_show		);
DECLARE_OLC_FUN( medit_size		);
DECLARE_OLC_FUN( medit_special		);
DECLARE_OLC_FUN( medit_train		);
DECLARE_OLC_FUN( medit_vulnerable	);
DECLARE_OLC_FUN( medit_wealth		);

DECLARE_OLC_FUN( medit_copy		);
DECLARE_OLC_FUN( medit_hitpoint         );
DECLARE_OLC_FUN( medit_gold             );
DECLARE_OLC_FUN( medit_game             );
DECLARE_OLC_FUN( medit_class            );

/*
 * MPEditor functions for MobProgs
 */
DECLARE_OLC_FUN( mpedit_show            );

DECLARE_OLC_FUN( mpedit_create          );
DECLARE_OLC_FUN( mpedit_arglist         );
DECLARE_OLC_FUN( mpedit_comlist         );


/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN( oedit_addaffect	);
DECLARE_OLC_FUN( oedit_anti		);
DECLARE_OLC_FUN( oedit_autoset		);
DECLARE_OLC_FUN( oedit_autoweapon   );
DECLARE_OLC_FUN( oedit_autoarmor   );
DECLARE_OLC_FUN( oedit_condition	);
DECLARE_OLC_FUN( oedit_cost		);
DECLARE_OLC_FUN( oedit_create		);
DECLARE_OLC_FUN( oedit_delaffect	);
DECLARE_OLC_FUN( oedit_delete		);
DECLARE_OLC_FUN( oedit_description	);
DECLARE_OLC_FUN( oedit_duration		);
DECLARE_OLC_FUN( oedit_ed		);
DECLARE_OLC_FUN( oedit_invoke		);
DECLARE_OLC_FUN( oedit_join		);
DECLARE_OLC_FUN( oedit_level            );
DECLARE_OLC_FUN( oedit_long		);
DECLARE_OLC_FUN( oedit_material		);
DECLARE_OLC_FUN( oedit_name		);
DECLARE_OLC_FUN( oedit_opedit           );
DECLARE_OLC_FUN( oedit_oplist           );
DECLARE_OLC_FUN( oedit_opremove         );
DECLARE_OLC_FUN( oedit_page		);
DECLARE_OLC_FUN( oedit_qcost		);
DECLARE_OLC_FUN( oedit_sepone		);
DECLARE_OLC_FUN( oedit_septwo		);
DECLARE_OLC_FUN( oedit_short		);
DECLARE_OLC_FUN( oedit_show		);
DECLARE_OLC_FUN( oedit_timer		);
DECLARE_OLC_FUN( oedit_value0		);
DECLARE_OLC_FUN( oedit_value1		);
DECLARE_OLC_FUN( oedit_value2		);
DECLARE_OLC_FUN( oedit_value3		);
DECLARE_OLC_FUN( oedit_value4		);
DECLARE_OLC_FUN( oedit_value5		);
DECLARE_OLC_FUN( oedit_weight		);

DECLARE_OLC_FUN( oedit_copy		);
DECLARE_OLC_FUN( set_ac_type            );
DECLARE_OLC_FUN( set_ac_vnum            );
DECLARE_OLC_FUN( set_ac_v1              );
DECLARE_OLC_FUN( set_ac_v2              );
DECLARE_OLC_FUN( set_ac_setspell        );

/*
 * OPEditor prototypes for obj progs
 */
DECLARE_OLC_FUN( opedit_arglist		);
DECLARE_OLC_FUN( opedit_comlist		);
DECLARE_OLC_FUN( opedit_create		);
DECLARE_OLC_FUN( opedit_show		);

/*
 * Project Editor Prototypes
 */
DECLARE_OLC_FUN( pedit_assign		);
DECLARE_OLC_FUN( pedit_complete		);
DECLARE_OLC_FUN( pedit_deadline		);
DECLARE_OLC_FUN( pedit_delete		);
DECLARE_OLC_FUN( pedit_description	);
DECLARE_OLC_FUN( pedit_name		);
DECLARE_OLC_FUN( pedit_priority		);
DECLARE_OLC_FUN( pedit_progress		);
DECLARE_OLC_FUN( pedit_show		);
DECLARE_OLC_FUN( pedit_status		);
DECLARE_OLC_FUN( pedit_undelete		);

/*
 * Race Editor Prototypes
 */
DECLARE_OLC_FUN( racedit_act		);
DECLARE_OLC_FUN( racedit_affect		);
DECLARE_OLC_FUN( racedit_align		);
DECLARE_OLC_FUN( racedit_allclass	);
DECLARE_OLC_FUN( racedit_form		);
DECLARE_OLC_FUN( racedit_immune		);
DECLARE_OLC_FUN( racedit_maxstat	);
DECLARE_OLC_FUN( racedit_offensive	);
DECLARE_OLC_FUN( racedit_parts		);
DECLARE_OLC_FUN( racedit_pcrace		);
DECLARE_OLC_FUN( racedit_points		);
DECLARE_OLC_FUN( racedit_resist		);
DECLARE_OLC_FUN( racedit_show		);
DECLARE_OLC_FUN( racedit_size		);
DECLARE_OLC_FUN( racedit_skill		);
DECLARE_OLC_FUN( racedit_startstat	);
DECLARE_OLC_FUN( racedit_vulnerable	);
DECLARE_OLC_FUN( racedit_weight		);


/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN( redit_affect		);
DECLARE_OLC_FUN( redit_copy		);
DECLARE_OLC_FUN( redit_create		);
DECLARE_OLC_FUN( redit_delete		);
DECLARE_OLC_FUN( redit_desc		);
DECLARE_OLC_FUN( redit_down		);
DECLARE_OLC_FUN( redit_east		);
DECLARE_OLC_FUN( redit_ed		);
DECLARE_OLC_FUN( redit_heal		);
DECLARE_OLC_FUN( redit_mana		);
DECLARE_OLC_FUN( redit_name		);
DECLARE_OLC_FUN( redit_north		);
DECLARE_OLC_FUN( redit_rpedit           );
DECLARE_OLC_FUN( redit_rpremove         );
DECLARE_OLC_FUN( redit_short		);
DECLARE_OLC_FUN( redit_show		);
DECLARE_OLC_FUN( redit_smell		);
DECLARE_OLC_FUN( redit_sound		);
DECLARE_OLC_FUN( redit_south		);
DECLARE_OLC_FUN( redit_up		);
DECLARE_OLC_FUN( redit_vehicle		);
DECLARE_OLC_FUN( redit_west		);

DECLARE_OLC_FUN( redit_rdamage          );
DECLARE_OLC_FUN( redit_edesc		);
DECLARE_OLC_FUN( redit_format		);
DECLARE_OLC_FUN( redit_import		);
DECLARE_OLC_FUN( redit_mdesc		);
DECLARE_OLC_FUN( redit_ndesc		);
DECLARE_OLC_FUN( redit_northeast	);
DECLARE_OLC_FUN( redit_northwest	);
DECLARE_OLC_FUN( redit_southeast	);
DECLARE_OLC_FUN( redit_southwest	);
DECLARE_OLC_FUN( redit_move		);
DECLARE_OLC_FUN( redit_rreset           );
DECLARE_OLC_FUN( redit_mlist		);
DECLARE_OLC_FUN( redit_olist		);
DECLARE_OLC_FUN( redit_mshow		);
DECLARE_OLC_FUN( redit_oshow		);
DECLARE_OLC_FUN( redit_rlist            );

/*
 * Function used by both aedit and redit
 */
DECLARE_OLC_FUN( edit_makelinks	);

/* rename_obj editor - Decklarean */
DECLARE_OLC_FUN( rename_obj_done	);
DECLARE_OLC_FUN( rename_obj_extra	);
DECLARE_OLC_FUN( rename_obj_keyword	);
DECLARE_OLC_FUN( rename_obj_long	);
DECLARE_OLC_FUN( rename_obj_short	);
DECLARE_OLC_FUN( rename_obj_show 	);

DECLARE_OLC_FUN( rpedit_arglist		);
DECLARE_OLC_FUN( rpedit_comlist		);
DECLARE_OLC_FUN( rpedit_create		);
DECLARE_OLC_FUN( rpedit_show		);


/*
 * Trap Editor Prototypes
 */
DECLARE_OLC_FUN( tedit_show             );
DECLARE_OLC_FUN( tedit_create           );
DECLARE_OLC_FUN( tedit_disarmable       );
DECLARE_OLC_FUN( tedit_arglist          );
DECLARE_OLC_FUN( tedit_comlist          );

/* Forge stuff  */
DECLARE_OLC_FUN( forge_show		);
DECLARE_OLC_FUN( forge_addaffect	);
DECLARE_OLC_FUN( forge_type		);

/* Command security editor */
DECLARE_OLC_FUN( secedit_add		);
DECLARE_OLC_FUN( secedit_create		);
DECLARE_OLC_FUN( secedit_delete		);
DECLARE_OLC_FUN( secedit_edit		);
DECLARE_OLC_FUN( secedit_list		);
DECLARE_OLC_FUN( secedit_remove		);
DECLARE_OLC_FUN( secedit_rename		);
DECLARE_OLC_FUN( secedit_show		);

/* Social editor prototypes */
DECLARE_OLC_FUN( sedit_show		);
DECLARE_OLC_FUN( sedit_delet		);
DECLARE_OLC_FUN( sedit_edlevel		);
DECLARE_OLC_FUN( sedit_delete		);
DECLARE_OLC_FUN( sedit_cnoarg		);
DECLARE_OLC_FUN( sedit_onoarg		);
DECLARE_OLC_FUN( sedit_cfound		);
DECLARE_OLC_FUN( sedit_ofound		);
DECLARE_OLC_FUN( sedit_vfound		);
DECLARE_OLC_FUN( sedit_nfound		);
DECLARE_OLC_FUN( sedit_cself		);
DECLARE_OLC_FUN( sedit_oself		);
DECLARE_OLC_FUN( sedit_undelete		);


/* Skill editor prototypes */
DECLARE_OLC_FUN( skedit_dammsg		);
DECLARE_OLC_FUN( skedit_delay		);
DECLARE_OLC_FUN( skedit_forget		);
DECLARE_OLC_FUN( skedit_mana		);
DECLARE_OLC_FUN( skedit_objmsg		);
DECLARE_OLC_FUN( skedit_offmsg		);
DECLARE_OLC_FUN( skedit_prereq		);
DECLARE_OLC_FUN( skedit_roommsg		);
DECLARE_OLC_FUN( skedit_show		);


/* Travel Editor prototypes */
DECLARE_OLC_FUN( travedit_afixed	);
DECLARE_OLC_FUN( travedit_atravel	);
DECLARE_OLC_FUN( travedit_dfixed	);
DECLARE_OLC_FUN( travedit_dtravel	);
DECLARE_OLC_FUN( travedit_movetime	);
DECLARE_OLC_FUN( travedit_room		);
DECLARE_OLC_FUN( travedit_stoptime	);
DECLARE_OLC_FUN( travedit_show		);

/* Temple editor prototypes */
DECLARE_OLC_FUN( tpedit_create		);
DECLARE_OLC_FUN( tpedit_imm		);
DECLARE_OLC_FUN( tpedit_obj1		);
DECLARE_OLC_FUN( tpedit_obj2		);
DECLARE_OLC_FUN( tpedit_save		);
DECLARE_OLC_FUN( tpedit_show		);

/*
 * Macros
 */
#define IS_BUILDER(ch, Area)	( !IS_NPC( ch ) &&\
				( ( can_aedit_all( ch, Area ) ) || \
				( ch->pcdata->security > Area->security	\
				|| ( ch->pcdata->security == Area->security \
				&& ( strstr( Area->builders, ch->name ) \
				|| strstr( Area->builders, "All" ) ) ) ) ) )
/*
#define IS_BUILDER(ch, Area)	( ( ch->pcdata->security >= Area->security  \
				|| strstr( Area->builders, ch->name )	    \
				|| strstr( Area->builders, "All" ) )	    \
				&& !IS_SWITCHED( ch ) )
*/
/*
#define IS_BUILDER(ch, Area)	( \
				( ( get_trust( ch ) >= IMPLEMENTOR ) || \
				( ch->pcdata->security > Area->security	\
				|| ( ch->pcdata->security == Area->security \
				&& ( strstr( Area->builders, ch->name ) \
				|| strstr( Area->builders, "All" ) ) ) ) ) )
*/


/* Return pointers to what is being edited. */
#define EDIT_AREA(Ch, Area)	( Area = (AREA_DATA *)(Ch)->desc->pEdit )
#define EDIT_BOARD(Ch, Board)	( Board = (BOARD_DATA *)(Ch)->desc->pEdit )
#define EDIT_CLAN(Ch, Clan)     ( Clan = (CLAN_DATA *)(Ch)->desc->pEdit )
#define EDIT_COMMAND(Ch, Cmd)	( Cmd = (int)(Ch)->desc->pEdit )
#define EDIT_DREAM(Ch, Dream)	( Dream = (DREAM_DATA *)(ch)->desc->pEdit )
#define EDIT_GROUP(Ch, Group)	( Group = (int)(Ch)->desc->pEdit )
#define EDIT_HELP(Ch, Help)     ( Help = (HELP_DATA *)(Ch)->desc->pEdit )
#define EDIT_MOB(Ch, Mob)	( Mob = (MOB_INDEX_DATA *)(Ch)->desc->pEdit )
#define EDIT_MPROG(Ch, MProg)   ( MProg = (MPROG_DATA *)(Ch)->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (OBJ_INDEX_DATA *)(Ch)->desc->pEdit )
#define EDIT_OREPROG(Ch, Prog)	( Prog = (OREPROG_DATA *)(Ch)->desc->pEdit )
#define EDIT_PROJECT(Ch, Proj)	( Proj = (PROJECT_DATA *)(Ch)->desc->pEdit )
#define EDIT_RACE(Ch, Sn)	( Sn = (int)(Ch)->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = (Ch)->in_room )
#define EDIT_SECURITY(ch, Sec)	( Sec = (int)(ch)->desc->pEdit )
#define EDIT_SKILL(Ch, Skill)	( Skill = (int)(Ch)->desc->pEdit )
#define EDIT_SOCIAL(Ch, Soc)	( Soc = (SOCIAL_DATA *)(Ch)->desc->pEdit )
#define EDIT_TRAVEL(Ch, Trav)	( Trav = (TRAVEL_DATA *)(Ch)->desc->pEdit )
#define RENAME_OBJ(Ch, Obj)	( Obj = (OBJ_DATA *)(Ch)->desc->pEdit )
#define FORGE_OBJ(Ch, Obj)	( Obj = (OBJ_DATA*)(Ch)->desc->pEdit )

#define EDIT_TEMPLE(Ch, Temple)	( Temple = (TEMPLE_DATA *)(Ch)->desc->pEdit )

#define EDIT_ISLAND(Ch, Island) ( Island = (ISLAND_DATA *)(Ch)->desc->pEdit )
#define EDIT_TRANS(Ch, Trans)	( Trans = (TRANS_DATA *)(Ch)->desc->pEdit )

