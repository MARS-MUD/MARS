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


/* this is a listing of all the commands and command related data */

bool	check_social	args( ( CHAR_DATA *ch, char *command,
			    char *argument ) );

/* wrapper function for safe command execution */
void	do_function	args( ( CHAR_DATA *ch, DO_FUN *do_fun,
				char *argument ) );
int	get_cmd_by_fun	args( ( DO_FUN *fun ) );
int	get_cmd_by_name	args( ( const char *command ) );

/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2
#define LOG_BUILD	3
#define LOG_ARGS	4	/* Log only if the command has arguments */

/*
 * Command levels
 */
#define ML 	MAX_LEVEL	/* implementor */
#define L1	MAX_LEVEL - 1  	/* creator */
#define L2	MAX_LEVEL - 2	/* supreme being */
#define L3	MAX_LEVEL - 3	/* deity */
#define L4 	MAX_LEVEL - 4	/* god */
#define L5	MAX_LEVEL - 5	/* immortal */
#define L6	MAX_LEVEL - 6	/* demigod */
#define L7	MAX_LEVEL - 7	/* angel */
#define L8	MAX_LEVEL - 8	/* avatar */
#define IM	LEVEL_IMMORTAL 	/* avatar */
#define HE	LEVEL_HERO	/* hero */

#define COM_IGNORE	1


/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
    sh_int		position;
    sh_int		level;
    sh_int		log;
    bitvector		flags;
    bitvector		sec_flags;
    int			usage;
    int			mob_usage;
    double		min_time;
    double		total_time;
    double		max_time;
    double		last_time;
    time_t		last_used;
};

/*
 * Flag bits for commands
 */
#define CMD_NONE		0
#define CMD_NOLIST		A  /* Command not listed by 'commands' or 'wizhelp' */
#define CMD_MOB			B  /* Command can only be used by mobs */
#define CMD_EXACT		C  /* Command must be typed exactly, in full */
#define CMD_DISABLED		D  /* Command (temporarily) not available */
#define CMD_CLAN		E  /* Command can only be used by clan members */
#define CMD_BOTH		F  /* Command requires both group and level. */
#define CMD_DEAD		G  /* Command cannot be used if dead */
#define CMD_UNHIDE		H  /* Command will remove hiding status */


/* the command table itself */
extern	struct	cmd_type	cmd_table	[];

bool	can_use_cmd		args( ( CHAR_DATA *ch, int cmd ) );

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN( do_admin	);
DECLARE_DO_FUN( do_addapply     );
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN( do_aedit	);
DECLARE_DO_FUN( do_aexits	);
DECLARE_DO_FUN( do_affects	);
DECLARE_DO_FUN( do_afk		);
DECLARE_DO_FUN( do_ainfo	);
DECLARE_DO_FUN( do_alia		);
DECLARE_DO_FUN( do_alias	);
DECLARE_DO_FUN( do_afind	);
DECLARE_DO_FUN( do_alist	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN( do_announce	);
DECLARE_DO_FUN( do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN( do_arealinks    );
DECLARE_DO_FUN( do_asave	);
DECLARE_DO_FUN( do_assassinate  );
DECLARE_DO_FUN( do_astat	);
DECLARE_DO_FUN( do_astrip	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN( do_autoassist	);
DECLARE_DO_FUN( do_autodamagedealt);
DECLARE_DO_FUN( do_autoexit	);
DECLARE_DO_FUN( do_autocoins	);
DECLARE_DO_FUN( do_autolist	);
DECLARE_DO_FUN( do_autoloot	);
DECLARE_DO_FUN( do_autometer	);
DECLARE_DO_FUN( do_autosac	);
DECLARE_DO_FUN( do_autosplit	);
DECLARE_DO_FUN( do_autotick	);
DECLARE_DO_FUN( do_avatar	);
DECLARE_DO_FUN( do_award	);
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN( do_badname	);
DECLARE_DO_FUN( do_balance	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN( do_bank		);
DECLARE_DO_FUN( do_bash		);
DECLARE_DO_FUN( do_beacon	);
DECLARE_DO_FUN( do_bedit	);
DECLARE_DO_FUN( do_berserk	);
DECLARE_DO_FUN( do_bid		);
DECLARE_DO_FUN( do_blist	);
DECLARE_DO_FUN( do_boards	);
DECLARE_DO_FUN( do_bodybag	);
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN( do_breathe     );
DECLARE_DO_FUN( do_brew		);
DECLARE_DO_FUN( do_brief	);
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN( do_cedit	);
DECLARE_DO_FUN( do_channels	);
DECLARE_DO_FUN( do_charge	);
DECLARE_DO_FUN( do_cinfo	);
DECLARE_DO_FUN( do_circle	);
DECLARE_DO_FUN( do_clans	);
DECLARE_DO_FUN( do_clantalk	);
DECLARE_DO_FUN( do_clear	);
DECLARE_DO_FUN( do_clone	);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN( do_cmdedit	);
DECLARE_DO_FUN( do_cmdstat	);
DECLARE_DO_FUN( do_coder	);
DECLARE_DO_FUN( do_coding	);
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN( do_combine	);
DECLARE_DO_FUN( do_compact	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_condition	);
DECLARE_DO_FUN( do_config	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN( do_count	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN( do_cset		);
DECLARE_DO_FUN( do_cstat	);
DECLARE_DO_FUN( do_ctimes	);
DECLARE_DO_FUN( do_deaf		);
DECLARE_DO_FUN( do_deathgrip	);
DECLARE_DO_FUN( do_debug	);
DECLARE_DO_FUN( do_delete	);
DECLARE_DO_FUN( do_delevel	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN( do_deposit	);
DECLARE_DO_FUN( do_descount	);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN( do_detract	);
DECLARE_DO_FUN( do_dirt		);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN( do_discharge	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN( do_dismount	);
DECLARE_DO_FUN( do_donate	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN( do_draw		);
DECLARE_DO_FUN( do_dream	);
DECLARE_DO_FUN( do_dreamedit	);
DECLARE_DO_FUN( do_dreamlist	);
DECLARE_DO_FUN( do_dreamstat	);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN( do_dual		);
DECLARE_DO_FUN( do_dump		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN( do_edit		);
DECLARE_DO_FUN( do_efind	);
DECLARE_DO_FUN( do_elist	);
DECLARE_DO_FUN( do_embark	);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN( do_empower	);
DECLARE_DO_FUN( do_enter	);
DECLARE_DO_FUN( do_envenom	);
DECLARE_DO_FUN( do_eplist	);
DECLARE_DO_FUN( do_epstat	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN( do_findlock	);
DECLARE_DO_FUN( do_finger	);
DECLARE_DO_FUN( do_flag		);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN( do_forcetick	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN( do_gain		);
DECLARE_DO_FUN( do_gedit	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN( do_glance	);
DECLARE_DO_FUN( do_gocial	);
DECLARE_DO_FUN( do_gossip	);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN( do_grant	);
DECLARE_DO_FUN( do_greet	);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN( do_groups	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN( do_guild    	);
DECLARE_DO_FUN( do_heal		);
DECLARE_DO_FUN( do_hedit	);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN( do_hero		);
DECLARE_DO_FUN( do_hfind	);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN( do_hlist	);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN(	do_hotboot	);
DECLARE_DO_FUN( do_housetalk	);
DECLARE_DO_FUN( do_ignore	);
DECLARE_DO_FUN( do_immlist	);
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN( do_imotd	);
DECLARE_DO_FUN( do_imote	);
DECLARE_DO_FUN( do_implementor	);
DECLARE_DO_FUN( do_incognito	);
DECLARE_DO_FUN( do_induct	);
DECLARE_DO_FUN( do_info		);
DECLARE_DO_FUN( do_instaroom	);
DECLARE_DO_FUN( do_introduce	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN( do_invoke	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN( do_iscore	);
DECLARE_DO_FUN( do_isocial	);
DECLARE_DO_FUN( do_jail		);
DECLARE_DO_FUN( do_join		);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN( do_kingdoms	);
DECLARE_DO_FUN(	do_knock	);
DECLARE_DO_FUN( do_last		);
DECLARE_DO_FUN( do_lastmod	);
DECLARE_DO_FUN( do_launch	);
DECLARE_DO_FUN( do_leave	);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_listen	);
DECLARE_DO_FUN( do_linkdead	);
DECLARE_DO_FUN( do_load		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN(	do_lore		);
DECLARE_DO_FUN( do_map		);
DECLARE_DO_FUN( do_matrix	);
DECLARE_DO_FUN( do_medit	);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN( do_mlist	);
DECLARE_DO_FUN( do_mount	);
DECLARE_DO_FUN( do_mountname	);
DECLARE_DO_FUN( do_mpacquaint	);
DECLARE_DO_FUN( do_mpaecho	);
DECLARE_DO_FUN( do_mpasound	);
DECLARE_DO_FUN( do_mpat		);
DECLARE_DO_FUN( do_mpcancel	);
DECLARE_DO_FUN( do_mpclose	);
DECLARE_DO_FUN( do_mpcommands	);
DECLARE_DO_FUN( do_mpdamage	);
DECLARE_DO_FUN( do_mpdelay	);
DECLARE_DO_FUN( do_mpecho	);
DECLARE_DO_FUN( do_mpechoaround	);
DECLARE_DO_FUN( do_mpechoat	);
DECLARE_DO_FUN( do_mpforce	);
DECLARE_DO_FUN( do_mpforget	);
DECLARE_DO_FUN( do_mpgive	);
DECLARE_DO_FUN( do_mpgoto       );
DECLARE_DO_FUN( do_mpgtransfer	);
DECLARE_DO_FUN( do_mpjunk       );
DECLARE_DO_FUN( do_mpkill       );
DECLARE_DO_FUN( do_mplist	);
DECLARE_DO_FUN( do_mplock	);
DECLARE_DO_FUN( do_mpmload      );
DECLARE_DO_FUN( do_mpoload      );
DECLARE_DO_FUN( do_mpopen	);
DECLARE_DO_FUN( do_mppeace	);
DECLARE_DO_FUN( do_mppurge      );
DECLARE_DO_FUN( do_mpremember	);
DECLARE_DO_FUN( do_mpsecho	);
DECLARE_DO_FUN( do_mpsechoaround);
DECLARE_DO_FUN( do_mpsechoat	);
DECLARE_DO_FUN( do_mpsecret	);
DECLARE_DO_FUN( do_mpsetpos	);
DECLARE_DO_FUN( do_mpstat	);
DECLARE_DO_FUN( do_mptake	);
DECLARE_DO_FUN( do_mpteleport   );
DECLARE_DO_FUN( do_mptransfer   );
DECLARE_DO_FUN( do_mpunhide	);
DECLARE_DO_FUN( do_mpunlock	);
DECLARE_DO_FUN( do_mreset	);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN( do_mob		);
DECLARE_DO_FUN( do_mobcount	);
DECLARE_DO_FUN( do_motd		);
DECLARE_DO_FUN(	do_murder	);
DECLARE_DO_FUN( do_newbie	);
DECLARE_DO_FUN( do_newlock	);
DECLARE_DO_FUN( do_new_score    );
DECLARE_DO_FUN( do_nochannels	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN( do_nofollow	);
DECLARE_DO_FUN( do_noloot	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN( do_northeast	);
DECLARE_DO_FUN( do_northwest	);
DECLARE_DO_FUN(	do_noshout	);
DECLARE_DO_FUN( do_nosummon	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_noexp	);
DECLARE_DO_FUN( do_nukereply	);
DECLARE_DO_FUN( do_oedit	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN( do_olocate	);
DECLARE_DO_FUN( do_olist	);
DECLARE_DO_FUN( do_omni		);
DECLARE_DO_FUN( do_ooc		);
DECLARE_DO_FUN( do_ooc_dot	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN( do_oplist	);
DECLARE_DO_FUN( do_opstat	);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN( do_oreset	);
DECLARE_DO_FUN( do_osay         );
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN( do_outcast	);
DECLARE_DO_FUN( do_outfit	);
DECLARE_DO_FUN( do_owhere	);
DECLARE_DO_FUN(	do_pack	);
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN( do_pecho	);
DECLARE_DO_FUN( do_pedit	);
DECLARE_DO_FUN( do_permban	);
DECLARE_DO_FUN( do_petname	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN(	do_pipe_system		);
DECLARE_DO_FUN( do_pkill	);
DECLARE_DO_FUN( do_pload	);
DECLARE_DO_FUN( do_plist        );
DECLARE_DO_FUN( do_pnuke        );
DECLARE_DO_FUN( do_pmote	);
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN( do_pour		);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN( do_pray		);
DECLARE_DO_FUN( do_prefix	);
DECLARE_DO_FUN( do_project	);
DECLARE_DO_FUN( do_prompt	);
DECLARE_DO_FUN( do_protect	);
DECLARE_DO_FUN( do_pull		);
DECLARE_DO_FUN( do_punload	);
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN( do_push		);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN( do_quest	);
DECLARE_DO_FUN( do_question	);
DECLARE_DO_FUN( do_quiet	);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN( do_racedit	);
DECLARE_DO_FUN( do_racelist	);
DECLARE_DO_FUN( do_racestat	);
DECLARE_DO_FUN( do_rdesc	);
DECLARE_DO_FUN( do_read		);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_new_reboot	);
DECLARE_DO_FUN( do_rebuild	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN( do_recent	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_relevel	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN( do_redit	);
DECLARE_DO_FUN( do_remote	);
DECLARE_DO_FUN( do_rename	);
DECLARE_DO_FUN( do_rename_char	);
DECLARE_DO_FUN( do_rename_obj	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN( do_replay	);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN( do_resets	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN( do_review	);
DECLARE_DO_FUN( do_rfind	);
DECLARE_DO_FUN( do_ride		);
DECLARE_DO_FUN( do_rinfo	);
DECLARE_DO_FUN( do_rlist	);
DECLARE_DO_FUN( do_road		);
DECLARE_DO_FUN( do_rplist	);
DECLARE_DO_FUN( do_rpstat	);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN( do_rub		);
DECLARE_DO_FUN( do_rules	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN( do_sail		);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_sayto	);
DECLARE_DO_FUN(	do_scan		);
DECLARE_DO_FUN( do_scatter      );
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN( do_scroll	);
DECLARE_DO_FUN( do_search	);
DECLARE_DO_FUN( do_secedit	);
DECLARE_DO_FUN( do_seclist	);
DECLARE_DO_FUN( do_sedit	);
DECLARE_DO_FUN( do_seize	);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN( do_separate	);
DECLARE_DO_FUN( do_set		);
DECLARE_DO_FUN(	do_setcolor	);
DECLARE_DO_FUN( do_setlev	);
DECLARE_DO_FUN( do_sever        );
DECLARE_DO_FUN( do_sheath	);
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN( do_show		);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN( do_sit		);
DECLARE_DO_FUN( do_skedit	);
DECLARE_DO_FUN( do_skills	);
DECLARE_DO_FUN( do_skstat	);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN( do_slist	);
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN( do_smell	);
DECLARE_DO_FUN( do_smite	);
DECLARE_DO_FUN( do_smote	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN( do_socials	);
DECLARE_DO_FUN( do_socstat	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_southeast	);
DECLARE_DO_FUN( do_southwest	);
DECLARE_DO_FUN( do_sockets	);
DECLARE_DO_FUN( do_spells	);
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN( do_sqldump	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN( do_stare	);
DECLARE_DO_FUN( do_stat		);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN( do_steer	);
DECLARE_DO_FUN( do_story	);
DECLARE_DO_FUN( do_string	);
DECLARE_DO_FUN( do_study	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN( do_tether	);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_track	);
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN( do_travstat	);
DECLARE_DO_FUN( do_triage	);
DECLARE_DO_FUN( do_trip		);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN( do_todo		);
DECLARE_DO_FUN( do_touch	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN( do_unalias	);
DECLARE_DO_FUN( do_ungreet	);
DECLARE_DO_FUN( do_unlaunch	);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN( do_unread	);
DECLARE_DO_FUN( do_untether	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN( do_uptime	);
DECLARE_DO_FUN( do_usage	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN( do_vfree	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN( do_violate	);
DECLARE_DO_FUN( do_vlist	);
DECLARE_DO_FUN( do_vnum		);
DECLARE_DO_FUN( do_vused	);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN( do_whisper      );
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN( do_whoborder	);
DECLARE_DO_FUN( do_whotext	);
DECLARE_DO_FUN( do_whois	);
DECLARE_DO_FUN(	do_wield	);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN( do_wizlist	);
DECLARE_DO_FUN( do_wiznet	);
DECLARE_DO_FUN( do_wizpwd	);
DECLARE_DO_FUN( do_withdraw	);
DECLARE_DO_FUN( do_worth	);
DECLARE_DO_FUN( do_wpeace	);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN( do_youcount	);
DECLARE_DO_FUN(	do_zap		);
DECLARE_DO_FUN( do_zecho	);
DECLARE_DO_FUN( do_butcher	);
DECLARE_DO_FUN( do_skin	        );
DECLARE_DO_FUN( do_sharpen	);
DECLARE_DO_FUN( do_blackjack	);
DECLARE_DO_FUN( do_whirlwind	);
DECLARE_DO_FUN( do_new_change    );
DECLARE_DO_FUN( do_idle          );
DECLARE_DO_FUN(do_backup);
DECLARE_DO_FUN(do_mudstat);
DECLARE_DO_FUN(do_empty);
DECLARE_DO_FUN(do_engage);
DECLARE_DO_FUN(do_setcoder);
DECLARE_DO_FUN(do_oldscore);
DECLARE_DO_FUN(do_istat);
DECLARE_DO_FUN(do_wizify);
DECLARE_DO_FUN(do_played_age);
DECLARE_DO_FUN(do_mortskill);
DECLARE_DO_FUN(do_mortspell);
DECLARE_DO_FUN(do_purify);