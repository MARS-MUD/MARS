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


#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "lookup.h"
#include "magic.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

//extern	char *	target_name;

/*
 * Local functions.
 */
#define CD CHAR_DATA
#define OD OBJ_DATA
bool	remove_obj	args( (CHAR_DATA *ch, int iWear, bool fReplace ) );
CD *	find_keeper	args( (CHAR_DATA *ch ) );
int	get_cost	args( (CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
void 	obj_to_keeper	args( (OBJ_DATA *obj, CHAR_DATA *ch ) );
OD *	get_obj_keeper	args( (CHAR_DATA *ch,CHAR_DATA *keeper,char *argument));
#undef OD
#undef	CD
static bool	steal_mount( CHAR_DATA *ch, char *arg1, char *arg2 );


/* RT part of the corpse looting code */

bool
can_loot( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *	owner;
    CHAR_DATA *	wch;

    if ( IS_IMMORTAL( ch ) )
	return TRUE;

    if ( !obj->owner || obj->owner == NULL )
	return TRUE;

    owner = NULL;
    for ( wch = char_list; wch != NULL ; wch = wch->next )
	if ( !str_cmp( wch->name, obj->owner ) )
	    owner = wch;

    if ( owner == NULL )
	return TRUE;

    if ( !str_cmp( ch->name, owner->name ) )
	return TRUE;

    if ( !IS_NPC( owner ) && IS_SET( owner->act, PLR_CANLOOT ) )
	return TRUE;

    if ( is_same_group( ch, owner ) )
	return TRUE;

    return FALSE;
}


bool
can_use_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fMsg )
{
    char	buf[MAX_INPUT_LENGTH];

    if ( xIS_SET( obj->race_flags, ch->race ) )
    {
        if ( fMsg )
        {
            sprintf( buf, "%s can not use $p.",
                     aoran( race_table[ch->race].name ) );
            act( buf, ch, obj, NULL, TO_CHAR );
        }
        return FALSE;
    }

    if ( xIS_SET( obj->class_flags, ch->class ) )
    {
        if ( fMsg )
        {
            sprintf( buf, "%s can not use $p.",
                     aoran( class_table[ch->class].name ) );
            act( buf, ch, obj, NULL, TO_CHAR );
        }
        return FALSE;
    }

    return TRUE;
}


void
get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    /* variables for AUTOSPLIT */
    CHAR_DATA *gch;
    int members;
    char buffer[SHORT_STRING_LENGTH];

    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( obj->item_type == ITEM_QUESTITEM && !IS_IMMORTAL( ch )
	 && obj->value[0] != ch->id )
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	act( "$d: you can't carry that many items.",
	    ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if ((!obj->in_obj || obj->in_obj->carried_by != ch)
    &&  (get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch)))
    {
	act( "$P: you can't carry that much weight.",
	    ch, NULL, obj, TO_CHAR );
	return;
    }

    if (!can_loot(ch,obj))
    {
	act("Corpse looting is not permitted.",ch,NULL,NULL,TO_CHAR );
	return;
    }

    if (obj->in_room != NULL)
    {
	for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
	    if (gch->on == obj)
	    {
		act( "$N appears to be using $p.",
		     ch, obj, gch, TO_CHAR );
		return;
	    }
    }

    if ( container != NULL )
    {
    	if ( is_donation_box( container ) &&  get_trust( ch ) < obj->level )
	{
	    send_to_char( "You are not powerful enough to use it.\n\r", ch );
	    return;
	}

    	if ( is_donation_box( container )
	&&   !CAN_WEAR( container, ITEM_TAKE )
	&&   !IS_OBJ_STAT( obj, ITEM_HAD_TIMER ) )
	    obj->timer = 0;
	act_color( AT_ACTION, "You get $p from $P.", ch, obj, container, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n gets $p from $P.", ch, obj, container, TO_ROOM, POS_RESTING );
	REMOVE_BIT( obj->extra_flags, ITEM_HAD_TIMER );
	obj_from_obj( obj );
	if ( obj->pIndexData->progtypes & OBJ_PROG_GET_FROM )
	{
	    obj_to_room( obj, ch->in_room );
	    add_obj_fall_event( obj );
	    oprog_get_from_trigger( obj, ch, container );
	    obj_from_room( obj );
	}
    }
    else
    {
	act_color( AT_ACTION, "You get $p.", ch, obj, container, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n gets $p.", ch, obj, container, TO_ROOM, POS_RESTING );
	oprog_get_trigger( obj, ch );
	obj_from_room( obj );
    }

    if ( obj->item_type == ITEM_MONEY)
    {
	ch->money.gold	  += obj->value[0];
	ch->money.silver += obj->value[1];
	ch->money.copper	  += obj->value[2];
	ch->money.fract	  += obj->value[3];
        if ( IS_SET( ch->act, PLR_AUTOSPLIT ) )
	{ /* AUTOSPLIT code */
	    members = 0;
	    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	    {
		if ( !IS_AFFECTED( gch, AFF_CHARM )
		&&   is_same_group( gch, ch ) )
		    members++;
	    }

	    if ( members > 1 && ( obj->value[0] > 1 || obj->value[1]
			     ||   obj->value[2] > 1 || obj->value[3] > 1 ) )
	    {
		sprintf( buffer,"%do%ds%dk%dc",
				obj->value[0], obj->value[1],
				obj->value[2], obj->value[3] );
		do_function( ch, &do_split, buffer );
	    }
        }

	extract_obj( obj );
    }
    else
    {
	obj_to_char( obj, ch );
    }

    return;
}


const char *god_name( CHAR_DATA *ch )
{
    const char *godname;
    static const char * god_name_table [ ] =          /* Numbered from 0 !! */
    {
        "Lord Ao", "Akadi", "Asmodeus", "Bane",
        "Chauntea", "Cyric", "Grumbar", "Istishia",
		"Kelemvor", "Kossuth", "Lathander", "Lolth",
		"Mystra", "Oghma", "Selune", "Shar",
		"Silvanus", "Sune", "Talos", "Tempus",
		"Torm", "Tyr", "Ubtao", "Auril", "Bahamut",
		"Beshaba", "Gond", "Helm", "Ilmater",
		"Loviatar", "Mask", "Mielikki", "Tiamat",
		"Tymora", "Umberlee", "Waukeen", "Azuth",
		"Deneir", "Eldath", "Lliira", "Lurue", 
		"Malar", "Milil", "Shaundakul", "Talona",
		"Tchazzar",
    };

    godname = god_name_table[
      number_range( 1, sizeof(god_name_table)/sizeof(god_name_table[0]) ) - 1];

    /* Put a check for clan deity in here, once clans are settled. */
    return godname;
}


int
handcount( CHAR_DATA *ch )
{
    int	count;

    count = 0;
    if ( get_eq_char( ch, WEAR_LIGHT ) != NULL ) count++;
    if ( get_eq_char( ch, WEAR_SHIELD )!= NULL ) count++;
    if ( get_eq_char( ch, WEAR_HOLD )  != NULL ) count++;
    if ( get_eq_char( ch, WEAR_WIELD ) != NULL ) count++;
    if ( get_eq_char( ch, WEAR_DUAL )  != NULL ) count++;
    return count;
}


void
do_draw( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	quiver;
    OBJ_DATA *	arrow;

    if ( ( quiver = get_eq_char( ch, WEAR_SHOULDER ) ) == NULL )
    {
        send_to_char( "You aren't wearing a quiver where you can get to it.\n\r", ch );
        return;
    }

    if ( quiver->item_type != ITEM_QUIVER )
    {
        send_to_char( "You aren't wearing a quiver where you can get to it.\n\r", ch );
        return;
    }

    if ( handcount( ch ) > 1 )
    {
        send_to_char( "You need a free hand to draw an arrow.\n\r", ch );
        return;
    }

    if ( get_eq_char( ch, WEAR_HOLD ) != NULL )
    {
        send_to_char( "You are already holding something in your hand.\n\r", ch );
        return;
    }

    if ( quiver->value[0] > 0 )
    {
        WAIT_STATE( ch, PULSE_VIOLENCE );
        act_color( AT_ACTION, "You draw an arrow from $p.", ch, quiver, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n draws an arrow from $p.", ch, quiver, NULL, TO_ROOM, POS_RESTING );
	quiver->value[0]--;
	arrow = create_object( get_obj_index( OBJ_VNUM_ARROW ), 0 );
	arrow->value[1] = quiver->value[1];
	arrow->value[2] = quiver->value[2];
	arrow->level    = quiver->level;
	obj_to_char( arrow, ch );
	wear_obj( ch, arrow, TRUE );
	if ( quiver->value[0] == 0 )
	    act( "Your $p is now out of arrows, you need to find another one.", ch, quiver, NULL, TO_CHAR );
    }
    else
        act( "Your $p is now out of arrows, you need to find another one.", ch, quiver, NULL, TO_CHAR );
}


void
do_get( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    bool found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (!str_cmp(arg2,"from"))
	argument = one_argument(argument,arg2);

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Get what?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj' */
	    obj = get_obj_list( ch, arg1, ch->in_room->contents );
	    if ( obj == NULL )
	    {
		act( "I see no $T here.", ch, NULL, arg1, TO_CHAR );
		return;
	    }

	    get_obj( ch, obj, NULL );
	}
	else
	{
	    /* 'get all' or 'get all.obj' */
	    found = FALSE;
	    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, NULL );
		}
	    }

	    if ( !found )
	    {
		if ( arg1[3] == '\0' )
		    send_to_char( "I see nothing here.\n\r", ch );
		else
		    act( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR );
	    }
	}
    }
    else
    {
	/* 'get ... container' */
	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	    return;
	}

	switch ( container->item_type )
	{
	default:
	    send_to_char( "That's not a container.\n\r", ch );
	    return;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_SHEATH:
	    break;

	case ITEM_CORPSE_PC:
	    {

		if (!can_loot(ch,container))
		{
		    send_to_char( "You can't do that.\n\r", ch );
		    return;
		}
	    }
	}

	if ( IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return;
	}

	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj container' */
	    obj = get_obj_list( ch, arg1, container->contains );
	    if ( obj == NULL )
	    {
		act( "I see nothing like that in the $T.",
		    ch, NULL, arg2, TO_CHAR );
		return;
	    }
	    get_obj( ch, obj, container );
	}
	else
	{
	    /* 'get all container' or 'get all.obj container' */
	    found = FALSE;
	    for ( obj = container->contains; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    if ( is_donation_box( container ) &&  !IS_IMMORTAL( ch ) )
		    {
			send_to_char("Don't be so greedy!\n\r",ch);
			return;
		    }
		    get_obj( ch, obj, container );
		}
	    }

	    if ( !found )
	    {
		if ( arg1[3] == '\0' )
		    act( "I see nothing in the $T.",
			ch, NULL, arg2, TO_CHAR );
		else
		    act( "I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR );
	    }
	}
    }

    return;
}



void
do_put( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char	buf1[SHORT_STRING_LENGTH];
    char	buf2[SHORT_STRING_LENGTH];
    OBJ_DATA *	container;
    OBJ_DATA *	obj;
    OBJ_DATA *	obj_next;
    int		objCount;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (!str_cmp(arg2,"in") || !str_cmp(arg2,"on"))
	argument = one_argument(argument,arg2);

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Put what in what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
    {
	act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	return;
    }

    if ( container->item_type != ITEM_CONTAINER
    &&	 container->item_type != ITEM_SHEATH )
    {
	send_to_char( "That's not a container or sheath.\n\r", ch );
	return;
    }

    if ( IS_SET( container->value[1], CONT_CLOSED ) )
    {
	act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	/* 'put obj container' */
	if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

    	if ( WEIGHT_MULT( obj ) != 100 )
    	{
           send_to_char( "You have a feeling that would be a bad idea.\n\r", ch );
            return;
        }

	if ( container->item_type == ITEM_CONTAINER )
	{
	    if ( get_obj_weight( obj ) + get_true_weight( container ) - container->weight
	         > container->value[0]
            ||  ( container->value[3] > 0 && get_obj_weight( obj ) > container->value[3] )
            ||  ( container->value[5] > 0
                  && get_obj_count( container ) + get_obj_count( obj ) - 1
                  > container->value[5] )
             )
            {
	        send_to_char( "It won't fit.\n\r", ch );
	        return;
	    }

	    if ( is_donation_box( container )
	    &&	 !CAN_WEAR( container, ITEM_TAKE ) )
	    {
	        if ( obj->timer )
		    SET_BIT( obj->extra_flags,ITEM_HAD_TIMER );
                else
	            obj->timer = number_range( 660, 780 );
            }
	}

	if ( container->item_type == ITEM_SHEATH )
	{
	    if ( container->contains != NULL )
	    {
	        send_to_char( "It already contains something.\n\r", ch );
	        return;
	    }

	    if ( obj->item_type != ITEM_WEAPON )
	    {
		send_to_char( "That's not a weapon.\n\r", ch );
		return;
	    }

	    if ( obj->value[0] == WEAPON_EXOTIC )
	    {
		send_to_char( "You can't put that type of weapon in a sheath.\n\r", ch );
		return;
	    }

	    if ( obj->value[0] != container->value[3]
	    &&	 obj->value[0] != container->value[4]
	    &&	 obj->value[0] != container->value[5] )
	    {
		act( "You can't put $p in $P.", ch, obj, container, TO_CHAR );
		return;
	    }

	}

	obj_from_char( obj );
	obj_to_obj( obj, container );

	if ( !IS_NPC( ch ) || ch->pIndexData->vnum != MOB_VNUM_SUPERMOB )
	{
	    if (IS_SET(container->value[1],CONT_PUT_ON))
	    {
	        act_color( AT_ACTION, "$n puts $p`X on $P`X.", ch, obj, container, TO_ROOM, POS_RESTING );
	        act_color( AT_ACTION, "You put $p`X on $P`X.", ch, obj, container, TO_CHAR, POS_RESTING );
	    }
	    else
	    {
	        act_color( AT_ACTION, "$n puts $p`X in $P`X.", ch, obj, container, TO_ROOM, POS_RESTING );
	        act_color( AT_ACTION, "You put $p`X in $P`X.", ch, obj, container, TO_CHAR, POS_RESTING );
	    }
	}
	oprog_put_trigger( obj, ch, container );
    }
    else
    {
	/* 'put all container' or 'put all.obj container' */

	if ( container->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	}

	objCount = 0;

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   WEIGHT_MULT(obj) == 100
	    &&   obj->wear_loc == WEAR_NONE
	    &&   obj != container
	    &&   can_drop_obj( ch, obj )
	    &&   get_obj_weight( obj ) + get_true_weight( container )
		 <= ( container->value[0] )
	    &&   get_obj_weight( obj ) < ( container->value[3] ) )
	    {
	    	if ( is_donation_box( container )
	    	&&  !CAN_WEAR( obj, ITEM_TAKE ) )
	    	{
	    	    if ( obj->timer != 0 )
			SET_BIT( obj->extra_flags, ITEM_HAD_TIMER );
	    	    else
	    	    	obj->timer = number_range( 660, 780 );
		}
		obj_from_char( obj );
		obj_to_obj( obj, container );
		objCount++;

		if ( obj_next == NULL || obj->pIndexData->vnum != obj_next->pIndexData->vnum )
		{
		    if ( IS_SET( container->value[1], CONT_PUT_ON ) && objCount == 1 )
		    {
			strcpy( buf1, "You put $p`X on $P`X." );
 			strcpy( buf2, "$n puts $p`X on $P`X." );
		    }
		    else if ( !IS_SET( container->value[1], CONT_PUT_ON ) && objCount == 1 )
		    {
			strcpy( buf1, "You put $p`X in $P`X." );
			strcpy( buf2, "$n puts $p`X in $P`X." );
		    }
		    else if ( IS_SET( container->value[1], CONT_PUT_ON ) && objCount != 1 )
		    {
			sprintf( buf1, "You put $p`X on $P`X[%d].", objCount );
			sprintf( buf2, "$n puts $p`X on $P`X[%d].", objCount );
		    }
		    else if ( !IS_SET( container->value[1], CONT_PUT_ON ) && objCount != 1 )
		    {
			sprintf( buf1, "You put $p`X in $P`X[%d].", objCount );
			sprintf( buf2, "$n puts $p`X in $P`X[%d].", objCount );
		    }
                    if ( !IS_NPC( ch ) || ch->pIndexData->vnum != MOB_VNUM_SUPERMOB )
                    {
		        act_color( AT_ACTION, buf1, ch, obj, container, TO_CHAR, POS_RESTING );
		        act_color( AT_ACTION, buf2, ch, obj, container, TO_ROOM, POS_RESTING );
		    }
		    oprog_put_trigger( obj, ch, container );
		    objCount = 0;
		}
	    }
	}
    }

    return;
}


void
do_drop( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf1[SHORT_STRING_LENGTH];
    char	buf2[SHORT_STRING_LENGTH];
    MONEY	cash;
    OBJ_DATA *	obj;
    OBJ_DATA *	obj_next;
    OBJ_DATA *	newObj;
    int		objCount;
    bool	found;
    int		coins;

    argument = get_money_string( argument, arg );
    if ( arg[0] != '\0' )
    {
	/* 'drop NNNN coins' */

	if ( money_value( &cash, arg ) == NULL )
	{
	    bugf( "Do_drop: can't handle %s", argument );
	    send_to_char( "Sorry I can't do that.  Please inform the coding team.\n\r", ch );
	    return;
	}

	if ( cash.gold  == 0 && cash.silver == 0
	&&   cash.copper == 0 && cash.fract == 0 )

	{
	    send_to_char( "Why bother?\n\r", ch );
	    return;
	}

	if ( cash.gold  > ch->money.gold  || cash.silver > ch->money.silver
	||   cash.copper > ch->money.copper || cash.fract > ch->money.fract )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	coins = cash.gold + cash.silver + cash.copper + cash.fract;
	money_subtract( &ch->money, &cash, FALSE );

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    switch ( obj->pIndexData->vnum )
	    {
	    case OBJ_VNUM_GOLD_ONE:
	    case OBJ_VNUM_SILVER_ONE:
	    case OBJ_VNUM_COPPER_ONE:
	    case OBJ_VNUM_FRACT_ONE:
	    case OBJ_VNUM_GOLD_SOME:
	    case OBJ_VNUM_SILVER_SOME:
	    case OBJ_VNUM_COPPER_SOME:
	    case OBJ_VNUM_FRACT_SOME:
	    case OBJ_VNUM_COINS:
		cash.gold    += obj->value[0];
		cash.silver += obj->value[1];
		cash.copper   += obj->value[2];
		cash.fract    += obj->value[3];
		extract_obj(obj);
		break;
	    }
	}

	newObj = create_money( &cash );
	obj_to_room( newObj, ch->in_room );
	add_obj_fall_event( newObj );
	if ( coins == 1 )
	{
	    act_color( AT_ACTION, "You drop a coin.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n drops a coin.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
	}
	else
	{
	    act_color( AT_ACTION, "You drop some coins.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n drops some coins.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
	}
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Drop what?\n\r", ch );
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	/* 'drop obj' */
	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	add_obj_fall_event( obj );
	act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
	act( "You drop $p.", ch, obj, NULL, TO_CHAR );
	oprog_drop_trigger( obj, ch );
	if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
	{
	    act("$p dissolves into smoke.",ch,obj,NULL,TO_ROOM);
	    act("$p dissolves into smoke.",ch,obj,NULL,TO_CHAR);
	    extract_obj(obj);
	}
    }
    else
    {
	/* 'drop all' or 'drop all.obj' */
	found = FALSE;
	objCount = 0;
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   can_drop_obj( ch, obj ) )
	    {
		found = TRUE;
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		add_obj_fall_event( obj );
		objCount++;
		if ( obj_next == NULL
		||   obj->pIndexData->vnum != obj_next->pIndexData->vnum )
		{
		    if ( objCount == 1 )
		    {
			strcpy( buf1, "You drop $p." );
			strcpy( buf2, "$n drops $p." );
		    }
		    else
		    {
			sprintf( buf1, "You drop $p[%d].", objCount );
			sprintf( buf2, "$n drops $p[%d].", objCount );
		    }
		    act_color( AT_ACTION, buf2, ch, obj, NULL, TO_ROOM, POS_RESTING );
		    act_color( AT_ACTION, buf1, ch, obj, NULL, TO_CHAR, POS_RESTING );
		    oprog_drop_trigger( obj, ch );
		    objCount = 0;
        	}
		if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
        	{
		    if ( obj_next == NULL
		    ||	 obj->pIndexData->vnum != obj_next->pIndexData->vnum )
		    {
             		act( "$p dissolves into smoke.", ch, obj, NULL, TO_ROOM );
            		act( "$p dissolves into smoke.", ch, obj, NULL, TO_CHAR );
            	    }
            	    extract_obj(obj);
        	}
	    }
	}

	if ( !found )
	{
	    if ( arg[3] == '\0' )
		act( "You are not carrying anything.",
		    ch, NULL, arg, TO_CHAR );
	    else
		act( "You are not carrying any $T.",
		    ch, NULL, &arg[4], TO_CHAR );
	}
    }

    return;
}



void
do_give( CHAR_DATA *ch, char *argument )
{
    char	arg1 [MAX_INPUT_LENGTH];
    char	arg2 [MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    MONEY	amt;
    int		coincount;
    int		diff;
    CHAR_DATA *	victim;
    OBJ_DATA *	obj;

    if ( IS_NULLSTR( argument ) )
    {
	send_to_char( "Give what to whom?\n\r", ch );
	return;
    }

    argument = get_money_string( argument, arg1 );

    if ( arg1[0] != '\0' )
    {
	/* 'give NNNN coins victim' */

	if ( money_value( &amt, arg1 ) == NULL )
	{
	    bugf( "Do_give: can't handle %s", argument );
	    send_to_char( "Sorry I can't do that.  Please inform the coding team.\n\r", ch );
	    return;
	}

	argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Give how much to whom?\n\r", ch );
	    return;
	}

	if ( amt.gold  == 0 && amt.silver == 0
	&&   amt.copper == 0 && amt.fract == 0 )
	{
	    send_to_char( "Why bother?\n\r", ch );
	    return;
	}

	if ( amt.gold  > ch->money.gold  || amt.silver > ch->money.silver
	||   amt.copper > ch->money.copper || amt.fract > ch->money.fract )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( IS_DEAD( victim ) )
	{
	    act( "$E can't take it.", ch, NULL, victim, TO_CHAR );
	    return;
	}

	money_subtract( &ch->money, &amt, FALSE );
	money_add( &victim->money, &amt, FALSE );

	coincount = amt.gold + amt.silver + amt.copper + amt.fract;
	strcpy( buf, money_string( &amt, FALSE, FALSE ) );
	act_color( AT_ACTION, "You give$t to $N.", ch, buf, victim, TO_CHAR, POS_RESTING);
	act_color( AT_ACTION, "$n gives you$t.", ch, buf, victim, TO_VICT, POS_RESTING);
	act_color( AT_ACTION,  coincount == 1 ? "$n gives $N a coin."
					      : "$n gives $N some coins.",
		   ch, NULL, victim,
		   TO_NOTVICT, POS_RESTING );

        mprog_bribe_trigger( victim, ch, &amt );

	if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_IS_CHANGER ) && can_see( victim, ch ) )
	{
	    MONEY change;

	    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_IS_CHANGER ) )
	    {
		buildbug( "%s asking %s for change", ch->short_descr, victim->short_descr );
		return;
	    }

	    change.silver = 0;
	    change.copper = 0;
	    change.fract = amt.fract
			+ amt.copper * FRACT_PER_COPPER
			+ amt.silver * FRACT_PER_COPPER * COPPER_PER_SILVER;
	    if ( amt.gold < 10000 )
	    {
		change.gold = 0;
		change.fract += amt.gold * FRACT_PER_COPPER * COPPER_PER_SILVER
				* SILVER_PER_GOLD;
	    }
	    else
		change.gold = amt.gold;

	    change.gold = 95 * change.gold / 100;
	    change.fract = 95 * change.fract / 100;

	    if ( change.gold != 0 )
		diff = 8;
	    else if ( change.silver != 0 )
		diff = 8;
	    else if ( change.copper != 0 )
		diff = 8;
	    else
		diff = change.fract;

	    if ( change.fract < 8 )
	    {
		act_color( AT_TELL,
	"`W$n tells you '`YI'm sorry, you did not give me enough to change.`W'"
		    ,victim, NULL, ch, TO_VICT, POS_RESTING );
		ch->reply = victim;
		sprintf( buf, "%do%ds%dk%dc %s",
			 amt.gold, amt.silver, amt.copper, amt.fract, ch->name );
		do_function( victim, &do_give, buf );
		return;
	    }

	    normalize( &change );

	    money_add( &victim->money, &change, FALSE );
	    sprintf( buf, "%d%c%d%c%d%c%d%c %s",
		     change.gold,   GOLD_INITIAL,
		     change.silver, SILVER_INITIAL,
		     change.copper, COPPER_INITIAL,
		     change.fract,  FRACT_INITIAL,
		     ch->name );
	    do_function( victim, &do_give, buf );
	    money_subtract( &victim->money, &change, TRUE );

	    act_color( AT_TELL, "`W$n tells you '`YThank you, come again.`W'",
		    victim, NULL, ch, TO_VICT, POS_RESTING );
	    ch->reply = victim;
	    return;
	}
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Give what to whom?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "You must remove it first.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
	act_color( AT_TELL, "`W$N tells you '`YSorry, you'll have to sell that.`W'",
	    ch, NULL, victim, TO_CHAR, POS_RESTING );
	ch->reply = victim;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( IS_DEAD( victim ) )
    {
	act( "$E can't take it.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (get_carry_weight(victim) + get_obj_weight(obj) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, victim );
    act_color( AT_ACTION, "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT, POS_RESTING );
    act_color( AT_ACTION, "$n gives you $p.",   ch, obj, victim, TO_VICT, POS_RESTING    );
    act_color( AT_ACTION, "You give $p to $N.", ch, obj, victim, TO_CHAR, POS_RESTING    );
    oprog_give_trigger( obj, ch, victim );
    if ( obj->deleted || ch->deleted || victim->deleted )
      return;
    mprog_give_trigger( victim, ch, obj );
    return;
}


/* for poisoning weapons and food/drink */
void
do_envenom(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int percent,skill;

    /* find out what */
    if (argument[0] == '\0')
    {
	send_to_char("Envenom what item?\n\r",ch);
	return;
    }

    obj =  get_obj_list(ch,argument,ch->carrying);

    if (obj== NULL)
    {
	send_to_char("You don't have that item.\n\r",ch);
	return;
    }

    if ((skill = get_skill(ch,gsn_envenom)) < 1)
    {
	send_to_char("Are you crazy? You'd poison yourself!\n\r",ch);
	return;
    }

    if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
    {
	if (IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
	{
	    act("You fail to poison $p.",ch,obj,NULL,TO_CHAR);
	    return;
	}

	if (number_percent() < skill)  /* success! */
	{
	    act("$n treats $p with deadly poison.",ch,obj,NULL,TO_ROOM);
	    act("You treat $p with deadly poison.",ch,obj,NULL,TO_CHAR);
	    if (!obj->value[3])
	    {
		obj->value[3] = 1;
		check_improve(ch,gsn_envenom,TRUE,4);
	    }
	    WAIT_STATE(ch,skill_table[gsn_envenom].beats);
	    return;
	}

	act("You fail to poison $p.",ch,obj,NULL,TO_CHAR);
	if (!obj->value[3])
	    check_improve(ch,gsn_envenom,FALSE,4);
	WAIT_STATE(ch,skill_table[gsn_envenom].beats);
	return;
     }

    if (obj->item_type == ITEM_WEAPON)
    {
        if (IS_WEAPON_STAT(obj,WEAPON_FLAMING)
        ||  IS_WEAPON_STAT(obj,WEAPON_FROST)
        ||  IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC)
        ||  IS_WEAPON_STAT(obj,WEAPON_SHARP)
        ||  IS_WEAPON_STAT(obj,WEAPON_VORPAL)
        ||  IS_WEAPON_STAT(obj,WEAPON_SHOCKING)
        ||  IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
        {
            act("You can't seem to envenom $p.",ch,obj,NULL,TO_CHAR);
            return;
        }

	if (obj->value[3] < 0
	||  attack_table[obj->value[3]].damage == DAM_BASH)
	{
	    send_to_char("You can only envenom edged weapons.\n\r",ch);
	    return;
	}

        if (IS_WEAPON_STAT(obj,WEAPON_POISON))
        {
            act("$p is already envenomed.",ch,obj,NULL,TO_CHAR);
            return;
        }

	percent = number_percent();
	if (percent < skill)
	{

            af.where     = TO_WEAPON;
            af.type      = gsn_poison;
            af.level     = ch->level * percent / 100;
            af.duration  = ch->level/2 * percent / 100;
            af.location  = 0;
            af.modifier  = 0;
            af.bitvector = WEAPON_POISON;
            affect_to_obj(obj,&af);

            act("$n coats $p with deadly venom.",ch,obj,NULL,TO_ROOM);
	    act("You coat $p with venom.",ch,obj,NULL,TO_CHAR);
	    check_improve(ch,gsn_envenom,TRUE,3);
	    WAIT_STATE(ch,skill_table[gsn_envenom].beats);
            return;
        }
	else
	{
	    act("You fail to envenom $p.",ch,obj,NULL,TO_CHAR);
	    check_improve(ch,gsn_envenom,FALSE,3);
	    WAIT_STATE(ch,skill_table[gsn_envenom].beats);
	    return;
	}
    }

    act("You can't poison $p.",ch,obj,NULL,TO_CHAR);
    return;
}

void
do_fill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Fill what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "You can't fill that.\n\r", ch );
	return;
    }

    if ( ch->in_room->sector_type == SECT_RIVER || ch->in_room->sector_type == SECT_LAKE )
    {
        if ( obj->value[1] != 0 && obj->value[2] != LIQ_WATER )
        {
            act( "Your $p doesn't contain water.", ch, obj, NULL, TO_CHAR );
            return;
        }
	act_color( AT_ACTION, "You fill $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n fills $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
	if ( obj->value[0] > 0 )
	    obj->value[1] = obj->value[0];
        return;
    }

    found = FALSE;
    for ( fountain = ch->in_room->contents; fountain != NULL;
	fountain = fountain->next_content )
    {
	if ( fountain->item_type == ITEM_FOUNTAIN )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
    {
	send_to_char( "There is no fountain here!\n\r", ch );
	return;
    }

    if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] )
    {
	send_to_char( "There is already another liquid in it.\n\r", ch );
	return;
    }

    if ( obj->value[1] >= obj->value[0] )
    {
	send_to_char( "Your container is full.\n\r", ch );
	return;
    }

    if ( fountain->value[1] != 0 )
    {
        send_to_char( "The fountain is dry.\n\r", ch );
        return;
    }

    sprintf( buf, "You fill $p with %s from $P.",
	liq_table[fountain->value[2]].liq_name );
    act_color( AT_ACTION, buf, ch, obj,fountain, TO_CHAR, POS_RESTING );
    sprintf( buf, "$n fills $p with %s from $P.",
	liq_table[fountain->value[2]].liq_name );
    act_color( AT_ACTION, buf, ch,obj, fountain, TO_ROOM, POS_RESTING );
    obj->value[3] = fountain->value[3];
    obj->value[2] = fountain->value[2];
    obj->value[1] = obj->value[0];
    oprog_fill_trigger( obj, ch, fountain );
    oprog_use_trigger( fountain, ch, obj );
    return;
}


void
do_pour (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_STRING_LENGTH],buf[MAX_STRING_LENGTH];
    OBJ_DATA *out, *in;
    CHAR_DATA *vch = NULL;
    int amount;

    argument = one_argument(argument,arg);

    if (arg[0] == '\0' || argument[0] == '\0')
    {
	send_to_char("Pour what into what?\n\r",ch);
	return;
    }

    if ((out = get_obj_carry(ch,arg, ch)) == NULL)
    {
	send_to_char("You don't have that item.\n\r",ch);
	return;
    }

    if (out->item_type != ITEM_DRINK_CON)
    {
	send_to_char("That's not a drink container.\n\r",ch);
	return;
    }

    if (!str_cmp(argument,"out"))
    {
	if (out->value[1] == 0)
	{
	    send_to_char("It's already empty.\n\r",ch);
	    return;
	}

	out->value[1] = 0;
	out->value[3] = 0;
	sprintf(buf,"You invert $p, spilling %s all over the ground.",
		liq_table[out->value[2]].liq_name);
	act(buf,ch,out,NULL,TO_CHAR);

	sprintf(buf,"$n inverts $p, spilling %s all over the ground.",
		liq_table[out->value[2]].liq_name);
	act(buf,ch,out,NULL,TO_ROOM);
	return;
    }

    if ((in = get_obj_here(ch,argument)) == NULL)
    {
	vch = get_char_room(ch,argument);

	if (vch == NULL)
	{
	    send_to_char("Pour into what?\n\r",ch);
	    return;
	}

	in = get_eq_char(vch,WEAR_HOLD);

	if (in == NULL)
	{
	    send_to_char("They aren't holding anything.",ch);
 	    return;
	}
    }

    if (in->item_type != ITEM_DRINK_CON)
    {
	send_to_char("You can only pour into other drink containers.\n\r",ch);
	return;
    }

    if (in == out)
    {
	send_to_char("You cannot change the laws of physics!\n\r",ch);
	return;
    }

    if (in->value[1] != 0 && in->value[2] != out->value[2])
    {
	send_to_char("They don't hold the same liquid.\n\r",ch);
	return;
    }

    if (out->value[1] == 0)
    {
	act("There's nothing in $p to pour.",ch,out,NULL,TO_CHAR);
	return;
    }

    if (in->value[1] >= in->value[0])
    {
	act("$p is already filled to the top.",ch,in,NULL,TO_CHAR);
	return;
    }

    amount = UMIN(out->value[1],in->value[0] - in->value[1]);

    in->value[1] += amount;
    out->value[1] -= amount;
    in->value[2] = out->value[2];

    if (vch == NULL)
    {
    	sprintf(buf,"You pour %s from $p into $P.",
	    liq_table[out->value[2]].liq_name);
    	act(buf,ch,out,in,TO_CHAR);
    	sprintf(buf,"$n pours %s from $p into $P.",
	    liq_table[out->value[2]].liq_name);
    	act(buf,ch,out,in,TO_ROOM);
    }
    else
    {
        sprintf(buf,"You pour some %s for $N.",
            liq_table[out->value[2]].liq_name);
        act(buf,ch,NULL,vch,TO_CHAR);
	sprintf(buf,"$n pours you some %s.",
	    liq_table[out->value[2]].liq_name);
	act(buf,ch,NULL,vch,TO_VICT);
        sprintf(buf,"$n pours some %s for $N.",
            liq_table[out->value[2]].liq_name);
        act(buf,ch,NULL,vch,TO_NOTVICT);
    }
}


void
do_drink( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    OBJ_DATA *	obj;
    int		amount;
    int		liquid;

    one_argument( argument, arg );

    if ( ( arg[0] == '\0' && ( ch->in_room->sector_type == SECT_RIVER || ch->in_room->sector_type == SECT_LAKE ) )
    ||	 ( !str_cmp( arg, "lake" )  && ch->in_room->sector_type == SECT_LAKE )
    ||	 ( !str_cmp( arg, "river" ) && ch->in_room->sector_type == SECT_RIVER ) )
    {
        if ( arg[0] == '\0' )
            strcpy( arg, sector_data[ch->in_room->sector_type].name );
        act_color( AT_ACTION, "You drink from the $t.", ch, arg, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n drinks from the $t.", ch, arg, NULL, TO_ROOM, POS_RESTING );
        if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] >= 0 )
        {
            ch->pcdata->condition[COND_THIRST] = MAX_COND;
        }
        return;
    }

    if ( arg[0] == '\0' )
    {
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
	    if ( obj->item_type == ITEM_FOUNTAIN )
		break;
	}

	if ( obj == NULL )
	{
	    send_to_char( "Drink what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
	{
	    if ( !str_cmp( arg, "rain" ) )
	    {
		if ( !IS_OUTSIDE( ch ) )
		{
		    send_to_char( "You are indoors.\n\r", ch );
		    return;
		}
//		if (	ch->in_room->sector_type == SECT_UNDERGROUND
//		     || ch->in_room->sector_type == SECT_SPACE
//		     || ch->in_room->sector_type == SECT_DEEPSPACE )
//		{
//		    send_to_char( "It never rains here!\n\r", ch );
//		    return;
//		}
		if ( weather_info.sky != SKY_RAINING && weather_info.sky != SKY_LIGHTNING )
		{
		    send_to_char( "It's not raining.\n\r", ch );
		    return;
		}
		amount = number_range( 1, 3 );
		gain_condition( ch, COND_THIRST, amount );
		act_color( AT_LBLUE, "You catch a few raindrops on your tongue.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
		act_color( AT_LBLUE, "$n catches a few raindrops on $s tongue.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
		if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] > 40 )
		    send_to_char( "You do not feel thirsty.\n\r", ch );
		return;
	    }
	    else
	    {
		send_to_char( "You can't find it.\n\r", ch );
		return;
	    }
	}
    }

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10 )
    {
	send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
	return;
    }

    switch ( obj->item_type )
    {
    default:
	send_to_char( "You can't drink from that.\n\r", ch );
	return;

    case ITEM_FOUNTAIN:
        if ( !can_use_obj( ch, obj, TRUE ) )
            return;

        if ( obj->value[1] != 0 )
        {
            send_to_char( "The fountain is dry.\n\r", ch );
	    oprog_use_trigger( obj, ch, ch );
            return;
        }
        if ( ( liquid = obj->value[2] )  < 0 )
        {
            bug( "Do_drink: bad liquid number %d.", liquid );
            liquid = obj->value[2] = 0;
        }
	amount = liq_table[liquid].liq_affect[4] * 3;
	break;

    case ITEM_DRINK_CON:
        if ( !can_use_obj( ch, obj, TRUE ) )
            return;

	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}

	if ( ( liquid = obj->value[2] )  < 0 )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

        amount = liq_table[liquid].liq_affect[4];
        amount = UMIN(amount, obj->value[1]);
	break;
     }
    if ( !IS_NPC( ch ) && !IS_IMMORTAL( ch )
    &&	 liq_table[liquid].liq_affect[COND_FULL] > 0
    &&   ch->pcdata->condition[COND_FULL] > 45 )
    {
	send_to_char( "You're too full to drink more.\n\r", ch );
	return;
    }

    act_color( AT_ACTION, "$n drinks $T from $p.",
	ch, obj, liq_table[liquid].liq_name, TO_ROOM, POS_RESTING );
    act_color( AT_ACTION, "You drink $T from $p.",
	ch, obj, liq_table[liquid].liq_name, TO_CHAR, POS_RESTING );

    gain_condition( ch, COND_DRUNK,
	amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36 );
    gain_condition( ch, COND_FULL,
	amount * liq_table[liquid].liq_affect[COND_FULL] / 4 );
    gain_condition( ch, COND_THIRST,
	amount * liq_table[liquid].liq_affect[COND_THIRST] / 10 );
    gain_condition(ch, COND_HUNGER,
	amount * liq_table[liquid].liq_affect[COND_HUNGER] / 2 );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10 )
	send_to_char( "You feel drunk.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   > 40 )
	send_to_char( "You are full.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
	send_to_char( "Your thirst is quenched.\n\r", ch );

    if ( obj->value[3] != 0 )
    {
	/* The drink was poisoned ! */
	AFFECT_DATA af;

	act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You choke and gag.\n\r", ch );
	af.where     = TO_AFFECTS;
	af.type      = gsn_poison;
	af.level	 = number_fuzzy(amount);
	af.duration  = 3 * amount;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_POISON;
	affect_join( ch, &af );
    }

    if ( obj->value[0] > 0 )
    {
        obj->value[1] -= amount;
        ch->carry_weight -= amount * 10;
    }

    oprog_use_trigger( obj, ch, ch );

    return;
}


void
do_eat( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		amnt;
    OBJ_DATA *	obj;
    int value;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Eat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) )
    {
	if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL
   &&   obj->item_type != ITEM_HERB && obj->item_type != ITEM_BERRY
   &&   obj->item_type != ITEM_TOKEN )
	{
	    send_to_char( "That's not edible.\n\r", ch );
	    return;
	}

	if ( !can_use_obj( ch, obj, TRUE ) )
	    return;

	if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] > 40
	&&   obj->item_type != ITEM_PILL
	&&   obj->item_type != ITEM_HERB
	&&   obj->item_type != ITEM_BERRY )
	{
	    send_to_char( "You are too full to eat more.\n\r", ch );
	    return;
	}

	if ( get_eq_char( ch, WEAR_HOLD ) != NULL )
	{
	    send_to_char(  "You dont have a hand free!\n\r", ch );
	    return;
	}
    }

    act_color( AT_ACTION, "$n eats $p.",  ch, obj, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_ACTION, "You eat $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );

    switch ( obj->item_type )
    {
    case ITEM_BERRY:
	amnt = number_range( obj->value[0], obj->value[1] );
	ch->hit = UMIN( ch->hit + amnt, ch->max_hit );
	update_pos( ch );
	act_color( AT_MAGIC, "You feel warm all over.", ch, NULL, NULL, TO_CHAR, POS_DEAD );
	oprog_use_trigger( obj, ch, ch );
	break;

    case ITEM_FOOD:
	if ( !IS_NPC( ch ) )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_HUNGER];
	    gain_condition( ch, COND_FULL, obj->value[0] );
	    gain_condition( ch, COND_HUNGER, obj->value[1]);
	    if ( condition == 0 && ch->pcdata->condition[COND_HUNGER] > 0 )
		send_to_char( "You are no longer hungry.\n\r", ch );
	    else if ( ch->pcdata->condition[COND_FULL] > 40 )
		send_to_char( "You are full.\n\r", ch );
	}

	if ( obj->value[3] != 0 )
	{
	    /* The food was poisoned! */
	    AFFECT_DATA af;

	    act_color( AT_ACTION, "$n chokes and gags.", ch, 0, 0, TO_ROOM, POS_RESTING );
	    act_color( AT_ACTION, "You choke and gag.\n\r", ch, NULL, NULL, TO_CHAR, POS_DEAD );

	    af.where	 = TO_AFFECTS;
	    af.type      = gsn_poison;
	    af.level 	 = number_fuzzy(obj->value[0]);
	    af.duration  = 2 * obj->value[0];
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	oprog_use_trigger( obj, ch, ch );
	break;

    case ITEM_HERB:
	if ( IS_SET( obj->value[5], POISONED_EATEN ) )
	{
	    /* The food was poisoned! */
	    AFFECT_DATA af;

	    act_color( AT_ACTION, "$n chokes and gags.", ch, 0, 0, TO_ROOM, POS_RESTING );
	    act_color( AT_ACTION, "You choke and gag.\n\r", ch, NULL, NULL, TO_CHAR, POS_DEAD );

	    af.where	 = TO_AFFECTS;
	    af.type      = gsn_poison;
	    af.level 	 = number_fuzzy(obj->value[0]);
	    af.duration  = 2 * obj->value[0];
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	else
	    obj_cast_spell( obj->value[1], obj->value[2], ch, ch, NULL );

	oprog_use_trigger( obj, ch, ch );
	break;

    case ITEM_PILL:
	obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
	obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
	obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
	oprog_use_trigger( obj, ch, ch );
	break;

	 case ITEM_TOKEN:
	 if ( !IS_NPC(ch))
	 {
        value = obj->value[0];
        ch->questpoints += value;
        break;
    }
    }

   

    extract_obj( obj );
    return;
}


/*
 * Remove an object.
 */
bool
remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	return TRUE;

    if ( !fReplace )
	return FALSE;

    if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
    {
	act( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }
    if ( (obj->item_type == ITEM_WEAPON) && IS_OBJ_STAT(obj,ITEM_DEATH) )
    {
        act( "The black mist around $p fades away.",ch,obj,NULL,TO_ALL);
        REMOVE_BIT(obj->extra_flags,ITEM_DEATH);
    }

    unequip_char( ch, obj );

    oprog_remove_trigger( obj, ch );
    if ( obj->wear_loc != WEAR_NONE )
    {
        buildbug( "Object #%d removed but still equipped on %s",
                  obj->pIndexData->vnum,
                  IS_NPC( ch ) ? ch->short_descr : ch->name );
	unequip_char( ch, obj );
    }

    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void
wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    char buf[MAX_STRING_LENGTH];
/*
    if ( ch->level < obj->level )
    {
	sprintf( buf, "You must be level %d to use this object.\n\r",
	    obj->level );
	send_to_char( buf, ch );
	act( "$n tries to use $p, but is too inexperienced.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }
*/
    if ( !can_use_obj( ch, obj, TRUE ) )
        return;

    if ( obj->item_type == ITEM_LIGHT )
    {
	if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
	    return;
	act( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
	act( "You light $p and hold it.",  ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_LIGHT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
    {
	if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
	&&   get_eq_char( ch, WEAR_FINGER_R ) != NULL
	&&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
	&&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
	{
	    act( "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM );
	    act( "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_FINGER_L );
	    return;
	}

	if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
	{
	    act( "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM );
	    act( "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_FINGER_R );
	    return;
	}

	bug( "Wear_obj: no free finger.", 0 );
	send_to_char( "You already wear two rings.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_EAR ) )
    {
	if ( get_eq_char( ch, WEAR_EAR_L ) != NULL
	&&   get_eq_char( ch, WEAR_EAR_R ) != NULL
	&&   !remove_obj( ch, WEAR_EAR_L, fReplace )
	&&   !remove_obj( ch, WEAR_EAR_R, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_EAR_L ) == NULL )
	{
	    act( "$n wears $p on $s left ear.",
		ch, obj, NULL, TO_ROOM );
	    act( "You wear $p on your left ear.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_EAR_L );
	    return;
	}

	if ( get_eq_char( ch, WEAR_EAR_R ) == NULL )
	{
	    act( "$n wears $p on $s right ear.",
		ch, obj, NULL, TO_ROOM );
	    act( "You wear $p on your right ear.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_EAR_R );
	    return;
	}

	bug( "Wear_obj: no free ear.", 0 );
	send_to_char( "You already wear two earrings.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
    {
	if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
	&&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
	&&   !remove_obj( ch, WEAR_NECK_1, fReplace )
	&&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
	{
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_NECK_1 );
	    return;
	}

	if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
	{
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_NECK_2 );
	    return;
	}

	bug( "Wear_obj: no free neck.", 0 );
	send_to_char( "You already wear two neck items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {
	if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
	    return;
	act( "$n wears $p on $s torso.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your torso.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_BODY );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {
	if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
	    return;
	act( "$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your head.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HEAD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_CONTACT ) )
    {
	if ( !remove_obj( ch, WEAR_EYES, fReplace ) )
	    return;
	act( "You place $p over your eyes.", ch, obj, NULL, TO_CHAR );
	act( "$n places $p over $s eyes.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_EYES );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FACE ) )
    {
	if ( !remove_obj( ch, WEAR_FACE, fReplace ) )
	    return;
	act( "$n wears $p on $s face.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your face.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_FACE );
	return;
    }

  /*
   * Make tail a conditional wear location
   * IS_SET(ch-part,Part name)
   */
  if (CAN_WEAR (obj, ITEM_WEAR_TAIL) && IS_SET (ch->parts, PART_TAIL))
    {
      if (!remove_obj (ch, WEAR_TAIL, fReplace))
        return;
      act ("$n wears $p on $s tail.", ch, obj, NULL, TO_ROOM);
      act ("You wear $p on your tail.", ch, obj, NULL, TO_CHAR);
      equip_char (ch, obj, WEAR_TAIL);
      return;
    }

  /*
   * Make horns a conditional wear location
   * IS_SET(ch-part,Part name)
   */
  if (CAN_WEAR (obj, ITEM_WEAR_HORNS) && IS_SET (ch->parts, PART_HORNS))
    {
      if (!remove_obj (ch, WEAR_HORNS, fReplace))
        return;
      act ("$n wears $p on $s horns.", ch, obj, NULL, TO_ROOM);
      act ("You wear $p on your horns.", ch, obj, NULL, TO_CHAR);
      equip_char (ch, obj, WEAR_HORNS);
      return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {
	if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
	    return;
	act( "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HANDS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {
	if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
	    return;
	act( "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_ARMS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
	if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
	    return;
	act( "$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p about your body.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_ABOUT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHOULDER ) )
	{
	if ( !remove_obj( ch, WEAR_SHOULDER, fReplace ) )
	    return;
	act( "$n wears $p over $s shoulder.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p over your shoulder.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_SHOULDER );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_RPATCH ) )
    {
	if ( !remove_obj( ch, WEAR_RPATCH, fReplace ) )
	    return;
	act( "$n wears $p as a symbol of $s religion.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p as a symbol of your religion.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_RPATCH );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_CPATCH ) )
    {
	if ( !remove_obj( ch, WEAR_CPATCH, fReplace ) )
	    return;
	act( "$n wears $p as $s clan blazon.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p as your clan blazon.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_CPATCH );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
	if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	    return;
	act( "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_WAIST );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
	if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
	&&   get_eq_char( ch, WEAR_WRIST_R ) != NULL
	&&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
	&&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
	{
	    act( "$n wears $p around $s left wrist.",
		ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your left wrist.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WRIST_L );
	    return;
	}

	if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
	{
	    act( "$n wears $p around $s right wrist.",
		ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your right wrist.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WRIST_R );
	    return;
	}

	bug( "Wear_obj: no free wrist.", 0 );
	send_to_char( "You already wear two wrist items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
	OBJ_DATA *weapon;

	if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	    return;

	weapon = get_eq_char(ch,WEAR_WIELD);
	if (weapon != NULL && ch->size < SIZE_LARGE
	&&  IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS))
	{
	    send_to_char("Your hands are tied up with your weapon!\n\r",ch);
	    return;
	}
	if ( get_eq_char( ch, WEAR_DUAL ) != NULL )
	{
	    send_to_char( "You cannot use a shield while using two weapons.\n\r", ch );
	    return;
	}

	act( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
	act( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_SHIELD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ANKLE ) )
    {
	if ( get_eq_char( ch, WEAR_ANKLE_L ) != NULL
	&&   get_eq_char( ch, WEAR_ANKLE_R ) != NULL
	&&   !remove_obj( ch, WEAR_ANKLE_L, fReplace )
	&&   !remove_obj( ch, WEAR_ANKLE_R, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_ANKLE_L ) == NULL )
	{
	    act( "$n wears $p around $s left ankle.",
		ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your left ankle.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_ANKLE_L );
	    return;
	}

	if ( get_eq_char( ch, WEAR_ANKLE_R ) == NULL )
	{
	    act( "$n wears $p around $s right ankle.",
		ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your right ankle.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_ANKLE_R );
	    return;
	}

	bug( "Wear_obj: no free ankle.", 0 );
	send_to_char( "You already wear two ankle items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
    {
	if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
	    return;
	act( "$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your legs.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_LEGS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
    {
	if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	    return;
	act( "$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_FEET );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
	int sn,skill;

	if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
	    return;

	if ( !IS_NPC(ch)
	&& get_obj_weight(obj) > (str_app[get_curr_stat(ch,STAT_STR)].wield
		* 10))
	{
	    send_to_char( "It is too heavy for you to wield.\n\r", ch );
	    return;
	}

	if (!IS_NPC(ch) && ch->size < SIZE_LARGE
	&&  IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
 	&&  get_eq_char(ch,WEAR_SHIELD) != NULL)
	{
	    send_to_char("You need two hands free for that weapon.\n\r",ch);
	    return;
	}

	act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
	act( "You wield $p.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_WIELD );

        sn = get_weapon_sn(ch);

	if (sn == gsn_hand_to_hand)
	   return;

        skill = get_weapon_skill(ch,sn);

        if (skill >= 100)
            act("$p feels like a part of you!",ch,obj,NULL,TO_CHAR);
        else if (skill > 85)
            act("You feel quite confident with $p.",ch,obj,NULL,TO_CHAR);
        else if (skill > 70)
            act("You are skilled with $p.",ch,obj,NULL,TO_CHAR);
        else if (skill > 50)
            act("Your skill with $p is adequate.",ch,obj,NULL,TO_CHAR);
        else if (skill > 25)
            act("$p feels a little clumsy in your hands.",ch,obj,NULL,TO_CHAR);
        else if (skill > 1)
            act("You fumble and almost drop $p.",ch,obj,NULL,TO_CHAR);
        else
            act("You don't even know which end is up on $p.",
                ch,obj,NULL,TO_CHAR);

	return;
    }

    if ( CAN_WEAR( obj, ITEM_HOLD ) )
    {
	if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_DUAL ) != NULL )
	{
	    send_to_char( "You cannot hold an item while using two weapons.\n\r", ch );
	    return;
	}

	act( "$n holds $p in $s hand.",   ch, obj, NULL, TO_ROOM );
	act( "You hold $p in your hand.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HOLD );
	return;
    }

    if ( CAN_WEAR(obj,ITEM_WEAR_FLOAT) )
    {
	if (!remove_obj(ch,WEAR_FLOAT, fReplace) )
	    return;
	act("$n releases $p to float next to $m.",ch,obj,NULL,TO_ROOM);
	act("You release $p and it floats next to you.",ch,obj,NULL,TO_CHAR);
	equip_char(ch,obj,WEAR_FLOAT);
	return;
    }

    if ( fReplace )
	send_to_char( "You can't wear, wield, or hold that.\n\r", ch );

    return;
}


void
do_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Wear, wield, or hold what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	OBJ_DATA *obj_next;

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
		wear_obj( ch, obj, FALSE );
	}
	return;
    }
    else
    {
	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	wear_obj( ch, obj, TRUE );
    }

    return;
}


void
do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	found = FALSE;
	for ( obj = ch->carrying; obj; obj = obj2 )
	{
	    obj2 = obj->next_content;
	    if ( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
	    {
		if ( remove_obj( ch, obj->wear_loc, TRUE ) )
		    found = TRUE;
	    }
	}
	if ( !found )
	    send_to_char( "You aren't wearing anything.\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc == WEAR_WIELD && ( obj2 = get_eq_char( ch, WEAR_DUAL ) ) )
    {
	remove_obj( ch, WEAR_DUAL, TRUE );
    }

    remove_obj( ch, obj->wear_loc, TRUE );
    return;
}


/* wear object as a secondary weapon */
void
do_dual( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    if ( *argument == '\0' )/* empty */
    {
	send_to_char( "Wear which weapon in your off-hand?\n\r" , ch );
	return;
    }

    obj = get_obj_carry( ch, argument, ch );

    if ( obj == NULL )
    {
	send_to_char( "You have no such thing in your inventory.\n\r" ,ch );
	return;
    }
    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char( "You can only dual wield weapons.\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch )
    &&	 ( !can_use_skpell( ch, gsn_dual ) || ch->pcdata->skill[gsn_dual].percent < 1 ) )
    {
	send_to_char( "You cannot.\n\r", ch );
	return;
    }

    /* check if the char is using a shield or a held weapon */

    if ( ( get_eq_char( ch, WEAR_SHIELD ) != NULL )
    ||	 ( get_eq_char( ch, WEAR_HOLD ) != NULL ) )
    {
	send_to_char( "You cannot dual wield while using a shield or holding an item.\n\r" , ch );
	return;
    }
/*
    if ( ch->level < obj->level )
    {
	ch_printf( ch, "You must be level %d to use this object.\n\r", obj->level );
	act( "$n tries to use $p, but is too inexperienced.",
	     ch, obj, NULL, TO_ROOM );
	return;
    }
*/
    /* check that the character is using a first weapon at all */
    if ( get_eq_char( ch, WEAR_WIELD ) == NULL )
    {
	send_to_char( "You need to wield a primary weapon, before using a second one!\n\r" , ch );
	return;
    }

    /* check for str - secondary weapons have to be lighter */
    if ( get_obj_weight( obj ) > ( str_app[get_curr_stat( ch, STAT_STR )].wield / 2 ) )
    {
	send_to_char("this weapon is too heavy to be used as a secondary weapon by you.\n\r", ch );
	return;
    }

    /* check if the secondary weapon is at least half as light as the primary weapon */
  /*
    if ( ( get_obj_weight( obj ) * 2 ) > get_obj_weight( get_eq_char( ch, WEAR_WIELD ) ) )
    {
	send_to_char( "Your secondary weapon has to be considerable lighter than the primary one.\n\r",ch );
	return;
    }
*/
    WAIT_STATE( ch, skill_table[gsn_dual].beats );

    /* at last- the char uses the weapon */
    if ( !remove_obj( ch, WEAR_DUAL, TRUE ) )
	return;

    if ( !IS_NPC( ch )
    &&	 ch->position == POS_FIGHTING
    &&	 get_skill( ch, gsn_dual ) < number_percent ( ) )
    {
	send_to_char( "You fail.\n\r", ch );
	check_improve( ch, gsn_dual, FALSE, 1 );
	return;
    }

    act( "$n wields $p in $s off-hand.", ch, obj, NULL, TO_ROOM );
    act( "You wield $p in your off-hand.",ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_DUAL );
    return;
}


void
do_sacrifice( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    OBJ_DATA *	obj;
    OBJ_DATA *	obj_next;
    CHAR_DATA *	vch;
    const char *godname;
    int		amount;
    bool	found;

    /* variables for AUTOSPLIT */
    CHAR_DATA *	gch;
    int		members;
    char	buffer[100];


    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	act( "$n offers $mself to " IMP_NAME ", who graciously declines.",
	    ch, NULL, NULL, TO_ROOM );
	send_to_char(
	    IMP_NAME " appreciates your offer and may accept it later.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	found = FALSE;
	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( can_see_obj( ch, obj ) && CAN_WEAR( obj, ITEM_TAKE )
		 && !CAN_WEAR( obj, ITEM_NO_SAC ) && obj->item_type != ITEM_QUESTITEM )
	    {
		found = TRUE;
		do_sacrifice( ch, obj->name );
	    }
	}
	if ( !found )
	{
	    send_to_char( "I see nothing here to sacrifice.\n\r", ch );
	}
	return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
	vch = get_char_room( ch, arg );
	if ( vch == ch )
	{
	    act( "$n offers $mself to " IMP_NAME ", who graciously declines.",
		ch, NULL, NULL, TO_ROOM );
	    send_to_char(
		IMP_NAME " appreciates your offer and may accept it later.\n\r", ch );
	}
	else if ( vch )
	    act( "Kill $M first!", ch, NULL, vch, TO_CHAR );
	else
	    send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    if ( obj->item_type == ITEM_CORPSE_PC )
    {
	if ( obj->contains )
	{
	   send_to_char( IMP_NAME " wouldn't like that.\n\r", ch );
	   return;
	}
    }


    if ( !CAN_WEAR( obj, ITEM_TAKE ) || CAN_WEAR( obj, ITEM_NO_SAC )
	 || obj->item_type == ITEM_QUESTITEM )
    {
	act( "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR );
	return;
    }

    if ( obj->in_room != NULL )
    {
	for ( gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room )
	    if ( gch->on == obj )
	    {
		act( "$N appears to be using $p.",
		    ch, obj, gch, TO_CHAR );
		return;
	    }
    }

    godname = god_name( ch );

    act_color( AT_ACTION, "You sacrifice $p to $T.", ch, obj, godname, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n sacrifices $p to $T.", ch, obj, godname, TO_ROOM, POS_RESTING );
    wiznet( "$N sends up $p as a burnt offering.",
	    ch, obj, WIZ_SACCING, 0, 0 );

    amount = UMAX( 1, obj->level * 3 );

    if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC )
    	amount = UMIN( amount, obj->cost );

    if ( amount == 1 )
        ch_printf( ch,
	    "%s gives you one %s for your sacrifice.\n\r",
	    godname, FRACT_NOUN );
    else
    {
	ch_printf( ch, "%s gives you %d %s for your sacrifice.\n\r",
		godname, amount, FRACT_PLURAL );
    }

    ch->money.fract += amount;

    if ( IS_SET( ch->act,PLR_AUTOSPLIT ) )
    { /* AUTOSPLIT code */
    	members = 0;
	for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    	{
    	    if ( is_same_group( gch, ch ) )
            members++;
    	}

	if ( members > 1 && amount > 1)
	{
	    sprintf( buffer, "%d %s", amount, FRACT_PLURAL );
	    do_function( ch, &do_split, buffer );
	}
    }

    extract_obj( obj );
    return;
}


void
do_quaff( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Quaff what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	send_to_char( "You do not have that potion.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_POTION )
    {
	send_to_char( "You can quaff only potions.\n\r", ch );
	return;
    }

    if (ch->level < obj->level)
    {
	send_to_char("This liquid is too powerful for you to drink.\n\r",ch);
	return;
    }

    if ( get_eq_char( ch, WEAR_HOLD ) != NULL )
    {
	send_to_char( "You don't have a free hand!", ch );
	return;
    }

    act( "$n quaffs $p.", ch, obj, NULL, TO_ROOM );
    act( "You quaff $p.", ch, obj, NULL ,TO_CHAR );

    obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
    obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
    obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );

    oprog_use_trigger( obj, ch, ch );

    extract_obj( obj );
    return;
}


void
do_recite( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( scroll = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You do not have that scroll.\n\r", ch );
	return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
	send_to_char( "You can recite only scrolls.\n\r", ch );
	return;
    }

    if ( ch->level < scroll->level)
    {
	send_to_char(
		"This scroll is too complex for you to comprehend.\n\r",ch);
	return;
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
	victim = ch;
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    act( "$n recites $p.", ch, scroll, NULL, TO_ROOM );
    act( "You recite $p.", ch, scroll, NULL, TO_CHAR );

    if (number_percent() >= 20 + get_skill(ch,gsn_scrolls) * 4/5)
    {
	send_to_char("You mispronounce a syllable.\n\r",ch);
	check_improve(ch,gsn_scrolls,FALSE,2);
    }

    else
    {
    	obj_cast_spell( scroll->value[1], scroll->value[0], ch, victim, obj );
    	obj_cast_spell( scroll->value[2], scroll->value[0], ch, victim, obj );
    	obj_cast_spell( scroll->value[3], scroll->value[0], ch, victim, obj );
	if ( victim )
	    oprog_use_trigger( scroll, ch, victim );
	else
	    oprog_use_trigger( scroll, ch, obj );
	check_improve(ch,gsn_scrolls,TRUE,2);
    }

    extract_obj( scroll );
    return;
}


void
do_brandish( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *staff;
    int sn;

    if ( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( staff->item_type != ITEM_STAFF )
    {
	send_to_char( "You can brandish only with a staff.\n\r", ch );
	return;
    }

    if ( ( sn = staff->value[3] ) < 0
    ||   sn >= top_skill
    ||   skill_table[sn].spell_fun == 0 )
    {
	bug( "Do_brandish: bad sn %d.", sn );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[2] > 0 )
    {
	act( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
	act( "You brandish $p.",  ch, staff, NULL, TO_CHAR );
	if ( ch->level < staff->level
	||   number_percent() >= 20 + get_skill(ch,gsn_staves) * 4/5)
 	{
	    act ("You fail to invoke $p.",ch,staff,NULL,TO_CHAR);
	    act ("...and nothing happens.",ch,NULL,NULL,TO_ROOM);
	    check_improve(ch,gsn_staves,FALSE,2);
	}
	else
	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next	= vch->next_in_room;

	    switch ( skill_table[sn].target )
	    {
	    default:
		bug( "Do_brandish: bad target for sn %d.", sn );
		return;

	    case TAR_IGNORE:
		if ( vch != ch )
		    continue;
		break;

	    case TAR_CHAR_OFFENSIVE:
		if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) )
		    continue;
		break;

	    case TAR_CHAR_DEFENSIVE:
		if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
		    continue;
		break;

	    case TAR_CHAR_SELF:
		if ( vch != ch )
		    continue;
		break;
	    }

	    obj_cast_spell( staff->value[3], staff->value[0], ch, vch, NULL );
	    oprog_use_trigger( staff, ch, vch );
	    check_improve(ch,gsn_staves,TRUE,2);
	}
    }

    if ( --staff->value[2] <= 0 )
    {
	act( "$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
	act( "Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
	extract_obj( staff );
    }

    return;
}



void
do_zap( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wand;
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
	send_to_char( "Zap whom or what?\n\r", ch );
	return;
    }

    if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( wand->item_type != ITEM_WAND )
    {
	send_to_char( "You can zap only with a wand.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting != NULL )
	{
	    victim = ch->fighting;
	}
	else
	{
	    send_to_char( "Zap whom or what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( wand->value[2] > 0 )
    {
	if ( victim != NULL )
	{
	    act( "$n zaps $N with $p.", ch, wand, victim, TO_NOTVICT );
	    act( "You zap $N with $p.", ch, wand, victim, TO_CHAR );
	    act( "$n zaps you with $p.",ch, wand, victim, TO_VICT );
	}
	else
	{
	    act( "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
	    act( "You zap $P with $p.", ch, wand, obj, TO_CHAR );
	}

 	if (ch->level < wand->level
	||  number_percent() >= 20 + get_skill(ch,gsn_wands) * 4/5)
	{
	    act( "Your efforts with $p produce only smoke and sparks.",
		 ch,wand,NULL,TO_CHAR);
	    act( "$n's efforts with $p produce only smoke and sparks.",
		 ch,wand,NULL,TO_ROOM);
	    check_improve(ch,gsn_wands,FALSE,2);
	}
	else
	{
	    obj_cast_spell( wand->value[3], wand->value[0], ch, victim, obj );
	    if ( victim )
		oprog_use_trigger( wand, ch, victim );
	    else
		oprog_use_trigger( wand, ch, obj );
	    check_improve(ch,gsn_wands,TRUE,2);
	}
    }

    if ( --wand->value[2] <= 0 )
    {
	act( "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
	act( "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
	extract_obj( wand );
    }

    return;
}


void
do_rub( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA	af;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char *	argline;
    OBJ_DATA *	herb;
    CHAR_DATA *	victim;

    argline = argument;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( herb = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	if ( !check_social( ch, "rub", argline ) )
	{
	    send_to_char( "You aren't carrying that.\n\r", ch );
	}
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "No way!  You are fighting!\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
	victim = ch;
    else
    {
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "They're not here.\n\r", ch );
	    return;
	}
    }
/*
    if ( herb->level > ch->level )
    {
	send_to_char( "You are too inexperienced to do that.\n\r", ch );
	return;
    }
*/
    if ( get_eq_char( ch, WEAR_HOLD ) != NULL )
    {
	send_to_char( "You dont have a free hand!", ch );
	return;
    }

    if ( ch == victim )
    {
	act_color( AT_ACTION, "You rub $p`X on your body.", ch, herb, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n rubs $p`X on $s body.", ch, herb, NULL, TO_ROOM, POS_RESTING );
    }
    else
    {
	act_color( AT_ACTION, "You rub $p`X on $N`X's body.", ch, herb, victim, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n rubs $p`X on $N`X's body.", ch, herb, victim, TO_NOTVICT, POS_RESTING );
	act_color( AT_ACTION, "$n rubs $p`X on your body.", ch, herb, victim, TO_VICT, POS_RESTING );
    }

    if ( herb->item_type != ITEM_HERB )
	return;

    if ( !IS_IMMORTAL( ch ) )
	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( IS_SET( herb->value[5], POISONED_RUBBED ) )
    {
	act_color( AT_ACTION, "Your skin blisters.", victim, NULL, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n's skin blisters.", victim, NULL, NULL, TO_ROOM, POS_RESTING );

	af.where     = TO_AFFECTS;
	af.type      = gsn_poison;
	af.level     = number_fuzzy( herb->value[0] );
	af.duration  = 10 + herb->value[0] / 2;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_POISON;
	affect_join( victim, &af );
	extract_obj( herb );
	return;
    }

    if ( number_percent( ) >= 20 + get_skill( ch, gsn_herbs ) * 4 / 5 )
    {
	act_color( AT_ACTION, "You smear $p around but nothing happens.",
		   ch, herb, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n smears $p around but nothing happens.",
		   ch, herb, NULL, TO_ROOM, POS_RESTING );
	check_improve( ch, gsn_herbs, FALSE, 2 );
    }
    else
    {
	obj_cast_spell( herb->value[2], herb->value[0], ch, victim, NULL );
	check_improve( ch, gsn_herbs, TRUE, 3 );
    }

    extract_obj( herb );
}


void
do_steal( CHAR_DATA *ch, char *argument )
{
    char	buf  [MAX_STRING_LENGTH];
    char	arg1 [MAX_INPUT_LENGTH];
    char	arg2 [MAX_INPUT_LENGTH];
    MONEY	amt;
    CHAR_DATA *	victim;
    OBJ_DATA *	obj;
    int 	percent;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( steal_mount( ch, arg1, arg2 ) )
        return;

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Steal what from whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim))
	return;

    if ( IS_NPC(victim)
	  && victim->position == POS_FIGHTING)
    {
	send_to_char(  "Kill stealing is not permitted.\n\r"
		       "You'd better not -- you might get hit.\n\r",ch);
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_steal].beats );
    percent  = number_percent();

    if (!IS_AWAKE(victim))
    	percent -= 10;
    else if (!can_see(victim,ch))
    	percent += 25;
    else
	percent += 50;

    if ( ((ch->level + 7 < victim->level || ch->level -7 > victim->level)
    && !IS_NPC(victim) && !IS_NPC(ch) )
    || ( !IS_NPC(ch) && percent > get_skill(ch,gsn_steal))
    || ( !IS_NPC(ch) && !is_clan(ch)) )
    {
	/*
	 * Failure.
	 */
	send_to_char( "Oops.\n\r", ch );
	affect_strip( ch, gsn_sneak );
	xREMOVE_BIT( ch->affected_by, AFF_SNEAK );

	act( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
	act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
	switch(number_range(0,3))
	{
	case 0 :
	   sprintf( buf, "%s is a lousy thief!", ch->name );
	   break;
        case 1 :
	   sprintf( buf, "%s couldn't rob %s way out of a paper bag!",
		    ch->name,(ch->sex == 2) ? "her" : "his");
	   break;
	case 2 :
	    sprintf( buf,"%s tried to rob me!",ch->name );
	    break;
	case 3 :
	    sprintf(buf,"Keep your hands out of there, %s!",ch->name);
	    break;
        }
        if (!IS_AWAKE(victim))
            do_function(victim, &do_wake, "");
	if (IS_AWAKE(victim))
	    do_function(victim, &do_yell, buf );
	if ( !IS_NPC(ch) )
	{
	    if ( IS_NPC(victim) )
	    {
	        check_improve(ch,gsn_steal,FALSE,2);
		multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	    else
	    {
		sprintf(buf,"$N tried to steal from %s.",victim->name);
		wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
		if ( !IS_SET(ch->act, PLR_THIEF) )
		{
		    SET_BIT(ch->act, PLR_THIEF);
		    send_to_char( "*** You are now a THIEF!! ***\n\r", ch );
		    save_char_obj( ch );
		}
	    }
	}

	return;
    }

    if ( !str_cmp( arg1, "coin"   )
    ||   !str_cmp( arg1, "coins"  )
    ||   !str_cmp( arg1, "gold"   )
    ||	 !str_cmp( arg1, "silver" )
    ||	 !str_cmp( arg1, "copper" ) )
    {
	amt.gold    = victim->money.gold    * number_range( 1, ch->level ) / MAX_LEVEL;
	amt.silver = victim->money.silver * number_range( 1, ch->level ) / MAX_LEVEL;
	amt.copper   = victim->money.copper   * number_range( 1, ch->level ) / MAX_LEVEL;
	amt.fract    = victim->money.fract    * number_range( 1, ch->level ) / MAX_LEVEL;

	if ( amt.gold <= 0 && amt.silver <= 0 && amt.copper <= 0 && amt.fract <= 0 )
	{
	    send_to_char( "You couldn't get any coins.\n\r", ch );
	    return;
	}

	money_add( &ch->money, &amt, FALSE );
	money_subtract( &victim->money, &amt, FALSE );
	ch_printf( ch, "Bingo!  You got%s.\n\r", money_string( &amt, FALSE, FALSE ) );
	check_improve(ch,gsn_steal,TRUE,2);
	return;
    }

    if ( ( obj = get_obj_carry( victim, arg1, ch ) ) == NULL )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj )
    ||   IS_SET(obj->extra_flags, ITEM_INVENTORY)
    ||   obj->level > ch->level )
    {
	send_to_char( "You can't pry it away.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	send_to_char( "You have your hands full.\n\r", ch );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	send_to_char( "You can't carry that much weight.\n\r", ch );
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, ch );
    act("You pocket $p.",ch,obj,NULL,TO_CHAR);
    check_improve(ch,gsn_steal,TRUE,2);
    send_to_char( "Got it!\n\r", ch );
    return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
    /*char buf[MAX_STRING_LENGTH];*/
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;

    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
	if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
	    break;
    }

    if ( pShop == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return NULL;
    }

    /*
     * Undesirables.
     *
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_KILLER) )
    {
	do_function(keeper, &do_say, "Killers are not welcome!");
	sprintf(buf, "%s the KILLER is over here!\n\r", ch->name);
	do_function(keeper, &do_yell, buf );
	return NULL;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_THIEF) )
    {
	do_function(keeper, &do_say, "Thieves are not welcome!");
	sprintf(buf, "%s the THIEF is over here!\n\r", ch->name);
	do_function(keeper, &do_yell, buf );
	return NULL;
    }
	*/
    /*
     * Shop hours.
     */
    if ( pShop->open_hour <= pShop->close_hour )
    {
	if ( time_info.hour < pShop->open_hour )
	{
	    do_function(keeper, &do_say, "Sorry, I am closed. Come back later.");
	    return NULL;
	}

	if ( time_info.hour > pShop->close_hour )
	{
	    do_function(keeper, &do_say, "Sorry, I am closed. Come back tomorrow.");
	    return NULL;
	}
    }
    else
    {
	if ( time_info.hour > pShop->close_hour && time_info.hour < pShop->open_hour )
	{
	    do_function(keeper, &do_say, "Sorry, I am closed. Come back later.");
	    return NULL;
	}
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
	do_function(keeper, &do_say, "I don't trade with folks I can't see.");
	return NULL;
    }

    return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper( OBJ_DATA *obj, CHAR_DATA *ch )
{
    OBJ_DATA *t_obj, *t_obj_next;

    /* see if any duplicates are found */
    for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next)
    {
	t_obj_next = t_obj->next_content;

	if (obj->pIndexData == t_obj->pIndexData
	&&  !str_cmp(obj->short_descr,t_obj->short_descr))
	{
	    /* if this is an unlimited item, destroy the new one */
	    if (IS_OBJ_STAT(t_obj,ITEM_INVENTORY))
	    {
		extract_obj(obj);
		return;
	    }
	    obj->cost = t_obj->cost; /* keep it standard */
	    break;
	}
    }

    if (t_obj == NULL)
    {
	obj->next_content = ch->carrying;
	ch->carrying = obj;
    }
    else
    {
	obj->next_content = t_obj->next_content;
	t_obj->next_content = obj;
    }

    obj->carried_by      = ch;
    obj->in_room         = NULL;
    obj->in_obj          = NULL;
    ch->carry_number    += get_obj_number( obj );
    ch->carry_weight    += get_obj_weight( obj );
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper( CHAR_DATA *ch, CHAR_DATA *keeper, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
    {
        if (obj->wear_loc == WEAR_NONE
        &&  can_see_obj( keeper, obj )
	&&  can_see_obj(ch,obj)
        &&  is_name( arg, obj->name ) )
        {
            if ( ++count == number )
                return obj;

	    /* skip other objects of the same name */
	    while (obj->next_content != NULL
	    && obj->pIndexData == obj->next_content->pIndexData
	    && !str_cmp(obj->short_descr,obj->next_content->short_descr))
		obj = obj->next_content;
        }
    }

    return NULL;
}


int
get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int cost;

    if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
	return 0;

    if ( fBuy )
    {
	cost = obj->cost * pShop->profit_buy  / 100;
    }
    else
    {
	OBJ_DATA *obj2;
	int itype;

	cost = 0;
	for ( itype = 0; itype < MAX_TRADE; itype++ )
	{
	    if ( obj->item_type == pShop->buy_type[itype] )
	    {
		cost = obj->cost * pShop->profit_sell / 100;
		break;
	    }
	}

	if ( !IS_OBJ_STAT( obj, ITEM_SELL_EXTRACT ) )
	    for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
	    {
	    	if ( obj->pIndexData == obj2->pIndexData
		&&   !str_cmp(obj->short_descr,obj2->short_descr) )
		{
	 	    if ( IS_OBJ_STAT( obj2, ITEM_INVENTORY ) )
			cost /= 2;
		    else
                    	cost = cost * 3 / 4;
		}
	    }
    }

    if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
    {
	if ( obj->value[1] == 0 )
	    cost /= 4;
	else
	    cost = cost * obj->value[2] / obj->value[1];
    }

    return cost;
}


void
do_brew( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    char	buf2[MAX_INPUT_LENGTH];
    OBJ_DATA *	herb;
    OBJ_DATA *	potion;
    OBJ_DATA *	water;
    int		h2o;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Brew what?\n\r", ch );
	return;
    }

    if ( ( herb = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	send_to_char( "You don't have that herb.\n\r", ch );
	return;
    }

    if ( herb->item_type != ITEM_HERB )
    {
	send_to_char( "You can't brew that.\n\r", ch );
	return;
    }

    h2o = liq_lookup( "water" );
    for ( water = ch->in_room->contents; water != NULL; water = water->next_content )
    {
	if ( water->item_type == ITEM_FOUNTAIN
	&&   can_see_obj( ch, water )
	&&   water->value[1] == 0
	&&   water->value[2] == h2o )
	    break;
    }

    if ( water == NULL )
    {
	for ( water = ch->carrying; water != NULL; water = water->next_content )
	{
	    if ( water->item_type == ITEM_DRINK_CON
	    &&	 water->wear_loc == WEAR_NONE
	    &&	 can_see_obj( ch, water )
	    &&	 water->value[2] == h2o )
		break;
	}
    }

    if ( water == NULL )
    {
	send_to_char( "You have no water.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) )
	WAIT_STATE( ch, PULSE_VIOLENCE * 3 );

    act_color( AT_ACTION, "You brew a tea from $p.", ch, herb, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n brews a tea from $p.", ch, herb, NULL, TO_ROOM, POS_RESTING );

    potion = create_object( get_obj_index( OBJ_VNUM_HERBAL_TEA ), herb->level );
    potion->level = herb->level;
    potion->value[0] = herb->value[0];

    if ( number_percent( ) < 20 + get_skill( ch, gsn_herbs ) * 4 / 5 )
    {
	potion->value[1] = herb->value[3];
	check_improve( ch, gsn_herbs, TRUE, 2 );
    }
    else
    {
	act_color( AT_ACTION, "The tea looks rather tepid.",
		   ch, NULL, NULL, TO_CHAR, POS_RESTING );
	check_improve( ch, gsn_herbs, FALSE, 2 );
    }

    if ( water->value[3] != 0 || IS_SET( herb->value[5], POISONED_BREWED ) )
	potion->value[5] = 1;

    strip_color( buf2, herb->short_descr );
    sprintf( buf, potion->short_descr, buf2 );
    free_string( potion->short_descr );
    potion->short_descr = str_dup( buf );
    sprintf( buf, potion->description, buf2 );
    free_string( potion->description );
    potion->description = str_dup( buf );

    extract_obj( herb );
    if ( water->item_type == ITEM_DRINK_CON )
	extract_obj( water );
    obj_to_char( potion, ch );
}


void
do_buy( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    const char *p;
    int		cost;
    MONEY	amt;
    int		roll;
    bool	fMount;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Buy what?\n\r", ch );
	return;
    }

    amt.gold = 0;
    amt.silver = 0;
    amt.copper = 0;
    amt.fract = 0;

    if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *pet;
	CHAR_DATA *keeper;
	ROOM_INDEX_DATA *pRoomIndexNext;
	ROOM_INDEX_DATA *in_room;

	smash_tilde( argument );

	if ( IS_NPC( ch ) )
	{
	    send_to_char( "You can't buy a pet.\n\r", ch );
	    return;
	}

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	argument = one_argument( argument, arg );

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( pRoomIndexNext == NULL )
	{
	    bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "Sorry, you can't buy that here.\n\r", ch );
	    return;
	}

	in_room     = ch->in_room;
	ch->in_room = pRoomIndexNext;
	pet         = get_char_room( ch, arg );
	ch->in_room = in_room;

	if ( pet == NULL || ( !IS_SET( pet->act, ACT_PET ) && !IS_SET( pet->act, ACT_MOUNT ) ) )
	{
	    send_to_char( "Sorry, you can't buy that here.\n\r", ch );
	    return;
	}

	/* The "!= 0 " is to force fMount to TRUE or FALSE, since IS_SET()
	 * actually returns the bit value. */
	fMount = IS_SET( pet->act, ACT_MOUNT ) != 0;

	if ( fMount && get_skill( ch, gsn_riding ) < 1 )
	{
	    send_to_char( "You don't know how to ride.\n\r", ch );
	    return;
	}

	if ( ch->pet != NULL && !fMount )
	{
	    send_to_char( "You already own a pet.\n\r", ch );
	    return;
	}

	if ( ch->mount != NULL && fMount )
	{
	    send_to_char( "You already own a mount.\n\r", ch );
	    return;
	}

	if ( pet->pIndexData->wealth <= 0 )
 	    cost = 8 * pet->level * pet->level;
 	else
	    cost = pet->pIndexData->wealth;

	if ( CASH_VALUE( ch->money ) < cost )
	{
	    send_to_char( "You can't afford it.\n\r", ch );
	    return;
	}

	if ( ch->level < pet->level )
	{
	    ch_printf( ch, "You're not powerful enough to master this %s.\n\r",
		       fMount ? "mount" : "pet" );
	    return;
	}

	if ( fMount && ch->size >= pet->size )
	{
	    send_to_char( "You are too big to ride a creature like that.\n\r", ch );
	    return;
	}

	/* haggle */
	roll = number_percent( );
	if ( roll < get_skill( ch, gsn_haggle ) )
	{
	    cost -= cost / 2 * roll / 96;
	    amt.fract = cost;
	    normalize( &amt );
	    sprintf( buf, "You haggle the price down to%s.",
		     money_string( &amt, FALSE, FALSE ) );
	    act_color( AT_ACTION, buf, ch, NULL, NULL, TO_CHAR, POS_RESTING );
	    check_improve( ch, gsn_haggle, TRUE, 4 );
	}

	deduct_cost( ch, cost );
	pet			= create_mobile( pet->pIndexData );
	if ( fMount )
	{
	    SET_BIT( pet->act, ACT_MOUNT );
	    REMOVE_BIT( pet->act, ACT_PET );
	}
	else
	{
	    REMOVE_BIT( pet->act, ACT_MOUNT );
	    SET_BIT( pet->act, ACT_PET );
	}

	if ( !fMount )
	    xSET_BIT( pet->affected_by, AFF_CHARM );

	pet->comm = COMM_NOSHOUT|COMM_NOCHANNELS;

	one_argument( argument, arg );
	if ( arg[0] != '\0' )
	{
	    sprintf( buf, "%s %s", pet->name, arg );
	    free_string( pet->name );
	    pet->name = str_dup( buf );
	    first_arg( argument, arg, FALSE );
	    p = aoran_skip( pet->short_descr );
	    sprintf( buf, "%s the %s", arg, p );
	    free_string( pet->short_descr );
	    pet->short_descr = str_dup( buf );
	}

	sprintf( buf, fMount
		      ? "%sThe name \"%s\" is branded on the leg.\n\r"
		      : "%sA neck tag says 'I belong to %s'.\n\r",
		 pet->description, ch->name );
	free_string( pet->description );
	pet->description = str_dup( buf );

	char_to_room( pet, ch->in_room );
	add_follower( pet, ch );
	if ( fMount )
	{
	    send_to_char( "Enjoy your mount.\n\r", ch );
	    act( "$n bought $N as a mount.", ch, NULL, pet, TO_ROOM );
	    one_argument( pet->name, buf );
	    do_mount( ch, buf );
	}
	else
	{
	    pet->leader = ch;
	    ch->pet = pet;
	    send_to_char( "Enjoy your pet.\n\r", ch );
	    act( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
	}
	mprog_buy_trigger( keeper, ch, pet, NULL );
	return;
    }
    else
    {
	CHAR_DATA *keeper;
	OBJ_DATA *obj,*t_obj;
	char arg[MAX_INPUT_LENGTH];
	int number, count = 1;

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	number = mult_argument(argument,arg);
	obj  = get_obj_keeper( ch,keeper, arg );
	cost = get_cost( keeper, obj, TRUE );

	if (number < 1 || number > 99)
	{
	    act_color( AT_TELL, "`W$n tells you '`YGet real!`W'", keeper, NULL, ch, TO_VICT, POS_RESTING );
	    ch->reply = keeper;
	    return;
	}

	if ( cost <= 0 || !can_see_obj( ch, obj ) )
	{
	    act_color( AT_TELL, "`W$n tells you '`YI don't sell that -- try 'list'`W'.",
		keeper, NULL, ch, TO_VICT, POS_RESTING );
	    ch->reply = keeper;
	    return;
	}

	if ( !IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	{
	    for ( t_obj = obj->next_content;
		  count < number && t_obj != NULL;
		  t_obj = t_obj->next_content )
	    {
	    	if ( t_obj->pIndexData == obj->pIndexData
	    	&&   !str_cmp(t_obj->short_descr,obj->short_descr ) )
		    count++;
	    	else
		    break;
	    }

	    if ( count < number )
	    {
	    	act_color( AT_TELL,
	    		   "`W$n tells you '`YI don't have that many in stock.`W'",
			    keeper, NULL, ch, TO_VICT, POS_RESTING );
	    	ch->reply = keeper;
	    	return;
	    }
	}

	if ( CASH_VALUE( ch->money ) < cost * number )
	{
	    if ( number > 1 )
		act_color( AT_TELL,
			   "`W$n tells you '`XYou can't afford to buy that many.`W'",
			   keeper, obj, ch, TO_VICT, POS_RESTING );
	    else
	    	act_color( AT_TELL,
	    		   "`W$n tells you '`XYou can't afford to buy $p`W'.",
			   keeper, obj, ch, TO_VICT, POS_RESTING );
	    ch->reply = keeper;
	    return;
	}

	if ( obj->level > ch->level )
	{
	    act_color( AT_TELL,
		       "`W$n tells you '`XYou can't use $p yet.`W'",
		       keeper, obj, ch, TO_VICT, POS_RESTING );
	    ch->reply = keeper;
	    return;
	}

	if (ch->carry_number +  number * get_obj_number(obj) > can_carry_n(ch))
	{
	    send_to_char( "You can't carry that many items.\n\r", ch );
	    return;
	}

	if ( ch->carry_weight + number * get_obj_weight(obj) > can_carry_w(ch))
	{
	    send_to_char( "You can't carry that much weight.\n\r", ch );
	    return;
	}

	/* haggle */
	roll = number_percent( );
	if ( !IS_OBJ_STAT( obj, ITEM_SELL_EXTRACT )
	&& roll < get_skill( ch, gsn_haggle ) )
	{
	    cost -= obj->cost / 2 * roll / 100;
	    act_color( AT_ACTION,
		       "You haggle with $N.",
		       ch, NULL, keeper, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n haggles with $N.",
		       ch, NULL, keeper, TO_ROOM, POS_RESTING );
	    check_improve( ch, gsn_haggle, TRUE, 4 );
	}

	amt.fract = cost;
	normalize( &amt );

	if ( number > 1 )
	{
	    MONEY amt2;
	    amt2.gold = 0;
	    amt2.silver = 0;
	    amt2.copper = 0;
	    amt2.fract = cost * number;
	    normalize( &amt2 );
	    sprintf( buf, "$n buys $p[%d].", number );
	    act_color( AT_ACTION, buf, ch, obj, NULL, TO_ROOM, POS_RESTING );
	    sprintf( buf, "You buy $p[%d] for%s.", number, money_string( &amt2, FALSE, FALSE ) );
	    act_color( AT_ACTION, buf, ch, obj, NULL, TO_CHAR, POS_RESTING );
	}
	else
	{
	    act_color( AT_ACTION, "$n buys $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
	    sprintf( buf, "You buy $p for%s.", money_string( &amt, FALSE, FALSE ) );
	    act_color( AT_ACTION, buf, ch, obj, NULL, TO_CHAR, POS_RESTING );
	}
	deduct_cost( ch, cost * number );
	money_add( &keeper->money, &amt, TRUE );

	t_obj = obj;
	for ( count = 0; count < number; count++ )
	{
	    if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    	t_obj = create_object( obj->pIndexData, obj->level );
	    else
	    {
		t_obj = obj;
		obj = obj->next_content;
	    	obj_from_char( t_obj );
	    }

	    if ( t_obj->timer > 0 && !IS_OBJ_STAT( t_obj, ITEM_HAD_TIMER ) )
	    	t_obj->timer = 0;
	    REMOVE_BIT( t_obj->extra_flags, ITEM_HAD_TIMER );
	    obj_to_char( t_obj, ch );
	    if ( cost < t_obj->cost )
	    	t_obj->cost = cost;
	}
	oprog_buy_trigger( t_obj, ch, keeper );
	mprog_buy_trigger( keeper, ch, NULL, t_obj );
    }
}


void
do_list( CHAR_DATA *ch, char *argument )
{
    MONEY	amt;
    BUFFER *	pBuf;
    char	buf[MAX_STRING_LENGTH];
    char *	p;

    if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
	ROOM_INDEX_DATA *	pRoomIndexNext;
	CHAR_DATA *		keeper;
	CHAR_DATA *		pet;
	bool			found;

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );

	if ( pRoomIndexNext == NULL )
	{
	    bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	pBuf = new_buf( );
	found = FALSE;
	for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
	{
	    if ( IS_NPC( pet ) && ( IS_SET( pet->act, ACT_PET ) || IS_SET( pet->act, ACT_MOUNT ) ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    add_buf( pBuf, " Lvl Price\n\r" );
		}

		amt.gold = 0;
		amt.silver = 0;
		amt.copper = 0;
		if ( pet->pIndexData->wealth <= 0 )
		    amt.fract = 8 * pet->level * pet->level;
		else
		    amt.fract = pet->pIndexData->wealth;
		normalize( &amt );
		p = buf;
		p += sprintf( p, "[%3d ", pet->level );
		if ( amt.gold == 0 )
		    p = stpcpy( p, "    " );
		else
		    p += sprintf( p, "%3d%c", amt.gold, GOLD_INITIAL );
		if ( amt.silver == 0 )
		    p = stpcpy( p, "    " );
		else
		    p += sprintf( p, " %2d%c", amt.silver, SILVER_INITIAL );
		if ( amt.copper == 0 )
		    p = stpcpy( p, "    " );
		else
		    p += sprintf( p, " %2d%c", amt.copper, COPPER_INITIAL );
		if ( amt.fract == 0 )
		    p = stpcpy( p, "   " );
		else
		    p += sprintf( p, " %d%c", amt.fract, FRACT_INITIAL );
		p += sprintf( p, "] (%s) %s`X\n\r",
			      IS_SET( pet->act, ACT_MOUNT ) ? "Mnt" : "Pet",
			      pet->short_descr );
		add_buf( pBuf, buf );
	    }
	}
	if ( !found )
	    add_buf( pBuf, "Sorry, we're out of pets right now.\n\r" );
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return;
    }
    else
    {
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost,count;
	bool found;
	char arg[MAX_INPUT_LENGTH];

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;
        one_argument( argument, arg );

	pBuf = new_buf( );
	found = FALSE;
	for ( obj = keeper->carrying; obj; obj = obj->next_content )
	{
	    if ( obj->wear_loc == WEAR_NONE
	    &&   can_see_obj( ch, obj )
	    &&   ( cost = get_cost( keeper, obj, TRUE ) ) > 0
	    &&   ( arg[0] == '\0'
	       ||  is_name( arg, obj->name ) ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    add_buf( pBuf, "[Lvl Price           Qty] Item\n\r" );
		}

		p = buf;
		amt.gold = 0;
		amt.silver = 0;
		amt.copper = 0;
		amt.fract = 0;
		if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
		{
		    *p++ = '[';
		    p += sprintf( p, "%3d ", obj->level );
		    amt.fract = obj->cost;
		    normalize( &amt );
		    if ( amt.gold == 0 )
			p = stpcpy( p, "    " );
		    else
			p += sprintf( p, "%3d%c", amt.gold, GOLD_INITIAL );
		    if ( amt.silver == 0 )
			p = stpcpy( p, "    " );
		    else
			p += sprintf( p, " %2d%c", amt.silver, SILVER_INITIAL );
		    if ( amt.copper == 0 )
			p = stpcpy( p, "    " );
		    else
			p += sprintf( p, " %2d%c", amt.copper, COPPER_INITIAL );
		    if ( amt.fract == 0 )
			p = stpcpy( p, "   " );
		    else
			p += sprintf( p, " %d%c", amt.fract, FRACT_INITIAL );
		    p += sprintf( p, " -- ] %s`X\n\r",
				  obj->short_descr );
		}
		else
		{
		    count = 1;

		    while ( obj->next_content != NULL
		    && obj->pIndexData == obj->next_content->pIndexData
		    && !str_cmp( obj->short_descr,
			         obj->next_content->short_descr ) )
		    {
			obj = obj->next_content;
			count++;
		    }

		    *p++ = '[';
		    p += sprintf( p, "%3d ", obj->level );
		    amt.fract = obj->cost;
		    normalize( &amt );
		    if ( amt.gold == 0 )
			p = stpcpy( p, "    " );
		    else
			p += sprintf( p, "%3d%c", amt.gold, GOLD_INITIAL );
		    if ( amt.silver == 0 )
			p = stpcpy( p, "    " );
		    else
			p += sprintf( p, " %2d%c", amt.silver, SILVER_INITIAL );
		    if ( amt.copper == 0 )
			p = stpcpy( p, "    " );
		    else
			p += sprintf( p, " %2d%c", amt.copper, COPPER_INITIAL );
		    if ( amt.fract == 0 )
			p = stpcpy( p, "   " );
		    else
			p += sprintf( p, " %d%c", amt.fract, FRACT_INITIAL );
		    p += sprintf( p, " %2d ] %s`X\n\r",
				  count,
				  obj->short_descr );
		}
		add_buf( pBuf, buf );
	    }
	}

	if ( !found && !IS_SET( keeper->pIndexData->progtypes, MP_LIST_PROG ) )
	    add_buf( pBuf, "You can't buy anything here.\n\r" );
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	mprog_list_trigger( keeper, ch );
	return;
    }
}


void
do_sell( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	keeper;
    OBJ_DATA *	obj;
    int		cost;
    MONEY	amt;
    int		roll;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Sell what?\n\r", ch );
	return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

    if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
	act( "$N isn't interested.", ch, NULL, keeper, TO_CHAR );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	act_color( AT_TELL, "`W$n tells you '`XYou don't have that item.`W'",
	    keeper, NULL, ch, TO_VICT, POS_RESTING );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }
    if (IS_SET(obj->extra_flags, ITEM_QUESTITEM))
    {
       stc("This is a quest object. Only a qestmaster will buy this item back!!\n\r",ch);
       return;
    }
    if (!can_see_obj(keeper,obj))
    {
	act( "$n doesn't see what you are offering.", keeper, NULL, ch, TO_VICT );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }
    if ( cost > CASH_VALUE( keeper->money ) )
    {
	act_color( AT_TELL,
		   "`W$n tells you '`XI'm afraid I don't have enough wealth to buy $p.`W'",
		   keeper, obj, ch, TO_VICT, POS_RESTING );
	return;
    }

    /* haggle */
    roll = number_percent( );
    if ( !IS_OBJ_STAT( obj, ITEM_SELL_EXTRACT )
    &&	 roll < get_skill( ch, gsn_haggle ) )
    {
	act_color( AT_ACTION, "You haggle with the shopkeeper.",
		ch, NULL, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n haggles with $N.",
		   ch, NULL, keeper, TO_ROOM, POS_RESTING );
        cost += obj->cost / 2 * roll / 100;
        cost = UMIN( cost, 95 * get_cost( keeper, obj, TRUE ) / 100 );
	cost = UMIN( cost, CASH_VALUE( keeper->money ) );
        check_improve( ch, gsn_haggle, TRUE, 4) ;
    }
    act_color( AT_ACTION, "$n sells $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
    amt.gold = 0;
    amt.silver = 0;
    amt.copper = 0;
    amt.fract = cost;
    normalize( &amt );
    sprintf( buf, "You sell $p for%s.",
	     money_string( &amt, FALSE, FALSE ) );
    act_color( AT_ACTION, buf, ch, obj, NULL, TO_CHAR, POS_RESTING );
    money_add( &ch->money, &amt, FALSE );
    deduct_cost( keeper, cost );
    normalize( &keeper->money );

    oprog_sell_trigger( obj, ch, keeper );
    mprog_sell_trigger( keeper, ch, obj );

    if ( obj->item_type == ITEM_TRASH || IS_OBJ_STAT( obj, ITEM_SELL_EXTRACT ) )
    {
	extract_obj( obj );
    }
    else
    {
	obj_from_char( obj );
	if (obj->timer)
	    SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
	else
	    obj->timer = number_range(50,100);
	obj_to_keeper( obj, keeper );
    }

    return;
}


void
do_sheath( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    OBJ_DATA *	sheath;
    OBJ_DATA *	weapon;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Sheath what?\n\r", ch );
	return;
    }

    for ( sheath = ch->carrying; sheath != NULL; sheath = sheath->next_content )
    {
	if ( sheath->wear_loc != WEAR_NONE
	&&   sheath->item_type == ITEM_SHEATH
	&&   can_see_obj( ch, sheath ) )
	    break;
    }

    if ( sheath == NULL )
    {
	send_to_char( "You aren't wearing a weapon sheath.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL
    &&	 get_eq_char( ch, WEAR_DUAL ) == NULL )
    {
	send_to_char( "You aren't wielding anything.\n\r", ch );
	return;
    }

    if ( ( weapon = get_obj_wear( ch, arg ) ) == NULL
    ||	 !can_see_obj( ch, weapon )
    ||	 ( weapon->wear_loc != WEAR_WIELD && weapon->wear_loc != WEAR_DUAL ) )
    {
	send_to_char( "You aren't wielding that.\n\r", ch );
	return;
    }

    for ( sheath = ch->carrying; sheath != NULL; sheath = sheath->next_content )
    {
	if ( sheath->wear_loc != WEAR_NONE
	&&   sheath->item_type == ITEM_SHEATH
	&&   can_see_obj( ch, sheath )
	&&   sheath->contains == NULL
	&&   ( weapon->value[0] == sheath->value[3]
	    || weapon->value[0] == sheath->value[4]
	    || weapon->value[0] == sheath->value[5] ) )
	    break;
    }

    if ( sheath == NULL )
    {
	send_to_char( "You aren't wearing a suitable weapon sheath.\n\r", ch );
	return;
    }

    if ( IS_SET( sheath->value[1], CONT_CLOSED ) )
    {
	act( "$p is closed.", ch, sheath, NULL, TO_CHAR );
	return;
    }

    if ( remove_obj( ch, weapon->wear_loc, TRUE ) )
    {
	obj_from_char( weapon );
	obj_to_obj( weapon, sheath );
	act_color( AT_ACTION, "$n sheathes $p`X in $P`X.", ch, weapon, sheath, TO_ROOM, POS_RESTING );
	act_color( AT_ACTION, "You sheath $p`X in $P`X.", ch, weapon, sheath, TO_CHAR, POS_RESTING );
	oprog_put_trigger( weapon, ch, sheath );
    }

    return;

}


void
do_value( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	keeper;
    OBJ_DATA *	obj;
    int		cost;
    MONEY	amt;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Value what?\n\r", ch );
	return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

    if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
	act( "$N isn't interested.", ch, NULL, keeper, TO_CHAR );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	act_color( AT_TELL, "`W$n tells you '`XYou don't have that item.`W'",
	    keeper, NULL, ch, TO_VICT, POS_RESTING );
	ch->reply = keeper;
	return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
        act( "$n doesn't see what you are offering.", keeper, NULL, ch, TO_VICT );
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }

    amt.gold = 0;
    amt.silver = 0;
    amt.copper = 0;
    amt.fract = cost;
    normalize( &amt );
    sprintf( buf,
	"`W$n tells you '`XI'll give you%s for $p.`W'",
	money_string( &amt, FALSE, FALSE ) );
    act_color( AT_TELL, buf, keeper, obj, ch, TO_VICT, POS_RESTING );
    ch->reply = keeper;

    return;
}


void
do_join( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObjIndex;
    OBJ_DATA *		aObj;
    OBJ_DATA *		bObj;
    OBJ_DATA *		obj;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Join what to what?\n\r", ch );
	return;
    }

    if ( ( aObj = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	ch_printf( ch, "You aren't carrying any %s.\n\r", arg1 );
	return;
    }
    if ( ( bObj = get_obj_carry( ch, arg2, ch ) ) == NULL )
    {
	ch_printf( ch, "You aren't carrying any %s.\n\r", arg2 );
	return;
    }

    if ( ch->level < aObj->level || ch->level < bObj->level )
    {
	send_to_char( "You are of too low a level to do that.\n\r", ch );
	return;
    }

    if ( !can_use_obj( ch, aObj, TRUE ) || !can_use_obj( ch, bObj, TRUE ) )
        return;

    if ( aObj->pIndexData->join != bObj->pIndexData->join ||
	 aObj->pIndexData == bObj->pIndexData ||
	 ( pObjIndex = get_obj_index( aObj->pIndexData->join ) ) == NULL )
    {
	ch_printf( ch, "%s cannot be joined with %s.\n\r",
		   capitalize( aObj->short_descr ), bObj->short_descr );
	return;
    }

    if ( ch->level < pObjIndex->level )
    {
	send_to_char( "You are of too low a level to do that.\n\r", ch );
	return;
    }

    oprog_join_trigger( aObj, ch, bObj );

    obj = create_object( pObjIndex, pObjIndex->level );
    sprintf( buf, "$n joins $p to $P to create %s.", obj->short_descr );
    act( buf, ch, aObj, bObj, TO_ROOM );
    sprintf( buf, "You join $p to $P to create %s.", obj->short_descr );
    act( buf, ch, aObj, bObj, TO_CHAR );

    obj_from_char( aObj );
    extract_obj( aObj );
    obj_from_char( bObj );
    extract_obj( bObj );
    obj_to_char( obj, ch );

}


void
do_separate( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pIndex;
    OBJ_DATA *		obj;
    OBJ_DATA *		aObj;
    OBJ_DATA *		bObj;
    char		buf[MAX_STRING_LENGTH];

    if ( ( obj = get_obj_carry( ch, argument, ch ) ) == NULL )
    {
	send_to_char( "You are not carrying that item.\n\r", ch );
	return;
    }

    if ( ch->level < obj->level )
    {
	send_to_char( "You are of too low a level to do that.\n\r", ch );
	return;
    }

    if ( !can_use_obj( ch, obj, TRUE ) )
        return;

    if ( !get_obj_index( obj->pIndexData->sep_one ) ||
	 !get_obj_index( obj->pIndexData->sep_two ) )
    {
	send_to_char( "That item cannot be separated.\n\r", ch );
	return;
    }

    pIndex = get_obj_index( obj->pIndexData->sep_one );
    aObj = create_object( pIndex, pIndex->level );
    pIndex = get_obj_index( obj->pIndexData->sep_two );
    bObj = create_object( pIndex, pIndex->level );

    if ( ch->level < aObj->level || ch->level < bObj->level )
    {
	extract_obj( aObj );
	extract_obj( bObj );
	send_to_char( "You are of too low a level to do that.\n\r", ch );
	return;
    }

    sprintf( buf, "$n separates $p into %s and %s.",
	     aObj->short_descr, bObj->short_descr );
    act( buf, ch, obj, NULL, TO_ROOM );
    sprintf( buf, "You separate $p into %s and %s.",
	     aObj->short_descr, bObj->short_descr );
    act( buf, ch, obj, NULL, TO_CHAR );

    oprog_separate_trigger( obj, ch );
    obj_from_char( obj );
    extract_obj( obj );
    obj_to_char( aObj, ch );
    obj_to_char( bObj, ch );

}


void
do_wield( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    bool	found;
    OBJ_DATA *	weapon;
    OBJ_DATA *	sheath;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Wield what?\n\r", ch );
	return;
    }

    /* find the weapon in a sheath or in their inventory */
    if ( arg2[0] != '\0' )
    {
	if ( ( sheath = get_obj_wear( ch, arg2 ) ) == NULL )
	{
	    act( "You're not wearing a $T.\n\r", ch, NULL, arg2, TO_CHAR );
	    return;
	}
	if ( sheath->item_type != ITEM_SHEATH )
	{
	    act( "A $T isn't a weapon sheath.", ch, NULL, arg2, TO_CHAR );
	    return;
	}
	if ( IS_SET( sheath->value[1], CONT_CLOSED ) )
	{
	    act( "$p is closed.", ch, sheath, NULL, TO_CHAR );
	    return;
	}
	if ( ( weapon = sheath->contains ) == NULL )
	{
	    act( "There's nothing in $p.", ch, sheath, NULL, TO_CHAR );
 	    return;
	}
	if ( !is_name( arg1, weapon->name ) )
	{
	    act( "There's no $T in $p.", ch, sheath, arg1, TO_CHAR );
	    return;
	}
    }
    else
    {
	weapon = NULL;
	found = FALSE;
	for ( sheath = ch->carrying; sheath != NULL; sheath = sheath->next_content )
	{
	    if ( sheath->item_type == ITEM_SHEATH
	    &&	 sheath->wear_loc != WEAR_NONE
	    &&	 can_see_obj( ch, sheath )
	    &&	 ( ( weapon = sheath->contains ) != NULL )
	    &&	 is_name( arg1, weapon->name ) )
	    {
		found = TRUE;
		break;
	    }
	}
	if ( !found )
	{
	    do_wear( ch, arg1 );
	    return;
	}
	if ( IS_SET( sheath->value[1], CONT_CLOSED ) )
	{
	    act( "$p is closed.", ch, sheath, NULL, TO_CHAR );
	    return;
	}
    }

    /*
     * Sanity check
     */
    if ( sheath == NULL )
    {
        bugf( "Do_wield: NULL sheath \"%s\"", arg2 );
	send_to_char( "I can't do that right now, sorry.\n\r", ch );
	return;
    }
    if ( weapon == NULL )
    {
        bugf( "Do_wield: NULL weapon \"%s\"", arg1 );
	send_to_char( "I can't do that right now, sorry.\n\r", ch );
	return;
    }

    get_obj( ch, weapon, sheath );
    if ( weapon->carried_by == ch )
	wear_obj( ch, weapon, TRUE );
//	do_wear( ch, arg1 );

    return;
}


void
do_donate( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    OBJ_DATA *		box;
    OBJ_DATA *		obj;
    ROOM_INDEX_DATA *	pRoom;
    int			vnum;

    if ( IS_NPC( ch ) )
    {
        send_to_char( "Mobiles cannot donate.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\n' )
    {
        send_to_char( "Donate what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

    /* now find the donation box, will have to tweak this later for clans */
    if ( ch->clan != NULL && ch->clan->box != 0 )
        vnum = ch->clan->box;
    else
        vnum = kingdom_table[ch->pcdata->kingdom].box;
    if ( vnum != 0 )
    {
        for ( box = object_list; box != NULL; box = box->next )
            if ( box->pIndexData->vnum == vnum )
                break;
    }
    else
        box = NULL;

    if ( box == NULL )
    {
        send_to_char( "You have no donation box.\n\r", ch );
        return;
    }

    pRoom = box->in_room;
    if ( !is_same_landmass( ch->in_room, pRoom ) )
    {
        send_to_char( "Your donation box is unavailable here.\n\r", ch );
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
        send_to_char( "You can't let go of it.\n\r", ch );
        return;
    }

    if ( get_obj_weight( obj ) + get_true_weight( box ) - box->weight > box->value[0]
    ||	 ( box->value[3] > 0 && get_obj_weight( obj ) > box->value[3] )
    ||  ( box->value[5] > 0
          && get_obj_count( box ) + get_obj_count( obj ) - 1 > box->value[5] ) )
    {
        send_to_char( "There's not enough room in the donation box.\n\r", ch );
        return;
    }

    act_color( AT_ACTION, "$n donates $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_ACTION, "You donate $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );

    if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
    {
        act_color( AT_ACTION, "$p dissolves into smoke.", ch, obj, NULL, TO_ROOM, POS_RESTING );
        act_color( AT_ACTION, "$p dissolves into smoke.", ch, obj, NULL, TO_CHAR, POS_RESTING );
        extract_obj( obj );
        return;
    }
    obj_from_char( obj );
    obj_to_obj( obj, box );
    act_color( AT_MAGIC, "Something falls into $p.", pRoom->people, box, NULL, TO_ALL, POS_RESTING );
    //save_donation_pit(obj, box );
    if ( obj->timer > 0 )
        SET_BIT( obj->extra_flags, ITEM_HAD_TIMER );
    else
        obj->timer = number_range( 660, 780 );

    return;
}


void
do_invoke( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *		obj;
    OBJ_DATA *		nObj;
    CHAR_DATA *		vch;
    MOB_INDEX_DATA *	pMobIndex;
    OBJ_INDEX_DATA *	pObjIndex;
    ROOM_INDEX_DATA *	loc;
    AFFECT_DATA		af;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL
	&& ( ( obj = get_obj_wear( ch, arg1 ) ) ) == NULL )
    {
        if ( ( obj = get_obj_list( ch, arg1, ch->in_room->contents ) ) != NULL )
        {
            if ( IS_SET( obj->wear_flags, ITEM_TAKE ) )
            {
                send_to_char( "You have to pick it up first.\n\r", ch );
                return;
            }
        }
        else
        {
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( ch->level < obj->level )
    {
	send_to_char( "You have not attained the level of mastery to use this item.", ch );
	act( "$n tries to use $p`X, but is too inexperienced.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( !can_use_obj( ch, obj, TRUE ) )
        return;

    if ( obj->ac_type <= INVOKE_NONE || obj->ac_type > INVOKE_SPELL )
    {
	act( "$p cannot be invoked.", ch, obj, NULL, TO_CHAR );
	return;
    }

    if ( obj->ac_type == INVOKE_SPELL && obj->ac_vnum < 1 )
    {
	act( "$p cannot be invoked.", ch, obj, NULL, TO_CHAR );
	return;
    }

    if ( !IS_NPC( ch ) )
    if (   ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL     ) && IS_EVIL   ( ch ) )
	|| ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD     ) && IS_GOOD   ( ch ) )
	|| ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL  ) && IS_NEUTRAL( ch ) ) )
    {
	act( "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
	act( "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	add_obj_fall_event( obj );
	return;
    }

    switch ( obj->ac_type )
    {
	default:   break;
	case INVOKE_OBJ:
	    act( "You invoke $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n invokes $p.", ch, obj, NULL, TO_ROOM );

	    if ( !( pObjIndex = get_obj_index( obj->ac_vnum ) ) )
	    {
		act( "$p whines and sparks, but nothing happens", ch, obj, NULL, TO_CHAR );
		break;
	    }
	    nObj = create_object( pObjIndex, pObjIndex->level );
	    if ( CAN_WEAR( nObj, ITEM_TAKE ) && obj->in_room == NULL )
	    {
		obj_to_char( nObj, ch );
	    }
	    else
	    {
		obj_to_room( nObj, ch->in_room );
		add_obj_fall_event( nObj );
	    }
	    act( "$p spawns $P.", ch, obj, nObj, TO_CHAR );
	    act( "$n's $p `Xspawns $P.", ch, obj, nObj, TO_ROOM );
	    oprog_invoke_trigger( obj, ch, nObj );
	    break;

	case INVOKE_MOB:
	    act( "You invoke $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n invokes $p.", ch, obj, NULL, TO_ROOM );

	    if ( !( pMobIndex = get_mob_index( obj->ac_vnum ) ) )
	    {
		act( "$p whines and sparks, but nothing happens", ch, obj, NULL, TO_CHAR );
		break;
	    }
	    vch = create_mobile( pMobIndex );
	    char_to_room( vch, ch->in_room );

	    act( "$p spawns $N.", ch, obj, vch, TO_CHAR );
	    act( "$n's $p spawns $N.", ch, obj, vch, TO_ROOM );
	    if ( vch->master )
		stop_follower( vch );
	    add_follower( vch, ch );
	    af.type	 = skill_lookup( "charm person" );
	    af.duration  = 50;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_CHARM;
	    affect_to_char( vch, &af );
	    oprog_invoke_trigger( obj, ch, vch );
	    break;

	case INVOKE_TRANS:
	    act( "You invoke $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n invokes $p.", ch, obj, NULL, TO_ROOM );

	    if ( !( loc = get_room_index( obj->ac_vnum ) ) )
	    {
		act( "$p whines and sparks, but nothing happens.", ch, obj, NULL, TO_CHAR );
		break;
	    }

	    if ( room_is_private( loc ) )
	    {
		send_to_char( "That room is private right now.\n\r", ch );
		break;
	    }

	    if ( ch->fighting )
	    {
		act( "$p pulses lightly, but fails to function.", ch, obj, NULL, TO_CHAR );
		break;
	    }
	    act( "Everything begins to spin, when it clears you are elsewhere.", ch, obj, NULL, TO_CHAR );
	    act( "$n invokes $p and vanishes in a swirling red mist.", ch, obj, NULL, TO_ROOM );
	    char_from_room( ch );
	    char_to_room( ch, loc );
	    act( "$n arrives in a swirling red mist.", ch, obj, NULL, TO_ROOM );
	    do_look( ch, "auto" );
	    oprog_invoke_trigger( obj, ch, ch );
	    break;

	case INVOKE_MORPH:
	    act( "You invoke $p.", ch, obj, NULL, TO_CHAR );
	    act(  "$n invokes $p.", ch, obj, NULL, TO_ROOM );

	    if ( !( pObjIndex = get_obj_index( obj->ac_vnum ) ) )
	    {
		act( "$p whines and sparks, but nothing happens", ch, obj, NULL, TO_CHAR );
		break;
	    }
	    nObj = create_object( pObjIndex, pObjIndex->level );
	    if ( CAN_WEAR( nObj, ITEM_TAKE ) && obj->in_room == NULL )
	    {
		obj_to_char( nObj, ch );
	    }
	    else
	    {
		obj_to_room( nObj, ch->in_room );
		add_obj_fall_event( nObj );
	    }

	    act( "$p's form wavers, then solidifies as $P.", ch, obj, nObj, TO_CHAR );
	    act( "$p wavers in form, then solidifies as $P.", ch, obj, nObj, TO_ROOM );
	    oprog_invoke_trigger( obj, ch, nObj );
	    extract_obj( obj );
	    break;

	case INVOKE_SPELL:
	    act( "You invoke $p.", ch, obj, NULL, TO_CHAR );
	    act(  "$n invokes $p.", ch, obj, NULL, TO_ROOM );
	    if ( obj->ac_vnum < 1 )
	    {
		act( "Nothing happens.", ch, NULL, NULL, TO_CHAR );
		act( "Nothing happens.", ch, NULL, NULL, TO_ROOM );
		break;
	    }
	    vch = NULL;
	    nObj = NULL;
	    if ( arg2[0] == '\0' )
		vch = ch;
	    else
	    {
		vch  = skill_table[obj->ac_vnum].target == TAR_IGNORE ?
		       get_char_world( ch, arg2 ) : get_char_room( ch, arg2 );
		nObj = skill_table[obj->ac_vnum].target == TAR_IGNORE ?
		       get_obj_world( ch, arg2 ) : get_obj_here( ch, arg2 );
	    }
	    if ( vch == NULL && nObj == NULL )
	    {
		send_to_char( "Nothing like that in heaven, hell or earth.\n\r", ch );
		break;
	    }
	    target_name = arg2;
	    if ( vch != NULL && IS_DEAD( vch ) )
	    {
		act( "Nothing happens.", ch, NULL, NULL, TO_CHAR );
		act( "Nothing happens.", ch, NULL, NULL, TO_ROOM );
		break;
	    }
	    oprog_invoke_trigger( obj, ch, NULL );
	    obj_cast_spell( obj->ac_vnum, obj->level, ch, vch, nObj );
	    break;
    }
    if ( obj->ac_charge[1] != -1 )
    if ( -- obj->ac_charge[0] <= 0 )
    {
	act( "Your $p sputters and sparks.", ch, obj, NULL, TO_CHAR );
	act( "$n's $p sputters and sparks..", ch, obj, NULL, TO_ROOM );
	obj->ac_type = 0;
	obj->ac_vnum = 0;
    }

}


void
do_rename( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    OBJ_DATA *	obj;
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    int		price;
    const char *oname;
    MONEY	amt;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobiles can't do that.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( IS_IMMORTAL( ch ) )
    {
	do_rename_obj( ch, argument );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rename what?\n\r", ch );
	return;
    }

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	if ( IS_NPC( vch ) && IS_SET( vch->act, ACT_ARTIFICER ) )
	    break;

    if ( vch == NULL )
    {
	send_to_char( "There is no artificer here.\n\r", ch );
	return;
    }

    if ( !can_see( vch, ch ) )
    {
	do_say( vch, "I don't deal with people I can't see!\n\r" );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	send_to_char( "You don't have that item.\n\r", ch );
	return;
    }

    if ( obj->level > ch->level )
    {
	sprintf( buf, "%s You are of too low a level to do that.", ch->name );
	do_sayto( vch, buf );
	return;
    }

    /*if ( IS_SET( obj->extra_flags, ITEM_NORENAME ) )
    {
	sprintf( buf, "%s I'm sorry but I cannot help you with that item.", ch->name );
	do_sayto( vch, buf );
	return;
    }*/

    price = UMAX( obj->level * FRACT_PER_COPPER, obj->cost / (2 * FRACT_PER_COPPER ) );
    price = UMAX( FRACT_PER_COPPER, price );
    amt.gold = 0;
    amt.silver = 0;
    amt.copper = 0;
    amt.fract = price;
    normalize( &amt );
    sprintf( buf, "%s That'll cost you%s.", ch->name, money_string( &amt, FALSE, FALSE ) );
    do_sayto( vch, buf );

    if ( CASH_VALUE( ch->money ) < price )
    {
	sprintf( buf, "%s You can't afford it.", ch->name );
	do_sayto( vch, buf );
	return;
    }

    deduct_cost( ch, price );

    act_color( AT_ACTION, "$N waves a glowing rod over your $t`X.", ch,
	       aoran_skip( obj->short_descr ), vch, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$N waves a glowing rod over $n's $t`X.", ch,
	       aoran_skip( obj->short_descr ), vch, TO_NOTVICT, POS_RESTING );
    sprintf( buf, "%s Now visualize what you want your %s`X to become.",
	     ch->name, aoran_skip( obj->short_descr ) );
    do_sayto( vch, buf );
    oname = aoran_skip( obj->short_descr );
    act_color( AT_ACTION, "The $t`X begins to flow and reshape in your hands.", ch, oname, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "The $t`X begins to flow and reshape in $n's hands.", ch, oname, NULL, TO_ROOM, POS_RESTING );

    do_rename_obj( ch, argument );
}


void
do_search( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	pObj;
    OBJ_DATA *	obj_next;
    int		chance;

    for ( pObj = ch->in_room->contents; pObj != NULL; pObj = obj_next )
    {
	obj_next = pObj->next_content;
	if ( !IS_SET( pObj->extra_flags, ITEM_HIDDEN ) )
	    continue;
	if ( pObj->level > ch->level )
	    continue;

	chance = 10 + ch->level - pObj->level;
	if ( ch->class == class_rogue )
	    chance += 10;
	if ( IS_NPC( ch ) || ( IS_IMMORTAL( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
	    chance = 200;
	if ( chance < number_percent( ) )
	    continue;

	REMOVE_BIT( pObj->extra_flags, ITEM_HIDDEN );
	if ( !can_see_obj( ch, pObj ) )
	{
	    SET_BIT( pObj->extra_flags, ITEM_HIDDEN );
	    continue;
	}

	act_color( AT_ACTION, "You reveal $p.", ch, pObj, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n reveals $p.", ch, pObj, NULL, TO_ROOM, POS_RESTING );
	return;
    }

    act_color( AT_ACTION, "You search and find nothing.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n searches for something.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    return;
}


void
do_stare ( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	lens;
    OBJ_DATA *	obj;
    CHAR_DATA *	victim;
    char	arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' && !ch->fighting )
    {
	send_to_char( "Stare at whom or what?\n\r", ch );
	return;
    }

    if ( !( lens = get_eq_char( ch, WEAR_EYES ) ) )
    {
	send_to_char( "You have no lenses in your eyes.\n\r", ch );
	return;
    }

    if ( lens->item_type != ITEM_LENS )
    {
	send_to_char( "You can only stare with magical lenses.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting )
	{
	    victim = ch->fighting;
	}
	else
	{
	    send_to_char( "Stare at whom or what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( !( victim = get_char_room ( ch, arg ) )
	    && !( obj  = get_obj_here  ( ch, arg ) ) )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( !IS_IMMORTAL( ch ) )
	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( ( lens->value[2] > 0 ) || ( lens->value[1] == -1 ) )
    {
	if ( victim )
	{
	    act_color( AT_ACTION, "You stare at $N with $p.", ch, lens, victim, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n stares at $N with $p.", ch, lens, victim, TO_ROOM, POS_RESTING );
	}
	else
	{
	    act_color( AT_ACTION, "You stare at $P`X with $p.", ch, lens, obj, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n stares at $P`X with $p.", ch, lens, obj, TO_ROOM, POS_RESTING );
	}

	obj_cast_spell( lens->value[3], lens->level, ch, victim, obj );

	if ( victim )
	    oprog_use_trigger( lens, ch, victim );
	else
	    oprog_use_trigger( lens, ch, obj );
    }

    if ( lens->value[1] != -1 && --lens->value[2] <= 0 )
    {
	act( "Your $p`X melts in your eyes.", ch, lens, NULL, TO_CHAR );
	act( "$n's $p`X melts in $s eyes.", ch, lens, NULL, TO_ROOM );
	extract_obj( lens );
    }

    return;
}


bool
add_obj_fall_event( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	to_room;
    EXIT_DATA *		pExit;
    EVENT_DATA *	evNew;

    if ( ( in_room = obj->in_room ) == NULL )
        return FALSE;

    if ( in_room->sector_type != SECT_AIR && !IS_SET( in_room->room_flags, ROOM_NO_FLOOR ) )
        return FALSE;

    if ( ( pExit = in_room->exit[DIR_DOWN] ) == NULL || IS_SET( pExit->exit_info, EX_CLOSED ) )
        return FALSE;

    if ( ( to_room = pExit->to_room ) == NULL )
        return FALSE;

    evNew	  = new_event( );
    evNew->fun 	  = event_obj_fall;
    evNew->type   = EVENT_OBJ_FALL;
    evNew->value0 = 0;
    add_event_obj( evNew, obj, 1 );

    return TRUE;
}


bool
event_obj_fall( EVENT_DATA *event )
{
    OBJ_DATA *		obj;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	to_room;
    EXIT_DATA *		pExit;
    CHAR_DATA *		ch;
    EVENT_DATA *	evNew;

    obj = event->owner.obj;

    if ( ( in_room = obj->in_room ) == NULL )
        return FALSE;

    if ( event->value0 > MAX_FALL )
    {
        extract_obj( obj );
        return TRUE;
    }

    if ( in_room->sector_type != SECT_AIR && !IS_SET( in_room->room_flags, ROOM_NO_FLOOR ) )
        return FALSE;

    if ( ( pExit = in_room->exit[DIR_DOWN] ) == NULL || IS_SET( pExit->exit_info, EX_CLOSED ) )
        return FALSE;

    if ( ( to_room = pExit->to_room ) == NULL )
        return FALSE;

    if ( ( ch = in_room->people ) != NULL )
    {
        act_color( AT_ACTION, "$p falls away below.", ch, obj, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$p falls away below.", ch, obj, NULL, TO_ROOM, POS_RESTING );
    }

    obj_from_room( obj );
    obj_to_room( obj, to_room );

    if ( ( ch = to_room->people ) != NULL )
    {
        act_color( AT_ACTION, "$p falls down from above.", ch, obj, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$p falls down from above.", ch, obj, NULL, TO_ROOM, POS_RESTING );
    }

    evNew = new_event( );
    evNew->fun = event_obj_fall;
    evNew->type = EVENT_OBJ_FALL;
    evNew->value0 = event->value0 + 1;
    add_event_obj( evNew, obj, 1 );

    return FALSE;
}


static bool
steal_mount( CHAR_DATA *ch, char *arg1, char *arg2 )
{
    CHAR_DATA *	owner;
    CHAR_DATA *	rider;
    CHAR_DATA *	victim;
    int		chance;

    if ( IS_NULLSTR( arg1 ) || !IS_NULLSTR( arg2 ) )
        return FALSE;

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
        return FALSE;

    if ( victim == ch )
    {
        send_to_char( "Steal yourself??\n\r", ch );
        return TRUE;
    }

    if ( !IS_NPC( victim ) || !IS_SET( victim->act, ACT_MOUNT ) )
    {
        act( "You can't steal $M.", ch, NULL, victim, TO_CHAR );
        return TRUE;
    }

    if ( ch->mount != NULL )
    {
        send_to_char( "You already have a mount.\n\r", ch );
        return TRUE;
    }

    if ( victim->mount == ch )
    {
        act( "$N is already yours.", ch, NULL, victim, TO_CHAR );
        return TRUE;
    }

    if ( ( rider = RIDDEN( victim ) ) != NULL )
    {
        act( "$S rider won't let you.", ch, NULL, victim, TO_CHAR );
        act_color( AT_ACTION, "$n tried to steal your mount!", ch, NULL,
                   rider, TO_VICT, POS_RESTING );
        act_color( AT_ACTION, "$n tried to steal $N's mount!", ch, NULL,
                   rider, TO_NOTVICT, POS_RESTING );
        return TRUE;
    }

    if ( victim->level > ch->level )
    {
        send_to_char( "You're not powerful enough to master that mount.\n\r", ch );
        return TRUE;
    }

    if ( ch->size >= victim->size )
    {
        act( "You're too big to ride $M.", ch, NULL, victim, TO_CHAR );
        return TRUE;
    }

    /*
     * The following three checks are from is_safe().
     * I can't just call is_safe() because of the level checks.
     */
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

    /* safe room? */
    if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
        send_to_char( "Not in this room.\n\r", ch );
        return TRUE;
    }

    if ( !IS_AWAKE( victim ) )
    {
        do_wake( ch, victim->name );
        if ( !IS_AWAKE( victim ) || victim->in_room != ch->in_room )
            return TRUE;
    }

    if ( !can_see( victim, ch ) )
    {
        act( "$N doesn't trust people $E can't see.", ch, NULL, victim, TO_CHAR );
        return TRUE;
    }

    WAIT_STATE( ch, skill_table[gsn_steal].beats );
    chance = URANGE( -5, number_percent( ) - 25, 75 );

    if ( chance > get_skill( ch, gsn_steal ) )
    {
        /* Failure */
        act_color( AT_ACTION, "You fail to steal $N.", ch, NULL, victim, TO_CHAR, POS_RESTING );
        act_color( AT_ACTION, "$n tries to steal you!.", ch, NULL, victim, TO_VICT, POS_RESTING );
        act_color( AT_ACTION, "$n tries to steal $N!", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
        check_improve( ch, gsn_steal, FALSE, 2 );
        return TRUE;
    }

    /* Success */
    if ( ( owner = victim->mount ) != NULL )
    {
        if ( owner->in_room != ch->in_room )
            act_color( AT_ACTION, "$N has stolen your mount!", owner, NULL,
                       ch, TO_CHAR, POS_RESTING );
        if ( victim->master != NULL )
            stop_follower( victim );
        owner->mount = NULL;
    }

    add_follower( victim, ch );
    victim->mount = ch;
    ch->mount = victim;

    act_color( AT_ACTION, "You steal $N!",  ch, NULL, victim, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n steals you!", ch, NULL, victim, TO_VICT, POS_RESTING );
    act_color( AT_ACTION, "$n steals $N!",  ch, NULL, victim, TO_NOTVICT, POS_RESTING );

    check_improve( ch, gsn_steal, TRUE, 2 );
    return TRUE;
}

void do_lore( CHAR_DATA *ch, char *argument )
{
  char object_name[MAX_INPUT_LENGTH + 100];

  OBJ_DATA *obj;
  //char buf[MAX_STRING_LENGTH];
  //AFFECT_DATA *paf;
  argument = one_argument(argument, object_name);
  if ( ( obj = get_obj_carry( ch, object_name, ch ) ) == NULL )
    {
      send_to_char( "You are not carrying that.\n\r", ch );
      return;
    }

  send_to_char("You ponder the item.\n\r",ch);
  if (number_percent() < get_skill(ch,gsn_lore) &&
      ch->level >= obj->level){

    ch_printf(ch,
	     "Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",

	     obj->name,
	     item_name(obj->item_type),
	     extra_bit_name( obj->extra_flags ),
	     obj->weight / 10,
	     obj->cost,
	     obj->level
	     );

    switch ( obj->item_type )
      {
      case ITEM_SCROLL: 
      case ITEM_POTION:
      case ITEM_PILL:
	ch_printf(ch, "Some level spells of:");

	if ( obj->value[1] >= 0 && obj->value[1] < top_skill )
	  {
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'", ch );
	  }

	if ( obj->value[2] >= 0 && obj->value[2] < top_skill )
	  {
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'", ch );
	  }

	if ( obj->value[3] >= 0 && obj->value[3] < top_skill )
	  {
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	  }

	if (obj->value[4] >= 0 && obj->value[4] < top_skill)
	  {
	    send_to_char(" '",ch);
	    send_to_char(skill_table[obj->value[4]].name,ch);
	    send_to_char("'",ch);
	  }

	send_to_char( ".\n\r", ch );
	break;

      case ITEM_WAND: 
      case ITEM_STAFF: 
	ch_printf(ch, "Has some charges of some level" );
      
	if ( obj->value[3] >= 0 && obj->value[3] < top_skill )
	  {
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	  }

	send_to_char( ".\n\r", ch );
	break;

      case ITEM_DRINK_CON:
        ch_printf(ch,"It holds %s-colored %s.\n\r",
		liq_table[obj->value[2]].liq_color,
		liq_table[obj->value[2]].liq_name);
        break;

      case ITEM_CONTAINER:
	ch_printf(ch,"Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
		obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
	if (obj->value[4] != 100)
	  {
	    ch_printf(ch,"Weight multiplier: %d%%\n\r",
		    obj->value[4]);
	  }
	break;
		
      case ITEM_WEAPON:
 	send_to_char("Weapon type is ",ch);
	switch (obj->value[0])
	  {
	  case(WEAPON_EXOTIC) : send_to_char("exotic.\n\r",ch);	break;
	  case(WEAPON_SWORD)  : send_to_char("sword.\n\r",ch);	break;	
	  case(WEAPON_DAGGER) : send_to_char("dagger.\n\r",ch);	break;
	  case(WEAPON_SPEAR)	: send_to_char("spear/staff.\n\r",ch);	break;
	  case(WEAPON_MACE) 	: send_to_char("mace/club.\n\r",ch);	break;
	  case(WEAPON_AXE)	: send_to_char("axe.\n\r",ch);		break;
	  case(WEAPON_FLAIL)	: send_to_char("flail.\n\r",ch);	break;
	  case(WEAPON_WHIP)	: send_to_char("whip.\n\r",ch);		break;
	  case(WEAPON_POLEARM): send_to_char("polearm.\n\r",ch);	break;
	  default		: send_to_char("unknown.\n\r",ch);	break;
	  }
	if (obj->pIndexData->new_format)
	  ch_printf(ch,"Damage is %dd%d (average %d).\n\r",
		  obj->value[1],obj->value[2],
		  (1 + obj->value[2]) * obj->value[1] / 2);
	else
	  ch_printf(ch, "Damage is %d to %d (average %d).\n\r",
		   obj->value[1], obj->value[2],
		   ( obj->value[1] + obj->value[2] ) / 2 );
        if (obj->value[4])  /* weapon flags */
	  {
            ch_printf(ch,"Weapons flags: %s\n\r",weapon_bit_name(obj->value[4]));
	  }
	break;

      case ITEM_ARMOR:
	ch_printf(ch, 
		 "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r", 
		 obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	break;
      }
  }
  return;
}


void do_study( CHAR_DATA *ch, char *argument ) /* study by Absalom */
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int sn = 0;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Study what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_STAFF && obj->item_type != ITEM_WAND &&
	obj->item_type != ITEM_SCROLL )
    {
	send_to_char( "You can only study scrolls, wands, and staves.\n\r", ch );
	return;
    }

    act( "$n studies $p.", ch, obj, NULL, TO_ROOM );
    act( "You study $p.", ch, obj, NULL, TO_CHAR );

    if (ch->level < obj->level)
	{
	send_to_char("You cannot glean any knowledge from it.\n\r",ch);
	act( "$p burns brightly and is gone.", ch, obj, NULL, TO_CHAR );
	extract_obj( obj );
	}

    if (obj->item_type == ITEM_STAFF)
	{
	sn = obj->value[3];
	if ( sn < 0 || sn >= top_skill || skill_table[sn].spell_fun == 0 )
	    {
	    bug( "Do_study: bad sn %d.", sn );
	    return;
	    }
	if ( number_percent() >= 20 + get_skill(ch,gsn_staves) * 4/5)
	    {
	    send_to_char("You cannot glean any knowledge from it.\n\r",ch);
	    check_improve(ch,gsn_staves,FALSE,2);
	    act( "$p burns brightly and is gone.", ch, obj, NULL, TO_CHAR );
	    extract_obj( obj );
	    return;
	    }
	if ( ch->pcdata->skill[sn].percent)
	    {
	    send_to_char("You already know that spell!\n\r",ch);
	    return;
	    }
	ch->pcdata->skill[sn].percent = 1;
	act("You have learned the art of $t!",
	ch,skill_table[sn].name,NULL,TO_CHAR);
	check_improve(ch,gsn_staves,TRUE,2);
	act( "$p burns brightly and is gone.", ch, obj, NULL, TO_CHAR );
	extract_obj( obj );
	return;
	}

    if (obj->item_type == ITEM_WAND)
	{
	sn = obj->value[3];
	if ( sn < 0 || sn >= top_skill || skill_table[sn].spell_fun == 0 )
	    {
	    bug( "Do_study: bad sn %d.", sn );
	    return;
	    }
	if ( number_percent() >= 20 + get_skill(ch,gsn_wands) * 4/5)
	    {
	    send_to_char("You cannot glean any knowledge from it.\n\r",ch);
	    check_improve(ch,gsn_wands,FALSE,2);
	    act( "$p burns brightly and is gone.", ch, obj, NULL, TO_CHAR );
	    extract_obj( obj );
	    return;
	    }
	if ( ch->pcdata->skill[sn].percent)
	    {
	    send_to_char("You already know that spell!\n\r",ch);
	    return;
	    }
	ch->pcdata->skill[sn].percent = 1;
	act("You have learned the art of $t!",
	ch,skill_table[sn].name,NULL,TO_CHAR);
	check_improve(ch,gsn_wands,TRUE,2);
	act( "$p burns brightly and is gone.", ch, obj, NULL, TO_CHAR );
	extract_obj( obj );
	return;
	}

    if (obj->item_type == ITEM_SCROLL)
	{
	sn = obj->value[1];
	if ( sn < 0 || sn >= top_skill || skill_table[sn].spell_fun == 0 )
	    {
	    bug( "Do_study: bad sn %d.", sn );
	    return;
	    }
	if ( number_percent() >= 20 + get_skill(ch,gsn_scrolls) * 4/5)
	    {
	    send_to_char("You cannot glean any knowledge from it.\n\r",ch);
	    check_improve(ch,gsn_scrolls,FALSE,2);
	    act( "$p burns brightly and is gone.", ch, obj, NULL, TO_CHAR );
	    extract_obj( obj );
	    return;
	    }
	if ( ch->pcdata->skill[sn].percent)
	    {
	    send_to_char("You already know that spell!\n\r",ch);
	    return;
	    }
	ch->pcdata->skill[sn].percent = 1;
	act("You have learned the art of $t!",
	ch,skill_table[sn].name,NULL,TO_CHAR);
	check_improve(ch,gsn_scrolls,TRUE,2);
	act( "$p burns brightly and is gone.", ch, obj, NULL, TO_CHAR );
	extract_obj( obj );
	return;
	}

}

void do_sharpen( CHAR_DATA *ch, char *argument)
{

  OBJ_DATA *obj;
  AFFECT_DATA af;
  int percent,skill;
char arg[MAX_INPUT_LENGTH];
argument = one_argument(argument, arg);

    if (argument == '\0')
    {
      send_to_char("Sharpen what weapon?\n\r",ch);
      return;
    }

{
/*   obj = get_obj_list(ch, argument, ch->carrying); */
   
   if ((obj = get_obj_carry( ch, arg, ch )) == NULL )
   {
     send_to_char("You do not have that item.\n\r",ch);
     return;
   }

   if ((skill = get_skill(ch, gsn_sharpen)) <1)
   {
     send_to_char("Sharpen, whats that?\n\r",ch);
     return;
   }
  
   if (obj->item_type == ITEM_WEAPON)
   {
        if (IS_WEAPON_STAT(obj,WEAPON_SHARP))
        {
           act("$p has already been sharpened.",ch,obj,NULL,TO_CHAR);
           return;
        }

        percent = number_percent();
        if (percent < skill)
        {
          af.where     = TO_WEAPON;
          af.type      = gsn_sharpen;
          af.level     = ch->level * percent / 100;
          af.duration  = -1;
          af.location  = 0;
          af.modifier  = 0;
          af.bitvector = WEAPON_SHARP;
          affect_to_obj(obj,&af);

 act("$n pulls out a stone and begins sharpening $p.",ch,obj,NULL,TO_ROOM);   
 act("You sharpen $p.",ch,obj,NULL,TO_CHAR);
 check_improve(ch,gsn_sharpen,TRUE,3);
 WAIT_STATE(ch,skill_table[gsn_sharpen].beats);
 return;

   }

}

send_to_char("You can only sharpen weapons.\n\r",ch);
return;
}
}
         
void do_butcher(CHAR_DATA *ch, char *argument)
{

    /* Butcher skill, created by Argawal */
    /* Original Idea taken fom Carrion Fields Mud */
    /* If you have an interest in this skill, feel free */
    /* to use it in your mud if you so desire. */
    /* All I ask is that Argawal is credited with creating */
    /* this skill, as I wrote it from scratch. */

    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int numst = 0;
    OBJ_DATA *steak;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if(get_skill(ch,gsn_butcher)==0)
    {
       send_to_char("Butchering is beyond your skills.\n\r",ch);
       return;
    }

    if(arg[0]=='\0')
    {
       send_to_char("Butcher what?\n\r",ch);
       return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents ); 
    if ( obj == NULL )
    {
        send_to_char( "It's not here.\n\r", ch ); 
        return; 
    }

    if( (obj->item_type != ITEM_CORPSE_NPC)
        && (obj->item_type!=ITEM_CORPSE_PC) )
    {
        send_to_char( "You can only butcher corpses.\n\r", ch ); 
        return; 
    }

    /* create and rename the steak */
    buf[0]='\0';
    strcat(buf,"A steak of ");
    strcat(buf,str_dup(obj->short_descr));
    strcat(buf," is here.");

    steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 

    steak->description=str_dup(buf);
    steak->value[0] = ch->level / 2;
    steak->value[1] = ch->level;

    buf[0]='\0';
    strcat(buf,"A steak of ");
    strcat(buf,str_dup(obj->short_descr));

    steak->short_descr=str_dup(buf);

    /* Check the skill roll, and put a random ammount of steaks here. */

    if(number_percent( ) < get_skill(ch,gsn_butcher))
    { 
       numst = dice(1,4);
       switch(numst)
       {
       case 1:
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
         obj_to_room( steak, ch->in_room );
         act( "$n butchers a corpse and creates a steak.", ch, steak, NULL, TO_ROOM );
         act( "You butcher a corpse and create a steak.", ch, steak, NULL, TO_CHAR );
         break;
       case 2: 
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         act( "$n butchers a corpse and creates two steaks.", ch, steak, NULL, TO_ROOM );
         act( "You butcher a corpse and create two steaks.", ch, steak, NULL, TO_CHAR );
         break; 
       case 3:
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         act( "$n butchers a corpse and creates three steaks.", ch, steak, NULL, TO_ROOM );
         act( "You butcher a corpse and create three steaks.", ch, steak, NULL, TO_CHAR );
         break;
       case 4:
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 ); 
         obj_to_room( steak, ch->in_room );
         act( "$n butchers a corpse and creates four steaks.", ch, steak, NULL, TO_ROOM );
         act( "You butcher a corpse and create four steaks.", ch, steak, NULL, TO_CHAR );
         break;
      } 
      check_improve(ch,gsn_butcher,TRUE,1);

    }   
    else
    {
       act( "$n fails to butcher a corpse, and destroys it.", ch, steak, NULL, TO_ROOM );
       act( "You fail to butcher a corpse, and destroy it.", ch, steak, NULL, TO_CHAR );
       check_improve(ch,gsn_butcher,FALSE,1);
    } 
    /* dump items caried */
    /* Taken from the original ROM code and added into here. */

    if ( obj->item_type == ITEM_CORPSE_PC )
    {   /* save the contents */ 
       {

            OBJ_DATA *t_obj, *next_obj; 
            for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
            {
                next_obj = t_obj->next_content; 
                obj_from_obj(t_obj); 
                if (obj->in_obj) /* in another object */
                    obj_to_obj(t_obj,obj->in_obj); 
                else if (obj->carried_by) /* carried */
                    if (obj->wear_loc == WEAR_FLOAT)
                        if (obj->carried_by->in_room == NULL)
                            extract_obj(t_obj); 
                        else
                            obj_to_room(t_obj,obj->carried_by->in_room); 
                    else
                        obj_to_char(t_obj,obj->carried_by); 
               else if (obj->in_room == NULL) /* destroy it */
                    extract_obj(t_obj); 
                else /* to a room */
                    obj_to_room(t_obj,obj->in_room); 
           }
      }

  }

    if ( obj->item_type == ITEM_CORPSE_NPC )
    {
       {
            OBJ_DATA *t_obj, *next_obj; 
            for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
            {
                next_obj = t_obj->next_content; 
                obj_from_obj(t_obj); 
                if (obj->in_obj) /* in another object */
                    obj_to_obj(t_obj,obj->in_obj); 
                else if (obj->carried_by) /* carried */
                    if (obj->wear_loc == WEAR_FLOAT)
                        if (obj->carried_by->in_room == NULL)
                            extract_obj(t_obj); 
                        else
                            obj_to_room(t_obj,obj->carried_by->in_room); 
                    else
                        obj_to_char(t_obj,obj->carried_by); 
                else if (obj->in_room == NULL) /* destroy it */
                    extract_obj(t_obj); 
                else /* to a room */
                    obj_to_room(t_obj,obj->in_room); 
         }
     }
  }

    /* Now remove the corpse */
    extract_obj(obj);
    return;
}

void do_skin(CHAR_DATA *ch, char *argument)
{

    /* Butcher skill, created by Argawal */
    /* Original Idea taken fom Carrion Fields Mud */
    /* If you have an interest in this skill, feel free */
    /* to use it in your mud if you so desire. */
    /* All I ask is that Argawal is credited with creating */
    /* this skill, as I wrote it from scratch. */

    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int numst = 0;
    OBJ_DATA *steak;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if(get_skill(ch,gsn_skin)==0)
    {
       send_to_char("Skinning is beyond your skills.\n\r",ch);
       return;
    }

    if(arg[0]=='\0')
    {
       send_to_char("Skin what?\n\r",ch);
       return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents ); 
    if ( obj == NULL )
    {
        send_to_char( "It's not here.\n\r", ch ); 
        return; 
    }

    if( (obj->item_type != ITEM_CORPSE_NPC)
        && (obj->item_type!=ITEM_CORPSE_PC) )
    {
        send_to_char( "You can only skin corpses.\n\r", ch ); 
        return; 
    }

    /* create and rename the steak */
    buf[0]='\0';
    strcat(buf,"A skin of ");
    strcat(buf,str_dup(obj->short_descr));
    strcat(buf," is here.");

    steak = create_object( get_obj_index( OBJ_VNUM_SKIN ), 0 ); 

    steak->description=str_dup(buf);
    steak->value[0] = ch->level / 2;
    steak->value[1] = ch->level;

    buf[0]='\0';
    strcat(buf,"A skin of ");
    strcat(buf,str_dup(obj->short_descr));

    steak->short_descr=str_dup(buf);

    /* Check the skill roll, and put a random ammount of steaks here. */

    if(number_percent( ) < get_skill(ch,gsn_skin))
    { 
       numst = dice(1,4);
       switch(numst)
       {
       case 1:
         steak = create_object( get_obj_index( OBJ_VNUM_SKIN ), 0 );
         obj_to_room( steak, ch->in_room );
         act( "$n skins a corpse and creates a hide.", ch, steak, NULL, TO_ROOM );
         act( "You skin a corpse and create a hide.", ch, steak, NULL, TO_CHAR );
         break;
       case 2: 
         steak = create_object( get_obj_index( OBJ_VNUM_SKIN ), 0 ); 
         obj_to_room( steak, ch->in_room );
         act( "$n skins a corpse and creates a hide.", ch, steak, NULL, TO_ROOM );
         act( "You skin a corpse and creates a hide.", ch, steak, NULL, TO_CHAR );
         break; 
       case 3:
         steak = create_object( get_obj_index( OBJ_VNUM_SKIN ), 0 ); 
         obj_to_room( steak, ch->in_room );
         act( "$n skins a corpse and creates a hide.", ch, steak, NULL, TO_ROOM );
         act( "You skin a corpse and create a hide.", ch, steak, NULL, TO_CHAR );
         break;
       case 4:
         steak = create_object( get_obj_index( OBJ_VNUM_SKIN ), 0 ); 
         obj_to_room( steak, ch->in_room );
         act( "$n skins a corpse and creates a hide.", ch, steak, NULL, TO_ROOM );
         act( "You skin a corpse and create a hide.", ch, steak, NULL, TO_CHAR );
         break;
      } 
      check_improve(ch,gsn_skin,TRUE,1);

    }   
    else
    {
       act( "$n fails to skin a corpse, and destroys it.", ch, steak, NULL, TO_ROOM );
       act( "You fail to skin a corpse, and destroy it.", ch, steak, NULL, TO_CHAR );
       check_improve(ch,gsn_skin,FALSE,1);
    } 
    /* dump items caried */
    /* Taken from the original ROM code and added into here. */

    if ( obj->item_type == ITEM_CORPSE_PC )
    {   /* save the contents */ 
       {

            OBJ_DATA *t_obj, *next_obj; 
            for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
            {
                next_obj = t_obj->next_content; 
                obj_from_obj(t_obj); 
                if (obj->in_obj) /* in another object */
                    obj_to_obj(t_obj,obj->in_obj); 
                else if (obj->carried_by) /* carried */
                    if (obj->wear_loc == WEAR_FLOAT)
                        if (obj->carried_by->in_room == NULL)
                            extract_obj(t_obj); 
                        else
                            obj_to_room(t_obj,obj->carried_by->in_room); 
                    else
                        obj_to_char(t_obj,obj->carried_by); 
               else if (obj->in_room == NULL) /* destroy it */
                    extract_obj(t_obj); 
                else /* to a room */
                    obj_to_room(t_obj,obj->in_room); 
           }
      }

  }

    if ( obj->item_type == ITEM_CORPSE_NPC )
    {
       {
            OBJ_DATA *t_obj, *next_obj; 
            for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
            {
                next_obj = t_obj->next_content; 
                obj_from_obj(t_obj); 
                if (obj->in_obj) /* in another object */
                    obj_to_obj(t_obj,obj->in_obj); 
                else if (obj->carried_by) /* carried */
                    if (obj->wear_loc == WEAR_FLOAT)
                        if (obj->carried_by->in_room == NULL)
                            extract_obj(t_obj); 
                        else
                            obj_to_room(t_obj,obj->carried_by->in_room); 
                    else
                        obj_to_char(t_obj,obj->carried_by); 
                else if (obj->in_room == NULL) /* destroy it */
                    extract_obj(t_obj); 
                else /* to a room */
                    obj_to_room(t_obj,obj->in_room); 
         }
     }
  }

    /* Now remove the corpse */
    extract_obj(obj);
    return;
}



void
do_empty(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *container, *into, *temp_obj, *temp_next;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];

  /* First, check to ensure they can dump.  */
  argument = one_argument( argument, arg1 );

  if (arg1[0] == '\0')
    {
      send_to_char("What do you want to empty out?\n\r",ch);
      return;
    }

  if ((container = get_obj_carry(ch, arg1, ch)) == NULL)
    {
      act("You don't have a $T.", ch, NULL, arg1, TO_CHAR);
      return;
    }

  if (container->item_type != ITEM_CONTAINER)
    {
      send_to_char("You can't empty that.\n\r",ch);
      return;
    }

  if (IS_SET(container->value[1], CONT_CLOSED))
    {
      act("$d is closed.", ch, NULL, container->short_descr, TO_CHAR);
      return;
    }

  /* Next, check to see if they want to dump into another container.  */
  argument = one_argument( argument, arg2 );

  if (!str_cmp(arg2,"in") || !str_cmp(arg2,"into") || !str_cmp(arg2,"on"))
    argument = one_argument(argument,arg2);

  if (arg2[0] != '\0')
    {
      /* Prefer obj in inventory other than object in room.  */
      if ((into = get_obj_carry(ch, arg2, ch)) == NULL)
        {
          if ((into = get_obj_here(ch, arg2)) == NULL)
            {
              send_to_char("You don't see that here.\n\r",ch);
              return;
            }
        }

      if (into->item_type != ITEM_CONTAINER)
        {
          send_to_char("You can't empty into that.\n\r",ch);
          return;
        }

      if (IS_SET(into->value[1], CONT_CLOSED))
        {
          act("$d is closed.", ch, NULL, into->short_descr, TO_CHAR);
          return;
        }

      act("You empty out the contents of $p into $P.",ch,container,into,TO_CHAR);
      act("$n empties out the contents of $p into $P.",ch,container,into,TO_ROOM);

      for (temp_obj=container->contains; temp_obj != NULL; temp_obj=temp_next)
        {
          temp_next = temp_obj->next_content;

          if ((get_obj_weight(temp_obj) + get_true_weight(into)
                > (into->value[0] * 10))
              ||  (get_obj_weight(temp_obj) > (into->value[3] * 10)))
            act("$P won't fit into $p.",ch,into,temp_obj,TO_CHAR);

          else 
            {
              obj_from_obj(temp_obj);
              obj_to_obj(temp_obj, into);
            }
        }
    }

  /* Dumping to the floor.  */
  else
    {
      act("You empty out the contents of $p.",ch,container,NULL,TO_CHAR);
      act("$n empties out the contents of $p.",ch,container,NULL,TO_ROOM);

      for (temp_obj=container->contains; temp_obj != NULL; temp_obj=temp_next)
        {
          temp_next = temp_obj->next_content;
          act("  ... $p falls out onto the ground.",ch,temp_obj,NULL,TO_CHAR);
          act("  ... $p falls out onto the ground.",ch,temp_obj,NULL,TO_ROOM);
  
          obj_from_obj(temp_obj);
          obj_to_room(temp_obj,ch->in_room);
  
          if (IS_OBJ_STAT(temp_obj,ITEM_MELT_DROP))
            {
              act("$p dissolves into smoke.",ch,temp_obj,NULL,TO_ROOM);
              act("$p dissolves into smoke.",ch,temp_obj,NULL,TO_CHAR);
              extract_obj(temp_obj);
            }
        }
    }
}

void do_purify (CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;
  int skill;

  /* find out what */
    if (argument[0] == '\0')
    {
        send_to_char ("Purify what food or drink?\n\r", ch);
        return;
    }

    obj = get_obj_list (ch, argument, ch->carrying);

    if (obj == NULL)
    {
        send_to_char ("You don't have that item.\n\r", ch);
        return;
    }

    if ((skill = get_skill (ch, gsn_purify)) < 1)
    {
        send_to_char("Thats an issue you may wish to take up with your patron.\n\r", ch);
        return;
    }

    if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
    {

        if (number_percent () < skill)
        {
          act ("$n calls upon $s deity to purify $p.", ch, obj, NULL, TO_ROOM);
          act ("You call upon your deity to purify $p.", ch, obj, NULL, TO_CHAR);
          obj->value[3] = 0;
          check_improve (ch, gsn_purify, TRUE, 4);
          WAIT_STATE (ch, skill_table[gsn_purify].beats);
          ch->mana -= 10;
          return;

        } else {

          act ("You fail to purify $p.", ch, obj, NULL, TO_CHAR);
          check_improve (ch, gsn_purify, FALSE, 3);
          WAIT_STATE (ch, skill_table[gsn_purify].beats);
          ch->mana -= 10;
          return;
        }
    }
}
