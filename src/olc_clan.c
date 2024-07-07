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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "db.h"
#include "interp.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"

static bool last_desc;

/*
 * Local functions
 */
/* (none) */

void
do_cedit( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	send_to_char( "CEdit:  There is no default clan to edit.\n\r", ch );
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	if ( cedit_create( ch, argument ) )
	{
	    ch->desc->editor = ED_CLAN;
	    cedit_show( ch, "" );
	}
	return;
    }

    if ( ( pClan = get_clan( command ) ) != NULL )
    {
	ch->desc->pEdit = (void *)pClan;
	ch->desc->editor = ED_CLAN;
	cedit_show( ch, "" );
    }
    else
    {
	send_to_char( "CEdit:  That clan does not exist.\n\r", ch );
    }

    return;
}


void
cedit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	command[MAX_INPUT_LENGTH];
    int		cmd;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    /*
     * Ensure that if the description was changed, the clan file gets saved.
     */
    if ( last_desc )
    {
	last_desc = FALSE;
	changed_clan = TRUE;
    }

    if ( !str_cmp( "done", command ) )
    {
	edit_done( ch );
	return;
    }

    if ( command[0] == '\0' )
    {
	cedit_show( ch, argument );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *cedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, cedit_table[cmd].name ) )
	{
	    if ( (*cedit_table[cmd].olc_fun) ( ch, argument ) )
		changed_clan = TRUE;
	    return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
cedit_box( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *		pClan;
    char		arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *	pBox;
    int			vnum;

    EDIT_CLAN( ch, pClan );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax: box <vnum>\n\r", ch );
        return FALSE;
    }

    if ( !str_cmp( arg, "none" ) )
    {
        pClan->box = 0;
        send_to_char( "Clan box cleared.\n\r", ch );
        return TRUE;
    }

    if ( !is_number( arg ) )
    {
        send_to_char( "CEdit:  Box vnum must be a number or the word \"none\".\n\r", ch );
        return FALSE;
    }

    vnum = atoi( arg );
    if ( ( pBox = get_obj_index( vnum ) ) == NULL )
    {
        ch_printf( ch, "CEdit:  No such object: vnum #%d.\n\r", vnum );
        return FALSE;
    }

    if ( pBox->item_type != ITEM_CONTAINER )
    {
        send_to_char( "CEdit:  Object is not a container.\n\r", ch );
        return FALSE;
    }

    pClan->box = vnum;
    send_to_char( "Clan donation box set.\n\r", ch );
    return TRUE;
}


/*
 * I'm leaving this code in place, but disabling it.
 * Renaming clans raises lots of issues right now.
 */
#if 0
bool
cedit_clan( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	arg[MAX_INPUT_LENGTH];

    EDIT_CLAN( ch, pClan );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  clan <newname>\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );
    if ( *argument != '\0' )
    {
	send_to_char( "CEdit:  Name must be one word.\n\r", ch );
	return FALSE;
    }

    strip_color( arg, arg );
    if ( get_clan( arg ) )
    {
	send_to_char( "CEdit:  Name in use.\n\r", ch );
	return FALSE;
    }

    free_string( pClan->name );
    pClan->name = str_dup( arg );
    sort_clans( );
    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}
#endif


bool
cedit_create( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	name[MAX_INPUT_LENGTH];

    argument = one_argument( argument, name );

    if ( name[0] == '\0' )
    {
	send_to_char( "CEdit:  No name given for new clan.\n\r", ch );
	return FALSE;
    }

    if ( get_clan( name ) != NULL )
    {
	send_to_char( "CEdit:  Clan already exists.\n\r", ch );
	return FALSE;
    }

    pClan = new_clan_data( );
    pClan->fHouse = TRUE;
    pClan->name = str_dup( name );
    pClan->next = NULL;
    pClan->who_name = str_dup( "New clan" );
    pClan->ctitle[MAX_CLAN_LEVELS - 1][0] = str_dup( "Leader" );
    pClan->ctitle[MAX_CLAN_LEVELS - 1][1] = str_dup( "Leader" );
    insert_clan( pClan );

    ch->desc->pEdit = (void *)pClan;
    send_to_char( "Clan created.\n\r", ch );
    return TRUE;
}


bool
cedit_deadly( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN( ch, pClan );

    pClan->pkill = TRUE;
    send_to_char( "Clan is now `RDeadly`X\n\r", ch );
    return TRUE;
}


bool
cedit_desc( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN( ch, pClan );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pClan->description );
	last_desc = TRUE;
	return TRUE;
    }

    send_to_char( "Syntax:  desc\n\r", ch );
    return FALSE;
}


bool
cedit_ftitle( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	arg[MAX_INPUT_LENGTH];
    int		value;
    char *	p;

    EDIT_CLAN( ch, pClan );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) || *argument == '\0' )
    {
	send_to_char( "Syntax: ftitle <levelnumber> <title-text>\n\r", ch );
	return FALSE;
    }

    value = atoi( arg );
    if ( value < 1 || value > MAX_CLAN_LEVELS )
    {
	ch_printf( ch, "CEdit:  Level must be 1 through %d.\n\r", MAX_CLAN_LEVELS );
	return FALSE;
    }
    value--;

    if ( !str_cmp( argument, "none" ) )
    {
	free_string( pClan->ctitle[value][1] );
	pClan->ctitle[value][1] = &str_empty[0];
	ch_printf( ch, "Female title for level %d removed.\n\r", value + 1 );
	return TRUE;
    }

    if ( ( p = string_change( ch, pClan->ctitle[value][1], argument ) ) != NULL )
    {
	pClan->ctitle[value][1] = p;
	ch_printf( ch, "Female title for level %d set.\n\r", value + 1 );
	return TRUE;
    }
    else
	return FALSE;
}


bool
cedit_induct( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    int		value;

    EDIT_CLAN( ch, pClan );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  induct <level>\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < 1 || value > MAX_CLAN_LEVELS )
    {
	ch_printf( ch, "CEdit:  level must be 1 to %d.\n\r", MAX_CLAN_LEVELS );
	return FALSE;
    }

    pClan->induct_level = value;

    send_to_char( "Induct level set.\n\r", ch );
    return TRUE;
}


bool
cedit_leader( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	buf[MAX_INPUT_LENGTH];

    EDIT_CLAN( ch, pClan );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  leader [string]\n\r", ch );
	return FALSE;
    }

    strcpy( buf, argument );
    strip_color( buf, buf );
    buf[0] = UPPER( buf[0] );
    free_string( pClan->leader );
    if ( !str_cmp( buf, "none" ) )
    {
	pClan->leader = &str_empty[0];
	send_to_char( "Leader removed.\n\r", ch );
    }
    else
    {
	pClan->leader = str_dup( buf );
	send_to_char( "Leader set.\n\r", ch);
    }
    return TRUE;
}


bool
cedit_losses( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN( ch, pClan );

    if ( !pClan->pkill )
    {
	send_to_char( "CEdit:  Clan is peaceful.\n\r", ch );
	return FALSE;
    }

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  losses [number]\n\r", ch );
	return FALSE;
    }

    pClan->plosses = atoi( argument );

    send_to_char( "Losses set.\n\r", ch);
    return TRUE;
}


bool
cedit_members( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN( ch, pClan );

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  members [number]\n\r", ch );
	return FALSE;
    }

    pClan->members = atoi( argument );

    send_to_char( "Members set.\n\r", ch);
    return TRUE;
}


bool
cedit_morgue( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	room[MAX_INPUT_LENGTH];
    int		value;

    EDIT_CLAN( ch, pClan );

    one_argument( argument, room );

    if ( !is_number( argument ) || argument[0] == '\0' )
    {
	send_to_char( "Syntax:  morgue [#rvnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( room );

    if ( value == 0 )
    {
	pClan->morgue = 0;
	send_to_char( "Morgue removed.\n\r", ch );
	return TRUE;
    }

    if ( !get_room_index( value ) )
    {
	send_to_char( "CEdit:  Room does not exist.\n\r", ch );
	return FALSE;
    }

    pClan->morgue = value;

    send_to_char( "Morgue set.\n\r", ch );
    return TRUE;
}


bool
cedit_mtitle( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	arg[MAX_INPUT_LENGTH];
    int		value;
    char *	p;

    EDIT_CLAN( ch, pClan );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) || *argument == '\0' )
    {
	send_to_char( "Syntax: mtitle <levelnumber> <title-text>\n\r", ch );
	return FALSE;
    }

    value = atoi( arg );
    if ( value < 1 || value > MAX_CLAN_LEVELS )
    {
	ch_printf( ch, "CEdit:  Level must be 1 through %d.\n\r", MAX_CLAN_LEVELS );
	return FALSE;
    }
    value--;

    if ( !str_cmp( argument, "none" ) )
    {
	free_string( pClan->ctitle[value][0] );
	pClan->ctitle[value][0] = &str_empty[0];
	ch_printf( ch, "Male title for level %d removed.\n\r", value + 1 );
	return TRUE;
    }

    if ( ( p = string_change( ch, pClan->ctitle[value][0], argument ) ) != NULL )
    {
	pClan->ctitle[value][0] = p;
	ch_printf( ch, "Male title for level %d set.\n\r", value + 1 );
	return TRUE;
    }
    else
	return FALSE;
}


bool
cedit_name( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char *	p;

    EDIT_CLAN( ch, pClan );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [name]\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pClan->who_name, argument ) ) != NULL )
    {
	pClan->who_name = p;
	send_to_char( "Name set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;

}


bool
cedit_outcast( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    int		value;

    EDIT_CLAN( ch, pClan );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  outcast <level>\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < 1 || value > MAX_CLAN_LEVELS )
    {
	ch_printf( ch, "CEdit:  level must be 1 to %d.\n\r", MAX_CLAN_LEVELS );
	return FALSE;
    }

    pClan->outcast_level = value;

    send_to_char( "Outcast level set.\n\r", ch );
    return TRUE;
}


bool
cedit_peaceful( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN( ch, pClan );

    pClan->pkill = FALSE;
    send_to_char( "Clan is now `CPeaceful`X\n\r", ch );
    return TRUE;
}


bool
cedit_recall( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	room[MAX_INPUT_LENGTH];
    int		value;

    EDIT_CLAN( ch, pClan );

    one_argument( argument, room );

    if ( !is_number( argument ) || argument[0] == '\0' )
    {
	send_to_char( "Syntax:  recall [#rvnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( room );

    if ( value == 0 )
    {
	pClan->recall = 0;
	send_to_char( "Recall removed.\n\r", ch );
	return TRUE;
    }

    if ( !get_room_index( value ) )
    {
	send_to_char( "CEdit:  Room does not exist.\n\r", ch );
	return FALSE;
    }

    pClan->recall = value;

    send_to_char( "Recall set.\n\r", ch );
    return TRUE;
}


bool
cedit_show( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    OBJ_INDEX_DATA *	pObj;
    CLAN_DATA *		pClan;
    BUFFER *		pBuf;
    int			i;

    EDIT_CLAN( ch, pClan );

    pBuf = new_buf( );

    buf_printf( pBuf, "Clan:      [%s]\n\r", pClan->name );
    buf_printf( pBuf, "Name:      [%s`X]\n\r", pClan->who_name );
    buf_printf( pBuf, "Description:\n\r%s`X", pClan->description );
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
	buf_printf( pBuf, "           [%2d]%c%s", i + 1,
	            str_cmp( pClan->ctitle[i][0], pClan->ctitle[i][1] ) ? '[' : ' ',
	            pClan->ctitle[i][0] );
//	buf_printf( pBuf, "           [%2d][%s]", i + 1, pClan->ctitle[i][0] );
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
#if 0
    buf_printf( pBuf, "Type:      [%s`X]\n\r", pClan->pkill ? "`RDeadly" : "`CPeaceful" );
#endif
    buf_printf( pBuf, "Members:   [%d]\n\r", pClan->members );
#if 0
    if ( pClan->pkill )
    {
	buf_printf( pBuf, "Wins:      [%d]\n\r", pClan->pkills );
	buf_printf( pBuf, "Losses:    [%d]\n\r", pClan->plosses );
    }
#endif

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}


bool
cedit_title( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    char	arg[MAX_INPUT_LENGTH];
    int		value;
    char *	p;

    EDIT_CLAN( ch, pClan );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) || *argument == '\0' )
    {
	send_to_char( "Syntax: title <levelnumber> <title-text>\n\r", ch );
	return FALSE;
    }

    value = atoi( arg );
    if ( value < 1 || value > MAX_CLAN_LEVELS )
    {
	ch_printf( ch, "CEdit:  Level must be 1 through %d.\n\r", MAX_CLAN_LEVELS );
	return FALSE;
    }
    value--;

    if ( !str_cmp( argument, "none" ) )
    {
	free_string( pClan->ctitle[value][0] );
	pClan->ctitle[value][0] = &str_empty[0];
	free_string( pClan->ctitle[value][1] );
	pClan->ctitle[value][1] = &str_empty[0];
	ch_printf( ch, "Title for level %d removed.\n\r", value + 1 );
	return TRUE;
    }

    if ( ( p = string_change( ch, pClan->ctitle[value][0], argument ) ) != NULL )
    {
	pClan->ctitle[value][0] = p;
	free_string( pClan->ctitle[value][1] );
	pClan->ctitle[value][1] = str_dup( pClan->ctitle[value][0] );
	ch_printf( ch, "Title for level %d set.\n\r", value + 1 );
	return TRUE;
    }
    else
	return FALSE;
}


bool
cedit_type( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;

    EDIT_CLAN( ch, pClan );

    if ( *argument != '\0' )
    {
        if ( !str_prefix( argument, "clan" ) )
        {
            pClan->fHouse = FALSE;
            send_to_char( "Type set to CLAN.\n\r", ch );
            return TRUE;
        }
        if ( !str_prefix( argument, "house" ) )
        {
            pClan->fHouse = TRUE;
            send_to_char( "Type set to HOUSE.\n\r", ch );
            return TRUE;
        }
    }

    send_to_char( "Syntax:  type clan|house\n\r", ch );
    return FALSE;
}


bool
cedit_wins( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN( ch, pClan );

    if ( !pClan->pkill )
    {
	send_to_char( "CEdit:  Clan is peaceful.\n\r", ch );
	return FALSE;
    }

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  wins [number]\n\r", ch );
	return FALSE;
    }

    pClan->pkills = atoi( argument );

    send_to_char( "Wins set.\n\r", ch);
    return TRUE;
}


bool
cedit_withdraw( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *	pClan;
    int		value;

    EDIT_CLAN( ch, pClan );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  withdraw <level>\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < 1 || value > MAX_CLAN_LEVELS )
    {
	ch_printf( ch, "CEdit:  level must be 1 to %d.\n\r", MAX_CLAN_LEVELS );
	return FALSE;
    }

    pClan->withdraw = value;

    send_to_char( "Withdrawal level set.\n\r", ch );
    return TRUE;
}

