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
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <mysql/mysql.h>

#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "magic.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"

char *	const	where_name	[] =
{
    "`C<`Wused as light`C>    `X ",
    "`C<`Wworn on finger`C>   `X ",
    "`C<`Wworn on finger`C>   `X ",
    "`C<`Wworn around neck`C> `X ",
    "`C<`Wworn around neck`C> `X ",
    "`C<`Wworn on torso`C>    `X ",
    "`C<`Wworn on head`C>     `X ",
    "`C<`Wworn on legs`C>     `X ",
    "`C<`Wworn on feet`C>     `X ",
    "`C<`Wworn on hands`C>     `X",
    "`C<`Wworn on arms`C>     `X ",
    "`C<`Wworn as shield`C>   `X ",
    "`C<`Wworn about body`C>  `X ",
    "`C<`Wworn about waist`C> `X ",
    "`C<`Wworn around wrist`C>`X ",
    "`C<`Wworn around wrist`C>`X ",
    "`C<`Wwielded`C>          `X ",
    "`C<`Wheld`C>            `X  ",
    "`C<`Wfloating nearby`C> `X  ",
    "`C<`Wdual wielded`C>    `X  ",
    "`C<`Wworn in eyes`C>    `X  ",
    "`C<`Wworn on face`C>    `X  ",
    "`C<`Wworn on ear`C>     `X  ",
    "`C<`Wworn on ear`C>      `X ",
    "`C<`Wclan blazon`C>    `X   ",
    "`C<`Wreligious symbol`C>`X  ",
    "`C<`Wover the shoulder`C>`X ",
    "`C<`Wworn on ankle`C>  `X   ",
    "`C<`Wworn on ankle`C>    `X ",
    "`C<`Wworn on tail`C>   `X   ",
    "`C<`Wworn on horns`C>`X     ",
};

/*
 * This string must be the same length as the wear_name strings.
*/
char *	const	in_sheath =
    " -containing-       ";


struct inventory_list_type
{
    int		type;
    char *	hdr;
};

struct inventory_list_type inventory_table[] =
{
    {	ITEM_WEAPON,	"Weapons"	},
    {	ITEM_QUIVER,	NULL		},
    {	ITEM_SHEATH,	NULL		},

    {	ITEM_ARMOR,	"Apparel",	},
    {	ITEM_JEWELRY,	NULL		},
    {	ITEM_CLOTHING,	NULL		},
    {	ITEM_LENS,	NULL		},

    {	ITEM_PILL,	"Alchemy"	},
    {	ITEM_POTION,	NULL		},
    {	ITEM_HERB,	NULL		},
    {	ITEM_BERRY,	NULL		},

    {	ITEM_STAFF,	"Magic"		},
    {	ITEM_SCROLL,	NULL		},
    {	ITEM_WAND,	NULL		},
    {	ITEM_PORTAL,	NULL		},
    {	ITEM_WARP_STONE,NULL		},

    {	ITEM_LIGHT,	"Misc"		},
    {	ITEM_TREASURE,	NULL		},
    {	ITEM_FURNITURE,	NULL		},
    {	ITEM_TRASH,	NULL		},
    {	ITEM_CONTAINER,	NULL		},
    {	ITEM_DRINK_CON,	NULL		},
    {	ITEM_KEY,	NULL		},
    {	ITEM_FOOD,	NULL		},
    {	ITEM_MONEY,	NULL		},
    {	ITEM_BOAT,	NULL		},
    {	ITEM_CORPSE_NPC,NULL		},
    {	ITEM_CORPSE_PC,	NULL		},
    {	ITEM_FOUNTAIN,	NULL		},
/*  {	ITEM_PROTECT,	NULL		}, */
    {	ITEM_MAP,	NULL		},
/*  {	ITEM_ROOM_KEY,	NULL		}, */
    {	ITEM_GEM,	NULL		},
/*  {	ITEM_JUKEBOX,	NULL		}, */
    {	ITEM_QUESTITEM,	NULL		},
    {	ITEM_NOTEBOARD,	NULL		},
    {	ITEM_PAPER,	NULL		},
    {	ITEM_INK,	NULL		},
    {	ITEM_PERMKEY,	NULL		},
    {	ITEM_BOOK,	NULL		},
    {   ITEM_TOKEN, "Tokens"},

    {	-1,		NULL		}
};


/*
 * Sky stuff
 */
#define SKY_WIDTH	72

const char *sky_map[] =
{
    "   W.X     ' .     :. p,X     :  Y:.,X    `  ,       B,X      .      .  ",
    " W. :.X .      G,X  :p.: .X  :` Y.X    .      :     B:   .X       :     ",
    "    W:X    G.X:       p:.,X:.:   Y`X      ,    c.X           .:    `    ",
    "   W.`:X       '. G.X  `  : ::.      Y.X      c'X      B.X R., ,X       ",
    " W:'  `:X .  G. X    `  :    .Y.X:.          ,     B.X      :  R:   . .X",
    ":' '.   .    G:.X      .'   '   :::.  ,  c.X   :c.X    `        R`.X    ",
    "      :       `        `        :. ::. :     '  :        ,   , R.`:X    ",
    "  ,       G:.X              `Y.X :. ::.c`X      c`.X   '        `      .",
    "     ..        G.:X :           .:   c.X:.    .              .          "
};

#define SKY_HEIGHT	( sizeof( sky_map ) / sizeof( sky_map[0] ) )

const char *sun_map[] =
{
    "\\'|'/",
    "- O -",
    "/.|.\\"
};

/*
 * Local functions.
 */
static	const	char *align_str	args( ( int min, int max ) );
static	char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
static 	char *	show_area	( AREA_DATA *pArea, bool fHere );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch, bool tf ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
int	show_inventory		args( ( CHAR_DATA *ch, int item_type,
					BUFFER *buf, const char *hdr ) );
static	void	show_sky	args( ( CHAR_DATA *ch ) );
void	show_vehicles_to_char	args( ( ROOM_INDEX_DATA *room, CHAR_DATA *ch ) );
void	who_line		args( ( CHAR_DATA *ch, CHAR_DATA *wch, BUFFER *pBuf, bool fClan ) );


static const char *
align_str( int min, int max )
{
    static char	buf[SHORT_STRING_LENGTH];

    strcpy( buf, "   " );

    if ( max >= 350 )
        buf[0] = 'G';
    if ( min < 350 && max > -350 )
        buf[1] = 'N';
    if ( min <= -350 )
        buf[2] = 'E';

    return buf;
}


static char *
format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];
    	   char	buf1[SHORT_STRING_LENGTH];

    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
    ||  (obj->description == NULL || obj->description[0] == '\0'))
	return buf;

    if ( IS_OBJ_STAT( obj, ITEM_INVIS )    )  strcat( buf, "`W(`CInvis`W)`X "     );
    if ( IS_AFFECTED( ch, AFF_DETECT_EVIL  )
         && IS_OBJ_STAT( obj, ITEM_EVIL )  )  strcat( buf, "`W(`RRed Aura`W)`X "  );
    if ( IS_AFFECTED( ch, AFF_DETECT_GOOD  )
    &&  IS_OBJ_STAT( obj,ITEM_BLESS )      )  strcat( buf,"`W(`BBlue Aura`W)`X "  );
    if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC )
         && IS_OBJ_STAT( obj, ITEM_MAGIC ) )  strcat( buf, "`W(`PMagical`W)`X "   );
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)       )  strcat( buf, "`W(`YGlowing`W)`X "   );
    if ( IS_OBJ_STAT( obj, ITEM_HUM )      )  strcat( buf, "`W(`OHumming`W)`X "   );
    if ( IS_OBJ_STAT( obj, ITEM_HIDDEN )   )  strcat( buf, "`W(`zHidden`W)`X "	  );

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( IS_IMMORTAL( ch ) && IS_SET( ch->comm, COMM_VNUMS ) )
	{
	    sprintf( buf1, "[%d]", obj->pIndexData->vnum );
	    strcat( buf, buf1 );
	}
	if ( obj->description != NULL )
	    strcat( buf, obj->description );
    }
    strcat( buf, "`X" );

    return buf;
}


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void
show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort,
                   bool fShowNothing )
{
    char	buf[MAX_STRING_LENGTH];
    BUFFER *	output;
    char **	prgpstrShow;
    int *	prgnShow;
    char *	pstrShow;
    OBJ_DATA *	obj;
    int		nShow;
    int		iShow;
    int		count;
    bool	fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf( );

    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
	if ( IS_SET( obj->extra_flags, ITEM_NOLIST )
	&&   obj->in_room != NULL
	&&   !IS_SET( obj->wear_flags, ITEM_TAKE )
	&&   ( !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
	    continue;

	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ))
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );

	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if (prgpstrShow[iShow][0] == '\0')
	{
	    free_string(prgpstrShow[iShow]);
	    continue;
	}

	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		add_buf(output,buf);
	    }
	    else
	    {
		add_buf(output,"     ");
	    }
	}
	add_buf( output, prgpstrShow[iShow] );
	add_buf( output, "\n\r" );
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC( ch ) || IS_SET( ch->comm, COMM_COMBINE ) )
	    add_buf( output, "     " );
	add_buf( output, "Nothing.\n\r" );
    }
    page_to_char( buf_string( output ), ch );

    /*
     * Clean up.
     */
    free_buf( output );
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}


/* To show chars in room */
void
show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char	buf[MAX_STRING_LENGTH];
    char	message[MAX_STRING_LENGTH];
    char	buf1[SHORT_STRING_LENGTH];
    char *	p;

    buf[0] = '\0';

    set_char_color( C_DEFAULT, ch );
    if ( !IS_NPC( victim )  && victim->desc == NULL )
    {
	if ( IS_SET( victim->act, PLR_SWITCHED ) )
	{
	    if ( get_trust( ch ) >= get_trust( victim ) )
    						strcat( buf, "(Switched) " );
	}
	else if ( IS_SET( victim->act2, PLR_PLOADED ) )
						strcat( buf, "(Ploaded) " );
	else					strcat( buf, "(Linkdead) " );
    }
    if ( RIDDEN( victim ) != NULL )
    {
	if ( ch != RIDDEN( victim ) )
	    strcat( buf, "(Ridden) " );
	else
	    strcat( buf, "(Your mount) " );
    }
    if ( IS_SET(victim->comm,COMM_AFK	  )   ) strcat( buf, "(`WAFK`X) "	     );
    if ( IS_NPC(victim) && ch->questmob > 0
    &&	 victim->pIndexData->vnum == ch->questmob )
						strcat( buf, "(`c[`CT`BA`WRG`BE`CT`c]`X) "     );
    if ( IS_DEAD( victim ) )			strcat( buf, "(Spirit) " );
    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(Invis) "      );
    if ( victim->invis_level >= LEVEL_HERO    )
    {
	sprintf( buf1, "(Wiz%d)", victim->invis_level );
	strcat( buf, buf1 );
    }
    if ( victim->desc && victim->desc->pString != NULL )
	strcat( buf, "`B<`XEditing`B>`X " );
    else if ( victim->desc && victim->desc->editor != ED_NONE )
    {
	switch( victim->desc->editor )
	{
	    default:
		strcat( buf, "`B<`XBuilding`B>`X " ); break;
	    case RENAME_OBJECT:
		strcat( buf, "`B<`XRemaking`B`X> " ); break;
	    case FORGE_OBJECT:
		strcat( buf, "`B<`XForging`B>`X " ); break;
	}
    }
    if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "(Hide) "       );
    if ( IS_AFFECTED(victim, AFF_SNEAK)       ) strcat( buf, "(Sneak) "      );
    if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "(`PCharmed`X) "    );
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "(`cTranslucent`X) ");
    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "(`PPink Aura`X) "  );
    if ( IS_EVIL(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "(`RRed Aura`X) "   );
    if ( IS_GOOD(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_GOOD)     ) strcat( buf, "(`YGolden Aura`X) ");
    if ( IS_SHIELDED( victim, SHLD_SANCTUARY) ) strcat( buf, "(`WWhite Aura`X) " );
    if ( IS_SHIELDED( victim, SHLD_BLACK_MANTLE) ) strcat( buf, "(`zDark Aura`X) " );
    if ( IS_AFFECTED( victim, AFF_PEACE ) )	strcat( buf, "(`GPeaceful`X) " );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER ) )
						strcat( buf, "(`RKILLER`X) "     );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF  ) )
						strcat( buf, "(`RTHIEF`X) "      );
    if ( !IS_NPC( victim ) && IS_SET( victim->act2, PLR_CODING ) )
						strcat( buf, "(`CC`X) "     );
    if ( IS_NPC( victim ) && IS_IMMORTAL( ch ) && IS_SET( ch->comm, COMM_VNUMS ) )
    {
	sprintf( buf1, "[%d]", victim->pIndexData->vnum );
	strcat( buf, buf1 );
    }

    if ( IS_TETHERED( victim ) && !xIS_SET( victim->affected_by, AFF_SEVERED ) )
    {
        char *p;
        p = buf + strlen( buf );
        sprintf( p, "%s `Xis tethered here.\n\r", victim->short_descr );
        while ( is_colcode( p ) )
            p += 2;
        *p = UPPER( *p );
        send_to_char( buf, ch );
        return;
    }

    if ( xIS_SET( victim->affected_by, AFF_SEVERED ) )
    {
        sprintf( buf + strlen( buf ), "The upper torso of %s is here, twitching.\n\r",
                 PERS( victim, ch ) );
        send_to_char( buf, ch );
        return;
    }

    if ( IS_NPC( victim )
    && 	 victim->position == victim->start_pos
    && 	 victim->long_descr[0] != '\0' )
    {
	strcat( buf, victim->long_descr );
	send_to_char( buf, ch );
	return;
    }

    if ( !IS_NPC( victim ) && !knows_char( ch, victim )
    &&	 !IS_NULLSTR( victim->long_descr )
    &&	 ( !IS_SET( victim->act2, PLR_POSE ) || IS_NULLSTR( victim->pcdata->pose ) ) )
    {
	strcat( buf, victim->long_descr );
	send_to_char( buf, ch );
	return;
    }


    p = buf + strlen( buf );
    strcpy( p, PERS( victim, ch ) );
    while ( is_colcode( p ) )
        p += 2;
    *p = UPPER( *p );
    if ( !IS_NPC( victim ) && !IS_NULLSTR( victim->pcdata->pose )
    &&	 IS_SET( victim->act2, PLR_POSE ) )
    {
        if ( !ispunct( *victim->pcdata->pose ) )
            strcat( buf, " " );
        strcat( buf, victim->pcdata->pose );
        strcat( buf, "`X\n\r" );
        send_to_char( buf, ch );
        return;
    }

    if ( !IS_NPC( victim ) && !IS_SET( ch->comm, COMM_BRIEF )
    &&   victim->position == POS_STANDING && ch->on == NULL )
    {
	if ( !IS_NULLSTR( victim->pcdata->lname ) && knows_char( ch, victim ) )
	{
	    strcat( buf, " " );
	    strcat( buf, victim->pcdata->lname );
	}
	strcat( buf, ", " );
	strcat( buf, aoran( race_table[victim->race].name ) );
	strcat( buf, " with " );
    strcat( buf, victim->pcdata->hair_adj);
    strcat( buf, " " );
	strcat( buf, victim->pcdata->hair_color );
	strcat( buf, " and " );
    strcat( buf, victim->pcdata->eye_adj);
    strcat( buf, " " );
	strcat( buf, victim->pcdata->eye_color );
	strcat( buf, " eyes" );
//	strcat( buf, victim->pcdata->title );
    }

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
    case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    case POS_SLEEPING:
	if ( victim->on != NULL )
	{
	    if ( IS_SET( victim->on->value[2], SLEEP_AT ) )
  	    {
		sprintf( message, " is asleep at %s.",
		    victim->on->short_descr );
		strcat( buf, message );
	    }
	    else if ( IS_SET( victim->on->value[2], SLEEP_ON ) )
	    {
		sprintf( message, " is asleep on %s.",
		    victim->on->short_descr );
		strcat( buf, message );
	    }
	    else
	    {
		sprintf( message, " is asleep in %s.",
		    victim->on->short_descr );
		strcat( buf, message );
	    }
	}
	else
	    strcat( buf, " is sleeping here." );
	break;
    case POS_RESTING:
        if ( victim->on != NULL )
	{
            if ( IS_SET( victim->on->value[2], REST_AT ) )
            {
                sprintf( message, " is resting at %s.",
                    victim->on->short_descr );
                strcat( buf, message );
            }
            else if ( IS_SET( victim->on->value[2], REST_ON ) )
            {
                sprintf( message, " is resting on %s.",
                    victim->on->short_descr );
                strcat( buf, message );
            }
            else
            {
                sprintf( message, " is resting in %s.",
                    victim->on->short_descr );
                strcat( buf, message );
            }
	}
        else
	    strcat( buf, " is resting here." );
	break;
    case POS_SITTING:
        if ( victim->on != NULL )
        {
            if ( IS_SET( victim->on->value[2], SIT_AT ) )
            {
                sprintf( message, " is sitting at %s.",
                    victim->on->short_descr );
                strcat( buf, message );
            }
            else if ( IS_SET( victim->on->value[2], SIT_ON ) )
            {
                sprintf( message, " is sitting on %s.",
                    victim->on->short_descr );
                strcat( buf, message );
            }
            else
            {
                sprintf( message, " is sitting in %s.",
                    victim->on->short_descr );
                strcat( buf, message );
            }
        }
        else
	    strcat(buf, " is sitting here.");
	break;
    case POS_STANDING:
	if ( victim->on != NULL )
	{
	    if ( IS_SET( victim->on->value[2], STAND_AT ) )
	    {
		sprintf( message, " is standing at %s.",
		    victim->on->short_descr );
		strcat( buf, message );
	    }
	    else if ( IS_SET( victim->on->value[2], STAND_ON ) )
	    {
		sprintf( message, " is standing on %s.",
		   victim->on->short_descr );
		strcat( buf, message );
	    }
	    else
	    {
		sprintf( message, " is standing in %s.",
		    victim->on->short_descr );
		strcat( buf, message );
	    }
	}
	else if ( MOUNTED( victim ) != NULL )
	{
	    strcat( buf, " is here, riding " );
	    strcat( buf, PERS( MOUNTED( victim ), ch ) );
	    strcat( buf, "`X.");
	}
	else
	    strcat( buf, " is here." );
	break;
    case POS_FIGHTING:
	strcat( buf, " is here, fighting " );
	if ( victim->fighting == NULL )
	    strcat( buf, "thin air??" );
	else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->in_room )
	{
	    strcat( buf, PERS( victim->fighting, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    }

    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}


/* for "look char" */
void
show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch, bool fGlance )
{
    char	buf[MAX_STRING_LENGTH];
    OBJ_DATA *	obj;
    int		index;
    int		iWear;
    int		percent;
    bool	found;
    char *	p;

    if ( can_see( victim, ch ) )
    {
	if( fGlance == FALSE )
	{
	    if ( ch == victim )
	    act_color( AT_ACTION, "$n looks at $mself.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
	    else
    	    {
		act_color( AT_ACTION, "$n looks at you.", ch, NULL, victim, TO_VICT, POS_RESTING    );
		act_color( AT_ACTION, "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	    }
	}
    }

    if ( !IS_NPC( victim ) )
    {
        strcpy( buf, PERS( victim, ch ) );
        if ( !IS_NULLSTR( victim->pcdata->lname ) && knows_char( ch, victim ) )
        {
	    	strcat( buf, " " );
	    	strcat( buf, victim->pcdata->lname );
	}
	if ( knows_char( ch, victim ) )
	{
	    strcat( buf, ", " );
	    strcat( buf, aoran( race_table[victim->race].name ) );
	}
	strcat( buf, " with " );
    strcat( buf, victim->pcdata->hair_adj );
    strcat( buf, " " );
	strcat( buf, victim->pcdata->hair_color );
	strcat( buf, " and " );
	strcat( buf, victim->pcdata->eye_adj);
    strcat( buf, " " );
    strcat( buf, victim->pcdata->eye_color);
	strcat( buf, " eyes.\n\r" );
	buf[0] = UPPER( buf[0] );
        send_to_char( buf, ch );
    }

    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
	send_to_char( "`X", ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    /* Should really change a lot of these send_to_char()'s to act()'s */

    if ( MOUNTED( victim ) )
    {
	sprintf( buf, "%s`X is riding %s`X.\n\r", victim->name, PERS( MOUNTED( victim ), ch ) );
	send_to_char( buf, ch );
    }
    if ( RIDDEN( victim ) )
    {
        sprintf( buf, "%s`X is being ridden by %s`X.\n\r", victim->short_descr, PERS( RIDDEN( victim ), ch ) );
        send_to_char( buf, ch );
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS( victim, ch ) );
    if ( has_colcode( buf ) )
        strcat( buf, "`X" );
    if ( !IS_NPC( victim )
         && !IS_NULLSTR( victim->pcdata->lname )
         && knows_char( ch, victim ) )
    {
	strcat( buf, " " );
	strcat( buf, victim->pcdata->lname );
    }

    if (percent >= 100)
	strcat( buf, " is `Wunharmed`X.\n\r");
    else if (percent >= 90)
	strcat( buf, " is `Binjured `Xslightly.\n\r");
    else if (percent >= 75)
	strcat( buf," is a bit `Obattered`X.\n\r");
    else if (percent >=  50)
	strcat( buf, " is `rdamaged `Xconsiderably.\n\r");
    else if (percent >= 30)
	strcat( buf, " is `Pquite hurt`X.\n\r");
    else if (percent >= 15)
	strcat ( buf, " is grievously `Rharmed`X.\n\r");
    else if (percent >= 0 )
	strcat (buf, " is nearly `Yvanquished`X.\n\r");
    else
	strcat(buf, " is `Cincapacitated`X.\n\r");

    p = &buf[0];
    while ( is_colcode( p ) )
        p += 2;
    *p = UPPER( *p );
    send_to_char( buf, ch );

    if ( IS_DEAD( ch ) && victim == ch )
        return;

    found = FALSE;
    for ( index = 0; index < MAX_WEAR; index++ )
    {
	iWear = wear_order[index];
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[iWear], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	    if ( obj->item_type == ITEM_SHEATH
	    &&	 obj->contains != NULL
	    &&	 !IS_SET( obj->value[1], SHEATH_CONCEAL )
	    &&	 !IS_SET( obj->value[1], CONT_CLOSED )
	    &&	 can_see_obj( ch, obj->contains ) )
	    {
		send_to_char( in_sheath, ch );
		send_to_char( format_obj_to_char( obj->contains, ch, TRUE ), ch );
		send_to_char( "\n\r", ch );
	    }
	}
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&   number_percent( ) < get_skill(ch,gsn_peek))
    {
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	check_improve(ch,gsn_peek,TRUE,4);
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}


void
show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch
	||   ( RIDDEN( rch ) && rch->in_room == RIDDEN( rch )->in_room && RIDDEN( rch ) != ch ) )
	    continue;

	if ( get_trust(ch) < rch->invis_level)
	    continue;

	if ( IS_NPC( rch ) && rch->pIndexData->vnum == MOB_VNUM_SUPERMOB
	&&   ( IS_NPC( ch ) || !IS_CODER( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
            if( MOUNTED( rch ) && ( rch->in_room == MOUNTED( rch )->in_room ) )
                show_char_to_char_0( MOUNTED( rch ), ch );
	}
	else if ( room_is_dark( ch->in_room )
	&&        IS_AFFECTED( rch, AFF_INFRARED ) )
	{
	    send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
	}
    }

    return;
}


static void
show_sky( CHAR_DATA *ch )
{
    BUFFER *	pBuf;
    bool	fDaytime;
    int		col;
    int		line;
    int		sector;
    int		star_pos;
    int		sun_pos;

    if ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
        sector = ch->in_room != NULL ? ch->in_room->sector_type : SECT_UNDERGROUND;
        switch ( sector )
        {
            default: break;
            case SECT_UNDERGROUND:
            case SECT_UNDERWATER:
                send_to_char( "You can't do that here.\n\r", ch );
                return;
        }
        if ( !IS_OUTSIDE( ch ) )
        {
            send_to_char( "You can't do that here.\n\r", ch );
            return;
        }
        if ( weather_info.sky == SKY_RAINING
        ||   weather_info.sky == SKY_LIGHTNING )
        {
            send_to_char( "It's raining too hard.\n\r", ch );
            return;
        }
    }

    pBuf = new_buf( );
    add_buf( pBuf, "You gaze at the heavens and see:\n\r" );

    fDaytime = ( time_info.hour >= hour_sunrise && time_info.hour <= hour_sunset );
    sun_pos = ( ( time_info.hour ) * SKY_WIDTH ) / HOURS_PER_DAY ;
    star_pos = ( sun_pos + SKY_WIDTH * time_info.month / DAYS_PER_MONTH ) % SKY_WIDTH;

    for ( line = 0; line < SKY_HEIGHT; line++ )
    {
        if ( fDaytime && ( line < 3 || line >= 6 ) )
            continue;
        add_buf( pBuf, " " );
        for ( col = SKY_WIDTH / 4; col <= 3 * SKY_WIDTH / 4; col++ )
        {
            if ( fDaytime )
            {
                if ( col >= sun_pos - 2 && col <= sun_pos + 2 )
                    buf_printf( pBuf, "`Y%c", sun_map[line - 3][col + 2 - sun_pos] );
                else
                    add_buf( pBuf, " " );
            }
            else
            {
                switch( sky_map[line][( SKY_WIDTH + col - star_pos ) % SKY_WIDTH] )
                {
                    case 'r':	add_buf( pBuf, " `r" ); break;
                    case 'R':	add_buf( pBuf, " `R" ); break;
                    case 'g':	add_buf( pBuf, " `g" ); break;
                    case 'G':	add_buf( pBuf, " `G" ); break;
                    case 'b':	add_buf( pBuf, " `b" ); break;
                    case 'B':	add_buf( pBuf, " `B" ); break;
                    case 'c':	add_buf( pBuf, " `c" ); break;
                    case 'C':	add_buf( pBuf, " `C" ); break;
                    case 'p':	add_buf( pBuf, " `p" ); break;
                    case 'P':	add_buf( pBuf, " `P" ); break;
                    case 'w':	add_buf( pBuf, " `w" ); break;
                    case 'W':	add_buf( pBuf, " `W" ); break;
                    case 'O':	add_buf( pBuf, " `O" ); break;
                    case 'Y':	add_buf( pBuf, " `Y" ); break;
                    case 'z':	add_buf( pBuf, " `z" ); break;
                    case 'X':	add_buf( pBuf, " `X" ); break;
                    case '`':	add_buf( pBuf, "``" ); break;
                    case '.':	add_buf( pBuf, "." ); break;
                    case ',':	add_buf( pBuf, "," ); break;
                    case '*':	add_buf( pBuf, "*" ); break;
                    case ':':	add_buf( pBuf, ":" ); break;
                    case ';':	add_buf( pBuf, ";" ); break;
                    case '\'':	add_buf( pBuf, "'" ); break;
                    case ' ':	add_buf( pBuf, " " ); break;
                    default:	add_buf( pBuf, " " ); break;
                }
            }
        }
        add_buf( pBuf, "\n\r" );
    }

    send_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


void
show_vehicles_to_char( ROOM_INDEX_DATA *list, CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *room;

    for ( room = list; room != NULL; room = room->next_in_room )
    {
	if ( can_see_room( ch, room ) )
	    ch_printf( ch, "%s is here.\n\r", ROOMNAME( room ) );
    }
}


bool
check_blind( CHAR_DATA *ch )
{

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) )
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}


void
do_condition( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    BUFFER *	pBuf;
    int		percent;

    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && *argument != '\0' )
        vch = get_char_world( ch, argument );
    else
        vch = ch;

    if ( vch == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    pBuf = new_buf( );

    if ( vch > 0 )
        percent = vch->hit * 100 / vch->max_hit;
    else
        percent = -1;

    if ( percent >= 100 )
	add_buf( pBuf, "You are in excellent condition.\n\r" );
    else if ( percent >= 90 )
	add_buf( pBuf, "You have a few scratches.\n\r" );
    else if ( percent >= 75 )
	add_buf( pBuf,"You have some small wounds and bruises.\n\r" );
    else if ( percent >=  50 )
	add_buf( pBuf, "You have quite a few wounds.\n\r" );
    else if ( percent >= 30 )
	add_buf( pBuf, "You have some big nasty wounds and scratches.\n\r" );
    else if ( percent >= 15 )
	add_buf ( pBuf, "You look pretty hurt.\n\r" );
    else if ( percent >= 0 )
	add_buf ( pBuf, "You are in awful condition.\n\r" );
    else
	add_buf(pBuf, "You are bleeding to death.\n\r");

    if ( IS_NPC( vch ) )
    {
        add_buf( pBuf, "Mobs never get hungry or thirsty.\n\t" );
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
        return;
    }

    percent = vch->pcdata->condition[COND_HUNGER] * 100 / MAX_COND;
    if ( percent < 0 )
        add_buf( pBuf, "You never get hungry.\n\r" );
    else if ( percent >= 100 )
        add_buf( pBuf, "You are not at all hungry.\n\r" );
    else if ( percent >= 75 )
        add_buf( pBuf, "You're not really hungry.\n\r" );
    else if ( percent >= 50 )
        add_buf( pBuf, "You could use something to eat.\n\r" );
    else if ( percent >= 25 )
        add_buf( pBuf, "You're starting to get hungry.\n\r" );
    else if ( percent >= 10 )
        add_buf( pBuf, "You're starting to feel hunger pangs.\n\r" );
    else if ( percent > 0 )
        add_buf( pBuf, "You are hungry.\n\r" );
    else
        add_buf( pBuf, "You are famished!\n\r" );

    percent = vch->pcdata->condition[COND_THIRST] * 100 / MAX_COND;
    if ( percent < 0 )
        add_buf( pBuf, "You never get thirsty.\n\r" );
    else if ( percent >= 100 )
        add_buf( pBuf, "Your thirst is completely slaked.\n\r" );
    else if ( percent >= 75 )
        add_buf( pBuf, "You are not thirsty.\n\r" );
    else if ( percent >= 50 )
        add_buf( pBuf, "You wouldn't mind having a refreshing drink.\n\r" );
    else if ( percent >= 25 )
        add_buf( pBuf, "You are getting thirsty.\n\r" );
    else if ( percent >= 10 )
        add_buf( pBuf, "You are thirsty.\n\r" );
    else if ( percent > 0 )
        add_buf( pBuf, "You are very thirsty.\n\r" );
    else
        add_buf( pBuf, "You are parched!\n\r" );

    percent = vch->pcdata->condition[COND_FULL] * 100 / MAX_COND;
    if ( percent < 0 )
        add_buf( pBuf, "You never feel full.\n\r" );
    else if ( percent >= 100 )
        add_buf( pBuf, "You are stuffed!\n\r" );
    else if ( percent >= 75 )
        add_buf( pBuf, "You feel sated.\n\r" );
    else if ( percent >= 50 )
        add_buf( pBuf, "You have room for a bit of food or drink.\n\r" );
    else if ( percent >= 25 )
        add_buf( pBuf, "Something to eat sounds like a good idea.\n\r" );
    else if ( percent >= 7 )
        add_buf( pBuf, "You're starting to feel some hunger pangs.\n\r" );
    else
        add_buf( pBuf, "Your stomach is empty.\n\r" );

    percent = vch->pcdata->condition[COND_TIRED] * 100 / MAX_COND;
    if ( percent < 0 )
        add_buf( pBuf, "You never get tired.\n\r" );
    else if ( percent >= 100 )
        add_buf( pBuf, "You feel fully rested.\n\r" );
    else if ( percent > 75 )
        add_buf( pBuf, "You feel rested.\n\r" );
    else if ( percent >= 50 )
        add_buf( pBuf, "You are a bit tired.\n\r" );
    else if ( percent >= 25 )
        add_buf( pBuf, "You feel fatigued.\n\r" );
    else if ( percent >= 7 )
        add_buf( pBuf, "You are tired.\n\r" );
    else
        add_buf( pBuf, "You are asleep on your feet!\n\r" );

    percent = vch->pcdata->condition[COND_DRUNK] * 100 / MAX_COND;
    if ( percent < 0 )
        add_buf( pBuf, "You never get drunk.\n\r" );
    else if ( percent >= 100 )
        add_buf( pBuf, "You are totally smashed!\n\r" );
    else if ( percent >= 75 )
        add_buf( pBuf, "You are staggering around drunkenly.\n\r" );
    else if ( percent >= 25 )
        add_buf( pBuf, "You are drunk.\n\r" );
    else if ( percent >= 7 )
        add_buf( pBuf, "You are a bit tipsy.\n\r" );
    else if ( percent > 0 )
        add_buf( pBuf, "You're feeling good.\n\r" );
    else
        add_buf( pBuf, "You are sober.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


/* changes your scroll */
void
do_scroll( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[SHORT_STRING_LENGTH];
    int lines;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( ch->lines == 0 )
	    send_to_char( "You do not page long messages.\n\r", ch );
	else
	{
	    sprintf( buf, "You currently display %d lines per page.\n\r",
		    ch->lines + 2 );
	    send_to_char( buf, ch );
	}
	return;
    }

    if ( !is_number( arg ) )
    {
	send_to_char( "You must provide a number.\n\r", ch );
	return;
    }

    lines = atoi( arg );

    if ( lines == 0 )
    {
        send_to_char( "Paging disabled.\n\r", ch );
        ch->lines = 0;
        return;
    }

    if ( lines < 10 || lines > 100 )
    {
	send_to_char( "You must provide a reasonable number.\n\r", ch );
	return;
    }

    sprintf( buf, "Scroll set to %d lines.\n\r", lines );
    send_to_char( buf, ch );
    ch->lines = lines - 2;
}


void
do_socials( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;
    BUFFER	*pBuf;
    char	 arg[MAX_INPUT_LENGTH];
    int		 col;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	pBuf = new_buf( );
	col = 0;

	for ( pSocial = social_first; pSocial; pSocial = pSocial->next )
	{
	    if ( !pSocial->deleted )
	    {
		buf_printf( pBuf, "%-12s", pSocial->name );
		if ( ++col % 6 == 0 )
		    add_buf( pBuf, "\n\r" );
	    }
	}
	if ( col % 6 != 0 )
	    add_buf( pBuf, "\n\r" );

	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
    }
    else
    {
	if ( !check_social( ch, arg, argument ) )
	{
	    do_socials( ch, "" );
	}
    }
    return;

}


/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "motd");
}

void do_imotd(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "imotd");
}

void do_rules(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "rules");
}

void do_story(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "story");
}

void do_wizlist(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "wizlist");
}


void
do_config( CHAR_DATA *ch, char *argument )
{
    char	arg [ MAX_INPUT_LENGTH ];
    char	buf[MAX_STRING_LENGTH];
    int		bit;
    int		bit2;
    int		comm;
    bool	fSet;

    if ( IS_NPC( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "[ Keyword   ] Option\n\r", ch );

	send_to_char( IS_SET( ch->comm, COMM_SHOW_AFFECTS )
            ? "[+AFFECTS   ] You will see affects in Score.\n\r"
	    : "[-affects   ] You will not see affects in Score.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_AUTOASSIST  )
            ? "[+AUTOASSIST] You automatically assist group members.\n\r"
	    : "[-autoassist] You don't automatically assist anybody.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act2, PLR_AUTOCHANNEL )
	    ? "[+AUTOCHAN  ] You toggle channels by channel name.\n\r"
	    : "[-autochan  ] You toggle channels with the CHANNEL command.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_AUTOCOINS  )
	    ? "[+AUTOCOINS ] You automatically get coins from corpses.\n\r"
	    : "[-autocoins ] You don't automatically get coins from corpses.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_AUTODAMAGEDEALT )
	    ? "[+AUTODAMAGE] You see points of damage inflicted.\n\r"
	    : "[-autodamage] You don't see damage points inflicted.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_AUTOEXIT  )
            ? "[+AUTOEXIT  ] You automatically see exits.\n\r"
	    : "[-autoexit  ] You don't automatically see exits.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_AUTOLOOT  )
	    ? "[+AUTOLOOT  ] You automatically loot corpses.\n\r"
	    : "[-autoloot  ] You don't automatically loot corpses.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act2, PLR_AUTOOPEN  )
	    ? "[+AUTOOPEN  ] You automatically open after unlocking.\n\r"
	    : "[-autoopen  ] You don't open after unlocking.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_AUTOSAC   )
	    ? "[+AUTOSAC   ] You automatically sacrifice corpses.\n\r"
	    : "[-autosac   ] You don't automatically sacrifice corpses.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_AUTOSPLIT  )
	    ? "[+AUTOSPLIT ] You automatically split coins with group members.\n\r"
	    : "[-autosplit ] You don't automatically split coins with group members.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_BLINK	)
	    ? "[+BLINK     ] You have blinking text enabled.\n\r"
	    : "[-blink     ] You have blinking text disabled.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->comm, COMM_BRIEF     )
	    ? "[+BRIEF     ] You see brief descriptions.\n\r"
	    : "[-brief     ] You see long descriptions.\n\r"
	    , ch );

	send_to_char( ch->desc->ansi
	    ? "[+COLOR     ] You have ansi color enabled.\n\r"
	    : "[-color     ] You have ansi color disabled.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->comm, COMM_COMBINE   )
	    ? "[+COMBINE   ] You see object lists in combined format.\n\r"
	    : "[-combine   ] You see object lists in single format.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->comm, COMM_COMPACT   )
	    ? "[+COMPACT   ] You do not receive a blank line before your prompt.\n\r"
	    : "[-compact   ] You receive a blank line before your prompt.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->comm, COMM_DETAIL )
	    ? "[+DETAIL    ] You receive a more detailed inventory list.\n\r"
	    : "[-detail    ] You receive a normal inventory list.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act2, PLR_FMETER )
	    ? "[+FMETER    ] You have a fight meter.\n\r"
	    : "[-fmeter    ] You don't have a fight meter.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_NOFOLLOW )
	    ? "[+NOFOLLOW  ] You do not allow followers.\n\r"
	    : "[-nofollow  ] You allow followers.\n\r"
	    , ch );

	    send_to_char( IS_SET( ch->act, PLR_NOEXP )
	    ? "[+NOEXP     ] You will not receive exp for kills.\n\r"
	    : "[-noexp     ] You will receive exp for kills.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act2, PLR_KEEPALIVE )
	    ? "[+KEEPALIVE ] You have a keepalive.\n\r"
	    : "[-keepalive ] You do not have a keepalive.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act2, PLR_NOPUNCT  )
	    ? "[+NOPUNCT   ] You don't automatically punctuate sentences.\n\r"
	    : "[-nopunct   ] You automatically punctuate sentences.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act2, PLR_AUTOTICK )
	    ? "[+AUTOTICK  ] You will see tick messages.\n\r"
	    : "[-autotick  ] You will not see tick messages.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->act, PLR_NOSUMMON )
	    ? "[+NOSUMMON  ] You cannot be summoned.\n\r"
	    : "[-nosummon  ] You may be summoned.\n\r"
	    , ch );

	if ( can_use_skpell( ch, gsn_peek ) )
	{
	    send_to_char( IS_SET( ch->act2, PLR_PEEK )
		? "[+PEEK      ] You see others inventories.\n\r"
		: "[-peek      ] You do not see others inventories.\n\r"
		, ch );
	}

	send_to_char( IS_SET( ch->comm, COMM_PROMPT    )
	    ? "[+PROMPT    ] You have a prompt.\n\r"
	    : "[-prompt    ] You don't have a prompt.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->comm, COMM_TELNET_GA )
	    ? "[+TELNETGA  ] You receive a telnet GA sequence.\n\r"
	    : "[-telnetga  ] You don't receive a telnet GA sequence.\n\r"
	    , ch );

	if ( IS_IMMORTAL( ch ) )
	{
	    send_to_char( IS_SET( ch->act2, PLR_AUTOMOVE )
		? "[+AUTOMOVE  ] You go to a newly dug room.\n\r"
		: "[-automove  ] You do not go to a newly dug room.\n\r"
		, ch );
	    send_to_char( IS_SET( ch->act2, PLR_NOCOPY )
		? "[+NOCOPY    ] You do not copy room flags when creating a new room.\n\r"
		: "[-nocopy    ] You copy room flags when creating a new room.\n\r"
		, ch );
	    send_to_char( IS_SET( ch->act2, PLR_RFLAGS )
		? "[+RFLAGS    ] You see room sector and flags.\n\r"
		: "[-rflags    ] You do not see room sector and flags.\n\r"
		, ch );
	    send_to_char( IS_SET( ch->comm, COMM_VNUMS )
		? "[+VNUMS     ] You see vnums.\n\r"
		: "[-vnums     ] You do not see vnums.\n\r"
		, ch );
	}
    }
    else
    {
	bit  = 0;
	bit2 = 0;
	comm = 0;
	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

             if ( !str_cmp( arg+1, "autoexit" ) ) bit  = PLR_AUTOEXIT;
	else if ( !str_cmp( arg+1, "autoassist")) bit  = PLR_AUTOASSIST;
	else if ( !str_cmp( arg+1, "autocoins") ) bit  = PLR_AUTOCOINS;
	else if ( !str_cmp( arg+1, "autodamage")) bit  = PLR_AUTODAMAGEDEALT;
	else if ( !str_cmp( arg+1, "autoloot" ) ) bit  = PLR_AUTOLOOT;
	else if ( !str_cmp( arg+1, "autoopen" ) ) bit2 = PLR_AUTOOPEN;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit  = PLR_AUTOSAC;
	else if ( !str_cmp( arg+1, "affects"  ) ) comm = COMM_SHOW_AFFECTS;
	else if ( !str_cmp( arg+1, "blink"    ) ) bit  = PLR_BLINK;
	else if ( !str_cmp( arg+1, "brief"    ) ) comm = COMM_BRIEF;
	else if ( !str_cmp( arg+1, "combine"  ) ) comm = COMM_COMBINE;
	else if ( !str_cmp( arg+1, "compact"  ) ) comm = COMM_COMPACT;
	else if ( !str_cmp( arg+1, "detail"   ) ) comm = COMM_DETAIL;
        else if ( !str_cmp( arg+1, "prompt"   ) ) comm = COMM_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) comm = COMM_TELNET_GA;
	else if ( !str_cmp( arg+1, "nofollow" ) ) bit  = PLR_NOFOLLOW;
	else if ( !str_cmp( arg+1, "noexp"    ) ) bit  = PLR_NOEXP;
	else if ( !str_cmp( arg+1, "nopunct"  ) ) bit2 = PLR_NOPUNCT;
	else if ( !str_cmp( arg+1, "nosummon" ) ) bit  = PLR_NOSUMMON;
	else if ( !str_cmp( arg+1, "autosplit") ) bit  = PLR_AUTOSPLIT;
	else if ( !str_cmp( arg+1, "fmeter"   ) ) bit2 = PLR_FMETER;
	else if ( !str_cmp( arg+1, "keepalive") ) bit2 = PLR_KEEPALIVE;
	else if ( !str_cmp( arg+1, "peek"     ) ) bit2 = PLR_PEEK;
	else if ( !str_cmp( arg+1, "autochan" ) ) bit2 = PLR_AUTOCHANNEL;
	else if ( !str_cmp( arg+1, "automove" ) ) bit2 = PLR_AUTOMOVE;
	else if ( !str_cmp( arg+1, "nocopy"   ) ) bit2 = PLR_NOCOPY;
	else if ( !str_cmp( arg+1, "rflags"   ) ) bit2 = PLR_RFLAGS;
	else if ( !str_cmp( arg+1, "vnums"    ) ) comm = COMM_VNUMS;
	else if ( !str_cmp( arg+1, "autotick"    ) ) bit2 = PLR_AUTOTICK;
	else if ( !str_cmp( arg+1, "color"    ) || !str_cmp( arg+1, "ansi" ) )
	{
	    ch->desc->ansi = fSet;
	    ch_printf( ch, "Color is now %s.\n\r", ch->desc->ansi ? "ON" : "OFF" );
	    return;
	}
	else
	{
	    send_to_char( "Config which option?\n\r", ch );
	    return;
	}

	if ( !bit && !bit2 && !comm )
	{
	    send_to_char( "Config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	{
	    if ( bit )
		SET_BIT( ch->act, bit );
	    if ( bit2 )
	    {
		SET_BIT( ch->act2, bit2 );
		if ( bit2 & PLR_KEEPALIVE )
		{
		    add_keepalive( ch );
		}
	    }
	    if ( comm )
		SET_BIT( ch->comm, comm );
	    sprintf( buf, "%s is now ON.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
	else
	{
	    if ( bit )
		REMOVE_BIT( ch->act, bit );
	    if ( bit2 )
	    {
		REMOVE_BIT( ch->act2, bit2 );
		if ( bit2 & PLR_KEEPALIVE )
		{
		    strip_event_desc( ch->desc, EVENT_DESC_KEEPALIVE );
		}
	    }
	    if ( comm )
		REMOVE_BIT( ch->comm, comm );
	    sprintf( buf, "%s is now OFF.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}

    }

}


/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist(CHAR_DATA *ch, char *argument)
{
    /* lists most player flags */
    if ( IS_NPC( ch ) )
	return;

    send_to_char("   action     status\n\r",ch);
    send_to_char("---------------------\n\r",ch);

    send_to_char( "autoassist      ",ch );
    if (IS_SET(ch->act,PLR_AUTOASSIST))
	send_to_char("`GON`X\n\r",ch);
    else
	send_to_char("`ROFF`X\n\r",ch);

    send_to_char( "autocoins       ", ch );
    if ( IS_SET( ch->act, PLR_AUTOCOINS ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );

    send_to_char( "autodamagedealt ", ch );
    if ( IS_SET( ch->act, PLR_AUTODAMAGEDEALT ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r",ch );

    send_to_char( "autoexit        ", ch );
    if ( IS_SET( ch->act,PLR_AUTOEXIT ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );

    send_to_char( "autoloot        ", ch );
    if ( IS_SET( ch->act, PLR_AUTOLOOT ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );

    send_to_char( "autometer       ", ch );
    send_to_char( IS_SET( ch->act2, PLR_FMETER ) ? "`GON`X\n\r" : "`ROFF`X\n\r", ch );
#if 0
    send_to_char( "autopunct       ", ch );
    if ( IS_SET( ch->act2, PLR_NOPUNCT ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );
#endif
    send_to_char( "autosac         ", ch );
    if ( IS_SET( ch->act, PLR_AUTOSAC ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );

    send_to_char( "autosplit       ", ch );
    if ( IS_SET( ch->act, PLR_AUTOSPLIT ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );

send_to_char( "autotick        ", ch );
    if ( IS_SET( ch->act2, PLR_AUTOTICK ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );

    send_to_char( "compact mode    ",ch );
    if ( IS_SET( ch->comm, COMM_COMPACT ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );

    send_to_char( "combine items   ", ch );
    if ( IS_SET( ch->comm, COMM_COMBINE ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );

    send_to_char( "prompt          ", ch );
    if ( IS_SET( ch->comm, COMM_PROMPT ) )
	send_to_char( "`GON`X\n\r", ch );
    else
	send_to_char( "`ROFF`X\n\r", ch );
#if 0
    if (!IS_SET(ch->act,PLR_CANLOOT))
	send_to_char("Your corpse is safe from thieves.\n\r",ch);
    else
	send_to_char("Your corpse may be looted.\n\r",ch);
#endif
    if (IS_SET(ch->act,PLR_NOSUMMON))
	send_to_char("You cannot be summoned.\n\r",ch);
    else
	send_to_char("You can be summoned.\n\r",ch);

    if (IS_SET(ch->act,PLR_NOEXP))
	send_to_char("You will not receive exp for kills.\n\r",ch);
    else
	send_to_char("You will receive exp for kills.\n\r",ch);

    if (IS_SET(ch->act,PLR_NOFOLLOW))
	send_to_char("You do not welcome followers.\n\r",ch);
    else
	send_to_char("You accept followers.\n\r",ch);

    send_to_char( "\n\rSee also CONFIG command for more options and settings.\n\r", ch );
}


void
do_autoassist( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_AUTOASSIST ) )
    {
	send_to_char( "Autoassist removed.\n\r", ch );
	REMOVE_BIT( ch->act, PLR_AUTOASSIST );
    }
    else
    {
	send_to_char( "You will now assist when needed.\n\r", ch );
	SET_BIT( ch->act, PLR_AUTOASSIST );
    }
}


void
do_autodamagedealt( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_AUTODAMAGEDEALT ) )
    {
	send_to_char( "Autodamagedealt is now OFF.\n\r", ch );
	REMOVE_BIT(ch->act,PLR_AUTODAMAGEDEALT);
    }
    else
    {
	send_to_char( "Autodamagedealt is now ON.\n\r", ch );
	SET_BIT( ch->act, PLR_AUTODAMAGEDEALT );
    }
}


void
do_autoexit( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_AUTOEXIT ) )
    {
	send_to_char( "Exits will no longer be displayed.\n\r", ch );
	REMOVE_BIT( ch->act, PLR_AUTOEXIT );
    }
    else
    {
	send_to_char( "Exits will now be displayed.\n\r", ch );
	SET_BIT( ch->act, PLR_AUTOEXIT );
    }
}


void
do_autotick( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act2, PLR_AUTOTICK ) )
    {
	send_to_char( "You will no longer see tick messages.\n\r", ch );
	REMOVE_BIT( ch->act2, PLR_AUTOTICK );
    }
    else
    {
	send_to_char( "Tick messages will be displayed.\n\r", ch );
	SET_BIT( ch->act2, PLR_AUTOTICK );
    }
}

void
do_autocoins( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_AUTOCOINS ) )
    {
	send_to_char( "Autocoins removed.\n\r", ch );
	REMOVE_BIT( ch->act, PLR_AUTOCOINS );
    }
    else
    {
	send_to_char( "Automatic coin looting set.\n\r", ch );
	SET_BIT( ch->act, PLR_AUTOCOINS );
    }
}


void
do_autoloot( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_AUTOLOOT ) )
    {
	send_to_char( "Autolooting removed.\n\r", ch );
	REMOVE_BIT( ch->act, PLR_AUTOLOOT );
    }
    else
    {
	send_to_char( "Automatic corpse looting set.\n\r", ch );
	SET_BIT( ch->act, PLR_AUTOLOOT );
    }
}


void
do_autometer( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    if ( IS_NPC( ch ) && !ch->desc )
    {
	send_to_char( "Mobs don't have fight meters.\n\r", ch );
	return;
    }

    rch = ch->desc->original ? ch->desc->original : ch->desc->character;
    TOGGLE_BIT( rch->act2, PLR_FMETER );
    ch_printf( ch, "Your fight meter is now %s.\n\r",
	IS_SET( rch->act2, PLR_FMETER ) ? "ON" : "off" );
    return;
}


void
do_autosac( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_AUTOSAC ) )
    {
	send_to_char( "Autosacrificing removed.\n\r", ch );
	REMOVE_BIT( ch->act, PLR_AUTOSAC );
    }
    else
    {
	send_to_char( "Automatic corpse sacrificing set.\n\r", ch );
	SET_BIT( ch->act, PLR_AUTOSAC );
    }
}


void
do_autosplit( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_AUTOSPLIT ) )
    {
	send_to_char( "Autosplitting removed.\n\r", ch );
	REMOVE_BIT( ch->act, PLR_AUTOSPLIT );
    }
    else
    {
	send_to_char( "Automatic coin splitting set.\n\r", ch );
	SET_BIT( ch->act, PLR_AUTOSPLIT );
    }
}


void
do_brief( CHAR_DATA *ch, char *argument )
{
    if ( IS_SET( ch->comm, COMM_BRIEF ) )
    {
      send_to_char( "Full descriptions activated.\n\r", ch );
      REMOVE_BIT( ch->comm, COMM_BRIEF );
    }
    else
    {
      send_to_char( "Short descriptions activated.\n\r", ch );
      SET_BIT( ch->comm, COMM_BRIEF );
    }
}


void
do_compact( CHAR_DATA *ch, char *argument )
{
    if ( IS_SET( ch->comm, COMM_COMPACT ) )
    {
      send_to_char( "Compact mode removed.\n\r", ch );
      REMOVE_BIT( ch->comm, COMM_COMPACT );
    }
    else
    {
      send_to_char( "Compact mode set.\n\r", ch );
      SET_BIT( ch->comm, COMM_COMPACT );
    }
}


void
do_show( CHAR_DATA *ch, char *argument )
{
    if ( IS_SET( ch->comm, COMM_SHOW_AFFECTS ) )
    {
      send_to_char( "Affects will no longer be shown in score.\n\r", ch );
      REMOVE_BIT( ch->comm, COMM_SHOW_AFFECTS );
    }
    else
    {
      send_to_char( "Affects will now be shown in score.\n\r", ch );
      SET_BIT( ch->comm, COMM_SHOW_AFFECTS );
    }
}


void
do_prompt( CHAR_DATA *ch, char *argument )
{
    char	buf[MSL];

    if ( argument[0] == '\0' )
    {
	if ( IS_SET( ch->comm, COMM_PROMPT ) )
	{
	    send_to_char( "You will no longer see prompts.\n\r", ch );
	    REMOVE_BIT( ch->comm, COMM_PROMPT );
	}
	else
	{
	    send_to_char( "You will now see prompts.\n\r", ch );
	    SET_BIT( ch->comm,COMM_PROMPT );
	}
	return;
    }

    if ( !str_cmp( argument, "all" ) )
	strcpy( buf, "<%h/%Hhp %m/%Mm %vmv %Xtnl> ");
    else if ( !str_cmp( argument, "full" ) )
        strcpy( buf, "<`Y%h/%Hhp `C%m/%Mm `G%vmv `P%Xtnl`w> " );
    else if (!str_cmp( argument, "colorful"))
    	strcpy( buf, "<`R%h`X/`r%H`Whp `P%m`X/`p%M`Wm `G%v`X/`g%V`Wmv `C%X`Wtnl`X> ");
    else if ( !str_cmp( argument, "show" ) )
    {
	ch_printf( ch, "Your current prompt is: %s\n\r", ch->prompt );
	return;
    }
    else if ( IS_IMMORTAL( ch ) && !str_prefix( "imm", argument ) )
	strcpy( buf, "[`W%R `w%r]%c[`W%z`w]%c[`W%o| `w%O`w ]`X " );
    else
    {
	if ( strlen( argument ) > 50 )
	    argument[50] = '\0';
	strcpy( buf, argument );
	smash_tilde( buf );
	if ( str_suffix( "%c", buf ) )
	    strcat( buf, " " );
    }

    free_string( ch->prompt );
    ch->prompt = str_dup( buf );
    ch_printf( ch, "Prompt set to %s\n\r", ch->prompt );
    return;
}


void
do_combine( CHAR_DATA *ch, char *argument )
{
    if ( IS_SET( ch->comm, COMM_COMBINE ) )
    {
      send_to_char( "Long inventory selected.\n\r", ch );
      REMOVE_BIT( ch->comm, COMM_COMBINE );
    }
    else
    {
      send_to_char( "Combined inventory selected.\n\r", ch );
      SET_BIT( ch->comm, COMM_COMBINE );
    }
}


void
do_noloot( CHAR_DATA *ch, char *argument )
{
    ch_printf( ch, "Players do not leave corpses on %s.\n\r", MUD_NAME );
    send_to_char( "See HELP DEATH.\n\r", ch );
    return;

    if ( IS_NPC( ch ) )
      return;

    if ( IS_SET( ch->act, PLR_CANLOOT ) )
    {
      send_to_char( "Your corpse is now safe from thieves.\n\r", ch );
      REMOVE_BIT( ch->act, PLR_CANLOOT );
    }
    else
    {
      send_to_char( "Your corpse may now be looted.\n\r", ch );
      SET_BIT( ch->act, PLR_CANLOOT );
    }
}


void
do_nofollow( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    if (IS_SET(ch->act,PLR_NOFOLLOW))
    {
        send_to_char( "You now accept followers.\n\r", ch );
        REMOVE_BIT( ch->act, PLR_NOFOLLOW );
    }
    else
    {
        send_to_char( "You no longer accept followers.\n\r", ch );
        SET_BIT( ch->act,PLR_NOFOLLOW );
        die_follower( ch );
    }
}

void do_nosummon(CHAR_DATA *ch, char *argument)
{
    if ( IS_NPC( ch ) )
    {
        if ( IS_SET( ch->imm_flags, IMM_SUMMON ) )
        {
	    send_to_char( "You are no longer immune to summon.\n\r", ch );
	    REMOVE_BIT( ch->imm_flags, IMM_SUMMON );
        }
        else
        {
	    send_to_char( "You are now immune to summoning.\n\r", ch );
	    SET_BIT( ch->imm_flags, IMM_SUMMON );
        }
    }
    else
    {
        if ( IS_SET( ch->act, PLR_NOSUMMON ) )
        {
            send_to_char( "You are no longer immune to summon.\n\r", ch );
            REMOVE_BIT( ch->act, PLR_NOSUMMON );
        }
        else
        {
            send_to_char( "You are now immune to summoning.\n\r", ch );
            SET_BIT( ch->act, PLR_NOSUMMON );
        }
    }
}


void
do_glance( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		chance;
    CHAR_DATA *	victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Glance at whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    chance = number_percent( );

    if ( !IS_NPC( ch ) && chance > get_skill( ch, gsn_glance ) )
    {
	show_char_to_char_1( victim, ch, FALSE );
	check_improve( ch, gsn_glance, FALSE, 3 );
    }

    if ( !IS_NPC( ch ) && chance <= get_skill( ch, gsn_glance ) )
    {
	show_char_to_char_1( victim, ch, TRUE );
	check_improve( ch, gsn_glance, TRUE, 3 );
    }

    return;
}


void
do_look( CHAR_DATA *ch, char *argument )
{
    char		buf  [MAX_STRING_LENGTH];
    char		arg1 [MAX_INPUT_LENGTH];
    char		arg2 [MAX_INPUT_LENGTH];
    char		arg3 [MAX_INPUT_LENGTH];
    EXIT_DATA *		pexit;
    CHAR_DATA *		victim;
    ROOM_INDEX_DATA *	portroom;
    ROOM_INDEX_DATA *	original;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	pShip;
    OBJ_DATA *	obj;
    EXTRA_DESCR_DATA *	ed;
    char *		pdesc;
    char *		pkey;
    int			door;
    int			number;
    int			count;
    char *		rdesc;
    int			hour;
    bool		edesc;
    bool		mdesc;
    bool		ndesc;
    bool		door_named;

//  if ( ch->desc == NULL )
//	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   room_is_dark( ch->in_room ) )
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    number = number_argument(arg1,arg3);
    count = 0;

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	set_char_color( AT_GREEN, ch );
	send_to_char( ROOMNAME( ch->in_room ), ch );

	if ( IS_IMMORTAL( ch ) && ( IS_NPC( ch ) || IS_SET( ch->comm, COMM_VNUMS ) ) )
	{
	    sprintf( buf, " `W[`BRoom %d`W", ch->in_room->vnum );
	    send_to_char( buf, ch );
	    if ( IS_VIRTUAL( ch->in_room ) )
	    {
		int x;
		int y;
		get_coordinates( ch->in_room, &x, &y );
		ch_printf( ch, " - (`B%d`W,`B%d`W)", x, y );
	    }
	    send_to_char( "]", ch );
	}

	send_to_char( "\n\r", ch );

	if ( IS_IMMORTAL( ch ) && IS_SET( ch->act2, PLR_RFLAGS ) )
	{
	    ch_printf( ch, "`c%s  ", flag_string( sector_types, ch->in_room->sector_type ) );
	    ch_printf( ch, "%s`X\n\r", flag_string( room_flags, ch->in_room->room_flags ) );
	}

	if ( arg1[0] == '\0'
	|| ( !IS_NPC( ch ) && !IS_SET( ch->comm, COMM_BRIEF ) ) )
	{
/*	    send_to_char( "  ",ch); */
	    mdesc = !IS_NULLSTR( ch->in_room->morning_desc );
	    edesc = !IS_NULLSTR( ch->in_room->evening_desc );
	    ndesc = !IS_NULLSTR( ch->in_room->night_desc );
	    hour = time_info.hour;
	    rdesc = ch->in_room->description;
	    if ( ( hour < hour_sunrise || hour >= hour_sunset + 1 ) && ndesc )
		rdesc = ch->in_room->night_desc;
	    if ( hour >= hour_sunrise && hour < 10 && mdesc )
		rdesc = ch->in_room->morning_desc;
	    if ( hour >=17 && hour <= 20  && edesc )
		rdesc = ch->in_room->evening_desc;
//	    set_char_color( ch->in_room->area->color, ch );
	    set_char_color( sector_data[ch->in_room->sector_type].room_color, ch );
	    send_to_char( rdesc, ch );
	}

        if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
	{
	    send_to_char( "\n\r", ch );
            do_function( ch, &do_exits, "auto" );
	}

	set_char_color( C_DEFAULT, ch );

	if ( ch->in_room->in_room == NULL )
	    show_vehicles_to_char( ch->in_room->next_in_room, ch );
	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people,   ch );

	if ( ( in_room = ch->in_room->in_room ) != NULL )
	{
	    send_to_char( "\n\rAround you, you see:\n\r\n\r", ch );
	    original = ch->in_room;
	    char_from_room( ch );
	    char_to_room( ch, in_room );
	    do_look( ch, "auto" );
	    char_from_room( ch );
	    char_to_room( ch, original );
	}

	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp(arg1, "in")  || !str_cmp(arg1,"on"))
    {
	if ( IS_DEAD( ch ) )
	{
	    send_to_char( "Spirits are unable to do that.\n\r", ch );
	    return;
	}

	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		oprog_look_in_trigger( obj, ch );
		break;
	    }

	    sprintf( buf, "It's %sfilled with a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than half-" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half-"     : "more than half-",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char( buf, ch );
	    oprog_look_in_trigger( obj, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_SHEATH:
	    if ( IS_SET( obj->value[1], CONT_CLOSED ) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p`X holds:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    oprog_look_in_trigger( obj, ch );
	    break;

	case ITEM_PORTAL:
	    if ( ( portroom = get_room_index( obj->value[3] ) ) == NULL )
	    {
		act( "You cannot see anything through $p`X.", ch, obj, NULL, TO_CHAR );
		break;
	    }
	    if ( IS_SET( obj->value[1], GATE_CLOSED ) )
	    {
	        send_to_char( "You have to open it first.\n\r", ch );
	        break;
            }
	    if ( IS_SET( obj->value[1], GATE_OPAQUE ) )
	    {
		act_color( AT_ACTION, "You cannot see anything through $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
		act_color( AT_ACTION, "$n looks into $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
		break;
	    }
	    act_color( AT_ACTION, "You look into $p and see...", ch, obj, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n looks into $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
	    original = ch->in_room;
	    char_from_room( ch );
	    char_to_room( ch, portroom );
	    do_look( ch, "auto" );
	    char_from_room( ch );
	    char_to_room( ch, original );
	    oprog_look_in_trigger( obj, ch );
	    break;
	}
	return;
    }

    if ( !str_cmp( arg1, "sky" ) )
    {
        show_sky( ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
    {
        bool glance = FALSE;
	show_char_to_char_1( victim, ch, glance );
	return;
    }

    if ( !IS_DEAD( ch ) )
    {
        for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
        {
            if ( can_see_obj( ch, obj ) )
            {  /* player can see object */
                pdesc = get_extra_descr( arg3, obj->extra_descr );
                if ( pdesc != NULL )
                {
                    if (++count == number)
                    {
                        send_to_char( pdesc, ch );
                        oprog_look_trigger( obj, ch );
                        return;
                    }
                    else
                    {
                        continue;
                    }
                }

                pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
                if ( pdesc != NULL )
                {
                    if (++count == number)
                    {
                        send_to_char( pdesc, ch );
                        oprog_look_trigger( obj, ch );
                        return;
                    }
                    else
                    {
                        continue;
                    }
                }

                if ( is_name( arg3, obj->name ) )
                {
                    if (++count == number)
                    {
                        send_to_char( obj->description, ch );
                        send_to_char( "\n\r",ch);
                        oprog_look_trigger( obj, ch );
                        return;
                    }
                }
            }
        }
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    oprog_look_trigger( obj, ch );
		    return;
	    	}

	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    oprog_look_trigger( obj, ch );
		    return;
	    	}

	    if ( is_name( arg3, obj->name ) )
	    {
		if (++count == number)
		{
		    send_to_char( obj->description, ch );
		    send_to_char("\n\r",ch);
		    oprog_look_trigger( obj, ch );
		    return;
		}
	    }
	}
    }

//    pdesc = get_extra_descr(arg3,ch->in_room->extra_descr);

    pdesc = NULL;
    pkey = NULL;
    for ( ed = ch->in_room->extra_descr; ed != NULL; ed = ed->next )
    {
        if ( is_name( arg3, ed->keyword ) )
        {
            pdesc = ed->description;
            pkey = ed->keyword;
            break;
        }
    }

    if ( pdesc != NULL )
    {
	if ( ++count == number )
	{
	    if ( !rprog_look_trigger( ch->in_room, ch, pkey ) )
		send_to_char( pdesc, ch );
	    return;
	}
    }

    if (count > 0 && count != number)
    {
    	if (count == 1)
    	    sprintf(buf,"You only see one %s here.\n\r",arg3);
    	else
    	    sprintf(buf,"You only see %d of those here.\n\r",count);

    	send_to_char(buf,ch);
    	return;
    }

    if ( !str_prefix( "auc", arg1 ) && !str_prefix( arg1, "auction" ) )
    {
	if ( !auc_obj )
	{
	    send_to_char( "There is nothing being auctioned.\n\r", ch );
	    return;
	}
	if ( !can_see_obj( ch, auc_obj ) )
	{
	    send_to_char( "You can't see it.\n\r", ch );
	    return;
	}

	ch_printf( ch, "Object: %s\n\r", auc_obj->short_descr );
	ch_printf( ch, "Type: %s   Level: %d\n\r",
		   flag_string( item_types, auc_obj->item_type ),
		   auc_obj->level );
	ch_printf( ch, "Value: %d   Price: %d\n\r", auc_obj->cost, auc_cost );
	return;
    }

    /* Look other ships in room */
    if ( ch->in_room->in_room != NULL )
        in_room = ch->in_room->in_room;
    else
        in_room = ch->in_room;
    for ( pShip = in_room->next_in_room; pShip != NULL; pShip = pShip->next_in_room )
    {
        if ( can_see_room( ch, pShip ) && is_name( arg3, pShip->name ) && ++count == number )
        {
	    mdesc = !IS_NULLSTR( pShip->morning_desc );
	    edesc = !IS_NULLSTR( pShip->evening_desc );
	    ndesc = !IS_NULLSTR( pShip->night_desc );
	    hour = time_info.hour;
	    rdesc = pShip->description;
	    if ( ( hour < hour_sunrise || hour >= hour_sunset + 1 ) && ndesc )
		rdesc = pShip->night_desc;
	    if ( hour >= hour_sunrise && hour < 10 && mdesc )
		rdesc = pShip->morning_desc;
	    if ( hour >= 17 && hour <= 20  && edesc )
		rdesc = pShip->evening_desc;
	    set_char_color( sector_data[pShip->sector_type].room_color, ch );
	    send_to_char( rdesc, ch );
	    return;
        }
    }

    door_named = FALSE;
    if ( ( door = door_lookup( arg1 ) ) == DIR_NONE )
    {
	for ( door = 0; door < MAX_DIR; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET( pexit->exit_info, EX_ISDOOR )
	    &&   pexit->keyword != NULL
	    &&   is_name( arg1, pexit->keyword ) )
	    {
		door_named = TRUE;
		break;
	    }
	}
    }

    if ( door >= MAX_DIR )
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    if ( IS_SET( pexit->exit_info, EX_SECRET )
    &&	 IS_SET( pexit->exit_info, EX_CLOSED )
    &&	 ( !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    {
	if ( door_named )
	    send_to_char( "You do not see that here.\n\r", ch );
	else
	    send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	}
    }

    eprog_look_trigger( pexit, ch->in_room, ch );
    return;
}


/* RT added back for the hell of it */
void
do_read( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    char *	arglist;
    OBJ_DATA *	book;
    OBJ_DATA *	obj;
    TEXT_DATA *	page;
    int		number;
    int		pages;

    arglist = argument;
    arglist = one_argument( arglist, arg );

    book = get_eq_char( ch, WEAR_HOLD );
    if ( book != NULL && book->item_type != ITEM_BOOK )
        book = NULL;

    if ( arg[0] == '\0' && book == NULL )
    {
        do_function( ch, &do_look, argument );
        return;
    }

    if ( IS_DEAD( ch ) )
    {
        send_to_char( "Spirits are unable to do that.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' || is_number( arg ) )
        obj = book;
    else
        obj = get_obj_here( ch, arg );

    if ( obj == NULL || obj->item_type != ITEM_BOOK )
    {
        do_function( ch, &do_look, argument );
        return;
    }

    if ( obj != book )
    {
        send_to_char( "You're not holding it.\n\r", ch );
        return;
    }

    if ( book->value[0] == 0 )
    {
        act( "$p is closed.", ch, book, NULL, TO_CHAR );
        return;
    }

    pages = count_pages( book->pIndexData );

    if ( is_number( arg ) )
    {
        number = atoi( arg );
        if ( number < 1 )
        {
            send_to_char( "Open to which page?\n\r", ch );
            return;
        }
        else if ( number > pages )
        {
            act( "$p doesn't have that many pages.", ch, book, NULL, TO_CHAR );
            return;
        }
    }
    else if ( is_number( arglist ) )
    {
        number = atoi( arglist );
        if ( number < 1 )
        {
            send_to_char( "Open to which page?\n\r", ch );
            return;
        }
        else if ( number > pages )
        {
            act( "$p doesn't have that many pages.", ch, book, NULL, TO_CHAR );
            return;
        }
    }
    else
    {
        number = book->value[0];
        if ( number > pages )
        {
            act( "You have reached the end of $p.", ch, book, NULL, TO_CHAR );
            return;
        }
    }

    book->value[0] = number;
    number = 1;
    for ( page = book->pIndexData->page; page != NULL; page = page->next )
    {
        if ( number == book->value[0] )
            break;
        number++;
    }

    if ( page == NULL )
    {
        bugf( "Read book: no page %d for #%d", number, book->pIndexData->vnum );
        return;
    }

    sprintf( buf, "You read page %d of $p:\n\r", number );
    act_color( AT_ACTION, buf, ch, book, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n reads a page from $p.", ch, book, NULL, TO_ROOM, POS_RESTING );
    book->value[0]++;
    page_to_char( page->text, ch );
    oprog_read_trigger( book, ch );

    return;
}


void
do_examine( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    OBJ_DATA *	obj;
    TEXT_DATA *	page;
    MONEY	amt;
    int		count;
    int		ccount;
    char *	p;
    int		percent;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) != NULL )
    {
        if ( victim->max_hit > 0 )
	    percent = ( 100 * victim->hit ) / victim->max_hit;
        else
	    percent = -1;

        p = stpcpy( buf, PERS( victim, ch ) );
        if ( has_colcode( buf ) )
            p = stpcpy( p, "`X" );
        if ( !IS_NPC( victim ) && !IS_NULLSTR( victim->pcdata->lname ) )
        {
            *p++ = ' ';
            p = stpcpy( p, victim->pcdata->lname );
        }

        if ( percent >= 100 )
	    p = stpcpy( p, " is in excellent condition.\n\r");
        else if ( percent >= 90 )
	    p = stpcpy( p, " has a few scratches.\n\r");
        else if ( percent >= 75 )
	    p = stpcpy( p," has some small wounds and bruises.\n\r");
        else if ( percent >=  50 )
	    p = stpcpy( p, " has quite a few wounds.\n\r");
        else if ( percent >= 30 )
	    p = stpcpy( p, " has some big nasty wounds and scratches.\n\r");
        else if ( percent >= 15 )
	    p = stpcpy( p, " looks pretty hurt.\n\r");
        else if ( percent >= 0 )
	    p = stpcpy( p, " is in awful condition.\n\r");
        else
	    p = stpcpy( p, " is bleeding to death.\n\r");

        p = buf;
        while ( is_colcode( p ) )
            p += 2;
        *p = UPPER( *p );

        send_to_char( buf, ch );
        if ( ch == victim )
            act_color( AT_ACTION, "$n examines $mself.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
        else
            act_color( AT_ACTION, "$n examines $N.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
        act_color( AT_ACTION, "$n examines you.", ch, NULL, victim, TO_VICT, POS_RESTING );

        return;
    }

    do_function(ch, &do_look, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	switch ( obj->item_type )
	{
	default:
	    break;

	case ITEM_BOOK:
	    count = 0;
	    for ( page = obj->pIndexData->page; page != NULL; page = page->next )
	        count++;
	    sprintf( buf, "$p has %d page%s.", count, count == 1 ? "" : "s" );
	    act( buf, ch, obj, NULL, TO_CHAR );
	    break;

	case ITEM_LIGHT:
	    p = stpcpy( buf, "$p will last " );
	    if ( obj->value[2] == 1 )
	        p = stpcpy( p, "one more hour" );
            else if ( obj->value[2] == 0 )
                p = stpcpy( buf, "$p has run out" );
            else if ( obj->value[2] < 0 )
                p = stpcpy( p, "indefinitely" );
            else
                p += sprintf( p, "%d hours", obj->value[2] );
	    p = stpcpy( p, ".\n\r" );
	    act( buf, ch, obj, NULL, TO_CHAR );
	    break;

	case ITEM_MONEY:
	    amt.gold = obj->value[0];
	    amt.silver = obj->value[1];
	    amt.copper = obj->value[2];
	    amt.fract = obj->value[3];
	    ccount = amt.gold + amt.silver + amt.copper + amt.fract;
	    if ( ccount == 0 )
		strcpy( buf, "Odd... there are no coins in the pile.\n\r" );
	    else if ( ccount == 1 )
		sprintf( buf, "There is%s here.\n\r",
			 money_string( &amt, FALSE, FALSE ) );
	    else
		sprintf( buf, "There are%s in the pile.\n\r",
			 money_string( &amt, FALSE, FALSE ) );
	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_DRINK_CON:
	case ITEM_PORTAL:
	case ITEM_SHEATH:
	    sprintf( buf, "in %s",argument );
	    do_function( ch, &do_look, buf );
	    break;
	}
    }

    return;
}


/*
 * Thanks to Zrin for auto-exit part.
 */
void
do_exits( CHAR_DATA *ch, char *argument )
{
    char		buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *	was_in_room;
    EXIT_DATA *		pexit;
    bool		found;
    bool		fAuto;
    int			door;
    char *		p;

    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    p = buf;
    if ( fAuto )
	p = stpcpy( p, "`RExits`W:`R" );
    else if ( IS_IMMORTAL( ch) )
	p += sprintf( p, "`WObvious exits from room %d:\n\r", ch->in_room->vnum );
    else
	p= stpcpy( p, "`WObvious exits:\n\r" );

    found = FALSE;
    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( ( pexit = get_exit( ch->in_room, door ) ) == NULL
	||   pexit->to_room == NULL
	||   !can_see_room( ch, pexit->to_room ) )
	    continue;

	if ( ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	||   ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	||   ( !IS_SET( pexit->exit_info, EX_SECRET ) && !IS_SET( pexit->exit_info, EX_HIDDEN ) ) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		*p++ = ' ';
		if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
		    *p++ = IS_SET( pexit->exit_info, EX_SECRET ) ? '<' : '[';
		    if ( IS_SET( pexit->exit_info, EX_HIDDEN ) )
			p = stpcpy( p, "`r" );
		}
		p = stpcpy( p, dir_name[door] );
		if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
		    if ( IS_SET( pexit->exit_info, EX_HIDDEN ) )
			p = stpcpy( p, "`R" );
		    *p++ = IS_SET( pexit->exit_info, EX_SECRET ) ? '>' : ']';
		}
	    }
	    else
	    {
		char *phrase;

		p = stpcpy( p, "`W" );
		if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
		    *p++ = IS_SET( pexit->exit_info, EX_SECRET ) ? '<' : '[';
		    if ( IS_SET( pexit->exit_info, EX_HIDDEN ) )
			p = stpcpy( p, "`w" );
		}
		else
		    *p++ = ' ';
		p += sprintf( p, "%-9s", capitalize( dir_name[door] ) );
		if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
		    if ( IS_SET( pexit->exit_info, EX_HIDDEN ) )
			p = stpcpy( p, "`W" );
		    *p++ = IS_SET( pexit->exit_info, EX_SECRET ) ? '>' : ']';
		}
		else
		    *p++ = ' ';
		p = stpcpy( p, " - " );

		if ( IS_IMMORTAL( ch ) )
		    p += sprintf( p, "[%6d %-7s]", pexit->to_room->vnum,
				  flag_string( size_types, pexit->size ) );

		if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
		    phrase = ROOMNAME( pexit->to_room );
		else
		    phrase = "Too dark to tell";
		p = stpcpy( p,
		    room_is_dark( pexit->to_room )
			? phrase
			: ROOMNAME( pexit->to_room )
		    );
		    p = stpcpy( p, "\n\r" );
	    }
	}
    }

    if ( !found )
	p = stpcpy( p, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	p = stpcpy( p, "\n\r" );

    p = stpcpy( p, "`w" );
    send_to_char( buf, ch );

    if ( !fAuto && ch->in_room->in_room != NULL )
    {
	was_in_room = ch->in_room;
	char_from_room( ch );
	char_to_room( ch, was_in_room->in_room );
	do_exits( ch, argument );
	char_from_room( ch );
	char_to_room( ch, was_in_room );
    }

    return;
}


void
do_immlist( CHAR_DATA *ch, char *argument )
{
    int		col;
    MYSQL	conn;
    int		lvl;
    char *	name;
    BUFFER *	pBuf;
    int		prev_lvl;
    char	query[MAX_INPUT_LENGTH];
    MYSQL_RES *	result;
    MYSQL_ROW	row;

    if ( !sql_flag )
    {
        send_to_char( "Immlist data not available right now.  Sorry.\n\r", ch );
        return;
    }

    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd,
         sql_db, sql_port, sql_socket, 0 ) == NULL )
    {
	bugf( "do_immlist: mysql_real_connect: connect failed." );
	log_string( mysql_error( &conn ) );
        send_to_char( "Immlist data not available right now.  Sorry.\n\r", ch );
	return;
    }

    sprintf( query, "SELECT name, level FROM users "
                    "WHERE level BETWEEN %d and %d "
                    "ORDER BY level DESC, name",
             LEVEL_IMMORTAL + 1, MAX_LEVEL );
    if ( mysql_query( &conn, query ) != 0 )
    {
        bugf( "do_immlist: mysql_query: query failed." );
        log_string( mysql_error( &conn ) );
        send_to_char( "Immlist data not available right now.  Sorry.\n\r", ch );
	mysql_close( &conn );
	return;
    }

    pBuf = new_buf( );
    col = 0;
    prev_lvl = 0;
    if ( ( result = mysql_store_result( &conn ) ) != NULL )
    {
        col = 0;
        prev_lvl = 0;
        while ( ( row = mysql_fetch_row( result ) ) != NULL )
        {
            lvl = row[1] != NULL ? atoi( row[1] ) : -1;
            if ( lvl != prev_lvl )
            {
                prev_lvl = lvl;
                if ( col % 4 != 0 )
                {
                    add_buf( pBuf, "\n\r" );
                    col = 0;
                }
                add_buf( pBuf, "`r-----------------------------" );
                buf_printf( pBuf, "`R[`W%-9s`R]", level_name( lvl ) );
                add_buf( pBuf, "`r-----------------------------" );
                add_buf( pBuf, "\n\r" );
            }
            name = row[0] != NULL ? row[0] : "Nobody?!?";
            buf_printf( pBuf, "    `C%c", *name );
            buf_printf( pBuf, "`c%-15s", name + 1 );
            col++;
            if ( ( col % 4 ) == 0 )
            {
                add_buf( pBuf, "\n\r" );
                col = 0;
            }
        }
        mysql_free_result( result );
    }

    mysql_close( &conn );

    if ( col != 0 )
        add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_worth( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_INPUT_LENGTH];
    MONEY	money;
    char *	p;

    money = ch->money;
    normalize( &money );

    if ( IS_NPC( ch ) )
    {
	ch_printf( ch, "You have %d %s %d %s %d %s and %d %s\n\r",
		   ch->money.gold,   ch->money.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		   ch->money.silver, ch->money.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		   ch->money.copper, ch->money.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		   ch->money.fract,  ch->money.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	act_color( AT_ACTION, "$n counts $s coins.",
	           ch, NULL, NULL, TO_ROOM, POS_RESTING );

	return;
    }

    p = buf;

    p = stpcpy( p, "You have" );
    p += sprintf( p, " %d %s", ch->money.gold,
		  ch->money.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL );
    p += sprintf( p, " %d %s", ch->money.silver,
		  ch->money.silver == 1 ? SILVER_NOUN : SILVER_PLURAL );
    p += sprintf( p, " %d %s", ch->money.copper,
		  ch->money.copper == 1 ? COPPER_NOUN : COPPER_PLURAL );
    p += sprintf( p, " and %d %s.\n\r", ch->money.fract,
		  ch->money.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
    p += sprintf( p, "You also have %d quest points.\n\r", ch->questpoints);
    if ( ch->money.gold    != money.gold
    ||	 ch->money.silver != money.silver
    ||	 ch->money.copper   != money.copper
    ||	 ch->money.fract    != money.fract
    || ( money.gold == 0 && money.silver == 0
      && money.copper == 0 && money.fract == 0 ) )
    {
	p = stpcpy( p, "That's worth" );
	if ( money.gold != 0 )
	    p += sprintf( p, " %d %s",
			  money.gold, money.gold == 1 ? GOLD_NOUN : GOLD_PLURAL );
	if ( money.silver != 0 )
	{
	    if ( money.gold != 0 && money.copper == 0 && money.fract == 0 )
		p = stpcpy( p, " and" );
	    p += sprintf( p, " %d %s",
			  money.silver, money.silver == 1 ? SILVER_NOUN : SILVER_PLURAL );
	}
	if ( money.copper != 0 )
	{
	    if ( ( money.gold != 0 || money.silver != 0 ) && money.fract == 0 )
		p = stpcpy( p, " and" );
	    p += sprintf( p, " %d %s",
			  money.copper, money.copper == 1 ? COPPER_NOUN : COPPER_PLURAL );
	}
	if ( money.fract != 0 )
	{
	    if ( money.gold != 0 || money.silver != 0 || money.copper != 0 )
		p = stpcpy( p, " and" );
	    p += sprintf( p, " %d %s",
			  money.fract, money.fract == 1 ? FRACT_NOUN : FRACT_PLURAL );
	}
	if ( money.gold  == 0 && money.silver == 0
	&&   money.copper == 0 && money.fract == 0 )
	    p = stpcpy( p, " nothing" );
	p = stpcpy( p, ".\n\r" );
    }

    if ( ch->level < LEVEL_HERO )
    {
	p += sprintf( p, "You have %d experience and are %d exp from your next level.\n\r",
		      ch->exp,
		      ( ch->level + 1 )
		      * exp_per_level( ch, ch->pcdata->points ) - ch->exp );
    }
    else
    {
	p = stpcpy( p, "You perceive that you can level no higher by natural means.\n\r" );
    }

    send_to_char( buf, ch );
    act_color( AT_ACTION, "$n counts $s coins.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    return;
}


void do_oldscore( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int i;

    sprintf( buf,
	"You are %s%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	ch->level, get_age(ch),
        ( ch->played + (int) (current_time - ch->logon) ) / 3600);
    send_to_char( buf, ch );

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );
	send_to_char( buf, ch );
    }

    sprintf(buf, "Race: %s  Sex: %s  Class: %s\n\r",
	race_table[ch->race].name,
	ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
 	IS_NPC(ch) ? "mobile" : class_table[ch->class].name);
    send_to_char(buf,ch);
	

    sprintf( buf,
	"You have %d/%d hit, %d/%d mana, %d/%d movement.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move);
    send_to_char( buf, ch );

    sprintf( buf,
	"You have %d practices and %d training sessions.\n\r",
	ch->practice, ch->train);
    send_to_char( buf, ch );

    sprintf( buf,
	"You are carrying %d/%d items with weight %ld/%d pounds.\n\r",
	ch->carry_number, can_carry_n(ch),
	get_carry_weight(ch) / 10, can_carry_w(ch) /10 );
    send_to_char( buf, ch );

    sprintf( buf,
	"Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
	ch->perm_stat[STAT_STR],
	get_curr_stat(ch,STAT_STR),
	ch->perm_stat[STAT_INT],
	get_curr_stat(ch,STAT_INT),
	ch->perm_stat[STAT_WIS],
	get_curr_stat(ch,STAT_WIS),
	ch->perm_stat[STAT_DEX],
	get_curr_stat(ch,STAT_DEX),
	ch->perm_stat[STAT_CON],
	get_curr_stat(ch,STAT_CON) );
    send_to_char( buf, ch );

    sprintf( buf,
	"You have scored %d exp, and have %ld gold, %ld silver coins, %ld copper coins and %ld copper pieces.\n\r",
	ch->exp,  ch->money.gold, ch->money.silver, ch->money.copper, ch->money.fract );
    send_to_char( buf, ch );

    /* RT shows exp to level */
    if (!IS_NPC(ch) && ch->level < LEVEL_HERO)
    {
      sprintf (buf, 
	"You need %d exp to level.\n\r",
	((ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp));
      send_to_char( buf, ch );
     }

    sprintf( buf, "Wimpy set to %d hit points.\n\r", ch->wimpy );
    send_to_char( buf, ch );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( "You are drunk.\n\r",   ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
	send_to_char( "You are thirsty.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER]   ==  0 )
	send_to_char( "You are hungry.\n\r",  ch );

    switch ( ch->position )
    {
    case POS_DEAD:     
	send_to_char( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_char( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",		ch );
	break;
    case POS_SITTING:
	send_to_char( "You are sitting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_char( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",		ch );
	break;
    }


    /* print AC values */
    if (ch->level >= 25)
    {	
	sprintf( buf,"Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
		 GET_AC(ch,AC_PIERCE),
		 GET_AC(ch,AC_BASH),
		 GET_AC(ch,AC_SLASH),
		 GET_AC(ch,AC_EXOTIC));
    	send_to_char(buf,ch);
    }

    for (i = 0; i < 4; i++)
    {
	char * temp;

	switch(i)
	{
	    case(AC_PIERCE):	temp = "piercing";	break;
	    case(AC_BASH):	temp = "bashing";	break;
	    case(AC_SLASH):	temp = "slashing";	break;
	    case(AC_EXOTIC):	temp = "magic";		break;
	    default:		temp = "error";		break;
	}
	
	send_to_char("You are ", ch);

	if      (GET_AC(ch,i) >=  101 ) 
	    sprintf(buf,"hopelessly vulnerable to %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 80) 
	    sprintf(buf,"defenseless against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 60)
	    sprintf(buf,"barely protected from %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 40)
	    sprintf(buf,"slightly armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 20)
	    sprintf(buf,"somewhat armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 0)
	    sprintf(buf,"armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -20)
	    sprintf(buf,"well-armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -40)
	    sprintf(buf,"very well-armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -60)
	    sprintf(buf,"heavily armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -80)
	    sprintf(buf,"superbly armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -100)
	    sprintf(buf,"almost invulnerable to %s.\n\r",temp);
	else
	    sprintf(buf,"divinely armored against %s.\n\r",temp);

	send_to_char(buf,ch);
    }


    /* RT wizinvis and holy light */
    if ( IS_IMMORTAL(ch))
    {
      send_to_char("Holy Light: ",ch);
      if (IS_SET(ch->act,PLR_HOLYLIGHT))
        send_to_char("on",ch);
      else
        send_to_char("off",ch);
 
      if (ch->invis_level)
      {
        sprintf( buf, "  Invisible: level %d",ch->invis_level);
        send_to_char(buf,ch);
      }

      if (ch->incog_level)
      {
	sprintf(buf,"  Incognito: level %d",ch->incog_level);
	send_to_char(buf,ch);
      }
      send_to_char("\n\r",ch);
    }

    if ( ch->level >= 15 )
    {
	sprintf( buf, "Hitroll: %d  Damroll: %d.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );
	send_to_char( buf, ch );
    }
    
    if ( ch->level >= 10 )
    {
	sprintf( buf, "Alignment: %d.  ", ch->alignment );
	send_to_char( buf, ch );
    }

    send_to_char( "You are ", ch );
         if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
    else                             send_to_char( "satanic.\n\r", ch );

    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
	do_function(ch, &do_affects, "");
}



void
do_score( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    char	arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( IS_IMMORTAL( ch ) && arg[0] != '\0' )
    {
	if ( ( vch = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They're not here.\n\r", ch );
	    return;
	}
	else if ( get_trust( vch ) > get_trust( ch ) )
	{
	    send_to_char( "You cannot.\n\r", ch );
	    return;
	}
    }
    else
	vch = ch;

    show_score( vch, ch );
}


void
show_score( CHAR_DATA *vch, CHAR_DATA *ch )
{
    int		ac;
    int		ac_table[] = { AC_PIERCE, AC_BASH, AC_SLASH, AC_EXOTIC };
    char *	ac_name [] = { "piercing", "bashing", "slashing", "magic" };
    char	buf [ MAX_STRING_LENGTH * 2];
    CLAN_DATA *	clan;
    CHAR_DATA *	fch;
    char *	where;
    char *	how;
    char *	txt;
    int		clvl;
    int		csex;
    int		i;
    bool	found = FALSE;
    char *	p;
    int		age;
    int		plr_secs;
    int		percent;

  if (IS_NPC(ch))
    {
      send_to_char("Use the oldscore function\n\r",ch);
      return;
    }
    p = stpcpy( buf, "`B******************************************************************************\n\r" );
    p = stpcpy( p, "`CYou are `W" );
    if ( IS_NPC( vch ) )
    {
	p = stpcpy( p, vch->short_descr );
    }
    else
    {
	plr_secs = vch->played + current_time - vch->logon;
	age = 17 + plr_secs / ( ( PULSE_TICK / PULSE_PER_SECOND )
			      * 24 * DAYS_PER_MONTH * MONTHS_PER_YEAR );

	p = stpcpy( p, vch->name );
	if ( !IS_NULLSTR( vch->pcdata->lname ) )
	{
	    *p++ = ',';
	    p = stpcpy( p, vch->pcdata->lname );
	}
	p = stpcpy( p, vch->pcdata->title );
	p = stpcpy( p, "`C.  Age`W: " );
	p += sprintf( p, "`B%d`W(`B%d hours`W)", age,
		     ( ( plr_secs + 1800 ) / 3600 ) );
    }
    p = stpcpy( p, "\n\r" );

    if ( !IS_NPC( vch ) )
    {
        p = stpcpy( p, PERS( vch, ch ) );
        if ( !IS_NULLSTR( vch->pcdata->lname ) )
        {
            *p++ = ' ';
            p = stpcpy( p, vch->pcdata->lname );
        }
        p = stpcpy( p, ", " );
        p = stpcpy( p, aoran( race_table[vch->race].name ) );
        p = stpcpy( p, " with " );
        p = stpcpy( p, vch->pcdata->hair_adj );
        p = stpcpy( p, " " );
        p = stpcpy( p, vch->pcdata->hair_color );
        p = stpcpy( p, " and " );
        p = stpcpy( p, vch->pcdata->eye_adj );
        p = stpcpy( p, " " );
        p = stpcpy( p, vch->pcdata->eye_color );
        p = stpcpy( p, " eyes.\n\r" );
    }
    else
        p = stpcpy( p, vch->long_descr );


    if ( !IS_NPC( vch ) )
    {
        p += sprintf( p, "`CHome: `W%s  ", capitalize( kingdom_table[vch->pcdata->kingdom].name ) );
    }

    if ( vch->clan )
    {
	clan = vch->clan;
	if ( clan->fHouse )
	    p = stpcpy( p, "`CHouse`W: " );
        else
	    p = stpcpy( p, "`CClan`W: " );
	p += sprintf( p, "%s", clan->who_name );
	if ( vch->clvl != 0 )
	{
	    p += sprintf( p, "  `CRank: `W%d", vch->clvl );
	    clvl = vch->clvl - 1;
	    csex = ( vch->sex == SEX_FEMALE );
	    if ( !IS_NULLSTR( clan->ctitle[clvl][csex] ) )
	        p += sprintf( p, " (%s)", clan->ctitle[clvl][csex] );
	}
//	if ( vch->clvl )
//	{
//	    p = stpcpy( p, "`cRank`w: `g" );
//	    switch( vch->clvl )
//	    {
//		case 5:
//		    p = stpcpy( p, clan->deity_title ); break;
//		case 4:
//		    p = stpcpy( p, clan->champ_title ); break;
//		case 3:
//		    p = stpcpy( p, clan->leader_title ); break;
//		case 2:
//		    p = stpcpy( p, clan->first_title ); break;
//		case 1:
//		    p = stpcpy( p, clan->second_title ); break;
//	    }
//	}
    }

    if ( !IS_NPC( vch ) || vch->clan != NULL )
	p = stpcpy( p, "\n\r" );

    p  = stpcpy ( p, "`B******************************************************************************\n\r" );
    p  = stpcpy ( p, "`CRace`W: `W" );
    p += sprintf( p, "%-12s", IS_NPC( vch ) ? "Mobile" : race_table[vch->race].name );
    p  = stpcpy ( p, "`CClass`W: `W" );
    p  = stpcpy ( p, IS_NPC( vch ) ? "Mobile" : class_table[vch->class].name );
    p  = stpcpy ( p, "   `CLevel`W: `W" );
    p += sprintf( p, "%-12d", vch->level );
    p  = stpcpy ( p, "`CSex`W: `W" );
    p  = stpcpy ( p, vch->sex == SEX_MALE    ? "Male"   :
		     vch->sex == SEX_FEMALE  ? "Female" : "Neutral" );
    p  = stpcpy ( p, "\n\r" );

    p  = stpcpy ( p, "`B******************************************************************************\n\r" );
    p  = stpcpy ( p, "`B| `CStr`W: `W" );
    p += sprintf( p, "%2d `W(`C%d`W) ",
		  IS_NPC( vch ) ? 13 : vch->perm_stat[STAT_STR],
		  get_curr_stat( vch, STAT_STR ) );
    if ( get_curr_stat( vch, STAT_STR ) < 10 ) *p++ = ' ';
    p  = stpcpy ( p, " `B| `CExp     `W: `W" );
    p += sprintf( p, "%-7d", vch->exp );
    p  = stpcpy ( p, " `B| `CHP      `W: `W" );
    p += sprintf( p, "%d`W/`W%d\n\r", vch->hit, vch->max_hit );

    p  = stpcpy ( p, "`B| `CInt`W: `W" );
    p += sprintf( p, "%2d `W(`C%d`W) ",
		  IS_NPC( vch ) ? 13 : vch->perm_stat[STAT_INT],
		  get_curr_stat( vch, STAT_INT ) );
    if ( get_curr_stat( vch, STAT_INT ) < 10 ) *p++ = ' ';
    p  = stpcpy ( p, " `B| `CTnl     `W: `W" );
    if ( vch->level >= LEVEL_HERO || IS_NPC( vch ) )
	p = stpcpy( p, "---    " );
    else
	p += sprintf( p, "%-7d", ( vch->level + 1 )
		      * exp_per_level( vch, vch->pcdata->points ) - vch->exp );

    p  = stpcpy ( p, " `B| " );
    p += sprintf( p, "`CMana    `W: `W%d`W/`W%d\n\r", vch->mana, vch->max_mana );

    p  = stpcpy ( p, "`B| `CWis`W: `W" );
    p += sprintf( p, "%2d `W(`C%d`W) ",
		  IS_NPC( vch ) ? 13 : vch->perm_stat[STAT_WIS],
		  get_curr_stat( vch, STAT_WIS ) );
    if ( get_curr_stat( vch, STAT_WIS ) < 10 ) *p++ = ' ';
    p  = stpcpy ( p, " `B| `PQP      `W: `W" );
    p += sprintf( p, "%-7d", vch->questpoints );
    p  = stpcpy ( p, " `B| `CMV      `W: `W" );
    p += sprintf( p, "%d`W/`W%d\n\r", vch->move, vch->max_move );

    p  = stpcpy ( p, "`B| `CDex`W: `W" );
    p += sprintf( p, "%2d `W(`C%d`W) ",
		  IS_NPC( vch ) ? 13 : vch->perm_stat[STAT_DEX],
		  get_curr_stat( vch, STAT_DEX ) );
    if ( get_curr_stat( vch, STAT_DEX ) < 10 ) *p++ = ' ';
    p  = stpcpy ( p, " `B| `YGold " );
    if ( vch->money.gold < 1000000000 ) *p++ = ' ';
    if ( vch->money.gold < 100000000  ) *p++ = ' ';
    if ( vch->money.gold < 10000000   ) *p++ = ' ';
    p  = stpcpy ( p, "`W: " );
    p += sprintf( p, "%d", vch->money.gold );
    if ( vch->money.gold < 1000000    ) *p++ = ' ';
    if ( vch->money.gold < 100000     ) *p++ = ' ';
    if ( vch->money.gold < 10000      ) *p++ = ' ';
    if ( vch->money.gold < 1000       ) *p++ = ' ';
    if ( vch->money.gold < 100        ) *p++ = ' ';
    if ( vch->money.gold < 10         ) *p++ = ' ';
    p  = stpcpy ( p, " `B| `CItems   `W: `W" );
    p += sprintf( p, "%d`W/`W%d\n\r", vch->carry_number, can_carry_n( vch ) );

    p  = stpcpy ( p, "`B| `CCon`W: `W" );
    p += sprintf( p, "%2d `W(`C%d`W) ",
		  IS_NPC( vch ) ? 13 : vch->perm_stat[STAT_CON],
		  get_curr_stat( vch, STAT_CON ) );
    if ( get_curr_stat( vch, STAT_CON ) < 10 ) *p++ = ' ';
    p  = stpcpy ( p, " `B| `WSilv    `W: `W" );
    p += sprintf( p, "%-7d", vch->money.silver );
    p  = stpcpy ( p, " `B| `CWeight  `W: " );
    p += sprintf( p, "%s", weight_string( get_carry_weight( vch ) ) );
    p += sprintf( p, "/`W%s\n\r", weight_string( can_carry_w( vch ) ) );

    p  = stpcpy ( p, "`B| `CPrac `W: `W" );
    p += sprintf( p, "%5d", vch->practice );
    p  = stpcpy ( p, "  `B| `OCopper  `W: `W" );
    p += sprintf( p, "%-7d", vch->money.copper );
    p  = stpcpy ( p, " `B| `CYou are `W: `W" );
    switch( vch->position )
    {
	case POS_DEAD:
	    p = stpcpy( p, "`RDEAD!" ); break;
	case POS_MORTAL:
	    p = stpcpy( p, "mortally wounded" ); break;
	case POS_INCAP:
	    p = stpcpy( p, "incapacitated" ); break;
	case POS_STUNNED:
	    p = stpcpy( p, "stunned" ); break;
	case POS_SLEEPING:
	    p = stpcpy( p, "sleeping " );
	    if ( vch->on )
	    {
		where = vch->on->short_descr;
		if ( IS_SET( vch->on->value[2], SLEEP_AT ) ) how = "at";
		else if ( IS_SET( vch->on->value[2], SLEEP_ON ) ) how = "on";
		else how = "in";
	    }
	    else
	    {
		how = "on";
		if ( vch->in_room->sector_type == SECT_INSIDE )
		{
		    where = "the floor";
		}
		else
		    where = "the ground";
	    }
	    p = stpcpy( p, how );
	    *p++ = ' ';
	    p = stpcpy( p, where );
	    break;
	case POS_RESTING:
	    p = stpcpy( p, "resting " );
	    if ( vch->on )
	    {
		where = vch->on->short_descr;
		if ( IS_SET( vch->on->value[2], REST_AT ) ) how = "at";
		else if ( IS_SET( vch->on->value[2], REST_ON ) ) how = "on";
		else how = "in";
	    }
	    else
	    {
		how = "on";
		if ( vch->in_room->sector_type ==  SECT_INSIDE )
		{
		    where = "the floor";
		}
		else
		    where = "the ground";
	    }
	    p = stpcpy( p, how );
	    *p++ = ' ';
	    p = stpcpy( p, where );
	    break;
	case POS_SITTING:
	    p = stpcpy( p, "sitting " );
	    if ( vch->on )
	    {
		where = vch->on->short_descr;
		if ( IS_SET( vch->on->value[2], SIT_AT ) ) how = "at";
		else if ( IS_SET( vch->on->value[2], SIT_ON ) ) how = "on";
		else how = "in";
	    }
	    else
	    {
		how = "on";
		if ( vch->in_room->sector_type == SECT_INSIDE )
		{
		    where = "the floor";
		}
		else
		    where = "the ground";
	    }
	    p = stpcpy( p, how );
	    *p++ = ' ';
	    p = stpcpy( p, where );
	    break;
	case POS_STANDING:
	    p = stpcpy( p, "standing" ); break;
	case POS_FIGHTING:
	    p = stpcpy( p, "fighting " );
	    fch = vch->fighting;
	    if ( fch )
	    {
		p = stpcpy( p, IS_NPC( fch ) ?
			    fch->short_descr :
			    fch->name );
	    }
	    else
	    {
		p = stpcpy( p, "nobody!" );
	    }
    }
    if ( vch->position != POS_DEAD ) *p++ = '.';
    p  = stpcpy ( p, "\n\r" );

    p  = stpcpy ( p, "`B| `CTrain`W: `W" );
    p += sprintf( p, "%5d", vch->train );
    p  = stpcpy ( p, "  `B| `wPieces  `W: `W" );
    p += sprintf( p, "%-7d", vch->money.fract );
    p = stpcpy ( p, " `B| `CYou are`W : `W" );
     if ( vch->ethos >  900 ) txt = "Lawful";
    else if ( vch->ethos >  700 ) txt = "Lawful";
    else if ( vch->ethos >  350 ) txt = "Lawful";
    else if ( vch->ethos >  100 ) txt = "Lawful";
    else if ( vch->ethos > -100 ) txt = "Neutral";
    else if ( vch->ethos > -350 ) txt = "Chaotic";
    else if ( vch->ethos > -700 ) txt = "Chaotic";
    else if ( vch->ethos > -900 ) txt = "Chaotic";
    else                  txt = "Chaotic";
     p = stpcpy( p, txt ); 
     txt = " ";
     p = stpcpy( p, txt ); 
	 if ( vch->alignment >  900 ) txt = "Good";
    else if ( vch->alignment >  700 ) txt = "Good";
    else if ( vch->alignment >  350 ) txt = "Good";
    else if ( vch->alignment >  100 ) txt = "Good";
    else if ( vch->alignment > -100 ) txt = "Neutral";
    else if ( vch->alignment > -350 ) txt = "Evil";
    else if ( vch->alignment > -700 ) txt = "Evil";
    else if ( vch->alignment > -900 ) txt = "Evil";
    else			      txt = "Evil";
    p = stpcpy( p, txt );
    p = stpcpy( p, "`c.\n\r" );

    p  = stpcpy ( p, "`B******************************************************************************\n\r" );
    if ( vch->level >= 12 )
    {
	p  = stpcpy ( p, "`CHitroll`W: `W" );
	p += sprintf( p, "%-4d", GET_HITROLL( vch ) );
	p  = stpcpy ( p, "  `CDamroll`W: `W" );
	p += sprintf( p, "%-4d", GET_DAMROLL( vch ) );
	found = TRUE;
    }
    if ( vch->level >= 18 )
    {
	p  = stpcpy ( p, "  `CSaving throw`W: `W" );
	p += sprintf( p, "%-4d", vch->saving_throw );
    }
    if ( vch->level >= 8 )
    {
	if ( found )
	{
	    p = stpcpy( p, "  " );
	}
	p = stpcpy( p, "`CAlign`W: `W" );
	p += sprintf( p, "%d", vch->alignment );
    p = stpcpy( p, "  `CEthos`W: `W" );
    p += sprintf( p, "%d", vch->ethos );
	found = TRUE;
    }
    if ( found )
    {
	p  = stpcpy ( p, "\n\r" );
    }

    if ( vch->level >= 20 )
    {
	p  = stpcpy(  p, "`CArmor `W:  `CPierce `W" );
	p += sprintf( p, "%d", GET_AC( vch, AC_PIERCE ) );
	p  = stpcpy(  p, "  `CBash `W" );
	p += sprintf( p, "%d", GET_AC( vch, AC_BASH ) );
	p  = stpcpy(  p, "  `CSlash `W" );
	p += sprintf( p, "%d", GET_AC( vch, AC_SLASH ) );
	p  = stpcpy(  p, "  `CMagic `W" );
	p += sprintf( p, "%d", GET_AC( vch, AC_EXOTIC ) );
	p  = stpcpy(  p, "\n\r" );
    }

    if ( !IS_NPC( vch ) )
    {
	p = stpcpy( p, "`CAutoexit`W: " );
	p = stpcpy( p, IS_SET( vch->act, PLR_AUTOEXIT ) ? "`CYes" : "`WNo" );
	p = stpcpy( p, "  `CAutoloot`W: " );
	p = stpcpy( p, IS_SET( vch->act, PLR_AUTOLOOT ) ? "`CYes" : "`WNo" );
	p = stpcpy( p, "  `CAutosac`W: " );
	p = stpcpy( p, IS_SET( vch->act, PLR_AUTOSAC ) ? "`CYes" : "`WNo" );
	p = stpcpy( p, "  `CAutocoins`W: " );
	p = stpcpy( p, IS_SET( vch->act, PLR_AUTOCOINS ) ? "`CYes" : "`WNo" );
	p = stpcpy( p, "  `CAutosplit`W: " );
	p = stpcpy( p, IS_SET( vch->act2, PLR_AUTOSPLIT ) ? "`CYes" : "`WNo" );
	p = stpcpy( p, "\n\r" );
    }

    if ( !IS_NPC( vch ) )
    {
	p += sprintf( p, "`CPage pausing set to `W%d `Clines of text.\n\r",
		      vch->lines + 2 );
    }

    for ( i = 0; i < sizeof( ac_table ) / sizeof( ac_table[0] ); i++ )
    {
	ac = ac_table[i];
	p = stpcpy ( p, "`CYou are `W" );
	     if ( GET_AC( vch, ac ) >=  200 ) txt = "`Whopelessly vulnerable `Cto`W ";
	else if ( GET_AC( vch, ac ) >=  175 ) txt = "`Wdefenseless `Cagainst`W ";
	else if ( GET_AC( vch, ac ) >=  150 ) txt = "`Wbarely protected `Cfrom`W ";
	else if ( GET_AC( vch, ac ) >=  125 ) txt = "`Wslightly armored `Cagainst`W ";
	else if ( GET_AC( vch, ac ) >=   75 ) txt = "`Wsomewhat armored `Cagainst`W ";
	else if ( GET_AC( vch, ac ) >=    0 ) txt = "`Warmored `Cagainst`W ";
	else if ( GET_AC( vch, ac ) >=  -50 ) txt = "`Wwell armored `Cagainst`W ";
	else if ( GET_AC( vch, ac ) >= -100 ) txt = "`Wvery well-armored `Cagainst`W ";
	else if ( GET_AC( vch, ac ) >= -150 ) txt = "`Wheavily armored `Cagainst`W ";
	else if ( GET_AC( vch, ac ) >= -200 ) txt = "`Wsuperbly armored `Cagainst`W ";
	else if ( GET_AC( vch, ac ) >= -250 ) txt = "`Walmost invulnerable `Cto`W ";
	else				      txt = "`Wdivinely armored `Cagainst`W ";
	p = stpcpy( p, txt );
	p = stpcpy( p, ac_name[i] );
	p = stpcpy( p, "`c.\n\r" );
    }

    if ( !IS_NPC( vch ) )
    {
	found = FALSE;
	if ( vch->pcdata->condition[COND_HUNGER] == 0 && !IS_IMMORTAL( vch ) )
	{
	    p = stpcpy( p, "`CYou are `Whungry`c.  " );
	    found = TRUE;
	}
	if ( vch->pcdata->condition[COND_THIRST] == 0 && !IS_IMMORTAL( vch ) )
	{
	    p = stpcpy( p, "`CYou are `Wthirsty`c.  " );
	    found = TRUE;
	}
	if ( vch->pcdata->condition[COND_DRUNK] > 10 )
	{
	    p = stpcpy( p, "`CYou are `Wdrunk`c.  " );
	    found = TRUE;
	}
	if ( found )
	    p = stpcpy( p, "\n\r" );

        percent = ( 100 * vch->pcdata->condition[COND_TIRED] / MAX_COND );
        if ( percent > 90 || percent < 0 || IS_IMMORTAL( vch ) )
            ;
        else if ( percent >= 75 )
            p = stpcpy( p, "`CYou feel `Wrested`c.\n\r" );
        else if ( percent >= 50 )
	    p = stpcpy( p, "`CYou are a bit `Wtired`c.\n\r" );
        else if ( percent >= 25 )
	    p = stpcpy( p, "`CYou feel `Wfatigued`c.\n\r" );
	else if ( percent >= 7 )
	    p = stpcpy( p, "`CYou are `Wtired`c.\n\r" );
	else
	    p = stpcpy( p, "`CYou are `Wasleep on your feet!\n\r" );

    }

    p  = stpcpy ( p, "`B******************************************************************************\n\r" );

    if ( !IS_NPC( vch ) && IS_IMMORTAL( vch ) )
    {
	p = stpcpy( p, "`CPoofin `W: `W" );
	if ( IS_NULLSTR( vch->pcdata->bamfin ) )
	    p += sprintf( p, "%s arrives in a swirling mist.", vch->name );
	else
	    p = stpcpy( p, vch->pcdata->bamfin );
	p = stpcpy( p, "\n\r" );

	p = stpcpy( p, "`CPoofout`W: `W" );
	if ( IS_NULLSTR( vch->pcdata->bamfout ) )
	    p += sprintf( p, "%s leaves in a swirling mist.", vch->name );
	else
	    p = stpcpy( p, vch->pcdata->bamfout );
	p = stpcpy( p, "\n\r" );

	p = stpcpy( p, "`CWizInvis `W: `W" );
	if ( vch->invis_level > 0 )
	    p += sprintf( p, "%d", vch->invis_level );
	else
	    p = stpcpy( p, "Off" );
	p = stpcpy( p, "  `CIncognito `W: `W" );
	if ( vch->incog_level > 0 )
	    p += sprintf( p, "%d", vch->incog_level );
	else
	    p = stpcpy( p, "Off" );
	p = stpcpy( p, "  `CHolylight`W: `W" );
	p = stpcpy( p, IS_SET( vch->act, PLR_HOLYLIGHT ) ? "On " : "Off" );
	p = stpcpy( p, "\n\r" );
    }

    send_to_char( buf, ch );

    if ( ch == vch && !IS_NPC( ch ) && IS_SET( ch->comm, COMM_SHOW_AFFECTS ) )
    {
	BUFFER *	pBuf;
	pBuf = new_buf( );
	show_affects( ch, vch, pBuf );
	send_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
    }

}

void
do_new_score (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int hitroll;
    int damroll;
    int align;
    char cposition[100];
    char calignment[100];

    if (ch->level <= 9)
    {
        align = 0;
        strcpy(calignment, "NULL");
    }
    else
    {
        align = ch->alignment;
        if (ch->alignment > 900)
            strcpy(calignment, "saintly");
        else if (ch->alignment > 700)
            strcpy(calignment, "very good");
        else if (ch->alignment > 350)
            strcpy(calignment, "good");
        else if (ch->alignment > 100)
            strcpy(calignment, "neutral good");
        else if (ch->alignment > -100)
            strcpy(calignment, "neutral bad");
        else if (ch->alignment > -350)
            strcpy(calignment, "bad");
        else if (ch->alignment > -700)
            strcpy(calignment, "evil");
        else if (ch->alignment > -900)
            strcpy(calignment, "chaotic");
        else
            strcpy(calignment, "satanic");
    }

    switch (ch->position)
    {
        case POS_DEAD:
            strcpy(cposition, "You are DEAD");
            break;
        case POS_MORTAL:
            strcpy(cposition, "You are Mortally Wounded");
            break;
        case POS_INCAP:
            strcpy(cposition, "You are Incapacitated");
            break;
        case POS_STUNNED:
            strcpy(cposition, "You are Stunned");
            break;
        case POS_SLEEPING:
            strcpy(cposition, "You are Sleeping");
            break;
        case POS_RESTING:
            strcpy(cposition, "You are Resting");
            break;
        case POS_SITTING:
            strcpy(cposition, "You are Sitting");
            break;
        case POS_STANDING:
            strcpy(cposition, "You are Standing");
            break;
        case POS_FIGHTING:
            strcpy(cposition, "You are Fighting");
            break;
    }

    if (ch->level >= 15)
    {
        hitroll = GET_HITROLL(ch);
        damroll = GET_DAMROLL(ch);
    }
    else
    {
        hitroll = 0;
        damroll = 0;
    }

    send_to_char ("    `C+`g------------------------------------------------------`C+`X\n\r", ch);

    sprintf (buf, "    `g|`C Scoresheet %41s `g|`X\n\r", ch->name);
    send_to_char(buf, ch);

    send_to_char ("    `C+`g------------------------`C+`g-----------------------------`C+`X\n\r", ch);

    sprintf (buf, "    `g|`c  Race:`C %-15s `g|`c Health:`C %4d `c/`C %-12d `g|`X\r\n",
	          race_table[ch->race].name,
                  ch->hit, ch->max_hit);
    send_to_char(buf, ch);

    sprintf (buf, "    `g|`c Class:`C %-15s `g|`c   Mana:`C %4d `c/`C %-12d `g|`X\n\r",
                  IS_NPC(ch) ? "mobile" : class_table[ch->class].name,
                  ch->mana, ch->max_mana);
    send_to_char(buf, ch);

    sprintf (buf, "    `g|`c   Sex:`C %-15s `g|`c   Move:`C %4d `c/`C %-12d `g|`X\n\r",
                  ch->sex == 0 ? "Sexless" : ch->sex == 1 ? "Male" : "Female",
                  ch->move, ch->max_move);
    send_to_char(buf, ch);

    sprintf(buf, "    `g|`c Level: `C%-11d `C+`g---`C+`g--------`C+`g--------------`C+     `g|`X\n\r",
                 ch->level);
    send_to_char(buf, ch);

    sprintf(buf, "    `g|`c   Age:`C %-11d `g|`c Str:`C %2d`c/`C%2d `g|`c Prce:`C %-6d `g|     |`X\n\r",
                 get_age(ch), get_curr_stat(ch, STAT_STR), ch->perm_stat[STAT_STR],
                 GET_AC(ch, AC_PIERCE));
    send_to_char(buf, ch);

    sprintf(buf, "    `g|`c   Exp:`C %-11d `g|`c Int:`C %2d`c/`C%2d `g| `cBash:`C %-6d `C+`g-----`C+`X\n\r",
                 ch->exp, get_curr_stat(ch, STAT_INT), ch->perm_stat[STAT_INT],
                 GET_AC(ch, AC_BASH));
    send_to_char(buf, ch);

    sprintf(buf, "    `C+`g--------------------`C+ `cWis:`C %2d`c/`C%2d `g|`c Slsh:`C %-6d `g|`X\n\r",
                 get_curr_stat(ch, STAT_WIS), ch->perm_stat[STAT_WIS],
                 GET_AC(ch, AC_SLASH));
    send_to_char(buf, ch);

    sprintf(buf, "    `g|`c Hitroll:`C %-9d `g|`c Dex:`C %2d`c/`C%2d `g|`c Magc:`C %-6d `g|`X\n\r",
                 hitroll, get_curr_stat(ch, STAT_DEX), ch->perm_stat[STAT_DEX],
                 GET_AC(ch, AC_EXOTIC));
    send_to_char(buf, ch);

    sprintf(buf, "    `g|`c Damroll:`C %-9d `g|`c Con:`C %2d`c/`C%2d `g|              |`X\n\r",
                 damroll, get_curr_stat(ch, STAT_CON), ch->perm_stat[STAT_CON]);
    send_to_char(buf, ch);



    send_to_char("    `C+`g--------------------`C+`g--`C+`g------------------------`C+`X\n\r", ch);

    sprintf(buf, "    `g|`c Alignment:`C %-10d `g|`c  Items:`C %4d `c/`C %-7d `g|`X\n\r",
                 align, ch->carry_number, can_carry_n(ch));
    send_to_char(buf, ch);

    sprintf(buf, "    `g|`c You are`C %-13s `g|`c Weight:`O %4d `c/`O %-7d `g|`X\n\r",
                 calignment, get_carry_weight(ch) / 10, can_carry_w(ch) / 10);
    send_to_char(buf, ch);

    send_to_char("    `O+`g-----------------------`O+`g------------------------`O+`X\n\r", ch);

    sprintf(buf, "    `g|`c Wimpy:`O %-39d `g|`X\n\r",
                 ch->wimpy);
    send_to_char(buf, ch);

    send_to_char("    `O+`g------------------------------------------------`O+`X\n\r", ch);

    send_to_char("\n\r", ch);

    if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
        do_function(ch, &do_affects, "");

}

void
show_affects( CHAR_DATA *ch, CHAR_DATA *vch, BUFFER *pBuf )
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_last = NULL;
    char	 buf[MAX_INPUT_LENGTH];

    if ( vch->affected )
    {
	sprintf( buf, "%s %s affected by the following spells:\n\r",
	    ch == vch ? "You" : IS_NPC( vch ) ? vch->short_descr : vch->name,
	    ch == vch ? "are" : "is" );
	buf[0] = UPPER( buf[0] );
	add_buf( pBuf, buf );
	for ( paf = vch->affected; paf != NULL; paf = paf->next )
	{
	    if ( paf_last != NULL && paf->type == paf_last->type )
		if ( ch->level >= 20 )
		    buf_printf( pBuf, "                      ");
		else
		    continue;
	    else
	    	buf_printf( pBuf, "Spell: %-15s", skill_table[paf->type].name );

	    if ( ch->level >= 5 )
	    {
		buf_printf( pBuf,
		    ": modifies %s by %d ",
		    affect_loc_name( paf->location ),
		    paf->modifier);
		if ( paf->duration == -1 )
		    buf_printf( pBuf, "permanently" );
		else
		    buf_printf( pBuf, "for %d hour%s", paf->duration,
				paf->duration == 1 ? "" : "s" );
	    }

	    add_buf( pBuf, "\n\r" );
	    paf_last = paf;
	}
    }
    else
    {
	sprintf( buf, "%s %s not affected by any spells.\n\r",
	    ch == vch ? "You" : IS_NPC( vch ) ? vch->short_descr : vch->name,
	    ch == vch ? "are" : "is" );
	buf[0] = UPPER( buf[0] );
	add_buf( pBuf, buf );
    }
    return;
}


void
do_affects( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    BUFFER *	pBuf;
    char	arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( IS_IMMORTAL( ch ) && arg[0] != '\0' )
    {
	if ( ( vch = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They're not here.\n\r", ch );
	    return;
	}
	else if ( get_trust( vch ) > get_trust( ch ) )
	{
	    send_to_char( "You cannot.\n\r", ch );
	    return;
	}
    }
    else
	vch = ch;

    pBuf = new_buf( );
    show_affects( ch, vch, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


void
do_time( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"It is %d o'clock %s, Day of %s of %s,\n\r%d%s the Month of %s.\n\r",
	(time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[time_info.day % DAYS_PER_WEEK],
	week_name[time_info.day / DAYS_PER_WEEK],
	day, suf,
	month_name[time_info.month]);
    send_to_char(buf,ch);
    sprintf(buf, MUD_NAME " started up at %s\rThe system time is %s\r",
	str_boot_time,
	(char *) ctime( &current_time )
	);

    send_to_char( buf, ch );
    return;
}


void
do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *p;

    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the weather from here.\n\r", ch );
	return;
    }

    p = buf + sprintf( buf, "The sky is %s and ",
		       sky_look[weather_info.sky] );

    switch( weather_info.wind_speed )
    {
	case WIND_CALM:
	    strcpy( p, "the air is calm.\n\r" );
	    break;
	case WIND_ZEPHYR:
	    sprintf( p, "light breezes blow from the %s.\n\r",
		     dir_name[weather_info.wind_dir] );
	    break;
	case WIND_LIGHT:
	    sprintf( p, "mild breezes blow from the %s.\n\r",
		     dir_name[weather_info.wind_dir] );
	    break;
	case WIND_MODERATE:
	    sprintf( p, "the wind blows from the %s.\n\r",
		     dir_name[weather_info.wind_dir] );
	    break;
	case WIND_STRONG:
	    sprintf( p, "a strong wind blows from the %s.\n\r",
		     dir_name[weather_info.wind_dir] );
	    break;
	case WIND_GALE:
	    sprintf( p, "a gale-force wind blows from the %s.\n\r",
		     dir_name[weather_info.wind_dir] );
	    break;
	case WIND_HURRICANE:
	    sprintf( p, "a hurricane blows from the %s.\n\r",
		     dir_name[weather_info.wind_dir] );
	    break;
	default:
	    strcpy( p, "A BROKEN WIND blows.\n\r" );
	    break;
    };

    send_to_char( buf, ch );

    return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
    char nohelp[MAX_STRING_LENGTH];
    int level;
    
    strcpy(nohelp, argument);
    output = new_buf();

    if ( argument[0] == '\0' )
	argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0' )
    {
	argument = one_argument(argument,argone);
	if (argall[0] != '\0')
	    strcat(argall," ");
	strcat(argall,argone);
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
    	level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

	if (level > get_trust( ch ) )
	    continue;

	if ( is_name( argall, pHelp->keyword ) )
	{
	    /* add seperator if found */
	    if (found)
		add_buf(output,
    "\n\r`C============================================================`X\n\r\n\r");
//    Someone edited my helps and put all the 0's to -1 so.... :-)
//	    if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) ) 
	    if ( pHelp->level >= -1 && str_cmp( argall, "imotd" ) )
	    {
		add_buf(output,pHelp->keyword);
		add_buf(output,"\n\r");
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( strlen( argall ) == 1 )   
	        ;     // DJR HACK - display only keywords 	       	    
	    else if ( pHelp->text[0] == '.' )
		add_buf(output,pHelp->text+1);
	    else
        add_buf(output, "\n\r`C============================================================`X\n\r");
		add_buf(output,pHelp->text);
        add_buf(output, "\n\r`C============================================================`X\n\r");
	    found = TRUE;
	    /* small hack :) */
	    if (ch->desc != NULL && ch->desc->connected != CON_PLAYING 
	    &&  		    ch->desc->connected != CON_GEN_GROUPS)
		break;
	}
    }

    if(!found)
        {
    	send_to_char("No help on that word.\n\r", ch);
        append_file( ch, NOHELP_FILE, nohelp );
        }
    else
	page_to_char(buf_string(output),ch);
    free_buf(output);
}
void
do_oldhelp( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pLimbo;
    char	argone[MAX_INPUT_LENGTH];
    char *	argnext;
    HELP_DATA *	pHelp;
    BUFFER *	pBuf;
    bool	found;
    int		helpnum;
    int		count;
    int		level;

    if ( ch->desc == NULL )
        return;

    pLimbo = get_area_data( 0 );

    if ( *argument == '\0' )
        argument = "summary";

    /* Check for "help all xxx" */
    argnext = one_argument( argument, argone );
    if ( *argnext != '\0' && !str_cmp( argone, "all" ) )
    {
        found = FALSE;
        pBuf = new_buf( );

        for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
        {
            level = pHelp->level < 0 ? -1 * pHelp->level : pHelp->level;

            if ( level > get_trust( ch ) )
                continue;

            if ( !IS_IMMORTAL( ch )
            &&	 pHelp->area != NULL
            &&	 pHelp->area != pLimbo
            &&	 ( ch->in_room == NULL || ch->in_room->area != pHelp->area ) )
                continue;

            if ( is_name( argnext, pHelp->keyword ) )
            {
                if ( found )
                    add_buf( pBuf, "\n\r`X============================================================\n\r\n\r" );
                add_buf( pBuf, pHelp->keyword );
                add_buf( pBuf, "\n\r" );
                add_buf( pBuf, *pHelp->text == '.' ? pHelp->text + 1 : pHelp->text );
                found = TRUE;
            }
        }

        if ( !found )
        {
            add_buf( pBuf, "No help on that word.\n\r" );
            if ( strlen( argnext ) <= 48 )
                append_file( ch, NOHELP_FILE, argnext );
        }
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
        return;
    }

    /* Look for exact match on keyword */
    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
        level = pHelp->level < 0 ? -1 * pHelp->level : pHelp->level;
        if ( level > get_trust( ch ) )
            continue;
        if ( pHelp->area != NULL
        &&   pHelp->area != pLimbo
        &&   ( ch->in_room == NULL || ch->in_room->area != pHelp->area ) )
            continue;
        if ( is_exact_name( argument, pHelp->keyword ) )
        {
            page_to_char( *pHelp->text == '.' ? pHelp->text + 1 : pHelp->text, ch );
            return;
        }
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
        level = pHelp->level < 0 ? -1 * pHelp->level : pHelp->level;
        if ( level > get_trust( ch ) )
            continue;
        if ( pHelp->area != NULL
        &&   pHelp->area != pLimbo
        &&   ( ch->in_room == NULL || ch->in_room->area != pHelp->area ) )
            continue;
        if ( is_name( argument, pHelp->keyword ) )
        {
            page_to_char( *pHelp->text == '.' ? pHelp->text + 1 : pHelp->text, ch );
            return;
        }
    }

    if ( IS_IMMORTAL( ch ) && is_number( argone ) && *argnext == '\0' )
    {
        helpnum = atoi( argone );
        count = 0;
        for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
        {
            count++;
            if ( count == helpnum )
            {
                pBuf = new_buf( );
                buf_printf( pBuf, "%s\n\r", pHelp->keyword );
                add_buf( pBuf, *pHelp->text == '.' ? pHelp->text + 1 : pHelp->text );
                page_to_char( buf_string( pBuf ), ch );
                free_buf( pBuf );
                return;
            }
        }
    }

    if ( strlen( argument ) <= 48 )
        append_file( ch, NOHELP_FILE, argument );
    send_to_char( "No help on that word.\n\r", ch );
    return;
}


/*
 * Who_comp, compares two chars, returns TRUE if ch1->level is higher
 * than ch2->level or ch1->level = ch2->level and ch1->name less than
 * ch2->name, else returns FALSE;
 */
bool
who_comp( CHAR_DATA *ch1, CHAR_DATA *ch2 )
{
    char *p;
    char *q;

    if ( ch1->level > ch2->level )
	return TRUE;
    if ( ch2->level > ch1->level )
	return FALSE;

    p = ch1->name;
    q = ch2->name;

    while( *p != '\0' && LOWER( *p ) == LOWER( *q ) )
    {
	p++;
	q++;
    }

    return ( LOWER( *p ) < LOWER( *q ) );
}


/*
 * Format one line of text for who list
 */
void
who_line( CHAR_DATA *ch, CHAR_DATA *wch, BUFFER *pBuf, bool fClan )
{
    char	buf[MAX_INPUT_LENGTH];
    char	buf2[SHORT_STRING_LENGTH];
    int		len;
    int		len2;
    char *	p;
    char *	s;

    p = buf;

    p = stpcpy( p, "`w[" );
    if ( wch->level < LEVEL_HERO )
    {
	p += sprintf( p, "%2d %6s %s",
	    wch->level,
	    race_table[wch->race].who_name,
	    class_table[wch->class].who_name );
    }
    else
    {
	if ( wch->pcdata->who_text == NULL || *wch->pcdata->who_text == '\0' )
	{
	    strcpy( buf2, level_name( wch->level ) );
	    buf2[0] = UPPER( buf2[0] );
	}
	else
	{
	    strcpy( buf2, wch->pcdata->who_text );
	}

//	len2 = len = strlen_wo_col( buf2 );
//	len  = ( WHOTEXT_LEN / 2 ) - len / 2;
//	len2 = ( WHOTEXT_LEN / 2 ) - len2 / 2 - ( len2 & 1 );
	len = ( WHOTEXT_LEN - strlen_wo_col( buf2 ) ) / 2;
	len2 = WHOTEXT_LEN - strlen_wo_col( buf2 ) - len;
	p += sprintf( p, "%*s%s%*s", len, "", buf2, len2, "" );
    }
    p = stpcpy( p, "`X] " );

    if ( is_ignoring( ch, wch, IGNORE_SET ) )
    {
        p = stpcpy( p, "`W(`RIgnored`W)`X" );
    }
    if ( wch->invis_level != 0 )
    {
	p += sprintf( p, "(W %d)", wch->invis_level );
    }

    if ( wch->incog_level != 0 )
    {
        p += sprintf( p, "(I %d)", wch->incog_level );
    }

    if ( IS_SET( wch->comm, COMM_AFK ) )
    {
	p = stpcpy( p, "(AFK)" );
    }

    if ( IS_DEAD( wch ) )
    {
	p = stpcpy( p, "(Spirit)" );
    }

    if ( IS_SET( wch->act2, PLR_CODING ) )
    {
	p = stpcpy( p, "`W(`CC`W)`X" );
    }

    if ( IS_SET( wch->act, PLR_QUESTOR ) )
    {
	p = stpcpy( p, "`W(`PQUEST`W)`X" );
    }

    if ( IS_SET( wch->act, PLR_KILLER ) )
    {
	p = stpcpy( p, "(KILLER)" );
    }

    if ( IS_SET( wch->act, PLR_THIEF ) )
    {
	p = stpcpy( p, "(THIEF)" );
    }

    if ( wch->desc != NULL && wch->desc->pString != NULL )
    {
	p = stpcpy( p, "`W(`gEd`W)`X" );
    }
    else if ( wch->desc != NULL && wch->desc->editor != ED_NONE )
    {
	switch( wch->desc->editor )
	{
	    default:
		p = stpcpy( p, "`W(`bB`W)`X" );
		break;
	    case RENAME_OBJECT:
		p = stpcpy( p, "`W(`gRe`W)`X" );
		break;
	    case FORGE_OBJECT:
		break;
	}
    }

    if ( *(p - 1) != ' ' )
	*p++ = ' ';

    p = stpcpy( p, wch->name );
    if ( !IS_NULLSTR( wch->pcdata->lname ) )
	p += sprintf( p, " %s", wch->pcdata->lname );

    if ( fClan && is_clan( wch ) )
    {
	p = stpcpy( p, ", " );
	if ( wch->clvl >= 1 && wch->clvl <= MAX_CLAN_LEVELS
	&&   !IS_NULLSTR( s = wch->clan->ctitle[wch->clvl - 1][wch->sex == SEX_FEMALE?1:0] ) )
	    p += sprintf( p, "%s of ", s );
	p = stpcpy( p, wch->clan->fHouse ? "House" : "Clan" );
	*p++ = ' ';
	p = stpcpy( p, wch->clan->who_name );
    }
    else
	p = stpcpy( p, wch->pcdata->title );


    p = stpcpy( p, "`w\n\r" );
    add_buf( pBuf, buf );
}


void
do_who( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    char		sName[MAX_INPUT_LENGTH];
    BUFFER *		output;
    CHAR_DATA *		wch;
    DESCRIPTOR_DATA *	d;
    CLIST_DATA *	who_first;
    CLIST_DATA *	who_char;
    CLIST_DATA *	who_tmp;
    int			cLevelsCount;
    int			cImm;
    int			cMatch;
    int			cMortal;
    int			iClass;
    int			iRace;
    int			iClan;
    int			iLevelLower;
    int			iLevelUpper;
    bool		rgfClass[MAX_CLASS];
    bool *		rgfRace;
    int			sizeClanFlags;
    int			sizeRaceFlags;
    bool *		rgfClan;	/* MUST free this before returns */
    bool		fClassRestrict = FALSE;
    bool		fClanRestrict = FALSE;
    bool		fClan = FALSE;
    bool		fNameRestrict = FALSE;
    bool		fRaceRestrict = FALSE;
    bool		fRestrict;
    bool		fImmortalOnly = FALSE;
    bool		fBrief = FALSE;
    int			julian_day;
    time_t		now;

    /*
     * Set default arguments.
     */
    sizeClanFlags  = ( top_clan + 1 ) * sizeof( bool );
    rgfClan	   = (bool *)alloc_mem( sizeClanFlags );
    sizeRaceFlags  = ( top_race + 1 ) * sizeof( bool );
    rgfRace	   = (bool *)alloc_mem( sizeRaceFlags );
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        rgfClass[iClass] = FALSE;
    for ( iRace = 0; iRace < top_race; iRace++ )
        rgfRace[iRace] = FALSE;
    for (iClan = 0; iClan <= top_clan; iClan++)
	rgfClan[iClan] = FALSE;

    /*
     * Parse arguments.
     */
    one_argument( argument, arg );
    fRestrict = arg[0] != '\0';
    cLevelsCount = 0;
    for ( ; ; )
    {
	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    switch ( ++cLevelsCount )
            {
		case 1: iLevelLower = atoi( arg ); break;
		case 2: iLevelUpper = atoi( arg ); break;
		default:
		    send_to_char( "Only two level numbers allowed.\n\r", ch );
		    free_mem( rgfRace, sizeRaceFlags );
		    free_mem( rgfClan, sizeClanFlags );
		    return;
	    }
	    continue;
	}

	if ( !str_prefix( "imm", arg ) && !str_prefix( arg, "immortals" ) )
	{
	    fImmortalOnly = TRUE;
	    continue;
	}

	iClass = class_lookup( arg );
	if ( iClass != NO_CLASS )
	{
	    rgfClass[iClass] = TRUE;
	    fClassRestrict = TRUE;
	    continue;
	}

	iRace = race_lookup( arg );
	if ( iRace != 0 && iRace < top_race )
	{
	    rgfRace[iRace] = TRUE;
	    fRaceRestrict = TRUE;
	    continue;
	}

	if ( !str_cmp( arg, "clan" )  || !str_cmp( arg, "clans" )
	||   !str_cmp( arg, "house" ) || !str_cmp( arg, "houses" ) )
	{
	    fClan = TRUE;
	    continue;
	}

	iClan = clan_lookup( arg );
	if ( iClan != 0 )
	{
	    rgfClan[iClan] = TRUE;
	    fClanRestrict = TRUE;
	    continue;
	}

	if ( !str_cmp( arg, "brief" ) )
	{
	    fBrief = TRUE;
	    continue;
	}

	if ( fNameRestrict )
	{
	    send_to_char( "You can only give one name.\n\r", ch );
	    free_mem( rgfRace, sizeRaceFlags );
	    free_mem( rgfClan, sizeClanFlags );
	    return;
	}
	else
	{
	    strcpy( sName, arg );
	    fNameRestrict = TRUE;
	    continue;
	}
    }

    /*
     * Build the list of characters to be displayed.
     * Insertion sort by level and name.
     */
    cMatch = 0;
    cImm = 0;
    cMortal = 0;
    who_first = NULL;
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING )
	    continue;
	wch = d->original ? d->original : d->character;
	if ( !can_see( ch, wch ) )
	    continue;
	if ( wch->level < iLevelLower
	||   wch->level > iLevelUpper
	|| ( fImmortalOnly && wch->level < LEVEL_IMMORTAL )
	|| ( fClassRestrict && !rgfClass[wch->class] )
	|| ( fRaceRestrict && !rgfRace[wch->race] )
	|| ( wch->level >= LEVEL_IMMORTAL && ( fClassRestrict || fRaceRestrict ) )
	|| ( fClan && !is_clan( wch ) )
	|| ( fClanRestrict && !is_clan( wch ) )
	|| ( fClanRestrict && !rgfClan[wch->clan->vnum] )
	|| ( fNameRestrict && str_prefix( sName, wch->name ) ) )
	    continue;

	who_char = new_clist_data( );
	who_char->lch = wch;

	if ( who_first == NULL || who_comp( wch, who_first->lch ) )
	{
	    who_char->next = who_first;
	    who_first = who_char;
	}
	else if ( who_first->next == NULL )
	{
	    who_char->next = NULL;
	    who_first->next = who_char;
	}
	else
	{
	    for ( who_tmp = who_first; who_tmp->next; who_tmp = who_tmp->next )
	    {
		if ( who_comp( wch, who_tmp->next->lch ) )
		{
		    who_char->next = who_tmp->next;
		    who_tmp->next = who_char;
		    break;
		}
	    }

	    if ( who_tmp->next == NULL )
	    {
		who_char->next = NULL;
		who_tmp->next = who_char;
	    }
	}

	cMatch++;
	if ( wch->level >= LEVEL_IMMORTAL )
	{
	    cImm++;
	}
	else
	{
	    cMortal++;
	}
    }

    /*
     * Now show matching chars, walking the list twice.
     * First pass show immortals, second pass show mortals.
     */
    output = new_buf();

    if ( cImm != 0 )
    {
        buf_printf( output, "\n\r%s`X\n\r`C[ `WPosition    `C]`W Name  Title \n\r`B=======================================================`X\n\r",
                    whoborder_imm[0] == '\0' ?
                    "`B====================== `CI`cm`Bm`bo`Wr`bt`Ba`cl`Cs`B ======================`X" :
                    whoborder_imm );
	for ( who_char = who_first; who_char != NULL; who_char = who_char->next )
	{
	    if ( who_char->lch->level >= LEVEL_IMMORTAL )
		who_line( ch, who_char->lch, output, fClan );
	}
    }

    if ( cMortal != 0 )
    {
        buf_printf( output, "\n\r%s`X\n\r`C[ `WL  Race  Class`C]`W Name  Title`X\n\r`B-------------------------------------------------------`X\n\r",
                    whoborder_mort[0] == '\0' ?
                    "`B----------------------- `CMortals `B-----------------------`X" :
                    whoborder_mort );
	for ( who_char = who_first; who_char != NULL; who_char = who_char->next )
	{
	    if ( who_char->lch->level < LEVEL_IMMORTAL )
		who_line( ch, who_char->lch, output, fClan );
	}
    }

    now = time( NULL );
    julian_day = localtime( &now )->tm_yday;
    if ( julian_day != today )
    {
	today = julian_day;
	most_yesterday = most_on;
	most_on = 0;
    }
    most_on = UMAX( cMatch, most_on );

    if ( fRestrict )
    {
	buf_printf( output, "\n\r%d player%s found.\n\r", cMatch,
		    cMatch == 1 ? "" : "s" );
    }
    else
    {
	buf_printf( output,
		    "\n\rYou see %d player%s on " MUD_NAME ", the most today",
		    cMatch, cMatch == 1 ? "" : "s" );
	if ( cMatch == most_on )
	    add_buf( output, ".\n\r" );
	else
	    buf_printf( output, " is %d.\n\r", most_on );
    }

    page_to_char( buf_string( output ), ch );

    free_buf( output );
    while ( who_first != NULL )
    {
	who_char = who_first;
	who_first = who_first->next;
	free_clist_data( who_char );
    }
    free_mem( rgfRace, sizeRaceFlags );
    free_mem( rgfClan, sizeClanFlags );
    return;
}


void
do_whois( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    MYSQL	conn;
    int		count;
    char	logtime[SHORT_STRING_LENGTH];
    char *	p;
    BUFFER *	pBuf;
    char	query[MAX_STRING_LENGTH];
    MYSQL_RES *	result;
    MYSQL_ROW	row;
    int		status;
    time_t	t;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Whois whom?\n\r", ch );
        return;
    }

    if ( !sql_flag )
    {
        send_to_char( "Whois information not available.  Sorry.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "self" ) )
        one_argument( ch->name, arg );

    /* allow imms to use wildcards */
    if ( IS_IMMORTAL( ch ) )
    {
        while ( ( p = strchr( arg, '*' ) ) != NULL )
            *p = '%';
    }
    else
    {
        p = arg;
        while ( *p != '\0' )
        {
            if ( !isalpha( *p ) )
                *p = '-';
            p++;
        }
    }

    if ( mysql_init( &conn ) == NULL )
    {
        send_to_char( "Whois information not available.  Sorry.\n\r", ch );
        return;
    }

    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
         sql_port, sql_socket, 0 ) == NULL )
    {
	ch_printf( ch, "Connect failure: %s\n\r", mysql_error( &conn ) );
	return;
    }

    sprintf( query, "SELECT level, class, race, name, last_log FROM users WHERE name like '%s' ORDER BY name", arg );
    status = mysql_query( &conn, query );
    count = 0;
    pBuf = new_buf( );
    if ( status == 0 )
    {
        if ( ( result = mysql_store_result( &conn ) ) != NULL )
        {
            while ( ( row = mysql_fetch_row( result ) ) != NULL )
            {
                count++;

                if ( row[4] != NULL )
                {
                    t = atol( row[4] );
                    strcpy( logtime, ctime( &t ) );
                    if ( ( p = strchr( logtime, '\n' ) ) != NULL )
                        *p = '\0';
                }
                else
                {
                    logtime[0] = '\0';
                }

                buf_printf( pBuf, "[%3d %3.3s %5.5s] %-12s %s\n\r",
			    row[0] != NULL ? atoi( row[0] ) : 0,	/* level */
			    row[1] != NULL ? row[1] : "?????",		/* class */
			    row[2] != NULL ? row[2] : "???",		/* race */
			    row[3] != NULL ? row[3] : "?????",		/* name */
			    logtime );					/* log time */
            }
            mysql_free_result( result );
        }
    }

    if ( status != 0 && IS_IMMORTAL( ch ) )
    {
        buf_printf( pBuf, "Query error: %s\n\r", mysql_error( &conn ) );
    }
    else if ( count == 0 )
    {
        add_buf( pBuf, "Player not found.\n\r" );
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    mysql_close( &conn );
    return;
}


void do_count ( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *	d;
    time_t		now;
    int			count;
    int			julian_day;

    count = 0;

    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && can_see( ch, d->character ) )
	    count++;

    now = time( NULL );
    julian_day = localtime( &now )->tm_yday;
    if ( julian_day != today )
    {
	today = julian_day;
	most_yesterday = most_on;
	most_on = 0;
    }
    most_on = UMAX( count, most_on );

    ch_printf( ch, "There %s %d character%s on `c" MUD_NAME "`X%s.\n\r",
		count == 1 ? "is" : "are",
		count,
		count == 1 ? ""   : "s",
		count == most_on ? ", the most today" : "" );
    return;
}



int
show_inventory( CHAR_DATA *ch, int item_type, BUFFER *buf, const char *hdr )
{
    int		count;
    bool	fCombine;
    int		iShow;
    int		nShow;
    OBJ_DATA *	obj;
    int *	prgnShow;
    char **	prgpstrShow;
    char *	pstrShow;

    count = 0;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	if ( obj->item_type == item_type
	&&   obj->wear_loc == WEAR_NONE
	&&   can_see_obj( ch, obj ) )
	    count++;

    if ( count == 0 )
	return 0;

    if ( hdr != NULL )
	buf_printf( buf, "\n\r  (%s)\n\r", hdr );

    prgpstrShow = (char **)alloc_mem( count * sizeof( char * ) );
    prgnShow	= (int *  )alloc_mem( count * sizeof( int    ) );
    nShow	= 0;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->item_type == item_type
	&&   obj->wear_loc == WEAR_NONE
	&&   can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, TRUE );
	    fCombine = FALSE;
	    if ( IS_NPC( ch ) || IS_SET( ch->comm, COMM_COMBINE ) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( prgpstrShow[iShow][0] == '\0' )
	{
	    free_string( prgpstrShow[iShow] );
	    continue;
	}

	if ( IS_NPC( ch ) || IS_SET( ch->comm, COMM_COMBINE ) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		buf_printf( buf, "(%2d)  ", prgnShow[iShow] );
	    }
	    else
	    {
		add_buf( buf, "      " );
	    }
	}
	add_buf( buf, prgpstrShow[iShow] );
	add_buf( buf, "`X\n\r" );
	free_string( prgpstrShow[iShow] );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof( char * ) );
    free_mem( prgnShow,    count * sizeof( int )    );

    return count;
}

void do_istat(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
        return;

    sprintf(buf, "Retrieving race information for %s%ss.\n\r",
        IS_IMMORTAL(ch) ? "immortal " : "", race_table[ch->race].name);
    send_to_char(buf, ch);
    sprintf(buf, "Stat [Base/Modified] Max[Maximum stat for race]\n\r");
    send_to_char(buf, ch);
    sprintf(buf, "`RStr`X[`R%2d`X/`R%2d`X] Max[`R%2d`X] %s\n\r",
        ch->perm_stat[STAT_STR],
        get_curr_stat(ch, STAT_STR),
        get_max_train(ch, STAT_STR),
        ch->perm_stat[STAT_STR] == get_max_train(ch, STAT_STR) ? "Maxed" : "");
    send_to_char(buf, ch);
    sprintf(buf, "`PInt`X[`P%2d`X/`P%2d`X] Max[`P%2d`X] %s\n\r",
            ch->perm_stat[STAT_INT],
            get_curr_stat(ch, STAT_INT),
            get_max_train(ch, STAT_INT),
        ch->perm_stat[STAT_INT] == get_max_train(ch, STAT_INT) ? "Maxed" : "");
    send_to_char(buf, ch);
    sprintf(buf, "`CWis`X[`C%2d`X/`C%2d`X] Max[`C%2d`X] %s\n\r",
            ch->perm_stat[STAT_WIS],
            get_curr_stat(ch, STAT_WIS),
        get_max_train(ch, STAT_WIS),
        ch->perm_stat[STAT_WIS] == get_max_train(ch, STAT_WIS) ? "Maxed" : "");
    send_to_char(buf, ch);
    sprintf(buf, "`GDex`X[`G%2d`X/`G%2d`X] Max[`G%2d`X] %s\n\r",
            ch->perm_stat[STAT_DEX],
            get_curr_stat(ch, STAT_DEX),
        get_max_train(ch, STAT_DEX),
        ch->perm_stat[STAT_DEX] == get_max_train(ch, STAT_DEX) ? "Maxed" : "");
    send_to_char(buf, ch);
    sprintf(buf, "`BCon`X[`B%2d`X/`B%2d`X] Max[`B%2d`X] %s\n\r",
            ch->perm_stat[STAT_CON],
            get_curr_stat(ch, STAT_CON),
        get_max_train(ch, STAT_CON),
        ch->perm_stat[STAT_CON] == get_max_train(ch, STAT_CON) ? "Maxed" : "");
    send_to_char(buf, ch);
    return;
}

void
do_inventory( CHAR_DATA *ch, char *argument )
{
    BUFFER *	buf;
    int		count;
    char *	hdr;
    int		i;
    int		total;

    if ( !IS_SET( ch->comm, COMM_DETAIL ) )
    {
	send_to_char( "You are carrying:\n\r", ch );
	show_list_to_char( ch->carrying, ch, TRUE, TRUE );
	return;
    }

    buf = new_buf( );

    add_buf(    buf, "Inventory:\n\r" );
    add_buf(    buf, "-------------------------------------------------\n\r" );
    buf_printf( buf, " Items:  %5d/%-5d\n\r", ch->carry_number, can_carry_n( ch ) );
    buf_printf( buf, " Weight: %s/", weight_string( get_carry_weight( ch ) ) );
    buf_printf( buf, "%s\n\r", weight_string( can_carry_w( ch ) ) );
    add_buf(    buf, "-------------------------------------------------\n\r" );

    hdr = NULL;
    total = 0;
    for ( i = 0; inventory_table[i].type >= 0; i++ )
    {
	if ( inventory_table[i].hdr != NULL )
	    hdr = inventory_table[i].hdr;
	count = show_inventory( ch, inventory_table[i].type, buf, hdr );
	total += count;
	if (  count != 0 )
	    hdr = NULL;
    }

    if ( total == 0 )
	add_buf( buf, "      Nothing." );

    add_buf(    buf, "\n\r-------------------------------------------------\n\r" );
    page_to_char( buf_string( buf ), ch );
    free_buf( buf );

    return;
}


void
do_equipment (CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int 	iWear;
    int         index;
    CHAR_DATA *victim;

    if ( IS_IMMORTAL ( ch ) && argument[0] )
    {
        victim = get_char_world ( ch, argument );
        if ( victim == NULL )
        {
            send_to_char ( "They're not in the game\n\r", ch );
            return;
        }
        act_new ( "$N is wearing:", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
    }
    else
    {
        victim = ch;
        send_to_char( "You are using:\n\r\n\r", ch );
    }

    for ( index = 0; index < MAX_WEAR; index++ )
    {

        iWear = wear_order[index];
        send_to_char ( where_name[iWear], ch );

        if ( ( obj = get_eq_char ( victim, iWear ) ) == NULL )
        {
	    send_to_char ( "----\n\r", ch );
        }
        else if ( can_see_obj ( ch, obj ) )
        {
            send_to_char ( format_obj_to_char ( obj, ch, TRUE ), ch );
            send_to_char ( "\n\r", ch );
            if ( obj->item_type == ITEM_SHEATH
	    &&	 obj->contains != NULL
	    &&	 !IS_SET( obj->value[1], SHEATH_CONCEAL )
	    &&	 !IS_SET( obj->value[1], CONT_CLOSED )
	    &&	 can_see_obj( ch, obj->contains ) )
	    {
		send_to_char( in_sheath, ch );
		send_to_char( format_obj_to_char( obj->contains, ch, TRUE ), ch );
		send_to_char( "\n\r", ch );
	    }
        }
        else
        {
            send_to_char ( "Something.\n\r", ch );
        }
    }
    send_to_char ( "\n\r", ch );
    return;
}


void
do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if (arg2[0] == '\0')
    {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
	{
	    if (obj2->wear_loc != WEAR_NONE
	    &&  can_see_obj(ch,obj2)
	    &&  obj1->item_type == obj2->item_type
	    &&  (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if (obj2 == NULL)
	{
	    send_to_char("You aren't wearing anything comparable.\n\r",ch);
	    return;
	}
    }

    else if ( (obj2 = get_obj_carry(ch,arg2,ch) ) == NULL )
    {
	send_to_char("You do not have that item.\n\r",ch);
	return;
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    break;

	case ITEM_WEAPON:
            value1 = (1 + obj1->value[2]) * obj1->value[1] + obj1->value[5];
            value2 = (1 + obj2->value[2]) * obj2->value[1] + obj2->value[5];
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}


void do_credits( CHAR_DATA *ch, char *argument )
{
    do_function(ch, &do_help, "diku" );
    do_function(ch, &do_help, "merc" );
    do_function(ch, &do_help, "rom" );
    do_function(ch, &do_help, "mars" );
    return;
}

void
do_where( CHAR_DATA *ch, char *argument )
{
    char		buf[MAX_STRING_LENGTH];
    char		arg[MAX_INPUT_LENGTH];
    CHAR_DATA *		victim;
    DESCRIPTOR_DATA *	d;
    bool		found;

    ch_printf( ch, "You are in %s`X on %s.\n\r",
               ch->in_room != NULL ? ch->in_room->area->name : "(none)",
               landmass_name( get_landmass( ch->in_room ) ) );

    if ( !IS_IMMORTAL( ch ) )
    {
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	ch_printf( ch, "Players near you in %s`X:\n\r", ch->in_room->area->name );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room != NULL
	    &&   !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
 	    &&   (is_room_owner(ch,victim->in_room)
	    ||    !room_is_private(victim->in_room))
	    &&   victim->in_room->area == ch->in_room->area
	    &&	 ( is_same_landmass( ch->in_room, victim->in_room )
		 || ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, ROOMNAME( victim->in_room ) );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
	    if ( victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    PERS(victim, ch), ROOMNAME( victim->in_room ) );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}


void
do_consider( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    char *	msg;
    int		diff;
    int		hpdiff;
    int		damdiff;
    char *	word;
    char *      word2;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
    {
	send_to_char( "Don't even think about it.\n\r", ch );
	return;
    }

    if ( ch->hit <= 0 )
    {
	send_to_char( "Forget it, you won't be fighting anybody for a while.\n\r", ch );
	return;
    }
    else if ( victim->hit <= 0 )
    {
	act( "Never mind, $N will expire soon anyway.", ch, NULL, victim, TO_CHAR );
	return;
    }

    diff = victim->level - ch->level;

         if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if ( diff <=   9 ) msg = "Oh boy, this is gonna be tough.";
    else if ( diff <=  12 ) msg = "$N laughs at you mercilessly.";
    else if ( diff <=  25 ) msg = "You got to be kidding!";
    else                    msg = "Death will thank you for your gift.";

    act( msg, ch, NULL, victim, TO_CHAR );

    hpdiff = ( 100 * ch->hit ) / victim->hit;
    if ( ( diff >= 0 && hpdiff <= 100 ) || ( diff <= 0 && hpdiff >= 100 ) )
	word = "Also";
    else
	word = "However";

    if ( hpdiff >= 301 ) msg = "$t, $E is of a very fragile constitution.";
    if ( hpdiff <= 300 ) msg = "$t, you are currently much healthier than $E.";
    if ( hpdiff <= 200 ) msg = "$t, you are currently healthier than $E.";
    if ( hpdiff <= 150 ) msg = "$t, you are currently slightly healthier than $E.";
    if ( hpdiff <= 125 ) msg = "$t, you are a teensy bit healthier than $E.";
    if ( hpdiff <= 110 ) msg = "$t, you have about the same health as $E.";
    if ( hpdiff <=  90 ) msg = "$t, $E is a teensy bit healthier than you.";
    if ( hpdiff <=  80 ) msg = "$t, $E is currently slightly healthier than you.";
    if ( hpdiff <=  67 ) msg = "$t, $E is currently healthier than you.";
    if ( hpdiff <=  50 ) msg = "$t, $E is currently much healthier than you.";
    if ( hpdiff <=  33 ) msg = "$t, $E ridicules your hit points.";

    act( msg, ch, word, victim, TO_CHAR );
 /* This bit is quite long and involved, for working out damage dealt */

    int theirdam = (ch->level/4) * (ch->level/8) + ch->level + 2;

    OBJ_DATA * wield = get_eq_char (ch, WEAR_WIELD);

    if (IS_NPC (victim))
        theirdam = dice (victim->damage[DICE_NUMBER], victim->damage[DICE_TYPE]);
    else
    {
        wield = get_eq_char (victim, WEAR_WIELD);
        if (wield == NULL)
        {
            /* unarmed */
            theirdam = number_range (victim->level / 2, victim->level * 3 / 2);
        }
        else
        {
            /* armed */
            theirdam = dice (wield->value[1], wield->value[2]);
        }

        if (get_skill (victim, gsn_enhanced_damage) > 0)
            theirdam *= 2;
    }

    int yourdam = (ch->level/4) * (ch->level/8) + ch->level + 2;;

    if (IS_NPC (ch))
        yourdam = dice (ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE]);
    else
    {
        wield = get_eq_char (ch, WEAR_WIELD);
        if (wield == NULL)
        {
            /* unarmed */
            yourdam = number_range (ch->level / 2, ch->level * 3 / 2);

        }
        else
        {
            /* armed */
            yourdam = dice (wield->value[1], wield->value[2]);
        }

        if (get_skill (ch, gsn_enhanced_damage) > 0)
            theirdam *= 2;
    }

    /* All of the above was setting it up.  It should be noted that even after all of that,
     * It doesn't take into account number of attacks, mana and a variety of other things.
     * Now for the damage messages: */

    damdiff = (theirdam / yourdam) * 100;
    if ( ( diff >= 0 && damdiff <= 100 ) || ( diff <= 0 && damdiff >= 100 ) )
        word2 = "So";
    else
        word2 = "Therefor";

    /* i.e. diff now is a percentage of your damage comparison */


         if (damdiff <= 20) msg = "$t, $N`O would be slaughtered by you`X";
    else if (damdiff <= 50)  msg = "$t, $N`Y has no hope against your strength`X";
    else if (damdiff <= 80)  msg = "$t, $N`c has less strength than you`X";
    else if (damdiff <= 120) msg = "$t, $N`C has about the same strength as you`X";
    else if (damdiff <= 150) msg = "$t, $N`g is stronger than you`X";
    else if (damdiff <= 200) msg = "$t, $N`G is a lot stronger than you`X";
    else if (damdiff <= 300) msg = "$t, $N`r is considerably stronger than you`X";
    else if (damdiff <= 500) msg = "$t, $N`W would slaughter you`X";
    else                     msg = "$t, `RAttacking $N would be suicide`X";

    act (msg, ch, word2, victim, TO_CHAR);

    return;
}


void
set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}


void
do_title( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    strcpy( buf, argument );

    if ( strlen_wo_col( buf ) > 45 )
    {
	send_to_char( "Sorry, too long.\n\r", ch );
	return;
    }

    smash_tilde( buf );
    set_title( ch, buf );
    send_to_char( "Ok.\n\r", ch );
}


void
do_description( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Silly mob, only a builder can change your description.\n\r", ch );
	return;
    }

    string_append( ch, &ch->description );
}


void
do_report( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_INPUT_LENGTH];

    sprintf( buf,
	"`WYou say '`GI have %d/%d hp %d/%d mana %d/%d mv %d xp.`W'",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    act_color( AT_SAY, buf, ch, NULL, NULL, TO_CHAR, POS_RESTING );

    sprintf( buf, "`W$n says '`GI have %d/%d hp %d/%d mana %d/%d mv %d xp.`W'",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    act_color( AT_SAY, buf, ch, NULL, NULL, TO_ROOM, POS_RESTING );

    return;
}

void do_practice (CHAR_DATA * ch, char *argument)                                                            
{                                                                                                            
    char buf[MAX_STRING_LENGTH];                                                                             
    int sn;
    //int spn = 0;
                                                                                                  
                                                                                                             
    if (IS_NPC (ch))                                                                                         
        return;                                                                                              
                                                                                                             
    if (argument[0] == '\0')                                                                                 
    {                                                                                                        
        int col;                                                                                             
                                                                                                             
        col = 0;
      /*
        send_to_char("`W+`B**************************************`C SPELLS `B***********************************`W+\n\r", ch);

        for (sn = 0; sn < top_skill; sn++)
        {
            if (skill_table[sn].name == NULL)
                break;
             if (ch->level < skill_table[sn].skill_level[ch->class]                                           
                || ch->pcdata->skill[sn].percent < 1  )                                    
                continue; 
            if ((skill_table[sn].spell_fun == spell_null) && (spn == 0))
            {
                spn++;
                send_to_char("\n\r`W+`B**************************************`C SKILLS `B***********************************`W+\n\r", ch);
                col = 0;
            }


            sprintf (buf, "  `C%-18s `B%3d%%  `X",
                     skill_table[sn].name, ch->pcdata->skill[sn].percent);
            send_to_char (buf, ch);


            if (++col % 3 == 0)
                send_to_char ("\n\r", ch);

        }

        if (col % 3 != 0)
            send_to_char ("\n\r", ch);

        sprintf (buf, "`W+`B************************************** `CPractices`z: `R%-3d `B***************************`W+`X\n\r",
                 ch->practice);
        send_to_char (buf, ch);
    }      
        */                                                                                       
        for (sn = 0; sn < top_skill; sn++)                                                                   
        {                                                                                                    
            if (skill_table[sn].name == NULL)                                                                
                break;                                                                                       
            if (ch->level < skill_table[sn].skill_level[ch->class]                                           
                || ch->pcdata->skill[sn].percent < 1  )                                    
                continue;                                                                                    
                                                                                                             
            sprintf (buf, "%-18s %3d%%  ",                                                                   
                     skill_table[sn].name, ch->pcdata->skill[sn].percent);                                         
            send_to_char (buf, ch);                                                                          
            if (++col % 3 == 0)                                                                              
                send_to_char ("\n\r", ch);                                                                   
        }                                                                                                    
                                                                                                             
        if (col % 3 != 0)                                    
            send_to_char ("\n\r", ch);

        sprintf (buf, "You have %d practice sessions left.\n\r",
                 ch->practice);
        send_to_char (buf, ch);                                                                              
    }        
                                                                                               
    else                                                                                                     
    {                                                                                                        
        CHAR_DATA *mob;                                                                                      
        int adept;                                                                                           
                                                                                                             
        if (!IS_AWAKE (ch))                                                                                  
        {                                                                                                    
            send_to_char ("In your dreams, or what?\n\r", ch);                                               
            return;                                                                                          
        }                                                                                                    
                                                                                                             
        for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)                                
        {                                                                                                    
            if (IS_NPC (mob) && IS_SET (mob->act, ACT_PRACTICE))                                             
                break;                                                                                       
        }                                                                                                    
                                                                                                             
        if (mob == NULL)                                                                                     
        {                                                                                                    
            send_to_char ("You can't do that here.\n\r", ch);                                                
            return;                                                                                          
        }                                                                                                    
                                                                                                             
        if (ch->practice <= 0)                                                                               
        {                                                                                                    
            send_to_char ("You have no practice sessions left.\n\r", ch);                                    
            return;                                                                                          
        }                                                                                                    
                                                                                                             
        if ((sn = find_spell (ch, argument)) < 0 || (!IS_NPC (ch)                                            
                                                     && (ch->level <                                         
                                                         skill_table                                         
                                                         [sn].skill_level                                    
                                                         [ch->class]                                         
                                                         || ch->                                             
                                                         pcdata->skill[sn].percent < 1    /* skill is not known */
                                                         ||                                                  
                                                         skill_table                                         
                                                         [sn].rating[ch->                                    
                                                                     class] ==                               
                                                         0)))                                                
        {                                                                                                    
            send_to_char ("You can't practice that.\n\r", ch);                                               
            return;                                                                                          
        }
                                                                                                      
        adept = IS_NPC (ch) ? 100 : class_table[ch->class].skill_adept;                                      
                                                                                                             
        if (ch->pcdata->skill[sn].percent >= adept)                                                                
        {                                                                                                    
            sprintf (buf, "You are already learned at %s.\n\r",                                              
                     skill_table[sn].name);                                                                  
            send_to_char (buf, ch);                                                                          
        }                                                                                                    
        else                                                                                                 
        {                                                                                                    
            ch->practice--;                                                                                  
            ch->pcdata->skill[sn].percent +=                                                                       
                int_app[get_curr_stat (ch, STAT_INT)].learn /                                                
                skill_table[sn].rating[ch->class];                                                           
            if (ch->pcdata->skill[sn].percent < adept)                                                             
            {                                                                                                
                act ("You practice $T.",                                                                     
                     ch, NULL, skill_table[sn].name, TO_CHAR);                                               
                act ("$n practices $T.",                                                                     
                     ch, NULL, skill_table[sn].name, TO_ROOM);                                               
            }                                                                                                
            else                                                                                             
            {                                                                                                
                ch->pcdata->skill[sn].percent = adept;                                                             
                act ("You are now learned at $T.",                                                           
                     ch, NULL, skill_table[sn].name, TO_CHAR);                                               
                act ("$n is now learned at $T.",                                                             
                     ch, NULL, skill_table[sn].name, TO_ROOM);                                               
            }                                                                                                
        }                                                                                                    
    }                                                                                                        
    return;                                                                                                  
}


/*
void
do_practice( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    BUFFER *	pBuf;
    int		col;
    int		i;
    int		max_gain;
    int		psn;
    int		sn;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "That's something you needn't worry about.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	pBuf = new_buf( );
	col    = 0;
	for ( sn = 1; sn < top_skill; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if ( ch->level < skill_table[sn].skill_level[ch->class]
	      || ch->pcdata->skill[sn].percent < 1)
		continue;

	    buf_printf( pBuf, "%-20s %3d%% ",
		skill_table[sn].name, ch->pcdata->skill[sn].percent );
	    if ( ++col % 3 == 0 )
		add_buf( pBuf, "\n\r" );
	}

	if ( col % 3 != 0 )
	    add_buf( pBuf, "\n\r" );

	buf_printf( pBuf, "You have %d practice sessions left.\n\r",
	    ch->practice );
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
    }
    else
    {
	CHAR_DATA *mob;
	int adept;

	if ( !IS_AWAKE( ch ) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	{
	    if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
		break;
	}

	if ( mob == NULL )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    send_to_char( "You have no practice sessions left.\n\r", ch );
	    return;
	}

	if ( !xIS_SET( mob->pIndexData->train, ch->class ) )
	{
	    ch_printf( ch, "%s cannot do that here.\n\r",
                       capitalize( aoran( class_table[ch->class].name ) ) );
	    return;
	}

	if ( ( sn = find_spell( ch,argument ) ) < 0
	|| ( !IS_NPC(ch)
	&&   (ch->level < skill_table[sn].skill_level[ch->class]
 	||    ch->pcdata->skill[sn].percent < 1 
	||    skill_table[sn].rating[ch->class] == 0)))
	{
	    send_to_char( "You can't practice that.\n\r", ch );
	    return;
	}

//	adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;
	adept = MAX_PRAC_PERCENT;
	max_gain = adept;

	if ( ch->pcdata->skill[sn].percent >= adept )
	{
	    sprintf( buf, "You can be trained no higher in %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else
	{
	    for ( i = 0; i < MAX_PREREQ; i++ )
	    {
		if ( ( psn = skill_table[sn].prereq[i] ) != 0 )
		{
		    if ( ch->pcdata->skill[psn].percent <= ch->pcdata->skill[sn].percent )
		    {
			ch_printf( ch, "You cannot practice %s until you improve in %s.\n\r",
				   skill_table[sn].name,
				   skill_table[psn].name );
			return;
		    }
		    else
		    {
			max_gain = UMIN( max_gain, ch->pcdata->skill[psn].percent );
		    }
		}
	    }

	    ch->practice--;
	    ch->pcdata->skill[sn].percent +=
		int_app[get_curr_stat(ch,STAT_INT)].learn /
	        skill_table[sn].rating[ch->class];

	    if ( ch->pcdata->skill[sn].percent > max_gain )
		ch->pcdata->skill[sn].percent = max_gain;

	    if ( ch->pcdata->skill[sn].percent < adept )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	    else
	    {
		ch->pcdata->skill[sn].percent = adept;
		act( "You are now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n is now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }

	    ch->pcdata->skill[sn].usage++;
	    if ( ch->pcdata->skill[sn].learned_age == 0 )
		ch->pcdata->skill[sn].learned_age = GET_AGE( ch );
	}
    }
    return;
}
*/


/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit/2 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    ch_printf( ch, "Wimpy set to %d hit points.\n\r", wimpy );
    return;
}


void
do_password( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char *	pwdnew;
    char *	p;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "You don't have a password.\n\r", ch );
	return;
    }

    argument = first_arg( argument, arg1, FALSE );
    argument = first_arg( argument, arg2, FALSE );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen( arg2 ) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}


void old_areas( CHAR_DATA *ch, char *argument );
void
old_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea1;
    AREA_DATA *	pArea2;
    BUFFER *	pBuf;
    char	arg[MAX_INPUT_LENGTH];
    char	buf[SHORT_STRING_LENGTH];
    int		iArea = 0;
    int		iAreaHalf;
    bool	fRestrict = FALSE;
    int		level = 0;
    bool	fHere;

    one_argument( argument, arg );
    if ( arg[ 0 ] != '\0' )
    {
	fRestrict = TRUE;
	if ( !str_cmp( arg, "." ) )
	    level = ch->level;
	else
	    level = atoi( arg );
	if ( level < 1 || level > MAX_LEVEL )
	{
	    send_to_char( "No such level.\n\r", ch );
	    return;
	}
    }

    /* Count up the areas to be listed */
    for ( pArea1 = area_first; pArea1; pArea1 = pArea1->next )
    {
	if ( IS_SET( pArea1->area_flags, AREA_PROTOTYPE ) )
	    continue;
	if ( fRestrict && ( pArea1->low_level > level || pArea1->high_level < level ) )
	    continue;
	iArea++;
    }

    /* Calculate the halfway point */
    iAreaHalf = ( iArea + 1 ) / 2;
    pArea2    = area_first;

    /* Position pArea1 to the first area */
    for ( pArea1 = area_first ; pArea1; pArea1 = pArea1->next )
    {
	if ( IS_SET( pArea1->area_flags, AREA_PROTOTYPE ) )
	    continue;
	if ( fRestrict && ( pArea1->low_level > level || pArea1->high_level < level ) )
	    continue;
	break;
    }

    /* Position pArea2 to the halfway mark */
    for ( iArea = 0; pArea2; pArea2 = pArea2->next )
    {
	if ( IS_SET( pArea2->area_flags, AREA_PROTOTYPE ) )
	    continue;
	if ( fRestrict && ( pArea2->low_level > level || pArea2->high_level < level ) )
	    continue;
	iArea++;
	if ( iArea >= iAreaHalf + 1 )
	    break;
    }

    /* Now display the areas */
    pBuf = new_buf ( );
    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
	fHere = ch->in_room != NULL && ch->in_room->area == pArea1;
	strcpy( buf, show_area( pArea1, fHere ) );
	fHere = ch->in_room != NULL && ch->in_room->area == pArea2;
	buf_printf( pBuf, "%s %s\n\r", buf, show_area( pArea2, fHere ) );

	/* advance to next area for first column */
	/* ugly, but it works */
	for ( pArea1 = pArea1->next; pArea1; pArea1 = pArea1->next )
	{
	    if ( IS_SET( pArea1->area_flags, AREA_PROTOTYPE ) )
		continue;
	if ( fRestrict && ( pArea1->low_level > level || pArea1->high_level < level ) )
	    continue;
	    break;
	}
	/* advance to next area for second column */
	if ( pArea2 )
	{
	    for ( pArea2 = pArea2->next; pArea2; pArea2 = pArea2->next )
	    {
		if ( IS_SET( pArea2->area_flags, AREA_PROTOTYPE ) )
		    continue;
		if ( fRestrict && ( pArea2->low_level > level || pArea2->high_level < level ) )
		    continue;
		break;
	    }
	}
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    BUFFER *	pBuf;
    char	arg[MAX_INPUT_LENGTH];
    bool	fRestrict;
    int		level;
    bool	fHere;

    one_argument( argument, arg );
    if ( arg[0] != '\0' )
    {
        fRestrict = TRUE;
        if ( !str_cmp( arg, "." ) )
            level = ch->level;
        else
            level = atoi( arg );
        if ( level < 1 || level > MAX_LEVEL )
        {
            send_to_char( "No such level.\n\r", ch );
            return;
        }
    }
    else
    {
        fRestrict = FALSE;
        level = 0;
    }

    pBuf = new_buf( );
    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
        if ( IS_SET( pArea->area_flags, AREA_PROTOTYPE ) )
            continue;
        if ( fRestrict && ( pArea->low_level > level || pArea->high_level < level ) )
            continue;
        fHere = ch->in_room != NULL && ch->in_room->area == pArea;
        if ( IS_SET( pArea->area_flags, AREA_NOLIST ) && !fHere )
            continue;
        buf_printf( pBuf, "%s\n\r", show_area( pArea, fHere ) );
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


static char *
show_area( AREA_DATA *pArea, bool fHere )
{
    static char	buf  [ MAX_INPUT_LENGTH ];
    int		count;
    char *	p;

    if ( pArea == NULL )
	return "";

    p = buf + sprintf( buf,
                fHere ? "`R>`W%s`R<%s`G" : "{`W%s`w}%s`G",
		level_range( pArea ),
		" " );
    p = stpcpy( p, pArea->name );
    p = stpcpy( p, "`Y" );

    count = 60 - strlen( pArea->credits ) - strlen_wo_col( buf );
    while ( count > 0 )
    {
	*p++ = ' ';
	count--;
    }

    p = stpcpy( p, pArea->credits );
    strcpy( p, "`X" );
    return buf;
}


const char *
level_range( AREA_DATA *pArea )
{
    static char buf[12];

	 if ( pArea == NULL )
	strcpy( buf, "None " );
    else if ( pArea->low_level == 1 && pArea->high_level >= LEVEL_HERO )
	strcpy( buf, " ALL " );
    else if ( pArea->low_level == -1 && pArea->high_level == -1 )
	strcpy( buf, "CLAN " );
    else if ( pArea->low_level == 0 && pArea->high_level == 0 )
	strcpy( buf, "None " );
    else if ( pArea->low_level >= LEVEL_IMMORTAL )
	strcpy( buf, " IMM " );
    else if ( pArea->low_level >= LEVEL_HERO )
	strcpy( buf, "HERO " );
    else if ( pArea->high_level >= LEVEL_HERO )
	sprintf( buf, "%2d-HE", pArea->low_level );
    else
	sprintf( buf, "%2d-%-2d", pArea->low_level, pArea->high_level );

    return buf;
}


void
do_kingdoms( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    char		buf[MAX_INPUT_LENGTH];
    const char *	format;
    char *		p;
    int			count;
    int			k;
    int			kStart;
    int			kEnd;
    bool		found;
    bool		fAll;
    bool		fCol;
    bool		fHere;

    pBuf = new_buf( );

    kStart = 0;
    kEnd = MAX_KINGDOM - 1;
    fAll = FALSE;

    fCol = FALSE;
    for ( k = kStart; k <= kEnd; k++ )
    {
        found = FALSE;
        for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
        {
            if ( pArea->kingdom != k )
                continue;
            if ( IS_SET( pArea->area_flags, AREA_PROTOTYPE ) && !IS_IMMORTAL( ch ) )
                continue;
            fHere = ch->in_room != NULL && ch->in_room->area == pArea;
            if ( !fHere && IS_SET( pArea->area_flags, AREA_NOLIST ) && !fAll )
                continue;
            if ( !found )
            {
                found = TRUE;
                if ( fCol )
                {
                    add_buf( pBuf, "\n\r" );
                    fCol = FALSE;
                }
                buf_printf( pBuf, "\n\r`%c", kingdom_table[k].color );
                add_buf( pBuf,
"-----------------------------------------------------------------------------\n\r" );
                buf_printf( pBuf, "`w                    %s\n\r",
                            kingdom_table[k].desc );
                buf_printf( pBuf, "`%c", kingdom_table[k].color );
                add_buf( pBuf,
"-----------------------------------------------------------------------------\n\r" );
            }
            if ( fHere )
                format = "`R>`W%s`R< `G";
            else
                format = "`w{`W%s`w} `G";
            p = buf + sprintf( buf, format, level_range( pArea ) );
            p = stpcpy( p, pArea->name );
            count = 39 - strlen_wo_col( buf );
            while ( count > 0 )
            {
                *p++ = ' ';
                count--;
            }
            *p = '\0';
            add_buf( pBuf, buf );
            fCol = !fCol;
            if ( fCol )
                add_buf( pBuf, " " );
            else
                add_buf( pBuf, "\n\r" );
        }
    }
    if ( fCol )
        add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_matrix( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    int		iClass;
    int		iRace;
    int		ccount[MAX_CLASS];
    int		rcount;
    bool	fAllow;

    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        ccount[iClass] = 0;

    pBuf = new_buf( );

    add_buf( pBuf, "            " );
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
        buf_printf( pBuf, "%s   ", class_table[iClass].who_name );
    }
    add_buf( pBuf, "\n\r" );

    add_buf( pBuf, "            " );
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
        buf_printf( pBuf, "%s   ", align_str(
                                            class_table[iClass].min_align,
                                            class_table[iClass].max_align ) );
    }
    add_buf( pBuf, "\n\r" );
    add_buf( pBuf, "            " );
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
        add_buf( pBuf, "---   " );
    }
    add_buf( pBuf, "\n\r" );

    for ( iRace = 0; !IS_NULLSTR( race_table[iRace].name ); iRace++ )
    {
        if ( !race_table[iRace].pc_race )
            continue;
        rcount = 0;
        buf_printf( pBuf, "%-5s", race_table[iRace].who_name );
        buf_printf( pBuf, " %s | ", align_str( race_table[iRace].min_align, race_table[iRace].max_align ) );

        for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        {
            if ( race_table[iRace].class_mult[iClass] != 0 )
            {
                rcount++;
                ccount[iClass]++;
                fAllow = TRUE;
            }
            else
                fAllow = FALSE;
            buf_printf( pBuf, " %s  | ", fAllow ? "Y" : " " );
        }
        buf_printf( pBuf, "%2d", rcount );
        add_buf( pBuf, "\n\r" );

        add_buf( pBuf, "            " );
        for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        {
            add_buf( pBuf, "---   " );
        }
        add_buf( pBuf, "\n\r" );

    }

    add_buf( pBuf, "            " );
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
        buf_printf( pBuf, "%s   ", class_table[iClass].who_name );
    }
    add_buf( pBuf, "\n\r" );

    add_buf( pBuf, "            " );
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
        buf_printf( pBuf, "%3d   ", ccount[iClass] );
    }
    add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_slist ( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    int		class_list[MAX_CLASS];
    int *	skill_list;
    char	arg[ MAX_INPUT_LENGTH];
    int		iClass;
    int		nClass;
    int		iSkill;
    int		nSkill;
    int		level;
    bool	fFound;
    int		col = 0;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "You don't need any stinking spells!\n\r", ch );
	return;
    }

    /* Determine what class(es) to list */
    if ( argument[0] == '\0' )
    {
//	/* no argument, copy the char's class list */
//	for ( nClass = 0; nClass < ch->nclass; nClass++ )
//	    class_list[ nClass ] = ch->class[ nClass ];

	class_list[0] = ch->class;
	nClass = 1;
    }
    else
    {
	/* arg list supplied, parse up to MAX_CLASS args */
	for ( nClass = 0; nClass < MAX_CLASS; nClass++ )
	{
	    argument = one_argument( argument, arg );
	    if ( arg[0] == '\0' )
		break;
	    if ( strlen( arg ) < 3 )
	    {
		send_to_char( "That's not a class!\n\r", ch );
		return;
	    }
	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    {
		if ( IS_NULLSTR( class_table[iClass].name ) )
		    continue;
		if ( IS_SET( class_table[iClass].flags, CLASS_DISABLED )
		||   class_table[iClass].tier != TIER_ONE )
		    continue;
		if ( !str_cmp( arg, class_table[ iClass ].who_name ) )
		{
		    class_list[ nClass ] = iClass;
		    break;
		}
		if ( !str_prefix( arg, class_table[ iClass ].name ) )
		{
		    class_list[ nClass ] = iClass;
		    break;
		}
	    }
	    if ( iClass >= MAX_CLASS )
	    {
		send_to_char( "That's not a class!\n\r", ch );
		return;
	    }
	}
    }

    /* Generate list of skills.  For each skill, find the lowest level */
    /* at which the skill can be used. */
    pBuf = new_buf( );
    skill_list = (int *)alloc_mem( top_skill * sizeof( *skill_list ) );

    for ( nSkill = 1; nSkill < top_skill; nSkill++ )
    {
	if ( IS_NULLSTR( skill_table[nSkill].name ) )
	    break;
	skill_list[nSkill] = MAX_LEVEL + 1;
	for ( iClass = 0; iClass < nClass; iClass++ )
	{
	    if ( skill_table[ nSkill ].skill_level[ class_list[ iClass ] ] < skill_list[ nSkill ] )
		skill_list[ nSkill ] = skill_table[ nSkill ].skill_level[ class_list[ iClass ] ];
	}
    }

    /* Print it out */
    fFound = FALSE;
    add_buf( pBuf, "Lvl          Spells/skills\n\r" );
    for ( level = 1; level <= LEVEL_HERO; level++ )
    {
	if ( fFound )
	{
	    add_buf( pBuf, "\n\r" );
	    fFound = FALSE;
	}
	col = 0;
	for ( iSkill = 0; iSkill < nSkill; iSkill++ )
	{
	    if ( skill_list[ iSkill ] == level )
	    {
		if ( !fFound )
		{
		    buf_printf( pBuf, "%3d:  ", level );
		    fFound = TRUE;
		}
		if ( col >= 3 )
		{
		    add_buf( pBuf, "\n\r" );
		    col = 0;
		    add_buf( pBuf, "      " );
		}
		buf_printf( pBuf, "%20s", skill_table[ iSkill ].name );
		col++;
	    }
	}
    }

    if ( col )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_mem( skill_list, top_skill * sizeof( *skill_list ) );
    free_buf( pBuf );
    return;
}


void
do_racelist( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    BUFFER *	pBuf;
    int		i;
    bool	fNpc;
    bool	fPc;

    argument = one_argument( argument, arg );

    if ( !IS_IMMORTAL( ch ) || !str_cmp( arg, "all" ) || arg[0] == '\0' )
    {
        fNpc = TRUE;
        fPc  = TRUE;
    }
    else if ( !str_cmp( arg, "mob" ) || !str_cmp( arg, "npc" ) )
    {
        fNpc = TRUE;
        fPc  = FALSE;
    }
    else if ( !str_prefix( arg, "players" ) || !str_cmp( arg, "pc" ) || str_prefix( arg, "chars" ) )
    {
        fNpc = FALSE;
        fPc  = TRUE;
    }
    else
    {
        send_to_char( "Syntax: racelist [mob|npc|pc|players|char|all]\n\r", ch );
        return;
    }

    pBuf = new_buf( );

    for ( i = 0; !IS_NULLSTR( race_table[i].name ) && !IS_NULLSTR( race_table[i].who_name ); i++ )
    {
        if ( ( race_table[i].pc_race && fPc ) || ( !race_table[i].pc_race && fNpc ) )
        {
            buf_printf( pBuf, "   %5s  %s\n\r",
                        race_table[i].who_name, race_table[i].name );
        }
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_listen( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA *	pExit;
    char	arg[MAX_INPUT_LENGTH];
    char *	msg;
    int		count;
    int		dir;

    one_argument( argument, arg );

    if ( IS_AFFECTED( ch, AFF_DEAF ) )
    {
        send_to_char( "YOu can't hear anything.\n\r", ch );
        return;
    }

    if ( ch->in_room == NULL )
    {
        send_to_char( "You hear nothing at all.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        if ( rprog_listen_trigger( ch->in_room, ch ) )
            return;
        if ( !IS_NULLSTR( ch->in_room->sound ) )
        {
            send_to_char( ch->in_room->sound, ch );
            return;
        }

        count = 0;
        for ( dir = 0; dir < MAX_DIR; dir++ )
        {
            if ( ( pExit = ch->in_room->exit[dir] ) != NULL
            &&	 ( IS_SET( pExit->progtypes, EXIT_PROG_LISTEN )
            ||     ( !IS_NULLSTR( pExit->sound_closed ) && IS_SET( pExit->exit_info, EX_CLOSED ) )
            ||	   ( !IS_NULLSTR( pExit->sound_open ) && !IS_SET( pExit->exit_info, EX_CLOSED ) ) ) )
            {
                ch_printf( ch, "You hear sounds from %s.\n\r", dir_from[rev_dir[dir]] );
                count++;
            }
        }
        if ( count == 0 )
            send_to_char( "There are no unusual sounds here.\n\r", ch );
        return;
    }

    if ( ( dir = dir_lookup( arg ) ) == DIR_NONE )
    {
        for ( dir = 0; dir < MAX_DIR; dir++ )
        {
            if ( ( pExit = ch->in_room->exit[dir] ) != NULL
            &&	 is_name( arg, pExit->keyword ) )
                break;
        }
    }

    if ( dir == DIR_NONE || dir >= MAX_DIR )
    {
        if ( !check_social( ch, "listen", argument ) )
            send_to_char( "No unusual sounds there.\n\r", ch );
        return;
    }

    if ( ( pExit = ch->in_room->exit[dir] ) == NULL )
    {
        send_to_char( "No unusual sounds there.\n\r", ch );
        return;
    }

    if ( eprog_listen_trigger( pExit, ch->in_room, ch ) )
        return;

    msg = IS_SET( pExit->exit_info, EX_CLOSED ) ? pExit->sound_closed : pExit->sound_open;
    if ( !IS_NULLSTR( msg ) )
        send_to_char( msg, ch );
    else
        send_to_char( "No unusual sounds there.\n\r", ch );

    return;
}


void
do_smell( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	obj;
    char	arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] != '\0'
    &&	 ( obj = get_obj_here( ch, arg ) ) != NULL
    &&	 ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD ) )
    {
        act_color( AT_ACTION, "$n sniffs $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
        if ( obj->value[4] < 0 )
            act( "$p has spoiled.", ch, obj, NULL, TO_CHAR );
        else
            act( "$p smells fresh.", ch, obj, NULL, TO_CHAR );
        return;
    }

    act_color( AT_ACTION, "$n sniffs the air.", ch, NULL, NULL, TO_ROOM, POS_RESTING);

    if ( rprog_smell_trigger( ch->in_room, ch ) )
        return;

    if ( !IS_NULLSTR( ch->in_room->smell ) )
        send_to_char( ch->in_room->smell, ch );
    else
        send_to_char( "There are no odors of note here.\n\r", ch );

    return;
}


void
web_who( int fd )
{
    DESCRIPTOR_DATA *	d;
    BUFFER *		output;
    CHAR_DATA		ch;
    CHAR_DATA *		wch;
    CLIST_DATA *	who_first;
    CLIST_DATA *	who_char;
    CLIST_DATA *	who_tmp;
    char *		txt;
    int			cMatch;
    int			cImm;
    int			cMortal;
    int			len;
    int			nBlock;
    int			nWrite;
    int			iStart;

    cMatch = 0;
    cImm = 0;
    cMortal = 0;
    who_first = NULL;

    memset( &ch, 0, sizeof( ch ) );
    ch.pcdata = new_pcdata( );
    ch.in_room = get_room_index( ROOM_VNUM_TEMPLE );
    ch.level = 1;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING )
	    continue;
	wch = d->original ? d->original : d->character;
	if ( !can_see( &ch, wch ) )
	    continue;

	who_char = new_clist_data( );
	who_char->lch = wch;

	if ( who_first == NULL || who_comp( wch, who_first->lch ) )
	{
	    who_char->next = who_first;
	    who_first = who_char;
	}
	else if ( who_first->next == NULL )
	{
	    who_char->next = NULL;
	    who_first->next = who_char;
	}
	else
	{
	    for ( who_tmp = who_first; who_tmp->next; who_tmp = who_tmp->next )
	    {
		if ( who_comp( wch, who_tmp->next->lch ) )
		{
		    who_char->next = who_tmp->next;
		    who_tmp->next = who_char;
		    break;
		}
	    }

	    if ( who_tmp->next == NULL )
	    {
		who_char->next = NULL;
		who_tmp->next = who_char;
	    }
	}

	cMatch++;
	if ( wch->level >= LEVEL_IMMORTAL )
	{
	    cImm++;
	}
	else
	{
	    cMortal++;
	}
    }

    /*
     * Now show matching chars, walking the list twice.
     * First pass show immortals, second pass show mortals.
     */
    output = new_buf( );

    if ( cImm != 0 )
    {
        buf_printf( output, "\n\r%s`X\n\r",
                    whoborder_imm[0] == '\0' ?
                    "====================== Immortals ======================" :
                    whoborder_imm );
//	add_buf( output, "\n\r`G_,.;^^\"'`'\"^^;.,_ `YImmortals`X `G_,.;^^\"'`'\"^^;.,_`X\n\r" );
	for ( who_char = who_first; who_char != NULL; who_char = who_char->next )
	{
	    if ( who_char->lch->level >= LEVEL_IMMORTAL )
		who_line( NULL, who_char->lch, output, FALSE );
	}
    }

    if ( cMortal != 0 )
    {
        buf_printf( output, "\n\r%s`X\n\r",
                    whoborder_mort[0] == '\0' ?
                    "----------------------- Mortals -----------------------" :
                    whoborder_mort );
//	add_buf( output, "\n\r`W-=`r)`R*`r(`W=-`Bxx`W-=`r)`R*`r(`W=-- `WMortals`X `W--=`r)`R*`r(`W=-`Bxx`W-=`r)`R*`r(`W=-`X\n\r" );
	for ( who_char = who_first; who_char != NULL; who_char = who_char->next )
	{
	    if ( who_char->lch->level < LEVEL_IMMORTAL )
		who_line( NULL, who_char->lch, output, FALSE );
	}
    }

    if ( cMatch == 0 )
	add_buf( output, "Nobody!\n\r" );

//    add_buf( output, "\n\r`ro`R--`w====`W---------------------------------------------------------`w===`R--`ro\n\r" );

    txt = buf_string( output );
    len = strlen( txt );
    for( iStart = 0; iStart < len; iStart += nWrite )
    {
	nBlock = UMIN( len - iStart, 2048 );
	if ( ( nWrite = write( fd, txt + iStart, nBlock ) ) < 0 )
	    break;
    }

//    write_to_descriptor( fd, buf_string( output ), 0, FALSE );

    free_pcdata( ch.pcdata );

    free_buf( output );
    while ( who_first != NULL )
    {
	who_char = who_first;
	who_first = who_first->next;
	free_clist_data( who_char );
    }

    return;
}


