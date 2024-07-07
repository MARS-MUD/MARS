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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include "merc.h"
#include "db.h"
#include "interp.h"
#include "lookup.h"
#include "recycle.h"
#include "tables.h"

/* temporary function */
static void read_ids( void );

/*
 * Local functions
 */
static	void	fix_header	args( ( AREA_DATA *pArea ) );
static	void	fread_board	args( ( FILE *fp ) );
static	void	fread_command	args( ( FILE *fp ) );
static	void	fread_group	args( ( FILE *fp ) );
static	void	fread_note	args( ( FILE *fp, BOARD_DATA *pBoard, time_t expire ) );
static	void	fread_project	args( ( FILE *fp ) );
static	void	fread_race	args( ( FILE *fp ) );
static	void	fread_skill	args( ( FILE *fp ) );
static	void	fread_social	args( ( FILE *fp ) );
static	void	fread_userrec	args( ( FILE *fp ) );
static	void	init_area_overland args( ( AREA_DATA *pArea ) );
static	void	load_board	args( ( BOARD_DATA *pBoard ) );
static	void	mprog_read_programs args( ( FILE *fp,
					MOB_INDEX_DATA *pMobIndex ) );
static	void	new_reset	args( ( ROOM_INDEX_DATA *pR,
					RESET_DATA *pReset ) );


/*
 * Macros for reading area headers
 */
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
		if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
		}

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
		}


/*
 * Abort mud.  Write string to shutdown.txt.  This will cause the
 * startup script to exit.
 */
void abort_mud( char *reason )
{
    FILE *fp;

    if ( IS_NULLSTR( reason ) )
	reason = "Previously preported error.";

    fprintf( stderr, "****ABORTING: %s\n", reason );
    if ( ( fp = fopen( SHUTDOWN_FILE, "w" ) ) )
    {
	fprintf( fp, "%s\n", reason );
	fclose( fp );
    }
    exit( EXIT_FAILURE );
}


/*
 * Set area vnums.  Eventually won't need this.
 */
void area_renumber( void )
{
    AREA_DATA *pArea;
    int iCount;

    iCount = 0;
    for ( pArea = area_first; pArea; pArea = pArea->next )
	pArea->vnum = iCount++;

    return;
}


void area_sort( AREA_DATA *pArea )
{
    AREA_DATA *fArea;

    if ( !pArea )	/* we're screwed */
    {
	abort_mud( "Area_sort: NULL pArea" );
    }

    area_last = pArea;

    if ( !area_first )
    {
	area_first = pArea;
	return;
    }

    for ( fArea = area_first; fArea; fArea = fArea->next )
    {
	if ( pArea->min_vnum == fArea->min_vnum ||
	   ( pArea->min_vnum > fArea->min_vnum &&
	   (!fArea->next || pArea->min_vnum < fArea->next->min_vnum) ) )
	{
	    pArea->next = fArea->next;
	    fArea->next = pArea;
	    return;
	}
    }

    pArea->next = area_first;
    area_first = pArea;
    return;

}


static void
init_area_overland( AREA_DATA *pArea )
{
    if ( pArea->min_vnum + pArea->maxx * pArea->maxy > pArea->max_vnum + 1 )
    {
	bugf( "Init_area_overland: number of virtual rooms too big for area." );
	pArea->maxx = 0;
	pArea->maxy = 0;
	return;
    }
    pArea->overland = new_overland( pArea->maxx, pArea->maxy );
}


/*
 * Load an area header, OLC format
 */
void
load_areadata( FILE *fp )
{
    AREA_DATA	*pArea;
    char	*word;
    bool	 fMatch;

    pArea		= alloc_perm( sizeof( *pArea ) );
    pArea->age		= 15;
    pArea->builders	= &str_empty[0];
    pArea->file_name	= str_dup( strArea );
    pArea->name		= str_dup( "New Area" );
    pArea->credits	= &str_empty[0];
    pArea->resetmsg	= &str_empty[0];
    pArea->norecall	= &str_empty[0];
    pArea->area_flags	= 0;
    pArea->low_level	= 0;
    pArea->high_level	= 0;
    pArea->min_vnum	= 0;
    pArea->max_vnum	= 0;
    pArea->maxx		= 0;
    pArea->maxy		= 0;
    pArea->nplayer	= 0;
    pArea->overland	= NULL;
    pArea->recall	= ROOM_VNUM_TEMPLE;
    pArea->security	= 0;
    pArea->vnum		= top_area; /* gets reset after boot */
    pArea->color	= AT_AREA;
    pArea->exitsize	= SIZE_MEDIUM;
    pArea->continent	= LAND_NONE;
    pArea->world	= WORLD_ALL;
    pArea->empty	= FALSE;

    area_version	= 0;

    for ( ; ; )
    {
	word = feof( fp ) ? "End" : fread_word( fp );
	fMatch = 0;

	switch( UPPER( word[0] ) )
	{
	    case 'B':
		SKEY( "Builders", pArea->builders );
		break;

	    case 'C':
		KEY( "Color", pArea->color, fread_number( fp ) );
		KEY( "Continent", pArea->continent, flag_value( continent_types, fread_word( fp ) ) );
		KEY( "Created", pArea->created, fread_number( fp ) );
		SKEY( "Credits", pArea->credits );
		if ( !str_cmp( word, "Changed" ) )
		{
		    fMatch = TRUE;
		    pArea->last_changed = fread_number( fp );
		    fread_to_eol( fp );
		    break;
		}
		break;

	    case 'E':
		if ( !str_cmp( word, "End" ) )
		{
		    fMatch = TRUE;
		    if ( pArea->min_vnum + pArea->maxx * pArea->maxy > pArea->max_vnum + 1 )
		    {
			bugf( "Load_areadata: overland area too big." );
			abort_mud( "Bad area data" );
		    }
		    area_sort( pArea );
		    fix_header( pArea );
		    top_area++;
		    if ( pArea->maxx != 0 && pArea->maxy != 0 )
			init_area_overland( pArea );
		    return;
		}
		if ( !str_cmp( word, "ExitSize" ) )
		{
		    int value;

		    word = fread_word( fp );
		    if ( ( value = flag_value( size_types, word ) ) == NO_VALUE )
		        value = SIZE_MEDIUM;
                    pArea->exitsize = value;
                    fMatch = TRUE;
                    break;
		}
		break;

	    case 'F':
		KEY( "Flags", pArea->area_flags, fread_flag( fp ) );
		break;

	    case 'K':
	        KEY( "Kingdom", pArea->kingdom, flag_value( kingdom_types, fread_word( fp ) ) );
		break;

	    case 'L':
		if ( !str_cmp( word, "Levels" ) )
		{
		    pArea->low_level = fread_number( fp );
		    pArea->high_level = fread_number( fp );
		    break;
		}
		break;

	    case 'N':
		SKEY( "Name", pArea->name );
		SKEY( "Norecall", pArea->norecall );
		break;

	    case 'O':
		if ( !str_cmp( word, "Overland" ) )
		{
		    pArea->maxx = fread_number( fp );
		    pArea->maxy = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'R':
		KEY( "Recall", pArea->recall, fread_number( fp ) );
		SKEY( "Resetmsg", pArea->resetmsg );
		break;

	    case 'S':
		KEY( "Security", pArea->security, fread_number( fp ) );
		break;

	    case 'V':
		KEY( "Version", area_version, fread_number( fp ) );
		if ( !str_cmp( word, "VNUMs" ) )
		{
		    pArea->min_vnum = fread_number( fp );
		    pArea->max_vnum = fread_number( fp );
		    fMatch = TRUE;
		    break;
		}

	    case 'W':
		KEY( "World", pArea->world, flag_value( world_types, fread_word( fp ) ) );
		break;

	    default:
		fread_to_eol( fp );
		break;
	}
    }

}


/*
 * Fix up the area information from old_style headers
 */
static void fix_header( AREA_DATA *pArea )
{
    char auth[SHORT_STRING_LENGTH];
    char buf [SHORT_STRING_LENGTH];
    char lvls[SHORT_STRING_LENGTH];
    char *p;
    char *q;

    if ( !( p = strchr( pArea->credits, '{' ) ) )
	return;
    p++;
    q = lvls;
    while ( *p != '\0' && *p != '}' )
	*q++ = *p++;
    *q = '\0';

    q = one_argument( lvls, buf );
    if ( is_number( buf ) )
    {
	pArea->low_level = atoi( buf );
	pArea->high_level = atoi( q );
	pArea->high_level = UMAX( pArea->low_level, pArea->high_level );
    }
    else
    {
	if ( !str_cmp( buf, "all" ) )
	{
	    pArea->low_level = 1;
	    pArea->high_level = LEVEL_HERO;
	}
	else
	{
	    pArea->low_level = 0;
	    pArea->high_level = 0;
	}
    }

    p++;
    while( isspace( *p ) )
	p++;
    p = first_arg( p, auth, TRUE );
    while ( isspace( *p ) )
	p++;
    free_string( pArea->credits );
    pArea->credits = str_dup( auth );
    if ( *p != '\0' )
    {
	free_string( pArea->name );
	pArea->name = str_dup( p );
    }

}


void
insert_dream( DREAM_DATA *pDream )
{
    DREAM_DATA *	pTmp;

    if ( dream_list == NULL )
    {
        dream_list = pDream;
        dream_last = pDream;
    }
    else if ( pDream->vnum > dream_last->vnum )
    {
        dream_last->next = pDream;
        dream_last = pDream;
        pDream->next = NULL;
    }
    else
    {
        for ( pTmp = dream_list; pTmp != NULL; pTmp = pTmp->next )
        {
            if ( pDream->vnum < pTmp->next->vnum )
            {
                pDream->next = pTmp->next;
                pTmp->next = pDream;
                break;
            }
        }
    }

}


void
load_badnames( void )
{
    char	buf[MAX_STRING_LENGTH*2];
    FILE *	fp;
    char	letter;
    char *	p;
    char *	word;

    badname_list = &str_empty[0];

    strcpy( buf, " " );
    p = buf;

    if ( ( fp = fopen( SYSTEM_DIR BADNAME_FILE, "r" ) ) == NULL )
	return;

    for ( ; ; )
    {
	letter = fread_letter( fp );
	if ( letter == '~' || feof( fp ) )
	{
	    fclose( fp );
	    badname_list = str_dup( &buf[1] );
	    return;
	}

	ungetc( letter, fp );
	word = fread_word( fp );
	p += sprintf( p, " %s", word );
    }
}


/*
 * Load the notes for one board
 */
static void
load_board( BOARD_DATA *pBoard )
{
    FILE *	fp;
    char	filename[SHORT_STRING_LENGTH];
    char	pathname[MAX_INPUT_LENGTH];
    char	letter;
    char *	p;
    char *	q;
    char *	word;
    time_t	expire;

    p = pBoard->name;
    q = filename;
    while ( *p != '\0' )
    {
	*q++ = LOWER( *p );
	p++;
    }
    *q = '\0';

    if ( pBoard->expire > 0 )
	expire = current_time - pBoard->expire * 60 * 60 * 24;
    else
	expire = 0;

    sprintf( pathname, "%s%s", BOARDS_DIR, filename );
    if ( ( fp = fopen( pathname, "r" ) ) == NULL )
    {
//	bug( "Load_board: notes file %s not found.", filename );
	return;
    }

    for ( ; ; )
    {
	letter = fread_letter( fp );
	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_board: '#' not found.", 0 );
	    break;
	}

	word = fread_word( fp );
	if ( !str_cmp( word, "$" ) )
	    break;
	else if ( !str_cmp( word, "NOTE" ) )
	    fread_note( fp, pBoard, expire );
	else
	{
	    bugf( "Load_board: bad section '%s'", word );
	    fread_to_eol( fp );
	    continue;
	}
    }

    fclose( fp );
    return;
}


/*
 * Load a single note
 */
static void
fread_note( FILE *fp, BOARD_DATA *pBoard, time_t expire )
{
    NOTE_DATA *	pNote;
    char *	word;
    bool	fMatch;

    pNote = new_note( );
    pNote->board = pBoard;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER( word[0] ) )
	{
	    case '*':
		fMatch = TRUE;
		fread_to_eol( fp );
		break;

	    case 'D':
		KEY( "Date",	pNote->date,	fread_string( fp ) );
		break;

	    case 'E':
		if ( !str_cmp( word, "End" ) )
		{
		    if ( pNote->date_stamp < expire
		    &&	 !IS_SET( pNote->flags, NOTE_PROTECTED ) )
		    {
			free_note( pNote );
		    }
		    else
		    {
			if ( pBoard->note_last == NULL )
			    pBoard->note_first = pNote;
			else
			    pBoard->note_last->next = pNote;
			pBoard->note_last = pNote;
		    }

		    return;
		}

		break;

	    case 'F':
		KEY( "Flags",	pNote->flags,	fread_flag( fp ) );
		break;

	    case 'S':
		KEY( "Sender",	pNote->sender,	fread_string( fp ) );
		KEY( "Stamp",	pNote->date_stamp, fread_number( fp ) );
		KEY( "Subject",	pNote->subject,	fread_string( fp ) );
		break;

	    case 'T':
		KEY( "Text",	pNote->text,	fread_string( fp ) );
		KEY( "To",	pNote->to_list,	fread_string( fp ) );
		break;

	}

	if ( !fMatch )
	{
	    bugf( "Fread_note: no match: %s", word );
	    fread_to_eol( fp );
	}
    }
}


/*
 * Load the note board list
 */
void
load_boards( void )
{
    FILE *	fp;
    char	letter;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR BOARDS_FILE, "r" ) ) == NULL )
    {
	bug( "Boards file not found.", 0 );
	return;
    }

    for ( ; ; )
    {
	letter = fread_letter( fp );
	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_boards: '#' not found.", 0 );
	    break;
	}

	word = fread_word( fp );
	if ( !str_cmp( word, "$" ) )
	    break;
	else if ( !str_cmp( word, "BOARD" ) )
	    fread_board( fp );
	else
	{
	    bugf( "Load_boards: bad section '%s'", word );
	    fread_to_eol( fp );
	    continue;
	}
    }

    fclose( fp );
    return;
}


static void
fread_board( FILE * fp )
{
    BOARD_DATA *	pBoard;
    char *		word;

    pBoard = new_board( );
    pBoard->name = str_dup( fread_word( fp ) );

    for ( ; ; )
    {
	word = feof( fp ) ? "End" : fread_word( fp );
	if ( !str_cmp( word, "End" ) )
	    break;
	else if ( !str_cmp( word, "Desc" ) )
	    pBoard->desc = fread_string( fp );
	else if ( !str_cmp( word, "Expire" ) )
	    pBoard->expire = fread_number( fp );
	else if ( !str_cmp( word, "Read" ) )
	    pBoard->read = fread_number( fp );
	else if ( !str_cmp( word, "Write" ) )
	    pBoard->write = fread_number( fp );
	else
	{
	    bugf( "Fread_board: no match: %s", word );
	    fread_to_eol( fp );
	}
    }

    if ( board_first == NULL )
	board_first = pBoard;
    else
	board_last->next = pBoard;
    board_last = pBoard;

    load_board( pBoard );

    return;
}


/*
 * Load the command and security tables
 */
void
load_commands( void )
{
    FILE *	fp;
    char	letter;
    int		sec;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR COMMAND_FILE, "r" ) ) == NULL )
    {
	bug( "Commands file not found.", 0 );
	return;
    }

    for ( ; ; )
    {
	letter = fread_letter( fp );
	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_commands: '#' not found.", 0 );
	    break;
	}

	word = fread_word( fp );
	if ( !str_cmp( word, "$" ) )
	    break;
	else if ( !str_cmp( word, "CMD" ) )
	    fread_command( fp );
	else
	{
	    bugf( "Load_commands: bad section '%s'", word );
	    fread_to_eol( fp );
	    continue;
	}

    }

    fclose( fp );

    if ( ( fp = fopen( SYSTEM_DIR SECGROUP_FILE, "r" ) ) == NULL )
    {
	bug( "Security groups file not found.", 0 );
	return;
    }

    sec = 0;
    for ( ; ; )
    {
	word = fread_word( fp );
	if ( !str_cmp( word, "#END" ) )
	    break;
	security_flags[sec].name     = str_dup( word );
	security_flags[sec].bit      = fread_flag( fp );
	security_flags[sec].settable = TRUE;
	sec++;
    }

    fclose( fp );
    return;
}


static void
fread_command( FILE *fp )
{
    int		iCmd;
    int		letter;
    char *	word;

    word = fread_word( fp );
    if ( ( iCmd = get_cmd_by_name( word ) ) == NO_COMMAND )
    {
	bugf( "Fread_command: bad command name: %s", word );
	do
	{
	    letter = getc( fp );
	}
	while ( letter != EOF && letter != '#' );
	ungetc( letter, fp );
	return;
    }

    for ( ; ; )
    {
	word = feof( fp ) ? "End" : fread_word( fp );
	if ( !str_cmp( word, "End" ) )
	    break;
	else if ( !str_cmp( word, "Pos" ) )
	{
	    cmd_table[iCmd].position = position_lookup( fread_word( fp ) );
	}
	else if ( !str_cmp( word, "Lvl" ) )
	{
	    cmd_table[iCmd].level = level_lookup( fread_word( fp ) );
	}
	else if ( !str_cmp( word, "Log" ) )
	{
	    cmd_table[iCmd].log = flag_value( log_types, fread_word( fp ) );
	}
	else if ( !str_cmp( word, "Flags" ) )
	{
	    /* Temporary hack to keep the new CMD_DEAD & CMD_UNHIDE flags */
	    bitvector old_flags;
	    old_flags = cmd_table[iCmd].flags & (CMD_DEAD|CMD_UNHIDE);
	    cmd_table[iCmd].flags = fread_flag( fp );
	    cmd_table[iCmd].flags |= old_flags;
	}
	else if ( !str_cmp( word, "Sec" ) )
	{
	    cmd_table[iCmd].sec_flags = fread_flag( fp );
	}
	else
	{
	    bugf( "Fread_command: no match: %s", word );
	    fread_to_eol( fp );
	}

    }

    return;
}


/*
 * Load the dream file.
 */
void
load_dreams( void )
{
    FILE *		fp;
    DREAM_DATA *	pDream;
    DREAMSEG_DATA *	pSeg;
    DREAMSEG_DATA *	pLast;
    AFFECT_DATA *	paf;
    AFFECT_DATA *	paf_last;
    char		letter;
    char *		word;
    int			vnum;

    if ( ( fp = fopen( SYSTEM_DIR DREAM_FILE, "r" ) ) == NULL )
    {
        bugf( "Dream file not found." );
        return;
    }

    for ( ; ; )
    {
        letter = fread_letter( fp );
        if ( letter == '*' )
        {
            fread_to_eol( fp );
            continue;
        }
        if ( letter != '#' )
        {
            bugf( "Load_dreams: '#' not found." );
            break;
        }

        word = fread_word( fp );
        if ( !is_number( word ) )
        {
            if ( !str_cmp( word, "$" ) )
                break;
            bugf( "Fread_dreams: bad section %s", word );
            fread_to_eol( fp );
            continue;
        }
        if ( ( vnum = atoi( word) ) == 0 )
            break;

        pDream = new_dream( );
        pDream->vnum = vnum;
        pLast = NULL;
        paf_last = NULL;
        for ( ; ; )
        {
            word = feof( fp ) ? "End" : fread_word( fp );

            if ( !str_cmp( word, "End" ) )
            {
                insert_dream( pDream );
                break;
            }
            else if ( !str_cmp( word, "Aff" ) )
            {
                paf = new_affect( );
                paf->where	= TO_AFFECTS;
                paf->type	= gsn_dream;
                paf->level	= LEVEL_HERO;
                paf->bitvector	= AFF_DREAM;
                paf->location	= fread_number( fp );
                paf->modifier	= fread_number( fp );
                paf->duration	= fread_number( fp );
                if ( paf_last == NULL )
                    pDream->affect = paf;
                else
                    paf_last->next = paf;
                paf_last = paf;
                continue;
            }
            else if ( !str_cmp( word, "Author" ) )
            {
                pDream->author = str_dup( fread_word( fp ) );
                continue;
            }
            else if ( !str_cmp( word, "Class" ) )
            {
                pDream->class_flags = fread_xbits( fp );
                continue;
            }
            else if ( !str_cmp( word, "Flags" ) )
            {
                pDream->dream_flags = fread_flag( fp );
                continue;
            }
            else if ( !str_cmp( word, "Race" ) )
            {
                pDream->race_flags = fread_xbits( fp );
                continue;
            }
            else if ( !str_cmp( word, "Seg" ) )
            {
                pSeg = new_dream_seg( );
                pSeg->text = fread_string( fp );
                if ( pDream->seg == NULL )
                {
                    pDream->seg = pSeg;
                }
                else
                {
                    pLast->next = pSeg;
                    pLast = pSeg;
                }
                pLast = pSeg;
                continue;
            }
            else if ( !str_cmp( word, "Sex" ) )
            {
                pDream->sex = sex_lookup( fread_word( fp ) );
                continue;
            }
            else if ( !str_cmp( word, "Title" ) )
            {
                pDream->title = fread_string( fp );
                continue;
            }
            else if ( !str_cmp( word, "Type" ) )
            {
                pDream->type = flag_value( dream_types, fread_word( fp ) );
                continue;
            }
            else
            {
                bugf( "Fread_dreams: bad keyword %s", word );
                fread_to_eol( fp );
                continue;
            }
        }
    }

    fclose( fp );
    return;
}


/*
 * Load the greet table, database of who knows whom.
 */
void
load_greet_data( void )
{
    FILE *	fp;
    int		c;
    int		count;
    int		greet_row;
    bool *	p;

    count = 0;
    if ( ( fp = fopen( SYSTEM_DIR GREET_FILE, "r" ) ) != NULL )
    {
	while ( ( c = fgetc( fp ) ) != EOF && c != '\n' )
	    count++;
	rewind( fp );
    }

    greet_size = UMAX( count, top_userlist + GREET_INCREMENT );
    greet_table = (bool *)malloc( greet_size * greet_size * sizeof( bool ) );
    memset( greet_table, FALSE, greet_size * greet_size );

    if ( fp != NULL )
    {
	greet_row = 0;
	p = greet_table;

	while ( ( c = fgetc( fp ) ) != EOF )
	{
	    switch( c )
	    {
		default:	break;
		case '1':	*p++ = TRUE;	break;
		case '0':	*p++ = FALSE;	break;
		case '\n':
		    greet_row++;
		    p = greet_table + greet_row * greet_size;
		    break;
	    }
	    if ( greet_row >= greet_size )
	        break;
	}

	fclose( fp );
    }

    return;
}


/*
 * Snarf a mob section.  new style
 */
void
load_mobiles( FILE *fp )
{
    MOB_INDEX_DATA *	pMobIndex;
    int			value;

    for ( ; ; )
    {
        int vnum;
        char letter;
        int iHash;

        letter                          = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_mobiles: # not found.", 0 );
            exit( 1 );
        }

        vnum                            = fread_number( fp );
        if ( vnum == 0 )
            break;
	if ( vnum > top_vnum_mob )
	    top_vnum_mob = vnum; 

        fBootDb = FALSE;
        if ( get_mob_index( vnum ) != NULL )
        {
            bug( "Load_mobiles: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;

        pMobIndex                       = alloc_perm( sizeof(*pMobIndex) );
	pMobIndex->area			= area_last;
        pMobIndex->vnum                 = vnum;
	newmobs++;
        pMobIndex->player_name          = fread_string( fp );
        pMobIndex->short_descr          = fread_string( fp );
        pMobIndex->long_descr           = fread_string( fp );
        pMobIndex->description          = fread_string( fp );
	pMobIndex->race		 	= race_lookup(fread_string( fp ));

        pMobIndex->long_descr[0]        = UPPER(pMobIndex->long_descr[0]);
        pMobIndex->description[0]       = UPPER(pMobIndex->description[0]);

        pMobIndex->act                  = fread_flag( fp ) | ACT_IS_NPC
					| race_table[pMobIndex->race].act;
        pMobIndex->affected_by          = fread_xbits( fp );
        pMobIndex->shielded_by	= fread_xbits( fp );
	xSET_BITS( pMobIndex->affected_by, race_table[pMobIndex->race].aff );
	pMobIndex->class		= fread_flag( fp );
        pMobIndex->pShop                = NULL;
        pMobIndex->alignment            = fread_number( fp );
        pMobIndex->ethos            = fread_number( fp );
        pMobIndex->group                = fread_number( fp );

        pMobIndex->level                = fread_number( fp );
        pMobIndex->hitroll              = fread_number( fp );  

	/* read hit dice */
        pMobIndex->hit[DICE_NUMBER]     = fread_number( fp );  
        /* 'd'          */                fread_letter( fp ); 
        pMobIndex->hit[DICE_TYPE]   	= fread_number( fp );
        /* '+'          */                fread_letter( fp );   
        pMobIndex->hit[DICE_BONUS]      = fread_number( fp ); 

 	/* read mana dice */
	pMobIndex->mana[DICE_NUMBER]	= fread_number( fp );
					  fread_letter( fp );
	pMobIndex->mana[DICE_TYPE]	= fread_number( fp );
					  fread_letter( fp );
	pMobIndex->mana[DICE_BONUS]	= fread_number( fp );

	/* read damage dice */
	pMobIndex->damage[DICE_NUMBER]	= fread_number( fp );
					  fread_letter( fp );
	pMobIndex->damage[DICE_TYPE]	= fread_number( fp );
					  fread_letter( fp );
	pMobIndex->damage[DICE_BONUS]	= fread_number( fp );
	pMobIndex->dam_type		= attack_lookup(fread_word(fp));

	/* read armor class */
	pMobIndex->ac[AC_PIERCE]	= fread_number( fp ) * 10;
	pMobIndex->ac[AC_BASH]		= fread_number( fp ) * 10;
	pMobIndex->ac[AC_SLASH]		= fread_number( fp ) * 10;
	pMobIndex->ac[AC_EXOTIC]	= fread_number( fp ) * 10;

	/* read flags and add in data from the race table */
	pMobIndex->off_flags		= fread_xbits( fp );
	xSET_BITS( pMobIndex->off_flags, race_table[pMobIndex->race].off );
	pMobIndex->imm_flags		= fread_flag( fp )
					| race_table[pMobIndex->race].imm;
	pMobIndex->res_flags		= fread_flag( fp )
					| race_table[pMobIndex->race].res;
	pMobIndex->vuln_flags		= fread_flag( fp )
					| race_table[pMobIndex->race].vuln;

	/* vital statistics */
	value				= position_lookup( fread_word( fp ) );
	pMobIndex->start_pos		= value == NO_POSITION ? POS_STANDING : value;
	value				= position_lookup( fread_word( fp ) );
	pMobIndex->default_pos		= value == NO_POSITION ? POS_STANDING : value;
	pMobIndex->sex			= sex_lookup(fread_word(fp));

	pMobIndex->wealth		= fread_number( fp );

	pMobIndex->form			= fread_flag( fp )
					| race_table[pMobIndex->race].form;
	pMobIndex->parts		= fread_flag( fp )
					| race_table[pMobIndex->race].parts;
	/* size */
	pMobIndex->size			= size_lookup(fread_word(fp));
	pMobIndex->weight		= fread_number( fp );
	pMobIndex->autoset		= flag_value( automob_types, fread_word( fp ) );
	pMobIndex->material		= str_dup(fread_word( fp ));

	pMobIndex->clan			= &str_empty[0];

	for ( ; ; )
        {
            letter = fread_letter( fp );

            if ( letter == 'C' )
            {
                pMobIndex->clan = str_dup( fread_word( fp ) );
            }
            else if ( letter == 'F' )
            {
		char *word;
		long vector;

                word                    = fread_word(fp);
		vector			= fread_flag(fp);

		if ( !str_prefix( word, "act" ) )
		    REMOVE_BIT( pMobIndex->act, vector );
                else if ( !str_prefix( word, "aff" ) )
		    xREMOVE_BIT( pMobIndex->affected_by, vector );
		else if ( !str_prefix( word,"off" ) )
		    xREMOVE_BIT( pMobIndex->off_flags, vector );
		else if (!str_prefix(word,"imm"))
		    REMOVE_BIT(pMobIndex->imm_flags,vector);
		else if (!str_prefix(word,"res"))
		    REMOVE_BIT(pMobIndex->res_flags,vector);
		else if (!str_prefix(word,"vul"))
		    REMOVE_BIT(pMobIndex->vuln_flags,vector);
		else if (!str_prefix(word,"for"))
		    REMOVE_BIT(pMobIndex->form,vector);
		else if (!str_prefix(word,"par"))
		    REMOVE_BIT(pMobIndex->parts,vector);
		else
		{
		    bug("Flag remove: flag not found.",0);
		    exit(1);
		}
	     }
	     else if ( letter == 'T' )
	     {
		char *	word;
		int	iClass;

		word = fread_word( fp );
		if ( ( iClass = class_lookup( word ) ) == NO_CLASS )
		{
		    bugf( "Load_mobiles: class '%s' not found.", word );
		    continue;
                }
		if ( !IS_SET( pMobIndex->act, ACT_TRAIN )
		&&   !IS_SET( pMobIndex->act, ACT_GAIN )
		&&   !IS_SET( pMobIndex->act, ACT_PRACTICE ) )
		{
		    bugf( "Load_mobiles: train flag on invalid mobile" );
		    continue;
                }
                xSET_BIT( pMobIndex->train, iClass );
	     }
	     else
	     {
		ungetc( letter, fp );
		break;
	     }
	}

	letter = fread_letter( fp );
	ungetc( letter, fp );
	if ( letter == '>' )
	    mprog_read_programs( fp, pMobIndex );

        iHash                   = vnum % MAX_KEY_HASH;
        pMobIndex->next         = mob_index_hash[iHash];
        mob_index_hash[iHash]   = pMobIndex;
        top_mob_index++;
        kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;

    }

    return;
}


/*
 * Load "most players" file
 */
void
load_most_players( void )
{
    FILE *fp;

    if ( ( fp = fopen( SYSTEM_DIR MAX_PLAYERS, "r" ) ) != NULL )
    {
	today   =	 fread_number( fp );
	most_on =	 fread_number( fp );
	most_yesterday = fread_number( fp );
	fclose( fp );
    }
}


/*
 * Snarf an obj section. new style
 */
void load_objects( FILE *fp )
{
    OBJ_INDEX_DATA *	pObjIndex;
    TEXT_DATA *		page;
    TEXT_DATA *		page_last;
    EXTRA_DESCR_DATA *	ed_last;
    AFFECT_DATA *	paf_last;
    char		mat_buf[MAX_STRING_LENGTH];
    char		mat_word[MAX_INPUT_LENGTH];
    int			value;
    char *		p;

    for ( ; ; )
    {
        int vnum;
        char letter;
        int iHash;

        page_last = NULL;
        letter                          = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_objects: # not found.", 0 );
            exit( 1 );
        }

        ed_last = NULL;
        vnum                            = fread_number( fp );
        if ( vnum == 0 )
            break;
	if ( vnum > top_vnum_obj )
	    top_vnum_obj = vnum;

        fBootDb = FALSE;
        if ( get_obj_index( vnum ) != NULL )
        {
            bug( "Load_objects: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;

        pObjIndex                       = alloc_perm( sizeof(*pObjIndex) );
        pObjIndex->vnum                 = vnum;
	pObjIndex->area			= area_last;
	pObjIndex->reset_num		= 0;
	newobjs++;
        pObjIndex->name                 = fread_string( fp );
        pObjIndex->short_descr          = fread_string( fp );
        pObjIndex->description          = fread_string( fp );

	xCLEAR_BITS( pObjIndex->material );
	p = fread_buf( fp, mat_buf, sizeof( mat_buf ) );
	while ( *p != '\0' )
	{
	    p = one_argument( p, mat_word );
	    if ( ( value = flag_value( material_types, mat_word ) ) != NO_FLAG )
		xSET_BIT( pObjIndex->material, value );
	}

        pObjIndex->item_type            = item_lookup(fread_word( fp ));
	if ( pObjIndex->item_type == NO_FLAG )
	    pObjIndex->item_type = ITEM_TRASH;
        pObjIndex->extra_flags          = fread_flag( fp );
		pObjIndex->extra_flags2          = fread_flag( fp );
        pObjIndex->wear_flags           = fread_flag( fp );
	switch(pObjIndex->item_type)
	{
	case ITEM_WEAPON:
	    pObjIndex->value[0]		= weapon_type(fread_word(fp));
	    pObjIndex->value[1]		= fread_number(fp);
	    pObjIndex->value[2]		= fread_number(fp);
	    pObjIndex->value[3]		= attack_lookup(fread_word(fp));
	    pObjIndex->value[4]		= fread_flag(fp);
	    pObjIndex->value[5]		= fread_number( fp );
	    break;
	case ITEM_CONTAINER:
	    pObjIndex->value[0]		= fread_number(fp);
	    pObjIndex->value[1]		= fread_flag(fp);
	    pObjIndex->value[2]		= fread_number(fp);
	    pObjIndex->value[3]		= fread_number(fp);
	    pObjIndex->value[4]		= fread_number(fp);
	    pObjIndex->value[5]		= fread_number( fp );
	    break;
        case ITEM_DRINK_CON:
	case ITEM_FOUNTAIN:
            pObjIndex->value[0]         = fread_number(fp);
            pObjIndex->value[1]         = fread_number(fp);
            pObjIndex->value[2]         = liq_lookup(fread_word(fp));
            pObjIndex->value[3]         = fread_number(fp);
            pObjIndex->value[4]         = fread_number(fp);
	    pObjIndex->value[5]		= fread_number( fp );
            break;
	case ITEM_LENS:
	case ITEM_WAND:
	case ITEM_STAFF:
	    pObjIndex->value[0]		= fread_number(fp);
	    pObjIndex->value[1]		= fread_number(fp);
	    pObjIndex->value[2]		= fread_number(fp);
	    pObjIndex->value[3]		= skill_lookup(fread_word(fp));
	    pObjIndex->value[4]		= fread_number(fp);
	    pObjIndex->value[5]		= fread_number( fp );
	    break;
	case ITEM_POTION:
	case ITEM_PILL:
	case ITEM_SCROLL:
 	    pObjIndex->value[0]		= fread_number(fp);
	    pObjIndex->value[1]		= skill_lookup(fread_word(fp));
	    pObjIndex->value[2]		= skill_lookup(fread_word(fp));
	    pObjIndex->value[3]		= skill_lookup(fread_word(fp));
	    pObjIndex->value[4]		= skill_lookup(fread_word(fp));
	    pObjIndex->value[5]		= fread_number( fp );
	    break;
	case ITEM_HERB:
 	    pObjIndex->value[0]		= fread_number(fp);
	    pObjIndex->value[1]		= skill_lookup(fread_word(fp));
	    pObjIndex->value[2]		= skill_lookup(fread_word(fp));
	    pObjIndex->value[3]		= skill_lookup(fread_word(fp));
	    pObjIndex->value[4]		= fread_number( fp );
	    pObjIndex->value[5]		= fread_flag( fp );
	    break;
	case ITEM_FURNITURE:
            pObjIndex->value[0]		= fread_number( fp );
            pObjIndex->value[1]		= fread_number( fp );
            pObjIndex->value[2]		= fread_flag( fp );
            pObjIndex->value[3]		= fread_number( fp );
	    pObjIndex->value[4]		= fread_number( fp );
	    pObjIndex->value[5]		= fread_number( fp );
            if ( pObjIndex->value[3] != 0 && pObjIndex->value[4] == 0 )
                pObjIndex->value[4] = pObjIndex->value[3];
	    break;
	     case ITEM_TOKEN:
            pObjIndex->value[0] = fread_number(fp);
            break;
	case ITEM_ARROW:
	case ITEM_QUIVER:
            pObjIndex->value[0]		= fread_number( fp );
            pObjIndex->value[1]		= fread_number( fp );
            pObjIndex->value[2]		= fread_flag( fp );
            pObjIndex->value[3]		= fread_number( fp );
	    pObjIndex->value[4]		= fread_number( fp );
	    pObjIndex->value[5]		= fread_number( fp );
            if ( pObjIndex->value[3] != 0 && pObjIndex->value[4] == 0 )
                pObjIndex->value[4] = pObjIndex->value[3];
	    break;
	case ITEM_SHEATH:
	    pObjIndex->value[0]		= fread_number( fp );
	    pObjIndex->value[1]		= fread_flag( fp );
	    pObjIndex->value[2]		= fread_number( fp );
	    pObjIndex->value[3]		= weapon_type( fread_word( fp ) );
	    pObjIndex->value[4]		= weapon_type( fread_word( fp ) );
	    pObjIndex->value[5]		= weapon_type( fread_word( fp ) );
	    break;
	default:
            pObjIndex->value[0]		= fread_flag( fp );
            pObjIndex->value[1]		= fread_flag( fp );
            pObjIndex->value[2]		= fread_flag( fp );
            pObjIndex->value[3]		= fread_flag( fp );
	    pObjIndex->value[4]		= fread_flag( fp );
	    pObjIndex->value[5]		= fread_number( fp );
	    break;
	}
	pObjIndex->level		= fread_number( fp );
        pObjIndex->weight               = fread_number( fp );
        pObjIndex->cost                 = fread_number( fp ); 
		pObjIndex->qcost                 = fread_number( fp ); 

        /* condition */
        letter 				= fread_letter( fp );
	switch (letter)
 	{
	    case ('P') :		pObjIndex->condition = 100; break;
	    case ('G') :		pObjIndex->condition =  90; break;
	    case ('A') :		pObjIndex->condition =  75; break;
	    case ('W') :		pObjIndex->condition =  50; break;
	    case ('D') :		pObjIndex->condition =  25; break;
	    case ('B') :		pObjIndex->condition =  10; break;
	    case ('R') :		pObjIndex->condition =   0; break;
	    default:			pObjIndex->condition = 100; break;
	}

	pObjIndex->ac_type		= fread_number( fp );
	pObjIndex->ac_vnum		= pObjIndex->ac_type == INVOKE_SPELL ?
					  skill_lookup( fread_word( fp ) ) :
					  fread_number( fp );
	pObjIndex->ac_charge[0]		= fread_number( fp );
	pObjIndex->ac_charge[1]		= fread_number( fp );
	pObjIndex->join			= fread_number( fp );
	pObjIndex->sep_one		= fread_number( fp );
	pObjIndex->sep_two		= fread_number( fp );
	pObjIndex->timer		= fread_number( fp );
	if ( area_version > 8 )
	    pObjIndex->duration		= fread_number( fp );

        paf_last = NULL;
        for ( ; ; )
        {
            char letter;

            letter = fread_letter( fp );

            if ( letter == 'A' )
            {
                AFFECT_DATA *paf;

                paf                     = alloc_perm( sizeof(*paf) );
		paf->where		= TO_OBJECT;
                paf->type               = -1;
                paf->level              = pObjIndex->level;
                paf->duration           = -1;
                paf->location           = fread_number( fp );
                paf->modifier           = fread_number( fp );
                paf->bitvector          = 0;
                paf->next               = NULL;
                if ( paf_last == NULL )
                    pObjIndex->affected = paf;
                else
                    paf_last->next	= paf;
                paf_last		= paf;
                top_affect++;
            }

	    else if (letter == 'F')
            {
                AFFECT_DATA *paf;

                paf                     = alloc_perm( sizeof(*paf) );
		letter 			= fread_letter(fp);
		switch (letter)
	 	{
		case 'A':
                    paf->where          = TO_AFFECTS;
		    break;
		case 'I':
		    paf->where		= TO_IMMUNE;
		    break;
		case 'R':
		    paf->where		= TO_RESIST;
		    break;
		case 'S':
		    paf->where		= TO_SHIELDS;
		    break;
		case 'V':
		    paf->where		= TO_VULN;
		    break;
		default:
            	    bug( "Load_objects: Bad where on flag set.", 0 );
            	   exit( 1 );
		}
                paf->type               = -1;
                paf->level              = pObjIndex->level;
                paf->duration           = -1;
                paf->location           = fread_number(fp);
                paf->modifier           = fread_number(fp);
                paf->bitvector          = fread_flag(fp);
                paf->next               = pObjIndex->affected;
                pObjIndex->affected     = paf;
                top_affect++;
            }

            else if ( letter == 'E' )
            {
                EXTRA_DESCR_DATA *ed;

                ed                      = alloc_perm( sizeof(*ed) );
                ed->keyword             = fread_string( fp );
                ed->description         = fread_string( fp );
                ed->next                = NULL;
                if ( ed_last == NULL )
                    pObjIndex->extra_descr  = ed;
                else
                    ed_last->next = ed;
                ed_last = ed;
                top_ed++;
            }

	    else if ( letter == 'P' )
	    {
		page = new_textlist( );
		page->text = fread_string( fp );
		if ( page_last == NULL )
		    pObjIndex->page = page;
		else
		    page_last->next = page;
		page_last = page;
	    }

	    else if ( letter == 'R' )
	    {
		char *	word;
		int	bit;
		word = fread_word( fp );
		if ( ( bit = race_lookup( word ) ) != 0 )
		    xSET_BIT( pObjIndex->race_flags, bit );
                else if ( ( bit = class_lookup( word ) ) != NO_CLASS )
                    xSET_BIT( pObjIndex->class_flags, bit );
                else
                    bugf( "Load_obj: bad race/class '%s'", word );
	    }
	    else if ( letter == '>' )
	    {
		ungetc( letter, fp );
		pObjIndex->progtypes = load_oreprogs( fp, &pObjIndex->oprogs, PTYPE_OBJ );
//		break;
	    }
	    else
            {
                ungetc( letter, fp );
                break;
            }
        }

        iHash                   = vnum % MAX_KEY_HASH;
        pObjIndex->next         = obj_index_hash[iHash];
        obj_index_hash[iHash]   = pObjIndex;
        top_obj_index++;
    }

    return;
}


bitvector load_oreprogs( FILE *fp, OREPROG_DATA **proglist, int prog_type )
{
    OREPROG_DATA *pProg;
    OREPROG_DATA *pLast;
    const char *  word;
    bitvector	  alltypes;
    bitvector	  type;
    char	  letter;
    bool	  done = FALSE;

    if ( ( letter = fread_letter( fp ) ) != '>' )
    {
	bug( "Load_oreprogs:  No progs", 0 );
	exit( 1 );
    }

    pProg = new_oreprog_data( );
    *proglist = pProg;
    pLast = pProg;
    alltypes = 0;

    while ( !done )
    {
	word = fread_word( fp );
	switch( prog_type )
	{
	    case PTYPE_EXIT: type = flag_value( eprog_types, word ); break;
	    case PTYPE_OBJ:  type = flag_value( oprog_types, word ); break;
	    case PTYPE_ROOM: type = flag_value( rprog_types, word ); break;
	    default:
		bug( "Load_oreprogs: bad prog_type %d", prog_type );
		exit( 1 );
	}
	if ( type == NO_FLAG )
	{
	    bugf( "Load_oreprogs: bad program type %s", word );
	    exit( 1 );
	}
	pProg->type = type;
	alltypes |= type;
	pProg->arglist = fread_string( fp );
	fread_to_eol( fp );
	pProg->comlist = fread_string( fp );
	fread_to_eol( fp );

	switch ( letter = fread_letter( fp ) )
	{
	    case '>':
		pProg = new_oreprog_data( );
		pLast->next = pProg;
		pLast = pProg;
		break;
	    case '|':
		pLast->next = NULL;
		fread_to_eol( fp );
		done = TRUE;
		break;
	    default:
		bug( "Load_oreprogs:  letter not '>' or '|'", 0 );
		break;
	}
    }
    return alltypes;
}


/*
 * Load resets for an area
 */
void load_resets( FILE *fp )
{
    RESET_DATA *	pReset;
    EXIT_DATA *		pExit;
    ROOM_INDEX_DATA *	pRoomIndex;
    int			iLastRoom = 0;
    int			iLastObj  = 0;
    char		letter;

    if ( !area_last )
	abort_mud( "Load_resets: no #AREA seen yet." );

    for ( ; ; )
    {
	if ( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	pReset = (RESET_DATA *)alloc_perm( sizeof( *pReset ) );
	pReset->command	= letter;
	/* if_flag */	  fread_number( fp );
	pReset->arg1	= fread_number( fp );
	pReset->arg2	= fread_number( fp );
	pReset->arg3	= (letter == 'G' || letter == 'R')
			    ? 0 : fread_number( fp );
	pReset->arg4	= (letter == 'P' || letter == 'M')
			    ? fread_number(fp) : 0;
	fread_to_eol( fp );

	/*
	 * Do some validation of parameters.
	 * We'll defer validation of mob and obj vnums until
	 * after all areas are loaded.
	 */
	switch( letter )
	{
	default:
	    bug( "Load_resets: bad command '%c'.", letter );
	    break;

	case 'M':
	    if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
	    {
		new_reset( pRoomIndex, pReset );
		iLastRoom = pReset->arg3;
	    }
	    break;

	case 'O':
	    if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
	    {
		new_reset( pRoomIndex, pReset );
		iLastObj = pReset->arg3;
	    }
	    break;

	case 'P':
	    pReset->arg2 = URANGE( 1, pReset->arg2, 10 );
	    if ( ( pRoomIndex = get_room_index ( iLastObj ) ) )
	    {
		new_reset( pRoomIndex, pReset );
	    }
	    break;

	case 'G':
	case 'E':
	    if ( ( pRoomIndex = get_room_index ( iLastRoom ) ) )
	    {
		new_reset( pRoomIndex, pReset );
		iLastObj = iLastRoom;
	    }
	    break;

	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );

	    if (   pReset->arg2 < 0
		|| pReset->arg2 >= MAX_DIR
		|| !pRoomIndex
		|| !( pExit = pRoomIndex->exit[pReset->arg2] )
		|| !IS_SET( pExit->exit_info, EX_ISDOOR ) )
/*				   ^^^^^^^^^---change to rs_flags	*/
	    {
		bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
		exit( 1 );
	    }

	    switch ( pReset->arg3 )	/* OLC 1.1b */
	    {
		default:
		    bug( "Load_resets: 'D': bad 'locks': %d." , pReset->arg3);
		case 0:
		    break;
		case 1: SET_BIT( pExit->rs_flags, EX_CLOSED );
		    break;
		case 2: SET_BIT( pExit->rs_flags, EX_CLOSED | EX_LOCKED );
		    break;
	    }
	    break;

	case 'R':
	    if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR )	/* Last Door. */
	    {
		bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
		exit( 1 );
	    }

	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) )
		new_reset( pRoomIndex, pReset );

	    break;
	}
    }

    return;
}


/*
 * Load skill groups table
 */
void load_groups( void )
{
    FILE *	fp;
    char *	word;
    int		gn;
    int		sn;

    /* Need to str_dup all the spells so gedit drop can free them if needed */
    for ( gn = 0; !IS_NULLSTR( group_table[gn].name ); gn++ )
    {
	for ( sn = 0; sn < MAX_IN_GROUP; sn++ )
	{
	    if ( IS_NULLSTR( group_table[gn].spells[sn] ) )
		group_table[gn].spells[sn] = &str_empty[0];
	    else
		group_table[gn].spells[sn] = str_dup( group_table[gn].spells[sn] );
	}
    }

    if ( ( fp = fopen( SYSTEM_DIR GSKILL_FILE, "r" ) ) == NULL )
    {
	perror( "Load_groups" );
//	exit( 1 );
	return;
    }

    for ( ; ; )
    {
	if ( fread_letter( fp ) != '#' )
	{
	    bug( "Load_groups: \"#\" not found.", 0 );
	    exit( 1 );
	}

	word = fread_word( fp );
	if ( *word == '$' )
	    break;
	if ( !str_cmp( word, "GROUP" ) )
	    fread_group( fp );
	else
	{
	    bug( "Load_groups: bad section name.", 0 );
	    exit( 1 );
	}
    }

    fclose( fp );
    return;
}


/*
 * Read one skill group
 */
static void fread_group( FILE *fp )
{
    char	group[SHORT_STRING_LENGTH];
    char *	word;
    char *	retval;
    int		gn;
    int		sn;
    int		index;
    int		iClass;

    strcpy( group, fread_word( fp ) );
    if ( ( gn = group_lookup( group ) ) == NO_SKILL )
    {
	bugf( "Fread_group: bad group '%s'", group );
	do
	{
	    retval = fgets( group, sizeof( group ), fp );
	}
	while ( strcasecmp( group, "End\n" ) && retval != NULL );
	return;
    }

    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	group_table[gn].rating[iClass] = -1;

    for ( index = 0; index < MAX_IN_GROUP; index++ )
    {
	if ( !IS_NULLSTR( group_table[gn].spells[index] ) )
	{
	    free_string( group_table[gn].spells[index] );
	    group_table[gn].spells[index] = NULL;
	}
    }

    index = 0;

    for ( ; ; )
    {
	word = feof( fp ) ? "End" : fread_word( fp );

	if ( !str_cmp( word, "Cla" ) )
	{
	    word = fread_word( fp );
	    if ( ( iClass = class_lookup( word ) ) == NO_CLASS )
	    {
		bugf( "Fread_group: bad class '%s' in '%s'", word, group_table[gn].name );
		fread_to_eol( fp );
		continue;
	    }
	    group_table[gn].rating[iClass] = fread_number( fp );
	    continue;
	}

	if ( !str_cmp( word, "End" ) )
	{
	    return;
	}

	if ( !str_cmp( word, "Grp" ) )
	{
	    if ( index >= MAX_IN_GROUP )
	    {
		bugf( "Fread_group: too many skills in '%s'", group_table[gn].name );
		fread_to_eol( fp );
		continue;
	    }
	    word = fread_word( fp );
	    if ( ( sn = group_lookup( word ) ) == NO_SKILL )
	    {
		bugf( "Fread_group: bad group '%s' in group '%s'", word, group_table[gn].name );
		fread_to_eol( fp );
		continue;
	    }
	    group_table[gn].spells[index] = str_dup( group_table[sn].name );
	    index++;
	    continue;
	}

	if ( !str_cmp( word, "Skl" ) )
	{
	    if ( index >= MAX_IN_GROUP )
	    {
		bugf( "Fread_group: too many skills in '%s'", group_table[gn].name );
		fread_to_eol( fp );
		continue;
	    }
	    word = fread_word( fp );
	    if ( ( sn = skill_lookup( word ) ) == NO_SKILL )
	    {
		bugf( "Fread_group: bad skill '%s' in group '%s'", word, group_table[gn].name );
		fread_to_eol( fp );
		continue;
	    }
	    group_table[gn].spells[index] = str_dup( skill_table[sn].name );
	    index++;
	    continue;
	}

	bugf( "Fread_group: bad '%s' in group '%s'", word, group_table[gn].name );
	fread_to_eol( fp );
    }

}


/*
 * Load projects file
 */
void
load_projects( void )
{
    FILE *	fp;
    char	letter;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR PROJECT_FILE, "r" ) ) == NULL )
	return;


    for ( ; ; )
    {
	letter = fread_letter( fp );
	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}
	if ( letter != '#' )
	{
	    bug( "Load_projects: '#' not found.", 0 );
	    break;
	}

	word = fread_word( fp );
	if ( !str_cmp( word, "PROJECT" ) )
	    fread_project( fp );
	else if ( !str_cmp( word, "$" ) )
	    break;
	else
	{
	    bugf( "Load_projects: bad section %s", word );
	    fread_to_eol( fp );
	    continue;	/* hope for the best */
	}
    }

    fclose( fp );
    return;
}


/*
 * Load one project
 */
static void
fread_project( FILE *fp )
{
    PROJECT_DATA *	pProj;
    char *		word;
    bool		fMatch;
    int			value;

    pProj = new_project( );

    for ( ; ; )
    {
	word = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch( UPPER( *word ) )
	{
	    case '*':
		fMatch = TRUE;
		fread_to_eol( fp );
		break;

	    case 'C':
		KEY( "Comp",	pProj->completed,	fread_number( fp ) );
		break;

	    case 'D':
		KEY( "Date",	pProj->date,		fread_number( fp ) );
		SKEY( "Desc",	pProj->description );
		KEY( "Due",	pProj->deadline,	fread_number( fp ) );
		break;

	    case 'E':
		if ( !str_cmp( word, "End" ) )
		{
		    if ( pProj->completed != 0
		    &&	 pProj->completed + 7 * 24 * 60 * 60 < current_time )
		    {
			free_project( pProj );
			return;
		    }
		    if ( project_list == NULL )
			project_list = pProj;
		    else
			project_last->next = pProj;
		    project_last = pProj;
		    return;
		}
		break;

	    case 'F':
		SKEY( "From",	pProj->assigner );
		break;

	    case 'N':
		SKEY( "Name",	pProj->name );
		break;

	    case 'P':
		if ( !str_cmp( word, "Pri" ) )
		{
		    value = flag_value( priority_flags, fread_word( fp ) );
		    pProj->priority = value == NO_FLAG ? PRI_MEDIUM : value;
		    fMatch = TRUE;
		    break;
		}
		SKEY( "Progress",	pProj->progress );
		break;

	    case 'S':
		SKEY( "Status", pProj->status );
		break;

	    case 'T':
		SKEY( "To", pProj->assignee );
		break;

	    case 'V':
		KEY( "Vnum",	pProj->vnum,		fread_number( fp ) );
		break;

	}

	if ( !fMatch )
	{
	    bugf( "Fread_project: no match: %s", word );
	    fread_to_eol( fp );
	}

    }
}


/*
 * Load race table data
 */
void
load_races( void )
{
    FILE *	fp;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR RACE_FILE, "r" ) ) == NULL )
    {
	perror( "Load_races:" );
	return;
    }

    for ( ; ; )
    {
	if ( fread_letter( fp ) != '#' )
	{
	    bug( "Load_races: \"#\" not found.", 0 );
	    abort_mud( "Bad race file" );
	}

	word = fread_word( fp );
	if ( *word == '$' )
	    break;
	else if ( !str_cmp( word, "RACE" ) )
	    fread_race( fp );
	else
	{
	    bugf( "Load_races: bad section name %s.", word );
	    abort_mud( "Bad race file" );
	}

    }

    fclose( fp );
    return;
}


/*
 * Load one race
 */
void
fread_race( FILE *fp )
{
    int		iClass;
    int		iRace;
    int		index;
    char *	skill;
    int		sn;
    int		value;
    char *	word;

    word = fread_word( fp );
    iRace = race_lookup( word );
    /* require exact match on race name */
    if ( iRace == NO_RACE || str_cmp( word, race_table[iRace].name ) )
    {
	bugf( "Fread_race: bad race \"%s\"", word );
	do
	{
	    word = fread_word( fp );
	}
	while( str_cmp( word, "End" ) );
	return;
    }

    index = 0;

    for ( ; ; )
    {
	word = fread_word( fp );
	if ( !str_cmp( word, "End" ) )
	    return;
	else if ( !str_cmp( word, "Act" ) )
	{
	    race_table[iRace].act = fread_flag( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Aff" ) )
	{
	    race_table[iRace].aff = fread_xbits( fp );
	    continue;
	}
	else if ( !str_cmp( word, "AlMax" ) )
	{
	    race_table[iRace].max_align = fread_number( fp );
	    continue;
	}
	else if ( !str_cmp( word, "AlMin" ) )
	{
	    race_table[iRace].min_align = fread_number( fp );
	    continue;
	}
	else if ( !str_cmp( word, "EtMax" ) )
	{
	    race_table[iRace].max_ethos = fread_number( fp );
	    continue;
	}
	else if ( !str_cmp( word, "EtMin" ) )
	{
	    race_table[iRace].min_ethos = fread_number( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Form" ) )
	{
	    race_table[iRace].form = fread_flag( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Imm" ) )
	{
	    race_table[iRace].imm = fread_flag( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Max" ) )
	{
	    race_table[iRace].max_stats[STAT_STR] = fread_number( fp );
	    race_table[iRace].max_stats[STAT_INT] = fread_number( fp );
	    race_table[iRace].max_stats[STAT_WIS] = fread_number( fp );
	    race_table[iRace].max_stats[STAT_DEX] = fread_number( fp );
	    race_table[iRace].max_stats[STAT_CON] = fread_number( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Off" ) )
	{
	    race_table[iRace].off = fread_xbits( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Parts" ) )
	{
	    race_table[iRace].parts = fread_flag( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Pc" ) )
	{
	    word = fread_word( fp );
	    if ( UPPER( *word ) == 'Y'
	    ||	 UPPER( *word ) == 'T'
	    ||	 atoi( word ) != 0 )
		race_table[iRace].pc_race = TRUE;
	    else
		race_table[iRace].pc_race = FALSE;
	    continue;
	}
	else if ( !str_cmp( word, "Points" ) )
	{
	    race_table[iRace].points = fread_number( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Res" ) )
	{
	    race_table[iRace].res = fread_flag( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Size" ) )
	{
	    race_table[iRace].size = size_lookup( fread_word( fp ) );
	    continue;
	}
	else if ( !str_cmp( word, "Skill" ) )
	{
	    if ( index >= MAX_RACE_SKILLS )
	    {
		bugf( "Fread_race: too many skills for %s",
		      race_table[iRace].name );
		fread_to_eol( fp );
		continue;
	    }
	    skill = fread_word( fp );
	    if ( !str_cmp( skill, "NULL" ) )
		race_table[iRace].skills[index] = NULL;
	    else
	    {
		if ( ( sn = skill_lookup( skill ) ) == NO_SKILL )
		{
		    bugf( "Bad skill '%s' in %s", skill,
			  race_table[iRace].name );
		    fread_to_eol( fp );
		    continue;
		}
		race_table[iRace].skills[index] = str_dup( skill );
	    }
	    index++;
	    continue;
	}
	else if ( !str_cmp( word, "Stat" ) )
	{
	    race_table[iRace].stats[STAT_STR] = fread_number( fp );
	    race_table[iRace].stats[STAT_INT] = fread_number( fp );
	    race_table[iRace].stats[STAT_WIS] = fread_number( fp );
	    race_table[iRace].stats[STAT_DEX] = fread_number( fp );
	    race_table[iRace].stats[STAT_CON] = fread_number( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Vuln" ) )
	{
	    race_table[iRace].vuln = fread_flag( fp );
	    continue;
	}
	else if ( !str_cmp( word, "WtMin" ) )
	{
	    race_table[iRace].weight_min = fread_number( fp );
	    continue;
        }
        else if ( !str_cmp( word, "WtMax" ) )
        {
            race_table[iRace].weight_max = fread_number( fp );
            continue;
	}
	else
	{
	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    {
		if ( class_table[iClass].who_name == NULL
		||   class_table[iClass].name[0] == '\0' )
		    continue;
		if ( !str_cmp( word, class_table[iClass].name ) )
		    break;
		if ( !str_cmp( word, class_table[iClass].who_name ) )
		    break;
	    }

	    if ( iClass >= MAX_CLASS )
	    {
		bugf( "Fread_race: bad keyword %s in %s", word,
		      race_table[iRace].name );
		fread_to_eol( fp );
		continue;
	    }

	    value = fread_number( fp );
	    race_table[iRace].class_mult[iClass] = value;
	}
    }

}


/*
 * Load an overland sector description
 */
void
load_sectdesc( FILE *fp )
{
    char	name[SHORT_STRING_LENGTH];
    char	type[SHORT_STRING_LENGTH];
    int		sector;
    char	desc[MAX_STRING_LENGTH];
    int		index;

    if ( area_last == NULL )
    {
        bugf( "load_sectdesc: no #AREA seen yet." );
        abort_mud( "Load_sectdesc: no #AREA" );
    }

    strcpy( name, fread_word( fp ) );
    strcpy( type, fread_word( fp ) );
    fread_buf( fp, desc, sizeof( desc ) );

    if ( ( sector = flag_value( sector_types, name ) ) == NO_FLAG )
    {
        bugf( "Load_sectdesc: bad sector type '%s'.", name );
        return;
    }

    if ( !str_cmp( type, "desc" ) )
        index = DESC_DESC;
    else if ( !str_cmp( type, "morning" ) )
        index = DESC_MORN;
    else if ( !str_cmp( type, "evening" ) )
        index = DESC_EVEN;
    else if ( !str_cmp( type, "night" ) )
        index = DESC_NIGHT;
    else
    {
        bugf( "Load_sectdesc: bad desctype %s, \"desc\" assumed.", type );
        index = DESC_DESC;
    }

    if ( area_last->overland == NULL )
    {
        bugf( "Load_sectdesc: #SECTOR %s with no overland.", name );
        return;
    }

    area_last->overland->sect_desc[sector][index] = str_dup( desc );
    return;
}


/*
 * Load skill table
 */
void
load_skills( void )
{
    FILE *	fp;
    char *	word;
    int		sn;
    int		iClass;

    for ( sn = 1; ; sn++ )
    {
	if ( skill_table[sn].name == NULL || skill_table[sn].name[0] == '\0' )
	    break;
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( skill_table[sn].skill_level[iClass] == 0 )
		skill_table[sn].skill_level[iClass] = L_APP;
	}
	if ( !IS_NULLSTR( skill_table[sn].noun_damage ) )
	    skill_table[sn].noun_damage = str_dup( skill_table[sn].noun_damage );
	if ( !IS_NULLSTR( skill_table[sn].msg_off ) )
	    skill_table[sn].msg_off = str_dup( skill_table[sn].msg_off );
	if ( !IS_NULLSTR( skill_table[sn].msg_obj ) )
	    skill_table[sn].msg_obj = str_dup( skill_table[sn].msg_obj );
	if ( !IS_NULLSTR( skill_table[sn].msg_room ) )
	    skill_table[sn].msg_room = str_dup( skill_table[sn].msg_room );
    }

    if ( ( fp = fopen( SYSTEM_DIR SKILL_FILE, "r" ) ) == NULL )
    {
	perror( "Load_skills:" );
	exit( 1 );
    }

    for ( ; ; )
    {
	if ( fread_letter( fp ) != '#' )
	{
	    bug( "Load_skills: \"#\" not found.", 0 );
	    exit( 1 );
	}

	word = fread_word( fp );
	if ( *word == '$' )
	    break;
	if ( !str_cmp( word, "SKILL" ) )
	    fread_skill( fp );
	else
	{
	    bug( "Load_skills: bad section name.", 0 );
	    exit( 1 );
	}
    }

    fclose( fp );
    return;
}


/*
 * Read one skill
 */
static void
fread_skill( FILE *fp )
{
    char *word;
    char *pskill;
    int   sn;
    int   psn;
    int   index;
    int   iClass;

    word = fread_word( fp );
    if ( ( sn = skill_lookup( word ) ) == NO_SKILL )
    {
	bugf( "Fread_skill: bad skill/spell '%s'", word );
	do
	{
	    word = fread_word( fp );
	} while ( str_cmp( word, "End" ) );
	return;
    }

    skill_table[sn].min_mana	= fread_number( fp );
    skill_table[sn].beats	= fread_number( fp );
				  fread_number( fp );
    /*
     * The third value read above used to be target type.  There is no
     * reason the target type should be in the OLC since it cannot be
     * changed without code change also in most cases; hence, removed.
     */

    for ( ; ; )
    {
	word = fread_word( fp );

	if ( !str_cmp( word, "End" ) )
	    return;

	if ( !str_cmp( word, "Dammsg" ) )
	{
	    skill_table[sn].noun_damage = fread_string( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Flags" ) )
	{
	    skill_table[sn].spell_flags = fread_flag( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Forget" ) )
	{
	    skill_table[sn].forget = fread_number( fp );
	    continue;
	}

	else if ( !str_cmp( word, "Offmsg" ) )
	{
	    skill_table[sn].msg_off = fread_string( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Objmsg" ) )
	{
	    skill_table[sn].msg_obj = fread_string( fp );
	    continue;
	}
	else if ( !str_cmp( word, "Prereq" ) )
	{
	    pskill = fread_word( fp );
	    psn = skill_lookup( pskill );
	    if ( psn == NO_SKILL )
	    {
		bugf( "Fread_skill: bad prerequisite skill '%s' for '%s'.",
		      pskill, skill_table[sn].name );
		continue;
	    }
	    for ( index = 0; index < MAX_PREREQ; index++ )
		if ( skill_table[sn].prereq[index] < 1 )
		    break;
	    if ( index >= MAX_PREREQ )
	    {
		bugf( "Fread_skill: Too many prerequisites for '%s'.",
		      skill_table[sn].name );
		continue;
	    }
	    skill_table[sn].prereq[index] = psn;
	    continue;
	}
	else if ( !str_cmp( word, "Roommsg" ) )
	{
	    skill_table[sn].msg_room = fread_string( fp );
	    continue;
	}

	/* Could use class_lookup() here, instead of this loop */
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( class_table[iClass].who_name == NULL || class_table[iClass].name[0] == '\0' )
		continue;
	    if ( !str_cmp( word, class_table[iClass].who_name ) )
		break;
	}
	if ( iClass >= MAX_CLASS )
	{
	    bugf( "Fread_skill: Bad class %s in '%s'.", word,
		  skill_table[sn].name );
	    fread_number( fp );
	    fread_number( fp );
	}
	else
	{
	    skill_table[sn].skill_level[iClass] = fread_number( fp );
	    skill_table[sn].rating[iClass] = fread_number( fp );
	}
    }
}


void
load_socials( void )
{
    FILE *	fp;
    char	letter;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR SOCIAL_FILE, "r" ) ) == NULL )
    {
	bug( "Social file not found.", 0 );
	return;
    }

    for ( ; ; )
    {
	letter = fread_letter( fp );
	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}
	if ( letter != '#' )
	{
	    bug( "Load_socials: '#' not found.", 0 );
	    break;
	}

	word = fread_word( fp );
	if ( !str_cmp( word, "SOCIAL" ) )
	    fread_social( fp );
	else if ( !str_cmp( word, "END" ) )
	    break;
	else
	{
	    bugf( "Fread_social: bad section %s", word );
	    fread_to_eol( fp );
	    continue;	/* hope for the best */
	}
    }
    fclose( fp );
    return;
}


static void
fread_social( FILE *fp )
{
    SOCIAL_DATA *pSocial;
    SOCIAL_DATA *pTmp;
    char	 name[MAX_INPUT_LENGTH];
    char *	 word;
    bool	 fMatch;
    bool	 done;

    pSocial = new_social( );

    done = FALSE;
    while ( !done )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    KEY( "Cnoarg",	pSocial->cnoarg,	fread_string( fp ) );
	    KEY( "Cfound",	pSocial->cfound,	fread_string( fp ) );
	    KEY( "Cnfound",	pSocial->nfound,	fread_string( fp ) );
	    KEY( "Cself",	pSocial->cself,		fread_string( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		done = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'L':
	    KEY( "Level",	pSocial->ed_level,	level_lookup( fread_word( fp ) ) );
	    break;

	case 'N':
	    KEY( "Nfound",	pSocial->nfound,	fread_string( fp ) );
	    if ( !str_cmp( word, "Name" ) )
	    {
		strcpy( name, fread_word( fp ) );
		str_lower( name, name );
		pSocial->name = str_dup( name );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'O':
	    KEY( "Onoarg",	pSocial->onoarg,	fread_string( fp ) );
	    KEY( "Ofound",	pSocial->ofound,	fread_string( fp ) );
	    KEY( "Oself",	pSocial->oself,		fread_string( fp ) );
	    break;

	case 'V':
	    KEY( "Vfound",	pSocial->vfound,	fread_string( fp ) );
	    break;
	}

	if ( !fMatch )
	{
	    bugf( "Fread_social: no match: %s", word );
	    fread_to_eol( fp );
	}
    }

    if ( !pSocial->name || *pSocial->name == '\0' )
    {
	bug( "Fread_social: no Name", 0 );
	free_social( pSocial );
	return;
    }

    if ( social_first == NULL || strcmp( name, social_first->name ) < 0 )
    {
	pSocial->next = social_first;
	social_first = pSocial;
	if ( social_last == NULL )
	{
	    social_last = social_first;
	}
    }
    else if ( strcmp( name, social_last->name ) > 0 )
    {
	social_last->next = pSocial;
	pSocial->next = NULL;
	social_last = pSocial;
    }
    else
    {
	for ( pTmp = social_first; pTmp; pTmp = pTmp->next )
	{
	    if ( strcmp( name, pTmp->next->name ) < 0 )
	    {
		pSocial->next = pTmp->next;
		pTmp->next = pSocial;
		break;
	    }
	}
    }

    return;
}


void
load_sql_data( void )
{
    FILE *	fp;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR SQL_FILE, "r" ) ) == NULL )
	return;

    for ( ; ; )
    {
	word = feof( fp ) ? "End" : fread_word( fp );
	if ( !str_cmp( word, "End" ) )
	    break;
	else if ( !str_cmp( word, "Db" ) )
	    sql_db = str_dup( fread_word( fp ) );
	else if ( !str_cmp( word, "Host" ) )
	    sql_host = str_dup( fread_word( fp ) );
	else if ( !str_cmp( word, "Port" ) )
	    sql_port = fread_number( fp );
	else if ( !str_cmp( word, "Pwd" ) )
	    sql_passwd = str_dup( fread_word( fp ) );
        else if ( !str_cmp( word, "Socket" ) )
            sql_socket = str_dup( fread_word( fp ) );
	else if ( !str_cmp( word, "User" ) )
	    sql_user = str_dup( fread_word( fp ) );
	else
	{
	    bugf( "Load_sql_data: no match: %s", word );
	    fread_to_eol( fp );
	}
    }

    fclose( fp );

    sql_flag = !IS_NULLSTR( sql_user ) && !IS_NULLSTR( sql_passwd ) && !IS_NULLSTR( sql_host );

    return;
}


void
load_sysconfig( void )
{
    FILE *	fp;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR CONFIG_FILE, "r" ) ) == NULL )
    {
	bug( "Load_sysconfig: file not found.", 0 );
	return;
    }

    for ( ; ; )
    {
	word = feof( fp ) ? "End" : fread_word( fp );
	if ( !str_cmp( word, "End" ) )
	    break;

	if ( !str_cmp( word, "pkill" ) )
	{
	    sys_pkill = fread_number( fp );
	    continue;
	}

	if ( !str_cmp( word, "webwho" ) )
	{
	    sys_webwho = (bool)fread_number( fp );
	    continue;
	}

	fread_to_eol( fp );
    }

    fclose( fp );
    return;
}


void
load_travels( FILE *fp )
{
    TRAVEL_DATA *	pTravel;
    TRAVEL_DATA *	travel_next;
    TRAVEL_DATA *	list;
    int			vnum;

    if ( area_last == NULL )
	abort_mud( "Load_travels: no #AREA seen yet." );

    vnum = fread_number( fp );
    if ( vnum == 0 )
	return;

    area_last->travel_vnum = vnum;

    area_last->link	= new_exit( );
    area_last->rev_link	= new_exit( );
    area_last->link->size = SIZE_TITANIC;
    area_last->rev_link->size = SIZE_TITANIC;

    list = NULL;

    for ( ; ; )
    {
	vnum = fread_number( fp );
	if ( vnum == 0 )
	{
	    fread_to_eol( fp );
	    break;
	}

	pTravel = new_travel( );
	pTravel->next = list;
	list = pTravel;
	pTravel->area = area_last;

	pTravel->room_vnum	= vnum;
	pTravel->exit_dir	= fread_number( fp );
	pTravel->stop_timer	= fread_number( fp );
	pTravel->move_timer	= fread_number( fp );
	pTravel->arrive_travel	= fread_string( fp );
	pTravel->arrive_room	= fread_string( fp );
	pTravel->depart_travel	= fread_string( fp );
	pTravel->depart_room	= fread_string( fp );
    }

    /* We now have a local list of the newly loaded travel room stops */
    /* in reverse order.  Copy it to the area's list, which will put */
    /* the list in the correct order. */
    for ( pTravel = list; pTravel != NULL; pTravel = travel_next )
    {
	travel_next = pTravel->next;
	pTravel->next = area_last->travel_first;
	area_last->travel_first = pTravel;
    }

    /* Postpone initialization until after all areas have been loaded. */
    return;
}


void
load_travel_state( void )
{
    int			dir;
    FILE *		fp;
    AREA_DATA *		pArea;
    EXIT_DATA *		pExit;
    TRAVEL_DATA *	pTravel;
    int			rdir;
    int			vnum;

    if ( ( fp = fopen( SYSTEM_DIR TRAVEL_FILE, "r" ) ) == NULL )
    {
	for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
	    if ( pArea->travel_room != NULL
	    &&	 pArea->travel_curr != NULL )
	    {
		bugf( "Load_travel_state: No Travel file found." );
		return;
	    }
	return;
    }

    for ( ; ; )
    {
	vnum = fread_number( fp );

	if ( vnum == 0 )
	    break;

	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    if ( pArea->travel_vnum == vnum )
		break;
	}

	if ( pArea == NULL )
	{
	    bug( "Load_travel_state: No area for room vnum %d.", vnum );
	    fread_to_eol( fp );
	    continue;
	}

	vnum = fread_number( fp );

	for ( pTravel = pArea->travel_first; pTravel != NULL; pTravel = pTravel->next )
	    if ( pTravel->room_vnum == vnum )
		break;

	if ( pTravel == NULL )
	{
	    bug( "Load_travel_state: No stop for room vnum %d.", vnum );
	    fread_to_eol( fp );
	    continue;
	}

	pArea->travel_curr = pTravel;
	pArea->travel_timer = fread_number( fp );
	pTravel->stopped = fread_number( fp );

	if ( pTravel->stopped )
	{
	    dir = pTravel->exit_dir;
	    rdir = rev_dir[dir];

	    pArea->travel_room->exit[dir] = pArea->link;
	    pArea->travel_room->exit[dir]->to_room = pTravel->room;

	    pExit = pTravel->room->exit[rdir];
	    pTravel->room->exit[rdir] = pArea->rev_link;
	    pTravel->room->exit[rdir]->to_room = pArea->travel_room;
	    pArea->rev_link = pExit;
	}

    }

    fclose( fp );
    return;
}


void
load_userlist( void )
{
    FILE *	fp;
    char	letter;
    char *	word;

    if ( ( fp = fopen( SYSTEM_DIR USER_FILE, "r" ) ) == NULL )
    {
	bug( "Load_userlist: userlist file not found.", 0 );
	return;
    }

    for ( ; ; )
    {
	letter = fread_letter( fp );

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_userlist: '#' not found.", 0 );
	    break;
	}

	word = fread_word( fp );
	if ( !str_cmp( word, "USER" ) )
	    fread_userrec( fp );
	else if ( !str_cmp( word, "END" ) )
	    break;
	else
	{
	    bugf( "Fread_userlist: bad section %s", word );
	    fread_to_eol( fp );
	    continue;
	}

    }

    fclose( fp );
    read_ids( );
    return;
}


static void
fread_userrec( FILE *fp )
{
    USERLIST_DATA *	pUser;
    USERLIST_DATA *	pTmp;
    char *		word;

    pUser = new_user_rec( );
    for ( ; ; )
    {
	word = feof( fp ) ? "End" : fread_word( fp );
	if ( !str_cmp( word, "Name" ) )
	    pUser->name = str_dup( fread_word( fp ) );
	else if ( !str_cmp( word, "Class" ) )
	    pUser->class = str_dup( fread_word( fp ) );
	else if ( !str_cmp( word, "Host" ) )
	    pUser->host = str_dup( fread_word( fp ) );
	else if ( !str_cmp( word, "Id" ) )
	    pUser->id = fread_number( fp );
	else if ( !str_cmp( word, "Last" ) )
	    pUser->lastlogon = fread_number( fp );
	else if ( !str_cmp( word, "LogC" ) )
	    pUser->logcount = fread_number( fp );
	else if ( !str_cmp( word, "Lvl" ) )
	    pUser->level = fread_number( fp );
	else if ( !str_cmp( word, "Plyd" ) )
	    pUser->played = fread_number( fp );
	else if ( !str_cmp( word, "Race" ) )
	    pUser->race = str_dup( fread_word( fp ) );
	else if ( !str_cmp( word, "End" ) )
	    break;
	else
	{
	    bugf( "Fread_userrec: no match: %s", word );
	    fread_to_eol( fp );
	}
    }

    if ( IS_NULLSTR( pUser->name ) )
    {
	bug( "Fread_userrec: 'End' with no name.", 0 );
	free_user_rec( pUser );
	return;
    }

    /* Hrm, could sort it here, same as we do the socials */
    if ( user_first == NULL || strcasecmp( pUser->name, user_first->name ) < 0 )
    {
	pUser->next = user_first;
	user_first = pUser;
	if ( user_last == NULL )
	{
	    user_last = pUser;
	}
    }
    else if ( strcasecmp( pUser->name, user_last->name ) > 0 )
    {
	user_last->next = pUser;
	pUser->next = NULL;
	user_last = pUser;
    }
    else
    {
	for ( pTmp = user_first; pTmp != NULL; pTmp = pTmp->next )
	{
	    if ( strcasecmp( pUser->name, pTmp->next->name ) < 0 )
	    {
		pUser->next = pTmp->next;
		pTmp->next = pUser;
		break;
	    }
	}
    }

    return;
}


/*
 * Load the vehicles location file.
 */
void
load_vehicles( void )
{
    ROOM_INDEX_DATA *	vehicle;
    ROOM_INDEX_DATA *	in_room;
    FILE *		fp;
    int			vnum;
    int			in_vnum;

    if ( ( fp = fopen( SYSTEM_DIR VEHICLE_FILE, "r" ) ) == NULL )
	return;

    for ( ; ; )
    {
	vnum = fread_number( fp );
	if ( vnum == 0 )
	    break;
	in_vnum = fread_number( fp );
	if ( ( vehicle = get_room_index( vnum ) ) == NULL )
	{
	    bug( "Load_vehicles: bad vehicle vnum %d.", vnum );
	    continue;
	}
	if ( ( in_room = get_vroom_index( in_vnum ) ) == NULL )
	{
	    bugf( "Load_vehicles: #%d in bad room %d.", vnum, in_vnum );
	    continue;
	}

	room_to_room( vehicle, in_room );
    }

    fclose( fp );
    return;
}


/*
 * Load virtual room data
 */
void
load_vrooms( FILE *fp )
{
    int			index;
    VIRTUAL_ROOM_DATA *	map;
    int			size;
    int			val;

    if ( area_last == NULL )
    {
	bugf( "Load_vrooms: no #AREA seen yet." );
	abort_mud( "Load_vrooms: No #AREA" );
    }

    if ( area_last->overland == NULL || area_last->overland->map == NULL )
    {
	bugf( "Load_vrooms: Overland not allocated." );
	abort_mud( "Load_vrooms: Overland not allocated." ); 
    }

    size = area_last->maxx * area_last->maxy;
    map = area_last->overland->map;

    for ( index = 0; index < size; index++ )
    {
	val = fread_number( fp );
	if ( val == -1 )
	{
	    bugf( "Unexpected end of virtual room list." );
	    return;
	}
	map[index].flags = val;
	map[index].sector_type = fread_number( fp );
    }

    while ( fread_number( fp ) != -1 )
	;

    return;
}


static void
mprog_read_programs( FILE *fp, MOB_INDEX_DATA *pMobIndex )
{
  MPROG_DATA *mprg;
  char        letter;
  bool        done = FALSE;

  if ( ( letter = fread_letter( fp ) ) != '>' )
  {
      bug( "Mprog_read_programs: vnum %d MOBPROG char", pMobIndex->vnum );
      exit( 1 );
  }
  pMobIndex->mobprogs = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
  top_mprog++;
  mprg = pMobIndex->mobprogs;

  while ( !done )
  {
    mprg->type = mprog_type( fread_word( fp ) );
    switch ( mprg->type )
    {
     case ERROR_PROG:
        bug( "Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum );
        exit( 1 );
      break;
     default:
        pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
        mprg->arglist        = fread_string( fp );
        fread_to_eol( fp );
        mprg->comlist        = fread_string( fp );
        fread_to_eol( fp );
        switch ( letter = fread_letter( fp ) )
        {
          case '>':
             mprg->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
             mprg       = mprg->next;
             mprg->next = NULL;
           break;
          case '|':
             mprg->next = NULL;
             fread_to_eol( fp );
             done = TRUE;
           break;
          default:
             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             exit( 1 );
           break;
        }
      break;
    }
  }

  return;

}


/*
 * Adds a reset to a room.  OLC
 * Similar to add_reset in olc.c
 */
void
new_reset( ROOM_INDEX_DATA *pR, RESET_DATA *pReset )
{
    RESET_DATA *pr;

    if ( !pR )
       return;

    pr = pR->reset_last;

    if ( !pr )
    {
        pR->reset_first = pReset;
        pR->reset_last  = pReset;
    }
    else
    {
        pR->reset_last->next = pReset;
        pR->reset_last       = pReset;
        pR->reset_last->next = NULL;
    }

    top_reset++;
    return;
}


static void
read_ids( void )
{
    USERLIST_DATA *	pUser;
    FILE *		fp;
    char		buf[MAX_INPUT_LENGTH];
    char		pfile[MAX_INPUT_LENGTH];
    char *		p;
    char *		pname;
    time_t		t;

    pname = stpcpy( pfile, PLAYER_DIR );
    for ( pUser = user_first; pUser != NULL; pUser = pUser->next )
    {
	if ( pUser->id != 0 )
	    continue;
	strcpy( pname, capitalize( pUser->name ) );
	if ( ( fp = fopen( pfile, "r" ) ) == NULL )
	    continue;

	while ( fgets( buf, sizeof( buf ), fp ) != NULL )
	{
	    if ( buf[0] != 'I' || buf[1] != 'd' || !isspace( buf[2] ) )
		continue;
	    p = buf + 3;
	    while ( isspace( *p ) )
		p++;
	    if ( !isdigit( *p ) )
		continue;

	    t = atoi( p );
	    while( isdigit( *p ) )
	        p++;
	    if ( !isspace( *p ) )
	        continue;
	    pUser->id = t;
	    break;
	}

	fclose( fp );
    }
}

