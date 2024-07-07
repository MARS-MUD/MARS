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

#define AREA_VERSION		9

/*
 * Local variable
 */
static	int	offset;	/* used for renumbering areas via asave export */

/*
 * Local functions
 */
static	bool	check_area	args( ( CHAR_DATA *ch, AREA_DATA *pArea ) );
static	int	fix_vnum	args( ( AREA_DATA *pArea, int oldvnum ) );
static	bool	save_area	args( ( AREA_DATA *pArea ) );
static	bool	save_area_list	args( ( void ) );
static	bool	save_dreams	args( ( void ) );
static	void	save_helps	args( ( FILE *fp, AREA_DATA *pArea ) );
static	void	save_mobiles	args( ( FILE *fp, AREA_DATA *pArea ) );
static	void	save_objects	args( ( FILE *fp, AREA_DATA *pArea ) );
static	void	save_oreprogs	args( ( FILE *fp, OREPROG_DATA *pProg,
					const struct flag_type *flag_table ) );
static	void	save_resets	args( ( FILE *fp, AREA_DATA *pArea ) );
static	void	save_rooms	args( ( FILE *fp, AREA_DATA *pArea ) );
static	bool	save_sectdesc	args( ( FILE *fp, AREA_DATA *pArea ) );
static	void	save_specials	args( ( FILE *fp, AREA_DATA *pArea ) );
static	void	save_shops	args( ( FILE *fp, AREA_DATA *pArea ) );
static	void	save_travels	args( ( FILE *fp, AREA_DATA *pArea ) );
static	void	save_userlist_force args( ( void ) );
static	void	save_vrooms	args( ( FILE *fp, AREA_DATA *pArea ) );
//   void    olcautosave     args( ( void ) );

void
do_asave( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    BUFFER *	output;
    HELP_DATA *	pHelp;
    char	arg[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char	arg3[MAX_INPUT_LENGTH];
    int		value;
    int		newbase;
    int		oldbase;
    bool	found;

    offset = 0;

    if ( ch == NULL )		/* Do an autosave */
    {
	found = FALSE;
	for ( pArea = area_first; pArea; pArea = pArea->next )
	    if ( IS_SET( pArea->area_flags, AREA_CHANGED ) )
		found = TRUE;
	if ( found )
	{
	    save_area_list( );
	    for ( pArea = area_first; pArea; pArea = pArea->next )
		if ( IS_SET( pArea->area_flags, AREA_CHANGED ) )
		    save_area( pArea );
	}
	save_boards_file( );
	if ( changed_clan )
	    save_clans( );
	save_cmd_files( );
	save_dreams( );
	if ( get_event_game( EVENT_GAME_SAVE_GREET ) != NULL )
	    save_greet_data( );
	save_projects( );
	save_race_file( );
	save_skill_files( );
	save_socials( );
	save_sysconfig( );
	save_userlist_force( );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r", ch );
	send_to_char( "  asave <vnum>    - saves a particular area\n\r", ch );
	send_to_char( "  asave area      - saves the area being edited\n\r", ch );
	send_to_char( "  asave changed   - saves all changed zones\n\r", ch );
	send_to_char( "  asave dreams    - saves the dream file\n\r", ch );
	send_to_char( "  asave world     - saves the world! (db dump)\n\r", ch );
        return;
    }

    if ( is_number( arg ) )
    {
	value = atoi( arg );
	if ( ( pArea = get_area_data( value ) ) == NULL )
	{
	    send_to_char( "That area does not exist.\n\r", ch );
	    return;
	}

	if ( !check_area( ch, pArea ) )
	    return;
	save_area_list( );
	save_area( pArea );
	ch_printf( ch, "Area %d (%s) saved.\n\r", value, pArea->name );
	return;
    }

    if ( !str_cmp( arg, "area" ) )
    {
	switch( ch->desc->editor )
	{
	    case ED_AREA:
		pArea = (AREA_DATA *)ch->desc->pEdit;
		break;
	    case ED_HELP:
		EDIT_HELP( ch, pHelp );
		pArea = pHelp->area ? pHelp->area : ch->in_room->area;
		break;
	    case ED_MOBILE:
		pArea = ( (MOB_INDEX_DATA *)ch->desc->pEdit )->area;
		break;
	    case ED_MPROG:
		pArea = ( (MOB_INDEX_DATA *)ch->desc->inEdit )->area;
		break;
	    case ED_OBJECT:
		pArea = ( (OBJ_INDEX_DATA *)ch->desc->pEdit )->area;
		break;
	    case ED_OPROG:
		pArea = ( (OBJ_INDEX_DATA *)ch->desc->inEdit )->area;
		break;
	    case ED_ROOM:
		pArea = ch->in_room->area;
		break;
	    case ED_RPROG:
		pArea = ch->in_room->area;
		break;
	    default:
		pArea = ch->in_room->area;
		break;
	}

	if ( !check_area( ch, pArea ) )
	    return;

	save_area_list( );
	save_area( pArea );
	ch_printf( ch, "Area %d (%s) saved.\n\r", pArea->vnum, pArea->name );
	return;
    }

    if ( !str_cmp( arg, "changed" ) )
    {
	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    if ( IS_SET( pArea->area_flags, AREA_CHANGED ) )
		break;
	}
	if ( pArea == NULL )
	{
	    send_to_char( "All areas up to date.\n\r", ch );
	    return;
	}

	found = FALSE;
	output = new_buf( );
	add_buf( output, "Saved areas:\n\r" );
	save_area_list( );
	for ( ; pArea; pArea = pArea->next )
	{
	    if ( !IS_SET( pArea->area_flags, AREA_CHANGED ) )
		continue;
	    if ( !check_area( NULL, pArea ) )
		continue;
	    found = TRUE;
	    save_area( pArea );
	    buf_printf( output, "  %12s - '%s`X'\n\r", pArea->file_name, pArea->name );
	}
	if ( !found )
	    add_buf( output, "None\n\r" );
	page_to_char( buf_string( output ), ch );
	free_buf( output );
	return;
    }

    if ( str_match( arg, "dr", "dreams" ) )
    {
        save_dreams( );
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "export" ) && get_trust( ch ) >= IMPLEMENTOR )
    {
        argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );

        if ( !is_number( arg2 ) || !is_number( arg3 ) )
        {
            send_to_char( "Syntax: asave export <area_vnum> <new_start_vnum>\n\r", ch );
            return;
        }

        value = atoi( arg2 );
	if ( ( pArea = get_area_data( value ) ) == NULL )
	{
	    send_to_char( "That area does not exist.\n\r", ch );
	    return;
	}

	if ( !check_area( ch, pArea ) )
	    return;

        newbase = atoi( arg3 );
        if ( newbase < 1 )
        {
            send_to_char( "New start vnum must be a valid vnum.\n\r", ch );
            return;
        }

        offset = newbase - pArea->min_vnum;
        if ( offset + pArea->max_vnum > MAX_VNUM )
        {
            send_to_char( "New vnum range out of valid vnum range.\n\r", ch );
            offset = 0;
            return;
        }
        if ( offset == 0 )
        {
            send_to_char( "New start vnum must be different from old start vnum.\n\r", ch );
            return;
        }

        oldbase = offset;save_area( pArea );

        ch_printf( ch, "Area %d (%s) exported with new vnum range %d-%d.\n\r",
                   value, pArea->name,
                   pArea->min_vnum + oldbase, pArea->max_vnum + oldbase );
        offset = 0;
        return;
    }

    if ( !str_cmp( arg, "world" ) )
    {
	if ( get_trust( ch ) < IMPLEMENTOR )
	{
	    send_to_char( "You are of too low a level to do that.\n\r", ch );
	    return;
	}

	found = FALSE;
	output = new_buf( );
	add_buf( output, "Saved areas:\n\r" );
	save_area_list( );
	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    if ( !check_area( NULL, pArea ) || !IS_BUILDER( ch, pArea ) )
		continue;
	    found = TRUE;
	    save_area( pArea );
	    buf_printf( output, "  %12s - '%s'\n\r", pArea->file_name, pArea->name );
	}
	if ( !found )
	    add_buf( output, "None\n\r" );
	page_to_char( buf_string( output ), ch );
	free_buf( output );

	/* force save */
	changed_boards = TRUE;
	save_boards_file( );
	changed_clan = TRUE;
	save_clans( );
	changed_command = TRUE;
	changed_secgroup = TRUE;
	save_cmd_files( );
	changed_dream = TRUE;
	save_dreams( );
	changed_project = TRUE;
	save_projects( );
	changed_race = TRUE;
	save_race_file( );
	changed_group = TRUE;
	changed_skill = TRUE;
	save_skill_files( );
	changed_social = TRUE;
	save_socials( );
	changed_system = TRUE;
	save_sysconfig ( );
	save_userlist_force( );
	return;
    }

    do_function( ch, do_asave, "" );
}

/*
 * Checks an area for valid vnums and filename, and builders access
 * Returns TRUE if okay to save
 */
static bool
check_area( CHAR_DATA *ch, AREA_DATA *pArea )
{
    if ( ch && !IS_BUILDER( ch, pArea ) )
    {
	ch_printf( ch, "You are not a builder for %s area.\n\r",
			pArea == ch->in_room->area ? "this" : "that" );
	return FALSE;
    }

    if ( pArea->min_vnum < 1 || pArea->max_vnum < pArea->min_vnum )
    {
	if ( ch )
	    send_to_char( "Area vnums not set.\n\r", ch );
	return FALSE;
    }

    if ( *pArea->file_name == '\0' )
    {
	if ( ch )
	    send_to_char( "Area filename not set.\n\r", ch );
	return FALSE;
    }

    return TRUE;
}


static int
fix_vnum( AREA_DATA *pArea, int oldvnum )
{
    if ( pArea == NULL || offset == 0 )
        return oldvnum;

    if ( oldvnum >= pArea->min_vnum && oldvnum <= pArea->max_vnum )
        return oldvnum + offset;
    else
        return oldvnum;
}


/*
 * Return a string for writing an extended bitvector to a file
 */
char *
print_xbits( EXT_BV *bits )
{
    static char buf[XBI * 12];
    char *	p;
    int		x;
    int		cnt;

    p = buf;
    for ( cnt = XBI-1; cnt > 0; cnt-- )
	if ( bits->bits[cnt] )
	    break;
    for ( x = 0; x <= cnt; x++ )
    {
	p = stpcpy( p, print_flags( bits->bits[x] ) );
	if ( x < cnt )
	    *p++ = '&';
    }
    *p = '\0';

    return buf;
}


/*
 * Save an area.
 * Return TRUE on success
 */
static bool
save_area( AREA_DATA *pArea )
{
    char	filename[SHORT_STRING_LENGTH];
    char	newname[SHORT_STRING_LENGTH];
    FILE	*fp;

    if ( pArea == NULL )
    {
	bugf( "Save_area: NULL pArea" );
	return FALSE;
    }

    if (    pArea->min_vnum < 1
	 || pArea->max_vnum < pArea->min_vnum
	 || *pArea->file_name == '\0' )
	return FALSE;

    sprintf( filename, "%s%s", TEMP_DIR, pArea->file_name );
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
	perror( filename );
	fpReserve = fopen( NULL_FILE, "r" );
	bug( "Open_area: fopen", 0 );
	return FALSE;
    }

    if ( IS_SET( pArea->area_flags, AREA_CHANGED ) )
    {
	REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
	pArea->last_changed = current_time;
    }

    fprintf( fp, "#AREADATA\n" );
    fprintf( fp, "Version     %d\n",	     AREA_VERSION );
    fprintf( fp, "Name        %s~\n",        pArea->name );
    fprintf( fp, "Credits     %s~\n",	     pArea->credits );
    fprintf( fp, "Builders    %s~\n",        fix_string( pArea->builders ) );
    fprintf( fp, "Kingdom     '%s'\n",	     flag_string( kingdom_types, pArea->kingdom ) );
    fprintf( fp, "Continent   %s\n",	     flag_string( continent_types, pArea->continent ) );
    fprintf( fp, "World       '%s'\n",	     flag_string( world_types, pArea->world ) );
    fprintf( fp, "VNUMs       %d %d\n",      
             fix_vnum( pArea, pArea->min_vnum ),
             fix_vnum( pArea, pArea->max_vnum ) );
    if ( pArea->maxx > 0 && pArea->maxy > 0 )
	fprintf( fp, "Overland    %d %d\n",  pArea->maxx, pArea->maxy );
    fprintf( fp, "Levels      %d %d\n",	     pArea->low_level, pArea->high_level );
    fprintf( fp, "Security    %d\n",         pArea->security );
    fprintf( fp, "Recall      %d\n",         pArea->recall );
    fprintf( fp, "Flags       %s\n",         print_flags( pArea->area_flags ) );
    fprintf( fp, "Color       %d\n",	     pArea->color );
    fprintf( fp, "ExitSize    %s\n",	     flag_string( size_types, pArea->exitsize ) );
    if ( pArea->created != 0 )
        fprintf( fp, "Created     %ld\n",    pArea->created );
    fprintf( fp, "Changed     %ld %s",	     pArea->last_changed,
					     ctime( &pArea->last_changed )  );
    if ( pArea->resetmsg && *pArea->resetmsg != '\0' )
	fprintf( fp, "Resetmsg    %s~\n",  pArea->resetmsg );
    if ( !IS_NULLSTR( pArea->norecall ) )
	fprintf( fp, "Norecall    %s~\n", pArea->norecall );

    fprintf( fp, "End\n\n" );

    save_helps( fp, pArea );
    save_mobiles( fp, pArea );
    save_objects( fp, pArea );
    save_rooms( fp, pArea );
    save_resets( fp, pArea );
    save_sectdesc( fp, pArea );
    save_shops( fp, pArea );
    save_specials( fp, pArea );
    save_travels( fp, pArea );

    if ( pArea->maxx > 0 && pArea->maxy > 0 )
	save_vrooms( fp, pArea );

    fprintf( fp, "#$\n" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    /*
     * Crufty use of offset to determine if it was an export or regular save.
     * Will have to fix this in the future but for now this is one of the RARE
     * occasions where speed is better than elegance, as long as it works.
     * 09/10/2005 REK
     */
    if ( offset == 0 )
        rename( filename, pArea->file_name );
    else
    {
        sprintf( newname, "%s%s", EXPORT_DIR, pArea->file_name );
        rename( filename, newname );
        offset = 0;
    }
    return TRUE;
}


/*
 * Save the area list.
 * Return TRUE on success, FALSE on error
 */
static bool
save_area_list( void )
{
    AREA_DATA	*pArea;
    FILE	*fp;
    char	*file;

    file = ( port == GAME_PORT ) ? AREA_LIST : AREA_LIST_BP;
    if ( ( fp = fopen( file, "w" ) ) == NULL )
    {
	perror( "Save_area_list: fopen" );
	return FALSE;
    }

    /*
     * Help files not assigned to an area
     */
    fprintf( fp, "help.are\n" );

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( pArea->min_vnum < 1 || pArea->max_vnum < pArea->min_vnum )
	{
	    bugf( "Save_area_list: bad vnums in %d (%s)",
		  pArea->vnum, pArea->name );
	    continue;
	}
	if ( *pArea->file_name == '\0' )
	{
	    bugf( "Save_area_list: Area %d (%s) filename not set",
		  pArea->vnum, pArea->name );
	    continue;
	}
	fprintf( fp, "%s\n", pArea->file_name );
    }

    fprintf( fp, "$\n" );
    fclose( fp );

    return TRUE;
}


void
save_boards_file( void )
{
    FILE *		fp;
    BOARD_DATA *	pBoard;

    if ( !changed_boards )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( TEMP_DIR BOARDS_FILE, "w" ) ) == NULL )
    {
	perror( "Save_boards_file: " TEMP_DIR BOARDS_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
	bug( "Save_boards_file: Can't open boards file.", 0 );
	return;
    }

    for ( pBoard = board_first; pBoard != NULL; pBoard = pBoard->next )
    {
	fprintf( fp, "#BOARD %s\n", pBoard->name );
	fprintf( fp, "Desc %s~\n", pBoard->desc );
	fprintf( fp, "Read %d\n", pBoard->read );
	fprintf( fp, "Write %d\n", pBoard->write );
	fprintf( fp, "Expire %d\n", pBoard->expire );
	fprintf( fp, "End\n\n" );
    }

    fprintf( fp, "#$\n" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    rename( TEMP_DIR BOARDS_FILE, SYSTEM_DIR BOARDS_FILE );

    changed_boards = FALSE;
    return;
}


void
save_cmd_files( void )
{
    FILE *	fp;
    int		iCmd;

    if ( changed_command )
    {
	fclose( fpReserve );
	if ( ( fp = fopen( TEMP_DIR COMMAND_FILE, "w" ) ) == NULL )
	{
	    perror( "Save_cmd_files: " TEMP_DIR COMMAND_FILE );
	    fpReserve = fopen( NULL_FILE, "r" );
	    bug( "Save_cmd_files: Can't open command file.", 0 );
	    return;
	}

	for ( iCmd = 0; !IS_NULLSTR( cmd_table[iCmd].name ); iCmd++ )
	{
	    if ( strchr( cmd_table[iCmd].name, '\'' ) )
		fprintf( fp, "#CMD \"%s\"\n", cmd_table[iCmd].name );
	    else if ( strchr( cmd_table[iCmd].name, '"' ) )
		fprintf( fp, "#CMD '%s'\n", cmd_table[iCmd].name );
	    else
		fprintf( fp, "#CMD %s\n", cmd_table[iCmd].name );
	    fprintf( fp, "Pos %s\n",
		     position_table[cmd_table[iCmd].position].short_name );
	    fprintf( fp, "Lvl %s\n", level_name( cmd_table[iCmd].level ) );
	    fprintf( fp, "Log %s\n", flag_string( log_types, cmd_table[iCmd].log ) );
	    fprintf( fp, "Flags %s\n", print_flags( cmd_table[iCmd].flags ) );
	    fprintf( fp, "Sec %s\n", print_flags( cmd_table[iCmd].sec_flags ) );
	    fprintf( fp, "End\n\n" );
	}

	fputs( "#$\n", fp );
	fclose( fp );

	rename( TEMP_DIR COMMAND_FILE, SYSTEM_DIR COMMAND_FILE );
	changed_command = FALSE;

	fpReserve = fopen( NULL_FILE, "r" );
    }

    if ( changed_secgroup )
    {
	fclose( fpReserve );
	if ( ( fp = fopen( TEMP_DIR SECGROUP_FILE, "w" ) ) == NULL )
	{
	    perror( "Do_cmdedit: " TEMP_DIR COMMAND_FILE );
	    fpReserve = fopen( NULL_FILE, "r" );
	    bug( "Save_cmd_files: Can't open command file.", 0 );
	    return;
	}

	for (iCmd = 0; security_flags[iCmd].name != NULL; iCmd++ )
	{
	    if ( *security_flags[iCmd].name != '\0' )
		fprintf( fp, "'%s' %s\n",
			 security_flags[iCmd].name,
			 print_flags( security_flags[iCmd].bit ) );
	}

	fputs( "#END\n", fp );
	fclose( fp );

	rename( TEMP_DIR SECGROUP_FILE, SYSTEM_DIR SECGROUP_FILE );
	changed_secgroup = FALSE;
	fpReserve = fopen( NULL_FILE, "r" );
    }
}


static bool
save_dreams( void )
{
    FILE *		fp;
    DREAM_DATA *	pDream;
    DREAMSEG_DATA *	pSeg;
    AFFECT_DATA *	paf;

    if ( !changed_dream )
        return FALSE;

    if ( ( fp = fopen( TEMP_DIR DREAM_FILE, "w" ) ) == NULL )
    {
        perror( DREAM_FILE );
        bugf( "Unable to open dream file." );
        return FALSE;
    }


    for ( pDream = dream_list; pDream != NULL; pDream = pDream->next )
    {
        fprintf( fp, "#%d\n", pDream->vnum );
        fprintf( fp, "Type %s\n", flag_string( dream_types, pDream->type ) );
        if ( !IS_NULLSTR( pDream->author ) )
            fprintf( fp, "Author '%s'\n", pDream->author );
        fprintf( fp, "Sex %s\n", sex_name( pDream->sex ) );
        if ( !IS_NULLSTR( pDream->title ) )
            fprintf( fp, "Title %s~\n", pDream->title );
        for ( paf = pDream->affect; paf != NULL; paf = paf->next )
            fprintf( fp, "Aff %d %d %d\n", paf->location, paf->modifier, paf->duration );
        if ( pDream->dream_flags != 0 )
            fprintf( fp, "Flags %s\n", print_flags( pDream->dream_flags ) );
        if ( !xIS_EMPTY( pDream->class_flags ) )
            fprintf( fp, "Class %s\n", print_xbits( &pDream->class_flags ) );
        if ( !xIS_EMPTY( pDream->race_flags ) )
            fprintf( fp, "Race %s\n", print_xbits( &pDream->race_flags ) );
        for ( pSeg = pDream->seg; pSeg != NULL; pSeg = pSeg->next )
            fprintf( fp, "Seg %s~\n", fix_string( pSeg->text ) );
        fprintf( fp, "End\n\n" );
    }

    fprintf( fp, "#0\n" );
    fprintf( fp, "\n#$\n" );

    fclose( fp );
    rename( TEMP_DIR DREAM_FILE, SYSTEM_DIR DREAM_FILE );

    changed_dream = FALSE;
    return TRUE;
}


void
save_greet_data( void )
{
    FILE *	fp;
    bool *	p;
    int		col;
    int		row;

    fp = fopen( TEMP_DIR GREET_FILE, "w" );
    p = greet_table;

    for ( row = 0; row < greet_size; row++ )
    {
	for ( col = 0; col < greet_size; col++ )
	{
	    putc( *p ? '1' : '0', fp );
	    p++;
	}
	putc( '\n', fp );
    }

    fclose( fp );
    rename( TEMP_DIR GREET_FILE, SYSTEM_DIR GREET_FILE );

    strip_event_game( EVENT_GAME_SAVE_GREET );

    return;
}

void olcautosave( void )
{
   AREA_DATA *pArea;
   DESCRIPTOR_DATA *d;                                                                                                    
   char buf[MAX_INPUT_LENGTH];                                                                                                                                                                                                                    
        save_cmd_files();
    save_race_file();
    save_skill_files();
    save_socials();
    save_vehicles();
    save_boards_file();;
        save_clans();
        for ( d = descriptor_list; d != NULL; d = d->next )                                                               
        {                                                                                                                 
           if ( d->editor)                                                                                                
                   send_to_char( "OLC Autosaving:\n\r", d->character );                                                   
           else                                                                                                           
                   log_string( "OLC Autosaving:" );                                                                       
        }                                                                                                                 
        sprintf( buf, "None.\n\r" );                                                                                      
                                                                                                                          
        for( pArea = area_first; pArea; pArea = pArea->next )                                                             
        {                                                                                                                 
            /* Save changed areas. */                                                                                     
            if ( IS_SET(pArea->area_flags, AREA_CHANGED) )                                                                
            {                                                                                                             
                save_area( pArea );                                                                                       
                sprintf( buf, "%24s - '%s'", pArea->name, pArea->file_name );                                             
                for ( d = descriptor_list; d != NULL; d = d->next )                                                       
                {                                                                                                         
                   if ( d->editor )                                                                                       
                   {                                                                                                      
                        send_to_char( buf, d->character );                                                                
                        send_to_char( "\n\r", d->character );                                                             
                   }                                                                                                      
                   else                                                                                                   
                        log_string( buf );                                                                                
                }                                                                                                         
                REMOVE_BIT( pArea->area_flags, AREA_CHANGED );                                                            
            }                                                                                                             
        }                                                                                                                 
                                                                                                                          
        if ( !str_cmp( buf, "None.\n\r" ) )                                                                               
        {                                                                                                                 
           for ( d = descriptor_list; d != NULL; d = d->next )                                                            
           {                                                                                                              
                if ( d->editor )                                                                                          
                        send_to_char( buf, d->character );                                                                
                else                                                                                                      
                        log_string( "None." );                                                                            
           }                                                                                                              
        }
}
/*
void olcautosave( void )
{
                                                                                                                        
   AREA_DATA *pArea;
   DESCRIPTOR_DATA *d;
   char buf[MAX_INPUT_LENGTH];
   
        save_cmd_files();
	save_race_file();
	save_skill_files();
	save_socials();
	save_vehicles();
	save_boards_file();;
        save_clans();
                                                                                                                        
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
                                                                                                                        
           if ( d->editor)
                   send_to_char( "OLC Autosaving:\n\r", d->character );
        }
*/
 //for( pArea = area_first; pArea; pArea = pArea->next )      
   //     {     

            /* Save changed areas. */     

     //       if ( IS_SET(pArea->area_flags, AREA_CHANGED) )     
       //     {     

         //       save_area( pArea );     
/*
for ( pArea = area_first; pArea; pArea = pArea->next )
if ( IS_SET( pArea->area_flags, AREA_CHANGED ) ) 
save_area( pArea );
                sprintf( buf, "%24s - '%s'", pArea->name, pArea->file_name );     

                for ( d = descriptor_list; d != NULL; d = d->next )     
                {                                                                                                       
                                                                                                                        
                   if ( d->editor )                                                                                     
                   {                                                                                                    
                        send_to_char( buf, d->character );                                                              
                        send_to_char( "\n\r", d->character );                                                           
                                                                                                                        
                                                                                                                        
                                                                                                                        
                REMOVE_BIT( pArea->area_flags, AREA_CHANGED );                                                          
            }                                                                                                           
                                                                                                                        
        }                                                                                                               
                                                                                                                        
        if ( !str_cmp( buf, "None.\n\r" ) )                                                                             
        {                                                                                                               
                                                                                                                        
           for ( d = descriptor_list; d != NULL; d = d->next )                                                          
           {                                                                                                            
                if ( d->editor )                                                                                        
                        send_to_char( buf, d->character );                                                              
           }                                                                                                            
                                                                                                                        
        }                                                                                                               
                                                                                                                        
        return;                                                                                                         
} 
*/                                  
/*****************************************************************************
 Name:		save_helps
 Purpose:	Save #HELPS section of an area file.
 Written by:	Walker <nkrendel@evans.Denver.Colorado.EDU>
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void
save_helps( FILE *fp, AREA_DATA *pArea )
{
    HELP_DATA *	pHelp;
    char	lev_buf[SHORT_STRING_LENGTH];
    char	elev_buf[SHORT_STRING_LENGTH];
    bool	found = FALSE;

    for( pHelp = help_first; pHelp; pHelp = pHelp->next )
    {
        if( pHelp->area && pHelp->area == pArea )
	{
	    if ( !pHelp->keyword || *pHelp->keyword == '\0' )
		continue;
	    if( !found )
	    {
		fprintf( fp, "#HELPS\n\n" );
		found = TRUE;
	    }
	    strcpy( lev_buf, level_name( pHelp->level ) );
	    strcpy( elev_buf, level_name( pHelp->ed_level ) );
	    fprintf( fp, "%s %s %s~\n%s~\n", 
		lev_buf,
		elev_buf,
		all_capitalize( pHelp->keyword ), 
		fix_string( pHelp->text ) );
	}
    }

    if( found )
        fprintf( fp, "\n0 0 $~\n\n" );

    return;
}


static void
save_mobiles( FILE *fp, AREA_DATA *pArea )
{
    MOB_INDEX_DATA *	pMob;
    MPROG_DATA *	pProg;
    const char *	type;
    int			vnum;
    int			iClass;

    fprintf( fp, "#MOBILES\n" );
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pMob = get_mob_index( vnum ) ) && pMob->area == pArea )
	{
	    fprintf( fp, "#%d\n",	fix_vnum( pArea, pMob->vnum ) );
	    fprintf( fp, "%s~\n",	pMob->player_name );
	    fprintf( fp, "%s~\n",	pMob->short_descr );
	    fprintf( fp, "%s~\n",	fix_string( pMob->long_descr ) );
	    fprintf( fp, "%s~\n",	fix_string( pMob->description ) );
	    fprintf( fp, "%s~\n",	race_table[pMob->race].name );
	    fprintf( fp, "%s ",		print_flags( pMob->act ) );
	    fprintf( fp, "%s ", 	print_xbits( &pMob->affected_by ) );
	    fprintf( fp, "%s ",		print_xbits( &pMob->shielded_by ) );
	    fprintf( fp, "%s ",		print_flags( pMob->class ) );
	    fprintf( fp, "%d ",		pMob->alignment );
	    fprintf( fp, "%d ",		pMob->ethos );
	    fprintf( fp, "%d\n",	fix_vnum( pArea, pMob->group ) );
	    fprintf( fp, "%d ",		pMob->level );
	    fprintf( fp, "%d ",		pMob->hitroll );
	    fprintf( fp, "%d%c%d+%d ",	pMob->hit[DICE_NUMBER],
					'd',
	    				pMob->hit[DICE_TYPE],
	    				pMob->hit[DICE_BONUS] );
	    fprintf( fp, "%d%c%d+%d ",	pMob->mana[DICE_NUMBER],
					'd',
					pMob->mana[DICE_TYPE],
					pMob->mana[DICE_BONUS] );
	    fprintf( fp, "%d%c%d+%d ",	pMob->damage[DICE_NUMBER],
					'd',
					pMob->damage[DICE_TYPE],
					pMob->damage[DICE_BONUS] );
	    fprintf( fp, "%s\n",	attack_table[pMob->dam_type].name );
	    fprintf( fp,"%d %d %d %d\n",pMob->ac[AC_PIERCE] / 10,
					pMob->ac[AC_BASH] / 10,
					pMob->ac[AC_SLASH] / 10,
					pMob->ac[AC_EXOTIC] / 10 );

	    fprintf( fp, "%s ",		print_xbits( &pMob->off_flags ) );
	    fprintf( fp, "%s ",		print_flags( pMob->imm_flags ) );
	    fprintf( fp, "%s ",		print_flags( pMob->res_flags ) );
	    fprintf( fp, "%s\n",	print_flags( pMob->vuln_flags ) );
	    fprintf( fp, "%s ",		position_table[pMob->start_pos].short_name );
	    fprintf( fp, "%s ", 	position_table[pMob->default_pos].short_name );
	    fprintf( fp, "%s ",		sex_name( pMob->sex ) );
	    fprintf( fp, "%ld\n",	pMob->wealth );
	    fprintf( fp, "%s ",		print_flags( pMob->form ^ race_table[pMob->race].form ) );
	    fprintf( fp, "%s ",		print_flags( pMob->parts ^ race_table[pMob->race].parts ) );
	    fprintf( fp, "%s ",		size_name( pMob->size ) );
	    fprintf( fp, "%d ",		pMob->weight );
	    fprintf( fp, "%s ",		flag_string( automob_types, pMob->autoset ) );
	    fprintf( fp, "'%s'\n",	pMob->material ? pMob->material : "none" );

	    if ( !IS_NULLSTR( pMob->clan ) )
		fprintf( fp, "C \"%s\"\n", pMob->clan );

	    if ( IS_SET( pMob->act, ACT_PRACTICE )
	    ||	 IS_SET( pMob->act, ACT_GAIN )
	    ||	 IS_SET( pMob->act, ACT_TRAIN ) )
	    {
		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		    if ( xIS_SET( pMob->train, iClass ) )
			fprintf( fp, "T '%s'\n", class_table[iClass].name );
	    }

	    for ( pProg = pMob->mobprogs; pProg; pProg = pProg->next )
	    {
		type = mprog_name( pProg->type );
		if ( !str_cmp( type, "error_prog" ) )
		    continue;
		fprintf( fp, ">%s ",	type );
		fprintf( fp, "%s~\n",	pProg->arglist );
		fprintf( fp, "%s~\n",	fix_string( pProg->comlist ) );
	    }
	    if ( pMob->mobprogs )
		fprintf( fp, "|\n" );
	}
    }
    fprintf( fp, "#0\n\n" );
    return;
}


static void
save_objects( FILE *fp, AREA_DATA *pArea )
{
    OBJ_INDEX_DATA *	pObj;
    AFFECT_DATA *	pAf;
    EXTRA_DESCR_DATA *	pEd;
    int			vnum;
    int			i;

    fprintf( fp, "#OBJECTS\n" );

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pObj = get_obj_index( vnum ) ) != NULL && pObj->area == pArea )
	{
	    fprintf( fp, "#%d\n",	fix_vnum( pArea, pObj->vnum ) );
	    fprintf( fp, "%s~\n",	pObj->name );
	    fprintf( fp, "%s~\n",	pObj->short_descr );
	    fprintf( fp, "%s~\n",	fix_string( pObj->description ) );

	    if ( !xIS_EMPTY( pObj->material ) )
		fprintf( fp, "%s", xbit_string( material_types, pObj->material ) );
	    fprintf( fp, "~\n" );

	    fprintf( fp, "%s ",		item_name( pObj->item_type ) );
	    fprintf( fp, "%s ",		print_flags( pObj->extra_flags ) );
		fprintf( fp, "%s ",		print_flags( pObj->extra_flags2 ) );
	    fprintf( fp, "%s\n",	print_flags( pObj->wear_flags ) );

	    switch( pObj->item_type )
	    {
	    case ITEM_WEAPON:
		fprintf( fp, "%s ",	weapon_name( pObj->value[0] ) );
		fprintf( fp, "%d ",	pObj->value[1] );
		fprintf( fp, "%d ",	pObj->value[2] );
		fprintf( fp, "%s ",	attack_table[pObj->value[3]].name );
		fprintf( fp, "%s ",	print_flags( pObj->value[4] ) );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    case ITEM_BOOK:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "%s ",	print_flags( pObj->value[1] ) );
		fprintf( fp, "%d ",	fix_vnum( pArea, pObj->value[2] ) );
		fprintf( fp, "%d ",	pObj->value[3] );
		fprintf( fp, "%d ",	pObj->value[4] );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    case ITEM_CONTAINER:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "%s ",	print_flags( pObj->value[1] ) );
		fprintf( fp, "%d ",	fix_vnum( pArea, pObj->value[2] ) );
		fprintf( fp, "%d ",	pObj->value[3] );
		fprintf( fp, "%d ",	pObj->value[4] );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    case ITEM_DRINK_CON:
	    case ITEM_FOUNTAIN:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "%d ",	pObj->value[1] );
		fprintf( fp, "'%s' ",	liq_table[pObj->value[2]].liq_name );
		fprintf( fp, "%d ",	pObj->value[3] );
		fprintf( fp, "%d ",	pObj->value[4] );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    case ITEM_LENS:
	    case ITEM_WAND:
	    case ITEM_STAFF:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "%d ",	pObj->value[1] );
		fprintf( fp, "%d ",	pObj->value[2] );
		fprintf( fp, "'%s' ",	pObj->value[3] != NO_SKILL ?
					skill_table[pObj->value[3]].name
					: "" );
		fprintf( fp, "%d ",	pObj->value[4] );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    case ITEM_PILL:
	    case ITEM_POTION:
	    case ITEM_SCROLL:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "'%s' ",	pObj->value[1] != NO_SKILL ?
					skill_table[pObj->value[1]].name
					: "" );
		fprintf( fp, "'%s' ",	pObj->value[2] != NO_SKILL ?
					skill_table[pObj->value[2]].name
					: "" );
		fprintf( fp, "'%s' ",	pObj->value[3] != NO_SKILL ?
					skill_table[pObj->value[3]].name
					: "" );
		fprintf( fp, "'%s' ",	pObj->value[4] != NO_SKILL ?
					skill_table[pObj->value[4]].name
					: "" );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    case ITEM_HERB:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "'%s' ",	pObj->value[1] != NO_SKILL ?
					skill_table[pObj->value[1]].name
					: "" );
		fprintf( fp, "'%s' ",	pObj->value[2] != NO_SKILL ?
					skill_table[pObj->value[2]].name
					: "" );
		fprintf( fp, "'%s' ",	pObj->value[3] != NO_SKILL ?
					skill_table[pObj->value[3]].name
					: "" );
		fprintf( fp, "%d\n",	pObj->value[4] );
		fprintf( fp, "%s\n",	print_flags( pObj->value[5] ) );
		break;
	    case ITEM_FURNITURE:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "%d ",	pObj->value[1] );
		fprintf( fp, "%s ",	print_flags( pObj->value[2] ) );
		fprintf( fp, "%d ",	pObj->value[3] );
		fprintf( fp, "%d ",	pObj->value[4] );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    case ITEM_PORTAL:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "%d ",	pObj->value[1] );
		fprintf( fp, "%d ",	pObj->value[2] );
		fprintf( fp, "%d ",	fix_vnum( pArea, pObj->value[3] ) );
		fprintf( fp, "%d ",	pObj->value[4] );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    case ITEM_SHEATH:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "%s ",	print_flags( pObj->value[1] ) );
		fprintf( fp, "%d ",	pObj->value[2] );
		fprintf( fp, "%s ",	weapon_name( pObj->value[3] ) );
		fprintf( fp, "%s ",	weapon_name( pObj->value[4] ) );
		fprintf( fp, "%s\n ",	weapon_name( pObj->value[5] ) );
		break;
		case ITEM_TOKEN:
		fprintf( fp, "%d ",	pObj->value[0] );
		break;
	    default:
		fprintf( fp, "%d ",	pObj->value[0] );
		fprintf( fp, "%d ",	pObj->value[1] );
		fprintf( fp, "%d ",	pObj->value[2] );
		fprintf( fp, "%d ",	pObj->value[3] );
		fprintf( fp, "%d ",	pObj->value[4] );
		fprintf( fp, "%d\n",	pObj->value[5] );
		break;
	    }

	    fprintf( fp, "%d ",		pObj->level );
	    fprintf( fp, "%d ",		pObj->weight );
	    fprintf( fp, "%d ",		pObj->cost );
		fprintf( fp, "%d ",		pObj->qcost );
	    fprintf( fp, "%c\n",	pObj->condition >= 100 ? 'P' :
					pObj->condition >=  90 ? 'G' :
					pObj->condition >=  75 ? 'A' :
					pObj->condition >=  50 ? 'W' :
					pObj->condition >=  25 ? 'D' :
					pObj->condition >=  10 ? 'B' :
					'R' );

	    fprintf( fp, "%d ",		pObj->ac_type );
	    if ( pObj->ac_type == INVOKE_SPELL )
		fprintf( fp, "'%s' ",	pObj->ac_vnum ?
					skill_table[pObj->ac_vnum].name
					: "" );
	    else
		fprintf( fp, "%d ",	fix_vnum( pArea, pObj->ac_vnum ) );
	    fprintf( fp, "%d ",		pObj->ac_charge[0] );
	    fprintf( fp, "%d\n",	pObj->ac_charge[1] );
	    fprintf( fp, "%d ",		fix_vnum( pArea, pObj->join ) );
	    fprintf( fp, "%d ",		fix_vnum( pArea, pObj->sep_one ) );
	    fprintf( fp, "%d ",		fix_vnum( pArea, pObj->sep_two ) );
	    fprintf( fp, "%d ",		pObj->timer );
	    fprintf( fp, "%d\n",	pObj->duration );
	    for ( i = 1; !IS_NULLSTR( race_table[i].name ); i++ )
	        if ( xIS_SET( pObj->race_flags, i ) )
	            fprintf( fp, "R '%s'\n", race_table[i].name );
	    for ( i = 0; i < MAX_CLASS; i++ )
	        if ( xIS_SET( pObj->class_flags, i ) )
	            fprintf( fp, "R '%s'\n", class_table[i].name );
	    for ( pAf = pObj->affected; pAf; pAf = pAf->next )
		fprintf( fp, "A\n%d %d\n",  pAf->location, pAf->modifier );

	    for ( pEd = pObj->extra_descr; pEd; pEd = pEd->next )
		fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
					fix_string( pEd->description ) );
	    if ( pObj->item_type == ITEM_BOOK )
	    {
	        TEXT_DATA *	page;
	        for ( page = pObj->page; page != NULL; page = page->next )
	            fprintf( fp, "P\n%s~\n", fix_string( page->text ) );
	    }
	    save_oreprogs( fp, pObj->oprogs, oprog_types );
	}
    }

    fprintf( fp, "#0\n\n" );
    return;
}


static void
save_oreprogs( FILE *fp, OREPROG_DATA *pFirst,
			   const struct flag_type *flag_table )
{
    OREPROG_DATA *pProg;

    if ( !pFirst )
	return;

    for ( pProg = pFirst; pProg; pProg = pProg->next )
    {
	if ( !str_cmp( flag_string( flag_table, pProg->type ), "error_prog" ) ||
	     !str_cmp( flag_string( flag_table, pProg->type ), "none" ) )
	    continue;
	fprintf( fp, ">%s %s~\n", flag_string( flag_table, pProg->type ),
		 pProg->arglist );
	fprintf( fp, "%s~\n", fix_string( pProg->comlist ) );
    }
    fprintf( fp, "|\n" );
    return;
}


void
save_projects( void )
{
    FILE *		fp;
    PROJECT_DATA *	pProj;

    if ( !changed_project )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( TEMP_DIR PROJECT_FILE, "w" ) ) == NULL )
    {
	perror( "Save_projects: " TEMP_DIR PROJECT_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
	bug( "Save_projects: can't open file.", 0 );
	return;
    }

    for ( pProj = project_list; pProj != NULL; pProj = pProj->next )
    {
	if ( pProj->deleted
	||   ( pProj->completed != 0 && pProj->completed + 7 * 24 * 60 * 60 < current_time ) )
	    continue;

	fprintf( fp, "#PROJECT\n" );
	fprintf( fp, "Name %s~\n", pProj->name );
	fprintf( fp, "Vnum %d\n",  pProj->vnum );
	fprintf( fp, "Pri %s\n", flag_string( priority_flags, pProj->priority ) );
	fprintf( fp, "From %s~\n", pProj->assigner );
	fprintf( fp, "To %s~\n", pProj->assignee );
	fprintf( fp, "Date %ld\n", pProj->date );
	fprintf( fp, "Due %ld\n", pProj->deadline );
	fprintf( fp, "Comp %ld\n", pProj->completed );
	fprintf( fp, "Status %s~\n", pProj->status );
	fprintf( fp, "Desc\n%s~\n", fix_string( pProj->description ) );
	fprintf( fp, "Progress\n%s~\n", fix_string( pProj->progress ) );
	fprintf( fp, "End\n\n" );
    }

    fprintf( fp, "#$\n" );

    fclose( fp );

    rename( TEMP_DIR PROJECT_FILE, SYSTEM_DIR PROJECT_FILE );
    changed_project = FALSE;

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


void
save_race_file( void )
{
    FILE *	fp;
    int		i;
    int		iRace;

    if ( !changed_race )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( TEMP_DIR RACE_FILE, "w" ) ) == NULL )
    {
	perror( "Save_race_file: " TEMP_DIR RACE_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
	bug( "Save_race_file: can't open file.", 0 );
	return;
    }

    for ( iRace = 0; !IS_NULLSTR( race_table[iRace].name ); iRace++ )
    {
	if ( !str_cmp( race_table[iRace].name, "unique" ) )
	    continue;
	fprintf( fp, "#RACE '%s'\n", race_table[iRace].name );
	fprintf( fp, "Act %s\n", print_flags( race_table[iRace].act ) );
	fprintf( fp, "Aff %s\n", print_xbits( &race_table[iRace].aff ) );
	fprintf( fp, "AlMax %d\n", race_table[iRace].max_align );
	fprintf( fp, "AlMin %d\n", race_table[iRace].min_align );
	fprintf( fp, "AlMax %d\n", race_table[iRace].max_ethos );
	fprintf( fp, "AlMin %d\n", race_table[iRace].min_ethos );
	fprintf( fp, "Form %s\n", print_flags( race_table[iRace].form ) );
	fprintf( fp, "Imm %s\n", print_flags( race_table[iRace].imm ) );
	fprintf( fp, "Max %d %d %d %d %d\n",
		 race_table[iRace].max_stats[STAT_STR],
		 race_table[iRace].max_stats[STAT_INT],
		 race_table[iRace].max_stats[STAT_WIS],
		 race_table[iRace].max_stats[STAT_DEX],
		 race_table[iRace].max_stats[STAT_CON] );
	fprintf( fp, "Off %s\n", print_xbits( &race_table[iRace].off ) );
	fprintf( fp, "Parts %s\n", print_flags( race_table[iRace].parts ) );
	fprintf( fp, "Pc %s\n", race_table[iRace].pc_race ? "TRUE" : "FALSE" );
	fprintf( fp, "Points %d\n", race_table[iRace].points );
	fprintf( fp, "Res %s\n", print_flags( race_table[iRace].res ) );
	fprintf( fp, "Size %s\n", size_name( race_table[iRace].size ) );
	for ( i = 0; i < MAX_RACE_SKILLS; i++ )
	    if ( !IS_NULLSTR( race_table[iRace].skills[i] ) )
		fprintf( fp, "Skill '%s'\n", race_table[iRace].skills[i] );
	    else
		fprintf( fp, "Skill NULL\n" );
	fprintf( fp, "Stat %d %d %d %d %d\n",
		 race_table[iRace].stats[STAT_STR],
		 race_table[iRace].stats[STAT_INT],
		 race_table[iRace].stats[STAT_WIS],
		 race_table[iRace].stats[STAT_DEX],
		 race_table[iRace].stats[STAT_CON] );
	fprintf( fp, "Vuln %s\n", print_flags( race_table[iRace].vuln ) );
	fprintf( fp, "WtMin %d\n", race_table[iRace].weight_min );
	fprintf( fp, "WtMax %d\n", race_table[iRace].weight_max );
	for ( i = 0; i < MAX_CLASS; i++ )
	    fprintf( fp, "'%s' %d\n", class_table[i].name,
		     race_table[iRace].class_mult[i] );
	fprintf( fp, "End\n\n" );
    }

    fputs( "#$\n", fp );
    fclose( fp );

    rename( TEMP_DIR RACE_FILE, SYSTEM_DIR RACE_FILE );

    changed_race = FALSE;
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


static void
save_resets( FILE *fp, AREA_DATA *pArea )
{
    RESET_DATA *	pReset;
    MOB_INDEX_DATA *	pLastMob = NULL;
    OBJ_INDEX_DATA *	pLastObj;
    ROOM_INDEX_DATA *	pRoom;
    char		buf[MAX_STRING_LENGTH];
    int			vnum;

    fprintf( fp, "#RESETS\n" );

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pRoom = get_room_index( vnum ) ) != NULL )
	{
	    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
	    {
		switch ( pReset->command )
		{
		default:
		    bugf( "Save_resets: bad command %c in [%s] %d.",
			  pReset->command, pArea->file_name, pRoom->vnum );
		    break;

		case 'M':
	            pLastMob = get_mob_index( pReset->arg1 );
		    fprintf( fp, "M 0 %d %d %d %d\n", 
		        fix_vnum( pArea, pReset->arg1 ),
	                pReset->arg2,
	                fix_vnum( pArea, pReset->arg3 ),
	                pReset->arg4 );
	            break;

		case 'O':
	            pLastObj = get_obj_index( pReset->arg1 );
		    fprintf( fp, "O 0 %d %d %d\n", 
		        fix_vnum( pArea, pReset->arg1 ),
		        pReset->arg2,
	                fix_vnum( pArea, pReset->arg3 ) );
	            break;

		case 'P':
	            pLastObj = get_obj_index( pReset->arg1 );
		    fprintf( fp, "P 0 %d %d %d %d\n", 
		        fix_vnum( pArea, pReset->arg1 ),
		        pReset->arg2,
	                fix_vnum( pArea, pReset->arg3 ),
	                pReset->arg4 );
	            break;

		case 'G':
	            if ( pLastMob == NULL )
	            {
	                sprintf( buf,
	                    "Save_resets: !NO_MOB! in [%s] #%d",
	                    pArea->file_name, pRoom->vnum );
	                bug( buf, 0 );
	            }
	            else
			fprintf( fp, "G 1 %d %d\n", 
			         fix_vnum( pArea, pReset->arg1 ),
				 pReset->arg2 ? pReset->arg2 : -1 );
	            break;

		case 'E':
	            if ( pLastMob == NULL )
	            {
	                sprintf( buf,
	                    "Save_resets: !NO_MOB! in [%s] %d",
	                    pArea->file_name, fix_vnum( pArea, pRoom->vnum ) );
	                bug( buf, 0 );
	            }
	            else
			fprintf( fp, "E 1 %d %d %d\n",
		                 fix_vnum( pArea, pReset->arg1 ),
		                 pReset->arg2 ? pReset->arg2 : -1,
		                 pReset->arg3 );
	            break;

		case 'D':
	            break;

		case 'R':
		    fprintf( fp, "R 0 %d %d\n", 
		        fix_vnum( pArea, pReset->arg1 ),
	                pReset->arg2 );
	            break;
	        }	/* End switch */
	    }	/* End for pReset */
	}  /* End if pRoom */
    }  /* End for Vnum */

    fprintf( fp, "S\n\n" );
    return;
}


static void
save_rooms( FILE *fp, AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *	pRoom;
    EXTRA_DESCR_DATA *	pEd;
    EXIT_DATA *		pExit;
    int			door;
    int			vnum;

    fprintf( fp, "#ROOMS\n" );

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pRoom = get_room_index( vnum ) ) != NULL && pRoom->area == pArea )
	{
	    fprintf( fp, "#%d\n", fix_vnum( pArea, pRoom->vnum ) );
	    fprintf( fp, "%s~\n", pRoom->name );
	    fprintf( fp, "%s~\n", fix_string( pRoom->description ) );
	    fprintf( fp, "%s~\n", fix_string( pRoom->morning_desc ) );
	    fprintf( fp, "%s~\n", fix_string( pRoom->evening_desc ) );
	    fprintf( fp, "%s~\n", fix_string( pRoom->night_desc ) );
	    fprintf( fp, "%s~\n", fix_string( pRoom->sound ) );
	    fprintf( fp, "%s~\n", fix_string( pRoom->smell ) );
	    fprintf( fp, "0 " );	/* old area vnum; remove */
	    fprintf( fp, "%s ",	  print_flags( pRoom->room_flags ) );
	    fprintf( fp, "%s ",   print_flags( pRoom->affect_flags ) );
	    fprintf( fp, "%d\n",  pRoom->sector_type );

	    if ( pRoom->heal_rate != 100 )
	    {
		fprintf( fp, "H %d", pRoom->heal_rate );
		fprintf( fp, pRoom->mana_rate == 100 ? "\n" : " " );
	    }
	    if ( pRoom->mana_rate != 100 )
	    {
		fprintf( fp, "M %d\n", pRoom->mana_rate );
	    }

	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if ( ( pExit = pRoom->exit[door] ) != NULL )
		{
		    if ( pExit->to_room != NULL
		    &&	 pExit->to_room->area->travel_curr != NULL
		    &&	 pExit->to_room == pExit->to_room->area->travel_room
		    &&	 pExit->to_room->area->travel_curr->room == pRoom )
			continue;
		    fprintf( fp, "D%s %s\n", dir_name[door], size_name( pExit->size ) );
		    fprintf( fp, "%s~\n", fix_string( pExit->sound_closed ) );
		    fprintf( fp, "%s~\n", fix_string( pExit->sound_open ) );
		    fprintf( fp, "%s~\n", fix_string( pExit->description ) );
		    fprintf( fp, "%s~\n", pExit->keyword );
		    fprintf( fp, "%d %d %d\n",
				pExit->rs_flags, 
				fix_vnum( pArea, pExit->key ),
				pExit->to_room ? fix_vnum( pArea, pExit->to_room->vnum ) : 0 );
		    save_oreprogs( fp, pExit->eprogs, eprog_types );
		}
	    }

	    for ( pEd = pRoom->extra_descr; pEd; pEd = pEd->next )
	    {
		fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
			 fix_string( pEd->description ) );
	    }

	    if ( pRoom->vehicle_type != VEHICLE_NONE )
	    {
		fprintf( fp, "V %s\n%s~\n",
			 flag_string( vehicle_types, pRoom->vehicle_type ),
			 pRoom->short_descr );
	    }

	    fprintf( fp, "S\n" );
	    save_oreprogs( fp, pRoom->rprogs, rprog_types );
	}
    }

    fprintf( fp, "#0\n\n" );
    return;
}


static bool
save_sectdesc( FILE *fp, AREA_DATA *pArea )
{
    int		i;
    int		j;
    char	secname[SHORT_STRING_LENGTH];
    char	sectype[SHORT_STRING_LENGTH];

    if ( pArea->overland == NULL )
        return FALSE;

    for ( i = 0; i < SECT_MAX; i++ )
    {
        strcpy( secname, flag_string( sector_types, i ) );
        for ( j = 0; j < 4; j++ )
        {
            if ( !IS_NULLSTR( pArea->overland->sect_desc[i][j] ) )
            {
                strcpy( sectype, flag_string( rdesc_types, j ) );
                fprintf( fp, "#SECTOR %s %s\n", secname, sectype );
                fprintf( fp, "%s~\n", fix_string( pArea->overland->sect_desc[i][j] ) );
            }
        }
    }

    return TRUE;
}


bool
save_skill_files( void )
{
    FILE *	fp;
    int		gn;
    int		iClass;
    int		index;
    char *	p;
    int		psn;
    int		sn;

    if ( changed_skill )
    {
	fclose( fpReserve );
	if ( ( fp = fopen( TEMP_DIR SKILL_FILE, "w" ) ) == NULL )
	{
	    fpReserve = fopen( NULL_FILE, "r" );
	    bug( "Save_skill_files: can't open skill file.", 0 );
	    return TRUE;;
	}

	for ( sn = 1; sn < top_skill; sn++ )
	{
	    if ( IS_NULLSTR( skill_table[sn].name ) )
		break;
	    fprintf( fp, "#SKILL '%s'\n", skill_table[sn].name );
	    fprintf( fp, "%d %d %d\n",
		     skill_table[sn].min_mana,
		     skill_table[sn].beats,
		     0 );

	    if ( skill_table[sn].spell_flags != 0 )
	        fprintf( fp, "Flags %s\n", print_flags( skill_table[sn].spell_flags ) );
	    if ( !IS_NULLSTR( skill_table[sn].noun_damage ) )
		fprintf( fp, "Dammsg %s~\n", skill_table[sn].noun_damage );
	    if ( skill_table[sn].forget != 0 )
		fprintf( fp, "Forget %d\n", skill_table[sn].forget );
	    if ( skill_table[sn].msg_off && skill_table[sn].msg_off[0] != '\0' )
		fprintf( fp, "Offmsg %s~\n", skill_table[sn].msg_off );
	    if ( skill_table[sn].msg_obj && skill_table[sn].msg_obj[0] != '\0' )
		fprintf( fp, "Objmsg %s~\n", skill_table[sn].msg_obj );
	    for ( index = 0; index < MAX_PREREQ; index++ )
	    {
		psn = skill_table[sn].prereq[index];
		if ( psn > 0 )
		    fprintf( fp, "Prereq '%s'\n", skill_table[psn].name );
	    }
	    if ( skill_table[sn].msg_room && skill_table[sn].msg_room[0] != '\0' )
		fprintf( fp, "Roommsg %s~\n", skill_table[sn].msg_room );

	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    {
		if ( class_table[iClass].who_name == NULL
		  || class_table[iClass].who_name[0] == '\0' )
		    break;
		if ( skill_table[sn].skill_level[iClass] != 0
		  && skill_table[sn].skill_level[iClass] != L_APP )
		    fprintf( fp, "  %s %d %d\n",
			     class_table[iClass].who_name,
			     skill_table[sn].skill_level[iClass],
			     skill_table[sn].rating[iClass] );
	    }
	    fprintf( fp, "End\n\n" );
	}

	fprintf( fp, "#$\n" );
	fclose( fp );
	rename( TEMP_DIR SKILL_FILE, SYSTEM_DIR SKILL_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
	changed_skill = FALSE;
    }

    if ( changed_group )
    {
	fclose( fpReserve );
	if ( ( fp = fopen( TEMP_DIR GSKILL_FILE, "w" ) ) == NULL )
	{
	    fpReserve = fopen( NULL_FILE, "r" );
	    bug( "Save_skill_files: can't open skills file", 0 );
	    return TRUE;
	}

	for ( gn = 0; gn < MAX_GROUP; gn++ )
	{
	    if ( IS_NULLSTR( group_table[gn].name ) )
		break;

	    fprintf( fp, "#GROUP '%s'\n", group_table[gn].name );

	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    {
		if ( class_table[iClass].who_name == NULL
		  || class_table[iClass].who_name[0] == '\0' )
		    break;
		if ( group_table[gn].rating[iClass] != -1 )
		{
		    fprintf( fp, "Cla %s %d\n",
			     class_table[iClass].who_name,
			     group_table[gn].rating[iClass] );
		}
	    }
	    for ( sn = 0; sn < MAX_IN_GROUP; sn++ )
	    {
		if ( !group_table[gn].spells[sn] || *group_table[gn].spells[sn] == '\0' )
		    continue;
		if ( group_find( group_table[gn].spells[sn] ) != NO_SKILL )
		    p = "Grp";
		else if ( skill_find( group_table[gn].spells[sn] ) != NO_SKILL )
		    p = "Skl";
		else
		{
		    bugf( "Gedit save: bad skill '%s' in group '%s'", group_table[gn].spells[sn], group_table[gn].name );
		    continue;
		}
		fprintf( fp, "%s '%s'\n", p, group_table[gn].spells[sn] );
	    }
	    fprintf( fp, "End\n\n" );
	}

	fprintf( fp, "#$\n" );
	fclose( fp );
	rename( TEMP_DIR GSKILL_FILE, SYSTEM_DIR GSKILL_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
	changed_group = FALSE;
    }

    return FALSE;
}


static void
save_shops( FILE *fp, AREA_DATA *pArea )
{
    MOB_INDEX_DATA *	pMob;
    SHOP_DATA *		pShop;
    int			iTrade;
    int			vnum;

    fprintf( fp, "#SHOPS\n" );
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pMob = get_mob_index( vnum ) ) != NULL
		&& pMob->area == pArea
		&& ( pShop = pMob->pShop ) )
	{
	    fprintf( fp, "%d ", pShop->keeper );
	    for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    {
		if ( pShop->buy_type[iTrade] != 0 )
		{
		    fprintf( fp, "%2d ", pShop->buy_type[iTrade] );
		}
		else
		    fprintf( fp, " 0 ");
	    }
	    fprintf( fp, "%8d %3d ", pShop->profit_buy, pShop->profit_sell );
	    fprintf( fp, "%4d %2d\n", pShop->open_hour, pShop->close_hour );
	}
    }

    fprintf( fp, "0\n\n" );
    return;
}


void
save_socials( void )
{
    FILE *		fp;
    SOCIAL_DATA *	pSave;

    if ( !changed_social )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( TEMP_DIR SOCIAL_FILE, "w" ) ) == NULL )
    {
	bug( "Sedit: fopen", 0 );
	perror( SOCIAL_FILE );
    }
    else
    {
	for ( pSave = social_first; pSave; pSave = pSave->next )
	{
	    if ( pSave->deleted )
		continue;
	    fprintf( fp, "#SOCIAL\n" );
	    fprintf( fp, "Name %s\n", pSave->name );
	    fprintf( fp, "Level %s\n",
		     level_name( pSave->ed_level ) );
	    if ( !IS_NULLSTR( pSave->cnoarg ) )
		fprintf( fp, "Cnoarg %s~\n", pSave->cnoarg );
	    if ( !IS_NULLSTR( pSave->onoarg ) )
		fprintf( fp, "Onoarg %s~\n", pSave->onoarg );
	    if ( !IS_NULLSTR( pSave->cfound ) )
		fprintf( fp, "Cfound %s~\n", pSave->cfound );
	    if ( !IS_NULLSTR( pSave->ofound ))
		fprintf( fp, "Ofound %s~\n", pSave->ofound );
	    if ( !IS_NULLSTR( pSave->vfound ) )
		fprintf( fp, "Vfound %s~\n", pSave->vfound );
	    if ( !IS_NULLSTR( pSave->nfound ) )
		fprintf( fp, "Nfound %s~\n", pSave->nfound );
	    if ( !IS_NULLSTR( pSave->cself ) )
		fprintf( fp, "Cself %s~\n", pSave->cself );
	    if ( !IS_NULLSTR( pSave->oself ) )
		fprintf( fp, "Oself %s~\n", pSave->oself );
	    fprintf( fp, "End\n\n" );
	}
	fprintf( fp, "#END\n" );
	fclose( fp );

	rename ( TEMP_DIR SOCIAL_FILE, SYSTEM_DIR SOCIAL_FILE );
	changed_social = FALSE;
    }

    fpReserve = fopen( NULL_FILE, "r" );
}


void
save_sysconfig( void )
{
    FILE *fp;

    if ( !changed_system )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( TEMP_DIR CONFIG_FILE, "w" ) ) == NULL )
    {
	fpReserve = fopen( NULL_FILE, "r" );
	bug( "Save_sysconfig: Unable to open file.", 0 );
	return;
    }

    fprintf( fp, "Pkill %d\n", sys_pkill );
    fprintf( fp, "Webwho %d\n", sys_webwho );

    fprintf( fp, "End\n\r" );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    rename( TEMP_DIR CONFIG_FILE, SYSTEM_DIR CONFIG_FILE );
    changed_system = FALSE;
    return;
}


static void
save_specials( FILE *fp, AREA_DATA *pArea )
{
    MOB_INDEX_DATA *	pMob;
    int			vnum;

    fprintf( fp, "#SPECIALS\n" );
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pMob = get_mob_index( vnum ) ) != NULL
		&& pMob->area == pArea
		&& pMob->spec_fun )
	    fprintf( fp, "M %d %s\n", fix_vnum( pArea, vnum ),
	             spec_name( pMob->spec_fun ) );
    }

    fprintf( fp, "S\n\n" );
    return;
}


static void
save_travels( FILE *fp, AREA_DATA *pArea )
{
    TRAVEL_DATA *	pTravel;

    if ( pArea->travel_vnum == 0 || pArea->travel_first == NULL )
	return;

    fprintf( fp, "#TRAVELS\n" );
    fprintf( fp, "%d\n", fix_vnum( pArea, pArea->travel_vnum ) );

    for ( pTravel = pArea->travel_first; pTravel != NULL; pTravel = pTravel->next )
    {
	fprintf( fp, "%d %d %d %d\n",
		 fix_vnum( pArea, pTravel->room_vnum ), pTravel->exit_dir,
		 pTravel->stop_timer, pTravel->move_timer );
	fprintf( fp, "%s~\n", fix_string( pTravel->arrive_travel ) );
	fprintf( fp, "%s~\n", fix_string( pTravel->arrive_room	 ) );
	fprintf( fp, "%s~\n", fix_string( pTravel->depart_travel ) );
	fprintf( fp, "%s~\n", fix_string( pTravel->depart_room	 ) );
    }    

    fprintf( fp, "0\n\n" );
    return;
}


void
save_vehicles( void )
{
    FILE *		fp;
    ROOM_INDEX_DATA *	pRoom;
    int			iHash;

    fclose( fpReserve );
    if ( ( fp = fopen( TEMP_DIR VEHICLE_FILE, "w" ) ) == NULL )
    {
	perror( "Save_vehicles: " TEMP_DIR VEHICLE_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoom = room_index_hash[iHash]; pRoom != NULL; pRoom = pRoom->next )
	{
	    if ( pRoom->vehicle_type != VEHICLE_NONE
	    &&	 pRoom->in_room != NULL )
		fprintf( fp, "%d %d\n", pRoom->vnum, pRoom->in_room->vnum );
	}
    }

    fprintf( fp, "0\n" );
    vehicle_moved = FALSE;

    fclose( fp );
    rename( TEMP_DIR VEHICLE_FILE, SYSTEM_DIR VEHICLE_FILE );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


static void
save_userlist_force( void )
{
    EVENT_DATA *	pEvent;

    if ( ( pEvent = get_event_game( EVENT_GAME_SAVE_USERLIST ) ) == NULL )
	return;

    save_userlist( );
    strip_event_game( EVENT_GAME_SAVE_USERLIST );
    return;
}


static void
save_vrooms( FILE *fp, AREA_DATA *pArea )
{
    VIRTUAL_ROOM_DATA *	pMap;
    int 		index;
    int			size;

    if ( pArea->overland == NULL )
	return;

    if ( ( pMap = pArea->overland->map ) == NULL )
    {
	bugf( "Save_vrooms: Area %s has no map.", pArea->name );
	return;
    }

    size = pArea->maxx * pArea->maxy;
    fprintf( fp, "#VROOMS\n" );
    for ( index = 0; index < size; index++ )
	fprintf( fp, "%d %d\n", pMap[index].flags, pMap[index].sector_type );

    fprintf( fp, "%d\n\n", -1 );
    return;
}

