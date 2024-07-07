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
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include  <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "db.h"
#include "interp.h"
#include "lookup.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"

/*
 * Local structure definition
 */
struct mod_data
{
    struct mod_data *	next;
    AREA_DATA *		pArea;
    time_t		mtime;
};

/*
 * Local functions.
 */
static int	you_count	args( ( char *str ) );


void
do_aexits( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    AREA_DATA *		fArea;
    ROOM_INDEX_DATA *	pRoom;
    OBJ_INDEX_DATA *	pObj;
    BUFFER *		pBuf;
    int			nArea;
    int			iArea;
    int			eArea;
    int			iExit;
    int			lvnum;
    int			uvnum;
    int			col;
    int			obj_vnum;
    bool		found;

    if ( IS_NPC( ch ) )
	return;

    if ( argument[ 0 ] == '\0' )
    {
	pArea = ch->in_room->area;
    }
    else
    {
	if ( is_number( argument ) )
	{
	    nArea = atoi( argument );
	    if ( nArea < top_area )
	        pArea = get_area_data( nArea );
	    else
            {
                for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
                    if ( pArea->min_vnum <= nArea && pArea->max_vnum >= nArea )
                        break;
            } 
	    if ( pArea == NULL )
	    {
		send_to_char( "No area has that vnum.\n\r", ch );
		return;
	    }
	}
	else
	{
	    send_to_char( "That's not an area number!\n\r", ch );
	    return;
	}
    }

    nArea = pArea->vnum;

    lvnum = pArea->min_vnum;
    uvnum = pArea->max_vnum;
    found = FALSE;
    pBuf = new_buf( );

    buf_printf( pBuf, "Links to %s (%d-%d):\n\r", pArea->name, lvnum, uvnum );

    col = 0;
    for ( fArea = area_first; fArea != NULL; fArea = fArea->next )
    {
        iArea = fArea->vnum;
        for ( pRoom = fArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
        {
            for ( iExit = 0; iExit < MAX_DIR; iExit++ )
            {
                if ( pRoom->exit[iExit] == NULL
                ||   pRoom->exit[iExit]->vnum == 0
                ||   pRoom->exit[iExit]->to_room == NULL
                ||   pRoom->exit[iExit]->to_room->area == NULL )
                    continue;
                eArea = pRoom->exit[iExit]->to_room->area->vnum;
                if ( iArea != nArea && eArea != nArea )
                    continue;
                if ( iArea == nArea && eArea == nArea )
                    continue;
                if ( pArea->overland != NULL )
                {
                    buf_printf( pBuf, " %6d->%-6d",
                                pRoom->vnum, pRoom->exit[iExit]->vnum );
                    if ( ++col % 5 == 0 )
                        add_buf( pBuf, "\n\r" );
                }
                else
                {
                    buf_printf( pBuf, "%6d --> %s to %d (%s)\n\r",
                        pRoom->vnum,
                        dir_name[ iExit ],
                        pRoom->exit[iExit]->vnum,
                        iArea == nArea
                            ? pRoom->exit[iExit]->to_room->area->name
                            : pRoom->area->name
                        );
                }
                found = TRUE;
            }
        }
    }
    if ( col % 5 != 0 )
        add_buf( pBuf, "\n\r" );

    for ( obj_vnum = 1; obj_vnum <= top_vnum_obj; obj_vnum++ )
    {
	if ( ( pObj = get_obj_index( obj_vnum ) ) == NULL )
	    continue;
	if ( pObj->item_type != ITEM_PORTAL )
	    continue;
	if ( ( obj_vnum < lvnum || obj_vnum > uvnum ) &&
	     ( pObj->value[3] < lvnum || pObj->value[3] > uvnum ) )
	    continue;
	if ( ( obj_vnum >= lvnum && obj_vnum <= uvnum ) &&
	     ( pObj->value[3] >= lvnum && pObj->value[3] <= uvnum ) )
	    continue;
	buf_printf( pBuf, " %5d portal to %d\n\r", obj_vnum, pObj->value[3] );
	found = TRUE;
    }

    if ( found )
	page_to_char( buf_string( pBuf ), ch );
    else
	send_to_char( "This area is not linked.\n\r", ch );
    free_buf( pBuf );
    return;
}


void
do_afind( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    BUFFER *	pBuf;
    int		kingdom;
    bool	found = FALSE;

    if ( *argument == '\0' )
    {
	send_to_char( "Afind what?\n\r", ch );
	return;
    }

    kingdom = flag_value( kingdom_types, argument );

    pBuf = new_buf( );
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( is_name( argument, pArea->name )
	||   is_name( argument, pArea->file_name )
	||   pArea->kingdom == kingdom
	||   is_name( argument, pArea->builders ) )
	{
	    buf_printf( pBuf, "[%3d]%c%-28s (%5d-%-5d) %-12.12s %2d  %-13.13s\n\r",
		 pArea->vnum,
		 IS_SET( pArea->area_flags, AREA_PROTOTYPE ) ? '*' : ' ',
		 pArea->name,
		 pArea->min_vnum,
		 pArea->max_vnum,
		 pArea->file_name,
		 pArea->security,
		 pArea->builders
		);
	    found = TRUE;
	}
    }

    if ( found )
	page_to_char( buf_string( pBuf ), ch );
    else
	send_to_char( "Area not found.\n\r", ch );
    free_buf( pBuf );

    return;
}


void
do_ainfo( CHAR_DATA *ch, char *argument )
{
    char		changed[SHORT_STRING_LENGTH];
    AFFECT_DATA *	paf;
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    EXIT_DATA *		pExit;
    EXTRA_DESCR_DATA *	pExtra;
    MOB_INDEX_DATA *	pMob;
    MPROG_DATA *	pMprog;
    OBJ_INDEX_DATA *	pObj;
    OREPROG_DATA *	pProg;
    RESET_DATA *	pReset;
    ROOM_INDEX_DATA *	pRoom;
    int			door;
    int			vnum;
    char *		p;
    int			affects;
    int			count;
    int			descs;
    int			eprogs;
    int			exits;
    int			exit_descs;
    int			exit_sounds;
    int			extras;
    int			invokes;
    int			joins;
    int			progs;
    int			resets;
    int			room_smells;
    int			room_sounds;
    int			shops;
    int			specs;
    int			mdescs;
    int			edescs;
    int			ndescs;

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
    buf_printf( pBuf, "[%d] %s`X, %d vnums (%d-%d), %s\n\r",
		pArea->vnum,
		pArea->name,
		pArea->max_vnum - pArea->min_vnum + 1,
		pArea->min_vnum,
		pArea->max_vnum,
		pArea->builders );
    if ( pArea->created != 0 )
        buf_printf( pBuf, "  Created: %s\n\r", date_string( pArea->created ) );
    if ( IS_SET( pArea->area_flags, AREA_CHANGED ) )
	strcpy( changed, ctime( &current_time ) );
    else if ( pArea->last_changed )
	strcpy( changed, ctime( &pArea->last_changed ) );
    else
	changed[0] = '\0';
    if ( ( p = strchr( changed, '\n' ) ) != NULL )
	*p = '\0';
    if ( changed[0] != '\0' )
	buf_printf( pBuf, "  Last changed %s\n\r", changed );

    /* Get stats on overlands. */
    if ( pArea->overland != NULL )
    {
        buf_printf( pBuf, "Overland: %dx%d, %d rooms.\n\r",
                    pArea->overland->x, pArea->overland->y,
                    pArea->overland->x * pArea->overland->y );
    }

    /* Get stats on rooms. */
    count	= 0;
    descs	= 0;
    eprogs	= 0;
    exits	= 0;
    exit_descs	= 0;
    extras	= 0;
    progs	= 0;
    resets	= 0;
    mdescs	= 0;
    edescs	= 0;
    ndescs	= 0;
    room_sounds = 0;
    room_smells = 0;
    exit_sounds = 0;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pRoom = get_room_index( vnum ) ) != NULL )
	{
	    count++;
	    for ( p = pRoom->description; *p != '\0'; p++ )
		if ( *p == '\n' )
		    descs++;
	    for ( p = pRoom->morning_desc; *p != '\0'; p++ )
		if ( *p == '\n' )
		    descs++;
	    for ( p = pRoom->evening_desc; *p != '\0'; p++ )
		if ( *p == '\n' )
		    descs++;
	    for ( p = pRoom->night_desc; *p != '\0'; p++ )
		if ( *p == '\n' )
		    descs++;

	    if ( !IS_NULLSTR( pRoom->sound ) )
		room_sounds++;
	    if ( !IS_NULLSTR( pRoom->smell ) )
		room_smells++;
	    for ( door = 0; door < MAX_DIR; door++ )
		if ( ( pExit = pRoom->exit[door] ) != NULL
		&&   pExit->to_room != NULL )
		{
		    exits++;
		    if ( !IS_NULLSTR( pExit->sound_closed ) )
			exit_sounds++;
		    if ( !IS_NULLSTR( pExit->sound_open ) )
			exit_sounds++;
		    if ( !IS_NULLSTR( pExit->description ) )
			exit_descs++;
                    for ( pProg = pExit->eprogs; pProg != NULL; pProg = pProg->next )
                        eprogs++;
		}

	    for ( pExtra = pRoom->extra_descr; pExtra != NULL; pExtra = pExtra->next )
		extras++;

	    for ( pProg = pRoom->rprogs; pProg != NULL; pProg = pProg->next )
		progs++;

	    for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
		resets++;
            if ( !IS_NULLSTR( pRoom->morning_desc ) )
                mdescs++;
            if ( !IS_NULLSTR( pRoom->evening_desc ) )
                edescs++;
            if ( !IS_NULLSTR( pRoom->night_desc ) )
                ndescs++;
	}
    }

    buf_printf( pBuf, "Rooms: %d\n\r", count );
    if ( count != 0 )
    {
	buf_printf( pBuf, "  Descriptions: %d lines.\n\r", descs );
	if ( mdescs != 0 )
	    buf_printf( pBuf, "    Mdescs: %d\n\r", mdescs );
	if ( edescs != 0 )
	    buf_printf( pBuf, "    Edescs: %d\n\r", edescs );
	if ( ndescs != 0 )
	    buf_printf( pBuf, "    Ndescs: %d\n\r", ndescs );
	if ( room_sounds != 0 )
	    buf_printf( pBuf, "    Sounds: %d\n\r", room_sounds );
	if ( room_smells != 0 )
	    buf_printf( pBuf, "    Smells: %d\n\r", room_smells );
	buf_printf( pBuf, "  Exits: %d\n\r", exits );
	if ( exits != 0 )
	{
	    if ( exit_sounds != 0 )
		buf_printf( pBuf, "    Exit sounds: %d\n\r", exit_sounds );
	    buf_printf( pBuf, "    Exit descs: %d\n\r", exit_descs );
	    buf_printf( pBuf, "    Eprogs: %d\n\r", eprogs );
	}
	buf_printf( pBuf, "  Extra descs: %d\n\r", extras );
	buf_printf( pBuf, "  Rprogs: %d\n\r", progs );
	buf_printf( pBuf, "  Resets: %d\n\r", resets );
    }

    /* Get stats on mobiles. */
    count	= 0;
    descs	= 0;
    progs	= 0;
    shops	= 0;
    specs	= 0;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pMob = get_mob_index( vnum ) ) != NULL )
	{
	    count++;
	    if ( !IS_NULLSTR( pMob->description ) )
		descs++;
	    for ( pMprog = pMob->mobprogs; pMprog != NULL; pMprog = pMprog->next )
		progs++;
	    if ( pMob->pShop != NULL )
		shops++;
	    if ( pMob->spec_fun != NULL )
		specs++;
	}
    }

    buf_printf( pBuf, "Mobiles: %d\n\r", count );
    if ( count != 0 )
    {
	buf_printf( pBuf, "  Descriptions: %d\n\r", descs );
	buf_printf( pBuf, "  Mprogs: %d\n\r", progs );
	buf_printf( pBuf, "  Shops: %d\n\r", shops );
	buf_printf( pBuf, "  Spec_procs: %d\n\r", specs );
    }

    /* Get stats on objects. */
    affects	= 0;
    count	= 0;
    extras	= 0;
    invokes	= 0;
    joins	= 0;
    progs	= 0;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pObj = get_obj_index( vnum ) ) != NULL )
	{
	    count++;
	    for ( paf = pObj->affected; paf != NULL; paf = paf->next )
		affects++;
	    for ( pExtra = pObj->extra_descr; pExtra != NULL; pExtra = pExtra->next )
		extras++;
	    if ( pObj->ac_type != INVOKE_NONE )
		invokes++;
	    if ( pObj->join != 0 || ( pObj->sep_one != 0 && pObj->sep_two != 0 ) )
		joins++;
	    for ( pProg = pObj->oprogs; pProg != NULL; pProg = pProg->next )
		progs++;
	}
    }

    buf_printf( pBuf, "Objects: %d\n\r", count );
    if ( count != 0 )
    {
	buf_printf( pBuf, "  Affects: %d\n\r", affects );
	buf_printf( pBuf, "  Extra descs: %d\n\r", extras );
	buf_printf( pBuf, "  Invokes: %d\n\r", invokes );
	buf_printf( pBuf, "  Joins: %d\n\r", joins );
	buf_printf( pBuf, "  Oprogs: %d\n\r", progs );  
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_alist( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	arg[MAX_INPUT_LENGTH];
    BUFFER *	pBuf;
    int		lvnum;
    int		uvnum;
    int		ncount;
    int		val;
    bool	found;
    bool	fChangedOnly;
    bool	fKingdom;
    bool	fProtoOnly;

    /* Set defaults */
    fChangedOnly = FALSE;
    fKingdom = FALSE;
    fProtoOnly = FALSE;
    ncount = 0;
    lvnum = 0;
    uvnum = MAX_VNUM;

    argument = one_argument( argument, arg );
    while ( arg[0] != '\0' )
    {
	if ( !str_prefix( arg, "changed" ) )
	    fChangedOnly = TRUE;
	else if ( !str_prefix( arg, "prototype" ) )
	    fProtoOnly = TRUE;
	else if ( !str_prefix( arg, "kingdom" ) )
	    fKingdom = TRUE;
	else if ( is_number( arg ) || !str_cmp( arg, "." ) )
	{
	    val = is_number( arg ) ? atoi( arg ) : ch->in_room->vnum;
	    switch( ++ncount )
	    {
		case 1: lvnum = val; break;
		case 2: uvnum = val; break;
		default:
		    send_to_char( "Too many vnums.\n\r", ch );
		    return;
	    }
	} 
	else
	{
	    /* could do area lookups by name here */
	}
	argument = one_argument( argument, arg );
    }

    if ( ncount == 1 )
	uvnum = lvnum;

    pBuf = new_buf( );
    found = FALSE;

    buf_printf( pBuf, "[%3s] %-28s (%-5s-%5s) %-11s %3s %-13s\n\r",
       "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( pArea->min_vnum > uvnum
	||   pArea->max_vnum< lvnum
	||   ( fProtoOnly && !IS_SET( pArea->area_flags, AREA_PROTOTYPE ) )
	||   ( fChangedOnly && !IS_SET( pArea->area_flags, AREA_CHANGED ) ) )
	    continue;
	found = TRUE;

	val = strlen( pArea->name ) - strlen_wo_col( pArea->name ) + 28;
	buf_printf( pBuf, "[%3d]%c%-*.*s`X %6d-%-6d %-12.12s %2d %-13.13s\n\r",
		 pArea->vnum,
		 IS_SET( pArea->area_flags, AREA_PROTOTYPE ) ? '*' : ' ',
		 val,
		 val,
		 pArea->name,
		 pArea->min_vnum,
		 pArea->max_vnum,
		 pArea->file_name,
		 pArea->security,
		 fKingdom ? flag_string( kingdom_types, pArea->kingdom ) : pArea->builders
		);
    }

    if ( found )
	page_to_char( buf_string( pBuf ), ch );
    else
	ch_printf( ch, "No %s%sareas found.\n\r",
		   fChangedOnly	? "changed "	: "",
		   fProtoOnly	? "prototype " : "" );

    free_buf( pBuf );
}


void
do_astat( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    BUFFER *		pBuf;
    int			vnum;

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

    if ( pArea )
    {
	pBuf = new_buf( );
	show_area_info( pArea, pBuf );
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
    }
    else
    {
	send_to_char( "Area not found.\n\r", ch );
    }

    return;
}


void
do_astrip( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *  vch;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;
    char	 arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( ( vch = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    for ( paf = vch->affected; paf; paf = paf_next )
    {
	paf_next = paf->next;
	affect_remove( vch, paf );
    }
    xCLEAR_BITS( vch->affected_by );

    if ( vch != ch )
	send_to_char( "All your affects have been removed.\n\r", vch );

    send_to_char( "Ok.\n\r", ch );
    return;
}


void
do_avatar( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		i;
    int		level;
    int		perm_hit;
    int		perm_mana;
    int		perm_move;
    int		practice;

    argument = one_argument( argument, arg );

    if ( IS_NPC( ch ) )
    {
        send_to_char( "If you want to change your level, see your builder.\n\r", ch );
        return;
    }

    if ( ch->level < LEVEL_IMMORTAL && ch->pcdata->avatar == NULL )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( arg[0] == 'r' && !str_prefix( arg, "return" ) )
    {
        if ( ch->pcdata->avatar == NULL )
        {
            send_to_char( "You are not an avatar.\n\r", ch );
            return;
        }

        ch->level	= ch->pcdata->avatar->level;
        ch->trust	= ch->pcdata->avatar->trust;
        ch->hit		= ch->pcdata->avatar->hit;
        ch->max_hit	= ch->pcdata->avatar->max_hit;
        ch->mana	= ch->pcdata->avatar->mana;
        ch->max_mana	= ch->pcdata->avatar->max_mana;
        ch->move	= ch->pcdata->avatar->move;
        ch->max_move	= ch->pcdata->avatar->max_move;
        SET_BIT( ch->act, ch->pcdata->avatar->holylight );

        free_avatar_data( ch->pcdata->avatar );
        ch->pcdata->avatar = NULL;

        send_to_char( "You return to godhood.\n\r", ch );
        return;
    }

    if ( !is_number( arg ) )
    {
        send_to_char( "Syntax: avatar <mortal level>\n\r", ch );
        send_to_char( "        avatar return\n\r", ch );
        return;
    }

    level = atoi( arg );
    if ( level < 1 || level > MAX_MORTAL )
    {
        ch_printf( ch, "Level must be 1 to %d.\n\r", MAX_MORTAL );
        send_to_char( "To return to godhood, use \"avatar return\".\n\r", ch );
        return;
    }

    perm_hit  = ch->pcdata->perm_hit;
    perm_mana = ch->pcdata->perm_mana;
    perm_move = ch->pcdata->perm_move;
    practice  = ch->practice;

    if ( ch->pcdata->avatar == NULL )
    {
        ch->pcdata->avatar = new_avatar_data( );
        ch->pcdata->avatar->level	= ch->level;
        ch->pcdata->avatar->trust	= ch->trust;
        ch->pcdata->avatar->hit		= ch->hit;
        ch->pcdata->avatar->max_hit	= ch->max_hit;
        ch->pcdata->avatar->mana	= ch->mana;
        ch->pcdata->avatar->max_mana	= ch->max_mana;
        ch->pcdata->avatar->move	= ch->move;
        ch->pcdata->avatar->max_move	= ch->max_move;
        ch->pcdata->avatar->holylight	= ch->act & PLR_HOLYLIGHT;
    }

    ch->level	 = 1;
    ch->exp	 = exp_per_level( ch, ch->pcdata->points );
    ch->max_hit	 = 10;
    ch->max_mana = 100;
    ch->max_move = 100;
    ch->practice = 0;
    advance_level( ch, TRUE );

    for ( i = ch->level; i < level; i++ )
    {
        ch->level++;
        advance_level( ch, TRUE );
    }

    ch->hit 		  = ch->max_hit;
    ch->mana		  = ch->max_mana;
    ch->move		  = ch->max_move;
    ch->pcdata->perm_hit  = perm_hit;
    ch->pcdata->perm_mana = perm_mana;
    ch->pcdata->perm_move = perm_move;
    ch->practice	  = practice;
    if ( !IS_IMMORTAL ( ch ) )
        REMOVE_BIT( ch->act, PLR_HOLYLIGHT );

    ch_printf( ch, "You are now level %d.\n\r", ch->level );

    return;
}


void
do_award( CHAR_DATA *ch, char *argument )
{
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		arg3[MAX_INPUT_LENGTH];
    int			award;
    char		buf[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *	pObjIndex;
    OBJ_DATA *		pObj;
    ROOM_INDEX_DATA *	oldroom;
    CHAR_DATA *		vch;
    CLAN_DATA *		pClan;
    bool		fDeduct;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg3[0] == '\0' )
    {
	send_to_char( "Syntax: AWARD player amount award\n\r", ch );
	send_to_char( "or:     AWARD player vnum obj\n\r", ch );
	send_to_char( "or:     AWARD clan amount honor\n\r\n\r", ch );
	send_to_char( "award being one of:\n\r", ch );
	ch_printf( ch, "  xp qp %s %s %s %s\n\r",
                   GOLD_PLURAL, SILVER_PLURAL, COPPER_PLURAL, FRACT_PLURAL );
	send_to_char( "amount being the size of the award.\n\r\n\r", ch );
	return;
    }

    if ( !str_prefix( arg3, "honor" ) )
    {
	if ( ( pClan = get_clan( arg1 ) ) == NULL )
	{
	    send_to_char( "There is no clan by that name.\n\r", ch );
	    return;
	}
	if ( !is_number( arg2 ) )
	{
	    do_award( ch, "" );
	    return;
	}
	award = atoi( arg2 );
	if ( award > 1000 )
	{
	    send_to_char( "You can't award more than 1000 honor points.\n\r", ch );
	    return;
	}
	fDeduct = ( award < 0 );
	if ( fDeduct )
	    award = UMAX( award, -pClan->honor );

	pClan->honor += award;
	save_clans( );
	if ( fDeduct )
	    award = -award;
	sprintf( buf, "%d honor point%s", award, award == 1 ? "" : "s" );

	if ( !fDeduct )
	{
	    act_color( AT_IMM, "You award $t to $T.", ch, buf, pClan->who_name, TO_CHAR, POS_DEAD );
	    act_color( AT_IMM, "$n awards $t to $T.", ch, buf, pClan->who_name, TO_ROOM, POS_DEAD );
	}
	else
	{
	    act_color( AT_IMM, "You dock $T for $t.", ch, buf, pClan->who_name,
		       TO_CHAR, POS_DEAD );
	}
	return;
    }

    if ( ( vch = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( vch ) )
    {
	send_to_char( "You can't award mobiles.\n\r", ch );
	return;
    }

    if ( !is_number( arg2 ) )
    {
	do_award( ch, "" );
	return;
    }
    award = atoi( arg2 );

    if ( !str_cmp( arg3, "xp" ) )
    {
	if ( award > 100 )
	{
	    send_to_char( "You can't award more than 100 xp.\n\r", ch );
	    return;
	}
	if ( award < 0 )
	    award = UMAX( award, -ch->exp );
	sprintf( buf, "%d experience point%s", award, award == 1 ? "" : "s" );
    }

    else if ( !str_cmp( arg3, "qp" ) )
    {
	if ( award > 2500 )
	{
	    send_to_char( "You can't award more than 2500 qp.\n\r", ch );
	    return;
	}
	if ( award < 0 )
	    award = UMAX( award, -vch->questpoints );
	sprintf( buf, "%d quest point%s", award, award == 1 ? "" : "s" );
	vch->questpoints += award;
	quest_log( "%s awarded %d qp, total %d.", vch->name, award, vch->questpoints );
    }

    else if ( !str_cmp( arg3, GOLD_NOUN ) || !str_cmp( GOLD_PLURAL, arg3 ) )
    {
	if ( award > 1000 )
	{
	    ch_printf( ch, "You can't award more than 1,000 %s.\n\r", GOLD_PLURAL );
	    return;
	}
	if ( award < 0 )
	    award = UMAX( award, -ch->money.gold );
	vch->money.gold += award;
	sprintf( buf, "%d %s", award, award == 1 ? GOLD_NOUN : GOLD_PLURAL );
    }

    else if ( !str_cmp( arg3, SILVER_NOUN ) || !str_cmp( SILVER_PLURAL, arg3 ) )
    {
	if ( award > 100 )
	{
	    ch_printf( ch, "You can't award more than 100 %s.\n\r", SILVER_PLURAL );
	    return;
	}
	if ( award < 0 )
	    award = UMAX( award, -ch->money.silver );
	vch->money.silver += award;
	sprintf( buf, "%d %s", award, award == 1 ? SILVER_NOUN : SILVER_PLURAL );
    }

    else if ( !str_cmp( arg3, COPPER_NOUN ) || !str_cmp( COPPER_PLURAL, arg3 ) )
    {
	if ( award > 100 )
	{
	    ch_printf( ch, "You can't award more than 100 %s.\n\r", COPPER_PLURAL );
	    return;
	}
	if ( award < 0 )
	    award = UMAX( award, -ch->money.copper );
	vch->money.copper += award;
	sprintf( buf, "%d %s", award, award == 1 ? COPPER_NOUN : COPPER_PLURAL );
    }

    else if ( !str_cmp( arg3, FRACT_NOUN ) || !str_cmp( FRACT_PLURAL, arg3 ) )
    {
	if ( award > 10 )
	{
	    ch_printf( ch, "You can't award more than 10 %s.\n\r", FRACT_PLURAL );
	    return;
	}
	if ( award < 0 )
	    award = UMAX( award, -ch->money.fract );
	vch->money.fract += award;
	sprintf( buf, "%d %s", award, award == 1 ? FRACT_NOUN : FRACT_PLURAL );
    }

    else if ( !str_cmp( arg3, "obj" ) )
    {
	if ( ( pObjIndex = get_obj_index( award ) ) == NULL )
	{
	    send_to_char( "No such object.\n\r", ch );
	    return;
	}
	pObj = create_object( pObjIndex, vch->level );
	obj_to_char( pObj, vch );
	strcpy( buf, pObj->short_descr );
    }
    else
    {
	do_award( ch, "" );
	return;
    }

    if ( ch->in_room != vch->in_room )
    {
	oldroom = ch->in_room;
	char_from_room( ch );
	char_to_room( ch, vch->in_room );
    }
    else
	oldroom = NULL;

    if ( award >= 0 )
    {
	act_color( AT_IMM, "You award $t `Xto $N.", ch, buf, vch, TO_CHAR, POS_DEAD );
	act_color( AT_IMM, "$n awards $t `Xto you.", ch, buf, vch, TO_VICT, POS_RESTING );
	act_color( AT_IMM, "$n awards $t `Xto $N.", ch, buf, vch, TO_NOTVICT, POS_RESTING );
    }
    else
    {
	act_color( AT_IMM, "You dock $N for $t.", ch, buf, vch, TO_CHAR, POS_DEAD );
    }

    if ( oldroom != NULL )
    {
	char_from_room( ch );
	char_to_room( ch, oldroom );
    }

    if ( !str_cmp( arg3, "xp" ) )
    {
	gain_exp( vch, award );
    }

    return;
}


void
do_badname( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH*2];
    char word[MAX_INPUT_LENGTH];
    FILE *fp;
    char *list;
    char *p;

    smash_tilde( argument );
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* show list */
	if ( IS_NULLSTR( badname_list ) )
	{
	    send_to_char( "The badname list is empty.\n\r", ch );
	    return;
	}

	list = badname_list;
	buf[0] = ' ';
	buf[1] = '\0';
	p = buf;
	while ( *list != '\0' )
	{
	    list = one_argument( list, arg );
	    if ( p - buf + strlen( arg ) > 78 )
	    {
		ch_printf( ch, "%s\n\r", &buf[1] );
		p = buf;
	    }
	    *p++ = ' ';
	    p = stpcpy( p, arg );
	}
	ch_printf( ch, "%s\n\r", &buf[1] );
	return;
    }

    if ( is_exact_name( arg, badname_list ) )
    {
	/* drop from list */
	list = badname_list;
	strcpy( buf, " " );
	p = buf;
	while ( *list != '\0' )
	{
	    list = one_argument( list, word );
	    if ( !str_cmp( word, arg ) )
		continue;
	    p += sprintf( p, " %s", word );
	}
	ch_printf( ch, "'%s' removed from badname list.\n\r", arg );
    }
    else
    {
	/* add to list */
	if ( strlen( badname_list ) + strlen( arg ) > sizeof( buf ) - 2 )
	{
	    send_to_char( "Badname list too long.\n\r", ch );
	    return;
	}

	if ( strlen( arg ) > 12 )	/* max username length */
	{
	    send_to_char( "New word too long.\n\r", ch );
	    return;
	}
	sprintf( buf, " %s %s", badname_list, arg );
	ch_printf( ch, "'%s' added to badname list.\n\r", arg );
    }

    free_string( badname_list );
    badname_list = str_dup( &buf[1] );

    if ( ( fp = fopen( TEMP_DIR BADNAME_FILE, "w" ) ) == NULL )
    {
	perror( BADNAME_FILE );
	send_to_char( "Error&w writing badname file.\n\r", ch );
	return;
    }

    /* Write it a word at a time, for readability. */
    list = &buf[1];
    while ( *list != '\0' )
    {
	list = one_argument( list, word );
	fprintf( fp, "%s\n", word );
    }

    fprintf( fp, "~\n" );
    fclose( fp );

    rename( TEMP_DIR BADNAME_FILE, SYSTEM_DIR BADNAME_FILE );
    return;
}


void
do_beacon( CHAR_DATA *ch, char *argument )
{
    char		buf[SHORT_STRING_LENGTH];
    ROOM_INDEX_DATA *	pRoom;
    int			vnum;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs can't beacon.\n\r", ch );
	return;
    }

    if ( *argument == '\0' )
    {
	if ( ch->pcdata->beacon == 0 )
	{
	    send_to_char( "Your beacon is not set.\n\r", ch );
	    return;
	}
	if ( ( pRoom = get_room_index( ch->pcdata->beacon ) ) == NULL )
	{
	    send_to_char( "Your beacon room is invalid.\n\r", ch );
	    return;
	}

	/* cheap hack for now, might want to do special beacon msgs later */
	sprintf( buf, "%d", ch->pcdata->beacon );
	do_goto( ch, buf );

	return;
    }

    if ( is_number( argument ) )
    {
	vnum = atoi( argument );
	if ( vnum < 0 )
	{
	    send_to_char( "Invalid vnum.\n\r", ch );
	    return;
	}
    }
    else if ( !str_cmp( argument, "." ) )
    {
	vnum = ch->in_room->vnum;
    }
    else
    {
	send_to_char ( "Set your beacon to where?\n\r", ch );
	return;
    }

    if ( vnum != 0 && get_room_index( vnum ) == NULL )
    {
	send_to_char( "That room does not exist.\n\r", ch );
	return;
    }

    ch_printf( ch, "Beacon %s.\n\r", vnum == 0 ? "cleared" : "set" );
    ch->pcdata->beacon = vnum;
}


void
do_blist( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *pBoard;
    BUFFER *	pBuf;
    bool	found;
    char	buf[SHORT_STRING_LENGTH];

    pBoard = next_board( ch, NULL );
    found = FALSE;

    pBuf = new_buf( );

    while ( pBoard != NULL )
    {
	if ( !found )
	{
	    add_buf( pBuf, "Board   Read Post Expire Description\n\r" );
	    found = TRUE;
	}

	if ( pBoard->expire == 0 )
	    strcpy( buf, "Never" );
	else
	    sprintf( buf, "%d", pBoard->expire );

	buf_printf( pBuf, "`R%-8s `G%3d  %3d %5s  `Y%s\n\r",
		    pBoard->name,
		    pBoard->read,
		    pBoard->write,
		    buf,
		    pBoard->desc );

	pBoard = next_board( ch, pBoard );
    }

    if ( !found )
	add_buf( pBuf, "No boards found.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_bodybag( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	obj;
    OBJ_DATA *	obj_next;
    OBJ_DATA *	body = NULL;
    OBJ_DATA *	in;
    OBJ_DATA *	in_next;
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    char	buf1[MAX_INPUT_LENGTH];
    char	name[MAX_INPUT_LENGTH];
    char *	p;
    int		howmany = 0;
    int		found = FALSE;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char ( "Bodybag whom?\r\n", ch );
	return;
    }

    if ( !str_cmp( arg, "corpse" ) )
    {
        send_to_char( "Whose corpse are you looking for?\n\r", ch );
        return;
    }

    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next;
	if ( obj->deleted )
	    continue;

	if ( obj->item_type != ITEM_CORPSE_PC )
	    continue;
	if ( obj->in_room == NULL )
	    continue;
	if ( !is_exact_name( "corpse", obj->name ) )
	    continue;
	if ( !is_name( arg, obj->name ) )
	    continue;

	found = TRUE;

        p = obj->name;
        do
        {
            p = first_arg( p, name, FALSE );
            if ( !str_prefix( arg, name ) )
                break;
        }
        while ( *p != '\0' );
        if ( name[0] == '0' || !str_cmp( name, "corpse" ) )
            continue;

	for ( in = obj->contains; in; in = in_next )
	{
	    in_next = in->next_content;

	    if ( howmany == 0 )
	    {
		sprintf ( buf1, "You get the bodybag of %s\r\n", name );
		body = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0 );
		sprintf( buf, "bodybag bag %s", name );
		free_string( body->name );
		body->name = str_dup( buf );
		sprintf( buf, "bodybag of %s", name );
		free_string( body->short_descr );
		body->short_descr = str_dup( buf );
		sprintf( buf, "The bodybag of %s lies here.", name );
		free_string( body->description );
		body->description = str_dup( buf );
		obj_to_char( body, ch );
	    }
	    obj_from_obj ( in );
	    obj_to_obj ( in, body );
	    howmany++;
	}

	if ( howmany )
	{
	    extract_obj ( obj );
	    body->timer = number_range( 35, 45 );
	}
    }

    if ( found == FALSE )
	send_to_char ( "You didn't find their corpse.\r\n", ch );
    else if ( howmany == 0 )
	send_to_char ( "The corpse was empty.\r\n", ch );
    else
	send_to_char ( buf1, ch );
    return;
}


void
do_cmdstat( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    bitvector	bits;
    BUFFER *	buf;
    DO_FUN *	do_fun;
    bool	found;
    int		i;
    int		iCmd;
    time_t	last_time;
    int		last_hr;
    int		last_min;
    int		last_sec;

    buf = new_buf( );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Show state of which command?\n\r", ch );
	return;
    }

    if ( ( iCmd = get_cmd_by_name( arg ) ) == NO_COMMAND )
    {
	send_to_char( "Command not found.\n\r", ch );
	return;
    }

    buf_printf( buf, "Command: %s\n\r", cmd_table[iCmd].name );
    buf_printf( buf, "Level: %d\n\r", cmd_table[iCmd].level );
    buf_printf( buf, "Min position: %s\n\r",
		position_table[cmd_table[iCmd].position].name );
    buf_printf( buf, "Log: %s\n\r",
		flag_string( log_types, cmd_table[iCmd].log ) );
    buf_printf( buf, "Flags: %s\n\r",
		flag_string( cmd_flags, cmd_table[iCmd].flags ) );
    buf_printf( buf, "Usage: %d (%d/%d)\n\r",
                cmd_table[iCmd].usage,
                cmd_table[iCmd].usage - cmd_table[iCmd].mob_usage,
                cmd_table[iCmd].mob_usage );
    if ( cmd_table[iCmd].usage != 0 )
    {
	last_time = current_time - cmd_table[iCmd].last_used;
	last_sec   = last_time % 60;
	last_time /= 60;
	last_min   = last_time % 60;
	last_time /= 60;
	last_hr    = last_time % 24;
	last_time /= 24;
	buf_printf( buf, "Times: min %8.6f\n\r", cmd_table[iCmd].min_time );
	buf_printf( buf, "       max %8.6f\n\r", cmd_table[iCmd].max_time );
	buf_printf( buf, "       avg %8.6f\n\r",
		    cmd_table[iCmd].total_time / cmd_table[iCmd].usage );
	buf_printf( buf, "      last %8.6f  ", cmd_table[iCmd].last_time );
	if ( last_time != 0 )
	    buf_printf( buf, "%ldd ", last_time );
	buf_printf( buf, "%02d:%02d:%02d\n\r", last_hr, last_min, last_sec );
    }

    do_fun = cmd_table[iCmd].do_fun;
    found = FALSE;
    for ( i = 0; !IS_NULLSTR( cmd_table[i].name ); i++ )
    {
	if ( cmd_table[i].do_fun == do_fun && i != iCmd )
	{
	    if ( !found )
	    {
		found = TRUE;
		add_buf( buf, "Equivalent command(s):\n\r" );
	    }
	    buf_printf( buf, "  (%3d) %s\n\r",
			cmd_table[i].level,
			cmd_table[i].name );
	}
    }

    if ( cmd_table[iCmd].sec_flags != 0 )
    {
	bits = cmd_table[iCmd].sec_flags;
	add_buf( buf, "Command group(s):\n\r" );
	for ( i = 0; security_flags[i].name != NULL; i++ )
	{
	    if ( IS_SET( cmd_table[iCmd].sec_flags, security_flags[i].bit ) )
	    {
		buf_printf( buf, "  %s\n\r", security_flags[i].name );
		REMOVE_BIT( bits, security_flags[i].bit );
	    }
	}
	if ( bits != 0 )
	{
	    buf_printf( buf, "***Orphaned bits: %s\n\r", print_flags( bits ) );
	}
    }

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


void
do_coding( CHAR_DATA *ch, char* argument )
{
    if ( !IS_CODER( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    ch->act2 ^= PLR_CODING;
    if ( IS_SET( ch->act2, PLR_CODING ) )
    {
	send_to_char( "You are now marked as coding.\n\r", ch );
	act( "$n announces $e is now coding.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	send_to_char( "You are no longer coding.\n\r", ch );
	act( "$n announces $e is no longer coding.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !str_cmp( argument, "afk" ) )
        do_afk( ch, "" );

    return;
}


#define MAX_CTIME_LIST	25
#define SORT_AVG	1
#define SORT_MAX	2
#define SORT_MIN	3

struct cmd_list_t
{
    int		index;
    double	time;
};

void
do_ctimes( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    BUFFER *	buf;
    struct cmd_list_t	cmdlist[MAX_CTIME_LIST];
    double	cmdtime;
    int		i;
    int		iCmd;
    int		j;
    bool	found;
    int		sort_type;

    argument = one_argument( argument, arg );

    /* initialize cmdtime to avoid warning, even though
       it always gets initialized properly. */
    cmdtime = 0.0;

    if ( arg[0] == '\0'
    ||	 !str_cmp( arg, "avg" )
    ||	 !str_prefix( arg, "average" ) )
	sort_type = SORT_AVG;
    else if ( str_match( arg, "ma", "maximum" ) )
	sort_type = SORT_MAX;
    else if ( str_match( arg, "mi", "minimum" ) )
	sort_type = SORT_MIN;
    else
    {
	send_to_char( "Sort on which field (min/max/avg)?\n\r", ch );
	return;
    }

    for ( i = 0; i < MAX_CTIME_LIST; i++ )
    {
	cmdlist[i].index = NO_COMMAND;
	cmdlist[i].time = 0.0;
    }

    for ( iCmd = 0; cmd_table[iCmd].name != NULL; iCmd++ )
    {
	if ( cmd_table[iCmd].usage == 0 || !can_use_cmd( ch, iCmd ) )
	    continue;
	switch( sort_type )
	{
	    case SORT_AVG:
		cmdtime = cmd_table[iCmd].total_time / cmd_table[iCmd].usage;
		break;
	    case SORT_MAX:
		cmdtime = cmd_table[iCmd].max_time;
		break;
	    case SORT_MIN:
		cmdtime = cmd_table[iCmd].min_time;
		break;
	}

	for ( i = 0; i < MAX_CTIME_LIST; i++ )
	{
	    if ( cmdtime > cmdlist[i].time )
	    {
		for ( j = MAX_CTIME_LIST; j > i + 1; j-- )
		    cmdlist[j - 1] = cmdlist[j - 2];
		cmdlist[i].index = iCmd;
		cmdlist[i].time = cmdtime;
		break;
	    }
	}
    }

    buf = new_buf( );
    found = FALSE;

    for ( i = 0; i < MAX_CTIME_LIST; i++ )
    {
	if ( cmdlist[i].index == NO_COMMAND )
	    break;
	if ( !found )
	{
	    add_buf( buf, "Command      Usage       Min       Max       Avg\n\r" );
	    found = TRUE;
	}
	iCmd = cmdlist[i].index;
	buf_printf( buf, "%-12s %5d  %8.6f  %8.6f  %8.6f\n\r",
		    cmd_table[iCmd].name,
		    cmd_table[iCmd].usage,
		    cmd_table[iCmd].min_time,
		    cmd_table[iCmd].max_time,
		    cmd_table[iCmd].total_time / cmd_table[iCmd].usage );
    }

    if ( !found )
	add_buf( buf, "No commands have been used yet.\n\r" );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


/*
 * elist and efind are identical except elist looks at rs_flags and
 * efind looks at the exit_info bits
 */
void
do_efind( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			count;
    int			door;
    AREA_DATA *		first;
    bitvector		flag;
    bool		found;
    AREA_DATA *		last;
    AREA_DATA *		pArea;
    EXIT_DATA *		pExit;
    ROOM_INDEX_DATA *	pRoom;
    BUFFER *		pBuf;
    int			vnum;

    if ( *argument == '\0' )
    {
        send_to_char( "Syntax:  efind exit_flag [area]\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    /* Note that flag_value can accept multiple flags */
    if ( ( flag = flag_value( exit_flags, arg ) ) == NO_FLAG )
    {
        send_to_char( "No such flag.\n\r", ch );
        return;
    }

    if ( *argument == '\0' || !str_cmp( argument, "." ) )
    {
        if ( ch->in_room == NULL || ch->in_room->area == NULL )
        {
            send_to_char( "No such area.\n\r", ch );
            return;
        }
        first = ch->in_room->area;
        last = first->next;
    }
    else if ( !str_cmp( argument, "all" ) && ( IS_SET( ch->act2, CODER ) || IS_SET( ch->act2, HBUILDER ) ) )
    {
        first = area_first;
        last = NULL;
    }
    else
    {
        first = find_area( ch, argument );
        if ( first == NULL )
        {
            send_to_char( "Area not found.\n\r", ch );
            return;
        }
        last = first->next;
    }

    pBuf = new_buf( );
    found = FALSE;
    count = 0;
    for ( pArea = first; pArea != last; pArea = pArea->next )
    {
        for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
        {
            if ( ( pRoom = get_room_index( vnum ) ) == NULL )
                continue;
            for ( door = 0; door < MAX_DIR; door++ )
            {
                if ( ( pExit = pRoom->exit[door] ) == NULL )
                    continue;
                if ( ( pExit->exit_info & flag ) != flag )
                    continue;
                count++;
                buf_printf( pBuf, "`W%3d `g%-10s `W[`g%5d`W] `w%s`X\n\r",
                            count, dir_name[door], vnum, pRoom->name );
                found = TRUE;
            }
        }
    }

    if ( !found )
        add_buf( pBuf, "No exits found.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


void
do_elist( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			count;
    int			door;
    AREA_DATA *		first;
    bitvector		flag;
    bool		found;
    AREA_DATA *		last;
    AREA_DATA *		pArea;
    EXIT_DATA *		pExit;
    ROOM_INDEX_DATA *	pRoom;
    BUFFER *		pBuf;
    int			vnum;

    if ( *argument == '\0' )
    {
        send_to_char( "Syntax:  efind exit_flag [area]\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    /* Note that flag_value can accept multiple flags */
    if ( ( flag = flag_value( exit_flags, arg ) ) == NO_FLAG )
    {
        send_to_char( "No such flag.\n\r", ch );
        return;
    }

    if ( *argument == '\0' || !str_cmp( argument, "." ) )
    {
        if ( ch->in_room == NULL || ch->in_room->area == NULL )
        {
            send_to_char( "No such area.\n\r", ch );
            return;
        }
        first = ch->in_room->area;
        last = first->next;
    }
    else if ( !str_cmp( argument, "all" ) && ( IS_SET( ch->act2, CODER ) || IS_SET( ch->act2, HBUILDER ) ) )
    {
        first = area_first;
        last = NULL;
    }
    else
    {
        first = find_area( ch, argument );
        if ( first == NULL )
        {
            send_to_char( "Area not found.\n\r", ch );
            return;
        }
        last = first->next;
    }

    pBuf = new_buf( );
    found = FALSE;
    count = 0;
    for ( pArea = first; pArea != last; pArea = pArea->next )
    {
        for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
        {
            if ( ( pRoom = get_room_index( vnum ) ) == NULL )
                continue;
            for ( door = 0; door < MAX_DIR; door++ )
            {
                if ( ( pExit = pRoom->exit[door] ) == NULL )
                    continue;
                if ( ( pExit->rs_flags & flag ) != flag )
                    continue;
                count++;
                buf_printf( pBuf, "`W%3d `g%-10s `W[`g%5d`W] `w%s`X\n\r",
                            count, dir_name[door], vnum, pRoom->name );
                found = TRUE;
            }
        }
    }

    if ( !found )
        add_buf( pBuf, "No exits found.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


void
do_findlock( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    BUFFER *		buf;
    OBJ_INDEX_DATA *	container;
    int			door;
    bool		found;
    int			i;
    OBJ_DATA *		obj;
    OBJ_INDEX_DATA *	pObj;
    ROOM_INDEX_DATA *	room;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Find locks for which object?\n\r", ch );
	return;
    }

    if ( is_number( arg ) )
    {
	pObj = get_obj_index( atoi( arg ) );
	if ( pObj == NULL )
	{
	    send_to_char( "No such object.\n\r", ch );
	    return;
	}
    }
    else
    {
	obj = get_obj_here( ch, arg );
	if ( obj == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
	pObj = obj->pIndexData;
    }

    buf = new_buf( );

    if ( pObj->item_type != ITEM_KEY && pObj->item_type != ITEM_PERMKEY )
	buf_printf( buf, "Warning: %s`X is not a key.\n\r", pObj->short_descr );

    found = FALSE;

    for ( i = 0; i < MAX_KEY_HASH; i++ )
    {
	for ( room = room_index_hash[i]; room != NULL; room = room->next )
	{
	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if ( room->exit[door] != NULL && room->exit[door]->key == pObj->vnum )
		{
		    buf_printf( buf, "Door leading %s in [%d] %s`X\n\r",
				capitalize( dir_name[door] ), room->vnum, room->name );
		    found = TRUE;
		}
	    }
	}
    }

    for ( i = 0; i < MAX_KEY_HASH; i++ )
    {
	for ( container = obj_index_hash[i]; container != NULL; container = container->next )
	{
	    if ( container->item_type == ITEM_CONTAINER && container->value[2] == pObj->vnum )
	    {
		buf_printf( buf, "[%5d] %s`X\n\r", container->vnum, container->short_descr );
		found = TRUE;
	    }
	}
    }

    if ( !found )
	buf_printf( buf, "%s`X does not unlock anything.\n\r", pObj->short_descr );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );

    return;
}


void
do_finger( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    char		last_time[SHORT_STRING_LENGTH];
    char		first_time[SHORT_STRING_LENGTH];
    char		buf[MAX_INPUT_LENGTH];
    BUFFER *		pBuf;
    USERLIST_DATA *	pUser;
    USERLIST_DATA *	pTmp;
    time_t		elapsed;
    time_t		tOldest;
    time_t		tNewest;
    time_t		temp;
    bool		found;
    bool		fRange;
    int			iCount;
    int			played;
    char *		t;
    char		tcolor;
    int			llev;
    int			ulev;
    int			khrs;
    int			hrs;
    int			mins;
    CHAR_DATA *		vch;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: finger <playername>\n\r", ch );
	send_to_char( "        finger <level>\n\r", ch );
	send_to_char( "        finger <level> <level>\n\r", ch );
	send_to_char( "        finger mortals|immortals|all\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "self" ) )
	strcpy( arg, ch->name );

    if ( get_trust( ch ) >= IMPLEMENTOR && !str_cmp( arg, "delete" ) )
    {
	if ( *argument == '\0' )
	{
	    send_to_char( "Delete whom from finger list?\n\r", ch );
	    return;
	}

	if ( user_first == NULL )
	{
	    send_to_char( "Odd, the finger list is empty.\n\r", ch );
	    return;
	}

	if ( !strcasecmp( argument, user_first->name ) )
	{
	    pUser = user_first;
	    user_first = pUser->next;
	    if ( user_last == pUser )
		user_last = NULL;
	}
	else
	{
	    for ( pTmp = user_first; pTmp->next != NULL; pTmp = pTmp->next )
		if ( !strcasecmp( argument, pTmp->next->name ) )
		    break;
	    if ( pTmp->next == NULL )
	    {
		send_to_char( "User not found.\n\r", ch );
		return;
	    }

	    pUser = pTmp->next;
	    pTmp->next = pUser->next;
	    if ( user_last == pUser )
		user_last = pTmp;
	}
	ch_printf( ch, "%s removed from finger list.\n\r", pUser->name );
	free_user_rec( pUser );
	queue_userlist_save( );
	return;
    }

    /* Check range */
    fRange = FALSE;
    llev = 0;
    ulev = 0;

    if ( !str_cmp( arg, "all" ) )
    {
	llev = 1;
	ulev = MAX_LEVEL;
	fRange = TRUE;
    }
    else if ( !str_cmp( arg, "imms" ) || str_match( arg, "imm", "immortals" ) )
    {
	llev = LEVEL_IMMORTAL;
	ulev = MAX_LEVEL;
	fRange = TRUE;
    }
    else if ( !str_cmp( arg, "morts" ) || str_match( arg, "mort", "mortals" ) )
    {
	llev = 1;
	ulev = LEVEL_IMMORTAL - 1;
	fRange = TRUE;
    }

    if ( fRange || is_number( arg ) )
    {
	/* Finger multiple users */
	if ( is_number( arg ) )
	{
	    llev = atoi( arg );
	    if ( is_number( argument ) )
		ulev = atoi( argument );
	    else if ( *argument == '\0' )
		ulev = llev;
	    else
	    {
		do_finger( ch, "" );
		return;
	    }
	}

	pBuf = new_buf( );
	iCount = 0;
	found = FALSE;
	for ( pUser = user_first; pUser != NULL; pUser = pUser->next )
	{
	    if ( pUser->level < llev || pUser->level > ulev )
		continue;
	    if ( !found )
	    {
		add_buf( pBuf, "Lev  Name         Race  Class  Last Logon\n\r" );
		add_buf( pBuf, "`W=======================================================`w\n\r" );
		found = TRUE;
	    }
	    elapsed = current_time - pUser->lastlogon;
	    if ( elapsed < 60 * 60 * 24 )
		tcolor = 'G';
	    else if ( elapsed < 60 * 60 * 24 * 7 )
		tcolor = 'g';
	    else if ( elapsed < 60 * 60 * 24 * 14 )
		tcolor = 'c';
	    else if ( elapsed < 60 * 60 * 24 * 30 )
		tcolor = 'Y';
	    else
		tcolor = 'R';
	    t = ctime( &pUser->lastlogon );
	    *(t + strlen( t ) - 1 ) = '\0';
	    buf_printf( pBuf, "%3d  `W%-12s %5s   %3s  `%c%s`w\n\r",
			pUser->level,
			pUser->name,
			pUser->race,
			pUser->class,
			tcolor,
			t );
	    iCount++;
	}

	if ( !found )
	    add_buf( pBuf, "No users found in that range.\n\r" );
	else
	    buf_printf( pBuf, "%d user%s found.\n\r", iCount, iCount != 1 ? "s" : "" );
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return;
    }

    /* Finger users by first logon time */
    if ( !str_cmp( arg, "new" ) )
    {
	argument = one_argument( argument, arg );
	if ( is_number( arg ) )
	    tOldest = atoi( arg );
	else if ( arg[0] == '\0' )
	    tOldest = 14;
	else
	{
	    send_to_char( "Syntax:  finger new <days> <days>\n\r", ch );
	    return;
	}

	if ( tOldest != 0 )
	    tOldest = current_time - tOldest * 60 * 60 * 24;

	if ( is_number( argument ) )
	    tNewest = atoi( argument );
	else if ( *argument == '\0' )
	    tNewest = 0;
	else
	{
	    send_to_char( "Syntax:  finger new <days> <days>\n\r", ch );
	    return;
	}

	tNewest = current_time - tNewest * 24 * 60 * 60;

	if ( tOldest > tNewest )
	{
	    temp = tOldest;
	    tOldest = tNewest;
	    tNewest = temp;
	}

	pBuf = new_buf( );
	iCount = 0;
	found = FALSE;

	for ( pUser = user_first; pUser != NULL; pUser = pUser->next )
	{
	    if ( pUser->id < tOldest || pUser->id > tNewest )
		continue;

	    if ( !found )
	    {
		add_buf( pBuf, "Lev  Name         First Logon\n\r" );
		add_buf( pBuf, "`W==========================================`w\n\r" );
		found = TRUE;
	    }

	    if ( pUser->id == 0 )
		t = "(Unknown)";
	    else
	    {
		t = ctime( &pUser->id );
		*(t + strlen( t ) - 1 ) = '\0';
	    }
	    buf_printf( pBuf, "%3d  `W%-12s `w%s %s\n\r",
			pUser->level,
			pUser->name,
			t,
                        strtimediff( pUser->id, current_time ) );
	    iCount++;
	}

	if ( !found )
	    add_buf( pBuf, "No users found in that range.\n\r" );
	else
	    buf_printf( pBuf, "%d user%s found.\n\r", iCount, iCount != 1 ? "s" : "" );

	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );

	return;
    }

    /* Finger a single user */
    for ( pUser = user_first; pUser != NULL; pUser = pUser->next )
    {
	if ( !str_prefix( arg, pUser->name ) )
	    break;
    }

    if ( pUser == NULL )
    {
	send_to_char( "That person was not found.\n\r", ch );
	return;
    }

    pBuf = new_buf( );

    if ( pUser->lastlogon == 0 )
        t = "(never)";
    else
    {
	t = ctime( &pUser->lastlogon );
	*( t + strlen( t ) - 1 ) = '\0';
    }
    strcpy( last_time, t );

    if ( pUser->id == 0 )
        t = "(unknown)";
    else
    {
	t = ctime( &pUser->id );
	*( t + strlen( t ) - 1 ) = '\0';
    }

    if ( ( vch = get_char_world( ch, pUser->name ) ) != NULL && !IS_NPC( vch ) )
        played = vch->played + current_time - vch->logon;
    else
        played = pUser->played;

    khrs = played / ( 3600 * 1000 );
    hrs = ( played / 3600 ) % 1000;
    mins = ( played / 60 ) % 60;
    if ( khrs != 0 )
        sprintf( buf, "%d,%03d hr %d min", khrs, hrs, mins );
    else
        sprintf( buf, "%d hr %d min", hrs, mins );

    strcpy( first_time, t );

    buf_printf( pBuf, "Name:         %s\n\r", pUser->name );
    buf_printf( pBuf, "Level:        %d\n\r", pUser->level );
    buf_printf( pBuf, "Race:         %s\n\r", pUser->race );
    buf_printf( pBuf, "Class:        %s\n\r", pUser->class );
    buf_printf( pBuf, "Played:       %s\n\r", buf );
    buf_printf( pBuf, "First logon:  %s, %s ago.\n\r", first_time,
                strtimediff( pUser->id, current_time ) );
    buf_printf( pBuf, "Last logon:   %s, %s ago.\n\r", last_time,
                strtimediff( pUser->lastlogon, current_time ) );
    buf_printf( pBuf, "Total logons: %d\n\r", pUser->logcount );
    buf_printf( pBuf, "From site:    %s\n\r", pUser->host );

    send_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_forcetick( CHAR_DATA *ch, char *argument )
{
    int value;

    if ( *argument == '\0' )
	value = 1;
    else if ( is_number ( argument ) )
	value = atoi( argument );
    else
    {
	send_to_char( "Syntax: forcetick\n\r", ch );
	send_to_char( "        forcetick <number>\n\r", ch );
	send_to_char( " Number, if given must be from 1 to 25.\n\r", ch );
	return;
    }

    if ( value < 1 || value > 25 )
    {
	send_to_char( "Number of ticks must be between 1 and 25.\n\r", ch );
	return;
    }

    if ( value == 1 )
	send_to_char( "You have forced a tick.\n\r", ch );
    else
	ch_printf( ch, "You force %d ticks.\n\r", value );

    forcetick = value;
    return;
}


void
do_grant( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    bitvector	bit;
    bool	fAdd;
    bool	found;
    char *	gname;
    int		level;
    int		sec;
    int		trust;
    CHAR_DATA *	vch;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs may not meddle in the affairs of the Immortals.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r", ch );
	send_to_char( "grant <name>   <securitygroup>  Grant group to player.\n\r", ch );
	send_to_char( "grant <name>  <+securitygroup>    \"     \"    \"    \"\n\r", ch );
	send_to_char( "grant <name>  <-securitygroup>  Remove player from group.\n\r", ch );
	send_to_char( "grant <level>  <securitygroup>  Add all connected imms of <level> or higher.\n\r", ch );
	send_to_char( "grant <level> <+securitygroup>   \"   \"      \"       \"   \"    \"     \"    \"\n\r", ch );
	send_to_char( "grant <level> <-securitygroup>  Remove connected imms from group.\n\r", ch );
	return;
    }

    fAdd = TRUE;
    if ( *argument == '-' )
    {
	fAdd = FALSE;
	argument++;
    }
    else if ( *argument == '+' )
    {
	argument++;
    }
    if ( *argument == '\0' )
    {
	send_to_char( "Grant which command?\n\r", ch );
	return;
    }

    if ( ( sec = security_lookup( argument ) ) < 0 )
    {
	send_to_char( "There is no security group of that name.\n\r", ch );
	return;
    }

    trust = get_trust( ch );
    bit = security_flags[sec].bit;
    gname = security_flags[sec].name;

    if ( trust < MAX_LEVEL && !IS_SET( ch->pcdata->sec_groups, bit ) )
    {
	send_to_char( "Cannot change others' access to groups you don't have.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
	sprintf( arg, "%d", LEVEL_IMMORTAL );

    if ( is_number( arg ) )
    {
	level = atoi( arg );
	if ( level < LEVEL_IMMORTAL )
	{
	    send_to_char( "Level must be an immortal level.\n\r", ch );
	    return;
	}
	if ( level > trust && !fAdd )
	{
	    send_to_char( "Level is limited to your trust level.\n\r", ch );
	    return;
	}

	found = FALSE;
	for ( vch = char_list; vch; vch = vch->next )
	{
	    if ( !IS_NPC( vch )
	    && get_trust( vch ) >= level )
	    {
		if ( fAdd && !IS_SET( vch->pcdata->sec_groups, bit ) )
		{
		    found = TRUE;
		    SET_BIT( vch->pcdata->sec_groups, bit );
		    if ( vch != ch )
		    {
			ch_printf( vch, "You have been added to the %s command group.\n\r", gname );
			ch_printf( ch, "%s added.\n\r", vch->name );
		    }
		    else
		    {
			send_to_char( "You have been added.\n\r", ch );
		    }
		}
		else if ( !fAdd
		     &&	  get_trust( vch ) <= trust
		     &&   IS_SET( vch->pcdata->sec_groups, bit ) )
		{
		    found = TRUE;
		    REMOVE_BIT( vch->pcdata->sec_groups, bit );
		    if ( vch != ch )
		    {
			ch_printf( vch, "You have been removed from the %s security group.\n\r", gname );
			ch_printf( ch, "%s removed.\n\r", vch->name );
		    }
		    else
		    {
			send_to_char( "You have been removed.\n\r", ch );
		    }
		}
	    }
	}
	if ( !found )
	    send_to_char( "No immortals changed.\n\r", ch );
	return;
    }
    else
    {
	if ( ( vch = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They're not here.\n\r", ch );
	    return;
	}

	if ( IS_NPC( vch ) )
	{
	    send_to_char( "Not on mobs!\n\r", ch );
	    return;
	}

	if ( fAdd )
	{
	    if ( IS_SET( vch->pcdata->sec_groups, bit ) )
	    {
		if ( ch == vch )
		    send_to_char( "You are already in that group.\n\r", ch );
		else
		    ch_printf( ch, "%s is already in that group.\n\r", vch->name );
		return;
	    }
	    SET_BIT( vch->pcdata->sec_groups, bit );
	    if ( vch != ch )
		ch_printf( vch, "You have been added to the %s command group.\n\r", gname );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	else
	{
	    if ( get_trust( vch ) > trust )
	    {
		send_to_char( "You are of too low a level to do that.\n\r", ch );
		return;
	    }
	    if ( !IS_SET( vch->pcdata->sec_groups, bit ) )
	    {
		if ( ch == vch )
		    send_to_char( "You are not in that group.\n\r", ch );
		else
		    ch_printf( ch, "%s is not in that group.\n\r", vch->name );
		return;
	    }
	    REMOVE_BIT( vch->pcdata->sec_groups, bit );
	    if ( vch != ch )
		ch_printf( vch, "You have been removed from the %s command group.\n\r", gname );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }
}


void
do_hfind( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *	pHelp;
    BUFFER *	pBuf;
    char *	nextkey;
    char	key[MAX_INPUT_LENGTH];
    char	buf[SHORT_STRING_LENGTH];
    int		count = 0;
    bool	found = FALSE;

    if ( *argument == '\0' )
    {
	send_to_char( "Hfind what?\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
    {
	++count;
	if ( pHelp->level > get_trust( ch ) )
	    continue;
	nextkey = one_argument( pHelp->keyword, key );
	while ( key[0] != '\0' )
	{
	    if ( is_name( argument, key ) )
	    {
		if ( !found )
		{
		    add_buf( pBuf, "  Vnum  Level  Area Keyword(s)\n\r" );
		    found = TRUE;
		}
		if ( pHelp->area )
		    sprintf( buf, "%3d", pHelp->area->vnum );
		else
		    strcpy( buf, "---" );
		buf_printf( pBuf, "`R[`W%4d`R] [`W%4d`R] [`W%3s`R]`X %s\n\r",
			count, pHelp->level, buf,
			pHelp->keyword );
		break;
	    }
	    nextkey = one_argument( nextkey, key );
	}
    }
    if ( found )
    {
	set_char_color( C_DEFAULT, ch );
	page_to_char( buf_string( pBuf ), ch );
    }
    else
	send_to_char( "Help not found.\n\r", ch );
    free_buf( pBuf );
    return;
}


void
do_hlist( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *	pHelp;
    BUFFER *	pBuf;
    int		uvnum;
    int		lvnum;
    int		atvnum;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char	buf[SHORT_STRING_LENGTH];
    bool	found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    lvnum = atoi( arg1 );
    uvnum = arg2[0] == '\0' ? lvnum + 19 : atoi( arg2 );

    if ( lvnum <= 0 || uvnum == 0 || lvnum > uvnum )
    {
	send_to_char( "Syntax: hlist [firsthelp#] [secondhelp#]\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    atvnum = 1;
    found = FALSE;

    for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
    {
	if ( atvnum >= lvnum && pHelp->level <= get_trust( ch ) )
	{
	    if ( !found )
	    {
		add_buf( pBuf, "  Vnum  Level  Area Keyword(s)\n\r" );
		found = TRUE;
	    }
	    if ( pHelp->area )
		sprintf( buf, "%3d", pHelp->area->vnum );
	    else
		strcpy( buf, "---" );
	    buf_printf( pBuf, "`R[`W%4d`R] [`W%4d`R] [`W%3s`R]`X %s\n\r",
			atvnum, pHelp->level, buf,
			pHelp->keyword );
	}
	atvnum++;
	if ( atvnum > uvnum )
	    break;
    }

    if ( found )
    {
	set_char_color( C_DEFAULT, ch );
	page_to_char( buf_string( pBuf ), ch );
    }
    else
	send_to_char( "No helps found in that range.\n\r", ch );
    free_buf( pBuf );

    return;
}


void
do_imote( CHAR_DATA *ch, char *argument )
{
    char		buf[MAX_INPUT_LENGTH*2];
    DESCRIPTOR_DATA *	d;
    char *		pLast;
    CHAR_DATA *		vch;

    if ( !IS_NPC( ch ) && IS_SET( ch->comm, COMM_NOEMOTE ) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Imote what?\n\r", ch );
	return;
    }

    REMOVE_BIT( ch->deaf, CHANNEL_IMMTALK );

    buf[0] = ' ';
    pLast = stpcpy( *argument == '\'' || *argument == ',' ? buf : &buf[1], argument );
    if ( !ispunct( *(pLast - 1) ) )
	strcpy( pLast, "." );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING )
	    continue;
	vch = d->original != NULL ? d->original : d->character;
	if ( can_see( vch, ch ) && IS_IMMORTAL( vch )
	&&   !is_ignoring( vch, ch, IGNORE_CHANNELS )
	&&   !IS_SET( vch->deaf, CHANNEL_IMMTALK ) )
	    act_color( AT_LBLUE, "`P$N`C$t", vch, buf, ch, TO_CHAR, POS_DEAD );
    }
    return;
}


void
do_iscore( CHAR_DATA *ch, char *argument )
{
    bitvector	bits;
    BUFFER *	buf;
    int		col;
    int		flag;
    bool	found;
    int		i;
    char *	p;
    char	str[SHORT_STRING_LENGTH];
    CHAR_DATA *	vch;
    char	word[SHORT_STRING_LENGTH];

    buf = new_buf();

    if ( *argument == '\0' || get_trust( ch ) < MAX_LEVEL )
	vch = ch;
    else
	vch = get_char_world( ch, argument );

    if ( vch == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( vch ) )
    {
	send_to_char( "Not with mobs.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( vch ) )
    {
	send_to_char( "That's not an immortal.\n\r", ch );
	return;
    }

    if ( vch != ch )
    {
	buf_printf( buf, "Immortal score for %s:\n\r", vch->name );
	buf_printf( buf, "Level: %d", vch->level );
	if ( get_trust( vch ) != vch->level )
	    buf_printf( buf, "  Trust: %d", get_trust( vch ) );
	add_buf( buf, "\n\r" );
    }

    add_buf( buf, "Poofin" );
    if ( IS_NULLSTR( vch->pcdata->bamfin ) )
	add_buf( buf, " not set.\n\r" );
    else
	buf_printf( buf, ":\n\r%s`X\n\r", vch->pcdata->bamfin );

    add_buf( buf, "Poofout" );
    if ( IS_NULLSTR( vch->pcdata->bamfout ) )
	add_buf( buf, " not set.\n\r" );
    else
	buf_printf( buf, ":\n\r%s`X\n\r", vch->pcdata->bamfout );

    buf_printf( buf, "Holylight %s.\n\r",
		IS_SET( vch->act, PLR_HOLYLIGHT ) ? "ON" : "off" );

    add_buf( buf, "Whotext" );
    if ( IS_NULLSTR( vch->pcdata->who_text ) )
	add_buf( buf, " not set.\n\r" );
    else
	buf_printf( buf, ": %s`X\n\r", vch->pcdata->who_text );

    if ( vch->invis_level != 0 )
	buf_printf( buf, "Wizinvisible at level %d.\n\r", vch->invis_level );
    if ( vch->incog_level != 0 )
	buf_printf( buf, "Incognito at level %d.\n\r", vch->incog_level );

    if ( vch->desc != NULL && vch->desc->editor != ED_NONE )
    {
	buf_printf( buf, "In%s(%s)\n\r", olc_ed_name( vch ), olc_ed_vnum( vch ) );
    }

    add_buf( buf, "Wiznet status:\n\r" );
    str[0] = '\0';
    p = str;
    for ( flag = 0; wiznet_table[flag].name != NULL; flag++ )
    {
	if ( wiznet_table[flag].level <= get_trust( vch ) )
	{
	    strcpy( word, IS_SET( vch->wiznet, wiznet_table[flag].flag )
			  ? all_capitalize( wiznet_table[flag].name )
			  : wiznet_table[flag].name );
	    if ( p - str + strlen( word ) > 78 )
	    {
		strcpy( p, "\n\r" );
		add_buf( buf, str );
		p = str;
		*p = '\0';
	    }
	    p += sprintf( p, "%s ", word );
	}
    }
    strcpy( p, "\n\r" );
    add_buf( buf, str );

    add_buf( buf, "Command group(s)" );
    if ( vch != ch )
	buf_printf( buf, " for %s", vch->name );
    add_buf( buf, ":\n\r" );
    bits = vch->pcdata->sec_groups;
    found = FALSE;
    for ( i = 0; security_flags[i].name != NULL; i++ )
    {
	if ( IS_SET( bits, security_flags[i].bit ) )
	{
	    buf_printf( buf, "  %s\n\r", security_flags[i].name );
	    REMOVE_BIT( bits, security_flags[i].bit ); 
	    found = TRUE;
	}
    }
    if ( !found )
	add_buf( buf, "  (none)\n\r" );
    if ( bits != 0 )
	buf_printf( buf, "  ***Orphaned bits %s\n\r", print_flags( bits ) );

    if ( !IS_NULLSTR( vch->pcdata->empower ) )
    {
        buf_printf( buf, "%-15s", "Empowerments:" );
        col = 1;
        p = vch->pcdata->empower;
        while ( *p != '\0' )
        {
            p = one_argument( p, str );
            buf_printf( buf, "%-15s", str );
            col++;
            if ( col % 5 == 0 )
                add_buf( buf, "\n\r" );
        }
        if ( col % 5 != 0 )
            add_buf( buf, "\n\r" );
    }

    if ( !IS_NULLSTR( vch->pcdata->detract ) )
    {
        buf_printf( buf, "%-15s", "Detractments:" );
        col = 1;
        p = vch->pcdata->detract;
        while ( *p != '\0' )
        {
            p = one_argument( p, str );
            buf_printf( buf, "%-15s", str );
            col++;
            if ( col % 5 == 0 )
                add_buf( buf, "\n\r" );
        }
        if ( col % 5 != 0 )
            add_buf( buf, "\n\r" );
    }

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


void
do_last( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	victim;
    char	pfile[MAX_INPUT_LENGTH + sizeof( PLAYER_DIR )];
    char	pname[MAX_INPUT_LENGTH];
    struct stat info;

    if ( *argument == '\0' )
    {
	send_to_char( "Check time on whom?\n\r", ch );
	return;
    }

    strcpy( pname, capitalize( argument ) );

    if ( ( victim = get_char_world( ch, argument ) )
	 && !IS_NPC( victim )
	 && !str_cmp( argument, victim->name ) )
    {
	ch_printf( ch, "%s is currently playing.\n\r", pname );
	return;
    }

    sprintf( pfile, "%s%s", PLAYER_DIR, pname );
    if ( stat( pfile, &info ) )
    {
	send_to_char( "No player by that name here.\n\r", ch );
	return;
    }

    /* Just a quick hack for now */
    ch_printf( ch, "%s was last here on %s", pname, ctime( &info.st_mtime ) );
    return;
}


void
do_lastmod( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    struct mod_data *	newMod;
    struct mod_data *	mod_first;
    struct mod_data *	mod_last;
    struct mod_data *	mod_next;
    struct mod_data *	pMod;
    struct stat 	stat_info;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf[SHORT_STRING_LENGTH];
    int			first_area;
    int			last_area;
    int			vnum;
    int			last_vnum;
    bool		fAll = FALSE;

    argument = one_argument( argument, arg1 );

    if ( !str_cmp( arg1, "all" ) )
    {
	fAll = TRUE;
	argument = one_argument( argument, arg1 );
    }

    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	first_area = 0;
	last_vnum = -1;
	for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
	    if ( pArea->vnum > last_vnum )
		last_vnum = pArea->vnum;
	sprintf( arg2, "%d", last_vnum );
    }
    else if ( !str_cmp( arg1, "." ) )
    {
	first_area = ch->in_room->area->vnum;
    }
    else if ( is_number( arg1 ) )
    {
	first_area = atoi( arg1 );
    }
    else
    {
	/* might want to look up an area by name here */
	send_to_char( "Start with which area?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	last_area = first_area;
    }
    else if ( !str_cmp( arg2, "." ) )
    {
	last_area = ch->in_room->area->vnum;
    }
    else if ( !str_cmp( arg2, "all" ) )
    {
	last_area = first_area;
	fAll = TRUE;
    }
    else if ( is_number( arg2 ) )
    {
	last_area = atoi( arg2 );
    }
    else
    {
	/* might want to look up an area by name here, too */
	send_to_char( "End with which area?\n\r", ch );
	return;
    }

    if ( !str_cmp( argument, "all" ) )
    {
	fAll = TRUE;
    }

    mod_first = mod_last = NULL;
    for ( vnum = first_area; vnum <= last_area; vnum++ )
    {
	if ( ( pArea = get_area_data( vnum ) ) == NULL )
	    continue;
	if ( pArea->last_changed == 0 && !IS_SET( pArea->area_flags, AREA_CHANGED ) && !fAll )
	    continue;
	newMod = alloc_mem( sizeof( *newMod ) );
	newMod->pArea = pArea;
	if ( IS_SET( pArea->area_flags, AREA_CHANGED ) )
	{
	    newMod->mtime = current_time;
	}
	else
	{
	    if ( stat( pArea->file_name, &stat_info ) )
	    {
		log_printf( "Problems statting area %d (%s)", vnum, pArea->name );
		perror( "Do_lastmod: stat:" );
		ch_printf( ch, "Problems with area %d.\n\r", vnum );
		free_mem( newMod, sizeof( *newMod ) );
		continue;
	    }
	    if ( pArea->last_changed == 0 )
		newMod->mtime = stat_info.st_mtime;
	    else
		newMod->mtime = pArea->last_changed;
	}
	if ( mod_first == NULL )
	{
	    mod_first = newMod;
	    mod_last = newMod;
	    newMod->next = NULL;
	}
	else if ( newMod->mtime > mod_first->mtime )
	{
	    newMod->next = mod_first;
	    mod_first = newMod;
	}
	else if ( mod_last->mtime >= newMod->mtime )
	{
	    mod_last->next = newMod;
	    mod_last = newMod;
	    newMod->next = NULL;
	}
	else
	{
	    for ( pMod = mod_first; ; pMod = pMod->next )
	    {
		if ( newMod->mtime > pMod->next->mtime )
		{
		    newMod->next = pMod->next;
		    pMod->next = newMod;
		    break;
		}
	    }
	}
    }

    if ( mod_first == NULL )
    {
	send_to_char( "No areas found.\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    for ( pMod = mod_first; pMod; pMod = mod_next )
    {
	mod_next = pMod->next;
	strcpy( buf, ctime( &pMod->mtime ) );
	buf[ strlen( buf ) - 1 ] = '\0';
	buf_printf( pBuf, "[%3d] %s %c%s`X, %s\n\r",
		    pMod->pArea->vnum, &buf[4],
		    pMod->pArea->last_changed ? ' ' : '*',
		    pMod->pArea->name,
		    pMod->pArea->builders );
	free_mem( pMod, sizeof( *pMod ) );
    }
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_launch( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	to_room;
    CHAR_DATA *		vch;
    int			type;
    bool		fOkay;

    argument = one_argument( argument, arg );

    if ( ( in_room = ch->in_room ) == NULL )
    {
	send_to_char( "Launch what?\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Launch where?\n\r", ch );
	return;
    }

    if ( in_room->vehicle_type == VEHICLE_NONE )
    {
	send_to_char( "You can't launch this room.\n\r", ch );
	return;
    }

    if ( ( to_room = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( !is_room_owner( ch, to_room ) && room_is_private( to_room )
    &&	 get_trust( ch ) < MAX_LEVEL )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( to_room == in_room )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( to_room->in_room != NULL )
    {
	send_to_char( "You can't launch anything there.\n\r", ch );
	return;
    }

    type = in_room->vehicle_type;

    switch( to_room->sector_type )
    {
	default:
	case SECT_NONE:
	case SECT_AIR:
	case SECT_INSIDE:
	case SECT_MOUNTAIN:
	case SECT_SWAMP:
	    fOkay = FALSE; break;
	case SECT_CITY:
	    fOkay = TRUE; break;
	case SECT_DESERT:
	case SECT_FIELD:
	case SECT_FOREST:
	case SECT_ROAD:
	    fOkay = ( type == VEHICLE_LAND || type == VEHICLE_AMPHIB );
	    break;
	case SECT_UNDERWATER:
	    fOkay = FALSE;
	    break;
	case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
	case SECT_LAKE:
	case SECT_RIVER:
	case SECT_OCEAN:
	    fOkay = ( type == VEHICLE_SEA || type == VEHICLE_AMPHIB );
	    break;
	case SECT_BEACH:
	    if ( type == VEHICLE_SEA && in_room->sector_type == SECT_BEACH )
	        fOkay = FALSE;
            else
                fOkay = TRUE;
            break;
    }

    if ( !fOkay )
    {
	send_to_char( "You can't launch this there.\n\r", ch );
	return;
    }

    if ( in_room->in_room != NULL )
    {
	act_color( AT_IMM, "$t suddenly fades and disappears into nothingness.",
		   in_room->in_room->people, ROOMNAME( in_room ), NULL, TO_ALL, POS_RESTING );
	room_from_room( in_room );
    }

    room_to_room( in_room, to_room );
    for ( vch = in_room->people; vch != NULL; vch = vch->next_in_room )
	if ( IS_AWAKE( vch ) )
	    do_look( vch, "auto" );

    act_color( AT_IMM, "The air shimmers as $t appears.", to_room->people,
               ROOMNAME( in_room ), NULL, TO_ALL, POS_RESTING );

    return;
}


void
do_linkdead( CHAR_DATA *ch, char *argument )
{
    BUFFER *		buf;
    DESCRIPTOR_DATA *	d;
    bool		found;
    int			iCount;
    char		sbuf[MAX_INPUT_LENGTH];
    char *		state;
    CHAR_DATA *		vch;
    CHAR_DATA *		vch_next;

    if ( *argument != '\0' && !str_prefix( argument, "clear" ) )
    {
	found = FALSE;
	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;
	    if ( !IS_NPC( vch )
	    &&	 vch->desc == NULL
	    &&	 can_see( ch, vch )
	    &&	 !IS_SET( vch->act, PLR_SWITCHED )
	    &&	 !IS_SET( vch->act2, PLR_PLOADED ) )
	    {
		do_quit( vch, "" );
		found = TRUE;
	    }
	}
	send_to_char( found ? "Ok.\n\r" : "All players have descriptors.\n\r", ch );
	return;
    }

    buf = new_buf( );

    found = FALSE;
    iCount = 0;
    for ( vch = char_list; vch != NULL; vch = vch->next )
    {
	if ( !IS_NPC( vch ) && vch->desc == NULL && can_see( ch, vch ) )
	{
	    if ( !found )
	    {
		add_buf( buf, "Name        In Room  State\n\r" );
		add_buf( buf, "=============================\n\r" );
		found = TRUE;
	    }
	    if ( IS_SET( vch->act, PLR_SWITCHED ) )
	    {
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
		    if ( d->original == vch )
			break;
		}
		if ( d == NULL )
		    state = "Switched into nobody?!?!";
		else
		{
		    sprintf( sbuf, "Switched into %s`X", PERS( d->character, ch ) );
		    state = sbuf;
		}
	    }
	    else if ( IS_SET( vch->act2, PLR_PLOADED ) )
		state = "Ploaded";
	    else
		state = "Linkdead";
	    buf_printf( buf, "%-12s %6d  %s\n\r",
			vch->name,
			vch->in_room != NULL ? vch->in_room->vnum : 0,
			state );
	    iCount++;
	}
    }

    if ( !found )
	add_buf( buf, "All players have descriptors.\n\r" );
    else
	buf_printf( buf, "%d player%s found.\n\r", iCount, iCount != 1 ? "s" : "" );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


void
do_mlist( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH];
    int			col;
    int			lvnum;
    int			uvnum;
    int			vnum;
    bool		found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /*
     * Process args.  Syntax allows "." for lower or upper vnum of current
     * area.  If no second arg, highest vnum of area containing first
     * arg is assumed.
     */
    if ( arg1[0] == '\0' || !str_cmp( arg1, "." ) )
    {
	lvnum = ch->in_room->area->min_vnum;
    }
    else
    {
	lvnum = atoi( arg1 );
    }

    if ( arg2[0] == '\0' )
    {
	if ( ( pArea = get_area( lvnum ) ) == NULL )
	{
	    send_to_char( "No areas have that vnum.\n\r", ch );
	    return;
	}
	uvnum = pArea->max_vnum;
    }
    else if ( !str_cmp( arg2, "." ) )
    {
	uvnum = ch->in_room->area->max_vnum;
    }
    else
    {
	uvnum = atoi( arg2 );
    }

    /*
     * Check args
     */
    if ( lvnum == 0 || uvnum == 0 )
    {
	send_to_char( "Syntax: mlist <lvnum> [<uvnum>]\n\r", ch );
	return;
    }
    if ( lvnum < 1 || uvnum < lvnum )
    {
	send_to_char( "Invalid vnum range.\n\r", ch );
	return;
    }

    found = FALSE;
    pBuf = new_buf( );
    col = 0;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( pArea->min_vnum > uvnum || pArea->max_vnum < lvnum )
	    continue;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
	    if ( vnum < lvnum || vnum > uvnum )
		continue;
	    if ( ( pMob = get_mob_index( vnum ) ) )
	    {
		found = TRUE;
		strip_color( buf, pMob->short_descr );
		buf_printf( pBuf, "`R[`W%5d`R] `W%3d - `w%-25.24s", vnum, pMob->level, buf );
		if ( ++col % 2 == 0 )
		    add_buf( pBuf, "\n\r" );
	    }
	}
    }

    if ( col % 2 )
	add_buf( pBuf, "\n\r" );

    if ( !found )
	add_buf( pBuf, "No mobiles found in that range.\n\r" );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_nukereply (CHAR_DATA *ch, char *argument)
{
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    DESCRIPTOR_DATA *d;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: nukerep <character>\n\r", ch );
	send_to_char( "        nukerep all\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if( ( victim = d->character ) != NULL )
		if ( victim->reply == ch && victim != ch && get_trust( victim ) <= get_trust( ch ) )
		    victim->reply = NULL;
	}
	send_to_char( "You nuked all replies to you.\r\n", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->reply == ch )
    {
	if ( get_trust( victim ) > get_trust( ch ) )
	{
	    send_to_char( "You cannot.\n\r", ch );
	    return;
	}
	victim->reply = NULL;
	send_to_char( "Reply nuked.\r\n", ch );
    }
    else
    {
	send_to_char( "Not necessary, but okay.\n\r", ch );
    }
    return;
}


void
do_olist( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH];
    int			col;
    int			lvnum;
    int			uvnum;
    int			vnum;
    bool		found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /*
     * Process args.  Syntax allows "." for lower or upper vnum of current
     * area.  If no second arg, highest vnum of area containing first
     * arg is assumed.
     */
    if ( arg1[0] == '\0' || !str_cmp( arg1, "." ) )
    {
	lvnum = ch->in_room->area->min_vnum;
    }
    else
    {
	lvnum = atoi( arg1 );
    }

    if ( arg2[0] == '\0' )
    {
	if ( ( pArea = get_area( lvnum ) ) == NULL )
	{
	    send_to_char( "No areas have that vnum.\n\r", ch );
	    return;
	}
	uvnum = pArea->max_vnum;
    }
    else if ( !str_cmp( arg2, "." ) )
    {
	uvnum = ch->in_room->area->max_vnum;
    }
    else
    {
	uvnum = atoi( arg2 );
    }

    /*
     * Check args
     */
    if ( lvnum == 0 || uvnum == 0 )
    {
	send_to_char( "Syntax: olist <lvnum> [<uvnum>]\n\r", ch );
	return;
    }
    if ( lvnum < 1 || uvnum < lvnum )
    {
	send_to_char( "Invalid vnum range.\n\r", ch );
	return;
    }

    found = FALSE;
    pBuf = new_buf( );
    col = 0;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( pArea->min_vnum > uvnum || pArea->max_vnum < lvnum )
	    continue;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
	    if ( vnum < lvnum || vnum > uvnum )
		continue;
	    if ( ( pObj = get_obj_index( vnum ) ) )
	    {
		found = TRUE;
		strip_color( buf, pObj->short_descr );
		buf_printf( pBuf, "`R[`W%5d`R] `W%3d - `w%-25.24s", vnum, pObj->level, buf );
		if ( ++col % 2 == 0 )
		    add_buf( pBuf, "\n\r" );
	    }
	}
    }

    if ( col % 2 )
	add_buf( pBuf, "\n\r" );

    if ( !found )
	add_buf( pBuf, "No objects found in that range.\n\r" );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_olocate( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    int			index;
    int			loc;
    int			minlevel = 0;
    int			maxlevel = MAX_LEVEL;
    char		arg1[ MAX_INPUT_LENGTH ];
    char		arg2[ MAX_INPUT_LENGTH ];
    bool		found = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Syntax: olocate wear_loc [min_level] [max_level].\n\r", ch );
	return;
    }

    loc = flag_value( wear_flags, arg1 );

    if ( loc & ITEM_TAKE )
	loc = NO_FLAG;

    if ( loc == NO_FLAG )
    {
	send_to_char( "Invalid wear location.\n\r", ch );
	return;
    }

    if ( arg2[ 0 ] != '\0' )
    {
	if ( !is_number( arg2 ) || ( minlevel = atoi( arg2 ) ) < 0 || minlevel > MAX_LEVEL )
	{
	    send_to_char( "Bad minimum level.\n\r", ch );
	    return; 
	}
    }

    if ( argument[ 0 ] != '\0' )
    {
	if ( !is_number( argument ) || ( maxlevel = atoi( argument ) ) < minlevel || maxlevel > MAX_LEVEL )
	{
	    send_to_char( "Bad maximum level.\n\r", ch );
	    return; 
	}
    }

    pBuf = new_buf( );
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	for ( index = pArea->min_vnum; index <= pArea->max_vnum; index++ )
	{
	    if ( ( pObj = get_obj_index( index ) ) == NULL )
		continue;
	    if ( !( pObj->wear_flags & loc ) )
		continue;
	    if ( pObj->level < minlevel || pObj->level > maxlevel )
		continue;
	    if ( !found )
	    {
		add_buf( pBuf, "  Vnum   lvl   Desc\n\r" );
		found = TRUE;
	    }
	    buf_printf( pBuf, "`R[`W%5d`R] [`W%3d`R] `w%s\n\r",
			pObj->vnum,
			pObj->level,
			pObj->short_descr );
	}
    }

    if ( found )
    {
	page_to_char( buf_string( pBuf ), ch );
    }
    else
    {
	send_to_char( "No objects found.\n\r", ch );
    }

    free_buf( pBuf );
    return;
}


void
do_omni( CHAR_DATA *ch, char *argument )
{
    BUFFER *		buf;
    int			cImm;
    int			cMortal;
    DESCRIPTOR_DATA *	d;
    char		tnl[SHORT_STRING_LENGTH];
    CHAR_DATA *		wch;
    CLIST_DATA *	who_char;
    CLIST_DATA *	who_first;
    CLIST_DATA *	who_tmp;

    cImm = 0;
    cMortal = 0;
    who_first = NULL;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING )
	    continue;
	wch = d->original != NULL ? d->original : d->character;
	if ( !can_see( ch, wch ) )
	    continue;

	who_char = new_clist_data( );
	who_char->lch = wch;
	who_char->d = d;

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

	if ( wch->level >= LEVEL_IMMORTAL )
	    cImm++;
	else
	    cMortal++;
    }

    buf = new_buf( );

    if ( cImm != 0 )
    {
	add_buf( buf, " `Y----Immortals:----\n\r" );
	add_buf( buf, "`GName       `RLevel Wiz `CIncog `G[  `WRoom`G] Idle  `WBuilding\n\r" );

	for ( who_char = who_first; who_char != NULL; who_char = who_char->next )
	{
	    wch = who_char->lch;
	    if ( wch->level < LEVEL_IMMORTAL )
		continue;

	    buf_printf( buf, "`G%-12s `R%3d %3d  `C%3d  `G[`W%6d`G] %4d%s`W%s`w%s\n\r",
			wch->name,
			wch->level,
			wch->invis_level,
			wch->incog_level,
			wch->in_room ? wch->in_room->vnum : 0,
			who_char->d->idle,
			who_char->d->pString != NULL ? "`CE" :
			    IS_SET( wch->comm, COMM_AFK ) ? "`WA" : " ",
			olc_ed_name( who_char->d->character ),
			olc_ed_vnum( who_char->d->character ) );
	}
    }

    if ( cMortal != 0 )
    {
	add_buf( buf, "\n\r `Y----Mortals:----\n\r" );
	add_buf( buf, "`GName          `RRace`G/`cClass `CPosition `BLev `O%hp `G[  `WRoom`G]   `YTnL `GIdle`X\n\r" );

	for ( who_char = who_first; who_char != NULL; who_char = who_char->next )
	{
	    wch = who_char->lch;
	    if ( wch->level >= LEVEL_IMMORTAL )
		continue;
	    if ( wch->level == LEVEL_HERO )
		strcpy( tnl, "---" );
	    else
		sprintf( tnl, "%d", ( wch->level + 1 )
		      * exp_per_level( wch, wch->pcdata->points ) - wch->exp );


	    buf_printf( buf, "`G%-12s `R%5s`G/`c%s   `C%-8s `B%3d `O%3d `G[`W%6d`G] `Y%5s `G%4d%s\n\r",
			wch->name,
			race_table[wch->race].who_name,
			class_table[wch->class].who_name,
			capitalize( flag_string( position_types, wch->position ) ),
			wch->level,
			wch->hit > 0 && wch->max_hit > 0 ? 100 * wch->hit / wch->max_hit : 0,
			wch->in_room != NULL ? wch->in_room->vnum : 0,
			tnl,
			who_char->d->idle,
                        IS_SET( wch->comm, COMM_AFK ) ? "`WA" : ""  );
	}
    }

    buf_printf( buf, "\n\r`W%2d `PImmortal%s\n\r", cImm, cImm == 1 ? "" : "s" );
    buf_printf( buf, "`W%2d `PMortal%s\n\r", cMortal, cMortal == 1 ? "" : "s" );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );

    while ( who_first != NULL )
    {
	who_char = who_first;
	who_first = who_first->next;
	free_clist_data( who_char );
    }

    return;
}

/* 
 * Function: int days_since_last_file_mod
 * Descr   : Coubts days since the pfile was last modified
 *           for use with plist.
 */

int days_since_last_file_mod( char *filename )
{
    int days;
    struct stat buf;
    //extern time_t current_time;

    if (!stat(filename, &buf))
    {
        days = (current_time - buf.st_mtime)/86400;
    }
    else
        days = 0;

    return days;
}

/*
 * Function: do_plist
 * Descr   : Lists the player files for use with pload.
 *
 * Returns : Player list
 * Syntax  : plist all | immortal
 * Written : v1.0b 2/2020
 * Author  : Praesius <ro@wr3tch.com>
 */

void do_plist( CHAR_DATA *ch, char *argument )
{
    DIR *dp;
    struct dirent *ep;
    char buf[80];
    char buffer[MAX_STRING_LENGTH*4];
    int days;
    bool fAll = TRUE, fImmortal = FALSE;

    buffer[0] = '\0';

    if ( argument[0] == '\0' || !str_cmp( argument, "all" ) )
    {
     	fAll = TRUE;
    }
    else
    if ( !str_prefix( argument, "immortal" ) )
    {
     	fImmortal = TRUE;
    }
    else
    {
     	send_to_char( "Syntax: PLIST [ALL/IMMORTAL]\n\r", ch );
        return;
    }

    if ( fImmortal )
    {
     	/*
         * change the string literals to GODS_DIR or where
         * you keep your gods folder
         */
	dp = opendir ("../gods");
    }
    else
    {
     	/*
         * change the string literals to PLAYER_DIR or where
         * you keep your gods folder
         */
        dp = opendir ("../player");
    }

    if (dp != NULL)
    {
        while ( (ep = readdir (dp)) )
        {
            if ( ep->d_name[0] == '.' )
                continue;

        /*
         * change the string literals to fit your needs like above
         */
            sprintf( buf, "%s%s", fImmortal ? "../gods/" : "../player/", ep->d_name );

            days = days_since_last_file_mod( buf );

        /*
         * color coding using Lopes comaptable color to highlight
         * inactivity. green = active, red = innactive
         * Just remove the colors if they cause problems.
         */
            sprintf( buf, "%-15s %s%-3d`X days\n\r",
                ep->d_name, days > 30 ? "`r" : days > 20 ? "`Y" : days > 10 ? "`g" : "`G", days );
            strcat( buffer, buf );
        }
        closedir (dp);
    }
    else
        perror ("Couldn't open the directory");

    page_to_char( buffer, ch );

  return;
}


/*
 * Function: do_pload
 * Descr   : Loads a player object into the mud, bringing them (and their
 *           pet) to you for easy modification.  Player must not be connected.
 *           Note: be sure to send them back when your done with them.
 * Returns : (void)
 * Syntax  : pload (who)
 * Written : v1.0 12/97
 * Author  : Gary McNickle <gary@dharvest.com>
 */
void
do_pload( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA	d;
    bool		isChar;
    char		name[MAX_INPUT_LENGTH];
    CHAR_DATA *		pch;
    USERLIST_DATA *	pUser;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Load who?\n\r", ch );
        return;
    }

    argument[0] = UPPER(argument[0]);
    argument = one_argument(argument, name);

    /* Dont want to load a second copy of a player who's already online! */
    if ( get_char_world( ch, name ) != NULL )
    {
	send_to_char( "That person is already connected!\n\r", ch );
	return;
    }

    isChar = load_char_obj( &d, name ); /* char pfile exists? */

    if ( !isChar )
    {
	send_to_char( "Load Who?  Are you sure?  I cant seem to find them.\n\r", ch );
	return;
    }

    pch		= d.character;
    pch->desc   = NULL;
    pch->next   = char_list;
    char_list	= pch;
    d.connected	= CON_PLAYING;
    reset_char( pch );
    SET_BIT( pch->act2, PLR_PLOADED );

    /* bring player to imm */
    pch->pcdata->pload_room = pch->room_vnum;
    char_to_room( pch, ch->in_room ); /* put in room imm is in */

    act_color( AT_IMM, "You have pulled $N from the pattern!",
	       ch, NULL, pch, TO_CHAR, POS_DEAD );
    act_color( AT_IMM, "$n has pulled $N from the pattern!",
	       ch, NULL, pch, TO_ROOM, POS_RESTING );

    if ( pch->pet != NULL )
    {
	char_to_room( pch->pet, pch->in_room );
	act( "$n has entered the game.", pch->pet, NULL, NULL, TO_ROOM );
    }

    if ( pch->mount != NULL )
    {
	char_to_room( pch->mount, pch->in_room );
	act( "$n has entered the game.", pch->mount, NULL, NULL, TO_ROOM );
    }

    if ( ( pUser = user_lookup( pch->name ) ) != NULL && pUser->played == 0 )
        update_userlist( pch, FALSE );

}


void
do_project( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;
    BUFFER *		buf;
    char		arg[MAX_INPUT_LENGTH];
    bool		found = FALSE;
    int			lvnum;
    int			uvnum;
    int			vnum;

    argument = one_argument( argument, arg );

    if ( ( arg[0] == '\0' ) || ( is_number( arg ) && is_number( argument ) ) )
    {
	buf = new_buf( );
	found = FALSE;
	if ( arg[0] == '\0' )
	{
	    lvnum = 0;
	    uvnum = 0;
	    for ( pProj = project_list; pProj != NULL; pProj = pProj->next )
		uvnum = UMAX( uvnum, pProj->vnum );
	}
	else
	{
	    lvnum = atoi( arg );
	    uvnum = atoi( argument );
	}
	for ( pProj = project_list; pProj != NULL; pProj = pProj->next )
	{
	    if ( pProj->deleted && !can_aedit_all( ch, NULL ) )
		continue;
	    if ( pProj->vnum < lvnum || pProj->vnum > uvnum )
		continue;
	    if ( !found )
	    {
		found = TRUE;
		add_buf( buf, "Num  Due date  [   Assigned ]  Pri [    Status  ] Name\n\r" );
	    }
	    buf_printf( buf, "%2d]%c%s [%12.12s] %4.4s [%12.12s] %s\n\r",
			pProj->vnum,
			pProj->deleted ? '*' : ' ',
			date_string( pProj->deadline ),
			pProj->assignee,
			flag_string( priority_flags, pProj->priority ),
			pProj->status,
			pProj->name );
	}
	if ( !found )
	    add_buf( buf, "No projects found.\n\r" );
	page_to_char( buf_string( buf ), ch );
	free_buf( buf );
	return;
    }

    if ( is_number( arg ) )
    {
	vnum = atoi( arg );
	for ( pProj = project_list; pProj != NULL; pProj = pProj->next )
	{
	    if ( pProj->deleted && !can_aedit_all( ch, NULL ) )
		continue;
	    if ( pProj->vnum == vnum )
	    {
		show_project( ch, pProj );
		return;
	    }
	}
	send_to_char( "Project not found.\n\r", ch );
	return;
    }

    for ( pProj = project_list; pProj != NULL; pProj = pProj->next )
    {
	if ( pProj->deleted && !can_aedit_all( ch, NULL ) )
	    continue;
	if ( !str_prefix( arg, pProj->name )
	||   is_exact_name( arg, pProj->assignee ) )
	{
	    show_project( ch, pProj );
	    return;
	}
    }

    send_to_char( "Project not found.\n\r", ch );
    return;
}


/*
 * Function: do_punload
 * Descr   : Returns a player, previously 'ploaded' back to the void from
 *           whence they came.  This does not work if the player is actually 
 *           connected.
 * Returns : (void)
 * Syntax  : punload (who)
 * Written : v1.0 12/97
 * Author  : Gary McNickle <gary@dharvest.com>
 */
void
do_punload( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	orig_room;
    CHAR_DATA *		victim;
    char		who[MAX_INPUT_LENGTH];

    argument = one_argument( argument, who );

    if ( ( victim = get_char_world( ch, who ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    /*
     * Person is legitimately logged on... was not ploaded.
     */
    if ( !IS_SET( victim->act2, PLR_PLOADED ) )
    {
	send_to_char( "I dont think that would be a good idea...\n\r", ch );
	return;
    }

    orig_room = get_vroom_index( victim->pcdata->pload_room );
    if ( orig_room != NULL ) /* return player and pet to orig room */
    {
	char_from_room( victim );
	char_to_room( victim, orig_room );
	if ( victim->pet != NULL )
	{
	    char_from_room( victim->pet );
	    char_to_room( victim->pet, victim->in_room );
	}
	if ( victim->mount != NULL )
	{
	    char_from_room( victim->mount );
	    char_to_room( victim->mount, victim->in_room );
	}
    }

    act_color( AT_IMM, "You have released $N back to the Pattern.",
	  ch, NULL, victim, TO_CHAR, POS_DEAD );
    act_color( AT_IMM, "$n has released $N back to the Pattern.",
	  ch, NULL, victim, TO_ROOM, POS_RESTING );

    do_quit( victim, "" );
}


void
do_racestat( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    char	arg[MAX_INPUT_LENGTH];
    BUFFER *	buf;
    bool	found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Stat which race?\n\r", ch );
	return;
    }

    found = FALSE;
    for ( iRace = 0; !IS_NULLSTR( race_table[iRace].name ); iRace++ )
    {
	if ( !str_prefix( arg, race_table[iRace].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
    {
	for ( iRace = 0; !IS_NULLSTR( race_table[iRace].name ); iRace++ )
	{
	    if ( !str_prefix( arg, race_table[iRace].who_name ) )
	    {
		found = TRUE;
		break;
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "No such race.\n\r", ch );
	return;
    }

    buf = new_buf( );
    show_race_info( iRace, buf );
    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


void
do_rdesc( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    BUFFER *		pBuf;
    char		dtype;
    const char *	dname;
    int			hour;
    bool		mdesc;
    bool		adesc;
    bool		edesc;
    bool		ndesc;
    char *		rdesc;

    if ( ( pRoom = ch->in_room ) == NULL )
    {
        send_to_char( "You're not in a room!\n\r", ch );
        return;
    }

    mdesc = !IS_NULLSTR( pRoom->morning_desc );
    adesc = !IS_NULLSTR( pRoom->description );
    edesc = !IS_NULLSTR( pRoom->evening_desc );
    ndesc = !IS_NULLSTR( pRoom->night_desc );

    if ( !mdesc && !adesc && !edesc && !ndesc )
    {
        send_to_char( "The room has no descriptions!\n\r", ch );
        return;
    }

    if ( !is_number( argument ) )
    {
        if ( *argument == '\0' )
        {
            send_to_char( "Descriptions found:\n\r", ch );
            if ( mdesc )
                send_to_char( "    Morning\n\r", ch );
            if ( adesc )
                send_to_char( "    Afternoon (normal)\n\r", ch );
            if ( edesc )
                send_to_char( "    Evening\n\r", ch );
            if ( ndesc )
                send_to_char( "    Night\n\r", ch );
            return;
        }

        dtype = '0';
        if ( str_match( argument, "m", "morning" )
        ||   str_match( argument, "m", "mdesc" ) )
            dtype = 'm';
        else if ( str_match( argument, "a", "afternoon" )
             ||   str_match( argument, "a", "adesc" )
             ||   str_match( argument, "no", "normal" )
             ||   str_match( argument, "d", "description" ) )
            dtype = 'a';
        else if ( str_match( argument, "e", "evening" )
             ||   str_match( argument, "e", "edesc" ) )
            dtype = 'e';
        else if ( str_match( argument, "n", "night" )
             ||    str_match( argument, "n", "night" ) )
            dtype = 'n';
        else
        {
            send_to_char( "No such room description type.\n\r", ch );
            return;
        }

        switch( dtype )
        {
            case 'm':
                if ( !mdesc )
                    send_to_char( "The room has no morning description.\n\r", ch );
                else
                    page_to_char( pRoom->morning_desc, ch );
                break;
            case 'a':
                if ( !adesc )
                    send_to_char( "The room has no afternoon (normal) description.\n\r", ch );
                else
                    page_to_char( pRoom->description, ch );
                break;
            case 'e':
                if ( !edesc )
                    send_to_char( "The room has no evening description.\n\r", ch );
                else
                    page_to_char( pRoom->evening_desc, ch );
                break;
            case 'n':
                if ( !ndesc )
                    send_to_char( "The room has no night description.\n\r", ch );
                else
                    page_to_char( pRoom->night_desc, ch );
                break;
        }
        return;
    }

    hour = atoi( argument );
    if ( hour < 0 || hour > 23 )
    {
        ch_printf( ch, "Bad hour: %d\n\r", hour );
        return;
    }

    rdesc = ch->in_room->description;
    dname = "afternoon (normal)";
    if ( ( hour < hour_sunrise || hour >= hour_sunset + 1 ) && ndesc )
    {
	rdesc = ch->in_room->night_desc;
	dname = "night";
    }
    if ( hour >= hour_sunrise && hour < 10 && mdesc )
    {
	rdesc = ch->in_room->morning_desc;
	dname = "morning";
    }
    if ( hour >=17 && hour <= 20  && edesc )
    {
	rdesc = ch->in_room->evening_desc;
	dname = "evening";
    }	

    pBuf = new_buf( );
    buf_printf( pBuf, "Room #%d %s description:\n\r", pRoom->vnum, dname );
    add_buf( pBuf, rdesc );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_rebuild( CHAR_DATA *ch, char *argument )
{
    int			cvs_pid;
    char		dbuf[MAX_INPUT_LENGTH];
    char *		dchar;
    DIR *		dp;
    struct dirent *	dentry;
    FILE *		logfile;
    pid_t		make_pid;
    char *		ochar;
    char *		oname;
    char *		p;
    struct stat		statbuf;
    int			status;

    stat( boot_file, &statbuf );
    send_to_char( ctime( &statbuf.st_mtime ), ch );

    if ( rebuild.pid != 0 )
    {
	send_to_char( "A build is already in progress.\n\r", ch );
	return;
    }

    rebuild.pid = fork( );
    if ( rebuild.pid == 0 )
    {
	/* child process */
	if ( chdir( SOURCE_DIR ) != 0 )
	{
	    perror( "Do_rebuild: chdir:" );
	    exit( EXIT_FAILURE );
	}

	/* set up log file */
	if ( ( logfile = fopen( LOG_DIR REBUILD_LOG, "w" ) ) != NULL )
	{
	    dup2( fileno( logfile ), fileno( stdout ) );
	    dup2( fileno( logfile ), fileno( stderr ) );
	    fclose( logfile );
	}

	/* Perform CVS update */
	cvs_pid = fork( );
	if ( cvs_pid == 0 )
	{
	    execlp( "cvs", "cvs", "update", (char *)NULL );
	    exit( EXIT_FAILURE );
	}
	else if ( cvs_pid != -1 )
	{
	    waitpid( cvs_pid, &status, 0 );
	    if ( status != 0 )
		exit( EXIT_FAILURE );
	}
	else
	    exit( EXIT_FAILURE );

	/* Rename binary and remove object files */
	oname = stpcpy( dbuf, OBJ_DIR );
	if ( ( dp = opendir( OBJ_DIR ) ) == NULL )
	{
	    perror( "Do_rebuild: opendir" );
	    exit( EXIT_FAILURE );
	}

	while ( ( dentry = readdir( dp ) ) != NULL )
	{
	    strcpy( oname, dentry->d_name );
	    /* Find files that end with ".o" */
	    ochar = strrchr( dbuf, 'o' );
	    dchar = strrchr( dbuf, '.' );
	    p = oname + strlen( oname );
	    if ( ochar != NULL
	    &&	 dchar != NULL
	    &&	 ochar == p - 1
	    &&	 dchar == p - 2 )
	    {
		unlink( dbuf );
	    }
	}
	closedir( dp );

	/* Compile mud */
	make_pid = fork( );
	if ( make_pid == 0 )
	{
	    execlp( "make", "make", (char *)NULL );
	    exit( EXIT_FAILURE );
	}
	else if ( make_pid != -1 )
	{
	    waitpid( make_pid, &status, 0 );
	    if ( status != 0 )
		exit( EXIT_FAILURE );
	    else
		exit( EXIT_SUCCESS );
	}
	else
	{
	    exit( EXIT_FAILURE );
	}
	exit( EXIT_SUCCESS );
    }
    else if ( rebuild.pid != -1 )
    {
	/* Parent process */
	rebuild.time = current_time;
	ch_printf( ch, "Rebuild commenced with process ID %d.\n\r", rebuild.pid );
	return;
    }
    else
    {
	send_to_char( "Fork failed.\n\r", ch );
	return;
    }
}


void
do_recent( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char *s;
    FILE *fp;
    int   lvl;
    int   i;

    if ( ( fp = fopen( SYSTEM_DIR RECENT_FILE, "r" ) ) != NULL )
    {
	i = 0;
	while( fgets( buf, sizeof( buf ), fp ) != NULL )
	{
	    if ( ( s = strchr( buf, '\n' ) ) != NULL )
		strcpy( s + 1, "\r" );
	    else
		strcat( buf, "\n\r" );
	    s = buf;
	    while( isspace( *s ) )
		s++;
	    lvl = atoi( s );
	    while( isdigit( *s ) )
		s++;
	    while( isspace( *s ) )
		s++;
	    if ( lvl <= get_trust( ch ) )
		send_to_char( s, ch );
	    i++;
	    if ( i >= MAX_RECENT_LINES )
		break;
	}
	fclose( fp );
    }


}


void
do_rename_char( CHAR_DATA* ch, char* argument )
{
    char		old_name[MAX_INPUT_LENGTH];
    char		new_name[MAX_INPUT_LENGTH];
    char		strsave [MAX_INPUT_LENGTH];
    CHAR_DATA *		victim;
    CHAR_DATA *		vch;
    FILE *		file;
    USERLIST_DATA *	pTmp;
    USERLIST_DATA *	pUser;

    argument = one_argument( argument, old_name );
    first_arg( argument, new_name, FALSE );

    if ( old_name[0] == '\0' || new_name[0] == '\0' )
    {
	send_to_char( "Rename who to what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, old_name ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Use MEdit or mset to rename mobiles.\n\r", ch );
	return;
    }

    /* allow rename self new_name, but otherwise only lower level */
    if ( victim != ch && get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You cannot.\n\r", ch );
	return;
    }

    if ( victim->desc != NULL && victim->desc->connected != CON_PLAYING )
    {
	send_to_char( "Wait until they've finished logging in.\n\r", ch );
	return;
    }

    /* Might have to check for clan here. */

    if ( !check_parse_name( new_name ) || !check_legal_name( new_name ) )
    {
	send_to_char( "The new name is illegal.\n\r", ch );
	return;
    }

    /* First, check if there is a player named that off-line */
    new_name[0] = UPPER( new_name[0] );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( new_name ) );

    fclose( fpReserve );
    file = fopen( strsave, "r" ); /* attempt to to open pfile */
    if ( file != NULL )
    {
	send_to_char( "A player with that name already exists.\n\r", ch );
	fclose( file );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }
    fpReserve = fopen( NULL_FILE, "r" );  /* reopen the extra file */

    /* Check .gz file ! */
    sprintf( strsave, "%s%s.gz", PLAYER_DIR, capitalize( new_name ) );

    fclose( fpReserve );
    file = fopen( strsave, "r" );
    if ( file != NULL )
    {
	send_to_char( "A player with that name already exists.\n\r", ch );
	fclose( file );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }
    fpReserve = fopen( NULL_FILE, "r" ); 

    for ( vch = char_list; vch != NULL; vch = vch->next )
    {
	if ( !str_cmp( vch->name, new_name ) )
	{
	    send_to_char ( "A player with that name already exists.\n\r", ch );
	    return;
	}
    }

    /* Save the filename of the old name */
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
    strcpy( old_name, victim->name );

    /* Rename the character and save them to a new file */
    free_string( victim->name );
    sql_update_name( old_name, new_name );
    victim->name = str_dup( capitalize( new_name ) );
    save_char_obj( victim );

    /* unlink the old file */
    unlink( strsave );

    /* Rename them in userlist too */
    for ( pUser = user_first; pUser != NULL; pUser = pUser->next )
	if ( !str_cmp( pUser->name, old_name ) )
	    break;

    if ( pUser != NULL )
    {
	free_string( pUser->name );
	pUser->name = str_dup( victim->name );

	if ( user_first == pUser )
	{
	    user_first = pUser->next;
	    if ( user_last == pUser )
		user_last = NULL;
	}
	else
	{
	    for ( pTmp = user_first; pTmp != NULL; pTmp = pTmp->next )
	    {
		if ( pTmp->next == pUser )
		{
		    pTmp->next = pUser->next;
		    if ( user_last == pUser )
			user_last = pTmp;
		    break;
		}
	    }
	}

	if ( user_first == NULL
	||   strcasecmp( pUser->name, user_first->name ) < 0 )
	{
	    pUser->next = user_first;
	    user_first = pUser;
	    if ( user_last == NULL )
	    {
		user_last = pUser;
	    }
	}
	else if ( strcasecmp( pUser->name, user_last->name ) > 0 )
	{
	    user_last->next = pUser;
	    pUser->next = NULL;
	    user_last = pUser;
	}
	else
	{
	    for ( pTmp = user_first; pTmp != NULL; pTmp = pTmp->next )
	    {
		if ( strcasecmp( pUser->name, pTmp->next->name ) < 0 )
		{
		    pUser->next = pTmp->next;
		    pTmp->next = pUser;
		    break;
		}
	    }
	}

	queue_userlist_save( );
    }

    send_to_char( "Player renamed.\n\r", ch );
    act_color( AT_IMM, "$N has renamed you to $n!",
	       victim, NULL, ch, TO_CHAR, POS_DEAD );
    return;
}


void
do_rfind( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    BUFFER *		pBuf;
    char		buf[MAX_INPUT_LENGTH];
    bool		fFound;

    if ( *argument == '\0' )
    {
	send_to_char( "Rfind what?\n\r", ch );
	return;
    }

    fFound = FALSE;
    pBuf = new_buf( );

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	for ( pRoom = pArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
	{
            strip_color( buf, pRoom->name );
            if ( is_name( argument, buf ) )
            {
                buf_printf( pBuf, "[%5d]  %s`X\n\r", pRoom->vnum, pRoom->name );
                fFound = TRUE;
            }
	}
    }

    if ( fFound )
	page_to_char( buf_string( pBuf ), ch );
    else
	send_to_char( "Room not found.\n\r", ch );

    free_buf( pBuf );
    return;
}


void
do_rlist( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH];
    int			col;
    int			lvnum;
    int			uvnum;
    int			vnum;
    bool		found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /*
     * Process args.  Syntax allows "." for lower or upper vnum of current
     * area.  If no second arg, highest vnum of area containing first
     * arg is assumed.
     */
    if ( arg1[0] == '\0' || !str_cmp( arg1, "." ) )
    {
	lvnum = ch->in_room->area->min_vnum;
    }
    else
    {
	lvnum = atoi( arg1 );
    }

    if ( arg2[0] == '\0' )
    {
	if ( ( pArea = get_area( lvnum ) ) == NULL )
	{
	    send_to_char( "No areas have that vnum.\n\r", ch );
	    return;
	}
	uvnum = pArea->max_vnum;
    }
    else if ( !str_cmp( arg2, "." ) )
    {
	uvnum = ch->in_room->area->max_vnum;
    }
    else
    {
	uvnum = atoi( arg2 );
    }

    /*
     * Check args
     */
    if ( lvnum == 0 || uvnum == 0 )
    {
	send_to_char( "Syntax: rlist <lvnum> [<uvnum>]\n\r", ch );
	return;
    }
    if ( lvnum < 1 || uvnum < lvnum )
    {
	send_to_char( "Invalid vnum range.\n\r", ch );
	return;
    }

    found = FALSE;
    pBuf = new_buf( );
    col = 0;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( pArea->min_vnum > uvnum || pArea->max_vnum < lvnum )
	    continue;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
	    if ( vnum < lvnum || vnum > uvnum )
		continue;
	    if ( ( pRoom = get_room_index( vnum ) ) )
	    {
		found = TRUE;
		strip_color( buf, pRoom->name );
		buf_printf( pBuf, "`R[`W%5d`R]%s`w%-17.16s", vnum,
			    !IS_SET( pRoom->area->area_flags, AREA_NO_UNFINISHED )
			    && IS_SET( pRoom->room_flags, ROOM_UNFINISHED )
			    ? "`B*" : " ",
			    buf );
		if ( ++col % 3 == 0 )
		    add_buf( pBuf, "\n\r" );
	    }
	}
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
do_seclist( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    bitvector	bit;
    bitvector	bits;
    BUFFER *	buf;
    int		cCmd;
    int		col;
    int		count;
    char	flag_buf[SHORT_STRING_LENGTH];
    bool	found;
    int		iCmd;
    int		index;
    CHAR_DATA *	rch;

    if ( ch->desc == NULL )
	return;
    else
	rch = ( ch->desc->original != NULL
		? ch->desc->original : ch->desc->character );

    if ( rch->pcdata == NULL )
    {
	send_to_char( "Security groups are of no concern to you.\n\r", ch );
	return;
    }

    buf = new_buf( );

    /* Find out what group(s) to display */
    bits = 0;
    count = 0;
    if ( *argument == '\0' )
    {
	for ( index = 0; security_flags[index].name != NULL; index++ )
	{
	    if ( get_trust( rch ) >= MAX_LEVEL
	    ||	 IS_SET( rch->pcdata->sec_groups, security_flags[index].bit ) )
	    {
		bits |= security_flags[index].bit;
		count++;
	    }
	}
    }
    else
    {
	while ( *argument != '\0' )
	{
	    argument = one_argument( argument, arg );
	    if ( ( bit = flag_value( security_flags, arg ) ) != NO_FLAG
	    &&	 ( IS_SET( rch->pcdata->sec_groups, bit )
		   || get_trust( rch ) >= MAX_LEVEL ) )
	    {
		count++;
		bits |= bit;
	    }
	}
    }

    if ( count == 0 )
    {
	add_buf( buf, "No groups found.\n\r" );
    }
    else if ( count == 1 )
    {
	/* if only one group specified, bits should only have one bit set. */
	col = 0;
	found = FALSE;
	buf_printf( buf, "Commands for %s:\n\r",
		    flag_string( security_flags, bits ) );
	for ( iCmd = 0; !IS_NULLSTR( cmd_table[iCmd].name ); iCmd++ )
	{
	    if ( IS_SET( cmd_table[iCmd].sec_flags, bits ) )
	    {
		found = TRUE;
		buf_printf( buf, "%-12s", cmd_table[iCmd].name );
		if ( ++col % 6 == 0 )
		    add_buf( buf, "\n\r" );
	    }
	}
	if ( col % 6 != 0 )
	    add_buf( buf, "\n\r" );
	if ( !found )
	    add_buf( buf, "(none)\n\r" );
    }
    else
    {
	add_buf( buf, "Flg Group                Cmds\n\r" );
	add_buf( buf, "=============================\n\r" );
	for ( index = 0; security_flags[index].name != NULL; index++ )
	{
	    bit = security_flags[index].bit;
	    if ( bit == 0 || !IS_SET( bits, bit ) )
		continue;
	    strcpy( flag_buf, print_flags( bit ) );
	    if ( flag_buf[0] >= 'a' )
		sprintf( flag_buf + strlen( flag_buf ), "%c", flag_buf[0] );
	    cCmd = 0;
	    for ( iCmd = 0; !IS_NULLSTR( cmd_table[iCmd].name ); iCmd++ )
		if ( IS_SET( cmd_table[iCmd].sec_flags, bit ) )
		    cCmd++;
	    buf_printf( buf, " %-2s %-20s %4d\n\r",
			flag_buf,
			security_flags[index].name,
			cCmd );
	}
    }

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


void
do_seize( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	obj;
    CHAR_DATA *	victim;
    bool	found;
    BANK_DATA *	bank;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Seize what from whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
	send_to_char( "Seize from yourself?!?\n\r", ch );
	return;
    }

    /* Seize only from lower level chars */
    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You cannot.\n\r", ch );
        return;
    }

    found = FALSE;
    while ( ( obj = get_obj_carry( victim, arg1, ch ) ) != NULL )
    {
	found = TRUE;
	obj_from_char( obj );
	obj_to_char( obj, ch );
	act_color( AT_IMM, "You seize $p from $N.",
		   ch, obj, victim, TO_CHAR, POS_DEAD );
    }

    while ( ( obj = get_obj_wear( victim, arg1 ) ) != NULL
    &&	    can_see_obj( ch, obj ) )
    {
	found = TRUE;
	obj_from_char( obj );
	obj_to_char( obj, ch );
	act_color( AT_IMM, "You seize $p from $N.",
		   ch, obj, victim, TO_CHAR, POS_DEAD );
    }

    if ( !IS_NPC( victim ) )
    {
	for ( bank = victim->pcdata->bank; bank != NULL; bank = bank->next )
	{
	    while ( ( obj = get_obj_bank( ch, bank, arg1 ) ) != NULL )
	    {
		found = TRUE;
		obj_from_bank( obj );
		obj_to_char( obj, ch );
		act_color( AT_IMM, "You seize $p from $N.",
			   ch, obj, victim, TO_CHAR, POS_DEAD );
	    }
	}
    }

    if ( found )
    	save_char_obj( victim ); 
    else
	send_to_char( "They're not carrying it.\n\r", ch );

    return;
}

/*----------------------------------------------------*/
/* This is the Rip code, for Rom based muds. Rip a    */
/* player in half, keep their legs, and they die a    */
/* minute later, on tick.                             */
/* -Ferric@uwyo.edu				      */
/*----------------------------------------------------*/

/*----------------------------------------------------*/
/* All standard Diku/Merc/Rom copyrights, for the code*/
/* mentioned above, apply. This file freely           */
/* distributable so long as these headers  remain     */
/* intact.                                            */
/*----------------------------------------------------*/

void do_sever( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *	mount;
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
       send_to_char("Who are you going to rip in half?\n\r",ch);
       return; 
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL ) 
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if( victim == ch )
    {
	send_to_char("You reach your neck fine, but cant' get your ankles.\n\r",ch);
	return;
    }

    if ( xIS_SET( victim->affected_by, AFF_SEVERED ) )
    {
	send_to_char( "They have no legs to rip off.\n\r", ch );
	return;
    }

    stop_fighting( victim, TRUE );

    xSET_BIT( victim->affected_by, AFF_SEVERED );
    if( !IS_NPC( victim ) )
        act_color( AT_MAGIC, "$n picks you up and rips you in half! Oh no!", ch, NULL, victim, TO_VICT, POS_RESTING );
    act_color( AT_MAGIC, "$n picks up $N and rips $S legs off!", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
    send_to_char( "You rip them in half!\n\r", ch );

    if ( ( mount = MOUNTED( victim ) ) != NULL )
    {
	victim->riding = FALSE;
	mount->riding = FALSE;
    }

    obj = create_object( get_obj_index( OBJ_VNUM_LEGS ), 0 );

    obj->timer = 5;


    if ( IS_NPC( victim ) )
    sprintf( buf, "A pair of %s's legs are here, twitching.", victim->short_descr );
	else
    sprintf( buf, "A pair of %s's legs are here, twitching.", victim->name );	
    free_string(obj->description);
    obj->description = str_dup( buf );


    if( IS_NPC( victim ) )
    sprintf( buf, "A pair of %s's legs", victim->short_descr );
	else
    sprintf( buf, "A pair of %s's legs", victim->name );
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );

    obj_to_char( obj, ch );

}


void
do_smite( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    char *	p;
    OBJ_DATA *	shoes;
    CHAR_DATA *	victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Smite whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "No, I don't think so.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && get_trust( victim ) > get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	act_color( AT_IMM, "$n tried to smite you!", ch, NULL, victim, TO_VICT, POS_DEAD );
	return;
    }

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );

    p = buf;
    p = stpcpy( p, "`R       *     `WW     W   H  H   AA    M   M   !!   `R  *       \n\r");
    p = stpcpy( p, "`R     *****   `W W W W    HHHH  AAAA   M M M   !!   `R*****     \n\r");
    p = stpcpy( p, "`R       *     `W W   W    H  H  A  A  M     M  !!   `R  *       \n\r");
    p = stpcpy( p, "\n\r" );
    send_to_char( buf, victim );

    act_color( AT_IMM, "$n raises $s hand and smites you!", ch, NULL, victim, TO_VICT, POS_DEAD );
    act_color( AT_IMM, "$n raises $s hand and smites $N!", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
    act_color( AT_IMM, "You raise your hand and smite $N!", ch, NULL, victim, TO_CHAR, POS_DEAD );

    if ( victim->hit >= 2 )
	victim->hit /= 2;

    if ( ( shoes = get_eq_char( victim, WEAR_FEET ) ) != NULL )
    {
	obj_from_char( shoes );
	act_color( AT_IMM, "You are blown out of your shoes and right onto your butt!", ch, NULL, victim, TO_VICT, POS_DEAD );
	act_color( AT_IMM, "$N is blown out of $S shoes and right onto $S butt!", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	act_color( AT_IMM, "$N is blown out of $S shoes and right onto $S butt!", ch, NULL, victim, TO_CHAR, POS_DEAD );
	obj_to_room( shoes, victim->in_room );
	add_obj_fall_event( shoes );
    }
    else
    {
        act_color( AT_IMM, "You are knocked on your butt!", ch, NULL, victim, TO_VICT, POS_DEAD );
        act_color( AT_IMM, "$N is knocked on $S butt!", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
        act_color( AT_IMM, "$N is knocked on $S butt!", ch, NULL, victim, TO_CHAR, POS_DEAD );
    }

    victim->riding = FALSE;
    if ( victim->mount != NULL )
        victim->mount->riding = FALSE;

    victim->position = POS_RESTING;
    return;
}


void
do_socstat( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;
    char 	 buf[MAX_STRING_LENGTH];
    char *	 p;

    if ( *argument == '\0' )
    {
	send_to_char( "Which social would you like to see?\n\r", ch );
	return;
    }

    for ( pSocial = social_first; pSocial; pSocial = pSocial->next )
    {
	if ( !str_prefix( argument, pSocial->name ) )
	{
	    p = buf;
	    p += sprintf( p, "Name   : %s`X\n\r", pSocial->name    );
	    p += sprintf( p, "cnoarg : %s`X\n\r", pSocial->cnoarg  );
	    p += sprintf( p, "onoarg : %s`X\n\r", pSocial->onoarg  );
	    p += sprintf( p, "cfound : %s`X\n\r", pSocial->cfound  );
	    p += sprintf( p, "ofound : %s`X\n\r", pSocial->ofound  );
	    p += sprintf( p, "vfound : %s`X\n\r", pSocial->vfound  );
	    p += sprintf( p, "nfound : %s`X\n\r", pSocial->nfound	 );
	    p += sprintf( p, "cself  : %s`X\n\r", pSocial->cself   );
	    p += sprintf( p, "oself  : %s`X\n\r", pSocial->oself   );
	    if ( pSocial->deleted )
	    {
		p = stpcpy( p, "\n\rThis social is scheduled to be deleted\n\r" );
	    }

	    send_to_char( buf, ch );
	    return;
	}
    }

    send_to_char( "No such social exists.\n\r", ch );
    return;
}


void
do_todo( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char *	arglist;
    char	buf[MAX_STRING_LENGTH];
    char	inbuf[MAX_INPUT_LENGTH*2];
    FILE *	fp;
    FILE *	fp_tmp;
    bool	found;
    int		line;
    int		line_del;
    char *	p;
    char *	s;

    arglist = one_argument( argument, arg );
    line = 0;
    found = FALSE;

    if ( arg[0] == '\0' || ( !str_cmp( arg, "show" ) && *arglist == '\0' ) )
    {
	if ( ( fp = fopen( SYSTEM_DIR TODO_FILE, "r" ) ) == NULL )
	{
	    send_to_char( "Nothing to do!\n\r", ch );
	    return;
	}

	while ( fgets( inbuf, sizeof( inbuf ), fp ) != NULL )
	{
	    found = TRUE;
	    line++;
	    if ( ( s = strchr( inbuf, '\n' ) ) != NULL )
		*s = '\0';
	    p = buf + sprintf( buf, "`W[%d]`w ", line );
	    sprintf( p, "%s\n\r", inbuf );
	    *p = UPPER( *p );
	    p = str_dup( buf );
	    s = format_string( p );
	    send_to_char( s, ch );
	    free_string( s );
	}
	fclose( fp );

	if ( !found )
	    send_to_char( "Nothing to do!\n\r", ch );

	return;
    }

    if ( !str_cmp( arg, "help" ) && *arglist == '\0' )
    {
        send_to_char( "Syntax: todo\n\r", ch );
        send_to_char( "        todo <text>\n\r", ch );
        send_to_char( "        todo delete <number>\n\r", ch );
        send_to_char( "        todo help\n\r", ch );
        send_to_char( "        todo remove <number>\n\r", ch );
        send_to_char( "        todo show\n\r", ch );
        return;
    }

    if ( ( !str_cmp( arg, "remove" ) || !str_cmp( arg, "delete" ) ) && is_number( arglist ) )
    {
	line_del = atoi( arglist );
	if ( line_del < 1 )
	{
	    send_to_char( "Bad line number.\n\r", ch );
	    return;
	}

	if ( ( fp = fopen( SYSTEM_DIR TODO_FILE, "r" ) ) == NULL )
	{
	    send_to_char( "Nothing to remove.\n\r", ch );
	    return;
	}

	if ( ( fp_tmp = fopen( TEMP_DIR TODO_FILE, "w" ) ) == NULL )
	{
	    perror( TEMP_DIR TODO_FILE );
	    send_to_char( "`RERROR`w opening temp file!\n\r", ch );
	    fclose( fp );
	    return;
	}

	while ( fgets( inbuf, sizeof( inbuf ), fp ) != NULL )
	{
	    line++;
	    if ( line == line_del )
		found = TRUE;
	    else
		fputs( inbuf, fp_tmp );
	}
	fclose( fp_tmp );
	fclose( fp );
	if ( found )
	{
	    rename( TEMP_DIR TODO_FILE, SYSTEM_DIR TODO_FILE );
	    send_to_char( "Line removed.\n\r", ch );
	}
	else
	{
	    unlink( TEMP_DIR TODO_FILE );
	    send_to_char( "Line not found.\n\r", ch );
	}

	return;
    }

    /* Add to file */
    smash_tilde( argument );
    if ( ( fp = fopen( SYSTEM_DIR TODO_FILE, "a" ) ) == NULL )
    {
	perror( TODO_FILE );
	send_to_char( "`RERROR`w opening to-do file!\n\r", ch );
	return;
    }

    fprintf( fp, "%s\n", argument );
    fclose( fp );
    send_to_char( "Line added.\n\r", ch );
    return;
}


void
do_ungreet( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *	gch;
    CHAR_DATA *	vch;
    bool *	p;
    int		i;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg2[0] == '\0' )
    {
        send_to_char( "Ungreet whom?\n\r", ch );
        return;
    }

    /* Ungreet all victim */
    if ( !str_cmp( arg1, "all" ) )
    {
        gch = get_char_world( ch, arg2 );
        if ( gch == NULL )
        {
            send_to_char( "They're not here.\n\r", ch );
            return;
        }
        if ( IS_NPC( gch ) )
        {
            send_to_char( "You cannot ungreet mobiles.\n\r", ch );
            return;
        }
        if ( IS_IMMORTAL( gch ) && ch != gch )
        {
            send_to_char( "You cannot ungreet Immortals.\n\r", ch );
            return;
        }
        if ( gch->pcdata->greet_id == 0 )
        {
            act( "Nobody knows $N.", ch, NULL, gch, TO_CHAR );
            return;
        }
        p = greet_table + gch->pcdata->greet_id;
        for ( i = 0; i < greet_size; i++ )
        {
            p += greet_size;
            *p = FALSE;
        }
        update_greet_data( );
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    gch = get_char_world( ch, arg1 );
    if ( gch == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }
    if ( IS_NPC( gch ) )
    {
        send_to_char( "You cannot ungreet mobiles.\n\r", ch );
        return;
    }
    if ( IS_IMMORTAL( gch ) && gch != ch )
    {
        send_to_char( "You cannot ungreet Immortals.\n\r", ch );
        return;
    }
    if ( gch->pcdata->greet_id == 0 )
    {
        act( "$N doesn't know anybody.", ch, NULL, gch, TO_CHAR );
        return;
    }

    /* Ungreet victim all */
    if ( !str_cmp( arg2, "all" ) )
    {
        memset( greet_table + gch->pcdata->greet_id * greet_size + 1, FALSE, greet_size - 1 );
        update_greet_data( );
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    /* Ungreet vict1 vict2 */
    vch = get_char_world( ch, arg2 );
    if ( gch == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }
    if ( IS_NPC( vch ) )
    {
        send_to_char( "You cannot ungreet mobiles.\n\r", ch );
        return;
    }
    if ( IS_IMMORTAL( vch ) && vch != ch )
    {
        send_to_char( "You cannot ungreet Immortals.\n\r", ch );
        return;
    }
    if ( vch->pcdata->greet_id == 0 )
    {
        act( "Nobody knows $N.", ch, NULL, vch, TO_CHAR );
        return;
    }

    update_known( gch, vch, FALSE );
    send_to_char( "Ok.\n\r", ch );
    return;
}


void
do_uptime( CHAR_DATA *ch, char *argument )
{
    char *	p;
    char *	t;
    struct stat stat_info;
    char	buf[MAX_INPUT_LENGTH];

    p = stpcpy( buf, MUD_NAME " boot statistics:\n\r" );
    p = stpcpy( p, "Uptime:   " );
    t = p;
    p = stpcpy( p, str_boot_time );
    if ( ( t = strrchr( t, '\n' ) ) != NULL )
	p = t;
    p = stpcpy( p, ", " );

    p = stpcpy( p, strtimediff( boot_time, current_time ) );
    p = stpcpy( p, ".\n\r" );
    send_to_char( buf, ch );

    p = stpcpy( buf, "Startup:  " );
    t = p;
    p = stpcpy( p, ctime( &startup_time ) );
    if ( ( t = strrchr( t, '\n' ) ) != NULL )
	p = t;
    p = stpcpy( p, ", " );
    p = stpcpy( p, strtimediff( startup_time, current_time ) );
    p = stpcpy( p, " ago.\n\r" );
    send_to_char( buf, ch );

    if ( stat( boot_file, &stat_info ) != 0 )
    {
	send_to_char( "Unable to stat executable.\n\r", ch );
	return;
    }

    p = stpcpy( buf, "Compiled: " );
    t = p;
    p = stpcpy( p, ctime( &stat_info.st_mtime ) );
    if ( ( t = strrchr( t, '\n' ) ) != NULL )
	p = t;
    p = stpcpy( p, ", " );
    p = stpcpy( p, strtimediff( stat_info.st_mtime, current_time ) );
    p = stpcpy( p, " ago.\n\r" );
    send_to_char( buf, ch );

    return;
}


#define USAGE_TABLE	80	/* keep it a multiple of 4 */
void
do_usage( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    char	arg[MAX_INPUT_LENGTH];
    char	buf[SHORT_STRING_LENGTH];
    int		i;
    int		iCount;
    int		index;
    int		j;
    int		l;
    int		list[USAGE_TABLE][2];
    int		usage;
    bool	fMobOnly;
    bool	fNoMob;

    argument = one_argument( argument, arg );

    fMobOnly = str_match( arg, "m",   "mobonly" )
            || str_match( arg, "nop", "noplayers" );
    fNoMob   = str_match( arg, "nom", "nomobiles" )
            || str_match( arg, "p",   "players" );

    pBuf = new_buf( );
    for ( index = 0; index < USAGE_TABLE; index++ )
    {
	list[index][0] = 0;	/* usage */
	list[index][1] = 0;	/* command */
    }

    iCount = 0;
    for ( index = 0; !IS_NULLSTR( cmd_table[index].name ); index++ )
    {
	/* determine which commands to count */
	if ( fNoMob )
	    usage = cmd_table[index].usage - cmd_table[index].mob_usage;
	else if ( fMobOnly )
	    usage = cmd_table[index].mob_usage;
	else
	    usage = cmd_table[index].usage;

	if ( usage <= 0 )
	    continue;

	/* if no entries in table, plug it in and continue */
	if ( iCount == 0 )
	{
	    list[iCount][0] = usage;
	    list[iCount][1] = index;
	    iCount++;
	    continue;
	}

	/* if table full and usage less than last entry, continue */
	if ( iCount >= USAGE_TABLE && usage <= list[iCount - 1][0] )
	    continue;

	/* Found something that needs to be added to list */

	/* find place in table to insert */
	for ( i = 0; list[i][0] >= usage; i++ )
	    ;


	for ( j = UMIN( iCount, USAGE_TABLE - 1 ); j > i; j-- )
	{
	    list[j][0] = list[j - 1][0];
	    list[j][1] = list[j - 1][1];
	}
	list[i][0] = usage;
	list[i][1] = index;
	if ( iCount < USAGE_TABLE )
	    iCount++;
    }

    for ( index = 0; index < USAGE_TABLE / 4; index++ )
    {
	if ( list[index][0] == 0 )
	    break;
	l = sprintf( buf, "%4d %-12s  ",
                     list[index][0], cmd_table[list[index][1]].name );
        while ( l > 19 && buf[l-1] == ' ' )
        {
            l--;
            buf[l] = '\0';
        }
        add_buf( pBuf, buf );

	if ( list[index+USAGE_TABLE/4][0] != 0 )
	    buf_printf( pBuf, "%4d %-12s  ",
			list[index+USAGE_TABLE/4][0],
			cmd_table[list[index+USAGE_TABLE/4][1]].name );
	if ( list[index+2*USAGE_TABLE/4][0] != 0 )
	    buf_printf( pBuf, "%4d %-12s  ",
			list[index+2*USAGE_TABLE/4][0],
			cmd_table[list[index+2*USAGE_TABLE/4][1]].name );
	if ( list[index+3*USAGE_TABLE/4][0] != 0 )
	    buf_printf( pBuf, "%4d %-12s  ",
			list[index+3*USAGE_TABLE/4][0],
			cmd_table[list[index+3*USAGE_TABLE/4][1]].name );
	add_buf( pBuf, "\n\r" );
    }

    iCount = 0;
    for ( index = 0; !IS_NULLSTR( cmd_table[index].name ); index++ )
    {
	if ( fMobOnly && cmd_table[index].mob_usage != 0 )
	    iCount++;
	else if ( fNoMob && ( ( cmd_table[index].usage - cmd_table[index].mob_usage ) > 0 ) )
	    iCount++;
	else if ( !fMobOnly && !fNoMob && cmd_table[index].usage != 0 )
	    iCount++;
    }

    buf_printf( pBuf, "%d different commands have been used", iCount );
    if ( fMobOnly )
        add_buf( pBuf, " by mobiles" );
    else if ( fNoMob )
        add_buf( pBuf, " by players" );
    add_buf( pBuf, ".\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_vfree( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    int		col;
    int *	list;
    int		high;
    int		i;
    int		j;
    int		low;
    int		min_diff;
    AREA_DATA *	pArea;
    int		sizelist;
    int		top;

    sizelist = ( top_area + 1 ) * 2 * sizeof( *list );
    list = malloc( sizelist );
    if ( list == NULL )
    {
	bug( "Do_vfree: unable to allocate %d bytes.", sizelist );
	return;
    }
    memset( list, 0, sizelist );
    top = 0;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
	low = pArea->min_vnum;
	high = pArea->max_vnum;
	if ( low < 1 || high < 1 || high < low )
	    continue;	/* skip areas whose vnums haven't been assigned yet */

	if ( top == 0 || low > list[top-1] )
	{
	    list[top] = low;
	    list[top+1] = high;
	    top += 2;
	    continue;
	}
	for ( i = 0; low > list[i]; i += 2 )
	    ;
	for ( j = top; j >= i; j-- )
	    list[j+2] = list[j];
	list[i] = low;
	list[i+1] = high;
	top += 2;
    }

    if ( !str_cmp( argument, "all" ) )
	min_diff = 1;
    else
	min_diff = 24;

    /* merge adjacent blocks */
    for ( i = 0; i < top - 2; i += 2 )
    {
	while ( i < top && list[i+2] - list[i+1] <= min_diff )
	{
	    for ( j = i + 1; j < top - 2; j++ )
		list[j] = list[j+2];
	    top -= 2;
	}
    }

    if ( top == 0 )
    {
	send_to_char( "There are no free vnums available.\n\r", ch );
	return;
    }

    pBuf = new_buf( );

    col = 0;
    if ( list[0] > min_diff )
    {
	buf_printf( pBuf, "%6d-%-6d ", 1, list[1] - 1 );
	col = 1;
    }

    for ( i = 1; i < top - 2; i+= 2 )
    {
	buf_printf( pBuf, "%6d-%-6d ", list[i] + 1, list[i+1] - 1 );
	col++;
	if ( col % 5 == 0 )
	    add_buf( pBuf, "\n\r" );
    }

    if ( MAX_VNUM - list[top - 1] > min_diff )
    {
	buf_printf( pBuf, "%6d-%-6d ", list[top-1] + 1, MAX_VNUM );
	col++;
	if ( col % 5 == 0 )
	    add_buf( pBuf, "\n\r" );
    }

    if ( col % 5 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    free( list );
    return;
}


void
do_vused( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    BUFFER *	buf;
    AREA_DATA *	pArea;
    int		lvnum;
    int		uvnum;
    int		ncount;
    int		mcount;
    int		ocount;
    int		rcount;
    int		vnum;
    bool	found;
    bool	fProtoOnly;
    bool	fNoProto;

    fNoProto = FALSE;
    fProtoOnly = FALSE;
    ncount = 0;
    lvnum = 0;
    uvnum = MAX_VNUM;

    argument = one_argument( argument, arg );
    while( arg[0] != '\0' )
    {
	if ( !str_prefix( arg, "prototype" ) )
	    fProtoOnly = TRUE;
	else if ( !str_prefix( arg, "noprototype" ) )
	    fNoProto = TRUE;
	else if ( ( pArea = find_area( ch, arg ) ) != NULL )
	{
	    switch( ++ncount )
	    {
		case 1: lvnum = pArea->min_vnum; break;
		case 2: uvnum = pArea->max_vnum; break;
		default:
		    send_to_char( "Too many areas/vnums.\n\r", ch );
		    return;
	    }
	}
	else if ( is_number( arg ) )
	{
	    switch( ++ncount )
	    {
		case 1: lvnum = atoi( arg ); break;
		case 2: uvnum = atoi( arg ); break;
		default:
		    send_to_char( "Too many areas/vnums.\n\r", ch );
		    return;
	    }
	}
	else
	{
	    ch_printf( ch, "Area '%s' not found.\n\r", arg );
	    return;
	}
	argument = one_argument( argument, arg );
    }

    if ( ncount == 1 )
	uvnum = lvnum;

    buf = new_buf( );
    found = FALSE;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
	if ( pArea->max_vnum < lvnum
	||   pArea->min_vnum > uvnum
	||   ( IS_SET( pArea->area_flags, AREA_PROTOTYPE ) && fNoProto )
	||   ( !IS_SET( pArea->area_flags, AREA_PROTOTYPE ) && fProtoOnly ) )
	    continue;

	if ( !found )
	{
	    add_buf( buf, "Area    Vnum Range   Vnums Rooms Mobs Objs Name, Builder\n\r" );
	    found = TRUE;
	}

	mcount = 0;
	ocount = 0;
	rcount = 0;
	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
	    if ( get_mob_index( vnum ) != NULL )
		mcount++;
	    if ( get_obj_index( vnum ) != NULL )
		ocount++;
	    if ( get_room_index( vnum ) != NULL )
		rcount++;
	}

	buf_printf( buf, "[%3d] %6d-%-6d %6d %4d %4d %4d  %s, %s\n\r",
		    pArea->vnum,
		    pArea->min_vnum,
		    pArea->max_vnum,
		    pArea->max_vnum - pArea->min_vnum + 1,
		    rcount,
		    mcount,
		    ocount,
		    pArea->name,
		    pArea->builders );

    }

    if ( !found )
	add_buf( buf, "No areas found.\n\r" );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );

    return;
}


void
do_whoborder( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char *	p;
    FILE *	fp;

    argument = one_argument( argument, arg );
    if ( *argument == '\0' )
    {
        send_to_char( "Syntax: whoborder immortal <text>\n\r", ch );
        send_to_char( "        whoborder mortal <text>\n\r\n\r", ch );
        ch_printf( ch, "%s`X\n\r", whoborder_imm[0] == '\0'
                   ? "(Immortal whoborder unset)" : whoborder_imm );
        ch_printf( ch, "%s`X\n\r", whoborder_mort[0] == '\0'
                   ? "(Mortal whoborder unset)" : whoborder_mort );
        return;
    }

    if ( str_match( arg, "imm", "immortals" ) )
        p = &whoborder_imm[0];
    else if ( str_match( arg, "mort", "mortals" ) )
        p = &whoborder_mort[0];
    else
    {
        do_whoborder( ch, "" );
        return;
    }

    /* We don't need to call smash_tilde(), since we read this with fgets() */
    strcpy( p, argument );

    if ( ( fp = fopen( TEMP_DIR WHOBORDER_FILE, "w" ) ) != NULL )
    {
        fprintf( fp, "%s\n%s", whoborder_imm, whoborder_mort );
        fclose( fp );
        rename( TEMP_DIR WHOBORDER_FILE, SYSTEM_DIR WHOBORDER_FILE );
    }
    else
    {
        bugf( "Unable to write whoborder file." );
        send_to_char( "Unable to write whoborder file.\n\r", ch );
        return;
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}


void
do_whotext( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs don't have whotexts.\n\r", ch );
	return;
    }

    if ( ( !str_cmp( argument, "none" ) || !str_cmp( argument, "clear" ) )
	   && ch->pcdata->who_text && *ch->pcdata->who_text != '\0' )

    {
	free_string( ch->pcdata->who_text );
	ch->pcdata->who_text = str_dup( "" );
	send_to_char( "Who text cleared.\n\r", ch );
	return;
    }

    strncpy_wocol( arg, argument, WHOTEXT_LEN );
    smash_tilde( arg );

    if ( arg[0] == '\0' )
    {
	if ( ch->pcdata->who_text && *ch->pcdata->who_text == '\0' )
	    send_to_char( "You currently have no whotext set.\n\r", ch );
	else
	    ch_printf( ch, "Your whotext is: %s\n\r", ch->pcdata->who_text );
	return;
    }

    free_string( ch->pcdata->who_text );
    ch->pcdata->who_text = str_dup( arg );
    send_to_char( "Who text set.\n\r", ch );
    return;
}


void
do_wizpwd( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char *	pwdnew;
    CHAR_DATA *	victim;

    argument = one_argument( argument, arg1 );
    argument = first_arg( argument, arg2, FALSE );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: wizpwd player newpassword.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Just use the password command.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You cannot.\n\r", ch );
	return;
    }

    pwdnew = crypt( arg2, victim->name );
    if ( strchr( pwdnew, '~' ) != NULL )
    {
	/* can't have tildes in passwords */
	send_to_char( "New password is unacceptable.\n\r", ch );
	return;
    }

    free_string( victim->pcdata->pwd );
    victim->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( victim );
    send_to_char( "Ok.\n\r", ch );
    return;
}


void
do_wpeace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;

    for ( vch = char_list; vch != NULL; vch = vch->next )
    {
	if ( vch->fighting != NULL )
	{
	    vch->fighting = NULL;
	    vch->position = IS_NPC( vch ) ? vch->default_pos : POS_STANDING;
	}

	if ( vch->desc != NULL && vch->desc->connected == CON_PLAYING )
	    act_color( AT_BLUE, "$N brings peace to the world!",
		       vch, NULL, ch, TO_CHAR, POS_RESTING );
    }
}


void
do_youcount( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    EXTRA_DESCR_DATA *	ed;
    char		arg[MAX_INPUT_LENGTH];
    BUFFER *		pBuf;
    int			count;
    int			acount;
    int			rcount;
    int			tcount;
    int			vnum;

    one_argument( argument, arg );
    if ( !str_cmp( arg, "area" ) )
    {
        argument = one_argument( argument, arg );

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
        buf_printf( pBuf, "[%d] %s`X, %d vnums (%d-%d), %s\n\r",
		    pArea->vnum,
		    pArea->name,
		    pArea->max_vnum - pArea->min_vnum + 1,
		    pArea->min_vnum,
		    pArea->max_vnum,
		    pArea->builders );
        acount = 0;
        rcount = 0;
        tcount = 0;
        for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
        {
            if ( ( pRoom = get_room_index( vnum ) ) != NULL )
            {
                tcount++;
                count  = you_count( pRoom->description );
                count += you_count( pRoom->morning_desc );
                count += you_count( pRoom->evening_desc );
                count += you_count( pRoom->night_desc );
                count += you_count( pRoom->sound );
                count += you_count( pRoom->smell );

                for ( ed = pRoom->extra_descr; ed != NULL; ed = ed->next )
                    count += you_count( ed->description );

                if ( count > 0 )
                {
                    buf_printf( pBuf, "%2d [%d] %s`X\n\r", count, pRoom->vnum, pRoom->name );
                    acount += count;
                    rcount++;
                }
            }
        }
        if ( rcount == 0 )
            add_buf ( pBuf, "This area has no you's.  Good.\n\r" );
        else
            buf_printf( pBuf, "Total you-count: %d in %d out of %d room%s.\n\r",
                        acount, rcount, tcount, tcount == 1 ? "" : "s" );
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
    }
    else
    {
        if ( !str_cmp( arg, "room" ) )
            argument = one_argument( argument, arg );
        pRoom = ( *argument == '\0' ) ? ch->in_room : find_location( ch, argument );
        if ( pRoom == NULL )
        {
            send_to_char( "No such location.\n\r", ch );
            return;
        }
        pBuf = new_buf( );
        buf_printf( pBuf, "Room [%d] - %s`X\n\r", pRoom->vnum, pRoom->name );
        rcount = 0;
        if ( ( count = you_count( pRoom->description ) ) != 0 )
        {
            buf_printf( pBuf, "%d - Description\n\r", count );
            rcount += count;
        }
        if ( ( count = you_count( pRoom->morning_desc ) ) != 0 )
        {
            buf_printf( pBuf, "%d - Morning description\n\r", count );
            rcount += count;
        }
        if ( ( count = you_count( pRoom->evening_desc ) ) != 0 )
        {
            buf_printf( pBuf, "%d - Evening description\n\r", count );
            rcount += count;
        }
        if ( ( count = you_count( pRoom->night_desc ) ) != 0 )
        {
            buf_printf( pBuf, "%d - Night description\n\r", count );
            rcount += count;
        }
        if ( ( count = you_count( pRoom->sound ) ) != 0 )
        {
            buf_printf( pBuf, "%d - Sound\n\r", count );
            rcount += count;
        }
        if ( ( count = you_count( pRoom->smell ) ) != 0 )
        {
            buf_printf( pBuf, "%d - Smell\n\r", count );
            rcount += count;
        }
        for ( ed = pRoom->extra_descr; ed != NULL; ed = ed->next )
        {
            if ( ( count = you_count( ed->description ) ) != 0 )
            {
                buf_printf( pBuf, "%d - Extra [%s]\n\r", count, ed->keyword );
                rcount += count;
            }
        }
        if ( rcount == 0 )
            add_buf( pBuf, "This room has no you's.  Good.\n\r" );
        else
            buf_printf( pBuf, "Total %d.\n\r", rcount );
        send_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
    }

    return;
}


static int
you_count( char *str )
{
    int		count;
    char	buf[MAX_STRING_LENGTH*2];
    char	word[MAX_STRING_LENGTH];
    char *	p;

    if ( IS_NULLSTR( str ) )
        return 0;

    strip_color( buf, str );
    p = buf;
    count = 0;
    while ( *p != '\0' )
    {
        p = one_argument( p, word );
        if ( ( word[0] == 'y' && word[1] == 'o' && word[2] == 'u' ) &&
             (  ( word[3] == '\0' )
             || ( word[3] == '.' || word[3] == '!' || word[3] == '?' )
             || ( word[3] == '\'' )
             || ( word[3] == 'r' && word[4] == '\0' ) ) )
        {
            count++;
        }
    }
    return count;
}


void
do_new_change( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char *	arglist;
    char	buf[MAX_STRING_LENGTH];
    char	inbuf[MAX_INPUT_LENGTH*2];
    FILE *	fp;
    FILE *	fp_tmp;
    bool	found;
    int		line;
    int		line_del;
    char *	p;
    char *	s;

    arglist = one_argument( argument, arg );
    line = 0;
    found = FALSE;

    if ( arg[0] == '\0' || ( !str_cmp( arg, "show" ) && *arglist == '\0' ) )
    {
	if ( ( fp = fopen( SYSTEM_DIR CHANGES_FILE, "r" ) ) == NULL )
	{
	    send_to_char( "No current changes. Do something, lazy.\n\r", ch );
	    return;
	}

	while ( fgets( inbuf, sizeof( inbuf ), fp ) != NULL )
	{
	    found = TRUE;
	    line++;
	    if ( ( s = strchr( inbuf, '\n' ) ) != NULL )
		*s = '\0';
	    p = buf + sprintf( buf, "`W[%d]`w ", line );
	    sprintf( p, "%s\n\r", inbuf );
	    *p = UPPER( *p );
	    p = str_dup( buf );
	    s = format_string( p );
	    send_to_char( s, ch );
	    free_string( s );
	}
	fclose( fp );

	if ( !found )
	    send_to_char( "No changes file. Create one, lazy.!\n\r", ch );

	return;
    }

    if ( !str_cmp( arg, "help" ) && *arglist == '\0' )
    {
        send_to_char( "Syntax: new_change\n\r", ch );
        send_to_char( "        new_change <text>\n\r", ch );
        send_to_char( "        new_change delete <number>\n\r", ch );
        send_to_char( "        new_change help\n\r", ch );
        send_to_char( "        new_change remove <number>\n\r", ch );
        send_to_char( "        new_change show\n\r", ch );
        return;
    }

    if ( ( !str_cmp( arg, "remove" ) || !str_cmp( arg, "delete" ) ) && is_number( arglist ) )
    {
	line_del = atoi( arglist );
	if ( line_del < 1 )
	{
	    send_to_char( "Bad line number.\n\r", ch );
	    return;
	}

	if ( ( fp = fopen( SYSTEM_DIR CHANGES_FILE, "r" ) ) == NULL )
	{
	    send_to_char( "Nothing to remove.\n\r", ch );
	    return;
	}

	if ( ( fp_tmp = fopen( TEMP_DIR CHANGES_FILE, "w" ) ) == NULL )
	{
	    perror( TEMP_DIR CHANGES_FILE );
	    send_to_char( "`RERROR`w opening temp file!\n\r", ch );
	    fclose( fp );
	    return;
	}

	while ( fgets( inbuf, sizeof( inbuf ), fp ) != NULL )
	{
	    line++;
	    if ( line == line_del )
		found = TRUE;
	    else
		fputs( inbuf, fp_tmp );
	}
	fclose( fp_tmp );
	fclose( fp );
	if ( found )
	{
	    rename( TEMP_DIR CHANGES_FILE, SYSTEM_DIR CHANGES_FILE );
	    send_to_char( "Line removed.\n\r", ch );
	}
	else
	{
	    unlink( TEMP_DIR CHANGES_FILE );
	    send_to_char( "Line not found.\n\r", ch );
	}

	return;
    }

    /* Add to file */
    smash_tilde( argument );
    if ( ( fp = fopen( SYSTEM_DIR CHANGES_FILE, "a" ) ) == NULL )
    {
	perror( CHANGES_FILE );
	send_to_char( "`RERROR`w opening changes file!\n\r", ch );
	return;
    }

    fprintf( fp, "%s\n", argument );
    fclose( fp );
    send_to_char( "Line added.\n\r", ch );
    return;
}

