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


/*
 * Local function
 */
static time_t date_value( char *str );


void
do_pedit( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;
    PROJECT_DATA *	pList;
    char		arg[MAX_INPUT_LENGTH];
    int			vnum;
    int			next_vnum;

    if ( ch->desc == NULL )
	return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "PEdit:  There is no default project to edit.\n\r", ch );
	return;
    }

    if ( is_number( arg ) )
    {
	vnum = atoi( arg );
	for ( pProj = project_list; pProj != NULL; pProj = pProj->next )
	{
	    if ( pProj->vnum == vnum )
		break;
	}

	if ( pProj == NULL )
	{
	    send_to_char( "Project not found.\n\r", ch );
	    return;
	}

	ch->desc->pEdit  = (void *)pProj;
	ch->desc->editor = ED_PROJECT;
	show_project( ch, pProj );
	return;
    }

    if ( !str_prefix( arg, "create" ) )
    {
	if ( !can_aedit_all( ch, NULL ) )
	{
	    send_to_char( "PEdit:  Insufficient security to create projects.\n\r", ch );
	    return;
	}

	next_vnum = 0;
	for ( pList = project_list; pList != NULL; pList = pList->next )
	    next_vnum = UMAX( next_vnum, pList->vnum );

	pProj		= new_project( );
	pProj->name	= str_dup( "New project" );
	pProj->vnum	= ++next_vnum;
	pProj->priority	= PRI_MEDIUM;
	pProj->assigner = str_dup( ch->name );
	pProj->date	= current_time;
	pProj->deadline = current_time + 60 * 60 * 24 * 14;
	pProj->status	= str_dup( "New" );

	if ( project_list == NULL )
	    project_list = pProj;
	else
	    project_last->next = pProj;
	project_last = pProj;

	ch->desc->pEdit  = (void *)pProj;
	ch->desc->editor = ED_PROJECT;

	show_project( ch, pProj );
	return;
    }

    for ( pProj = project_list; pProj != NULL; pProj = pProj->next )
    {
	if ( !str_prefix( arg, pProj->name ) )
	{
	    ch->desc->pEdit  = (void *)pProj;
	    ch->desc->editor = ED_PROJECT;
	    show_project( ch, pProj );
	    return;
	}
    }

    send_to_char( "PEdit what?\n\r", ch );
    return;
}


void
pedit( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;
    char		arg[MAX_INPUT_LENGTH];
    char		command[MAX_INPUT_LENGTH];
    int			cmd;

    EDIT_PROJECT( ch, pProj );

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' || !str_cmp( command, "show" ) )
    {
	pedit_show( ch, argument );
	return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	save_projects( );
	return;
    }

    if ( !str_prefix( "pro", command )
    &&	 !str_prefix( command, "progress" )
    &&	 ( !str_cmp( pProj->assignee, "All" ) || is_exact_name( ch->name, pProj->assignee ) ) )
    {
	changed_project |= pedit_progress( ch, argument );
	return;
    }

    if ( !str_prefix( "st", command )
    &&	 !str_prefix( command, "status" )
    &&	 ( !str_cmp( pProj->assignee, "All" ) || is_exact_name( ch->name, pProj->assignee ) ) )
    {
	changed_project |= pedit_status( ch, argument );
	return;
    }

    if ( !can_aedit_all( ch, NULL ) )
    {
	send_to_char( "PEdit:  Insufficient security to modify project.\n\r", ch );
	interpret( ch, arg );
	return;
    }

    for ( cmd = 0; *pedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, pedit_table[cmd].name ) )
	{
	    if ( (*pedit_table[cmd].olc_fun) ( ch, argument ) )
		changed_project = TRUE;
	    return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
pedit_assign( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;
    USERLIST_DATA *	plist;
    char		name[MAX_INPUT_LENGTH];
    char		name_del[MAX_INPUT_LENGTH];
    char		buf_old[MAX_INPUT_LENGTH];
    char		buf_new[MAX_INPUT_LENGTH];
    char *		p;
    char *		pbuf;
    bool		fAdd;
    bool		fAll;
    bool		fNone;
    bool		fRemove;

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  assign <name | \"all\" | \"none\">\n\r", ch );
	return FALSE;
    }

    EDIT_PROJECT( ch, pProj );

    strcpy( buf_old, pProj->assignee );

    fAdd = FALSE;
    fAll = FALSE;
    fNone = FALSE;
    fRemove = FALSE;
    while ( *argument != '\0' )
    {
	argument = one_argument( argument, name );
	name[0] = UPPER( name[0] );
	if ( !str_cmp( name, "All" ) )
	{
	    strcpy( buf_old, "All" );
	    fAll = TRUE;
	    break;
	}
	else if ( !str_cmp( name, "None" ) )
	{
	    buf_old[0] = '\0';
	    fNone = TRUE;
	    break;
	}

	for ( plist = user_first; plist != NULL; plist = plist->next )
	    if ( !str_cmp( plist->name, name ) )
		break;
	if ( plist == NULL )
	{
	    ch_printf( ch, "PEdit:  \"%s\" not found.\n\r", name );
	    return FALSE;
	}

	if ( is_exact_name( name, buf_old ) )
	{
	    /* remove assignee */
	    buf_new[0] = '\0';
	    p = buf_new;
	    pbuf = buf_old;
	    while ( *pbuf != '\0' )
	    {
		pbuf = first_arg( pbuf, name_del, FALSE );
		if ( str_cmp( name, name_del ) )
		    p += sprintf( p, " %s", name_del );
		else
		    fRemove = TRUE;
	    }
	}
	else
	{
	    /* add assignee */
	    if ( !str_cmp( buf_old, "All" ) )
		buf_old[0] = '\0';
	    sprintf( buf_new, "%s %s", buf_old, name );
	    fAdd = TRUE;
	}

	p = buf_new;
	while ( isspace( *p ) )
	    p++;
	strcpy( buf_old, p );
    }

    free_string( pProj->assignee );
    pProj->assignee = str_dup( buf_old );

    if ( fAll )
    {
	send_to_char( "Assigned to All.\n\r", ch );
	return TRUE;
    }
    if ( fNone )
    {
	send_to_char( "All assignees removed.\n\r", ch );
	return TRUE;
    }

    if ( fAdd )
	send_to_char( "Assignee added.\n\r", ch );
    if ( fRemove )
	send_to_char( "Assignee removed.\n\r", ch );

    return TRUE;
}


bool
pedit_complete( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;

    EDIT_PROJECT( ch, pProj );

    if ( *argument == '\0' )
    {
	if ( pProj->completed == 0 )
	    send_to_char( "Project marked \"complete\".\n\r", ch );
	else
	    send_to_char( "Project completion date changed.\n\r", ch );
	pProj->completed = current_time;
	pProj->fCompleted = TRUE;
	return TRUE;
    }
    else if ( !str_cmp( argument, "no" ) )
    {
	if ( pProj->completed == 0 )
	{
	    send_to_char( "Ok.\n\r", ch );
	    return FALSE;
	}
	pProj->completed = 0;
	pProj->fCompleted = FALSE;
	send_to_char( "Completion status removed.\n\r", ch );
	return TRUE;
    }
    else
    {
	send_to_char( "Syntax:  completed\n\r"
				 "         completed no\n\r", ch );
	return FALSE;
    }
}


bool
pedit_deadline( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;
    time_t		deadline;

    EDIT_PROJECT( ch, pProj );

    if ( ( deadline = date_value( argument ) ) == -1 )
    {
	send_to_char( "Syntax: deadline mm/dd/yyyy\n\r"
				 "        deadline tomorrow\n\r"
				 "        deadline next <day | week | month>\n\r"
				 "        deadline <number> <days | weeks | months>\n\r", ch );
	return FALSE;
    }

    if ( deadline < current_time )
    {
	send_to_char( "PEdit:  Deadline has already passed.\n\r", ch );
	return FALSE;
    }

    pProj->deadline = deadline;
    send_to_char( "Deadline set.\n\r", ch );
    return TRUE;
}


bool
pedit_delete( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;

    EDIT_PROJECT( ch, pProj );

    if ( argument[0] == '\0' )
    {
	ch_printf( ch, "Syntax:  delete %d\n\r", pProj->vnum );
	return FALSE;
    }

    pProj->deleted = TRUE;

    edit_done( ch );

    send_to_char( "Project scheduled for deletion.\n\r", ch );
    return TRUE;
}


bool
pedit_description( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;
    char *		text;

    EDIT_PROJECT( ch, pProj );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pProj->description );
	return TRUE;
    }

    if ( ( text = get_note_text( ch, argument ) ) == NULL )
    {
	send_to_char( "PEdit:  No such note.\n\r", ch );
	return FALSE;
    }

    free_string( pProj->description );
    pProj->description = str_dup( text );
    show_project( ch, pProj );

    return TRUE;
}


bool
pedit_name( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;

    EDIT_PROJECT( ch, pProj );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  name <new name>\n\r", ch );
	return FALSE;
    }

    free_string( pProj->name );
    pProj->name = str_dup( argument );
    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}


bool
pedit_priority( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;
    int			priority;

    EDIT_PROJECT( ch, pProj );

    if ( *argument == '\0'
    ||	 ( priority = flag_value( priority_flags, argument ) ) == NO_FLAG )
    {
	send_to_char( "Syntax:  priority <critical| high | medium | low>\n\r", ch );
	return FALSE;
    }

    if ( priority == pProj->priority )
    {
	send_to_char( "Ok.\n\r", ch );
	return FALSE;
    }

    pProj->priority = priority;
    send_to_char( "Priority set.\n\r", ch );
    return TRUE;
}


bool
pedit_progress( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;

    EDIT_PROJECT( ch, pProj );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pProj->progress );
	return TRUE;
    }

    send_to_char( "Syntax:  progress\n\r", ch );
    return FALSE;
}


bool
pedit_show( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *proj;

    EDIT_PROJECT( ch, proj );
    show_project( ch, proj );

    return FALSE;
}


bool
pedit_status( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;

    EDIT_PROJECT( ch, pProj );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  status <new status>\n\r", ch );
	return FALSE;
    }

    free_string( pProj->status );
    pProj->status = str_dup( argument );
    send_to_char( "Status updated.\n\r", ch );
    return TRUE;
}


bool
pedit_undelete( CHAR_DATA *ch, char *argument )
{
    PROJECT_DATA *	pProj;

    EDIT_PROJECT( ch, pProj );

    if ( !pProj->deleted )
    {
	send_to_char( "PEdit:  This project is not deleted.\n\r", ch );
	return FALSE;
    }

    pProj->deleted = FALSE;
    send_to_char( "Project un-deleted.\n\r", ch );
    return TRUE;
}


void
show_project( CHAR_DATA *ch, PROJECT_DATA *pProj )
{
    BUFFER *	buf;

    if ( pProj == NULL )
	return;

    buf = new_buf( );

    buf_printf( buf, "Name:        [%s]  Vnum %d\n\r", pProj->name, pProj->vnum );
    buf_printf( buf, "Assigner:    [%s]\n\r", pProj->assigner );
    buf_printf( buf, "Assigned to: [%s]\n\r", IS_NULLSTR( pProj->assignee ) ? "(none)" : pProj->assignee );
    buf_printf( buf, "Date:        [%s]\n\r", date_string( pProj->date ) );
    buf_printf( buf, "Due date:    [%s]\n\r", date_string( pProj->deadline ) );
    buf_printf( buf, "Priority:    [%s]\n\r", flag_string( priority_flags, pProj->priority ) );
    if ( pProj->completed != 0 )
	buf_printf( buf, "Completed:   [%s]\n\r", date_string( pProj->completed ) );
    else
	buf_printf( buf, "Status:      [%s]\n\r", pProj->status );
    buf_printf( buf, "Description:\n\r%s", IS_NULLSTR( pProj->description ) ? "\n\r" : pProj->description );
    buf_printf( buf, "Progress:\n\r%s", IS_NULLSTR( pProj->progress ) ? "\n\r" : pProj->progress );

    if ( pProj->deleted )
	add_buf( buf, "\n\rThis project is scheduled for deleteion.\n\r" );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


static time_t
date_value( char *str )
{
    char	arg[MAX_INPUT_LENGTH];
    int		num;
    int		val;
    int		mmon;
    int		dday;
    int		yyr;
    struct tm	time_data;

    str = one_argument( str, arg );

    if ( arg[0] == '\0' )
	return -1;

    if ( !str_cmp( arg, "tomorrow" ) )
	return current_time + 60 * 60 * 24;

    if ( !str_cmp( arg, "next" ) )
    {
	if ( !str_prefix( str, "day" ) )
	    return current_time + 60 * 60 * 24;
	else if ( !str_prefix( str, "week" ) )
	    return current_time + 60 * 60 * 24 * 7;
	else if ( !str_prefix( str, "month" ) )
	    return current_time + 60 * 60 * 24 * 30;
	else
	    return -1;
    }

    if ( is_number( arg ) )
    {
	num = atoi( arg );
	if ( num < 1 )
	    return -1;
	if ( *str == '\0' || !str_prefix( str, "days" ) )
	    return current_time + num * 60 * 60 * 24;
	else if ( !str_prefix( str, "weeks" ) )
	    return current_time + num * 60 * 60 * 24 * 7;
	else if ( !str_prefix( str, "months" ) )
	    return current_time + num * 60 * 60 * 24 * 30;
	else
	    return -1;
    }

    time_data = *localtime( &current_time );
    mmon = time_data.tm_mon + 1;
    dday = time_data.tm_mday;
    yyr  = time_data.tm_year - 100;

    num = 0;
    str = arg;
    if ( !isdigit( *str ) )
	return -1;

    while ( *str != '\0' )
    {
	if ( *str == '/' )
	{
	    str++;
	    if ( !isdigit( *str ) )
		return -1;
	    continue;
	}
	if ( isdigit( *str ) )
	{
	    num++;
	    val = atoi( str );
	    while ( isdigit( *str ) )
		str++;
	    switch( num )
	    {
		case 1:		mmon = val; continue;
		case 2:		dday = val; continue;
		case 3:		yyr  = val; continue;
		default:	return -1;
	    }
	}
	return -1;
    }

    if ( yyr < 100 )
	yyr += 100;

    if ( yyr >= 2000 )
	yyr -= 1900;

    time_data.tm_mday = dday;
    time_data.tm_mon  = mmon - 1;
    time_data.tm_year = yyr;
    return mktime( &time_data );
}


