/****************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com    *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this   *
*  code is allowed provided you add a credit line to the effect of:         *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest     *
*  of the standard diku/rom credits. If you use this or a modified version  *
*  of this code, let me know via email: moongate@moongate.ams.com. Further  *
*  updates will be posted to the rom mailing list. If you'd like to get     *
*  the latest version of quest.c, please send a request to the above add-   *
*  ress. Quest Code v2.01. Please do not remove this notice from this file. *
****************************************************************************/
/*  quest.c  $Revision: 1.1.1.1 $  $Date: 2010/07/12 13:01:43 $  */
/* dwatch egate artifice fdmud code copyright 2002-2008 Bob Kircher	  */

#include <ctype.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"


#define MAX_MOBLIST	512	/* should be a power of 2 */


/* Instead of hard coding the vnums of quest items here (items that can
   be purchased with quest points), I placed them in a table in const.c
   --REK */

/* Object vnums for object quest 'tokens'. In Moongate, the tokens are
   things like 'the Shield of Moongate', 'the Sceptre of Moongate'. These
   items are worthless and have the rot-death flag, as they are placed
   into the world when a player receives an object quest. */

#define QUEST_OBJQUEST1 30
#define QUEST_OBJQUEST2 31
#define QUEST_OBJQUEST3 32
#define QUEST_OBJQUEST4 33
#define QUEST_OBJQUEST5 34

/* Local functions */

void generate_quest	args( ( CHAR_DATA *ch, CHAR_DATA *questman, int t ) );
void quest_update	args( ( void ) );
bool quest_level_diff   args( ( int clevel, int mlevel) );
bool chance		args( ( int num ) );

/* CHANCE function. I use this everywhere in my code, very handy :> */

bool chance(int num)
{
    if ( number_range( 1, 100 ) <= num )
	return TRUE;
    else
	return FALSE;
}

/* The main quest function */

void
do_quest( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *		questman;
    CHAR_DATA *		victim;
    OBJ_DATA *		obj = NULL;
    OBJ_DATA *		obj_next;
    OBJ_INDEX_DATA *	questinfoobj;
    OBJ_INDEX_DATA *	pObj = NULL;
    MOB_INDEX_DATA *	questinfo;
    BUFFER *		pBuf;
    char		buf [MAX_STRING_LENGTH];
    char *		arg0;
    char		arg1 [MAX_INPUT_LENGTH];
    char		arg2 [MAX_INPUT_LENGTH];
    char		arg3 [MAX_INPUT_LENGTH];
    int			countdown_val;
    int			item_count;
    int			value;
    int			index;

    arg0     = argument;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY QUIT.\n\r",ch );
	if ( get_trust( ch ) >= DEITY )
	    send_to_char( "          QUEST GRANT <player> <amount>.\n\r", ch );
        send_to_char( "For more information, type 'HELP QUEST'.\n\r",ch );
	return;
    }

    if ( !strcmp( arg1, "info" ) )
    {
	if ( IS_SET( ch->act, PLR_QUESTOR ) )
	{
	    if ( ch->questmob == -1 && ch->questgiver->short_descr != NULL )
	    {
		ch_printf( ch, "Your quest is ALMOST complete!\n\r"
			   "Get back to %s before your time runs out!\n\r",
			   ch->questgiver->short_descr );
	    }
	    else if ( ch->questobj > 0 )
	    {
                questinfoobj = get_obj_index( ch->questobj );
		if ( questinfoobj != NULL )
		{
		    ch_printf( ch, "You are on a quest to recover the fabled %s!\n\r",
		    	       questinfoobj->name );
		}
		else
		    send_to_char( "You aren't currently on a quest.\n\r", ch );
		return;
	    }
	    else if ( ch->questmob > 0 )
	    {
                questinfo = get_mob_index( ch->questmob );
		if ( questinfo != NULL )
		{
		    ch_printf( ch, "You are on a quest to slay the dreaded %s!\n\r",
				questinfo->short_descr );
		}
		else
		    send_to_char( "You aren't currently on a quest.\n\r", ch );
		return;
	    }
	}
	else
	    send_to_char( "You aren't currently on a quest.\n\r", ch );
	return;
    }
    if ( !strcmp( arg1, "points" ) )
    {
	ch_printf( ch, "You have %d quest points.\n\r", ch->questpoints );
	return;
    }
    else if ( !strcmp( arg1, "time" ) )
    {
	if ( !IS_SET( ch->act, PLR_QUESTOR ) )
	{
	    send_to_char( "You aren't currently on a quest.\n\r", ch );
	    if ( ch->nextquest > 1 )
	    {
		ch_printf( ch, "There are %d minutes remaining until you can go on another quest.\n\r",
			   ch->nextquest );
	    }
	    else if ( ch->nextquest == 1 )
	    {
		send_to_char( "There is less than a minute remaining until you can go on another quest.\n\r", ch );
	    }
	}
	else if ( ch->countdown > 0 )
	{
	    ch_printf( ch, "Time left for current quest: %d minutes.\n\r",
			ch->countdown );
	}
	return;
    }
    else if ( !str_cmp( arg1, "grant" ) )
    {
	log_printf( "%s: quest %s", ch->name, arg0 );
	if ( get_trust( ch ) < DEITY )
	{
	    do_quest( ch, "" );
	    return;
	}
	if ( !( victim = get_char_room( ch, arg2 ) ) )
	{
	    send_to_char( "That player is not here.\n\r", ch );
	    return;
	}

	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}
	if ( !is_number( arg3 ) )
	{
	    do_quest( ch, "" );
	    return;
	}

	value = atoi( arg3 );
	if ( value == 0 )
	{
	    send_to_char( "Why bother?\n\r", ch );
	    return;
	}

	if ( value > 25 )
	{
	    send_to_char( "You cannot grant more than 25 quest points.\n\r", ch );
	    return;
	}
	if ( victim->questpoints + value < 0 )
	    value = 0 - victim->questpoints;

	victim->questpoints += value;
	if ( value > 0 )
	{
	    sprintf( buf, "%d quest point%s", value,
		value == 1 ? "" : "s" );
	    act( "You grant $t to $N.", ch, buf, victim, TO_CHAR );
	    act( "$n grants you $t!", ch, buf, victim, TO_VICT );
	    act( "$n grants $t to $N.", ch, buf, victim, TO_NOTVICT );
	    log_qp( victim, value, QP_GRANT );
	    return;
	}
	else
	{
	    ch_printf( ch, "You reduce %s's quest points by %d.\n\r",
			victim->name, 0 - value );
	    log_qp( victim, value, QP_GRANT );
	    return;
	}
    }

/* Checks for a character in the room with spec_questmaster set. This special
   procedure must be defined in special.c. You could instead use an
   ACT_QUESTMASTER flag instead of a special procedure. */

    for ( questman = ch->in_room->people; questman; questman = questman->next_in_room )
    {
	if ( !IS_NPC( questman ) ) continue;
        if ( questman->spec_fun == spec_lookup( "spec_questmaster" ) ) break;
    }

    if ( questman == NULL || questman->spec_fun != spec_lookup( "spec_questmaster" ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    if ( questman->fighting != NULL )
    {
	send_to_char( "Wait until the fighting stops.\n\r", ch );
        return;
    }

    if ( !can_see( questman, ch ) )
    {
	do_say( questman, "I don't deal with people I can't see!" );
	return;
    }

    ch->questgiver = questman;

/* And, of course, you will need to change the following lines for YOUR
   quest item information. Quest items on Moongate are unbalanced, very
   very nice items, and no one has one yet, because it takes awhile to
   build up quest points :> Make the item worth their while. */

    if ( !strcmp( arg1, "list" ) )
    {
	pBuf = new_buf( );
	act( "$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM );
	act( "You ask $N for a list of quest items.",ch, NULL, questman, TO_CHAR );
	item_count = 0;
	add_buf( pBuf, "Current Quest Items available for purchase:\n\r" );
	add_buf( pBuf, "=========================================\n\r" );
	for ( index = 0; quest_table[ index ].vnum; index++ )
	{
	    if ( ( pObj = get_obj_index( quest_table[ index ].vnum ) ) == NULL )
	    {
		bug( "Do_quest: bad vnum %d", quest_table[ index ].vnum );
		continue;
	    }
	    buf_printf( pBuf, "%5dqp - lvl %3d  %s`X\n\r",
			quest_table[ index ].qcost,
			pObj->level,
			pObj->short_descr );
	    item_count++;
	}
	if ( item_count )
	    add_buf( pBuf, "=========================================\n\r" );
	add_buf( pBuf, "5000qp - 500 pracs\n\r" );
	add_buf( pBuf, " 500qp - 50 pracs\n\r" );
	add_buf( pBuf, "  50qp - 5 pracs\n\r" );
	if ( ch->clan != NULL )
	{
	    add_buf( pBuf, "=========================================\n\r" );
	/*	add_buf( pBuf, "100000qp - clan creation voucher (comes with one room clan hall)\n\r" );
	    add_buf( pBuf, " 10000qp - clan hall additional room voucher\n\r" );
	    add_buf( pBuf, " 5000qp  - clan healer voucher (level 5)\n\r" );
		add_buf( pBuf, " 5000qp  - clan guard voucher (level 5)\n\r" );
		add_buf( pBuf, " 2000qp  - clan healer/guard level up voucher (+1 level)\n\r" );*/
	    buf_printf( pBuf, "Honor points may be purchased for your %s at 1 qp per honor point.\n\r",
			ch->clan->fHouse ? "house" : "clan" );
	}
	add_buf( pBuf, "=========================================\n\r" );
	add_buf( pBuf, "For items with similar names, you may use single quotes.\n\r" );
	add_buf( pBuf, "e.g. quest buy 'clan guard' so that you buy the voucher\n\r" );
	add_buf( pBuf, "and not a pair of spiked shoulderguards.\n\r" );
	add_buf( pBuf, "Quest point refunds will `RNOT`X be given for your mistake.\n\r" );
	add_buf( pBuf, "=========================================\n\r\n\r" );
	add_buf( pBuf, "To buy an item, type 'QUEST BUY <item>'.\n\r" );
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return;
    }
    else if (!str_cmp( arg1, "quit" ))
    {
        act( "$n informs $N $e does not want the quest.", ch, NULL, questman,TO_ROOM);
        act ("You inform $N you do not want the quest.",ch, NULL, questman, TO_CHAR);
        if (ch->questgiver != questman)
        {
            sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
            do_say(questman,buf);
            return;
        }

        if (IS_SET(ch->act, PLR_QUESTOR))
        {
            if (ch->questobj > 0)                  
 {
                bool obj_found = FALSE;

                for (obj = ch->carrying; obj != NULL; obj= obj_next)
                {
                    obj_next = obj->next_content;

                    if (obj != NULL && obj->pIndexData->vnum == ch->questobj)
                    {
                        obj_found = TRUE;
                        break;
                    }
                }
                if (obj_found == TRUE)
                {                                                  
   sprintf(buf, "I'm sorry to hear that you can't complete the quest.");
                    do_say(questman,buf);

                    REMOVE_BIT(ch->act, PLR_QUESTOR);
                    ch->questgiver = NULL;
                    ch->countdown = 0;
                    ch->questmob = 0;
                    ch->questobj = 0;
                    ch->nextquest = 10;
                    extract_obj(obj);
                    return;
                }
            }
            else
            {     
           sprintf(buf, "I'm sorry to hear that you can't complete the quest.");
                do_say(questman,buf);

                REMOVE_BIT(ch->act, PLR_QUESTOR);
                ch->questgiver = NULL;
                ch->countdown = 0;
                ch->questmob = 0;
                ch->questobj = 0;
                ch->nextquest = 10;
                return;
            }
        }
    }                                                                                                   
      

    else if ( !strcmp( arg1, "buy" ) )
    {
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "To buy an item, type 'QUEST BUY <item>'.\n\r", ch );
	    return;
	}
	if ( ch->questpoints == 0 )
	{
	    sayto( questman, ch, "You don't have any quest points to spend." );
	    return;
	}
	if ( is_number( arg2 ) && !str_prefix( arg3, "honor" ) )
	{
	    value = atoi( arg2 );
	    if ( value == 0 )
	    {
		sayto( questman, ch, "Why are you wasting my time with this?" );
		return;
	    }
	    if ( value < 0 )
	    {
		sayto( questman, ch, "Oh, a wise guy, eh?" );
		return;
	    }
	    if ( ch->clan == NULL )
	    {
		sayto( questman, ch, "That's very nice of you, %s, but you're not in a clan or house.", ch->name );
		return;
	    }
	    if ( value > ch->questpoints )
	    {
		sayto( questman, ch, "You don't have that many quest points." );
		return;
	    }

	    act( "$n buys some honor points for $s $t.", ch,
		 ch->clan->fHouse ? "house" : "clan", NULL, TO_ROOM );
	    ch_printf( ch, "You buy %d honor points for your %s.\n\r",
		       value, ch->clan->fHouse ? "house" : "clan" );
	    ch->questpoints -= value;
	    ch->clan->honor += value;
	    log_qp( ch, value, QP_SPEND );
	    save_clans( );
	    return;
	}
	if ( ( arg2[0] == '5' ) && ( arg2[1] == ' ' || arg2[1] == '\0') )
	{
	    if ( ch->questpoints >= 50 )
	    {
		ch->questpoints -= 50;
		log_qp( ch, 50, QP_SPEND );
	        ch->practice += 5;
    	        act( "$N gives 5 practices to $n.", ch, NULL, questman, TO_ROOM );
    	        act( "$N gives you 5 practices.",   ch, NULL, questman, TO_CHAR );
	        return;
	    }
	    else
	    {
		sayf( questman, "Sorry, %s, but you don't have enough quest points for that.",
			 ch->name );
		return;
	    }
	}
	else if ( ( arg2[0] == '5') && ( arg2[1] == '0' ) && ( arg2[2] == '\0' ))
	{
	    if ( ch->questpoints >= 500 )
	    {
		ch->questpoints -= 500;
		log_qp( ch, 500, QP_SPEND );
	        ch->practice += 50;
    	        act( "$N gives 50 practices to $n.", ch, NULL, questman, TO_ROOM );
    	        act( "$N gives you 50 practices.",   ch, NULL, questman, TO_CHAR );
	        return;
	    }
	    else
	    {
		sayf( questman, "Sorry, %s, but you don't have enough quest points for that.",
			 ch->name );
		return;
	    }
	}

	for ( index = 0; quest_table[ index ].vnum; index++ )
	{
	    if ( ( pObj = get_obj_index( quest_table[ index ].vnum ) ) == NULL )
	    {
		bug( "Do_quest: bad vnum %d", quest_table[ index ].vnum );
		continue;
	    }
	    if ( is_name( arg2, pObj->name ) )
		break;
	}
	if ( quest_table[ index ].vnum == 0 )
	{
	    sayf( questman, "I don't have that item, %s.", ch->name );
	    return;
	}
	if ( ch->questpoints < quest_table[ index ].qcost )
	{
	    sayf( questman, "Sorry, %s, but you don't have enough quest points for that.", ch->name );
	    return;
	}
	if ( ch->level < pObj->level )
	{
	    sayf( questman, "Sorry, %s, but you are of too low a level to purchase that.", ch->name );
	    return;
	}
	obj = create_object( pObj, ch->level );
	ch->questpoints -= quest_table[ index ].qcost;
	log_qp( ch, quest_table[ index ].qcost, QP_SPEND );
	if ( obj != NULL )
	{
            obj->level = ch->level;
    	    act( "$N gives $p to $n.", ch, obj, questman, TO_ROOM );
    	    act( "$N gives you $p.",   ch, obj, questman, TO_CHAR );
	    obj_to_char( obj, ch );
	}
	return;
    }
    else if ( !strcmp( arg1, "request" ) )
    {
        act( "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM );
	act( "You ask $N for a quest.",ch, NULL, questman, TO_CHAR );
	if ( IS_SET( ch->act, PLR_QUESTOR ) )
	{
	    sayf( questman, "But you're already on a quest!" );
	    return;
	}
	if ( ch->nextquest > 0 )
	{
	    if ( ch->questnone )
		sayf( questman, "I'm sorry, %s, but I still don't have a quest for you.", ch->name );
	    else
		sayf( questman, "You're very brave, %s, but let someone else have a chance.",ch->name);
	    sayf( questman, "Come back later." );
	    return;
	}

	sayf( questman, "Thank you, brave %s!", ch->name );
        ch->questmob = 0;
	ch->questobj = 0;
	ch->questnone = FALSE;

	countdown_val = number_range( 10, 30 );
	generate_quest( ch, questman, countdown_val );

        if ( ch->questmob > 0 || ch->questobj > 0 )
	{
            ch->countdown = countdown_val;
	    SET_BIT( ch->act, PLR_QUESTOR );
	    sayf( questman, "You have %d minutes to complete this quest.",
		     ch->countdown );
	    sayf( questman, "May the gods go with you!" );
	}
	return;
    }
    else if ( !strcmp( arg1, "complete" ) )
    {
        act( "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM );
	act( "You inform $N you have completed $s quest.",ch, NULL, questman, TO_CHAR );
	if ( ch->questgiver != questman )
	{
	    sayf( questman, "I never sent you on a quest!  Perhaps you're thinking of someone else." );
	    return;
	}

	if ( IS_SET( ch->act, PLR_QUESTOR ) )
	{
	    if ( ch->questmob == -1 && ch->countdown > 0 )
	    {
		MONEY	reward;
		int	pointreward;
		int	pracreward;

	    	reward.gold = 0;
	    	reward.silver = 0;
	    	reward.copper = 0;
	    	reward.fract = number_range( 2, 24 ) * FRACT_PER_COPPER;
	    	normalize( &reward );
	    	pointreward = number_range( 25, 50 );

		sayf( questman, "Congratulations, %s, on completing your quest!",
			 ch->name );
		sayf( questman, "As a reward, I am giving you %d quest points, and%s.",
			 pointreward, money_string( &reward, FALSE, FALSE ) );
		if ( chance( 15 ) )
		{
		    pracreward = number_range( 1, 5 );
		    sayf( questman, "You also gain %d practices!\n\r",
			     pracreward );
		    ch->practice += pracreward;
		}

		REMOVE_BIT( ch->act, PLR_QUESTOR );
		ch->questgiver = NULL;
		ch->countdown = 0;
		ch->questmob = 0;
		ch->questobj = 0;
		ch->nextquest = 10;
		money_add( &ch->money, &reward, FALSE );
		ch->questpoints += pointreward;
		log_qp( ch, pointreward, QP_GAIN );

	        return;
	    }
	    else if ( ch->questobj > 0 && ch->countdown > 0 )
	    {
		bool obj_found = FALSE;

    		for ( obj = ch->carrying; obj != NULL; obj= obj_next )
    		{
        	    obj_next = obj->next_content;

		    if ( obj != NULL && obj->pIndexData->vnum == ch->questobj )
		    {
			obj_found = TRUE;
            	        break;
		    }
        	}
		if ( obj_found )
		{
		    MONEY	reward;
		    int		pointreward;
		    int		pracreward;

		    reward.gold = 0;
		    reward.silver = 0;
		    reward.copper = 0;
		    reward.fract = number_range( 2, 24 ) * FRACT_PER_COPPER;
		    normalize( &reward );
		    pointreward = number_range( 25, 50 );

		    act_color( AT_ACTION,
			       "You hand $p to $N.",
			       ch, obj, questman, TO_CHAR, POS_RESTING );
		    act_color( AT_ACTION,
			       "$n hands $p to $N.",
			       ch, obj, questman, TO_ROOM, POS_RESTING );

		    sayf( questman, "Congratulations, %s, on completing your quest!",
			     ch->name );
		    sayf( questman, "As a reward, I am giving you %d quest points, and%s.",
			   pointreward, money_string( &reward, FALSE, FALSE ) );
		    if ( chance( 15 ) )
		    {
			pracreward = number_range( 1, 5 );
			sayf( questman, "You also gain %d practices!\n\r",
			      pracreward );
			ch->practice += pracreward;
		    }

		    REMOVE_BIT( ch->act, PLR_QUESTOR );
		    ch->questgiver = NULL;
		    ch->countdown = 0;
		    ch->questmob = 0;
		    ch->questobj = 0;
		    ch->nextquest = 10;
		    money_add( &ch->money, &reward, FALSE );
		    ch->questpoints += pointreward;
		    log_qp( ch, pointreward, QP_GAIN );
		    extract_obj( obj );
		    return;
		}
		else
		{
		    sayf( questman, "You haven't completed the quest yet, but there is still time!" );
		    return;
		}
		return;
	    }
	    else if ( ( ch->questmob > 0 || ch->questobj > 0) && ch->countdown > 0 )
	    {
		sayf( questman, "You haven't completed the quest yet, but there is still time!" );
		return;
	    }
	}
	if ( ch->nextquest > 0 )
	    sayf( questman, "But you didn't complete your quest in time!" );
	else
	    sayf( questman, "You have to REQUEST a quest first, %s.", ch->name );
	return;
    }

    send_to_char( "QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY.\n\r", ch );
    send_to_char( "For more information, type 'HELP QUEST'.\n\r", ch );
    return;
}

void generate_quest( CHAR_DATA *ch, CHAR_DATA *questman, int t )
{
    CHAR_DATA *		victim;
    CHAR_DATA *		vsearch;
    ROOM_INDEX_DATA *	room;
    OBJ_DATA *		questitem;
    char		buf1 [MAX_INPUT_LENGTH];
    CHAR_DATA *		moblist[ MAX_MOBLIST ];
    int			pIndex;

    /*  Randomly selects a mob from the world mob list. If you don't
	want a mob to be selected, make sure it is immune to summon.
	Or, you could add a new mob flag called ACT_NOQUEST. The mob
	is selected for both mob and obj quests, even tho in the obj
	quest the mob is not used. This is done to assure the level
	of difficulty for the area isn't too great for the player. */

    pIndex = 0;
    for ( vsearch = char_list; vsearch; vsearch = vsearch->next )
    {
	if ( vsearch->level < ch->level
		|| vsearch->level > ch->level + 5 + ch->level / 10
		|| !IS_NPC( vsearch )
		|| vsearch->pIndexData->pShop != NULL
		|| IS_SET( vsearch->act, ACT_GAIN )
		|| IS_SET( vsearch->act, ACT_TRAIN )
		|| IS_SET( vsearch->act, ACT_PRACTICE )
		|| IS_SET( vsearch->act, ACT_IS_HEALER )
		|| IS_SET( vsearch->act, ACT_PET )
		|| IS_SET( vsearch->act, ACT_MOUNT )
		|| IS_SET( vsearch->act, ACT_NOQUEST )
		|| xIS_SET( vsearch->affected_by, AFF_CHARM )
		|| vsearch->in_room == NULL
		|| room_is_private( vsearch->in_room )
		|| vsearch->pIndexData->vnum < 100
		|| IS_SET( vsearch->in_room->area->area_flags, AREA_PROTOTYPE )
		|| IS_SET( vsearch->in_room->area->area_flags, AREA_NOQUEST )
		|| IS_SET( vsearch->in_room->room_flags, ROOM_SAFE )
		|| !is_same_landmass( ch->in_room, vsearch->in_room )
		|| !can_see( ch, vsearch ) )
	    continue;
	moblist[ pIndex++] = vsearch;
	if ( pIndex >= MAX_MOBLIST )
	    break;
    }

    ch->questnone = FALSE;
    if ( ( pIndex < 5 ) || chance( 10 ) )
    {
	sayf( questman, "I'm sorry, but I don't have any quests for you"
		" at this time.");
	sayf( questman, "Try again later." );
	ch->nextquest = 2;
	ch->questnone = TRUE;
	quest_log( "%s denied Quest", ch->name );
        return;
    }

    victim = moblist[ pIndex = number_range( 1, pIndex ) - 1 ];
    room = victim->in_room;

    quest_log( "%s assigned Quest", ch->name );

    buf1[0] = '\0';
    if ( room->area->name != NULL )
    {
	strcpy( buf1, room->area->name );
    }

    /*  40% chance it will send the player on a 'recover item' quest. */

    if ( chance( 40 ) )
    {
	int objvnum = 0;

	switch( number_range( 0,4 ) )
	{
	    case 0: objvnum = QUEST_OBJQUEST1; break;
	    case 1: objvnum = QUEST_OBJQUEST2; break;
	    case 2: objvnum = QUEST_OBJQUEST3; break;
	    case 3: objvnum = QUEST_OBJQUEST4; break;
	    case 4: objvnum = QUEST_OBJQUEST5; break;
	}

        questitem = create_object( get_obj_index( objvnum ), ch->level );
	questitem->item_type = ITEM_QUESTITEM;
	questitem->timer = t * ( PULSE_AREA / PULSE_TICK );
	questitem->value[0] = ch->id;
	obj_to_room( questitem, room );
	add_obj_fall_event( questitem );
	ch->questobj = questitem->pIndexData->vnum;

	sayf( questman, "Vile pilferers have stolen %s from the royal treasury!",
	      questitem->short_descr );
	sayf( questman, "My court wizardess, with her magic mirror, has pinpointed its location." );

	/* I changed my area names so that they have just the name of the area
	   and none of the level stuff. You may want to comment these next two
	   lines. - Vassago */

	if ( buf1[0] != '\0' )
	{
	    sayf( questman, "Look in the general area of %s for %s!",
		  buf1, room->name );
	}
	return;
    }

    /* Quest to kill a mob */

    else
    {
	switch(number_range(0,1))
	{
	    case 0:
	    sayf( questman, "An enemy of mine, %s, is making vile threats against the crown.",
		  victim->short_descr );
	    sayf( questman, "This threat must be eliminated!" );
	    break;

	    case 1:
	    sayf( questman, "Rune's most heinous criminal, %s, has escaped from the dungeon!",
		  victim->short_descr );
	    sayf( questman, "Since the escape, %s has murdered %d civilians!",
		  victim->short_descr, number_range( 2, 20 ) );
	    sayf( questman, "The penalty for this crime is death, and you are to deliver the sentence!" );
	    break;
	}

	if ( room->name != NULL )
	{
	    sayf( questman, "Seek %s out somewhere in the vicinity of %s!",
		  victim->short_descr, room->name );

	/* I changed my area names so that they have just the name of the area
	   and none of the level stuff. You may want to comment these next two
	   lines. - Vassago */

	    sayf( questman, "That location is in the general area of %s.",
		  buf1 );
	}
	ch->questmob = victim->pIndexData->vnum;
    }
    return;
}

/* Level differences to search for. Moongate has 350
   levels, so you will want to tweak these greater or
   less than statements for yourself. - Vassago */

bool quest_level_diff( int clevel, int mlevel )
{
    if ( clevel < 6 && mlevel < 15 ) return TRUE;
    else if (clevel > 6 && clevel < 15 && mlevel < 30) return TRUE;
    else if (clevel > 14 && clevel < 25 && mlevel > 29 && mlevel < 45) return TRUE;
    else if (clevel > 24 && clevel < 35 && mlevel > 44 && mlevel < 55) return TRUE;
    else if (clevel > 34 && clevel < 60 && mlevel > 54 && mlevel < 80) return TRUE;
    else if (clevel > 59 && mlevel > 79) return TRUE;
    else return FALSE;
}

/* Called from update_handler() by pulse_area */

void quest_update( void )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    CHAR_DATA *rch;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->character && d->connected == CON_PLAYING )
	{
	    ch = d->character;
	    rch = ( d->original ? d->original : d->character );

	    if ( rch->nextquest > 0 )
	    {
		rch->nextquest--;
		if ( rch->nextquest == 0 )
		{
		    send_to_char( "You may now quest again.\n\r", ch );
		    continue;
		}
	    }
	    else if ( IS_SET( rch->act, PLR_QUESTOR ) )
	    {
		if ( --rch->countdown <= 0 )
		{

		    rch->nextquest = 10;
		    ch_printf( ch, "You have run out of time for your quest!\n\r"
			       "You may quest again in %d minutes.\n\r",
			       ch->nextquest );
		    REMOVE_BIT( rch->act, PLR_QUESTOR );
		    rch->questgiver = NULL;
		    rch->countdown = 0;
		    rch->questmob = 0;
		}
		if ( rch->countdown > 0 && rch->countdown < 6 )
		{
		    send_to_char( "Better hurry, you're almost out of time for your quest!\n\r",ch);
		    return;
		}
	    }
	}
    }
    return;
}

void log_qp( CHAR_DATA *ch, int value, int how )
{
    char *h;

    switch ( how )
    {
	case QP_GAIN:	h = "gains";	break;
	case QP_LOSS:	h = "loses";	break;
	case QP_SPEND:	h = "spends";	break;
	case QP_GRANT:	h = "granted";	break;
	default:	h = "BUGGED";	break;
    }

    quest_log( "%s %s %d quest points, total %d.",
	     ch->name, h, value, ch->questpoints );
}


void quest_log( const char *fmt, ... )
{
    char	tbuf[SHORT_STRING_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    va_list	args;
    FILE *	fp;

    strcpy( tbuf, ctime( &current_time ) );
    tbuf[strlen( tbuf ) - 1] = '\0';

    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );

    fclose( fpReserve );
    if ( ( fp = fopen( SYSTEM_DIR QUEST_FILE, "a" ) ) == NULL )
    {
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }
    fprintf( fp, "%s :: %s\n", tbuf, buf );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
}


void sayf( CHAR_DATA *ch, const char *fmt, ... )
{
    char buf[MAX_STRING_LENGTH*2];      /* better safe than sorry */
    va_list args;

    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );

    do_say( ch, buf );
}
