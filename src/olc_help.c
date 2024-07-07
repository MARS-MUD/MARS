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
#include "lookup.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"

/*
 * Local functions
 */
/* None */


void
do_hedit( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *	pHelp;
    AREA_DATA *	pArea;
    AREA_DATA *	pLimbo;
    char	command[MAX_INPUT_LENGTH];
    char	arg[MAX_INPUT_LENGTH];
    int		vnum;
    int		count;

    strcpy( arg, argument );
    argument = one_argument( argument, command );

    pLimbo = get_area_data( 0 );
    pArea  = get_area_edit( ch );
    if ( pArea == NULL )
        pArea = ch->in_room->area;

    /*
     * This is a major deviation from the normal "create" syntax...
     * but we need to allow "hedit create food", etc.
     */
    if ( !str_cmp( command, "c" ) )
    {
	if ( get_help( argument, TRUE ) )
	{
	    send_to_char( "HEdit:  Help entry already exists.\n\r", ch );
	    return;
	}

	if ( *argument == '\0' )
	{
	    send_to_char( "Create new help with what keyword?\n\r", ch );
	    return;
	}

	if ( !IS_BUILDER( ch, pArea ) )
	{
	    send_to_char( "HEdit: Insufficient security to create Help.\n\r", ch );
	    return;
	}
	pHelp = new_help( );
	if ( !help_first )
	    help_first = pHelp;
	if ( help_last )
	    help_last->next = pHelp;
	help_last = pHelp;

	pHelp->ed_level = get_trust( ch );
	pHelp->keyword	= str_dup( all_capitalize( argument ) );
	pHelp->area	= can_aedit_all( ch, pLimbo ) ? pLimbo : pArea;
    }
    else /* edit an existing help */
    {
	if ( is_number( arg ) )
	{
	    vnum = atoi( arg );
	    count = 0;
	    if ( vnum > 0 )
	    {
		for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
		    if ( ++count == vnum )
			break;
	    }
	    else
		pHelp = NULL;
	}
	else if ( ( pHelp = get_help( arg, TRUE ) ) == NULL )
	{
	    pHelp = get_help( arg, FALSE );
	}
    }

    if ( pHelp == NULL )
    {
	send_to_char( "HEdit:  Help entry not found.\n\r", ch );
	return;
    }

    ch->desc->pEdit  = (void *)pHelp;
    ch->desc->editor = ED_HELP;
    ch->desc->inEdit = NULL;
    ch->desc->editin = ED_NONE;

    hedit_show( ch, "" );
    return;
}


void
hedit( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *	pHelp;
    char	command[MAX_INPUT_LENGTH];
    char	arg[MAX_INPUT_LENGTH];
    int		cmd;

    EDIT_HELP( ch, pHelp );

    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	hedit_show( ch, "" );
	return;
    }

    if ( !str_cmp( command, "done" ) )
    {
	if ( !str_cmp( pHelp->keyword, "greeting" ) )
	    strcpy( help_greeting, pHelp->text );
	edit_done( ch );
	return;
    }

    if ( !pHelp->area )
    {
	send_to_char( "HEdit:  Help is read-only.\n\r", ch );
	interpret( ch, arg );
	return;
    }
    if ( !IS_BUILDER( ch, pHelp->area ) || get_trust( ch ) < pHelp->ed_level )
    {
	send_to_char( "HEdit:  Insufficient security to modify Help.\n\r", ch );
	interpret( ch, arg );
	return;
    }

    for ( cmd = 0; *hedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, hedit_table[cmd].name ) )
	{
	    if ( (*hedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pHelp->area->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Default to standard interpreter */
    interpret( ch, arg );
    return;
}


bool
hedit_area( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *	pHelp;
    AREA_DATA *	pArea;

    EDIT_HELP( ch, pHelp );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  area <area_num #>\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( ".", argument ) )
    {
	pArea = ch->in_room->area;
    }
    else
    {
	if ( !is_number( argument ) || ( pArea = get_area_data( atoi( argument ) ) ) == NULL )
	{
	    send_to_char( "HEdit:  Area not found.\n\r", ch );
	    return FALSE;
	}
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "HEdit:  Insufficient security to modify area.\n\r", ch );
	return FALSE;
    }

    SET_BIT( pHelp->area->area_flags, AREA_CHANGED );
    pHelp->area = pArea;
    send_to_char( "Area changed.\n\r", ch );
    return TRUE;
}


bool
hedit_desc( CHAR_DATA *ch, char *argument )
{
    HELP_DATA	*pHelp;
    char	 buf[MAX_STRING_LENGTH];
    char	*p;

    EDIT_HELP( ch, pHelp );
    if ( *argument == '\0' )
    {
	string_append( ch, &pHelp->text );
	return TRUE;
    }

    if ( strlen( pHelp->text ) + strlen( argument ) >= sizeof( buf ) - 1 )
    {
	send_to_char( "HEdit:  String too long.\n\r", ch );
	return FALSE;
    }

    p = stpcpy( buf, pHelp->text );
    p = stpcpy( p, argument );
    p = stpcpy( p, "\n" );
    free_string( pHelp->text );
    pHelp->text = str_dup( buf );
    string_append( ch, &pHelp->text );
    return TRUE;
}


bool
hedit_edlevel ( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *	pHelp;
    int		value;

    EDIT_HELP( ch, pHelp );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  edlevel [number]\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );

    if ( value > get_trust( ch ) )
    {
	send_to_char( "HEdit:  Limited to your trust level.\n\r", ch );
	return FALSE;
    }

    pHelp->ed_level = value;

    send_to_char( "Edit level set.\n\r", ch );
    return TRUE;
}


bool
hedit_import( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    char	buf[ MAX_STRING_LENGTH ];
    char	arg[ MAX_INPUT_LENGTH ];

    EDIT_HELP( ch, pHelp );
    argument = first_arg( argument, arg, FALSE );

    if ( arg[0] == '\0' )
	first_arg( pHelp->keyword, arg, FALSE );

    if ( read_desc_file( ch, buf, arg, "hlp", TRUE ) )
	return FALSE;

    free_string( pHelp->text );
    pHelp->text = str_dup( buf );

    hedit_show( ch, "" );
    return TRUE;
}


bool
hedit_level( CHAR_DATA *ch, char *argument )
{
    HELP_DATA	*pHelp;

    EDIT_HELP( ch, pHelp );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    pHelp->level = atoi( argument );

    send_to_char( "Level set.\n\r", ch );
    return TRUE;
}


bool
hedit_name( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *	pHelp;
    HELP_DATA *	pOther;
    char	buf[MAX_INPUT_LENGTH];
    char *	p;

    EDIT_HELP( ch, pHelp );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  keyword  [string] (replaces keys)\n\r", ch );
	send_to_char( "         keyword +[string] (appends to keys)\n\r", ch );
	return FALSE;
    }

    if ( *argument == '+' )
    {
	if ( strlen( argument ) + strlen( pHelp->keyword ) > MAX_INPUT_LENGTH )
	{
	    send_to_char( "HEdit:  Key list too long.\n\r", ch );
	    return FALSE;
	}
	argument++;
	while( isspace( *argument ) )
	    argument++;
	if ( argument[0] == '\0' )
	{
	    send_to_char( "Syntax:  keyword  [string] (replaces keys)\n\r", ch);
	    send_to_char( "         keyword +[string] (appends to keys)\n\r", ch );
	    return FALSE;
	}
	p = stpcpy( buf, pHelp->keyword );
	*p++ = ' ';
	p = stpcpy( p, argument );
	free_string( pHelp->keyword );
	pHelp->keyword = str_dup( buf );

	send_to_char( "Keyword(s) set.\n\r", ch);
	return TRUE;
    }

    if ( ( pOther = get_help( argument, TRUE ) ) != NULL  && pOther != pHelp )
    {
	send_to_char( "Keyword already taken.\n\r", ch );
	return FALSE;
    }

    free_string( pHelp->keyword );
    pHelp->keyword = str_dup( argument );

    send_to_char( "Keyword(s) set.\n\r", ch );
    return TRUE;
}


bool
hedit_delet(CHAR_DATA *ch, char *argument)
{
  send_to_char( "If you want to delete this help, spell it out.\n\r", ch );
  return FALSE;
}


bool
hedit_delete( CHAR_DATA *ch, char *argument )
{
    HELP_DATA	*pHelp;
    HELP_DATA	*pMark;

    EDIT_HELP( ch, pHelp );

    if ( argument[0] != '\0' )
    {
	send_to_char( "Type delete by itself.\n\r", ch );
	return FALSE;
    }

    if ( pHelp == help_first )
	help_first = pHelp->next;
    for ( pMark = help_first; pMark; pMark = pMark->next )
    {
	if ( pHelp == pMark->next )
	{
	    pMark->next = pHelp->next;
	    if ( pHelp == help_last )
	        help_last = pMark;
	    free_help( pHelp );
	    ch->desc->pEdit = NULL;
	    ch->desc->editor = 0;
	    send_to_char( "Deleted.\n\r", ch );
	    return TRUE;
	}
    }

    return FALSE;
}


bool
hedit_show( CHAR_DATA *ch, char *argument )
{
    HELP_DATA	*pHelp;
    BUFFER	*pBuf;

    EDIT_HELP( ch, pHelp );
    pBuf = new_buf( );

    buf_printf( pBuf, "Keyword(s):    [%s]\n\r",
		pHelp->keyword ? all_capitalize( pHelp->keyword ) : "none" );
    add_buf( pBuf,    "Area:          " );
    if ( !pHelp->area )
	add_buf( pBuf, "(No Area)" );
    else if ( pHelp->area == get_area_data( 0 ) )
	add_buf( pBuf, "[0] All" );
    else
	buf_printf( pBuf, "[%d] %s", pHelp->area->vnum, pHelp->area->name );
    add_buf( pBuf, "\n\r" );
    buf_printf( pBuf, "Edit level:    [%d] %s\n\r", pHelp->ed_level,
		level_name( pHelp->ed_level ) );
    buf_printf( pBuf, "Level:         [%d]\n\r", pHelp->level );
    buf_printf( pBuf, "Description:\n\r%s\n\r",
		pHelp->text ? pHelp->text : "none." );
    if ( !pHelp->area )
	add_buf( pBuf, "\n\rNote:  Changes to this help cannot be saved.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}

