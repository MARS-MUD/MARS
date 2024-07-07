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
#include <time.h>
#include <sys/types.h>
#include "merc.h"
#include "interp.h"


void
do_charge( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    int		chance;

    one_argument( argument, arg );

    if ( ( chance = get_skill( ch, gsn_charge ) ) == 0
    ||	 ( !IS_NPC( ch ) && ch->level < skill_table[gsn_charge].skill_level[ch->class] ) )
    {
	send_to_char(
	    "You better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
        victim = ch->fighting;
        if ( victim == NULL )
        {
            send_to_char( "But you aren't fighting anyone!\n\r", ch );
            return;
        }
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( is_safe( ch, victim ) )
        return;

    if ( IS_NPC( victim ) && victim->fighting != NULL
    &&   !is_same_group( ch, victim->fighting ) )
    {
        send_to_char( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Charge yourself?\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    /* Modifiers */

    chance += get_curr_stat( ch, STAT_DEX );
    chance -= get_curr_stat( victim, STAT_DEX ) * 3 / 2;

    if ( xIS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ) )
	chance += 10;
    if ( xIS_SET( victim->off_flags, OFF_FAST ) || IS_AFFECTED( victim, AFF_HASTE ) )
	chance -= 20;

    chance += ch->level - victim->level;
    chance = URANGE( 2, chance, 100 );
    chance = 3 * chance / 4;

    WAIT_STATE( ch, skill_table[gsn_charge].beats ); 

    if ( number_percent( ) < chance )
    {
        act_color( AT_ACTION, "You charge into $N!", ch, NULL, victim, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n charges into you!", ch, NULL, victim, TO_VICT, POS_RESTING );
        act_color( AT_ACTION, "$n charges into $N!", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
        check_improve( ch, gsn_charge, TRUE, 1 );
        damage( ch, victim, number_range( ch->level/2, 3*ch->level/2 ), gsn_charge, DAM_BASH, TRUE );
    }
    else
    {
        act_color( AT_ACTION, "Your charge misses $N and you go sprawling.", ch, NULL, victim, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n's charge misses you and $e goes sprawling.", ch, NULL, victim, TO_VICT, POS_RESTING );
        act_color( AT_ACTION, "$n's charge misses $N and $e goes sprawling.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
        check_improve( ch, gsn_charge, FALSE, 1 );
        DAZE_STATE( ch, 3 * PULSE_VIOLENCE );
        ch->position = POS_RESTING;
        damage( ch, victim, 0, gsn_charge, DAM_BASH, TRUE );
    }

    check_killer( ch, victim );
    return;
}


/*
 * No action required.  Simply let the event driver strip the event.
 */
bool
event_char_nopk( EVENT_DATA *event )
{
    return FALSE;
}

