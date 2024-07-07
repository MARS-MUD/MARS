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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"


/*
 *  Local functions
 */
const char *	get_health_str( int percent )
{
    if ( percent >= 100 ) 
        return " is unharmed.\n\r";
    else if ( percent >= 90 ) 
        return " is injured slightly.\n\r";
    else if ( percent >= 75 ) 
        return" is a bit battered.\n\r";
    else if ( percent >=  50 ) 
        return " is damaged considerably.\n\r";
    else if ( percent >= 30 )
        return " is quite hurt.\n\r";
    else if ( percent >= 15 )
        return " is grievously harmed.\n\r";
    else if ( percent >= 0 )
        return " is nearly vanquished.\n\r";
    else
        return " is incapacitated.\n\r";
}


void
do_triage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *		victim;
    CLIST_DATA *	t_char;
    CLIST_DATA *	t_first;
    CLIST_DATA *	tmp;
    BUFFER *		pBuf;
    char		buf[MAX_INPUT_LENGTH];
    char *		p;
    char *		q;
    int			count;
    int			prct;
    int			sn;

    if ( ch->in_room == NULL )
    {
        send_to_char( "There is nobody here, not even you.\n\r", ch );
        return;
    }

    if ( ( sn = skill_lookup( "triage" ) ) == NO_SKILL )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ( prct = get_skill( ch, sn ) ) == 0 )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    count = 0;
    if ( number_percent( ) > prct )
    {
        for ( victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room )
            if ( victim != ch && can_see( victim, ch ) )
                count++;
        send_to_char( "You look around the room and can't decide who needs help the most.\n\r", ch );
        if ( count > 1 )
            check_improve( ch, sn, FALSE, 4 );
        return;
    }

    t_first = NULL;

    for ( victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room )
    {
        if ( victim == ch || !can_see( victim, ch ) )
            continue;

        t_char = new_clist_data( );
        t_char->lch = victim;
        t_char->number = ( 100 * victim->hit ) / UMAX( 1, victim->max_hit );
        if ( t_first == NULL || t_char->number < t_first->number )
        {
            t_char->next = t_first;
            t_first = t_char;
        }
        else if ( t_first->next == NULL )
        {
            t_char->next = NULL;
            t_first->next = t_char;
        }
        else
        {
            for ( tmp = t_first; tmp->next != NULL; tmp = tmp->next )
            {
                if ( t_char->number < tmp->next->number )
                {
                    t_char->next = tmp->next;
                    tmp->next = t_char->next;
                    break;
                }
            }
            if ( tmp->next == NULL )
            {
                t_char->next = NULL;
                tmp->next = t_char;
            }
        }
    }

    if ( t_first == NULL )
    {
        send_to_char( "There is nobody here to triage.\n\r", ch );
        return;
    }

    pBuf = new_buf( );
    add_buf( pBuf, "You briefly examine everyone in the room and see:\n\r" );

    while ( t_first != NULL )
    {
        t_char = t_first;
        t_first = t_char->next;

        p = stpcpy( buf, PERS( t_char->lch, ch ) );
        if ( has_colcode( buf ) )
            p = stpcpy( p, "`X" );
        q = buf;
        while ( is_colcode( q ) )
            q += 2;
        *q = UPPER( *q );
        add_buf( pBuf, buf );
        add_buf( pBuf, get_health_str( t_char->number ) );

        free_clist_data( t_char );
    }

    if ( count > 1 )
        check_improve( ch, sn, TRUE, 4 );

    act_color( AT_ACTION, "$n briefly examines everyone in the room.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


