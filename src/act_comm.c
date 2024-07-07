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
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"


/*
 * Quotes at logoff
  */
struct quote_type
{
    char *  text;
    char *  by;
};
/*
 * Drunk struct
 */
struct structdrunk
{
    int		min_drunk_level;
    int		number_of_reps;
    char *	replacement[11];
};

struct structdrunk drunk[] =
{
    { 3, 10,
        { "a", "a", "a", "A", "aa", "ah", "Ah", "ao", "aw", "oa", "ahhhh" }
    },
    { 8, 5, { "b", "b", "b", "B", "B", "vb" } },
    { 3, 5, { "c", "c", "C", "cj", "sj", "zj" } },
    { 5, 2, { "d", "d", "D" } },
    { 3, 3, { "e", "e", "eh", "E" } },
    { 4, 5, { "f", "f", "ff", "fff", "fFf", "F" } },
    { 8, 2, { "g", "g", "G" } },
    { 9, 6, { "h", "h", "hh", "hhh", "Hhh", "HhH", "H" } },
    { 7, 6, { "i", "i", "Iii", "ii", "iI", "Ii", "I" } },
    { 9, 5, { "j", "j", "jj", "Jj", "jJ", "J" } },
    { 7, 2, { "k", "k", "K" } },
    { 3, 2, { "l", "l", "L" } },
    { 5, 8, { "m", "m", "mm", "mmm", "mmmm", "mmmmm", "MmM", "mM", "M" } },
    { 6, 6, { "n", "n", "nn", "Nn", "nnn", "nNn", "N" } },
    { 3, 6, { "o", "o", "ooo", "ao", "aOoo", "Ooo", "ooOo" } },
    { 3, 2, { "p", "p", "P" } },
    { 5, 5, { "q", "q", "Q", "ku", "ququ", "kukeleku" } },
    { 4, 2, { "r", "r", "R" } },
    { 2, 5, { "s", "ss", "zzZzssZ", "ZSssS", "sSzzsss", "sSss" } },
    { 5, 2, { "t", "t", "T" } },
    { 3, 6, { "u", "u", "uh", "Uh", "Uhuhhuh", "uhU", "uhhu" } },
    { 4, 2, { "v", "v", "V" } },
    { 4, 2, { "w", "w", "W" } },
    { 5, 6, { "x", "x", "X", "ks", "iks", "kz", "xz" } },
    { 3, 2, { "y", "y", "Y" } },
    { 2, 9,
        { "z", "z", "ZzzZz", "Zzz", "Zsszzsz", "szz", "sZZz", "ZSz", "zZ", "Z" }
    }     
};

/*
 * Logoff quote tables, if you add  more than 15 make sure to increment
  * MAX_QUOTE in merc.h
  */
const struct quote_type quote_table [MAX_QUOTES] =
{
    { "Cogito Ergo Sum", "Descartes" }, /* 1 */
    { "Stories of imagination tend to upset those without one.", "Sir Terry Pratchett" },
    { "Fantasy is hardly an escape from reality. It's a way of understanding it.", "Llyod Alexander"},
    { "Fantasy is escapist, and that is its glory. If a soldier is imprisioned by the enemy, don't we consider it his duty to escape?", "J.R.R. Tolkien"},
    { "The moment you doubt whether you can fly, you cease for ever to be able to do it.", "J.M. Barrie"},
    { "If you give a man an answer, all he gains is a little fact. But give him a question and he'll look for his own answers.", "Patrick Rothfuss"},
    { "Imagination is everything. It is the preview of life's coming attractions.", "Albert Einstein"},
    { "Do not meddle in the affairs of wizards, for they are subtle and quick to anger.", "J.R.R. Tolkien"},
    { "They can keep their heaven. When I die, I’d sooner go to Middle-earth.", "George R.R. Martin"},
    { "Come away, O human child!\n\rTo the waters and the wild\n\rWith a faery, hand in hand,\n\rFor the world's more full of \n\rweeping than you can understand.", "W.B. Yeats"},
    { "Closed in a room, my imagination becomes the universe, and the rest of the world is missing out.", "Criss Jami"},
    { "Beautiful and full of monsters? All the best stories are.", "Laini Taylor"},
    { "When you compare the sorrows of real life to the pleasures of the imaginary one, you will never want to live again, only to dream forever.", "Alexandre Dumas"},
    { "She moved like a poem and smiled like a sphinx.", "Laini Taylor"},
    { "Fantasy is a necessary ingredient in living, it's a way of looking at life through the wrong end of a telescope.", "Dr Seuss"}
};

/*
 * Local functions
 */
static	void	add_channel	args( ( CHAR_DATA *ch, BUFFER *pbuf,
					bitvector channel, const char *chan ) );
static	void	add_tell	args( ( CHAR_DATA *ch, char *tell ) );
static	char *	makedrunk	args( ( CHAR_DATA *ch, char *string ) );
void	talk_channel	args( ( CHAR_DATA *ch, const char *argument,
				bitvector channel, const char *verb ) );
void do_quote args((CHAR_DATA *ch));

static void
add_channel( CHAR_DATA *ch, BUFFER *pBuf, bitvector channel,
			 const char *chan )
{
    char  buf[MAX_INPUT_LENGTH];
    char *word;

    if ( IS_SET( ch->deaf, channel ) )
    {
	str_lower( buf, chan );
	word = "off";
    }
    else
    {
	str_upper( buf, chan );
	word = "ON";
    }

    buf_printf( pBuf, "%-13.13s %s\n\r", buf, word );
    return;
}


static void
add_tell( CHAR_DATA *ch, char *tell )
{
    if ( IS_NPC( ch ) )
        return;

    ch->pcdata->tell_buf_index++;
    if ( ch->pcdata->tell_buf_index >= TELLBUF_SIZE )
        ch->pcdata->tell_buf_index = 0;

    free_string( ch->pcdata->tell_buf[ch->pcdata->tell_buf_index] );
    ch->pcdata->tell_buf[ch->pcdata->tell_buf_index] = str_dup( tell );
    return;
}


void
do_channels( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    char	arg[MAX_INPUT_LENGTH];
    bitvector	bit;
    bitvector	on_bits;
    bitvector	off_bits;

    if ( *argument == '\0' )
    {
	pBuf = new_buf( );
	add_buf( pBuf, "channel    status\n\r" );
	add_buf( pBuf, "-----------------\n\r" );
	if ( get_trust( ch ) >= SUPREME )
	    add_channel( ch, pBuf, CHANNEL_ADMIN, "admin" );
	add_channel( ch, pBuf, CHANNEL_AUCTION, "auction" );
	if ( is_clan( ch ) || get_trust( ch ) >= L_ADMIN )
	    add_channel( ch, pBuf, CHANNEL_CLAN, "clan/house" );
	if ( IS_CODER( ch ) )
	    add_channel( ch, pBuf, CHANNEL_CODER, "coder" );
	add_channel( ch, pBuf, CHANNEL_GOSSIP, "gossip" );
	if ( IS_HERO( ch ) )
	    add_channel( ch, pBuf, CHANNEL_HERO, "hero" );
	if ( IS_IMMORTAL( ch ) )
	    add_channel( ch, pBuf, CHANNEL_IMMTALK, "immtalk" );
	if ( IS_IMMORTAL( ch ) )
	    add_channel( ch, pBuf, CHANNEL_PRAY, "pray" );
	if ( ch->level >= IMPLEMENTOR )
	    add_channel( ch, pBuf, CHANNEL_IMPLEMENTOR, "implementor" );
	if ( ch->level <= LEVEL_NEWBIE || IS_IMMORTAL( ch ) )
	    add_channel( ch, pBuf, CHANNEL_NEWBIE, "newbie" );
	add_channel( ch, pBuf, CHANNEL_OOC, "ooc" );
	add_channel( ch, pBuf, CHANNEL_QUESTION, "question" );
	add_channel( ch, pBuf, CHANNEL_SHOUT, "shout" );
	add_channel( ch, pBuf, CHANNEL_YELL, "yell" );

	if ( IS_SET( ch->comm, COMM_QUIET ) )
	    add_buf( pBuf, "Quiet mode is set.\n\r" );
	if ( IS_SET( ch->comm, COMM_AFK ) )
	    add_buf( pBuf, "You are AFK.\n\r" );

	if ( ch->lines != PAGELEN )
	{
	    if ( ch->lines )
	    {
		buf_printf( pBuf, "You display %d lines of scroll.\n\r", ch->lines + 2 );
	    }
	    else
		add_buf( pBuf, "Scroll buffering is off.\n\r" );
	}

	if ( IS_SET( ch->comm, COMM_NOSHOUT ) )
	    add_buf( pBuf, "You cannot shout.\n\r" );

	if ( IS_SET( ch->comm,COMM_NOTELL ) )
	    add_buf( pBuf, "You cannot use tell.\n\r" );

	if ( IS_SET( ch->comm, COMM_NOCHANNELS ) )
	   add_buf( pBuf, "You cannot use channels.\n\r" );

	if ( IS_SET( ch->comm, COMM_NOEMOTE ) )
	    add_buf( pBuf, "You cannot show emotions.\n\r" );

	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return;
    }

    on_bits = 0;
    off_bits = 0;
    argument = one_argument( argument, arg );
    while ( arg[0] != '\0' )
    {
	if ( ( arg[0] != '+' && arg[0] != '-' ) || arg[1] == '\0' )
	{
	    send_to_char( "+ or - which channel?\n\r", ch );
	    return;
	}
	if ( !str_cmp( &arg[1], "all" ) )
	{
	    bit = CHANNEL_AUCTION | CHANNEL_GOSSIP
		| CHANNEL_OOC
		| CHANNEL_QUESTION | CHANNEL_SHOUT
		| CHANNEL_YELL;
	    if ( ch->clan || get_trust( ch ) >= SUPREME ) bit |= CHANNEL_CLAN;
	    if ( IS_CODER( ch ) )	bit |= CHANNEL_CODER;
	    if ( IS_HERO( ch ) )	bit |= CHANNEL_HERO;
	    if ( IS_IMMORTAL( ch ) )	bit |= CHANNEL_IMMTALK;
	    if ( IS_IMMORTAL( ch ) )	bit |= CHANNEL_PRAY;
	    if ( ch->level >= IMPLEMENTOR ) bit |= CHANNEL_IMPLEMENTOR;
	    if ( ch->level <= LEVEL_NEWBIE || IS_IMMORTAL( ch ) ) bit |= CHANNEL_NEWBIE;
	    if ( get_trust( ch ) >= SUPREME ) bit |= CHANNEL_ADMIN;
	}
	else if ( IS_IMMORTAL( ch ) && !str_prefix( &arg[1], "mortal" ) )
	{
	    bit = CHANNEL_AUCTION | CHANNEL_GOSSIP
		| CHANNEL_OOC | CHANNEL_HERO
		| CHANNEL_QUESTION | CHANNEL_NEWBIE
		| CHANNEL_SHOUT | CHANNEL_YELL;
	    if ( ch->clan || get_trust( ch ) >= SUPREME ) bit |= CHANNEL_CLAN;
	}
	else if ( !str_prefix( &arg[1], "auction" ) ) bit = CHANNEL_AUCTION;
	else if ( !str_prefix( &arg[1], "clan" ) && ( ch->clan != NULL || get_trust( ch ) >= L_ADMIN ) )
	    bit = CHANNEL_CLAN;
	else if ( !str_prefix( &arg[1], "coder" ) && IS_CODER( ch ) )
	    bit = CHANNEL_CODER;
	else if ( !str_prefix( &arg[1], "gossip" ) )	bit = CHANNEL_GOSSIP;
	else if ( !str_prefix( &arg[1], "hero" ) && IS_HERO( ch ) )
	    bit = CHANNEL_HERO;
	else if ( !str_prefix( &arg[1], "house" ) && ( ch->clan != NULL || get_trust( ch ) >= L_ADMIN ) )
	    bit = CHANNEL_CLAN;
	else if ( !str_prefix( &arg[1], "immtalk" ) && IS_IMMORTAL( ch ) )
	    bit = CHANNEL_IMMTALK;
	else if ( !str_prefix( &arg[1], "implementor" ) && ch->level >= IMPLEMENTOR )
	    bit = CHANNEL_IMPLEMENTOR;
	else if ( !str_prefix( &arg[1], "newbie" ) && ( IS_IMMORTAL( ch ) || ch->level <= LEVEL_NEWBIE ) )
	    bit = CHANNEL_NEWBIE;
	else if ( !str_prefix( &arg[1], "ooc" ) )	bit = CHANNEL_OOC;
	else if ( !str_prefix( &arg[1], "pray" ) && IS_IMMORTAL( ch ) )
	    bit = CHANNEL_PRAY;
	else if ( !str_prefix( &arg[1], "question" ) )	bit = CHANNEL_QUESTION;
	else if ( !str_prefix( &arg[1], "shout" ) )	bit = CHANNEL_SHOUT;
	else if ( !str_prefix( &arg[1], "admin" ) && get_trust( ch ) >= SUPREME )
	    bit = CHANNEL_ADMIN;
	else if ( !str_prefix( &arg[1], "yell" ) )	bit = CHANNEL_YELL;
	else
	{
	    act_color( C_DEFAULT, "$t is not a channel.", ch, &arg[1], NULL, TO_CHAR, POS_DEAD );
	    return;
	}
	if ( arg[0] == '+' )
	    off_bits |= bit;
	else
	    on_bits |= bit;
	argument = one_argument( argument, arg );
    }
    ch->deaf |= on_bits;
    ch->deaf &= ~off_bits;
    send_to_char( "Ok.\n\r", ch );
    return;
}


/* RT deaf blocks out all shouts */
void
do_deaf( CHAR_DATA *ch, char *argument )
{
    if ( IS_SET( ch->comm, COMM_DEAF ) )
    {
	send_to_char( "You can now hear tells again.\n\r", ch );
	REMOVE_BIT( ch->comm, COMM_DEAF );
    }
    else 
    {
	send_to_char( "From now on, you won't hear tells.\n\r", ch );
	SET_BIT( ch->comm, COMM_DEAF );
    }
}


void
do_ignore( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    BUFFER *		pBuf;
    IGNORE_DATA *	pId;
    USERLIST_DATA *	pList;

    if ( IS_NPC( ch ) )
    {
        send_to_char( "You can't ignore anybody.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        if ( ch->pcdata->ignoring == NULL )
        {
            send_to_char( "You aren't ignoring anybody.\n\r", ch );
            return;
        }
        pBuf = new_buf( );
        add_buf( pBuf, "You are ignoring:\n\r" );
        for ( pId = ch->pcdata->ignoring; pId != NULL; pId = pId->next )
        {
            add_buf( pBuf, pId->name );
            add_buf( pBuf, "\n\r" );
        }
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
        return;
    }

    if ( !str_cmp( arg, ch->name ) )
    {
        send_to_char( "You can't ignore yourself.\n\r", ch );
        return;
    }

    for ( pId = ch->pcdata->ignoring; pId != NULL; pId = pId->next )
        if ( !str_cmp( arg, pId->name ) )
            break;

    if ( pId == NULL )
    {
        /* Add player to delete list */
        for ( pList = user_first; pList != NULL; pList = pList->next )
            if ( !str_cmp( arg, pList->name ) )
                break;

        if ( pList == NULL )
        {
            send_to_char( "That player was not found.\n\r", ch );
            return;
        }

        strcpy( arg, pList->name );	/* to get proper capitalization */

        pId = new_ignore_data( );
        pId->name = str_dup( arg );
        pId->flags = IGNORE_SET | IGNORE_CHANNELS | IGNORE_TELLS;

        if ( ch->pcdata->ignoring != NULL )
            ch->pcdata->ignoring->prev = pId;
        pId->next = ch->pcdata->ignoring;
        ch->pcdata->ignoring = pId;
        ch_printf( ch, "You are now ignoring %s.\n\r", arg );
    }
    else
    {
        /* Remove player from delete list */
	if ( pId->next != NULL )
	    pId->next->prev = pId->prev;
	if ( pId->prev == NULL )
	    ch->pcdata->ignoring = pId->next;
        else
            pId->prev->next = pId->next;
        free_ignore_data( pId );
        arg[0] = UPPER( arg[0] );
        ch_printf( ch, "You are no longer ignoring %s.\n\r", arg );
    }

    return;
}


/* RT quiet blocks out all communication */
void
do_quiet( CHAR_DATA *ch, char * argument )
{
     if ( IS_SET( ch->comm, COMM_QUIET ) )
     {
	send_to_char("Quiet mode removed.\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_QUIET);
     }
    else
    {
	send_to_char("From now on, you will only hear says and emotes.\n\r",ch);
	SET_BIT(ch->comm,COMM_QUIET);
    }
}


/* afk command */
void
do_afk( CHAR_DATA *ch, char * argument )
{
    if ( IS_NPC( ch ) )
    {
        send_to_char( "Forget it.  You're not going anywhere.\n\r", ch );
        return;
    }

    if ( IS_SET( ch->comm, COMM_AFK ) )
    {
	send_to_char( "`YAFK`W mode removed.`X ", ch );
	if ( *buf_string( ch->pcdata->buffer ) != '\0' )
	{
    	    send_to_char( "`WYou have tells to replay.`X\n\r", ch );
	}
	else
	    send_to_char( "\n\r", ch );

	act( "$n has returned to $s keyboard.", ch, NULL, ch, TO_ROOM );
	REMOVE_BIT( ch->comm, COMM_AFK );
    }
    else
    {
	send_to_char( "`WYou are now in `YAFK `Wmode and all tells will be archived, but\n\r", ch );
	send_to_char( "please be aware that you are not safe while in `YAFK`W mode.`X\n\r", ch );
	act( "$n has left $s keyboard.", ch, NULL, ch, TO_ROOM );
	SET_BIT( ch->comm, COMM_AFK );
    }

    if ( IS_CODER( ch ) && str_match( argument, "cod", "coding" ) )
        do_coding( ch, "" );

}


void
do_replay( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "You can't replay.\n\r", ch );
	return;
    }

    if ( buf_string( ch->pcdata->buffer)[0] == '\0' )
    {
	send_to_char( "You have no tells to replay.\n\r", ch );
	return;
    }

    page_to_char( buf_string( ch->pcdata->buffer ), ch );
    clear_buf( ch->pcdata->buffer );
}


static char *
makedrunk( CHAR_DATA *ch, char *string )
{
    int		drunklevel;
    static char	buf[MAX_INPUT_LENGTH];
    int		pos;
    int		randomnum;
    char	temp;

    drunklevel = IS_NPC( ch ) ? 0 : ch->pcdata->condition[COND_DRUNK];
    if ( drunklevel <= 0 )
        return string;

    pos = 0;
    while ( *string != '\0' )
    {
        temp = toupper( *string );
        if ( temp >= 'A' && temp <= 'Z' )
        {
            if ( drunklevel > drunk[temp - 'A'].min_drunk_level )
            {
                randomnum = number_range( 0, drunk[temp - 'A'].number_of_reps );
                strcpy( &buf[pos], drunk[temp - 'A'].replacement[randomnum] );
                pos += strlen( drunk[temp - 'A'].replacement[randomnum] );
            }
            else
            {
                buf[pos++] = *string;
            }
        }
        else if ( temp >= '0' && temp <= '9' && number_percent( ) < drunklevel * 2 )
        {
            buf[pos++] = number_range( '0', '9' );
        }
        else if ( is_colcode( string ) && number_percent( ) < drunklevel * 2 )
        {
            randomnum = number_range( 1, 15 );
            buf[pos++] = *string++;
            buf[pos++] = colorcode_list[randomnum];
        }
        else
        {
            buf[pos++] = *string;
        }

        if ( pos >= MAX_INPUT_LENGTH - 10 )
            break;
        string++;
    }

    buf[pos] = '\0';
    return buf;
}


/*
 * Generic channel function
 */
void
talk_channel( CHAR_DATA *ch, const char *argument,
		   bitvector channel, const char *verb )
{
    DESCRIPTOR_DATA *	d;
    CHAR_DATA *		och;
    CHAR_DATA *		rch;
    CHAR_DATA *		vch;
    char		buf[MAX_STRING_LENGTH];
    char		arg[MAX_STRING_LENGTH];
    char *		s;
    char *		t;

    if ( *argument == 0 )
    {
	if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOCHANNEL ) )
	{
	    TOGGLE_BIT( ch->deaf, channel );
	    sprintf( buf, "%s channel is now %s.\n\r", verb,
		     IS_SET( ch->deaf, channel ) ? "OFF" : "ON" );
	}
	else
	{
	    sprintf( buf, "%s what?\n\r", verb );
	}
	buf[0] = UPPER( buf[0] );
	send_to_char( buf, ch );
	return;
    }

    if ( IS_SET( ch->comm, COMM_QUIET ) )
    {
	send_to_char( "You must turn off quiet mode first.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->comm, COMM_NOCHANNELS ) )
    {
	send_to_char( "The gods have revoked your channel privileges.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) && IS_SET( ch->in_room->room_flags, ROOM_SILENT ) )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    REMOVE_BIT( ch->deaf, channel );

    /* might want to call makedrunk() here, when we install the snippet */
    strcpy( arg, argument );

    rch = ( ch->desc && ch->desc->original ? ch->desc->original : ch );

    switch( channel )
    {
	default:
	    sprintf( buf, "You %s '$t'", verb );
	    act_color( AT_CHANNEL, buf, ch, arg, NULL, TO_CHAR, POS_RESTING );
	    sprintf( buf, "$n %ss '$t'", verb );
	    break;
	case CHANNEL_CLAN:
//	    sprintf( buf, "%s>$n: $t", ch->clan ? ch->clan->name : "(unclanned)" );
	    sprintf( buf, "`YYou tell your %s, '`g$t`Y'", ch->clan == NULL ? "(Unclanned)" : ch->clan->fHouse ? "house" : "clan" );
	    act_color( AT_DGREEN, buf, ch, arg, NULL, TO_CHAR, POS_DEAD );
	    if ( ch->clan != NULL && ch->clvl != 0
	    &&	 !IS_NULLSTR( s = ch->clan->ctitle[ch->clvl-1][ch->sex == SEX_FEMALE?1:0] ) )
		sprintf( buf, "`Y%s %s tells the %s, '`g$t`Y'", s,
			 IS_NPC( ch ) ? ch->short_descr : ch->name,
			 ch->clan == NULL ? "(Unclanned)" : ch->clan->fHouse ? "house" : "clan" );
	    else
		sprintf( buf, "`Y%s tells the %s, '`g$t`Y'",
			 IS_NPC( ch ) ? ch->short_descr : ch->name,
			 ch->clan == NULL ? "(Unclanned)" : ch->clan->fHouse ? "house" : "clan" );
	    break;
	case CHANNEL_CODER:
	    sprintf( buf, "$n (`WCODER`w): $t" );
	    act_color( AT_GREY, buf, ch, arg, NULL, TO_CHAR, POS_DEAD );
	    break;
	case CHANNEL_GOSSIP:
	    sprintf( buf, "`WYou %s '`c$t`W'", verb );
	    act_color( AT_GOSSIP, buf, ch, arg, NULL, TO_CHAR, POS_SLEEPING );
	    sprintf( buf, "`WGossip Spreads Throughout the Lands '`c$t`W'" );
	    break;
	case CHANNEL_HERO:
	    sprintf( buf, "$n HERO: $t: " );
	    act_color( AT_CHANNEL, buf, ch, arg, NULL, TO_CHAR, POS_SLEEPING );
	    break;
	case CHANNEL_IMMTALK:
	    sprintf( buf, "`W[`P$n`W]: `C$t" );
	    act_color( AT_LBLUE, buf, ch, arg, NULL, TO_CHAR, POS_DEAD );
	    break;
	case CHANNEL_IMPLEMENTOR:
	    sprintf( buf, "$n <`GIMP`R> `G$t" );
	    act_color( AT_RED, buf, ch, arg, NULL, TO_CHAR, POS_DEAD );
	    break;
	case CHANNEL_NEWBIE:
	    sprintf( buf, "NEWBIE: `C$n: `X'$t'" );
	    act_color( AT_GREEN, buf, ch, arg, NULL, TO_CHAR, POS_SLEEPING );
	    break;
	case CHANNEL_OOC:
	    if ( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
		t = ch->name;
	    else
		t = "$n";
	    sprintf( buf, "%s `C[`BOOC`C]`X: $t", t );
	    act_color( AT_CHANNEL, buf, ch, arg, NULL, TO_CHAR, POS_DEAD );
	    break;
	case CHANNEL_QUESTION:
	    sprintf( buf, "You %s '`Y$t`X'", verb );
	    act_color( AT_BLUE, buf, ch, arg, NULL, TO_CHAR, POS_SLEEPING );
	    sprintf( buf, "$n %ss '`Y$t`X'", verb );
	    break;
	case CHANNEL_ADMIN:
	    sprintf( buf, "`W[`g$n Admins`W]: `P$t" );
	    act_color( AT_PINK, buf, ch, arg, NULL, TO_CHAR, POS_DEAD );
	    break;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	och = d->original ? d->original : d->character;
	vch = d->character;
	if ( d->connected == CON_PLAYING
	     && !IS_SET( och->comm, COMM_QUIET )
	     && !IS_SET( och->deaf, channel )
	     && !is_ignoring( och, ch, IGNORE_CHANNELS )
	     && vch != ch )
	{
	    if ( !IS_IMMORTAL( och ) && IS_SET( och->in_room->room_flags, ROOM_SILENT ) )
	        continue;
	    if ( channel == CHANNEL_CLAN && ( !och->clan || och->clan != ch->clan ) )
		continue;
	    if ( channel == CHANNEL_CODER && !IS_CODER( och ) )
		continue;
	    if ( channel == CHANNEL_HERO && !IS_HERO( och ) )
		continue;
	    if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL( och ) )
		continue;
	    if ( channel == CHANNEL_IMPLEMENTOR && och->level < IMPLEMENTOR )
		continue;
	    if ( channel == CHANNEL_NEWBIE && och->level > LEVEL_NEWBIE
		 && !IS_IMMORTAL( och ) )
		continue;
	    if ( channel == CHANNEL_SHOUT && IS_AFFECTED( vch, AFF_DEAF ) )
		continue;
	    if ( channel == CHANNEL_ADMIN && get_trust( och ) < SUPREME )
		continue;
	    if ( channel == CHANNEL_YELL && ( !vch->in_room || !vch->in_room->area || vch->in_room->area != ch->in_room->area || IS_AFFECTED( vch, AFF_DEAF ) ) )
		continue;

	    switch( channel )
	    {
	    default:
		act_color( AT_CHANNEL, buf, ch, arg, vch, TO_VICT, POS_RESTING );
		break;
	    case CHANNEL_CLAN:
		if ( get_trust( och ) >= L_ADMIN )
		    ch_printf( vch, "`R%s>", ch->clan ? ch->clan->name : "(unclanned)" );
		act_color( AT_DGREEN, buf, ch, arg, vch, TO_VICT, POS_DEAD );
		break;
	    case CHANNEL_CODER:
		act_color( AT_GREY, buf, ch, arg, vch, TO_VICT, POS_DEAD );
		break;
	    case CHANNEL_GOSSIP:
		if ( IS_IMMORTAL( och ) && get_trust( och ) >= get_trust( ch ) )
		    ch_printf( vch, "(%s)", PERS( ch, och ) );
		if ( xIS_SET( vch->affected_by, AFF_DEAF ) )
		    break;
		act_color( AT_GOSSIP, buf, ch, arg, vch, TO_VICT, POS_SLEEPING );
		break;
	    case CHANNEL_HERO:
		act_color( AT_CHANNEL, buf, ch, arg, vch, TO_VICT, POS_SLEEPING );
		break;
	    case CHANNEL_IMMTALK:
		act_color( AT_LBLUE, buf, ch, arg, vch, TO_VICT, POS_DEAD );
		break;
	    case CHANNEL_IMPLEMENTOR:
		act_color( AT_RED, buf, ch, arg, vch, TO_VICT, POS_DEAD );
		break;
	    case CHANNEL_NEWBIE:
		act_color( AT_GREEN, buf, ch, arg, vch, TO_VICT, POS_DEAD );
		break;
	    case CHANNEL_OOC:
		act_color( AT_CHANNEL, buf, ch, arg, vch, TO_VICT, POS_DEAD );
		break;
	    case CHANNEL_QUESTION:
		act_color( AT_BLUE, buf, ch, arg, vch, TO_VICT, POS_SLEEPING );
		break;
	    case CHANNEL_ADMIN:
		act_color( AT_PINK, buf, ch, arg, vch, TO_VICT, POS_DEAD );
		break;
	    }
	}
    }
}


void
auc_channel( const char *arg )
{
    DESCRIPTOR_DATA *	d;
    char		buf[MAX_STRING_LENGTH];

    sprintf( buf, "`GAUCTION: `g%s`X\n\r", arg );

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	  && !IS_SET( (d->original ? d->original : d->character)->deaf, CHANNEL_AUCTION ) )
	{
	    write_to_buffer( d, buf, 0 );
	}
    }
}


void
do_auction( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    int		bid;
    MONEY	amt;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Auction which item?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "remove" ) )
    {
	if ( !auc_obj || auc_held != ch )
	{
	    send_to_char( "You are not auctioning anything.\n\r", ch );
	    return;
	}
	if ( auc_bid )
	{
	    send_to_char( "You may not remove your item after a bid has been made.\n\r", ch );
	    return;
	}

	REMOVE_BIT( ch->deaf, CHANNEL_AUCTION );
	sprintf( buf, "%s has been removed from the auction.", auc_obj->short_descr );
	auc_channel( buf );
	act( "$p appears suddenly in your hands.", ch, auc_obj, NULL, TO_CHAR );
	act( "$p appears suddenly in the hands of $n.", ch, auc_obj, NULL,
	     TO_ROOM );
	obj_to_char( auc_obj, ch );
	auc_obj = NULL;
	auc_held = NULL;
	auc_cost = 0;
	auc_count = -1;
	return;
    }

    if ( !str_cmp( arg1, "stop" ) && get_trust( ch ) >= ANGEL )
    {
	if ( !auc_obj )
	{
	    send_to_char( "There is no item being auctioned.\n\r", ch );
	    return;
	}

	log_printf( "Auction stopped by %s.", ch->name );
	wiznet( "Auction stopped by $N.", ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );

	REMOVE_BIT( ch->deaf, CHANNEL_AUCTION );
	sprintf( buf, "%s has been removed from the auction.", auc_obj->short_descr );
	auc_channel( buf );
	act( "$p appears suddenly in your hands.", auc_held, auc_obj, NULL, TO_CHAR );
	act( "$p appears suddenly in the hands of $n.", auc_held, auc_obj, NULL,
	     TO_ROOM );
	obj_to_char( auc_obj, auc_held );
	auc_obj = NULL;
	auc_held = NULL;
	auc_cost = 0;
	auc_count = -1;
	return;
    }

    if ( auc_obj )
    {
	send_to_char( "There is already an object being auctioned.\n\r", ch );
	return;
    }

    amt.gold = 0;
    amt.silver = 0;
    amt.copper = 0;

    if ( *argument == '\0' )
    {
	amt.fract = 1;
    }
    else
    {
	if ( money_value( &amt, argument ) == NULL )
	{
	    send_to_char( "Auction it for how much?\n\r", ch );
	    return;
	}
    }

    normalize( &amt );
    bid = CASH_VALUE( amt );
    if ( bid <= 0 )
    {
	send_to_char( "That is too low of a starting bidding price.\n\r", ch );
	return;
    }

    if ( ( auc_obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You are not carrying that item.\n\r", ch );
	return;
    }

    if ( auc_obj->pIndexData->vnum < 100	/* no limbo objects */
      || auc_obj->item_type == ITEM_QUESTITEM	/* no quest targets */
      || ( auc_obj->item_type == ITEM_CONTAINER && auc_obj->contains ) )
    {
	send_to_char( "You can't auction that.\n\r", ch );
	auc_obj = NULL;
	return;
    }

    REMOVE_BIT( ch->deaf, CHANNEL_AUCTION );
    act( "$p disappears from your inventory.", ch, auc_obj, NULL, TO_CHAR );
    act( "$p disappears from the inventory of $n.", ch, auc_obj, NULL, TO_ROOM );
    obj_from_char( auc_obj );
    auc_held = ch;
    auc_bid = NULL;
    auc_cost = bid;
    auc_count = 0;
    sprintf( buf, "%s a level %d object for%s.", auc_obj->short_descr,
	     auc_obj->level,
	     money_string( &amt, FALSE, FALSE ) );
    auc_channel( buf );
    sprintf( buf, "%s auctioning %s for%s.", auc_held->name,
	     auc_obj->short_descr,
	     money_string( &amt, FALSE, FALSE ) );
    wiznet( buf, NULL, NULL, WIZ_AUCTION, 0, 0 );
    return;
}


void
do_bid( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    MONEY	amt;
    int		bid;
    int		min_bid;

    if ( !auc_obj )
    {
	send_to_char( "There is nothing being auctioned at the moment.\n\r", ch );
	return;
    }

    if ( auc_held == ch )
    {
	if ( auc_bid )
	{
	    send_to_char( "You may not bid on your own item.\n\r", ch );
	    return;
	}
	else
	{
	    send_to_char( "If you want your item back, you should 'auction remove' it.\n\r", ch );
	    return;
	}
    }

    if ( auc_bid == ch )
    {
	send_to_char( "You already hold the highest bid.\n\r", ch );
	return;
    }

    min_bid = UMAX( 1, auc_cost / 8 ) + auc_cost;
    while ( isspace( *argument ) )
	argument++;
    if ( *argument == '\0' )
    {
	amt.gold = 0;
	amt.silver = 0;
	amt.copper = 0;
	amt.fract = min_bid;
    }
    else
    {
	argument = get_money_string( argument, arg );
	if ( money_value( &amt, arg ) == NULL )
	{
	    send_to_char( "Bid how much?\n\r", ch );
	    return;
	}
    }

    normalize( &amt );
    bid = CASH_VALUE( amt );

    if ( min_bid > bid )
    {
	ch_printf( ch, "You must bid at least%s",
		   money_string( &amt, FALSE, FALSE ) );
	return;
    }

    if ( bid > CASH_VALUE( ch->money ) )
    {
	send_to_char( "You are not carrying that much.\n\r", ch );
	return;
    }

    REMOVE_BIT( ch->deaf, CHANNEL_AUCTION );
    sprintf( buf, "%s bid on %s.",
	     money_string( &amt, FALSE, FALSE ),
	     auc_obj->short_descr );
    auc_channel( buf );
    sprintf( buf, "%s bidding%s on %s.",
	     IS_NPC( ch ) ? ch->short_descr : ch->name,
	     money_string( &amt, FALSE, FALSE ),
	     auc_obj->short_descr );
    wiznet( buf, NULL, NULL, WIZ_AUCTION, 0, 0 );
    auc_cost = bid;
    auc_count = 0;
    auc_bid = ch;
    return;
}


void
do_admin( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_ADMIN, "admin" );
}

/* RT chat replaced with ROM gossip */
void
do_gossip( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_GOSSIP, "gossip" );
}


void
do_hero( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_HERO, "hero" );
}

void
do_ooc( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_OOC, "ooc" );
}


void
do_ooc_dot( CHAR_DATA *ch, char *argument )
{
    if ( ch->desc != NULL && ch->desc->editor != ED_NONE )
    {
	send_to_char( "While editing, you must use \"gossip\" not \".\" to use the gossip channel.\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_GOSSIP, "gossip" );
    return;
}


void
do_pray( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH * 2];
    DESCRIPTOR_DATA*	d;
    char *		msg;
    char		shortname[4];
    USERLIST_DATA *	user;
    CHAR_DATA *		vch;

    if ( *argument == '\0' )
    {
	if ( !IS_NPC( ch ) && IS_SET( ch->act2, PLR_AUTOCHANNEL ) )
	{
	    TOGGLE_BIT( ch->deaf, CHANNEL_PRAY );
	    ch_printf( ch, "Pray channel is now %s.\n\r",
		     IS_SET( ch->deaf, CHANNEL_PRAY ) ? "OFF" : "ON" );
	}
	else
	{
	    send_to_char( "Pray what?\n\r", ch );
	}
	return;
    }

    if ( IS_SET( ch->comm, COMM_QUIET ) )
    {
	send_to_char( "You must turn off quiet mode first.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->comm, COMM_NOCHANNELS ) )
    {
	send_to_char( "The gods have revoked your channel privileges.\n\r", ch );
	return;
    }

    REMOVE_BIT( ch->deaf, CHANNEL_PRAY );

    /* See if this is a directed prayer */
    msg = one_argument( argument, arg );

    for ( user = user_first; user != NULL; user = user->next )
    {
        /* require at least three letters of a name */
        strncpy( shortname, user->name, 3 );
        shortname[3] = '\0';
        if ( str_match( arg, shortname, user->name ) && user->level > LEVEL_IMMORTAL )
            break;
    }

    if ( user != NULL && *msg == '\0' )
    {
        ch_printf( ch, "Pray what to %s?\n\r", user->name );
        return;
    }

    if ( user != NULL )
    {
        if ( user->level < get_trust( ch ) )
        {
            send_to_char( "They should pray to you, not you to them.\n\r", ch );
            return;
        }
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            vch = d->original != NULL ? d->original : d->character;
            if ( !str_cmp( vch->name, user->name ) )
            {
                if ( !is_ignoring( vch, ch, IGNORE_CHANNELS )
                &&   !IS_SET( vch->comm, COMM_QUIET )
                &&   !IS_SET( vch->deaf, CHANNEL_PRAY )
                &&   vch != ch )
                {
                    set_char_color( AT_LBLUE, d->character );
                    ch_printf( d->character, "`c%s prays to `Pyou`c, '`C%s`c'\n\r",
                               ch->name, msg );
                }
                break;
            }
        }
        set_char_color( AT_LBLUE, ch );
        ch_printf( ch, "`cYou pray to `P%s`c, '`C%s`c'\n\r", user->name, msg );
        return;
    }

    set_char_color( AT_LBLUE, ch );
    ch_printf( ch, "`cYou pray to the gods, '`C%s`c'\n\r", argument );
    sprintf( buf, "`c%s prays to the gods, '`C%s`c'\n\r", ch->name, argument );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        vch = d->original != NULL ? d->original : d->character;
        if ( vch->level > LEVEL_IMMORTAL
        &&   vch->level >= ch->level
        &&   vch != ch
        &&   d->connected == CON_PLAYING
        &&   !is_ignoring( vch, ch, IGNORE_CHANNELS )
        &&   !IS_SET( vch->comm, COMM_QUIET )
        &&   !IS_SET( vch->deaf, CHANNEL_PRAY ) )
        {
            set_char_color( AT_LBLUE, d->character );
            send_to_char( buf, d->character );
        }
    }

    return;
}


/* RT question channel */
void
do_question( CHAR_DATA *ch, char *argument )
{
    char  buf[MAX_INPUT_LENGTH+2];
    char *p;

    p = stpcpy( buf, argument );
    if ( !strchr( buf, '?' ) )
	strcpy( p, "?" );

    talk_channel( ch, buf, CHANNEL_QUESTION, "question" );
}


/* RT answer channel - uses same line as questions */
void
do_answer( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "answer" );
}


/* clan channels */
void
do_clantalk( CHAR_DATA *ch, char *argument )
{
    if ( ch->clan == NULL || ch->clan->fHouse )
    {
	send_to_char( "You aren't in a clan.\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_CLAN, "clantalk" );
    return;
}


void
do_coder( CHAR_DATA *ch, char *argument )
{
    if ( !IS_CODER( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_CODER, "coder" );
}


void
do_housetalk( CHAR_DATA *ch, char *argument )
{
    if ( ch->clan == NULL || !ch->clan->fHouse )
    {
	send_to_char( "You are not a member of a noble house.\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_CLAN, "house talk" );
    return;
}


void
do_immtalk( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
}


void
do_implementor( CHAR_DATA *ch, char *argument )
{
    if ( ch->level < IMPLEMENTOR )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_IMPLEMENTOR, "implementor" );
}


void
do_newbie( CHAR_DATA *ch, char *argument )
{
    if ( !IS_IMMORTAL( ch ) && ch->level > LEVEL_NEWBIE )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_NEWBIE, "newbie" );
}


void
do_say( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    CHAR_DATA       *   vch;
    char		buf[MAX_INPUT_LENGTH + 2];
    char *		p;
    char *		verb;
    char                buf2[MAX_INPUT_LENGTH + 2];

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    p = stpcpy( buf, makedrunk( ch, argument ) ) - 1;
    if ( *p == '!' )
        verb = "exclaim";
    else if ( *p == '?' )
        verb = "ask";
    else
        verb = "say";

    if ( *p == '`' && ( strlen( buf ) == 1 || *(p-1) != '`' ) )
    {
	p++;
	*p = '`';
    }

    if ( !IS_SET( ch->act2, PLR_NOPUNCT ) )
    {
        if ( !ispunct ( *p ) )
            strcpy( p + 1, "." );

        p = buf;
        while( is_colcode( p ) )
            p += 2;
        *p = UPPER( *p );
    }

    pRoom = ch->in_room;
    MOBtrigger = FALSE;

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if ( vch != ch )
	{
	    if ( xIS_SET( vch->affected_by, AFF_DEAF ) )
		sprintf( buf2, "`W$n says something, that your damaged ears cannot hear." );
	    else
		sprintf( buf2,  "`W$n %ss '`G$t`W'", verb );
	    act_color( AT_SAY, buf2, ch, buf, vch, TO_VICT, POS_RESTING );
	 }

	else
	    act_color( AT_SAY, "`WYou $t '`G$T`W'", ch, verb, buf, TO_CHAR, POS_RESTING );
    }

    mprog_speech_trigger( buf, ch );

    if ( ch->in_room == pRoom && !char_died( ch ) )
	rprog_speech_trigger( pRoom, ch, buf );

    return;
}


void
do_sayto( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH + 2];
    ROOM_INDEX_DATA *	pRoom;
    CHAR_DATA *		vch;
    char *		p;
    char		punct;
    char 		fmt[MAX_INPUT_LENGTH];
    CHAR_DATA *         rch; // for room observers.

    argument = one_argument( argument, arg );

    if ( *argument == '\0' || arg[0] == '\0' )
    {
	send_to_char( "Say what to whom?\n\r", ch );
	return;
    }

    if ( ( vch = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    pRoom = ch->in_room;

    p = stpcpy( buf, makedrunk( ch, argument ) );
    punct = *(p - 1);

    if ( !IS_SET( ch->act2, PLR_NOPUNCT ) )
    {
        if ( !ispunct( punct ) )
            strcpy( p, "." );

        p = buf;
        while( is_colcode( p ) )
            p += 2;
        *p = UPPER( *p );
    }

    switch ( punct )
    {
        case '?': strcpy( fmt, "`WYou ask $N, '`G$t`W'" );		break;
        case '!': strcpy( fmt, "`WYou exclaim to $N, '`G$t`W'" );	break;
        default : strcpy( fmt, "`WYou say to $N, '`G$t`W'" );		break;
    }
    MOBtrigger = FALSE;
    act_color( AT_SAY, fmt, ch, buf, vch, TO_CHAR, POS_RESTING );

    /* notifiy them thier message was not received*/
    if ( xIS_SET( vch->affected_by, AFF_DEAF ) )
	act_color ( AT_SAY, "$N does not appear able to hear you.", ch, NULL, vch, TO_CHAR, POS_RESTING );

    switch ( punct )
    {
        case '?': strcpy( fmt, "`W$n asks you, '`G$t`W'" );		break;
        case '!': strcpy( fmt, "`W$n exclaims to you, '`G$t`W'" );	break;
        default : strcpy( fmt, "`W$n says to you, '`G$t`W'" );		break;
    }
    MOBtrigger = FALSE;

    if ( xIS_SET( vch->affected_by, AFF_DEAF ) )
	strcpy( fmt, "`W$n says something to you, but you cannot hear them." );

    act_color( AT_SAY, fmt, ch, buf, vch, TO_VICT, POS_RESTING );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room ) 
    {
        switch ( punct )
        {
            case '?': 
	        sprintf ( fmt, "`W%s asks %s, '`G$t`W'", PERS( ch, rch ), PERS( vch, rch ) ); 		
	    break;

	    case '!': 
	        sprintf ( fmt, "`W%s exclaims to %s, '`G$t`W'", PERS( ch, rch ), PERS( vch, rch ) );	
	    break;

	    default : 
	        sprintf ( fmt, "`W%s says to %s, '`G$t`W'", PERS( ch, rch ), PERS( vch, rch ) );		
	    break;
        }

	if ( rch != ch && rch != vch )
	{
	    if ( xIS_SET( rch->affected_by, AFF_DEAF ) )
	         sprintf ( fmt, "`W%s says something to %s, but you cannot hear them.", 
			   PERS( ch, rch ), PERS( vch, rch ) );

	    MOBtrigger = FALSE;
	    act_color( AT_SAY, fmt, rch, buf, NULL, TO_CHAR, POS_RESTING );
	}
    }

    mprog_speech_trigger( buf, ch );

    if ( ch->in_room == pRoom && !char_died( ch ) )
	rprog_speech_trigger( pRoom, ch, buf );

    return;
}


void
do_osay( CHAR_DATA *ch, char *argument )
{
    if ( *argument == '\0' )
    {
	send_to_char ( "Osay what?\n\r", ch );
	return;
    }
    MOBtrigger = FALSE;
    act_color( AT_SAY, "$n says OOC: '$T'", ch, NULL, argument, TO_ROOM, POS_RESTING );
    MOBtrigger = FALSE;
    act_color( AT_SAY, "You say OOC: '$T'", ch, NULL, argument, TO_CHAR, POS_RESTING );
    return;
}


void
do_shout( CHAR_DATA *ch, char *argument )
{
    /*
     * Don't let mobs shout, unless they're switched or ordered to.
     * Specifically, don't let MobProgs shout.
     */
    if ( !ch->desc && !IS_AFFECTED( ch, AFF_CHARM ) )
	return;

    if ( IS_SET( ch->comm, COMM_NOSHOUT ) )
    {
	send_to_char( "You can't shout.\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_SHOUT, "shout" );
    WAIT_STATE( ch, 12 );
    return;
}


void
do_review( CHAR_DATA *ch, char *argument )
{
    int		ptr0;
    int		ptr;

    if ( IS_NPC( ch ) )
	return;

    ptr = ptr0 = ch->pcdata->tell_buf_index;
    if ( ch->pcdata->tell_buf[ptr] == NULL )
    {
	send_to_char( "No tells.\n\r", ch );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	for ( ptr = 0; ptr < TELLBUF_SIZE; ptr++ )
	{
	    free_string( ch->pcdata->tell_buf[ptr] );
	    ch->pcdata->tell_buf[ptr] = NULL;
	}
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    do
    {
	ptr++;
	if ( ptr >= TELLBUF_SIZE )
	    ptr = 0;
	if ( ch->pcdata->tell_buf[ptr] != NULL )
	    send_to_char( ch->pcdata->tell_buf[ptr], ch );
    }
    while ( ptr != ptr0 );
}


void
do_tell( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    CHAR_DATA *	victim;

    if ( IS_SET( ch->comm, COMM_NOTELL) || IS_SET( ch->comm, COMM_DEAF ) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->comm, COMM_QUIET ) )
    {
	send_to_char( "You must turn off quiet mode first.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->comm, COMM_DEAF ) )
    {
	send_to_char( "You must turn off deaf mode first.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) && IS_SET( ch->in_room->room_flags, ROOM_SILENT ) )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    if ( IS_SET( ch->comm, COMM_AFK ) )
        send_to_char( "You are AFK and will not recieve a reply.\n\r", ch );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->in_room->room_flags, ROOM_SILENT )
    &&	 ( !IS_IMMORTAL( victim ) || !IS_IMMORTAL( ch ) ) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC( victim ) )
    {
	act( "$N seems to have misplaced $S link...try again later.",
	    ch, NULL, victim, TO_CHAR );
        sprintf( buf, "`W%s tells you '`Y%s`W'\n\r", PERS( ch, victim ), argument );
        buf[2] = UPPER( buf[2] );
        add_buf( victim->pcdata->buffer,buf );
        add_tell( victim, buf );
        sprintf( buf, "`WYou tell %s '`Y%s`W'\n\r", PERS( victim, ch ), argument );
        add_tell( ch, buf );
	return;
    }

    if ( !( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL) && !IS_AWAKE( victim ) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( ( IS_SET( victim->comm, COMM_QUIET )
	|| IS_SET( victim->comm, COMM_DEAF ) )
    && !IS_IMMORTAL( ch ) )
    {
	act( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
  	return;
    }

    if ( IS_SET( victim->comm, COMM_AFK ) )
    {
	if ( IS_NPC( victim ) )
	{
	    act( "$E is AFK, and not receiving tells.", ch, NULL, victim, TO_CHAR );
	    return;
	}

	act( "$E is AFK, but your tell will go through when $E returns.",
	    ch, NULL, victim, TO_CHAR );
        sprintf( buf, "`WYou tell %s '`Y%s`W'\n\r", PERS( victim, ch ), argument );
        add_tell( ch, buf );
	if ( !is_ignoring( victim, ch, IGNORE_TELLS ) )
	{
	    sprintf( buf, "`W%s tells you '`Y%s`W'\n\r", PERS( ch, victim ), argument );
	    buf[2] = UPPER( buf[2] );
	    add_buf( victim->pcdata->buffer, buf );
	    add_tell( victim, buf );
	}
	return;
    }

    if ( !IS_NPC( victim ) && victim->desc != NULL && victim->desc->pString != NULL )
    {
	act_color( C_DEFAULT,
	     "$E is editing some text, but your tell will go through when $E is finished.",
	     ch, NULL, victim, TO_CHAR, POS_DEAD );
        sprintf( buf, "`WYou tell %s '`Y%s`W'\n\r", PERS( victim, ch ), argument );
        add_tell( ch, buf );
	if ( !is_ignoring( victim, ch, IGNORE_TELLS ) )
	{
	    sprintf( buf, "`W%s tells you '`Y%s`W'\n\r", PERS( ch, victim ), argument );
	    buf[2] = UPPER( buf[2] );
	    add_buf( victim->pcdata->buffer, buf );
	    add_tell( victim, buf );
	}
	return;
    }

    MOBtrigger = FALSE;
    act_color( AT_TELL, "`WYou tell $N '`Y$t`W'", ch, argument, victim, TO_CHAR, POS_RESTING );
    sprintf( buf, "`WYou tell %s '`Y%s`W'\n\r", PERS( victim, ch ), argument );
    add_tell( ch, buf );
    if ( !is_ignoring( victim, ch, IGNORE_TELLS ) )
    {
        MOBtrigger = FALSE;
        act_color( AT_TELL, "`W$n tells you '`Y$t`W'",ch,argument,victim,TO_VICT,POS_DEAD);
        victim->reply	= ch;
        if ( ch != victim )
        {
	    sprintf( buf, "`W%s tells you '`Y%s`W'\n\r", PERS( ch, victim ), argument );
	    buf[2] = UPPER( buf[2] );
	    add_tell( victim, buf );
	}
    }

    mprog_tell_trigger( argument, victim, ch );
    return;
}

void                                                                                                                      
do_gocial(CHAR_DATA *ch, char *argument)                                                                                  
{                                                                                                                         
        SOCIAL_DATA *pSocial;                                                                                             
        BUFFER      *pBuf;                                                                                                
        char        arg[MIL];                                                                                             
        int         col;                                                                                                  
        DESCRIPTOR_DATA *d;
        char buf[MSL];                                                                                               
                                                                                                                          
        argument = one_argument( argument, arg);                                                                           
        if ( arg[0] == '\0' )    
        {           
                pBuf = new_buf( );    
                col = 0;    

                for ( pSocial = social_first; pSocial; pSocial = pSocial->next)    
                {    
                        if ( !pSocial->deleted )                                                                        
            {                                                                                                        
                buf_printf( pBuf, "%-12s", pSocial->name );                                                          
                if ( ++col % 6 == 0 )                                                                                
                    add_buf( pBuf, "\n\r" );    
                     for (d = descriptor_list; d != NULL; d = d->next)    
        {    
            CHAR_DATA *vch;    
            vch = d->original ? d->original : d->character;          
            if (d->connected == CON_PLAYING &&                       
                d->character != ch &&                                
                !IS_SET(vch->comm,COMM_NOCHANNELS))    
            {                                                  
                act_new(buf,ch,NULL,vch,TO_VICT,POS_DEAD);     
            }                                                  
        }                                                                         
            }                                                                                                        
        }                                                 
     
        if ( col % 6 != 0 )     
            add_buf( pBuf, "\n\r" );     

        page_to_char( buf_string( pBuf ), ch );     
        free_buf( pBuf );     
    }     
    else     
    {     
        if ( !check_social( ch, arg, argument ) )     
        {     
            do_socials( ch, "" );     
        }     
    }     
    return;     

}      
          
void
do_whisper( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    int		chance;
    bool	lastMufd;
    CHAR_DATA *	lch;
    char	overheard[MAX_STRING_LENGTH];
    char *	p;
    char	text[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    char *	whisper;
    char	word[MAX_INPUT_LENGTH];

    argument = one_argument (argument, arg);

    if (arg[0] == '\0' || *argument == '\0')
    {
	send_to_char( "Whisper what to whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char("They aren't here.\n\r", ch);		
	return;
    }

    p = stpcpy( text, makedrunk( ch, argument ) ) - 1;
    if ( !ispunct( *p ) )
        strcpy( p + 1, "." );
    p = text;
    while ( is_colcode( p ) )
        p += 2;
    *p = UPPER( *p );

    MOBtrigger = FALSE;
    act_color( AT_SAY, "`WYou `Pwhisper `Wto $N, '`X$t`W'", ch, text, victim, TO_CHAR, POS_RESTING );
    MOBtrigger = FALSE;
    if ( xIS_SET ( victim->affected_by, AFF_DEAF ) )
	act_color ( AT_SAY, "`W$n whispers something to you, but you cannot hear them.",
		    ch, NULL, victim, TO_VICT, POS_RESTING );
    else
        act_color( AT_SAY, "`W$n `Pwhispers `Wto you, '`X$t`W'", ch, text, victim, TO_VICT, POS_RESTING );

    for ( lch = ch->in_room->people; lch != NULL; lch = lch->next_in_room )
    {
        if ( lch == ch || lch == victim )
            continue;

        if ( IS_IMMORTAL( ch ) && get_trust( ch ) > get_trust( lch ) )
        {
	    sprintf( buf, "%s whispers something secretly to %s`X.", PERS( ch, lch ), PERS( victim, lch ) );
	    act_color( AT_ACTION, "$t", lch, buf, NULL, TO_CHAR, POS_RESTING );
	    continue;
        }

        whisper = text;
        p = overheard;
        *p = '\0';
        lastMufd = TRUE;
        while ( *whisper != '\0' )
        {
            whisper = first_arg( whisper, word, FALSE );
            chance = 20;	/* put skill mods here */
            if ( number_percent( ) > chance )
            {
                switch( number_bits( 2 ) )
                {
                    case 0:	p = stpcpy( p, "..." );	break;
                    case 1:	p = stpcpy( p, ".." );	break;
                    default:	p = stpcpy( p, "." );	break;
                }
                lastMufd = TRUE;
            }
            else
            {
                p += sprintf( p, "%s%s", lastMufd ? "" : " ", word );
                lastMufd = FALSE;
            }
        }

        p = overheard;
        while ( *p != '\0' )
        {
            if ( *p != '.' )
                break;
            p++;
        }
        if ( *p != '\0' )
        {
            sprintf( buf, "%s whispers to %s`W, '`G$t`W'", PERS( ch, lch ), PERS( victim, lch ) );
        }
        else
        {
            sprintf( buf, "%s whispers something secretly to %s.", PERS( ch, lch ), PERS( victim, lch ) );
        }

	if ( xIS_SET ( lch->affected_by, AFF_DEAF ) )
	    sprintf( buf, "%s whispers something to %s, but you are too deaf to hear.", 
		     PERS( ch, lch ), PERS( victim, lch ) );

	act_color( AT_ACTION, buf, lch, overheard, NULL, TO_CHAR, POS_RESTING );
    }

    MOBtrigger = FALSE;
    mprog_whisper_trigger( text, victim, ch );
    return;

}


void
do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if ( *argument == '\0' )
    {
        send_to_char( "Reply what?\n\r", ch );
        return;
    }

    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) && IS_SET( ch->in_room->room_flags, ROOM_SILENT ) )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->in_room->room_flags, ROOM_SILENT )
    &&	 ( !IS_IMMORTAL( victim ) || !IS_IMMORTAL( ch ) ) )
    {
	act_new( "$E can't hear you.", ch, 0, victim, TO_CHAR, POS_DEAD );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC( victim ) )
    {
        act( "$N seems to have misplaced $S link...try again later.",
            ch, NULL, victim, TO_CHAR );
        sprintf( buf, "`WYou reply to %s '`Y%s`W'\n\r", PERS( victim, ch ), argument );
        add_tell( ch, buf );
        sprintf( buf, "`W%s tells you '`Y%s`W'\n\r", PERS( ch, victim ), argument );
        buf[2] = UPPER( buf[2] );
        add_buf( victim->pcdata->buffer, buf );
        add_tell( victim, buf );
        return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE( victim ) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( ( IS_SET( victim->comm, COMM_QUIET )
	|| IS_SET( victim->comm, COMM_DEAF ) )
    &&  !IS_IMMORTAL( ch ) && !IS_IMMORTAL( victim ) )
    {
        act_new( "$E is not receiving tells.", ch, 0, victim, TO_CHAR, POS_DEAD );
        return;
    }

    if ( !IS_IMMORTAL( victim ) && !IS_AWAKE( ch ) )
    {
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
    }

    if ( IS_SET( victim->comm, COMM_AFK ) )
    {
        if ( IS_NPC( victim ) )
        {
            act_new( "$E is AFK, and not receiving tells.",
		ch, NULL, victim, TO_CHAR, POS_DEAD );
            return;
        }

        act_new( "$E is AFK, but your tell will go through when $E returns.",
            ch, NULL, victim, TO_CHAR, POS_DEAD );
        sprintf( buf, "`WYou reply to %s '`Y%s`W'\n\r", PERS( victim, ch ), argument );
        add_tell( ch, buf );
        sprintf( buf, "`W%s tells you '`Y%s`W'\n\r", PERS( ch, victim ), argument );
	buf[2] = UPPER( buf[2] );
        add_buf( victim->pcdata->buffer, buf );
        add_tell( victim, buf );
        return;
    }

    if ( !IS_NPC( victim ) && victim->desc != NULL && victim->desc->pString != NULL )
    {
	act_color( C_DEFAULT,
	     "$E is editing some text, but your tell will go through when $E is finished.",
	     ch, NULL, victim, TO_CHAR, POS_DEAD );
        sprintf( buf, "`WYou reply to %s '`Y%s`W'\n\r", PERS( victim, ch ), argument );
        add_tell( ch, buf );
	sprintf( buf, "`W%s tells you '`Y%s`W'\n\r", PERS( ch, victim ), argument );
	buf[2] = UPPER( buf[2] );
	add_buf( victim->pcdata->buffer, buf );
	add_tell( victim, buf );
	return;
    }

    sprintf( buf, "`WYou reply to %s '`Y%s`W'\n\r", PERS( victim, ch ), argument );        add_tell( ch, buf );
    if ( ch != victim )
    {
        sprintf( buf, "`W%s tells you '`Y%s`W'\n\r", PERS( ch, victim ), argument );
        buf[2] = UPPER( buf[2] );
        add_tell( victim, buf );
    }

    MOBtrigger = FALSE;
    act_color( AT_TELL, "`WYou reply to $N '`X$t`W'", ch, argument, victim,
	       TO_CHAR, POS_DEAD );
    MOBtrigger = FALSE;
    act_color( AT_TELL, "`W$n tells you '`X$t`W'", ch, argument, victim,
	       TO_VICT, POS_DEAD );
    victim->reply	= ch;

    mprog_tell_trigger( argument, victim, ch );
    return;
}


void
do_yell( CHAR_DATA *ch, char *argument )
{
    if ( IS_SET( ch->comm, COMM_NOSHOUT ) )
    {
	send_to_char( "You can't yell.\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_YELL, "yell" ); 
}


void do_emote (CHAR_DATA * ch, char * argument)  
{  
    char output[MSL];  
    int npos = 0, pos = 0;  
    bool fSay = FALSE, addname = FALSE;  
   
    if (argument[0] == '\0')  
    {  
        send_to_char ("Syntax: emote <text>\n\r", ch);  
        return;  
    }  
   
    if (strstr (argument, ch->name) == NULL)  
		addname = TRUE;  
   
    if (strstr (argument, "`") != NULL)  
    {  
        send_to_char ("Please do not use color codes in your emotes.\n\r", ch);  
        return;  
    }  
   
    while (*argument != '\0')  
    {  
        if (npos > (MSL - 4))  
            break;  
   
        switch(*argument)  
        {  
            default:  
                output[npos++] = *argument++;  
                break;  
            case '@':  
                for (pos = 0; ch->name[pos] != '\0'; pos++)  
                    output[npos++] = ch->name[pos];  
                argument++;  
                break;  
            case '\"':  
                if (!fSay)  
                {  
                    fSay = TRUE;  
                    output[npos++] = '\"';  
                    output[npos++] = '`';  
                    output[npos++] = 'G';  
					argument++;  
                    break;  
                }  
                else  
                {  
                    fSay = FALSE;  
                    output[npos++] = '`';  
                    output[npos++] = 'g';  
                    output[npos++] = '\"';  
					argument++;  
                    break;  
                }  
                break;  
        }  
    }  
    output[npos] = '\0';  
   
	//Simply remove the MOBtrigger lines if your MUD doesn't have progs.  
    MOBtrigger = FALSE;  
	if (addname)  
	{  
		act ("`g$n $T `W(`X$n`W)`X", ch, NULL, output, TO_CHAR);  
		act ("`g$n $T `W(`X$n`W)`X", ch, NULL, output, TO_ROOM);  
	}  
	else  
	{  
		act ("`g$T `W(`X$n`W)`X", ch, NULL, output, TO_CHAR);  
		act ("`g$T `W(`X$n`W)`X", ch, NULL, output, TO_ROOM);  
	}  
    MOBtrigger = TRUE;  
}  

void
do_oldemote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH*2];
    char *pLast;

    if ( !IS_NPC( ch ) && IS_SET( ch->comm, COMM_NOEMOTE ) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\n\r", ch );
	return;
    }

    buf[0] = ' ';
    pLast = stpcpy( *argument == '\'' || *argument == ',' ? buf : &buf[1], argument );
    if ( !ispunct( *(pLast - 1) ) )
	strcpy( pLast, "." );
    act_color( AT_EMOTE, "$n$T", ch, NULL, buf, TO_ROOM, POS_RESTING );
    act_color( AT_EMOTE, "$n$T", ch, NULL, buf, TO_CHAR, POS_RESTING );
    return;
}


void
do_pmote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char *letter,*name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\n\r", ch );
	return;
    }

    act_color( AT_EMOTE, "$n $t", ch, argument, NULL, TO_CHAR, POS_RESTING );

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
	if ( /* vch->desc == NULL || */ vch == ch )
	    continue;

	if ( ( letter = strstr( argument, vch->name ) ) == NULL )
	{
	    act_color( AT_EMOTE, "$n $t", ch, argument, vch, TO_VICT, POS_RESTING );
	    continue;
	}

	strcpy( temp, argument );
	temp[strlen( argument ) - strlen( letter )] = '\0';
   	last[0] = '\0';
 	name = vch->name;

	for ( ; *letter != '\0'; letter++ )
	{
	    if ( *letter == '\'' && matches == strlen( vch->name ) )
	    {
		strcat( temp,"r" );
		continue;
	    }

	    if ( *letter == 's' && matches == strlen( vch->name ) )
	    {
		matches = 0;
		continue;
	    }

 	    if ( matches == strlen( vch->name ) )
	    {
		matches = 0;
	    }

	    if ( *letter == *name )
	    {
		matches++;
		name++;
		if ( matches == strlen( vch->name ) )
		{
		    strcat( temp, "you" );
		    last[0] = '\0';
		    name = vch->name;
		    continue;
		}
		strncat( last, letter, 1 );
		continue;
	    }

	    matches = 0;
	    strcat( temp, last );
	    strncat( temp, letter, 1 );
	    last[0] = '\0';
	    name = vch->name;
	}

	act_color( AT_EMOTE, "$n $t", ch, temp, vch, TO_VICT, POS_RESTING );
    }

    return;
}


void
do_remote( CHAR_DATA *ch, char *argument )
{
    char		buf[MAX_STRING_LENGTH];
    char		arg[MAX_INPUT_LENGTH];
    CHAR_DATA *		victim;
    ROOM_INDEX_DATA *	pRoom;
    char *		plast;

    if ( !IS_NPC( ch ) && IS_SET( ch->comm, COMM_NOEMOTE ) )
    {
	send_to_char( "You are an emotionless blob.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Emote to whom?\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) )
	|| ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
/*
    if ( IS_SET( victim->in_room->room_flags, ROOM_SILENT ) )
    {
        act( "$E can't hear you.", ch, NULL, victim, TO_CHAR );
        return;
    }
*/
    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC( victim ) && ( !( victim->desc ) ) )
    {
        act( "$E is link-dead.", ch, NULL, victim, TO_CHAR );
        return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
	;

    buf[ 0 ] = ' ';
    strcpy( argument[0] == '\'' ? buf : &buf[1], argument );
    if ( isalpha( plast[-1] ) )
	strcat( buf, "." );

    pRoom = ch->in_room;
    if ( ch != victim )
    {
	char_from_room( ch );
	char_to_room( ch, victim->in_room );
    }
    if ( !is_ignoring( victim, ch, IGNORE_CHANNELS ) )
        act_color( AT_EMOTE, "From far away, $n$t", ch, buf, victim, TO_VICT, POS_RESTING );
    act_color( AT_EMOTE, "From far away, $n$t", ch, buf, victim, TO_CHAR, POS_RESTING );
    char_from_room( ch );
    char_to_room( ch, pRoom );
    return;
}


void
do_pose( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_INPUT_LENGTH+2];
    char	word[MAX_INPUT_LENGTH];
    char *	p;
    bool	fNoPose;

    if ( IS_NPC( ch ) )
    {
        send_to_char( "You can't pose.\n\r", ch );
        return;
    }

    p = one_argument( argument, word );
    if ( !str_cmp( word, ch->name ) )
        strcpy( buf, p );
    else
        strcpy( buf, argument );
    smash_tilde( buf );

    if ( buf[0] == '\0' )
    {
        if ( IS_NULLSTR( ch->pcdata->pose ) )
        {
            send_to_char( "Your pose is unset.\n\r", ch );
            return;
        }
        send_to_char( "Your pose is:\n\r", ch );
        send_to_char( ch->name, ch );
        if ( !ispunct( *ch->pcdata->pose ) )
            send_to_char( " ", ch );
        send_to_char( ch->pcdata->pose, ch );
        send_to_char( "`X\n\r", ch );
        send_to_char( "Pose is ", ch );
        send_to_char( IS_SET( ch->act2, PLR_POSE ) ? "ON" : "Off", ch );
        send_to_char( ".\n\r", ch );
        return;
    }

    if ( !str_cmp( buf, "off" ) )
    {
        REMOVE_BIT( ch->act2, PLR_POSE );
        send_to_char( "Pose is off.\n\r", ch );
        return;
    }

    if ( !str_cmp( buf, "on" ) )
    {
        SET_BIT( ch->act2, PLR_POSE );
        send_to_char( "Pose is now ON.\n\r", ch );
        if ( IS_NULLSTR( ch->pcdata->pose ) )
        {
            send_to_char( "However, you currently have no pose set.\n\r", ch );
        }
        return;
    }

    if ( !str_cmp( buf, "none" ) || !str_cmp( buf, "clear" ) )
    {
        free_string( ch->pcdata->pose );
        ch->pcdata->pose = &str_empty[0];
        send_to_char( "Pose cleared.\n\r", ch );
        return;
    }

    if ( strlen_wo_col( buf ) > 70 || strlen( buf ) > 256 )
    {
        send_to_char( "Pose too long.\n\r", ch );
        return;
    }

    p = buf + strlen( buf ) - 1;
    if ( !ispunct( *p ) )
        strcpy( p + 1, "." );

    fNoPose = IS_NULLSTR( ch->pcdata->pose );

    free_string( ch->pcdata->pose );
    ch->pcdata->pose = str_dup( buf );
    if ( fNoPose )
        SET_BIT( ch->act2, PLR_POSE );
    send_to_char( "Pose set.\n\r", ch );
    if ( !IS_SET( ch->act2, PLR_POSE ) )
        send_to_char( "Pose is OFF.\n\r", ch );

    return;
}


void
do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    send_to_char( "Bug logged.\n\r", ch );
    return;
}

void
do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Typo logged.\n\r", ch );
    return;
}

void
do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}


void
do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *	d;
    DESCRIPTOR_DATA *	d_next;
    char		buf[MAX_STRING_LENGTH];
    CHAR_DATA *		mob;
    CHAR_DATA *		mob_next;
    int			id;

    if ( IS_NPC(ch) )
	return;

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }

    if ( current_time - ch->logon < 15 )
    {
        send_to_char( "Type delete again to confirm this command.\n\r", ch );
        send_to_char( "WARNING: this command is irreversible.\n\r", ch );
        send_to_char( "Typing delete with an argument will undo delete status.\n\r\n\r",
            ch );
        if ( ch->desc != NULL )
            bust_a_prompt( ch );
        send_to_char( "delete\n\r", ch );
    }

    //send_to_char( 
    //"Thank you for visiting MARS! We hope to see you again soon!\n\r", ch );
    do_quote (ch);

    if ( !IS_SET( ch->act2, PLR_PLOADED ) )
    {
	act( "$n has left MARS", ch, NULL, NULL, TO_ROOM );
	sprintf( log_buf, "%s has quit.", ch->name );
	log_string( log_buf );
	wiznet("$N has left MARS.",ch,NULL,WIZ_LOGINS,0,get_trust(ch));
    }

    if ( auc_held && ch == auc_held && auc_obj )
    {
	if ( auc_bid )
	{
	    if ( auc_bid->money.gold < auc_cost )
	    {
		sprintf( buf, "Holder of %s has left; bidder cannot pay for item; returning to owner.", auc_obj->short_descr );
		act( "$p appears in the hands of $n.", ch, auc_held, NULL, TO_ROOM );
		obj_to_char( auc_obj, ch );
	    }
	    else
	    {
		sprintf( buf, "Holder of %s has left; selling item to last bidder.",
			 auc_obj->short_descr );
		act( "$p appears in your hands.", auc_bid, auc_obj, NULL, TO_CHAR );
		act( "$p appears in the hands of $n.", auc_bid, auc_obj, NULL, TO_ROOM );
		obj_to_char( auc_obj, auc_bid );
		auc_bid->money.gold -= auc_cost;
		ch->money.gold += auc_cost;
	    }
	}
	else
	{
	    sprintf( buf, "Holder of %s has left; removing item from auction.",
		     auc_obj->short_descr );
	    act( "$p appears in the hands of $n.", ch, auc_obj, NULL, TO_ROOM );
	    obj_to_char( auc_obj, ch );
	}
	auc_channel( buf );
	auc_obj = NULL;
	auc_bid = NULL;
	auc_held = NULL;
	auc_cost = 0;
	auc_count = -1;
    }

    if ( auc_bid && auc_bid == ch && auc_obj )
    {
	sprintf( buf, "Highest bidder for %s has left; returning item to owner.", auc_obj->short_descr );
	if ( auc_held )
	{
	    act( "$p appears in your hands.", auc_held, auc_obj, NULL, TO_CHAR );
	    act( "$p appears in the hands of $n.", auc_held, auc_obj, NULL, TO_ROOM );
	    obj_to_char( auc_obj, auc_held );
	}
	auc_channel( buf );
	auc_obj = NULL;
	auc_bid = NULL;
	auc_held = NULL;
	auc_cost = 0;
	auc_count = -1;
    }

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj( ch );
    if ( ch->pcdata->deathstate == DEATH_SEARCH )
    {
        for ( mob = char_list; mob != NULL; mob = mob_next )
        {
            mob_next = mob->next;
            if ( IS_NPC( mob ) && mob->id == ch->pcdata->deathmob )
            {
                extract_char( mob, TRUE );
                break;
            }
        }
    }
    update_userlist( ch, FALSE );
    id = ch->id;
    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d != NULL )
	close_socket( d );

    /* toast evil cheating bastards */
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
	CHAR_DATA *tch;

	d_next = d->next;
	tch = d->original ? d->original : d->character;
	if ( tch && tch->id == id )
	{
	    extract_char( tch, TRUE );
	    close_socket( d );
	} 
    }

    return;
}


void
do_follow( CHAR_DATA *ch, char *argument )
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master != NULL )
    {
	act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( ch->master == NULL )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }

    if (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOFOLLOW) && !IS_IMMORTAL(ch))
    {
	act("$N doesn't seem to want any followers.\n\r",
             ch,NULL,victim, TO_CHAR);
        return;
    }

    REMOVE_BIT(ch->act,PLR_NOFOLLOW);

    if ( ch->master != NULL )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}


void
add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( ch->master == NULL )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	xREMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    if ( ch->in_room != NULL && ch->master->in_room != NULL && can_see( ch->master, ch ) )
    {
	act( "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
    	act( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );
    }
    if (ch->master->pet == ch)
	ch->master->pet = NULL;

    ch->master = NULL;
    ch->leader = NULL;
    return;
}

/* nukes charmed monsters and pets */
void
nuke_pets( CHAR_DATA *ch )
{    
    CHAR_DATA *pet;

    /* Assumes mobs don't have pets/mounts; not sure this will always be true. */
    if ( IS_NPC( ch ) )
	return;

    if ( ( pet = ch->pet ) != NULL )
    {
    	stop_follower( pet );
    	if ( pet->in_room != NULL )
    	    act_color( AT_ACTION, "$N slowly fades away.", ch, NULL, pet, TO_NOTVICT, POS_RESTING );
    	extract_char( pet, TRUE );
    }
    ch->pet = NULL;

    if ( ch->mount != NULL
    &&	 ( ch->mount->in_room == ch->in_room || ch->mount->in_room == NULL ) )
    {
	pet = ch->mount;
	do_dismount( ch, "" );
	if ( pet->in_room != NULL )
	    act_color( AT_ACTION, "$N slowly fades away.", ch, NULL, pet, TO_NOTVICT, POS_RESTING );
	else
	    log_string( "Nuke_pets: Extracting null pet" );
        ch->mount = NULL;
        ch->riding = FALSE;            
	extract_char( pet, TRUE );
    }
    else if ( ch->mount != NULL )
    {
	ch->mount->mount = NULL;
	ch->mount->riding = FALSE;
    }
    ch->mount = NULL;

    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master != NULL )
    {
    	if (ch->master->pet == ch)
    	    ch->master->pet = NULL;
	stop_follower( ch );
    }

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }

    return;
}



void
do_order( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    char	arg[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    CHAR_DATA *	och;
    CHAR_DATA *	och_next;
    int		cmd;
    bool	found;
    bool	fAll;

    argument = one_argument( argument, arg );
    one_argument(argument,arg2);

    if (!str_cmp(arg2,"delete"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }

    cmd = get_cmd_by_name( argument );
    if ( cmd != NO_COMMAND
    &&   !IS_IMMORTAL( ch )
    &&   IS_SET( cmd_table[cmd].flags, CMD_MOB ) )
    {
	send_to_char( "Do it yourself!\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;

        if ( !strcmp( arg, "mount" ) )
        {
            if (ch->mount == NULL )
            {
                send_to_char( "Your don't have a mount.\n\r", ch );
                return;
            }

            if ( ch->mount->in_room != ch->in_room )
            {
                send_to_char( "Your mount isn't here!\n\r", ch );
                return;
            }
            else
            {
                victim = ch->mount;
            }
        }
	else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( victim->mount == ch )
	{
	    if ( !mount_success( ch, victim, FALSE ) )
	    {
		act( "$n ignores your orders.", ch, NULL, victim, TO_CHAR );
		return;
	    }
	    else
	    {
		sprintf( buf, "$n orders you to '%s'.", argument );
		act( buf, ch, NULL, victim, TO_VICT );
		interpret( victim, argument );
		return;
	    }
	}
	else if ( !IS_AFFECTED( victim, AFF_CHARM ) || victim->master != ch 
	||	  ( IS_IMMORTAL( victim ) && victim->trust >= ch->trust ) )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	    sprintf( buf, "$n orders you to '%s'.", argument );
	    act( buf, ch, NULL, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
    {
	WAIT_STATE(ch,PULSE_VIOLENCE);
	send_to_char( "Ok.\n\r", ch );
    }
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}



void do_group( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = (ch->leader != NULL) ? ch->leader : ch;
	sprintf( buf, "%s's group:\n\r", PERS(leader, ch) );
	send_to_char( buf, ch );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		sprintf( buf,
		"[%2d %s] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
		    gch->level,
		    IS_NPC(gch) ? "Mob" : class_table[gch->class].who_name,
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   gch->max_hit,
		    gch->mana,  gch->max_mana,
		    gch->move,  gch->max_move,
		    gch->exp    );
		send_to_char( buf, ch );
	    }
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act_new("$N isn't following you.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
	return;
    }

    if (IS_AFFECTED(victim,AFF_CHARM))
    {
        send_to_char("You can't remove charmed mobs from your group.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM))
    {
    	act_new("You like your master too much to leave $m!",
	    ch,NULL,victim,TO_VICT,POS_SLEEPING);
    	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act_new("$n removes $N from $s group.",
	    ch,NULL,victim,TO_NOTVICT,POS_RESTING);
	act_new("$n removes you from $s group.",
	    ch,NULL,victim,TO_VICT,POS_SLEEPING);
	act_new("You remove $N from your group.",
	    ch,NULL,victim,TO_CHAR,POS_SLEEPING);
	return;
    }

    victim->leader = ch;
    act_new("$N joins $n's group.",ch,NULL,victim,TO_NOTVICT,POS_RESTING);
    act_new("You join $n's group.",ch,NULL,victim,TO_VICT,POS_SLEEPING);
    act_new("$N joins your group.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char	msg_char[MAX_INPUT_LENGTH];
    char	msg_grp[MAX_INPUT_LENGTH];
    CHAR_DATA *	gch;
    int		members;
    MONEY	amount;
    MONEY	share;
    MONEY	extra;
    bitvector	values;

    if ( *argument == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }

    if ( money_value( &amount, argument ) == NULL )
    {
	send_to_char( "Split how much of what?\n\r", ch );
	return;
    }

    if ( amount.gold  < 0 || amount.silver < 0
    ||   amount.copper < 0 || amount.fract < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount.gold  == 0 && amount.silver == 0
    &&   amount.copper == 0 && amount.fract == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }

    if ( ch->money.gold  <  amount.gold  || ch->money.silver < amount.silver
    ||   ch->money.copper <  amount.copper || ch->money.fract < amount.fract )
    {
	send_to_char( "You don't have that much to split.\n\r", ch );
	return;
    }

    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
	    members++;
    }

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }

    ch->money.gold	-= amount.gold;
    ch->money.silver	-= amount.silver;
    ch->money.copper	-= amount.copper;
    ch->money.fract	-= amount.fract;

    share.gold	 = amount.gold   / members;
    share.silver = amount.silver / members;
    share.copper = amount.copper / members;
    share.fract  = amount.fract  / members;

    extra.gold	 = amount.gold   % members;
    extra.silver = amount.silver % members;
    extra.copper = amount.copper % members;
    extra.fract  = amount.fract  % members;

    if ( share.gold   == 0 && share.silver == 0
    &&   share.copper == 0 && share.fract  == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    /*
     * Okay... there are sixteen possible combinations of coins we could
     * be splitting here.  We've already trapped one of them (all amounts
     * zero), now we do some bit twiddling to determine what messages
     * to display.
     */
    values				= 0;
    if ( amount.gold	!= 0 )	values |= 8;	/* 1000 */
    if ( amount.silver	!= 0 )	values |= 4;	/* 0100 */
    if ( amount.copper	!= 0 )	values |= 2;	/* 0010 */
    if ( amount.fract	!= 0 )	values |= 1;	/* 0001 */
    switch( values )
    {
	default:	/* should never happen */
	    strcpy( msg_char, "You give everyone a serious problem!" );
	    strcpy( msg_grp,  "$n gives you a serious problem!" );
	    break;
	case  1:	/* 0001: fract copper only */
	    sprintf( msg_char, "You split %d %s.\n\r"
			       "Your share is %d %s.",
		     amount.fract, amount.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.fract + extra.fract,
		     share.fract + extra.fract    == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s.\n\r"
			      "Your share is %d %s.",
		     amount.fract, amount.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.fract,  share.fract    == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    break;
	case  2:	/* 0010: copper only */
	    sprintf( msg_char, "You split %d %s.\n\r"
			       "Your share is %d %s.",
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.copper + extra.copper,
		     share.copper + extra.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s.\n\r"
			      "Your share is %d %s.",
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.copper,  share.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL );
	    break;
	case  3:	/* 0011: coppers and fractional copper */
	    sprintf( msg_char, "You split %d %s and %d %s.\n\r"
			       "Your share is %d %s and %d %s.",
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.copper + extra.copper,
		     share.copper + extra.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.fract + extra.fract,
		     share.fract + extra.fract	  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s and %d %s.\n\r"
			      "Your share is %d %s and %d %s.",
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.copper,  share.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.fract,   share.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    break;
	case  4:	/* 0100: silver only */
	    sprintf( msg_char, "You split %d %s.\n\r"
			       "Your share is %d %s.",
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.silver + extra.silver,
		     share.silver + extra.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s.\n\r"
			      "Your share is %d %s.",
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.silver,  share.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL );
	    break;
	case  5:	/* 0101: silver and fractional copper */
	    sprintf( msg_char, "You split %d %s and %d %s.\n\r"
			       "Your share is %d %s and %d %s.",
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.fract,   amount.fract == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.silver + extra.silver,
		     share.silver + extra.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.fract + extra.fract,
		     share.fract + extra.fract	  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s and %d %s.\n\r"
			      "Your share is %d %s and %d %s.",
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.silver,  share.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.fract,   share.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    break;
	case  6:	/* 0110: silver and copper */
	    sprintf( msg_char, "You split %d %s and %d %s.\n\r"
			       "Your share is %d %s, and %d %s.",
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.silver + extra.silver,
		     share.silver + extra.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.copper + extra.copper,
		     share.copper + extra.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s, and %d %s.\n\r"
			      "Your share is %d %s, and %d %s.",
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.silver,  share.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.copper,  share.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL );
	    break;
	case  7:	/* 0111: silver, copper and fractional copper */
	    sprintf( msg_char, "You split %d %s, %d %s and %d %s.\n\r"
			       "Your share is %d %s, %d %s and %d %s.",
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.silver + extra.silver,
		     share.silver + extra.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.copper + extra.copper,
		     share.copper + extra.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.fract + extra.fract,
		     share.fract + extra.fract	  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    sprintf( msg_grp, "$n splits %d s%s, %d %s and %d %s.\n\r"
			      "Your share is %d %s, %d %s and %d %s.",
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.silver,  share.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.copper,  share.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.fract,   share.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    break;
	case  8:	/* 1000: gold only */
	    sprintf( msg_char, "You split %d %s.\n\r"
			       "Your share is %d %s.",
		     amount.gold, amount.gold     == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.gold + extra.gold,
		     share.gold + extra.gold      == 1 ? GOLD_NOUN   : GOLD_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s.\n\r"
			      "Your share is %d %s.",
		     amount.gold, amount.gold     == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.gold,  share.gold      == 1 ? GOLD_NOUN   : GOLD_PLURAL );
	    break;
	case  9:	/* 1001: gold and fract copper */
	    sprintf( msg_char, "You split %d %s and %d %s.\n\r"
			       "Your share is %d %s and %d %s.",
		     amount.gold,  amount.gold    == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.fract, amount.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.gold + extra.gold,
		     share.gold + extra.gold	  == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.fract + extra.fract,
		     share.fract + extra.fract	  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s and %d %s.\n\r"
			      "Your share is %d %s and %d %s.",
		     amount.gold,  amount.gold    == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.fract, amount.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.gold,   share.gold     == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.fract,  share.fract    == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    break;
	case 10:	/* 1010: gold and copper */
	    sprintf( msg_char, "You split %d %s and %d %s.\n\r"
			       "Your share is %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.gold + extra.gold,
		     share.gold + extra.gold	  == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.copper + extra.copper,
		     share.copper + extra.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s and %d %s.\n\r"
			      "Your share is %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.gold,    share.gold    == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.copper,  share.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL );
	    break;
	case 11:	/* 1011: gold, copper and fract copper */
	    sprintf( msg_char, "You split %d %s, %d %s and %d %s.\n\r"
			       "Your share is %d %s, %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.gold + extra.gold,
		     share.gold + extra.gold	  == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.copper + extra.copper,
		     share.copper + extra.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.fract + extra.fract,
		     share.fract + extra.fract	  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s, %d %s and %d %s.\n\r"
			      "Your share is %d %s, %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.gold,    share.gold    == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.copper,  share.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.fract,   share.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    break;
	case 12:	/* 1100: gold and silver */
	    sprintf( msg_char, "You split %d %s and %d %s.\n\r"
			       "Your share is %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.gold + extra.gold,
		     share.gold + extra.gold	  == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.silver + extra.silver,
		     share.silver + extra.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s and %d %s.\n\r"
			      "Your share is %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.gold,    share.gold    == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.silver,  share.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL );
	    break;
	case 13:	/* 1011: gold, silver, and fract copper */
	    sprintf( msg_char, "You split %d %s, %d %s and %d %s.\n\r"
			       "Your share is %d %s, %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.gold + extra.gold,
		     share.gold + extra.gold	  == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.silver + extra.silver,
		     share.silver + extra.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.fract + extra.fract,
		     share.fract + extra.fract	  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s, %d %s and %d %s.\n\r"
			      "Your share is %d %s, %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.gold,    share.gold    == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.silver,  share.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.fract,   share.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    break;
	case 14:	/* 1110: gold, silver, and copper */
	    sprintf( msg_char, "You split %d %s, %d %s and %d %s.\n\r"
			       "Your share is %d %s, %d %s, and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.gold + extra.gold,
		     share.gold + extra.gold	  == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.silver + extra.silver,
		     share.silver + extra.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.copper + extra.copper,
		     share.copper + extra.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s, %d %s, and %d %s.\n\r"
			      "Your share is %d %s, %d %s, and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.gold,    share.gold    == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.silver,  share.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.copper,  share.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL );
	    break;
	case 15:	/* 1111: all of them */
	    sprintf( msg_char, "You split %d %s, %d %s, %d %s and %d %s.\n\r"
			       "Your share is %d %s, %d %s, %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.gold + extra.gold,
		     share.gold + extra.gold	  == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.silver + extra.silver,
		     share.silver + extra.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.copper + extra.copper,
		     share.copper + extra.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.fract + extra.fract,
		     share.fract + extra.fract	  == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    sprintf( msg_grp, "$n splits %d %s, %d %s, %d %s and %d %s.\n\r"
			      "Your share is %d %s, %d %s, %d %s and %d %s.",
		     amount.gold,   amount.gold   == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     amount.silver, amount.silver == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     amount.copper, amount.copper == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     amount.fract,  amount.fract  == 1 ? FRACT_NOUN  : FRACT_PLURAL,
		     share.gold,    share.gold    == 1 ? GOLD_NOUN   : GOLD_PLURAL,
		     share.silver,  share.silver  == 1 ? SILVER_NOUN : SILVER_PLURAL,
		     share.copper,  share.copper  == 1 ? COPPER_NOUN : COPPER_PLURAL,
		     share.fract,   share.fract   == 1 ? FRACT_NOUN  : FRACT_PLURAL );
	    break;
    }

    act_color( AT_ACTION, msg_char, ch, NULL, NULL, TO_CHAR, POS_RESTING );
    ch->money.gold	+= share.gold + extra.gold;
    ch->money.silver	+= share.silver + extra.silver;
    ch->money.copper	+= share.copper + extra.copper;
    ch->money.fract	+= share.fract + extra.fract;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group(gch,ch) && !IS_AFFECTED( gch, AFF_CHARM ) )
	{
	    act_color( AT_ACTION, msg_grp, ch, NULL, gch, TO_VICT, POS_RESTING );
	    gch->money.gold	+= share.gold;
	    gch->money.silver	+= share.silver;
	    gch->money.copper	+= share.copper;
	    gch->money.fract	+= share.fract;
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	gch;
    bool	grouped;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    grouped = FALSE;
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	{
	    if ( ch != gch )
		grouped = TRUE;
	    act_color( AT_PURPLE, "`w$n tells the group '`p$t`w'",
		ch, argument, gch, TO_VICT, POS_SLEEPING );
	}
    }
    if ( grouped )
	act_color( AT_PURPLE, "`wYou tell the group '`p$t`w'",
	    ch, argument, NULL, TO_CHAR, POS_SLEEPING );
    else
	send_to_char( "There's nobody else in your group.\n\r", ch );
    return;
}


void
do_clear( CHAR_DATA *ch, char *argument )
{
    int i;

    if ( ch->desc != NULL && ch->desc->ansi )
	send_to_char( "\033[2J\033[H", ch );
    else
	for ( i = -2; i < ch->lines; i++ )
	    send_to_char( "\n\r", ch );
}


void
do_info( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		indx;
    bool	leftcol;
    BUFFER *	pBuf;
    CHAR_DATA *	rch;

    one_argument( argument, arg );

    rch = ch->desc == NULL || ch->desc->original == NULL ? ch : ch->desc->original;

    if ( arg[0] == '\0' )
    {
	if ( IS_SET( ch->info, INFO_ON ) )
	    do_info( ch, "status" );
	else
	    do_info( ch, "on" );
	return;
    }

    if ( !str_prefix( arg, "help" ) )
    {
	send_to_char( "Info commands:\n\r", ch );
	send_to_char( "   on     - Info ON.\n\r", ch );
	send_to_char( "   off    - Info OFF.\n\r", ch );
	send_to_char( "   all    - Turn all Info channels ON.\n\r", ch );
	send_to_char( "   status - Show current Info settings.\n\r", ch );
	send_to_char( "  <flags> - Toggle Info channel(s)\n\r", ch );
	send_to_char( "   help   - Show this help.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "on" ) )
    {
	send_to_char( "Info system now ON.\n\r", ch );
	SET_BIT( ch->info, INFO_ON );
	SET_BIT( rch->info, INFO_ON );
	return;
    }

    if ( !str_prefix( arg, "off" ) )
    {
	send_to_char( "Info system now OFF.\n\r", ch );
	REMOVE_BIT( ch->info, INFO_ON );
	REMOVE_BIT( rch->info, INFO_ON );
	return;
    }

    if ( !str_prefix( arg, "all" ) )
    {
	for ( indx = 0; !IS_NULLSTR( info_table[indx].name ); indx++ )
	{
	    if ( info_table[indx].level <= get_trust( ch ) )
	    {
		SET_BIT( ch->info, info_table[indx].flag );
		SET_BIT( rch->info, info_table[indx].flag );
	    }
	}
	send_to_char( "All Info channels ON.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "status" ) )
    {
	pBuf = new_buf( );
	leftcol = TRUE;
	add_buf( pBuf,
		 "    8--------------------8--------------------8\n\r"
		 "    | Channel     Status | Channel     Status |\n\r"
		 "    8--------------------8--------------------8"  );
	for ( indx = 0; !IS_NULLSTR( info_table[indx].name ); indx++ )
	{
	    if ( info_table[indx].level <= get_trust( ch )
	    &&	 info_table[indx].flag != INFO_ON )
	    {
		if ( leftcol )
		    add_buf( pBuf, "\n\r    |" );
		leftcol = !leftcol;
		if ( IS_SET( rch->info, info_table[indx].flag ) )
		    buf_printf( pBuf, " `C%-14s  `GON `w|", all_capitalize( info_table[indx].name ) );
		else
		    buf_printf( pBuf, " `C%-14s `coff `w|", info_table[indx].name );
	    }
	}

	if ( !leftcol )
	    add_buf( pBuf, "                    |" );
	add_buf( pBuf, "\n\r" );

	add_buf( pBuf, "    8--------------------8--------------------8\n\r" );
	add_buf( pBuf, "    8               `CInfo `wis " );
	add_buf( pBuf, IS_SET( rch->info, INFO_ON ) ? "`GON " : "`coff" );
	add_buf( pBuf, "`X               8\n\r" );
	add_buf( pBuf, "    8--------------------8--------------------8\n\r" );

	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return;
    }

    while ( *argument != '\0' )
    {
	argument = one_argument( argument, arg );
	indx = info_lookup( arg );

	if ( indx == NO_VALUE || get_trust( ch ) < info_table[indx].level )
	{
	    ch_printf( ch, "No such channel: %s.\n\r", arg );
	    continue;
	}

	if ( IS_SET( ch->info, info_table[indx].flag ) )
	{
	    ch_printf( ch, "You will no longer see `C%s`X Info.\n\r",
		       info_table[indx].name );
	    REMOVE_BIT( ch->info, info_table[indx].flag );
	    REMOVE_BIT( rch->info, info_table[indx].flag );
	}
	else
	{
	    ch_printf( ch, "You will now see `C%s`X Info.\n\r",
		       all_capitalize( info_table[indx].name ) );
	    SET_BIT( ch->info, info_table[indx].flag );
	    SET_BIT( rch->info, info_table[indx].flag );
	}
    }

    return;
}


const char *
parse_social_string( const char *s )
{
    static char		buf[MAX_INPUT_LENGTH];
    const char *	p;
    char *		q;

    p = s;
    q = buf;

    while ( *p != '\0' )
    {
        if ( *p == '$' && *(p+1) == 'n' )
        {
            q = stpcpy( q, "`P$n`C" );
            p += 2;
        }
        else
        {
            *q++ = *p++;
        }
    }

    *q = '\0';

    return buf;
}


void
herald_announce( const char *str )
{
    DESCRIPTOR_DATA *	d;
    char		arg[MAX_STRING_LENGTH];
    char		buf[MAX_STRING_LENGTH];
    char *		p;

    if ( IS_NULLSTR( str ) )
        return;

    strcpy( arg, str );
    p = arg;
    while ( is_colcode( p ) )
        p += 2;
    *p = UPPER( *p );
    p = arg + strlen( arg ) - 1;
    if ( !ispunct( *p ) )
        strcpy( p + 1, "." );

    sprintf( buf, "`BA `Cherald `Bannounces: `W%s\n\r", arg );

    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING )
            send_to_char( buf, d->character );

    return;
}


void
info( const char *string, CHAR_DATA *ich, void *vo,
	bitvector flag, bitvector flag_skip, int min_level )
{
    char		buf[MAX_STRING_LENGTH];
    CHAR_DATA *		ch;
    DESCRIPTOR_DATA *	d;

    sprintf( buf, "`B[`wINFO`B]`w: `C%s", string );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	ch = d->character;
	if ( d->connected == CON_PLAYING
	&&   ch != NULL
	&&   ( flag == 0 || IS_SET( ch->info, INFO_ON ) )
	&&   ( flag == 0 || IS_SET( ch->info, flag ) )
	&&   ( flag_skip != 0 || !IS_SET( ch->info, flag_skip ) )
	&&   get_trust( ch ) >= min_level )
	{
	    act_color( AT_LBLUE, buf, ch, vo, ich, TO_CHAR, POS_DEAD );
	}
    }
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach == NULL || bch == NULL)
	return FALSE;

    if ( ach->leader != NULL ) ach = ach->leader;
    if ( bch->leader != NULL ) bch = bch->leader;
    return ach == bch;
}


void
sayto( CHAR_DATA *ch, CHAR_DATA *vch, const char *fmt, ... )
{
    char	buf[MAX_INPUT_LENGTH * 2];
    char	name[MAX_INPUT_LENGTH];
    char *	p;
    va_list	args;

    if ( ch == NULL || vch == NULL )
	return;

    one_argument( vch->name, name );
    p = stpcpy( buf, name );
    p = stpcpy( p, " " );

    va_start( args, fmt );
    vsnprintf( p, sizeof( buf ) - strlen( buf ), fmt, args );
    va_end( args );

    do_sayto( ch, buf );
}


void 
do_noexp (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
    {
        send_to_char("No. You are an NPC. Quit talking and die!\n\r",ch);
	  return;
    }
    if (!IS_SET (ch->act, PLR_NOEXP))
    {
        SET_BIT(ch->act, PLR_NOEXP);
        send_to_char("You will no longer get exp for kills. Remember to set this back when you want exp for kills!\n\r",ch);
        return;
    }
    else
    {
        REMOVE_BIT(ch->act, PLR_NOEXP);
        send_to_char("You turn your ability to gain exp back on.\n\r",ch);
        return;
    }
 }

 void do_quote( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    int number;

    number = number_range( 0, MAX_QUOTES);

    sprintf ( buf, "\n\r%s\n\r - %s\n\r",
       quote_table[number].text,
       quote_table[number].by);
    send_to_char ( buf, ch );
    return;
}