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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "recycle.h"

#define MAX_VISIBILITY	6

const char *const dist_name[] =
{
    "right here",
    "close by",
    "a short distance",
    "not far",
    "rather far off",
    "in the distance",
    "almost out of sight",
};

static void scan_dir	args( ( CHAR_DATA *ch, int door, BUFFER *pBuf,
				int limit ) );

void
do_scan( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	to_room;
    EXIT_DATA *		pExit;
    bool		found;
    int			dir;
    BUFFER *		pBuf;


    if ( !check_blind( ch ) )
	return;

    pBuf = new_buf( );

    in_room = ch->in_room;

    if ( *argument == '\0' )
    {
	act_color( AT_ACTION, "$n scans intently all around.",
		   ch, NULL, NULL, TO_ROOM, POS_RESTING );
	found = FALSE;
	if ( ( to_room = ch->in_room->in_room ) != NULL )
	{
	    char_from_room( ch );
	    char_to_room( ch, to_room );
        }
	for ( dir = 0; dir < MAX_DIR; dir++ )
	{
	    if ( ( pExit = get_exit( ch->in_room, dir ) ) != NULL && !IS_SET( pExit->exit_info, EX_SECRET ) )
	    {
		scan_dir( ch, dir, pBuf, 1 );
		found = TRUE;
	    }
	}
	if ( !found )
	    add_buf( pBuf, "There are no exits here.\n\r" );
    }
    else
    {
	if ( ( dir = door_lookup( argument ) ) == DIR_NONE )
	{
	    send_to_char( "That's not a direction!\n\r", ch );
	    return;
	}
	act_color( AT_ACTION, "$n scans intently $T.",
		   ch, NULL, dir_desc[dir], TO_ROOM, POS_RESTING );
	if ( ( to_room = ch->in_room->in_room ) != NULL )
	{
	    char_from_room( ch );
	    char_to_room( ch, to_room );
        }
	if ( ( pExit = get_exit( ch->in_room, dir ) ) == NULL || IS_SET( pExit->exit_info, EX_SECRET ) )
	{
	    send_to_char( "There is no exit in that direction.\n\r", ch );
	    return;
	}

	scan_dir( ch, dir, pBuf, MAX_VISIBILITY );
    }

    if ( to_room != NULL )
    {
        char_from_room( ch );
        char_to_room( ch, in_room );
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

}


static void
scan_dir( CHAR_DATA *ch, int door, BUFFER *pBuf, int limit )
{
    char		buf[SHORT_STRING_LENGTH];
    int			distance;
    bool		found;
    EXIT_DATA *		pExit;
    CHAR_DATA *		rch;
    ROOM_INDEX_DATA *	room;
    ROOM_INDEX_DATA *	veh;
    int			visibility;

    visibility = MAX_VISIBILITY;
    if ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
	switch( weather_info.sunlight )
	{
	    case SUN_DARK:	visibility = 2;  break;
	    case SUN_SET:	visibility = 4;  break;
	    case SUN_RISE:	visibility = 4;  break;
	    case SUN_LIGHT:			 break;
	}
	switch( weather_info.sky )
	{
	    case SKY_CLOUDLESS:			 break;
	    case SKY_CLOUDY:	visibility -= 1; break;
	    case SKY_RAINING:	visibility -= 2; break;
	    case SKY_LIGHTNING:	visibility -= 3; break;
	}
    }

    visibility = UMIN( visibility, sizeof( dist_name ) /sizeof( dist_name[0] ) - 1 );
    visibility = UMIN( visibility, limit );

    room = ch->in_room;
    found = FALSE;
    for ( distance = 1; distance <= visibility; distance++ )
    {
	if ( ( pExit = get_exit( room, door ) ) != NULL
	     && pExit->to_room != NULL
	     && pExit->to_room != room )
	{
	    if ( IS_SET( pExit->exit_info, EX_SECRET ) )
		break;
	    if ( IS_SET( pExit->exit_info, EX_CLOSED ) )
	    {
		if ( IS_NULLSTR( pExit->keyword ) )
		    strcpy( buf, "door" );
                else
		    strcpy( buf, pExit->keyword );
		buf_printf( pBuf, "A closed %s %s %s.\n\r",
			    buf,
			    dist_name[distance - 1],
			    dir_desc[door] );
		found = TRUE;
		break;
	    }

	    if ( IS_SET( pExit->to_room->room_flags, ROOM_DARK ) )
	    {
		visibility--;
		continue;
	    }

	    if ( IS_SET( pExit->to_room->room_flags, ROOM_NOSCAN ) )
	    {
		continue;
	    }

	    room = pExit->to_room;
	    if ( room->sector_type == SECT_INSIDE
		 ? distance > 4
		 : distance > visibility )
	    {
		break;
	    }

	    for ( rch = room->people; rch != NULL; rch = rch->next_in_room )
	    {
		if ( can_see( ch, rch ) )
		{
		    strcpy( buf, PERS( rch, ch ) );
		    buf[0] = UPPER( buf[0] );
		    buf_printf( pBuf, "%s who is %s %s.\n\r",
				buf,
				dist_name[distance],
				dir_desc[door] );
		    found = TRUE;
		}
	    }

	    for ( veh = room->next_in_room; veh != NULL; veh = veh->next_in_room )
	    {
	        if ( can_see_room( ch, veh ) )
	        {
	            buf_printf( pBuf, "%s which is %s %s.\n\r",
	                        veh->short_descr,
				dist_name[distance],
				dir_desc[door] );
	            found = TRUE;
                }
            }

	}
    }

    if ( !found )
    {
	buf_printf( pBuf, "You can't see anything %s.\n\r", dir_desc[door] );
    }

    return;
}

