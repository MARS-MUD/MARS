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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "db.h"
#include "interp.h"
#include "lookup.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"

void
do_cmdedit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		iCmd;
    SOCIAL_DATA *pSocial;

    if ( *argument == '\0' )
    {
	send_to_char( "CmdEdit:  There is no default command to edit.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    iCmd = get_cmd_by_name( arg );
    if ( iCmd == NO_COMMAND )
    {
	for ( pSocial = social_first; pSocial != NULL; pSocial = pSocial->next )
	{
	    if ( !str_prefix( arg, pSocial->name ) )
	    {
		ch_printf( ch, "CmdEdit:  %s is a social.  Try SEdit.\n\r",
			   capitalize( pSocial->name ) );
		return;
	    }
	}

	send_to_char( "CmdEdit:  No such command.\n\r", ch );
	return;
    }

    ch->desc->pEdit = (void *)iCmd;
    ch->desc->editor = ED_CMD;

    cmdedit_show( ch, "" );
    return;
}


void
cmdedit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    bitvector	bits;
    int		cmd;
    char	command[MAX_INPUT_LENGTH];
    int		iCmd;
    int		value;

    EDIT_COMMAND( ch, iCmd );

    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( !str_cmp( command, "done" ) )
    {
	edit_done( ch );

	if ( get_trust( ch ) < cmd_table[iCmd].level || !changed_command )
	    return;

	save_cmd_files( );

	return;
    }

    if ( get_trust( ch ) < cmd_table[iCmd].level )
	send_to_char( "CmdEdit:  Insufficient security to modify command.\n\r", ch );

    if ( command[0] == '\0' )
    {
	cmdedit_show( ch, "" );
	return;
    }

    if ( get_trust( ch ) < cmd_table[iCmd].level )
    {
	interpret( ch, arg );
	return;
    }

    /* Handle flags */
    if ( ( value = position_lookup( arg ) ) != NO_POSITION )
    {
	cmd_table[iCmd].position = value;
	send_to_char( "Position set.\n\r", ch );
	changed_command = TRUE;
	return;
    }

    if ( is_number( arg ) )
	value = atoi( arg );
    else
	value = flag_value( level_types, arg );
    if ( value != NO_FLAG )
    {
	if ( value < 0 )
	{
	    send_to_char( "CmdEdit:  Bad value for command level.\n\r", ch );
	    return;
	}
	else if ( value > get_trust( ch ) )
	{
	    send_to_char( "CmdEdit:  Cannot set level higher than your trust.\n\r", ch );
	    return;
	}
	cmd_table[iCmd].level = value;
	changed_command = TRUE;
	send_to_char( "Level set.\n\r", ch );
	return;
    }

    if ( ( value = flag_value( log_types, arg ) ) != NO_FLAG )
    {
	cmd_table[iCmd].log = value;
	changed_command = TRUE;
	send_to_char( "Logging type set.\n\r", ch );
	return;
    }

    if ( ( bits = flag_value( cmd_flags, arg ) ) != NO_FLAG )
    {
	TOGGLE_BIT( cmd_table[iCmd].flags, bits );
	changed_command = TRUE;
	send_to_char( "Command flags toggled.\n\r", ch );
	return;
    }

    /* Search table and dispatch command */
    for ( cmd = 0; *cmdedit_table[cmd].name != '\0'; cmd++ )
    {
	if ( !str_prefix( command, cmdedit_table[cmd].name ) )
	{
	    changed_command |= (*cmdedit_table[cmd].olc_fun) ( ch, argument );
	    return;
	}
    }

    /* Default to standard interpreter */
    interpret( ch, arg );
    return;

}


bool
cmdedit_add( CHAR_DATA *ch, char *argument )
{
    int		iCmd;
    int		sec;

    EDIT_COMMAND( ch, iCmd );

    if ( *argument == '\0' )
    {
	send_to_char( "CmdEdit:  Add to which security group?\n\r", ch );
	return FALSE;
    }

    if ( ( sec = security_lookup( argument ) ) < 0 )
    {
	send_to_char( "CmdEdit:  That group does not exist.\n\r", ch );
	return FALSE;
    }

    if ( IS_SET( cmd_table[iCmd].sec_flags, security_flags[sec].bit ) )
    {
	send_to_char( "CmdEdit:  Already in that group.\n\r", ch );
	return FALSE;
    }

    SET_BIT( cmd_table[iCmd].sec_flags, security_flags[sec].bit );
    send_to_char( "Group set.\n\r", ch );
    return TRUE;
}


bool
cmdedit_remove( CHAR_DATA *ch, char *argument )
{
    int		count;
    int		iCmd;
    int		index;
    int		sec;

    EDIT_COMMAND( ch, iCmd );

    sec = NO_FLAG;

    if ( cmd_table[iCmd].sec_flags == 0 )
    {
	send_to_char( "CmdEdit:  Command is not in any security groups.\n\r", ch );
	return FALSE;
    }

    if ( *argument == '\0' )
    {
	count = 0;
	for ( index = 0; security_flags[index].name != NULL; index++ )
	{
	    if ( *security_flags[index].name != '\0'
	    &&	 IS_SET( cmd_table[iCmd].sec_flags, security_flags[index].bit ) )
	    {
		count++;
		sec = index;
	    }
	    if ( count != 1 )
	    {
		send_to_char( "CmdEdit:  Remove from which security group?\n\r", ch );
		return FALSE;
	    }
	}
    }
    else
    {
	if ( ( sec = security_lookup( argument ) ) < 0 )
	{
	    send_to_char( "CmdEdit:  That group does not exist.\n\r", ch );
	    return FALSE;
	}
	if ( !IS_SET( cmd_table[iCmd].sec_flags, security_flags[sec].bit ) )
	{
	    send_to_char( "CmdEdit:  Command is not in that group.\n\r", ch );
	    return FALSE;
	}
    }

    if ( sec == NO_FLAG )
    {
	bugf( "Cmdedit_remove: bad logic testing group membership of '%s'.",
	      cmd_table[iCmd].name );
	send_to_char( "CmdEdit:  Internal error.\n\r", ch );
	return FALSE;
    }

    REMOVE_BIT( cmd_table[iCmd].sec_flags, security_flags[sec].bit );
    send_to_char( "Removed.\n\r", ch );
    return TRUE;
}


bool
cmdedit_show( CHAR_DATA *ch, char *argument )
{
    BUFFER *	buf;
    bool	found;
    int		iCmd;
    int		idx;
    const char *p;

    EDIT_COMMAND( ch, iCmd );
    buf = new_buf( );

    buf_printf( buf, "Command:  [%s]\n\r", cmd_table[iCmd].name );
    if (  IS_SET( cmd_table[iCmd].flags, CMD_NOLIST )
    &&	 !IS_SET( cmd_table[iCmd].flags, CMD_MOB ) )
    {
	p = "(none)";
	for ( idx = 0; cmd_table[idx].name[0] != '\0'; idx++ )
	{
	    if ( cmd_table[idx].do_fun == cmd_table[iCmd].do_fun
	    &&	 !IS_SET( cmd_table[idx].flags, CMD_NOLIST ) )
	    {
		p = cmd_table[idx].name;
		break;
	    }
	}
	buf_printf( buf, " Actual:  [%s]\n\r", p );
    }
    buf_printf( buf, "Level:    [%3d]", cmd_table[iCmd].level );
    if ( !is_number( ( p = level_name( cmd_table[iCmd].level ) ) ) )
	buf_printf( buf, " (%s) ", p );
    add_buf( buf, "\n\r" );
    buf_printf( buf, "Position: [%s]\n\r",
		position_table[cmd_table[iCmd].position].name );
    buf_printf( buf, "Log:      [%s]\n\r",
		flag_string( log_types, cmd_table[iCmd].log ) );
    buf_printf( buf, "Flags:    [%s]\n",
		flag_string( cmd_flags, cmd_table[iCmd].flags ) );

    found = FALSE;
    for ( idx = 0; security_flags[idx].name != NULL; idx++ )
    {
	if ( IS_SET( cmd_table[iCmd].sec_flags, security_flags[idx].bit ) )
	{
	    if ( !found )
	    {
		add_buf( buf, "Security group(s):\n\r" );
		found = TRUE;
	    }
	    if ( *security_flags[idx].name == '\0' )
		buf_printf( buf, "***Orphaned bit %s\n\r",
			    print_flags( security_flags[idx].bit ) );
	    else
		buf_printf( buf, "%s\n\r", security_flags[idx].name );
	}
    }

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return FALSE;
}


/***************************************************************************
 * Command security section (SecEdit).
 * This should probably be in its own file.  However, since the commands
 * and security are tightly coupled, and both together are still one of
 * the smaller OLC files, I decided to put them in one file.
 * If the lack of modularity bothers you, feel free to split them.
 ***************************************************************************/

#define GROUP_NAME(sec)	( ((sec) < 0)  ? "(None)" : security_flags[(sec)].name )

void
do_secedit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		sec;

    if ( *argument == '\0' )
    {
	ch->desc->pEdit = (void *)NO_FLAG;
	ch->desc->editor = ED_SECURITY;
	secedit_show( ch, "" );
	return;
    }

    if ( ( sec = security_lookup( argument ) ) >= 0 )
    {
	ch->desc->pEdit = (void *)sec;
	ch->desc->editor = ED_SECURITY;
	secedit_show( ch, "" );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !str_prefix( arg, "create" ) )
    {
	if ( secedit_create( ch, argument ) )
	{
	    ch->desc->editor = ED_SECURITY;
	    changed_secgroup = TRUE;
	}
	return;
    }
    else if ( !str_cmp( arg, "list" ) )
    {
	secedit_list( ch, "" );
	return;
    }

    send_to_char( "SecEdit:  That group does not exist.\n\r", ch );
    return;
}


void
secedit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		cmd;
    char	command[MAX_INPUT_LENGTH];

    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( !str_cmp( command, "done" ) )
    {
	edit_done( ch );
	save_cmd_files( );
	return;
    }

    if ( command[0] == '\0' )
    {
	secedit_show( ch, "" );
	return;
    }

    /* Search table and dispatch command */
    for ( cmd = 0; *secedit_table[cmd].name != '\0'; cmd++ )
    {
	if ( !str_prefix( command, secedit_table[cmd].name ) )
	{
	    /* safeguard against accidental deletion */
	    if ( !str_cmp( secedit_table[cmd].name, "delete" )
	    &&	 str_cmp( command, "delete" ) )
		continue;
	    changed_secgroup |= (*secedit_table[cmd].olc_fun) ( ch, argument );
	    return;
	}
    }

    /* Default to standard interpreter */
    interpret( ch, arg );
    return;

}


bool
secedit_add( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char *	arglist;
    bitvector	bit;
    bool	changed;
    int		iCmd;
    int		sec;

    EDIT_SECURITY( ch, sec );

    arglist = argument;
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  add <command> [<command> ... ]\n\r", ch );
	return FALSE;
    }
    if ( sec < 0 )
    {
	send_to_char( "SecEdit:  No security group is currently selected.\n\r", ch );
	return FALSE;
    }
    else
    {
	bit = security_flags[sec].bit;
    }

    while ( arg[0] != '\0' )
    {
	if ( ( iCmd = get_cmd_by_name( arg ) ) == NO_COMMAND )
	{
	    send_to_char( "SecEdit:  Command not found.\n\r", ch );
	    return FALSE;
	}
	argument = one_argument( argument, arg );
    }

    changed = FALSE;
    argument = one_argument( arglist, arg );
    while ( arg[0] != '\0' )
    {
	iCmd = get_cmd_by_name( arg );
	if ( !IS_SET( cmd_table[iCmd].sec_flags, bit ) )
	{
	    SET_BIT( cmd_table[iCmd].sec_flags, bit );
	    changed = TRUE;
	    ch_printf( ch, "%s added.\n\r", capitalize( arg ) );
	}
	argument = one_argument( argument, arg );
    }

    if ( !changed )
	send_to_char( "No comamnds changed.\n\r", ch );

    changed_command = changed;
    return changed;
}


bool
secedit_create( CHAR_DATA *ch, char *argument )
{
    bitvector	allbits;
    bitvector	bit;
    char	buf[MAX_INPUT_LENGTH];
    int		cnt;
    int		sec;

    if ( *argument == '\0' )
    {
	send_to_char( "SecEdit:  Create what?\n\r", ch );
	return FALSE;
    }

    if ( security_lookup( argument ) >= 0 )
    {
	send_to_char( "SecEdit:  That group name is in use.\n\r", ch );
	return FALSE;
    }

    /* Find a slot in the table for the new group. */
    for ( sec = 0; ; sec++ )
    {
	if ( security_flags[sec].name == NULL )
	{
	    send_to_char( "SecEdit:  Security group table is full.  New groups cannot be created.\n\r", ch );
	    return FALSE;
	}
	if ( *security_flags[sec].name == '\0' )
	    break;
    }

    /* Now find a free bit */
    allbits = 0;
    for ( cnt = 0; security_flags[cnt].name != NULL; cnt++ )
	SET_BIT( allbits, security_flags[cnt].bit );
    for ( bit = 1; bit != 0; bit <<= 1 )
	if ( !IS_SET( allbits, bit ) )
	    break;

    strcpy( buf, argument );
    buf[0] = UPPER( buf[0] );
    security_flags[sec].name = str_dup( buf );
    security_flags[sec].bit = bit;
    security_flags[sec].settable = TRUE;
    ch->desc->pEdit = (void *)sec;

    send_to_char( "Security group created.\n\r", ch );
    return TRUE;
}


bool
secedit_delete( CHAR_DATA *ch, char *argument )
{
    bitvector		bit;
    DESCRIPTOR_DATA *	d;
    int			delete;
    int			iCmd;
    int			sec;
    CHAR_DATA *		vch;

    EDIT_SECURITY( ch, sec );

    if ( *argument == '\0' )
    {
	if ( sec < 0 )
	{
	    send_to_char( "SecEdit:  No default group to delete.\n\r", ch );
	    return FALSE;
	}
	else
	{
	    delete = sec;
	}
    }
    else if ( ( delete = security_lookup( argument ) ) < 0 )
    {
	send_to_char( "SecEdit:  Security group not found.\n\r", ch );
	return FALSE;
    }

    bit = security_flags[delete].bit;

    /* Delete from command table */
    for ( iCmd = 0; !IS_NULLSTR( cmd_table[iCmd].name ); iCmd++ )
	REMOVE_BIT( cmd_table[iCmd].sec_flags, bit );

    /*
     * Remove from all active players.
     * Walk the descriptor list to catch logging players.
     * Walk the char list to catch linkdead players.
     * We will still probably miss some reconnecting players,
     * and will definitely miss players not online.
     */
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING
	&&   d->character != NULL
	&&   d->character->pcdata != NULL )
	    REMOVE_BIT( d->character->pcdata->sec_groups, bit );
    }
    for ( vch = char_list; vch != NULL; vch = vch->next )
	if ( vch->pcdata != NULL )
	    REMOVE_BIT( vch->pcdata->sec_groups, bit );

    security_flags[delete].bit = 0;
    security_flags[delete].settable = FALSE;
    free_string( security_flags[delete].name );
    security_flags[delete].name = &str_empty[0];
    security_flags[delete].settable = FALSE;
    if ( sec == delete )
	ch->desc->pEdit = (void *)NO_FLAG;

    send_to_char( "Security group deleted.\n\r", ch );
    changed_command = TRUE;
    return TRUE;
}


bool
secedit_edit( CHAR_DATA *ch, char *argument )
{
    int		sec;

    if ( *argument == '\0' )
    {
	send_to_char( "SecEdit:  Edit what?\n\r", ch );
	return FALSE;
    }

    if ( ( sec = security_lookup( argument ) ) < 0 )
    {
	send_to_char( "SecEdit:  Group not found.\n\r", ch );
	return FALSE;
    }

    ch->desc->pEdit = (void *)sec;
    secedit_show( ch, "" );
    return FALSE;
}


bool
secedit_list( CHAR_DATA *ch, char *argument )
{
    bitvector	bit;
    BUFFER *	buf;
    int		cmdcount;
    int		cnt;
    bool	found;
    int		iCmd;
    int		sec;

    EDIT_SECURITY( ch, sec );

    buf = new_buf( );
    found = FALSE;

    for ( cnt = 0; security_flags[cnt].name != NULL; cnt++ )
    {
	if ( *security_flags[cnt].name != '\0' )
	{
	    if ( !found )
	    {
		add_buf( buf, " Group           Commands\n\r" );
		add_buf( buf, "=========================\n\r" );
		found = TRUE;
	    }
	    cmdcount = 0;
	    bit = security_flags[cnt].bit;
	    for ( iCmd = 0; !IS_NULLSTR( cmd_table[iCmd].name ); iCmd++ )
		if ( IS_SET( cmd_table[iCmd].sec_flags, bit ) )
		    cmdcount++;
	    buf_printf( buf, "%c%-20.20s %3d\n\r",
			ch->desc->editor == ED_SECURITY && cnt == sec ? '*' : ' ',
			security_flags[cnt].name,
			cmdcount );
	}
    }

    if ( !found )
	add_buf( buf, "Currently there are no command security groups defined.\n\r" );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return FALSE;
}


bool
secedit_remove( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char *	arglist;
    bitvector	bit;
    bool	changed;
    int		iCmd;
    int		sec;

    EDIT_SECURITY( ch, sec );

    arglist = argument;
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  remove <command> [<command> ... ]\n\r", ch );
	return FALSE;
    }

    if ( sec < 0 )
    {
	send_to_char( "SecEdit:  No security group is currently selected.\n\r", ch );
	return FALSE;
    }
    else
    {
	bit = security_flags[sec].bit;
    }

    while( arg[0] != '\0' )
    {
	if ( get_cmd_by_name( arg ) == NO_COMMAND )
	{
	    send_to_char( "SecEdit:  Command not found.\n\r", ch );
	    return FALSE;
	}
	argument = one_argument( argument, arg );
    }


    changed = FALSE;
    argument = one_argument( arglist, arg );
    while ( arg[0] != '\0' )
    {
	iCmd = get_cmd_by_name( arg );
	if ( IS_SET( cmd_table[iCmd].sec_flags, bit ) )
	{
	    REMOVE_BIT( cmd_table[iCmd].sec_flags, bit );
	    changed = TRUE;
	    ch_printf( ch, "%s removed.\n\r", capitalize( cmd_table[iCmd].name ) );
	}
	argument = one_argument( argument, arg );
    }

    if ( !changed )
	send_to_char( "No commands changed.\n\r", ch );

    changed_command = changed;
    return changed;
}


bool
secedit_rename( CHAR_DATA *ch, char *argument )
{
    int		sec;

    EDIT_SECURITY( ch, sec );

    if ( sec < 0 )
    {
	send_to_char( "SecEdit:  No security group is currently selected.\n\r", ch );
	return FALSE;
    }

    if ( *argument == '\0' )
    {
	send_to_char( "SecEdit:  Rename to what?\n\r", ch );
	return FALSE;
    }

    if ( security_lookup( argument ) >= 0 )
    {
	send_to_char( "SecEdit:  Name already in use.\n\r", ch );
	return FALSE;
    }

    free_string( security_flags[sec].name );
    security_flags[sec].name = str_dup( argument );
    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


bool
secedit_show( CHAR_DATA *ch, char *argument )
{
    bitvector	bit;
    BUFFER *	buf;
    bool	found;
    int		iCmd;
    int		sec;
    int		col;

    EDIT_SECURITY( ch, sec );

    buf = new_buf( );

    buf_printf( buf, "Group: [%s]\n\r", GROUP_NAME( sec ) );
    if ( sec >= 0 )
    {
	bit = security_flags[sec].bit;
	add_buf( buf, "Commands:\n\r" );
	col = 0;
	found = FALSE;
	for ( iCmd = 0; !IS_NULLSTR( cmd_table[iCmd].name ); iCmd++ )
	{
	    if ( IS_SET( cmd_table[iCmd].sec_flags, bit ) )
	    {
		found = TRUE;
		buf_printf( buf, "%-12s", cmd_table[iCmd].name );
		if ( ++col % 6 == 0 )
		    add_buf( buf, "\n\r" );
	    }
	}
	if ( col % 6 != 0 )
	    add_buf( buf, "\n\r" );
	if ( !found )
	    add_buf( buf, "(none)\n\r" );
    }

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return FALSE;
}

