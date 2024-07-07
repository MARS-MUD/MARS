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
#include <sys/types.h>
#include <time.h>
#include "merc.h"
#include "db.h"
#include "interp.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"

struct wear_type
{
    int	wear_loc;
    int	wear_bit;
};

const struct wear_type wear_table[] =
{
    {	WEAR_NONE,	ITEM_TAKE		},
    {	WEAR_LIGHT,	ITEM_LIGHT		},
    {	WEAR_FINGER_L,	ITEM_WEAR_FINGER	},
    {	WEAR_FINGER_R,	ITEM_WEAR_FINGER	},
    {	WEAR_NECK_1,	ITEM_WEAR_NECK		},
    {	WEAR_NECK_2,	ITEM_WEAR_NECK		},
    {	WEAR_BODY,	ITEM_WEAR_BODY		},
    {	WEAR_HEAD,	ITEM_WEAR_HEAD		},
    {	WEAR_FACE,	ITEM_WEAR_FACE		},
    {	WEAR_LEGS,	ITEM_WEAR_LEGS		},
    {	WEAR_FEET,	ITEM_WEAR_FEET		},
    {	WEAR_HANDS,	ITEM_WEAR_HANDS		},
    {	WEAR_ARMS,	ITEM_WEAR_ARMS		},
    {	WEAR_SHIELD,	ITEM_WEAR_SHIELD	},
    {	WEAR_ABOUT,	ITEM_WEAR_ABOUT		},
    {	WEAR_WAIST,	ITEM_WEAR_WAIST		},
    {	WEAR_WRIST_L,	ITEM_WEAR_WRIST		},
    {	WEAR_WRIST_R,	ITEM_WEAR_WRIST		},
    {	WEAR_DUAL,	ITEM_WIELD		},
    {	WEAR_WIELD,	ITEM_WIELD		},
    {	WEAR_EYES,	ITEM_WEAR_CONTACT	},
    {	WEAR_HOLD,	ITEM_HOLD		},
    {	WEAR_EAR_L,	ITEM_WEAR_EAR		},
    {	WEAR_EAR_R,	ITEM_WEAR_EAR		},
    {	WEAR_ANKLE_L,	ITEM_WEAR_ANKLE		},
    {	WEAR_ANKLE_R,	ITEM_WEAR_ANKLE		},
    {	WEAR_SHOULDER,	ITEM_WEAR_SHOULDER	},
    {	WEAR_CPATCH,	ITEM_WEAR_CPATCH	},
    {	WEAR_RPATCH,	ITEM_WEAR_RPATCH	},
    {	WEAR_TAIL,	ITEM_WEAR_TAIL          },
    {	WEAR_HORNS,	ITEM_WEAR_HORNS          },
    {	NO_FLAG,	NO_FLAG			}
};


/*
 * Local functions
 */
static	bool	change_exit	args( ( CHAR_DATA *ch, char *argument,
					int door ) );
static	void	display_resets	args( ( CHAR_DATA *ch ) );
static	bool	epedit_create	args( ( CHAR_DATA *ch, int dir ) );
static	OREPROG_DATA *get_eprog_data( ROOM_INDEX_DATA *pRoom, int dir, int vnum );
static	OREPROG_DATA *get_rprog_data( ROOM_INDEX_DATA *pRoom, int vnum );
static	bool redit_epedit	( CHAR_DATA *ch, char *argument, int dir );
static	bool redit_epremove	( CHAR_DATA *ch, char *argument, int dir );
static	bitvector wear_bit	args( ( int loc ) );
static	bool	validate_reset	args( ( CHAR_DATA *ch, int num, char rtype ) );


/* Entry point for editing room_index_data. */
void
do_redit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    char		command[MAX_INPUT_LENGTH];
    char		arg[MAX_INPUT_LENGTH];
    char *		argp;
    int			players;

    if( IS_NPC( ch ) )
    {
	send_to_char( "Mobs dont build, they are built!\n\r", ch );
	return;
    }

    argument = one_argument( argument, command );
    pRoom = ch->in_room;

    if ( command[0] == 'r' && !str_prefix( command, "reset" ) )
    {
	players = pRoom->area->nplayer;
	pRoom->area->nplayer = 0;
	reset_room( pRoom );
	send_to_char( "Room reset.\n\r", ch );
	pRoom->area->nplayer = players;
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	if ( redit_create( ch, argument ) )
	{
	    char_from_room( ch );
	    char_to_room( ch, ch->desc->pEdit );
	    SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
    	}
    	else
    	    return;
    }

    if ( command[0] == 'd' && !str_prefix( command, "delete" ) )
    {
        argp = one_argument( argument, arg );
	if ( str_cmp( arg, "room" ) || !is_number( argp ) )
	{
	    send_to_char( "Syntax:  redit delete room <vnum>\n\r", ch );
	    return;
	}

	pArea = get_vnum_area( atoi( argp ) );

	if ( redit_delete( ch, argument ) )
	{
	    if ( pArea != NULL )
	    {
	        SET_BIT( pArea->area_flags, AREA_CHANGED );
            }
	}
	return;
    }

    if ( is_number( command ) )
    {
	pRoom = get_room_index( atoi( command ) );
	if (!pRoom )
	{
	    send_to_char( "REdit:  That room does not exist.\n\r", ch );
	    return;
	}
	if ( room_is_private( pRoom ) )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
	char_from_room( ch );
	char_to_room( ch, pRoom );
    }

    /*
     * Builder defaults to editing current room.
     */
    ch->desc->editor = ED_ROOM;
    redit_show( ch, "" );
    return;
}


/* Room Interpreter, called by do_redit. */
void
redit( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    AREA_DATA *		pArea;
    char		arg[MAX_STRING_LENGTH];
    char		command[MAX_INPUT_LENGTH];
    int			cmd;
    int			value;

    if ( IS_NPC( ch ) )
    {
	interpret( ch, argument );
	return;
    }

    EDIT_ROOM( ch, pRoom );
    pArea = pRoom->area;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( !IS_BUILDER( ch, pArea ) )
        send_to_char( "REdit:  Insufficient security to modify room.\n\r", ch );

    if ( command[0] == '\0' )
    {
	redit_show( ch, argument );
	return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
        interpret( ch, arg );
        return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *redit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, redit_table[cmd].name ) )
	{
	    if ( (*redit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Take care of flags. */
    if ( ( value = flag_value( room_flags, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT( pRoom->room_flags, value );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Room flag toggled.\n\r", ch );
        return;
    }

    if ( ( value = flag_value( sector_types, arg ) ) != NO_FLAG )
    {
        pRoom->sector_type  = value;

	if ( IS_VIRTUAL( pRoom ) )
	{
	    int offset;
	    offset = pRoom->vnum - pArea->min_vnum;
	    pArea->overland->map[offset].sector_type = value;
	}

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Sector type set.\n\r", ch );
        return;
    }

    if ( ( value = flag_value( room_aff_flags, command ) ) != NO_FLAG )
    {
        pRoom->affect_flags = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Room affect set.\n\r", ch );
        return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
redit_affect( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    char		arg[MAX_INPUT_LENGTH];
    int			value;

    EDIT_ROOM( ch, pRoom );

    argument = one_argument( argument, arg );

    if ( ( value = flag_value( room_aff_flags, arg ) ) == NO_FLAG )
    {
        send_to_char( "Affect not found.\n\r", ch );
        return FALSE;
    }

    pRoom->affect_flags = value;
    send_to_char( value == ROOM_AFF_NONE ? "Affect cleared.\n\r" : "Affect set.\n\r", ch );
    return TRUE;
}


bool
redit_copy( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    ROOM_INDEX_DATA *	pSrc;
    EXTRA_DESCR_DATA *	pEd;
    EXTRA_DESCR_DATA *	pEd_next;
    EXTRA_DESCR_DATA *	pSrcEd;
    OREPROG_DATA *	pProg;
    OREPROG_DATA *	pSrcProg;
    OREPROG_DATA *	pLastProg;
    char		arg[MAX_INPUT_LENGTH];

    EDIT_ROOM( ch, pRoom );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  copy <room_vnum> (progs)\n\r", ch );
	return FALSE;
    }

    if ( ( pSrc = get_room_index( atoi( arg ) ) ) == NULL )
    {
	send_to_char( "REdit:  Room vnum does not exist.\n\r", ch );
	return FALSE;
    }

    if ( pSrc == pRoom )
    {
	send_to_char( "REdit:  Cannot copy a room onto itself.\n\r", ch );
	return FALSE;
    }

    if ( IS_SET( pSrc->area->area_flags, AREA_PROTOTYPE )
    &&  !IS_BUILDER( ch, pSrc->area ) )
    {
	send_to_char( "You may not copy rooms from that area.\n\r", ch );
	return FALSE;
    }

    free_string( pRoom->name );
    pRoom->name		= str_dup( pSrc->name );
    free_string( pRoom->description );
    pRoom->description	= str_dup( pSrc->description );
    free_string( pRoom->morning_desc );
    pRoom->morning_desc = str_dup( pSrc->morning_desc );
    free_string( pRoom->evening_desc );
    pRoom->evening_desc = str_dup( pSrc->evening_desc );
    free_string( pRoom->night_desc );
    pRoom->night_desc	= str_dup( pSrc->night_desc );
    free_string( pRoom->sound );
    pRoom->sound = str_dup( pSrc->sound );
    free_string( pRoom->smell );
    pRoom->smell = str_dup( pSrc->smell );
    free_string( pRoom->short_descr );
    pRoom->short_descr = str_dup( pSrc->short_descr );

    pRoom->room_flags	= pSrc->room_flags;
    pRoom->sector_type	= pSrc->sector_type;
    pRoom->heal_rate	= pSrc->heal_rate;
    pRoom->mana_rate	= pSrc->mana_rate;
    pRoom->clan		= pSrc->clan;
    pRoom->vehicle_type	= pSrc->vehicle_type;

    /* free up any existing extra descs */
    for ( pEd = pRoom->extra_descr; pEd; pEd = pEd_next )
    {
	pEd_next = pEd->next;
	free_extra_descr( pEd );
    }
    pRoom->extra_descr = NULL;

    /* now copy extra descs from source room */
    /* Note that this will reverse the order of the ed's */
    for ( pSrcEd = pSrc->extra_descr; pSrcEd; pSrcEd = pSrcEd->next )
    {
	pEd			= new_extra_descr( );
	pEd->keyword		= str_dup( pSrcEd->keyword );
	pEd->description	= str_dup( pSrcEd->description );
	pEd->next		= pRoom->extra_descr;
	pRoom->extra_descr	= pEd;
    }

    if ( LOWER( *argument == 'p' ) && !str_prefix( argument, "progs" ) )
    {
        if ( pSrc->rprogs == NULL )
        {
            send_to_char( "REdit:  Source room has no progs.\n\r", ch );
        }
        else
        {
            /* Fix compiler warning; code is correct. */
            pLastProg = NULL;

            /* Clear existing progs */
            while ( ( pProg = pRoom->rprogs ) != NULL )
            {
                pRoom->rprogs = pProg->next;
                free_oreprog_data( pProg );
            }

            /* Copy the progs, preserving order. */
            for ( pSrcProg = pSrc->rprogs; pSrcProg != NULL; pSrcProg = pSrcProg->next )
            {
                pProg = new_oreprog_data( );
                pProg->type = pSrcProg->type;
                pProg->arglist = str_dup( pSrcProg->arglist );
                pProg->comlist = str_dup( pSrcProg->comlist );
                pProg->triggered = pSrcProg->triggered;

                if ( pRoom->rprogs == NULL )
                    pRoom->rprogs = pProg;
                else
                    pLastProg->next = pProg;
                pLastProg = pProg;
            }
            pRoom->progtypes = pSrc->progtypes;
        }
    }

    redit_show( ch, "" );
    return TRUE;
}


bool
redit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    ROOM_INDEX_DATA *	vRoom;
    OBJ_DATA *		obj;
    CHAR_DATA *		rch;
    int			value;
    int			iHash;
    int			count;

    EDIT_ROOM( ch, pRoom );

    vRoom = NULL;
    if ( argument[ 0 ] == '\0' )
    {
	if ( get_room_index( ch->in_room->vnum ) == NULL )
	{
	    value = ch->in_room->vnum;
	    vRoom = ch->in_room;
	}
	else
	{
	    pArea = ch->in_room->area;
	    value = pArea->overland == NULL ?
		    pArea->min_vnum :
		    pArea->min_vnum + pArea->maxx * pArea->maxy;
	    for ( ; value <= pArea->max_vnum; value++ )
		if ( !get_room_index( value ) )
		    break;

	    if ( value > pArea->max_vnum )
	    {
		send_to_char( "REdit:  No free room vnums in this area.\n\r", ch );
		return FALSE;
	    }
	}
    }
    else
    {
	value = atoi( argument );
    }

    /* OLC 1.1b */
    if ( value <= 0 || value >= MAX_VNUM )
    {
	ch_printf( ch, "Syntax:  create [0 < vnum < %d]\n\r", MAX_VNUM );
	return FALSE;
    }

    if ( value <= 0 )
    {
	send_to_char( "Syntax:  create [vnum > 0]\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char( "REdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "REdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_room_index( value ) )
    {
	send_to_char( "REdit:  Room vnum already exists.\n\r", ch );
	return FALSE;
    }

    /* See how many rooms already exist in the area */
    count = 0;
    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pRoom = room_index_hash[iHash]; pRoom != NULL; pRoom = pRoom->next )
        {
            if ( pRoom->vnum >= pArea->min_vnum
            &&	 pRoom->vnum <= pArea->max_vnum )
                count++;
        }
    }

    pRoom			= new_room_index();
    pRoom->area			= pArea;
    pRoom->vnum			= value;
    if ( ch->in_room )
    {
	if ( count == 0 || IS_SET( ch->act2, PLR_NOCOPY ) )
	{
	    pRoom->room_flags  = IS_SET( pArea->area_flags, AREA_NO_UNFINISHED ) ? 0 : ROOM_UNFINISHED;
	    pRoom->sector_type = SECT_INSIDE;
	}
	else
	{
	    pRoom->room_flags	= ch->in_room->room_flags;
	    REMOVE_BIT( pRoom->room_flags, ROOM_VIRTUAL );
	    pRoom->sector_type	= ch->in_room->sector_type;
	}
    }

    if ( !IS_SET( pArea->area_flags, AREA_NO_UNFINISHED ) )
	SET_BIT( pRoom->room_flags, ROOM_UNFINISHED );

    if ( value > top_vnum_room )
        top_vnum_room = value;

    iHash			= value % MAX_KEY_HASH;
    pRoom->next			= room_index_hash[iHash];
    room_index_hash[iHash]	= pRoom;

    insert_room( pRoom );

    /* if virtual room was realized, copy data to new room. */
    if ( vRoom != NULL )
    {
	while ( vRoom->people != NULL )
	{
	    rch = vRoom->people;
	    char_from_room( rch );
	    char_to_room( rch, pRoom );
	}
	while ( vRoom->contents != NULL )
	{
	    obj = vRoom->contents;
	    obj_from_room( obj );
	    obj_to_room( obj, pRoom );
	}
	pRoom->extra_descr	= vRoom->extra_descr;
	vRoom->extra_descr	= NULL;
	pRoom->reset_first	= vRoom->reset_first;
	vRoom->reset_first	= NULL;
	pRoom->reset_last	= vRoom->reset_last;
	vRoom->reset_last	= NULL;
	pRoom->rprogs		= vRoom->rprogs;
	vRoom->rprogs		= NULL;
	pRoom->progtypes	= vRoom->progtypes;
	vRoom->progtypes	= 0;
	pRoom->name		= vRoom->name;
	vRoom->name		= NULL;
	pRoom->description	= vRoom->description;
	vRoom->description	= NULL;
	pRoom->morning_desc	= vRoom->morning_desc;
	vRoom->morning_desc	= NULL;
	pRoom->evening_desc	= vRoom->evening_desc;
	vRoom->evening_desc	= NULL;
	pRoom->night_desc	= vRoom->night_desc;
	vRoom->night_desc	= NULL;
	pRoom->room_flags	= vRoom->room_flags;
	REMOVE_BIT( pRoom->room_flags, ROOM_VIRTUAL );
	pRoom->sector_type	= vRoom->sector_type;
	pRoom->heal_rate	= vRoom->heal_rate;
	pRoom->mana_rate	= vRoom->mana_rate;
    }

    ch->desc->pEdit		= (void *)pRoom;

    if ( argument[0] == '\0' && vRoom == NULL )
	ch_printf( ch, "Room %d created.\n\r", pRoom->vnum );
    else
	send_to_char( "Room created.\n\r", ch );
    return TRUE;
}


bool
redit_delete( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    AREA_DATA *		pTravArea;
    ROOM_INDEX_DATA *	pRoom;
    ROOM_INDEX_DATA *	exRoom;
    DESCRIPTOR_DATA *	d;
    CHAR_DATA *		vch;
    TRAVEL_DATA *	pTravel;
    char		arg[MAX_INPUT_LENGTH];
    char		logbuf[MAX_INPUT_LENGTH];
    int			dir;
    int			iHash;
    int			vnum;

    argument = one_argument( argument, arg );
    if ( str_cmp( arg, "room" ) || !is_number( argument ) )
    {
        send_to_char( "Syntax:  delete room <vnum>\n\r", ch );
        return FALSE;
    }

    vnum = atoi( argument );
    if ( vnum < 1 || vnum > MAX_VNUM )
    {
        ch_printf( ch, "REdit:  Vnum must be between 1 and %d.\n\r", MAX_VNUM );
        return FALSE;
    }

    if ( ( pArea = get_vnum_area( vnum ) ) == NULL )
    {
        send_to_char( "REdit:  That vnum is not assigned to an area.\n\r", ch );
        return FALSE;
    }

    if ( ( pRoom = get_room_index( vnum ) ) == NULL )
    {
	send_to_char( "REdit:  vnum does not exist.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "REdit:  That vnum is in an area you cannot build in.\n\r", ch );
        return FALSE;
    }

    if ( pArea == get_vnum_area( ROOM_VNUM_LIMBO ) && !IS_CODER( ch ) )
    {
	send_to_char( "REdit:  Only Coders may delete rooms in Limbo.\n\r", ch );
	return FALSE;
    }

    if ( ch->in_room == pRoom )
    {
	send_to_char( "REdit:  You cannot delete the room you are in.\n\r", ch );
	return FALSE;
    }

    if ( IS_SET( pRoom->room_flags, ROOM_VIRTUAL ) )
    {
        send_to_char( "REdit:  Room is virtual.\n\r", ch );
        return FALSE;
    }

    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
        if ( pRoom->exit[dir] != NULL
        &&   pRoom->exit[dir]->to_room != NULL
        &&   pRoom->exit[dir]->to_room->area != pArea )
        {
            ch_printf( ch, "REdit:  Delete exit %s to room #%d first.\n\r",
                       dir_name[dir], pRoom->exit[dir]->to_room->vnum );
            return FALSE;
        }
    }

    for ( pTravArea = area_first; pTravArea != NULL; pTravArea = pTravArea->next )
    {
        if ( pTravArea->travel_room == pRoom )
        {
            ch_printf( ch, "REdit:  Room is traveling room in area #%d, %s`X\n\r",
                       pTravArea->vnum, pTravArea->name );
            return FALSE;
        }

        for ( pTravel = pTravArea->travel_first; pTravel != NULL; pTravel = pTravel->next )
        {
            if ( pTravel->room == pRoom )
            {
                ch_printf( ch, "Room is scheduled stop for traveling room in area #%d, %s`X\n\r",
                           pTravArea->vnum, pTravArea->name );
                return FALSE;
            }
        }
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( exRoom = room_index_hash[iHash]; exRoom != NULL; exRoom = exRoom->next )
        {
            if ( exRoom->area == pArea )
                continue;
            for ( dir = 0; dir < MAX_DIR; dir++ )
            {
                if ( exRoom->exit[dir] != NULL && exRoom->exit[dir]->to_room == pRoom )
                {
                    ch_printf( ch, "REdit:  Delete exit %s in room #%d first.\n\r",
                               dir_name[dir], exRoom->vnum );
                    return FALSE;
                }
            }
        }
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected != CON_PLAYING )
	    continue;
	if ( d->editor == ED_ROOM || d->editin == ED_ROOM )
	{
	    vch = d->original ? d->original : d->character;
	    if ( vch->in_room == pRoom )
	    {
		send_to_char( "Somebody is editing that room!\n\r", ch );
		return FALSE;
	    }
	}
    }

    sprintf( logbuf, "Redit_delete: %s deleting room #%d.", ch->name, vnum );
    wiznet( "$t", ch, logbuf, WIZ_BUILD, 0, get_trust( ch ) );
    log_string( logbuf );
    delete_room( pRoom );
    send_to_char( "Room deleted.\n\r", ch );
    return TRUE;

}


bool
redit_desc( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM( ch, pRoom );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pRoom->description );
	return TRUE;
    }

    send_to_char( "Syntax:  desc\n\r", ch );
    return FALSE;
}


bool
redit_down( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_down( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_DOWN ) );
}


bool
redit_east( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_east( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_EAST ) );
}


bool
redit_ed( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    EXTRA_DESCR_DATA *	ed;
    char		command[MAX_INPUT_LENGTH];
    char		keyword[MAX_INPUT_LENGTH];

    EDIT_ROOM( ch, pRoom );

    argument = one_argument( argument, command );
    one_argument( argument, keyword );

    if ( command[0] == '\0' || keyword[0] == '\0' )
    {
	send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	send_to_char( "         ed edit [keyword]\n\r", ch );
	send_to_char( "         ed delete [keyword]\n\r", ch );
	send_to_char( "         ed show   [keyword]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	    return FALSE;
	}

	ed			=   new_extra_descr();
	ed->keyword		=   str_dup( keyword );
	ed->description		=   str_dup( "" );
	ed->next		=   pRoom->extra_descr;
	pRoom->extra_descr	=   ed;

	string_append( ch, &ed->description );

	return TRUE;
    }


    if ( !str_cmp( command, "edit" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	string_append( ch, &ed->description );

	return TRUE;
    }


    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	if ( !ped )
	    pRoom->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr( ed );

	send_to_char( "Extra description deleted.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "show" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed show [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	page_to_char( ed->description, ch );
	return FALSE;
    }

    redit_ed( ch, "" );
    return FALSE;
}


bool
redit_edesc( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM( ch, pRoom );

    if ( *argument == '\0' )
    {
	string_append( ch, &pRoom->evening_desc );
	return TRUE;
    }
    else
    {
	send_to_char( "Syntax:  edesc\n\r", ch );
	return FALSE;
    }
}


bool
redit_heal( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;
    int		     value;
    char	     buf[MAX_INPUT_LENGTH];
    char *	     p;

    EDIT_ROOM( ch, pRoom );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  heal <amount>\n\r", ch );
	return FALSE;
    }

    strcpy( buf, argument );
    if ( ( p = strchr( buf, '%' ) ) != NULL )
	*p = '\0';

    if ( !is_number( buf ) )
    {
	send_to_char( "REdit:  Heal rate must be numeric.\n\r", ch );
	return FALSE;
    }

    value = atoi( buf );
    if ( value < -400 || value > 400 )
    {
	send_to_char( "REdit:  Value must be >= -400% and <= 400%.\n\r", ch );
	return FALSE;
    }

    pRoom->heal_rate = value;
    send_to_char( "Heal rate set.\n\r", ch );
    return TRUE;
}


bool
redit_mana( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    int			value;
    char		buf[MAX_INPUT_LENGTH];
    char *		p;

    EDIT_ROOM( ch, pRoom );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  mana <amount>\n\r", ch );
	return FALSE;
    }

    strcpy( buf, argument );
    if ( ( p = strchr( buf, '%' ) ) != NULL )
	*p = '\0';

    if ( !is_number( buf ) )
    {
	send_to_char( "REdit:  Mana recovery rate must be numeric.\n\r", ch );
	return FALSE;
    }

    value = atoi( buf );
    if ( value < -400 || value > 400 )
    {
	send_to_char( "REdit:  Value must be >= -400% and <= 400%.\n\r", ch );
	return FALSE;
    }

    pRoom->mana_rate = value;
    send_to_char( "Mana recovery rate set.\n\r", ch );
    return TRUE;
}


bool
redit_mdesc( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM( ch, pRoom );

    if ( *argument == '\0' )
    {
	string_append( ch, &pRoom->morning_desc );
	return TRUE;
    }
    else
    {
	send_to_char( "Syntax:  mdesc\n\r", ch );
	return FALSE;
    }
}


bool
redit_name( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    char *		p;

    EDIT_ROOM( ch, pRoom );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [name]\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pRoom->name, argument ) ) != NULL )
    {
	pRoom->name = p;
	send_to_char( "Name set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;

}


bool
redit_ndesc( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM( ch, pRoom );

    if ( *argument == '\0' )
    {
	string_append( ch, &pRoom->night_desc );
	return TRUE;
    }
    else
    {
	send_to_char( "Syntax:  ndesc\n\r", ch );
	return FALSE;
    }
}


bool
redit_north( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_north( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_NORTH ) );
}


bool
redit_northeast( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_northeast( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_NORTHEAST ) );
}


bool
redit_northwest( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_northwest( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_NORTHWEST ) );
}


bool
redit_rpedit( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *	pRprog;
    ROOM_INDEX_DATA *	pRoom;
    char		command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );

    if ( is_number( command ) )
    {
	if ( ( pRprog = get_rprog_data( ch->in_room, atoi( command ) ) ) == NULL )
	{
	    send_to_char( "RPEdit:  Room has no such RoomProg.\n\r", ch );
	    return FALSE;
	}
	ch->desc->editin = ch->desc->editor;
	ch->desc->editor = ED_RPROG;
	ch->desc->inEdit = ch->desc->pEdit;
	ch->desc->pEdit  = (void *)pRprog;
	rpedit_show( ch, argument );
	return FALSE;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	ch->desc->editin = ch->desc->editor;
	ch->desc->editor = ED_RPROG;
	if ( rpedit_create( ch, argument ) )
	{
	    rpedit_show( ch, argument );
	    return TRUE;
	}
	ch->desc->editor = ch->desc->editin;
	ch->desc->editin = 0;
	return FALSE;
    }

    EDIT_ROOM( ch, pRoom );
    if ( command[0] == '\0'
    &&	 pRoom->rprogs != NULL
    &&	 pRoom->rprogs->next == NULL )
    {
        return redit_rpedit( ch, "0" );
    }

    send_to_char( "RPEdit:  There is no default RoomProg to edit.\n\r", ch );
    return FALSE;
}


bool
redit_rpremove( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *	pProg;
    OREPROG_DATA *	pPrev;
    ROOM_INDEX_DATA *	pRoom;
    int			value;
    int			vnum;

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  rpremove #", ch );
	return FALSE;
    }

    EDIT_ROOM( ch, pRoom );
    value = 0;
    vnum = atoi( argument );

    if ( pRoom->rprogs == NULL )
    {
	send_to_char( "No such RoomProg.\n\r", ch );
	return FALSE;
    }

    pPrev = NULL;
    for ( pProg = pRoom->rprogs; value < vnum; value++ )
    {
	pPrev = pProg;
	pProg = pProg->next;
	if ( pProg == NULL )
	{
	    send_to_char( "No such RoomProg.\n\r", ch );
	    return FALSE;
	}
    }

    if ( pPrev == NULL )
	pRoom->rprogs = pRoom->rprogs->next;
    else
	pPrev->next = pProg->next;

    free_oreprog_data( pProg );
    pRoom->progtypes = 0;
    for ( pProg = pRoom->rprogs; pProg != NULL; pProg = pProg->next )
	SET_BIT( pRoom->progtypes, pProg->type );
    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


bool
redit_short( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    char *		p;

    EDIT_ROOM( ch, pRoom );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [short descr]\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pRoom->short_descr, argument ) ) != NULL )
    {
	pRoom->short_descr = p;
	send_to_char( "Short description set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;

}


bool
redit_show( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    BUFFER *		pBuf;

    EDIT_ROOM( ch, pRoom );

    pBuf = new_buf( );
    show_room_info( ch, pRoom, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}


bool
redit_smell( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM( ch, pRoom );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pRoom->smell );
	return TRUE;
    }

    send_to_char( "Syntax:  smell\n\r", ch );
    return FALSE;
}


bool
redit_sound( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM( ch, pRoom );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pRoom->sound );
	return TRUE;
    }

    send_to_char( "Syntax:  sound\n\r", ch );
    return FALSE;
}


bool
redit_south( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_south( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_SOUTH ) );
}


bool
redit_southeast( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_southeast( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_SOUTHEAST ) );
}


bool
redit_southwest( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_southwest( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_SOUTHWEST ) );
}


bool
redit_up( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_up( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_UP ) );
}


bool
redit_vehicle( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    char		arg[MAX_INPUT_LENGTH];
    int			type;

    EDIT_ROOM( ch, pRoom );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  vehicle <vehicle_type>\n\r", ch );
	send_to_char( "See \"? vehicles\" for list.\n\r", ch );
	return FALSE;
    }

    if ( ( type = flag_value( vehicle_types, arg ) ) == NO_FLAG )
    {
	send_to_char( "REdit:  Bad vehicle type.\n\r", ch );
	return FALSE;
    }

    pRoom->vehicle_type = type;
    send_to_char( "Vehicle type set.\n\r", ch );

    if ( type == VEHICLE_NONE && pRoom->in_room != NULL )
	room_from_room( pRoom );

    return TRUE;
}


bool
redit_west( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
        do_west( ch, argument );
        return FALSE;
    }
    return ( change_exit( ch, argument, DIR_WEST ) );
}


/************************************************************************/
/*			EPEdit section					*/
/************************************************************************/
void
epedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    OREPROG_DATA *	pProg;
    EXIT_DATA *		pExit;
    char		arg[MAX_INPUT_LENGTH];
    char		command[MAX_INPUT_LENGTH];
    bitvector		flags;
    int			cmd;
    int			dir;

    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
        if ( ( pExit = ch->in_room->exit[dir] ) != NULL )
        {
            for ( pProg = pExit->eprogs; pProg != NULL; pProg = pProg->next )
            {
                if ( pProg == (OREPROG_DATA *)ch->desc->pEdit )
                    break;
            }
            if ( pProg != NULL )
                break;
        }
    }

    if ( dir >= MAX_DIR )
    {
        send_to_char( "Room changed.  Returning to REdit.\n\r", ch );
        edit_done( ch );
        return;
    }

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
        epedit_show( ch, "" );
        return;
    }

    if ( !str_cmp( command, "done" ) )
    {
        edit_done( ch );
        return;
    }

    pArea = ch->in_room->area;

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "EPEdit:  Insufficient security to modify area.\n\r", ch );
	interpret( ch, arg );
	return;
    }

    /* Search table and dispatch command. */
    for ( cmd = 0; *epedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, epedit_table[cmd].name ) )
	{
	    if ( (*epedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    if ( ( flags = flag_value( eprog_types, arg ) ) != NO_FLAG )
    {
        pProg->type = flags;
        pExit->progtypes = 0;

        for ( pProg = pExit->eprogs; pProg != NULL; pProg = pProg->next )
            SET_BIT( pExit->progtypes, pProg->type );
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Exit Prog type set.\n\r", ch );
        return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


static bool
epedit_create( CHAR_DATA *ch, int dir )
{
    ROOM_INDEX_DATA *	pRoom;
    EXIT_DATA *		pExit;
    OREPROG_DATA *	pProg;
    OREPROG_DATA *	pPtr;

    EDIT_ROOM( ch, pRoom );

    if ( ( pExit = pRoom->exit[dir] ) == NULL )
        return FALSE;

    pProg       = new_oreprog_data( );
    pProg->type = EXIT_PROG_EXIT;
    pProg->next = NULL;
    SET_BIT( pExit->progtypes, pProg->type );

    if ( pExit->eprogs == NULL )
    {
        pExit->eprogs = pProg;
    }
    else
    {
        for ( pPtr = pExit->eprogs; pPtr->next != NULL; pPtr = pPtr->next )
            ;
        pPtr->next = pProg;
    }

    ch->desc->inEdit = ch->desc->pEdit;
    ch->desc->pEdit = (void *)pProg;
    ch->desc->editin = ch->desc->editor;
    ch->desc->editor = ED_EPROG;

    send_to_char( "ExitProg created.\n\r", ch );
    return TRUE;
}


bool
epedit_show( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *	pProg;
    BUFFER *		pBuf;
    int			dir;

    EDIT_OREPROG( ch, pProg );

    if ( ( dir = get_eprog_dir( ch->in_room, pProg ) ) == DIR_NONE )
    {
        send_to_char( "EPEdit: Exit for ExitProg not found!\n\r", ch );
        return FALSE;
    }

    pBuf = new_buf( );

    buf_printf( pBuf, "%s room [%5d] %s\n\r",
                capitalize( dir_name[dir] ), ch->in_room->vnum,
                ch->in_room->name );
    buf_printf( pBuf, "RoomProg type: %s\n\r", flag_string( eprog_types, pProg->type ) );
    buf_printf( pBuf, "Arguments: %s\n\r", pProg->arglist );
    buf_printf( pBuf, "Commands:\n\r%s", pProg->comlist );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}


static OREPROG_DATA *
get_eprog_data( ROOM_INDEX_DATA *pRoom, int dir, int vnum )
{
    OREPROG_DATA *	pProg;
    int			count;

    if ( pRoom == NULL )
        return NULL;


    if ( pRoom->exit[dir] != NULL )
    {
        count = 0;
        for ( pProg = pRoom->exit[dir]->eprogs; pProg != NULL; pProg = pProg->next )
        {
            if ( count == vnum )
                return pProg;
            count++;
        }
    }

    return NULL;
}


int
get_eprog_dir( ROOM_INDEX_DATA *pRoom, OREPROG_DATA *pProg )
{
    OREPROG_DATA *	pPrg;
    int			dir;

    if ( pRoom == NULL )
        return DIR_NONE;

    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
        if ( pRoom->exit[dir] != NULL )
        {
            for ( pPrg = pRoom->exit[dir]->eprogs; pPrg != NULL; pPrg = pPrg->next )
                if ( pPrg == pProg )
                    return dir;
        }
    }

    return DIR_NONE;
}


static bool
redit_epedit( CHAR_DATA *ch, char *argument, int dir )
{
    OREPROG_DATA *	pProg;
    char		cmd[MAX_INPUT_LENGTH];

    argument = one_argument( argument, cmd );

    if ( ch->in_room->exit[dir] == NULL )
    {
        ch_printf( ch, "EPEdit: Room has no exit %s.\n\r", dir_name[dir] );
        return FALSE;
    }

    if ( is_number( cmd ) )
    {
        if ( ( pProg = get_eprog_data( ch->in_room, dir, atoi( cmd ) ) ) == NULL )
        {
            send_to_char( "EPEdit:  Exit has no such ExitProg.\n\r", ch );
            return FALSE;
        }

        ch->desc->editin = ch->desc->editor;
        ch->desc->editor = ED_EPROG;
        ch->desc->pEdit  = pProg;
        epedit_show( ch, "" );
        return FALSE;
    }

    if ( cmd[0] == 'c' && !str_prefix( cmd, "create" ) )
    {
        if ( epedit_create( ch, dir ) )
        {
            epedit_show( ch, "" );
            return TRUE;
        }
        return FALSE;
    }

    if ( cmd[0] == '\0'
    &&	 ch->in_room->exit[dir]->eprogs != NULL
    &&	 ch->in_room->exit[dir]->eprogs->next == NULL )
    {
        return redit_epedit( ch, "0", dir );
    }

    send_to_char( "EPEdit:  There is no default ExitProg to edit.\n\r", ch );
    return FALSE;
}


static bool
redit_epremove( CHAR_DATA *ch, char *argument, int dir )
{
    OREPROG_DATA *	pProg;
    OREPROG_DATA *	pPrev;
    EXIT_DATA *		pExit;
    int			vnum;
    int			count;

    if ( !is_number( argument ) )
    {
        send_to_char( "Syntax:  <dir> repremove #\n\r", ch );
        return FALSE;
    }

    EDIT_OREPROG( ch, pProg );

    if ( ( pExit = ch->in_room->exit[dir] ) == NULL )
    {
        ch_printf( ch, "EPEdit:  Exit %s does not exist.\n\r", dir_name[dir] );
        return FALSE;
    }

    if ( pExit->eprogs == NULL )
    {
        ch_printf( ch, "EPEdit:  Exit %s has no ExitProgs.\n\r", dir_name[dir] );
        return FALSE;
    }

    vnum = atoi( argument );
    count = 0;

    pPrev = NULL;
    for ( pProg = pExit->eprogs; count < vnum; count++ )
    {
	pPrev = pProg;
	pProg = pProg->next;
	if ( pProg == NULL )
	{
	    send_to_char( "No such ExitProg.\n\r", ch );
	    return FALSE;
	}
    }

    if ( pPrev == NULL )
        pExit->eprogs = pExit->eprogs->next;
    else
        pPrev->next = pProg->next;

    free_oreprog_data( pProg );

    pExit->progtypes = 0;
    for ( pProg = pExit->eprogs; pProg != NULL; pProg = pProg->next )
        SET_BIT( pExit->progtypes, pProg->type );

    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


/************************************************************************/
/*			RPEdit section					*/
/************************************************************************/

void
rpedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    OREPROG_DATA *	pProg;
    char		command[MAX_INPUT_LENGTH];
    char		arg[MAX_INPUT_LENGTH];
    int			cmd;
    int			value;

    for ( pProg = ch->in_room->rprogs; pProg; pProg = pProg->next )
	if ( pProg == (OREPROG_DATA *)ch->desc->pEdit )
	    break;

    if ( !pProg )
    {
	send_to_char( "RPEdit:  Room changed.  Returning to REditor.\n\r", ch );
	edit_done( ch );
	return;
    }

    pRoom = ch->in_room;
    pArea = ch->in_room->area;
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	rpedit_show( ch, "" );
	return;
    }

    if ( !str_cmp( command, "done" ) )
    {
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "RPEdit:  Insufficient security to modify area.\n\r", ch );
	interpret( ch, arg );
	return;
    }

    /* Search table and dispatch command. */
    for ( cmd = 0; *rpedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, rpedit_table[cmd].name ) )
	{
	    if ( (*rpedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    if ( ( value = flag_value( rprog_types, arg ) ) != NO_FLAG )
    {
	pProg->type = value;
	pRoom->progtypes = 0;

	for ( pProg = pRoom->rprogs; pProg; pProg = pProg->next )
	    SET_BIT( pRoom->progtypes, pProg->type );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "RoomProg type set.\n\r",ch);
	return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
rpedit_arglist( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *pProg;

    EDIT_OREPROG( ch, pProg );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  arglist [string]\n\r", ch );
	return FALSE;
    }
    free_string( pProg->arglist );
    pProg->arglist = str_dup( argument );

    send_to_char( "Arglist set.\n\r", ch );
    return TRUE;
}

bool
rpedit_comlist( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *pProg;

    EDIT_OREPROG( ch, pProg );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pProg->comlist );
	return TRUE;
    }

    send_to_char( "Syntax:  comlist    - line edit\n\r", ch );
    return FALSE;
}


bool
rpedit_create( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    OREPROG_DATA *	pProg;
    OREPROG_DATA *	pNext;

    EDIT_ROOM( ch, pRoom );

    SET_BIT( pRoom->progtypes, ROOM_PROG_ENTER );
    pProg		= new_oreprog_data( );
//    pProg->on_obj	= NULL;
//    pProg->in_room	= pRoom;
//    pProg->on_exit	= NULL;
    pProg->type		= ROOM_PROG_ENTER;
    pProg->next		= NULL;

    if ( !pRoom->rprogs )
    {
	pRoom->rprogs = pProg;
    }
    else
    {
	for ( pNext = pRoom->rprogs; pNext->next; pNext = pNext->next )
	    ;
	pNext->next = pProg;
    }

    ch->desc->pEdit = (void *)pProg;
    send_to_char( "RoomProg created.\n\r", ch );
    return TRUE;
}


bool
rpedit_show( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *pProg;
    BUFFER *	  pBuf;

    EDIT_OREPROG( ch, pProg );
    pBuf = new_buf( );

    buf_printf( pBuf, "Room: [%5d] %s\n\r", ch->in_room->vnum,
	ch->in_room->name );
    buf_printf( pBuf, "RoomProg type: %s\n\r", flag_string( rprog_types, pProg->type ) );
    buf_printf( pBuf, "Arguments: %s\n\r", pProg->arglist );
    buf_printf( pBuf, "Commands:\n\r%s", pProg->comlist );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}


void
do_instaroom( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs dont build, they are built!\n\r", ch );
	return;
    }

    if ( *argument == '\0' )
	pRoom = ch->in_room;
    else
	pRoom = find_location( ch, argument );

    if ( pRoom == NULL )
    {
	send_to_char( "Room not found\n\r", ch );
	return;
    }

    if ( IS_SET( pRoom->room_flags, ROOM_VIRTUAL ) )
    {
        send_to_char( "Room is virtual and cannot have resets.\n\r", ch );
        return;
    }

    if ( !IS_BUILDER( ch, pRoom->area ) )
    {
	send_to_char( "Instaroom: Invalid security for editing this area.\n\r",
                      ch );
	return;
    }

   // if ( !IS_SET( pRoom->area->area_flags, AREA_PROTOTYPE ) )
   // {
//	send_to_char( "Room is not in a prototype area.\n\r", ch );
//	return;
 //   }

    instaroom( ch, pRoom );
    SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
    if ( pRoom->reset_first == NULL )
	send_to_char( "Resets cleared.\n\r", ch );
    else
	send_to_char( "Resets created.\n\r", ch );

    return;
}


void
do_resets( CHAR_DATA *ch, char *argument )
{
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		arg3[MAX_INPUT_LENGTH];
    char		arg4[MAX_INPUT_LENGTH];
    char		arg5[MAX_INPUT_LENGTH];
    RESET_DATA *	pReset = NULL;
    AREA_DATA *		pOther;
    MOB_INDEX_DATA *	pMob;
    OBJ_INDEX_DATA *	pObj;
    OBJ_INDEX_DATA *	pContainer;
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    ROOM_INDEX_DATA *	pRoom;
    int			vnum;
    bitvector		wflag;
    bool		fObj;
    bool		fMob;
    bool		found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );

    /*
     * Display resets in current room.
     * -------------------------------
     */
    if ( arg1[0] == '\0' || !str_prefix( arg1, "list" ) )
    {
	if ( ch->in_room->reset_first )
	{
	    send_to_char(
		"Resets: M = mobile, R = room, O = object, "
		"P = pet, S = shopkeeper\n\r", ch );
	    display_resets( ch );
	    return;
	}
	else
	{
	    send_to_char( "No resets in this room.\n\r", ch );
	    return;
	}
    }

    if ( !str_prefix( arg1, "find" ) )
    {
	if ( !str_prefix( arg2, "obj" ) && is_number( arg3 ) )
	{
	    vnum = atoi( arg3 );
	    if ( get_obj_index( vnum ) == NULL )
	    {
	        send_to_char( "No object has that vnum.\n\r", ch );
	        return;
	    }
	    fObj = TRUE;
	    fMob = FALSE;
	}
	else if ( !str_prefix( arg2, "mob" ) && is_number( arg3 ) )
	{
	    vnum = atoi( arg3 );
	    if ( get_mob_index( vnum ) == NULL )
	    {
	        send_to_char( "No mobile has that vnum.\n\r", ch );
	        return;
	    }
	    fObj = FALSE;
	    fMob = TRUE;
	}
	else if ( is_number( arg2 ) )
	{
	    vnum = atoi( arg2 );
	    if ( get_mob_index( vnum ) == NULL && get_obj_index( vnum ) == NULL )
	    {
	        send_to_char( "No mob or object has that vnum.\n\r", ch );
	        return;
	    }
	    fObj = TRUE;
	    fMob = TRUE;
	}
	else
	{
	    do_function( ch, do_resets, "help" );
	    return;
	}
	pBuf = new_buf( );
	found = FALSE;
	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    for ( pRoom = pArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
	    {
                for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
                {
                    if ( pReset->arg1 == vnum )
                    {
                        switch ( UPPER( pReset->command ) )
                        {
                        case 'M':
                            if ( fMob )
                            {
                                buf_printf( pBuf, "[%5d] %c %s`X\n\r", pRoom->vnum, pReset->command, pRoom->name );
                                found = TRUE;
                            }
                            break;
                        case 'E':
                        case 'G':
                        case 'O':
                        case 'P':
                            if ( fObj )
                            {
                                buf_printf( pBuf, "[%5d] %c %s`X\n\r", pRoom->vnum, pReset->command, pRoom->name );
                                found = TRUE;
                            }
                            break;
                        }
                    }
                }
	    }
	}
	if ( found )
	    page_to_char( buf_string( pBuf ), ch );
	else
	    send_to_char( "No resets found for that vnum.\n\r", ch );
	free_buf( pBuf );
	return;
    }

    if ( !str_cmp( arg1, "walk" ) )
    {
        if ( ch->in_room == NULL )
        {
            send_to_char( "Nowhere has no resets.\n\r", ch );
            return;
        }
        if ( ch->in_room->reset_first == NULL )
        {
            send_to_char( "No resets in this room.\n\r", ch );
            return;
        }
        pBuf = new_buf( );
        for ( pReset = ch->in_room->reset_first; pReset != NULL; pReset = pReset->next )
        {
            buf_printf( pBuf, "%c %5d %5d %5d %5d\n\r", pReset->command,
                        pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4 );
        }
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
        return;
    }

    if ( !IS_BUILDER( ch, ch->in_room->area ) )
    {
	send_to_char( "Resets: Invalid security for editing this area.\n\r",
                      ch );
	return;
    }

    /*
     * Take index number and search for commands.
     * ------------------------------------------
     */
    if ( is_number( arg1 ) )
    {
	ROOM_INDEX_DATA *pRoom = ch->in_room;

	/*
	 * Delete a reset.
	 * ---------------
	 */
	if ( !str_cmp( arg2, "delete" ) )
	{
	    int insert_loc = atoi( arg1 );

	    if ( !ch->in_room->reset_first )
	    {
		send_to_char( "No resets in this area.\n\r", ch );
		return;
	    }

	    if ( insert_loc-1 <= 0 )
	    {
		insert_loc = 1;
		pReset = pRoom->reset_first;

		if ( pReset->command == 'M'
		&&   pReset->next != NULL
		&&   ( pReset->next->command == 'E'
		||     pReset->next->command == 'G' ) )
		{
		    send_to_char( "Mobile is equipped.  Delete equipping reset first.\n\r", ch );
		    return;
		}
		if ( pReset->command == 'O'
		&&   pReset->next != NULL
		&&   pReset->next->command == 'P' )
		{
		    send_to_char( "Object has object reset into it.  Delete that reset first.\n\r", ch );
		    return;
		}

		pRoom->reset_first = pRoom->reset_first->next;
		if ( !pRoom->reset_first )
		    pRoom->reset_last = NULL;
	    }
	    else
	    {
		int iReset = 0;
		RESET_DATA *prev = NULL;

		for ( pReset = pRoom->reset_first;
		  pReset;
		  pReset = pReset->next )
		{
		    if ( ++iReset == insert_loc )
			break;
		    prev = pReset;
		}

		if ( !pReset )
		{
		    send_to_char( "Reset not found.\n\r", ch );
		    return;
		}

		if ( pReset->command == 'M'
		&&   pReset->next != NULL
		&&   ( ( pReset->next->command == 'E'
		||     pReset->next->command == 'G' ) ) )
		{
		    send_to_char( "Mobile is equipped.  Delete equipping reset first.\n\r", ch );
		    return;
		}
		if ( ( pReset->command == 'O' || pReset->command == 'G' || pReset->command == 'E' )
		&&   pReset->next != NULL
		&&   pReset->next->command == 'P' )
		{
		    send_to_char( "Object has object reset into it.  Delete that reset first.\n\r", ch );
		    return;
		}

		if ( prev )
		    prev->next = prev->next->next;
		else
		    pRoom->reset_first = pRoom->reset_first->next;

		for ( pRoom->reset_last = pRoom->reset_first;
		  pRoom->reset_last->next;
		  pRoom->reset_last = pRoom->reset_last->next );
	    }

	    free_reset_data( pReset );
	    SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
	    send_to_char( "Reset deleted.\n\r", ch );
	    return;
	}
	else
	if ( is_number(arg3) )
	{
	switch( arg2[0] )
	{
	    case 'M':
	    case 'm':
		pMob = get_mob_index( atoi( arg3 ) );
		if ( !pMob )
		{
		    send_to_char( "No such mobile.\n\r", ch );
		    return;
		}
		pOther = pMob->area;
		if ( IS_SET( pOther->area_flags, AREA_PROTOTYPE )
		     && !IS_BUILDER( ch, pOther ) )
		{
		    send_to_char( "Can't reset a mob from that area.\n\r", ch );
		    return;
		}
		pReset = new_reset_data();
		pReset->command = UPPER(arg2[0]);
		pReset->arg1 = atoi(arg3);
		pReset->arg3 = ch->in_room->vnum;
		pReset->arg4 = is_number(arg4) ? atoi(arg4) : 1;
		pReset->arg2 = is_number(arg5) ? atoi(arg5) : pReset->arg4;
		break;

	    case 'E':
	    case 'e':
                if ( !validate_reset( ch, atoi( arg1 ), 'M' ) )
                {
                    send_to_char( "No mobile to equip.\n\r", ch );
                    return;
                }
		pObj = get_obj_index( atoi( arg3 ) );
		if ( pObj == NULL )
		{
		    send_to_char( "No such object.\n\r", ch );
		    return;
		}
		pOther = pObj->area;
		if ( IS_SET( pOther->area_flags, AREA_PROTOTYPE )
		     && !IS_BUILDER( ch, pOther ) )
		{
		    send_to_char( "Can't reset an object from that area.\n\r", ch );
		    return;
		}
		if ( ( wflag = flag_value( wear_loc_types, arg4 ) ) == NO_FLAG )
		{
		    send_to_char( "Bad wear_loc.\n\r", ch );
		    return;
		}
		pReset = new_reset_data( );
		pReset->command = wflag != WEAR_NONE ? 'E' : 'G';
		pReset->arg1 = atoi( arg3 );
		pReset->arg2 = is_number( arg5 ) ? atoi( arg5 ) : -1;
		pReset->arg3 = wflag;
		pReset->arg4 = 0;
		break;

	    case 'O':
	    case 'o':
		pObj = get_obj_index( atoi( arg3 ) );
		if ( pObj == NULL )
		{
		    send_to_char( "No such object.\n\r", ch );
		    return;
		}

		pOther = pObj->area;
		if ( IS_SET( pOther->area_flags, AREA_PROTOTYPE )
		     && !IS_BUILDER( ch, pOther ) )
		{
		    send_to_char( "Can't reset an object from that area.\n\r", ch );
		    return;
		}
		pReset = new_reset_data();
		pReset->command = UPPER(arg2[0]);
		pReset->arg1 =	atoi(arg3);
		pReset->arg2 =	is_number(arg4) ? atoi(arg4) : -1;
		pReset->arg3 =	ch->in_room->vnum;
		pReset->arg4 =	0;
		break;

	   case 'G':
	   case 'g':
                if ( !validate_reset( ch, atoi( arg1 ), 'M' ) )
                {
                    send_to_char( "No mobile to equip.\n\r", ch );
                    return;
                }
		pObj = get_obj_index( atoi( arg3 ) );
		if ( !pObj )
		{
		    send_to_char( "No such object.\n\r", ch );
		    return;
		}
		pOther = pObj->area;
		if ( IS_SET( pOther->area_flags, AREA_PROTOTYPE )
		     && !IS_BUILDER( ch, pOther ) )
		{
		    send_to_char( "Can't reset an object from that area.\n\r", ch );
		    return;
		}
		pReset = new_reset_data();
		pReset->command = UPPER(arg2[0]);
		pReset->arg1 = atoi(arg3);
		pReset->arg2 = is_number(arg4) ? atoi(arg4) : -1;
		pReset->arg3 = 0;
		pReset->arg4 = 0;
		break;

	    case 'P':
	    case 'p':
                if ( !validate_reset( ch, atoi( arg1 ),'O' )
                &&   !validate_reset( ch, atoi( arg1 ), 'G' )
                &&   !validate_reset( ch, atoi( arg1 ), 'E' ) )
                {
                    send_to_char( "No container to load.\n\r", ch );
                    return;
                }
		pObj = get_obj_index( atoi( arg3 ) );
		if ( pObj == NULL )
		{
		    send_to_char( "No such object.\n\r", ch );
		    return;
		}
		pOther = pObj->area;
		pContainer = get_obj_index( atoi( arg4 ) );
		if ( pContainer == NULL )
		{
		    send_to_char( "Object being reset into does not exist.\n\r", ch );
		    return;
		}
		if ( pContainer->item_type != ITEM_CONTAINER
		&&   pContainer->item_type != ITEM_SHEATH )
		{
		    send_to_char( "Object being reset into is not a container.\n\r", ch );
                    return;
		}
		if ( IS_SET( pOther->area_flags, AREA_PROTOTYPE )
		     && !IS_BUILDER( ch, pOther ) )
		{
		    send_to_char( "Can't reset an object from that area.\n\r", ch );
		    return;
		}
		pReset = new_reset_data( );
		pReset->command = UPPER(arg2[0]);
		pReset->arg1 = atoi( arg3 );
		pReset->arg2 = is_number( arg5 ) ? atoi( arg5 ) : 1;
		pReset->arg3 = pContainer->vnum;;
		pReset->arg4 = 99;
		break;

	    case 'R':
	    case 'r':
		pReset = new_reset_data();
		pReset->command = UPPER(arg2[0]);
		pReset->arg1 = ch->in_room->vnum;
		pReset->arg2 = UMIN( atoi(arg3), MAX_DIR );
		pReset->arg3 = 0;
		pReset->arg4 = 0;
		break;

	    default:
		    send_to_char( "Syntax: RESET <number> E <vnum> <wear_loc> <max#>\n\r", ch );
		    send_to_char( "        RESET <number> P <vnum> <in vnum> <max#>\n\r", ch );
		    send_to_char( "        RESET <number> O <vnum> <max#>\n\r", ch );
		    send_to_char( "        RESET <number> G <vnum> <max#>\n\r", ch );
		    send_to_char( "        RESET <number> M <vnum> <max room> <max world>\n\r", ch );
		    send_to_char( "        RESET <number> R <max_dir>\n\r",ch );
		    send_to_char( "        RESET <number> DELETE\n\r", ch );
		    send_to_char( "M=MOB O=OBJ E=EQ G=INV P=IN_OBJ R=RANDOM_EXITS\n\r", ch );
		    return;
	}

	add_reset( ch->in_room, pReset, atoi(arg1) );
	SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
	send_to_char( "Reset added.\n\r", ch );
	return;
	}

    send_to_char( "Syntax: RESET <number> E <vnum> <wear_loc> <max#>\n\r", ch );
    send_to_char( "        RESET <number> P <vnum> <in vnum> <max#>\n\r", ch );
    send_to_char( "        RESET <number> O <vnum> <max#>\n\r", ch );
    send_to_char( "        RESET <number> G <vnum> <max#>\n\r", ch );
    send_to_char( "        RESET <number> M <vnum> <max room> <max world>\n\r", ch );
    send_to_char( "        RESET <number> R <max_dir>\n\r",ch );
    send_to_char( "        RESET <number> DELETE\n\r", ch );
    send_to_char( "        RESET FIND <vnum>\n\r", ch );
    send_to_char( "        RESET FIND MOB <vnum>\n\r", ch );
    send_to_char( "        RESET FIND OBJ <vnum>\n\r", ch );
    send_to_char( "M=MOB O=OBJ E=EQ G=INV P=IN_OBJ R=RANDOM_EXITS\n\r", ch );
    return;

    }

    send_to_char( "Syntax: RESET <number> E <vnum> <wear_loc> <max#>\n\r", ch );
    send_to_char( "        RESET <number> P <vnum> <in vnum> <max#>\n\r", ch );
    send_to_char( "        RESET <number> O <vnum> <max#>\n\r", ch );
    send_to_char( "        RESET <number> G <vnum> <max#>\n\r", ch );
    send_to_char( "        RESET <number> M <vnum> <max room> <max world>\n\r", ch );
    send_to_char( "        RESET <number> R <max_dir>\n\r",ch );
    send_to_char( "        RESET <number> DELETE\n\r", ch );
    send_to_char( "        RESET FIND <vnum>\n\r", ch );
    send_to_char( "        RESET FIND MOB <vnum>\n\r", ch );
    send_to_char( "        RESET FIND OBJ <vnum>\n\r", ch );
    send_to_char( "M=MOB O=OBJ E=EQ G=INV P=IN_OBJ R=RANDOM_EXITS\n\r", ch );
    return;

}


void
do_mreset( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    MOB_INDEX_DATA *	pMobIndex;
    RESET_DATA *	pReset;
    CHAR_DATA *		newmob;
    AREA_DATA *		pOther;
    char		arg1[ MAX_INPUT_LENGTH ];
    char		arg2[ MAX_INPUT_LENGTH ];
    char		output [ MAX_STRING_LENGTH ];

    pRoom = ch->in_room;
    if ( pRoom == NULL )
    {
        send_to_char( "You can't create any resets here.\n\r", ch );
        return;
    }

    if ( !IS_BUILDER( ch, pRoom->area ) )
    {
	send_to_char( "Mreset: Invalid security for editing this area.\n\r",
                      ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
	send_to_char ( "Syntax:  mreset <vnum> <max_room #> <max_world #>\n\r", ch );
	return;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "No mobile has that vnum.\n\r", ch );
	return;
    }

    if ( ( pOther = pMobIndex->area ) != pRoom->area
	&& IS_SET( pOther->area_flags, AREA_PROTOTYPE )
	&& !IS_BUILDER( ch, pOther ) )
    {
	send_to_char( "Cannot load mob from another prototype area.\n\r", ch );
	return;
    }

    /*
     * Create the mobile reset.
     */
    pReset              = new_reset_data();
    pReset->command	= 'M';
    pReset->arg1	= pMobIndex->vnum;
    pReset->arg4	= is_number( arg2 ) ? atoi( arg2 ) : MAX_MOB;
    pReset->arg3	= pRoom->vnum;
    pReset->arg2	= is_number( argument ) ? atoi( argument ) : pReset->arg4;
    add_reset( pRoom, pReset, 0/* Last slot*/ );

    /*
     * Create the mobile.
     */
    newmob = create_mobile( pMobIndex );
    char_to_room( newmob, pRoom );

    sprintf( output, "%s (%d) has been loaded and added to resets.\n\r"
	"There will be a maximum of %d loaded to this room.\n\r",
	capitalize( pMobIndex->short_descr ),
	pMobIndex->vnum,
	pReset->arg4 );
    send_to_char( output, ch );
    act( "$n has created $N!", ch, NULL, newmob, TO_ROOM );

    SET_BIT( pRoom->area->area_flags, AREA_CHANGED );

    return;
}


void
do_oreset( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    OBJ_INDEX_DATA *	pObjIndex;
    OBJ_DATA *		newobj;
    OBJ_DATA *		to_obj;
    CHAR_DATA *		to_mob;
    AREA_DATA *		pOther;
    char		arg1 [MAX_INPUT_LENGTH];
    char		arg2 [MAX_INPUT_LENGTH];
    char		arg3 [MAX_INPUT_LENGTH];
    int			olevel = 0;
    RESET_DATA *	mReset;
    RESET_DATA *	nReset;
    RESET_DATA *	oReset;
    RESET_DATA *	pReset;
    char		output [MAX_STRING_LENGTH];
    int			reset_count;

    pRoom = ch->in_room;
    if ( pRoom == NULL )
    {
        send_to_char( "You can't create any resets here.\n\r", ch );
        return;
    }

    if ( !IS_BUILDER( ch, pRoom->area ) )
    {
	send_to_char( "Oreset: Invalid security for editing this area.\n\r",
                      ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
	send_to_char ( "Syntax:  oreset <vnum> <args>\n\r", ch );
	send_to_char ( "        -no_args               = into room\n\r", ch );
	send_to_char ( "        -<obj_name>            = into obj\n\r", ch );
	send_to_char ( "        -<mob_name> <wear_loc> = into mob\n\r", ch );
	return;
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "Oreset: No object has that vnum.\n\r", ch );
	return;
    }

    if ( ( pOther = pObjIndex->area ) != pRoom->area
	&& IS_SET( pOther->area_flags, AREA_PROTOTYPE )
	&& !IS_BUILDER( ch, pOther ) )
    {
	send_to_char( "Oreset: Cannot load object from another prototype area.\n\r", ch );
	return;
    }

    /*
     * Load into room.
     */
    if ( arg2[0] == '\0' )
    {
	pReset		= new_reset_data();
	pReset->command	= 'O';
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= -1;
	pReset->arg3	= pRoom->vnum;
	add_reset( pRoom, pReset, 0/* Last slot*/ );

	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	obj_to_room( newobj, pRoom );

	sprintf( output, "%s (%d) has been loaded and added to resets.\n\r",
	    capitalize( pObjIndex->short_descr ),
	    pObjIndex->vnum );
	send_to_char( output, ch );
    }
    else
    /*
     * Load into object's inventory.
     */
    if ( ( to_obj = get_obj_list( ch, arg2, pRoom->contents ) ) != NULL )
    {
	/*
	 * Make sure the object is already reset here.
	 */
	reset_count = 0;
	for ( oReset = pRoom->reset_first; oReset != NULL; oReset = oReset->next )
	{
	    reset_count++;
	    if ( oReset->command == 'O'
	    &&	 oReset->arg1 == to_obj->pIndexData->vnum )
	        break;
	}
	if ( oReset == NULL )
	{
	    ch_printf( ch, "Oreset:  %s is not reset in this room.\n\r", to_obj->short_descr );
	    return;
	}

	/*
	 * Make sure to_obj is a container or sheath.
	 */
	if ( to_obj->item_type != ITEM_CONTAINER
	&&   to_obj->item_type != ITEM_SHEATH )
	{
	    ch_printf( ch, "Oreset:  %s is neither a container nor sheath.\n\r", to_obj->short_descr );
	    return;
	}

	pReset		= new_reset_data();
	pReset->command	= 'P';
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= URANGE( 1, atoi( argument ), 9 );
	pReset->arg3	= to_obj->pIndexData->vnum;
	pReset->arg4	= 99;

	/*
	 * Find the first reset slot after the container.
	 * This would be the next M, O or R reset.
	 */
	for ( nReset = oReset->next; nReset != NULL; nReset = nReset->next )
	{
	    reset_count++;
	    if ( nReset->command == 'M'
	    ||	 nReset->command == 'O'
	    ||	 nReset->command == 'R' )
	        break;
	}
	if ( nReset == NULL )
	    reset_count = 0; /* force to end of reset list */
	add_reset( pRoom, pReset, reset_count );

	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	newobj->cost = 0;
	obj_to_obj( newobj, to_obj );

	sprintf( output, "%s (%d) has been loaded into "
	    "%s (%d) and added to resets.\n\r",
	    capitalize( newobj->short_descr ),
	    newobj->pIndexData->vnum,
	    to_obj->short_descr,
	    to_obj->pIndexData->vnum );
	send_to_char( output, ch );
    }
    else
    /*
     * Load into mobile's inventory.
     */
    if ( ( to_mob = get_char_room( ch, arg2 ) ) != NULL )
    {
	int	wear_loc;

	/*
	 * Make sure it's a mobile!
	 */
	if ( !IS_NPC( to_mob ) )
	{
	    ch_printf( ch, "Oreset:  %s is not a mobile.\n\r", to_mob->name );
	    return;
	}

	/*
	 * Make sure the mobile is already reset here.
	 */
	reset_count = 0;
	for ( mReset = pRoom->reset_first; mReset != NULL; mReset = mReset->next )
	{
	    reset_count++;
	    if ( mReset->command == 'M'
	    &&	 mReset->arg1 == to_mob->pIndexData->vnum )
		break;
	}
	if ( mReset == NULL )
	{
	    ch_printf( ch, "Oreset:  %s is not reset in this room.\n\r", to_mob->short_descr );
	    return;
	}

	/*
	 * Make sure the location on mobile is valid.
	 */
	if ( *argument == '\0' || ( *argument == 'i' && !str_prefix( argument, "inventory" ) ) )
	{
	    wear_loc = WEAR_NONE;
	}
	else
	{
	    if ( ( wear_loc = flag_value( wear_loc_types, argument ) ) == NO_FLAG )
	    {
		argument = one_argument( argument, arg3 );
		if ( ( to_obj = get_obj_list( ch, arg3, to_mob->carrying ) ) == NULL )
		{
		    ch_printf( ch, "Oreset: Invalid wear_loc or container \"%s\".  '? wear-loc'\n\r", arg3 );
		    return;
		}

		/*
		 * Make sure the container is already reset onto the mob.
		 */
		for ( oReset = mReset->next; oReset != NULL; oReset = oReset->next )
		{
		    reset_count++;
		    if ( ( oReset->command == 'G' || oReset->command == 'E' )
		    &&	 oReset->arg1 == to_obj->pIndexData->vnum )
		        break;
                    if ( oReset->command != 'E' && oReset->command != 'G' )
                    {
                        oReset = NULL;
                        break;
                    }
		}

                if ( oReset == NULL )
                {
                    ch_printf( ch, "Oreset:  %s is not reset on %s.\n\r", to_obj->short_descr, to_mob->short_descr );
                    return;
                }

                if ( to_obj->item_type != ITEM_CONTAINER
                &&   to_obj->item_type != ITEM_SHEATH )
                {
                    ch_printf( ch, "Oreset:  %s is neither a container nor sheath.\n\r", to_obj->short_descr );
                    return;
                }
                pReset		= new_reset_data();
                pReset->command	= 'P';
                pReset->arg1	= pObjIndex->vnum;
                pReset->arg2	= URANGE( 1, atoi( argument ), 9 );
                pReset->arg3	= to_obj->pIndexData->vnum;
                pReset->arg4	= 99;

                /*
                 * Find the first reset slot after the container.
                 * This would be the first reset that is not a 'P' reset.
                 */
                for ( nReset = oReset->next; nReset != NULL; nReset = nReset->next )
                {
                    reset_count++;
                    if ( nReset->command != 'P' )
                        break;
                }
                if ( nReset == NULL )
                    reset_count = 0; /* force to end of reset list */
                add_reset( pRoom, pReset, reset_count );
                newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
                obj_to_obj( newobj, to_obj );

                sprintf( output, "%s (%d) has been loaded into "
                         "%s (%d) carried by %s (%d) and added to resets.\n\r",
                         capitalize( newobj->short_descr ),
                         newobj->pIndexData->vnum,
                         to_obj->short_descr,
                         to_obj->pIndexData->vnum,
                         to_mob->short_descr,
                         to_mob->pIndexData->vnum );
                send_to_char( output, ch );

                act( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
                SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
		return;
	    }
	}

	/*
	 * Disallow loading a sword(WEAR_WIELD) into WEAR_HEAD.
	 */
	if ( !IS_SET( pObjIndex->wear_flags, wear_bit(wear_loc) ) )
	{
	    sprintf( output,
	        "%s (%d) has wear flags: [%s]\n\r",
	        capitalize( pObjIndex->short_descr ),
	        pObjIndex->vnum,
		flag_string( wear_flags, pObjIndex->wear_flags ) );
	    send_to_char( output, ch );
	    return;
	}

	/*
	 * Can't load into same position.
	 */
	if ( wear_loc != WEAR_NONE && get_eq_char( to_mob, wear_loc ) )
	{
	    send_to_char( "Oreset:  Object already equipped.\n\r", ch );
	    return;
	}

	pReset		= new_reset_data();
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= -1;
	pReset->arg3	= wear_loc;
	if ( pReset->arg3 == WEAR_NONE )
	    pReset->command = 'G';
	else
	    pReset->command = 'E';

	/*
	 * Find the first reset slot after our mob.
	 * This would be the next M, O or R reset.
	 */
	for ( nReset = mReset->next; nReset != NULL; nReset = nReset->next )
	{
	    reset_count++;
	    if ( nReset->command == 'M'
	    ||	 nReset->command == 'O'
	    ||	 nReset->command == 'R' )
	        break;
	}
	if ( nReset == NULL )
	    reset_count = 0; /* force to end of reset list */
	add_reset( pRoom, pReset, reset_count );

	olevel  = URANGE( 0, to_mob->level - 2, LEVEL_HERO );
//      newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

	if ( to_mob->pIndexData->pShop )	/* Shop-keeper? */
	{
	    switch ( pObjIndex->item_type )
	    {
	    default:		olevel = 0;				break;
	    case ITEM_PILL:	olevel = number_range(  0, 10 );	break;
	    case ITEM_POTION:	olevel = number_range(  0, 10 );	break;
	    case ITEM_HERB:	olevel = number_range(  0, 10 );	break;
	    case ITEM_SCROLL:	olevel = number_range(  5, 15 );	break;
	    case ITEM_WAND:	olevel = number_range( 10, 20 );	break;
	    case ITEM_STAFF:	olevel = number_range( 15, 25 );	break;
	    case ITEM_ARMOR:	olevel = number_range(  5, 15 );	break;
	    case ITEM_WEAPON:	if ( pReset->command == 'G' )
	    			    olevel = number_range( 5, 15 );
				else
				    olevel = number_fuzzy( olevel );
		break;
	    }

	    newobj = create_object( pObjIndex, olevel );
	    if ( pReset->arg3 == WEAR_NONE )
		SET_BIT( newobj->extra_flags, ITEM_INVENTORY );
	}
	else
	    newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

	obj_to_char( newobj, to_mob );
	if ( pReset->command == 'E' )
	    equip_char( to_mob, newobj, pReset->arg3 );

	sprintf( output, "%s (%d) has been loaded "
	    "%s of %s (%d) and added to resets.\n\r",
	    capitalize( pObjIndex->short_descr ),
	    pObjIndex->vnum,
	    flag_string( wear_loc_strings, pReset->arg3 ),
	    to_mob->short_descr,
	    to_mob->pIndexData->vnum );
	send_to_char( output, ch );
    }
    else	/* Display Syntax */
    {
	send_to_char( "Oreset:  That mobile or container isn't here.\n\r", ch );
	return;
    }

    act( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
    SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
    return;
}


/*****************************************************************************
 Name:		add_reset
 Purpose:	Inserts a new reset in the given index slot.
 Called by:	do_resets, redit_mreset, redit_oreset(olc_room.c).
 ****************************************************************************/
void
add_reset( ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index )
{
    RESET_DATA *reset;
    int iReset = 0;

    if ( !room->reset_first )
    {
	room->reset_first	= pReset;
	room->reset_last	= pReset;
	pReset->next		= NULL;
	return;
    }

    index--;

    if ( index == 0 )	/* First slot (1) selected. */
    {
	pReset->next = room->reset_first;
	room->reset_first = pReset;
	return;
    }

    /*
     * If negative slot( <= 0 selected) then this will find the last.
     */
    for ( reset = room->reset_first; reset->next; reset = reset->next )
    {
	if ( ++iReset == index )
	    break;
    }

    pReset->next	= reset->next;
    reset->next		= pReset;
    if ( !pReset->next )
	room->reset_last = pReset;
    return;
}


/*****************************************************************************
 Name:		change_exit
 Purpose:	Command interpreter for changing exits.
 Called by:	redit_<dir>.  This is a local function.
 ****************************************************************************/
static bool
change_exit( CHAR_DATA *ch, char *argument, int door )
{
    ROOM_INDEX_DATA *	pRoom;
    EXIT_DATA *		pExit;
    ROOM_INDEX_DATA *	pLinkRoom;
    ROOM_INDEX_DATA *	pToRoom;
    AREA_DATA *		pArea;
    char		buf[SHORT_STRING_LENGTH];
    char		command[MAX_INPUT_LENGTH];
    char		arg[MAX_INPUT_LENGTH];
    char		total_arg[MAX_STRING_LENGTH];
    int			rev;
    int			value = 0;

    EDIT_ROOM( ch, pRoom );

    /* Often used data. */
    rev = rev_dir[door];

    if ( argument[0] == '\0' || !str_cmp( argument, "help" ) )
    {
	do_help( ch, "EXIT_OLC" );
	return FALSE;
    }

    /*
     * Check for virtual room
     */
    if ( IS_SET( pRoom->room_flags, ROOM_VIRTUAL ) )
    {
	send_to_char( "REdit:  Cannot modify virtual room exits.\n\r", ch );
	return FALSE;
    }

    /*
     * Now parse the arguments.
     */
    strcpy( total_arg, argument );
    argument = one_argument( argument, command );
    one_argument( argument, arg );


    /* Hand off to epedit/epremove */
    if ( !str_prefix( command, "epedit" ) )
        return redit_epedit( ch, argument, door );
    if ( !str_prefix( command, "epremove" ) )
        return redit_epremove( ch, argument, door );

    if ( !str_cmp( command, "delete" ) )
    {

	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Exit does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( pRoom->exit[door]->to_room == 0 )
	{
	   send_to_char( "REdit:  Bad Room number 0.. don't delete it :)", ch );
	   return FALSE;
	}

	if ( !IS_BUILDER( ch, pRoom->exit[door]->to_room->area ) )
	{
	    send_to_char( "REdit:  Cannot unlink that area.\n\r", ch );
	    return FALSE;
	}

	/*
	 * Remove To Room Exit.
	 */
	if ( pRoom->exit[door]->to_room->exit[rev] )
	{
	    SET_BIT( pRoom->exit[door]->to_room->area->area_flags, AREA_CHANGED );
	    free_exit( pRoom->exit[door]->to_room->exit[rev] );
	    pRoom->exit[door]->to_room->exit[rev] = NULL;
	}

	/*
	 * Remove this exit.
	 */
	free_exit( pRoom->exit[door] );
	pRoom->exit[door] = NULL;

	send_to_char( "Exit unlinked.\n\r", ch );
	return TRUE;
    }

    /*
     * Create a two-way exit.
     */
    if ( !str_cmp( command, "link" ) )
    {
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] link [vnum]\n\r", ch );
	    return FALSE;
	}

	if ( !( pLinkRoom = get_room_index( atoi(arg) ) ) )
	{
	    send_to_char( "REdit:  Room does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( !IS_BUILDER( ch, pLinkRoom->area ) )
	{
	    send_to_char( "REdit:  Cannot link to that area.\n\r", ch );
	    return FALSE;
	}

	if ( pLinkRoom->exit[rev] )
	{
	    send_to_char( "REdit:  Remote side's exit exists.\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )		/* No exit.		*/
	    pRoom->exit[door] = new_exit();

	pRoom->exit[door]->to_room = pLinkRoom;	/* Assign data.		*/
	pRoom->exit[door]->from_room = pRoom;
	pRoom->exit[door]->vnum = value;

	pExit			= new_exit();	/* No remote exit.	*/

	pExit->to_room		= ch->in_room;	/* Assign data.		*/
	pExit->from_room	= pLinkRoom;
	pExit->vnum		= ch->in_room->vnum;

	pLinkRoom->exit[rev]	= pExit;	/* Link exit to room.	*/

	if ( pRoom->sector_type == SECT_INSIDE
	||   pLinkRoom->sector_type == SECT_INSIDE )
	{
	    pRoom->exit[door]->size = pRoom->area->exitsize;
	    pLinkRoom->exit[rev]->size = pRoom->area->exitsize;
	}
	else
	{
	    pRoom->exit[door]->size = SIZE_TITANIC;
	    pLinkRoom->exit[rev]->size = SIZE_TITANIC;
	}

	SET_BIT( pRoom->exit[door]->to_room->area->area_flags, AREA_CHANGED );

	send_to_char( "Two-way link established.\n\r", ch );
	return TRUE;
    }

    /*
     * Create room and make two-way exit.
     */
    if ( !str_cmp( command, "dig" ) )
    {
	if ( arg[0] != '\0' && !is_number( arg ) )
	{
	    send_to_char( "Syntax: [direction] dig <vnum>\n\r", ch );
	    return FALSE;
	}

	if ( ch->in_room->exit[door] != NULL )
	{
	    ch_printf( ch, "REdit:  This room already has an exit %s.\n\r",
		       dir_name[door] );
	    return FALSE;
	}

	if ( arg[ 0 ] == '\0' )
	{
	    pArea = ch->in_room->area;
	    for ( value = pArea->min_vnum; value <= pArea->max_vnum; value++ )
		if ( !get_room_index( value ) )
		    break;
	    if ( value > pArea->max_vnum )
	    {
		send_to_char( "REdit:  No free room vnums in this area.\n\r", ch );
		return FALSE;
	    }
	    sprintf( arg, "%d", value );
	}

	if ( redit_create( ch, arg ) )		/* Create the room.	*/
	{
	    sprintf( buf, "link %s", arg );
	    change_exit( ch, buf, door);	/* Create the exits.	*/
	    if ( IS_SET( ch->act2, PLR_AUTOMOVE ) )
	    {
		char_from_room( ch );
		char_to_room( ch, ch->desc->pEdit );
	    }
	    return TRUE;
	}
	return FALSE;
    }

    /*
     * Create one-way exit.
     */
    if ( !str_cmp( command, "room" ) )
    {
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] room [vnum]\n\r", ch );
	    return FALSE;
	}

	if ( !( pLinkRoom = get_room_index( atoi( arg ) ) ) )
	{
	    send_to_char( "REdit:  Room does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( !IS_BUILDER( ch, pLinkRoom->area ) )
	{
	    send_to_char( "REdit:  Cannot link to that area.\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();

	pRoom->exit[door]->to_room = pLinkRoom;
	pRoom->exit[door]->from_room = pRoom;
	pRoom->exit[door]->vnum = value;

	pRoom->exit[door]->size = pRoom->sector_type == SECT_INSIDE
				  ? pRoom->area->exitsize : SIZE_TITANIC;

	send_to_char( "One-way link established.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "remove" ) )
    {
	if ( arg[0] == '\0' )
	{
	    send_to_char( "Syntax:  [direction] remove [key/name/desc]\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Exit does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( !str_cmp( argument, "key" ) )
	{
	    pRoom->exit[door]->key = 0;
            send_to_char( "Exit key removed.\n\r", ch );
            return TRUE;
	}

	if ( !str_cmp( argument, "name" ) )
	{
	    free_string( pRoom->exit[door]->keyword );
	    pRoom->exit[door]->keyword = &str_empty[0];
            send_to_char( "Exit name removed.\n\r", ch );
            return TRUE;
	}

	if ( argument[0] == 'd' && !str_prefix( argument, "description" ) )
	{
	    free_string( pRoom->exit[door]->description );
	    pRoom->exit[door]->description = &str_empty[0];
            send_to_char( "Exit description removed.\n\r", ch );
            return TRUE;
	}

	send_to_char( "Syntax:  [direction] remove [key/name/desc]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "size" ) )
    {
	if ( arg[0] == '\0' )
	{
	    send_to_char( "Syntax:  [direction] size [value]\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Exit does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( is_number( argument ) )
	    value = atoi( argument );
	else
	    value = flag_value( size_types, argument );

	if ( value < 0 || value >= MAX_SIZE )
	{
	    send_to_char( "REdit:  Bad value for size.\n\r", ch );
	    return FALSE;
	}

	if ( ( pToRoom = pRoom->exit[door]->to_room ) != NULL
	&&   pToRoom->exit[rev] != NULL
	&&   pToRoom->exit[rev]->to_room == pRoom )
	{
	    if ( !IS_BUILDER( ch, pToRoom->area ) )
	    {
		send_to_char( "REdit:  Cannot modify other side of exit.\n\r", ch );
		return FALSE;
	    }
	    pToRoom->exit[rev]->size = value;
	    SET_BIT( pToRoom->area->area_flags, AREA_CHANGED );
	}

	pRoom->exit[door]->size = value;

	send_to_char( "Exit size set.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "key" ) )
    {
	OBJ_INDEX_DATA *pObjIndex;

	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] key [vnum]\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Exit does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( !( pObjIndex = get_obj_index( atoi( arg ) ) ) )
	{
	    send_to_char( "REdit:  Item does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( pObjIndex->item_type != ITEM_KEY && pObjIndex->item_type != ITEM_PERMKEY)
	{
	    send_to_char( "REdit:  Item is not a key.\n\r", ch );
	    return FALSE;
	}

	pRoom->exit[door]->key = pObjIndex->vnum;

	send_to_char( "Exit key set.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "name" ) )
    {
	if ( arg[0] == '\0' )
	{
	    send_to_char( "Syntax:  [direction] name [string]\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Exit does not exist.\n\r", ch );
	    return FALSE;
	}

	free_string( pRoom->exit[door]->keyword );
	pRoom->exit[door]->keyword = str_dup( argument );

	send_to_char( "Exit name set.\n\r", ch );
	return TRUE;
    }

    if ( command[0] == 'd' && !str_prefix( command, "description" ) )
    {
	if ( arg[0] == '\0' )
	{
	    if ( pRoom->exit[door] == NULL )
	    {
		send_to_char( "REdit:  Exit does not exist.\n\r", ch );
		return FALSE;
	    }

	    string_append( ch, &pRoom->exit[door]->description );
	    return TRUE;
	}

	send_to_char( "Syntax:  [direction] desc\n\r", ch );
	return FALSE;
    }

    if ( command[0] == 's' && !str_prefix( command, "sound" ) )
    {
	if ( arg[0] != '\0' )
	{
	    if ( pRoom->exit[door] == NULL )
	    {
		send_to_char( "REdit:  Exit does not exist.\n\r", ch );
		return FALSE;
	    }

	    if ( !str_prefix( arg, "closed" ) )
	    {
		string_append( ch, &pRoom->exit[door]->sound_closed );
		return TRUE;
	    }
	    else if ( !str_prefix( arg, "open" ) )
	    {
		string_append( ch, &pRoom->exit[door]->sound_open );
		return TRUE;
	    }
	}

	send_to_char( "Syntax:  [direction] sound [open|closed]\n\r", ch );
	return FALSE;
    }

    /*
     * Set the exit flags, needs full argument.
     * ----------------------------------------
     */
    if ( ( value = flag_value( exit_flags, total_arg ) ) != NO_FLAG )
    {
	ROOM_INDEX_DATA *pToRoom;

	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Exit does not exist.\n\r", ch );
	    return FALSE;
	}

	/*
	 * Set door bits for this room.
	 */
	TOGGLE_BIT(pRoom->exit[door]->rs_flags, value);
	pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

	/*
	 * Set door bits of connected room.
	 * Skip one-way exits and non-existant rooms.
	 */
	if ( ( pToRoom = pRoom->exit[door]->to_room ) != NULL
	&&   pToRoom->exit[rev] != NULL
	&&   pToRoom->exit[rev]->to_room == pRoom )
	{
	    value &= ~EX_HIDDEN; /* Don't set hidden flag on the other side */
	    value &= ~EX_SECRET; /* don't set secret flag on the other side either */
	    TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
	    pToRoom->exit[rev]->exit_info =  pToRoom->exit[rev]->rs_flags;
	}

	send_to_char( "Exit flag toggled.\n\r", ch );
	return TRUE;
    }

    /*
     * Subcommand not found.
     * Recursively call self with no arguments and return.
     */
    change_exit( ch, "", DIR_NORTH );
    return FALSE;
}


bool
delete_room( ROOM_INDEX_DATA *pRoom )
{
    CHAR_DATA *		vch;
    CHAR_DATA *		v_next;
    OBJ_DATA *		obj;
    OBJ_DATA *		obj_next;
    ROOM_INDEX_DATA *	exRoom;
    ROOM_INDEX_DATA *	pPrev;
    int			dir;
    int			iHash;

    if ( pRoom == NULL )
        return FALSE;

    /* Remove all chars.  Purge mobs, trans players. */
    for ( vch = pRoom->people; vch != NULL; vch = v_next )
    {
        v_next = vch->next_in_room;
        if ( vch->deleted )
            continue;
        if ( IS_NPC( vch ) && !IS_SET( vch->act, ACT_PET ) && !IS_SET( vch->act, ACT_MOUNT ) )
        {
            extract_char( vch, TRUE );
        }
        else
        {
            char_from_room( vch );
            char_to_room( vch, get_room_index( ROOM_VNUM_TEMPLE ) );
            do_look( vch, "auto" );
        }
    }

    /* Remove all objects. */
    for ( obj = pRoom->contents; obj; obj = obj_next )
    {
        obj_next = obj->next;
        if ( !obj->deleted )
            extract_obj( obj );
    }

    if ( pRoom->in_room != NULL )
        room_from_room( pRoom );

    /* Delete all exits to this room */
    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( exRoom = room_index_hash[iHash]; exRoom != NULL; exRoom = exRoom->next )
        {
            for ( dir = 0; dir < MAX_DIR; dir++ )
            {
                if ( exRoom->exit[dir] != NULL && exRoom->exit[dir]->to_room == pRoom )
                {
                    SET_BIT( exRoom->area->area_flags, AREA_CHANGED );
                    free_exit( exRoom->exit[dir] );
                    exRoom->exit[dir] = NULL;
                }
            }
        }
    }

    iHash = pRoom->vnum % MAX_KEY_HASH;
    if ( room_index_hash[iHash] == pRoom )
    {
        room_index_hash[iHash] = pRoom->next;
    }
    else
    {
        for ( exRoom = room_index_hash[iHash]; exRoom != NULL; exRoom = exRoom->next )
        {
            if ( exRoom->next == pRoom )
            {
                exRoom->next = pRoom->next;
                break;
            }
        }
    }

    if ( pRoom->area->room_list == pRoom )
    {
        pRoom->area->room_list = pRoom->next_in_area;
    }
    else
    {
        for ( pPrev = pRoom->area->room_list;
              pPrev->next_in_area != NULL;
              pPrev = pPrev->next_in_area )
        {
            if ( pPrev->next_in_area == pRoom )
                break;
        }
        if ( pPrev->next_in_area == NULL )
        {
            bugf( "Delete_room: room not found." );
        }
        else
        {
            pPrev->next_in_area = pRoom->next_in_area;
        }
    }

    free_room_index( pRoom );

    return TRUE;
}


static void
display_resets( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA	*pRoom;
    RESET_DATA		*pReset;
    MOB_INDEX_DATA	*pMob = NULL;
    char 		buf   [ MAX_STRING_LENGTH ];
    char		buf2  [SHORT_STRING_LENGTH];
    char 		final [ MAX_STRING_LENGTH ];
    char		mname[MAX_INPUT_LENGTH];
    char		oname[MAX_INPUT_LENGTH];
    char		rname[MAX_INPUT_LENGTH];
    int 		iReset = 0;

    EDIT_ROOM(ch, pRoom);
    final[0]  = '\0';

    send_to_char (
  " No.  Loads    Description       Location         Vnum    Max   Description"
  "\n\r"
  "==== ======== ============= =================== ======== [R  W] ==========="
  "\n\r", ch );

    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
	OBJ_INDEX_DATA  *pObj;
	MOB_INDEX_DATA  *pMobIndex;
	OBJ_INDEX_DATA  *pObjIndex;
	OBJ_INDEX_DATA  *pObjToIndex;
	ROOM_INDEX_DATA *pRoomIndex;

	final[0] = '\0';
	sprintf( final, "[%2d] ", ++iReset );

	switch ( pReset->command )
	{
	default:
	    sprintf( buf, "Bad reset command: %c.", pReset->command );
	    strcat( final, buf );
	    break;

	case 'M':
	    if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1 );
                strcat( final, buf );
                break;
	    }

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3 );
                strcat( final, buf );
                break;
	    }

            pMob = pMobIndex;
	    strip_color( mname, pMob->short_descr );
	    strip_color( rname, pRoomIndex->name );
            sprintf( buf, "M[%5d] %-13.13s in room             R[%5d] [%-2d%2d] %-15.14s\n\r",
                       pReset->arg1, mname, pReset->arg3,
                       pReset->arg4, pReset->arg2, rname );
            strcat( final, buf );

	    /*
	     * Check for pet shop.
	     * -------------------
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;

		pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
		if ( pRoomIndexPrev
		    && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
                    final[5] = 'P';
	    }

	    break;

	case 'O':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Load Object - Bad Object %d\n\r",
		    pReset->arg1 );
                strcat( final, buf );
                break;
	    }

            pObj       = pObjIndex;

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Load Object - Bad Room %d\n\r", pReset->arg3 );
                strcat( final, buf );
                break;
	    }

	    strip_color( oname, pObj->short_descr );
	    strip_color( rname, pRoomIndex->name );
            sprintf( buf, "O[%5d] %-13.13s in room             "
                          "R[%5d]        %-15.15s\n\r",
                          pReset->arg1, oname,
                          pReset->arg3, rname );
            strcat( final, buf );

	    break;

	case 'P':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Put Object - Bad Object %d\n\r",
                    pReset->arg1 );
                strcat( final, buf );
                break;
	    }

            pObj       = pObjIndex;

	    if ( ( pObjToIndex = get_obj_index( pReset->arg3 ) ) == NULL
	    ||	 ( pObjToIndex->item_type != ITEM_CONTAINER && pObjToIndex->item_type != ITEM_SHEATH ) )
	    {
                sprintf( buf, "Put Object - Bad To Object %d\n\r",
                    pReset->arg3 );
                strcat( final, buf );
                break;
	    }

	    strip_color( oname, pObj->short_descr );
	    strip_color( mname, pObjToIndex->short_descr );
	    sprintf( buf,
		"O[%5d] %-13.13s inside              O[%5d] [%-2d  ] %-15.15s\n\r",
		pReset->arg1,
		oname,
		pReset->arg3,
		pReset->arg2,
		mname );
            strcat( final, buf );

	    break;

	case 'G':
	case 'E':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Give/Equip Object - Bad Object %d\n\r",
                    pReset->arg1 );
                strcat( final, buf );
                break;
	    }

            pObj       = pObjIndex;

	    if ( !pMob )
	    {
                sprintf( buf, "Give/Equip Object - No Previous Mobile\n\r" );
                strcat( final, buf );
                break;
	    }

	    strip_color( mname, pMob->short_descr );
	    strip_color( oname, pObj->short_descr );
	    if ( pReset->arg2 > 0 )
	    {
		sprintf( buf2, "[%4d]", pReset->arg2 );
	    }
	    else
	    {
		strcpy( buf2, "      " );
	    }
	    sprintf( buf,
		"O[%5d] %-13.13s %-19.19s %c[%5d] %s %-15.15s\n\r",
		pReset->arg1,
		oname,
		(pReset->command == 'G') ?
		    flag_string( wear_loc_strings, WEAR_NONE )
		  : flag_string( wear_loc_strings, pReset->arg3 ),
		  pReset->command == 'G' && pMob->pShop != NULL ? 'S' : 'M',
		  pMob->vnum,
		  buf2,
		  mname );
	    strcat( final, buf );

	    break;

	/*
	 * Doors are set in rs_flags don't need to be displayed.
	 * If you want to display them then uncomment the new_reset
	 * line in the case 'D' in load_resets in db.c and here.
	 */
#if 0
	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );
	    sprintf( buf, "R[%5d] %s door of %-19.19s reset to %s\n\r",
		pReset->arg1,
		capitalize( dir_name[ pReset->arg2 ] ),
		pRoomIndex->name,
		flag_string( door_resets, pReset->arg3 ) );
	    strcat( final, buf );

	    break;
#endif
	/*
	 * End Doors Comment.
	 */
	case 'R':
	    if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
		sprintf( buf, "Randomize Exits - Bad Room %d\n\r",
		    pReset->arg1 );
		strcat( final, buf );
		break;
	    }

	    sprintf( buf, "R[%5d] Exits are randomized in %s\n\r",
		pReset->arg1, pRoomIndex->name );
	    strcat( final, buf );

	    break;
	}
	send_to_char( final, ch );
    }

    return;
}


static bool
validate_reset( CHAR_DATA *ch, int num, char rtype )
{
    RESET_DATA *	reset;
    int			cnt;

    if ( ch->in_room->reset_first == NULL )
	return FALSE;

    num--;
    if ( num == 0 )
        return FALSE;

    cnt = 0;
    for ( reset = ch->in_room->reset_first; reset->next != NULL; reset = reset->next )
    {
        if ( reset->command == rtype )
            return TRUE;
        if ( ++cnt == num )
            break;
    }

    if ( reset->command == rtype )
        return TRUE;

    return FALSE;
}


/*****************************************************************************
 Name:		get_rprog_data
 Purpose:	gets a pointer to a room_prog
 Called by:	redit_rpedit(olc_room.c).
 ****************************************************************************/
static OREPROG_DATA *
get_rprog_data( ROOM_INDEX_DATA *pRoom, int vnum )
{
    OREPROG_DATA *pProg;
    int		  value;

    if ( pRoom == NULL )
	return NULL;

    value = 0;
    for ( pProg = pRoom->rprogs; pProg; pProg = pProg->next )
    {
	if ( value == vnum )
	    return pProg;
	value++;
    }

    return NULL;
}


/*****************************************************************************
 Name:		instaroom
 Purpose:	Creates resets for room based on current contents
 Called by:	TBD
 ****************************************************************************/
void
instaroom( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom )
{
    RESET_DATA *	pReset;
    RESET_DATA *	pContainerReset;
    CHAR_DATA *		pMob;
    CHAR_DATA *		pMob2;
    OBJ_DATA *		pObj;
    OBJ_DATA *		pObj2;
    OBJ_DATA *		inObj;
    AREA_DATA *		pOther;
    int			count;
    int			mobinroom;
    int			mobinworld;

    /* Delete current resets */
    while ( pRoom->reset_first != NULL )
    {
        pReset = pRoom->reset_first;
        pRoom->reset_first = pRoom->reset_first->next;
        free_reset_data( pReset );
    }
    pRoom->reset_last = NULL;
    SET_BIT( pRoom->area->area_flags, AREA_CHANGED );

    for ( pMob = pRoom->people; pMob != NULL; pMob = pMob->next_in_room )
    {
        if ( !IS_NPC( pMob ) || pMob->master != NULL )
            continue;

        /* See if we already have a reset for this mob */
        for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
            if ( pReset->command == 'M' && pReset->arg1 == pMob->pIndexData->vnum )
                break;
        if ( pReset != NULL )
            continue;

        /* Okay, we have a mob in the room that we don't already have a reset for */

	if ( ( pOther = pMob->pIndexData->area ) != pRoom->area
	&&   IS_SET( pOther->area_flags, AREA_PROTOTYPE )
	&&   !IS_BUILDER( ch, pOther ) )
	{
	    ch_printf( ch, "Instaroom: cannot load mob #%d from another area.\n\r",
	               pMob->pIndexData->vnum );
	    continue;
	}

        /* Count how many are in the world */
        mobinworld = 0;
        for ( pMob2 = char_list; pMob2 != NULL; pMob2 = pMob2->next )
            if ( pMob2->pIndexData == pMob->pIndexData )
                mobinworld++;

        /* Count how many are in the room */
        mobinroom = 0;
        for ( pMob2 = pRoom->people; pMob2 != NULL; pMob2 = pMob2->next_in_room )
            if ( pMob2->pIndexData == pMob->pIndexData )
                mobinroom++;

        /* Create the reset for the mob */
        pReset = new_reset_data( );
        pReset->command = 'M';
        pReset->arg1 = pMob->pIndexData->vnum;
        pReset->arg2 = mobinworld;
        pReset->arg3 = pRoom->vnum;
        pReset->arg4 = mobinroom;
        add_reset( pRoom, pReset, 0 );

        /* Add the mob's inventory/equipment */
        for ( pObj = pMob->carrying; pObj != NULL; pObj = pObj->next_content )
        {
	    if ( ( pOther = pObj->pIndexData->area ) != pRoom->area
	    &&   IS_SET( pOther->area_flags, AREA_PROTOTYPE )
	    &&   !IS_BUILDER( ch, pOther ) )
	    {
		ch_printf( ch, "Instaroom: cannot load object #%d from another area.\n\r",
			   pObj->pIndexData->vnum );
		continue;
	    }

            pReset = new_reset_data( );
            pReset->command = pObj->wear_loc == WEAR_NONE ? 'G' : 'E';
            pReset->arg1 = pObj->pIndexData->vnum;
            pReset->arg2 = -1;
            pReset->arg3 = pObj->wear_loc;
            pReset->arg4 = 0;
            add_reset( pRoom, pReset, 0 );
            for ( inObj = pObj->contains; inObj != NULL; inObj = inObj->next_content )
            {
                if ( ( pOther = inObj->pIndexData->area ) != pRoom->area
                &&   IS_SET( pOther->area_flags, AREA_PROTOTYPE )
                &&   !IS_BUILDER( ch, pOther ) )
                {
                    ch_printf( ch, "Instaroom: cannot load object #%d from another area.\n\r",
                               inObj->pIndexData->vnum );
                    continue;
                }

                pReset = new_reset_data( );
                pReset->command = 'P';
                pReset->arg1 = inObj->pIndexData->vnum;
                pReset->arg2 = 0;
                pReset->arg3 = pObj->pIndexData->vnum;
                pReset->arg4 = 99;
                add_reset( pRoom, pReset, 0 );
            }
        }
    }

    /* Now do the objects in the room */
    for ( pObj = pRoom->contents; pObj != NULL; pObj = pObj->next_content )
    {
	if ( ( pOther = pObj->pIndexData->area ) != pRoom->area
	&&   IS_SET( pOther->area_flags, AREA_PROTOTYPE )
	&&   !IS_BUILDER( ch, pOther ) )
	{
	    ch_printf( ch, "Instaroom: cannot load object #%d from another area.\n\r",
	               pObj->pIndexData->vnum );
	    continue;
	}

        pReset = new_reset_data( );
        pReset->command = 'O';
        pReset->arg1 = pObj->pIndexData->vnum;
        pReset->arg2 = -1;
        pReset->arg3 = pRoom->vnum;
        pReset->arg4 = 0; /* unused */
        add_reset( pRoom, pReset, 0 );

        for ( pObj2 = pObj->contains; pObj2 != NULL; pObj2 = pObj2->next_content )
        {
	    if ( ( pOther = pObj2->pIndexData->area ) != pRoom->area
	    &&   IS_SET( pOther->area_flags, AREA_PROTOTYPE )
	    &&   !IS_BUILDER( ch, pOther ) )
	    {
		ch_printf( ch, "Instaroom: cannot load object #%d from another area.\n\r",
			   pObj2->pIndexData->vnum );
		continue;
	    }

            count = 0;
            for ( pContainerReset = pRoom->reset_first; pContainerReset != NULL; pContainerReset = pContainerReset->next )
            {
		if ( pContainerReset->command == 'P'
		&&   pContainerReset->arg1 == pObj2->pIndexData->vnum
		&&   pContainerReset->arg3 == pObj->pIndexData->vnum )
		    count++;
            }
            if ( count > 0 )
                continue;
            pReset = new_reset_data( );
            pReset->command = 'P';
            pReset->arg1 = pObj2->pIndexData->vnum;
            pReset->arg2 = count_obj_list( pObj2->pIndexData, pObj->contains );
            pReset->arg3 = pObj->pIndexData->vnum;
            pReset->arg4 = 99;
            add_reset( pRoom, pReset, 0 );
        }
    }
    return;
}


/*****************************************************************************
 Name:		show_room_info
 Purpose:	Displays info on a room
 Called by:	redit_show(olc_room.c) do_rstat(act_wiz.c).
 ****************************************************************************/
void
show_room_info( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom, BUFFER *pBuf )
{
    char		buf[MAX_INPUT_LENGTH];
    int			door;
    EXTRA_DESCR_DATA *	ed;
    OREPROG_DATA *	prog;
    int			pcount;
    bool		fcnt;
    OBJ_DATA *		obj;
    EXIT_DATA *		pExit;
    CHAR_DATA *		rch;
    bool		mdesc;
    bool		edesc;
    bool		ndesc;

    buf_printf( pBuf, "`WDescription`w:\n\r%s`X", pRoom->description );

    if ( !IS_NULLSTR( pRoom->smell ) )
        buf_printf( pBuf, "`WSmell`w:\n\r%s`X", pRoom->smell );
    if ( !IS_NULLSTR( pRoom->sound ) )
        buf_printf( pBuf, "`WSound`w:\n\r%s`X", pRoom->sound );

    buf_printf( pBuf, "Name:       [%s`X]\n\r", pRoom->name );
    buf_printf( pBuf, "Area:       [%5d] %s\n\r",
	    pRoom->area->vnum, pRoom->area->name );
    buf_printf( pBuf, "Vnum:       [%5d]  ", pRoom->vnum );
    buf_printf( pBuf, "Heal rate: [%d%%]  ", pRoom->heal_rate );
    buf_printf( pBuf, "Mana rate: [%d%%]\n\r", pRoom->mana_rate );
    buf_printf( pBuf, "Sector:     [%s]\n\r",
	    flag_string( sector_types, pRoom->sector_type ) );
    buf_printf( pBuf, "Room flags: [%s]\n\r",
	    flag_string( room_flags, pRoom->room_flags ) );
    buf_printf( pBuf, "Affects:    [%s]\n\r",
	    flag_string( room_aff_flags, pRoom->affect_flags ) );

    mdesc = !IS_NULLSTR( pRoom->morning_desc );
    edesc = !IS_NULLSTR( pRoom->evening_desc );
    ndesc = !IS_NULLSTR( pRoom->night_desc );
    if ( mdesc || edesc || ndesc )
    {
        add_buf( pBuf, "Alt descs:  [" );
        if ( mdesc )
            add_buf( pBuf, "morning" );
        if ( edesc )
        {
            if ( mdesc )
                add_buf( pBuf, " " );
            add_buf( pBuf, "evening" );
        }
        if ( ndesc )
        {
            if ( mdesc || edesc )
                add_buf( pBuf, " " );
            add_buf( pBuf, "night" );
        }
        add_buf( pBuf, "]\n\r" );
    }

    if ( pRoom->vehicle_type != VEHICLE_NONE )
    {
	buf_printf( pBuf, "Vehicle:    [%s]\n\r",
		    flag_string( vehicle_types, pRoom->vehicle_type ) );
	buf_printf( pBuf, "Short desc: [%s]\n\r", pRoom->short_descr );
    }

    if ( pRoom->extra_descr )
    {
	add_buf( pBuf, "Desc Kwds:  [" );
	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    add_buf( pBuf, ed->keyword );
	    if ( ed->next )
		add_buf( pBuf, " " );
	}
	add_buf( pBuf, "]\n\r" );
    }

    add_buf( pBuf, "Rprogs:     [" );
    pcount = 0;
    for ( prog = pRoom->rprogs; prog != NULL; prog = prog->next )
        pcount++;
    if ( pcount != 0 )
        buf_printf( pBuf, "%d", pcount );
    else
        add_buf( pBuf, "(none)" );
    add_buf( pBuf, "]\n\r" );

    add_buf( pBuf, "Characters: [" );
    fcnt = FALSE;
    for ( rch = pRoom->people; rch; rch = rch->next_in_room )
    {
	if ( can_see( ch, rch ) )
	{
	    first_arg( rch->name, buf, FALSE );
	    add_buf( pBuf, buf );
	    if ( rch->next_in_room )
		add_buf( pBuf, " " );
	    fcnt = TRUE;
	}
    }
    if ( !fcnt )
	add_buf( pBuf, "(none)" );
    add_buf( pBuf, "]\n\r" );

    add_buf( pBuf, "Objects:    [" );
    fcnt = FALSE;
    for ( obj = pRoom->contents; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    one_argument( obj->name, buf );
	    add_buf( pBuf, buf );
	    if ( obj->next_content )
		add_buf( pBuf, " " );
	    fcnt = TRUE;
	}
    }

    if ( !fcnt )
	add_buf( pBuf, "(none)" );
    add_buf( pBuf, "]\n\r" );

    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( ( pExit = pRoom->exit[door] ) )
	{
	    char word[MAX_INPUT_LENGTH];
	    char reset_state[MAX_STRING_LENGTH];
	    char buf1[MAX_INPUT_LENGTH];
	    char *state;
	    int i, length;

	    buf_printf( pBuf, "-%-5s to [%5d] Key: [%5d]",
		capitalize(dir_name[door]),
		pExit->to_room ? pExit->to_room->vnum : 0,
		pExit->key );

	    /*
	     * Format up the exit info.
	     * Capitalize all flags that are not part of the reset info.
	     */
	    strcpy( reset_state, flag_string( exit_flags, pExit->rs_flags ) );
	    state = flag_string( exit_flags, pExit->exit_info );
	    strcpy( buf1, " Exit flags: [" );
	    for (; ;)
	    {
		state = one_argument( state, word );

		if ( word[0] == '\0' )
		{
		    int end;

		    end = strlen(buf1) - 1;
		    buf1[end] = ']';
//		    strcat( buf1, "\n\r" );
		    add_buf( pBuf, buf1 );
		    break;
		}

		if ( str_infix( word, reset_state ) )
		{
		    length = strlen(word);
		    for (i = 0; i < length; i++)
			word[i] = toupper(word[i]);
		}
		strcat( buf1, word );
		strcat( buf1, " " );
	    }

	    buf_printf( pBuf, " %s", flag_string( size_types, pExit->size ) );

            if ( pExit->eprogs != NULL )
            {
                pcount = 0;
                for ( prog = pExit->eprogs; prog != NULL; prog = prog->next )
                    pcount++;
                buf_printf( pBuf, " Eprogs: [%d]", pcount );
            }
            add_buf( pBuf, "\n\r" );

            if ( !IS_NULLSTR( pExit->sound_closed ) )
                buf_printf( pBuf, "Sound (closed):\n\r%s`X", pExit->sound_closed );
            if ( !IS_NULLSTR( pExit->sound_open ) )
                buf_printf( pBuf, "Sound (open):\n\r%s`X", pExit->sound_open );

	    if ( pExit->keyword && pExit->keyword[0] != '\0' )
	    {
		buf_printf( pBuf, "Kwds: [%s]\n\r", pExit->keyword );
	    }
	    if ( pExit->description && pExit->description[0] != '\0' )
	    {
		buf_printf( pBuf, "%s`X", pExit->description );
	    }
//	    else
//	    {
//		add_buf( pBuf, "\n\r" );
//	    }
	}
    }

}


/*****************************************************************************
 Name:		wear_bit
 Purpose:	Converts a wear_loc into a bit.
 Called by:	redit_oreset(olc_room.c).
 ****************************************************************************/
static bitvector
wear_bit( int loc )
{
    int flag;

    for ( flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++ )
    {
        if ( loc == wear_table[flag].wear_loc )
            return wear_table[flag].wear_bit;
    }

    return 0;
}

