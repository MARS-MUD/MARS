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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "magic.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"


/*
 *  Local functions
 */
static	void	fread_clan	args( ( FILE *fp ) );


/*
 * Insert a new clan into the clan list sorted by name
 */
void
insert_clan( CLAN_DATA *pNew )
{
    CLAN_DATA *	pList;
    char 	new_name[MAX_INPUT_LENGTH];
    char 	list_name[MAX_INPUT_LENGTH];

    if ( !clan_first )
    {
	clan_first = pNew;
	pNew->next = NULL;
	return;
    }

    str_lower( new_name, pNew->name );

    if ( strcmp( new_name, str_lower( list_name, clan_first->name ) ) < 0 )
    {
	pNew->next = clan_first;
	clan_first = pNew;
	return;
    }

    for ( pList = clan_first; pList->next != NULL; pList = pList->next )
    {
	str_lower( list_name, pList->next->name );
	if ( strcmp( new_name, list_name ) < 0 )
	{
	    pNew->next = pList->next;
	    pList->next = pNew;
	    return;
	}
    }

    pList->next = pNew;
    pNew->next = NULL;
    return;
}


void
do_cinfo( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    BUFFER *	pBuf;

    if ( *argument == '\0' )
    {
	if ( ch->clan != NULL )
	    argument = ch->clan->name;
        else
        {
	    send_to_char( "Get info on which clan?\n\r", ch );
	    return;
	}
    }

    if ( ( pClan = get_clan( argument ) ) == NULL )
    {
	send_to_char( "There is no clan by that name.\n\r", ch );
	return;
    }

    pBuf = new_buf( );

    buf_printf( pBuf, "%s %s`X:\n\r", pClan->fHouse ? "House" : "Clan",
                pClan->who_name );
    if ( str_cmp( pClan->ctitle[0][0], pClan->ctitle[0][1] ) )
	buf_printf( pBuf, "%s/%s: %s\n\r",
		    pClan->ctitle[0][0], pClan->ctitle[0][1], pClan->leader );
    else
	buf_printf( pBuf, "%s: %s\n\r", pClan->ctitle[0][0], pClan->leader );

    buf_printf( pBuf, "\n\r%d member%s\n\r", pClan->members,
		pClan->members != 1 ? "s" : "" );
    add_buf( pBuf, "\n\r" );
    add_buf( pBuf, pClan->description );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


void
do_clans( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    BUFFER *	pBuf;
    bool	found;
    int		len;

    pBuf = new_buf( );

    found = FALSE;
    for ( pClan = clan_first; pClan; pClan = pClan->next )
    {
	if ( !found )
	{
	    add_buf( pBuf, "Clans and houses of " MUD_NAME ":\n\r" );
	    found = TRUE;
	}
	add_buf( pBuf, pClan->fHouse ? "House " : "Clan  " );
	len = 25 + strlen( pClan->who_name ) - strlen_wo_col( pClan->who_name );
	buf_printf( pBuf, "%-*.*s`X  %-12.12s\n\r",
		    len, len, pClan->who_name,
		    pClan->leader );
    }

    if ( !found )
	add_buf( pBuf, MUD_NAME " has no clans or houses at this time.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_cset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    CLAN_DATA *	pClan;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    int		value;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( !str_cmp( arg2, "clan" )
    ||	 !str_cmp( arg2, "house" )
    ||	 !str_cmp( arg2, "clvl" ) )
    &&	 *argument != '\0' )
        argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: cset <name> <level or clan>\n\r", ch );
	return;
    }

    if ( ( vch = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( vch ) )
    {
	send_to_char( "Not on mobs.\n\r", ch );
	return;
    }

    if ( is_number( arg2 ) )
    {
	value = atoi( arg2 );
	if ( value < 0 || value > MAX_CLAN_LEVELS )
	{
	    ch_printf( ch, "Level must be 0 to %d.\n\r", MAX_CLAN_LEVELS );
	    return;
	}

	if ( vch->clan == NULL )
	{
	    send_to_char( "They're not in a clan.\n\r", ch );
	    return;
	}

	vch->clvl = value;
	ch_printf( vch, "Your clan level is now %d.\n\r", value );
	send_to_char( "Ok.\n\r", ch );
	sql_update_player_clan( vch );
	return;
    }

    if ( !str_cmp( arg2, "none" ) )
    {
        if ( vch->clan == NULL )
        {
            send_to_char( "They're not in a clan.\n\r", ch );
            return;
        }

        if ( !IS_NPC( vch ) )
            vch->clan->members--;
        vch->clvl = 0;
        vch->clan = NULL;
        save_clans( );
        sql_update_player_clan( vch );

        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( ( pClan = get_clan( arg2 ) ) == NULL )
    {
        send_to_char( "No such clan or house.\n\r", ch );
        return;
    }

    if ( pClan == vch->clan )
    {
        ch_printf( ch, "They're already a member of that %s.\n\r",
                   pClan->fHouse ? "house" : "clan" );
        return;
    }

    if ( vch->clan != NULL && !IS_NPC( vch ) )
        vch->clan->members--;
    pClan->members++;
    save_clans( );
    vch->clan = pClan;
    sql_update_player_clan( vch );
    act( "You are now a member of $t $T.", vch,
         pClan->fHouse ? "house" : "clan", pClan->who_name, TO_CHAR );
    send_to_char( "Ok.\n\r", ch );
}


void
do_cstat( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *		rch;
    CHAR_DATA *		vch;
    CLAN_DATA *		pClan;
    BUFFER *		pBuf;
    OBJ_INDEX_DATA *	pObj;
    ROOM_INDEX_DATA *	pRoom;
    MONEY		money;
    char		arg[MAX_INPUT_LENGTH];
    char		buf[MAX_STRING_LENGTH];
    char *		p;
    const char *	s;
    int			i;
    bool		isMortal;

    if ( ch->desc == NULL )
        return;

    rch = ch->desc->original == NULL ? ch : ch->desc->original;
    isMortal = !IS_IMMORTAL( rch );

    if ( isMortal && rch->clan == NULL )
    {
        send_to_char( "You're not in a clan or house.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    vch = get_char_world( ch, arg );
    if ( vch != NULL && IS_NPC( vch ) )
        vch = get_char_room( ch, arg );

    if ( vch != NULL )
    {
        if ( vch->clan == NULL )
        {
            act_new( "$N is not a member of any clan or house.", ch, NULL, vch, TO_CHAR, POS_DEAD );
            return;
        }
        p = stpcpy( buf, "$N is " );
        if ( vch->clvl <= 0 )
        {
            s = "a member";
        }
        else
        {
            s = vch->clan->ctitle[URANGE( 1, vch->clvl, MAX_CLAN_LEVELS ) - 1][vch->sex == SEX_FEMALE];
            if ( IS_NULLSTR( s ) )
                s = "a member";
        }
        p += sprintf( p, "%s of %s %s.", s,
                      vch->clan->fHouse ? "house" : "clan",
                      vch->clan->who_name );
	act_new( buf, ch, NULL, vch, TO_CHAR, POS_DEAD );
	return;
    }

    if ( isMortal && arg[0] != '\0' && str_cmp( rch->clan->name, arg ) )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        if ( rch->clan != NULL )
            strcpy( arg, rch->clan->name );
        else
        {
            send_to_char( "Cstat who/what?.\n\r", ch );
            return;
        }
    }

    if ( ( pClan = get_clan( arg ) ) == NULL )
    {
        if ( isMortal )
            send_to_char( "There's nobody here by that name.\n\r", ch );
        else
            send_to_char( "No character or clan with that name found.\n\r", ch );
        return;
    }

    if ( isMortal && pClan != rch->clan )
    {
        send_to_char( "There's nobody here by that name.\n\r", ch );
        return;
    }

    pBuf = new_buf( );

    add_buf( pBuf, pClan->fHouse ? "House:" : "Clan: " );
    buf_printf( pBuf, "     [%s]\n\r", pClan->name );
    buf_printf( pBuf, "Name:      [%s`X]\n\r", pClan->who_name );
    buf_printf( pBuf, "Type:      [%s]\n\r", pClan->fHouse ? "house" : "clan" );
    buf_printf( pBuf, "Leader:    [%s]\n\r", pClan->leader[0] != '\0' ? pClan->leader : "(none)" );
    add_buf( pBuf, "Titles:    [ 1]" );
    if ( str_cmp( pClan->ctitle[0][0], pClan->ctitle[0][1] ) )
        add_buf( pBuf, "[" );
    else
        add_buf( pBuf, " " );
    buf_printf( pBuf, "%s", pClan->ctitle[0][0] );
    if ( str_cmp( pClan->ctitle[0][0], pClan->ctitle[0][1] ) )
	buf_printf( pBuf, "]/[%s]", pClan->ctitle[0][1] );
    add_buf( pBuf, "\n\r" );
    for ( i = 1; i < MAX_CLAN_LEVELS; i++ )
    {
	if ( IS_NULLSTR( pClan->ctitle[i][0] ) && IS_NULLSTR( pClan->ctitle[i][1] ) )
	    continue;
	buf_printf( pBuf, "           [%2d]%c%s", i + 1,
	            str_cmp( pClan->ctitle[i][0], pClan->ctitle[i][1] ) ? '[' : ' ',
	            pClan->ctitle[i][0] );
	if ( str_cmp( pClan->ctitle[i][0], pClan->ctitle[i][1] ) )
	    buf_printf( pBuf, "]/[%s]", pClan->ctitle[i][1] );
	add_buf( pBuf, "\n\r" );
    }

    buf_printf( pBuf, "Induct:    [%d]\n\r", pClan->induct_level );
    buf_printf( pBuf, "Outcast:   [%d]\n\r", pClan->outcast_level );
    pRoom = get_room_index( pClan->recall );
    buf_printf( pBuf, "Recall:    [%5d] %s`X\n\r", pClan->recall, pRoom ? pRoom->name : "(none)" );
    pRoom = get_room_index( pClan->morgue );
    buf_printf( pBuf, "Morgue:    [%5d] %s`X\n\r", pClan->morgue, pRoom ? pRoom->name : "(none)" );
    pObj = get_obj_index( pClan->box );
    buf_printf( pBuf, "Box:       [%5d] %s`X\n\r", pClan->box, pObj ? pObj->short_descr : "(none)" );

    money.gold   = 0;
    money.silver = 0;
    money.copper = 0;
    money.fract  = pClan->bank;
    normalize( &money );
    buf_printf( pBuf, "Bank:      [%d%c %d%c %d%c %d%c]\n\r",
                money.gold,   GOLD_INITIAL,
                money.silver, SILVER_INITIAL,
                money.copper, COPPER_INITIAL,
                money.fract,  FRACT_INITIAL );

    buf_printf( pBuf, "Honor pts: [%d]\n\r", pClan->honor );
    buf_printf( pBuf, "Members:   [%d]\n\r", pClan->members );

    buf_printf( pBuf, "Description:\n\r%s`X", pClan->description );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_discharge( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    int		color;
    CHAR_DATA *	vch;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs can't discharge players.\n\r", ch );
	return;
    }

    if ( ch->clan == NULL )
    {
	send_to_char( "You're not a member of a clan.\n\r", ch );
	return;
    }

    if ( ch->clvl == 0 || ch->clvl > ch->clan->outcast_level )
    {
	send_to_char( "You cannot discharge members.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Discharge whom?\n\r", ch );
	return;
    }

    if ( ( vch = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( vch ) )
    {
	send_to_char( "You can't discharge mobs.\n\r", ch );
	return;
    }

    if ( vch->clan != ch->clan )
    {
	ch_printf( ch, "They're not in your %s.\n\r", ch->clan->fHouse ? "house" : "clan" );
	return;
    }

    if ( vch->clvl != 0 && vch->clvl <= ch->clvl )
    {
	act( "You cannot discharge $m.", ch, NULL, vch, TO_CHAR );
	act( "$n tried to discharge you!", ch, NULL, vch, TO_VICT );
	return;
    }

    vch->clan = NULL;
    vch->clvl = 0;
    save_char_obj( vch );
    sql_update_player_clan( vch );

    color = ch->clan->pkill ? AT_RED : AT_LBLUE;
    act_color( color, "You have discharged $N.",
               ch, NULL, vch, TO_CHAR, POS_RESTING );
    act_color( color, "You have been honorably discharged from $t`X.",
               vch, ch->clan->who_name, NULL, TO_CHAR, POS_RESTING );
    act_color( color, "$n has been honorably discharged from $t`X.",
               vch, ch->clan->who_name, ch, TO_NOTVICT, POS_RESTING );

    ch->clan->members--;
    ch->clan->honor -= 2;
    save_clans( );

    sprintf( buf, "%s has discharged %s from %s`X.", ch->name, vch->name, ch->clan->who_name );
    wiznet( buf, NULL, NULL, WIZ_CLANS, 0, 0 );

    return;
}


void
do_induct( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    int		color;
    CHAR_DATA *	vch;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs can't induct players.\n\r", ch );
	return;
    }

    if ( ch->clan == NULL )
    {
	send_to_char( "You're not a member of a clan.\n\r", ch );
	return;
    }

    if ( ch->clvl == 0 || ch->clvl > ch->clan->induct_level )
    {
	send_to_char( "You cannot induct new members.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Induct whom?\n\r", ch );
	return;
    }

    if ( ( vch = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( vch ) )
    {
	send_to_char( "You can't induct mobs.\n\r", ch );
	return;
    }

    if ( vch->clan != NULL )
    {
	send_to_char( "They're already in a clan.\n\r", ch );
	return;
    }

    color = ch->clan->pkill ? AT_RED : AT_LBLUE;

    if ( !IS_NPC( vch ) && vch->pcdata->outcast >= MAX_OUTCASTS )
    {
	act_color( color, "The faithless $N may not join your $t.",
	           ch, ch->clan->fHouse ? "house" : "clan", vch,
	           TO_CHAR, POS_RESTING );
        return;
    }

    vch->clan = ch->clan;
    vch->clvl = 0;
    save_char_obj( vch );
    sql_update_player_clan( vch );

    act_color( color, "You induct $N into $t`X.", ch, ch->clan->who_name,
	       vch, TO_CHAR, POS_RESTING );
    act_color( color, "You are now a member of $t`X.", vch, ch->clan->who_name,
	       NULL, TO_CHAR, POS_RESTING );
    act_color( color, "$n has been inducted into $t`X.", vch, ch->clan->who_name,
	       ch, TO_NOTVICT, POS_RESTING );

    vch->clan->members++;
    vch->clan->honor++;
    save_clans( );

    sprintf( buf, "%s inducted %s into %s`X.", ch->name, vch->name, ch->clan->who_name );
    wiznet( buf, NULL, NULL, WIZ_CLANS, 0, 0 );

    return;
}


void
do_outcast( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    int		color;
    CHAR_DATA *	vch;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs can't outcast players.\n\r", ch );
	return;
    }

    if ( ch->clan == NULL )
    {
	send_to_char( "You're not a member of a clan.\n\r", ch );
	return;
    }

    if ( ch->clvl == 0 || ch->clvl > ch->clan->outcast_level )
    {
	send_to_char( "You cannot outcast members.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Outcast whom?\n\r", ch );
	return;
    }

    if ( ( vch = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( vch ) )
    {
	send_to_char( "You can't outcast mobs.\n\r", ch );
	return;
    }

    if ( vch->clan != ch->clan )
    {
	ch_printf( ch, "They're not in your %s.\n\r", ch->clan->fHouse ? "house" : "clan" );
	return;
    }

    if ( vch->clvl != 0 && vch->clvl <= ch->clvl )
    {
	act( "You cannot outcast $m.", ch, NULL, vch, TO_CHAR );
	act( "$n tried to outcast you!", ch, NULL, vch, TO_VICT );
	return;
    }

    vch->clan = NULL;
    vch->clvl = 0;
    SET_BIT( vch->act2, PLR_OUTCAST );
    if ( !IS_NPC( vch ) )
        vch->pcdata->outcast++;
    save_char_obj( vch );
    sql_update_player_clan( vch );

    color = ch->clan->pkill ? AT_RED : AT_LBLUE;
    act_color( color, "You have outcast $N.",
               ch, NULL, vch, TO_CHAR, POS_RESTING );
    act_color( color, "You are no longer a member of $t`X.",
               vch, ch->clan->who_name, NULL, TO_CHAR, POS_RESTING );
    act_color( color, "$n has been outcast from $t`X.",
               vch, ch->clan->who_name, ch, TO_NOTVICT, POS_RESTING );

    ch->clan->members--;
    ch->clan->honor -= 10;
    save_clans( );

    sprintf( buf, "%s has outcast %s from %s`X.",
             ch->name, vch->name, ch->clan->who_name );
    wiznet( buf, NULL, NULL, WIZ_CLANS, 0, 0 );
    herald_announce( buf );

    return;
}


void
do_setlev( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *		vch;
    DESCRIPTOR_DATA *	d;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf[MAX_STRING_LENGTH];
    char		buf2[SHORT_STRING_LENGTH];
    const char *	cword;
    const char *	p;
    const char *	s;
    int			level;

    if ( IS_NPC( ch ) )
    {
        send_to_char( "Mobiles can't do this.\n\r", ch );
        return;
    }

    if ( ch->clan == NULL || ch->clvl != 1 )
    {
        send_to_char( "You may not use this command.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    cword = ch->clan->fHouse ? "house" : "clan";

    if ( !is_number( arg2 ) )
    {
        ch_printf( ch, "Syntax: setlev <player> <%s level>\n\r", cword );
        return;
    }

    if ( ( vch = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }
    if ( IS_NPC( vch ) )
    {
        send_to_char( "Not on mobiles.\n\r", ch );
        return;
    }
    if ( vch->clan != ch->clan )
    {
        ch_printf( ch, "They're not a member of your %s.\n\r", cword );
        return;
    }
    if ( IS_DEAD( vch ) )
    {
        send_to_char( "They're dead!\n\r", ch );
        return;
    }
    if ( vch == ch )
    {
        send_to_char( "Not on yourself!\n\r", ch );
        return;
    }
    if ( vch->clvl != 0 && vch->clvl <= ch->clvl )
    {
        send_to_char( "You can't change the level of someone of their rank.\n\r", ch );
        act_new( "$n tried to change your %t rank!", ch, cword,
                 vch, TO_VICT, POS_DEAD );
        return;
    }

    level = atoi( arg2 );
    if ( level < 0 || level > MAX_CLAN_LEVELS )
    {
        ch_printf( ch, "Bad %s rank.\n\r", cword );
        return;
    }
    if ( level != 0 && level <= ch->clvl )
    {
        send_to_char( "Rank must be lower (numerically higher) than yours.\n\r", ch );
        return;
    }
    if ( level == vch->clvl )
    {
        send_to_char( "They already hold that rank.\n\r", ch );
        return;
    }

    if ( level != 0 )
        s = ch->clan->ctitle[level - 1][vch->sex == SEX_FEMALE ? 1 : 0];
    else
        s = NULL;
    if ( IS_NULLSTR( s ) )
    {
        sprintf( buf2, "%s rank %d", cword, level );
        s = buf2;
    }

    if ( vch->clvl == 0 || ( level != 0 && level < vch->clvl ) )
        p = "promoted";
    else
        p = "demoted";

    sprintf( buf, "You have been %s to %s!", p, s );
    act_color( AT_GREEN, buf, vch, NULL, NULL, TO_CHAR, POS_DEAD );
    sprintf( buf, "$N has been %s to %s!", p, s );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING
        &&   d->character != NULL
        &&   d->character->clan == ch->clan
        &&   d->character != vch )
            act_color( AT_GREEN, buf, d->character, NULL, vch, TO_CHAR, POS_DEAD );
    }

    vch->clvl = level;
    save_char_obj( vch );
}


static void
fread_clan( FILE *fp )
{
    CLAN_DATA *	pClan;
    char *	word;
    int		val;
    bool	done;
    bool	fMatch;

    pClan = new_clan_data( );

    done = FALSE;

    while ( !done )
    {
	word = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;
	switch( UPPER( *word ) )
	{
	    case '*':
		fread_to_eol( fp );
		break;

	    case 'B':
		if ( !str_cmp( word, "Bank" ) )
		{
		    pClan->bank = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		if ( !str_cmp( word, "Box" ) )
		{
		    pClan->box = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'C':
		if ( !str_cmp( word, "Clan" ) )
		{
		    pClan->name = str_dup( fread_word( fp ) );
		    pClan->fHouse = FALSE;
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'D':
		if ( !str_cmp( word, "Desc" ) )
		{
		    pClan->description = fread_string( fp );
		    fMatch = TRUE;
		    break;
		}
		if ( !str_cmp( word, "Deity" ) )
		{
		    pClan->leader = str_dup( fread_word( fp ) );
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'E':
		if ( !str_cmp( word, "End" ) )
		{
		    fMatch = TRUE;
		    done = TRUE;
		    break;
		}
		break;

	    case 'F':
		if ( !str_cmp( word, "Ftitle" ) )
		{
		    fMatch = TRUE;
		    val = fread_number( fp );
		    if ( val < 0 || val >= MAX_CLAN_LEVELS )
		    {
			log_printf( "Fread_clan: bad clan level %d", val );
			fread_to_eol( fp );
			break;
		    }
		    pClan->ctitle[val][1] = fread_string( fp );
		    break;
		}
		break;

	    case 'H':
		if ( !str_cmp( word, "Honor" ) )
		{
		    pClan->honor = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		if ( !str_cmp( word, "House" ) )
		{
		    pClan->name = str_dup( fread_word( fp ) );
		    pClan->fHouse = TRUE;
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'I':
		if ( !str_cmp( word, "Induct" ) )
		{
		    pClan->induct_level = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'L':
		if ( !str_cmp( word, "Leader" ) )
		{
		    pClan->leader = str_dup( fread_word( fp ) );
		    fMatch = TRUE;
		    break;
		}
		if ( !str_cmp( word, "Loss" ) )
		{
		    pClan->plosses = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'M':
		if ( !str_cmp( word, "Members" ) )
		{
		    pClan->members = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		if ( !str_cmp( word, "Morgue" ) )
		{
		    pClan->morgue = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		if ( !str_cmp( word, "Mtitle" ) )
		{
		    fMatch = TRUE;
		    val = fread_number( fp );
		    if ( val < 0 || val >= MAX_CLAN_LEVELS )
		    {
			log_printf( "Fread_clan: bad clan level %d", val );
			fread_to_eol( fp );
			break;
		    }
		    pClan->ctitle[val][0] = fread_string( fp );
		    break;
		}
		break;

	    case 'N':
		if ( !str_cmp( word, "Name" ) )
		{
		    pClan->who_name = str_dup( fread_string( fp ) );
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'O':
		if ( !str_cmp( word, "Outcast" ) )
		{
		    pClan->outcast_level = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'P':
		if ( !str_cmp( word, "Pkill" ) )
		{
		    pClan->pkill = fread_number( fp );
		    pClan->pkill = TRUE; /* All clans are pkill now */
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'R':
		if ( !str_cmp( word, "Recall" ) )
		{
		    pClan->recall = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'T':
		if ( !str_cmp( word, "Title" ) )
		{
		    fMatch = TRUE;
		    val = fread_number( fp );
		    if ( val < 0 || val >= MAX_CLAN_LEVELS )
		    {
			log_printf( "Fread_clan: bad clan level %d", val );
			fread_to_eol( fp );
			break;
		    }
		    pClan->ctitle[val][0] = fread_string( fp );
		    pClan->ctitle[val][1] = str_dup( pClan->ctitle[val][0] );
		    break;
		}
		break;

	    case 'W':
		if ( !str_cmp( word, "Wins" ) )
		{
		    pClan->pkills = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		if ( !str_cmp( word, "Withdraw" ) )
		{
		    pClan->withdraw = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		break;

	}
	if ( !fMatch )
	{
	    fread_to_eol( fp );
	    log_printf( "Fread_clan: bad keyword '%s'", word );
	}
    }

    if ( *pClan->name == '\0' )
    {
	log_string( "Fread_clan: no name" );
	return;
    }

    pClan->next = clan_first;
    clan_first = pClan;
    return;

}


/*
 * Called from boot_db() at boot time.
 */
void
load_clans( void )
{
    FILE *	fp;
    char	letter;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR CLAN_FILE, "r" ) ) == NULL )
    {
	perror( "Load clans:" );
	return;
    }

    for ( ; ; )
    {
	letter = fread_letter( fp );
	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_clans: '#' not found.", 0 );
	    break;
	}

	word = fread_word( fp );
	if	( !str_cmp( word, "CLAN" ) )
	    fread_clan( fp );
	else if ( !str_cmp( word, "$" ) )
	    break;
	else
	{
	    bug( "Load_clans: word not CLAN or '$'", 0 );
	    break;
	}

    }

    fclose( fp );

    sort_clans( );

    return;
}


void
save_clans( void )
{
    FILE *	fp;
    CLAN_DATA *	pClan;
    int		i;

//  if ( !changed_clan )
//	return;

    fclose( fpReserve );
    if ( ( fp = fopen( TEMP_DIR CLAN_FILE, "w" ) ) == NULL )
    {
	fpReserve = fopen( NULL_FILE, "r" );
	perror( "Save_clans: " TEMP_DIR CLAN_FILE );
	return;
    }

    for ( pClan = clan_first; pClan; pClan = pClan->next )
    {
	fprintf( fp, "#CLAN\n" );
	if ( pClan->fHouse )
	    fprintf( fp, "House %s\n", pClan->name );
	else
	    fprintf( fp, "Clan %s\n", pClan->name );
	fprintf( fp, "Name %s~\n", pClan->who_name );
	if ( *pClan->description != '\0' )
	    fprintf( fp, "Desc\n%s~\n", fix_string( pClan->description ) );
	if ( pClan->leader && *pClan->leader != '\0' )
	    fprintf( fp, "Leader %s\n", pClan->leader );
	if ( pClan->recall )
	    fprintf( fp, "Recall %d\n", pClan->recall );
	if ( pClan->morgue )
	    fprintf( fp, "Morgue %d\n", pClan->morgue );
	if ( pClan->members )
	    fprintf( fp, "Members %d\n", pClan->members );
	if ( pClan->box != 0 )
	    fprintf( fp, "Box %d\n", pClan->box );
	if ( pClan->bank != 0 )
	    fprintf( fp, "Bank %d\n", pClan->bank );
	if ( pClan->honor != 0 )
	    fprintf( fp, "Honor %d\n", pClan->honor );
	if ( pClan->withdraw != 0 )
	    fprintf( fp, "Withdraw %d\n", pClan->withdraw );
	if ( pClan->pkills )
	    fprintf( fp, "Wins %d\n", pClan->pkills );
	if ( pClan->plosses )
	    fprintf( fp, "Loss %d\n", pClan->plosses );
	fprintf( fp, "Pkill %d\n", pClan->pkill );
	fprintf( fp, "Induct %d\n", pClan->induct_level );
	fprintf( fp, "Outcast %d\n", pClan->outcast_level );
	for ( i = 0; i < MAX_CLAN_LEVELS; i++ )
	{
	    if ( !IS_NULLSTR( pClan->ctitle[i][0] ) )
		fprintf( fp, "Mtitle %d %s~\n", i, pClan->ctitle[i][0] );
	    if ( !IS_NULLSTR( pClan->ctitle[i][1] ) )
		fprintf( fp, "Ftitle %d %s~\n", i, pClan->ctitle[i][1] );
	}
	fprintf( fp, "End\n\n" );
    }

    fprintf( fp, "#$\n" );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    rename( TEMP_DIR CLAN_FILE, SYSTEM_DIR CLAN_FILE );
    changed_clan = FALSE;
    return;
}


void
sort_clans( void )
{
    CLAN_DATA *temp_list;
    CLAN_DATA *clan_next;
    CLAN_DATA *pClan;

    if ( !clan_first )
	return;

    temp_list = clan_first;
    clan_first = NULL;

    for ( pClan = temp_list; pClan; pClan = clan_next )
    {
	clan_next = pClan->next;
	insert_clan( pClan );
    }
}

