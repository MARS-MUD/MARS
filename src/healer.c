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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"

void
do_heal( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	mob;
    char	arg[MAX_INPUT_LENGTH];
    int 	cost;	
    MONEY	amt;
    int		sn;
    SPELL_FUN *	spell;
    char *	words;	

    /* check for healer */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_IS_HEALER ) )
            break;
    }

    if ( mob == NULL )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* display price list */
	act_color( AT_SAY,
	           "`W$N says '`GI offer the following spells:`W'",
	           ch, NULL, mob, TO_CHAR, POS_RESTING );
	ch_printf( ch, "  light: cure light wounds       2 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  serious: cure serious wounds   3 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  critic: cure critical wounds   5 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  heal: healing spell	        10 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  full: full healing spell    1000 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  blind: cure blindness          4 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  disease: cure disease          3 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  poison:  cure poison	         5 %s\n\r", COPPER_PLURAL ); 
	ch_printf( ch, "  uncurse: remove curse	        10 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  refresh: restore movement      1 %s\n\r", COPPER_NOUN );
	ch_printf( ch, "  mana:  restore mana	         2 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "  cancel:  cancels spells        10 %s\n\r", COPPER_PLURAL );
	send_to_char(  " Type heal <type> to be healed.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "light" ) )
    {
        spell = spell_cure_light;
	sn    = skill_lookup( "cure light" );
	words = "judicandus dies";
	cost  = 2 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "serious" ) )
    {
	spell = spell_cure_serious;
	sn    = skill_lookup( "cure serious" );
	words = "judicandus gzfuajg";
	cost  = 3 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "critical" ) )
    {
	spell = spell_cure_critical;
	sn    = skill_lookup( "cure critical" );
	words = "judicandus qfuhuqar";
	cost  = 5 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "heal" ) )
    {
	spell = spell_heal;
	sn = skill_lookup( "heal" );
	words = "pzar";
	cost  = 10 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "full" ) )
    {
	spell = spell_full_heal;
	sn = skill_lookup( "full heal" );
	words = "judicandus raufruba";
	cost  = 1000 * FRACT_PER_COPPER;
    }
    else if ( !str_prefix( arg, "blindness" ) )
    {
	spell = spell_cure_blindness;
	sn    = skill_lookup( "cure blindness" );
      	words = "judicandus noselacri";		
        cost  = 4 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "disease" ) )
    {
	spell = spell_cure_disease;
	sn    = skill_lookup( "cure disease" );
	words = "judicandus eugzagz";
	cost  = 3 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "poison" ) )
    {
	spell = spell_cure_poison;
	sn    = skill_lookup( "cure poison" );
	words = "judicandus sausabru";
	cost  = 5 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "uncurse" ) || !str_prefix( arg, "curse" ) )
    {
	spell = spell_remove_curse; 
	sn    = skill_lookup( "remove curse" );
	words = "candussido judifgz";
	cost  = 10 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "mana" ) || !str_prefix( arg, "energize" ) )
    {
        spell = NULL;
        sn = -1;
        words = "energizer";
        cost = 2 * FRACT_PER_COPPER;
    }

    else if ( !str_prefix( arg, "refresh" ) || !str_prefix( arg, "moves" ) )
    {
	spell =  spell_refresh;
	sn    = skill_lookup( "refresh" );
	words = "candusima"; 
	cost  = 1 * FRACT_PER_COPPER;
    }

     else if ( !str_prefix( arg, "cancel" ) )
    {
	spell = spell_cancellation;
	sn = skill_lookup( "cancellation" );
	words = "cancilesca";
	cost  = 10 * FRACT_PER_COPPER;
    }

    else 
    {
	act_color( AT_SAY, "`W$N says '`CType 'heal' for a list of spells.`W'",
	    ch, NULL, mob, TO_CHAR, POS_RESTING );
	return;
    }

    if ( cost > CASH_VALUE( ch->money ) )
    {
	act_color( AT_SAY,
		   "`W$N says '`CYou do not have enough coin for that.`W'",
		   ch, NULL, mob, TO_CHAR, POS_RESTING );
	return;
    }

    WAIT_STATE( ch, PULSE_VIOLENCE );

    deduct_cost( ch, cost );
    amt.gold   = 0;
    amt.silver = 0;
    amt.copper = cost;
    amt.fract  = 0;
    money_add( &mob->money, &amt, TRUE );
    act( "$n utters the words '$T'.", mob, NULL, words, TO_ROOM );

    if ( spell == NULL )  /* restore mana trap...kinda hackish */
    {
	ch->mana += dice( 2, 8 ) + mob->level / 3;
	ch->mana = UMIN( ch->mana, ch->max_mana );
	send_to_char( "A warm glow passes through you.\n\r", ch );
	return;
     }

     if (sn == -1)
	return;

     spell( sn, mob->level, mob, ch, TARGET_CHAR );
}
