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


#define unix 1
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
#include "olc.h"
#include "recycle.h"

/* Decklarean */
const char colors[ ] = ".rgObpcwzRGYBPCW";
static bool fFound = FALSE;

char *extract_line( char **src );
char *format_prog( CHAR_DATA *ch, char *s, char *i, char *f );
char *format_string_partial( CHAR_DATA *ch, char *s, char *i, char *f );
char *new_format_string( char *oldstring );
void string_number( CHAR_DATA *ch, char *pString );

/*
 *  string_number()
 *  Displays a string with line numbers and (optionally) color codes revealed.
 */
void
string_number( CHAR_DATA *ch, char *pString )
{
    BUFFER *	pBuf;
    char	oneline[MAX_STRING_LENGTH];
    const char *src;
    char *	dest;
    char *	p;
    int		count;

    if ( pString == NULL )
    {
        send_to_char( "(blank)\n\r", ch );
        return;
    }

    pBuf = new_buf( );
    count = 0;

    for ( src = pString; *src != '\0';  )
    {
        dest = oneline;

        while ( *src != '\0' && *src != '\n' )
        {
            *dest = *src;
            /* show the color codes */
            if ( *src == '`' && IS_SET( ch->act2, PLR_SEE_COL_CODE ) )
            {
                if ( *(src+1) == '`' )
                {
                    *++dest = *++src;
                }
                else
                {
                    if ( ( p = strchr( colorcode_list, *(src+1) ) ) != NULL )
                    {
                        *++dest = *++src;
                        *++dest = '`';
                        *++dest = '`';
                        *++dest = *src;
                    }
                }
            }
            src++;
            dest++;
        }

        if ( *src == '\n' )
            *dest++ = *src++;

        if ( *src == '\r' )
            *dest++ = *src++;

        *dest = '\0';
        count++;
        buf_printf( pBuf, "`X%2d] %s", count, oneline );
    }

    set_char_color( AT_YELLOW, ch );
    send_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
}


/*****************************************************************************
 Name:		string_edit
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
void
string_edit( CHAR_DATA *ch, char **pString )
{
    DESCRIPTOR_DATA *	d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( pString == d->pString && *pString != NULL )
        {
            send_to_char( "Sorry, somebody else is already editing that.\n\r", ch );
            return;
        }
    }

    act( "$n begins editing some text.", ch, NULL, NULL, TO_ROOM );
    send_to_char("-========- Entering EDIT Mode -=========-\n\r", ch );
    send_to_char("  Type /h  or .h on a new line for help\n\r", ch );
    send_to_char(" Terminate with a @ on a blank line.\n\r", ch );
    send_to_char("-=======================================-\n\r", ch );

    free_string( *pString );

    *pString = str_dup( "" );

    ch->desc->pString = pString;

    return;
}



/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void
string_append( CHAR_DATA *ch, char **pString )
{
    DESCRIPTOR_DATA *	d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( pString == d->pString && *pString != NULL )
        {
            send_to_char( "Sorry, somebody else is already editing that.\n\r", ch );
            return;
        }
    }

    act( "$n begins editing some text.", ch, NULL, NULL, TO_ROOM );
    send_to_char("-=======- Entering APPEND Mode -========-\n\r", ch );
    send_to_char("  Type /h or .h on a new line for help\n\r", ch );
    send_to_char(" Terminate with a @ on a blank line.\n\r", ch );
    send_to_char("-=======================================-\n\r", ch );

    if ( *pString == NULL )
    {
        *pString = str_dup( "" );
    }

    string_number( ch, *pString );

    if ( *(*pString + strlen( *pString ) - 1) != '\r' )
    send_to_char( "\n\r", ch );

    ch->desc->pString = pString;

    return;
}


/*****************************************************************************
 Name:		string_replace
 Purpose:	Substitutes one string for another.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char * string_replace( char * orig, char * old, char * new, int offset )
{
    char xbuf[MAX_STRING_LENGTH];
    char *p;
    int i;

    xbuf[0] = '\0';
    fFound = FALSE;
    if ( strlen( orig ) > MAX_STRING_LENGTH - 2 )
	return orig;
    if ( strlen( orig ) + strlen( new ) - strlen( old ) > MAX_STRING_LENGTH - 2  )
	return orig;
    strcpy( xbuf, orig );
    p = orig;
    while ( offset > 0 )
    {
	p = strchr( p, '\n' );
	if ( p == NULL )
	    return orig;
	p++;
	offset--;
    }
    if ( strstr( p, old ) != NULL )
    {
        i = strlen( orig ) - strlen( strstr( p, old ) /*+ ( p - orig )*/ );
        xbuf[i] = '\0';
        strcat( xbuf, new );
        strcat( xbuf, &orig[i+strlen( old )] );
        fFound = TRUE;
    }

    free_string( orig );

    return str_dup( xbuf );
}


/* OLC 1.1b */
/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	buf[MAX_STRING_LENGTH];
    char *	s;

    /*
     * Thanks to James Seng
     */
    smash_tilde( argument );

    strcpy( buf, "Tell Mac he's got a bug.\n\r" );

    if ( *argument == '.'|| *argument == '/' )
    {
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	char arg4 [MAX_INPUT_LENGTH];

	if ( *(argument+1) == '/' && IS_IMMORTAL( ch ) )
	{
	    interpret( ch, argument+2 );
	    return;
	}

	argument = one_argument( argument, arg1 );

	if ( !str_cmp( arg1, ".@" ) || !str_cmp( arg1, "/@" ) )
	{
	    ch->desc->pString = NULL;
	    REMOVE_BIT( ch->act2, PLR_SEE_COL_CODE );
	    act( "$n finishes editing $s text.", ch, NULL, NULL, TO_ROOM);
	    if ( !IS_NPC( ch ) && *buf_string( ch->pcdata->buffer ) != '\0' )
	    {
		send_to_char( "You have unreceived tells.  Type 'replay' to view.\n\r", ch );
	    }
	    if ( ( pArea = get_area_edit( ch ) ) != NULL )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}

	if ( !str_cmp( arg1, ".c" ) || !str_cmp( arg1, "/c"  ) )
	{
	    send_to_char( "String cleared.\n\r", ch );
	    free_string( *ch->desc->pString );
	    *ch->desc->pString = str_dup( "" );
//	    **ch->desc->pString = '\0';
	    return;
	}

	if ( !str_cmp( arg1, ".i" ) || !str_cmp( arg1, "/i" ) )
	{
	    *ch->desc->pString = string_insline( ch, argument,
			*ch->desc->pString );
	    return;
	}

	if ( !str_cmp( arg1, "./" ) || !str_cmp( arg1, "//" ) )
	{
	    interpret( ch, argument );
	    return;
	}

	/* prevent conflict with string_insline */
	argument = first_arg( argument, arg2, FALSE );
	argument = first_arg( argument, arg3, FALSE );

	if ( !str_cmp( arg1, ".s" ) || !str_cmp( arg1, "/s" ) )
	{
	    page_to_char( *ch->desc->pString, ch );
	    return;
	}

	if ( !str_cmp( arg1, ".l" ) || !str_cmp( arg1, "/l" ) )
	{
	    send_to_char( "String so far:\n\r", ch );
	    string_number( ch, *ch->desc->pString );
	    return;
	}

	if ( !str_cmp( arg1, ".r" ) || !str_cmp( arg1, "/r" ) )
	{
	    char *p;

	    argument = first_arg( argument, arg4, FALSE );
	    if ( arg2[0] == '\0' )
	    {
		send_to_char(
		    "usage:  .r \"old string\" \"new string\"\n\r", ch );
		return;
	    }

	    p = arg2;
	    while ( *p )
	    {
		if ( *p == '\\' && *( p + 1 ) == 'n' )
		{
		    *p++ = '\n';
		    *p = '\r';
		}
		p++;
	    }
	    p = arg3;
	    while ( *p )
	    {
		if ( *p == '\\' && *( p + 1 ) == 'n' )
		{
		    *p++ = '\n';
		    *p = '\r';
		}
		p++;
	    }
	    p = arg4;
	    while ( *p )
	    {
		if ( *p == '\\' && *( p + 1 ) == 'n' )
		{
		    *p++ = '\n';
		    *p = '\r';
		}
		p++;
	    }

	    if ( arg4[0] != '\0' )
	    {
		if ( is_number( arg2 ) )
		{
		    *ch->desc->pString =
			string_replace( *ch->desc->pString, arg3, arg4, atoi( arg2 ) - 1 );
		    sprintf( buf, "'%s' replaced with '%s'.\n\r", arg3, arg4 );
		}
		else
		{
		    send_to_char( "Mismatched quotes, or first argument not numeric.\n\r", ch );
		    return;
                }
	    }
	    else
	    {
		*ch->desc->pString =
			string_replace( *ch->desc->pString, arg2, arg3, 0 );
		sprintf( buf, "'%s' replaced with '%s'.\n\r", arg2, arg3 );
	    }
	    if ( !fFound )
	    {
		strcpy( buf, "String not found.\n\r" );
	    }
	    send_to_char( buf, ch );
	    return;
	}

	if ( !str_cmp( arg1, ".d" ) || !str_cmp( arg1, "/d" ) )
	{
	    *ch->desc->pString = string_delline( ch, arg2, arg3,
			*ch->desc->pString );
	    return;
	}

        if ( !str_cmp( arg1, ".f" ) || !str_cmp( arg1, "/f" ) )
        {
	    switch ( ch->desc->editor )
	    {
		default:
	            *ch->desc->pString = format_string_partial( ch,
			*ch->desc->pString, arg2, arg3 );
	            send_to_char( "String formatted.\n\r", ch );
	            return;
		case ED_EPROG:
		case ED_MPROG:
		case ED_OPROG:
		case ED_RPROG:
		    s = format_prog( ch, *ch->desc->pString, arg2, arg3 );
	            if ( s != NULL )
	            {
	                *ch->desc->pString = s;
	                send_to_char( "Program formatted.\n\r", ch );
	            }
	            else
	            {
	                send_to_char( "Program unchanged.\n\r", ch );
	            }
	            return;
            }
        }

	if ( !str_cmp( arg1, ".&" ) || !str_cmp( arg1, "/&" ) )
	{
	    ch->act2 ^= PLR_SEE_COL_CODE;
	    return;
	}

        if ( !str_cmp( arg1, ".h" ) || !str_cmp( arg1, "/h" )
          || !str_cmp( arg1, ".?" ) || !str_cmp( arg1, "/?" ) )
        {
	    send_to_char(
	"String help (commands on blank line):   \n\r"
	"/c               - clear string so far \n\r"
	"/d n             - delete line number n\n\r"
	"/d n1 n2         - delete lines n1 through n2\n\r"
	"/f               - format (word wrap) string  \n\r"
	"/f n1 [n2]       - word wrap just lines n1 - n2\n\r"
	"/h or /?         - get help (this info)\n\r"
	"/i n             - insert blank line before line n\n\r"
	"/i n <string>    - insert string before line n\n\r"
	"/l               - list string with line numbers\n\r"
	"/r 'old' 'new'   - replace a substring (recognizes '', \"\")\n\r"
	"                   \\n matches end of line, use to split/merge lines\n\r"
	"/r n 'old' 'new' - same, except begin search at line n\n\r"
	"/s               - show string so far  \n\r"
	"/&               - toggle display of color codes\n\r"
	"//               - perform a regular command\n\r"
	"@                - done, exit string editor\n\r", ch );
	    send_to_char("You may use a period (\".\") in place of a slash in the above commands\n\r"
				   "(eg .r 'old' 'new' etc)\n\r", ch );
            return;
        }

        send_to_char( "String:  Invalid command.\n\r", ch );
        return;
    }

    if ( !str_cmp( argument, "@" ) )
    {
	ch->desc->pString = NULL;
	REMOVE_BIT( ch->act2, PLR_SEE_COL_CODE );
        act( "$n finishes editing $s text.", ch, NULL, NULL, TO_ROOM);
	if ( !IS_NPC( ch ) && *buf_string( ch->pcdata->buffer ) != '\0' )
	{
	    send_to_char( "You have unreceived tells.  Type 'replay' to view.\n\r", ch );
	}
	if ( ( pArea = get_area_edit( ch ) ) != NULL )
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }

    /*
     * Truncate strings to MAX_STRING_LENGTH.
     * --------------------------------------
     */
    if ( strlen( *ch->desc->pString ) + strlen( argument ) >= ( MAX_STRING_LENGTH - 100 ) )
    {
        send_to_char("String too long, last line skipped.\n\r", ch );

	/* Force character out of editing mode. */
        ch->desc->pString = NULL;
        return;
    }

    strcpy( buf, *ch->desc->pString );
    strcat( buf, argument );
    strcat( buf, "\n\r" );
    free_string( *ch->desc->pString );
    *ch->desc->pString = str_dup( buf );
    return;
}

/*****************************************************************************
 Name:		extract_line
 Purpose:	copies a line of text to a static buffer, discards newline
 Called by:	format_prog(string.c)
 ****************************************************************************/
char *extract_line( char **src )
{
    static char buf[ MAX_STRING_LENGTH ];

    char *s = *src;
    char *d = buf;

    while ( *s == ' ' )
	s++;

    while ( *s != '\0' && *s != '\n' && *s != '\r' )
    {
	*d++ = *s++;
    }

    *d = '\0';
    while ( *s != '\0' && ( *s == '\n' || *s == '\r' ) )
	s++;

    *src = s;
    return buf;
}

/*****************************************************************************
 Name:		format_prog
 Purpose:	Special prog formatting.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *format_prog( CHAR_DATA *ch, char *s, char *init, char *final )
{
    char	buf[MAX_STRING_LENGTH*2];
    char	word [MAX_INPUT_LENGTH];
    int		indent = 0;
    int		index;
    char 	*p;
    char	*q;
    char	*t;

    p = s;
    q = buf;
    while ( *p != '\0' )
    {
	if ( ( q - buf ) > ( MAX_STRING_LENGTH - 128 ) )
	{
	    send_to_char( "Sorry, prog too long to format.\n\r", ch );
	    return NULL;
	}

	t = extract_line( &p );
	one_argument( t, word );

	if ( !str_cmp( word, "if" ) )
	{
	    for ( index = 0; index < UMIN( indent, 10 ); index++ )
		q = stpcpy( q, "  " );
	    q = stpcpy( q, t );
	    indent++;
	}
	else if ( !str_cmp( word, "or" ) )
	{
	    indent--;
	    for ( index = 0; index < UMIN( indent, 10 ); index++ )
		q = stpcpy( q, "  " );
	    q = stpcpy( q, t );
	    indent++;
	}
	else if ( !str_cmp( word, "else" ) )
	{
	    indent--;
	    for ( index = 0; index < UMIN( indent, 10 ); index++ )
		q = stpcpy( q, "  " );
	    q = stpcpy( q, t );
	    indent++;
	}
	else if ( !str_cmp( word, "endif" ) )
	{
	    indent--;
	    for ( index = 0; index < UMIN( indent, 10 ); index++ )
		q = stpcpy( q, "  " );
	    q = stpcpy( q, t );
	}
	else
	{
	    for ( index = 0; index < UMIN( indent, 10 ); index++ )
		q = stpcpy( q, "  " );
	    q = stpcpy( q, t );
	}
	q = stpcpy( q, "\n\r" );
    }

    if ( strlen( buf ) >= MAX_STRING_LENGTH - 2 )
    {
	send_to_char( "Sorry, prog too long to format.\n\r", ch );
	return NULL;
    }
    free_string( s );
    return str_dup( buf );
}

/*
 *  Thanks to Kalgen for the new procedure (no more bug!)
 *  Original wordwrap() written by Surreality.
 */
/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add(string.c) (many)olc_act.c
 ****************************************************************************/
char *format_string( char *oldstring /*, bool fSpace */)
{
    char	xbuf [ MAX_STRING_LENGTH ];
    char	xbuf2[ MAX_STRING_LENGTH ];
    char *	rdesc;
    int		i = 0;
    int		linelen;
    bool	cap = TRUE;

    xbuf[ 0 ] = xbuf2 [ 0 ] = 0;

    i = 0;

    if ( strlen( oldstring ) >= ( MAX_STRING_LENGTH - 100 ) )	/* OLC 1.1b */
    {
	bug( "String to format_string() longer than MAX_STRING_LENGTH.", 0 );
	return (oldstring);
    }

    /*
     * Copy string, discard CR's, replace newlines with spaces.
     * Collapse consecutive spaces to a single space.
     */
    for ( rdesc = oldstring; *rdesc; rdesc++ )
    {
	if ( *rdesc == '\n' )
	{
	    if ( i > 0 && xbuf[ i - 1 ] != ' ' )
	    {
		xbuf[ i ] = ' ';
		i++;
	    }
	}
	else if ( *rdesc == '\r' )
	    ;
	else if ( *rdesc == ' ' )
	{
	    if ( i > 0 && xbuf[ i - 1 ] != ' ')
	    {
		xbuf[ i ] = ' ';
		i++;
	    }
	}
	else if ( *rdesc == ')' )
	{
	    if ( i > 2 && xbuf[ i - 1 ] == ' ' && xbuf[ i - 2 ] == ' ' && 
		( xbuf[i - 3] == '.' || xbuf[i-3] == '?' || xbuf[i-3] == '!'))
	    {
		xbuf[ i - 2 ] = *rdesc;
		xbuf[ i - 1 ] = ' ';
		xbuf[ i]      = ' ';
		i++;
	    }
	    else
	    {
		xbuf[ i ] = *rdesc;
		i++;
	    }
	}
	else if ( ( *rdesc == '.' || *rdesc == '?' || *rdesc=='!' )
	       && !isalnum( *(rdesc+1) )
	       && ( *(rdesc+1) != '`' )
	       && ( i > 2 ) )
	{
	    if ( i > 2 && xbuf[ i - 1 ] == ' ' && xbuf[ i - 2 ] == ' ' && 
		( xbuf[i-3] == '.' || xbuf[i-3] == '?' || xbuf[i-3] == '!' ) )
	    {
		xbuf[ i - 2 ] = *rdesc;
		if ( *( rdesc + 1 ) != '\"' )
		{
		    xbuf[ i - 1 ] = ' ';
		    xbuf[ i ]     = ' ';
		    i++;
		}
		else
		{
		    xbuf[ i - 1 ] = '\"';
		    xbuf[ i ]     = ' ';
		    xbuf[ i + 1 ] = ' ';
		    i+=2;
		    rdesc++;
		}
	    }
	    else
	    {
		xbuf[ i ] = *rdesc;
		if ( *( rdesc + 1 ) != '\"' )
		{
		    xbuf[ i + 1 ] = ' ';
		    xbuf[ i + 2 ] = ' ';
		    i += 3;
		}
		else
		{
		    xbuf[ i + 1 ] = '\"';
		    xbuf[ i + 2 ] = ' ';
		    xbuf[ i + 3 ] = ' ';
		    i += 4;
		    rdesc++;
		}
	    }
	    cap = TRUE;
	}
	else
	{
	    xbuf[ i ] = *rdesc;
	    if ( cap )
	    {
		cap = FALSE;
		xbuf[ i ] = UPPER( xbuf[ i ] );
	    }
	    i++;
	}
    }

    xbuf[ i ] = 0;
    strcpy( xbuf2, xbuf );

    rdesc = xbuf2;

    xbuf[ 0 ] = 0;

    for ( ; ; )
    {
	linelen = 76;
	for ( i = 0; i <= linelen; i++ )
	{
	    if ( !*( rdesc + i ) )
		break;
	    if ( *( rdesc+i) == '`'
		 && strchr( ".rgObpcwzRGYBPCWx`", *(rdesc+i+1) ) != NULL )
	    {
		i++;
		linelen += 2;
	    }
	}
	if ( i <= linelen )
	{
	    break;
	}
	for ( i = ( xbuf[0] ? linelen : linelen - 3 ) ; i ; i--)
	{
	    if ( *( rdesc + i ) == ' ')
		break;
	}
	if ( i )
	{
	    *( rdesc + i ) = 0;
	    strcat( xbuf, rdesc );
	    strcat( xbuf, "\n\r" );
	    rdesc += i + 1;
	    while ( *rdesc == ' ' )
		rdesc++;
	}
	else
	{
	    bug ( "No spaces", 0 );
	    *( rdesc + linelen - 1 ) = 0;
	    strcat( xbuf, rdesc );
	    strcat( xbuf, "-\n\r" );
	    rdesc += linelen;
	}
    }
    while ( *( rdesc + i ) && 
		( *( rdesc + i ) == ' '||
		  *( rdesc + i ) == '\n'||
		  *( rdesc + i ) == '\r' ) )
	i--;

    *( rdesc + i + 1 ) = 0;
    strcat( xbuf, rdesc );
    if ( xbuf[ strlen( xbuf ) - 2 ] != '\n')
	strcat( xbuf, "\n\r" );

    free_string( oldstring );
    return( str_dup( xbuf ) );
}

char *new_format_string( char *oldstring )
{
    char buf1[ MAX_STRING_LENGTH ];
//    char buf2[ MAX_STRING_LENGTH ];
    char *pSrc;
    char *pDst;

    /* Sanity check */
    if ( strlen( oldstring ) > MAX_STRING_LENGTH - 100 )
    {
	bug( "String to format_string() longer than MAX_STRING_LENGTH.", 0 );
	return ( oldstring );
    }

    pSrc = oldstring;
    pDst = buf1;
    while ( *pSrc )
    {
	if ( *pSrc == '\n' )
	{
	    if ( pDst > buf1 && *(pDst - 1) != ' ')
		*pDst++ = ' ';
	}
	else if ( *pSrc == '\r' )
	    ;
	else
	{
	    *pDst++ = *pSrc;
	}
	pSrc++;
    }
    *pDst = '\0';
    free_string( oldstring );
    return str_dup( buf1 );
}

/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
 		Understands quates, parenthesis (barring ) ('s) and
 		percentages.  Argument is converted to lowercase if
 		fCase is TRUE.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *
first_arg( char *argument, char *arg_first, bool fCase )
{
    char cEnd;

    while ( *argument == ' ' )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"'
      || *argument == '%'  || *argument == '(' )
    {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
	}
        else cEnd = *argument++;
    }

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	if ( fCase )
	{
	    if ( is_colcode( argument ) )
	    {
	        *arg_first++ = *argument++;
	        *arg_first   = *argument;
	    }
	    else if ( *argument == '`' && *(argument + 1) == '`' )
	    {
	        *arg_first++ = *argument++;
	        *arg_first   = *argument;
	    }
	    else
	    {
		*arg_first = LOWER( *argument );
	    }
	}
	else
            *arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( *argument == ' ' )
	argument++;

    return argument;
}


/*
 * Used in olc_act.c for aedit_builders.
 */
char * string_unpad( char * argument )
{
    char buf[MAX_STRING_LENGTH];
    char *s;

    s = argument;

    while ( *s == ' ' )
        s++;

    strcpy( buf, s );
    s = buf;

    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;

        while( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }

    free_string( argument );
    return str_dup( buf );
}



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char * string_proper( char * argument )
{
    char *s;

    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER(*s);
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
	{
            s++;
        }
    }

    return argument;
}



/*
 * Returns an all-caps string.		OLC 1.1b
 */
char* all_capitalize( const char *str )
{
    static char strcap [ MAX_STRING_LENGTH+MAX_INPUT_LENGTH ];
           int  i;
    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = UPPER( str[i] );
    strcap[i] = '\0';
    return strcap;
}

char *string_delline( CHAR_DATA *ch, char *argument, char* argument2, char *old )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH*2];
    char oneline[MAX_STRING_LENGTH];
    char *dline;
    int count = 0;
    int ln;
    int ln2;

    argument = one_argument( argument, arg1 );
    if ( !is_number( arg1 ) )
    {
	send_to_char( "Delete which line?\n\r", ch );
	return old;
    }
    ln = atoi( arg1 ) - 1;

    argument2 = one_argument( argument2, arg2 );
    if ( arg2[0] == '\0' )
	ln2 = ln;
    else if ( is_number( arg2 ) )
	ln2 = atoi( arg2 ) - 1;
    else
    {
	send_to_char( "Delete to which line?\n\r", ch );
	return old;
    }

    if ( ln < 0 || ln2 < 0 )
    {
	send_to_char( "Line doesn't exist.\n\r", ch );
	return old;
    }
    if ( ln2 < ln )
    {
	send_to_char( "End line less than begin line.\n\r", ch );
	return old;
    }

    buf[0] = '\0';
    for ( dline = old; *dline != '\0'; )
    {
	int curr = 0;

	for ( ; *dline != '\0' && *dline != '\n'; dline++, curr++ )
	{
	    oneline[curr] = *dline;
	}

	if ( *dline == '\n' )
	{
	    oneline[curr] = *dline;
	    dline++, curr++;
	}
	if ( *dline == '\r' )
	{
	    oneline[curr] = *dline;
	    dline++, curr++;
	}
	oneline[curr] = '\0';
	curr++;

	if ( count < ln || count > ln2 )
	    strcat( buf, oneline );

	count++;
    }

    if ( count <= ln2 )
    {
	send_to_char( "Line doesn't exist.\n\r", ch );
	return old;
    }
    ch_printf( ch, "Line%s deleted.\n\r", ln == ln2 ? "" : "s" );
    free_string( old );
    return str_dup( buf );
}

char *
string_insline( CHAR_DATA *ch, char *argument, char *old )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char oneline[MAX_STRING_LENGTH];
    char *dline;
    int count = 0;
    int ln = 0;

    argument = one_argument( argument, arg );
    if ( is_number( arg ) )
        ln = atoi( arg ) - 1;
    else
    {
        send_to_char( "Syntax: .i # [newstring]\n\r", ch );
        return old;
    }

    if ( ln < 0 )
    {
        send_to_char( "Line doesn't exist.\n\r", ch );
        return old;
    }

    if ( strlen( argument ) + strlen( old ) > MAX_STRING_LENGTH - 3 )
    {
        send_to_char( "String too long.\n\r", ch );
        return old;
    }

    buf[0] = '\0';
    for ( dline = old; *dline != '\0'; )
    {
        int curr = 0;

        for ( ; *dline != '\0' && *dline != '\n'; dline++,  curr++ )
        {
            oneline[curr] = *dline;
        }

        if ( *dline == '\n' )
        {
            oneline[curr] = *dline;
            dline++, curr++;
        }

        if ( *dline == '\r' )
        {
            oneline[curr] = *dline;
            dline++, curr++;
        }

        oneline[curr] = '\0';
        curr++;

        if ( ln == count )
        {
            if ( argument[0] != '\0' )
                strcat( buf, argument);
            strcat( buf, "\n\r\0" );

            count++;
        }

        strcat(buf, oneline);

        count++;
    }

    if ( count <= ln )
    {
        send_to_char( "Line doesn't exist.\n\r", ch );
        return old;
    }
    send_to_char( "Line inserted.\n\r", ch );
    free_string( old );
    return str_dup( buf ); 
}


char *
format_string_partial( CHAR_DATA *ch, char *old, char *start, char *end )
{
    char	first  [ MAX_STRING_LENGTH ];
    char	middle [ MAX_STRING_LENGTH ];
    int		iStart;
    int		count;
    char *	p;
    char *	q;
    char	c;

    if ( strlen( old ) > MAX_STRING_LENGTH - 100 )
    {
	send_to_char( "String too long to format.\n\r", ch );
	return old;  
    }

    if ( start[0] == '\0' )
    {
	iStart = 0;
    }
    else if ( !is_number( start ) )
    {
	send_to_char( "Syntax: .f n1 n2\n\r", ch );
	return old;
    }
    else
    {
	iStart = atoi( start ) - 1;
    }

    first[ 0 ] = middle[ 0 ] = '\0';
    p = old;

    /* Copy lines to first buffer until line iStart */
    q = first;
    count = iStart;
    while ( count )
    {
	while ( ( c = *p ) != '\0' )
	{
	    if ( c != '\r' )
		*q++ = c;
	    p++;
	    if ( c == '\n' )
		break;
	}
	if ( c == '\0' )
	{
	    send_to_char( "Line not found.\n\r", ch );
	    return old;
	}
	if ( *p == '\r' )
	    *q++ = *p++;
	count--;
    }
    *q = '\0';

    /* copy end - iStart lines to middle buffer, or all lines if no end */
    if ( *end == '\0' )
    {
	q = middle;
	while ( *p )
	{
	    *q++ = *p++;
	}
	*q = '\0';
    }
    else
    {
	count = atoi( end ) - iStart;
	if ( count < 1 )
	{
	    send_to_char( "Bad range.\n\r", ch );
	    return old;
	}
	q = middle;
	while ( count )
	{
	    while ( ( c = *p ) != '\0' )
	    {
		if ( c != '\r' )
		    *q++ = c;
		p++;
		if ( c == '\n' )
		    break;
	    }
	    if ( c == '\0' )
	    {
		send_to_char( "Line not found.\n\r", ch );
		return old;
	    }
	    if ( *p == '\r' )
		*q++ = *p++;
	    count--;
	}
	*q = '\0';
    }

    q = format_string( str_dup( middle ) );
    strcat( first, q );
    strcat( first, p );
    free_string( q );
    return str_dup( first );
}

/*****************************************************************************
 Name:		string_change
 Purpose:	Replaces a substring if the argument starts with .r, else
 		copies the argument.  In either case it str_dup's the result
 		Returns NULL on error.  Reports errors to player.
 		src is always freed.
 Called by:	none
 ****************************************************************************/
char *string_change( CHAR_DATA *ch, char *src, char *argument )
{
    char buf [ MAX_STRING_LENGTH];
    char oldstr[MAX_INPUT_LENGTH];
    char newstr[MAX_INPUT_LENGTH];
    char *p;
    char *s;
    char *d;

    if ( src == NULL || argument == NULL )
	return NULL;

    if ( strlen( argument ) < 3 || !( ( *argument == '.' || *argument == '/' ) && LOWER(*(argument+1)) == 'r' ) )
    {
	free_string( src );
	strcpy( buf, argument );
	return str_dup( buf );
    }

    if ( src == NULL )
    {
	send_to_char( "No input string to search.\n\r", ch );
	return NULL;
    }

    argument += 2;
    argument = first_arg( argument, oldstr, FALSE );
    if ( oldstr[0] == '\0' )
    {
	send_to_char( "Replace what?\n\r", ch );
	return NULL;
    }
    while( isspace( *argument ) )
	argument++;
    if ( *argument == '\0' )
    {
	send_to_char( "Replace what with what?\n\r", ch );
	return NULL;
    }
    argument = first_arg( argument, newstr, FALSE );
    if ( strlen( src ) + strlen( newstr ) - strlen( oldstr ) > MAX_INPUT_LENGTH - 2 )
    {
	send_to_char( "String too long.\n\r", ch );
	return NULL;
    }

    if ( ( p = strstr( src, oldstr ) ) == NULL )
    {
	send_to_char( "String not found.\n\r", ch );
	return NULL;
    }

    s = src;
    d = buf;
    while ( s < p )
    {
	*d++ = *s++;
    }

    d = stpcpy( d, newstr );
    strcpy( d, p + strlen( oldstr ) );

    free_string( src );
    return str_dup( buf );

}
