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
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "merc.h"
#include "db.h"
#include "interp.h"
#include "lookup.h"             
#include "olc.h"
#include "recycle.h"
#include "tables.h"

/*
 * Semi-local functions
 */
void	check_mem_change	args( ( void ) );
void	quest_update		args( ( void ) );

/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );

void	aggr_update	args( ( void ) );
void	auc_update	args( ( void ) );
void	char_update	args( ( void ) );
void	mobile_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	orprog_update	args( ( void ) );
void	overland_update	args( ( void ) );
void	rebuild_check	args( ( void ) );
void	rprog_time_check args( ( void ) );
void	travel_update	args( ( void ) );
void	weather_update	args( ( void ) );
void    underwater_update args( ( void ) );
//void    olcautosave     args( ( void ) );
//void    save_area       args( ( AREA_DATA *pArea ) );
bool  save_area	args( ( AREA_DATA *pArea ) );
static void	dump_garbage	args( ( void ) );
void    who_html_update args( ( void ) );
//void    olcautosave     args( ( void ) );
//static  bool    save_area   args( ( AREA_DATA *pArea ) );
//static  bool    save_area_list  args( ( void ) );

/* used for saving */

int	save_number = 0;



/*
 * Advancement stuff.
 */
void
advance_level( CHAR_DATA *ch, bool hide )
{
    char buf[MAX_STRING_LENGTH];
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;

    ch->pcdata->last_level =
	( ch->played + (int) (current_time - ch->logon) ) / 3600;

    add_hp	= con_app[get_curr_stat(ch,STAT_CON)].hitp + number_range(
		    class_table[ch->class].hp_min,
		    class_table[ch->class].hp_max );
    add_mana 	= number_range(2,(2*get_curr_stat(ch,STAT_INT)
				  + get_curr_stat(ch,STAT_WIS))/5);
    if ( !IS_SET( class_table[ch->class].flags, CLASS_FMANA ) )
	add_mana /= 2;
    add_move	= number_range( 1, (get_curr_stat(ch,STAT_CON)
				  + get_curr_stat(ch,STAT_DEX))/6 );
    add_prac	= wis_app[get_curr_stat(ch,STAT_WIS)].practice;

    add_hp = add_hp * 9/10;
    add_mana = add_mana * 9/10;
    add_move = add_move * 9/10;

    add_hp	= UMAX(  2, add_hp   );
    add_mana	= UMAX(  2, add_mana );
    add_move	= UMAX(  6, add_move );

    ch->max_hit 	+= add_hp;
    ch->max_mana	+= add_mana;
    ch->max_move	+= add_move;
    ch->practice	+= add_prac;
    ch->train		+= 1;

    ch->pcdata->perm_hit	+= add_hp;
    ch->pcdata->perm_mana	+= add_mana;
    ch->pcdata->perm_move	+= add_move;

    if ( !hide )
    {
    	sprintf( buf,
	    "You gain %d hit point%s, %d mana, %d move, and %d practice%s.\n\r",
	    add_hp, add_hp == 1 ? "" : "s", add_mana, add_move,
	    add_prac, add_prac == 1 ? "" : "s");
	send_to_char( buf, ch );
    }
    update_userlist( ch, FALSE );
    return;
}


void
gain_exp( CHAR_DATA *ch, int gain )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) || ch->level >= LEVEL_HERO )
	return;

    ch->exp = UMAX( exp_per_level( ch, ch->pcdata->points ), ch->exp + gain );
    while ( ch->level < LEVEL_HERO && ch->exp >=
	exp_per_level( ch, ch->pcdata->points ) * ( ch->level+1 ) )
     // while (ch->level < LEVEL_HERO && ch->pcdata->exp_to_lvl >= ch->pcdata->exp_each_lvl )
    {
	send_to_char( "You raise a level!!  ", ch );
	ch->level += 1;
	//ch->pcdata->exp_to_lvl = 0; /* Reset the exp */
        //ch->pcdata->exp_each_lvl += 100; /* Add 100 more exp to gain the next level */
	sql_update_level( ch );
	sprintf( buf, "%s gained level %d", ch->name, ch->level );
	log_string( buf );
	sprintf( buf, "$N has attained level %d!",ch->level );
	wiznet( buf, ch, NULL,WIZ_LEVELS, 0, 0 );
	advance_level( ch, FALSE );
 	affect_strip(ch,gsn_plague);
                affect_strip(ch,gsn_poison);
                affect_strip(ch,gsn_blindness);
                affect_strip(ch,gsn_sleep);
                affect_strip(ch,gsn_curse);

                ch->hit        = ch->max_hit;
                ch->mana       = ch->max_mana;
                ch->move       = ch->max_move;
                update_pos( ch);
 	send_to_char( "`WYou have been restored to your full strength!`X\n\r", ch );
	save_char_obj( ch );
    }

    return;
}



/*
 * Regeneration stuff.
 */
int
hit_gain( CHAR_DATA *ch )
{
    int		gain;
    int		number;
    bool	dam;

    if (ch->in_room == NULL)
	return 0;

    if ( IS_DEAD( ch ) )
        return 0;

    dam = FALSE;
    if ( ch->in_room->heal_rate < 0 )
        dam = TRUE;
    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE && ch->on->value[3] < 0 )
        dam = TRUE;

    if ( !dam && ch->hit >= ch->max_hit )
        return 0;

    if ( IS_NPC( ch ) )
    {
	gain =  5 + ch->level;
 	if ( IS_AFFECTED( ch, AFF_REGENERATION ) )
	    gain *= 2;

	switch( ch->position )
	{
	    default : 		gain /= 2;			break;
	    case POS_SLEEPING: 	gain = 3 * gain/2;		break;
	    case POS_RESTING:  					break;
	    case POS_FIGHTING:	gain /= 3;		 	break;
 	}
    }
    else
    {
	gain = UMAX( 3, get_curr_stat( ch, STAT_CON ) - 3 + ch->level/2 );
	gain += class_table[ch->class].hp_max - 10;
 	number = number_percent();
	if (number < get_skill( ch, gsn_fast_healing ) )
	{
	    gain += number * gain / 100;
	    if ( ch->hit < ch->max_hit )
		check_improve( ch, gsn_fast_healing, TRUE, 8 );
	}

	switch ( ch->position )
	{
	    default:	   	gain /= 4;			break;
	    case POS_SLEEPING: 					break;
	    case POS_RESTING:  	gain /= 2;			break;
	    case POS_FIGHTING: 	gain /= 6;			break;
	}

	if ( !dam )
	{
	    if ( ch->pcdata->condition[COND_HUNGER] == 0 )
	        gain /= 2;

	    if ( ch->pcdata->condition[COND_THIRST] == 0 )
	        gain /= 2;
        }

    }

    gain = gain * ch->in_room->heal_rate / 100;

    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
        gain = gain * ch->on->value[3] / 100;

    if ( !dam )
    {
        if ( IS_AFFECTED( ch, AFF_POISON ) )
	    gain /= 4;

        if ( IS_AFFECTED( ch, AFF_PLAGUE ) )
	    gain /= 8;

	if ( IS_AFFECTED( ch, AFF_HASTE ) || IS_AFFECTED( ch, AFF_SLOW ) )
	    gain /=2 ;
    }

    if ( gain < 0 )
    {
        if ( gain + ch->hit < 1 )
            return ( -( ch->hit - 1 ) );
        else
            return gain;
    }

    return UMIN( gain, ch->max_hit - ch->hit );
}


int
mana_gain( CHAR_DATA *ch )
{
    int		gain;
    int		number;
    bool	dam;

    if ( ch->in_room == NULL )
	return 0;

    if ( IS_DEAD( ch ) )
        return 0;

    dam = ( ch->in_room->mana_rate < 0 );
    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE && ch->on->value[4] < 0 )
        dam = TRUE;

    if ( !dam && ch->mana >= ch->max_mana )
        return 0;

    if ( IS_NPC( ch ) )
    {
	gain = 5 + ch->level;
	switch (ch->position)
	{
	    default:		gain /= 2;		break;
	    case POS_SLEEPING:	gain = 3 * gain/2;	break;
   	    case POS_RESTING:				break;
	    case POS_FIGHTING:	gain /= 3;		break;
    	}
    }
    else
    {
	gain = ( get_curr_stat( ch, STAT_WIS )
	      + get_curr_stat( ch, STAT_INT ) + ch->level ) / 2;
	number = number_percent( );
	if (number < get_skill( ch, gsn_meditation ) )
	{
	    gain += number * gain / 100;
	    if (ch->mana < ch->max_mana)
	        check_improve( ch, gsn_meditation, TRUE, 8 );
	}
	if ( !IS_SET( class_table[ch->class].flags, CLASS_FMANA ) )
	    gain /= 2;

	switch ( ch->position )
	{
	    default:		gain /= 4;			break;
	    case POS_SLEEPING: 					break;
	    case POS_RESTING:	gain /= 2;			break;
	    case POS_FIGHTING:	gain /= 6;			break;
	}

	if ( !dam )
	{
	    if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
	        gain /= 2;

	    if ( ch->pcdata->condition[COND_THIRST] == 0 )
	        gain /= 2;
        }

    }

    gain = gain * ch->in_room->mana_rate / 100;

    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
	gain = gain * ch->on->value[4] / 100;

    if ( !dam )
    {
        if ( IS_AFFECTED( ch, AFF_POISON ) )
	    gain /= 4;

	if ( IS_AFFECTED( ch, AFF_PLAGUE ) )
            gain /= 8;

        if ( IS_AFFECTED( ch, AFF_HASTE ) || IS_AFFECTED( ch, AFF_SLOW ) )
            gain /=2 ;
    }

    if ( gain < 0 )
    {
        if ( gain + ch->mana < 1 )
            return ( -( ch->mana - 1 ) );
        else
            return gain;
    }

    return UMIN( gain, ch->max_mana - ch->mana );
}



int move_gain( CHAR_DATA *ch )
{
    int gain;

    if (ch->in_room == NULL)
	return 0;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMAX( 15, ch->level );

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_stat(ch,STAT_DEX);		break;
	case POS_RESTING:  gain += get_curr_stat(ch,STAT_DEX) / 2;	break;
	}

	if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    gain = gain * ch->in_room->heal_rate/100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[3] / 100;

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
        gain /= 8;

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
        gain /=2 ;

    return UMIN(gain, ch->max_move - ch->move);
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0
    ||	 IS_NPC( ch )
    ||	 ch->level >= LEVEL_IMMORTAL
    ||	 IS_DEAD( ch ) )
	return;

    condition				= ch->pcdata->condition[iCond];
    if (condition == -1)
        return;
    ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );

    if ( ch->pcdata->condition[iCond] == 0 && !IS_AFFECTED( ch, AFF_SLEEP ) )
    {
        switch (iCond)
        {
            case COND_HUNGER:
                /* DEATH FROM HUNGER */
                ch->hit -= 5;
                if (ch->hit > 0 )
                  {
                   send_to_char ("You are hungry.\n\r", ch);
                  }
                if (ch->hit <= 0 )
                   {

                     act("You have `Rdied`X from `Ostarvation!`X",ch,NULL,NULL,TO_CHAR);
                     act("$n withers away and dies from starvation!",ch,NULL,NULL,TO_ROOM);
                     if (ch->exp > exp_per_level (ch, ch->pcdata->points) * ch->level)
                             gain_exp (ch, (2 * (exp_per_level (ch, ch->pcdata->points)
                                      * ch->level - ch->exp) / 3) + 50);

                     sprintf (log_buf, "%s died from hunger in %s [room %d]",
                             ch->name, ch->in_room->name, ch->in_room->vnum);

                     wiznet (log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);

                      //make_corpse (ch);
                      int i;

                      extract_char (ch, FALSE);
                      while (ch->affected)
                      affect_remove (ch, ch->affected);
                      ch->affected_by = race_table[ch->race].aff;
                      for (i = 0; i < 4; i++)
                          ch->armor[i] = 100;
                      ch->position = POS_RESTING;
                      ch->hit = UMAX (5, ch->hit);
                      ch->mana = UMAX (5, ch->mana);
                      ch->move = UMAX (5, ch->move);
                      ch->pcdata->condition[COND_THIRST] = 48;
                      ch->pcdata->condition[COND_HUNGER] = 48;
                      ch->pcdata->condition[COND_FULL] = 48;
                      die_pc( ch );

                      return;
                    }

                break;

            case COND_THIRST:

                 /* DEATH FROM THIRST */
                    ch->hit -= 5;
                    if (ch->hit > 0 )
                       {
                         send_to_char ("You are thirsty.\n\r", ch);
                       }
                    if (ch->hit <= 0 )
                       {
                          act("You have `Rdied`X from `cdehydration!`X",ch,NULL,NULL,TO_CHAR);
                          act("$n withers away and dies from dehydration!",ch,NULL,NULL,TO_ROOM);
                          if (ch->exp > exp_per_level (ch, ch->pcdata->points) * ch->level)
                                  gain_exp (ch, (2 * (exp_per_level (ch, ch->pcdata->points)
                                           * ch->level - ch->exp) / 3) + 50);

                          sprintf (log_buf, "%s died from dehydration in %s [room %d]",
                                            ch->name, ch->in_room->name, ch->in_room->vnum);

                          wiznet (log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);

                          //make_corpse (ch);
                          int i;

                          extract_char (ch, FALSE);
                          while (ch->affected)
                          affect_remove (ch, ch->affected);
                          ch->affected_by = race_table[ch->race].aff;
                          for (i = 0; i < 4; i++)
                              ch->armor[i] = 100;
                          ch->position = POS_RESTING;
                          ch->hit = UMAX (5, ch->hit);
                          ch->mana = UMAX (5, ch->mana);
                          ch->move = UMAX (5, ch->move);
                          ch->pcdata->condition[COND_THIRST] = 48;
                          ch->pcdata->condition[COND_HUNGER] = 48;
                          ch->pcdata->condition[COND_FULL] = 48;
                          die_pc( ch );

                          return;
                       }

                break;
   /* {
	switch ( iCond )
	{
	case COND_HUNGER:
	    send_to_char( "You are hungry.\n\r",  ch );
	    break;

	case COND_THIRST:
	    send_to_char( "You are thirsty.\n\r", ch );
	    break;
  */
	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char( "You are sober.\n\r", ch );
	    break;

        case COND_TIRED:
            if ( condition != 0 )
                send_to_char( "You are getting tired.\n\r", ch );
            break;
	}
    }

    return;
}


/*
 * Auction update
 */
void auc_update( void )
{
    char	buf[MAX_STRING_LENGTH];
    MONEY	amt;

    if ( !auc_obj )
	return;

    amt.gold = 0;
    amt.silver = 0;
    amt.copper = 0;
    amt.fract = auc_cost;
    normalize( &amt );
    switch( auc_count / PULSE_AUCTION )
    {
    case 1:
	sprintf( buf, "%s for%s (going ONCE).", auc_obj->short_descr,
		 money_string( &amt, FALSE, FALSE ) );
	auc_channel( buf );
	return;
    case 2:
	sprintf( buf, "%s for%s (going TWICE).", auc_obj->short_descr,
		 money_string( &amt, FALSE, FALSE ) );
	auc_channel( buf );
	return;
    case 3:
	sprintf( buf, "%s for%s (going THRICE).", auc_obj->short_descr,
		 money_string( &amt, FALSE, FALSE ) );
	auc_channel( buf );
	return;
    }

    if ( auc_bid != NULL && CASH_VALUE( auc_bid->money ) >= auc_cost )
    {
	sprintf( buf, "%s for%s SOLD! to %s.", auc_obj->short_descr,
		 money_string( &amt, FALSE, FALSE ),
		 auc_bid->name );
	while( auc_bid->money.fract < amt.fract )
	{
	    auc_bid->money.copper--;
	    auc_bid->money.fract += FRACT_PER_COPPER;
	}
	while( auc_bid->money.copper < amt.copper )
	{
	    auc_bid->money.silver--;
	    auc_bid->money.copper += COPPER_PER_SILVER;
	}
	while( auc_bid->money.silver < amt.silver )
	{
	    auc_bid->money.gold--;
	    auc_bid->money.silver += SILVER_PER_GOLD;
	}
	money_subtract( &auc_bid->money, &amt, FALSE );
	money_add( &auc_held->money, &amt, FALSE );
	obj_to_char( auc_obj, auc_bid );
	act( "$p appears in your hands.", auc_bid, auc_obj, NULL, TO_CHAR );
	act( "$p appears in the hands of $n.", auc_bid, auc_obj, NULL, TO_ROOM );
    }
    else if ( auc_bid )
    {
	sprintf( buf, "%s not carried for %s, ending auction.",
		 money_string( &amt, FALSE, FALSE ),
		 auc_obj->short_descr );
	obj_to_char( auc_obj, auc_held );
	act( "$p appears in your hands.", auc_held, auc_obj, NULL, TO_CHAR );
	act( "$p appears in the hands of $n.", auc_held, auc_obj, NULL, TO_ROOM );
    }
    else
    {
	sprintf( buf, "%s not sold, ending auction.", auc_obj->short_descr );
	obj_to_char( auc_obj, auc_held );
	act( "$p appears in your hands.", auc_held, auc_obj, NULL, TO_CHAR );
	act( "$p appears in the hands of $n.", auc_held, auc_obj, NULL,
	     TO_ROOM );
    }
    auc_channel( buf );

    auc_count = -1;
    auc_cost = 0;
    auc_obj = NULL;
    auc_held = NULL;
    auc_bid = NULL;
    return;
}


/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void
mobile_update( void )
{
    CHAR_DATA *	ch;
    CHAR_DATA *	ch_next;
    EXIT_DATA *	pexit;
    int		door;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;

	if ( !IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch,AFF_CHARM))
	    continue;

	if (ch->in_room->area->empty && !IS_SET(ch->act,ACT_UPDATE_ALWAYS))
	    continue;

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 )
	{
	    if ( ( *ch->spec_fun ) ( ch ) )
		continue;
	}

	if ( ch->pIndexData->pShop != NULL ) /* give him some gold */
	{
	    if ( CASH_VALUE( ch->money ) < ch->pIndexData->wealth )
	    {
		ch->money.fract += ch->pIndexData->wealth
				* number_range( 1, 20 ) / 5000;
		normalize( &ch->money );
	    }
	}

	/* Check random mobprog trigger */
	if ( ch->in_room->area->nplayer > 0 )
	{
	    mprog_random_trigger( ch );
	}

	/* That's all for sleeping / busy monster, and empty zones */
	if ( ch->position != POS_STANDING )
	    continue;

        /* No scavenge or wandering in proto and frozen areas */
        if ( IS_SET( ch->in_room->area->area_flags, AREA_PROTOTYPE )
        &&   IS_SET( ch->in_room->area->area_flags, AREA_FREEZE ) )
            continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->contents != NULL
	&&   number_bits( 6 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && can_loot(ch, obj)
		     && obj->cost > max  && obj->cost > 0)
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_SENTINEL) 
	&& number_bits(3) == 0
	&& ( door = number_bits( 4 ) ) < MAX_DIR
	&& ( pexit = get_exit( ch->in_room, door ) ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET( pexit->exit_info, EX_NO_MOB )
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->to_room->area == ch->in_room->area ) 
	&& ( !IS_SET( pexit->to_room->area->area_flags, AREA_PROTOTYPE )
	||   !IS_SET( pexit->to_room->area->area_flags, AREA_FREEZE ) )
	&& ( ch->race == race_fish || IS_AFFECTED( ch, AFF_SWIM )
	||   !is_water( pexit->to_room ) )
	&& ( !IS_SET(ch->act, ACT_OUTDOORS)
	||   pexit->to_room->sector_type != SECT_INSIDE ) 
	&& ( !IS_SET(ch->act, ACT_INDOORS)
	||   pexit->to_room->room_flags == SECT_INSIDE ) )
	{
	    move_char( ch, door, FALSE );
	}
    }

    return;
}



/*
 * Update the weather.
 */
void
weather_update( void )
{
    char		buf[MAX_STRING_LENGTH];
    char		buf1[MAX_INPUT_LENGTH];
    int			day;
    char *		suf;
    DESCRIPTOR_DATA *	d;
    int 		diff;
    int 		val;

    buf[0] = '\0';

    ++time_info.hour;
    if ( time_info.hour >= HOURS_PER_DAY )
    {
	time_info.hour = 0;
	time_info.day++;
    }

    if ( time_info.day   >= DAYS_PER_MONTH )
    {
	time_info.day = 0;
	time_info.month++;
	switch( time_info.month )
	{
	    case 0:
	    case 8:
		hour_sunrise = HOUR_SUNRISE + 2;
		hour_sunset  = HOUR_SUNSET  - 2;
		break;
	    case 1:
		hour_sunrise = HOUR_SUNRISE + 1;
		hour_sunset  = HOUR_SUNSET  - 1;
		break;
	    case 2:
		hour_sunrise = HOUR_SUNRISE;
		hour_sunset  = HOUR_SUNSET;
		break;
	    case 3:
		hour_sunrise = HOUR_SUNRISE - 1;
		hour_sunset  = HOUR_SUNSET  + 1;
		break;
	    case 4:
		hour_sunrise = HOUR_SUNRISE - 2;
		hour_sunset  = HOUR_SUNSET  + 2;
		break;
	    case 5:
		hour_sunrise = HOUR_SUNRISE - 1;
		hour_sunset  = HOUR_SUNSET  + 1;
		break;
	    case 6:
		hour_sunrise = HOUR_SUNRISE;
		hour_sunset  = HOUR_SUNSET;
		break;
	    case 7:
		hour_sunrise = HOUR_SUNRISE + 1;
		hour_sunset  = HOUR_SUNSET  - 1;
		break;
	}
    }

    if ( time_info.month >= MONTHS_PER_YEAR )
    {
	time_info.month = 0;
	time_info.year++;
    }

    if ( time_info.hour == 0 )
    {
	day = time_info.day + 1;
	     if ( day > 4 && day < 20	) suf = "th";
	else if ( day % 10 == 1		) suf = "st";
	else if ( day % 10 == 2		) suf = "nd";
	else if ( day % 10 == 3		) suf = "rd";
	else				  suf = "th";
	sprintf( buf1, "The Day of %s of %s, %d%s the Month of %s has begun.\n\r",
		 day_name[time_info.day % DAYS_PER_WEEK],
		 week_name[time_info.day / DAYS_PER_WEEK],
		 day, suf,
		 month_name[time_info.month] );
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&	 IS_AWAKE( d->character )
	    &&	 d->character->in_room != NULL
	    &&	 !IS_SET( d->character->in_room->room_flags, ROOM_NO_RESETMSG ) )
	    {
		send_to_char( buf1, d->character );
//		ch_printf( d->character,
//			   "The %s day of the %s week of the %s month has begun.\n\r",
//			   ordinal[( time_info.day % DAYS_PER_WEEK ) + 1],
//			   ordinal[(time_info.day / DAYS_PER_WEEK ) + 1],
//			   ordinal[time_info.month + 1] );
	    }
	}
    }
    else if ( time_info.hour == 1 )
    {
        strcat( buf, "`cThe witching hour has passed.`X\n\r" );
    }
    else if ( time_info.hour == hour_sunrise - 1 )
    {
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "`cThe darkness begins to lift.`X\n\r" );
    }
    else if ( time_info.hour == hour_sunrise )
    {
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "`cThe `Ysun `crises triumphantly in the west.`X\n\r" );
    }
    else if ( time_info.hour == 11 )
    {
        strcat( buf, "`YThe sun sits high in its heavenly cradle.`X\n\r" );
    }
    else if ( time_info.hour == 13 )
    {
        strcat( buf, "`YThe afternoon sun bakes the land.`X\n\r" );
    }
    else if ( time_info.hour == hour_sunset )
    {
	weather_info.sunlight = SUN_SET;
	strcat( buf, "`cThe sun begins its slow descent in the east.`X\n\r" );
    }
    else if ( time_info.hour == hour_sunset + 1 )
    {
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "`cDarkness spreads its embrace about the land.`X\n\r" );
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 4 && time_info.month <= 7 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "A storm front advances ominously.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The clouds open and rain begins to fall.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The storm front moves on.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Lightning bolts streak across the sky.\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain has stopped falling.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "Lightning no longer flashes in the sky.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    /* Disgustingly crude but simple wind model for now. */
    val = number_range( WIND_CALM, WIND_HURRICANE );
    if ( val < weather_info.wind_speed )
	weather_info.wind_speed--;
    else if ( val > weather_info.wind_speed )
	weather_info.wind_speed++;

    switch( number_bits( 3 ) )
    {
	case 0:
	    weather_info.wind_dir = dir_next[weather_info.wind_dir][0];
	    break;
	case 1:
	    weather_info.wind_dir = dir_next[weather_info.wind_dir][1];
	    break;
    }

    if ( buf[0] == '\0' )
    {
	switch( number_bits( 5 ) )
	{
	    case 1: strcpy( buf, "A crow swoops down and pecks at your ear.\n\r" ); break;
	}
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}



/*
 * Update all chars, including mobs.
*/
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_quit;

    ch_quit	= NULL;

    /* update save counter */
    save_number++;

    if (save_number > 29)
	save_number = 0;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	ch_next = ch->next;

	if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_PLOADED ) )
	    continue;

	if ( ch->summon_timer > 0 )
	{
	    ch->summon_timer--;
	    if ( IS_NPC( ch ) && ch->summon_timer == 0 )
	    {
		if ( IS_SET( ch->act, ACT_UNDEAD ) )
		    act_color( AT_ORANGE, "$n disintegrates into dust.",
			       ch, NULL, NULL, TO_ROOM, POS_RESTING );
		else
		    act_color( AT_BLUE, "$n is consumed by a swirling vortex.",
			       ch, NULL, NULL, TO_ROOM, POS_RESTING );
		extract_char( ch, TRUE );
		continue;
	    }
	}

	if ( ch->timer > 30 )
	    ch_quit = ch;


	if(xIS_SET(ch->affected_by, AFF_SEVERED))
        {
           xREMOVE_BIT(ch->affected_by, AFF_SEVERED);
            act_color(AT_RED, 
                "With a last gasp of breath, $n dies due to massive lower body trauma.\n\r",
                ch,NULL,NULL,TO_ROOM, POS_RESTING);
            if(!IS_NPC(ch))
            send_to_char("Your injuries prove too much, and you die.\n\r",ch);
            raw_kill( ch, ch, 0 );
            continue;
        }

	if ( ch->position >= POS_STUNNED )
	{
            /* check to see if we need to go home */
            if (IS_NPC(ch) && ch->zone != NULL && ch->zone != ch->in_room->area
            && ch->desc == NULL &&  ch->fighting == NULL 
	    && !IS_AFFECTED(ch,AFF_CHARM) && number_percent() < 5)
            {
            	act("$n wanders on home.",ch,NULL,NULL,TO_ROOM);
            	extract_char(ch,TRUE);
            	continue;
            }

	    if ( ch->hit <= ch->max_hit )
		ch->hit  += hit_gain( ch );
	    else
		ch->hit = ch->max_hit;

	    if ( ch->mana <= ch->max_mana )
		ch->mana += mana_gain( ch );
	    else
		ch->mana = ch->max_mana;

	    if ( ch->move < ch->max_move )
		ch->move += move_gain( ch );
	    else
		ch->move = ch->max_move;
	}

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );

	if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL )
	{
	    OBJ_DATA *obj;

	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room != NULL )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p flickers and goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	 	else if ( obj->value[2] <= 5 && ch->in_room != NULL)
		    act("$p flickers.",ch,obj,NULL,TO_CHAR);
	    }

	    if (IS_IMMORTAL(ch))
		ch->timer = 0;

	    if ( ++ch->timer >= 12 )
	    {
		if ( ch->was_in_room == 0 && ch->in_room != NULL )
		{
		    ch->was_in_room = ch->in_room->vnum;
		    if ( ch->fighting != NULL )
			stop_fighting( ch, TRUE );
		    act( "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    if (ch->level > 1)
		        save_char_obj( ch );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

	    if ( !IS_NPC( ch ) )
	    {
		gain_condition( ch, COND_DRUNK,  -1 );
		gain_condition( ch, COND_FULL, ch->size > SIZE_MEDIUM ? -4 : -2 );
		gain_condition( ch, COND_THIRST, -1 );
		gain_condition( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);
		if ( ch->position == POS_RESTING )
		    gain_condition( ch, COND_TIRED, 0 );
		else if ( ch->position <= POS_SLEEPING )
		    gain_condition( ch, COND_TIRED,
				    get_curr_stat( ch, STAT_CON ) / 2 );
		else
		    gain_condition( ch, COND_TIRED,
			get_curr_stat( ch, STAT_CON ) > 19 ? -1 : -2 );
	    }
	}

	for ( paf = ch->affected; paf != NULL; paf = paf_next )
	{
	    paf_next	= paf->next;
	    if ( paf->duration > 0 )
	    {
		paf->duration--;
		if (number_range(0,4) == 0 && paf->level > 0)
		  paf->level--;  /* spell strength fades with time */
            }
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		if ( paf_next == NULL
		||   paf_next->type != paf->type
		||   paf_next->duration > 0 )
		{
		    if ( paf->type > 0 )
		    {
			if ( skill_table[paf->type].msg_off
			     && skill_table[paf->type].msg_off[0] != '\0' )
			{
			    send_to_char( skill_table[paf->type].msg_off, ch );
			    send_to_char( "\n\r", ch );
			}
			if ( skill_table[paf->type].msg_room
			     && skill_table[paf->type].msg_room[0] != '\0'
			     && ch->in_room
			     && ch->in_room->people )
			{
			    act_color( AT_DGREEN, skill_table[paf->type].msg_room,
				       ch, NULL, NULL, TO_ROOM, POS_RESTING );
			}
		    }
		}

		affect_remove( ch, paf );
		if ( paf->type == AFF_FLYING )
		    check_nofloor( ch );
	    }
	}

/*
 * Drowning stuff
 */
 if(ch->desc && ch->desc->connected == CON_PLAYING)
	  if(!IS_NPC(ch))
	    {
	      if( ch->in_room && (ch->in_room->sector_type == SECT_WATER_SWIM && !IS_AFFECTED(ch,AFF_FLYING)))
		{
		  OBJ_DATA *obj;
		  bool has_boat;
		  int chance;
		  
		  /*
		   * Look for a boat.
		   */
		  has_boat = FALSE;
		  
		  if (IS_IMMORTAL(ch))
		    has_boat = TRUE;
		  
		  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
		    {
		      if ( obj->item_type == ITEM_BOAT )
			{
			  has_boat = TRUE;
			  break;
			}
		    }
		  if ( !has_boat )
		    {	    
		      if(!IS_NPC(ch) && ch->level < skill_table[gsn_swimming].skill_level[ch->class])
			{
			  send_to_char("You shouldn't be here, report this to an IMM.\n\r",ch);
			  return;
			}
		      
		      if((chance = get_skill(ch,gsn_swimming)) == 0)
			{
			  send_to_char("You shouldn't be here, report this to an IMM.\n\r",ch);
			  return;
			}
		      
		      if(IS_SET(ch->vuln_flags, VULN_DROWNING))
			chance /= 2;
		      
		      chance -= ((ch->max_move / ch->move) / 2); 
		      chance -= (get_carry_weight(ch) / 10); 
		      
		      //if(ch->race == race_lookup("dwarf")) /* dwarf's can't float. */
		//	{
		//	  chance = 0;
	//		}
		      
		      if(number_percent() < chance)
			{
			  check_improve(ch,gsn_swimming,TRUE, 5);
			  send_to_char("You manage to tread water to keep yourself from drowning!\n\r",ch);
			  act("$n manages to tread water despite the weight of their gear!", ch, NULL, NULL, TO_ROOM);


			}
		      else
			{
			  check_improve(ch,gsn_swimming,FALSE, 5);
			  send_to_char("You sink below the surface and gasp for one last breath while you still can!\n\r",ch);
			  char_from_room(ch);
			  char_to_room(ch, get_room_index(ROOM_VNUM_UNDERWATER));
			  do_look(ch,"auto");
			  if (ch->pet != NULL)
			    {
			      char_from_room( ch->pet );
			      char_to_room( ch->pet, ch->in_room );
			      do_look(ch->pet,"auto");
			    }
			  if (ch->mount != NULL)
			    {
			      char_from_room(ch->mount);
			      char_to_room( ch->mount, ch->in_room );
			      do_look(ch->mount,"auto");
			    }
			  return;
			}
		    }
		}
	    }
/* trading this out for underwater_update */
/*
                     if( ch->in_room && (ch->in_room->sector_type == SECT_UNDERWATER && !IS_AFFECTED(ch,AFF_FLYING))) 
			{
			int dam;
			send_to_char("You choke and gag as your lungs fill with water!\n\r", ch);
			act("$n thrashes about in the water gasping for air!", ch, NULL, NULL, TO_ROOM);
			dam = number_range( 1 * ch->level, 5 * ch->level);
			damage(ch, ch, dam, TYPE_UNDEFINED, DAM_DROWNING,FALSE);
			 if (ch->hit <= 0 )
                   {

                     act("Darkness consumes you as the last of your air escapes your lungs and you drown.",ch,NULL,NULL,TO_CHAR);
                     act("$n drowns, the last of their air bubbling toward the surface",ch,NULL,NULL,TO_ROOM);
                     if (ch->exp > exp_per_level (ch, ch->pcdata->points) * ch->level)
                             gain_exp (ch, (2 * (exp_per_level (ch, ch->pcdata->points)
                                      * ch->level - ch->exp) / 3) + 50);

                     sprintf (log_buf, "%s died from drowning in %s [room %d]",
                             ch->name, ch->in_room->name, ch->in_room->vnum);

                     wiznet (log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);

                      //make_corpse (ch);
                      int i;

                      extract_char (ch, FALSE);
                      while (ch->affected)
                      affect_remove (ch, ch->affected);
                      ch->affected_by = race_table[ch->race].aff;
                      for (i = 0; i < 4; i++)
                          ch->armor[i] = 100;
                      ch->position = POS_RESTING;
                      ch->hit = UMAX (5, ch->hit);
                      ch->mana = UMAX (5, ch->mana);
                      ch->move = UMAX (5, ch->move);
                      ch->pcdata->condition[COND_THIRST] = 48;
                      ch->pcdata->condition[COND_HUNGER] = 48;
                      ch->pcdata->condition[COND_FULL] = 48;
                      die_pc ( ch );
                      return;
                    }

		}
*/
	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */

        if (is_affected(ch, gsn_plague) && ch != NULL)
        {
            AFFECT_DATA *af, plague;
            CHAR_DATA *vch;
            int dam;

	    if (ch->in_room == NULL)
		continue;

	    act("$n writhes in agony as plague sores erupt from $s skin.",
		ch,NULL,NULL,TO_ROOM);
	    send_to_char("You writhe in agony from the plague.\n\r",ch);
            for ( af = ch->affected; af != NULL; af = af->next )
            {
            	if (af->type == gsn_plague)
                    break;
            }

            if ( af == NULL )
            {
            	xREMOVE_BIT( ch->affected_by, AFF_PLAGUE );
            	continue;
            }

            if (af->level == 1)
            	continue;

	    plague.where		= TO_AFFECTS;
            plague.type 		= gsn_plague;
            plague.level 		= af->level - 1; 
            plague.duration 	= number_range(1,2 * plague.level);
            plague.location		= APPLY_STR;
            plague.modifier 	= -5;
            plague.bitvector 	= AFF_PLAGUE;

            for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
            {
                if (!saves_spell(plague.level - 2,vch,DAM_DISEASE) 
		&&  !IS_IMMORTAL(vch)
            	&&  !IS_AFFECTED(vch,AFF_PLAGUE) && number_bits(4) == 0)
            	{
            	    send_to_char("You feel hot and feverish.\n\r",vch);
            	    act("$n shivers and looks very ill.",vch,NULL,NULL,TO_ROOM);
            	    affect_join(vch,&plague);
            	}
            }

	    dam = UMIN(ch->level,af->level/5+1);
	    ch->mana -= dam;
	    ch->move -= dam;
	    damage( ch, ch, dam, gsn_plague,DAM_DISEASE,FALSE);
        }
	else if ( IS_AFFECTED(ch, AFF_POISON) && ch != NULL
	     &&   !IS_AFFECTED(ch,AFF_SLOW))

	{
	    AFFECT_DATA *poison;

	    poison = affect_find(ch->affected,gsn_poison);

	    if (poison != NULL)
	    {
	        act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
	        send_to_char( "You shiver and suffer.\n\r", ch );
	        damage(ch,ch,poison->level/10 + 1,gsn_poison,
		    DAM_POISON,FALSE);
	    }
	}

	else if ( ch->position == POS_INCAP && number_range(0,1) == 0)
	{
	    damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,FALSE);
	}
	else if ( ch->position == POS_MORTAL )
	{
	    damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,FALSE);
	}
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;

	if (ch->desc != NULL && ch->desc->descriptor % 30 == save_number)
	{
	    save_char_obj(ch);
	}

        if (ch == ch_quit)
	{
            do_function(ch, &do_quit, "" );
	}
    }

    return;
}




/*
 * Update all objs.
 * This function is performance sensitive.
 */
void
obj_update( void )
{   
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf, *paf_next;

    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	CHAR_DATA *rch;
	char *message;

	obj_next = obj->next;

	/* go through affects and decrement */
        for ( paf = obj->affected; paf != NULL; paf = paf_next )
        {
            paf_next    = paf->next;
            if ( paf->duration > 0 )
            {
                paf->duration--;
                if (number_range(0,4) == 0 && paf->level > 0)
                  paf->level--;  /* spell strength fades with time */
            }
            else if ( paf->duration < 0 )
                ;
            else
            {
                if ( paf_next == NULL
                ||   paf_next->type != paf->type
                ||   paf_next->duration > 0 )
                {
                    if ( paf->type > 0 && skill_table[paf->type].msg_obj )
                    {
			if (obj->carried_by != NULL)
			{
			    rch = obj->carried_by;
			    act(skill_table[paf->type].msg_obj,
				rch,obj,NULL,TO_CHAR);
			}
			if (obj->in_room != NULL 
			&& obj->in_room->people != NULL)
			{
			    rch = obj->in_room->people;
			    act(skill_table[paf->type].msg_obj,
				rch,obj,NULL,TO_ALL);
			}
                    }
                }

                affect_remove_obj( obj, paf );
            }
        }

	if ( obj->timer > 0
	&& ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON )
	&&   obj->value[4] > 0 && --obj->value[4] == 0 )
	{
	    obj->value[3] = TRUE;
	    obj->value[4] = -1;
	    if ( obj->carried_by != NULL )
	        act_color( AT_ACTION, "$p spoils.", obj->carried_by, obj, NULL, TO_CHAR, POS_RESTING );
            else if ( obj->in_room != NULL && obj->in_room->people != NULL )
	        act_color( AT_ACTION, "$p spoils.", obj->in_room->people, obj, NULL, TO_ALL, POS_RESTING );
        }

        if ( obj->item_type == ITEM_LIGHT
             && obj->in_room != NULL
             && obj->value[2] > 0 )
        {
            if ( --obj->value[2] == 0 )
            {
                act( "$p flickers and goes out.", obj->in_room->people, obj, NULL, TO_ALL );
                extract_obj( obj );
                continue;
            }
            else if ( obj->value[2] <= 5 )
                act( "$p flickers.", obj->in_room->people, obj, NULL, TO_ALL );
        }

	if ( obj->timer <= 0 || --obj->timer > 0 )
	if (obj->in_room &&
                obj->in_room->sector_type == SECT_AIR &&
                (obj->wear_flags & ITEM_TAKE) &&
                obj->in_room->exit[5] &&
                obj->in_room->exit[5]->to_room)
               {
               ROOM_INDEX_DATA *new_room = obj->in_room->exit[5]->to_room;
 
               if (( rch = obj->in_room->people ) != NULL )
                  {
                  act( "$p falls away.", rch, obj, NULL, TO_ROOM );
                  act( "$p falls away.", rch, obj, NULL, TO_CHAR );
                  }
 
               obj_from_room(obj);
               obj_to_room(obj, new_room);
 
               if (( rch = obj->in_room->people ) != NULL )
                  {
                  act( "$p floats by.", rch, obj, NULL, TO_ROOM );
                  act( "$p floats by.", rch, obj, NULL, TO_CHAR );
                  }
               }
	    continue;

	switch ( obj->item_type )
	{
	default: message = "$p crumbles into dust.";		break;
	case ITEM_TRASH:
	    if ( obj->pIndexData->vnum == OBJ_VNUM_BLOOD )
	        message = "$p`X dries up.";
	    else if ( obj->pIndexData->vnum == OBJ_VNUM_BLOODSTAIN )
	        message = "$p`X dissipates.";
            else if ( obj->pIndexData->vnum == OBJ_VNUM_BLOOD_TRAIL )
                message = is_water( obj->in_room ) ? "$p`X dissipates." : "$p dries up.";
            else
	        message = "$p crumbles into dust.";
            break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         break;
	case ITEM_CORPSE_NPC: message = "$p decays into dust."; break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; break;
	case ITEM_FOOD:       message = "$p decomposes.";	break;
	case ITEM_POTION:     message = "$p has evaporated from disuse.";	
								break;
	case ITEM_PORTAL:     message = "$p fades out of existence."; break;
	case ITEM_CONTAINER: 
	    if ( CAN_WEAR( obj, ITEM_WEAR_FLOAT ) )
		if ( obj->contains != NULL )
		    message = 
		"$p flickers and vanishes, spilling its contents on the floor.";
		else
		    message = "$p flickers and vanishes.";
	    else
		message = "$p crumbles into dust.";
	    break;
	}

	if ( obj->carried_by != NULL )
	{
	    if ( IS_NPC( obj->carried_by ) 
	    &&   obj->carried_by->pIndexData->pShop != NULL )
	    {
		obj->carried_by->money.fract += obj->cost/5;
	    }
	    else
	    {
	    	act( message, obj->carried_by, obj, NULL, TO_CHAR );
		if ( obj->wear_loc == WEAR_FLOAT)
		    act(message,obj->carried_by,obj,NULL,TO_ROOM);
	    }
	}
	else if ( obj->in_room != NULL
	&&      ( rch = obj->in_room->people ) != NULL )
	{
	    if ( ! ( obj->in_obj && is_donation_box( obj->in_obj )
	           && !CAN_WEAR( obj->in_obj, ITEM_TAKE ) ) )
	    {
	    	act( message, rch, obj, NULL, TO_ROOM );
	    	act( message, rch, obj, NULL, TO_CHAR );
	    }
	}

        if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
	&&  obj->contains)
	{   /* save the contents */
     	    OBJ_DATA *t_obj, *next_obj;

	    for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
	    {
		next_obj = t_obj->next_content;
		obj_from_obj(t_obj);

		if (obj->in_obj) /* in another object */
		    obj_to_obj(t_obj,obj->in_obj);

		else if (obj->carried_by)  /* carried */
		    if (obj->wear_loc == WEAR_FLOAT)
			if (obj->carried_by->in_room == NULL)
			    extract_obj(t_obj);
			else
			{
			    obj_to_room(t_obj,obj->carried_by->in_room);
			    add_obj_fall_event( t_obj );
                        }
		    else
		    	obj_to_char(t_obj,obj->carried_by);

		else if (obj->in_room == NULL)  /* destroy it */
		    extract_obj(t_obj);

		else /* to a room */
		{
		    obj_to_room(t_obj,obj->in_room);
		    add_obj_fall_event( t_obj );
                }
	    }
	}

	extract_obj( obj );
    }

    return;
}


void
overland_update( void )
{
    OVERLAND_DATA *	overland;
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    ROOM_INDEX_DATA *	room_next;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( pArea->overland == NULL )
	    continue;

	overland = pArea->overland;
	for ( pRoom = overland->room_first; pRoom != NULL; pRoom = room_next )
	{
	    room_next = pRoom->next;
	    if ( pRoom->people != NULL
	    ||	 pRoom->next_in_room != NULL
	    ||	 pRoom->contents != NULL )
	        continue;

	    if ( --pRoom->vtimer > 0 )
		continue;

	    if ( pRoom->next == NULL )
		overland->room_last = pRoom->prev;
	    else
		pRoom->next->prev = pRoom->prev;
	    if ( pRoom->prev == NULL )
		overland->room_first = pRoom->next;
	    else
		pRoom->prev->next = pRoom->next;

	    free_room_index( pRoom );
	    top_vroom--;
	}
    }

}


/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update( void )
{
    MPROG_ACT_LIST *tmp_act;
    MPROG_ACT_LIST *tmp_act_next;
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;

    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
	wch_next = wch->next;
	if ( IS_NPC( wch ) && wch->mpactnum )
	{
	    for ( tmp_act = wch->mpact; tmp_act; tmp_act = tmp_act_next )
	    {
		mprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch,
				      tmp_act->obj, tmp_act->vo, MP_ACT_PROG );
		tmp_act_next = tmp_act->next;
		free_mpact_data( tmp_act );
	    }
	    wch->mpactnum = 0;
	    wch->mpact = NULL;
	    continue;
	}

	if ( IS_NPC(wch)
	||   wch->level >= LEVEL_IMMORTAL
	||   wch->in_room == NULL 
	||   wch->in_room->area->empty)
	    continue;

	for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||   IS_SET(ch->in_room->room_flags,ROOM_SAFE)
	    ||   IS_AFFECTED(ch,AFF_CALM)
	    ||   ch->fighting != NULL
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !IS_AWAKE(ch)
	    ||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
	    ||   !can_see( ch, wch )
	    ||	 ( ch->clan != NULL && ch->clan == wch->clan )
	    ||   number_bits(1) == 0)
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count	= 0;
	    victim	= NULL;
	    for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
	    {
		vch_next = vch->next_in_room;

		if ( !IS_NPC(vch)
		&&   vch->level < LEVEL_IMMORTAL
		&&   ch->level >= vch->level - 5 
		&&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
		&&   can_see( ch, vch ) )
		{
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( victim == NULL )
		continue;

	    if ( RIDDEN( ch ) != NULL )
	    {
		if ( !mount_success( RIDDEN( wch ), ch, FALSE ) )
		{
		    send_to_char( "Your mount escapes your control!\n\r", RIDDEN( ch ) );
		    multi_hit( ch, victim, TYPE_UNDEFINED );
		}
		else
		{
		    send_to_char( "You manage to keep your mount under control.\n\r", RIDDEN( ch ) );
		}
	    }
	    else
		multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}

/* 
 * Update for characters underwater
 */
 void underwater_update( void )                                                                      
{                                                                                                   
   CHAR_DATA *ch;                                                                                   
   CHAR_DATA *ch_next;                                                                              
                                                                                                    
   for ( ch = char_list; ch != NULL; ch = ch_next)                                                  
   {                                                                                                
    ch_next = ch->next;                                                                             
                                                                                                    
    if (!IS_NPC(ch)                                                                                 
    && !IS_IMMORTAL(ch)                                                                             
    && IS_SET(ch->in_room->sector_type, SECT_UNDERWATER) )                                          
    {                                                                                               
                                                                                                    
     if ( ch->hit > 20)                                                                             
     {                                                                                              
      ch->position = POS_RESTING;                                                                   
       ch->hit /= 2;                                                                                
      send_to_char("You're drowning!!!\n\r", ch);                                                   
     }                                                                                              
     else                                                                                           
     {                                                                                              
      ch->hit = 1;                                                                                  
      //raw_kill(ch);                                                                               
      die_pc (ch);                                                                                  
      send_to_char("You have drowned!!\n\r", ch );                                                  
      sprintf(log_buf, "%s died from drowning in %s [room %d]",                                     
                ch->name, ch->in_room->name, ch->in_room->vnum);                                    
      wiznet (log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);                                               
     }                                                                                              
    }                                                                                               
   }                                                                                                
} 
/*
 * Empty out the garbage room.
 */
void
dump_garbage( void )
{
    ROOM_INDEX_DATA *	pRoom;
    CHAR_DATA *		mob;
    CHAR_DATA *		mob_next;
    OBJ_DATA *		obj;
    OBJ_DATA *		obj_next;

    if ( ( pRoom = get_room_index( ROOM_VNUM_DISPOSAL ) ) == NULL )
        return;

    for ( mob = pRoom->people; mob != NULL; mob = mob_next )
    {
        mob_next = mob->next_in_room;
        if ( IS_NPC( mob ) )
            extract_char( mob, TRUE );
    }
    for ( obj = pRoom->contents; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        extract_obj( obj );
    }
    return;
}


/*
 * Check all rooms for time_progs.
 */
void
rprog_time_check( void )
{
    int			iHash;
    ROOM_INDEX_DATA *	room;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( room = room_index_hash[iHash]; room != NULL; room = room->next )
	{
	    if ( room->progtypes & ROOM_PROG_TIME )
	    {
		rprog_time_trigger( room, time_info.hour );
	    }
	}
    }
}


void
rebuild_check( void )
{
    char	buf[SHORT_STRING_LENGTH];
    int		status;

    if ( rebuild.pid == 0 )
	return;

    if ( waitpid( rebuild.pid, &status, WNOHANG ) == 0 )
	return;

    if ( status == 0 )
	sprintf( buf, "Build `GSuccessful`X, %ld seconds.",
		current_time - rebuild.time );
    else
	sprintf( buf, "Build FAILED, terminated with status %X", status );
    wiznet( buf, NULL, NULL, WIZ_BUG, 0, 0 );
    rebuild.pid = 0;
}


/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void
update_handler( void )
{
    static int		pulse_area;
    static int		pulse_mobile;
    static int		pulse_overland;
    static int		pulse_point;
    static int		pulse_travel;
    static int		pulse_vehicle_save;
    static int		pulse_violence;
    static int          pulse_quest;
    static int 		pulse_underwater;
    static int 		count;
    static time_t	last_tick = 0;
	DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;

	
	
    if ( --pulse_area     <= 0 )
    {
	pulse_area	= PULSE_AREA;
	/* number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 ); */
	area_update	( );
     olcautosave     ( );  

    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update	( );
	orprog_update( );
    }

    if ( --pulse_overland <= 0 )
    {
	pulse_overland = PULSE_OVERLAND;
	overland_update( );
    }

    if ( --pulse_travel <= 0 )
    {
	pulse_travel = PULSE_TRAVEL;
	travel_update( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }

    if ( --pulse_underwater <= 0)
    {
    	pulse_underwater = PULSE_UNDERWATER;
    	underwater_update ( );
        who_html_update( );
    }
    	
    if ( --pulse_quest	<= 0) 
    { 
	pulse_quest = PULSE_QUEST; 
	quest_update	( ); 
    } 

    if ( --pulse_point    <= 0 || forcetick != 0 )
    {
	char buf[SHORT_STRING_LENGTH];

	if ( forcetick != 0 )
	{
	    travel_update( );
	    forcetick--;
	}
/*
    for (count = 0; tick_table[count]; count++);
   for (d = descriptor_list; d != NULL; d = d->next)
   {
       if (d->connected == CON_PLAYING && IS_SET( ch->act2, PLR_AUTOTICK ))
           send_to_char(tick_table[number_range(0, count - 1)], d->character );
   }*/
   for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {

        ch = d->character;

          for (count = 0; tick_table[count]; count++);
   
   {
      if (IS_SET(ch->act2,PLR_AUTOTICK))
           send_to_char(tick_table[number_range(0, count - 1)], d->character );
   }
		}
	}
	sprintf( buf, "TICK!  %ld", current_time - last_tick );
	last_tick = current_time;
	wiznet( buf, NULL, NULL, WIZ_TICKS, 0, 0 );
	pulse_point     = PULSE_TICK;
/* number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 ); */
	weather_update	( );
	mprog_time_trigger( );
	rprog_time_check( );
	quest_update	( );
	char_update	( );
	obj_update	( );
    }

    if ( auc_count >= 0 && ++auc_count % PULSE_AUCTION == 0 )
	auc_update( );

    aggr_update( );
    check_mem_change( );
    if ( changed_clan )
    {
	save_clans( );
    }

    if ( --pulse_vehicle_save <= 0 )
    {
	pulse_vehicle_save = PULSE_VEHICLE_SAVE;
	if ( vehicle_moved )
	    save_vehicles( );

	/*
	 * toss garbage collection in here rather than create
	 * another pulse variable
	 */
	dump_garbage	( );
    }

    rebuild_check( );

    update_events( );

    tail_chain( );
    return;
}


void
orprog_update( void )
{
    OBJ_DATA *		obj;
    OBJ_DATA *		obj_next;
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;

    for ( obj = object_list; obj; obj = obj_next )
    {
	obj_next = obj->next;
	if ( obj->deleted )
	    continue;
	/* ie: carried or in room */
	if ( !obj->in_obj /* && !obj->stored_by */ &&
	     ( ( obj->in_room && obj->in_room->area->nplayer ) ||
	     (obj->carried_by && obj->carried_by->in_room &&
	      obj->carried_by->in_room->area->nplayer) ) )
	    oprog_random_trigger( obj );
    }

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( pArea->nplayer > 0 )
	{
//	    int room;

//	    for ( room = pArea->min_vnum; room <= pArea->max_vnum; room++ )
//		if ( ( pRoom = get_room_index( room ) ) )
//		    rprog_random_trigger( pRoom );
            for ( pRoom = pArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
                rprog_random_trigger( pRoom );

	}
    }
    return;
}


bool
event_dream( EVENT_DATA *pEvent )
{
    CHAR_DATA *		ch;
    AFFECT_DATA *	paf;
    EVENT_DATA *	pNew;
    DREAM_DATA *	pDream;
    DREAMSEG_DATA *	pSeg;
    int			count;
    int			timer;

    ch = pEvent->owner.ch;

    if ( ch->position != POS_SLEEPING )
        return FALSE;	/* They woke up */
    if ( ( pDream = get_dream_index( pEvent->value0 ) ) == NULL )
        return FALSE;	/* No dream */
    if ( pDream->seg == NULL )
        return FALSE;	/* Incomplete dream */

    count = 0;
    for ( pSeg = pDream->seg; pSeg != NULL; pSeg = pSeg->next )
    {
        if ( pEvent->value1 == count )
            break;
        count++;
    }

    if ( pSeg != NULL )
    {
        send_to_char( pSeg->text, ch );
        if ( number_bits( 2 ) == 0 && pDream->type == DREAM_NIGHTMARE )
            act_color( AT_ACTION, "$n tosses and turns fitfully in $s sleep.",
                       ch, NULL, NULL, TO_ROOM, POS_RESTING );

//      timer = UMAX( 1, pEvent->value2 - 2 + number_range( 0, 4 ) ); 
        timer = UMAX( 10, strlen( pSeg->text ) / 16 ) * PULSE_PER_SECOND - 4 + number_range( 0, 8 );

        pNew		= new_event( );
        pNew->fun	= pEvent->fun;
        pNew->value0	= pEvent->value0;
        pNew->value1	= pEvent->value1 + 1;
        pNew->value2	= timer;
        pNew->type	= pEvent->type;
        add_event_char( pNew, ch, timer );
        return FALSE;
    }

    if ( pDream->type == DREAM_NIGHTMARE )
    {
        ch->position = POS_SITTING;
        act_color( AT_ACTION, "You wake up in a cold sweat.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n wakes up in a cold sweat.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    }

    /* Apply the dream affect here. */
    for ( paf = pDream->affect; paf != NULL; paf = paf->next )
        affect_to_char( ch, paf );

    return FALSE;
}


void
travel_update( void )
{
    AREA_DATA *		pArea;
    TRAVEL_DATA *	pTravel;
    bool		fChanged;
    int			dir;
    int			rdir;
    EXIT_DATA *		pExit;

    fChanged = FALSE;
    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
	if ( pArea->travel_room == NULL
	||   pArea->travel_first == NULL
	||   ( pTravel = pArea->travel_curr ) == NULL
	||   pTravel->room == NULL
	||   --pArea->travel_timer > 0 )
	    continue;

	while ( pTravel->room == NULL || pTravel->exit_dir == DIR_NONE )
	{
	    pTravel = pTravel->next;
	    pArea->travel_curr = pTravel;
	    if ( pTravel == NULL )
	    {
		pArea->travel_curr = pArea->travel_first;
		pTravel = pArea->travel_curr;
		if ( pTravel->room == NULL || pTravel->exit_dir == DIR_NONE )
		{
		    break;
		}
	    }
	}

	if ( pTravel->room == NULL || pTravel->exit_dir == DIR_NONE )
	{
	    pArea->travel_timer = 1;
	    continue;
	}

	fChanged = TRUE;

	if ( pTravel->stopped )
	{
	    pTravel->stopped = FALSE;

	    rprog_depart_trigger( pArea->travel_room, pTravel->room );
	    rprog_depart_trigger( pTravel->room, pArea->travel_room );

	    send_to_room( pArea->color, pArea->travel_room,
			  pTravel->depart_travel, POS_RESTING );
	    send_to_room( pTravel->room->area->color, pTravel->room,
			  pTravel->depart_room, POS_RESTING );

	    dir = pTravel->exit_dir;
	    rdir = rev_dir[dir];

	    pArea->travel_room->exit[dir] = NULL;

	    pExit = pTravel->room->exit[rdir];
	    pTravel->room->exit[rdir] = pArea->rev_link;
	    pArea->rev_link = pExit;

	    if ( ( pArea->travel_curr = pArea->travel_curr->next ) == NULL )
		pArea->travel_curr = pArea->travel_first;

	    pArea->travel_timer = number_fuzzy( pArea->travel_curr->move_timer );

	}
	else
	{
	    pTravel->stopped = TRUE;
	    pArea->travel_timer = number_fuzzy( pTravel->stop_timer );

	    dir = pTravel->exit_dir;
	    rdir = rev_dir[dir];

	    pArea->travel_room->exit[dir] = pArea->link;
	    pArea->travel_room->exit[dir]->to_room = pTravel->room;

	    pExit = pTravel->room->exit[rdir];
	    pTravel->room->exit[rdir] = pArea->rev_link;
	    pTravel->room->exit[rdir]->to_room = pArea->travel_room;
	    pArea->rev_link = pExit;

	    send_to_room( pArea->color, pArea->travel_room,
			  pTravel->arrive_travel, POS_RESTING );
	    send_to_room( pTravel->room->area->color, pTravel->room,
			  pTravel->arrive_room, POS_RESTING );

	    rprog_arrive_trigger( pArea->travel_room, pTravel->room );
	    rprog_arrive_trigger( pTravel->room, pArea->travel_room );
	}

    }

    if ( fChanged )
	save_travel_state( );
}
                                       
void
save_travel_state( void )
{
    AREA_DATA *		pArea;
    FILE *		fp;

    if ( ( fp = fopen( SYSTEM_DIR TRAVEL_FILE, "w" ) ) == NULL )
    {
	bugf( "Save_travel_state: unable to open save file." );
	return;
    }

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
	if ( pArea->travel_room == NULL
	||   pArea->travel_curr == NULL
	||   pArea->travel_curr->room == NULL )
	    continue;
	fprintf( fp, "%d %d %d %d\n",
		 pArea->travel_vnum, pArea->travel_curr->room_vnum,
		 pArea->travel_timer, pArea->travel_curr->stopped );

    }

    fprintf( fp, "0\n" );
    fclose( fp );
}

