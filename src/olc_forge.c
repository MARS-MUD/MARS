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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "merc.h"
#include "db.h"
#include "interp.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"

void
do_rename_obj( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	pObj;
    char	command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax: rename <obj name>.\n\r", ch );
	return;
    }

    if ( !( pObj = get_obj_carry( ch, command, ch ) ) )
    {
	send_to_char( "You don't have that obj.\n\r", ch );
    }
    else
    {
	ch->desc->pEdit = (void *)pObj;
	ch->desc->editor = RENAME_OBJECT;
	rename_obj_show( ch, "" );
    }

    return;
}


void
rename_obj( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	command[MAX_INPUT_LENGTH];
    int		cmd;

    if ( IS_NPC( ch ) )
    {
	interpret( ch, argument );
	return;
    }

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	rename_obj_show( ch, argument );
	return;
    }

    if ( !str_cmp( command, "done" ) )
    {
	edit_done( ch );
	return;
    }

    for ( cmd = 0; *rename_obj_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, rename_obj_table[cmd].name ) )
	{
	    (*rename_obj_table[cmd].olc_fun) ( ch, argument );
	    return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
rename_obj_done( CHAR_DATA *ch, char *argument )
{
    edit_done( ch );
    return FALSE;
}


bool
rename_obj_extra( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *		pObj;
    EXTRA_DESCR_DATA *	ed;
    char		buf[MAX_INPUT_LENGTH];
    char *		p;

    RENAME_OBJ( ch, pObj );

    /* allow room for trailing \n\r */
    if ( strlen( argument ) > sizeof( buf ) - 2 )
    {
	send_to_char( "New description is too long.\n\r", ch );
	return FALSE;
    }

    if ( ( ed = pObj->extra_descr ) == NULL )
    {
	ed = new_extra_descr( );
	ed->keyword = str_dup( pObj->name );
	pObj->extra_descr = ed;
    }

    if ( *argument != '\0' )
    {
	p = stpcpy( buf, argument );
	strcpy( p, "\n\r" );
	free_string( ed->description );
	ed->description = str_dup( buf );
    }

    string_append( ch, &ed->description );

    return TRUE;
}


bool
rename_obj_keyword( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *		pObj;
    EXTRA_DESCR_DATA *	ed;
    char *		p;

    RENAME_OBJ( ch, pObj );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  keyword <new_keyword_list>\n\r", ch );
	return FALSE;
    }

    for ( p = argument; *p != '\0'; p++ )
    {
	if ( is_colcode( p ) )
	{
	    send_to_char( "The keywords may not have color codes.\n\r", ch );
	    return FALSE;
	}
    }

    if ( ( p = string_change( ch, pObj->name, argument ) ) != NULL )
    {
	pObj->name = p;
	if ( ( ed = pObj->extra_descr ) != NULL )
	{
	    free_string( ed->keyword );
	    ed->keyword = str_dup( p );
	}
	send_to_char( "Keywords set.\n\r", ch );
	return TRUE;
    }

    return FALSE;
}


bool
rename_obj_long( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	pObj;
    char	buf[MAX_INPUT_LENGTH];
    char *	p;

    RENAME_OBJ( ch, pObj );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  long <new_long_description>\n\r", ch );
	return FALSE;
    }

    if ( strlen_wo_col( argument ) > 60 )
    {
	send_to_char( "New long description too long.\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pObj->description, argument ) ) != NULL )
    {
	strcpy( buf, p );
	free_string( p );
	p = buf;
	while ( is_colcode( p ) )
	    p += 2;
	*p = UPPER( *p );
	if ( *p == '\0' )
	    strcpy( buf, "An object owned by an idiot lies here." );
	p = buf + strlen( buf ) - 1;
	if ( *p != '.' && *p != '!' )
	    strcat( buf, "." );
	pObj->description = str_dup( buf );
	send_to_char( "Long (in room) description set.\n\r", ch );
	return TRUE;
    }

    return FALSE;
}


bool
rename_obj_short( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *	pObj;
    char	buf[MAX_INPUT_LENGTH];
    char *	p;

    RENAME_OBJ( ch, pObj );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  short <new_long_description>\n\r", ch );
	return FALSE;
    }

    if ( strlen_wo_col( argument ) > 60 )
    {
	send_to_char( "New short description too long.\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pObj->short_descr, argument ) ) != NULL )
    {
	strcpy( buf, p );
	free_string( p );
	p = buf;
	while ( is_colcode( p ) )
	    p += 2;
	if ( *p == '\0' )
	    strcpy( buf, "an object owned by an idiot" );
	pObj->short_descr = str_dup( buf );
	send_to_char( "Short (inventory) description set.\n\r", ch );
	return TRUE;
    }

    return TRUE;
}


bool
rename_obj_show( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    OBJ_DATA *	pObj;

    RENAME_OBJ( ch, pObj );

    pBuf = new_buf( );
    add_buf( pBuf, "\n\rRenaming an item:  Type \"done\" when finished, \"commands\" for commands.\n\r\n\r" );

    buf_printf( pBuf, "Keywords:          %s\n\r", pObj->name );
    buf_printf( pBuf, "Short description: %s`X\n\r", pObj->short_descr );
    buf_printf( pBuf, "Long description:  %s`X\n\r", pObj->description );
    buf_printf( pBuf, "Extra description: " );
    if ( pObj->extra_descr == NULL )
	add_buf( pBuf, "(none)\n\r" );
    else
	buf_printf( pBuf, "\n\r%s", pObj->extra_descr->description );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}

