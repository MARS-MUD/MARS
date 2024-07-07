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
#include "olc.h"
#include "interp.h"
#include "recycle.h"


void
do_bedit( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    char		command[MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs can't do this.\n\r", ch );
	return;
    }

    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	send_to_char( "BEdit:  There is no default board to edit.\n\r", ch );
	return;
    }

    if ( !str_prefix( command, "create" ) )
    {
	if ( bedit_create( ch, argument ) )
	{
	    ch->desc->editor = ED_BOARD;
	    bedit_show( ch, "" );
	}
	return;
    }

    if ( ( pBoard = find_board( ch, command ) ) == NULL )
    {
	send_to_char( "BEdit:  That board does not exist.\n\r", ch );
	return;
    }

    ch->desc->pEdit = (void *)pBoard;
    ch->desc->editor = ED_BOARD;
    bedit_show( ch, "" );

    return;
}


void
bedit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	command[MAX_INPUT_LENGTH];
    int		cmd;

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	bedit_show( ch, "" );
	return;
    }

    if ( !str_cmp( command, "done" ) )
    {
	edit_done( ch );
	save_boards_file( );
	return;
    }

    /* Search table and dispatch command. */
    for ( cmd = 0; *bedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, bedit_table[cmd].name ) )
	{
	    changed_boards |= (*bedit_table[cmd].olc_fun) ( ch, argument );
	    return;
	}
    }

    /* Default to standard interpreter. */
    interpret( ch, arg );
    return;
}


bool
bedit_create( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    char		arg[MAX_INPUT_LENGTH];
    char *		p;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  bedit create <new_board_name>\n\r", ch );
	return FALSE;
    }

    if ( strlen( arg ) > 8 )
    {
	send_to_char( "BEdit:  Name limited to 8 letters.\n\r", ch );
	return FALSE;
    }

    for ( p = arg; *p != '\0'; p++ )
    {
	if ( !isalpha( *p ) )
	{
	    send_to_char( "BEdit:  Name must be letters only.\n\r", ch );
	    return FALSE;
	}
    }

    if ( !str_cmp( arg, "create" ) || !str_cmp( arg, "delete" ) )
    {
	send_to_char( "BEdit:  Board may not be named \"create\" or \"delete\".\n\r", ch );
	return FALSE;
    }

    for ( pBoard = board_first; pBoard != NULL; pBoard = pBoard->next )
    {
	if ( !str_cmp( arg, pBoard->name ) )
	{
	    send_to_char( "BEdit:  Board exists.\n\r", ch );
	    return FALSE;
	}
    }

    /* Okay, looks like a valid name, create the board. */
    arg[0] = UPPER( arg[0] );
    pBoard = new_board( );
    pBoard->name = str_dup( arg );
    if ( board_first == NULL )
	board_first = pBoard;
    else
	board_last->next = pBoard;
    board_last = pBoard;

    ch->desc->pEdit = (void *)pBoard;
    send_to_char( "Board created.\n\r", ch );
    return TRUE;
}


bool
bedit_desc( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    char *		p;

    EDIT_BOARD( ch, pBoard );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  description <new_description>\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pBoard->desc, argument ) ) != NULL )
    {
	pBoard->desc = p;
	send_to_char( "Description set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;
}


bool
bedit_expire( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    int			value;

    EDIT_BOARD( ch, pBoard );

    if ( !is_number( argument ) && str_cmp( argument, "none" )
    &&	 str_cmp( argument, "never" ) )
    {
	send_to_char( "Syntax:  expire [number_of_days]\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );	/* "none/never" will convert to 0 */

    if ( value != 0 && value < 7 )
    {
	send_to_char( "BEdit:  Expiration cannot be less than seven days.\n\r", ch );
	return FALSE;
    }

    pBoard->expire = value;

    send_to_char( "Expiration time set.\n\r", ch );
    return TRUE;
}


bool
bedit_read( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    int			value;

    EDIT_BOARD( ch, pBoard );

    if ( get_trust( ch ) < pBoard->read )
    {
	send_to_char( "BEdit:  Unable to modify read level.\n\r", ch );
	return FALSE;
    }

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  read [number]\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );

    if ( value > get_trust( ch ) )
    {
	send_to_char( "BEdit:  Limited to your trust level.\n\r", ch );
	return FALSE;
    }

    pBoard->read = value;

    send_to_char( "Read level set.\n\r", ch );
    return TRUE;
}


bool
bedit_rename( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    BOARD_DATA *	pList;
    char		arg[MAX_INPUT_LENGTH];
    char *		p;

    EDIT_BOARD( ch, pBoard );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  rename <new_board_name>\n\r", ch );
	return FALSE;
    }

    if ( strlen( arg ) > 8 )
    {
	send_to_char( "BEdit:  Name limited to 8 letters.\n\r", ch );
	return FALSE;
    }

    for ( p = arg; *p != '\0'; p++ )
    {
	if ( !isalpha( *p ) )
	{
	    send_to_char( "BEdit:  Name must be letters only.\n\r", ch );
	    return FALSE;
	}
    }

    if ( !str_cmp( arg, "create" ) || !str_cmp( arg, "delete" ) )
    {
	send_to_char( "BEdit:  Board may not be named \"create\" or \"delete\".\n\r", ch );
	return FALSE;
    }

    for ( pList = board_first; pList != NULL; pList = pList->next )
    {
	if ( !str_cmp( arg, pList->name ) )
	{
	    send_to_char( "BEdit:  Board exists.\n\r", ch );
	    return FALSE;
	}
    }

    arg[0] = UPPER( arg[0] );
    free_string( pBoard->name );
    pBoard->name = str_dup( arg );

    send_to_char( "Board renamed.\n\r", ch );
    return TRUE;
}


bool
bedit_show( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    BUFFER *		pBuf;

    EDIT_BOARD( ch, pBoard );

    pBuf = new_buf( );
    show_board( ch, pBoard, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}


bool
bedit_write( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    int			value;

    EDIT_BOARD( ch, pBoard );

    /*if ( get_trust( ch ) < pBoard->write )
    {
	send_to_char( "BEdit:  Unable to modify write level.\n\r", ch );
	return FALSE;
    }*/

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  write [number]\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );

    if ( value > get_trust( ch ) )
    {
	send_to_char( "BEdit:  Limited to your trust level.\n\r", ch );
	return FALSE;
    }

    pBoard->write = value;

    send_to_char( "Write level set set.\n\r", ch );
    return TRUE;
}


void
show_board( CHAR_DATA *ch, BOARD_DATA *pBoard, BUFFER *pBuf )
{
    buf_printf( pBuf, "Board:         [%8s]\n\r", pBoard->name );
    buf_printf( pBuf, "Description:   [%s]\n\r", pBoard->desc );
    buf_printf( pBuf, "Min read lvl:  [%d]\n\r", pBoard->read );
    buf_printf( pBuf, "Min write lvl: [%d]\n\r", pBoard->write );
    if ( pBoard->expire != 0 )
	buf_printf( pBuf, "Expires in:    [%d] days\n\r", pBoard->expire );
    else
	buf_printf( pBuf, "Expires:       [never]\n\r" );
    return;
}


