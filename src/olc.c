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




#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"


struct olc_help_type
{
    char *	command;
    int		editor;
    const void *structure;
    char *	desc;
};

/*
 * Local functions
 */
static	void	show_cmdpos_cmds args(( CHAR_DATA *ch ) );
static	void	show_color_cmds	args( ( CHAR_DATA *ch ) );
static	void	show_flag_cmds	args( ( CHAR_DATA *ch,
				const struct flag_type *flag_table ) );
static	void	show_grp_list	args( ( CHAR_DATA *ch ) );
static	void	show_liq_cmds	args( ( CHAR_DATA *ch ) );
static	void	show_race_cmds	args( ( CHAR_DATA *ch ) );
static	void	show_skill_cmds	args( ( CHAR_DATA *ch ) );
static	void	show_spell_cmds	args( ( CHAR_DATA *ch, int tar ) );
static	void	show_spec_cmds	args( ( CHAR_DATA *ch ) );


const struct olc_cmd_type aedit_table[] =
{
    {	"age",		aedit_age		},
    {	"allset",	aedit_allset		},
    {	"builder",	aedit_builder		},
    {	"circle",	aedit_circle		},
/*  {	"color",	aedit_color		}, */
    {	"commands",	show_commands		},
    {	"continent",	aedit_continent		},
    {	"created",	aedit_created		},
    {	"credits",	aedit_credits		},
    {	"exitsize",	aedit_exitsize		},
    {	"filename",	aedit_file		},
    {	"kingdom",	aedit_kingdom		},
    {	"level",	aedit_level		},
    {	"line",		aedit_line		},
    {	"lvnum",	aedit_lvnum		},
    {	"makelinks",	edit_makelinks		},
    {	"mark",		aedit_mark		},
    {	"name",		aedit_name		},
    {	"norecall",	aedit_norecall		},
    {	"overlands",	aedit_overland		},
    {	"recall",	aedit_recall		},
    {	"resetmsg",	aedit_resetmsg		},
    {	"rooms",	aedit_rooms		},
    {	"security",	aedit_security		},
    {	"setmobs",	aedit_setmobs		},
    {	"setobjs",	aedit_setobjs		},
    {	"show",		aedit_show		},
    {	"stamp",	aedit_stamp		},
    {	"travel",	aedit_travel		},
    {	"uvnum",	aedit_uvnum		},
    {	"vnum",		aedit_vnum		},
    {	"version",	show_version		},
    {	"world",	aedit_world		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type bedit_table[] =
{
    {	"commands",	show_commands		},
    {	"description",	bedit_desc		},
    {	"expire",	bedit_expire		},
    {	"read",		bedit_read		},
    {	"rename",	bedit_rename		},
    {	"show",		bedit_show		},
    {	"version",	show_version		},
    {	"write",	bedit_write		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type cedit_table[] =
{
    {	"box",		cedit_box		},
//  {	"clan",		cedit_clan		},
    {	"commands",	show_commands		},
//  {	"deadly",	cedit_deadly		},
    {	"description",	cedit_desc		},
    {	"ftitle",	cedit_ftitle		},
    {	"induct",	cedit_induct		},
    {	"leader",	cedit_leader		},
//  {	"losses",	cedit_losses		},
    {	"members",	cedit_members		},
    {	"morgue",	cedit_morgue		},
    {	"mtitle",	cedit_mtitle		},
    {	"name",		cedit_name		},
    {	"outcast",	cedit_outcast		},
//  {	"peaceful",	cedit_peaceful		},
    {	"recall",	cedit_recall		},
    {	"title",	cedit_title		},
    {	"type",		cedit_type		},
    {	"version",	show_version		},
//  {	"wins",		cedit_wins		},
    {	"withdraw",	cedit_withdraw		},
    {	"",		NULL			}
};

const struct olc_cmd_type cmdedit_table[] =
{
    {	"add",		cmdedit_add		},
    {	"commands",	show_commands		},
    {	"remove",	cmdedit_remove		},
    {	"show",		cmdedit_show		},
    {	"version",	show_version		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type dreamedit_table[] =
{
    {	"add",		dreamedit_add		},
    {	"addaffect",	dreamedit_addaffect	},
    {	"author",	dreamedit_author	},
    {	"class",	dreamedit_class		},
    {	"commands",	show_commands		},
    {	"delaffect",	dreamedit_delaffect	},
    {	"delete",	dreamedit_delete	},
    {	"edit",		dreamedit_edit		},
    {	"insert",	dreamedit_insert	},
    {	"race",		dreamedit_race		},
//    {	"remove",	dreamedit_remove	},
    {	"show",		dreamedit_show		},
    {	"title",	dreamedit_title		},
    {	"version",	show_version		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type epedit_table[] =
{
    {	"arglist",	rpedit_arglist		},
    {	"comlist",	rpedit_comlist		},
    {	"commands",	show_commands		},
    {	"show",		epedit_show		},
    {	"version",	show_version		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type gedit_table[] =
{
    {	"add",		gedit_add		},
    {	"commands",	show_commands		},
    {	"drop",		gedit_drop		},
    {	"show",		gedit_show		},
    {	"version",	show_version		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type hedit_table[] =
{
    {	"area",		hedit_area		},
    {   "commands",	show_commands		},
    {   "delet",	hedit_delet		},
    {   "delete",	hedit_delete		},
    {	"edlevel",	hedit_edlevel		},
    {	"import",	hedit_import		},
    {   "keyword",	hedit_name		},
    {   "name",		hedit_name		},
    {   "level",	hedit_level		},
    {   "show",		hedit_show		},
    {   "desc",		hedit_desc		},
    {   "text",		hedit_desc		},
    {   "version",      show_version            },
    {   "?",            show_help               },

    {   "",		0,			}
};

const struct olc_cmd_type medit_table[] =
{
    {	"ac",		medit_ac		},
    {	"alignment",	medit_align		},
    {	"automob",	medit_automob		},
    {	"clan",		medit_clan		},
    {	"commands",	show_commands		},
    {	"copy",		medit_copy		},
    {	"damage",	medit_damage		},
    {	"damtype",	medit_damtype		},
    {	"delete",	medit_delete		},
    {	"description",	medit_desc		},
    {   "ethos",         medit_ethos     },
    {	"exec",		exec_command		},
    {	"form",		medit_form		},
    {	"group",	medit_group		},
    {	"hitdice",	medit_hitdice		},
    {	"hitroll",	medit_hitroll		},
    {	"immune",	medit_immune		},
    {	"level",	medit_level		},
    {	"long",		medit_long		},
    {	"manadice",	medit_mana		},
    {	"mpedit",	medit_mpedit		},
    {	"mplist",	medit_mplist		},
    {	"name",		medit_name		},
    {	"mpremove",	medit_mpremove		},
    {	"mpswap",	medit_mpswap		},
    {	"offensive",	medit_offensive		},
    {	"parts",	medit_parts		},
    {	"position",	medit_position		},
    {	"resistant",	medit_resistant		},
    {	"shop",		medit_shop		},
    {	"short",	medit_short		},
    {	"show",		medit_show		},
    {	"size",		medit_size		},
    {	"special",	medit_special		},
    {	"teaches",	medit_train		},
    {   "version",	show_version		},
    {	"vulnerable",	medit_vulnerable	},
    {	"wealth",	medit_wealth		},
    {	"?",		show_help		},

    {	"",		0,			}
};

const struct olc_cmd_type mpedit_table[] =
{
    {   "arglist",      mpedit_arglist,         },
    {   "comlist",      mpedit_comlist,         },
    {   "commands",     show_commands,          },
    {   "version",      show_version,           },
    {   "?",            show_help,              },
    {   "",             0,                      }
};

const struct olc_cmd_type oedit_table[] =
{
    {	"addaffect",	oedit_addaffect		},
    {	"anti",		oedit_anti		},
    {	"autoset",	oedit_autoset		},
    {	"autoweapon",	oedit_autoweapon		},
    {	"autoarmor",	oedit_autoarmor		},
    {	"commands",	show_commands		},
    {	"condition",	oedit_condition		},
    {	"copy",		oedit_copy		},
    {	"cost",		oedit_cost		},
    {   "create",	oedit_create		},
    {	"delaffect",	oedit_delaffect		},
    {	"delete",	oedit_delete		},
    {	"description",	oedit_description	},
    {	"duration",	oedit_duration		},
    {	"ed",		oedit_ed		},
    {	"exec",		exec_command		},
    {	"invoke",	oedit_invoke		},
    {	"join",		oedit_join		},
    {	"level",	oedit_level		},
    {	"long",		oedit_long		},
    {	"material",	oedit_material		},
    {	"name",		oedit_name		},
    {	"opedit",	oedit_opedit		},
    {	"oplist",	oedit_oplist		},
    {	"opremove",	oedit_opremove		},
    {	"page",		oedit_page		},
    {   "qcost",    oedit_qcost     },
    {	"sepone",	oedit_sepone		},
    {	"septwo",	oedit_septwo		},
    {	"short",	oedit_short		},
    {	"show",		oedit_show		},
    {	"timer",	oedit_timer		},
    {	"v0",		oedit_value0		},
    {	"v1",		oedit_value1		},
    {	"v2",		oedit_value2		},
    {	"v3",		oedit_value3		},
    {	"v4",		oedit_value4		},
    {	"v5",		oedit_value5		},
    {	"version",	show_version		},
    {	"weight",	oedit_weight		},
    {	"?",		show_help		},
    {	"",		0,			}
};

const struct olc_cmd_type opedit_table[] =
{
    {	"arglist",	opedit_arglist		},
    {	"comlist",	opedit_comlist		},
    {	"commands",	show_commands		},
    {	"version",	show_version		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type pedit_table[] =
{
    {	"assign",	pedit_assign		},
    {	"completed",	pedit_complete		},
    {	"commands",	show_commands		},
    {	"deadline",	pedit_deadline		},
    {	"delete",	pedit_delete		},
    {	"description",	pedit_description	},
    {	"due",		pedit_deadline		},
    {	"name",		pedit_name		},
    {	"priority",	pedit_priority		},
    {	"progress",	pedit_progress		},
    {	"show",		pedit_show		},
    {	"status",	pedit_status		},
    {	"undelete",	pedit_undelete		},
    {	"version",	show_version,		},
    {	"",		NULL			}
};

const struct olc_cmd_type racedit_table[] =
{
    {	"act",		racedit_act		},
    {	"affect",	racedit_affect		},
    {	"alignment",	racedit_align		},
    {	"allclass",	racedit_allclass	},
    {	"commands",	show_commands		},
    {	"form",		racedit_form		},
    {	"immune",	racedit_immune		},
    {	"maxstat",	racedit_maxstat		},
    {	"offensive",	racedit_offensive	},
    {	"parts",	racedit_parts		},
    {	"pcrace",	racedit_pcrace		},
    {	"points",	racedit_points		},
    {	"resist",	racedit_resist		},
    {	"show",		racedit_show		},
    {	"size",		racedit_size		},
    {	"skill",	racedit_skill		},
    {	"startstat",	racedit_startstat	},
    {	"version",	show_version		},
    {	"vulnerable",	racedit_vulnerable	},
    {	"weight",	racedit_weight		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type redit_table[] =
{
    {	"commands",	show_commands		},
    {	"affect",	redit_affect		},
    {	"copy",		redit_copy,		},
    {	"create",	redit_create		},
    {	"delete",	redit_delete		},
    {	"description",	redit_desc		},
    {	"down",		redit_down		},
    {	"east",		redit_east		},
    {	"ed",		redit_ed		},
    {	"edesc",	redit_edesc		},
    {	"exec",		exec_command		},
    {	"heal",		redit_heal		},
    {	"makelinks",	edit_makelinks		},
    {	"mana",		redit_mana		},
    {	"mdesc",	redit_mdesc		},
    {	"north",	redit_north		},
    {	"name",		redit_name		},
    {	"ndesc",	redit_ndesc		},
    {	"neast",	redit_northeast		},
    {	"northeast",	redit_northeast		},
    {	"northwest",	redit_northwest		},
    {	"nwest",	redit_northwest		},
    {	"rpedit",	redit_rpedit		},
    {	"rpremove",	redit_rpremove		},
    {	"smell",	redit_smell		},
    {	"sound",	redit_sound		},
    {	"south",	redit_south		},
    {	"short",	redit_short		},
    {	"show",		redit_show		},
    {	"seast",	redit_southeast		},
    {	"southeast",	redit_southeast		},
    {	"southwest",	redit_southwest		},
    {	"swest",	redit_southwest		},
    {	"up",		redit_up		},
    {	"vehicle",	redit_vehicle		},
    {	"version",	show_version		},
    {	"west",		redit_west		},
    {	"walk",		exec_command		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type rename_obj_table[] =
{
    {	"commands",	show_commands		},
    {	"done",		rename_obj_done		},
    {	"extra",	rename_obj_extra	},
    {	"keyword",	rename_obj_keyword	},
    {	"long",		rename_obj_long		},
    {	"short",	rename_obj_short	},
    {	"show",		rename_obj_show		},
    {	"",		NULL			}
};

const struct olc_cmd_type rpedit_table[] =
{
    {	"arglist",	rpedit_arglist		},
    {	"comlist",	rpedit_comlist		},
    {	"commands",	show_commands		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type secedit_table[] =
{
    {	"add",		secedit_add		},
    {	"create",	secedit_create		},
    {	"delete",	secedit_delete		},
    {	"edit",		secedit_edit		},
    {	"list",		secedit_list		},
    {	"remove",	secedit_remove		},
    {	"rename",	secedit_rename		},
    {	"show",		secedit_show		},
    {	"commands",	show_commands		},
    {	"version",	show_version		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type sedit_table[] =
{
    {	"level",	sedit_edlevel		},
    {	"cnoarg",	sedit_cnoarg		},
    {	"onoarg",	sedit_onoarg		},
    {	"cfound",	sedit_cfound		},
    {	"ofound",	sedit_ofound		},
    {	"vfound",	sedit_vfound		},
    {	"nfound",	sedit_nfound		},
    {	"cself",	sedit_cself		},
    {	"oself",	sedit_oself		},
    {	"delet",	sedit_delet		},
    {	"delete",	sedit_delete		},
    {	"show",		sedit_show		},
    {	"undelete",	sedit_undelete		},
    {   "commands",	show_commands,		},
    {	"version",	show_version		},
    {	"",		NULL			}
};

const struct olc_cmd_type skedit_table[] =
{
    {	"dammsg",	skedit_dammsg		},
    {	"delay",	skedit_delay		},
    {	"forget",	skedit_forget		},
    {	"mana",		skedit_mana		},
    {	"objmsg",	skedit_objmsg		},
    {	"prerequisite",	skedit_prereq		},
    {	"roommsg",	skedit_roommsg		},
    {	"show",		skedit_show		},
    {	"wearoff",	skedit_offmsg		},
    {   "commands",	show_commands,		},
    {	"version",	show_version		},
    {	"?",		show_help		},
    {	"",		NULL			}
};

const struct olc_cmd_type travedit_table[] =
{
    {	"afixed",	travedit_afixed		},
    {	"atravel",	travedit_atravel	},
    {	"dfixed",	travedit_dfixed		},
    {	"dtravel",	travedit_dtravel	},
    {	"commands",	show_commands		},
    {	"movetime",	travedit_movetime	},
    {	"room",		travedit_room		},
    {	"stoptime",	travedit_stoptime	},
    {	"show",		travedit_show		},
    {	"version",	show_version		},
    {	"walk",		exec_command		},
    {	"",		NULL			}
};


/*
 * This table contains help commands and a brief description of each.
 * ------------------------------------------------------------------
 */
const struct olc_help_type help_table[] =
{
    {	"act",		ED_MOBILE,	act_flags,	 "Mobile attributes."		},
    {	"affect",	ED_MOBILE,	affect_flags,	 "Mobile affects."		},
    {	"apply",	ED_OBJECT,	apply_types,	 "Object/dream affects."	},
    {	"apply",	ED_DREAM,	apply_types,	 "Object/dream affects."	},
    {	"area",		ED_AREA,	area_flags,	 "Area attributes."		},
    {	"books",	ED_OBJECT,	book_flags,	 "Book flags."			},
    {	"classes",	ED_MOBILE,	class_flags,	 "Classes for mobiles."		},
    {	"cmd",		ED_CMD,		cmd_flags,	 "Command flags."		},
    {	"cmdpos",	ED_CMD,		position_table,	 "Positions for commands"	},
    {	"colors",	ED_ROOM,	color_types,	 "Room colors."			},
    {	"container",	ED_OBJECT,	container_flags, "Container status."		},
    {	"continent",	ED_AREA,	continent_types, "Continents."			},
    {	"dreamflag",	ED_DREAM,	dream_flags,	 "Dream flags."			},
    {	"dreamtype",	ED_DREAM,	dream_types,	 "Dream types."			},
    {   "eprogs",	ED_EPROG,	eprog_types,	 "Types of ExitProgs."		},
    {	"exit",		ED_ROOM,	exit_flags,	 "Exit types."			},
    {	"extra",	ED_OBJECT,	extra_flags,	 "Object attributes."		},
    {	"extra2",	ED_OBJECT,	extra_flags2,	 "More Object attributes."		},
    {	"form",		ED_MOBILE,	form_flags,	 "Mobile body form."		},
    {	"furniture",	ED_OBJECT,	furniture_flags, "Sit/rest/sleep/stand."	}, 
    {	"groups",	ED_CMD,		group_table,	 "Skill/spell groups."		}, 
    {	"herbs",	ED_OBJECT,	herb_flags,	 "Herb poisons."		},
    {   "immune",	ED_MOBILE,	imm_flags,	 "Types of immunities."		},
    {	"invoke",	ED_OBJECT,	invoke_types,	 "Types of object invokes."	},
    {	"kingdoms",	ED_AREA,	kingdom_types,	 "Kingdoms."			},
    {	"liquid",	ED_OBJECT,	liq_table,	 "Types of liquids."		},
    {	"log",		ED_CMD,		log_types,	 "Command logging."		},
    {	"material",	ED_OBJECT,	material_types,	 "Material obj is made from."	},
    {   "mprogs",	ED_MPROG,	mprog_types,	 "Types of MobProgs."		},
    {	"offensive",	ED_MOBILE,	off_flags,	 "Mobile offensive behavior."	},
    {   "oprogs",	ED_OPROG,	oprog_types,	 "Types of ObjProgs."		},
    {	"parts",	ED_MOBILE,	part_flags,	 "Mobile body parts."		},
    {	"portals",	ED_OBJECT,	portal_flags,	 "Portal flags."		},
    {	"position",	ED_MOBILE,	position_types,	 "Mobile positions."		},
    {	"races",	ED_MOBILE,	race_table,	 "Mobile races."		},
    {	"resistance",	ED_MOBILE,	res_flags,	 "Mobile resistance."		},
    {	"room",		ED_ROOM,	room_flags,	 "Room attributes."		},
    {	"room",		ED_AREA,	room_flags,	 "Room attributes."		},
    {	"roomaff",	ED_ROOM,	room_aff_flags,	 "Room affects.",		},
    {   "rprogs",	ED_RPROG,	rprog_types,	 "Types of RoomProgs."		},
    {	"sector",	ED_ROOM,	sector_types,	 "Sector types, terrain."	},
    {	"sector",	ED_AREA,	sector_types,	 "Sector types, terrain."	},
    {	"sex",		ED_MOBILE,	sex_types,	 "Sexes."			},
    {	"sheath",	ED_OBJECT,	sheath_flags,	 "Sheath flags."		},
    {	"shields",	ED_MOBILE,	shield_types,	 "Shields"			},
    {	"size",		ED_MOBILE,	size_types,	 "Mobile size."			},
    {	"skills",	ED_SKILL,	&skill_table[1], "Names of current skills.",	},
    {	"spec",		ED_OBJECT,	spec_table,	 "Available special programs." 	},
    {	"spells",	ED_OBJECT,	skill_table,	 "Names of current spells." 	},
    {	"spellflags",	ED_SKILL,	spell_flags,	 "Names of spell flags." 	},
    {	"type",		ED_OBJECT,	item_types,	 "Types of objects."		},
    {	"vehicles",	ED_ROOM,	vehicle_types,	 "Types of vehicles."		},
    {	"vulnerability",ED_MOBILE,	vuln_flags,	 "Mobile vulnerabilities."	},
    {	"weapon",	ED_OBJECT,	weapon_flags,	 "Type of weapon." 		},
    {	"weapon-class",	ED_OBJECT,	weapon_types,	 "Class of weapon"		},
    {	"weapon-spec",	ED_OBJECT,	weapon_type_olc, "Special weapon flags",	},
    {	"wear",		ED_OBJECT,	wear_flags,	 "Where to wear object."	},
    {	"wear-loc",	ED_ROOM,	wear_loc_types,	 "Where mobile wears object."	},
    {	"worlds",	ED_AREA,	world_types,	 "World an area belongs to."	},
    {	"",		0,		0,		 ""				}
};

const struct editor_cmd_type editor_table[] =
{
/*  {   command		function	}, */

    {	"area",		do_aedit	},
    {	"board",	do_bedit	},
    {	"clan",		do_cedit	},
    {	"cmd",		do_cmdedit	},
    {	"command",	do_cmdedit	},
    {	"dream",	do_dreamedit	},
    {	"group",	do_gedit	},
    {	"help",		do_hedit	},
    {	"mobile",	do_medit	},
    {	"object",	do_oedit	},
    {	"project",	do_pedit	},
    {	"race",		do_racedit	},
    {	"room",		do_redit	},
    {	"security",	do_secedit	},
    {	"social",	do_sedit	},
    {	"skill",	do_skedit	},

    {	"",		0,		}
};


/*****************************************************************************
 Name:		can_aedit_all
 Purpose:	Returns TRUE if builder can edit critical area_data fields.
 Called by:	aedit_xxx()
 ****************************************************************************/
bool
can_aedit_all( CHAR_DATA *ch, AREA_DATA *pArea )
{
    if ( get_trust( ch ) >= IMPLEMENTOR || IS_SET( ch->act2, HBUILDER ) )
    {
	return TRUE;
    }
    return FALSE;
}




/*****************************************************************************
 Name:		do_edit
 Purpose:	Entry point for editors
 Called by:	interpret() (interp.c)
 ****************************************************************************/
void
do_edit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    DO_FUN *	do_fun;
    int		cmd;
    int		index;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Edit what?\n\r", ch );
	return;
    }

    do_fun = NULL;
    for ( index = 0; editor_table[index].name[0] != '\0'; index++ )
    {
	if ( !str_prefix( arg, editor_table[index].name ) )
	{
	    do_fun = editor_table[index].do_fun;
	    break;
	}
    }

    cmd = get_cmd_by_fun( do_fun );
    if ( cmd == NO_COMMAND || !can_use_cmd( ch, cmd ) )
    {
	send_to_char( "You cannot edit that.\n\r", ch );
	return;
    }

    do_function( ch, do_fun, argument );
    return;
}


/*****************************************************************************
 Name:		edit_done
 Purpose:	Resets builder information on completion.
 Called by:	all core edit functions (aedit, redit, etc)
 ****************************************************************************/
bool
edit_done( CHAR_DATA *ch )
{
    /* one level of edit nesting */
    if ( ch->desc->editin || ch->desc->inEdit )
    {
	ch->desc->pEdit = ch->desc->inEdit;
	ch->desc->inEdit = NULL;
	ch->desc->editor = ch->desc->editin;
	ch->desc->editin = ED_NONE;
    }
    else
    {
	ch->desc->pEdit = NULL;
	ch->desc->editor = 0;
    }
    ch->pcdata->etimer = 0;

    send_to_char( "Ok.\n\r", ch );

    return FALSE;
}


/*****************************************************************************
 Name:		exec_command
 Purpose:	Executes a command through the normal interpreter.
 ****************************************************************************/
bool
exec_command( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if ( *argument == '\0' )
    {
	send_to_char( "Exec what?\n\r", ch );
	return FALSE;
    }
    strcpy( arg, argument );
    interpret( ch, arg );
    return FALSE;
}


/*****************************************************************************
 Name:		get_area_data
 Purpose:	Returns pointer to area with given vnum.
 ****************************************************************************/
AREA_DATA *
get_area_data( int vnum )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        if ( pArea->vnum == vnum )
            return pArea;
    }

    return 0;
}


/*****************************************************************************
 Name:		get_area_edit
 Purpose:	Returns pointer to area containing what's being edited
 ****************************************************************************/
AREA_DATA *
get_area_edit( CHAR_DATA *ch )
{
    HELP_DATA *pHelp;

    if ( IS_NPC( ch ) )
	return NULL;

    switch( ch->desc->editor )
    {
	case ED_AREA:
	    return (AREA_DATA *)ch->desc->pEdit;
        case ED_EPROG:
            return ch->in_room->area;
	case ED_HELP:
	    pHelp = (HELP_DATA *)ch->desc->pEdit;
	    return pHelp->area ? pHelp->area : ch->in_room->area;
	    break;
	case ED_MOBILE:
	    return ( (MOB_INDEX_DATA *)ch->desc->pEdit )->area;
	case ED_MPROG:
	    return ( (MOB_INDEX_DATA *)ch->desc->inEdit )->area;
	case ED_OBJECT:
	    return ( (OBJ_INDEX_DATA *)ch->desc->pEdit )->area;
	case ED_OPROG:
	    return ( (OBJ_INDEX_DATA *)ch->desc->inEdit )->area;
	case ED_ROOM:
	    return ch->in_room->area;
	case ED_RPROG:
	    return ch->in_room->area;
	case ED_TRAVEL:
	    return (AREA_DATA *)ch->desc->inEdit;
	default:
	    return NULL;
    }
}


/* Executed from comm.c.  Minimizes compiling when changes are made. */
bool
run_olc_editor( DESCRIPTOR_DATA *d, char *argument  )
{
    CHAR_DATA * ch;
    int		trust;
    int		cmd;
    char	command[MAX_INPUT_LENGTH];
    char *	arglist;

    if ( d == NULL )
	return FALSE;

    ch = d->original ? d->original : d->character;

    while( isspace( *argument ) )
	argument++;

    if ( !isalpha( argument[0] ) && !isdigit( argument[0] ) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	arglist = argument + 1;
	while ( isspace( *arglist ) )
	    arglist++;
    }
    else
    {
	arglist = one_argument( argument, command );
    }

    trust = get_trust( ch );

    /*
     * Short circuit OLC if a common normal command.
     * Probably need to seriously revamp this part someday.
     */
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	    && !str_prefix( command, cmd_table[cmd].name )
	    && ( ( cmd_table[cmd].level <= trust ) 
	    || ( IS_CODER( ch ) ) ) )
	{
	    break;
	}
    }

    if (    cmd_table[cmd].do_fun == do_say
//	 || cmd_table[cmd].do_fun == do_chat
//	 || cmd_table[cmd].do_fun == do_ooc
         || ( cmd_table[cmd].do_fun == do_north	&& *arglist == '\0' )
         || ( cmd_table[cmd].do_fun == do_south && *arglist == '\0' )
         || ( cmd_table[cmd].do_fun == do_east	&& *arglist == '\0' )
         || ( cmd_table[cmd].do_fun == do_west	&& *arglist == '\0' )
         || ( cmd_table[cmd].do_fun == do_up	&& *arglist == '\0' )
         || ( cmd_table[cmd].do_fun == do_down	&& *arglist == '\0' )
         || cmd_table[cmd].do_fun == do_look
	 || cmd_table[cmd].do_fun == do_immtalk )
    {
	return FALSE;
    }

    switch ( d->editor )
    {
    case ED_AREA:
	aedit( d->character, argument );
	break;
    case ED_BOARD:
	bedit( d->character, argument );
	break;
    case ED_CLAN:
	cedit( d->character, argument );
	break;
    case ED_CMD:
	cmdedit( d->character, argument );
	break;
    case ED_DREAM:
        dreamedit( d->character, argument );
        break;
    case ED_EPROG:
        epedit( d->character, argument );
        break;
    case ED_GROUP:
	gedit( d->character, argument );
	break;
    case ED_HELP:
	hedit( d->character, argument );
	break;
    case ED_MOBILE:
	medit( d->character, argument );
	break;
    case ED_MPROG:
	mpedit( d->character, argument );
	break;
    case ED_OBJECT:
	oedit( d->character, argument );
	break;
    case ED_OPROG:
	opedit( d->character, argument );
	break;
    case ED_PROJECT:
	pedit( d->character, argument );
	break;
    case ED_RACE:
	racedit( d->character, argument );
	break;
    case ED_ROOM:
	redit( d->character, argument );
	break;
    case ED_RPROG:
	rpedit( d->character, argument );
	break;
    case ED_SECURITY:
	secedit( d->character, argument );
	break;
    case ED_SOCIAL:
	sedit( d->character, argument );
	break;
    case ED_SKILL:
	skedit( d->character, argument );
	break;
    case ED_TRAVEL:
	travedit( d->character, argument );
	break;
    case RENAME_OBJECT:
	rename_obj( d->character, argument );
	break;
    default:
	return FALSE;
    }

    /* Mobs shouldn't be able to get into an editor, but check anyway */
    if ( ch && !IS_NPC( ch ) )
    {
	ch->pcdata->etimer = 0;
    }
    return TRUE;
}


const char *
olc_ed_name( CHAR_DATA *ch )
{
    switch (ch->desc->editor)
    {
	case ED_AREA:		return " AEdit ";
	case ED_BOARD:		return " BEdit ";
	case ED_CLAN:		return " CEdit ";
	case ED_CMD:		return " CmdEdit ";
	case ED_DREAM:		return " DreamEdit ";
	case ED_EPROG:		return " EPEedit ";
	case ED_GROUP:		return " GEdit ";
	case ED_HELP:		return " HEdit ";
	case ED_MOBILE:		return " MEdit ";
	case ED_MPROG:		return " MPEdit ";
	case ED_OBJECT:		return " OEdit ";
	case ED_OPROG:		return " OPEdit ";
	case ED_PROJECT:	return " PEdit ";
	case ED_RACE:		return " RaEdit ";
	case ED_ROOM:		return " REdit ";
	case ED_RPROG:		return " RPEdit ";
	case ED_SECURITY:	return " SecEdit ";
	case ED_SKILL:		return " SkEdit ";
	case ED_SOCIAL:		return " SEdit ";
	case ED_TRAVEL:		return " TravEdit ";
	case RENAME_OBJECT:	return " Rename Obj ";
	default:		return "";
    }
}


const char *
olc_ed_vnum( CHAR_DATA *ch )
{
    AREA_DATA *		pArea;
    BOARD_DATA *	pBoard;
    CLAN_DATA *		pClan;
    DREAM_DATA *	pDream;
    HELP_DATA *		pHelp;
    MOB_INDEX_DATA *	pMob;
    OBJ_INDEX_DATA *	pObj;
    OBJ_DATA *		obj;
    PROJECT_DATA *	pProj;
    ROOM_INDEX_DATA *	pRoom;
    SOCIAL_DATA *	pSocial;
    TRAVEL_DATA *	pTravel;
    int			sn;
    static char		buf[MAX_INPUT_LENGTH];

    buf[0] = '\0';
    switch ( ch->desc->editor )
    {
    case ED_AREA:
	pArea = (AREA_DATA *)ch->desc->pEdit;
	sprintf( buf, "%s", pArea ? pArea->name : "----" );
	if ( IS_SET( ch->act2, PLR_MARKING ) )
	    strcat( buf, "(MARKING)" );
	break;
    case ED_BOARD:
	pBoard = (BOARD_DATA *)ch->desc->pEdit;
	sprintf( buf, "%s", pBoard->name );
	buf[0] = UPPER( buf[0] );
	break;
    case ED_CLAN:
	pClan = (CLAN_DATA *)ch->desc->pEdit;
	sprintf( buf, "%s", pClan ? pClan->name : "(none)" );
	buf[0] = UPPER( buf[0] );
	break;
    case ED_CMD:
	sn = (int)ch->desc->pEdit;
	strcpy( buf, cmd_table[sn].name );
	break;
    case ED_DREAM:
	pDream = (DREAM_DATA *)ch->desc->pEdit;
	sprintf( buf, "%d", pDream->vnum );
	break;
    case ED_EPROG:
        sn = get_eprog_dir( ch->in_room, (OREPROG_DATA *)ch->desc->pEdit );
        sprintf( buf, "%d %s", ch->in_room->vnum,
                 sn == DIR_NONE ? "???" : capitalize( dir_name[sn] ) );
        break;
    case ED_GROUP:
	sn = (int)ch->desc->pEdit;
	strcpy( buf, group_table[sn].name );
	break;
    case ED_HELP:
	pHelp = (HELP_DATA *)ch->desc->pEdit;
	sprintf( buf, "%s", pHelp->keyword );
	break;
    case ED_MOBILE:
	pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
	sprintf( buf, "%d", pMob ? pMob->vnum : 0 );
	break;
    case ED_MPROG:
	pMob = (MOB_INDEX_DATA *)ch->desc->inEdit;
	sprintf( buf, "%d", pMob ? pMob->vnum : 0 );
	break;
    case ED_OBJECT:
	pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit;
	sprintf( buf, "%d", pObj ? pObj->vnum : 0 );
	break;
    case ED_OPROG:
	pObj = (OBJ_INDEX_DATA *)ch->desc->inEdit;
	sprintf( buf, "%d", pObj ? pObj->vnum : 0 );
	break;
    case ED_PROJECT:
	pProj = (PROJECT_DATA *)ch->desc->pEdit;
	sprintf( buf, "%d", pProj ? pProj->vnum : 0 );
	break;
    case ED_RACE:
	sn = (int)ch->desc->pEdit;
	strcpy( buf, race_table[sn].name );
	break;
    case ED_ROOM:
	pRoom = ch->in_room;
	sprintf( buf, "%d", pRoom ? pRoom->vnum : 0 );
	break;
    case ED_RPROG:
	pRoom = ch->in_room;
	sprintf( buf, "%d", pRoom ? pRoom->vnum : 0 );
	break;
    case ED_SECURITY:
	sn = (int)ch->desc->pEdit;
	strcpy( buf, ( sn < 0 ) ? "(none)" : security_flags[sn].name );
	break;
    case ED_SKILL:
	sn = (int)ch->desc->pEdit;
	strcpy( buf, skill_table[sn].name );
	break;
    case ED_SOCIAL:
	pSocial = (SOCIAL_DATA *)ch->desc->pEdit;
	strcpy( buf, pSocial ? pSocial->name : "(none)" );
	break;
    case ED_TRAVEL:
	pTravel = (TRAVEL_DATA *)ch->desc->pEdit;
	sprintf( buf, "%d", pTravel->room_vnum );
	break;
    case RENAME_OBJECT:
	obj = (OBJ_DATA *)ch->desc->pEdit;
	strcpy( buf, obj->short_descr );
	break;
    default:
	strcpy( buf, "" );
	break;
    }

    return buf;
}


/*****************************************************************************
 Name:		read_desc_file
 Purpose:	Reads in a description file.
 Called by:	hedit_import, redit_import
 Returns:	0: success
 		1: error
 		2: can't open file
 ****************************************************************************/
int
read_desc_file( CHAR_DATA *ch, char *buf, char *filename, char *fext,
		bool fOpen )
{
    char	buf1[ MAX_INPUT_LENGTH ];
    char	fname[ MAX_INPUT_LENGTH ];
    char	fullname[ MAX_INPUT_LENGTH ];
    int		c;
    int		incount;
    int		outcount;
    int		retval;     /* unused & ignored; only here to keep GCC happy */
    char	*p;
    FILE	*fp;

    p = filename;
    while ( *p != '\0' )
    {
	if ( !isalnum( *p )
        &&   *p != '_'
        &&   *p != '-' )
	{
	    ch_printf( ch, "Bad character in filename: %c\n\r", *p );
	    return DESC_BAD;
	}
	p++;
    }

    strcpy( fullname, UPLOAD_DIR );
    strcpy( fname, filename );
    strcat( fname, "." );
    strcat( fname, fext );
    strcat( fullname, fname );

    fclose( fpReserve );
    if ( ( fp = fopen( fullname, "r" ) ) == NULL )
    {
	fpReserve = fopen( NULL_FILE, "r" );
	if ( fOpen )
	{
	    ch_printf( ch, "Can't open file %s\n\r", fname );
	}
	return DESC_MISSING;
    }

    incount = outcount = 0;
    p = buf;
    while ( ( c = getc( fp ) ) != EOF )
    {
	incount++;
	if ( c == '\r' )
	{
	    continue;
	}
	if ( ( c < ' ' && !isspace( c ) ) || ( c > 127 ) )
	{
	    fclose( fp );
	    fpReserve = fopen( NULL_FILE, "r" );
	    ch_printf( ch, "Bad character in %s at offset %d.\n\r", fname, incount );
	    sprintf( buf1, "rm -f %s", fullname );
	    retval = system( buf1 );
	    return DESC_BAD;
	}
	if ( outcount > ( MAX_STRING_LENGTH - MAX_INPUT_LENGTH ) )
	{
	    ch_printf( ch, "File %s too long: truncated at offset %d\n\r", fname, incount );
	    break;
	}
	*p++ = c;
	outcount++;
	if ( c == '\n' )
	{
	    *p++ = '\r';
	    outcount++;
	}
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    sprintf( buf1, "rm -f %s", fullname );
    retval = system( buf1 );
    if ( p == buf )
    {
	ch_printf( ch, "Empty file %s.\n\r", fname );
	return DESC_BAD;
    }
    if ( p > buf && *(p-1) != '\n' && *(p-1) != '\r' )
    {
        *p++ = '\n';
        *p++ = '\r';
    }
    *p = '\0';
    smash_tilde( buf );

    return SUCCESS;
}


/*****************************************************************************
 Name:		show_olc_cmds
 Purpose:	Format up the commands from given table.
 Called by:	show_commands(olc_act.c).
 ****************************************************************************/
void
show_olc_cmds( CHAR_DATA *ch, const struct olc_cmd_type *olc_table )
{
    char buf  [ MAX_STRING_LENGTH ];
    char *p;
    int  cmd;
    int  col;

    buf[0] = '\0';
    col = 0;
    p = buf;

    for ( cmd = 0; olc_table[cmd].name[0] != '\0'; cmd++ )
    {
	p += sprintf( p, "%-15.15s", olc_table[cmd].name );
	if ( ++col % 5 == 0 )
	    p = stpcpy( p, "\n\r" );
    }

    if ( col % 5 != 0 )
	p = stpcpy( p, "\n\r" );

    send_to_char( buf, ch );
    return;
}


/*****************************************************************************
 Name:		show_commands
 Purpose:	Display all olc commands.
 Called by:	olc interpreters.
 ****************************************************************************/
bool
show_commands( CHAR_DATA *ch, char *argument )
{
    switch (ch->desc->editor)
    {
	case ED_AREA:
	    show_olc_cmds( ch, aedit_table );
	    break;
	case ED_BOARD:
	    show_olc_cmds( ch, bedit_table );
	    break;
	case ED_CLAN:
	    show_olc_cmds( ch, cedit_table );
	    break;
	case ED_CMD:
	    show_olc_cmds( ch, cmdedit_table );
	    break;
	case ED_DREAM:
	    show_olc_cmds( ch, dreamedit_table );
	    break;
	case ED_EPROG:
	    show_olc_cmds( ch, epedit_table );
	    break;
	case ED_GROUP:
	    show_olc_cmds( ch, gedit_table );
	    break;
	case ED_HELP:
	    show_olc_cmds( ch, hedit_table );
	    break;
	case ED_MOBILE:
	    show_olc_cmds( ch, medit_table );
	    break;
	case ED_MPROG:
	    show_olc_cmds( ch, mpedit_table );
	    break;
	case ED_OBJECT:
	    show_olc_cmds( ch, oedit_table );
	    break;
	case ED_OPROG:
	    show_olc_cmds( ch, opedit_table );
	    break;
	case ED_PROJECT:
	    show_olc_cmds( ch, pedit_table );
	    break;
	case ED_RACE:
	    show_olc_cmds( ch, racedit_table );
	    break;
	case ED_ROOM:
	    show_olc_cmds( ch, redit_table );
	    break;
	case ED_RPROG:
	    show_olc_cmds( ch, rpedit_table );
	    break;
	case ED_SKILL:
	    show_olc_cmds( ch, skedit_table );
	    break;
	case ED_SOCIAL:
	    show_olc_cmds( ch, sedit_table );
	    break;
	case ED_SECURITY:
	    show_olc_cmds( ch, secedit_table );
	    break;
	case ED_TRAVEL:
	    show_olc_cmds( ch, travedit_table );
	    break;
	case RENAME_OBJECT:
	    show_olc_cmds( ch, rename_obj_table );
	    break;
//	case FORGE_OBJECT:
//	    show_olc_cmds( ch, forge_obj_table );
//	    break;
    }

    return FALSE;
}


/*****************************************************************************
 Name:		show_flag_cmds
 Purpose:	Displays settable flags and stats.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table )
{
    BUFFER *	pBuf;
    int		flag;
    int		col;

    col = 0;
    pBuf = new_buf( );
    for ( flag = 0; flag_table[flag].name != NULL; flag++ )
    {
	if ( flag_table[flag].settable )
	{
	    buf_printf( pBuf, "%-19.18s", flag_table[flag].name );
	    if ( ++col % 4 == 0 )
		add_buf( pBuf, "\n\r" );
	}
    }

    if ( col % 4 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


/*****************************************************************************
 Name:		show_help
 Purpose:	Displays help for many tables used in OLC.
 Called by:	olc interpreters.
 ****************************************************************************/
bool
show_help( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    char	arg[MAX_INPUT_LENGTH];
    char	spell[MAX_INPUT_LENGTH];
    int		cnt;
    bool	fAll;

    argument = one_argument( argument, arg );
    one_argument( argument, spell );

    /*
     * Display syntax.
     */
    fAll = !str_cmp( arg, "all" );
    if ( arg[0] == '\0' || fAll )
    {
	pBuf = new_buf( );
	add_buf( pBuf, "Syntax:  ? [command]\n\r\n\r" );
	add_buf( pBuf, "[command]  [description]\n\r" );
	for ( cnt = 0; help_table[cnt].command[0] != '\0'; cnt++ )
	{
	    if ( !fAll && help_table[cnt].editor != ED_NONE && help_table[cnt].editor != ch->desc->editor )
		continue;	
	    if ( cnt > 0 && fAll && help_table[cnt].structure == help_table[cnt - 1].structure )
	        continue;
	    buf_printf( pBuf, "%-10.10s -%s\n\r",
	        capitalize( help_table[cnt].command ),
		help_table[cnt].desc );
	}
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return FALSE;
    }

    /*
     * Find the command, show changeable data.
     * ---------------------------------------
     */
    for ( cnt = 0; *help_table[cnt].command; cnt++ )
    {
        if (  arg[0] == help_table[cnt].command[0]
          && !str_prefix( arg, help_table[cnt].command ) )
	{
	    if ( help_table[cnt].structure == color_types )
	    {
		show_color_cmds( ch );
		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == group_table )
	    {
		show_grp_list( ch );
		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == liq_table )
	    {
		show_liq_cmds( ch );
		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == position_table )
	    {
		show_cmdpos_cmds( ch );
		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == race_table )
	    {
		show_race_cmds( ch );
		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == spec_table )
	    {
		show_spec_cmds( ch );
		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == skill_table )
	    {

		if ( spell[0] == '\0' )
		{
		    send_to_char( "Syntax:  ? spells "
		        "[ignore/attack/defend/self/object/all]\n\r", ch );
		    return FALSE;
		}

		if ( !str_prefix( spell, "all" ) )
		    show_spell_cmds( ch, -1 );
		else if ( !str_prefix( spell, "ignore" ) )
		    show_spell_cmds( ch, TAR_IGNORE );
		else if ( !str_prefix( spell, "attack" ) )
		    show_spell_cmds( ch, TAR_CHAR_OFFENSIVE );
		else if ( !str_prefix( spell, "defend" ) )
		    show_spell_cmds( ch, TAR_CHAR_DEFENSIVE );
		else if ( !str_prefix( spell, "self" ) )
		    show_spell_cmds( ch, TAR_CHAR_SELF );
		else if ( !str_prefix( spell, "object" ) )
		    show_spell_cmds( ch, TAR_OBJ_INV );
		else
		    send_to_char( "Syntax:  ? spell "
		        "[ignore/attack/defend/self/object/all]\n\r", ch );

		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == &skill_table[1] )
	    /* Cheesy way of differentiating between spells and skills */
	    {
		show_skill_cmds( ch );
		return FALSE;
	    }
	    else
	    {
		show_flag_cmds( ch, help_table[cnt].structure );
		return FALSE;
	    }
	}
    }

    show_help( ch, "" );
    return FALSE;
}


/*****************************************************************************
 Name:		show_color_cmds
 Purpose:	Displays available room description colors.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_color_cmds( CHAR_DATA *ch )
{
    /* quick hack, been coding for hours and I'm tired. */
    send_to_char( "`rdarkred\n\r", ch );
    send_to_char( "`Rbrightred\n\r", ch );
    send_to_char( "`gdarkgreen\n\r", ch );
    send_to_char( "`Gbrightgreen\n\r", ch );
    send_to_char( "`bdarkblue\n\r", ch );
    send_to_char( "`Blightblue\n\r", ch );
    send_to_char( "`Oorange\n\r", ch );
    send_to_char( "`Yyellow\n\r", ch );
    send_to_char( "`pdarkpurple\n\r", ch );
    send_to_char( "`Plightpurple\n\r", ch );
    send_to_char( "`cdarkcyan\n\r", ch );
    send_to_char( "`Clightcyan\n\r", ch );
    send_to_char( "`wlightgrey\n\r", ch );
    send_to_char( "`Wwhite`X\n\r", ch );
}


/*****************************************************************************
 Name:		show_grp_list
 Purpose:	Displays available skill groups.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_grp_list( CHAR_DATA *ch )
{
    BUFFER *	pBuf;
    int		gn;
    int		col;

    pBuf = new_buf( );
    col = 0;
    for ( gn = 0; gn < MAX_GROUP; gn++ )
    {
	if ( group_table[gn].name && *group_table[gn].name != '\0' )
	{
	    buf_printf( pBuf, "%-19.18s", group_table[gn].name );
	    if ( ++col % 4 == 0 )
		add_buf( pBuf, "\n\r" );
	}
    }
    if ( col % 4 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


/*****************************************************************************
 Name:		show_liq_cmds
 Purpose:	Displays available liquid types.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_liq_cmds( CHAR_DATA *ch )
{
    BUFFER *	pBuf;
    int		liq;
    int		col;

    pBuf = new_buf( );
    col = 0;
    for ( liq = 0; liq_table[liq].liq_name; liq++ )
    {
	buf_printf( pBuf, "%-19.18s", liq_table[liq].liq_name );
	if ( ++col % 4 == 0 )
	    add_buf( pBuf, "\n\r" );
    }

    if ( col % 4 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


/*****************************************************************************
 Name:		show_cmdpos_cmds
 Purpose:	Displays available positions for commands.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_cmdpos_cmds( CHAR_DATA *ch )
{
    BUFFER *	buf;
    int		i;

    buf = new_buf( );
    for ( i = 0; position_table[i].name != NULL; i++ )
    {
	buf_printf( buf, "%s\n", position_table[i].name );
    }
    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


/*****************************************************************************
 Name:		show_race_cmds
 Purpose:	Displays available mobile races.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_race_cmds( CHAR_DATA *ch )
{
    BUFFER *	pBuf;
    int		race;
    int		col;

    pBuf = new_buf( );
    col = 0;
    for ( race = 0; race_table[race].name; race++ )
    {
	buf_printf( pBuf, "%-19.18s", race_table[race].name );
	if ( ++col % 4 == 0 )
	    add_buf( pBuf, "\n\r" );
    }

    if ( col % 4 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


/*****************************************************************************
 Name:		show_skill_cmds
 Purpose:	Displays all skill functions.
 		Could be improved by:
 		(1) Adding a check for a particular class.
 		(2) Adding a check for a level range.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_skill_cmds( CHAR_DATA *ch )
{
    BUFFER *	pBuf;
    int		sn;
    int		col;

    col = 0;
    pBuf = new_buf( );

    for ( sn = 0; skill_table[sn].name != NULL; sn++ )
    {
	if ( skill_table[sn].name[0] == '\0' )
	    break;

	if ( !str_cmp( skill_table[sn].name, "reserved" )
	  || skill_table[sn].spell_fun != spell_null )
	    continue;

	buf_printf( pBuf, "%-19.18s", skill_table[sn].name );
	if ( ++col % 4 == 0 )
	    add_buf( pBuf, "\n\r" );
    }

    if ( col % 4 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


/*****************************************************************************
 Name:		show_spell_cmds
 Purpose:	Displays all spell functions.
 		Does remove those damn immortal commands from the list.
 		Could be improved by:
 		(1) Adding a check for a particular class.
 		(2) Adding a check for a level range.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_spell_cmds( CHAR_DATA *ch, int tar )
{
    BUFFER *		pBuf;
    DESCRIPTOR_DATA *	d;
    OBJ_INDEX_DATA *	pObj;
    bitvector		flags;
    int			sn;
    int			col;

    col = 0;
    pBuf = new_buf( );

    for ( sn = 0; skill_table[sn].name != NULL; sn++ )
    {
	if ( skill_table[sn].name[0] == '\0' )
	    break;

	if ( !str_cmp( skill_table[sn].name, "reserved" )
	  || skill_table[sn].spell_fun == spell_null )
	    continue;

	if ( tar == -1 || skill_table[sn].target == tar )
	{
	    if ( ( d = ch->desc ) != NULL
	    &&	 d->editor == ED_OBJECT
	    &&	 ( pObj = d->pEdit ) != NULL )
	    {
		flags = skill_table[sn].spell_flags;
		switch( pObj->item_type )
		{
		    case ITEM_SCROLL:
			if ( IS_SET( flags, SPELL_NOSCROLL ) )
			    continue;
			break;
		    case ITEM_WAND:
			if ( IS_SET( flags, SPELL_NOWAND ) )
			    continue;
			break;
		    case ITEM_STAFF:
			if ( IS_SET( flags, SPELL_NOSTAFF ) )
			    continue;
			break;
		    case ITEM_POTION:
			if ( IS_SET( flags, SPELL_NOPOTION ) )
			    continue;
			break;
		    case ITEM_PILL:
			if ( IS_SET( flags, SPELL_NOPILL ) )
			    continue;
			break;
		    case ITEM_LENS:
			if ( IS_SET( flags, SPELL_NOLENS ) )
			    continue;
			break;
		    case ITEM_HERB:
			if ( IS_SET( flags, SPELL_NOHERB ) )
			    continue;
			break;
		    default:
			if ( IS_SET( flags, SPELL_NOINVOKE ) )
			    continue;
			break;
		}
	    }
	    buf_printf( pBuf, "%-19.18s", skill_table[sn].name );
	    if ( ++col % 4 == 0 )
		add_buf( pBuf, "\n\r" );
	}
    }

    if ( col % 4 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


/*****************************************************************************
 Name:		show_spec_cmds
 Purpose:	Displays settable special functions.
 Called by:	show_help(olc.c).
 ****************************************************************************/
static void
show_spec_cmds( CHAR_DATA *ch )
{
    BUFFER *	pBuf;
    int		spec;
    int		col;

    pBuf = new_buf( );
    col = 0;
    add_buf( pBuf, "Precede special functions with 'spec_'\n\r\n\r" );
    for ( spec = 0; *spec_table[spec].function; spec++ )
    {
	buf_printf( pBuf, "%-19.18s", &spec_table[spec].name[5] );
	if ( ++col % 4 == 0 )
	    add_buf( pBuf, "\n\r" );
    }

    if ( col % 4 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


bool
show_version( CHAR_DATA *ch, char *argument )
{
    send_to_char( VHEADER, ch );
    send_to_char( VERSION, ch );
    send_to_char( "\n\r", ch );
    send_to_char( AUTHOR DATE, ch );
    send_to_char( "\n\r", ch );
    send_to_char( CREDITS, ch );
    send_to_char( "\n\r", ch );

    return FALSE;
}    

