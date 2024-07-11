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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"

DECLARE_DO_FUN( do_rnum );

/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;


/*
 * Command table.
 */
struct	cmd_type	cmd_table	[] =
{
    /*
     * Common movement commands.
     */
    { "north",		do_north,	POS_STANDING,    0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "east",		do_east,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "south",		do_south,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "west",		do_west,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "up",		do_up,		POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "down",		do_down,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "northeast",	do_northeast,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "northwest",	do_northwest,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "southeast",	do_southeast,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "southwest",	do_southwest,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "neast",		do_northeast,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "nwest",		do_northwest,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "seast",		do_southeast,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "swest",		do_southwest,	POS_STANDING,	 0,  LOG_NEVER, CMD_NOLIST|CMD_UNHIDE },
    { "road",		do_road,	POS_STANDING,	 0,  LOG_NEVER, CMD_DEAD|CMD_UNHIDE },
    { "ride",		do_ride,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "at",             do_at,          POS_DEAD,       L8,  LOG_NORMAL, 0 },
    { "auction",        do_auction,     POS_SLEEPING,    0,  LOG_NORMAL, CMD_DEAD },
    { "buy",		do_buy,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "butcher",		do_butcher,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "channels",       do_channels,    POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "clans",		do_clans,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "clantalk",	do_clantalk,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "finger",		do_finger,	POS_DEAD,	L2,  LOG_NORMAL, 0 },
    { "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "go",		do_enter,	POS_STANDING,	 0,  LOG_NORMAL, CMD_NOLIST|CMD_DEAD|CMD_UNHIDE },
/*  { "goto",           do_goto,        POS_DEAD,       L8,  LOG_NORMAL, 0 }, */
    { "group",          do_group,       POS_SLEEPING,    0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "hit",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_NOLIST|CMD_DEAD|CMD_UNHIDE },
    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL, CMD_DEAD },
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "look",		do_look,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "ls",		do_look,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
     { "lore",		do_lore,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "order",		do_order,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "practice",       do_practice,	POS_SLEEPING,    0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "quest",		do_quest,	POS_STANDING,	 3,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "rest",		do_rest,	POS_SLEEPING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "sit",		do_sit,		POS_SLEEPING,    0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "sockets",        do_sockets,	POS_DEAD,       L4,  LOG_NORMAL, 0 },
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, CMD_UNHIDE },
    { "steer",		do_steer,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "tell",		do_tell,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "unlock",         do_unlock,      POS_RESTING,     0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "wield",		do_wield,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "wizhelp",	do_wizhelp,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "wizify",     do_wizify,  POS_DEAD,   ML,  LOG_ALWAYS, 0 },

    /*
     * Moved here to avoid masking by other commands.
     */
    { "set",		do_set,		POS_DEAD,	L2,  LOG_ALWAYS, 0 },
    { "set_coder",		do_setcoder,		POS_DEAD,	ML,  LOG_ALWAYS, 0 },

    /*
     * Informational commands.
     */
    { "affects",	do_affects,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "cinfo",		do_cinfo,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "condition",	do_condition,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "count",		do_count,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL, CMD_DEAD },
    { "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
/*  { "groups",		do_groups,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 }, */
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "hinfo",		do_cinfo,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "houses",		do_clans,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "housetalk",	do_housetalk,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "htalk",		do_housetalk,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "ignore",		do_ignore,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "info",           do_info,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "kingdoms",	do_kingdoms,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "map",		do_map,		POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "matrix",		do_matrix,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "motd",		do_motd,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "played", do_played_age, POS_SLEEPING, 0, LOG_NORMAL, 0 },
    { "racelist",	do_racelist,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "read",		do_read,	POS_RESTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "report",		do_report,	POS_RESTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "rules",		do_rules,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "score2",		do_new_score,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "oldscore",		do_oldscore,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "scan",		do_scan,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "skills",		do_skills,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "skin",		do_skin,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "smell",		do_smell,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "gocial",	do_gocial,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "show",		do_show,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "spells",		do_spells,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "story",		do_story,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "triage",		do_triage,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "track",		do_track,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "idlers",        do_idle,     POS_DEAD,    0,  LOG_NORMAL, 0 },
    { "whois",		do_whois,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "wizlist",	do_wizlist,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "worth",		do_worth,	POS_SLEEPING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },

    /*
     * Configuration commands.
     */
    { "alia",		do_alia,	POS_DEAD,	 0,  LOG_NORMAL, CMD_NOLIST, COM_CFIG },
    { "alias",		do_alias,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "autolist",	do_autolist,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "autoassist",	do_autoassist,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "autodamagedealt", do_autodamagedealt, POS_DEAD,   0,  LOG_NORMAL, 0 },
    { "autoexit",	do_autoexit,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "autogold",	do_autocoins,	POS_DEAD,        0,  LOG_NORMAL, CMD_NOLIST },
    { "autocoins",	do_autocoins,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "autoloot",	do_autoloot,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "autometer",	do_autometer,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "autosac",	do_autosac,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "autosplit",	do_autosplit,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "autotick",	do_autotick,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "brief",		do_brief,	POS_DEAD,        0,  LOG_NORMAL, 0 },
/*  { "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL, 0 }, */
    { "clear",		do_clear,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "combine",	do_combine,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "compact",	do_compact,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "delete",		do_delete,	POS_STANDING,	 0,  LOG_ALWAYS, CMD_EXACT|CMD_UNHIDE },
    { "nofollow",	do_nofollow,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "noloot",		do_noloot,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "nosummon",	do_nosummon,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "noexp",		do_noexp,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "outfit",		do_outfit,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER,  0 },
    { "prompt",		do_prompt,	POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "scroll",		do_scroll,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "title",		do_title,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "unalias",	do_unalias,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "display",    do_display, POS_DEAD,    0,  LOG_NORMAL, 1 },

    /*
     * Communication commands.
     */
    { "afk",		do_afk,		POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "answer",		do_answer,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
/*  { "auction",	do_auction,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 }, */
    { "boards",		do_boards,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "deaf",		do_deaf,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "dream",		do_dream,	POS_SLEEPING,	L8,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "gossip",		do_gossip,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { ",",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, CMD_NOLIST },
    { "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { ";",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL, CMD_NOLIST|CMD_UNHIDE },
    { "hero",		do_hero,	POS_RESTING,	HE,  LOG_NORMAL, 0 },
    { "newbie",		do_newbie,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "note",		do_note,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "pose",		do_pose,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "ooc",		do_ooc,		POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { ".",		do_ooc_dot,	POS_SLEEPING,	 0,  LOG_NORMAL, CMD_NOLIST },
    { "osay",           do_osay,        POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "pmote",		do_pmote,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "pray",		do_pray,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "question",	do_question,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "quiet",		do_quiet,	POS_SLEEPING, 	 0,  LOG_NORMAL, 0 },
    { "remote",		do_remote,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "reply",		do_reply,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "replay",		do_replay,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "review",		do_review,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "say",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "'",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, CMD_NOLIST|CMD_UNHIDE },
    { "\"",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, CMD_NOLIST|CMD_UNHIDE },
    { "sayto",		do_sayto,	POS_RESTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "whisper",        do_whisper,     POS_RESTING,     0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "shout",		do_shout,	POS_RESTING,	 3,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "unread",		do_unread,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "yell",		do_yell,	POS_RESTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },

    /*
     * Object manipulation commands.
     */
    { "bid",		do_bid,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "brew",		do_brew,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "breathe",       do_breathe,    POS_RESTING,     0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "donate",		do_donate,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "drop",		do_drop,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "dual",		do_dual,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "envenom",	do_envenom,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "empty",		do_empty,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "glance",		do_glance,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "heal",		do_heal,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "hold",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "invoke",		do_invoke,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "join",		do_join,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "knock",		do_knock,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "list",		do_list,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "pour",		do_pour,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    {"purify", do_purify, POS_RESTING, 0, LOG_NORMAL, 0, 0},
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "rename",		do_rename,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "rub",		do_rub,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "sell",		do_sell,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "sheath",		do_sheath,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "sharpen",		do_sharpen,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "junk",           do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, CMD_NOLIST|CMD_DEAD|CMD_UNHIDE },
    { "search",		do_search,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "separate",	do_separate,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "stare",		do_stare,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "study",		do_study,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
/*  { "unlock",		do_unlock,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD }, */
    { "value",		do_value,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },

    /* Avoid conflict with "list" */
    { "listen",		do_listen,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    /*
     * Combat commands.
     */
    { "assassinate",       do_assassinate,    POS_FIGHTING,    0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "backstab",	do_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "bash",		do_bash,	POS_FIGHTING,    0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "bs",		do_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_NOLIST|CMD_DEAD|CMD_UNHIDE },
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "charge",		do_charge,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "circle",		do_circle,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "dirt",		do_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "dk",		    do_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "engage",		do_engage,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
/*  { "draw",		do_draw,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE }, */
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "blackjack",      do_blackjack,   POS_FIGHTING,    0,  LOG_NORMAL, 1 },
    { "murder",		do_murder,	POS_FIGHTING,	 5,  LOG_ALWAYS, CMD_EXACT|CMD_DEAD|CMD_UNHIDE },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "trip",		do_trip,	POS_FIGHTING,    0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "deathgrip",      do_deathgrip,   POS_RESTING,     0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
        { "whirlwind",     do_whirlwind,   POS_FIGHTING,    0,  LOG_NORMAL, 1 },


    /*
     * Clan commands
     */
    { "discharge",	do_discharge,	POS_RESTING,	 0,  LOG_ALWAYS, CMD_CLAN|CMD_DEAD|CMD_UNHIDE },
    { "induct",		do_induct,	POS_RESTING,	 0,  LOG_ALWAYS, CMD_CLAN|CMD_DEAD|CMD_UNHIDE },
    { "outcast",	do_outcast,	POS_RESTING,	 0,  LOG_ALWAYS, CMD_CLAN|CMD_DEAD|CMD_UNHIDE },
    { "setlev",		do_setlev,	POS_RESTING,	 0,  LOG_ALWAYS, CMD_CLAN|CMD_DEAD|CMD_UNHIDE },

    /*
     * Miscellaneous commands.
     */
    { "balance",	do_balance,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "deposit",	do_deposit,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "dismount",	do_dismount,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "embark",		do_embark,	POS_STANDING,	 1,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "enter", 		do_enter, 	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "gain",		do_gain,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "greet",		do_greet,	POS_RESTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
/*  { "group",		do_group,	POS_SLEEPING,	 0,  LOG_NORMAL, CMD_DEAD }, */
    { "groups",		do_groups,	POS_SLEEPING,    0,  LOG_NORMAL, 0 },
    { "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "introduce",	do_introduce,	POS_RESTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "leave",		do_leave,	POS_STANDING,	 1,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "mount",		do_mount,	POS_STANDING,    0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "mountname",	do_mountname,	POS_RESTING,	L7,  LOG_NORMAL, 0 },
    { "petname",	do_petname,	POS_RESTING,	L7,  LOG_NORMAL, 0 },
/*  { "practice",	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE }, */
    { "pull",		do_pull,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "push",		do_push,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "quit",		do_quit,	POS_DEAD,	 0,  LOG_NORMAL, CMD_EXACT|CMD_UNHIDE },
    { "recall",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "rent",		do_rent,	POS_DEAD,	 0,  LOG_NORMAL, CMD_NOLIST },
    { "sail",		do_sail,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "save",		do_save,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD },
    { "tether",		do_tether,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "touch",		do_touch,	POS_STANDING,	 0,  LOG_NORMAL, 0 },
    { "train",		do_train,	POS_RESTING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "untether",	do_untether,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 0|CMD_UNHIDE },
    { "where",		do_where,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "withdraw",	do_withdraw,	POS_STANDING,	 0,  LOG_NORMAL, CMD_DEAD|CMD_UNHIDE },


    /*
     * Immortal commands.
     */
    { "admin",		do_admin,	POS_DEAD,	L2,  LOG_NORMAL, 0 },
    { "[",		do_admin,	POS_DEAD,	L2,  LOG_NORMAL, CMD_NOLIST },
    { "advance",	do_advance,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
     { "addapply",	do_addapply,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "aedit",		do_aedit,	POS_DEAD,	L6,  LOG_BUILD,  0 },
    { "aexits",		do_aexits,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "afind",		do_afind,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "ainfo",		do_ainfo,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "alist",		do_alist,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "allow",		do_allow,	POS_DEAD,	L2,  LOG_ALWAYS, 0 },
    { "announce",	do_announce,	POS_DEAD,	L7,  LOG_ALWAYS, 0 },
    { "asave",		do_asave,	POS_DEAD,	L6,  LOG_BUILD,  0 },
    { "astat",		do_astat,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "astrip",		do_astrip,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "arealinks",  do_arealinks,   POS_DEAD,       ML, LOG_NORMAL, 1 },
/*  { "at",		do_at,		POS_DEAD,	L6,  LOG_NORMAL, 0 }, */
    { "avatar",		do_avatar,	POS_RESTING,	1,   LOG_ALWAYS, 0 },
    { "award",		do_award,	POS_DEAD,	L5,  LOG_ALWAYS, 0 },
    { "badname",	do_badname,	POS_DEAD,	L5,  LOG_ARGS,   0 },
    { "bamfin",		do_bamfin,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "bamfout",	do_bamfout,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "ban",		do_ban,		POS_DEAD,	L2,  LOG_ARGS  , 0 },
    { "beacon",		do_beacon,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "bedit",		do_bedit,	POS_DEAD,	ML,  LOG_BUILD,  0 },
    { "blist",		do_blist,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "bodybag",	do_bodybag,	POS_DEAD,	IM,  LOG_ALWAYS, 0 },
    { "cedit",		do_cedit,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "clone",		do_clone,	POS_DEAD,	L5,  LOG_ALWAYS, 0 },
    { "cmdstat",	do_cmdstat,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "cmdedit",	do_cmdedit,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "coder",		do_coder,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "\\",		do_coder,	POS_DEAD,	L1,  LOG_NORMAL, CMD_NOLIST },
    { "coding",		do_coding,	POS_DEAD,	ML,  LOG_NORMAL, CMD_NOLIST },
    { "copyover",	do_hotboot,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_EXACT },
    { "cset",		do_cset,	POS_DEAD,	L1,  LOG_ALWAYS, 0 },
    { "cstat",		do_cstat,	POS_DEAD,	MAX_MORTAL, LOG_NORMAL, 0 },
    { "ctimes",		do_ctimes,	POS_DEAD,	L1,  LOG_NORMAL, 0 },
    { "debug",		do_debug,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_NOLIST },
    { "dedit",		do_dreamedit,	POS_DEAD,	L2,  LOG_ALWAYS, 0 },
    { "deny",		do_deny,	POS_DEAD,	L1,  LOG_ALWAYS, 0 },
    { "descount",	do_descount,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "detract",	do_detract,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "disconnect",	do_disconnect,	POS_DEAD,	L4,  LOG_ALWAYS, 0 },
    { "dlist",		do_dreamlist,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "dreamedit",	do_dreamedit,	POS_DEAD,	L2,  LOG_ALWAYS, 0 },
    { "dstat",		do_dreamstat,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "dump",		do_dump,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_NOLIST },
    { "echo",		do_recho,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "edit",		do_edit,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "efind",		do_efind,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "elist",		do_elist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "empower",	do_empower,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "eplist",		do_eplist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "epstat",		do_epstat,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "findlock",	do_findlock,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
/*  { "finger",		do_finger,	POS_DEAD,	L1,  LOG_NORMAL, 0 }, */
    { "flag",		do_flag,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "force",		do_force,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "forcetick",	do_forcetick,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "freeze",		do_freeze,	POS_DEAD,	L4,  LOG_ALWAYS, 0 },
    { "gecho",		do_echo,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "gedit",		do_gedit,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "goto",		do_goto,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "grant",		do_grant,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "guild",		do_guild,	POS_DEAD,	L4,  LOG_ALWAYS, 0 },
    { "hedit",		do_hedit,	POS_DEAD,	ML,  LOG_BUILD,  0 },
    { "hfind",		do_hfind,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "hlist",		do_hlist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "holylight",	do_holylight,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "hotboot",	do_hotboot,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_EXACT },
    { ":",		do_immtalk,	POS_DEAD,	IM,  LOG_NORMAL, CMD_NOLIST },
    { "immtalk",	do_immtalk,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "imote",		do_imote,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "imotd",          do_imotd,       POS_DEAD,       IM,  LOG_NORMAL, 0 },
    { "implementor",	do_implementor,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "=",		do_implementor,	POS_DEAD,	ML,  LOG_NORMAL, CMD_NOLIST },
    { "incognito",	do_incognito,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "instaroom",	do_instaroom,	POS_DEAD,	L6,  LOG_BUILD,	 0 },
    { "invis",		do_invis,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "isocial",	do_isocial,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "iscore",		do_iscore,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "istat",      do_istat,   POS_DEAD,    0,  LOG_NEVER,  0 },
    { "jail",		do_jail,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "last",		do_last,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "lastmod",	do_lastmod,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "launch",		do_launch,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "linkdead",	do_linkdead,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "load",		do_load,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "log",		do_log,		POS_DEAD,	L1,  LOG_ALWAYS, 0 },
    { "medit",		do_medit,	POS_DEAD,	L6,  LOG_BUILD,  0 },
    { "memory",		do_memory,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "mfind",		do_mfind,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "mlist",		do_mlist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "mload",		do_mload,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "mob",		do_mob,		POS_DEAD,	 1,  LOG_NORMAL, CMD_NOLIST },
    { "mobcount",	do_mobcount,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "mpcommands",	do_mpcommands,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "mplist",		do_mplist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "mpstat",		do_mpstat,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "mreset",		do_mreset,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "mset",		do_mset,	POS_DEAD,	L8,  LOG_ALWAYS, 0 },
    { "mstat",		do_mstat,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "mwhere",		do_mwhere,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "mskill",     do_mortskill, POS_DEAD, L4,  LOG_NORMAL, 0 },
    { "mspell",     do_mortspell, POS_DEAD, L4,  LOG_NORMAL, 0 },
    { "newlock",	do_newlock,	POS_DEAD,	L4,  LOG_ALWAYS, 0 },
    { "new_change",      do_new_change,    POS_DEAD,   ML,  LOG_NORMAL, 0 },
    { "nochannels",	do_nochannels,	POS_DEAD,	L5,  LOG_ALWAYS, 0 },
    { "noemote",	do_noemote,	POS_DEAD,	L5,  LOG_ALWAYS, 0 },
    { "noshout",	do_noshout,	POS_DEAD,	L5,  LOG_ALWAYS, 0 },
    { "notell",		do_notell,	POS_DEAD,	L5,  LOG_ALWAYS, 0 },
    { "nukereply",	do_nukereply,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "pnuke",      do_pnuke,   POS_DEAD,   ML,  LOG_NORMAL, 0 },
    { "oedit",		do_oedit,	POS_DEAD,	L6,  LOG_BUILD,  0 },
    { "ofind",		do_ofind,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "olist",		do_olist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "oload",		do_oload,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "olocate",	do_olocate,	POS_DEAD,	L4,  LOG_NORMAL, 0 },
    { "omni",		do_omni,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "oplist",		do_oplist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "opstat",		do_opstat,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "oreset",		do_oreset,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "oset",		do_oset,	POS_DEAD,	IM,  LOG_ALWAYS, 0 },
    { "ostat",		do_ostat,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "owhere",		do_owhere,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "pardon",		do_pardon,	POS_DEAD,	L3,  LOG_ALWAYS, 0 },
    { "pack",		do_pack,	POS_DEAD,	IM,  LOG_ALWAYS, 0 },
    { "peace",		do_peace,	POS_DEAD,	L5,  LOG_NORMAL, 0 },
    { "pecho",		do_pecho,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "pedit",		do_pedit,	POS_DEAD,	L8,  LOG_ALWAYS, 0 },
    { "permban",	do_permban,	POS_DEAD,	L1,  LOG_ALWAYS, 0 },
    { "pkill",		do_pkill,	POS_DEAD,	L2,  LOG_ARGS,   0 },
    { "pload",		do_pload,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "plist",          do_plist,       POS_DEAD,       ML,  LOG_ALWAYS, 0 },
    { "poofin",		do_bamfin,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "poofout",	do_bamfout,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "prefix",		do_prefix,	POS_DEAD,	IM,  LOG_NORMAL, CMD_EXACT },
    { "project",	do_project,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "protect",	do_protect,	POS_DEAD,	L1,  LOG_ALWAYS, 0 },
    { "punload",	do_punload,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "pipe",	        do_pipe_system,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "purge",		do_purge,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "pwhere",		do_mwhere,	POS_DEAD,	L8,  LOG_NORMAL, CMD_NOLIST  },
    { "racestat",	do_racestat,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "racedit",	do_racedit,	POS_DEAD,	L1,  LOG_BUILD,  0 },
    { "rdesc",		do_rdesc,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "reboot",		do_reboot,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_EXACT|CMD_DISABLED },
    { "nreboot",		do_new_reboot,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_EXACT },
    { "rebuild",	do_rebuild,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_EXACT },
    { "recent",		do_recent,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "redit",		do_redit,	POS_DEAD,	L6,  LOG_BUILD,  0 },
    { "rename_char",	do_rename_char,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "resets",		do_resets,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "restore",	do_restore,	POS_DEAD,	L4,  LOG_ALWAYS, 0 },
    { "return",         do_return,      POS_DEAD,       L7,  LOG_NORMAL, 0 },
    { "rfind",		do_rfind,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "rinfo",		do_rinfo,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "rip",            do_sever,       POS_STANDING,   ML,  LOG_ALWAYS, 0 },
    { "rlist",		do_rlist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "rnumber",	do_rnum,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "rplist",		do_rplist,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "rpstat",		do_rpstat,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "rstat",		do_rstat,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "scatter",	do_scatter, 	POS_DEAD, 	IM,  LOG_NORMAL, 0 },
    { "secedit",	do_secedit,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "seclist",	do_seclist,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "sedit",		do_sedit,	POS_SLEEPING,	ML,  LOG_BUILD,  0 },
    { "sedshow",	do_socstat,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "seize",		do_seize,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_EXACT },
/*  { "set",		do_set,		POS_DEAD,	L2,  LOG_ALWAYS, 0 }, */
    { "shutdown",	do_shutdown,	POS_DEAD,	ML,  LOG_ALWAYS, CMD_EXACT },
    { "skedit",		do_skedit,	POS_DEAD,	ML,  LOG_BUILD,  0 },
    { "skstat",		do_skstat,	POS_DEAD,	L2,  LOG_NORMAL, 0 },
    { "slay",		do_slay,	POS_DEAD,	L3,  LOG_ALWAYS, CMD_EXACT },
    { "slist",		do_slist,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "slookup",	do_slookup,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "smite",		do_smite,	POS_DEAD,	L3,  LOG_ALWAYS, CMD_EXACT },
    { "smote",		do_smote,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "snoop",		do_snoop,	POS_DEAD,	L2,  LOG_ALWAYS, 0 },
/*  { "sockets",	do_sockets,	POS_DEAD,	L4,  LOG_NORMAL, 0 }, */
    { "socstat",	do_socstat,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "sqldump",	do_sqldump,	POS_DEAD,	ML,  LOG_ARGS,   0 },
    { "sset",		do_sset,	POS_DEAD,	L8,  LOG_ALWAYS, 0 },
    { "stat",		do_stat,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "string",		do_string,	POS_DEAD,	L8,  LOG_ALWAYS, 0 },
    { "switch",		do_switch,	POS_DEAD,	L7,  LOG_ALWAYS, 0 },
    { "teleport",	do_transfer,    POS_DEAD,	L5,  LOG_ALWAYS, 0 },
    { "todo",		do_todo,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "transfer",	do_transfer,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "travstat",	do_travstat,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "trust",		do_trust,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "ungreet",	do_ungreet,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "unlaunch",	do_unlaunch,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "uptime",		do_uptime,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "users",		do_sockets,	POS_DEAD,       L4,  LOG_NORMAL, 0 },
    { "usage",		do_usage,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "violate",	do_violate,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "vlist",		do_vlist,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "whoborder",	do_whoborder,	POS_DEAD,	ML,  LOG_ARGS,   0 },
    { "whotext",	do_whotext,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "wizinvis",	do_invis,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "wizlock",	do_wizlock,	POS_DEAD,	L2,  LOG_ALWAYS, 0 },
    { "wiznet",		do_wiznet,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "wizpwd",		do_wizpwd,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "wpeace",		do_wpeace,	POS_DEAD,	L2,  LOG_ALWAYS, 0 },
    { "vfree",		do_vfree,	POS_DEAD,	L8,  LOG_NORMAL, 0 },
    { "vnum",		do_vnum,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "vused",		do_vused,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "youcount",	do_youcount,	POS_DEAD,	L6,  LOG_NORMAL, 0 },
    { "zecho",		do_zecho,	POS_DEAD,	L6,  LOG_ALWAYS, 0 },
    { "mudbackup",	do_backup,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "mudstat",	do_mudstat,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },

    /* "immlist has to come after "immtalk" :) */
    { "immlist",	do_immlist,	POS_DEAD,	0,   LOG_NORMAL, 0 },

    /* "setcolor" has to come after "set" */
    { "setcolor",	do_setcolor,	POS_DEAD,	0,   LOG_NORMAL, 0 },
    /* "bank" has to come after "ban" */
    { "bank",		do_bank,	POS_STANDING,	0,   LOG_NORMAL, CMD_DEAD },

    /*
     *  Mobile commands, can only be performed by mobiles.
     */
    { "mpacquaint",	do_mpacquaint,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpaecho",	do_mpaecho,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpasound",	do_mpasound,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpat",		do_mpat,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpcancel",	do_mpcancel,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpclose",	do_mpclose,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpdamage",	do_mpdamage,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpdelay",	do_mpdelay,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpecho",		do_mpecho,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpechoaround",	do_mpechoaround,POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpechoat",	do_mpechoat,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpforce",	do_mpforce,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpforget",	do_mpforget,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpgive",		do_mpgive,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpgoto",		do_mpgoto,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpgtransfer",	do_mpgtransfer,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpjunk",		do_mpjunk,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpkill",		do_mpkill,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mplock",		do_mplock,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpmload",	do_mpmload,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpoload",	do_mpoload,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpopen",		do_mpopen,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mppeace",	do_mppeace,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mppurge",	do_mppurge,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpremember",	do_mpremember,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpsecho",	do_mpsecho,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpsechoaround",	do_mpsechoaround,POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpsechoat",	do_mpsechoat,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpsecret",	do_mpsecret,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpsetpos",	do_mpsetpos,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mptake",		do_mptake,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpteleport",	do_mpteleport,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mptransfer",	do_mptransfer,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpunhide",	do_mpunhide,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },
    { "mpunlock",	do_mpunlock,	POS_DEAD,	 1,  LOG_NORMAL, CMD_MOB | CMD_NOLIST },

    /*
     * de/re level
     */
    {"delevel",               do_delevel,         POS_DEAD,           L8, LOG_NORMAL, 1},
    {"relevel",	         do_relevel,	        POS_DEAD,           1,  LOG_ALWAYS, 1},
    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL, 0 }
};




/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void
interpret( CHAR_DATA *ch, char *argument )
{
    int			cmd;
    char		command[MAX_INPUT_LENGTH];
    double		elapsed;
    bool		found;
    char		logline[MAX_INPUT_LENGTH];
    char                *message;
    struct timeval	start;
    struct timeval	stop;
    int			trust;

    /*
     * Stop up residual falling
     */
    ch->fall_count = 0;

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace(*argument) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	&&   !str_prefix( command, cmd_table[cmd].name )
//	&&   ( IS_NPC( ch ) || !IS_SET( cmd_table[cmd].flags, CMD_MOB ) )
//	&&   cmd_table[cmd].level <= trust )
	&&   can_use_cmd( ch, cmd ) )
	{
	    found = TRUE;
	    break;
	}
    }


    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||	 ( cmd_table[cmd].log == LOG_ARGS && *argument != '\0' )
    ||   cmd_table[cmd].log == LOG_ALWAYS )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	wiznet(" Log: $N: $t", ch, logline, WIZ_SECURE, 0, get_trust( ch ) );
	log_string( log_buf );
    }

    if ( cmd_table[cmd].log == LOG_BUILD )
    {
	sprintf( log_buf, "Build: %s: %s", ch->name, logline );
	wiznet( "$t", ch, log_buf, WIZ_BUILD, 0, get_trust( ch ) );
	log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

// if ( !check_social( ch, command, argument ) )
 if ( !found )                                                                                                                 
    {                                                                                                                               
        if ( rprog_interp_trigger( ch->in_room, ch, logline ) )                                                                     
             return;                                                                                                                
                                                                                                                                    
        if ( check_social( ch, command, argument ) )                                                                                
            return;                    
                switch (number_range(0,20)) 
                { 
                default : message = NULL;
                        break;
                case 0: send_to_char("Huh?\n\r",ch);
                        break;
                case 1: send_to_char("That's not a real thing, you know.\n\r",ch);
                        break;
                case 2: send_to_char("I don't understand....\n\r",ch);
                        break;
                case 3: send_to_char("Ain't gonna happen..\n\r",ch);
                        break;
                case 4: send_to_char("You wish.\n\r",ch);
                        break;  
                case 5: send_to_char("NO!\n\r",ch);
                        break;
                case 6: send_to_char("That's not a command.. Type 'help' or 'commands'.\n\r",ch);
                        break;
                case 7: send_to_char("Not right now.. Try again in an hour.\n\r",ch);
                        break;
                case 8: send_to_char("Are you dense? That's not possible.\n\r",ch);
                        break;
                case 9: send_to_char("Nope Nope Nope Nope.\n\r",ch);
                        break;
                case 10: send_to_char("Why would you even type such a thing?\n\r",ch);
                        break;
                case 11: send_to_char("How silly, thinking that's a command!\n\r",ch);
                        break;
                case 12: send_to_char("Nuh-uh.\n\r",ch);
                        break;
                case 13: send_to_char("Narp.\n\r",ch);
                        break;
                case 14: send_to_char("Negative, Ghost Rider. The pattern is full.\n\r",ch);
                        break;
                case 15: send_to_char("Not today, Satan.\n\r",ch);
                        break;
                case 16: send_to_char("That may have been a command, once. But it isn't now.\n\r",ch);
                        break;
                case 17: send_to_char("What?!\n\r",ch);
                        break;
                case 18: send_to_char("h'Wha?.\n\r",ch);
                        break;
                case 19: send_to_char("Yes. Wait a minute... no.\n\r",ch);
                        break;
                case 20: send_to_char("Try typing 'help' or 'commands', because that's neither a command nor social.\n\r",ch);
                        break;
                }
                send_to_char( message, ch ); 
        return;
}
/*    if ( !found )
    {
	if ( rprog_interp_trigger( ch->in_room, ch, logline ) )
	     return;

	if ( check_social( ch, command, argument ) )
	    return;

        send_to_char( "Huh?\n\r", ch );
	return;
    } */

    if ( IS_SET( cmd_table[cmd].flags, CMD_DISABLED )
    &&	 str_cmp( cmd_table[cmd].name, "cmdedit" ) )
    {
	send_to_char( "That command has been disabled by the gods.\n\r", ch );
	return;
    }

    if ( IS_SET( cmd_table[cmd].flags, CMD_EXACT )
    &&   str_cmp( command, cmd_table[cmd].name ) )
    {
	ch_printf( ch, "If you want to %s, spell it out!\n\r",
		   all_capitalize( cmd_table[cmd].name ) );
	return;
    }

    if ( IS_SET( cmd_table[cmd].flags, CMD_DEAD )
    && IS_DEAD( ch )
    && ch->level < MAX_LEVEL )
    {
        send_to_char( "Spirits are unable to do that.\n\r", ch );
        return;
    }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r", ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r", ch);
	    break;

	case POS_SITTING:
	    send_to_char( "Better stand up first.\n\r",ch);
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r", ch);
	    break;

	}
	return;
    }

    /*
     * Dispatch the command.
     */
    if ( ++recursion_counter >= MAX_RECURSION )
    {
	recursion_counter--;
	bugf( "Excessive command recursion: %s", ch->name );
	return;
    }

    /*
     * No hiding, if applicable.
     */
    if ( IS_SET( cmd_table[cmd].flags, CMD_UNHIDE ) )
        xREMOVE_BIT( ch->affected_by, AFF_HIDE );

    gettimeofday( &start, NULL );
    (*cmd_table[cmd].do_fun) ( ch, argument );
    gettimeofday( &stop, NULL );
    if ( stop.tv_sec > start.tv_sec
    || ( stop.tv_sec == start.tv_sec && stop.tv_usec > start.tv_usec ) )
    {
	elapsed = ( ( stop.tv_sec - start.tv_sec ) * 1000000.0 + ( stop.tv_usec - start.tv_usec ) ) / 1000000.0;
	if ( elapsed > cmd_table[cmd].max_time )
	    cmd_table[cmd].max_time = elapsed;
	if ( cmd_table[cmd].min_time == 0 || cmd_table[cmd].min_time > elapsed )
	    cmd_table[cmd].min_time = elapsed;
	cmd_table[cmd].total_time += elapsed;
	cmd_table[cmd].last_time = elapsed;
    }
    cmd_table[cmd].usage++;
    if ( IS_NPC( ch ) )
        cmd_table[cmd].mob_usage++;
    cmd_table[cmd].last_used = current_time;
    recursion_counter--;

    tail_chain( );
    return;
}


/* function to keep argument safe in all commands -- no static strings */
void
do_function( CHAR_DATA *ch, DO_FUN *do_fun, char *argument )
{
    char *command_string;

    /* copy the string */
    command_string = str_dup(argument);

    /* dispatch the command */
    (*do_fun) (ch, command_string);

    /* free the string */
    free_string(command_string);
}


bool
can_use_cmd( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *	rch;
    int		trust;
    bool	has_group;

    if ( !IS_NPC( ch ) && IS_SET( cmd_table[cmd].flags, CMD_MOB ) )
	return FALSE;

    if ( ch->clan == NULL && IS_SET( cmd_table[cmd].flags, CMD_CLAN ) )
	return FALSE;

    trust = get_trust( ch );

    if ( !IS_NPC( ch ) )
    {
        if ( ch->level >= MAX_LEVEL )
            return TRUE;
        if ( is_exact_name( cmd_table[cmd].name, ch->pcdata->detract ) )
            return FALSE;
        if ( is_exact_name( cmd_table[cmd].name, ch->pcdata->empower ) )
            return TRUE;
    }

    if ( cmd_table[cmd].sec_flags != 0 )
    {
	if ( trust >= MAX_LEVEL )
	    return TRUE;
	rch = ch->desc != NULL && ch->desc->original != NULL ? ch->desc->original : ch;
	if ( trust < LEVEL_IMMORTAL )
	    return FALSE;

        has_group = ( cmd_table[cmd].sec_flags & rch->pcdata->sec_groups ) != 0;

        if ( IS_SET( cmd_table[cmd].flags, CMD_BOTH ) )
            return trust >= cmd_table[cmd].level && has_group;
        else
            return trust >= cmd_table[cmd].level || has_group;
    }

    return ( cmd_table[cmd].level <= trust );
}


bool
check_social( CHAR_DATA *ch, char *command, char *argument )
{
    SOCIAL_DATA *pSocial;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    for ( pSocial = social_first; pSocial; pSocial = pSocial->next )
    {
	if ( !pSocial->deleted && !str_prefix( command, pSocial->name ) )
	    break;
    }

    if ( pSocial == NULL )
	return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r",             ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too badly for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r",      ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	if ( !str_cmp( pSocial->name, "snore" ) )
	{
	    break;
	}
	send_to_char( "In your dreams, or what?\n\r",             ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	xREMOVE_BIT( ch->affected_by, AFF_HIDE );
	act_color( AT_SOCIAL, pSocial->cnoarg, ch, NULL, victim, TO_CHAR, POS_RESTING );
	act_color( AT_SOCIAL, pSocial->onoarg, ch, NULL, victim, TO_ROOM, POS_RESTING );
    }
    else if ( !( victim = get_char_world( ch, arg ) ) )
    {
	act_color( AT_SOCIAL, pSocial->nfound, ch, NULL, NULL, TO_CHAR, POS_RESTING );
    }
    else if ( victim == ch )
    {
	xREMOVE_BIT( ch->affected_by, AFF_HIDE );
	act_color( AT_SOCIAL, pSocial->cself, ch, NULL, victim, TO_CHAR, POS_RESTING );
	act_color( AT_SOCIAL, pSocial->oself, ch, NULL, victim, TO_ROOM, POS_RESTING );
    }
    else if ( get_char_room( ch, arg ) == NULL && can_see( ch, victim ) )
    {
        if ( !IS_NPC( victim ) && !IS_DEAD( ch ) && !IS_DEAD( victim ) )
        {
            ROOM_INDEX_DATA *	original;
            OBJ_DATA *		on;
            char *		ldbase = "From far away, ";
            char		ldmsg [ MAX_STRING_LENGTH ];

            original = ch->in_room;
            on = ch->on;
            char_from_room( ch );
            char_to_room( ch, victim->in_room );

            strcpy( ldmsg, ldbase );
            strcat( ldmsg, IS_NULLSTR( pSocial->cfound ) ? pSocial->cnoarg : pSocial->cfound );
            act_color( AT_SOCIAL, ldmsg, ch, NULL, victim, TO_CHAR, POS_RESTING );

	    if ( !is_ignoring( victim, ch, IGNORE_CHANNELS ) )
	    {
	        strcpy( ldmsg, ldbase );
	        strcat( ldmsg, IS_NULLSTR( pSocial->vfound ) ? pSocial->onoarg : pSocial->vfound );
                act_color( AT_SOCIAL, ldmsg, ch, NULL, victim, TO_VICT, POS_RESTING );
            }

            char_from_room( ch );
            char_to_room( ch, original );
            ch->on = on;
        }
        else
        {
            act_color( AT_SOCIAL, pSocial->nfound, ch, NULL, NULL, TO_CHAR, POS_RESTING );
        }
    }
    else
    {
	xREMOVE_BIT( ch->affected_by, AFF_HIDE );
	act_color( AT_SOCIAL, pSocial->cfound, ch, NULL, victim, TO_CHAR, POS_RESTING);
	act_color( AT_SOCIAL, pSocial->vfound, ch, NULL, victim, TO_VICT, POS_RESTING );
	act_color( AT_SOCIAL, pSocial->ofound, ch, NULL, victim, TO_NOTVICT, POS_RESTING );

	if ( !IS_NPC( ch )
	    && IS_NPC( victim )
	    && !IS_AFFECTED( victim, AFF_CHARM )
	    && IS_AWAKE( victim )
	    && !IS_SET( victim->pIndexData->progtypes, MP_ACT_PROG ) )
	{
	    switch ( number_bits( 4 ) )
	    {
	    case 0:
	        if ( ( victim->level < ch->level )
	        && victim->pIndexData->pShop == NULL
	        && !IS_SET( victim->act, ACT_GAIN )
	        && !IS_SET( victim->act, ACT_TRAIN )
	        && !IS_SET( victim->act, ACT_PRACTICE )
	        && !IS_SET( victim->act, ACT_ARTIFICER )
	        && !IS_SET( victim->act, ACT_IS_HEALER )
	        && !IS_SET( victim->act, ACT_IS_CHANGER )
	        && victim->spec_fun != spec_cast_adept
	        && victim->spec_fun != spec_questmaster
                && !( victim->fighting ) )
		    multi_hit( victim, ch, TYPE_UNDEFINED );
		break;

	    case 1: case 2: case 3: case 4:
	    case 5: case 6: case 7: case 8:
		act_color( AT_SOCIAL, pSocial->cfound,
		    victim, NULL, ch, TO_CHAR, POS_RESTING );
		act_color( AT_SOCIAL, pSocial->vfound,
		    victim, NULL, ch, TO_VICT, POS_RESTING );
		act_color( AT_SOCIAL, pSocial->ofound,
		    victim, NULL, ch, TO_NOTVICT, POS_RESTING );
		break;

	    case 9: case 10: case 11: case 12:
		act_color( AT_SOCIAL, "You slap $N.",  victim, NULL, ch, TO_CHAR,POS_RESTING );
		act_color( AT_SOCIAL, "$n slaps you.", victim, NULL, ch, TO_VICT, POS_RESTING );
		act_color( AT_SOCIAL, "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT, POS_RESTING );
		break;
	    }
	}
    }

    return TRUE;
}


/*
 * Extract a money string suitable for passing to money_value().
 * Returns pointer to first non_whitespace char after the extracted
 * string.  Similarities to one_argument().
 */
char *
get_money_string( char *argument, char *buf )
{
    char *	argptr;
    char *	p;
    char *	q;
    bool	fGold;
    bool	fSilver;
    bool	fCopper;
    bool	fFract;
    char	temp[MAX_INPUT_LENGTH];
    bool	valid;

    if ( argument == NULL )
	return NULL;

    if ( *argument == '\0' || buf == NULL )
	return argument;

    while ( isspace( *argument ) )
	argument++;

    *buf = 0;

    fGold   = FALSE;
    fSilver = FALSE;
    fCopper = FALSE;
    fFract  = FALSE;
    for ( ; ; )
    {
	while ( isspace( *argument ) )
	    argument++;

	if ( !isdigit( *argument ) )
	    return argument;

	argptr = argument;
	p = temp;
	valid = FALSE;

	while ( isdigit( *argument ) )
	    *p++ = *argument++;
	*p = '\0';
	while ( isspace( *argument ) )
	    argument++;
	q = p;
	if ( !isalpha( *argument ) )
	    return argptr;
	while ( isalpha( *argument ) )
	    *p++ = *argument++;
	*p = '\0';
	if ( !str_prefix( q, GOLD_NOUN ) && !fGold )
	    valid = fGold = TRUE;
	else if ( !str_prefix( q, GOLD_PLURAL ) && !fGold )
	    valid = fGold = TRUE;
	else if ( !str_prefix( q, SILVER_NOUN ) && !fSilver )
	    valid = fSilver = TRUE;
	else if ( !str_prefix( q, SILVER_PLURAL ) && !fSilver )
	    valid = fSilver = TRUE;
	else if ( !str_prefix( q, COPPER_NOUN ) && !fCopper )
	    valid = fCopper = TRUE;
	else if ( !str_prefix( q, COPPER_PLURAL ) && !fCopper )
	    valid = fCopper = TRUE;
	else if ( !str_prefix( q, FRACT_NOUN ) && !fFract )
	    valid = fFract = TRUE;
	else if ( !str_prefix( q, FRACT_PLURAL ) && !fFract )
	    valid = fFract = TRUE;
	if ( !valid )
	    return argptr;
	strcat( buf, temp );
    }
}


/*
 * Return true if an argument is completely numeric.
 */
bool
is_number ( const char *arg )
{
    if ( *arg == '\0' )
        return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit( *arg ) )
            return FALSE;
    }

    return TRUE;
}


/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int
number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}


/*
 * Given a string like 14*foo, return 14 and 'foo'
*/
int
mult_argument(char *argument, char *arg)
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '*' )
        {
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '*';
            strcpy( arg, pdot+1 );
            return number;
        }
    }

    strcpy( arg, argument );
    return 1;
}


/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *
one_argument( const char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	if ( is_colcode( argument ) )
	{
	    *arg_first++ = *argument++;
	    *arg_first   = *argument;
	}
	else if ( *argument == '`' && *(argument + 1) == '`' )
	{
	    *arg_first++ = *argument++;
	    *arg_first   = *argument;
	}
	else
	    *arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return (char *)argument;
}

/*
 * Contributed by Alander.
 */
void
do_commands( CHAR_DATA *ch, char *argument )
{
    BUFFER *pBuf;
    int cmd;
    int col;

    col = 0;
    pBuf = new_buf( );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level <  LEVEL_HERO
        &&   can_use_cmd( ch, cmd )
	&&   !IS_SET( cmd_table[cmd].flags, CMD_NOLIST ) )
	{
	    buf_printf( pBuf, "%-12s", cmd_table[cmd].name );
	    if ( ++col % 6 == 0 )
		add_buf( pBuf, "\n\r" );
	}
    }

    if ( col % 6 != 0 )
	add_buf( pBuf, "\n\r" );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_wizhelp( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    char *	arglist;
    char	arg[MAX_INPUT_LENGTH];
    int		cmd;
    int		col;
    int		lcmd;
    int         line;
    int         lvl;
    int		ucmd;
    bool        fAll;
    bool	found;

    arglist = argument;
    arglist = one_argument( arglist, arg );

    if ( !str_cmp( arg, "syntax" ) || !str_cmp( arg, "help" ) )
    {
	send_to_char( "Syntax: wizhelp           "
		      "Show all available imm commands.\n\r", ch );
	send_to_char( "        wizhelp <n>       "
		      "Show all level <n> commands.\n\r", ch );
	send_to_char( "        wizhelp <n1> <n2> "
		      "Show all commands between levels n1 and n2.\n\r", ch );
	send_to_char( "        wizhelp <cmd>     "
		      "Show level of <cmd>.\n\r", ch );
	return;
    }
    fAll = FALSE;
    if ( !str_cmp( arg, "all" ) )
    {
        fAll = TRUE;
        arglist = one_argument( arglist, arg );
    }
    if ( arg[0] == '\0' )
    {
	lcmd = fAll ? LEVEL_HERO : LEVEL_IMMORTAL;
	ucmd = MAX_LEVEL;
//	ucmd = get_trust( ch );
    }
    else if ( is_number( arg ) )
    {
	lcmd = atoi( arg );
	if ( *arglist == '\0' )
	    ucmd = lcmd;
	else if ( is_number( arglist ) )
	    ucmd = atoi( arglist );
	else
	{
	    do_function( ch, do_wizhelp, "syntax" );
	    return;
	}
    }
    else
    {
	pBuf = new_buf( );
	found = FALSE;
	while ( arg[0] != '\0' )
	{
	    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	    {
	        if ( cmd_table[cmd].level >= LEVEL_HERO
	        &&   can_use_cmd( ch, cmd )
	        &&   !str_prefix( arg, cmd_table[cmd].name )
	        &&   ( !IS_SET( cmd_table[cmd].flags, CMD_NOLIST )
	             || fAll ) )
		{
		    found = TRUE;
		    buf_printf( pBuf, "%12s is a level %d command.\n\r",
			        capitalize( cmd_table[cmd].name ),
			        cmd_table[cmd].level );
		}
	    }
	    arglist = one_argument( arglist, arg );
	}
	if ( found )
	    page_to_char( buf_string( pBuf ), ch );
	else
	    send_to_char( "No commands found.\n\r", ch );
	free_buf( pBuf );
	return;
    }

    if ( lcmd > ucmd )
    {
        int tmp;
        tmp = ucmd;
        ucmd = lcmd;
        lcmd = tmp;
    }

    pBuf = new_buf( );
    found = FALSE;
    col = 0;
    for ( lvl = ucmd; lvl >= lcmd; lvl-- )
    {
        line = 0;
        if ( col != 0 )
        {
            col = 0;
            add_buf( pBuf, "\n\r" );
        }
        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
        {
            if ( can_use_cmd( ch, cmd )
            &&   cmd_table[cmd].level == lvl
            &&   ( !IS_SET( cmd_table[cmd].flags, CMD_NOLIST )
                 || fAll ) )
            {
                found = TRUE;
                if ( col == 0 )
                {
                    col = 1;
                    if ( line == 0 )
                        buf_printf( pBuf, "`CLevel `W%3d`c:  ", lvl );
                    else
                        add_buf( pBuf, "            " );
                }
                buf_printf( pBuf, "`%c%-12s",
                            col & 1 ? 'W' : 'B',
                            cmd_table[cmd].name );
                if ( ++col % 6 == 0 )
                {
                    col = 0;
                    line++;
                    add_buf( pBuf, "\n\r" );
                }
            }
        }
    }

    if ( col != 0 )
	add_buf( pBuf, "\n\r" );
    if ( found )
	page_to_char( buf_string( pBuf ), ch );
    else
	send_to_char( "No commands found.\n\r", ch );
    free_buf( pBuf );
    return;
}


int
get_cmd_by_fun( DO_FUN *fun )
{
    int index;

    if ( fun == NULL )
	return NO_COMMAND;

    for ( index = 0; cmd_table[index].name[0] != '\0'; index++ )
	if ( cmd_table[index].do_fun == fun )
	    return index;

    return NO_COMMAND;
}


int get_cmd_by_name( const char *command )
{
    int index;

    if ( command == NULL || *command == '\0' )
	return NO_COMMAND;

    for ( index = 0; cmd_table[index].name[0] != '\0'; index++ )
	if ( !str_prefix( command, cmd_table[index].name ) )
	    return index;

    return NO_COMMAND;
}


#include "olc.h"
void
do_rnum( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    ROOM_INDEX_DATA *	pRoom;
    char		buf[MAX_INPUT_LENGTH];
    int			col;
    int			vnum;
    bool		found;

    if ( *argument == '\0' )
    {
	if ( ch->in_room )
	    pArea = ch->in_room->area;
	else
	    pArea = NULL;
    }

    else if ( is_number( argument ) )
    {
	vnum = atoi( argument );
	if ( ( pArea = get_area_data( vnum ) ) == NULL )
	{
	    for ( pArea = area_first; pArea; pArea = pArea->next )
	    {
		if ( pArea->min_vnum <= vnum && pArea->max_vnum >= vnum )
		    break;
	    }
	}
    }
    else
    {
	for ( pArea = area_first; pArea; pArea = pArea->next )
	    if ( is_name( argument, pArea->name ) )
		break;
	if ( pArea == NULL )
	{
	    pRoom = find_location( ch, argument );
	    if ( pRoom )
		pArea = pRoom->area;
	}
    }

    if ( pArea == NULL )
    {
	send_to_char( "Area not found.\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    col	 = 0;

    found = ( pArea->room_list != NULL );

    for ( pRoom = pArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
    {
        strip_color( buf, pRoom->name );
	buf_printf( pBuf, "`R[`W%5d`R]%s`w%-17.16s", pRoom->vnum,
			!IS_SET( pRoom->area->area_flags, AREA_NO_UNFINISHED )
			&& IS_SET( pRoom->room_flags, ROOM_UNFINISHED )
			? "`B*" : " ",
			buf );
	if ( ++col % 3 == 0 )
	    add_buf( pBuf, "\n\r" );
    }

    if ( col % 3 )
	add_buf( pBuf, "\n\r" );

    if ( !found )
	add_buf( pBuf, "No rooms found in that range.\n\r" );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}

void 
do_played_age( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch;
    char buf[MAX_STRING_LENGTH];
	sprintf(buf, "You are %d years old, played %d hours.\n\r",get_age(ch), (int)(ch->played)/3600);
	send_to_char(buf,ch);
	sprintf(buf, "You have connected to MARS %d times.\n\r",ch->pcdata->timesplayed);
	send_to_char(buf,ch);
    return;
}
