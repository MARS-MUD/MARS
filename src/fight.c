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
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"


/*
 * Local functions.
 */
bool	check_blade_barrier	args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_brawl		args ( ( CHAR_DATA *ch ) );
bool 	check_force_shield 	args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	check_static_shield 	args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	check_flame_shield 	args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_assist	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_shield_block     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    dam_message 	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                            int dt, bool immune ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim, 
			    int total_levels ) );
void	make_corpse	args( ( CHAR_DATA *ch ) );
void	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, 
bool secondary ) );
void    mob_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_phase     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_counter   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt ) );
bool    dragonkin       args( ( CHAR_DATA *ch, char *spell_name));
/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void
violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	ch_next	= ch->next;

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
	{
	    check_brawl( ch );
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	}
	else
	    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

	/*
	 * Fun for the whole family!
	 */
	check_assist(ch,victim);

	mprog_hitprcnt_trigger( ch, victim );
	mprog_fight_trigger( ch, victim );
    }

    return;
}

/* for auto assisting */
void
check_assist( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *rch, *rch_next;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
    {
	rch_next = rch->next_in_room;

	if ( IS_AWAKE( rch ) && rch->fighting == NULL )
	{

	    if ( RIDDEN( rch ) == ch || MOUNTED( rch ) == ch )
		multi_hit( rch, victim, TYPE_UNDEFINED );

	    /* quick check for ASSIST_PLAYER */
	    if (!IS_NPC(ch) && IS_NPC(rch) 
	    && xIS_SET( rch->off_flags, ASSIST_PLAYERS )
	    &&  rch->level + 6 > victim->level)
	    {
		do_function(rch, &do_emote, "screams and attacks!");
		multi_hit(rch,victim,TYPE_UNDEFINED);
		continue;
	    }

	    /* PCs next */
	    if (!IS_NPC(ch) || IS_AFFECTED(ch,AFF_CHARM))
	    {
		if ( ( (!IS_NPC(rch) && IS_SET(rch->act,PLR_AUTOASSIST))
		||     IS_AFFECTED(rch,AFF_CHARM)) 
		&&   is_same_group(ch,rch) 
		&&   !is_safe(rch, victim))
		    multi_hit (rch,victim,TYPE_UNDEFINED);
		continue;
	    }

	    /* now check the NPC cases */

 	    if (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM))
	    {
		if ( ( IS_NPC( rch ) && xIS_SET( rch->off_flags, ASSIST_ALL ) )

		||   ( IS_NPC( rch ) && rch->group && rch->group == ch->group )

		||   ( IS_NPC( rch ) && rch->race == ch->race 
		   && xIS_SET( rch->off_flags, ASSIST_RACE ) )

		||   ( IS_NPC( rch ) && xIS_SET( rch->off_flags, ASSIST_ALIGN )
		   &&   ( ( IS_GOOD( rch )    && IS_GOOD( ch ) )
		     ||   ( IS_EVIL( rch )    && IS_EVIL( ch ) )
		     ||   ( IS_NEUTRAL( rch ) && IS_NEUTRAL( ch ) ) ) ) 

		||   ( rch->pIndexData == ch->pIndexData 
		   &&  xIS_SET( rch->off_flags, ASSIST_VNUM ) ) )

	   	{
		    CHAR_DATA *vch;
		    CHAR_DATA *target;
		    int number;

		    if (number_bits(1) == 0)
			continue;

		    target = NULL;
		    number = 0;
		    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
		    {
			if (can_see(rch,vch)
			&&  is_same_group(vch,victim)
			&&  number_range(0,number) == 0)
			{
			    target = vch;
			    number++;
			}
		    }

		    if (target != NULL)
		    {
			do_function(rch, &do_emote, "screams and attacks!");
			multi_hit(rch,target,TYPE_UNDEFINED);
		    }
		}	
	    }
	}
    }
}


void
check_brawl( CHAR_DATA *ch )
{
    CHAR_DATA *	rch;
    CHAR_DATA *	vch;
    CHAR_DATA *	is_fighting[MAX_BRAWLS];
    int		chance;
    int		counter;
    int		to_fight;

    if ( ch->in_room == NULL || !IS_SET( ch->in_room->room_flags, ROOM_BRAWL ) )
        return;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        chance = number_range( 1, 200 );

        if ( rch->fighting == NULL && chance <= 3
        && ( ( !IS_NPC( rch ) && ( !IS_IMMORTAL( rch ) || !IS_SET( rch->act, PLR_HOLYLIGHT ) ) )
          || ( IS_NPC( rch )
            && !IS_SET( rch->act, ACT_GAIN )
            && !IS_SET( rch->act, ACT_TRAIN )
            && !IS_SET( rch->act, ACT_PRACTICE )
            && !IS_SET( rch->act, ACT_ARTIFICER )
            && !IS_SET( rch->act, ACT_IS_CHANGER )
            && ch->spec_fun != spec_cast_adept
            && ch->spec_fun != spec_questmaster
            && !IS_SET( rch->act, ACT_IS_HEALER ) ) )
        &&   IS_AWAKE( rch ) )
        {
            /* make a list of everybody already fighting in the room */
            counter = 0;
            for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
            {
                if ( vch->fighting != NULL && counter < MAX_BRAWLS )
                {
                    is_fighting[counter++] = vch;
                    /* if a mob, put them in the list three times */
                    if ( IS_NPC( vch ) )
                    {
                        if ( counter < MAX_BRAWLS )
                            is_fighting[counter++] = vch;
                        if ( counter < MAX_BRAWLS )
                            is_fighting[counter++] = vch;
                    }
                }
            }
            /* now randomly pick somebody from the list */
            to_fight = number_range( 0, counter - 1 );
            vch = is_fighting[to_fight];

            if ( !IS_NPC( ch ) && !IS_NPC( vch ) )
            {
                /* check pkill rules */
                continue;
            }

            /* resting plauers/mobs less likely to be drawn into fight */
            if ( rch->position <= POS_RESTING && chance > 1 )
                continue;

            /* can't fight what you can't see */
            if ( !can_see( rch, vch ) )
                continue;

            /* not against group members */
            if ( is_same_group( rch, vch ) )
                continue;

	    /* charmed mobs and pets, whether grouped or not */
	    if ( IS_AFFECTED( rch, AFF_CHARM )
	    &&   ( (rch->master == vch )
	    ||   ( vch->master == rch )
	    ||   ( vch->master == rch->master ) ) )
		continue;

	    if ( IS_SET( rch->act, ACT_PET )
	    &&   ( (rch->master == vch )
	    ||   ( vch->master == rch )
	    ||   ( vch->master == rch->master ) ) )
		continue;

	    if ( ( rch->master != NULL || vch->master != NULL )
	    &&   ( rch->master == vch || vch->master == rch || rch->master == vch->master ) )
		continue;

	    if ( vch != NULL )
	    {
		rch->fighting = vch;
		rch->position = POS_FIGHTING;
		act_color( AT_RED, "You find yourself caught up in the brawl and attack $N!",
			   rch, NULL, vch, TO_CHAR, POS_RESTING );
		act_color( AT_RED, "$n finds $mself involved in the brawl and attacks $N.",
			   rch, NULL, vch, TO_NOTVICT, POS_RESTING );
		act_color( AT_RED, "$n finds $mself involved in the brawl and attacks you!",
			   rch, NULL, vch, TO_VICT, POS_RESTING );
            }
	    else
	        bugf( "Check_brawl - person to fight is NULL." );

        }
    }

    return;
}


/*
 * Do one group of attacks.
 */
void
multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int    	chance;
    OBJ_DATA *	dualed;

    /* decrement the wait */
    if (ch->desc == NULL)
	ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
	ch->daze = UMAX(0,ch->daze - PULSE_VIOLENCE); 


    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
	return;

    if (IS_NPC(ch))
    {
	mprog_hitprcnt_trigger( ch, victim );
	mprog_fight_trigger( ch, victim );
	mob_hit(ch,victim,dt);
	return;
    }

    if ( MOUNTED( ch ) != NULL )
    {
	if ( !mount_success( ch, MOUNTED( ch ), FALSE ) )
	{
	    send_to_char( "You fall off your mount as you start your attacks!\n\r", ch );
	    act_color( AT_ACTION, "$n falls off $s mount as $e starts $s attacks.",
		       ch, NULL, NULL, TO_ROOM, POS_RESTING );
	}
    }

    one_hit( ch, victim, dt, FALSE );

    if ( ( dualed = get_eq_char( ch, WEAR_DUAL ) ) != NULL )
    {
	if ( !IS_NPC( ch )
	&&   ch->pcdata->skill[gsn_dual].percent < class_table[ch->class].skill_adept
	&&   get_skill( ch, gsn_dual ) <= number_percent ( ) )
	{
	    if ( !IS_OBJ_STAT( dualed, ITEM_NOREMOVE )
	    &&	 number_bits( 4 ) == 0 ) /* 1 chance in 16 they drop it */
	    {
		act_color( AT_YELLOW, "You drop your $p`X!", ch, dualed, NULL, TO_CHAR, POS_DEAD );
		act_color( AT_YELLOW, "$n drops $s $p`X.", ch, dualed, NULL, TO_ROOM, POS_RESTING );
		unequip_char( ch, dualed );
		check_improve( ch, gsn_dual, FALSE, 1 );
	    }
	    check_improve( ch, gsn_dual, FALSE, 1 );
	}
	else
	{
	    one_hit( ch, victim, dt, TRUE );
	    check_improve( ch, gsn_dual, TRUE, 1 );
	    if ( ch->fighting != victim )
		return;
	}
    }

    if ( ch->fighting != victim )
	return;

    if ( IS_AFFECTED( ch, AFF_HASTE ) )
	one_hit( ch, victim, dt, FALSE );
	
	if ( ch->fighting != victim || dt == gsn_assassinate )
        return;

    if ( ch->fighting != victim || dt == gsn_backstab )
	return;

     if ( ch->fighting != victim || dt == gsn_circle )
     return;
     
//if (dt == gsn_circle)
  //      thac0 -= 10 * (100 - get_skill(ch,gsn_circle));

    chance = get_skill( ch, gsn_second_attack ) / 2;

    if ( IS_AFFECTED( ch, AFF_SLOW ) )
	chance /= 2;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch,gsn_second_attack,TRUE,5);
	if ( ch->fighting != victim )
	    return;
    }

    chance = get_skill(ch,gsn_third_attack)/4;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance = 0;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch,gsn_third_attack,TRUE,6);
	if ( ch->fighting != victim )
	    return;
    }

    chance = get_skill(ch,gsn_fourth_attack)/6;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance = 0;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	//send_to_char("`Rfourth attack", ch);
	//send_to_char("`Rfourth attack", victim);
	check_improve(ch,gsn_fourth_attack,TRUE,7);
	if (ch->fighting != victim )
	    return;
    }

    chance = get_skill(ch,gsn_fifth_attack)/6;

    if ( IS_AFFECTED( ch, AFF_SLOW ) )
	chance = 0;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	//send_to_char("`Rfifth attack", ch);
	//send_to_char("`Rfifth attack", victim);
	check_improve(ch,gsn_fifth_attack,TRUE,7);
	if( ch->fighting != victim )
	    return;
    }

    if ( MOUNTED( ch ) )
    {
        chance = get_skill( ch, gsn_warmount );

        if ( number_percent( ) < chance - 10 )
        {
	    one_hit( ch->mount, victim, dt, FALSE );
	    send_to_char( "`Rwar mount`", ch );
	    check_improve( ch, gsn_warmount, TRUE, 7 );
	    if ( ch->fighting != victim )
		return;
	    ch->mount->fighting = NULL; // we dont want the mount to attack/kill steal, just do one line of damage.
	}
	else
	    check_improve( ch, gsn_warmount, FALSE, 1 );
    }

    return;
}


/* procedure for all mobile attacks */
void
mob_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int		chance;
    int		number;
    CHAR_DATA *	vch;
    CHAR_DATA *	vch_next;

    one_hit( ch, victim, dt, FALSE );

    if ( ch->fighting != victim )
	return;

    /* Area attack -- BALLS nasty! */

    if ( xIS_SET( ch->off_flags, OFF_AREA_ATTACK ) )
    {
	for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
	{
	    vch_next = vch->next_in_room;
	    if ((vch != victim && vch->fighting == ch))
		one_hit(ch,vch,dt, FALSE );
	}
    }

    if  ( IS_AFFECTED( ch, AFF_HASTE ) 
    ||  ( xIS_SET( ch->off_flags, OFF_FAST ) && !IS_AFFECTED( ch, AFF_SLOW ) ) )
	one_hit( ch, victim, dt, FALSE );
    if ( ch->fighting != victim || dt == gsn_assassinate )
        return;

    if ( ch->fighting != victim || dt == gsn_backstab )
	return;

    chance = get_skill( ch, gsn_second_attack ) / 2;

    if ( IS_AFFECTED( ch, AFF_SLOW ) && !xIS_SET( ch->off_flags, OFF_FAST ) )
	chance /= 2;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	if ( ch->fighting != victim )
	    return;
    }

    chance = get_skill( ch, gsn_third_attack ) / 4;

    if ( IS_AFFECTED( ch, AFF_SLOW ) && !xIS_SET( ch->off_flags, OFF_FAST ) )
	chance = 0;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	if ( ch->fighting != victim )
	    return;
    }

    chance = get_skill( ch, gsn_fourth_attack ) / 6;

    if ( IS_AFFECTED( ch, AFF_SLOW ) && !xIS_SET( ch->off_flags, OFF_FAST ) )
	chance = 0; 

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	if ( ch->fighting != victim )
	    return;
    }

    chance = get_skill( ch, gsn_fifth_attack ) / 6;

    if ( IS_AFFECTED( ch, AFF_SLOW ) && !xIS_SET( ch->off_flags, OFF_FAST ) )
        chance = 0; 

    if ( number_percent( ) < chance )
    {
        one_hit( ch, victim, dt, FALSE );
        if ( ch->fighting != victim )
            return;
    }  

    /* oh boy!  Fun stuff! */

    if ( ch->wait > 0 )
	return;

    number = number_range( 0,2 );

    if ( number == 1 && IS_SET( ch->pIndexData->class, MCLASS_MAGE ) )
    {
	/*  { mob_cast_mage(ch,victim); return; } */ ;
    }

    if ( number == 2 && IS_SET( ch->pIndexData->class, MCLASS_CLERIC ) )
    {	
	/* { mob_cast_cleric(ch,victim); return; } */ ;
    }

    /* now for the skills */

    number = number_range( 0, 8 );

    switch ( number )
    {
    case (0) :
	if ( xIS_SET( ch->off_flags, OFF_BASH ) )
	    do_function( ch, &do_bash, "" );
	break;

    case (1) :
	if ( xIS_SET( ch->off_flags, OFF_BERSERK ) && !IS_AFFECTED( ch, AFF_BERSERK ) )
	    do_function( ch, &do_berserk, "" );
	break;


    case (2) :
	if ( xIS_SET( ch->off_flags, OFF_DISARM ) 
	|| ( get_weapon_sn( ch ) != gsn_hand_to_hand 
	&& ( IS_SET( ch->pIndexData->class, MCLASS_FIGHTER )
	||  IS_SET (ch->pIndexData->class, MCLASS_ROGUE ) ) ) )
	    do_function( ch, &do_disarm, "" );
	break;

    case (3) :
	if ( xIS_SET( ch->off_flags, OFF_KICK ) )
	    do_function( ch, &do_kick, "" );
	break;

    case (4) :
	if ( xIS_SET( ch->off_flags, OFF_KICK_DIRT ) )
	    do_function( ch, &do_dirt, "" );
	break;

    case (5) :
	if ( xIS_SET( ch->off_flags, OFF_TAIL ) )
	{
	    /* do_function( ch, &do_tail, "" ) */ ;
	}
	break; 

    case (6) :
	if ( xIS_SET( ch->off_flags, OFF_TRIP ) )
	    do_function( ch, &do_trip, "" );
	break;

    case (7) :
	if ( xIS_SET( ch->off_flags, OFF_CRUSH ) )
	{
	    /* do_function( ch, &do_crush, "" ) */ ;
	}
	break;
    case (8) :
	if ( xIS_SET( ch->off_flags, OFF_BACKSTAB ) )
	{
	    do_function( ch, &do_backstab, "" );
	}
    }
}


/*
 * Hit one guy once.
 */
void
one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_99;
    int dam;
    int diceroll;
    int sn,skill;
    int dam_type;
    bool result;
    int chance;

    sn = -1;


    /* just in case */
    if ( victim == ch || ch == NULL || victim == NULL )
	return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     */
    if ( !secondary )
	wield = get_eq_char( ch, WEAR_WIELD );
    else
	wield = get_eq_char( ch, WEAR_DUAL );

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else 
	    dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
    	if (wield != NULL)
    	    dam_type = attack_table[wield->value[3]].damage;
    	else
    	    dam_type = attack_table[ch->dam_type].damage;
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = 20 + get_weapon_skill(ch,sn);

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC( ch ) )
    {
	thac0_00 = 100;
	thac0_99 =  30;   /* as good as a mage */
	if ( IS_SET( ch->pIndexData->class, MCLASS_MAGE ) )
	    thac0_99 = UMIN( thac0_99, class_table[class_mage].thac0_99 );
	if ( IS_SET( ch->pIndexData->class, MCLASS_CLERIC ) )
	    thac0_99 = UMIN( thac0_99, class_table[class_cleric].thac0_99 );
	if ( IS_SET( ch->pIndexData->class, MCLASS_FIGHTER ) )
	    thac0_99 = UMIN( thac0_99, class_table[class_fighter].thac0_99 );
	if ( IS_SET( ch->pIndexData->class, MCLASS_ROGUE ) )
	    thac0_99 = UMIN( thac0_99, class_table[class_rogue].thac0_99 );
    }
    else
    {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_99 = class_table[ch->class].thac0_99;
    }
    thac0  = interpolate( ch->level, thac0_00, thac0_99 );

    if ( thac0 < 0 )
        thac0 = thac0/2;

    if ( thac0 < -5 )
        thac0 = -5 + (thac0 + 5) / 2;

    thac0 -= GET_HITROLL( ch ) * skill/100;
    thac0 += 5 * ( 100 - skill ) / 100;

    /* Pretty much force backstab and assassinate to hit */
    if ( dt == gsn_assassinate )
        thac0 -= 10 * ( 100 - get_skill( ch, gsn_assassinate ) );

    switch( dam_type )
    {
        case DAM_PIERCE : victim_ac = GET_AC( victim, AC_PIERCE ) / 10; break;
        case DAM_BASH   : victim_ac = GET_AC( victim, AC_BASH   ) / 10; break;
        case DAM_SLASH  : victim_ac = GET_AC( victim, AC_SLASH  ) / 10; break;
        default:          victim_ac = GET_AC( victim, AC_EXOTIC ) / 10; break;
    }; 

    if ( dt == gsn_backstab )
	thac0 -= 10 * ( 100 - get_skill( ch, gsn_backstab ) );

    switch( dam_type )
    {
	case DAM_PIERCE : victim_ac = GET_AC( victim, AC_PIERCE ) / 10;	break;
	case DAM_BASH	: victim_ac = GET_AC( victim, AC_BASH   ) / 10;	break;
	case DAM_SLASH	: victim_ac = GET_AC( victim, AC_SLASH  ) / 10;	break;
	default:	  victim_ac = GET_AC( victim, AC_EXOTIC ) / 10;	break;
    }; 

    if ( victim_ac < -15 )
	victim_ac = ( victim_ac + 15 ) / 5 - 15;

    if ( !can_see( ch, victim ) )
	victim_ac -= 20;

    if ( victim->position < POS_FIGHTING )
	victim_ac += 20;

    if ( victim->position < POS_RESTING )
	victim_ac += 60;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 7 ) ) >= 100 )
	;

    /* put hits on wiznet while testing combat */
    sprintf( log_buf, "%s-->%s: dice %d  to_hit %d  ac %d: %s",
	     PERS( ch, ch ), PERS( victim, victim ),
	     diceroll, thac0, victim_ac,
	     diceroll == 0 || ( diceroll != 99 && diceroll < thac0 - victim_ac )
	     ? "Miss" : "Hit" );
    wiznet( log_buf, NULL, NULL, WIZ_COMBAT, 0, 0 );

    if ( diceroll == 0
    || ( diceroll != 99 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt, dam_type, TRUE );
	tail_chain( );
	return;
    }

    /*
     * Check shields.
     */
    if ( IS_SHIELDED( victim, SHLD_GHOST ) )
        if ( number_percent( ) < 50 )
            spell_harm( skill_lookup( "harm" ), 35, victim, ch, TARGET_CHAR );

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC( ch ) &&  wield == NULL )
	dam = dice( ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE] );
    else
    {
	if ( sn != -1 )
	    check_improve( ch, sn, TRUE,5 );
	if ( wield != NULL )
	{
            dam = ( dice( wield->value[1], wield->value[2] ) + wield->value[5] ) * skill/100;

	    if (get_eq_char(ch,WEAR_SHIELD) == NULL)  /* no shield = more */
		dam = dam * 11/10;


	    /* sharpness! */
	    if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
	    {
		int percent;

		if ((percent = number_percent()) <= (skill / 8))
		    dam = 2 * dam + (dam * 2 * percent / 100);
	    }
	}
	else
	    dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
    }

    /*
     * Bonuses.
     */
    if ( get_skill(ch,gsn_enhanced_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_enhanced_damage))
        {
            check_improve(ch,gsn_enhanced_damage,TRUE,6);
            dam += 2 * ( dam * diceroll/300);
        }
    }

    if ( wield != NULL   &&   IS_OBJ_STAT(wield,ITEM_DEATH) )
        {
        if (victim->alignment > 700)
                dam = (110 * dam) / 100;

        else if (victim->alignment > 350)
                dam = (105*dam) / 100;

        else dam = (102*dam) / 100;
        }


    if ( !IS_AWAKE(victim) )
	dam *= 4;
     else if (victim->position < POS_FIGHTING)
	dam = dam * 6 / 2;
	if ( dt == gsn_assassinate && wield != NULL) 
    {
        if ( wield->value[0] != 2 )
            dam *= 4 + (ch->level / 5); 
        else 
            dam *= 4 + (ch->level / 4);
    }

    dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;
        

    if ( dt == gsn_backstab && wield != NULL) 
    {
    	if ( wield->value[0] != 2 )
	    dam *= 2 + (ch->level / 10); 
	else 
	    dam *= 2 + (ch->level / 8);
    }

    dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;

    if ( dam <= 0 )
	dam = 1;

    //check for a counter
    if ( !check_counter( ch, victim, dam, dt ) )
        result = damage( ch, victim, dam, dt, dam_type, TRUE );
    else
        return;

    /* but do we have a funky weapon? */
    if (result && wield != NULL)
    { 
	int dam;

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON))
	{
	    int level;
	    AFFECT_DATA *poison, af;

	    if ((poison = affect_find(wield->affected,gsn_poison)) == NULL)
		level = wield->level;
	    else
		level = poison->level;

	    if (!saves_spell(level / 2,victim,DAM_POISON)) 
	    {
		send_to_char("You feel poison coursing through your veins.",
		    victim);
		act("$n is poisoned by the venom on $p.",
		    victim,wield,NULL,TO_ROOM);

    		af.where     = TO_AFFECTS;
    		af.type      = gsn_poison;
    		af.level     = level * 3/4;
    		af.duration  = level / 2;
    		af.location  = APPLY_STR;
    		af.modifier  = -1;
    		af.bitvector = AFF_POISON;
    		affect_join( victim, &af );
	    }

	    /* weaken the poison if it's temporary */
	    if (poison != NULL)
	    {
	    	poison->level = UMAX(0,poison->level - 2);
	    	poison->duration = UMAX(0,poison->duration - 1);

	    	if (poison->level == 0 || poison->duration == 0)
		    act("The poison on $p has worn off.",ch,wield,NULL,TO_CHAR);
	    }
 	}


    	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
	{
	    dam = number_range(1, wield->level / 5 + 1);
	    act("$p draws life from $n.",victim,wield,NULL,TO_ROOM);
	    act("You feel $p drawing your life away.",
		victim,wield,NULL,TO_CHAR);
	    damage(ch,victim,dam,0,DAM_NEGATIVE,FALSE);
	    ch->alignment = UMAX(-1000,ch->alignment - 1);
	    ch->hit += dam/2;
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
	{
	    dam = number_range(1,wield->level / 4 + 1);
	    act("$n is burned by $p.",victim,wield,NULL,TO_ROOM);
	    act("$p sears your flesh.",victim,wield,NULL,TO_CHAR);
	    fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_FIRE,FALSE);
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST))
	{
	    dam = number_range(1,wield->level / 6 + 2);
	    act("$p freezes $n.",victim,wield,NULL,TO_ROOM);
	    act("The cold touch of $p surrounds you with ice.",
		victim,wield,NULL,TO_CHAR);
	    cold_effect(victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_COLD,FALSE);
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING))
	{
	    dam = number_range(1,wield->level/5 + 2);
	    act("$n is struck by lightning from $p.",victim,wield,NULL,TO_ROOM);
	    act("You are shocked by $p.",victim,wield,NULL,TO_CHAR);
	    shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE);
	}
	 if (ch->fighting == victim && IS_OBJ_STAT(wield,ITEM_DEATH))
	{
            dam = number_range(3,wield->level / 4 + 4);
            act("The evil power of $p torments $n.",victim,wield,NULL,TO_ROOM);
            act("The evil power of $p torments you.",victim,wield,NULL,TO_CHAR);
            damage(ch,victim,dam,0,DAM_NEGATIVE,FALSE);

            if ( !IS_NPC(victim) && ( victim->alignment > 0 )  )
                victim->alignment -= 1;
	}
    /*Vorpal Weapon! */
if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VORPAL))
{
            dam = 350;
            act("$p inflicts massive vorpal damage upon $n.",victim,wield,NULL,TO_ROOM);
            act("$p inflicts massive vorpal damage upon you.",victim,wield,NULL,TO_CHAR);
            damage(ch,victim,dam,dt,DAM_NEGATIVE,TRUE);
}
    
    }
      
    tail_chain( );
    return;
}


/*
 * Inflict damage from a hit.
 */
bool
damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int dam_type,
	     bool show ) 
{
    OBJ_DATA *	corpse;
    bool	immune;
    int		qploss;

    if ( victim->position == POS_DEAD || IS_DEAD( victim ) )
	return FALSE;

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 1200 /* && dt >= TYPE_HIT */)
    {
	char buf[MAX_INPUT_LENGTH];

	sprintf( buf, "$N hitting %s for %d hit points with dt %d!",
		 PERS( victim, ch ), dam, dam_type );
	wiznet( buf, ch, NULL, WIZ_DAMAGE, 0, 0 );
//	bug( "Damage: %d: more than 1200 points!", dam );
	dam = 12000;
	if (!IS_IMMORTAL(ch))
	{
	    OBJ_DATA *obj;
	    obj = get_eq_char( ch, WEAR_WIELD );
	    send_to_char("You really shouldn't cheat.\n\r",ch);
	    if (obj != NULL)
	    	extract_obj(obj);
	}

    }

    /*
     * Un-hide anybody who might be taking damage.
     */
    xREMOVE_BIT( victim->affected_by, AFF_HIDE );

    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return FALSE;
	check_killer( ch, victim );

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    if (victim->timer <= 4)
	    	victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );
    }

    /*
     * Inviso attacks ... not.
     */
    if ( IS_AFFECTED( ch, AFF_INVISIBLE ) )
    {
	affect_strip( ch, gsn_invis );
	affect_strip( ch, gsn_mass_invis );
	xREMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC( victim ) 
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
	dam = 9 * dam / 10;

    if ( dam > 1 && IS_AFFECTED( victim, AFF_DANCING ) )
	dam += dam / 2;

    if ( dam > 1 && IS_SHIELDED( victim, SHLD_SANCTUARY ) )
	dam /= 2;

	if ( dam > 1 && IS_SHIELDED( victim, SHLD_BLACK_MANTLE ) )
	dam /= 2;

    if ( dam > 1 && ( ( IS_SHIELDED( victim, SHLD_PROTECT_EVIL ) && IS_EVIL( ch ) )
    ||		      ( IS_SHIELDED( victim, SHLD_PROTECT_GOOD ) && IS_GOOD( ch ) ) ) )
	dam -= dam / 4;

    if ( IS_SHIELDED( victim, SHLD_GHOST ) )
        dam -= dam / 10;

    immune = FALSE;


    /*
     * Check for parry, and dodge.
     */
    if ( dt >= TYPE_HIT && ch != victim)
    {
        if ( check_parry( ch, victim ) )
	    return FALSE;
	if ( check_dodge( ch, victim ) )
	    return FALSE;
	if ( check_phase( ch, victim) )
            return FALSE;
	if ( check_shield_block(ch,victim))
	    return FALSE;
	if ( IS_SHIELDED( victim, SHLD_FORCE ) && check_force_shield( ch, victim ) )
	    return FALSE;
	if ( IS_SHIELDED( victim, SHLD_STATIC ) && check_static_shield( ch, victim ) )
	    return FALSE;

    }
// If taking damage, check if attacker gets burned -- Tandon

    if ( IS_SHIELDED( victim, SHLD_FLAME ) && dam_type <= 3 )
	check_flame_shield( ch, victim );

    check_blade_barrier( ch, victim );

    switch( check_immune( victim, dam_type ) )
    {
	case IS_IMMUNE:
	    immune = TRUE;
	    dam = 0;
	    break;
	case IS_RESISTANT:	
	    dam -= dam/3;
	    break;
	case IS_VULNERABLE:
	    dam += dam/2;
	    break;
    }

    if ( show )
    	dam_message( ch, victim, dam, dt, immune );

    if ( dam == 0 )
	return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( " ", victim, NULL, NULL, TO_ROOM );
	act_color( AT_RED, "$n is DEAD!!\n\r", victim, 0, 0, TO_ROOM, POS_RESTING );
	send_to_char( "\n\r`RYou have been KILLED!!`X\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "That really did HURT!\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "You sure are BLEEDING!\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		ch->in_room->vnum );
	    log_string( log_buf );

	    /*
	     * Dying penalty:
	     * 2/3 way back to previous level.
	     */
	    if ( victim->exp > exp_per_level(victim,victim->pcdata->points) 
			       * victim->level )
	    gain_exp( victim, (2 * (exp_per_level(victim,victim->pcdata->points)
			         * victim->level - victim->exp)/3) + 50 );

	    /* Quest point loss for death */
	    if ( victim->level < LEVEL_HERO )
	    {
		/* Yeah, this sets the QP loss to the same thing either way.
		   I did it this way in case we want to tweak it later.
		   If we do change it, remove this comment. */
		if ( IS_SET( ch->act, PLR_QUESTOR ) )
		{
		    qploss = number_range( 25, 50 );
		}
		else
		{
		    qploss = number_range( 25, 50 );
		}
		qploss = UMIN( victim->questpoints, qploss );
		victim->questpoints -= qploss;
		if ( qploss )
		    log_qp( victim, qploss, QP_LOSS );
	    }
	}

        sprintf( log_buf, "%s got toasted by %s at %s [room %d]",
            (IS_NPC(victim) ? victim->short_descr : victim->name),
            (IS_NPC(ch) ? ch->short_descr : ch->name),
            ch->in_room->name, ch->in_room->vnum);

        if (IS_NPC(victim))
            wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
        else
            wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);

	raw_kill( ch, victim, dam );
        /* dump the flags */
        if (ch != victim && !IS_NPC(ch) && !is_same_clan(ch,victim))
        {
            if (IS_SET(victim->act,PLR_KILLER))
                REMOVE_BIT(victim->act,PLR_KILLER);
            else
                REMOVE_BIT(victim->act,PLR_THIEF);
        }

        /* RT new auto commands */

	if (!IS_NPC(ch)
	&&  (corpse = get_obj_list(ch,"corpse",ch->in_room->contents)) != NULL
	&&  corpse->item_type == ITEM_CORPSE_NPC && can_see_obj(ch,corpse))
	{
	    OBJ_DATA *coins;

	    corpse = get_obj_list( ch, "corpse", ch->in_room->contents ); 

	    if ( IS_SET(ch->act, PLR_AUTOLOOT) &&
		 corpse && corpse->contains) /* exists and not empty */
            {
		do_function(ch, &do_get, "all corpse");
	    }

 	    if ( IS_SET( ch->act,PLR_AUTOCOINS ) &&
	        corpse && corpse->contains  && /* exists and not empty */
		!IS_SET(ch->act,PLR_AUTOLOOT))
	    {
		if ((coins = get_obj_list(ch,"gcash",corpse->contains))
		     != NULL)
		{
		    do_function(ch, &do_get, "all.gcash corpse");
	      	}
	    }

	    if (IS_SET(ch->act, PLR_AUTOSAC))
	    {
       	        if (IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains)
       	      	{
		    return TRUE;  /* leave if corpse has treasure */
	      	}
	        else
		{
		    do_function(ch, &do_sacrifice, "corpse");
		}
	    }
	}

	return TRUE;
    }

    if ( victim == ch )
	return TRUE;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    recall_char(victim, "", 0 );
	    return TRUE;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
	&&   victim->hit < victim->max_hit / 5) 
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	{
	    do_function(victim, &do_flee, "" );
	}
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait < PULSE_VIOLENCE / 2 )
    {
	do_function (victim, &do_flee, "" );
    }

    tail_chain( );
    return TRUE;
}


bool
is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->in_room == NULL || ch->in_room == NULL )
	return TRUE;

    if ( IS_DEAD( ch ) )
    {
        send_to_char( "Spirits may not harm others.\n\r", ch );
        return TRUE;
    }

    if ( IS_DEAD( victim ) )
    {
        send_to_char( "Spirits cannot be harmed.\n\r", ch );
        return TRUE;
    }

    if ( victim->fighting == ch || victim == ch )
	return FALSE;


    if ( sys_pkill != 0 && !IS_NPC( ch ) && !IS_NPC( victim ) )
    {
        if ( sys_pkill > MAX_LEVEL )
        {
            send_to_char( "Player killing is not allowed.\n\r", ch );
            return TRUE;
        }

        if ( ch->level < sys_pkill )
        {
            send_to_char( "You are of too low a level to do that.\n\r", ch );
            return TRUE;
        }

        if ( IS_IMMORTAL( ch ) )
            return FALSE;

        if ( victim->level < sys_pkill )
        {
            act( "$E is safe from the likes of you.", ch, NULL, victim, TO_CHAR );
            return TRUE;
        }
    }

    if ( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_PEACE ) || IS_AFFECTED( victim, AFF_PEACE ) )
    {
	send_to_char( "A wave of peace overcomes you.\n\r", ch );
	return TRUE;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) || IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
        act( "$E is safe from the likes of you.", ch, NULL, victim, TO_CHAR );
        return TRUE;
    }

    /* killing mobiles */
    if ( IS_NPC( victim ) )
    {

	/* safe room? */
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	{
	    send_to_char("Not in this room.\n\r",ch);
	    return TRUE;
	}

	if (victim->pIndexData->pShop != NULL)
	{
	    send_to_char("The shopkeeper wouldn't like that.\n\r",ch);
	    return TRUE;
	}

	/* no killing healers, trainers, etc */
	if ( IS_SET( victim->act,ACT_TRAIN )
	||   IS_SET( victim->act, ACT_GAIN )
	||   IS_SET( victim->act,ACT_PRACTICE )
	||   IS_SET( victim->act,ACT_IS_HEALER )
	||   IS_SET( victim->act,ACT_IS_CHANGER ) )
	{
	    send_to_char( "I don't think the gods would approve.\n\r", ch );
	    return TRUE;
	}

	if ( !IS_NPC( ch ) )
	{
	    /* no pets */
	    if ( IS_SET( victim->act, ACT_PET ) )
	    {
		act("But $N looks so cute and cuddly...",
		    ch,NULL,victim,TO_CHAR);
		return TRUE;
	    }

	    /* no charmed creatures unless owner */
	    if (IS_AFFECTED(victim,AFF_CHARM) && ch != victim->master)
	    {
		send_to_char("You don't own that monster.\n\r",ch);
		return TRUE;
	    }
	}
    }


	/* killing players */
    else
    {
	/* NPC doing the killing */
	if ( IS_NPC( ch ) )
	{
	    /* safe room check */
	    if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
	    {
		send_to_char( "Not in this room.\n\r", ch );
		return TRUE;
	    }

	    /* charmed mobs and pets cannot attack players while owned */
	    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
	    &&  ch->master->fighting != victim)
	    {
		send_to_char("Players are your friends!\n\r",ch);
		return TRUE;
	    }
	}
	/* player doing the killing */
	else
	{
	    if ( ch->level <= LEVEL_NEWBIE )
	    {
	        send_to_char( "You are of too low a level to do that.\n\r", ch );
	        return TRUE;
            }
            if ( victim->level <= LEVEL_NEWBIE )
            {
                act( "Leave $M alone.", ch, NULL, victim, TO_CHAR );
                return TRUE;
            }
	    if ( get_event_char( ch, EVENT_CHAR_NOPK ) != NULL )
	    {
		send_to_char( "Not yet.\n\r", ch );
		return TRUE;
	    }
	    if ( get_event_char( victim, EVENT_CHAR_NOPK ) != NULL )
	    {
		send_to_char( "Not yet.\n\r", ch );
		return TRUE;
	    }
	    if (IS_SET(victim->act,PLR_KILLER) || IS_SET(victim->act,PLR_THIEF))
		return FALSE;

	    if (ch->level > victim->level + 8)
	    {
		send_to_char("Pick on someone your own size.\n\r",ch);
		return TRUE;
	    }
	}
    }
    return FALSE;
}


bool
is_safe_spell(CHAR_DATA *ch, CHAR_DATA *victim, bool area )
{
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim == ch && area)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL && !area)
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {
	/* safe room? */
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	    return TRUE;

	if (victim->pIndexData->pShop != NULL)
	    return TRUE;

	/* no killing healers, trainers, etc */
	if ( IS_SET( victim->act,ACT_TRAIN )
	||   IS_SET( victim->act, ACT_GAIN )
	||   IS_SET( victim->act,ACT_PRACTICE )
	||   IS_SET( victim->act,ACT_IS_HEALER )
	||   IS_SET( victim->act,ACT_IS_CHANGER ) )
	    return TRUE;

	if (!IS_NPC(ch))
	{
	    /* no pets */
	    if (IS_SET(victim->act,ACT_PET))
	   	return TRUE;

	    /* not player's own mount */
	    if ( MOUNTED( ch ) == victim )
		return TRUE;

	    /* no charmed creatures unless owner */
	    if (IS_AFFECTED(victim,AFF_CHARM) && (area || ch != victim->master))
		return TRUE;

	    /* legal kill? -- cannot hit mob fighting non-group member */
	    if (victim->fighting != NULL && !is_same_group(ch,victim->fighting))
		return TRUE;
	}
	else
	{
	    /* area effect spells do not hit other mobs */
	    if (area && !is_same_group(victim,ch->fighting))
		return TRUE;
	}
    }
    /* killing players */
    else
    {
	if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL)
	    return TRUE;

	/* NPC doing the killing */
	if (IS_NPC(ch))
	{
	    /* charmed mobs and pets cannot attack players while owned */
	    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
	    &&  ch->master->fighting != victim)
		return TRUE;

	    /* safe room? */
	    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
		return TRUE;

	    /* legal kill? -- mobs only hit players grouped with opponent*/
	    if (ch->fighting != NULL && !is_same_group(ch->fighting,victim))
		return TRUE;
	}

	/* player doing the killing */
	else
	{
	    if ( get_event_char( ch, EVENT_CHAR_NOPK ) != NULL
	    ||	 get_event_char( victim, EVENT_CHAR_NOPK ) != NULL )
		return TRUE;
	    if (!is_clan(ch))
		return TRUE;

	    if (IS_SET(victim->act,PLR_KILLER) || IS_SET(victim->act,PLR_THIEF))
		return FALSE;

	    if (!is_clan(victim))
		return TRUE;

	    if (ch->level > victim->level + 8)
		return TRUE;
	}

    }
    return FALSE;
}
/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	victim = victim->master;

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim)
    ||   IS_SET(victim->act, PLR_KILLER)
    ||   IS_SET(victim->act, PLR_THIEF))
	return;

    /*
     * Charm-o-rama.
     */
    if ( xIS_SET(ch->affected_by, AFF_CHARM) )
    {
	if ( ch->master == NULL )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Check_killer: %s bad AFF_CHARM",
		IS_NPC(ch) ? ch->short_descr : ch->name );
	    bug( buf, 0 );
	    affect_strip( ch, gsn_charm_person );
	    xREMOVE_BIT( ch->affected_by, AFF_CHARM );
	    return;
	}
/*
	send_to_char( "*** You are now a KILLER!! ***\n\r", ch->master );
  	SET_BIT(ch->master->act, PLR_KILLER);
*/

	stop_follower( ch );
	return;
    }

    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * And current killers stay as they are.
     */
    if ( IS_NPC(ch)
    ||   ch == victim
    ||   ch->level >= LEVEL_IMMORTAL
    ||   !is_clan(ch)
    ||   IS_SET(ch->act, PLR_KILLER) 
    ||	 ch->fighting  == victim)
	return;

    send_to_char( "*** You are now a KILLER!! ***\n\r", ch );
    SET_BIT(ch->act, PLR_KILLER);
    sprintf(buf,"$N is attempting to murder %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
    save_char_obj( ch );
    return;
}



/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    chance = get_skill(victim,gsn_parry) / 2;

    if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
    {
	if (IS_NPC(victim))
	    chance /= 2;
	else
	    return FALSE;
    }

    if (!can_see(ch,victim))
	chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
	return FALSE;

    act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
    act( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_parry,TRUE,6);
    return TRUE;
}


/*
*check for Phase
*/
bool check_phase( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( !IS_NPC( victim ) && victim->pcdata->skill[gsn_phase].percent < 2 )
	return FALSE;

    chance = get_skill(victim,gsn_phase) / 2;

    if (!can_see(victim,ch))
	chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    act( "`YYour body phases to avoid $n's attack.`X", ch, NULL, victim, TO_VICT    );
    act( "`Y$N's body phases to avoid your attack.`X", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_phase,TRUE,6);
    return TRUE;
}


/*
*Deathgrip
*/

void do_deathgrip( CHAR_DATA *ch, char *argument )
{
        int sn;
        OBJ_DATA *obj;
        AFFECT_DATA af;

        sn = skill_lookup("deathgrip");

        if ( IS_AFFECTED(ch,AFF_DEATHGRIP) )
        {
                send_to_char("You already have a grip of death.\n\r",ch);
                return;
        }

        if ( get_skill(ch,sn) < 1 )
        {
                send_to_char("What's that?\n\r",ch);
                return;
        }

        if (  get_skill(ch,sn)  <  (number_range(0, 100))  )
        {
                send_to_char("You failed to create a grip of death.\n",ch);
                check_improve(ch,sn,FALSE,1);
                return;
        }

        obj = get_eq_char(ch,WEAR_WIELD);

        if ( (obj != NULL) && !IS_OBJ_STAT(obj,ITEM_DEATH) )
        {
                SET_BIT( obj->extra_flags, ITEM_DEATH);
                act("$p flickers with dark power.",ch,obj,NULL,TO_ALL);
        }

       /* Now for adding the affect to the player */

        af.where        = TO_AFFECTS;
        af.type         = sn;          
        af.level        = ch->level;
        af.duration     = ch->level / 3;
        af.location     = APPLY_DAMROLL;
        af.modifier     = ch->level / 8;
        af.bitvector    = AFF_DEATHGRIP;

        affect_to_char(ch, &af);

        act("`z$n's hands are shrouded with a black mist.`X",ch,NULL,NULL,TO_ROOM);
        send_to_char("`zYour hands are shrouded with a black mist.`X\n",ch);

        check_improve(ch,sn,TRUE,1);
}

/*
 * Check for shield block.
 */
bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
        return FALSE;


    chance = get_skill(victim,gsn_shield_block) / 5 + 3;


    if ( get_eq_char( victim, WEAR_SHIELD ) == NULL )
        return FALSE;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    act( "You block $n's attack with your shield.",  ch, NULL, victim, 
TO_VICT    );
    act( "$N blocks your attack with a shield.", ch, NULL, victim, 
TO_CHAR    );
    check_improve(victim,gsn_shield_block,TRUE,6);
    return TRUE;
}


/*
 * Check for dodge.
 */
bool
check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( MOUNTED( victim ) != NULL )
	return FALSE;

    chance = get_skill(victim,gsn_dodge) / 2;

    if (!can_see(victim,ch))
	chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
    act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_dodge,TRUE,6);
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC(victim) && victim->hit < 1 )
    {
	victim->position = POS_DEAD;
	return;
    }

    if ( victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void
set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( ( IS_NPC( ch ) && ch->pIndexData->vnum == MOB_VNUM_SUPERMOB )
    ||	 ( IS_NPC( victim ) && victim->pIndexData->vnum == MOB_VNUM_SUPERMOB ) )
        return;

    if ( IS_AFFECTED( ch, AFF_SLEEP ) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= IS_NPC(fch) ? fch->default_pos : POS_STANDING;
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void
make_corpse( CHAR_DATA *ch )
{
    char	buf[MAX_STRING_LENGTH];
    MONEY	cash;
    OBJ_DATA *	corpse;
    int		weight;
    char *	name;

    if ( ch->weight == 0 )
        weight = number_range( race_table[ch->race].weight_min, race_table[ch->race].weight_max );
    else
        weight = ch->weight;

    if ( IS_NPC( ch ) )
    {
	if ( ch->pIndexData->vnum == MOB_VNUM_CORPSE )
	    return;

	if ( IS_SET( ch->form, FORM_INSTANT_DECAY ) )
	    return;

	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	cash = ch->money;
	if ( cash.gold > 0 || cash.silver > 0 || cash.copper > 0 || cash.fract > 0 )
	{
	    obj_to_obj( create_money( &ch->money ), corpse );
	    ch->money.gold = 0;
	    ch->money.silver = 0;
	    ch->money.copper = 0;
	    ch->money.fract = 0;
	}
	corpse->cost = 0;
	corpse->weight = weight;
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->value[5]= ch->id;
	REMOVE_BIT( ch->act, PLR_CANLOOT );
	if ( !is_clan( ch ) )
	    corpse->owner = str_dup( ch->name );
	else
	{
	    cash.gold = ch->money.gold / 2;
	    cash.silver = ch->money.silver / 2;
	    cash.copper = ch->money.copper / 2;
	    cash.fract = ch->money.fract / 2;
	    corpse->owner = NULL;
	    if ( cash.gold > 0 || cash.silver > 0 || cash.copper > 0 || cash.fract > 0 )
	    {
		obj_to_obj( create_money( &cash ), corpse);
		ch->money.gold -= cash.gold;
		ch->money.silver -= cash.silver;
		ch->money.copper -= cash.copper;
		ch->money.fract -= cash.fract;
	    }
	}

	corpse->cost = 0;
	corpse->weight = weight;
	sprintf( buf, "corpse %s", name );
	free_string( corpse->name );
	corpse->name = str_dup( buf );
    }

    corpse->level = ch->level;

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

/* for animate dead */
    if ( ( IS_NPC( ch ) ) && ( !IS_SET( ch->act, ACT_UNDEAD ) ) )
        corpse->value[5] = ch->pIndexData->vnum;


    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    /* Players keep gear in the rift */
    if ( IS_NPC( ch ) )
    {
        stuff_corpse( ch, corpse );
    }
    else
    {
        /* We want the corpse to go away eventually, no matter what. */
        corpse->timer = number_range( 2820, 2940 );
    }

    obj_to_room( corpse, ch->in_room );
    add_obj_fall_event( corpse );
}


void
stuff_corpse( CHAR_DATA *ch, OBJ_DATA *corpse )
{
    OBJ_DATA *	obj;
    OBJ_DATA *	obj_next;

    if ( IS_NPC( ch ) )
	corpse->timer	= number_range( 3, 6 );
    else
	corpse->timer	= number_range( 25, 40 );


    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	bool floating = FALSE;

	obj_next = obj->next_content;
	if (obj->wear_loc == WEAR_FLOAT)
	    floating = TRUE;
	obj_from_char( obj );
	if (obj->item_type == ITEM_POTION)
	    obj->timer = number_range(500,1000);
	if (obj->item_type == ITEM_SCROLL)
	    obj->timer = number_range(1000,2500);
	if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH) && !floating)
	{
	    obj->timer = number_range(5,10);
	    REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
	}
	REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);

	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    extract_obj( obj );
     if (IS_SET (obj->extra_flags, ITEM_QUESTOBJ))
    		obj_to_char (obj, ch);
	else if (floating)
	{
	    if (IS_OBJ_STAT(obj,ITEM_ROT_DEATH)) /* get rid of it! */
	    { 
		if (obj->contains != NULL)
		{
		    OBJ_DATA *in, *in_next;

		    act("$p evaporates,scattering its contents.",
			ch,obj,NULL,TO_ROOM);
		    for (in = obj->contains; in != NULL; in = in_next)
		    {
			in_next = in->next_content;
			obj_from_obj(in);
			obj_to_room(in,ch->in_room);
			add_obj_fall_event( obj );
		    }
		 }
		 else
		    act("$p evaporates.",
			ch,obj,NULL,TO_ROOM);
		 extract_obj(obj);
	    }
	    else
	    {
		act_color( AT_ACTION, "$p falls to the floor.", ch, obj, NULL, TO_ROOM, POS_RESTING );
		obj_to_room( obj, ch->in_room );
		add_obj_fall_event( obj );
	    }
	}
	else
	    obj_to_obj( obj, corpse );
    }

    return;
}


/*
 * Improved Death_cry contributed by Diavolo, enhancements from Fallon.
 */
void
death_cry( CHAR_DATA *ch, int dam )
{
    OBJ_DATA *		obj;
    char *		msg;
    int			door;
    int			vnum;
    bool		body[16];
    int			count;
    int			i;
    int			num;
    bool		fMsg;
    bool		smash;

    for ( i = 0; i < 16; i++ )
        body[i] = FALSE;

    if ( dam > ch->max_hit / 4 )
    {
        smash = TRUE;
        act_color( AT_ACTION, "$n's body flies apart as $e is `RSMASHED`X to pieces!",
                   ch, NULL, NULL, TO_ROOM, POS_RESTING );
    }
    else
        smash = FALSE; 

    fMsg = FALSE;
    count = 0;

    do
    {
        vnum = 0;
        msg = NULL;

        do
            num = number_bits( 4 );
        while ( body[num] );
        body[num] = TRUE;

        switch ( num )
        {
        case  0: 
            if ( ch->material == NULL )
            {
                msg  = "$n splatters blood on your armor.";		
            }
            break;

        case 1:
            if ( IS_SET( ch->parts, PART_TAIL ) )
            {
                msg = "$n's tail is torn off and lies there, with a final twitch.";
                vnum = OBJ_VNUM_TAIL;
            }
            break;

        case  2: 							
            if ( IS_SET( ch->parts, PART_GUTS ) )
            {
                msg = "$n spills $s guts all over the floor.";
                vnum = OBJ_VNUM_GUTS;
            }
            break;

        case  3: 
            if ( IS_SET( ch->parts, PART_HEAD ) )
            {
                msg  = "$n's severed head plops on the ground.";
                vnum = OBJ_VNUM_SEVERED_HEAD;				
            }
            break;

        case  4: 
            if ( IS_SET( ch->parts, PART_HEART ) )
            {
                msg  = "$n's heart is torn from $s chest.";
                vnum = OBJ_VNUM_TORN_HEART;				
            }
            break;

        case  5: 
            if ( IS_SET( ch->parts, PART_ARMS ) )
            {
                msg  = "$n's arm is sliced from $s dead body.";
                vnum = OBJ_VNUM_SLICED_ARM;				
            }
            break;
        case  6: 
            if ( IS_SET( ch->parts, PART_LEGS ) )
            {
                msg  = "$n's leg is sliced from $s dead body.";
                vnum = OBJ_VNUM_SLICED_LEG;				
            }
            break;

        case 7:
            if ( IS_SET( ch->parts, PART_BRAINS ) )
            {
                msg = "$n's head is shattered, and $s brains splash all over you.";
                vnum = OBJ_VNUM_BRAINS;
            }
            break;
        case 8:
            if ( IS_SET( ch->parts, PART_WINGS ) )
            {
                msg = "$n's wings are ripped from $s dead body.";
                vnum = OBJ_VNUM_WINGS;
            }
            break;
        }

        if ( msg != NULL )
        {
            act_color( AT_ACTION, msg, ch, NULL, NULL, TO_ROOM, POS_RESTING );
            fMsg = TRUE;
        }

        if ( vnum != 0 )
        {
            char buf[MAX_STRING_LENGTH];
            OBJ_DATA *obj;
            char *name;

            name	= IS_NPC( ch ) ? ch->short_descr : ch->name;
            obj		= create_object( get_obj_index( vnum ), 0 );
            obj->timer	= number_range( 4, 7 );

            sprintf( buf, obj->short_descr, name );
            free_string( obj->short_descr );
            obj->short_descr = str_dup( buf );

            sprintf( buf, obj->description, name );
            free_string( obj->description );
            obj->description = str_dup( buf );

            if (obj->item_type == ITEM_FOOD)
            {
                if (IS_SET(ch->form,FORM_POISON))
                    obj->value[3] = 1;
                else if (!IS_SET(ch->form,FORM_EDIBLE))
                    obj->item_type = ITEM_TRASH;
            }

            obj_to_room( obj, ch->in_room );
            add_obj_fall_event( obj );
        }

        count++;
        if ( number_bits( 2 ) == 0 || count > 6 )
            smash = FALSE;

    } while ( smash );

    if ( !fMsg )
        act_color( AT_ACTION, "$n hits the ground... and doesn't get up.",
                   ch, NULL, NULL, TO_ROOM, POS_RESTING );

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
        if ( ( obj->pIndexData->vnum == OBJ_VNUM_BLOOD || obj->pIndexData->vnum == OBJ_VNUM_BLOODSTAIN )
        &&   obj->timer > 0 )
            break;

    if ( IS_NPC( ch ) && !IS_SET( ch->act, ACT_UNDEAD ) )
    {
        if ( obj == NULL )
        {
            obj = create_object( get_obj_index( is_water( ch->in_room ) ? OBJ_VNUM_BLOODSTAIN : OBJ_VNUM_BLOOD ), 1 );
            obj->timer = 0;
            obj_to_room( obj, ch->in_room );
            add_obj_fall_event( obj );
        }

        obj->timer += number_range( 3, 5 );
    }

    if ( IS_NPC( ch) )
	msg = "You hear something's death cry.\n\r";
    else
	msg = "You hear someone's death cry.\n\r";

    for ( door = 0; door < MAX_DIR; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   pexit->to_room != ch->in_room )
	{
	    act_color( AT_ACTION, msg, pexit->to_room->people, NULL, NULL,
	               TO_ROOM, POS_RESTING );
	}
    }

    return;
}


void
raw_kill( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
    int i;

    stop_fighting( victim, TRUE );

    if ( IS_NPC( victim) )
	mprog_death_trigger( victim, ch, dam );
    else
	death_cry( victim, dam );

    rprog_death_trigger( victim->in_room, victim );
    make_corpse( victim );

    if ( IS_NPC( victim ) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE( 0, victim->level, MAX_LEVEL-1 )].killed++;
	extract_char( victim, TRUE );
	return;
    }

    strip_event_char( victim, EVENT_CHAR_HEAL );
    victim->pcdata->deathroom = victim->in_room->vnum;
    extract_char( victim, FALSE );
    while ( victim->affected != NULL )
	affect_remove( victim, victim->affected );
    victim->affected_by	= race_table[victim->race].aff;
    for ( i = 0; i < 4; i++ )
    	victim->armor[i]= 100;
    victim->position	= POS_RESTING;
    victim->hit		= UMAX( 1, victim->hit  );
    victim->mana	= UMAX( 1, victim->mana );
    victim->move	= UMAX( 1, victim->move );
    die_pc( victim );
/*  save_char_obj( victim ); we're stable enough to not need this :) */
    return;
}


void
group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( victim == ch )
	return;

    members = 0;
    group_levels = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
        {
	    members++;
	    group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
	}
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
	group_levels = ch->level ;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) || IS_NPC(gch))
	    continue;
	     if(IS_SET(gch->act, PLR_NOEXP))
        {
        send_to_char("You have NOEXP turned on! You gained no exp!\n\r",ch);
        xp = 0;
        }

/*	Taken out, add it back if you want it
	if ( gch->level - lch->level >= 5 )
	{
	    send_to_char( "You are too high for this group.\n\r", gch );
	    continue;
	}

	if ( gch->level - lch->level <= -5 )
	{
	    send_to_char( "You are too low for this group.\n\r", gch );
	    continue;
	}
*/

	xp = xp_compute( gch, victim, group_levels );  
	sprintf( buf, "You receive %d experience points.\n\r", xp );
	send_to_char( buf, gch );
	gain_exp( gch, xp );
/* random item generation. It works, but needs too muc h work right now. 
 * I'll fuck with it later.
 */
 /* 	 
	  if (gch == ch)
        {
            if (number_percent () < 50)
                iGen (gch, victim, number_range (UMAX (1, victim->level - 3),
                                                 UMIN (victim->level + 5, 50)));
            if (number_percent () < 30)
                iGen (gch, victim, number_range (UMAX (1, victim->level - 3),
                                                 UMIN (victim->level + 5, 50)));
            if (number_percent () < 15)
                iGen (gch, victim, number_range (UMAX (1, victim->level - 3),
                                                 UMIN (victim->level + 5, 50)));
            if (number_percent () < 5)
                iGen (gch, victim, number_range (UMAX (1, victim->level - 3),
                                                 UMIN (victim->level + 5, 50)));
        }
        else
        {
            if (number_percent () < 20)
                iGen (gch, victim, number_range (UMAX (1, victim->level - 3),
                                                 UMIN (victim->level + 5, 50)));
            if (number_percent () < 5)
                iGen (gch, victim, number_range (UMAX (1, victim->level - 3),
                                                 UMIN (victim->level + 5, 50)));
        }
*/
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	    {
		act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
		act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		add_obj_fall_event( obj );
	    }
	}
    }

    if ( !IS_NPC( ch )
	&& IS_SET( ch->act, PLR_QUESTOR )
	&& IS_NPC( victim )
	&& ch->questmob == victim->pIndexData->vnum )
    {
	send_to_char( "You have almost completed your QUEST!\n\r", ch );
	send_to_char( "Return to the questmaster before your time runs out!\n\r", ch );
	ch->questmob = -1;
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels )
{
    int xp,base_exp;
    int align,ethos,level_range;
    int change;
    int time_per_level;

    level_range = victim->level - gch->level;

    /* compute the base exp */
    switch (level_range)
    {
 	default : 	base_exp =   0;		break;
	case -9 :	base_exp =   1;		break;
	case -8 :	base_exp =   2;		break;
	case -7 :	base_exp =   5;		break;
	case -6 : 	base_exp =   9;		break;
	case -5 :	base_exp =  11;		break;
	case -4 :	base_exp =  22;		break;
	case -3 :	base_exp =  33;		break;
	case -2 :	base_exp =  50;		break;
	case -1 :	base_exp =  66;		break;
	case  0 :	base_exp =  83;		break;
	case  1 :	base_exp =  99;		break;
	case  2 :	base_exp = 121;		break;
	case  3 :	base_exp = 143;		break;
	case  4 :	base_exp = 165;		break;
    } 

    if (level_range > 4)
	base_exp = 160 + 20 * (level_range - 4);
      if(IS_NPC(victim) )/* at max a mob with all worth 5 level above his own*/
       {
         if( is_affected( victim, skill_lookup("sanctuary") ) )
            base_exp = (base_exp * 130) / 100;
            if( is_affected( victim, skill_lookup("black mantle") ) )
            base_exp = (base_exp * 130) / 100;
         if( is_affected( victim, skill_lookup("haste") ) )
            base_exp = (base_exp * 120) / 100;
         if( xIS_SET(victim->off_flags,OFF_AREA_ATTACK) ) 
            base_exp = (base_exp * 120) / 100;
         if( xIS_SET(victim->off_flags,OFF_BACKSTAB) ) 
            base_exp = (base_exp * 120) / 100;
         if( xIS_SET(victim->off_flags,OFF_FAST) )
            base_exp = (base_exp * 120) / 100;
         if( xIS_SET(victim->off_flags,OFF_DODGE) )
            base_exp = (base_exp * 110) / 100;
         if( xIS_SET(victim->off_flags,OFF_PARRY) )
            base_exp = (base_exp * 110) / 100;
         if( xIS_SET(victim->off_flags,AFF_REGENERATION) )
            base_exp = (base_exp * 110) / 100;
         if( xIS_SET(victim->off_flags,AFF_BERSERK) )
            base_exp = (base_exp * 110) / 100;

         if( victim->spec_fun != 0 )
          {
           if(   !str_cmp(spec_name(victim->spec_fun),"spec_breath_any")
              || !str_cmp(spec_name(victim->spec_fun),"spec_breath_acid")
              || !str_cmp(spec_name(victim->spec_fun),"spec_breath_fire")
              || !str_cmp(spec_name(victim->spec_fun),"spec_breath_frost")
              || !str_cmp(spec_name(victim->spec_fun),"spec_breath_gas")
              || !str_cmp(spec_name(victim->spec_fun),"spec_breath_lightning")
              || !str_cmp(spec_name(victim->spec_fun),"spec_nasty")
             )
             base_exp = (base_exp * 125) / 100;
           
           else if(   !str_cmp(spec_name(victim->spec_fun),"spec_cast_cleric")
                   || !str_cmp(spec_name(victim->spec_fun),"spec_cast_mage")
                   || !str_cmp(spec_name(victim->spec_fun),"spec_cast_undead")
                   || !str_cmp(spec_name(victim->spec_fun),"spec_thief")
                  ) 
                base_exp = (base_exp * 120) / 100;
         
           else if( !str_cmp(spec_name(victim->spec_fun),"spec_poison") )
                base_exp = (base_exp * 110) / 100;
          }
       }

    /* do alignment computations */

    align = victim->alignment - gch->alignment;

    if (IS_SET(victim->act,ACT_NOALIGN))
    {
	/* no change */
    }

    else if (align > 500) /* monster is more good than slayer */
    {
	change = (align - 500) * base_exp / 500 * gch->level/total_levels; 
	change = UMAX(1,change);
        gch->alignment = UMAX(-1000,gch->alignment - change);
    }

    else if (align < -500) /* monster is more evil than slayer */
    {
	change =  ( -1 * align - 500) * base_exp/500 * gch->level/total_levels;
	change = UMAX(1,change);
	gch->alignment = UMIN(1000,gch->alignment + change);
    }

    else /* improve this someday */
    {
	change =  gch->alignment * base_exp/500 * gch->level/total_levels;  
	gch->alignment -= change;
    }
    
    /* calculate exp multiplier */
    if (IS_SET(victim->act,ACT_NOALIGN))
	xp = base_exp;

    else if (gch->alignment > 500)  /* for goodie two shoes */
    {
	if (victim->alignment < -750)
	    xp = (base_exp *4)/3;

 	else if (victim->alignment < -500)
	    xp = (base_exp * 5)/4;

        else if (victim->alignment > 750)
	    xp = base_exp / 4;

   	else if (victim->alignment > 500)
	    xp = base_exp / 2;

        else if (victim->alignment > 250)
	    xp = (base_exp * 3)/4; 

	else
	    xp = base_exp;
    }

    else if (gch->alignment < -500) /* for baddies */
    {
	if (victim->alignment > 750)
	    xp = (base_exp * 5)/4;

  	else if (victim->alignment > 500)
	    xp = (base_exp * 11)/10; 

   	else if (victim->alignment < -750)
	    xp = base_exp/2;

	else if (victim->alignment < -500)
	    xp = (base_exp * 3)/4;

	else if (victim->alignment < -250)
	    xp = (base_exp * 9)/10;

	else
	    xp = base_exp;
    }

    else if (gch->alignment > 200)  /* a little good */
    {

	if (victim->alignment < -500)
	    xp = (base_exp * 6)/5;

 	else if (victim->alignment > 750)
	    xp = base_exp/2;

	else if (victim->alignment > 0)
	    xp = (base_exp * 3)/4; 

	else
	    xp = base_exp;
    }

    else if (gch->alignment < -200) /* a little bad */
    {
	if (victim->alignment > 500)
	    xp = (base_exp * 6)/5;

	else if (victim->alignment < -750)
	    xp = base_exp/2;

	else if (victim->alignment < 0)
	    xp = (base_exp * 3)/4;

	else
	    xp = base_exp;
    }

    else /* neutral */
    {

	if (victim->alignment > 500 || victim->alignment < -500)
	    xp = (base_exp * 4)/3;

	else if (victim->alignment < 200 && victim->alignment > -200)
	    xp = base_exp/2;

 	else
	    xp = base_exp;
    }

    /* more exp at the low levels */
    if (gch->level < 6)
    	xp = 10 * xp / (gch->level + 4);

    /* less at high */
    if (gch->level > 35 )
	xp =  15 * xp / (gch->level - 25 );

/* do ethos computations */

    ethos = victim->ethos - gch->ethos;

    if (IS_SET(victim->act,ACT_NOALIGN))
    {
    /* no change */
    }

    else if (ethos > 500) /* monster is more lawful than slayer */
    {
    change = (ethos - 500) * base_exp / 500 * gch->level/total_levels; 
    change = UMAX(1,change);
        gch->ethos = UMAX(-1000,gch->ethos - change);
    }

    else if (ethos < -500) /* monster is more chaotic than slayer */
    {
    change =  ( -1 * ethos - 500) * base_exp/500 * gch->level/total_levels;
    change = UMAX(1,change);
    gch->ethos = UMIN(1000,gch->ethos + change);
    }

    else /* improve this someday */
    {
    change =  gch->ethos * base_exp/500 * gch->level/total_levels;  
    gch->ethos -= change;
    }
    
    /* calculate exp multiplier */
    if (IS_SET(victim->act,ACT_NOALIGN))
    xp = base_exp;

    else if (gch->ethos > 500)  /* for the very lawful */
    {
    if (victim->ethos < -750)
        xp = (base_exp *4)/3;

    else if (victim->ethos < -500)
        xp = (base_exp * 5)/4;

        else if (victim->ethos > 750)
        xp = base_exp / 4;

    else if (victim->ethos > 500)
        xp = base_exp / 2;

        else if (victim->ethos > 250)
        xp = (base_exp * 3)/4; 

    else
        xp = base_exp;
    }

    else if (gch->ethos < -500) /* for chaos */
    {
    if (victim->ethos > 750)
        xp = (base_exp * 5)/4;

    else if (victim->ethos > 500)
        xp = (base_exp * 11)/10; 

    else if (victim->ethos < -750)
        xp = base_exp/2;

    else if (victim->ethos < -500)
        xp = (base_exp * 3)/4;

    else if (victim->ethos < -250)
        xp = (base_exp * 9)/10;

    else
        xp = base_exp;
    }

    else if (gch->ethos > 200)  /* a little lawful */
    {

    if (victim->ethos < -500)
        xp = (base_exp * 6)/5;

    else if (victim->ethos > 750)
        xp = base_exp/2;

    else if (victim->ethos > 0)
        xp = (base_exp * 3)/4; 

    else
        xp = base_exp;
    }

    else if (gch->ethos < -200) /* a little chaotic */
    {
    if (victim->ethos > 500)
        xp = (base_exp * 6)/5;

    else if (victim->ethos < -750)
        xp = base_exp/2;

    else if (victim->ethos < 0)
        xp = (base_exp * 3)/4;

    else
        xp = base_exp;
    }

    else /* neutral */
    {

    if (victim->ethos > 500 || victim->ethos < -500)
        xp = (base_exp * 4)/3;

    else if (victim->ethos < 200 && victim->ethos > -200)
        xp = base_exp/2;

    else
        xp = base_exp;
    }

    /* more exp at the low levels */
    if (gch->level < 6)
        xp = 10 * xp / (gch->level + 4);

    /* less at high 
    if (gch->level > 35 )
    xp =  15 * xp / (gch->level - 25 );

*/
    /* reduce for playing time */
/*
    {
	// compute quarter-hours per level 
	time_per_level = 4 *
			 (gch->played + (int) (current_time - gch->logon))/3600
			 / gch->level;

	time_per_level = URANGE(2,time_per_level,12);
	if (gch->level < 15)  // make it a curve 
	    time_per_level = UMAX(time_per_level,(15 - gch->level));
	xp = xp * time_per_level / 12;
    }
*/
    /* randomize the rewards */
    xp = number_range (xp * 3/4, xp * 5/4);

    /* adjust for grouping */
    xp = xp * gch->level/( UMAX(1,total_levels -1) );

    return xp;
}


void dam_message( CHAR_DATA *ch, CHAR_DATA *victim,int dam,int dt,bool immune )
{
    char buf1[256], buf2[256], buf3[256];
    char dam_p[64], dam_v[64];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;
    double pct = 0;

    if ( victim->max_hit == 0 )
    {
	pct = dam;
    }

    else
    {
	pct = (double)dam/victim->max_hit;
    }

    if (ch == NULL || victim == NULL)
	return;

	 if ( pct ==   0 ) { vs = "`Wmiss`X";		vp = "`Wmisses`X";		}
    else if ( pct <= .05 ) { vs = "`gscratch`X";		vp = "`gscratches`X";	}
    else if ( pct <= .10 ) { vs = "`Ggraze`X";		vp = "`Ggrazes`X";		}
    else if ( pct <= .15 ) { vs = "`rhit`X";		vp = "`rhits`X";		}
    else if ( pct <= .20 ) { vs = "`pinjure`X";		vp = "`pinjures`X";		}
    else if ( pct <= .25 ) { vs = "`Rwound`X";		vp = "`Rwounds`X";		}
    else if ( pct <= .30 ) { vs = "`rmaul`X";   	vp = "`rmauls`X";	}
    else if ( pct <= .35 ) { vs = "`gdecimate`X";	vp = "`gdecimates`X";	}
    else if ( pct <= .40 ) { vs = "`gdevastate`X";	vp = "`gdevastates`X";	}
    else if ( pct <= .45 ) { vs = "`pmaim`X";		vp = "`pmaims`X";	}
    else if ( pct <= .50 ) { vs = "`cMUTILATE`X";	vp = "`cMUTILATES`X";	}
    else if ( pct <= .55 ) { vs = "`cDISEMBOWEL`X";	vp = "`cDISEMBOWELS`X";	}
    else if ( pct <= .60 ) { vs = "`cDISMEMBER`X";	vp = "`cDISMEMBERS`X";	}
    else if ( pct <= .65 ) { vs = "`cMASSACRE`X";	vp = "`cMASSACRES`X";	}
    else if ( pct <= .70 ) { vs = "`cMANGLE`X";		vp = "`cMANGLES`X";	}
    else if ( pct <= .75 ) { vs = "`R*** DEMOLISH ***`X";
			     vp = "`R*** DEMOLISHES ***`X";			}
    else if ( pct <= .80 ) { vs = "`G*** DEVASTATE ***`X";
			     vp = "`G*** DEVASTATES ***`X";			}
    else if ( pct <= .85 ) { vs = "`R=== OBLITERATE ===`X";
			     vp = "`R=== OBLITERATES ===`X";		}
    else if ( pct <= .90)  { vs = "`P>>> ANNIHILATE <<<`X";
			     vp = "`P>>> ANNIHILATES <<<`X";		}
    else if ( pct <= .95)  { vs = "`B<<< ERADICATE >>>`X";
			     vp = "`B<<< ERADICATES >>>`X";		}
    else                   { vs = "`Ydo UNSPEAKABLE things to`X";
			     vp = "`Ydoes UNSPEAKABLE things to`X";	}

    punct   = (dam <= .50) ? '.' : '!';

    dam_p[0] = '\0';
    dam_v[0] = '\0';
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTODAMAGEDEALT ) && dam > 0 )
	sprintf( dam_p, " `Y[`R%d pt%s`X`Y]`X", dam, dam == 1 ? "" : "s" );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_AUTODAMAGEDEALT ) && dam > 0 )
	sprintf( dam_v, " `Y[`R%d pt%s`Y]`X", dam, dam == 1 ? "" : "s" );

    if ( dt == TYPE_HIT )
    {
	if (ch  == victim)
	{
	    sprintf( buf1, "$n %s $mself%c",vp,punct);
	    sprintf( buf2, "You %s yourself%c%s", vs, punct, dam_p );
	}
	else
	{
	    sprintf( buf1, "$n %s $N%c",  vp, punct );
	    sprintf( buf2, "You %s $N%c%s", vs, punct, dam_p );
	    sprintf( buf3, "$n %s you%c%s", vp, punct, dam_v );
	}
    }
    else
    {
	if ( dt >= 0 && dt < top_skill )
	    attack	= skill_table[dt].noun_damage;
	else if ( dt >= TYPE_HIT
	&& dt < TYPE_HIT + MAX_DAMAGE_MESSAGE) 
	    attack	= attack_table[dt - TYPE_HIT].noun;
	else
	{
	    bug( "Dam_message: bad dt %d.", dt );
	    dt  = TYPE_HIT;
	    attack  = attack_table[0].name;
	}

	if (immune)
	{
	    if (ch == victim)
	    {
		sprintf( buf1, "$n is unaffected by $s own %s`X.", attack );
		sprintf( buf2, "Luckily, you are immune to that." );
	    } 
	    else
	    {
	    	sprintf( buf1, "$N is unaffected by $n's %s`X!", attack );
	    	sprintf( buf2, "$N is unaffected by your %s`X!", attack );
	    	sprintf( buf3, "$n's %s`X is powerless against you.", attack );
	    }
	}
	else
	{
	    if (ch == victim)
	    {
		sprintf( buf1, "$n's %s`X %s $m%c", attack, vp, punct );
		sprintf( buf2, "Your %s %s you%c%s", attack, vp, punct, dam_p );
	    }
	    else
	    {
	    	sprintf( buf1, "$n's %s`X %s $N%c",  attack, vp, punct );
	    	sprintf( buf2, "Your %s`X %s $N%c%s",  attack, vp, punct, dam_p );
	    	sprintf( buf3, "$n's %s`X %s you%c%s", attack, vp, punct, dam_v );
	    }
	}
    }

    if ( ch == victim )
    {
	act( buf1, ch, NULL, NULL, TO_ROOM );
	act( buf2, ch, NULL, NULL, TO_CHAR );
    }
    else
    {
    	act( buf1, ch, NULL, victim, TO_NOTVICT );
    	act( buf2, ch, NULL, victim, TO_CHAR );
    	act( buf3, ch, NULL, victim, TO_VICT );
    }

    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void
disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	return;

    if ( IS_OBJ_STAT(obj,ITEM_NOREMOVE))
    {
	act("$S weapon won't budge!",ch,NULL,victim,TO_CHAR);
	act("$n tries to disarm you, but your weapon won't budge!",
	    ch,NULL,victim,TO_VICT);
	act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
	return;
    }

    act_color( AT_YELLOW, "$n DISARMS you and sends your weapon flying!", 
	 ch, NULL, victim, TO_VICT, POS_RESTING );
    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n disarms $N!",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_OBJ_STAT(obj,ITEM_NODROP) || IS_OBJ_STAT(obj,ITEM_INVENTORY) )
	obj_to_char( obj, victim );
    else
    {
	obj_to_room( obj, victim->in_room );
	add_obj_fall_event( obj );
	if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
	    get_obj(victim,obj,NULL);
    }

    return;
}

void do_berserk( CHAR_DATA *ch, char *argument)
{
    int		chance;
    int		hp_percent;

    if ( ( chance = get_skill( ch, gsn_berserk ) ) == 0
    ||   ( IS_NPC( ch ) && !xIS_SET( ch->off_flags, OFF_BERSERK ) )
    ||   ( !IS_NPC( ch )
    &&   ch->level < skill_table[gsn_berserk].skill_level[ch->class] ) )
    {
	send_to_char( "You turn red in the face, but nothing happens.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_BERSERK ) || is_affected( ch, gsn_berserk )
    ||   is_affected( ch, skill_lookup( "frenzy" ) ) )
    {
	send_to_char( "You get a little madder.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CALM ) )
    {
	send_to_char( "You're feeling too mellow to berserk.\n\r", ch );
	return;
    }

    if ( ch->mana < 50 )
    {
	send_to_char( "You can't get up enough energy.\n\r", ch );
	return;
    }

    /* modifiers */

    /* fighting */
    if ( ch->position == POS_FIGHTING )
	chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit/ch->max_hit;
    chance += 25 - hp_percent/2;

    if ( number_percent( ) < chance )
    {
	AFFECT_DATA af;

	WAIT_STATE( ch, PULSE_VIOLENCE );
	ch->mana -= 50;
	ch->move /= 2;

	/* heal a little damage */
	ch->hit += ch->level * 2;
	ch->hit = UMIN( ch->hit, ch->max_hit );

	send_to_char( "Your pulse races as you are consumed by rage!\n\r", ch );
	act( "$n gets a wild look in $s eyes.", ch, NULL, NULL, TO_ROOM );
	check_improve( ch, gsn_berserk, TRUE, 2 );

	af.where	= TO_AFFECTS;
	af.type		= gsn_berserk;
	af.level	= ch->level;
	af.duration	= number_fuzzy( ch->level / 8 );
	af.modifier	= UMAX( 1, ch->level / 5 );
	af.bitvector 	= AFF_BERSERK;

	af.location	= APPLY_HITROLL;
	affect_to_char( ch, &af );

	af.location	= APPLY_DAMROLL;
	affect_to_char( ch, &af );

	af.modifier	= UMAX( 10, 10 * ( ch->level / 5 ) );
	af.location	= APPLY_AC;
	affect_to_char( ch, &af );
    }

    else
    {
	WAIT_STATE( ch,3 * PULSE_VIOLENCE );
	ch->mana -= 25;
	ch->move /= 2;

	send_to_char( "Your pulse speeds up, but nothing happens.\n\r", ch );
	check_improve( ch, gsn_berserk, FALSE, 2 );
    }
}


void
do_bash( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    int		chance;

    one_argument( argument, arg );

    if ( ( chance = get_skill( ch, gsn_bash ) ) == 0
    ||	 ( IS_NPC( ch ) && !xIS_SET( ch->off_flags, OFF_BASH ) )
    ||	 ( !IS_NPC( ch )
    &&	  ch->level < skill_table[gsn_bash].skill_level[ch->class] ) )
    {	
	send_to_char( "Bashing? What's that?\n\r", ch );
	return;
    }

    if (arg[0] == '\0')
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

    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't bash while riding.\n\r", ch );
	return;
    }

    if ( victim->position < POS_FIGHTING )
    {
	act( "You'll have to let $M get back up first.", ch, NULL, victim, TO_CHAR );
	return;
    } 

    if ( victim == ch )
    {
	send_to_char( "You try to bash your brains out, but fail.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_NPC( victim ) && 
	victim->fighting != NULL && 
	!is_same_group( ch, victim->fighting ) )
    {
        send_to_char( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	act( "But $N is your friend!", ch, NULL, victim, TO_CHAR );
	return;
    }

    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;

    if ( ch->size < victim->size )
	chance += ( ch->size - victim->size ) * 15;
    else
	chance += ( ch->size - victim->size ) * 10; 


    /* stats */
    chance += get_curr_stat( ch, STAT_STR );
    chance -= ( get_curr_stat( victim, STAT_DEX ) * 4 ) / 3;
    chance -= GET_AC( victim, AC_BASH ) / 25;
    /* speed */
    if ( xIS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ) )
        chance += 10;
    if ( xIS_SET( victim->off_flags, OFF_FAST ) || IS_AFFECTED( victim, AFF_HASTE ) )
        chance -= 30;

    /* level */
    chance += ( ch->level - victim->level );

    if ( !IS_NPC( victim )
	&& chance < get_skill( victim, gsn_dodge ) )
    {	/*
        act("$n tries to bash you, but you dodge it.",ch,NULL,victim,TO_VICT);
        act("$N dodges your bash, you fall flat on your face.",ch,NULL,victim,TO_CHAR);
        WAIT_STATE(ch,skill_table[gsn_bash].beats);
        return;*/
	chance -= 3 * ( get_skill( victim, gsn_dodge ) - chance );
    }

    /* now the attack */
    if ( number_percent() < chance )
    {
	act("$n sends you sprawling with a powerful bash!",
		ch,NULL,victim,TO_VICT);
	act("You slam into $N, and send $M flying!",ch,NULL,victim,TO_CHAR);
	act("$n sends $N sprawling with a powerful bash.",
		ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_bash,TRUE,1);

	DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
	WAIT_STATE(ch,skill_table[gsn_bash].beats);
	victim->position = POS_RESTING;
	damage(ch,victim,number_range(2,2 + 2 * ch->size + chance/20),gsn_bash,
	    DAM_BASH,FALSE);

	if ( RIDDEN( victim ) != NULL )
	{
	    mount_success( RIDDEN( victim ), victim, FALSE );
	}
    }
    else
    {
	damage(ch,victim,0,gsn_bash,DAM_BASH,FALSE);
	act("You fall flat on your face!",
	    ch,NULL,victim,TO_CHAR);
	act("$n falls flat on $s face.",
	    ch,NULL,victim,TO_NOTVICT);
	act("You evade $n's bash, causing $m to fall flat on $s face.",
	    ch,NULL,victim,TO_VICT);
	check_improve(ch,gsn_bash,FALSE,1);
	ch->position = POS_RESTING;
	WAIT_STATE(ch,skill_table[gsn_bash].beats * 3/2); 
    }
	check_killer(ch,victim);
}

void
do_dirt( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    int		chance;

    one_argument( argument, arg );

    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't kick dirt while riding.\n\r", ch );
	return;
    }

    if ( ( chance = get_skill( ch, gsn_dirt ) ) == 0
    ||   ( IS_NPC( ch ) && !xIS_SET( ch->off_flags, OFF_KICK_DIRT ) )
    ||   ( !IS_NPC( ch )
    &&    ch->level < skill_table[gsn_dirt].skill_level[ch->class] ) )
    {
	send_to_char( "You get your feet dirty.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	victim = ch->fighting;
	if ( victim == NULL )
	{
	    send_to_char( "But you aren't in combat!\n\r", ch );
	    return;
	}
    }

    else if ( ( victim = get_char_room( ch,arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( victim, AFF_BLIND ) )
    {
	act( "$E's already been blinded.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Very funny.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_NPC( victim ) &&
	 victim->fighting != NULL && 
	!is_same_group( ch, victim->fighting ) )
    {
        send_to_char( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	act( "But $N is such a good friend!", ch, NULL, victim, TO_CHAR );
	return;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat( ch, STAT_DEX );
    chance -= 2 * get_curr_stat( victim, STAT_DEX );

    /* speed  */
    if ( xIS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ) )
	chance += 10;
    if ( xIS_SET( victim->off_flags, OFF_FAST ) || IS_AFFECTED( victim, AFF_HASTE ) )
	chance -= 25;

    /* level */
    chance += ( ch->level - victim->level ) * 2;

    /* sloppy hack to prevent false zeroes */
    if ( chance % 5 == 0 )
	chance += 1;

    /* terrain */

    switch ( ch->in_room->sector_type )
    {
	case(SECT_INSIDE):		chance -= 20;	break;
	case(SECT_CITY):		chance -= 10;	break;
	case(SECT_FIELD):		chance +=  5;	break;
	case(SECT_FOREST):				break;
	case(SECT_HILLS):				break;
	case(SECT_MOUNTAIN):		chance -= 10;	break;
	case(SECT_WATER_SWIM):		chance  =  0;	break;
	case(SECT_WATER_NOSWIM):	chance  =  0;	break;
	case(SECT_AIR):			chance  =  0;  	break;
	case(SECT_DESERT):		chance += 10;   break;
	case(SECT_BEACH):		chance += 10;   break;
    }

    if ( chance == 0 )
    {
	send_to_char("There isn't any dirt to kick.\n\r",ch);
	return;
    }

    /* now the attack */
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	act("$n is blinded by the dirt in $s eyes!",victim,NULL,NULL,TO_ROOM);
	act("$n kicks dirt in your eyes!",ch,NULL,victim,TO_VICT);
        damage(ch,victim,number_range(2,5),gsn_dirt,DAM_NONE,FALSE);
	send_to_char("You can't see a thing!\n\r",victim);
	check_improve(ch,gsn_dirt,TRUE,2);
	WAIT_STATE(ch,skill_table[gsn_dirt].beats);

	af.where	= TO_AFFECTS;
	af.type 	= gsn_dirt;
	af.level 	= ch->level;
	af.duration	= 0;
	af.location	= APPLY_HITROLL;
	af.modifier	= -4;
	af.bitvector 	= AFF_BLIND;

	affect_to_char(victim,&af);
    }
    else
    {
	damage(ch,victim,0,gsn_dirt,DAM_NONE,TRUE);
	check_improve(ch,gsn_dirt,FALSE,2);
	WAIT_STATE(ch,skill_table[gsn_dirt].beats);
    }
	check_killer(ch,victim);
}

void do_blackjack( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);
/*
    if ( (chance = get_skill(ch,gsn_blackjack)) == 0)
    {
	send_to_char("You cant blackjack.\n\r",ch);
	return;
    }
*/
    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't in combat!\n\r",ch);
	    return;
	}
    }


    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(victim,AFF_SLEEP))
    {
	act("$E's already been blackjacked.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("Very funny.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
	return;
    }


    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;

    if (chance == 0)
    {
	send_to_char("You failed horrbly!.\n\r",ch);
	return;
    }

    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	act("$n is knocked out cold!",victim,NULL,NULL,TO_ROOM);
	act("$n wacks you upside the head!",ch,NULL,victim,TO_VICT);
/*
        damage(ch,victim,number_range(2,5),gsn_blackjack,DAM_NONE,FALSE);
*/
	send_to_char("You are knocked out cold!\n\r",victim);
	check_improve(ch,gsn_blackjack,TRUE,2);

	af.where	= TO_AFFECTS;
	af.type 	= gsn_sleep;
	af.level 	= ch->level;
	af.duration	= 4 + ch->level;
	af.location	= APPLY_NONE;
	af.modifier	= 0;
	af.bitvector 	= AFF_SLEEP;

	affect_to_char(victim,&af);
        victim->position = POS_SLEEPING;

    }
    else
    {
/*
	damage(ch,victim,0,gsn_blackjack,DAM_NONE,TRUE);
*/
	check_improve(ch,gsn_blackjack,FALSE,2);
    }
}


void 
do_whirlwind( CHAR_DATA *ch, char * argument )
{
CHAR_DATA *rch, *rch_next;
OBJ_DATA *wield;
bool found = FALSE;

if ( !IS_NPC( ch )
   && ch->level < skill_table[gsn_whirlwind].skill_level[ch->class] )
{
send_to_char("You dont know how to do that!\n\r",ch);
return;
}

if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
{
send_to_char("You need a weapon first!\n\r",ch);
return;
}


act ( "$n holds $p firmly and starts a spinning move!", ch, wield, NULL,
TO_ROOM );
act ( "You hold $p firmly and start spinning!", ch, wield, NULL, TO_CHAR
);

for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
{
rch_next = rch->next_in_room;   

{
if ( IS_NPC( rch ) )
{
found = TRUE;
act( "$n turns towards you!", ch, NULL, rch, TO_VICT );
one_hit( ch, rch, gsn_whirlwind, TRUE );
}
}
if ( !found )
{
act( "$n looks dizzy and falls!", ch, NULL, NULL, TO_ROOM );
act( "You feel dizzy and fall down!", ch, NULL, NULL, TO_CHAR );
}

WAIT_STATE( ch, skill_table[gsn_whirlwind].beats );

if ( !found && number_percent() < 25 )
{
act("$n loses his balance and falls!",ch, NULL,NULL, TO_ROOM);
act("You lose your balance and fall!",ch, NULL,NULL,TO_CHAR);
}
return;
}
}
void
do_trip( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    int		chance;

    one_argument( argument, arg );

    if ( ( chance = get_skill( ch, gsn_trip ) ) == 0
    ||   ( IS_NPC( ch ) && !xIS_SET( ch->off_flags, OFF_TRIP ) )
    ||   ( !IS_NPC( ch ) 
	  && ch->level < skill_table[gsn_trip].skill_level[ch->class] ) )
    {
	send_to_char("Tripping?  What's that?\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't fighting anyone!\n\r",ch);
	    return;
 	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't trip while riding.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
	send_to_char("Kill stealing is not permitted.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(victim,AFF_FLYING))
    {
	act("$S feet aren't on the ground.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim->position < POS_FIGHTING)
    {
	act("$N is already down.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("You fall flat on your face!\n\r",ch);
	WAIT_STATE(ch,2 * skill_table[gsn_trip].beats);
	act("$n trips over $s own feet!",ch,NULL,NULL,TO_ROOM);
	return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("$N is your beloved master.",ch,NULL,victim,TO_CHAR);
	return;
    }

    /* modifiers */

    /* size */
    if (ch->size < victim->size)
        chance += (ch->size - victim->size) * 10;  /* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= get_curr_stat(victim,STAT_DEX) * 3 / 2;

    /* speed */
    if ( xIS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ) )
	chance += 10;
    if ( xIS_SET( victim->off_flags, OFF_FAST ) || IS_AFFECTED( victim, AFF_HASTE ) )
	chance -= 20;

    /* level */
    chance += (ch->level - victim->level) * 2;


    /* now the attack */
    if (number_percent() < chance)
    {
	act("$n trips you and you go down!",ch,NULL,victim,TO_VICT);
	act("You trip $N and $N goes down!",ch,NULL,victim,TO_CHAR);
	act("$n trips $N, sending $M to the ground.",ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_trip,TRUE,1);

	DAZE_STATE(victim,2 * PULSE_VIOLENCE);
        WAIT_STATE(ch,skill_table[gsn_trip].beats);
	victim->position = POS_RESTING;
	damage(ch,victim,number_range(2, 2 +  2 * victim->size),gsn_trip,
	    DAM_BASH,TRUE);
    }
    else
    {
	damage(ch,victim,0,gsn_trip,DAM_BASH,TRUE);
	WAIT_STATE(ch,skill_table[gsn_trip].beats*2/3);
	check_improve(ch,gsn_trip,FALSE,1);
    } 
	check_killer(ch,victim);
}


void
do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
/*  Allow player killing
    if ( !IS_NPC(victim) )
    {
        if ( !IS_SET(victim->act, PLR_KILLER)
        &&   !IS_SET(victim->act, PLR_THIEF) )
        {
            send_to_char( "You must MURDER a player.\n\r", ch );
            return;
        }
    }
*/
    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}


void
do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) || (IS_NPC(ch) && IS_SET(ch->act,ACT_PET)))
	return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    if (IS_NPC(ch))
	sprintf(buf, "Help! I am being attacked by %s!",ch->short_descr);
    else
    	sprintf( buf, "Help!  I am being attacked by %s!", ch->name );
    do_function(victim, &do_yell, buf );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

void
do_assassinate( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if (arg[0] == '\0')
    {
        send_to_char("Assassinate whom?\n\r",ch);
        return;
    }
    
	if (!IS_AFFECTED(ch, AFF_SNEAK))
	{
		send_to_char("You must be sneaking to assassinate!\n\r", ch);
		return;
	}
	
    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't assassinate someone while riding.\n\r", ch );
	return;
    }

    if (ch->fighting != NULL)
    {
	send_to_char("You are too exposed and conspicuous'\n\r",ch);
	return;
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
	send_to_char( "You need to wield a weapon to assassinate someone.\n\r", ch );
	return;
    }

    if ( victim->hit < victim->max_hit / 3)
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_assassinate].beats );
    if ( number_percent( ) < get_skill(ch,gsn_assassinate)
    || ( get_skill(ch,gsn_assassinate) >= 2 && !IS_AWAKE(victim) ) )
    {
	check_improve(ch,gsn_assassinate,TRUE,1);
	multi_hit( ch, victim, gsn_assassinate );
    }
    else
    {
	check_improve(ch,gsn_assassinate,FALSE,1);
	damage( ch, victim, 0, gsn_assassinate,DAM_NONE,TRUE);
    }

    return;
}

void
do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if (arg[0] == '\0')
    {
        send_to_char("Backstab whom?\n\r",ch);
        return;
    }

    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't backstab while riding.\n\r", ch );
	return;
    }

    if (ch->fighting != NULL)
    {
	send_to_char("You're facing the wrong end.\n\r",ch);
	return;
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
	send_to_char( "You need to wield a weapon to backstab.\n\r", ch );
	return;
    }

    if ( victim->hit < victim->max_hit / 3)
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( number_percent( ) < get_skill(ch,gsn_backstab)
    || ( get_skill(ch,gsn_backstab) >= 2 && !IS_AWAKE(victim) ) )
    {
	check_improve(ch,gsn_backstab,TRUE,1);
	multi_hit( ch, victim, gsn_backstab );
    }
    else
    {
	check_improve(ch,gsn_backstab,FALSE,1);
	damage( ch, victim, 0, gsn_backstab,DAM_NONE,TRUE);
    }

    return;
}


void 
do_circle( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
 
    one_argument( argument, arg );
 
    if (arg[0] == '\0')
    {
        send_to_char("Circle whom?\n\r",ch);
        return;
    }
 
 
    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
 
    if ( is_safe( ch, victim ) )
      return;
 
    if (IS_NPC(victim) &&
         victim->fighting != NULL &&
        !is_same_group(ch,victim->fighting))
 
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }
 
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
        send_to_char( "You need to wield a weapon to circle.\n\r", ch );
        return;
    }
 
    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char( "You must be fighting in order to circle.\n\r", ch );
        return;
    }
 
    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_circle].beats );
    if ( number_percent( ) < get_skill(ch,gsn_circle)
    || ( get_skill(ch,gsn_circle) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_circle,TRUE,1);
        multi_hit( ch, victim, gsn_circle );
    }
    else
    {
        check_improve(ch,gsn_circle,FALSE,1);
        damage( ch, victim, 0, gsn_circle,DAM_NONE,TRUE);
    }
 
    return;
}
 


void
do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	was_in;
    ROOM_INDEX_DATA *	now_in;
    CHAR_DATA *		victim;
    CHAR_DATA *		pet;
    CHAR_DATA *		next_pet;
    int			attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
        if ( ch->position == POS_FIGHTING )
            ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < MAX_FLEE_ATTEMPTS; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = get_exit( was_in, door ) ) == NULL
	||   pexit->to_room == NULL
	||   IS_SET( pexit->exit_info, EX_CLOSED )
	||   number_range( 0, ch->daze ) != 0
	|| ( IS_NPC( ch )
	&&   IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB ) ) )
	    continue;

	move_char( ch, door, FALSE );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	for ( pet = was_in->people; pet != NULL; pet = next_pet )
	{
	    next_pet = pet->next_in_room;
	    if ( IS_NPC( pet )
	    &&	 ( IS_SET( pet->act, ACT_PET ) || IS_SET( pet->act, ACT_MOUNT ) )
	    &&	 pet->master == ch )
	        do_flee( pet, "" );
        }

	if ( !IS_NPC( ch ) )
	{
	    send_to_char( "You flee from combat!\n\r", ch );
	    if ( ( ch->class == class_rogue ) 
	    &&   ( number_percent( ) < 3 * ( ch->level/2 ) ) )
		send_to_char( "You managed to sneak away safely.\n\r", ch );
            else
	    {
	        send_to_char( "You lost 10 exp.\n\r", ch );
	        gain_exp( ch, -10 );
	    }
	}

	stop_fighting( ch, TRUE );
	return;
    }

    send_to_char( "PANIC! You couldn't escape!\n\r", ch );
    return;
}


void
do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( IS_NPC(fch) && !is_same_group(ch,victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( number_percent( ) > get_skill(ch,gsn_rescue))
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	check_improve(ch,gsn_rescue,FALSE,1);
	return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );
    check_improve(ch,gsn_rescue,TRUE,1);

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    check_killer( ch, fch );
    set_fighting( ch, fch );
    set_fighting( fch, ch );
    return;
}


void
do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_kick].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) && !xIS_SET( ch->off_flags, OFF_KICK ) )
	return;

    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't kick while riding.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( get_skill(ch,gsn_kick) > number_percent())
    {
	damage(ch,victim,number_range( 1, ch->level ), gsn_kick,DAM_BASH,TRUE);
	check_improve(ch,gsn_kick,TRUE,1);
    }
    else
    {
	damage( ch, victim, 0, gsn_kick,DAM_BASH,TRUE);
	check_improve(ch,gsn_kick,FALSE,1);
    }
	check_killer(ch,victim);
    return;
}




void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance,hth,ch_weapon,vict_weapon,ch_vict_weapon;

    hth = 0;

    if ((chance = get_skill(ch,gsn_disarm)) == 0)
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL 
    &&   ( ( hth = get_skill( ch, gsn_hand_to_hand ) ) == 0
    ||    ( IS_NPC( ch ) && !xIS_SET( ch->off_flags, OFF_DISARM ) ) ) )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    /* find weapon skills */
    ch_weapon = get_weapon_skill(ch,get_weapon_sn(ch));
    vict_weapon = get_weapon_skill(victim,get_weapon_sn(victim));
    ch_vict_weapon = get_weapon_skill(ch,get_weapon_sn(victim));

    /* modifiers */

    /* skill */
    if ( get_eq_char(ch,WEAR_WIELD) == NULL)
	chance = chance * hth/150;
    else
	chance = chance * ch_weapon/100;

    chance += (ch_vict_weapon/2 - vict_weapon) / 2; 

    /* dex vs. strength */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_STR);

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* and now the attack */
    if (number_percent() < chance)
    {
    	WAIT_STATE( ch, skill_table[gsn_disarm].beats );
	disarm( ch, victim );
	check_improve(ch,gsn_disarm,TRUE,1);
    }
    else
    {
	WAIT_STATE(ch,skill_table[gsn_disarm].beats);
	act("You fail to disarm $N.",ch,NULL,victim,TO_CHAR);
	act("$n tries to disarm you, but fails.",ch,NULL,victim,TO_VICT);
	act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_disarm,FALSE,1);
    }
    check_killer(ch,victim);
    return;
}


void
do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && victim->level >= get_trust(ch) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
    act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
    act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    victim->hit = 1;
    victim->mana = 1;
    raw_kill( ch, victim, victim->max_hit );
    return;
}


bool
check_blade_barrier( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int		chance;

    if ( !IS_SHIELDED( victim, SHLD_BLADES ) )
        return FALSE;

    chance = 40;
    if ( victim->level > ch->level )
        chance += 5;

    if ( number_percent( ) >= chance )
        return FALSE;

    damage( victim, ch, number_range( 25, 35 ), gsn_blade_barrier, DAM_SLASH, TRUE );
    return TRUE;
}


bool
check_force_shield( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int		chance;

    if ( !IS_SHIELDED( victim, SHLD_FORCE ) )
	return FALSE;

    chance = 100 / 15;

    if ( victim->level >= ch->level )
	chance += 2;

    if ( number_percent( ) >= chance )
        return FALSE;

    act( "Your force-shield blocks $n's attack!", ch, NULL, victim, TO_VICT );
    act( "$N's force-shield blocks your attack.", ch, NULL, victim, TO_CHAR );

    return TRUE;
}


/*
 *  Shield Spell Group by Tandon
 *  Static Shield Check
 */
bool
check_static_shield( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int			chance;
    int			sn;
    AFFECT_DATA *	shock;

    if ( !IS_SHIELDED( victim, SHLD_STATIC ) )
	return FALSE;

    chance = 10;

    if ( victim->level >= ch->level )
	chance += 2;

    if ( number_percent( ) >= chance )
        return FALSE;


    sn = skill_lookup( "static shield" );
    shock = affect_find( victim->affected, sn );

    if(shock != NULL)
    {
	damage (victim, ch, number_fuzzy(shock->level / 5), sn, DAM_ENERGY, TRUE);
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL)
	return TRUE;

    act( "Your static shield catches $n!", victim, NULL, ch, TO_VICT );
    act( "$N's static shield catches you!", victim, NULL, ch, TO_CHAR );

//	spell_heat_metal (skill_lookup( "heat metal" ),
//                     victim->level/2, victim, (void *) ch, TARGET_CHAR);

    return TRUE;
}


/*
 *  Shield Spell Group by Tandon
 *  Flame Shield Check
 */
bool
check_flame_shield( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int 		chance;
    int			sn;
    AFFECT_DATA *	burn;

    if ( !IS_SHIELDED( victim, SHLD_FLAME ) )
	return FALSE;

    if ( get_eq_char( victim, WEAR_WIELD ) != NULL )
	return FALSE;

    chance = 100 / 3;

    if ( victim->level >= ch->level )
	chance += 2;

    if ( number_percent( ) >= chance )
        return FALSE;

    sn = skill_lookup( "flame shield" );
    burn = affect_find( victim->affected, sn );

    if ( burn != NULL )
    {   
	fire_effect( ch, burn->level, number_fuzzy(10), TARGET_CHAR );
	damage( victim, ch, number_fuzzy( burn->level ), sn, DAM_FIRE, TRUE );
    }

    return TRUE;
}


bool
check_counter( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    int 	chance;
    int		dam_type;
    OBJ_DATA *	wield;

    if ( ( get_eq_char( victim, WEAR_WIELD ) == NULL )
    ||   ( !IS_AWAKE( victim ) )
    ||   ( !can_see(victim,ch) )
    ||   ( get_skill(victim,gsn_counter) < 1 ) )
    {
        return FALSE;
    }

    wield = get_eq_char( victim, WEAR_WIELD );

    chance = get_skill( victim, gsn_counter ) / 6;
    chance += ( victim->level - ch->level ) / 2;
    chance += 2 * ( get_curr_stat( victim, STAT_DEX ) - get_curr_stat( ch, STAT_DEX ) );
    chance += get_weapon_skill( victim, get_weapon_sn( victim ) ) -
                        get_weapon_skill( ch, get_weapon_sn( ch ) );
    chance += ( get_curr_stat( victim, STAT_STR ) - get_curr_stat( ch, STAT_STR ) );

    if ( number_percent( ) >= chance )
        return FALSE;

        dt = gsn_counter;

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else
            dt += ch->dam_type;
    }

    if ( dt < TYPE_HIT )
    {
    	if (wield != NULL)
    	    dam_type = attack_table[wield->value[3]].damage;
    	else
    	    dam_type = attack_table[ch->dam_type].damage;
    }
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if ( dam_type == -1 )
	dam_type = DAM_BASH;

    act( "You reverse $n's attack and counter with your own!", ch, NULL, victim, TO_VICT    );
    act( "$N reverses your attack!", ch, NULL, victim, TO_CHAR    );

    damage( victim, ch, dam/2, gsn_counter , dam_type ,TRUE ); /* DAM MSG NUMBER!! */

    check_improve( victim, gsn_counter, TRUE, 6 );

    return TRUE;
}


bool
dragonkin( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int sn;

    if ( ch->position != POS_FIGHTING )
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 3 ) == 0 )
        break;
    }

    if ( victim == NULL )
    return FALSE;

    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
    return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}

void do_breathe3(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  int damdice, damtype, sn;
  char target[MAX_STRING_LENGTH];
  char points[MAX_STRING_LENGTH];
  damdice = 0;
  argument = one_argument(argument, target);
  argument = one_argument(argument, points);
  if (target[0] == '\0')
	{
		if ((victim = ch->fighting) == NULL)
		{
	    send_to_char("You must specify a target if not fighting.\n\r",
										ch);
	    return;
		}
  }
	else
	{
		if ((victim = get_char_room(ch, target)) == NULL)
		{
	    send_to_char("They aren't here.\n\r", ch);
	    return;
		}
  }
    if (!ch->race == race_lookup("dragonborn")) 
      {
        send_to_char("You aren't kin of the dragons!", ch);
        return;
          }

    if (IS_NPC(ch))
    {
        send_to_char("NPCs do not do this. Set their special action with spec_breath\n\r", ch);
            return;
    }
    if (ch->race == race_lookup("dragonborn"))

  {
  
	/*if (ch->race == race_lookup("reddrg"))*/
if (ch->pcdata->hair_color == "black scales" )	{
	sn = skill_lookup("acid breath");
	damtype = DAM_ACID;
    } else if (ch->pcdata->hair_color == "copper scales" ) {
	sn = skill_lookup("acid breath");
	damtype = DAM_ACID;
    } else if (ch->pcdata->hair_color == "red scales" ) {
	sn = skill_lookup("fire breath");
	damtype = DAM_FIRE;
    } else if (ch->pcdata->hair_color == "blue scales" ) {
	sn = skill_lookup("frost breath");
	damtype = DAM_COLD;
    } else if (ch->pcdata->hair_color == "green scales" ) {
	sn = skill_lookup("acid breath");
	damtype = DAM_ACID;
    } else if (ch->pcdata->hair_color == "bronze scales" ) {
	sn = skill_lookup("lightning breath");
	damtype = DAM_LIGHTNING;
      } else if (ch->pcdata->hair_color == "white scales" ) {
	sn = skill_lookup("frost breath");
	damtype = DAM_COLD;
      } else if (ch->pcdata->hair_color == "brass scales" ) {
	sn = skill_lookup("fire breath");
	damtype = DAM_FIRE;
    } else if (ch->pcdata->hair_color == "silver scales" ) {
	sn = skill_lookup("lightning breath");
	damtype = number_range( DAM_LIGHTNING, DAM_COLD);
    } else {
	send_to_char("You huff and puff..\r\n", ch);
	return;
    }
  if (!IS_NPC(ch))
	{
		if (is_safe(ch, victim) && victim != ch)
		{
	    send_to_char("Not on that target.\n\r", ch);
	    return;
		}


	check_killer(ch, victim);
    }
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	send_to_char("You can't do that on your own follower.\n\r", ch);
	return;
    }

     if (ch->mana < 1) {
	send_to_char
	    ("Little tendrils of smoke come out your nostrils..\r\n", ch);
	return;
    }
	if (ch->mana > 0) {

	    damdice = ((ch->mana / 10));
	    if (damdice > ch->mana) {
		damdice = (ch->mana);
		ch->mana = 0;
	    } else {
		ch->mana -= damdice;
	    }
        damage(ch, victim, number_range((damdice * (get_age(ch) / 2)),
					(damdice * get_age(ch))), sn, damtype, TRUE);
	WAIT_STATE(ch, ((24) + (damdice / 5)));

    return;
    }
}
}



void do_breathe(CHAR_DATA * ch, char *argument)
 
{
   
    CHAR_DATA *vch, *vch_next;
    CHAR_DATA *victim;
    int dam,hp_dam,dice_dam;
    int hpch;
    int sn;
    int level;
    int target;
  
 
    if (!ch->race == race_lookup("dragonborn")) 
      {
        send_to_char("You aren't kin of the dragons!", ch);
        return;
          }

    if (IS_NPC(ch))
    {
        send_to_char("NPCs do not do this. Set their special action with spec_breath\n\r", ch);
            return;
    }
    if (ch->race == race_lookup("dragonborn"))

  {
    if (ch->pcdata->hair_color == "red scales" )	{
     act("$n breathes forth a cone of fire.",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a cone of hot fire over you!",ch,NULL,victim,TO_VICT);
    act("You breath forth a cone of fire.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX( 10, ch->hit );
    hp_dam  = number_range( hpch/9+1, hpch/5 );
    dice_dam = dice(level,20);

    dam = UMAX(hp_dam + dice_dam /10, dice_dam + hp_dam / 10);
    fire_effect(victim->in_room,level,dam/2,TARGET_ROOM);

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch,vch,TRUE)
	||  (IS_NPC(vch) && IS_NPC(ch)
	&&   (ch->fighting != vch || vch->fighting != ch)))
	    continue;

	if (vch == victim) /* full damage */
	{
	    if (saves_spell(level,vch,DAM_FIRE))
	    {
		fire_effect(vch,level/2,dam/4,TARGET_CHAR);
		damage(ch,vch,dam/2,sn,DAM_FIRE,TRUE);
	    }
	    else
	    {
		fire_effect(vch,level,dam,TARGET_CHAR);
		damage(ch,vch,dam,sn,DAM_FIRE,TRUE);
	    }
	}
	else /* partial damage */
	{
	    if (saves_spell(level - 2,vch,DAM_FIRE))
	    {
		fire_effect(vch,level/4,dam/8,TARGET_CHAR);
		damage(ch,vch,dam/4,sn,DAM_FIRE,TRUE);
	    }
	    else
	    {
		fire_effect(vch,level/2,dam/4,TARGET_CHAR);
		damage(ch,vch,dam/2,sn,DAM_FIRE,TRUE);
	    }
	}
    }
}
   
	
    } else if (ch->pcdata->hair_color == "copper scales" ) {
act("$n breathes forth a cone of fire.",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a cone of hot fire over you!",ch,NULL,victim,TO_VICT);
    act("You breath forth a cone of fire.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX( 10, ch->hit );
    hp_dam  = number_range( hpch/9+1, hpch/5 );
    dice_dam = dice(level,20);

    dam = UMAX(hp_dam + dice_dam /10, dice_dam + hp_dam / 10);
    fire_effect(victim->in_room,level,dam/2,TARGET_ROOM);

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch,vch,TRUE)
	||  (IS_NPC(vch) && IS_NPC(ch)
	&&   (ch->fighting != vch || vch->fighting != ch)))
	    continue;

	if (vch == victim) /* full damage */
	{
	    if (saves_spell(level,vch,DAM_FIRE))
	    {
		fire_effect(vch,level/2,dam/4,TARGET_CHAR);
		damage(ch,vch,dam/2,sn,DAM_FIRE,TRUE);
	    }
	    else
	    {
		fire_effect(vch,level,dam,TARGET_CHAR);
		damage(ch,vch,dam,sn,DAM_FIRE,TRUE);
	    }
	}
	else /* partial damage */
	{
	    if (saves_spell(level - 2,vch,DAM_FIRE))
	    {
		fire_effect(vch,level/4,dam/8,TARGET_CHAR);
		damage(ch,vch,dam/4,sn,DAM_FIRE,TRUE);
	    }
	    else
	    {
		fire_effect(vch,level/2,dam/4,TARGET_CHAR);
		damage(ch,vch,dam/2,sn,DAM_FIRE,TRUE);
	    }
	}
    }

	
    }  else {
	send_to_char("You huff and puff..\r\n", ch);
	return;
    }
}
