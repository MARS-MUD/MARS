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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "magic.h"
#include "tables.h"
#include "recycle.h"


/*
 * Local functions.
 */
OBJ_DATA *	has_key		args( ( CHAR_DATA *ch, int key ) );
static int	get_sail_time	args( ( int dir ) );


void
move_char( CHAR_DATA *ch, int door, bool follow )
{
    char		buf[MAX_INPUT_LENGTH];
    CHAR_DATA *		fch;
    CHAR_DATA *		fch_next;
    CHAR_DATA *		mch;
    CHAR_DATA *		rch;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	to_room;
    EXIT_DATA *		pexit;
    OBJ_DATA *		obj;
    int			size;
    int			from_dir;
    bool		fMountTrigger;
    bool		fValidRev;

    if ( door < 0 || door >= MAX_DIR )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    if ( IS_TETHERED( ch ) )
        return;

    if ( ( mch = MOUNTED( ch ) ) != NULL && IS_TETHERED( mch ) )
    {
        send_to_char( "Your mount is tethered.\n\r", ch );
        return;
    }

    in_room = ch->in_room;

    if ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
        int drunk = 0;
        int nd = 0;
        drunk = ch->pcdata->condition[COND_DRUNK];

        if ( number_percent() < drunk )
        {
            /* Pick random doors until we get one that is not the original door */
            for ( nd = door; nd == door; nd = number_door( ) )
                ;

            door = nd;
            if ( get_exit( in_room, door ) == NULL )
            {
                if ( number_percent( ) < 50 )
                {
                    act_color( AT_ACTION, "You trip and fall on your butt in a drunken stupor.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
                    act_color( AT_ACTION, "$n trips and falls on $s butt in a drunken stupor.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
                    ch->position = POS_SITTING;
                    return;
                }
                else
                {
                    act_color( AT_ACTION, "$n drunkenly tries to leave $t.",
                               ch, dir_name[door], NULL, TO_ROOM, POS_RESTING );
                }
            }
            else
            {
                send_to_char( "You're too drunk to think clearly!  You wander"
                              " off in the wrong direction.\n\r", ch );
            }
        }
    }

    if ( ( pexit = get_exit( in_room, door ) ) == NULL
    ||   ( to_room = pexit->to_room ) == NULL
    ||	 ( ( IS_SET( pexit->exit_info, EX_SECRET ) && IS_SET( pexit->exit_info, EX_CLOSED) )
         && ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    ||	 !can_see_room( ch, pexit->to_room ) )
    {
	if ( ch->in_room->vehicle_type != VEHICLE_NONE
	&&   ( ch->in_room->helmsman == NULL
	  ||   ch->in_room->helmsman == ch ) )
	{
	    if ( move_vehicle( ch, in_room, door ) )
		ch->in_room->helmsman = ch;
	    return;
	}
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return;
    }

    if ( IS_DEAD( ch ) && in_room->area != to_room->area )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return;
    }

	if ( (in_room->sector_type == SECT_WATER_SWIM || to_room->sector_type == SECT_WATER_SWIM ) && MOUNTED(ch))
        {
            sprintf( buf,"You can't take your mount there.\n\r");
            send_to_char(buf, ch);
            return;
        } 


	if(( in_room->sector_type == SECT_WATER_SWIM || to_room->sector_type == SECT_WATER_SWIM ) && !IS_AFFECTED(ch, AFF_SWIM) && !IS_AFFECTED(ch,AFF_FLYING))
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
		  send_to_char("You need to learn to swim first",ch);
		  return;
		}

	      if((chance = get_skill(ch,gsn_swimming)) == 0)
		{
		  send_to_char("You dont know how to swim.\n\r",ch);
		  return;
		}

	      if(IS_SET(ch->vuln_flags, VULN_DROWNING))
	        chance /= 2;

	      chance -= ((ch->max_move / ch->move) / 2); /* how rested is the swimmer? */
	      chance -= (get_carry_weight(ch) / 10); /* how heavy is the swimmer? */

	      if(number_percent() < chance)
		{
		  check_improve(ch,gsn_swimming,TRUE, 5);
		}
	      else
		{
		  check_improve(ch,gsn_swimming,FALSE, 5);
		  send_to_char("You drown.\n\r",ch);
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

    if ( ch->race == race_fish
    &&	 to_room->sector_type != SECT_WATER_SWIM
    &&	 to_room->sector_type != SECT_WATER_NOSWIM
    &&	 to_room->sector_type != SECT_UNDERWATER
    &&	 to_room->sector_type != SECT_SWAMP
    &&	 to_room->sector_type != SECT_LAKE
    &&	 to_room->sector_type != SECT_RIVER
    &&	 to_room->sector_type != SECT_OCEAN )
    {
        send_to_char( "Fish out of water! Stay where you can breathe.\n\r", ch );
        return;
    }

    if ( IS_SET( pexit->exit_info, EX_CLOSED )
    &&  (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS))
    &&   !( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    {
	act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM )
    &&   ch->master != NULL
    &&   in_room == ch->master->in_room )
    {
	send_to_char( "What?  And leave your beloved master?\n\r", ch );
	return;
    }

    if ( !is_room_owner( ch, to_room ) && room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( MOUNTED( ch ) && IS_SET( to_room->room_flags, ROOM_NO_MOUNT ) )
    {
	send_to_char( "You cannot ride there.\n\r", ch );
	return;
    }

    if ( MOUNTED( ch ) )
    {
	if ( MOUNTED( ch )->position < POS_FIGHTING )
	{
	    send_to_char( "Your mount must be standing.\n\r", ch );
	    return; 
	}
	if ( !mount_success( ch, MOUNTED( ch ), FALSE ) )
	{
	    send_to_char( "Your mount stubbornly refuses to go that way.\n\r", ch );
	    return;
	}
    }

    if ( ch->mount != NULL && ch->riding )
    {
	size = UMAX( ch->size, ch->mount->size );
	if ( ch->size >= ch->mount->size - 2
	&&   ch->mount->size >= ch->size - 2 )
	    size = UMIN( size + 1, SIZE_TITANIC );
    }
    else
	size = ch->size;

    if ( ( size > pexit->size )
    &&	 ( IS_NPC( ch ) || !IS_IMMORTAL( ch )
        || !IS_SET( ch->act, PLR_HOLYLIGHT ) || ch->riding ) )
    {
	send_to_char( "You ", ch );
	if ( ch->riding )
	    ch_printf( ch, "and %s ", PERS( ch->mount, ch ) );
	send_to_char( "can't fit through there.\n\r", ch );
	return;
    }

    if ( to_room->sector_type == SECT_BOILING
    &&	 ( IS_NPC( ch )
           || !IS_IMMORTAL( ch )
           || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    {
	send_to_char( "You really don't want to go there.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) )
    {
	int iClass, iGuild;
	int move;

	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    for ( iGuild = 0; iGuild < MAX_GUILD; iGuild ++ )
	    {
	    	if ( iClass != ch->class
	    	&&   to_room->vnum == class_table[iClass].guild[iGuild] )
	    	{
		    send_to_char( "You aren't allowed in there.\n\r", ch );
		    return;
		}
	    }
	}

	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR )
	{
	    if ( MOUNTED( ch ) )
	    {
		if ( !IS_AFFECTED( MOUNTED( ch ), AFF_FLYING ) )
		{
		    send_to_char( "Your mount can't fly.\n\r", ch );
		    return;
		}
	    }
	    else if ( !IS_AFFECTED( ch, AFF_FLYING ) && !IS_IMMORTAL( ch ) )
	    {
		send_to_char( "You can't fly.\n\r", ch );
		return;
	    }
	}

	if ( door == DIR_UP
	&&   IS_SET( to_room->room_flags, ROOM_NO_FLOOR ) )
	{
	    if ( MOUNTED( ch ) )
	    {
		if ( !IS_AFFECTED( MOUNTED( ch ), AFF_FLYING ) )
		{
		    send_to_char( "Your mount can't fly.\n\r", ch );
		    return;
		}
	    }
	    else if ( !IS_AFFECTED( ch, AFF_FLYING ) && !IS_IMMORTAL( ch ) )
	    {
		send_to_char( "You can't fly.\n\r", ch );
		return;
	    }
	}

	if ( ( in_room->sector_type == SECT_WATER_NOSWIM
	||     to_room->sector_type == SECT_WATER_NOSWIM )
	&&    ( MOUNTED( ch ) && !IS_AFFECTED( MOUNTED( ch ), AFF_FLYING ) ) )
        {
            send_to_char( "You can't take your mount there.\n\r", ch );
            return;
        } 

	if (( in_room->sector_type == SECT_WATER_NOSWIM
	||    to_room->sector_type == SECT_WATER_NOSWIM )
  	&&    !IS_AFFECTED(ch,AFF_FLYING))
	{
	    OBJ_DATA *obj;
	    bool found;

	    /*
	     * Look for a boat.
	     */
	    found = FALSE;

	    if (IS_IMMORTAL(ch))
		found = TRUE;

	    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	    {
		if ( obj->item_type == ITEM_BOAT )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
	    {
		send_to_char( "You need a boat to go there.\n\r", ch );
		return;
	    }
	}

	move = sector_data[UMIN(SECT_MAX-1, in_room->sector_type)].move_loss
	     + sector_data[UMIN(SECT_MAX-1, to_room->sector_type)].move_loss
	     ;

        move /= 2;  /* i.e. the average */


	/* conditional effects */
	if ( !MOUNTED( ch ) )
	{
	    if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED( ch, AFF_HASTE ) )
		move /= 2;

	    if ( IS_AFFECTED( ch, AFF_SLOW ) )
		move *= 2;

	    if ( ch->move < move )
	    {
		send_to_char( "You are too exhausted.\n\r", ch );
		return;
	    }

	    if ( xIS_SET( ch->affected_by, AFF_SEVERED ) )
	    {
		send_to_char( "Move without legs? How?\n\r", ch );
		return;
	    }
	}
	else
	{
            if ( IS_AFFECTED( MOUNTED( ch ), AFF_FLYING )
	    ||   IS_AFFECTED( MOUNTED( ch ), AFF_HASTE ) )
                move /= 2;

            if ( IS_AFFECTED( MOUNTED( ch ), AFF_SLOW ) )
                move *= 2;

	    if ( MOUNTED( ch )->move < move )
	    {
		send_to_char( "Your mount is too exhausted.\n\r", ch );
		return;
            }

	    if ( xIS_SET( MOUNTED( ch )->affected_by, AFF_SEVERED ) )
	    {
		send_to_char( "Move without legs? How?\n\r", ch );
		return;
	    }
	}

	WAIT_STATE( ch, 1 );

	if ( MOUNTED( ch ) )
	    MOUNTED( ch )->move -= move;
	else
	    ch->move -= move;
    }

    if ( ( rch = RIDDEN( ch ) ) != NULL && rch->in_room == ch->in_room )
    {
	if ( !mount_success( rch, ch, FALSE ) )
	{
	    act( "Your mount escapes your control, and leaves $T.", rch, NULL, dir_name[door], TO_CHAR );
	    if ( RIDDEN( ch ) )
		ch = RIDDEN( ch );
	}
	else
	{
	    send_to_char("You steady your mount.\n\r", rch);
	    return;
	}
    }


    if ( !IS_AFFECTED( ch, AFF_SNEAK )
    &&   ch->invis_level < LEVEL_HERO )
    {
	if ( MOUNTED( ch ) )
	{
	    if ( !IS_AFFECTED( MOUNTED( ch ), AFF_FLYING ) )
		act_color( AT_ACTION, "$n leaves $t, riding on $N.",
			   ch, dir_name[door], MOUNTED( ch ), TO_ROOM, POS_RESTING );
            else
		act_color( AT_BLUE, "$n soars $t on $N.",
			   ch, dir_name[door], MOUNTED( ch ), TO_ROOM, POS_RESTING );
        }
	else
	{
	    if ( !RIDDEN( ch ) )
		act_color( AT_ACTION, "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING );
	}
    }

    if ( !IS_IMMORTAL( ch )
    && ( !IS_NPC( ch ) || !IS_SET( ch->act, ACT_UNDEAD ) )
    && !IS_DEAD( ch )
    && ch->hit < ch->max_hit / 2 )
    {
        act_color( AT_RED, "Blood trickles from your wounds.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
        if ( !IS_AFFECTED( ch, AFF_SNEAK ) )
            act_color( AT_RED, "$n leaves a trail of blood.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
        obj = create_object( get_obj_index( OBJ_VNUM_BLOOD_TRAIL ), 1 );
        sprintf( buf, "A blood trail leads %s.", dir_name[door] );
        free_string( obj->description );
        obj->description = str_dup( buf );
        obj->timer = number_range( 2, 4 );
        obj_to_room( obj, ch->in_room );
        add_obj_fall_event( obj );
    }

    eprog_exit_trigger( ch->in_room->exit[door], ch->in_room, ch );

    from_dir = rev_dir[door];
    fValidRev = ( to_room->exit[from_dir] != NULL
                  && to_room->exit[from_dir]->to_room == ch->in_room );


    char_from_room( ch );
    ch->from_dir = rev_dir[door];
    char_to_room( ch, to_room );
    if ( !IS_AFFECTED( ch, AFF_SNEAK )
    &&   ch->invis_level < LEVEL_HERO )
    {
	if( !MOUNTED( ch ) )
	{
	    if ( !RIDDEN( ch ) )
	    {
	        if ( fValidRev )
	            act_color( AT_ACTION, "$n has arrived from $t.", ch, dir_from[door], NULL, TO_ROOM, POS_RESTING );
		else
		    act_color( AT_ACTION, "$n has arrived.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
            }
	}
	else
	{
	    if ( !IS_AFFECTED( MOUNTED( ch ), AFF_FLYING ) )
	    {
		if ( fValidRev )
		    act_color( AT_ACTION, "$n has arrived from $t, riding on $N.", ch, dir_from[door], MOUNTED( ch ), TO_ROOM, POS_RESTING );
		else
		    act_color( AT_ACTION, "$n has arrived, riding on $N.", ch, NULL, MOUNTED( ch ), TO_ROOM, POS_RESTING );
	    }
	    else
	    {
		if ( fValidRev )
		    act_color( AT_BLUE, "$n soars in from $t, riding on $N.", ch, dir_from[door], MOUNTED( ch ), TO_ROOM, POS_RESTING );
		else
		    act_color( AT_BLUE, "$n soars in, riding on $N.", ch, NULL, MOUNTED( ch ), TO_ROOM, POS_RESTING );
            }
        }
    }

    if ( MOUNTED( ch ) )
    {
	mch = MOUNTED( ch );
	fMountTrigger = TRUE;
    }
    else
    {
	fMountTrigger = FALSE;
	mch = NULL;
    }

    do_function( ch, &do_look, "auto" );

    if ( !IS_NPC( ch )
    &&	 IS_SET( ch->act2, PLR_MARKING )
    &&	 ch->desc != NULL
    &&	 ch->desc->editor == ED_AREA
    &&	 IS_VIRTUAL( ch->in_room )
    &&	 get_room_index( ch->in_room->vnum ) == NULL
    &&	 ch->in_room->area == (AREA_DATA *)ch->desc->pEdit
    &&	 ch->in_room->sector_type != ch->pcdata->mark_sector )
    {
	ch->in_room->area->overland->map[ch->in_room->vnum - ch->in_room->area->min_vnum].sector_type = ch->pcdata->mark_sector;
	ch->in_room->sector_type = ch->pcdata->mark_sector;
	SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
	ch_printf( ch, "Sector type set to %s.\n\r",
		   flag_string( sector_types, ch->in_room->sector_type ) );
    }

    if ( to_room->exit[rev_dir[door]] != NULL
    &&	 to_room->exit[rev_dir[door]]->to_room == in_room )
	eprog_enter_trigger( to_room->exit[rev_dir[door]], ch->in_room, ch );
    else
	rprog_enter_trigger( ch->in_room, ch );

    if (in_room == to_room) /* no circular follows */
	return;

    for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
	fch_next = fch->next_in_room;

	if ( fch == mch )
	{
	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door, TRUE );
	    continue;
	}

//	if ( IS_NPC( fch ) && fch->mount && !fch->riding )
//	    continue;

	if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM) 
	&&   fch->position < POS_STANDING)
	    do_function(fch, &do_stand, "");

	if ( fch->master == ch && fch->position == POS_STANDING 
	&&   can_see_room( fch, to_room ) )
	{

	    if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
	    &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
	    {
		act("You can't bring $N into the city.",
		    ch,NULL,fch,TO_CHAR);
		act("You aren't allowed in the city.",
		    fch,NULL,NULL,TO_CHAR);
		continue;
	    }

	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door, TRUE );
	}
    }

    if ( ch->mount != NULL && ch->in_room != ch->mount->in_room )
    {
	ch->mount->riding = FALSE;
	ch->riding = FALSE;
    }

    if ( ( IS_SET( to_room->room_flags, ROOM_NO_FLOOR ) || to_room->sector_type == SECT_AIR )
    &&	 to_room != in_room
    &&	 ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
    &&	 !IS_AFFECTED( ch, AFF_FLYING )
    &&	 ( MOUNTED( ch ) == NULL || !IS_AFFECTED( MOUNTED( ch ), AFF_FLYING ) )
    &&	 ( ( pexit = to_room->exit[DIR_DOWN] ) != NULL )
    &&	 !IS_SET( pexit->exit_info, EX_CLOSED )
    &&	 ( ( to_room = pexit->to_room ) != NULL ) )
    {
        if ( ++ch->fall_count > MAX_FALL )
        {
            buildbug( "%s falling too far in room #%d", ch->name, ch->in_room->vnum );
            char_from_room( ch );
            char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
            return;
        }
        act_color( AT_ACTION, "$n falls through the air to the room below.", ch,
             NULL, NULL, TO_ROOM, POS_RESTING );
        act_color( AT_ACTION, "You fall down through where you thought the ground was!",
             ch, NULL, NULL, TO_CHAR, POS_RESTING );
        move_char( ch, DIR_DOWN, FALSE );
        ch->fall_count--;
        act_color( AT_WHITE, "$n falls down from above.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
        damage( ch, ch, 5, TYPE_HIT, DAM_BASH, TRUE );
    }

    check_room_aff( ch );

    mprog_entry_trigger( ch );
    mprog_greet_trigger( ch );
    if ( fMountTrigger && MOUNTED( ch ) )
    {
	mprog_entry_trigger( ch->mount );
	mprog_greet_trigger( ch->mount );
    }

    check_life_xtal( ch );

    return;
}


void
do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTH, FALSE );
    return;
}


void
do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST, FALSE );
    return;
}


void
do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH, FALSE );
    return;
}


void
do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST, FALSE );
    return;
}


void
do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP, FALSE );
    return;
}


void
do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN, FALSE );
    return;
}


void
do_northwest( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTHWEST, FALSE );
    return;
}


void
do_northeast( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTHEAST, FALSE );
    return;
}


void
do_southwest( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTHWEST, FALSE );
    return;
}


void
do_southeast( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTHEAST, FALSE );
    return;
}


void
check_nofloor( CHAR_DATA *ch )
{
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;

    if ( ( IS_SET( ch->in_room->room_flags, ROOM_NO_FLOOR ) || ch->in_room->sector_type == SECT_AIR )
    &&	 ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
    &&   ( pexit = ch->in_room->exit[DIR_DOWN] ) != NULL
    &&	 ( to_room = pexit->to_room ) != NULL
    &&	 to_room != ch->in_room )
    {
        act_color( AT_ACTION, "You fall through where the floor should have been.", ch,
                   NULL, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n falls down to the room below.", ch, NULL, NULL,
                   TO_ROOM, POS_RESTING );
        damage( ch, ch, 5, TYPE_HIT, DAM_BASH, TRUE );
        move_char( ch, DIR_DOWN, FALSE );
    }

    return;
}


void
check_riding( DESCRIPTOR_DATA *d )
{
    CHAR_DATA *		ch;
    CHAR_DATA *		och;
    CHAR_DATA *		rch;
    ROOM_INDEX_DATA *	oldroom;

    ch = d->character;

    if ( d->riding
    &&	 d->connected == CON_PLAYING
    &&	 ch->position == POS_STANDING
    &&	 MOUNTED( ch ) != NULL )
    {
	oldroom = ch->in_room;
	if ( d->ride_dir >= 0 && d->ride_dir < MAX_DIR )
	    move_char( ch, d->ride_dir, FALSE );
	else
	    do_road( ch, "" );

	if ( ch->in_room == oldroom
	||   ( d->ride_dir >= 0 && d->ride_dir < MAX_DIR && ch->in_room->sector_type == SECT_ROAD )
	||   !IS_SET( ch->in_room->room_flags, ROOM_VIRTUAL ) )
	    d->riding = FALSE;
	else
	{
	    for ( och = ch->in_room->people; och != NULL; och = och->next_in_room )
	    {
		if ( och == ch
		||   och == ch->mount
		||   och->master == ch
		||   ( ( rch = RIDDEN( och ) ) != NULL && rch->master == ch ) )
		    continue;
		d->riding = FALSE;
		break;
	    }
	}
    }
    else
	d->riding = FALSE;
}


void
check_room_aff( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *	room;
    AFFECT_DATA		af;
    CHAR_DATA *		mount;
    int			level;
    int			sn;

    if ( ( room = ch->in_room ) == NULL )
        return;

    if ( IS_IMMORTAL( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
        return;

    if ( IS_SET( room->affect_flags, ROOM_AFF_BLIND ) )
    {
        level = ch->level;
        sn = gsn_blindness;

        if ( !IS_AFFECTED( ch, AFF_BLIND ) && !saves_spell( level, ch, DAM_OTHER ) )
        {
            af.where	 = TO_AFFECTS;
            af.type	 = sn;
            af.level	 = level;
            af.duration	 = level / 2;
            af.location	 = APPLY_HITROLL;
            af.modifier	 = -4;
            af.bitvector = AFF_BLIND;
            affect_to_char( ch, &af );
            act_color( AT_MAGIC, "You are blinded!", ch, NULL, NULL, TO_CHAR, POS_RESTING );
            act_color( AT_MAGIC, "$n appears to be blinded.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
            return;
        }
    }

    if ( IS_SET( room->affect_flags, ROOM_AFF_CURSE ) )
    {
        level = ch->level;
        sn = gsn_curse;

        if ( !IS_AFFECTED( ch, AFF_CURSE ) && !saves_spell( level, ch, DAM_NEGATIVE ) )
        {
            af.where	 = TO_AFFECTS;
            af.type	 = sn;
            af.level	 = level;
            af.duration	 = level;
            af.location	 = APPLY_HITROLL;
            af.modifier	 = -1 * ( level / 8 );
            af.bitvector = AFF_CURSE;
            affect_to_char( ch, &af );
            act_color( AT_MAGIC, "You feel unclean", ch, NULL, NULL, TO_CHAR, POS_RESTING );
            act_color( AT_MAGIC, "$n looks very uncomfortable.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
            return;
        }
    }

    if ( IS_SET( room->affect_flags, ROOM_AFF_PLAGUE ) )
    {
        level = ch->level;
        sn = gsn_plague;

        if ( !IS_AFFECTED( ch, AFF_PLAGUE ) && !saves_spell( level, ch, DAM_NEGATIVE ) )
        {
            af.where	 = TO_AFFECTS;
            af.type	 = sn;
            af.level	 = level;
            af.duration	 = level;
            af.location	 = APPLY_STR;
            af.modifier	 = -5;
            af.bitvector = AFF_PLAGUE;
            affect_to_char( ch, &af );
            act_color( AT_MAGIC, "You scream in agony as plague sores erupt from your skin.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
            act_color( AT_MAGIC, "$n screams in agony as plague sores erupt from $s skin.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
            return;
        }
    }

    if ( IS_SET( room->affect_flags, ROOM_AFF_POISON ) )
    {
        level = ch->level;
        sn = gsn_poison;

        if ( !saves_spell( level, ch, DAM_POISON ) )
        {
            af.where	 = TO_AFFECTS;
            af.type	 = sn;
            af.level	 = level;
            af.duration	 = level;
            af.location	 = APPLY_STR;
            af.modifier	 = -2;
            af.bitvector = AFF_POISON;
            affect_join( ch, &af );
            act_color( AT_MAGIC, "You feel very sick.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
            act_color( AT_MAGIC, "$n looks very ill.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
            return;
        }
    }

    if ( IS_SET( room->affect_flags, ROOM_AFF_SLEEP ) )
    {
        if ( ch->fighting != NULL )
            stop_fighting( ch, FALSE );

        if ( ( mount = MOUNTED( ch ) ) != NULL )
        {
            ch->riding = FALSE;
            mount->riding = FALSE;
        }

        act_color( AT_MAGIC, "You fall into a deep stupor.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_MAGIC, "$n falls into a deep stupor.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
        ch->position = POS_SLEEPING;
        return;
    }

    if ( IS_SET( room->affect_flags, ROOM_AFF_SLOW ) )
    {
        level = ch->level;
        sn = skill_lookup( "slow" );

        if ( saves_spell( level, ch, DAM_OTHER ) )
            return;

        if ( IS_AFFECTED( ch, AFF_HASTE ) )
        {
            if ( !check_dispel( level, ch, skill_lookup( "haste" ) ) )
                return;
        }

        af.where	 = TO_AFFECTS;
        af.type	 	 = sn;
        af.level	 = level;
        af.duration	 = level / 2;
        af.location	 = APPLY_DEX;
        af.modifier	 = -1 - (level > 25) - (level >=50) - (level >= 70) - (level > 90);
        af.bitvector 	 = AFF_SLOW;
        affect_join( ch, &af );
        act_color( AT_MAGIC, "You feel yourself slowing d o w n...", ch, NULL, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_MAGIC, "$n starts to move in slow motion.", ch, NULL, NULL, TO_ROOM, POS_RESTING );

        return;
    }

    if ( IS_SET( room->affect_flags, ROOM_AFF_WEAKEN ) )
    {
        level = ch->level;
        sn = skill_lookup( "weaken" );

        if ( !saves_spell( level, ch, DAM_OTHER ) )
        {
            af.where	 = TO_AFFECTS;
            af.type	 = sn;
            af.level	 = level;
            af.duration	 = level / 2;
            af.location	 = APPLY_STR;
            af.modifier	 = -1 * (level/10);
            af.bitvector = AFF_WEAKEN;
            affect_join( ch, &af );
            act_color( AT_MAGIC, "You feel your strength slip away.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
            act_color( AT_MAGIC, "$n looks tired and weak.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
            return;
        }
    }

    return;
}


/*
 * Find a door.
 * Find secret door if fSecret is set.
 */
int
find_door( CHAR_DATA *ch, char *arg, bool fSecret )
{
    EXIT_DATA * pexit;
    int		door;

    if ( ( door = door_lookup( arg ) ) == DIR_NONE )
    {
	for ( door = 0; door < MAX_DIR; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET( pexit->exit_info, EX_ISDOOR )
	    &&   pexit->keyword != NULL
	    &&	 ( ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
		 || ( fSecret || !IS_SET( pexit->exit_info, EX_CLOSED ) || !IS_SET( pexit->exit_info, EX_SECRET ) ) )
	    &&   is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return DIR_NONE;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL
    ||	 ( IS_SET( pexit->exit_info, EX_SECRET ) && !fSecret
         && ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) ) )
    {
	if ( pexit != NULL && IS_SET( pexit->exit_info, EX_CLOSED ) )
	    act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	else
	    send_to_char( "There is no door in that direction.\n\r", ch );
	return DIR_NONE;
    }

    if ( !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	send_to_char( "There is no door in that direction.\n\r", ch );
	return DIR_NONE;
    }

    return door;
}


/* based on do_open */
void
do_knock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int door;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
        send_to_char( "Knock on what?\n\r", ch );
        return;
    }

    if ( ( door = find_door( ch, arg, FALSE ) ) >= 0 )
    {
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
             send_to_char( "It's already open, why knock?\n\r", ch );
             return;
        }

	act( "You knock on the $d $t.", ch, dir_desc[door], pexit->keyword, TO_CHAR );
	act( "$n knocks on the $d $t.", ch, dir_desc[door], pexit->keyword, TO_ROOM );

	eprog_knock_trigger( pexit, ch->in_room, ch );

	if ( ( to_room   = pexit->to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
        {
	    CHAR_DATA *rch;
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
	    {
		if ( IS_AFFECTED( rch, AFF_DEAF ) )
		    continue;
		act_color( AT_ACTION, "You hear someone knock on the $d $t.",
			   rch, dir_desc[rev_dir[door]], pexit_rev->keyword,
			   TO_CHAR, POS_RESTING );
	    }
        }
    }

    return;
}


void
do_open( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    OBJ_DATA *	obj;
    int		number;
    int		door;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) != NULL
    &&	 obj->item_type == ITEM_BOOK )
    {
	if ( obj->wear_loc != WEAR_HOLD )
	{
	    send_to_char( "You're not holding it.\n\r", ch );
	    return;
	}

        if ( obj->pIndexData->page == NULL )
        {
            act( "$p has no pages and cannot be opened.", ch, obj, NULL, TO_CHAR );
            return;
        }

        if ( obj->value[0] != 0 && *argument == '\0' )
        {
            send_to_char( "It's already open.\n\r", ch );
            return;
        }

        if ( IS_SET( obj->value[1], CONT_LOCKED ) )
        {
            send_to_char( "It's locked.\n\r", ch );
            return;
        }

        if ( *argument != 0 )
        {
            if ( !is_number( argument ) )
            {
                send_to_char( "That's not a page number.\n\r", ch );
                return;
            }
            number = atoi( argument );
        }
        else
            number = 1;

        if ( number < 1 )
        {
            send_to_char( "How's that again?  Open to which page?\n\r", ch );
            return;
        }

        if ( count_pages( obj->pIndexData ) < number )
        {
            act( "$p doesn't have that many pages.", ch, obj, NULL, TO_CHAR );
            return;
        }

        obj->value[0] = number;
        sprintf( buf, "You open $p to page %d.", number );
        act_color( AT_ACTION, buf, ch, obj, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n opens $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
        oprog_open_trigger( obj, ch );
        return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
        /* open book */
        if ( obj->item_type == ITEM_BOOK )
        {
            send_to_char( "You're not holding it.\n\r", ch );
            return;
        }

 	/* open portal */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], GATE_CLOSEABLE ) )
	    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
	    }

	    if ( !IS_SET( obj->value[1], GATE_CLOSED ) )
	    {
		send_to_char( "It's already open.\n\r", ch );
		return;
	    }

	    if ( IS_SET( obj->value[1], GATE_LOCKED ) )
	    {
		send_to_char( "It's locked.\n\r", ch );
		return;
	    }

	    REMOVE_BIT( obj->value[1], GATE_CLOSED );
	    act_color( AT_ACTION, "You open $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n opens $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
	    oprog_open_trigger( obj, ch );
	    return;
 	}

	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER)
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	act("You open $p.",ch,obj,NULL,TO_CHAR);
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	oprog_open_trigger( obj, ch );
	return;
    }

    if ( ( door = find_door( ch, arg, FALSE ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );
	eprog_open_trigger( pexit, ch->in_room, ch );

	/* open the other side */
	if ( ( to_room   = pexit->to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}


void
do_close( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    OBJ_DATA *	obj;
    int		door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) )
    &&	 obj->item_type == ITEM_BOOK )
    {
        if ( obj->value[0] == 0 )
        {
            send_to_char( "It's already closed.\n\r", ch );
            return;
        }

        obj->value[0] = 0;
        act_color( AT_ACTION, "You close $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n closes $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
        oprog_close_trigger( obj, ch );
        return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
        /* close book */
        if ( obj->item_type == ITEM_BOOK )
        {
            send_to_char( "You're not holding it.\n\r", ch );
            return;
        }

	/* portal stuff */
	if ( obj->item_type == ITEM_PORTAL )
	{

	    if ( !IS_SET( obj->value[1], GATE_CLOSEABLE ) )
	    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
	    }

	    if ( IS_SET( obj->value[1], GATE_CLOSED ) )
	    {
		send_to_char( "It's already closed.\n\r", ch );
		return;
	    }

	    SET_BIT( obj->value[1], GATE_CLOSED );
	    act_color( AT_ACTION, "You close $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n closes $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
	    oprog_close_trigger( obj, ch );
	    return;
	}

	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT(obj->value[1], CONT_CLOSED);
	act("You close $p.",ch,obj,NULL,TO_CHAR);
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	oprog_close_trigger( obj, ch );
	return;
    }

    if ( ( door = find_door( ch, arg, FALSE ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );
	eprog_close_trigger( pexit, ch->in_room, ch );

	/* close the other side */
	if ( ( to_room   = pexit->to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}


OBJ_DATA *
has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return obj;
    }

    return FALSE;
}


void
do_lock( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    OBJ_DATA *	obj;
    OBJ_DATA *	key;
    int		door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( MOUNTED( ch ) )
    {
	send_to_char( "You can't reach the lock from your mount.\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
        /* lock book */
        if ( obj->item_type == ITEM_BOOK )
        {
            if ( obj->value[0] != 0 )
            {
                send_to_char( "It's not closed.\n\r", ch );
                return;
            }

            if ( obj->value[2] <= 0 )
            {
                send_to_char( "It can't be locked.\n\r", ch );
                return;
            }

            if ( !( key = has_key( ch, obj->value[2] ) ) )
            {
                send_to_char( "You lack the key.\n\r", ch );
                return;
            }

            if ( IS_SET(obj->value[1], CONT_LOCKED) )
            {
                send_to_char( "It's already locked.\n\r", ch );
                return;
            }

            SET_BIT( obj->value[1], CONT_LOCKED );
            act_color( AT_ACTION, "You lock $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
            act_color( AT_ACTION, "$n locks $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
            oprog_lock_trigger( obj, ch, key );
            return;
        }

	/* portal stuff */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], GATE_CLOSEABLE ) )
	    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
	    }
	    if ( !IS_SET( obj->value[1], GATE_CLOSED ) )
	    {
		send_to_char( "It's not closed.\n\r", ch );
	 	return;
	    }

	    if ( obj->value[2] <= 0 )
	    {
		send_to_char( "It can't be locked.\n\r", ch );
		return;
	    }

	    if ( !( key = has_key( ch, obj->value[2] ) ) )
	    {
		send_to_char( "You lack the key.\n\r", ch );
		return;
	    }

	    if ( IS_SET( obj->value[1], GATE_LOCKED ) )
	    {
		send_to_char( "It's already locked.\n\r", ch );
		return;
	    }

	    SET_BIT( obj->value[1], GATE_LOCKED );
	    act_color( AT_ACTION, "You lock $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n locks $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
	    oprog_lock_trigger( obj, ch, key );
	    return;
	}

	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !( key = has_key( ch, obj->value[2] ) ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	act("You lock $p.",ch,obj,NULL,TO_CHAR);
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	oprog_lock_trigger( obj, ch, key );
	return;
    }

    if ( ( door = find_door( ch, arg, FALSE ) ) >= 0 )
    {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( ( key = has_key( ch, pexit->key ) ) == NULL )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	eprog_lock_trigger( pexit, ch->in_room, ch, key );

	/* lock the other side */
	if ( ( to_room   = pexit->to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}


void
do_unlock( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    OBJ_DATA *	obj;
    OBJ_DATA *	key;
    int		door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( MOUNTED( ch ) )
    {
	send_to_char( "You can't reach the lock from your mount.\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
        /* unlock book */
        if ( obj->item_type == ITEM_BOOK )
        {
            if ( obj->value[0] != 0 )
            {
                send_to_char( "It's not closed.\n\r", ch );
                return;
            }

            if ( obj->value[2] <= 0 )
            {
                send_to_char( "It can't be unlocked.\n\r", ch );
                return;
            }

            if ( !( key = has_key( ch, obj->value[2] ) ) )
            {
                send_to_char( "You lack the key.\n\r", ch );
                return;
            }

            if ( !IS_SET(obj->value[1], CONT_LOCKED) )
            {
                send_to_char( "It's already unlocked.\n\r", ch );
                return;
            }

            REMOVE_BIT( obj->value[1], CONT_LOCKED );
            act_color( AT_ACTION, "You unlock $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
            act_color( AT_ACTION, "$n unlocks $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
            oprog_unlock_trigger( obj, ch, key );
            if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOOPEN )
            &&	 get_eq_char( ch, WEAR_HOLD ) == obj )
	        do_open( ch, arg );
            return;
        }

 	/* portal stuff */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], GATE_CLOSEABLE ) )
	    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
	    }

	    if ( !IS_SET( obj->value[1], GATE_CLOSED ) )
	    {
		send_to_char( "It's not closed.\n\r", ch );
		return;
	    }

	    if ( obj->value[2] <= 0)
	    {
		send_to_char( "It can't be unlocked.\n\r", ch );
		return;
	    }

	    if ( !( key = has_key( ch, obj->value[2] ) ) )
	    {
		send_to_char( "You lack the key.\n\r", ch );
		return;
	    }

	    if ( !IS_SET( obj->value[1], GATE_LOCKED ) )
	    {
		send_to_char( "It's already unlocked.\n\r", ch );
		return;
	    }

	    REMOVE_BIT( obj->value[1], GATE_LOCKED );
	    act_color( AT_ACTION, "You unlock $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n unlocks $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
	    oprog_unlock_trigger( obj, ch, key );
	    if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOOPEN ) )
		do_open( ch, arg );
	    return;
	}

	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !( key = has_key( ch, obj->value[2] ) ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	act("You unlock $p.",ch,obj,NULL,TO_CHAR);
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	oprog_unlock_trigger( obj, ch, key );
	if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOOPEN ) )
	    do_open( ch, arg );
	return;
    }

    if ( ( door = find_door( ch, arg, FALSE ) ) >= 0 )
    {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( ( key = has_key( ch, pexit->key) ) == NULL )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	eprog_unlock_trigger( pexit, ch->in_room, ch, key );

	/* unlock the other side */
	if ( ( to_room   = pexit->to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOOPEN ) )
	    do_open( ch, arg );
    }

    return;
}


void
do_pick( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	gch;
    OBJ_DATA *	obj;
    int		door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    if ( MOUNTED( ch ) )
    {
	send_to_char( "You can't pick locks while mounted.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) )
	WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
	    act( "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    if ( !IS_NPC(ch) && number_percent( ) > get_skill(ch,gsn_pick_lock))
    {
	send_to_char( "You failed.\n\r", ch);
	check_improve(ch,gsn_pick_lock,FALSE,2);
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
        /* pick book */
        if ( obj->item_type == ITEM_BOOK )
        {
            if ( obj->value[0] != 0 )
            {
                send_to_char( "It's not closed.\n\r", ch );
                return;
            }

            if ( obj->value[2] <= 0 )
            {
                send_to_char( "It can't be unlocked.\n\r", ch );
                return;
            }

            if ( !IS_SET( obj->value[1], CONT_LOCKED ) )
            {
                send_to_char( "It's already unlocked.\n\r", ch );
                return;
            }

            if ( IS_SET( obj->value[1], CONT_PICKPROOF ) )
            {
                send_to_char( "You failed.\n\r", ch );
                return;
            }

            REMOVE_BIT( obj->value[1], CONT_LOCKED );
            act_color( AT_ACTION, "You pick the lock on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
            act_color( AT_ACTION, "$n picks the lock on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
            oprog_pick_trigger( obj, ch );
            check_improve( ch, gsn_pick_lock, TRUE, 2 );
            if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOOPEN )
            &&	 get_eq_char( ch, WEAR_HOLD ) == obj )
	        do_open( ch, arg );
            return;
        }

	/* portal stuff */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], GATE_CLOSEABLE ) )
	    {	
		send_to_char( "You can't do that.\n\r", ch );
		return;
	    }

	    if ( !IS_SET( obj->value[1], GATE_CLOSED ) )
	    {
		send_to_char( "It's not closed.\n\r", ch );
		return;
	    }

	    if ( obj->value[2] <= 0 )
	    {
		send_to_char( "It can't be unlocked.\n\r", ch );
		return;
	    }

	    if ( IS_SET( obj->value[1], GATE_PICKPROOF ) )
	    {
		send_to_char( "You failed.\n\r", ch );
		return;
	    }

	    REMOVE_BIT( obj->value[1], GATE_LOCKED );
	    act_color( AT_ACTION, "You pick the lock on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n picks the lock on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
	    oprog_pick_trigger( obj, ch );
	    check_improve( ch, gsn_pick_lock, TRUE, 2 );
	    if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOOPEN ) )
		do_open( ch, arg );
	    return;
	}

	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
        act_color( AT_ACTION, "You pick the lock on $p.",
                   ch, obj, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n picks the lock on $p.",
                   ch, obj, NULL, TO_ROOM, POS_RESTING );
	oprog_pick_trigger( obj, ch );
	check_improve(ch,gsn_pick_lock,TRUE,2);
	if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOOPEN ) )
	    do_open( ch, arg );
	return;
    }

    if ( ( door = find_door( ch, arg, FALSE ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 && !IS_IMMORTAL(ch))
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act_color( AT_ACTION, "$n picks the lock on the $d.",
	           ch, NULL, pexit->keyword, TO_ROOM, POS_RESTING );
	check_improve(ch,gsn_pick_lock,TRUE,2);
	eprog_pick_trigger( pexit, ch->in_room, ch );

	/* pick the other side */
	if ( ( to_room   = pexit->to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOOPEN ) )
	    do_open( ch, arg );
    }

    return;
}


void
do_stand( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	obj;
    OBJ_DATA *	from_obj;
    bool	fTriggered;

    obj = NULL;
    from_obj = ch->on;

    if (argument[0] != '\0')
    {
	if (ch->position == POS_FIGHTING)
	{
	    send_to_char("Maybe you should finish fighting first?\n\r",ch);
	    return;
	}
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
	if (obj->item_type != ITEM_FURNITURE
	||  (!IS_SET(obj->value[2],STAND_AT)
	&&   !IS_SET(obj->value[2],STAND_ON)
	&&   !IS_SET(obj->value[2],STAND_IN)))
	{
	    send_to_char("You can't seem to find a place to stand.\n\r",ch);
	    return;
	}
	if (ch->on != obj && count_users(obj) >= obj->value[0])
	{
	    act_new("There's no room to stand on $p.",
		ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
	}
 	ch->on = obj;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED( ch, AFF_SLEEP ) || check_nightmare( ch ) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

	if (obj == NULL)
	{
	    send_to_char( "You wake and stand up.\n\r", ch );
	    act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	    ch->on = NULL;
	}
	else if (IS_SET(obj->value[2],STAND_AT))
	{
	   act_new("You wake and stand at $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	   act("$n wakes and stands at $p.",ch,obj,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],STAND_ON))
	{
	    act_new("You wake and stand on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    act("$n wakes and stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	else 
	{
	    act_new("You wake and stand in $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    act("$n wakes and stands in $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_STANDING;
	do_function(ch, &do_look, "auto");
	break;

    case POS_RESTING: case POS_SITTING:
	if (obj == NULL)
	{
	    send_to_char( "You stand up.\n\r", ch );
	    act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
	    ch->on = NULL;
	}
	else if (IS_SET(obj->value[2],STAND_AT))
	{
	    act("You stand at $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands at $p.",ch,obj,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],STAND_ON))
	{
	    act("You stand on $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	else
	{
	    act("You stand in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	return;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	return;
    }

    if ( obj == NULL )
        fTriggered = rprog_stand_trigger( ch->in_room, ch );
    else
        fTriggered = oprog_use_trigger( obj, ch, NULL );

    if ( from_obj != NULL && !fTriggered )
        oprog_unuse_trigger( from_obj, ch );

    return;
}


void
do_rest( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if ( check_nightmare( ch ) )
    {
        send_to_char( "You can't wake up.\n\r", ch );
        return;
    }

    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't rest while mounted.\n\r", ch );
	return;
    }

    if ( RIDDEN( ch ) != NULL )
    {
	send_to_char( "You can't rest while being ridden.\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You are already fighting!\n\r", ch );
	return;
    }

    /* okay, now that we know we can rest, find an object to rest on */
    if (argument[0] != '\0')
    {
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else obj = ch->on;

    if (obj != NULL)
    {
        if (obj->item_type != ITEM_FURNITURE
    	||  (!IS_SET(obj->value[2],REST_ON)
    	&&   !IS_SET(obj->value[2],REST_IN)
    	&&   !IS_SET(obj->value[2],REST_AT)))
    	{
	    send_to_char("You can't rest on that.\n\r",ch);
	    return;
    	}

        if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
        {
	    act_new("There's no more room on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
    	}

	ch->on = obj;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	if (IS_AFFECTED(ch,AFF_SLEEP))
	{
	    send_to_char("You can't wake up!\n\r",ch);
	    return;
	}

	if (obj == NULL)
	{
	    send_to_char( "You wake up and start resting.\n\r", ch );
	    act ("$n wakes up and starts resting.",ch,NULL,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],REST_AT))
	{
	    act_new("You wake up and rest at $p.",
		    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
	    act("$n wakes up and rests at $p.",ch,obj,NULL,TO_ROOM);
	}
        else if (IS_SET(obj->value[2],REST_ON))
        {
            act_new("You wake up and rest on $p.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
            act("$n wakes up and rests on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
            act_new("You wake up and rest in $p.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
            act("$n wakes up and rests in $p.",ch,obj,NULL,TO_ROOM);
        }
	ch->position = POS_RESTING;
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	if (obj == NULL)
	{
	    send_to_char( "You rest.\n\r", ch );
	    act( "$n sits down and rests.", ch, NULL, NULL, TO_ROOM );
	}
        else if (IS_SET(obj->value[2],REST_AT))
        {
	    act("You sit down at $p and rest.",ch,obj,NULL,TO_CHAR);
	    act("$n sits down at $p and rests.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],REST_ON))
        {
	    act("You sit on $p and rest.",ch,obj,NULL,TO_CHAR);
	    act("$n sits on $p and rests.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
	    act("You rest in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests in $p.",ch,obj,NULL,TO_ROOM);
        }
	ch->position = POS_RESTING;
	break;

    case POS_SITTING:
	if (obj == NULL)
	{
	    send_to_char("You rest.\n\r",ch);
	    act("$n rests.",ch,NULL,NULL,TO_ROOM);
	}
        else if (IS_SET(obj->value[2],REST_AT))
        {
	    act("You rest at $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests at $p.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],REST_ON))
        {
	    act("You rest on $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
	    act("You rest in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests in $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_RESTING;
	break;
    }

    if ( obj == NULL )
	rprog_rest_trigger( ch->in_room, ch );
    else
	oprog_use_trigger( obj, ch, NULL );

    return;
}


void
do_sit( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if ( check_nightmare( ch ) )
    {
        send_to_char( "You can't wake up.\n\r", ch );
        return;
    }

    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't sit while mounted.\n\r", ch );
	return;
    }

    if ( RIDDEN( ch ) != NULL )
    {
	send_to_char( "You can't sit while being ridden.\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "Maybe you should finish this fight first?\n\r", ch );
	return;
    }

    /* okay, now that we know we can sit, find an object to sit on */
    if (argument[0] != '\0')
    {
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else obj = ch->on;

    if (obj != NULL)                                                              
    {
	if (obj->item_type != ITEM_FURNITURE
	||  (!IS_SET(obj->value[2],SIT_ON)
	&&   !IS_SET(obj->value[2],SIT_IN)
	&&   !IS_SET(obj->value[2],SIT_AT)))
	{
	    send_to_char("You can't sit on that.\n\r",ch);
	    return;
	}

	if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
	{
	    act_new("There's no more room on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
	}

	ch->on = obj;
    }
    switch (ch->position)
    {
	case POS_SLEEPING:
	    if (IS_AFFECTED(ch,AFF_SLEEP))
	    {
		send_to_char("You can't wake up!\n\r",ch);
		return;
	    }

            if (obj == NULL)
            {
            	send_to_char( "You wake and sit up.\n\r", ch );
            	act( "$n wakes and sits up.", ch, NULL, NULL, TO_ROOM );
            }
            else if (IS_SET(obj->value[2],SIT_AT))
            {
            	act_new("You wake and sit at $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits at $p.",ch,obj,NULL,TO_ROOM);
            }
            else if (IS_SET(obj->value[2],SIT_ON))
            {
            	act_new("You wake and sit on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits at $p.",ch,obj,NULL,TO_ROOM);
            }
            else
            {
            	act_new("You wake and sit in $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits in $p.",ch,obj,NULL,TO_ROOM);
            }

	    ch->position = POS_SITTING;
	    break;
	case POS_RESTING:
	    if (obj == NULL)
		send_to_char("You stop resting.\n\r",ch);
	    else if (IS_SET(obj->value[2],SIT_AT))
	    {
		act("You sit at $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits at $p.",ch,obj,NULL,TO_ROOM);
	    }

	    else if (IS_SET(obj->value[2],SIT_ON))
	    {
		act("You sit on $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    ch->position = POS_SITTING;
	    break;
	case POS_SITTING:
	    send_to_char("You are already sitting down.\n\r",ch);
	    break;
	case POS_STANDING:
	    if (obj == NULL)
    	    {
		send_to_char("You sit down.\n\r",ch);
    	        act("$n sits down on the ground.",ch,NULL,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SIT_AT))
	    {
		act("You sit down at $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits down at $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SIT_ON))
	    {
		act("You sit on $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else
	    {
		act("You sit down in $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits down in $p.",ch,obj,NULL,TO_ROOM);
	    }
    	    ch->position = POS_SITTING;
    	    break;
    }

    if ( obj == NULL )
	rprog_sit_trigger( ch->in_room, ch );
    else
	oprog_use_trigger( obj, ch, NULL );
    return;
}


void
do_sleep( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if ( MOUNTED( ch ) != NULL )
    {
	send_to_char( "You can't sleep while mounted.\n\r", ch );
	return;
    }

    if ( RIDDEN( ch ) != NULL )
    {
	send_to_char( "You can't sleep while being ridden.\n\r", ch );
	return;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_SITTING:
    case POS_STANDING: 
	if (argument[0] == '\0' && ch->on == NULL)
	{
	    send_to_char( "You go to sleep.\n\r", ch );
	    act( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
	    ch->position = POS_SLEEPING;
	    set_dreaming( ch );
	}
	else  /* find an object and sleep on it */
	{
	    if (argument[0] == '\0')
		obj = ch->on;
	    else
	    	obj = get_obj_list( ch, argument,  ch->in_room->contents );

	    if (obj == NULL)
	    {
		send_to_char("You don't see that here.\n\r",ch);
		return;
	    }
	    if (obj->item_type != ITEM_FURNITURE
	    ||  (!IS_SET(obj->value[2],SLEEP_ON) 
	    &&   !IS_SET(obj->value[2],SLEEP_IN)
	    &&	 !IS_SET(obj->value[2],SLEEP_AT)))
	    {
		send_to_char("You can't sleep on that!\n\r",ch);
		return;
	    }

	    if (ch->on != obj && count_users(obj) >= obj->value[0])
	    {
		act_new("There is no room on $p for you.",
		    ch,obj,NULL,TO_CHAR,POS_DEAD);
		return;
	    }

	    ch->on = obj;
	    if (IS_SET(obj->value[2],SLEEP_AT))
	    {
		act("You go to sleep at $p.",ch,obj,NULL,TO_CHAR);
		act("$n goes to sleep at $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SLEEP_ON))
	    {
	        act("You go to sleep on $p.",ch,obj,NULL,TO_CHAR);
	        act("$n goes to sleep on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else
	    {
		act("You go to sleep in $p.",ch,obj,NULL,TO_CHAR);
		act("$n goes to sleep in $p.",ch,obj,NULL,TO_ROOM);
	    }
	    ch->position = POS_SLEEPING;
	    set_dreaming( ch );
	}
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    if ( obj == NULL )
	rprog_sleep_trigger( ch->in_room, ch );
    else
	oprog_use_trigger( obj, ch, NULL );

    return;
}


void
do_wake( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    CHAR_DATA *		victim;
    ROOM_INDEX_DATA *	room;

    one_argument( argument, arg );

    room = ch->in_room;

    if ( arg[0] == '\0' )
    {
        if ( check_nightmare( ch ) )
        {
            send_to_char( "You can't wake up.\n\r", ch );
            return;
        }
        do_function( ch, &do_stand, "" );
        if ( ch->in_room == room )
            rprog_wake_trigger( room, ch, ch );
        return;
    }

    if ( !IS_AWAKE( ch ) )
    {
        send_to_char( "You are asleep yourself!\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_AWAKE( victim ) )
    {
        act( "$N is already awake.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( IS_AFFECTED( victim, AFF_SLEEP ) || check_nightmare( victim ) )
    {
        act( "You can't wake $M!", ch, NULL, victim, TO_CHAR );
        return;
    }

    act_color( AT_ACTION, "You wake $N.",  ch, NULL, victim, TO_CHAR, POS_SLEEPING );
    act_color( AT_ACTION, "$n wakes you.", ch, NULL, victim, TO_VICT, POS_SLEEPING );
    act_color( AT_ACTION, "$n wakes $N.",  ch, NULL, victim, TO_NOTVICT, POS_RESTING );
    do_function( victim, &do_stand, "" );
    if ( ch->in_room == room && victim->in_room == room )
        rprog_wake_trigger( room, victim, ch );
    return;
}


void
do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( MOUNTED( ch ) )
    {
	send_to_char( "You can't sneak while riding.\n\r", ch );
	return;
    }

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if (IS_AFFECTED(ch,AFF_SNEAK))
	return;

    if ( number_percent( ) < get_skill(ch,gsn_sneak))
    {
	check_improve(ch,gsn_sneak,TRUE,3);
	af.where     = TO_AFFECTS;
	af.type      = gsn_sneak;
	af.level     = ch->level; 
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }
    else
	check_improve(ch,gsn_sneak,FALSE,3);

    return;
}


void
do_hide( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    OBJ_DATA *	pObj;

    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    {
	if ( ch->in_room == NULL )
	{
	    send_to_char( "You can't do that here, because there is no \"here\".\n\r", ch );
	    return;
	}

	if ( ( pObj = get_obj_carry( ch, arg, ch ) ) != NULL )
	{
	    if ( !can_drop_obj( ch, pObj ) )
	    {
		send_to_char( "You can't let go of it.\n\r", ch );
		return;
	    }
	    obj_from_char( pObj );
	    obj_to_room( pObj, ch->in_room );
	}
	else if ( ( pObj = get_obj_list( ch, arg, ch->in_room->contents ) ) != NULL )
	{
	    if ( !IS_SET( pObj->wear_flags, ITEM_TAKE ) )
	    {
		send_to_char( "You can't hide that.\n\r", ch );
		return;
	    }
	}
	else
	{
	    send_to_char( "You don't see that here.\n\r", ch );
	    return;
	}

	act_color( AT_ACTION, "You hide $p.", ch, pObj, NULL, TO_CHAR, POS_RESTING );
	SET_BIT( pObj->extra_flags, ITEM_HIDDEN );
	act_color( AT_ACTION, "$n hides $p.", ch, pObj, NULL, TO_ROOM, POS_RESTING );
	return;
    }

    send_to_char( "You attempt to hide.\n\r", ch );

    if ( MOUNTED( ch ) )
    {
	send_to_char( "You can't hide while riding.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
	xREMOVE_BIT( ch->affected_by, AFF_HIDE );

    if ( number_percent( ) < get_skill(ch,gsn_hide ) )
    {
	xSET_BIT( ch->affected_by, AFF_HIDE );
	check_improve( ch, gsn_hide, TRUE, 3 );
    }
    else
	check_improve( ch, gsn_hide, FALSE, 3 );

    return;
}


void
do_recall( CHAR_DATA *ch, char *argument )
{
/*
    if ( ch->level > LEVEL_NEWBIE && !IS_NPC( ch )
    && ( !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    {
        send_to_char( "You are of too high a level to do that.\n\r", ch );
        return;
    }
*/
    recall_char( ch, argument, 0 );
}


/*
 * Contributed by Alander.
 */
void
do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis			);
    affect_strip ( ch, gsn_mass_invis			);
    affect_strip ( ch, gsn_sneak			);
    xREMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    xREMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    xREMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "Ok.\n\r", ch );
    return;
}


void
recall_char( CHAR_DATA *ch, char *argument, int room )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    int	recall_room;

    if ( IS_NPC( ch ) && !IS_SET( ch->act, ACT_PET ) && ch->mount == NULL )
    {
	send_to_char( "Only players can recall.\n\r", ch );
	return;
    }

    act( "$n prays for transportation!", ch, 0, 0, TO_ROOM );

    if ( ch->clan != NULL && ( !str_cmp( argument, "clan" ) || !str_cmp( argument, "house" ) ) )
	room = ch->clan->recall;

    if ( room != 0 )
        recall_room = room;
    else if ( ch->in_room && ch->in_room->area && ch->in_room->area->recall )
	recall_room = ch->in_room->area->recall;
    else
	recall_room = ROOM_VNUM_TEMPLE;

    if ( ( location = get_room_index( recall_room ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
    ||	 !is_same_landmass( ch->in_room, location )
    ||   IS_AFFECTED( ch, AFF_CURSE ) )
    {
	if ( IS_NULLSTR( ch->in_room->area->norecall ) )
	    send_to_char( IMP_NAME " has forsaken you.\n\r", ch );
	else
	    ch_printf( ch, "%s\n\r", ch->in_room->area->norecall );
	return;
    }

    if ( ( victim = ch->fighting ) != NULL )
    {
	int lose,skill;

	skill = get_skill(ch,gsn_recall);

	if ( number_percent() < 80 * skill / 100 )
	{
	    check_improve(ch,gsn_recall,FALSE,6);
	    WAIT_STATE( ch, 4 );
	    sprintf( buf, "You failed.\n\r");
	    send_to_char( buf, ch );
	    return;
	}

	lose = (ch->desc != NULL) ? 25 : 50;
	gain_exp( ch, 0 - lose );
	check_improve(ch,gsn_recall,TRUE,4);
	sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }

    ch->move /= 2;
    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_function(ch, &do_look, "auto" );

    if ( ch->pet != NULL )
	recall_char( ch->pet, "", room );

    if ( MOUNTED( ch ) )
	recall_char( ch->mount, "", room );

    check_room_aff( ch );

    mprog_entry_trigger( ch );
    mprog_greet_trigger( ch );
    return;
}


void
do_train( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *mob;
    sh_int stat = - 1;
    char *pOutput = NULL;
    int cost;

    if ( IS_NPC(ch) )
	return;

    /*
     * Check for trainer.
     */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
	if ( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) )
	    break;
    }

    if ( mob == NULL || !can_see( ch, mob ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    if ( !xIS_SET( mob->pIndexData->train, ch->class ) )
    {
        ch_printf( ch, "%s cannot do that here.\n\r",
                   capitalize( aoran( class_table[ch->class].name ) ) );
        return;
    }

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "You have %d training sessions.\n\r", ch->train );
	send_to_char( buf, ch );
	argument = "foo";
    }

    cost = 1;

    if ( !str_cmp( argument, "str" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_STR )
	    cost    = 1;
	stat        = STAT_STR;
	pOutput     = "strength";
    }

    else if ( !str_cmp( argument, "int" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_INT )
	    cost    = 1;
	stat	    = STAT_INT;
	pOutput     = "intelligence";
    }

    else if ( !str_cmp( argument, "wis" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_WIS )
	    cost    = 1;
	stat	    = STAT_WIS;
	pOutput     = "wisdom";
    }

    else if ( !str_cmp( argument, "dex" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_DEX )
	    cost    = 1;
	stat  	    = STAT_DEX;
	pOutput     = "dexterity";
    }

    else if ( !str_cmp( argument, "con" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_CON )
	    cost    = 1;
	stat	    = STAT_CON;
	pOutput     = "constitution";
    }

    else if ( !str_cmp(argument, "hp" ) )
	cost = 1;

    else if ( !str_cmp(argument, "mana" ) )
	cost = 1;
  
  else if ( !str_cmp(argument, "move" ) )
	cost = 1;

    else
    {
	strcpy( buf, "You can train:" );
	if ( ch->perm_stat[STAT_STR] < get_max_train(ch,STAT_STR)) 
	    strcat( buf, " str" );
	if ( ch->perm_stat[STAT_INT] < get_max_train(ch,STAT_INT))  
	    strcat( buf, " int" );
	if ( ch->perm_stat[STAT_WIS] < get_max_train(ch,STAT_WIS)) 
	    strcat( buf, " wis" );
	if ( ch->perm_stat[STAT_DEX] < get_max_train(ch,STAT_DEX))  
	    strcat( buf, " dex" );
	if ( ch->perm_stat[STAT_CON] < get_max_train(ch,STAT_CON))  
	    strcat( buf, " con" );
	strcat( buf, " hp mana move");

	if ( buf[strlen(buf)-1] != ':' )
	{
	    strcat( buf, ".\n\r" );
	    send_to_char( buf, ch );
	}
	else
	{
	    /*
	     * This message dedicated to Jordan ... you big stud!
	     */
	    act( "You have nothing left to train, you $T!",
		ch, NULL,
		ch->sex == SEX_MALE   ? "big stud" :
		ch->sex == SEX_FEMALE ? "hot babe" :
					"wild thing",
		TO_CHAR );
	}

	return;
    }

    if (!str_cmp("hp",argument))
    {
    	if ( cost > ch->train )
    	{
       	    send_to_char( "You don't have enough training sessions.\n\r", ch );
            return;
        }

	ch->train -= cost;
        ch->pcdata->perm_hit += 10;
        ch->max_hit += 10;
        ch->hit +=10;
        act( "Your durability increases!",ch,NULL,NULL,TO_CHAR);
        act( "$n's durability increases!",ch,NULL,NULL,TO_ROOM);
        return;
    }

    if (!str_cmp("mana",argument))
    {
        if ( cost > ch->train )
        {
            send_to_char( "You don't have enough training sessions.\n\r", ch );
            return;
        }

	ch->train -= cost;
        ch->pcdata->perm_mana += 10;
        ch->max_mana += 10;
        ch->mana += 10;
        act( "Your power increases!",ch,NULL,NULL,TO_CHAR);
        act( "$n's power increases!",ch,NULL,NULL,TO_ROOM);
        return;
    }

if (!str_cmp("move",argument))
    {
        if ( cost > ch->train )
        {
            send_to_char( "You don't have enough training sessions.\n\r", ch );
            return;
        }

	ch->train -= cost;
        ch->pcdata->perm_move += 10;
        ch->max_move += 10;
        ch->move += 10;
        act( "Your stamina increases!",ch,NULL,NULL,TO_CHAR);
        act( "$n's stamina increases!",ch,NULL,NULL,TO_ROOM);
        return;
    }

    if ( ch->perm_stat[stat]  >= get_max_train(ch,stat) )
    {
	act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }

    if ( cost > ch->train )
    {
	send_to_char( "You don't have enough training sessions.\n\r", ch );
	return;
    }

    ch->train		-= cost;

    ch->perm_stat[stat]		+= 1;
    act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
    return;
}


void
do_push( CHAR_DATA *ch, char *argument )
{
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf [MAX_INPUT_LENGTH];
    int			door;
    EXTRA_DESCR_DATA *	edesc;
    bool		found;
    ROOM_INDEX_DATA *	from_room;
    char *		keylist;
    char		keyword[MAX_INPUT_LENGTH];
    EXIT_DATA *		pExit;
    ROOM_INDEX_DATA *	to_room;
    CHAR_DATA *		victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Push who what where?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	found = FALSE;
	for ( edesc = ch->in_room->extra_descr; edesc; edesc = edesc->next )
	{
	    if ( is_name( arg1, edesc->keyword ) )
	    {
		found = TRUE;
		break;
	    }
	}

	if ( !found )
	{
	    ch_printf( ch, "I see no %s here.\n\r", arg1 );
	    return;
	}

	for ( keylist = edesc->keyword; keylist != NULL; )
	{
	    keylist = one_argument( keylist, keyword );
	    if ( !str_prefix( arg1, keyword ) )
		break;
	}

	if ( !IS_SET( ch->in_room->progtypes, ROOM_PROG_PUSH ) )
	{
	    sprintf( buf, "You try to push the %s, but nothing happens.", keyword );
	    act_color( AT_ACTION, buf, ch, NULL, NULL, TO_CHAR, POS_RESTING );
	    sprintf( buf, "$n tries to push the %s.", keyword );
	    act_color( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM, POS_RESTING );
	    return;
        }

	rprog_push_trigger( ch->in_room, ch, keyword );
	return;
    }

    if ( IS_TETHERED( victim ) )
    {
        act( "$N is tethered.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( IS_IMMORTAL( victim ) && !IS_IMMORTAL( ch ) )
    {
        act_color( AT_ACTION, "You try to push $N.  Hopefully nothing happens to you.", ch, NULL, victim, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n tried to push you!", ch, NULL, victim, TO_VICT, POS_DEAD );
        act_color( AT_ACTION, "$n foolishly tries to push $N!", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
        return;
    }

    if ( is_safe( ch, victim ) )
    {
	send_to_char( "You cannot.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	act_color( AT_ACTION, "You attempt to push yourself.  Ook!", ch,
		   NULL, NULL, TO_CHAR, POS_RESTING );
	return;
    }

    if ( victim->position == POS_FIGHTING )
    {
	send_to_char( "They're fighting!  Best leave them alone!\n\r", ch );
	return;
    }

    if ( victim->level >= ch->level + 10 )
    {
	act( "$N ignores you.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->position != POS_STANDING )
    {
	send_to_char( "You can't push someone who is not standing.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
	door = number_door( );
    else
    {
	if ( ( door = door_lookup( arg2 ) ) == DIR_NONE )
	{
	    for ( door = 0; door < MAX_DIR; door++ )
		if ( ( pExit = ch->in_room->exit[door] ) != NULL
		&&   pExit->keyword != NULL
		&&   is_name( arg2, pExit->keyword ) )
		    break;
	}
	if ( door >= MAX_DIR )
	{
	    act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	    return;
	}
    }

    pExit = get_exit( ch->in_room, door );
    if ( pExit == NULL || IS_SET( pExit->exit_info, EX_CLOSED ) )
    {
	act_color( AT_ACTION, "There is no exit, but you push $M around anyways.",
		   ch, NULL, victim, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n pushes $N against a wall.",
		   ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	act_color( AT_ACTION, "$n pushes you against a wall, ouch.",
		   ch, NULL, victim, TO_VICT, POS_RESTING );
	return;
    }

    if ( pExit->to_room->sector_type == SECT_BOILING )
    {
	act_color( AT_ACTION, "If you want to boil $N, you'll have to find another way to do it.",
                   ch, NULL, victim, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n tries to push $N into the boiling waters!.",
		   ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	act_color( AT_ACTION, "$n tried to push you into the boiling waters!.",
		   ch, NULL, victim, TO_VICT, POS_RESTING );
	return;
    }

    if ( room_is_private( pExit->to_room ) )
    {
	act_color( AT_ACTION, "The room is private, $M bounces right back at you.",
		   ch, NULL, victim, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n pushes $N, but $E bounces right back at $m.",
		   ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	act_color( AT_ACTION, "$n pushes you, but you bounce right back at $m.",
		   ch, NULL, victim, TO_VICT, POS_RESTING );
	return;
    }

    sprintf( buf, "You slam into $N, pushing $M %s.", dir_name[door] );
    act_color( AT_ACTION, buf, ch, NULL, victim, TO_CHAR, POS_RESTING );
    sprintf( buf, "$n slams into you, pushing you %s.", dir_name[door] );
    act_color( AT_ACTION, buf, ch, NULL, victim, TO_VICT, POS_RESTING );
    sprintf( buf, "$n slams into $N, pushing $M %s.", dir_name[door] );
    act_color( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT, POS_RESTING );

    from_room = victim->in_room;
    to_room = pExit->to_room;

    char_from_room( victim );
    char_to_room( victim, pExit->to_room );
    do_look( victim, "auto" );
    check_room_aff( victim );
    act_color( AT_ACTION, "$n comes flying into the room.",
	       victim, NULL, NULL, TO_ROOM, POS_RESTING );

    if ( to_room->exit[rev_dir[door]] != NULL
    &&	 to_room->exit[rev_dir[door]]->to_room == from_room )
	eprog_enter_trigger( to_room->exit[rev_dir[door]], victim->in_room, victim );
    else
	rprog_enter_trigger( victim->in_room, victim );

    if ( from_room == to_room ) /* no circular follows */
	return;

    check_nofloor( victim );

    mprog_greet_trigger( victim );

    return;
}


void
do_pull( CHAR_DATA *ch, char *argument )
{
    EXTRA_DESCR_DATA *	edesc;
    char		arg[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH*2];
    char		keyword[ MAX_INPUT_LENGTH ];
    char *		keylist;
    bool		found;

    one_argument( argument, arg );

    if ( ch->in_room == NULL )
    {
	send_to_char( "You're not in a room!\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pull what?\n\r", ch );
	return;
    }

    found = FALSE;
    for ( edesc = ch->in_room->extra_descr; edesc; edesc = edesc->next )
    {
	if ( is_name( arg, edesc->keyword ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
    {
	ch_printf( ch, "I see no %s here.\n\r", arg );
	return;
    }

    for ( keylist = edesc->keyword; keylist != NULL; )
    {
	keylist = one_argument( keylist, keyword );
	if ( !str_prefix( arg, keyword ) )
	    break;
    }

    if ( !IS_SET( ch->in_room->progtypes, ROOM_PROG_PULL ) )
    {
        sprintf( buf, "You try to pull the %s, but nothing happens.", keyword );
        act_color( AT_EMOTE, buf, ch, NULL, NULL, TO_CHAR, POS_RESTING );
        sprintf( buf, "$n tries to pull the %s.", keyword );
        act_color( AT_EMOTE, buf, ch, NULL, NULL, TO_ROOM, POS_RESTING );
        return;
    }

    rprog_pull_trigger( ch->in_room, ch, keyword );

    return;

}


void
do_ride( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		dir;

    if ( ch->desc == NULL )
	return;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
	strcpy( arg, "road" );

    if ( MOUNTED( ch ) == NULL )
    {
	send_to_char( "You're not riding anything.\n\r", ch );
	return;
    }

    if ( ch->in_room != ch->mount->in_room )
    {
	send_to_char( "Your mount is not here.\n\r", ch );
	return;
    }

    if ( IS_TETHERED( ch->mount ) )
    {
        send_to_char( "Your mount is tethered.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "road" ) )
	dir = -1;
    else if ( ( dir = dir_lookup( arg ) ) == DIR_NONE )
    {
	send_to_char( "That's not a direction.\n\r", ch );
	return;
    }

    ch->desc->ride_dir = dir;
    ch->desc->riding = TRUE;
    check_riding( ch->desc );
}


void
do_road( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_INPUT_LENGTH];
    EXIT_DATA *	pExit;
    int		door;
    int		count;
    int		road_dir;
    char *	p;

    if ( ch->in_room == NULL || ch->in_room->sector_type != SECT_ROAD )
    {
	send_to_char( "You're not on a road.\n\r", ch );
	return;
    }

    count = 0;
    road_dir = DIR_NONE;
    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( door != ch->from_dir
	&&   ( pExit = get_exit( ch->in_room, door ) ) != NULL
	&&   pExit->to_room != NULL
	&&   pExit->to_room->sector_type == SECT_ROAD )
	{
	    road_dir = door;
	    count++;
	}
    }    

    if ( count == 1 )
    {
	ch_printf( ch, "You follow the road %s.\n\r", dir_name[road_dir] );
	move_char( ch, road_dir, FALSE );
	return;
    }

    if ( count == 0 )
    {
	send_to_char( "You are at the end of the road.\n\r", ch );
	return;
    }

    p = stpcpy( buf, "Roads lead" );
    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( door != ch->from_dir
	&&   ( pExit = get_exit( ch->in_room, door ) ) != NULL
	&&   pExit->to_room != NULL
	&&   pExit->to_room->sector_type == SECT_ROAD )
	{
	    if ( count == 1 )
		p = stpcpy( p, " and" );
	    count--;
	    *p++ = ' ';
	    p = stpcpy( p, dir_name[door] );
	}
    }

    p = stpcpy( p, ".\n\r" );
    send_to_char( buf, ch );

    return;
}


void
do_embark( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	vehicle;
    ROOM_INDEX_DATA *	from_room;
    CHAR_DATA *		fch;
    CHAR_DATA *		fch_next;

    if ( *argument == '\0' )
    {
	send_to_char( "Embark what?\n\r", ch );
	return;
    }

    if ( ( vehicle = get_room_room( ch, argument ) ) == NULL )
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    act_color( AT_ACTION, "You embark on $t.", ch,
	       ROOMNAME( vehicle ), NULL, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n embarks on $t.", ch,
	       ROOMNAME( vehicle ), NULL, TO_ROOM, POS_RESTING );

    from_room = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, vehicle );
    do_look( ch, "auto" );
    act_color( AT_ACTION, "$n embarks on $t.", ch,
	       ROOMNAME( vehicle ), NULL, TO_ROOM, POS_RESTING );

    /* Protect against circular follows, even though this can't happen (yeah, right) */
    if ( from_room == vehicle )
        return;

    for ( fch = from_room->people; fch != NULL; fch = fch_next )
    {
        fch_next = fch->next_in_room;
        if ( fch->master == ch && IS_AFFECTED( fch, AFF_CHARM )
        &&   fch->position < POS_STANDING )
        {
            do_stand( fch, "" );
        }

        if ( fch->master == ch && fch->position == POS_STANDING )
        {
            if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
            &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
            {
                act( "You can't bring $N into the city.",
                      ch, NULL, fch, TO_CHAR );
                act( "You aren't allowed in the city.",
                      fch, NULL, NULL, TO_CHAR );
                continue;
            }
            act_color( AT_ACTION, "You follow $N.", fch, NULL, ch, TO_CHAR, POS_RESTING );
            do_embark( fch, argument );
        }
    }
    rprog_enter_trigger( ch->in_room, ch );
    check_nofloor( ch );

    return;
}


/* RT Enter portals */
void
do_enter( CHAR_DATA *ch, char *argument)
{    
    ROOM_INDEX_DATA *	location;
    int			size;

    if ( ch->fighting != NULL )
    {
        send_to_char( "No way!  You are too busy fighting!\n\r", ch );
	return;
    }

    if ( IS_TETHERED( ch ) )
        return;

    /* nifty portal stuff */
    if ( argument[0] != '\0' )
    {
        ROOM_INDEX_DATA *old_room;
	OBJ_DATA *portal;
	CHAR_DATA *fch, *fch_next;

        old_room = ch->in_room;

	portal = get_obj_list( ch, argument,  ch->in_room->contents );

	if (portal == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}

	if ( portal->item_type != ITEM_PORTAL 
        ||  ( IS_SET( portal->value[1], GATE_CLOSED ) && !IS_TRUSTED( ch, ANGEL ) ) )
	{
	    send_to_char( "You can't seem to find a way in.\n\r", ch );
	    return;
	}

	if ( !IS_TRUSTED( ch, ANGEL ) && !IS_SET( portal->value[1], GATE_NOCURSE )
	&&   ( IS_AFFECTED( ch, AFF_CURSE ) 
	||   ( IS_SET( old_room->room_flags, ROOM_NO_RECALL )
        &&     IS_SET( portal->wear_flags, ITEM_TAKE ) ) ) )
	{
	    send_to_char( "Something prevents you from leaving...\n\r", ch );
	    return;
	}

	if ( get_trust( ch ) < portal->level )
	{
	    send_to_char( "You are of too low a level to do that.\n\r", ch );
	    return;
	}

	if ( IS_SET( portal->value[1], GATE_RANDOM ) || portal->value[3] == -1 )
	{
	    location = get_random_room( ch );
	    portal->value[3] = location->vnum; /* for record keeping :) */
	}
	else if ( IS_SET( portal->value[1],GATE_BUGGY ) && ( number_percent() < 5 ) )
	    location = get_random_room( ch );
	else
	    location = get_room_index( portal->value[3] );

	if ( location == NULL
	||   location == old_room
	||   !can_see_room(ch,location) 
	||   ( room_is_private( location ) && !IS_TRUSTED( ch, IMPLEMENTOR ) ) )
	{
	   act( "$p`X doesn't seem to go anywhere.", ch, portal, NULL, TO_CHAR );
	   return;
	}

        if ( IS_NPC(ch ) && IS_SET( ch->act, ACT_AGGRESSIVE )
        &&   IS_SET( location->room_flags, ROOM_LAW ) )
        {
            send_to_char( "Something prevents you from leaving...\n\r", ch );
            return;
        }

	if ( IS_SET( portal->value[1], GATE_WINDOW ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( ch->mount != NULL && ch->riding )
	{
	    size = UMAX( ch->size, ch->mount->size );
	    if ( ch->size >= ch->mount->size - 2
	    &&   ch->mount->size >= ch->size - 2 )
	        size = UMIN( size + 1, SIZE_TITANIC );
        }
        else
	    size = ch->size;

        if ( size > portal->value[4] )
        {
	    send_to_char( "You ", ch );
	    if ( ch->riding )
	        ch_printf( ch, "and %s ", PERS( ch->mount, ch ) );
            send_to_char( "can't fit through there.\n\r", ch );
            return;
        }

	act_color( AT_ACTION, "$n steps into $p.", ch, portal, NULL, TO_ROOM, POS_RESTING );

	if ( IS_SET( portal->value[1], GATE_NORMAL_EXIT ) )
	    act( "You enter $p.", ch, portal, NULL,TO_CHAR );
	else
	    act( "You walk through $p`X and find yourself somewhere else...",
	        ch, portal, NULL, TO_CHAR ); 

	char_from_room( ch );
	char_to_room( ch, location );

	if ( IS_SET( portal->value[1], GATE_GOWITH ) ) /* take the gate along */
	{
	    obj_from_room( portal );
	    obj_to_room( portal, location );
	}

	if ( IS_SET( portal->value[1], GATE_NORMAL_EXIT ) )
	    act( "$n has arrived.", ch, portal, NULL, TO_ROOM );
	else
	    act( "$n has arrived through $p.", ch, portal, NULL, TO_ROOM );

	do_function( ch, &do_look, "auto" );

	/* charges */
	if (portal->value[0] > 0)
	{
	    portal->value[0]--;
	    if (portal->value[0] == 0)
		portal->value[0] = -1;
	}

	/* protect against circular follows */
	if ( old_room == location )
	    return;

    	for ( fch = old_room->people; fch != NULL; fch = fch_next )
    	{
            fch_next = fch->next_in_room;

            if ( !IS_VALID(  portal ) || portal->value[0] == -1 ) 
	    /* no following through dead portals */
                continue;

            if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM)
            &&   fch->position < POS_STANDING)
            	do_function(fch, &do_stand, "");

            if ( fch->master == ch && fch->position == POS_STANDING)
            {
                if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
                &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
                {
                    act("You can't bring $N into the city.",
                    	ch,NULL,fch,TO_CHAR);
                    act("You aren't allowed in the city.",
                    	fch,NULL,NULL,TO_CHAR);
                    continue;
            	}

            	act( "You follow $N.", fch, NULL, ch, TO_CHAR );
		do_function(fch, &do_enter, argument);
            }
    	}

 	if ( IS_VALID( portal ) && portal->value[0] == -1 )
	{
	    act("$p fades out of existence.",ch,portal,NULL,TO_CHAR);
	    if (ch->in_room == old_room)
		act("$p fades out of existence.",ch,portal,NULL,TO_ROOM);
	    else if (old_room->people != NULL)
	    {
		act("$p fades out of existence.", 
		    old_room->people,portal,NULL,TO_CHAR);
		act("$p fades out of existence.",
		    old_room->people,portal,NULL,TO_ROOM);
	    }
	    extract_obj(portal);
	}
	rprog_enter_trigger( ch->in_room, ch );
	check_nofloor( ch );
	return;
    }

    send_to_char("Nope, can't do it.\n\r",ch);
    return;
}


void
do_leave( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	to_room;
    ROOM_INDEX_DATA *	from_room;
    CHAR_DATA *		fch;
    CHAR_DATA *		fch_next;

    if ( ( to_room = ch->in_room->in_room ) == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    from_room = ch->in_room;
    act_color( AT_ACTION, "You leave $t.", ch, ROOMNAME( ch->in_room ),
	       NULL, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n leaves $t.", ch, ROOMNAME( ch->in_room ),
	       NULL, TO_ROOM, POS_RESTING );
    char_from_room( ch );
    char_to_room( ch, to_room );
    do_look( ch, "auto" );
    act_color( AT_ACTION, "$n arrives from $t.", ch, ROOMNAME( from_room ),
	       NULL, TO_ROOM, POS_RESTING );


    /* Protect against circular follows, even though this can't happen (yeah, right) */
    if ( from_room == to_room )
        return;

    for ( fch = from_room->people; fch != NULL; fch = fch_next )
    {
        fch_next = fch->next_in_room;
        if ( fch->master == ch && IS_AFFECTED( fch, AFF_CHARM )
        &&   fch->position < POS_STANDING )
        {
            do_stand( fch, "" );
        }

        if ( fch->master == ch && fch->position == POS_STANDING )
        {
            if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
            &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
            {
                act( "You can't bring $N into the city.",
                      ch, NULL, fch, TO_CHAR );
                act( "You aren't allowed in the city.",
                      fch, NULL, NULL, TO_CHAR );
                continue;
            }
            act_color( AT_ACTION, "You follow $N.", fch, NULL, ch, TO_CHAR, POS_RESTING );
            do_leave( fch, argument );
        }
    }
    rprog_enter_trigger( ch->in_room, ch );
    check_nofloor( ch );

    return;
}


bool
move_vehicle( CHAR_DATA *ch, ROOM_INDEX_DATA *veh, int dir )
{
    ROOM_INDEX_DATA *	aroom;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	to_room;
    CHAR_DATA *		hch;
    EXIT_DATA *		pExit;
    CHAR_DATA *		rch;
    CHAR_DATA *		rch_next;
    CLIST_DATA *	list;
    CLIST_DATA *	lchar;
    int			type;
    bool		fOkay;

    if ( veh == NULL || veh->vehicle_type == VEHICLE_NONE )
    {
	if ( ch != NULL )
	    send_to_char( "You can't do that.\n\r", ch );
	return FALSE;
    }

    in_room = veh->in_room;
    if ( in_room == NULL )
    {
	if ( ch != NULL )
	    send_to_char( "You can't do that.\n\r", ch );
	return FALSE;
    }

    if ( ( pExit = get_exit( in_room, dir ) ) == NULL
    ||	 ( to_room = pExit->to_room ) == NULL )
    {
	if ( ch != NULL )
	    send_to_char( "You can't steer in that direction.\n\r", ch );
	return FALSE;
    }

    for ( rch = veh->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( !can_see_room( rch, to_room ) )
	{
	    if ( ch != NULL )
		send_to_char( "You can't steer in that direction.\n\r", ch );
	    return FALSE;
	}
    }


    if ( room_is_private( to_room ) )
    {
	if ( ch != NULL )
	    send_to_char( "That room is private right now.\n\r", ch );
	return FALSE;
    }

    type = veh->vehicle_type;

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
	if ( ch != NULL )
	    send_to_char( "You can't steer there.\n\r", ch );
	return FALSE;
    }

    if ( ch != NULL )
    {
	act_color( AT_ACTION, "You steer $t $T.", ch, ROOMNAME( veh ), dir_name[dir], TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n steers $t $T.", ch, ROOMNAME( veh ), dir_name[dir], TO_ROOM, POS_RESTING );
    }
    else
    {
	act_color( AT_ACTION, "$t travels $T.", veh->people, ROOMNAME( veh ), dir_name[dir], TO_ALL, POS_RESTING );
    }

    if ( in_room->people != NULL )
	act_color( AT_ACTION, "$t leaves $T.", in_room->people, ROOMNAME( veh ), dir_name[dir], TO_ALL, POS_RESTING );

    if ( to_room->people != NULL )
	act_color( AT_ACTION, "$t arrives from $T.", to_room->people, ROOMNAME( veh ), dir_from[dir], TO_ALL, POS_RESTING );

    for ( aroom = to_room->next_in_room; aroom != NULL; aroom = aroom->next_in_room )
	act_color( AT_ACTION, "$t arrives from $T.", aroom->people,
		   ROOMNAME( veh ), dir_from[dir], TO_ALL, POS_RESTING );

    hch = veh->helmsman;
    list = NULL;

    for ( rch = veh->people; rch != NULL; rch = rch_next )
    {
	rch_next = rch->next_in_room;
	char_from_room( rch );
	char_to_room( rch, to_room );
	do_look( rch, "auto" );
	char_from_room( rch );
	lchar = new_clist_data( );
	lchar->lch = rch;
	lchar->next = list;
	list = lchar;
    }

    room_from_room( veh );
    room_to_room( veh, to_room );

    while ( ( lchar = list ) != NULL )
    {
	list = lchar->next;
	char_to_room( lchar->lch, veh );
	free_clist_data( lchar );
    }

    veh->helmsman = hch;
    return TRUE;
}


void
do_sail( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *	veh;
    EVENT_DATA *	pEvent;
    int			delay;
    int			dir;

    if ( ( veh = ch->in_room ) == NULL
    ||	 veh->in_room == NULL )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( veh->helmsman != NULL && veh->helmsman != ch )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( !str_cmp( arg, "stop" ) )
    {
	if ( ( pEvent = get_event_room( veh, EVENT_ROOM_SAIL ) ) == NULL )
	{
	    act_color( C_DEFAULT, "$t is not underway.", ch, veh->short_descr,
		       NULL, TO_CHAR, POS_RESTING );
	    return;
	}

	strip_event_room( veh, EVENT_ROOM_SAIL );
	act_color( AT_ACTION, "$t comes to a stop.", ch, veh->short_descr,
		   NULL, TO_ALL, POS_RESTING );
        veh->helmsman = ch;
	return;
    }

    if ( ( dir = dir_lookup( arg ) ) == DIR_NONE )
    {
	send_to_char( "You can't sail there.\n\r", ch );
	return;
    }

    if ( weather_info.wind_speed == WIND_CALM )
    {
	send_to_char( "There is no wind right now.\n\r", ch );
	return;
    }

    if ( weather_info.wind_dir == dir )
    {
	send_to_char( "You can't sail directly into the wind.\n\r", ch );
	return;
    }

    if ( ( pEvent = get_event_room( veh, EVENT_ROOM_SAIL ) ) != NULL )
    {
	pEvent->value0 = dir;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !move_vehicle( NULL, veh, dir ) )
    {
	send_to_char( "You can't sail there.\n\r", ch );
	return;
    }

    veh->helmsman = ch;

    strip_event_room( veh, EVENT_ROOM_SAIL );

    delay = get_sail_time( dir );

    pEvent = new_event( );
    pEvent->fun = event_room_sail;
    pEvent->type = EVENT_ROOM_SAIL;
    pEvent->value0 = dir;
    add_event_room( pEvent, veh, delay );

    return;
}


void
do_steer( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			dir;

    argument = one_argument( argument, arg );
    dir = dir_lookup( arg );

    if ( dir == DIR_NONE )
    {
	send_to_char( "Steer where?\n\r", ch );
	return;
    }

    if ( ch->in_room->helmsman != NULL && ch->in_room->helmsman != ch )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }


    if ( move_vehicle( ch, ch->in_room, dir ) )
	ch->in_room->helmsman = ch;

    return;
}


bool
event_room_sail( EVENT_DATA *pEvent )
{
    ROOM_INDEX_DATA *	veh;
    int			delay;
    int			dir;
    EVENT_DATA *	ev_new;

    veh = pEvent->owner.room;
    dir = pEvent->value0;

    if ( dir == weather_info.wind_dir || !move_vehicle( NULL, veh, dir )
    ||	 weather_info.wind_speed == WIND_CALM
    ||	 veh->in_room->sector_type == SECT_CITY )
    {
	act_color( AT_ACTION, "$t comes to a stop.", veh->people,
		   veh->short_descr, NULL, TO_ALL, POS_RESTING );
	return FALSE;
    }

    delay = get_sail_time( dir );

    ev_new = new_event( );
    ev_new->fun = event_room_sail;
    ev_new->type = EVENT_ROOM_SAIL;
    ev_new->value0 = dir;

    add_event_room( ev_new, veh, delay );
    return FALSE;
}


static int
get_sail_time( int dir )
{
    int time_val;
    int wind_dir;
    int wind_from;

    wind_from = weather_info.wind_dir;
    wind_dir = rev_dir[wind_from];

    if ( wind_dir == dir
    ||	 wind_dir == dir_next[wind_dir][0]
    ||	 wind_dir == dir_next[wind_dir][1] )
	time_val = 4 * PULSE_PER_SECOND;
    else if ( dir == dir_next[wind_from][0]
	 ||   dir == dir_next[wind_from][1] )
	time_val = 10 * PULSE_PER_SECOND;
    else
	time_val = 6 * PULSE_PER_SECOND;

    switch( weather_info.wind_speed )
    {
	case WIND_CALM:		time_val *= 4; break;
	case WIND_ZEPHYR:	time_val *= 2; break;
	case WIND_LIGHT:	time_val = 3 * time_val / 2; break;
	case WIND_MODERATE:	break;
	case WIND_STRONG:	time_val = 3 * time_val / 4; break;
	case WIND_GALE:		time_val /= 2; break;
	case WIND_HURRICANE:	break;
    }

    return time_val;
}


void
check_life_xtal( CHAR_DATA *ch )
{
    CHAR_DATA *	ghost;
    CHAR_DATA *	mob;
    CHAR_DATA *	vch;
    char	saybuf[MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) && ch->pIndexData->vnum != MOB_VNUM_LIFE )
        return;

    if ( !IS_NPC( ch ) && !IS_DEAD( ch ) )
        return;

    ghost = NULL;
    mob = NULL;

    if ( IS_NPC( ch ) )
    {
        for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
        {
            if ( !IS_NPC( vch ) && IS_DEAD( vch ) && vch->pcdata->deathmob == ch->id )
            {
                mob = ch;
                ghost = vch;
                break;
            }
        }
    }
    else
    {
        for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
        {
            if ( IS_NPC( vch ) && vch->id == ch->pcdata->deathmob )
            {
                mob = vch;
                ghost = ch;
                break;
            }
        }
    }

    if ( mob == NULL || ghost == NULL )
        return;

    SET_BIT( mob->act, ACT_SENTINEL );
    send_to_char( "\n\rYou have found the one who bears your life crystal!\n\r\n\r", ghost );
    ghost->pcdata->deathstate = DEATH_FOUND;

    sprintf( saybuf, "%s %s", ghost->name, "I bear your life crystal." );
    do_sayto( mob, saybuf );
    sprintf( saybuf, "%s %s", ghost->name, "You need only touch me, and I shall transfer it to you." );
    do_sayto( mob, saybuf );
}


bool
set_dreaming( CHAR_DATA *ch )
{
    DREAM_DATA *	pDream;
    EVENT_DATA *	pEvent;
    DREAMLIST_DATA *	item;
    DREAMLIST_DATA *	item_list;
    int			count;
    int			num;

    if ( get_event_char( ch, EVENT_CHAR_DREAM ) != NULL )
        return FALSE;	/* already dreaming */

    if ( !IS_NPC( ch ) && ch->pcdata->forcedream[0] != 0
         && ( pDream = get_dream_index( ch->pcdata->forcedream[0] ) ) != NULL )
    {
        pEvent = new_event( );
        pEvent->fun = event_dream;
        pEvent->type = EVENT_CHAR_DREAM;
        pEvent->value0 = ch->pcdata->forcedream[0];
        pEvent->value1 = 0;
        pEvent->value2 = PULSE_DREAM;
        add_event_char( pEvent, ch, pEvent->value2 );

        if ( !IS_NPC( ch ) )
        {
            ch->pcdata->lastdream[1] = ch->pcdata->lastdream[0];
            ch->pcdata->lastdream[0] = pEvent->value0;
        }

        ch->pcdata->forcedream[0] = ch->pcdata->forcedream[1];
        ch->pcdata->forcedream[1] = 0;
        return TRUE;
    }

    if ( number_bits( 2 ) != 0 )
    {
        send_to_char( "Normally you wouldn't start dreaming now, but for testing purposes you will.\n\r", ch );
    }

    count = 0;
    item_list = NULL;

    for ( pDream = dream_list; pDream != NULL; pDream = pDream->next )
    {
        if ( pDream->seg == NULL || IS_NULLSTR( pDream->seg->text ) )
            continue;
        if ( pDream->sex != SEX_EITHER && pDream->sex != ch->sex )
            continue;
        if ( !IS_NPC ( ch )
        &&   ( ch->pcdata->lastdream[0] == pDream->vnum
        ||     ch->pcdata->lastdream[1] == pDream->vnum ) )
            continue;
        if ( !xIS_EMPTY( pDream->class_flags ) && !xIS_SET( pDream->class_flags, ch->class ) )
            continue;
        if ( !xIS_EMPTY( pDream->race_flags ) && !xIS_SET( pDream->race_flags, ch->race ) )
            continue;

        count++;
        item = new_dreamlist( );
        item->dream = pDream;
        item->next = item_list;
        item_list = item;
    }

    if ( count == 0 )
        return FALSE;	/* No dreams available */

    num = number_range( 1, count );
    count = 1;
    for ( item = item_list; item != NULL; item = item->next )
    {
        if ( count == num )
            break;
        count++;
    }

    if ( item == NULL )
    {
        bugf( "Set_dreaming: dream #%d not found.", count );
        item = item_list;
        while ( item_list != NULL )
        {
            item = item_list;
            item_list = item->next;
            free_dreamlist( item );
        }
        return FALSE;
    }

    pEvent = new_event( );
    pEvent->fun = event_dream;
    pEvent->type = EVENT_CHAR_DREAM;
    pEvent->value0 = item->dream->vnum;
    pEvent->value1 = 0;
    pEvent->value2 = PULSE_DREAM;
    add_event_char( pEvent, ch, pEvent->value2 );

    if ( !IS_NPC( ch ) )
    {
        ch->pcdata->lastdream[1] = ch->pcdata->lastdream[0];
        ch->pcdata->lastdream[0] = pEvent->value0;
    }

    while ( item_list != NULL )
    {
        item = item_list;
        item_list = item->next;
        free_dreamlist( item );
    }

    return TRUE;
}


/* random room generation procedure */
ROOM_INDEX_DATA *
get_random_room( CHAR_DATA *ch )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    ROOM_INDEX_DATA **	roomlist;
    int			count;
    int			index;

    roomlist = (ROOM_INDEX_DATA **)calloc( top_room, sizeof( ROOM_INDEX_DATA * ) );
    if ( roomlist == NULL )
    {
        bugf( "Get_random_room: unable to allocate memory." );
        return NULL;
    }

    count = 0;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
        if ( IS_SET( pArea->area_flags, AREA_PROTOTYPE )
        ||   IS_SET( pArea->area_flags, AREA_MUDSCHOOL )
        ||   IS_SET( pArea->area_flags, AREA_CLAN )
        ||   IS_SET( pArea->area_flags, AREA_QUEST )
        ||   IS_SET( pArea->area_flags, AREA_ARENA ) )
            continue;

        for ( pRoom = pArea->room_list;
              pRoom != NULL;
              pRoom = pRoom->next_in_area )
        {
            if ( !can_see_room( ch, pRoom )
            ||	 room_is_private( pRoom )
            ||	 IS_SET( pRoom->room_flags, ROOM_PRIVATE )
            ||	 IS_SET( pRoom->room_flags, ROOM_SOLITARY )
            ||	 IS_SET( pRoom->room_flags, ROOM_SAFE )
            ||	 IS_SET( pRoom->room_flags, ROOM_GODS_ONLY )
            ||	 IS_SET( pRoom->room_flags, ROOM_HEROES_ONLY )
            ||	 IS_SET( pRoom->room_flags, ROOM_NEWBIES_ONLY )
            ||	 IS_SET( pRoom->room_flags, ROOM_NO_IN ) )
                continue;

            if ( IS_NPC( ch )
            &&	 IS_SET( ch->act, ACT_AGGRESSIVE )
            &&	 IS_SET( pRoom->room_flags, ROOM_LAW ) )
                continue;

            roomlist[count++] = pRoom;
        }
    }

    if ( count == 0 )
    {
        free( roomlist );
        return NULL;
    }
    else if ( count == 1 )
    {
        index = 0;
    }
    else
        index = number_range( 0, count );

    pRoom = roomlist[index];

    free( roomlist );

    return pRoom;
}

