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
#include "magic.h"

void
do_announce( CHAR_DATA *ch, char *argument )
{
    if ( IS_NULLSTR( argument ) )
    {
        send_to_char( "Announce what?\n\r", ch );
        return;
    }

    herald_announce( argument );

    return;
}


void
do_descount( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    BUFFER *		pBuf;
    char *		p;
    int			col;
    int			dcount;
    int			ecount;
    int			i;
    int			mcount;
    int			ncount;
    int			rcount;
    int			tcount;
    int			total;
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

    if ( pArea == NULL )
    {
	send_to_char( "Area not found.\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    col    = 0;
    rcount = 0;
    tcount = 0;
    total  = 0;

    buf_printf( pBuf, "[%d] %s`X, %d vnums\n\r", pArea->vnum, pArea->name,
                pArea->max_vnum - pArea->min_vnum + 1 );

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
        if ( ( pRoom = get_room_index( vnum ) ) == NULL )
            continue;

        dcount = 0;
        ecount = 0;
        mcount = 0;
        ncount = 0;
        rcount++;

        p = pRoom->morning_desc;
        while ( *p != '\0' )
        {
            if ( *p == '\n' )
                mcount++;
            p++;
        }
        p = pRoom->description;
        while ( *p != '\0' )
        {
            if ( *p == '\n' )
                dcount++;
            p++;
        }
        p = pRoom->evening_desc;
        while ( *p != '\0' )
        {
            if ( *p == '\n' )
                ecount++;
            p++;
        }
        p = pRoom->night_desc;
        while ( *p != '\0' )
        {
            if ( *p == '\n' )
                ncount++;
            p++;
        }

        buf_printf( pBuf, "[%6d] ", vnum );

        if ( mcount == 0 )
            add_buf( pBuf, " - " );
        else
            buf_printf( pBuf, "%2d ", mcount );

        if ( dcount == 0 )
            add_buf( pBuf, " - " );
        else
            buf_printf( pBuf, "%2d ", dcount );

        if ( ecount == 0 )
            add_buf( pBuf, " - " );
        else
            buf_printf( pBuf, "%2d ", ecount );

        if ( ncount == 0 )
            add_buf( pBuf, " -   " );
        else
            buf_printf( pBuf, "%2d   ", ncount );

        col++;
        if ( col % 3 == 0 )
            add_buf( pBuf, "\n\r" );
        total += mcount + dcount + ecount + ncount;
        if ( mcount != 0 || dcount != 0 || ecount != 0 || ncount != 0 )
            tcount++;
    }

    if ( col % 3 != 0 )
        add_buf( pBuf, "\n\r" );

    buf_printf( pBuf, "%d line%s in %d of %d room%s.\n\r",
                total, total == 1 ? "" : "s", tcount,
                rcount, rcount == 1 ? "" : "s" );

    if ( pArea->overland != NULL )
    {
        add_buf( pBuf, "\n\r" );
        for ( i = 0; i < SECT_MAX; i++ )
        {
            if ( sector_data[i].sector_type == SECT_NONE || IS_NULLSTR( sector_data[i].name ) )
                break;
            dcount = 0;
            ecount = 0;
            mcount = 0;
            ncount = 0;
            p = pArea->overland->sect_desc[i][DESC_DESC];
            if ( !IS_NULLSTR( p ) )
            {
                while ( *p != 0 )
                {
                    if ( *p == '\n' )
                        dcount++;
                    p++;
                }
            }
            p = pArea->overland->sect_desc[i][DESC_MORN];
            if ( !IS_NULLSTR( p ) )
            {
                while ( *p != 0 )
                {
                    if ( *p == '\n' )
                        mcount++;
                    p++;
                }
            }
            p = pArea->overland->sect_desc[i][DESC_EVEN];
            if ( !IS_NULLSTR( p ) )
            {
                while ( *p != 0 )
                {
                    if ( *p == '\n' )
                        ecount++;
                    p++;
                }
            }
            p = pArea->overland->sect_desc[i][DESC_NIGHT];
            if ( !IS_NULLSTR( p ) )
            {
                while ( *p != 0 )
                {
                    if ( *p == '\n' )
                        ncount++;
                    p++;
                }
            }
            buf_printf( pBuf, "%-12s", sector_data[i].name );
            if ( mcount == 0 )
                add_buf( pBuf, " - " );
            else
                buf_printf( pBuf, "%2d ", mcount );

            if ( dcount == 0 )
                add_buf( pBuf, " - " );
            else
                buf_printf( pBuf, "%2d ", dcount );

            if ( ecount == 0 )
                add_buf( pBuf, " - " );
            else
                buf_printf( pBuf, "%2d ", ecount );

            if ( ncount == 0 )
                add_buf( pBuf, " - " );
            else
                buf_printf( pBuf, "%2d ", ncount );

            add_buf( pBuf, "\n\r" );
        }
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_detract( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    char	buf1[SHORT_STRING_LENGTH];
    int		cmd;
    char *	command;
    int		col;
    bool	has;
    char *	p;
    char *	str;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Detract what from whom?\n\r", ch );
        return;
    }

    if ( ( vch = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    if ( IS_NPC( vch ) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust( ch ) < get_trust( vch ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        if ( IS_NULLSTR( vch->pcdata->detract ) )
        {
            ch_printf( ch, "%s has no detractments.\n\r", vch->name );
            return;
        }
        p = buf + sprintf( buf, "Detractments for %s:\n\r", vch->name );
        str = vch->pcdata->detract;
        col = 0;
        while ( *str != '\0' )
        {
            str = one_argument( str, buf1 );
            p += sprintf( p, "%-15s", buf1 );
            col++;
            if ( col % 5 == 0 )
                p = stpcpy( p, "\n\r" );
        }
        if ( col % 5 != 0 )
            p = stpcpy( p, "\n\r" );
        send_to_char( buf, ch );
        return;
    }

    if ( get_trust( ch ) <= get_trust( vch ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    cmd = get_cmd_by_name( arg2 );

    if ( cmd == NO_COMMAND || !can_use_cmd( ch, cmd ) )
    {
        send_to_char( "No such command.\n\r", ch );
        return;
    }

    command = cmd_table[cmd].name;
    has = is_exact_name( cmd_table[cmd].name, vch->pcdata->detract );

    if ( !can_use_cmd( vch, cmd ) && !has )
    {
        ch_printf( ch, "%s already may not that command.\n\r", vch->name );
        return;
    }

    if ( !has )
    {
        if ( is_exact_name( command, vch->pcdata->empower ) )
        {
            send_to_char( "They have been empowered with that command.  Remove the empowerment first.\n\r", ch );
            return;
        }
        if ( IS_NULLSTR( vch->pcdata->detract ) )
            strcpy( buf, command );
        else
            sprintf( buf, "%s %s", command, vch->pcdata->detract );
        free_string( vch->pcdata->detract );
        vch->pcdata->detract = str_dup( buf );
        act_color( AT_IMM, "$t has been detracted from $N.", ch, command,
                   vch, TO_CHAR, POS_DEAD );
        act_color( AT_IMM, "$t has been detracted from you.", vch, command,
                   NULL, TO_CHAR, POS_DEAD );
        return;
    }

    str = vch->pcdata->detract;
    p = buf;
    while ( *str != '\0' )
    {
        str = one_argument( str, buf1 );
        if ( str_cmp( buf1, command ) )
        {
            *p++ = ' ';
            p = stpcpy( p, buf1 );
        }
    }
    *p++ = '\0';
    *p   = '\0';

    free_string( vch->pcdata->detract );
    vch->pcdata->detract = str_dup( buf + 1 );
    act_color( AT_IMM, "$t is no longer detracted from $N.", ch, command,
               vch, TO_CHAR, POS_DEAD );
    act_color( AT_IMM, "$t is no longer detracted from you.", vch, command,
               NULL, TO_CHAR, POS_DEAD );
    return;
}


void
do_dream( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *		vch;
    char		arg[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    DREAM_DATA *	pDream;
    int			vnum;
    EVENT_DATA *	pEvent;

    if ( get_event_char( ch, EVENT_CHAR_DREAM ) != NULL )
    {
        send_to_char( "You are still dreaming.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
        if ( set_dreaming( ch ) )
        {
            if ( ch->position != POS_SLEEPING )
            {
                act_color( AT_ACTION, "You go to sleep.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
                act_color( AT_ACTION, "$n goes to sleep.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
                ch->position = POS_SLEEPING;
            }
        }
        else
            send_to_char( "No dreams are available right now.\n\r", ch );
        return;
    }

    if ( !is_number( arg ) )
    {
        if ( get_trust( ch ) >= SUPREME )
            send_to_char( "Syntax:  dream <dream_vnum> [victim]\n\r", ch );
        else
            send_to_char( "Syntax:  dream <dream_vnum>\n\r", ch );
        return;
    }

    vnum = atoi( arg );
    if ( ( pDream = get_dream_index( vnum ) ) == NULL )
    {
        send_to_char( "That dream was not found.\n\r", ch );
        return;
    }

    if ( get_trust( ch ) >= SUPREME && arg2[0] != '\0' )
    {
        if ( ( vch = get_char_world( ch, arg2 ) ) == NULL )
        {
            send_to_char( "They're not here.\n\r", ch );
            return;
        }
        if ( IS_NPC( ch ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }
        if ( get_trust( vch ) > get_trust( ch ) )
        {
            send_to_char( "Dream it yourself!\n\r", ch );
            return;
        }

        if ( vch->pcdata->forcedream[1] != 0 && get_trust( ch ) != IMPLEMENTOR )
        {
            act_new( "$N already has two dreams to dream.", ch, NULL, vch, TO_CHAR, POS_DEAD );
            return;
        }

        vch->pcdata->forcedream[1] = vch->pcdata->forcedream[0];
        vch->pcdata->forcedream[0] = vnum;
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( ch->position != POS_SLEEPING )
    {
        act_color( AT_ACTION, "You go to sleep.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n goes to sleep.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
        ch->position = POS_SLEEPING;
    }
    pEvent = new_event( );
    pEvent->fun = event_dream;
    pEvent->type = EVENT_CHAR_DREAM;
    pEvent->value0 = vnum;
    pEvent->value1 = 0;
    pEvent->value2 = PULSE_DREAM;
    add_event_char( pEvent, ch, pEvent->value2 );

    if ( !IS_NPC( ch ) )
    {
        ch->pcdata->lastdream[1] = ch->pcdata->lastdream[0];
        ch->pcdata->lastdream[0] = pEvent->value0;
    }

}


void
do_dreamlist( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    BUFFER *		pBuf;
    int			lvnum;
    int			uvnum;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf[MAX_STRING_LENGTH];
    int			count;
    char		color;
    char *		p;
    char *		q;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( !is_number( arg2 ) && arg2[0] != '\0' )
    ||	 ( !is_number( arg1 ) && arg1[0] != '\0' ) )
    {
        send_to_char( "Syntax:  dlist <vnum>\n\r", ch );
        send_to_char( "         dlist <vnum> <vnum>\n\r", ch );
        return;
    }

    lvnum = atoi( arg1 );
    uvnum = arg2[0] == '\0' ? MAX_VNUM : atoi( arg2 );

    pBuf = new_buf( );
    count = 0;
    for ( pDream = dream_list; pDream != NULL; pDream = pDream->next )
    {
        if ( pDream->vnum < lvnum )
            continue;
        if ( pDream->vnum > uvnum )
            break;

        count++;
        color = pDream->type == DREAM_NIGHTMARE ? 'R' : 'C';
        buf_printf( pBuf, "`%c[`w%2d`%c]`w ", color, pDream->vnum, color );
        buf_printf( pBuf, "%s, %s\n\r",
                    IS_NULLSTR( pDream->author ) ? "(Unknown)" : pDream->author,
                    IS_NULLSTR( pDream->title ) ? "(None)" : pDream->title );
        if ( pDream->seg == NULL )
        {
            add_buf( pBuf, "(no dream segments)\n\r" );
            continue;
        }
        p = pDream->seg->text;
        q = buf;
        while ( *p != '\n' && *p != '\r' && *p != '\0' )
            *q++ = *p++;
        q = stpcpy( q, "\n\r" );
        add_buf( pBuf, buf );
    }

    if ( count == 0 )
        add_buf( pBuf, "No dreams found.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_dreamstat( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    DREAM_DATA *pDream;
    BUFFER *	pBuf;
    int		vnum;

    one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
        send_to_char( "Syntax:  dstat <vnum>\n\r", ch );
        return;
    }

    vnum = atoi( arg );
    if ( ( pDream = get_dream_index( vnum ) ) == NULL )
    {
        send_to_char( "No dream has that vnum.\n\r", ch );
        return;
    }

    pBuf = new_buf( );
    show_dream_info( pDream, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_empower( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    char	buf1[SHORT_STRING_LENGTH];
    int		cmd;
    char *	command;
    int		col;
    bool	has;
    char *	p;
    char *	str;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Empower who with what?\n\r", ch );
        return;
    }

    if ( ( vch = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    if ( IS_NPC( vch ) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust( ch ) < get_trust( vch ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        if ( IS_NULLSTR( vch->pcdata->empower ) )
        {
            ch_printf( ch, "%s has no empowerments.\n\r", vch->name );
            return;
        }
        p = buf + sprintf( buf, "Empowerments for %s:\n\r", vch->name );
        str = vch->pcdata->empower;
        col = 0;
        while ( *str != '\0' )
        {
            str = one_argument( str, buf1 );
            p += sprintf( p, "%-15s", buf1 );
            col++;
            if ( col % 5 == 0 )
                p = stpcpy( p, "\n\r" );
        }
        if ( col % 5 != 0 )
            p = stpcpy( p, "\n\r" );
        send_to_char( buf, ch );
        return;
    }

    if ( get_trust( ch ) <= get_trust( vch ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    cmd = get_cmd_by_name( arg2 );

    if ( cmd == NO_COMMAND || !can_use_cmd( ch, cmd ) )
    {
        send_to_char( "No such command.\n\r", ch );
        return;
    }

    command = cmd_table[cmd].name;
    has = is_exact_name( cmd_table[cmd].name, vch->pcdata->empower );

    if ( can_use_cmd( vch, cmd ) && !has )
    {
        ch_printf( ch, "%s already can use that command.\n\r", vch->name );
        return;
    }

    if ( !has )
    {
        if ( is_exact_name( command, vch->pcdata->detract ) )
        {
            send_to_char( "That command has been detracted from them.  Remove the detractment first.\n\r", ch );
            return;
        }
        if ( IS_NULLSTR( vch->pcdata->empower ) )
            strcpy( buf, command );
        else
            sprintf( buf, "%s %s", command, vch->pcdata->empower );
        free_string( vch->pcdata->empower );
        vch->pcdata->empower = str_dup( buf );
        act_color( AT_IMM, "$N has been empowered with $t.", ch, command,
                   vch, TO_CHAR, POS_DEAD );
        act_color( AT_IMM, "You have been empowered with $t!", vch, command,
                   NULL, TO_CHAR, POS_DEAD );
        return;
    }

    str = vch->pcdata->empower;
    p = buf;
    while ( *str != '\0' )
    {
        str = one_argument( str, buf1 );
        if ( str_cmp( buf1, command ) )
        {
            *p++ = ' ';
            p = stpcpy( p, buf1 );
        }
    }
    *p++ = '\0';
    *p   = '\0';

    free_string( vch->pcdata->empower );
    vch->pcdata->empower = str_dup( buf + 1 );
    act_color( AT_IMM, "$N is no longer empowered with $t.", ch, command,
               vch, TO_CHAR, POS_DEAD );
    act_color( AT_IMM, "You are no longer empowered with $t.", vch, command,
               NULL, TO_CHAR, POS_DEAD );
    return;
}


void
do_isocial( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *	d;
    ROOM_INDEX_DATA *	ch_room;
    ROOM_INDEX_DATA *	from_room;
    ROOM_INDEX_DATA *	in_room;
    OBJ_DATA *		ch_on;
    OBJ_DATA *		och_on;
    OBJ_DATA *		vch_on;
    CHAR_DATA *		vch;
    CHAR_DATA *		och;
    SOCIAL_DATA *	pSocial;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		to_char[MAX_INPUT_LENGTH];
    char		to_others[MAX_INPUT_LENGTH];
    char		to_victim[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( IS_SET( ch->comm, COMM_QUIET ) )
    {
        send_to_char( "You must turn off quiet mode first.\n\r", ch );
        return;
    }

    if ( IS_SET( ch->comm, COMM_NOCHANNELS ) )
    {
        send_to_char( "The gods have revoked your channel privileges.\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Isocial which social?\n\r", ch );
	return;
    }

    REMOVE_BIT( ch->deaf, CHANNEL_IMMTALK );

    for ( pSocial = social_first; pSocial != NULL; pSocial = pSocial->next )
        if ( !str_prefix( arg1, pSocial->name ) )
            break;

    if ( pSocial == NULL )
    {
        send_to_char( "Do what to whom?\n\r", ch );
        return;
    }

    vch = NULL;
    if ( arg2[0] != '\0' && ( vch = get_char_world( ch, arg2 ) ) == NULL )
    {
        if ( pSocial->nfound != NULL )
            act_color( AT_YELLOW, pSocial->nfound, ch, NULL, NULL, TO_CHAR, POS_DEAD );
        else
            act_color( AT_YELLOW, "They're not here.", ch, NULL, NULL, TO_CHAR, POS_DEAD );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        strcpy( to_char,   parse_social_string( pSocial->cnoarg ) );
        strcpy( to_others, parse_social_string( pSocial->onoarg ) );
        to_victim[0] = '\0';
    }
    else if ( vch == ch )
    {
        strcpy( to_char,   parse_social_string( pSocial->cself ) );
        strcpy( to_others, parse_social_string( pSocial->oself ) );
        to_victim[0] = '\0';
    }
    else
    {
        strcpy( to_char,   parse_social_string( pSocial->cfound ) );
        strcpy( to_others, parse_social_string( pSocial->ofound ) );
        strcpy( to_victim, parse_social_string( pSocial->vfound ) );
    }

    in_room = get_room_index( ROOM_VNUM_SPAM );

    ch_on = ch->on;
    ch_room = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, in_room );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        och = d->original != NULL ? d->original : d->character;
        if ( d->connected == CON_PLAYING	 &&
             IS_IMMORTAL( och )			 &&
             d->character != ch			 &&
             d->character != vch		 &&
             !IS_NULLSTR( to_others )		 &&
             !IS_SET( och->deaf, CHANNEL_IMMTALK ) &&
             !is_ignoring( och, ch, IGNORE_CHANNELS ) &&
             !IS_SET( och->comm, COMM_QUIET ) )
        {
            och_on = d->character->on;
            from_room = d->character->in_room;
            char_from_room( d->character );
            char_to_room( d->character, in_room );
            act_color( AT_LBLUE, to_others, ch, NULL, vch, TO_ROOM, POS_DEAD );
            char_from_room( d->character );
            char_to_room( d->character, from_room );
            d->character->on = och_on;
        }
        if ( d->connected == CON_PLAYING	 &&
             IS_IMMORTAL( och )			 &&
             d->character != ch			 &&
             d->character == vch		 &&
             !IS_NULLSTR( to_victim )		 &&
             !IS_SET( och->comm, CHANNEL_IMMTALK ) &&
             !is_ignoring( och, ch, IGNORE_CHANNELS ) &&
             !IS_SET( och->comm, COMM_QUIET ) )
        {
            vch_on = d->character->on;
            from_room = d->character->in_room;
            char_from_room( d->character );
            char_to_room( d->character, in_room );
            act_color( AT_LBLUE, to_victim, ch, NULL, vch, TO_VICT, POS_DEAD );
            char_from_room( d->character );
            char_to_room( d->character, from_room );
            d->character->on = vch_on;
        }
    }

    act_color( AT_LBLUE, to_char, ch, NULL, vch, TO_CHAR, POS_DEAD );

    char_from_room( ch );
    char_to_room( ch, ch_room );
    ch->on = ch_on;
    return;
}


void
do_mobcount( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    BUFFER *		pBuf;
    CHAR_DATA *		pMob;
    ROOM_INDEX_DATA *	pRoom;
    int			area_in_area;
    int			area_in_game;
    int			game_in_area;
    int			mobcount;
    int			vnum;

    if ( *argument == '\0' )
    {
        if ( ch->in_room != NULL )
            pArea = ch->in_room->area;
        else
            pArea = NULL;
    }
    else if ( is_number( argument ) )
    {
        vnum = atoi( argument );
        if ( ( pArea = get_area_data( vnum ) ) == NULL )
        {
            for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
                if ( pArea->min_vnum <= vnum && pArea->max_vnum >= vnum )
                    break;
        }
    }
    else
    {
        for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
            if ( is_name( argument, pArea->name ) )
                break;
        if ( pArea == NULL )
        {
            pRoom = find_location( ch, argument );
            if ( pRoom != NULL )
                pArea = pRoom->area;
        }
    }

    if ( pArea == NULL )
    {
        send_to_char( "Area not found.\n\r", ch );
        return;
    }

    mobcount = 0;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
        if ( get_mob_index( vnum ) != NULL )
            mobcount++;

    pBuf = new_buf( );
    buf_printf( pBuf, "[%d] %s`X, %d vnums (%d-%d), %d mob%s\n\r",
                pArea->vnum,
                pArea->name,
                pArea->max_vnum - pArea->min_vnum + 1,
                pArea->min_vnum,
                pArea->max_vnum,
                mobcount,
                mobcount == 1 ? "" : "s" );

    area_in_area = 0;
    area_in_game = 0;
    game_in_area = 0;

    for ( pMob = char_list; pMob != NULL; pMob = pMob->next )
    {
        if ( IS_NPC( pMob ) )
        {
            if ( pMob->pIndexData->vnum >= pArea->min_vnum
            &&	 pMob->pIndexData->vnum <= pArea->max_vnum )
                area_in_game++;

            if ( pMob->in_room != NULL
            &&	 pMob->in_room->vnum >= pArea->min_vnum
            &&	 pMob->in_room->vnum <= pArea->max_vnum )
            {
                game_in_area++;
                if ( pMob->pIndexData->vnum >= pArea->min_vnum
                &&   pMob->pIndexData->vnum <= pArea->max_vnum )
                    area_in_area++;
            }
        }
    }

    buf_printf( pBuf, "Mobs from area in area: %d\n\r", area_in_area );
    if ( area_in_area != area_in_game )
        buf_printf( pBuf, "Mobs from area in game: %d\n\r", area_in_game );
    if ( area_in_area != game_in_area )
        buf_printf( pBuf, "Mobs from game in area: %d\n\r", game_in_area );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_pkill( CHAR_DATA *ch, char *argument )
{
    int		value;

    if ( *argument == '\0' )
    {
        if ( sys_pkill == 0 )
            send_to_char( "Player killing is enabled.\n\r", ch );
        else if ( sys_pkill > MAX_LEVEL )
            send_to_char( "Player killing is disabled.\n\r", ch );
        else
            ch_printf( ch, "Player killing is limited to level %d and above.\n\r",
                       sys_pkill > LEVEL_NEWBIE + 1 ? sys_pkill : LEVEL_NEWBIE + 1 );
        return;
    }

    if ( is_number( argument ) )
    {
        value = atoi( argument );
        if ( value <= LEVEL_NEWBIE || value > MAX_LEVEL )
        {
            ch_printf( ch, "Level must be %d to %d.\n\r", LEVEL_NEWBIE + 1, MAX_LEVEL );
            return;
        }
        sys_pkill = value;
        send_to_char( "Pkill level set.\n\r", ch );
        changed_system = TRUE;
        save_sysconfig( );
        return;
    }

    if ( !str_cmp( argument, "on" )
    ||	 !str_cmp( argument, "true" )
    ||	 !str_prefix( argument, "yes" )
    ||	 str_match( argument, "ena", "enabled" ) )
    {
        sys_pkill = 0;
        send_to_char( "Ok.\n\r", ch );
        changed_system = TRUE;
        save_sysconfig( );
        return;
    }

    if ( !str_cmp( argument, "off" )
    ||	 !str_cmp( argument, "false" )
    ||	 !str_prefix( argument, "no" )
    ||	 str_match( argument, "dis", "disabled" ) )
    {
        sys_pkill = MAX_LEVEL + 1;
        send_to_char( "Ok.\n\r", ch );
        changed_system = TRUE;
        save_sysconfig( );
        return;
    }

    send_to_char(
    "Syntax: pkill\n\r"
    "        pkill on|true|yes|enabled\n\r"
    "        pkill off|false|no|disabled\n\r"
    "        pkill <level>\n\r", ch );

    return;
}


void
do_rinfo( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    BUFFER *		pBuf;
    EXTRA_DESCR_DATA *	pEx;
    EXIT_DATA *		pExit;
    OREPROG_DATA *	pProg;
    RESET_DATA *	pReset;
    ROOM_INDEX_DATA *	pRoom;
    int			c;
    int			dir;
    int			n;

    one_argument( argument, arg );
    pRoom = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( pRoom == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    pBuf = new_buf( );

    buf_printf( pBuf, "Room: [%5d] %s`X\n\r", pRoom->vnum, pRoom->name );
    buf_printf( pBuf, "Area: [%5d] %s`X\n\r",
	    pRoom->area->vnum, pRoom->area->name );
    n = count_lines( pRoom->description );
    buf_printf( pBuf, "Description:  %d line%s.\n\r", n, n == 1 ? "" : "s" );
    if ( ( n = count_lines( pRoom->morning_desc ) ) != 0 )
        buf_printf( pBuf, "Morning desc: %d line%s.\n\r", n, n == 1 ? "" : "s" );
    if ( ( n = count_lines( pRoom->evening_desc ) ) != 0 )
        buf_printf( pBuf, "Evening desc: %d line%s.\n\r", n, n == 1 ? "" : "s" );
    if ( ( n = count_lines( pRoom->night_desc ) ) != 0 )
        buf_printf( pBuf, "Night desc:   %d line%s.\n\r", n, n == 1 ? "" : "s" );
    if ( ( n = count_lines( pRoom->sound ) ) != 0 )
        buf_printf( pBuf, "Sound: %d line.%s\n\r", n, n == 1 ? "" : "s" );
    if ( ( n = count_lines( pRoom->smell ) ) != 0 )
        buf_printf( pBuf, "Smell: %d line%s.\n\r", n, n == 1 ? "" : "s" );

    n = 0;
    c = 0;
    for ( pEx = pRoom->extra_descr; pEx != NULL; pEx = pEx->next )
    {
        c++;
        n += count_lines( pEx->description );
    }
    if ( c != 0 )
        buf_printf( pBuf, "%d extra description%s with %d line%s.\n\r",
                    c, c == 1 ? "" : "s",
                    n, n == 1 ? "" : "s" );

    c = 0;
    for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
        c++;
    if ( c != 0 )
        buf_printf( pBuf, "Resets: %d\n\r", c );

    n = 0;
    c = 0;
    for ( pProg = pRoom->rprogs; pProg != NULL; pProg = pProg->next )
    {
        c++;
        n += count_lines( pProg->comlist );
    }

    if ( c != 0 )
        buf_printf( pBuf, "%d room prog%s with %d line%s.\n\r",
                    c, c == 1 ? "" : "s",
                    n, n == 1 ? "" : "s" );

    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
        if ( ( pExit = pRoom->exit[dir] ) != NULL )
        {
            buf_printf( pBuf, "%s %-5s to [%d] %s`X\n\r",
                        IS_SET( pExit->rs_flags, EX_ISDOOR ) ? "Door" : "Exit",
                        dir_name[dir],
                        pExit->to_room != NULL ? pExit->to_room->vnum : 0,
                        pExit->to_room != NULL ? pExit->to_room->name : "nowhere" );
            if ( ( n = count_lines( pExit->description ) ) != 0 )
                buf_printf( pBuf, "  description:   %d line%s.\n\r",
                            n, n == 1 ? "" : "s" );
            if ( ( n = count_lines( pExit->sound_closed ) ) != 0 )
                buf_printf( pBuf, "  sound(closed): %d line%s.\n\r",
                            n, n == 1 ? "" : "s" );
            if ( ( n = count_lines( pExit->sound_open ) ) != 0 )
                buf_printf( pBuf, "  sound(open):   %d line%s.\n\r",
                            n, n == 1 ? "" : "s" );
            n = 0;
            c = 0;
            for (  pProg = pExit->eprogs; pProg != NULL; pProg = pProg->next )
            {
                c++;
                n += count_lines( pProg->comlist );
            }
            if ( c != 0 )
                buf_printf( pBuf, "  %d exit prog%s with %d line%s.\n\r",
                            c, c == 1 ? "" : "s",
                            n, n == 1 ? "" : "s" );
        }
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_skstat( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    int		n;
    int		sn;

    if ( IS_NULLSTR( argument ) )
    {
        send_to_char( "Skstat what skill/spell?\n\r", ch );
        return;
    }

    if ( is_number( argument ) )
    {
        n = atoi( argument );
        for ( sn = 0; !IS_NULLSTR( skill_table[sn].name ); sn++ )
            if ( sn == n )
                break;
        if ( IS_NULLSTR( skill_table[sn].name ) )
            sn = NO_SKILL;
    }
    else
        sn = skill_lookup( argument );

    if ( sn == NO_SKILL )
    {
        send_to_char( "No such skill/spell.\n\r", ch );
        return;
    }

    pBuf = new_buf( );
    show_skill_info( sn, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


void
do_travstat( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    BUFFER *	pBuf;
    bool	found;

    if ( *argument == '\0' )
    {
        pArea = ch->in_room->area;
    }
    else if ( !str_cmp( argument, "all" ) )
    {
        pBuf = new_buf( );
        found = FALSE;
        for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
        {
            if ( pArea->travel_room == NULL )
                continue;
            buf_printf( pBuf, "[%5d] ", pArea->travel_room->vnum );
            if ( pArea->travel_curr != NULL )
            {
                if ( pArea->travel_curr->stopped )
                    add_buf( pBuf, "at " );
                else
                    add_buf( pBuf, "-->" );
                buf_printf( pBuf, " [%d] %s\n\r",
                            pArea->travel_curr->room->vnum,
                            pArea->travel_curr->room->name );
            }
            else
            {
                add_buf( pBuf, "under repair\n\r" );
            }
            found = TRUE;
        }
        if ( !found )
            add_buf( pBuf, "There are currently no traveling rooms.\n\r" );
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
        return;
    }
    else
    {
        pArea = find_area( ch, argument );
    }

    if ( pArea == NULL )
    {
        send_to_char( "Area not found.\n\r", ch );
        return;
    }

    pBuf = new_buf( );

    show_travel_info( pArea, pBuf );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_unlaunch( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	vehicle;
    char		arg[MAX_INPUT_LENGTH];
    bool		here;
    bool		there;

    one_argument( argument, arg );
    vehicle = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );

    if ( vehicle == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    here = ch->in_room == vehicle;

    if ( vehicle->vehicle_type == VEHICLE_NONE )
    {
	ch_printf( ch, "%s room can't be launched or unlaunched.\n\r",
                   here ? "This" : "That" );
	return;
    }

    if ( vehicle->in_room == NULL )
    {
        send_to_char( "Room is not launched.\n\r", ch );
        return;
    }

    there = ch->in_room == vehicle->in_room;

    act_color( AT_IMM, "$t suddenly fades and disappears into nothingness.",
               vehicle->in_room->people, ROOMNAME( vehicle ), NULL, TO_ALL, POS_RESTING );
    room_from_room( vehicle );
    vehicle_moved = TRUE;

    act_color( AT_IMM, "$t disappears into a strange sort of limbo.",
               vehicle->people, ROOMNAME( vehicle ), NULL, TO_ALL, POS_RESTING );

    if ( !here && !there )
        send_to_char( "Room unlaunched.\n\r", ch );

    return;
}


void
do_vlist( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;
    char		arg[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH];
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    ROOM_INDEX_DATA *	in_room;
    int			vtype;
    bool		found;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
        vtype = VEHICLE_NONE;	/* Cheating; VEHICLE_NONE means list all types. */
    else
    {
        if ( ( vtype = flag_lookup( arg, vehicle_types ) ) == NO_FLAG )
        {
            send_to_char( "No such vehicle type.\n\r", ch );
            return;
        }
    }

    pBuf = new_buf( );
    found = FALSE;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
        for ( pRoom = pArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
        {
            if ( pRoom->vehicle_type == VEHICLE_NONE
            ||	 ( vtype != VEHICLE_NONE && vtype != pRoom->vehicle_type ) )
                continue;
            strip_color( buf, pRoom->short_descr );
            buf[25] = '\0';	/* unconditionally truncate names to 25 chars */
            buf_printf( pBuf, "[%5d] %-25s --> ", pRoom->vnum, buf );
            if ( pRoom->in_room == NULL )
                add_buf( pBuf, "(not launched)" );
            else
            {
                in_room = pRoom->in_room;
                while ( in_room->in_room != NULL )
                    in_room = in_room->in_room;
                strip_color( buf, in_room->name );
                buf_printf( pBuf, "[%6d] %s", in_room->vnum, in_room->name );
            }
            add_buf( pBuf, "\n\r" );
            found = TRUE;
        }
    }

    if ( !found )
        add_buf( pBuf, "No vehicles found.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


void do_pipe_system(CHAR_DATA *ch, char *argument)
{
	FILE *read_fp;
	char buffer[MSL *2];
	int chars_read;

	memset(buffer, '\0', sizeof(buffer));
	read_fp = popen(argument, "r");

	if(read_fp != NULL)
	{
		chars_read = fread(buffer, sizeof(char), MSL *2, read_fp);
		if(chars_read > 0)
		{
			page_to_char(buffer, ch);
		}
		pclose(read_fp);
	}

	return;
}

//extern bool newlock;
//extern bool wizlock;

void
do_mudstat( CHAR_DATA *ch, char *argument )
{
  char buf[64];

  send_to_char("`W  Current MUD Status`X\n\r`W---------------------`X\n\r", ch);
  sprintf(buf,"   Newlock is: %s\n\r",newlock ? "`rON`X" : "`GOFF`X");
  send_to_char(buf, ch);
  sprintf(buf,"   Wizlock is: %s\n\r",wizlock ? "`rON`X" : "`GOFF`X");
  send_to_char(buf, ch);
}

void
do_setcoder(CHAR_DATA *ch, char *argument)
{
char arg1 [MAX_INPUT_LENGTH];
char arg2 [MAX_INPUT_LENGTH];
CHAR_DATA *victim;

    argument = one_argument(argument, arg1);
    argument = one_argument( argument, arg2 );

if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char("set_coder command: set_coder <target> on | off.\n\r",ch);
        return;
    }

if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

if ( !IS_NPC( victim )
    &&	 victim != ch
    &&	 ( get_trust( ch ) < SUPREME
         || get_trust( ch ) < get_trust( victim ) ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

   /* clear zones for mobs */
    victim->zone = NULL;


   if ( !str_prefix( arg2, "on" ) )
    {
if (!IS_NPC(victim))
            SET_BIT(ch->act2, CODER);
	send_to_char( "Coder set to `GON`X.\n\r", ch );
	return;
    }   
  
   if ( !str_prefix( arg2, "off" ) )
    {
if (!IS_NPC(victim))
            REMOVE_BIT(ch->act2, CODER);
	send_to_char( "Coder set to `ROFF`X.\n\r", ch );
	return;
    }
}


void do_wizify (CHAR_DATA *ch, char * argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int level;

   argument = one_argument (argument, arg1);
    strcpy (arg2, argument);

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char ("Syntax: wizify <char> <security>.\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("They aren't online.\n\r", ch);
        return;
    }

    if (IS_NPC (victim))
    {
        send_to_char ("Mobs can't be Immortals!!!\n\r", ch);
        return;
    }

   value = is_number (arg2) ? atoi (arg2) : 0;

        if (value < 0 || value > 9)
        {
            sprintf (buf,
                     "Builder security range is 0 to 9\n\r.");
            send_to_char (buf, ch);
            return;
        }

    if (victim->level > ch->level )
    {
       send_to_char("Try it on someone your own size.\n\r", ch);
              return;
    }

//    if (victim->pcdata->security >= ch->pcdata->security )
  //  {
   //    send_to_char("You aren't trusted at a high enough level.\n\r", ch);
    //   return;
   // }

    if (victim->level <= LEVEL_HERO)
    {
      send_to_char("You can only wizify Immortals!\n\r",ch);
      return;
    }

    /*Set the max stats and security*/
            victim->max_hit = 30000;
            victim->max_mana = 30000;
            victim->max_move = 30000;
        victim->perm_stat[STAT_STR] = 25;
        victim->perm_stat[STAT_INT] = 25;
        victim->perm_stat[STAT_WIS] = 25;
        victim->perm_stat[STAT_DEX] = 25;
        victim->perm_stat[STAT_CON] = 25;
        victim->incog_level = level;
        victim->invis_level = level;
            victim->pcdata->security = value;

           /* Set the common act flags for Immortals. */
            SET_BIT (victim->act, PLR_HOLYLIGHT);
        SET_BIT (victim->act, PLR_NOSUMMON);
        REMOVE_BIT(victim->act, PLR_CANLOOT);

        /*Restore the character */
        victim->hit = victim->max_hit;
        victim->mana = victim->max_mana;
        victim->move = victim->max_move;
            update_pos (victim);

   sprintf(buf, "%s has been Wizified! All stats have been set.\n\r",victim->name);
   send_to_char(buf,ch);
   send_to_char("Alright!!! Suddenly you feel like a TRUE IMMORTAL!\n\r",victim);
   return;
}

void do_mortskill(CHAR_DATA *ch, char *argument)
{
    BUFFER *buffer;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char skill_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
    char skill_columns[LEVEL_HERO + 1];
    int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
    bool found = FALSE;
    CHAR_DATA *victim;

    if (IS_NPC(ch))
        return;

    one_argument(argument,arg);

    if ( arg[0] == '\0' )
    {
        send_to_char( "`RList skills for whom?`X\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        send_to_char( "`RThey aren't here.`X\n\r", ch );
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char( "`RUse this for skills on players.`X\n\r", ch );
        return;
    }

    /* initialize data */
    for (level = 0; level < LEVEL_HERO + 1; level++)
    {
        skill_columns[level] = 0;
        skill_list[level][0] = '\0';
    }

    for (sn = 0; sn < top_skill; sn++)
    {
        if (skill_table[sn].name == NULL )
            break;

        if ((level = skill_table[sn].skill_level[victim->class]) < LEVEL_HERO + 1
            &&  level >= min_lev && level <= max_lev
            &&  skill_table[sn].spell_fun == spell_null
            &&  victim->pcdata->skill[sn].percent > 0)
        {
            found = TRUE;
            level = skill_table[sn].skill_level[victim->class];

            if (victim->level < level)
                sprintf(buf,"`C%-18s `Yn`W/`Ya      `X", skill_table[sn].name);
            else
                sprintf(buf,"`C%-18s `Y%3d%%      `X",skill_table[sn].name,
                    victim->pcdata->skill[sn].percent);

            if (skill_list[level][0] == '\0')
                sprintf(skill_list[level],"\n\rLevel `W%3d:`X %s",level,buf);
                  else /* append */
            {
                if ( ++skill_columns[level] % 2 == 0)
                    strcat(skill_list[level],"\n\r           ");
                strcat(skill_list[level],buf);
            }
        }
    }

    /* return results */
    if (!found)
    {
        send_to_char("`RNo skills found.`X\n\r",ch);
        return;
    }

    buffer = new_buf();

    for (level = 0; level < LEVEL_HERO + 1; level++)
        if (skill_list[level][0] != '\0')
            add_buf(buffer,skill_list[level]);

    add_buf(buffer,"\n\r");
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_mortspell(CHAR_DATA *ch, char *argument)
{
    BUFFER *buffer;
    char buff[100];
    char arg[MAX_INPUT_LENGTH];
    char spell_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
    char spell_columns[LEVEL_HERO + 1];
    int sn, gn, col, level, min_lev = 1, max_lev = LEVEL_HERO, mana;
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (IS_NPC(ch))
        return;

    one_argument(argument,arg);

    if ( arg[0] == '\0' )
    {
        send_to_char( "`RList spells for whom?`X\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        send_to_char( "`RThey aren't here.`X\n\r", ch );
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char( "`RUse this for skills on players.`X\n\r", ch );
        return;
    }

    /* groups */
    col = 0;
    for (gn = 0; gn < MAX_GROUP; gn++)
    {
        if (group_table[gn].name == NULL)
            break;
        if (victim->pcdata->group_known[gn])
        {
            sprintf(buff,"`G%-20s`X ",group_table[gn].name);
            send_to_char(buff,ch);

            if (++col % 3 == 0)
                send_to_char("\n\r",ch);
        }
    }

    if ( col % 3 != 0 )
    {
        send_to_char( "\n\r", ch );
        sprintf(buff,"`GCreation points: `W%d`X\n\r",victim->pcdata->points);
        send_to_char(buff,ch);
    }

    /* initialize data */
    for (level = 0; level < LEVEL_HERO + 1; level++)
    {
        spell_columns[level] = 0;
        spell_list[level][0] = '\0';
    }

    for (sn = 0; sn < top_skill; sn++)
    {
        if (skill_table[sn].name == NULL )
        break;

        if ((level = skill_table[sn].skill_level[victim->class]) < LEVEL_HERO + 1
            &&  level >= min_lev && level <= max_lev
            &&  skill_table[sn].spell_fun != spell_null
            &&  victim->pcdata->skill[sn].percent > 0)
        {
            found = TRUE;
            level = skill_table[sn].skill_level[victim->class];
            if (victim->level < level)
                sprintf(buf,"`C%-18s `Yn`W/`Ya`X      ", skill_table[sn].name);
            else
            {
                mana = UMAX(skill_table[sn].min_mana, 100/(2 + victim->level - level));
                sprintf(buf,"`C%-18s  `Y%3d `Gmana`X  ",skill_table[sn].name,mana);
            }

            if (spell_list[level][0] == '\0')
                sprintf(spell_list[level],"\n\rLevel `W%3d:`X %s",level,buf);
            else /* append */
            {
                if ( ++spell_columns[level] % 2 == 0)
                    strcat(spell_list[level],"\n\r           ");
                strcat(spell_list[level],buf);
            }
        }
    }

    /* return results */
    if (!found)
    {
        send_to_char("`RNo spells found.`X\n\r",ch);
        return;
    }

    buffer = new_buf();
    for (level = 0; level < LEVEL_HERO + 1; level++)
        if (spell_list[level][0] != '\0')
            add_buf(buffer,spell_list[level]);
    add_buf(buffer,"\n\r");
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}
