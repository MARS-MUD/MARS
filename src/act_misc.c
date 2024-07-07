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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "magic.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"


static void rename_pet( CHAR_DATA *ch, char *argument, char *type );


void
do_balance( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char *p;

    p = stpcpy( buf, "balance " );
    strncpy( p, argument, sizeof( buf ) - ( p - buf ) - 1 );
    buf[sizeof( buf ) - 1] = '\0';
    do_function( ch, do_bank, buf );
    return;
}


void
do_bank( CHAR_DATA *ch, char *argument )
{
    MONEY		amt;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    BANK_DATA * 	bank;
    bool		found;
    OBJ_DATA *		obj;
    BUFFER *		pBuf;
    ROOM_INDEX_DATA *	room;
    SHOP_DATA *		pShop;
    CHAR_DATA *		teller;
    BANK_DATA *		vbank;
    CHAR_DATA *		victim;
    int			fee;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Your kind cannot do this.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' || !str_prefix( arg1, "help" ) )
    {
	send_to_char( "bank deposit <amount>  - Add to or create bank account.\n\r"
		      "bank balance           - Check your bank account.\n\r"
		      "bank info              - List the items in your safe deposit box.\n\r"
		      "bank list              - List the banks where you have accounts.\n\r"
		      "bank retrieve <item>   - Retrieve an item from your safe deposit box.\n\r"
		      "bank store <item>      - Place an object in your safe deposit box.\n\r"
		      "bank transfer          - Transfer funds from your account to another person.\n\r"
		      "bank withdraw <amount> - Remove from your account.\n\r"
		      "\n\r"
		      "bank help              - Show this help.\n\r"
		      , ch );
	return;
    }

    if ( !str_prefix( arg1, "list" ) )
    {
	pBuf = new_buf( );
	found = FALSE;
	for ( bank = ch->pcdata->bank; bank != NULL; bank = bank->next )
	{
	    if ( ( room = get_room_index( bank->vnum ) ) == NULL )
	    {
		bugf( "Do_bank: %s has account in nonexistent room %d.",
		     ch->name, bank->vnum );
		continue;
	    }
	    if ( !IS_SET( room->room_flags, ROOM_BANK ) )
	    {
		buildbug( "Do_bank: %s has account in room %d not flagged bank.",
			  ch->name, bank->vnum );
		continue;
	    }
	    if ( IS_IMMORTAL( ch ) )
		buf_printf( pBuf, "%6d - ", bank->vnum );
	    buf_printf( pBuf, "%s`X, %s, %d item%s.\n\r",
			room->name,
			money_string( &bank->acct, FALSE, TRUE ),
			bank->count,
			bank->count == 1 ? "" : "s" );
	    found = TRUE;
	}

	if ( !found )
	    add_buf( pBuf, "You do not have any bank accounts.\n\r" );
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return;
    }

    if ( !IS_SET( ch->in_room->room_flags, ROOM_BANK ) )
    {
	send_to_char( "You are not in a bank.\n\r", ch );
	return;
    }

    /* find the character's bank info, if any */
    for ( bank = ch->pcdata->bank; bank != NULL; bank = bank->next )
	if ( bank->vnum == ch->in_room->vnum )
	    break;

    /* find a teller, if any */
    for ( teller = ch->in_room->people; teller != NULL; teller = teller->next_in_room )
	if ( IS_NPC( teller ) && IS_SET( teller->act, ACT_IS_CHANGER ) )
	    break;

    if ( teller != NULL && !can_see( teller, ch ) )
    {
	do_say( teller, "I don't deal with folks I can't see." );
	return;
    }

    if ( teller != NULL && ( pShop = teller->pIndexData->pShop ) != NULL )
    {
	if ( pShop->open_hour <= pShop->close_hour )
	{
	    if ( time_info.hour < pShop->open_hour )
	    {
		sayto( teller, ch, "Sorry, the bank is closed.  Come back later." );
		return;
	    }
	    if ( time_info.hour > pShop->close_hour )
	    {
		sayto( teller, ch, "Sorry, the bank is closed.  Come back tomorrow." );
		return;
	    }
	}
	else
	{
	    if ( time_info.hour > pShop->close_hour && time_info.hour < pShop->open_hour )
	    {
		sayto( teller, ch, "Sorry, the bank is closed.  Come back later." );
		return;
	    }
	}
    }

    if ( !str_prefix( "bal", arg1 ) && !str_prefix( arg1, "balance" ) )
    {
	if ( bank == NULL )
	{
	    send_to_char( "You do not have an account in this bank.\n\r", ch );
	    return;
	}
	ch_printf( ch, "Your balance is%s.\n\r", money_string( &bank->acct, TRUE, FALSE ) );
	return;
    }

    else if ( !str_prefix( "dep", arg1 ) && !str_prefix( arg1, "deposit" ) )
    {
	if ( !str_cmp( argument, "all" ) )
	    amt = ch->money;
	else
	{
	    if ( money_value( &amt, argument ) == NULL )
	    {
		send_to_char( "Deposit how much of what?\n\r", ch );
		return;
	    }
	}

	if ( amt.gold <= 0 && amt.silver <= 0 && amt.copper <= 0 && amt.fract <= 0 )
	{
	    send_to_char( "Why bother?\n\r", ch );
	    return;
	}

	if ( amt.gold	 > ch->money.gold
	||   amt.silver > ch->money.silver
	||   amt.copper	 > ch->money.copper
	||   amt.fract	 > ch->money.fract )
	{
	    send_to_char( "You don't have that much.\n\r", ch );
	    return;
	}

	if ( bank == NULL )
	{
	    bank = new_bank( );
	    bank->owner = ch;
	    bank->vnum  = ch->in_room->vnum;
	    bank->next  = ch->pcdata->bank;
	    ch->pcdata->bank = bank;
	}
	money_add( &bank->acct, &amt, TRUE );
	money_subtract( &ch->money, &amt, FALSE );
	act_color( AT_ACTION, "$n makes a deposit.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
	ch_printf( ch, "Your balance is now%s.\n\r", money_string( &bank->acct, FALSE, FALSE ) );
	return;
    }

    else if ( !str_prefix( arg1, "info" ) )
    {
	if ( bank == NULL )
	{
	    send_to_char( "You do not have a safe deposit box here.\n\r", ch );
	    return;
	}

	if ( bank->storage == NULL )
	{
	    send_to_char( "Your safe deposit box is empty.\n\r", ch );
	    return;
	}

	send_to_char( "Your safe deposit box contains:\n\r", ch );
	show_list_to_char( bank->storage, ch, TRUE, TRUE );
	return;
    }

    else if ( !str_prefix( "ret", arg1 ) && !str_prefix( arg1, "retrieve" ) )
    {
	if ( *argument == '\0' )
	{
	    send_to_char( "Retrieve what?\n\r", ch );
	    return;
	}

	if ( bank == NULL )
	{
	    send_to_char( "You do not have a safe deposit box here.\n\r", ch );
	    return;
	}

	if ( bank->storage == NULL )
	{
	    send_to_char( "Your safe deposit box is empty.\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_bank( ch, bank, argument ) ) == NULL )
	{
	    send_to_char( "You do not have that object in your safe deposit box.\n\r", ch );
	    return;
	}

	obj_from_bank( obj );
	obj_to_char( obj, ch );
	act_color( AT_ACTION, "You retrieve $p`X from your safe deposit box.",
		   ch, obj, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n retrieves $p`X from $s safe deposit box.",
		   ch, obj, NULL, TO_ROOM, POS_RESTING );
	oprog_retrieve_trigger( obj, ch );
	return;
    }

    else if ( !str_prefix( arg1, "store" ) )
    {
	if ( *argument == '\0' )
	{
	    send_to_char( "Store what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_carry( ch, argument, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	fee = count_obj_levels( obj ) * FRACT_PER_COPPER;
	if ( fee < 1 )
	    fee = 1;

	amt.gold   = 0;
	amt.silver = 0;
	amt.copper = 0;
	amt.fract  = fee;
	normalize( &amt );

	ch_printf( ch, "That will cost you%s",
		   money_string( &amt, FALSE, FALSE ) );
        if ( !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
            send_to_char( ".\n\r", ch );
        else
            send_to_char( " but for you, no charge.\n\r", ch );

	if ( CASH_VALUE( ch->money ) < fee
	&& IS_IMMORTAL( ch )
	&& !IS_SET( ch->act, PLR_HOLYLIGHT ) )
	{
	    ch_printf( ch, "Unfortunately, you only have%s.\n\r",
		       money_string( &ch->money, FALSE, FALSE ) );
	    return;
	}

	if ( bank == NULL )
	{
	    bank = new_bank( );
	    bank->owner = ch;
	    bank->vnum  = ch->in_room->vnum;
	    bank->next  = ch->pcdata->bank;
	    ch->pcdata->bank = bank;
	}

	obj_from_char( obj );
	obj_to_bank( obj, bank );
	if ( !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
	    deduct_cost( ch, fee );

	act_color( AT_ACTION, "You place $p`X in your safe deposit box.",
		   ch, obj, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n places $p`X in $s safe deposit box.",
		   ch, obj, NULL, TO_ROOM, POS_RESTING );

	oprog_store_trigger( obj, ch );
	return;
    }

    else if ( !str_prefix( arg1, "transfer" ) )
    {
	argument = get_money_string( argument, arg2 );

	if ( arg2[0] == '\0' || *argument == '\0' )
	{
	    send_to_char( "Transfer how much to whom?\n\r", ch );
	    return;
	}

	if ( money_value( &amt, arg2 ) == NULL )
	{
	    send_to_char( "Transfer how much to whom?\n\r", ch );
	    return;
	}

	if ( money_compare( &bank->acct, &amt ) < 0 )
	{
	    send_to_char( "You don't have that much in your account.\n\r", ch );
	    return;
	}

	if ( amt.gold <= 0 && amt.silver <= 0 && amt.copper <= 0 && amt.fract <= 0 )
	{
	    send_to_char( "Why bother?\n\r", ch );
	    return;
	}

	if ( ( victim = get_char_room( ch, argument ) ) == NULL )
	{
	    send_to_char( "They're not here.\n\r", ch );
	    return;
	}

	if ( IS_NPC( victim ) )
	{
	    act( "$e doesn't have an account here.", ch, NULL, victim, TO_CHAR );
	    return;
	}

	for ( vbank = victim->pcdata->bank; vbank != NULL; vbank = vbank->next )
	    if ( vbank->vnum == ch->in_room->vnum )
		break;

	if ( vbank == NULL )
	{
	    act( "$e doesn't have an account here.", ch, NULL, victim, TO_CHAR );
	    return;
	}

	money_subtract( &bank->acct, &amt, TRUE );
	money_add( &vbank->acct, &amt, TRUE );

	act_color( AT_ACTION, "$n transfers$t to your account.",
		   ch, money_string( &amt, FALSE, FALSE ), victim, TO_VICT, POS_RESTING );
	act_color( AT_ACTION, "$n makes a transaction.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	ch_printf( ch, "Your balance is now%s.\n\r", money_string( &bank->acct, FALSE, FALSE ) );
	return;
    }

    else if ( !str_prefix( "with", arg1 ) && !str_prefix( arg1, "withdraw" ) )
    {
	if ( bank == NULL )
	{
	    send_to_char( "You do not have an account in this bank.\n\r", ch );
	    return;
	}

	if ( !str_cmp( argument, "all" ) )
	    amt = bank->acct;
	else if ( money_value( &amt, argument ) == NULL )
	{
	    send_to_char( "Withdraw how much of what?\n\r", ch );
	    return;
	}

	if ( money_compare( &bank->acct, &amt ) < 0 )
	{
	    send_to_char( "You don't have that much.\n\r", ch );
	    return;
	}

	if ( amt.gold <= 0 && amt.silver <= 0 && amt.copper <= 0 && amt.fract <= 0 )
	{
	    send_to_char( "Why bother?\n\r", ch );
	    return;
	}

	money_subtract( &bank->acct, &amt, TRUE );
	money_add( &ch->money, &amt, FALSE );
	act_color( AT_ACTION, "$n makes a withdrawal.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
	ch_printf( ch, "Your balance is now%s.\n\r", money_string( &bank->acct, FALSE, FALSE ) );
	return;
    }

    else
    {
	do_function( ch, do_bank, "help" );
	return;
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}


void
do_deposit( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char *p;

    p = stpcpy( buf, "deposit " );
    strncpy( p, argument, sizeof( buf ) - ( p - buf ) - 1 );
    buf[sizeof( buf ) - 1] = '\0';
    do_function( ch, do_bank, buf );
    return;
}


/* RT code to delete yourself */
void
do_delete( CHAR_DATA *ch, char *argument )
{
    char	strsave[MAX_INPUT_LENGTH];
    int		gid;
    int		i;
    bool *	p;

    if ( IS_NPC( ch ) )
    {
	do_mppurge( ch, "self" );
	return;
    }

    if ( ch->desc->editor != ED_NONE )
    {
	send_to_char( "You probably mistyped an OLC delete command.\n\r", ch );
	send_to_char( "Otherwise, if you really want to delete yourself, exit the OLC editor first.\n\r", ch );
	return;
    }

   if ( IS_SET( ch->pcdata->confirm_flags, CONF_DELETE ) )
   {
	if ( argument[0] != '\0' )
	{
	    send_to_char( "Delete status removed.\n\r", ch );
	    REMOVE_BIT( ch->pcdata->confirm_flags, CONF_DELETE );
	    return;
	}
	else
	{
    	    if ( ( gid = ch->pcdata->greet_id ) != 0 )
    	    {
    	        p = greet_table + gid;
    	        for ( i = 0; i < greet_size; i++ )
                {
                    *p = FALSE;
                    p += greet_size;
                }
    	        memset( greet_table + gid * greet_size, FALSE, greet_size );
    	        update_greet_data( );
    	    }
    	    if ( ch->clan != NULL )
    	    {
    	        ch->clan->members--;
    	        save_clans( );
    	        ch->clan = NULL;
    	    }
    	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    wiznet( "$N turns $Mself into line noise.", ch, NULL, 0, 0, 0 );
	    stop_fighting( ch, TRUE );
	    do_function( ch, &do_quit, "" );
	    ch->level = 0;
	    update_userlist( ch, FALSE );
	    unlink( strsave );
	    sql_remove_user( ch );
	    return;
 	}
    }

    if ( argument[0] != '\0' )
    {
	send_to_char( "Just type delete. No argument.\n\r", ch );
	return;
    }

    send_to_char( "Type delete again to confirm this command.\n\r", ch );
    send_to_char( "WARNING: this command is irreversible.\n\r", ch );
    send_to_char( "Typing delete with an argument will undo delete status.\n\r",
	ch );
    SET_BIT( ch->pcdata->confirm_flags, CONF_DELETE );
    wiznet( "$N is contemplating deletion.", ch, NULL, 0, 0, get_trust( ch ) );
}


void
do_dismount( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mount;

    if( ( mount = MOUNTED( ch ) ) != NULL )
    {
	act_color( AT_ACTION, "You dismount from $N.", ch, NULL, mount, TO_CHAR, POS_RESTING );
	act_color( AT_ACTION, "$n dismounts from $N.", ch, NULL, mount, TO_NOTVICT, POS_RESTING );
	act_color( AT_ACTION, "$n dismounts from you.", ch, NULL, mount, TO_VICT, POS_RESTING );

	ch->riding = FALSE;
	mount->riding = FALSE;
    }
    else
    {
	send_to_char( "You aren't mounted.\n\r", ch );
	return;
    }
}


void
do_greet( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    char	arg[MAX_INPUT_LENGTH];

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "No way!  You are still fighting!\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        if ( !check_social( ch, "greet", argument ) )
            send_to_char( "Greet whom?\n\r", ch );
        return;
    }

    if ( ( vch = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    if ( vch->position == POS_FIGHTING || vch->position <= POS_SLEEPING )
    {
        send_to_char( "They're not paying any attention to you.\n\r", ch );
        return;
    }


    if ( knows_char( vch, ch ) )
    {
        if ( !check_social( ch, "greet", argument ) )
        {
            act_color( AT_ACTION, "You introduce yourself to $N.", ch, NULL, vch, TO_CHAR, POS_RESTING );
            act_color( AT_ACTION, "$N nods in recognition to you.", ch, NULL, vch, TO_CHAR, POS_RESTING );
        }
        return;
    }
    else
        act_color( AT_ACTION, "You introduce yourself to $N.", ch, NULL, vch, TO_CHAR, POS_RESTING );

    update_known( ch, vch, TRUE );
    act_color( AT_ACTION, "$n, $t, introduces $mself to you.", ch, ch->short_descr, vch, TO_VICT, POS_RESTING );
    act_color( AT_ACTION, "$n introduces $mself to $N.", ch, NULL, vch, TO_NOTVICT, POS_RESTING );

    return;
}


void
do_introduce( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *	gch1;
    CHAR_DATA *	gch2;
    CHAR_DATA *	rch;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "No way!  You are still fighting!\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Introduce whom to whom?\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        strcpy( arg2, arg1 );
        strcpy( arg1, "self" );
    }

    if ( ( gch1 = get_char_room( ch, arg1 ) ) == NULL
    ||	 ( gch2 = get_char_room( ch, arg2 ) ) == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    if ( IS_NPC( gch1 ) || IS_NPC( gch2 ) )
    {
        send_to_char( "Mobiles require no introduction.\n\r", ch );
        return;
    }

    if ( ( IS_IMMORTAL( gch1 ) && IS_SET( gch1->act, PLR_HOLYLIGHT ) )
    ||   ( IS_IMMORTAL( gch2 ) && IS_SET( gch2->act, PLR_HOLYLIGHT ) ) )
    {
        send_to_char( "Immortals require no introduction.\n\r", ch );
        return;
    }

    if ( gch1->position == POS_FIGHTING || gch1->position <= POS_SLEEPING )
    {
        act( "$N isn't paying any attention to you.", ch, NULL, gch1, TO_CHAR );
        return;
    }

    if ( gch2->position == POS_FIGHTING || gch2->position <= POS_SLEEPING )
    {
        act( "$N isn't paying any attention to you.", ch, NULL, gch2, TO_CHAR );
        return;
    }

    if ( gch1 == ch || gch2 == ch )
    {
        if ( gch1 == gch2 )
        {
            send_to_char( "You already know yourself.\n\r", ch );
            return;
        }

        if ( gch1 == ch )
            gch1 = gch2;

        act_color( AT_ACTION, "You introduce yourself to $N.", ch, NULL, gch1, TO_CHAR, POS_RESTING );
        update_known( ch, gch1, TRUE );
        act_color( AT_ACTION, "$n, $t, introduces $mself to you.", ch, ch->short_descr, gch1, TO_VICT, POS_RESTING );
        act_color( AT_ACTION, "$n introduces $mself to $N.", ch, NULL, gch1, TO_NOTVICT, POS_RESTING );
        return;
    }

    if ( !knows_char( ch, gch1 ) )
    {
        act( "You don't know $N.", ch, NULL, gch1, TO_CHAR );
        return;
    }

    if ( !knows_char( ch, gch2 ) )
    {
        act( "You don't know $N.", ch, NULL, gch2, TO_CHAR );
        return;
    }

    if ( gch1 == gch2 )
    {
        act( "$N already knows $mself.", ch, NULL, gch1, TO_CHAR );
        return;
    }

    update_known( gch1, gch2, TRUE );
    update_known( gch2, gch1, TRUE );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        if ( rch == ch )
        {
            sprintf( buf, "You introduce %s to %s.", PERS( gch1, ch ), PERS( gch2, ch ) );
            act_color( AT_ACTION, buf, ch, NULL, NULL, TO_CHAR, POS_RESTING );
        }
        else if ( rch == gch1 )
        {
            sprintf( buf, "$n introduces you to %s, %s.", gch2->name, gch2->short_descr );
            act_color( AT_ACTION, buf, ch, NULL, rch, TO_VICT, POS_RESTING );
        }
        else if ( rch == gch2 )
        {
            sprintf( buf, "$n introduces %s, %s, to you.", gch1->name, gch1->short_descr );
            act_color( AT_ACTION, buf, ch, NULL, rch, TO_VICT, POS_RESTING );
        }
        else
        {
            sprintf( buf, "$n introduces %s to %s.", PERS( gch1, rch ), PERS( gch2, rch ) );
            act_color( AT_ACTION, buf, ch, NULL, rch, TO_VICT, POS_RESTING );
        }
    }

    return;
}


void
do_mount( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    CHAR_DATA *	mount;
    char *	p;

    argument = one_argument( argument, arg );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Consider yourself lucky they don't try to mount YOU!\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' && ch->mount
    &&	 ch->mount->in_room == ch->in_room && can_see( ch, ch->mount ) )
    {
	mount = ch->mount;
    }
    else if ( ( mount = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "Mount what?\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && get_skill( ch, gsn_riding ) < 1 )
    {
	send_to_char( "You don't know how to ride!\n\r", ch );
	return;
    } 

    if ( !IS_NPC( mount ) || !IS_SET( mount->act, ACT_MOUNT ) ) 
    {
	send_to_char( "You can't ride that.\n\r", ch ); 
	return;
    }

    if ( mount->level - 5 > ch->level )
    {
	send_to_char( "That beast is too powerful for you to ride.", ch );
	return;
    }    

    if( ( mount->mount ) && ( !mount->riding ) && ( mount->mount != ch ) )
    {
	sprintf( buf, "%s belongs to %s, not you.\n\r",
	    mount->short_descr, mount->mount->name );
	p = buf;
	while( is_colcode( p ) )
	    p += 2;
	*p = UPPER( *p );
	send_to_char( buf, ch );
	return;
    } 

    if ( mount->position < POS_STANDING )
    {
	send_to_char( "Your mount must be standing.\n\r", ch );
	return;
    }

    if ( RIDDEN( mount ) )
    {
	send_to_char( "This beast is already ridden.\n\r", ch );
	return;
    }
    else if ( MOUNTED( ch ) )
    {
	send_to_char( "You are already riding.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_MOUNT ) )
    {
	send_to_char( "You can't ride in here.\n\r", ch );
	return;
    }

    if( !mount_success( ch, mount, TRUE) )
    {
	send_to_char( "You fail to mount the beast.\n\r", ch );  
	return; 
    }

    if ( ch->size >= mount->size )
    {
        act( "You're too big to ride $M.", ch, NULL, mount, TO_CHAR );
        return;
    }

    act_color( AT_ACTION, "You hop on $N's back.", ch, NULL, mount, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n hops on $N's back.", ch, NULL, mount, TO_NOTVICT, POS_RESTING );
    act_color( AT_ACTION, "$n hops on your back!", ch, NULL, mount, TO_VICT, POS_RESTING );

    ch->mount = mount;
    ch->riding = TRUE;
    mount->mount = ch;
    mount->riding = TRUE;

    affect_strip( ch, gsn_sneak );
    xREMOVE_BIT( ch->affected_by, AFF_SNEAK );
    affect_strip( ch, gsn_hide );
    xREMOVE_BIT( ch->affected_by, AFF_HIDE );
}


void
do_mountname( CHAR_DATA *ch, char *argument )
{
    rename_pet( ch, argument, "mount" );
    return;
}


void
do_petname( CHAR_DATA *ch, char *argument )
{
    rename_pet( ch, argument, "pet" );
    return;
}


void
do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    save_char_obj( ch );
    send_to_char(
"Your character has been saved!\n\r", ch );
    /*WAIT_STATE( ch, PULSE_VIOLENCE );*/
    return;
}


void
do_setcolor( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    char *	p;
    int		temp_col_table[16];
    int		i;
    int		mcolor;	/* mud color */
    int		pcolor;	/* player's color */
    static const char clist[] = ".rgObpcwzRGYBPCW";
    static const char *colors[] =
		{ "black", "dark red", "dark green", "orange",
		  "dark blue", "purple", "cyan", "grey",
		  "dark grey", "red", "green", "yellow",
		  "blue", "pink", "light blue", "white" };

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Silly mobile.  You can't do that.\n\r", ch );
	return;
    }

    argument = first_arg( argument, arg1, FALSE );
    argument = first_arg( argument, arg2, FALSE );

    if ( arg1[0] != '\0' )
    {
	if ( !str_cmp( arg1, "reset" ) )
	{
	    for ( i = 1; i < 16; i++ )
		ch->pcdata->colormap[i] = i;
	}
	else
	{
	    if ( is_number( arg1 ) )
	    {
		mcolor = atoi( arg1 );
	    }
	    else
	    {
		if ( arg1[0] == '`' )
		    p = strchr( clist, arg1[1] );
		else
		    p = strchr( clist, arg1[0] );
		if ( p == NULL )
		    mcolor = -1;
		else
		    mcolor = p - clist;
	    }
	    if ( mcolor < 1 || mcolor > 15 )
	    {
		send_to_char( "Map what to what?\n\r", ch );
		return;
	    }

	    if ( is_number( arg2 ) )
	    {
		pcolor = atoi( arg2 );
	    }
	    else
	    {
		if ( arg2[0] == '`' )
		    p = strchr( clist, arg2[1] );
		else
		    p = strchr( clist, arg2[0] );
		if ( p == NULL )
		    pcolor = -1;
		else
		    pcolor = p - clist;
	    }
	    if ( pcolor < 1 || pcolor > 15 )
	    {
		send_to_char( "Map what to what?\n\r", ch );
		return;
	    }
	    ch->pcdata->colormap[mcolor] = pcolor;
	}
    }

    /* save the player's color mappings */
    for ( i = 1; i < 16; i++ )
    {
	temp_col_table[i] = ch->pcdata->colormap[i];
	ch->pcdata->colormap[i] = i;
    }

    /*
     * I would have liked for the colors in the first column here to have
     * been displayed in their proper colors regardless of the player's
     * mappings.  But the colormap table gets restored before the color
     * codes are processed in write_to_descriptor().  *Sigh*
     */
    p = buf;
    p += sprintf( p, "You see %s%-12s `was %s%s\n\r",
		  ansi_color[1], colors[1], 
		  ansi_color[temp_col_table[1]], colors[temp_col_table[1]] );
    for ( i = 2; i < 16; i++ )
	p += sprintf( p, "        %s%-12s `was %s%s\n\r",
		      ansi_color[i], colors[i], 
		      ansi_color[temp_col_table[i]], colors[temp_col_table[i]] );

    send_to_char( buf, ch );

    /* restore player's color map */
    for ( i = 1; i < 16; i++ )
    {
	ch->pcdata->colormap[i] = temp_col_table[i];
    }
    return;
}


void
do_tether( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	mount;
    char	arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( MOUNTED( ch ) != NULL )
    {
        send_to_char( "Not while you're riding.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        if ( ( mount = ch->mount ) == NULL )
        {
            send_to_char( "You don't have a mount.\n\r", ch );
            return;
        }
    }
    else
    {
        mount = get_char_room( ch, arg );
        if ( mount == NULL )
        {
            send_to_char( "They're not here.\n\r", ch );
            return;
        }
    }

    if ( mount == ch )
    {
        send_to_char( "You can't tether yourself!\n\r", ch );
        return;
    }

    if ( !IS_NPC( mount ) )
    {
        act_new( "You can't tether $M.", ch, NULL, mount, TO_CHAR, POS_RESTING );
        return;
    }

    if ( !IS_SET( mount->act, ACT_MOUNT ) )
    {
        act( "$N is not a mount.", ch, NULL, mount, TO_CHAR );
        return;
    }

    if ( RIDDEN( mount ) != NULL )
    {
        act( "$N has a rider, they must dismount first.", ch, NULL, mount, TO_CHAR );
        return;
    }

    if ( IS_SET( mount->act, ACT_TETHERED ) )
    {
        act( "$N is already tethered.", ch, NULL, mount, TO_CHAR );
        return;
    }

    SET_BIT( mount->act, ACT_TETHERED );
    act_color( AT_ACTION, "You tether $N.", ch, NULL, mount, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n tethers you.", ch, NULL, mount, TO_VICT, POS_RESTING );
    act_color( AT_ACTION, "$n tethers $N.", ch, NULL, mount, TO_NOTVICT, POS_RESTING );

    return;
}


void
do_touch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	mob;
    char	saybuf[MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) || ch->pcdata->deathstate != DEATH_FOUND )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
    {
        if ( IS_NPC( mob ) && mob->id == ch->pcdata->deathmob )
            break;
    }

    if ( mob == NULL )
    {
        send_to_char( "The creature bearing your life crystal is not here.\n\r", ch );
        return;
    }

    ch->pcdata->deathstate = DEATH_RESURRECT;
    ch->pcdata->deathmob = 0;

    act_color( AT_MAGIC, "$N transfers the crystal holding your essence to you.", ch, NULL, mob, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$N transfers the crystal holding $n's life essence to $m.", ch, NULL, mob, TO_ROOM, POS_RESTING );

    sprintf( saybuf, "%s %s", ch->name, "I will now send you back to the place of your birth." );
    do_sayto( mob, saybuf );
    sprintf( saybuf, "%s %s", ch->name, "You must then seek out a cleric or necromancer who will restore your life essence to your corporeal form." );
    do_sayto( mob, saybuf );
    sprintf( saybuf, "%s %s", ch->name, "Good luck to you, and be more careful next time." );
    do_sayto( mob, saybuf );

    act_color( AT_MAGIC, "$n dissolves into mist, which then dissipates.", mob, NULL, NULL, TO_ALL, POS_RESTING );
    act_color( AT_MAGIC, "$n seems to shimmer as $e fades away.", ch, NULL, NULL, TO_ROOM, POS_RESTING );

    extract_char( mob, TRUE );
    char_from_room( ch );
    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
    do_look( ch, "auto" );
    act_color( AT_ACTION, "$n appears in the room.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    return;
}


void
do_untether( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	mount;
    char	arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( MOUNTED( ch ) != NULL )
    {
        send_to_char( "Not while you're riding.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        if ( ( mount = ch->mount ) == NULL )
        {
            send_to_char( "You don't have a mount.\n\r", ch );
            return;
        }
    }
    else
    {
        mount = get_char_room( ch, arg );
        if ( mount == NULL )
        {
            send_to_char( "They're not here.\n\r", ch );
            return;
        }
    }

    if ( mount == ch )
    {
        send_to_char( "You can't untether yourself!\n\r", ch );
        return;
    }

    if ( !IS_NPC( mount ) )
    {
        act_new( "You can't untether $M.", ch, NULL, mount, TO_CHAR, POS_RESTING );
        return;
    }

    if ( !IS_SET( mount->act, ACT_MOUNT ) )
    {
        act( "$N is not a mount.", ch, NULL, mount, TO_CHAR );
        return;
    }

    if ( !IS_SET( mount->act, ACT_TETHERED ) )
    {
        act( "$N is not tethered.", ch, NULL, mount, TO_CHAR );
        return;
    }

    REMOVE_BIT( mount->act, ACT_TETHERED );
    act_color( AT_ACTION, "You untether $N.", ch, NULL, mount, TO_CHAR, POS_RESTING );
    act_color( AT_ACTION, "$n untethers you.", ch, NULL, mount, TO_VICT, POS_RESTING );
    act_color( AT_ACTION, "$n untethers $N.", ch, NULL, mount, TO_NOTVICT, POS_RESTING );

    return;
}


void
do_withdraw( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char *p;

    p = stpcpy( buf, "withdraw " );
    strncpy( p, argument, sizeof( buf ) - ( p - buf ) - 1 );
    buf[sizeof( buf ) - 1] = '\0';
    do_function( ch, do_bank, buf );
    return;
}


/******************************* DO_PETNAME ****************************/
/*
 dharvest.c

 These functions written and copywrite (c) 1997, by Gary McNickle
 for use with ROM MUD v2.4+  All rights reserved by author.

 You are granted limited liscense to use this code, as long as this credit
 message is not changed.  Feel free to modify to suit your specific mud
 (colors, options, etc) but please give credit where it's due.

 [gary@dharvest.com / http://www.dharvest.com]
*/
/** Function: rename_pet (formerly do_petname)
  * Descr   : renames pets/mounts, also allows changing of short/long and 
  *           descritpion[sic] variables.
  * Returns : (void)
  * Syntax  : petname ([name|short|long|desc]) (argument)
  * Written : v1.1 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  * Modified: 8/2004 MacGregor/Nibios
  *           Changed to subroutine callable by both do_petname
  *           and do_mountname.
  *	      Fixed memory leak in "name" subcommand.
  *	      Allowed blank argument after command, shows field's current
  *	      value.
  */
static void
rename_pet( CHAR_DATA *ch, char *argument, char *type )
{
    CHAR_DATA *	vch;
    CHAR_DATA *	pet;
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    char	command[MAX_INPUT_LENGTH];
    char *	p;

    /* smash all tilde's right away. */
    smash_tilde( argument );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        ch_printf( ch, "Rename whose %s?\n\r", type );
        return;
    }

    if ( ( vch = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    if ( IS_NPC( vch ) )
    {
        send_to_char( "Mobs don't have pets.\n\r", ch );
        return;
    }

    if ( !str_cmp( type, "pet" ) )
        pet = vch->pet;
    else
        pet = vch->mount;

    if ( pet == NULL )
    {
        ch_printf( ch, "They dont have a %s!\n\r", type );
        return;
    }

    if ( ch->in_room != pet->in_room )
    {
        ch_printf( ch, "Kinda hard for their %s to learn his new name if he's\n\r"
                       "not even with you! *boggle*\n\r", type );
        return;
    }

    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
        ch_printf( ch,
        "\n\rsyntax: %sname playername [name|short|long|desc] <argument>\n\r\n\r"
        "   example: \"%sname jojo name fido\n\r"
        "            \"%sname bobo short is hungry!\"\n\r"
        "\n\rTry using color in the descriptions!\n\r\n\r"
        "See \"help petname\" and \"help color\" for more information.\n\r",
        type, type, type );
        return;
    } /* syntax */

    if ( !str_prefix( command, "name" ) )
    {
        strip_color( buf, argument );
        p = buf;
        while ( isspace( *p ) )
            p++;

        if ( *p == '\0' )
        {
            ch_printf( ch, "Set the %s's name to what?\n\r", type );
            return;
        }
        else
        {
            free_string( pet->name );
            pet->name = str_dup( buf );
            sprintf( buf, "$N's %s has been renamed to \"%s\".", type, pet->name );
            act_color( AT_IMM, buf, ch, NULL, vch, TO_CHAR, POS_RESTING );
            sprintf( buf, "Your %s has been renamed to \"%s\".", type, pet->name );
            act_color( AT_IMM, buf, vch, NULL, NULL, TO_CHAR, POS_RESTING );
        }
    } /* name */

    else if ( !str_prefix( command, "short" ) )
    {
        if ( *argument == '\0' )
        {
            ch_printf( ch, "Set the %s's short desct to what?\n\r", type );
            return;
        }

        free_string( pet->short_descr );

        /* short description should not have an "\n\r" appended to it. */
        pet->short_descr = str_dup( argument );
        sprintf( buf, "$N's %s's short description has been set to \"%s\".", type, pet->short_descr );
        act_color( AT_IMM, buf, ch, NULL, vch, TO_CHAR, POS_DEAD );
        sprintf( buf, "Your %s's short description has been set to \"%s\".", type, pet->short_descr );
        act_color( AT_IMM, buf, vch, NULL, NULL, TO_CHAR, POS_RESTING );


    } /* short */

    else if ( !str_prefix( command, "long" ) )
    {
        if ( argument[0] == '\0' )
        {
            ch_printf( ch, "Set the %s's long description to what?\n\r", type );
            return;
        }

        sprintf( buf, "$N's %s's long description has been set to \"%s\".", type, argument );
        act_color( AT_IMM, buf, ch, NULL, vch, TO_CHAR, POS_DEAD );
        sprintf( buf, "Your %s's long description has been set to \"%s\".", type, argument );
        act_color( AT_IMM, buf, vch, NULL, NULL, TO_CHAR, POS_RESTING );

        /* long descr needs "\n\r" appended to it. */
        p = stpcpy( buf, argument );
        if ( !ispunct( *(p-1) ) )
            *p++ = '.';
        p = stpcpy( p, "\n\r" );
        p = buf;
        while ( is_colcode (p ) )
            p += 2;
        *p = UPPER( *p );
        free_string( pet->long_descr );
        pet->long_descr = str_dup( buf );

    } /* long */

    else if ( !str_prefix( command, "description" ) )
    {
        if ( argument[0] != '\0' )
        {
            p = stpcpy( buf, argument );
            p = strcpy( p, "\n\r" );
            free_string( pet->description );
            pet->description = str_dup( buf );
        }
        ch_printf( vch, "Please wait while %s sets your %s's description.\n\r",
                   PERS( ch, vch ), type );
        string_append( ch, &pet->description );
    } /* desc */

    else
        do_help( ch, "petname" );

    return;

} /* end of do_petname */



