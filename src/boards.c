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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"


/* Local functions */
static void append_note( NOTE_DATA *pNote );
static bool attach_note( CHAR_DATA *ch, BOARD_DATA *pBoard );
static const char * board_access_string( CHAR_DATA *ch, BOARD_DATA *pBoard );
static void delete_note( NOTE_DATA *pNote );
static BOARD_DATA * get_current_board( CHAR_DATA *ch );
static time_t get_timestamp( void );
static bool hide_note( CHAR_DATA *ch, NOTE_DATA *pNote );
static bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pNote );

static void note_catchup( CHAR_DATA *ch, BOARD_DATA *pBoard );
static void note_clear	( CHAR_DATA *ch );
static void note_delete	( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_forward( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_help	( CHAR_DATA *ch );
static void note_list	( CHAR_DATA *ch, BOARD_DATA *pBoard );
static void note_minus	( CHAR_DATA *ch, BOARD_DATA *pBoard );
static void note_plus	( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_protect( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_read	( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_remove	( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_reply	( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_send	( CHAR_DATA *ch );
static void note_show	( CHAR_DATA *ch );
static void note_subject( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_take	( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument, char *cmd );
static void note_to	( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument );
static void note_write	( CHAR_DATA *ch, BOARD_DATA *pBoard );

static void update_read( CHAR_DATA *ch, NOTE_DATA *pNote );
static void write_board( BOARD_DATA *pBoard );


void
do_boards( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    BOARD_INFO *	pInfo;
    char		arg[MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs can't do this.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	show_board_list( ch, TRUE );
	return;
    }

    if ( !str_cmp( arg, "new" ) )
    {
	show_board_list( ch, FALSE );
	return;
    }

    if ( ( !str_cmp( arg, "reset" ) || !str_cmp( arg, "clear" ) )
    &&	 !str_cmp( argument, "unread" ) )
    {
	while ( ch->pcdata->last_read != NULL )
	{
	    pInfo = ch->pcdata->last_read;
	    ch->pcdata->last_read = pInfo->next;
	    free_board_info( pInfo );
	}
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( ( pBoard = find_board( ch, arg ) ) == NULL )
    {
	send_to_char( "Board not found.\n\r", ch );
	return;
    }

    free_string( ch->pcdata->current_board );
    ch->pcdata->current_board = str_dup( pBoard->name );


    ch_printf( ch, "`GCurrent board set to `R%s`G.\n\r", pBoard->name );
    ch_printf( ch, "You can %s this board.`X\n\r",
	       board_access_string( ch, pBoard ) );
    return;
}


void
do_note( CHAR_DATA *ch, char *argument )
{
    BOARD_DATA *	pBoard;
    char		arg[MAX_INPUT_LENGTH];

    if ( ch->pnote != NULL && ch->pnote->board == NULL )
    {
	send_to_char( "You cannot use new note commands with old notes.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( ( pBoard = get_current_board( ch ) ) == NULL )
    {
	send_to_char( "I can't find your current board!\n\r", ch );
	return;
    }

    /* ugly hack to let mobs post notes on noteboards */
    if ( IS_NPC( ch )
    &&	 str_cmp( arg, "clear" )
    &&	 str_cmp( arg, "to" )
    &&	 str_cmp( arg, "subject" )
    &&	 str_cmp( arg, "+" )
    &&	 str_cmp( arg, "post" )
    &&	 str_cmp( arg, "send" ) )
    {
	send_to_char( "Mobiles can't do that.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )		/* no arg defaults to "note read" */
    {
	note_read( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "cancel" ) || !str_prefix( arg, "clear" ) )
    {
	note_clear( ch );
	return;
    }

    if ( !str_prefix( arg, "catchup" ) )
    {
	note_catchup( ch, pBoard );
	return;
    }

    if ( !str_prefix( arg, "copy" ) )	/* synonym for "note take" */
    {
	note_take( ch, pBoard, argument, "Copy" );
	return;
    }

    if ( str_match( arg, "del", "delete" ) && get_trust( ch ) >= MAX_LEVEL - 1 )
    {
	note_delete( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "forward" ) )
    {
	note_forward( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "list" ) )
    {
	note_list( ch, pBoard );
	return;
    }

    if ( !str_prefix( arg, "minus" ) || !str_cmp( arg, "-" ) )
    {
	note_minus( ch, pBoard );
	return;
    }

    if ( !str_prefix( arg, "protect" )
    &&	 ( get_trust( ch ) >= ANGEL || IS_CODER( ch ) ) )
    {
	note_protect( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "plus" ) || !str_cmp( arg, "+" ) )
    {
	note_plus( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "read" ) )
    {
	note_read( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "post" ) || !str_prefix( arg, "send" ) )
    {
	note_send( ch );
	return;
    }

    if ( !str_prefix( arg, "remove" ) )
    {
	note_remove( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "reply" ) )
    {
	note_reply( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "show" ) )
    {
	note_show( ch );
	return;
    }

    if ( !str_prefix( arg, "subject" ) )
    {
	note_subject( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "to" ) )
    {
	note_to( ch, pBoard, argument );
	return;
    }

    if ( !str_prefix( arg, "take" ) )
    {
	note_take( ch, pBoard, argument, "Take" );
	return;
    }

    if ( !str_prefix( arg, "write" ) )
    {
	note_write( ch, pBoard );
	return;
    }

    /* Command not found */
    note_help( ch );

}


void
do_unread( CHAR_DATA *ch, char *argument )
{
    do_boards( ch, "new" );
    return;
}


static void
append_note( NOTE_DATA *pNote )
{
    BOARD_DATA *	pBoard;
    char *		strtime;

    pNote->date_stamp = get_timestamp( );
    strtime = ctime( &pNote->date_stamp );
    *(strtime + strlen( strtime ) - 1) = '\0';
    pNote->date = str_dup( strtime );

    pBoard = pNote->board;
    pNote->next = NULL;

    if ( pBoard->note_first == NULL )
	pBoard->note_first = pNote;
    else
	pBoard->note_last->next = pNote;
    pBoard->note_last = pNote;

    write_board( pBoard );
    return;
}


static bool
attach_note( CHAR_DATA *ch, BOARD_DATA *pBoard )
{
    NOTE_DATA *	pNote;

    if ( pBoard == NULL || get_trust( ch ) < pBoard->write )
	return FALSE;

    if ( ch->pnote != NULL )
	return TRUE;

    pNote = new_note( );
    pNote->sender = str_dup( ch->name );
    pNote->board = pBoard;
    ch->pnote = pNote;
    return TRUE;
}


static const char *
board_access_string( CHAR_DATA *ch, BOARD_DATA *pBoard )
{
    bool	fRead;
    bool	fWrite;
    int		trust;

    trust = get_trust( ch );
    fRead  = trust >= pBoard->read;
    fWrite = trust >= pBoard->write;

    if ( fRead && fWrite )
	return "both read and post on";
    if ( fRead && !fWrite )
	return "only read";
    if ( !fRead && fWrite )
	return "only post on";
    return( "neither read nor post on" );
}


BOARD_DATA *
board_lookup( const char *name )
{
    BOARD_DATA *	pBoard;

    for ( pBoard = board_first; pBoard != NULL; pBoard = pBoard->next )
	if ( !str_cmp( name, pBoard->name ) )
	    break;

    return pBoard;
}


static void
delete_note( NOTE_DATA *pNote )
{
    BOARD_DATA *	pBoard;
    NOTE_DATA *		pPrev;

    pBoard = pNote->board;
    if ( pBoard == NULL )
	return;

    if ( pNote == pBoard->note_first )
    {
	pBoard->note_first = pNote->next;
	if ( pBoard->note_first == NULL )
	    pBoard->note_last = NULL;
    }
    else
    {
	for ( pPrev = pBoard->note_first; pPrev != NULL; pPrev = pPrev->next )
	    if ( pPrev->next == pNote )
		break;

	if ( pPrev == NULL )
	{
	    bug( "Remove_note:  Note not found.", 0 );
	    return;
	}

	pPrev->next = pNote->next;
	if ( pBoard->note_last == pNote )
	    pBoard->note_last = pPrev;
    }

    free_note( pNote );
    return;
}


BOARD_DATA *
find_board( CHAR_DATA *ch, const char *name )
{
    BOARD_DATA *	pBoard;
    int			count;
    int			trust;
    int			vnum;

    if ( IS_NULLSTR( name ) )
	return NULL;

    if ( is_number( name ) )
	vnum = atoi( name );
    else
	vnum = -1;

    count = 0;
    trust = get_trust( ch );

    for ( pBoard = board_first; pBoard != NULL; pBoard = pBoard->next )
    {
	if ( pBoard->read > trust && pBoard->write > trust )
	    continue;
	if ( ++count == vnum || !str_prefix( name, pBoard->name ) )
	    break;
    }

    return pBoard;
}


BOARD_INFO *
get_board_info( CHAR_DATA *ch, const char *name )
{
    BOARD_INFO *	pInfo;

    if ( IS_NPC( ch ) || IS_NULLSTR( name ) )
	return NULL;

    for ( pInfo = ch->pcdata->last_read; pInfo != NULL; pInfo = pInfo->next )
	if ( !str_cmp( name, pInfo->name ) )
	    break;

    return pInfo;
}


static BOARD_DATA *
get_current_board( CHAR_DATA *ch )
{
    BOARD_DATA *	pBoard;
    BOARD_INFO *	pInfo;

    if ( IS_NPC( ch ) )
	return board_first;

    if ( ( pBoard = board_lookup( ch->pcdata->current_board ) ) == NULL )
	pBoard = next_board( ch, NULL );

    if ( pBoard == NULL )
	return NULL;	/* no boards available */

    if ( get_board_info( ch, pBoard->name ) == NULL )
    {
	pInfo = new_board_info( );
	pInfo->name = str_dup( pBoard->name );
	pInfo->next = ch->pcdata->last_read;
	ch->pcdata->last_read = pInfo;
    }

    if ( str_cmp( ch->pcdata->current_board, pBoard->name ) )
    {
	free_string( ch->pcdata->current_board );
	ch->pcdata->current_board = str_dup( pBoard->name );
    }

    return pBoard;
}


char *
get_note_text( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		vnum;
    int		note_num;
    BOARD_DATA *pBoard;
    NOTE_DATA *	pnote;
    NOTE_DATA *	list;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return NULL;

    if ( ( pBoard = get_current_board( ch ) ) == NULL )
	return NULL;

    if ( is_number( arg ) && *argument == '\0' )
    {
	vnum = atoi( arg );
	list = pBoard->note_first;
    }
    else
    {
	if ( !is_number( argument ) )
	    return NULL;

	vnum = atoi( argument );
	if ( vnum < 0 )
	    return NULL;

	if ( ( pBoard = find_board( ch, arg ) ) == NULL )
	    return NULL;

	list = pBoard->note_first;
    }

    note_num = 0;

    for ( pnote = list; pnote != NULL; pnote = pnote->next )
	if ( is_note_to( ch, pnote ) && note_num++ == vnum )
	    return pnote->text;

    return NULL;
}


static time_t
get_timestamp( void )
{
    static time_t	last_timestamp;
    time_t		val;

    val = current_time <= last_timestamp ? last_timestamp + 1 : current_time;
    last_timestamp = val;
    return val;
}


static bool
hide_note( CHAR_DATA *ch, NOTE_DATA *pNote )
{
    BOARD_INFO *	pInfo;

    if ( IS_NPC( ch ) )
	return TRUE;

    if ( ( pInfo = get_board_info( ch, pNote->board->name ) ) == NULL )
    {
	pInfo = new_board_info ( );
	pInfo->name = str_dup( pNote->board->name );
	pInfo->next = ch->pcdata->last_read;
	ch->pcdata->last_read = pInfo;
    }

    if ( pNote->date_stamp <= pInfo->time
    ||	 !str_cmp( ch->name, pNote->sender )
    ||	 !is_note_to( ch, pNote ) )
	return TRUE;

    return FALSE;
}


static bool
is_note_to( CHAR_DATA *ch, NOTE_DATA *pNote )
{
    char *list;
    char  name[MAX_INPUT_LENGTH];
    int	  minlevel;
    int   maxlevel;
    int   nums;

    if ( pNote->board == NULL )
	return FALSE;

    if ( !str_cmp( ch->name, pNote->sender ) )
	return TRUE;

    if ( get_trust( ch ) < pNote->board->read )
	return FALSE;

    list = pNote->to_list;
    nums = 0;
    minlevel = 1;
    maxlevel = MAX_LEVEL;

    while ( *list != '\0' )
    {
	list = one_argument( list, name );

	strip_trailing_comma( name );

	if ( !str_cmp( name, "all" ) )
	    return TRUE;

	if ( !str_cmp( name, ch->name ) )
	    return TRUE;

	if ( IS_IMMORTAL( ch ) && str_match( name, "imm", "immortals" ) )
	    return TRUE;

	if ( IS_IMMORTAL( ch ) && !str_cmp( name, "imms" ) )
	    return TRUE;

	if ( ch->level >= SUPREME && str_match( name, "admin", "administratorss" ) )
	    return TRUE;

	if ( ch->level >= SUPREME && !str_cmp( name, "admins" ) )
	    return TRUE;

	if ( ch->level >= IMPLEMENTOR && str_match( name, "imp", "implementors" ) )
	    return TRUE;

	if ( ch->level >= IMPLEMENTOR && !str_cmp( name, "imps" ) )
	    return TRUE;

	if ( ( IS_CODER( ch ) || ch->level >= IMPLEMENTOR )
	&&   str_match( name, "coder", "coders" ) )
	    return TRUE;

	if ( ch->clan != NULL && !str_cmp( name, ch->clan->name ) )
	    return TRUE;

	if ( is_number( name ) )
	{
	    nums++;
	    switch( nums )
	    {
		case 1:	minlevel = atoi( name ); break;
		case 2: maxlevel = atoi( name ); break;
	    }
	    continue;
	}

    }

    if ( nums == 1 && ch->level >= minlevel )
	return TRUE;

    if ( nums > 1 && get_trust( ch ) >= minlevel && ch->level <= maxlevel )
	return TRUE;

    return FALSE;
}


BOARD_DATA *
next_board( CHAR_DATA *ch, BOARD_DATA *pBoard )
{
    int		trust;

    if ( IS_NPC( ch ) )
	return NULL;

    trust = get_trust( ch );

    if ( pBoard == NULL )
	pBoard = board_first;
    else
	pBoard = pBoard->next;

    while ( pBoard != NULL )
    {
	if ( trust >= pBoard->read || trust >= pBoard->write )
	    break;
	pBoard = pBoard->next;
    }

    return pBoard;
}


static void
note_catchup( CHAR_DATA *ch, BOARD_DATA *pBoard )
{
    BOARD_INFO *	pInfo;

    if ( IS_NPC( ch ) )
	return;

    if ( ( pInfo = get_board_info( ch, pBoard->name ) ) == NULL )
    {
	pInfo = new_board_info( );
	pInfo->name = str_dup( pBoard->name );
	pInfo->next = ch->pcdata->last_read;
	ch->pcdata->last_read = pInfo;
    }

    pInfo->time = current_time;
    send_to_char( "Ok.\n\r", ch );
    return;
}


static void
note_clear( CHAR_DATA *ch )
{
    if ( ch->pnote != NULL )
    {
	free_note( ch->pnote );
	ch->pnote = NULL;
    }
    send_to_char( "Ok.\n\r", ch );
    return;
}


static void
note_delete( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    NOTE_DATA *	pNote;
    int		anum;
    int		vnum;

    if ( !is_number( argument ) )
    {

	send_to_char( "Delete which note?\n\r", ch );
	return;
    }

    anum = atoi( argument );
    vnum = 0;

    for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
	if ( is_note_to( ch, pNote ) && ++vnum == anum )
	    break;

    if ( pNote == NULL )
    {
	send_to_char( "Note not found.\n\r", ch );
	return;
    }

    delete_note( pNote );
    write_board( pBoard );
    send_to_char( "Ok.\n\r", ch );
    return;
}


static void
note_forward( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    BOARD_DATA *	newboard;
    BUFFER *		pBuf;
    CHAR_DATA *		rch;
    DESCRIPTOR_DATA *	d;
    NOTE_DATA *		oldnote;
    NOTE_DATA *		newnote;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH + 10];
    char *		p;
    int			anum;
    int			vnum;

    argument = one_argument( argument, arg1 );
    if ( !is_number( arg1 ) || *argument == '\0' )
    {
	send_to_char( "Forward which note to whom/where?\n\r", ch );
	return;
    }

    anum = atoi( arg1 );
    vnum = 0;
    for ( oldnote = pBoard->note_first; oldnote != NULL; oldnote = oldnote->next )
	if ( is_note_to( ch, oldnote ) && ++vnum == anum )
	    break;

    if ( oldnote == NULL )
    {
	send_to_char( "Note not found.\n\r", ch );
	return;
    }

    p = one_argument( argument, arg2 );
    if ( *p == '\0'
    ||	 !str_cmp( arg2, "all" )
    ||	 ( newboard = find_board( ch, arg2 ) ) == NULL )
    {
	newboard = pBoard;
    }
    else
    {
	/* valid new board specified, remove from to_list */
	argument = p;
    }

    if ( get_trust( ch ) < newboard->write )
    {
	ch_printf( ch, "You may not post on %s board.\n\r",
		   newboard == pBoard ? "this" : "that" );
	return;
    }

    newnote = new_note( );
    newnote->sender = str_dup( ch->name );
    newnote->board = newboard;
    newnote->to_list = str_dup( argument );

    if ( !strncmp( oldnote->subject, "Fwd: ", 5 ) )
	p = oldnote->subject + 5;
    else
	p = oldnote->subject;

    sprintf( buf, "Fwd: %s", p );
    newnote->subject = str_dup( buf );

    pBuf = new_buf( );

    add_buf( pBuf, "\n\rForwarded " );
    if ( pBoard == newboard )
	add_buf( pBuf, "note" );
    else
	buf_printf( pBuf, "%s", pBoard->name );
    add_buf( pBuf, ":\n\r\n\r" );

    buf_printf( pBuf, "%s: %s\n\r", oldnote->sender, oldnote->subject );
    buf_printf( pBuf, "%s\n\r", oldnote->date );
    buf_printf( pBuf, "To: %s\n\r", oldnote->to_list );
    add_buf( pBuf, oldnote->text );
    newnote->text = str_dup( buf_string( pBuf ) );
    free_buf( pBuf );

    append_note( newnote );

    act_color( AT_GREEN, "You forward a note on `R$t`X.", ch, newboard->name, NULL, TO_CHAR, POS_SLEEPING );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	rch = d->original != NULL ? d->original : d->character;
	if ( d->connected == CON_PLAYING
	&&   is_note_to( rch, newnote ) )
	{
	    act_color( AT_GREEN, "A note has been forwarded on `R$t`X by $n!",
		       ch, newboard->name, d->character, TO_VICT, POS_SLEEPING );
	}
    }

}


static void
note_help( CHAR_DATA *ch )
{
    send_to_char(
"Reading notes:\n\r"
"NOTE or NOTE READ   - read next note on current board.\n\r"
"NOTE LIST           - list notes on current board.\n\r"
"\n\r"
"Writing notes:\n\r"
"NOTE TO             - sets the addressee(s) of a note.\n\r"
"NOTE SUBJECT        - Set the subject of a note.\n\r"
"NOTE + or NOTE PLUS - add a line of text to the note.\n\r"
"NOTE POST           - Finish writing the note and post it.\n\r"
"NOTE WRITE          - use the text editor to write the text of a note.\n\r"
"                      (alternative to NOTE +)\n\r"
"\n\r"
"Abandoning a note in progress:\n\r"
"NOTE CLEAR          - Cancel the note being written without sending it.\n\r"
"NOTE CANCEL         - Alias for NOTE CLEAR.\n\r"
"\n\r"
"See also HELP NOTE and HELP BOARDS.\n\r",
    ch );
}


static void
note_list( CHAR_DATA *ch, BOARD_DATA *pBoard )
{
    BUFFER *	pBuf;
    NOTE_DATA *	pNote;
    struct tm	ndate;
    int		vnum;

    vnum = 0;
    pBuf = new_buf( );
    for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
    {
	if ( is_note_to( ch, pNote ) )
	{
	    vnum++;
	    ndate = *localtime( &pNote->date_stamp );
	    buf_printf( pBuf, "`g[`W%3d%s%s`g] `G%02d/%02d/%02d `Y%s`G: %s\n\r",
			vnum,
			hide_note( ch, pNote ) ? " " : "`GN",
			( get_trust( ch ) >= ANGEL || IS_CODER( ch ) ) ?
			  IS_SET( pNote->flags, NOTE_PROTECTED ) ? "`pP" : " " : "",
			ndate.tm_mon + 1,
			ndate.tm_mday,ndate.tm_year % 100,
			pNote->sender,
			pNote->subject );
	}
    }
    if ( vnum == 0 )
	buf_printf( pBuf, "`GNo notes found on `R%s`G.\n\r", pBoard->name );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


static void
note_minus( CHAR_DATA *ch, BOARD_DATA *pBoard )
{
    char	buf[MAX_STRING_LENGTH];
    int		len;
    bool	found;

    if ( !attach_note( ch, pBoard ) )
    {
	send_to_char( "You may not post on this board.\n\r", ch );
	return;
    }

    if ( IS_NULLSTR( ch->pnote->text ) )
    {
	send_to_char( "No lines left to remove.\n\r", ch );
	return;
    }

    strcpy( buf, ch->pnote->text );
    found = FALSE;

    for ( len = strlen( buf ); len > 0; len-- )
    {
	if ( buf[len] == '\r' )
	{
	    if ( !found )  /* found end of line to be deleted */
	    {
		if ( len > 0 )
		    len--;
		found = TRUE;
	    }
	    else	  /* found end of line before line to be deleted */
	    {
		buf[len + 1] = '\0';
		free_string( ch->pnote->text );
		ch->pnote->text = str_dup( buf );
		send_to_char( "Ok.\n\r", ch );
		return;
	    }
	}
    }

    /* only one line of text found in buffer */
    buf[0] = '\0';
    free_string( ch->pnote->text );
    ch->pnote->text = str_dup( buf );
    send_to_char( "Ok.\n\r", ch );
    return;
}


static void
note_plus( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    BUFFER *	pBuf;

    if ( !attach_note( ch, pBoard ) )
    {
	send_to_char( "You may not post on this board.\n\r", ch );
	return;
    }

    if ( strlen( ch->pnote->text ) + strlen( argument ) >= MAX_STRING_LENGTH - 4 )
    {
	send_to_char( "Note too long.\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    add_buf( pBuf, ch->pnote->text );
    add_buf( pBuf, argument );
    add_buf( pBuf, "\n\r" );
    free_string( ch->pnote->text );
    ch->pnote->text = str_dup( buf_string( pBuf ) );
    free_buf( pBuf );
    send_to_char( "Okay.\n\r", ch );
    return;
}


static void
note_protect( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    NOTE_DATA *	pNote;
    int		anum;
    int		vnum;

    if ( !is_number( argument ) )
    {
	send_to_char( "Protect which note?\n\r", ch );
	return;
    }

    anum = atoi( argument );
    vnum = 0;

    for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
	if ( is_note_to( ch, pNote ) && ++vnum == anum )
	    break;

    if ( pNote == NULL )
    {
	send_to_char( "Note not found.\n\r", ch );
	return;
    }

    TOGGLE_BIT( pNote->flags, NOTE_PROTECTED );
    write_board( pBoard );
    ch_printf( ch, "Note %sprotected.\n\r",
	       IS_SET( pNote->flags, NOTE_PROTECTED ) ? "" : "un-" );
    return;
}


static void
note_read( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    BOARD_DATA *	start_board;
    BUFFER *		pBuf;
    NOTE_DATA *		pNote;
    char *		old;
    int			anum;
    int			vnum;

    if ( *argument == '\0' || !str_prefix( argument, "next" ) )
    {
	if ( pBoard == NULL )
	{
	    send_to_char( "No current board?!?\n\r", ch );
	    return;
	}

	start_board = pBoard;

	for ( ; ; )
	{
	    vnum = 1;
	    for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
	    {
		if ( !hide_note( ch, pNote ) )
		{
		    pBuf = new_buf( );
		    buf_printf( pBuf, "\n\r`g[`W%3d`g] `Y%s`G: %s\n\r`G%s\n\rTo: %s\n\r",
				vnum,
				pNote->sender,
				pNote->subject,
				pNote->date,
				pNote->to_list );
		    add_buf( pBuf, pNote->text );
		    page_to_char( buf_string( pBuf ), ch );
		    free_buf( pBuf );
		    update_read( ch, pNote );
		    return;
		}
		else if ( is_note_to( ch, pNote ) )
		    vnum++;
	    }

	    old = pBoard->name;
	    if ( ( pBoard = next_board( ch, pBoard ) ) == NULL )
		pBoard = next_board( ch, NULL ); /* wrap around to 1st board */
	    if ( pBoard == NULL || pBoard == start_board )
	    {
		ch_printf( ch,
			   "`GNo new notes on `R%s`G, returning to `R%s`G.\n\r",
			   old, pBoard->name );
//	uncomment to reset to first board
//		pBoard = next_board( ch, NULL );
		if ( pBoard != NULL )
		{
		    free_string( ch->pcdata->current_board );
		    ch->pcdata->current_board = str_dup( pBoard->name );
		}
		return;
	    }

	    free_string( ch->pcdata->current_board );
	    ch->pcdata->current_board = str_dup( pBoard->name );
	    ch_printf( ch, "`GNo new notes on `R%s`G, checking `R%s`G...\n\r",
		       old, pBoard->name );
	}

	return;
    }

    else if ( is_number( argument ) )
    {
	anum = atoi( argument );
	vnum = 0;

	for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
	{
	    if ( is_note_to( ch, pNote ) && ++vnum == anum )
	    {
		pBuf = new_buf( );
		buf_printf( pBuf, "\n\r`g[`W%3d`g] `Y%s`G: %s\n\r`G%s\n\rTo: %s\n\r",
			    vnum,
			    pNote->sender,
			    pNote->subject,
			    pNote->date,
			    pNote->to_list );
		add_buf( pBuf, pNote->text );
		page_to_char( buf_string( pBuf ), ch );
		free_buf( pBuf );
		update_read( ch, pNote );
		return;
	    }
	}

	send_to_char( "Note not found.\n\r", ch );
	return;
    }
    else
    {
	send_to_char( "Read which number?\n\r", ch );
	return;
    }

}


static void
note_remove( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    NOTE_DATA *	pNote;
    char *	to_list;
    char	to_new[MAX_INPUT_LENGTH];
    char	to_one[MAX_INPUT_LENGTH];
    char *	p;
    int		anum;
    int		vnum;
    bool	found;

    if ( !is_number( argument ) )
    {
	send_to_char( "Remove which note?\n\r", ch );
	return;
    }

    anum = atoi( argument );
    vnum = 0;

    for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
    {
	if ( is_note_to( ch, pNote ) && ++vnum == anum )
	    break;
    }

    if ( pNote == NULL )
    {
	send_to_char( "Note not found.\n\r", ch );
	return;
    }

    to_new[0] = '\0';
    to_list = pNote->to_list;
    p = to_new;
    found = FALSE;

    while ( *to_list != '\0' )
    {
	to_list = first_arg( to_list, to_one, FALSE );
	strip_trailing_comma( to_one );
	if ( to_one[0] != '\0' )
	{
	    if ( str_cmp( ch->name, to_one ) )
	    {
		*p++ = ' ';
		p = stpcpy( p, to_one );
	    }
	    else
	    {
		found = TRUE;
	    }
	}
    }

    if ( str_cmp( ch->name, pNote->sender ) && to_new[0] != '\0' )
    {
	if ( found )
	{
	    free_string( pNote->to_list );
	    pNote->to_list = str_dup( to_new + 1 );
	}
	else
	{
	    send_to_char( "That note is neither from nor to you.\n\r", ch );
	    return;
	}
    }
    else
    {
	delete_note( pNote );
    }

    write_board( pBoard );
    send_to_char( "Ok.\n\r", ch );
    return;
}


static void
note_reply( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    NOTE_DATA *	oldnote;
    NOTE_DATA *	newnote;
    char	buf[MAX_STRING_LENGTH*2];
    char	to_name[MAX_INPUT_LENGTH];
    int		anum;
    int		vnum;
    char *	p;
    char *	q;

    if ( !is_number( argument ) )
    {
	send_to_char( "Reply to which note?\n\r", ch );
	return;
    }

    anum = atoi( argument );
    vnum = 0;

    for ( oldnote = pBoard->note_first; oldnote != NULL; oldnote = oldnote->next )
    {
	if ( is_note_to( ch, oldnote ) && ++vnum == anum )
	    break;
    }

    if ( oldnote == NULL )
    {
	send_to_char( "Note not found.\n\r", ch );
	return;
    }

    if ( !attach_note( ch, oldnote->board ) )
    {
        send_to_char( "You may not post on this board.\n\r", ch );
        return;
    }

    newnote = ch->pnote;
    if ( !strncmp( oldnote->subject, "Re: ", 4 ) )
        strcpy( buf, oldnote->subject );
    else
        sprintf( buf, "Re: %s", oldnote->subject );
    free_string( newnote->subject );
    newnote->subject = str_dup( buf );

    strcpy( buf, oldnote->sender );
    p = buf + strlen( buf );
    q = one_argument( oldnote->to_list, to_name );
    while ( to_name[0] != '\0' )
    {
        if ( str_cmp( to_name, oldnote->sender ) && str_cmp( to_name, ch->name ) )
        {
            *p++ = ' ';
            p = stpcpy( p, to_name );
        }
        q = one_argument( q, to_name );
    }
    *p = '\0';
    free_string( newnote->to_list );
    newnote->to_list = str_dup( buf );

    sprintf( buf, ">On %s, %s wrote:\n\r", oldnote->date,
		       oldnote->sender );
    p = buf + strlen ( buf );
    q = oldnote->text;

    while ( *q != '\0' )
    {
        if ( *q == '>' )
        {
            do
                q++;
            while ( *q != '\r' && *q != '\0' );
            if ( *q == '\r' )
                q++;
            continue;
        }
        *p++ = '>';
        while ( *q != '\r' && *q != '\0' )
            *p++ = *q++;
        if ( *q == '\r' )
        {
            *p++ = '\r';
            q++;
        }
    }
    *p = '\0';

    if ( strlen( buf ) > MAX_STRING_LENGTH - 256 )
    {
        send_to_char( "Note too long to quote reply.\n\r", ch );
        free_note( ch->pnote );
        ch->pnote = NULL;
        return;
    }
    free_string( newnote->text );
    newnote->text = str_dup( buf );

    string_append( ch, &ch->pnote->text );
    return;
}


static void
note_send( CHAR_DATA *ch )
{
    NOTE_DATA *		pNote;
    DESCRIPTOR_DATA *	d;
    CHAR_DATA *		rch;

    if ( ( pNote = ch->pnote ) == NULL )
    {
	send_to_char( "You have no note in progress.\n\r", ch );
	return;
    }

    if ( IS_NULLSTR( pNote->to_list ) )
    {
	send_to_char( "You need to provide a recipient (\"note to ...\").\n\r", ch );
	return;
    }

    if ( IS_NULLSTR( pNote->subject ) )
    {
	send_to_char( "You need to provide a subject (\"note subject ...\").\n\r", ch );
	return;
    }

    append_note( pNote );

    act_color( AT_GREEN, "You post a note on `R$t`X.", ch,
	       pNote->board->name, NULL, TO_CHAR, POS_SLEEPING );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	rch = d->original != NULL ? d->original : d->character;
	if ( d->connected == CON_PLAYING
	&&   is_note_to( rch, pNote ) )
	{
	    act_color( AT_GREEN, "A note has been posted on `R$t`X by $n!",
		       ch, pNote->board->name, d->character, TO_VICT, POS_SLEEPING );
	}
    }

    ch->pnote = NULL;
    return;
}


static void
note_show( CHAR_DATA *ch )
{
    BUFFER *	pBuf;

    if ( ch->pnote == NULL )
    {
	send_to_char( "You have no note in progress.\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    buf_printf( pBuf, "`GBoard: `R%s`G\n\r", ch->pnote->board->name );
    buf_printf( pBuf, "`Y%s`G: %s\n\r", ch->pnote->sender, ch->pnote->subject );
    buf_printf( pBuf, "`GTo: %s\n\r", ch->pnote->to_list );
    add_buf( pBuf, ch->pnote->text );
    page_to_char( buf_string( pBuf ), ch);
    free_buf( pBuf );
    return;
}


static void
note_subject( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    if ( !attach_note( ch, pBoard ) )
    {
	send_to_char( "You may not post on this board.\n\r", ch );
	return;
    }

    free_string( ch->pnote->subject );
    ch->pnote->subject = str_dup( argument );
    send_to_char( "Ok.\n\r", ch );
    return;
}


static void
note_take( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument, char *cmd )
{
    char		buf[MAX_INPUT_LENGTH];
    BUFFER *		pBuf;
    NOTE_DATA *		pNote;
    EXTRA_DESCR_DATA *	note_text;
    OBJ_DATA *		ink;
    OBJ_DATA *		paper;
    OBJ_DATA *		note_obj;
    int			anum;
    int			vnum;

    if ( !is_number( argument ) )
    {
	ch_printf( ch, "%s which note?\n\r", cmd );
	return;
    }

    anum = atoi( argument );
    vnum = 0;

    for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
	if ( is_note_to( ch, pNote ) && ++vnum == anum )
	    break;

    if ( pNote == NULL )
    {
	send_to_char( "Note not found.\n\r", ch );
	return;
    }

    for ( paper = ch->carrying; paper != NULL; paper = paper->next_content )
	if ( paper->item_type == ITEM_PAPER )
	    break;

    if ( paper == NULL && !IS_IMMORTAL( ch ) )
    {
	send_to_char( "You have no paper.\n\r", ch );
	return;
    }

    for ( ink = ch->carrying; ink != NULL; ink = ink->next_content )
	if ( ink->item_type == ITEM_INK && ink->value[1] > 0 )
	    break;

    if ( ink == NULL && !IS_IMMORTAL( ch ) )
    {
	for ( ink = ch->carrying; ink != NULL; ink = ink->next_content )
	    if ( ink->item_type == ITEM_INK )
		break;
	if ( ink == NULL )
	    send_to_char( "You have no ink.\n\r", ch );
	else
	    act_color( AT_GREEN, "$p`X is empty.", ch, ink, NULL, TO_CHAR, POS_DEAD );
	return;
    }

    note_obj = create_object( get_obj_index( OBJ_VNUM_NOTE ), 1 );

    sprintf( buf, "note %s %s", pBoard->name, pNote->sender );
    free_string( note_obj->name );
    note_obj->name = str_dup( buf );


    sprintf( buf, "a note from %s", pNote->sender );
    free_string( note_obj->short_descr );
    note_obj->short_descr = str_dup( buf );

    sprintf( buf, "A note from %s lies here.", pNote->sender );
    free_string( note_obj->description );
    note_obj->description = str_dup( buf );

    note_text = new_extra_descr( );
    note_text->keyword = str_dup( note_obj->name );
    pBuf = new_buf( );

    buf_printf( pBuf, "%s: %s\n\r", pNote->sender, pNote->subject );
    buf_printf( pBuf, "%s\n\r", pNote->date );
    buf_printf( pBuf, "To: %s\n\r", pNote->to_list );
    add_buf( pBuf, pNote->text );
    free_string( note_text->description );
    note_text->description = str_dup( buf_string( pBuf ) );
    free_buf( pBuf );

    note_text->next = note_obj->extra_descr;
    note_obj->extra_descr = note_text;

    act_color( AT_ACTION, "$n makes a copy of a note.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_ACTION, "You make a copy of $p.", ch, note_obj, NULL, TO_CHAR, POS_RESTING );

    if ( !IS_IMMORTAL( ch ) && paper != NULL )
	extract_obj( paper );
    if ( !IS_IMMORTAL( ch ) && ink != NULL )
    {
	ink->value[1]--;
	if ( ink->value[1] <= 0 )    
	    act( "$p`X is now empty.", ch, ink, NULL, TO_CHAR );
    }

    obj_to_char( note_obj, ch );
    return;
}


static void
note_to( CHAR_DATA *ch, BOARD_DATA *pBoard, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    char	arg[MAX_INPUT_LENGTH];
    char *	p;

    if ( !attach_note( ch, pBoard ) )
    {
	send_to_char( "You may not post on this board.\n\r", ch );
	return;
    }

    buf[0] = '\0';
    buf[1] = '\0';
    p = buf;

    argument = first_arg( argument, arg, FALSE );
    while ( arg[0] != '\0' )
    {
	if ( !str_cmp( arg, "self" ) )
	    strcpy( arg, ch->name );
	*p++ = ' ';
	p = stpcpy( p, arg );
	argument = first_arg( argument, arg, FALSE );
    }

    if ( buf[1] == '\0' )
    {
	send_to_char( "Note to whom?\n\r", ch );
	return;
    }

    free_string( ch->pnote->to_list );
    ch->pnote->to_list = str_dup( buf + 1 );
    send_to_char( "Ok.\n\r", ch );
    return;
}


static void
note_write( CHAR_DATA *ch, BOARD_DATA *pBoard )
{
    if ( IS_NPC( ch ) )	/* extra safety check */
    {
	send_to_char( "Mobiles can't use note write.\n\r", ch );
	return;
    }

    if ( !attach_note( ch, pBoard ) )
    {
	send_to_char( "You may not post on this board.\n\r", ch );
	return;
    }

    string_append( ch, &ch->pnote->text );
    return;
}


void
show_board_list( CHAR_DATA *ch, bool fAll )
{
    BOARD_DATA *	pBoard;
    BUFFER *		pBuf;
    NOTE_DATA *		pNote;
    time_t		last_read;
    int			count;
    int			new;
    int			posts;
    int			total;
    int			trust;
    bool		found;

    pBuf = new_buf ( );
    count = 0;
    total = 0;
    found = FALSE;
    trust = get_trust( ch );

    for ( pBoard = board_first; pBoard != NULL; pBoard = pBoard->next )
    {
	if ( pBoard->read > trust && pBoard->write > trust )
	    continue;

	new = 0;
	posts = 0;
	count++;

	last_read = 0;

	for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
	{
	    if ( is_note_to( ch, pNote ) )
	    {
		posts++;
		if ( !hide_note( ch, pNote ) )
		    new++;
	    }
	}

	if ( !found )
	{
	    if ( fAll )
		add_buf( pBuf, "`W   Board    Msgs New  Description\n\r" );
	    else if ( new > 0 )
		add_buf( pBuf, "New notes:\n\r" );
	    found = fAll || new > 0;
	}

	if ( fAll )
	{
	    buf_printf( pBuf, "`W%2d `R%-8s `P%4d `%c%3d  `Y%s`X\n\r",
			count, pBoard->name, posts, new == 0 ? 'c' : 'G',
			new, pBoard->desc );
	}
	else if ( new > 0 )
	{
	    buf_printf( pBuf, "`W%2d `R%-8s `G%3d  `Y%s`X\n\r",
			count, pBoard->name, new, pBoard->desc );
	}

    }

    if ( !found )
    {
	if ( fAll )
	    add_buf( pBuf, "No boards found!\n\r" );
	else
	    add_buf( pBuf, "You have no unread notes.\n\r" );
    }
    else if ( fAll )
    {
	pBoard = find_board( ch, ch->pcdata->current_board );
	if ( pBoard == NULL )
	    pBoard = next_board( ch, NULL );
	if ( pBoard != NULL )
	{
	    buf_printf( pBuf, "`GCurrent board is `R%s`G.\n\r", pBoard->name );
	    buf_printf( pBuf, "`GYou can %s this board.`X\n\r",
			board_access_string( ch, pBoard ) );
	}
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


static void
update_read( CHAR_DATA *ch, NOTE_DATA *pNote )
{
    BOARD_INFO *	pInfo;

    if ( IS_NPC( ch ) )
	return;

    if ( ( pInfo = get_board_info( ch, pNote->board->name ) ) == NULL )
    {
	pInfo = new_board_info ( );
	pInfo->name = str_dup( pNote->board->name );
	pInfo->next = ch->pcdata->last_read;
	ch->pcdata->last_read = pInfo;
    }

    pInfo->time = UMAX( pInfo->time, pNote->date_stamp );
    return;
}


static void
write_board( BOARD_DATA *pBoard )
{
    NOTE_DATA *	pNote;
    char	filename[MAX_INPUT_LENGTH];
    char	filepath[MAX_INPUT_LENGTH];
    char	temppath[MAX_INPUT_LENGTH];
    FILE *	fp;
    time_t	expiration;
    char *	p;
    char *	q;

    if ( pBoard == NULL || IS_NULLSTR( pBoard->name ) )
	return;

    if ( pBoard->expire != 0 )
	expiration = current_time - pBoard->expire * 60 * 60 * 24;
    else
	expiration = 0;

    p = pBoard->name;
    q = filename;
    while ( *p != '\0' )
    {
	*q++ = LOWER( *p );
	p++;
    }
    *q = '\0';

    sprintf( temppath, "%s%s", TEMP_DIR, filename );
    sprintf( filepath, "%s%s", BOARDS_DIR, filename );

    fclose( fpReserve );
    if ( ( fp = fopen( temppath, "w" ) ) == NULL )
    {
	perror( temppath );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }

    for ( pNote = pBoard->note_first; pNote != NULL; pNote = pNote->next )
    {
	if ( pNote->date_stamp < expiration
	&&   !IS_SET( pNote->flags, NOTE_PROTECTED ) )
	    continue;

	fprintf( fp, "#NOTE\n" );
        fprintf( fp, "Sender  %s~\n", pNote->sender );
        fprintf( fp, "Date    %s~\n", pNote->date );
        fprintf( fp, "Stamp   %ld\n", pNote->date_stamp );
        fprintf( fp, "To      %s~\n", pNote->to_list );
        fprintf( fp, "Subject %s~\n", pNote->subject );
	if ( pNote->flags != 0 )
	    fprintf( fp, "Flags    %s\n", print_flags( pNote->flags ) );
	if ( pNote->on_board != 0 && pBoard == board_first )
	    fprintf( fp, "Board   %d\n", pNote->on_board );
        fprintf( fp, "Text\n%s~\n", fix_string( pNote->text ) );
        fprintf( fp, "End\n\n" );
    }

    fprintf( fp, "#$\n" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    rename( temppath, filepath );
    return;
}

