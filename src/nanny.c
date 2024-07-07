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


#include <arpa/telnet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"

/*
 * Socket and TCP/IP stuff
 */
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };

/*
 * Local functions
 */
static	bool	check_lname	args( ( char *name ) );
static	bool	check_playing	args( ( DESCRIPTOR_DATA *d, char *name ) );
static	bool	check_quick_connect
                                args( ( DESCRIPTOR_DATA *d, char *argument, bool fAnsi ) );
static	bool	check_reconnect	args( ( DESCRIPTOR_DATA *d, char *name, bool fConn ) );
static	bool	check_reset	args( ( DESCRIPTOR_DATA *d, const char *argument ) );
static	int	count_kingdoms	args( ( CHAR_DATA *ch ) );
static	void	death_continue	args( ( CHAR_DATA *ch ) );
static	void	send_greeting	args( ( DESCRIPTOR_DATA *d ) );
static	void	show_eye_choices args(( DESCRIPTOR_DATA *d ) );
static	void	show_hair_choices args((DESCRIPTOR_DATA *d ) );
static	void	show_eye_adj_choices args(( DESCRIPTOR_DATA *d ) );
static	void	show_hair_adj_choices args((DESCRIPTOR_DATA *d ) );
static	int	show_kingdoms	args( ( DESCRIPTOR_DATA *d ) );
static	void	show_races	args( ( DESCRIPTOR_DATA *d ) );
static	void	update_recent	args( ( int wizlvl, const char *str ) );

/*
 * Deal with sockets that haven't logged in yet.
 */
void
nanny( DESCRIPTOR_DATA *d, char *argument )
{
    DESCRIPTOR_DATA *d_old, *d_next;
    ROOM_INDEX_DATA *pRoom;
    EVENT_DATA *event;
    char	buf[MAX_STRING_LENGTH];
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	ch;
    char *	pwdnew;
    char *	p;
    int		iClass,race,i,weapon;
    int		count;
    bool	fOld;
    bool	found;

    while ( isspace(*argument) )
	argument++;

    ch = d->character;

    strip_event_desc( d, EVENT_DESC_LOGIN );
    event = new_event( );
    event->type = EVENT_DESC_LOGIN;
    event->fun = event_desc_login;
    add_event_desc( event, d, 15 * 60 * PULSE_PER_SECOND );

    if ( !str_cmp( argument, "quit" ) )
    {
	close_socket( d );
	return;
    }

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;

    case CON_GET_ANSI:
	if ( check_mssp( d, argument ) )
	    return;
	if ( check_quick_connect( d, argument, TRUE ) )
	    return;
	if ( !strcmp( argument, "WEBWHO" ) ) /* Note: case_sensitive compare */
	{
	    web_who( d->descriptor );
	    close_socket( d );
	    return;
	}
	if ( *argument == '\0' || !str_prefix( argument, "yes" ) )
	{
	    d->ansi = TRUE;
	}
	else
	{
	    d->ansi = FALSE;
	}
	d->connected = CON_GET_NAME;
	send_greeting( d );
	return;

    case CON_GET_NAME:
	if ( check_mssp( d, argument ) )
	    return;
	if ( check_quick_connect( d, argument, d->ansi ) )
	    return;
	if ( argument[0] == '\0' )
	{
	    close_socket( d );
	    return;
	}

	if ( !str_cmp( argument, "mac" ) )
	    strcpy( argument, "MacGregor" );

	argument[0] = UPPER( argument[0] );
	if ( !check_parse_name( argument ) )
	{
	    write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
	    return;
	}

	fOld = load_char_obj( d, argument );
	ch   = d->character;

	if (IS_SET(ch->act, PLR_DENY))
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string( log_buf );
	    write_to_buffer( d, "You are denied access.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	if (check_ban(d->host,BAN_PERMIT) && !IS_SET(ch->act,PLR_PERMIT))
	{
	    write_to_buffer(d,"Your site has been banned from this mud.\n\r",0);
	    close_socket(d);
	    return;
	}

	if ( check_reconnect( d, argument, FALSE ) )
	{
	    fOld = TRUE;
	}
	else
	{
	    if ( wizlock && !IS_IMMORTAL(ch)) 
	    {
		write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
		close_socket( d );
		return;
	    }
	}

	if ( fOld )
	{
	    /* Old player */
	    write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	}
	else
	{
	    /* New player */
 	    if (newlock)
	    {
                write_to_buffer( d, "The game is newlocked.\n\r", 0 );
                close_socket( d );
                return;
            }

	    if (check_ban(d->host,BAN_NEWBIES))
	    {
		write_to_buffer(d,
		    "New players are not allowed from your site.\n\r",0);
		close_socket(d);
		return;
	    }

	    if ( !check_legal_name( argument ) )
	    {
		write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
		return;
	    }

	    write_to_buffer( d, d->ansi ? "\x1B[H\x1B[2J" : "\n\r\n\r\n\r" , 0 );
	    write_to_buffer( d, "\n\r"
"`C---------------------------------------------------------------------------------\n\r\n\r"
"`WWelcome to MARS, the Multi-Adventurer Roleplaying System!                            \n\r"
"Here at MARS you must use acceptable RP-friendly character names, but not anything from\n\r"
"works of fantasy fiction. So no Bilbo Baggins or Drizzt Do'Urden.                      \n\r"
"                                                                                   \n\r\n\r"
"What is your name?                                                                     \n\r"
"`C---------------------------------------------------------------------------------\n\r\n\r"
            , 0 );

            sprintf( buf, "`WAre you sure that you wish to keep the name %s? `X", argument );
            write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
	write_to_buffer( d, "\n\r", 2 );

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );

	if (check_playing(d,ch->name))
	    return;

	if ( check_reconnect( d, ch->name, TRUE ) )
	    return;

	sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
	log_string( log_buf );
	wiznet(log_buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));
	update_recent( ch->invis_level, log_buf );

	if ( IS_IMMORTAL(ch) )
	{
	    do_function(ch, &do_help, "imotd" );
	    d->connected = CON_READ_IMOTD;
 	}
	else
	{
	    do_function(ch, &do_help, "motd" );
	    d->connected = CON_READ_MOTD;
	}
	break;

/* RT code for breaking link */

    case CON_BREAK_CONNECT:
	switch( *argument )
	{
	case 'y' : case 'Y':
            for ( d_old = descriptor_list; d_old != NULL; d_old = d_next )
	    {
		d_next = d_old->next;
		if (d_old == d || d_old->character == NULL)
		    continue;

		if (str_cmp(ch->name,d_old->original ?
		    d_old->original->name : d_old->character->name))
		    continue;

		close_socket(d_old);
	    }
	    if (check_reconnect(d,ch->name,TRUE))
	    	return;
	    write_to_buffer(d,"Reconnect attempt failed.\n\rName: ",0);
            if ( d->character != NULL )
            {
                free_char( d->character );
                d->character = NULL;
            }
	    d->connected = CON_GET_NAME;
	    break;

	case 'n' : case 'N':
	    write_to_buffer(d,"Name: ",0);
            if ( d->character != NULL )
            {
                free_char( d->character );
                d->character = NULL;
            }
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer(d,"Please type Y or N? ",0);
	    break;
	}
	break;

    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    if ( d->ansi )
	        write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
            else
                write_to_buffer( d, "\n\r\n\r\n\r", 0 );

            write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r\n\r"
"`WIn the depths of your soul is the word.  In that word, is safety.  With that \n\r"
"word, you can unlock great secrets, amazing mysteries, or terrible tragedies.\n\r"
"If you forget this word, then MARS will be lost to you.\n\r"
"Find your word, and remember it well.  Write it down if you must.\n\r"
"Lose it not.\n\r\n\r`C"
"---------------------------------------------------------------------------\n\r\n\r"
            , 0 );

	    sprintf( buf, "`WWhat shall your password be?`X%s ", echo_off_str );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    write_to_buffer( d,
"\n\r`GDidn't like that name?\n\r"
"Okay, what name do you want instead? `X"
	    , 0 );

	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
	write_to_buffer( d, "\n\r", 2 );

	if ( strlen(argument) < 5 )
	{
	    write_to_buffer( d,
		"Password must be at least five characters long.\n\rPassword: ",
		0 );
	    return;
	}

	pwdnew = crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rPassword: ",
		    0 );
		return;
	    }
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );
	write_to_buffer( d, "\n\rPlease confirm password by typing it again: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
	write_to_buffer( d, "\n\r", 2 );

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d,
"Passwords do not match. Please enter your password again: ",
		0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );


        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );

        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"You may now select a surname. Surnames follow the same rules as your first name.\n\r"
"---------------------------------------------------------------------------\n\r"
        , 0 );
        write_to_buffer( d, "`WYour surname, or `CENTER`W for none? `X", 0 );
	d->connected = CON_GET_NEW_LNAME;
	break;

    case CON_GET_NEW_LNAME:
	if ( check_reset( d, argument ) )
	    return;

	if ( !str_cmp( argument, "N" )
	||   !str_cmp( argument, "no" )
	||   !str_cmp( argument, "none" ) )
	    *argument = '\0';

	if ( *argument != '\0' )
	{
	    char lname[MAX_INPUT_LENGTH];

	    if ( !check_lname( argument ) )
	    {
		write_to_buffer( d,
		"Illegal surname, try again.\n\r\n\r"
		"Your surname? ", 0 );
		break;
	    }
	    strcpy( lname, argument );
	    lname[0] = UPPER( lname[0] );
	    ch->pcdata->lname = str_dup( lname );
	}

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );


          write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"You may now select your ethos:\n\r"
"\n\r`C"
"---------------------------------------------------------------------------\n\r"
    , 0 );
    write_to_buffer( d, "\n\r`WWill you be Lawful, Neutral or Chaotic? `X", 0 );
    d->connected = CON_GET_ETHOS;
    break;

    case CON_GET_ETHOS:
    if ( check_reset( d, argument ) )
        return;
        switch( UPPER( *argument ) )
        {
            case 'L':   ch->ethos =     1000; break;
            case 'N':   ch->ethos =         0; break;
            case 'C':   ch->ethos =    -1000; break;
            default:
                write_to_buffer( d,
"\n\r`GThat was not one of the choices.\n\r\n\r"
"Will you be Lawful, Neutral or Chaotic? `X"
        , 0 );
        return;
    }

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );

        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"You may now select your alignment:\n\r"
"\n\r`C"
"---------------------------------------------------------------------------\n\r"
	, 0 );
	write_to_buffer( d, "\n\r`WWill you be Good, Neutral or Evil? `X", 0 );
	d->connected = CON_GET_ALIGNMENT;
	break;

    case CON_GET_ALIGNMENT:
	if ( check_reset( d, argument ) )
	    return;
        switch( UPPER( *argument ) )
        {
            case 'G':	ch->alignment =  750; break;
            case 'N':	ch->alignment =    0; break;
            case 'E':	ch->alignment = -750; break;
            default:
                write_to_buffer( d,
"\n\r`GThat was not one of the choices.\n\r\n\r"
"Will you be Good, Neutral or Evil? `X"
		, 0 );
		return;
	}

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );

        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"You may now select your race.\n\r"
"The following races are available:\n\r"
"\n\r\n\r`Y"
        , 0 );

	show_races( d );

        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
        , 0 );

	write_to_buffer( d, "\n\r`WWhat is your race (`Yhelp`W for more information)? `X", 0 );

	d->connected = CON_GET_NEW_RACE;
	break;

    case CON_GET_NEW_RACE:
	if ( check_reset( d, argument ) )
	    return;
	one_argument( argument, arg );

	if ( !str_cmp( arg, "help" ) )
	{
	    argument = one_argument( argument, arg );
	    if ( argument[0] == '\0' )
		do_function( ch, &do_help, "race help" );
	    else
		do_function( ch, &do_help, argument );
            write_to_buffer( d,
		"\n\r`WWhat is your race (`Yhelp`W for more information)? `X",0 );
	    break;
  	}

	race = race_lookup( argument );

	if ( race == 0
	||   ch->alignment > race_table[race].max_align
	||   ch->alignment < race_table[race].min_align
	||   !race_table[race].pc_race )
	{
	    write_to_buffer( d, "That is not a valid race.\n\r", 0 );
	    show_races( d );
            write_to_buffer( d,
		"\n\rWhat is your race (help for more information)? ",0 );
	    break;
	}

        ch->race = race;
	/* initialize stats */
	for (i = 0; i < MAX_STATS; i++)
	    ch->perm_stat[i] = race_table[race].stats[i];
	xSET_BITS( ch->affected_by, race_table[race].aff );
	ch->imm_flags	= ch->imm_flags|race_table[race].imm;
	ch->res_flags	= ch->res_flags|race_table[race].res;
	ch->vuln_flags	= ch->vuln_flags|race_table[race].vuln;
	ch->form	= race_table[race].form;
	ch->parts	= race_table[race].parts;

	/* add skills */
	for (i = 0; i < MAX_RACE_SKILLS; i++)
	{
	    if (race_table[race].skills[i] == NULL)
	 	break;
	    group_add(ch,race_table[race].skills[i],FALSE);
	}
	/* add cost */
	ch->pcdata->points = race_table[race].points;
	ch->size = race_table[race].size;

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );

        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"Now that you have chosen a Race, please select a Gender.\n\r"
"\n\r`C"
"---------------------------------------------------------------------------\n\r"
        , 0 );

        write_to_buffer( d, "\n\r`WWhich gender shall you choose (M/F/N)? ", 0 );
        d->connected = CON_GET_NEW_SEX;
        break;


    case CON_GET_NEW_SEX:
        if ( check_reset( d, argument ) )
            return;
	ch->lines = 0;
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;    
			    ch->pcdata->true_sex = SEX_MALE;
			    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE; 
			    ch->pcdata->true_sex = SEX_FEMALE;
			    break;
        case 'n': case 'N': ch->sex = SEX_NEUTRAL;
                            ch->pcdata->true_sex = SEX_NEUTRAL;
                            break;
	default:
	    write_to_buffer( d,
"`GIt's is an easy question, Mortal. CHOOSE!\n\r\n\r"
"Are you male, female or neither? `X", 0 );
	    return;
	}

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );

write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"It is now time to select your distinguishing features such as hair and eye color\n\r"
    , 0 );

    show_hair_choices( d );
    write_to_buffer( d,
"`C---------------------------------------------------------------------------\n\r"
    , 0 );

    write_to_buffer( d, "\n\r`WWhich hair color shall be your own?`X ", 0 );
    d->connected = CON_GET_HAIR_COLOR;
    break;

    case CON_GET_HAIR_COLOR:
        if ( check_reset( d, argument ) )
            return;
        if ( *argument == '\0' )
        {
            write_to_buffer( d, "\n\r", 2 );
            show_hair_choices( d );
            write_to_buffer( d, "\n\r`WWhich hair color shall be your own?`X ", 0 );
            return;
        }

        found = FALSE;
        for ( i = 0; !IS_NULLSTR( hair_color_table[i].desc ); i++ )
        {
            if ( is_exact_name( race_table[ch->race].who_name, hair_color_table[i].list ) )
            {
                if ( !str_cmp( argument, hair_color_table[i].desc ) )
                {
                    found = TRUE;
                    break;
                }
                if ( !str_prefix( argument, hair_color_table[i].desc ) )
                {
                    found = TRUE;
                    break;
                }
            }
        }

        if ( !found )
        {
            write_to_buffer( d,
"`GI do not see that here.  Please choose again.\n\r\n\r`Y"
        , 0 );
        show_hair_choices( d );
            write_to_buffer( d, "\n\r`WWhich hair color shall be your own?`X ", 0 );
        return;
        }

        free_string( ch->pcdata->hair_color );
        ch->pcdata->hair_color = str_dup( hair_color_table[i].desc );

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );


        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"It is now time to select the type of hair you have.                        \n\r"
    , 0 );

    show_hair_adj_choices( d );
    write_to_buffer( d,
"`C---------------------------------------------------------------------------\n\r"
    , 0 );

    write_to_buffer( d, "\n\r`WWhich hair style shall be your own?`X ", 0 );
    d->connected = CON_GET_HAIR_ADJ;
    break;

    case CON_GET_HAIR_ADJ:
        if ( check_reset( d, argument ) )
            return;
        if ( *argument == '\0' )
        {
            write_to_buffer( d, "\n\r", 2 );
            show_hair_adj_choices( d );
            write_to_buffer( d, "\n\r`WWhich hair style shall be your own?`X ", 0 );
            return;
        }

        found = FALSE;
        for ( i = 0; !IS_NULLSTR( hair_adj_table[i].desc ); i++ )
        {
            if ( is_exact_name( race_table[ch->race].who_name, hair_adj_table[i].list ) )
            {
                if ( !str_cmp( argument, hair_adj_table[i].desc ) )
                {
                    found = TRUE;
                    break;
                }
                if ( !str_prefix( argument, hair_adj_table[i].desc ) )
                {
                    found = TRUE;
                    break;
                }
            }
        }

        if ( !found )
        {
            write_to_buffer( d,
"`GI do not see that here.  Please choose again.\n\r\n\r`Y"
        , 0 );
        show_hair_adj_choices( d );
            write_to_buffer( d, "\n\r`WWhich hair style shall be your own?`X ", 0 );
        return;
        }

        free_string( ch->pcdata->hair_adj );
        ch->pcdata->hair_adj = str_dup( hair_adj_table[i].desc );

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );

write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"It is now time to select the type of eyes you have.                        \n\r"
    , 0 );

    show_eye_adj_choices( d );
    write_to_buffer( d,
"`C---------------------------------------------------------------------------\n\r"
    , 0 );

    write_to_buffer( d, "\n\r`WWhich eye type shall be your own?`X ", 0 );
    d->connected = CON_GET_EYE_ADJ;
    break;

    case CON_GET_EYE_ADJ:
        if ( check_reset( d, argument ) )
            return;
        if ( *argument == '\0' )
        {
            write_to_buffer( d, "\n\r", 2 );
            show_eye_adj_choices( d );
            write_to_buffer( d, "\n\r`WWhich eye type shall be your own?`X ", 0 );
            return;
        }

        found = FALSE;
        for ( i = 0; !IS_NULLSTR( eye_adj_table[i].desc ); i++ )
        {
            if ( is_exact_name( race_table[ch->race].who_name, eye_adj_table[i].list ) )
            {
                if ( !str_cmp( argument, eye_adj_table[i].desc ) )
                {
                    found = TRUE;
                    break;
                }
                if ( !str_prefix( argument, eye_adj_table[i].desc ) )
                {
                    found = TRUE;
                    break;
                }
            }
        }

        if ( !found )
        {
            write_to_buffer( d,
"`GI do not see that here.  Please choose again.\n\r\n\r`Y"
        , 0 );
        show_eye_adj_choices( d );
            write_to_buffer( d, "\n\r`WWhich eye type shall be your own?`X ", 0 );
        return;
        }

        free_string( ch->pcdata->eye_adj );
        ch->pcdata->eye_adj = str_dup( eye_adj_table[i].desc );

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );

        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"Now you can select your eye color\n\r\n\r`Y"
    , 0 );

    show_eye_choices( d );

    write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
    , 0 );

    write_to_buffer( d, "`WWhat shall the color of your eyes be?`X ", 0 );
    d->connected = CON_GET_EYE_COLOR;
    break;

    case CON_GET_EYE_COLOR:
        if ( check_reset( d, argument ) )
            return;

        if ( *argument == '\0' )
        {
            write_to_buffer( d, "\n\r", 2 );
            show_eye_choices( d );
            write_to_buffer( d, "`WWhat shall the color of your eyes be?`X ", 0 );
            return;
        }

        found = FALSE;
        for ( i = 0; !IS_NULLSTR( eye_color_table[i].desc ); i++ )
        {
            if ( is_exact_name( race_table[ch->race].who_name, eye_color_table[i].list ) )
            {
                if ( !str_cmp( argument, eye_color_table[i].desc ) )
                {
                    found = TRUE;
                    break;
                }
                if ( !str_prefix( argument, eye_color_table[i].desc ) )
                {
                    found = TRUE;
                    break;
                }
            }
        }

        if ( !found )
        {
            write_to_buffer( d,
"`GI do not see that color here.  Please choose again.\n\r\n\r`Y"
        , 0 );
        show_eye_choices( d );
            write_to_buffer( d, "WWhat shall the color of your eyes be?`X ", 0 );
        return;
        }

        free_string( ch->pcdata->eye_color );
        ch->pcdata->eye_color = str_dup( eye_color_table[i].desc );

    free_string( ch->short_descr );
    ch->short_descr = str_dup( aoran( race_table[ch->race].name ) );
    sprintf( buf, "%s with %s,  %-2s and %s,  %-2s eyes is here.\n\r",
             aoran( race_table[ch->race].name ),
             ch->pcdata->hair_adj,
             ch->pcdata->hair_color,
             ch->pcdata->eye_adj,
             ch->pcdata->eye_color );
        buf[0] = UPPER( buf[0] );
    free_string( ch->long_descr );
    ch->long_descr = str_dup( buf );

        if ( d->ansi )
            write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
        else
            write_to_buffer( d, "\n\r\n\r\n\r", 0 );

	/*iClass = class_lookup( "commoner" );

        ch->class = iClass;

	sprintf( log_buf, "%s@%s new player.", ch->name, d->host );
	log_string( log_buf );
	wiznet( "Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0 );
        wiznet( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust( ch ) );
	update_recent( 0, log_buf );
*/
        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"You may now select your class.  The following classes are available \n\r"
"Cleric - holy warriors, able to protect and heal their companions \n\r"
"Druid - Can summon animals and draw on powerful magic from nature \n\r"
"Fighter - Basic fighters and warriors \n\r"
"Mage - Powerful spell casters \n\r"
"Rogue - Able to sneak in the shadows and steal your gold then slip away unseen \n\r"
"You may type HELP class for a more detailed description. \n\r"
"---------------------------------------------------------------------------\n\r"
        , 0 );

strcpy (buf, "Select a class [");
            for (iClass = 0; iClass < MAX_CLASS; iClass++)
            {
             	if (iClass > 0)
                    strcat (buf, " ");
                strcat (buf, class_table[iClass].name);
            }
            strcat (buf, "]: ");
            write_to_buffer (d, buf, 0);
            d->connected = CON_GET_NEW_CLASS;
            break;

case CON_GET_NEW_CLASS:
            iClass = class_lookup (argument);

            if (iClass == -1)
            {
                write_to_buffer (d, "That's not a class.\n\rWhat IS your class? ",
                              0);
                return;
            }

            ch->class = iClass;

            sprintf (log_buf, "%s@%s new player.", ch->name, d->host);
            log_string (log_buf);
            wiznet ("Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0);
            wiznet (log_buf, NULL, NULL, WIZ_SITES, 0, get_trust (ch));
            update_recent( 0, log_buf );
           /* write_to_buffer (d, "\n\r", 2);
            send_to_desc ("You may be good, neutral, or evil.\n\r", d);
            send_to_desc ("Which alignment (G/N/E)? ", d);
            d->connected = CON_GET_ALIGNMENT;
            break;
*/
	if ( d->ansi )
	    write_to_buffer( d, "\x1B[H\x1B[2J", 0 );
	else
	    write_to_buffer( d, "\n\r\n\r\n\r", 0 );

	write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r\n\r"
"Please select your hometown:\n\r"
"\n\r"
	, 0 );

	count = show_kingdoms( d );

	write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r\n\r"
	, 0 );

	sprintf( buf, "`WWhich city will you choose (`Y1`W-`Y%d`W or `Yhelp`W)? `X", count );

	write_to_buffer( d, buf, 0 );
	d->connected = CON_GET_CONTINENT;
	break;


    case CON_GET_CONTINENT:
	if ( check_reset( d, argument ) )
	    return;

	one_argument( argument, arg );
	if ( !str_cmp( arg, "help" ) )
	{
	    argument = one_argument( argument, arg );
	    if ( *argument  == '\0' )
	        do_function( ch, &do_help, "kingdoms" );
            else
                do_function( ch, do_help, argument );
            sprintf( buf, "`WWhich city will you choose (`Y1`W-`Y%d`W or `Yhelp`W)? `X", count_kingdoms( ch ) );
            write_to_buffer( d, buf, 0 );
            break;
	}

	if ( !is_number( arg ) )
	{
	    count = show_kingdoms( d );
	    sprintf( buf, "`WWhich city will you choose (`Y1`W-`Y%d`W or `Yhelp`W)? `X", count );
	    write_to_buffer( d, buf, 0 );
	    return;
	}

	count = atoi( arg );
	if ( count < 1 || count > count_kingdoms( ch ) )
	{
	    count = show_kingdoms( d );
	    sprintf( buf, "`WWhich city will you choose (`Y1`W-`Y%d`W or `Yhelp`W)? `X", count );
	    write_to_buffer( d, buf, 0 );
	    return;
	}

	count = 0;
	for ( i = 1; i < MAX_KINGDOM; i++ )
	{
	    if ( race_table[ch->race].kingdom_list[i] && kingdom_table[i].recall != 0 )
	    {
	        count++;
	        if ( count == atoi( arg ) )
	        {
	            ch->pcdata->kingdom = count;
	            break;
                }
            }
	}
            group_add (ch, "rom basics", FALSE);
            group_add (ch, class_table[ch->class].base_group, FALSE);
        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
"Now you may customize your character and select skills, spells, and groups\n\r"
"to fine tune your character class\n\r"
        , 0 );
        write_to_buffer( d, "\n\r`C"
"---------------------------------------------------------------------------\n\r"
        , 0 );

            write_to_buffer (d,"Do you wish to customize this character?\n\r", 0);
            write_to_buffer
                (d, "Customization takes time, but allows a wider range of skills and abilities.\n\r",
                 0);
            write_to_buffer (d,"Customize (Y/N)? ", 0);
            d->connected = CON_DEFAULT_CHOICE;
            break;

        case CON_DEFAULT_CHOICE:
            write_to_buffer (d, "\n\r", 2);
            switch (argument[0])
            {
                case 'y':
                case 'Y':
                    ch->gen_data = new_gen_data ();
                    ch->gen_data->points_chosen = ch->pcdata->points;
                    do_function (ch, &do_help, "group header");
                    list_group_costs (ch);
                    write_to_buffer (d,
                                     "You already have the following skills:\n\r",
                                     0);
                    do_function (ch, &do_skills, "");
                    do_function (ch, &do_help, "menu choice");
                    d->connected = CON_GEN_GROUPS;
                    break;
                case 'n':
                case 'N':
                    group_add (ch, class_table[ch->class].default_group,
                               TRUE);
                    write_to_buffer (d, "\n\r", 2);
                    write_to_buffer (d,
                                     "Please pick a weapon from the following choices:\n\r",
                                     0);
                    buf[0] = '\0';
                    for (i = 0; weapon_table[i].name != NULL; i++)
                        if (ch->pcdata->skill[*weapon_table[i].gsn].percent > 0)
                        {
                            strcat (buf, weapon_table[i].name);
                            strcat (buf, " ");
                        }
                    strcat (buf, "\n\rYour choice? ");
                    write_to_buffer (d, buf, 0);
                    d->connected = CON_PICK_WEAPON;
                    break;
                default:
                    write_to_buffer (d, "Please answer (Y/N)? ", 0);
                    return;
            }
            break;

case CON_PICK_WEAPON:
            write_to_buffer (d, "\n\r", 2);
            weapon = weapon_lookup (argument);
            if (weapon == -1
                || ch->pcdata->skill[*weapon_table[weapon].gsn].percent <= 0)
            {
             	write_to_buffer (d,
                                 "That's not a valid selection. Choices are:\n\r",
                                 0);
                buf[0] = '\0';
                for (i = 0; weapon_table[i].name != NULL; i++)
                    if (ch->pcdata->skill[*weapon_table[i].gsn].percent > 0)
                    {
                        strcat (buf, weapon_table[i].name);
                        strcat (buf, " ");
                    }
                strcat (buf, "\n\rYour choice? ");
                write_to_buffer (d, buf, 0);
                return;
            }

             ch->pcdata->skill[*weapon_table[weapon].gsn].percent = 40;
            write_to_buffer (d, "\n\r", 2);
            do_function (ch, &do_help, "motd");
            d->connected = CON_READ_MOTD;
            break;

        case CON_GEN_GROUPS:
            send_to_char ("\n\r", ch);

            if (!str_cmp (argument, "done"))
            {
                if (ch->pcdata->points == race_table[ch->race].points)
                {
                    send_to_char ("You didn't pick anything.\n\r", ch);
                    break;
                }

                if (ch->pcdata->points < 30 + race_table[ch->race].points)
                {
                    sprintf (buf,
                             "You must take at least %d points of skills and groups",
                             30 + race_table[ch->race].points);
                    send_to_char (buf, ch);
                    break;
                }

                sprintf (buf, "Creation points: %d\n\r", ch->pcdata->points);
                send_to_char (buf, ch);
                sprintf (buf, "Experience per level: %d\n\r",
                         exp_per_level (ch, ch->gen_data->points_chosen));
                if (ch->pcdata->points < 30)
                    ch->train = (30 - ch->pcdata->points + 1) / 2;
                free_gen_data (ch->gen_data);
                ch->gen_data = NULL;
                send_to_char (buf, ch);
                write_to_buffer (d, "\n\r", 2);
                write_to_buffer (d,
                                 "Please pick a weapon from the following choices:\n\r",
                                 0);
                buf[0] = '\0';
                for (i = 0; weapon_table[i].name != NULL; i++)
                    if (ch->pcdata->skill[*weapon_table[i].gsn].percent > 0)
                    {
                        strcat (buf, weapon_table[i].name);
                        strcat (buf, " ");
                    }
                strcat (buf, "\n\rYour choice? ");
                write_to_buffer (d, buf, 0);
                d->connected = CON_PICK_WEAPON;
   break;
            }

            if (!parse_gen_groups (ch, argument))
                send_to_char
                    ("Choices are: list,learned,premise,add,drop,info,help, and done.\n\r",
                     ch);

            do_function (ch, &do_help, "menu choice");
		break;

    case CON_READ_IMOTD:
	write_to_buffer(d,"\n\r",2);
        do_function(ch, &do_help, "motd");
        d->connected = CON_READ_MOTD;
	break;

    case CON_READ_MOTD:
        if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0')
        {
            write_to_buffer( d, "Warning! Null password!\n\r",0 );
            write_to_buffer( d, "Please report old password with bug.\n\r",0);
            write_to_buffer( d,
                "Type 'password null <new password>' to fix.\n\r",0);
        }

	write_to_buffer( d, 
    "\n\rWelcome to " MUD_NAME ".\n\r",
	    0 );
	ch->next	= char_list;
	char_list	= ch;
	d->connected	= CON_PLAYING;
	strip_event_desc( d, EVENT_DESC_LOGIN );
	count_players( );
	reset_char(ch);

	event = new_event( );
	event->type = EVENT_CHAR_NOPK;
	event->fun = event_char_nopk;
	add_event_char( event, ch, number_range(  1 * 60 * PULSE_PER_SECOND,
	                                         10 * 60 * PULSE_PER_SECOND ) );

	if ( ch->level == 0 )
	{

	    /*
	     * Buff newbies a bit.
	     */
	    ch->max_hit  = 50;

	    ch->perm_stat[class_table[ch->class].attr_prime] += 3;

	    ch->level	= 1;
	    ch->exp	= exp_per_level(ch,ch->pcdata->points);
	    ch->hit	= ch->max_hit;
	    ch->mana	= ch->max_mana;
	    ch->move	= ch->max_move;
	    ch->lines	= PAGELEN;
	    ch->train	 = 3;
	    ch->practice = 5;
	    sprintf( buf, "the Newbie");
	    set_title( ch, buf );

	    do_function ( ch, &do_pack, "self" );

	    char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    send_to_char( "\n\r", ch );
	    do_function( ch, &do_help, "newbie info" );
	    send_to_char( "\n\r", ch );
	    sql_add_user( ch );
	    SET_BIT( ch->comm, COMM_DETAIL );
	}
	else if ( ( pRoom = get_vroom_index( ch->room_vnum ) ) != NULL )
	{
	    char_to_room( ch, pRoom );
	}
	else if ( IS_IMMORTAL( ch ) )
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	}

	ch->pcdata->logcount++;
	update_userlist( ch, TRUE );
	act_color( AT_PINK, "$n has entered the game.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
	do_function(ch, &do_look, "auto" );

	if ( ch->pcdata->deathstate == DEATH_SEARCH )
	    create_rift_mob( ch );

	wiznet( "$N has entered the game.", ch, NULL,
		WIZ_LOGINS, 0, get_trust( ch ) );

	if ( ch->pet != NULL )
	{
	    char_to_room( ch->pet, ch->in_room );
	    act_color( AT_PINK, "$n has entered the game.", ch->pet, NULL, NULL, TO_ROOM, POS_RESTING );
	}

	if ( ch->mount != NULL )
	{
	    char_to_room( ch->mount, ch->in_room );
	    act_color( AT_PINK, "$n has entered the game.", ch->mount, NULL, NULL, TO_ROOM, POS_RESTING );
	    add_follower( ch->mount, ch );
	    do_mount( ch, ch->mount->name );
	}

	mprog_greet_trigger( ch );
    ch->pcdata->timesplayed ++;
	show_board_list( ch, FALSE );
//	do_function(ch, &do_unread, "");
	break;
    }

    return;
}


void
check_death_cmds( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *		ch;
    OBJ_DATA *		corpse;

    ch = d->character;

    switch ( ch->pcdata->deathstate )
    {
        default:
            substitute_alias( d, d->incomm );
            return;

        case DEATH_ASK:
            if ( str_match( argument, "cont", "continue" ) )
            {
                death_continue( ch );
                return;
            }
            else if ( str_match( argument, "perm", "permanent" ) )
            {
		send_to_char(
"\n\r`WDeath nods and makes a note in his records.  Turning to you, he speaks.\n\r\n\r"
"`W'`GTHAT CHOICE, WHEN MADE CANNOT BE UNDONE.  IF YOU CONTINUE, YOUR SOUL WILL BE\n\r"
"TAKEN FROM THIS PLACE FOREVER, NEVER TO RETURN UPON THE WORLD, AND NEVER TO BURDEN \n\r"
"YOUR BODY WITH IT'S PRESENCE.  IF THIS IS WHAT YOU REALLY WANT, THEN TYPE `RPERMANENT.`G \n\r"
"OR TYPE `BCONTINUE`G TO ATTEMPT TO RETURN TO THE LIVING.\n\r"
"Your choice?`X "
		, ch );
		ch->pcdata->deathstate = DEATH_CONFIRM;
		return;
            }
            else
            {
		send_to_char( "\n\r"
"`WDeath sighs deeply, his cold breath ruffling the edges of his hood.\n\r"
"'`GYOU MUST RESPOND WITH `YPERMANENT`G OR `YCONTINUE.`W'\n\r"
		, ch );
		return;
            }
            substitute_alias( d, d->incomm );
            return;

        case DEATH_CONFIRM:
            if ( str_match( argument, "cont", "continue" ) )
            {
                death_continue( ch );
                return;
            }
            else if ( str_match( argument, "perm", "permanent" ) )
            {
		send_to_char( "\n\r"
"`WDeath waves a hand dismissively, '`GSO SHALL IT BE DONE.`W'\n\r\n\r"
"...those are the last words you hear before the final blackness \n\r"
"envelopes you.`X\n\r\n\r"
		, ch );
		if ( ( corpse = find_corpse( ch ) ) != NULL
		&&     corpse->in_room != NULL )
		{
		    stuff_corpse( ch, corpse );
		    corpse->value[5] = 0;
		    if ( corpse->in_room->people != NULL )
		        act_color( AT_MAGIC, "$p glows briefly.",
		                   corpse->in_room->people, corpse,
		                   NULL, TO_ALL, POS_RESTING );
                }
		SET_BIT( ch->pcdata->confirm_flags, CONF_DELETE );
		do_delete( ch, "" );
		return;
            }
            else
            {
		send_to_char( "\n\r"
"`WDeath sighs, his cold breath ruffling the edges of his hood.\n\r"
"'`GYOU MUST CHOOSE, `YPERMANENT`G OR `YCONTINUE.`W'\n\r"
		, ch );
		return;
            }
            return;
    }

}


/*
 * Check a name for suitability for a new user
 */
bool
check_legal_name( char *name )
{
    USERLIST_DATA *	pUser;
    char *		list;
    char		arg[MAX_INPUT_LENGTH];
    char		word[MAX_INPUT_LENGTH];

    /*
     * Prevent players from naming themselves after imms.
     */
    for ( pUser = user_first; pUser != NULL; pUser = pUser->next )
    {
	if ( pUser->level < LEVEL_IMMORTAL )
	    continue;
	if ( !str_infix( name, pUser->name ) || !str_infix( pUser->name, name ) )
	    return FALSE;
    }

    if ( !str_infix( name, "artifice" ) || !str_infix( "artifice", name ) )
	return FALSE;

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex != NULL;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_name( name, pMobIndex->player_name ) )
		    return FALSE;
	    }
	}
    }

    /*
     * Check badname list.
     */
    one_argument( name, arg ); /* do this just to convert to lc */
    list = badname_list;
    while ( !IS_NULLSTR( list ) )
    {
	list = one_argument( list, word );
	if ( strstr( arg, word ) != NULL )
	    return FALSE;
    }

    return TRUE;
}


/*
 * check a last name for suitability.
 * Must be 3-12 chars
 * Must start with letter
 * May contain quotes, but not more than two
 */
static bool
check_lname( char *name )
{
    int ccount;		/* count of letters */
    int qcount;		/* count of quotes */

    ccount = 0;
    qcount = 0;
    if ( strlen( name ) < 3 || strlen( name ) > 12 )
	return FALSE;
    if ( !isalpha( *name ) )
	return FALSE;
    name++;
    ccount++;

    while ( *name != '\0' )
    {
	if ( isalpha( *name ) )
	    ccount++;
	else if ( *name == '\'' )
	{
	    qcount++;
	    if ( qcount > ccount || qcount > 2 || *(name-1) == '\'' )
		return FALSE;
	}
	else
	    return FALSE;
	name++;
    }

    return TRUE;
}


/*
 * Parse a name for acceptability.
 */
bool
check_parse_name( char *name )
{
    CLAN_DATA * clan;

    /*
     * Reserved words.
     */
    if (is_exact_name(name,
	"all auto immortal self someone something the you loner"))
    {
	return FALSE;
    }

    /* check clans */
    for ( clan = clan_first; clan; clan = clan->next )
    {
	if ( LOWER( name[0] ) == LOWER( clan->name[0] )
	&&  !str_cmp( name, clan->name ) )
	   return FALSE;
    }

    if (str_cmp(capitalize(name),"Alander") && (!str_prefix("Alan",name)
    || !str_suffix("Alander",name)))
	return FALSE;

    /*
     * Length restrictions.
     */

    if ( strlen(name) <  2 )
	return FALSE;

    if ( strlen(name) > 12 )
	return FALSE;

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll,adjcaps = FALSE,cleancaps = FALSE;
 	int total_caps = 0;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;

	    if ( isupper(*pc)) /* ugly anti-caps hack */
	    {
		if (adjcaps)
		    cleancaps = TRUE;
		total_caps++;
		adjcaps = TRUE;
	    }
	    else
		adjcaps = FALSE;

	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;

	if (cleancaps || (total_caps > (strlen(name)) / 2 && strlen(name) < 3))
	    return FALSE;
    }

    return TRUE;
}


/*
 * Check if already playing.
 */
static bool
check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
	if ( dold != d
	&&   dold->character != NULL
	&&   dold->connected != CON_GET_NAME
	&&   dold->connected != CON_GET_OLD_PASSWORD
	&&   !str_cmp( name, dold->original
	         ? dold->original->name : dold->character->name ) )
	{
	    write_to_buffer( d, "That character is already playing.\n\r",0);
	    write_to_buffer( d, "Do you wish to connect anyway (Y/N)?",0);
	    d->connected = CON_BREAK_CONNECT;
	    return TRUE;
	}
    }

    return FALSE;
}


static bool
check_quick_connect( DESCRIPTOR_DATA *d, char *argument, bool fAnsi )
{
    char	arg[MAX_INPUT_LENGTH];
    char	name[MAX_INPUT_LENGTH];
    char	password[MAX_INPUT_LENGTH];
    CHAR_DATA *	ch;

    if ( d == NULL || IS_NULLSTR( argument ) )
        return FALSE;
    d->ansi = fAnsi;

    argument = one_argument( argument, arg );
    if ( !str_match( arg, "c", "connect" ) )
        return FALSE;

    argument = first_arg( argument, name, FALSE );
    argument = first_arg( argument, password, FALSE );
    if ( name[0] == '\0' )
        return FALSE;

    if ( password[0] == '\0' )
    {
        close_socket( d );
        return TRUE;
    }

    if ( !str_cmp( name, "mac" ) )
        strcpy( name, "MacGregor" );

    name[0] = UPPER( name[0] );

    /* Greeting is required by DIKU license. */
    if ( d->connected == CON_GET_ANSI )
    {
        send_greeting( d );
        write_to_buffer( d, name, 0 );
        write_to_buffer( d, "\n\r", 2 );
    }

    if ( !load_char_obj( d, name ) )
    {
        write_to_buffer( d, "There is nobody here by that name.\n\r", 0 );
        close_socket( d );
        return TRUE;
    }

    ch = d->character;

    if ( strcmp( crypt( password, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	write_to_buffer( d, "Wrong password.\n\r", 0 );
	close_socket( d );
	return TRUE;
    }

    if ( check_playing( d, ch->name ) )
        return TRUE;
    if ( check_reconnect( d, ch->name, TRUE ) )
        return TRUE;

    if ( wizlock && !IS_IMMORTAL( ch ) )
    {
        write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
        close_socket( d );
        return TRUE;
    }

    sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
    log_string( log_buf );
    wiznet( log_buf, NULL, NULL,WIZ_SITES, 0, get_trust( ch ) );
    update_recent( ch->invis_level, log_buf );

    if ( IS_IMMORTAL( ch ) )
    {
	do_function( ch, &do_help, "imotd" );
	d->connected = CON_READ_IMOTD;
    }
    else
    {
	do_function( ch, &do_help, "motd" );
	d->connected = CON_READ_MOTD;
    }
    return TRUE;
}


/*
 * Look for link-dead player to reconnect.
 */
static bool
check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&&   ( !fConn || ch->desc == NULL )
	&&   !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		if ( d->character->pet != NULL )
		    extract_char( d->character->pet, TRUE );
		if ( d->character->mount != NULL )
		    extract_char( d->character->mount, TRUE );

		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "Reconnecting.", ch );
		if ( *buf_string( ch->pcdata->buffer ) != '\0' )
		    send_to_char( "Type replay to see missed tells.", ch );
		send_to_char( "\n\r", ch );
		act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );

		sprintf( log_buf, "%s@%s reconnected.", ch->name, d->host );
		log_string( log_buf );
		wiznet("$N groks the fullness of $S link.",
		    ch,NULL,WIZ_LINKS,0,0);
		update_recent( ch->invis_level, log_buf );
		d->connected = CON_PLAYING;
		strip_event_desc( d, EVENT_DESC_LOGIN );
		count_players( );
	    }
	    return TRUE;
	}
    }

    return FALSE;
}


static bool
check_reset( DESCRIPTOR_DATA *d, const char *argument )
{
    if ( !str_cmp( argument, "reset" ) || !str_cmp( argument, "restart" ) )
    {
	d->character->level = 0;
	write_to_buffer( d, "\n\rYour surname, or ENTER for none? ", 0 );
	d->connected = CON_GET_NEW_LNAME;
	return TRUE;
    }
    else
	return FALSE;
}


static int
count_kingdoms( CHAR_DATA *ch )
{
    int		count;
    int		i;

    count = 0;
    for ( i = 1; i < MAX_KINGDOM; i++ )
        if ( race_table[ch->race].kingdom_list[i] && kingdom_table[i].recall != 0 )
            count++;

    return count;
}


bool
create_rift_mob( CHAR_DATA *ch )
{
    CHAR_DATA *		mob;
    MOB_INDEX_DATA *	mobIndex;
    int			lvnum;
    int			max_vnum;
    ROOM_INDEX_DATA *	rift;
    int			uvnum;

    if ( IS_NPC( ch ) )
    {
        bugf( "Create_rift_mob: creating mob for NPC" );
        return FALSE;
    }

    if ( ( rift = get_room_index( ROOM_VNUM_RIFT ) ) == NULL )
    {
        bugf( "Create_rift_mob: invalid ROOM_VNUM_RIFT" );
        return FALSE;
    }
    max_vnum = rift->area->max_vnum;

    lvnum = ROOM_VNUM_RIFT + 1;
    uvnum = lvnum;
    while ( get_room_index( uvnum ) != NULL && uvnum <= max_vnum )
        uvnum++;

    if ( uvnum <= lvnum + 1 )
    {
        bugf( "Create_rift_mob: not enough rift rooms." );
        return FALSE;
    }

    if ( ( mobIndex = get_mob_index( MOB_VNUM_LIFE ) ) == NULL )
    {
        bugf( "Create_rift_mob: invalid MOB_VNUM_LIFE" );
        return FALSE;
    }

    do
        rift = get_room_index( number_range( lvnum, uvnum - 1 ) );
    while ( rift == ch->in_room );

    mob = create_mobile( mobIndex );
    ch->pcdata->deathmob = mob->id;
    char_to_room( mob, rift );
    act_color( AT_MAGIC, "There is a brief pulse of light and $n appears",
               mob, NULL, NULL, TO_ROOM, POS_RESTING );
    return TRUE;
}


static void
death_continue( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *	mob_rift;
    ROOM_INDEX_DATA *	rift;
    MOB_INDEX_DATA *	mobIndex;
    CHAR_DATA *		mob;
    int			lvnum;
    int			max_vnum;
    int			uvnum;

    if ( IS_NPC( ch ) )
        return;

    if ( ch->level <= LEVEL_NEWBIE )
    {

        if ( ( rift = get_room_index( ROOM_VNUM_NEWBIE_MORGUE ) ) == NULL )
        {
            bugf( "Death_continue: no rift." );
            if ( ( rift = get_room_index( ROOM_VNUM_SCHOOL ) ) == NULL )
                rift = get_room_index( ROOM_VNUM_LIMBO );
            char_from_room( ch );
            char_to_room( ch, rift );
            ch->pcdata->deathstate = DEATH_NONE;
            ch->pcdata->deathroom = 0;
            ch->pcdata->deathmob = 0;
            send_to_char( "You live again!\n\r", ch );
            return;
        }

        send_to_char(
"\n\r`WDeath nods approvingly and speaks:\n\r\n\r"
"'`GI HOLD YOUR SOUL, BUT SINCE YOU ARE SO YOUNG I WILL\n\r"
"GIVE IT BACK TO YOU. THERE IS, OF COURSE, ONE CATCH. YOU MUST NOW\n\r"
"SEEK OUT SOMEONE WHO CAN RESTORE YOUR SOUL TO YOUR MORTAL SHELL.`W'\n\r\n\r"
        , ch );

        char_from_room( ch );
        char_to_room( ch, rift );
        do_look( ch, "auto" );
        ch->pcdata->deathstate = DEATH_RESURRECT;
        return;
    }

    if ( ( rift = get_room_index( ROOM_VNUM_RIFT ) ) == NULL )
    {
        bugf( "Death_continue: no rift." );
        if ( ( rift = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
            rift = get_room_index( ROOM_VNUM_LIMBO );
        /* If we don't even have limbo we have a major problem. */
        /* Crash and burn, and analyze the core dump. */
        char_from_room( ch );
        char_to_room( ch, rift );
        ch->pcdata->deathstate = DEATH_NONE;
        ch->pcdata->deathroom = 0;
        ch->pcdata->deathmob = 0;
        send_to_char( "You live again!\n\r", ch );
        return;
    }

    max_vnum = rift->area->max_vnum;

    lvnum = ROOM_VNUM_RIFT + 1;
    uvnum = lvnum;
    while ( get_room_index( uvnum ) != NULL && uvnum <= max_vnum )
        uvnum++;

    if ( uvnum <= lvnum + 1 )	/* not enough rift rooms; need 2 min */
    {
        bugf( "Death_continue: not enough rift rooms." );
        if ( ( rift = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
            rift = get_room_index( ROOM_VNUM_LIMBO );
        char_from_room( ch );
        char_to_room( ch, rift );
        ch->pcdata->deathstate = DEATH_NONE;
        ch->pcdata->deathroom = 0;
        ch->pcdata->deathmob = 0;
        send_to_char( "You live again!\n\r", ch );
        return;
    }

    if ( ( mobIndex = get_mob_index( MOB_VNUM_LIFE ) ) == NULL )
    {
        bugf( "Death_continue: No life crystal mob." );
        if ( ( rift = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
            rift = get_room_index( ROOM_VNUM_LIMBO );
        char_from_room( ch );
        char_to_room( ch, rift );
        ch->pcdata->deathstate = DEATH_NONE;
        ch->pcdata->deathroom = 0;
        ch->pcdata->deathmob = 0;
        send_to_char( "You live again!\n\r", ch );
        return;
    }

    rift = get_room_index( number_range( lvnum, uvnum - 1 ) );
    do
        mob_rift = get_room_index( number_range( lvnum, uvnum - 1 ) );
    while ( rift == mob_rift );
    mob = create_mobile( mobIndex );

    ch->pcdata->deathmob = mob->id;

    char_from_room( ch );
    char_to_room( ch, rift );
    act_color( AT_MAGIC, "There is a brief pulse of light and $n appears",
               ch, NULL, NULL, TO_ROOM, POS_RESTING );
    char_to_room( mob, mob_rift );
    act_color( AT_MAGIC, "There is a brief pulse of light and $n appears",
               mob, NULL, NULL, TO_ROOM, POS_RESTING );

    send_to_char(
"\n\r`WDeath of Rats speaks to you in a low squeak:\n\r\n\r"
"'`GDEATH HAS YOUR SOUL.  IF YOU WISH TO HAVE IT BACK, YOU MUST SEEK\n\r"
"HIM OUT HERE IN THE LAND OF THE DEAD AND BARGAIN WITH HIM FOR IT.\n\r"
"GOOD LUCK, AS HE DOES NOT GIVE UP EASILY..`W'\n\r\n\r"
"`WDeath of Rats scurries off into the fog of the deadlands.`X\n\r\n\r"
    , ch );

    do_look( ch, "auto" );
    act_color( AT_ACTION, "Another soul has perished and seeks the path back to life.", ch, NULL, NULL, TO_ROOM, POS_RESTING );

    ch->pcdata->deathstate = DEATH_SEARCH;
    return;
}


void
die_pc( CHAR_DATA *ch )
{
    send_to_char( "\n\r"
"`WAs your life force departs your mortal remains you feel a moment of confusion\n\r"
"and disorientation.  As the feeling passes a dark hooded figure, known as Death\n\r"
"appears before you, and gestures dismissively.  He breathes out a gust of cold air\n\r"
"as he speaks to you of the Choices:\n\r\n\r"
"'`GAS YOU NO DOUBT HAVE NOTICED, YOU ARE DEAD. BUT, THIS IS NOT THE END.\n\r"
"`GI GIVE YOU THE CHOICE, WILL YOUR DEATH BE `RPERMANENT`G, OR\n\r"
" WILL YOU `YCONTINUE`G ON A PATH BACK TO THE LIVING?`W'\n\r\n\r"
"Your choice?\n\r\n\r"
    , ch );

    ch->pcdata->deathstate = DEATH_ASK;
    return;
}


OBJ_DATA *
find_corpse( CHAR_DATA *ch )
{
    OBJ_DATA *	corpse;

    if ( IS_NPC( ch ) )
        return NULL;

    for ( corpse = object_list; corpse != NULL; corpse = corpse->next )
        if ( corpse->item_type == ITEM_CORPSE_PC
        &&   corpse->value[5] == ch->id )
            break;

    return corpse;
}


static void
send_greeting( DESCRIPTOR_DATA *d )
{
    static int	greet_num;
    char	greet_key[SHORT_STRING_LENGTH];
    HELP_DATA *	pHelp;
    char *	help_greeting;

    if ( greet_num == 0 )
	strcpy( greet_key, "GREETING" );
    else
	sprintf( greet_key, "GREETING%d", greet_num );
    greet_num++;

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
	if ( is_exact_name( greet_key, pHelp->keyword ) )
	    break;
    }

    if ( pHelp == NULL )
    {
	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
	    if ( is_exact_name( "GREETING", pHelp->keyword ) )
		break;
	greet_num = 1;
    }

    if ( pHelp == NULL )
	help_greeting = "What is your name? ";
    else
	help_greeting = pHelp->text;

    if ( *help_greeting == '.' )
        write_to_buffer( d, help_greeting+1, 0 );
    else
        write_to_buffer( d, help_greeting  , 0 );
    write_to_buffer( d, "`X", 0 );
}


static void
show_eye_choices( DESCRIPTOR_DATA *d )
{
    char	buf[MAX_STRING_LENGTH];
    char *	p;
    char *	race;
    int		i;
    int		pos;

    p = stpcpy( buf, "`Y" );
    pos = 0;
    race = race_table[d->character->race].who_name;
    for ( i = 0; !IS_NULLSTR( eye_color_table[i].desc ); i++ )
    {
        if ( is_exact_name( race, eye_color_table[i].list ) )
        {
            p += sprintf( p, "      %-22s", eye_color_table[i].desc );
            pos++;
            if ( pos > 1 )
            {
                p = stpcpy( p, "\n\r" );
                pos = 0;
            }
        }
    }
    if ( pos != 0 )
        p = stpcpy( p, "\n\r" );
    write_to_buffer( d, buf, 0 );
    return;
}

static void
show_eye_adj_choices( DESCRIPTOR_DATA *d )
{
    char	buf[MAX_STRING_LENGTH];
    char *	p;
    char *	race;
    int		i;
    int		pos;

    p = stpcpy( buf, "`Y" );
    pos = 0;
    race = race_table[d->character->race].who_name;
    for ( i = 0; !IS_NULLSTR( eye_adj_table[i].desc ); i++ )
    {
        if ( is_exact_name( race, eye_adj_table[i].list ) )
        {
            p += sprintf( p, "      %-22s", eye_adj_table[i].desc );
            pos++;
            if ( pos > 1 )
            {
                p = stpcpy( p, "\n\r" );
                pos = 0;
            }
        }
    }
    if ( pos != 0 )
        p = stpcpy( p, "\n\r" );
    write_to_buffer( d, buf, 0 );
    return;
}

static void
show_hair_choices( DESCRIPTOR_DATA *d )
{
    char	buf[MAX_STRING_LENGTH];
    char *	p;
    char *	race;
    int		i;
    int		pos;

    p = stpcpy( buf, "`Y" );
    pos = 0;
    race = race_table[d->character->race].who_name;
    for ( i = 0; !IS_NULLSTR( hair_color_table[i].desc ); i++ )
    {
        if ( is_exact_name( race, hair_color_table[i].list ) )
        {
            p += sprintf( p, "      %-22s", hair_color_table[i].desc );
            pos++;
            if ( pos > 1 )
            {
                p = stpcpy( p, "\n\r" );
                pos = 0;
            }
        }
    }
    if ( pos != 0 )
        p = stpcpy( p, "\n\r" );
    write_to_buffer( d, buf, 0 );
    return;
}

static void
show_hair_adj_choices( DESCRIPTOR_DATA *d )
{
    char	buf[MAX_STRING_LENGTH];
    char *	p;
    char *	race;
    int		i;
    int		pos;

    p = stpcpy( buf, "`Y" );
    pos = 0;
    race = race_table[d->character->race].who_name;
    for ( i = 0; !IS_NULLSTR( hair_adj_table[i].desc ); i++ )
    {
        if ( is_exact_name( race, hair_adj_table[i].list ) )
        {
            p += sprintf( p, "      %-22s", hair_adj_table[i].desc );
            pos++;
            if ( pos > 1 )
            {
                p = stpcpy( p, "\n\r" );
                pos = 0;
            }
        }
    }
    if ( pos != 0 )
        p = stpcpy( p, "\n\r" );
    write_to_buffer( d, buf, 0 );
    return;
}


static int
show_kingdoms( DESCRIPTOR_DATA *d )
{
    char	buf[MAX_STRING_LENGTH];
    int		count;
    int		k;
    char *	p;
    int		race;

    race = d->character->race;
    p = buf;
    *p = '\0';
    count = 0;

    for ( k = 1; k < MAX_KINGDOM; k++ )
    {
        if ( race_table[race].kingdom_list[k] && kingdom_table[k].recall != 0 )
        {
            count++;
            p += sprintf( p, "    `Y%d.  `W%s\n\r", count, kingdom_table[k].desc );
	}
    }

    write_to_buffer( d, buf, 0 );
    return count;
}


static void
show_races( DESCRIPTOR_DATA *d )
{
    char	buf[MAX_STRING_LENGTH];
    int		col;
    char *	p;
    int		race;

    col = 0;
    p = buf;
    for ( race = 1; race_table[race].name != NULL; race++ )
    {
        if ( !race_table[race].pc_race )
            continue;
        if ( d->character->alignment > race_table[race].max_align
        ||   d->character->alignment < race_table[race].min_align )
            continue;
        p += sprintf( p, "      %-12s", race_table[race].name );
        col++;
        if ( col % 3 == 0 )
            p = stpcpy( p, "\n\r" );
    }
    if ( col % 3 != 0 )
        p = stpcpy( p, "\n\r" );
    write_to_buffer( d, buf, 0 );
}


static void
update_recent( int wizlvl, const char *str )
{
    char buf[MAX_RECENT_LINES][SHORT_STRING_LENGTH];
    char *s;
    FILE *fp;
    int   i;

    for ( i = 0; i < MAX_RECENT_LINES; i++ )
	buf[i][0] = '\0';

    s = ctime( &current_time );
    *( s + strlen( s ) - 1 ) = '\0';
    sprintf( buf[0], "%d %s: %s", wizlvl, s, str );

    if ( ( fp = fopen( SYSTEM_DIR RECENT_FILE, "r" ) ) != NULL )
    {
	i = 1;
	while( fgets( buf[i], sizeof( buf[0] ), fp ) != NULL )
	{
	    if ( ( s = strchr( buf[i], '\n' ) ) != NULL )
		*s = '\0';
	    i++;
	    if ( i >= MAX_RECENT_LINES )
		break;
	}
	fclose( fp );
    }

    if ( ( fp = fopen( SYSTEM_DIR RECENT_FILE, "w" ) ) != NULL )
    {
	for ( i = 0; i < MAX_RECENT_LINES; i++ )
	{
	    if ( buf[i][0] == '\0' )
		break;
	    fprintf( fp, "%s\n", buf[i] );
	}
	fclose( fp );
    }

    return;
}


