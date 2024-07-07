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
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gd.h>
#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "olc.h"
#include "db.h"
#include "recycle.h"
#include "tables.h"


/*
 * Local functions
 */
static bool	aedit_travel_add    args( ( CHAR_DATA *ch, char *argument ) );
static bool	aedit_travel_delete args( ( CHAR_DATA *ch, char *argument ) );
static bool	aedit_travel_edit   args( ( CHAR_DATA *ch, char *argument ) );
static void	aedit_travel_show   args( ( CHAR_DATA *ch, char *argument ) );
static bool	aedit_travel_create args( ( CHAR_DATA *ch, char *argument ) );
static bool	check_range	args( ( int lower, int upper ) );
static void	check_travel	args( ( TRAVEL_DATA *pTravel ) );
static bool	dump_ascii	args( ( CHAR_DATA *ch, char *filebase ) );
static bool	dump_html	args( ( CHAR_DATA *ch, char *filebase ) );
static bool	dump_jpeg	args( ( CHAR_DATA *ch, char *filebase, char *size ) );
static bool	dump_png	args( ( CHAR_DATA *ch, char *filebase, char *size ) );
static int	line_x_major	args( ( unsigned x0, unsigned y0, unsigned dx,
					unsigned dy, int dir, int maxx,
					int sector, VIRTUAL_ROOM_DATA *map ) );
static int	line_y_major	args( ( unsigned x0, unsigned y0, unsigned dx,
					unsigned dy, int dir, int maxx,
					int sector, VIRTUAL_ROOM_DATA *map ) );
static bool	overland_create args( ( CHAR_DATA *ch, char *argument ) );
static bool	overland_desc	args( ( CHAR_DATA *ch, char *argument ) );
static bool	overland_delete	args( ( CHAR_DATA *ch, char *argument ) );
static bool	overland_dump	args( ( CHAR_DATA *ch, char *argument ) );
static bool	overland_edesc	args( ( CHAR_DATA *ch, char *argument ) );
static bool	overland_help	args( ( CHAR_DATA *ch, char *argument ) );
static bool	overland_import	args( ( CHAR_DATA *ch, char *argument ) );
static bool	overland_mdesc	args( ( CHAR_DATA *ch, char *argument ) );
static bool	overland_ndesc	args( ( CHAR_DATA *ch, char *argument ) );
static bool	travedit_setdir	args( ( CHAR_DATA *ch, int dir ) );

/*
 * Area flags which only the Head Builder can change
 */
#define HBUILDER_FLAGS ( AREA_PROTOTYPE | \
                         AREA_CLAN |\
                         AREA_MUDSCHOOL |\
                         AREA_QUEST |\
                         AREA_TRAVEDIT |\
                         AREA_ARENA |\
                         AREA_NOLIST )


void
do_aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    char		command[MAX_INPUT_LENGTH];
    int			vnum;
    ROOM_INDEX_DATA *	pRoom;

    argument = one_argument( argument, command );
    pArea = ch->in_room->area;

    if( IS_NPC ( ch ) )
    {
	send_to_char( "Mobs dont build, they are built!\n\r", ch );
	return;
    }

    /*
     * Builder defaults to editing current area.
     */
    if ( command[0] == '\0' )
    {
	ch->desc->pEdit = (void *)pArea;
	ch->desc->editor = ED_AREA;
	aedit_show( ch, "" );
	return;
    }

    if ( command[0] == 'r' && !str_prefix( command, "reset" ) )
    {
	if ( ch->desc->editor == ED_AREA )
	    reset_area( (AREA_DATA *)ch->desc->pEdit );
	else
	    reset_area( pArea );
	send_to_char( "Area reset.\n\r", ch );
	return;
    }

    if ( command[0] == 'p' && !str_prefix( command, "purge" ) )
    {
	if ( ch->desc->editor == ED_AREA )
	    pArea = (AREA_DATA *)ch->desc->pEdit;
	if ( !IS_BUILDER( ch, pArea ) )
	{
	    send_to_char( "AEdit:  Insufficient security to purge area.\n\r", ch );
	    return;
	}
	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
	    if ( ( pRoom = get_room_index( vnum ) ) != NULL )
	        purge_room( pRoom );
	}
	if ( pArea->overland != NULL )
	    for ( pRoom = pArea->overland->room_first; pRoom != NULL; pRoom = pRoom->next )
	        purge_room( pRoom );
	send_to_char( "Area purged.\n\r", ch );
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	if ( !can_aedit_all( ch, NULL ) )
	{
	    send_to_char( "AEdit:  Insufficient security to create area.\n\r", ch );
	    return;
	}
	if ( aedit_create( ch, argument ) )
	{
	    ch->desc->editor = ED_AREA;
	    pArea = (AREA_DATA *)ch->desc->pEdit;
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
	    aedit_show( ch, "" );
	}
	return;
    }

    if ( is_number( command ) )
    {
	if ( ( pArea = get_area_data( atoi( command ) ) ) == NULL )
	{
	    send_to_char( "No such area vnum exists.\n\r", ch );
	    return;
	}
    }
    else
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( is_exact_name( command, pArea->name ) )
	    break;
    }
    if ( !pArea )
    {
	send_to_char( "AEdit:  Area not found.\n\r", ch );
	return;
    }

    ch->desc->pEdit = (void *)pArea;
    ch->desc->editor = ED_AREA;
    REMOVE_BIT( ch->act2, PLR_MARKING );
    aedit_show( ch, "" );
    return;

}


/* Area Interpreter, called by do_aedit. */
void
aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	command[MAX_INPUT_LENGTH];
    char	arg[MAX_INPUT_LENGTH];
    int		cmd;
    int		value;

    if ( IS_NPC( ch ) )
    {
	interpret( ch, argument );
	return;
    }

    EDIT_AREA( ch, pArea );
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( !IS_BUILDER( ch, pArea ) )
	send_to_char( "AEdit:  Insufficient security to modify area.\n\r", ch );

    if ( command[0] == '\0' )
    {
	aedit_show( ch, argument );
	return;
    }

    if ( !str_cmp( command, "done") )
    {
	REMOVE_BIT( ch->act2, PLR_MARKING );
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg );
	return;
    }

    if ( !str_prefix( command, "northwest" )
    ||	 !str_prefix( command, "northeast" )
    ||	 !str_prefix( command, "southwest" )
    ||	 !str_prefix( command, "southeast" )
    ||	 !str_prefix( command, "nwest" )
    ||	 !str_prefix( command, "neast" )
    ||	 !str_prefix( command, "swest" )
    ||	 !str_prefix( command, "seast" )
    ||	 !str_prefix( command, "east" )
    ||	 !str_prefix( command, "west" ) )
    {
	interpret( ch, arg );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *aedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, aedit_table[cmd].name ) )
	{
	    if ( (*aedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Take care of flags. */
    if ( ( value = flag_value( area_flags, arg ) ) != NO_FLAG )
    {
	if ( !can_aedit_all( ch, pArea ) && value != ( value & ~HBUILDER_FLAGS ) )
	{
	    ch_printf( ch, "AEdit:  Insufficient security to edit flag(s) [%s]\n\r",
		       flag_string( area_flags, value & HBUILDER_FLAGS ) );
	    return;
	}
	TOGGLE_BIT( pArea->area_flags, value );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Flag toggled.\n\r", ch );
	return;
    }

    /* Take care of sector settings, in Overlands */
    if ( IS_VIRTUAL( ch->in_room )
    &&	 ch->in_room->area == pArea
    &&	 get_room_index( ch->in_room->vnum ) == NULL
    &&	 ( value = flag_value( sector_types, arg ) ) != NO_FLAG )
    {
	ch->in_room->sector_type = value;
	pArea->overland->map[ch->in_room->vnum - pArea->min_vnum].sector_type = value;
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Sector type set.\n\r", ch );
	return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
aedit_age( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;
    char	 age[MAX_STRING_LENGTH];

    EDIT_AREA( ch, pArea );

    one_argument( argument, age );

    if ( !is_number( age ) || age[0] == '\0' )
    {
	send_to_char( "Syntax:  age [#age]\n\r", ch );
	return FALSE;
    }

    pArea->age = atoi( age );

    send_to_char( "Age set.\n\r", ch );
    return FALSE;	/* This doesn't require saving the area info */
}


bool
aedit_allset( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    int			index;
    ROOM_INDEX_DATA *	pRoom;

    EDIT_AREA( ch, pArea );

    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "AEdit:  Insufficient security to edit this area.\n\r", ch );
        return FALSE;
    }

    if ( !IS_SET( pArea->area_flags, AREA_PROTOTYPE ) )
    {
        send_to_char( "AEdit:  Area is not a prototype area.\n\r", ch );
        return FALSE;
    }

    for ( index = 0; index < MAX_KEY_HASH; index++ )
    {
        for ( pRoom = room_index_hash[index]; pRoom != NULL; pRoom = pRoom->next )
        {
            if ( pRoom->vnum >= pArea->min_vnum && pRoom->vnum <= pArea->max_vnum )
            {
                instaroom( ch, pRoom );
            }
        }
    }

    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


bool
aedit_builder( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;
    char	 name[MAX_STRING_LENGTH];
    char	 buf[MAX_STRING_LENGTH];

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, name );

    if ( name[0] == '\0' )
    {
	send_to_char( "Syntax:  builder [$name]  -toggles builder\n\r", ch );
	send_to_char( "Syntax:  builder All      -allows everyone\n\r", ch );
	return FALSE;
    }

    name[0] = UPPER( name[0] );

    if ( strstr( pArea->builders, name ) != '\0' )
    {
	pArea->builders = string_replace( pArea->builders, name, "\0", 0 );
	pArea->builders = string_unpad( pArea->builders );

	if ( pArea->builders[0] == '\0' )
	{
	    free_string( pArea->builders );
	    pArea->builders = str_dup( "None" );
	}
	send_to_char( "Builder removed.\n\r", ch );
	return TRUE;
    }
    else
    {
	buf[0] = '\0';
	if ( strstr( pArea->builders, "None" ) != '\0' )
	{
	    pArea->builders = string_replace( pArea->builders, "None", "\0", 0 );
	    pArea->builders = string_unpad( pArea->builders );
	}

	if (pArea->builders[0] != '\0' )
	{
	    strcat( buf, pArea->builders );
	    strcat( buf, " " );
	}
	strcat( buf, name );
	free_string( pArea->builders );
	pArea->builders = string_proper( str_dup( buf ) );

	send_to_char( "Builder added.\n\r", ch );
	return TRUE;
    }

    return FALSE;
}


bool
aedit_circle( CHAR_DATA *ch, char *argument )
{
    VIRTUAL_ROOM_DATA *	map;
    AREA_DATA *		pArea;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		sect_name[MAX_INPUT_LENGTH];
    int			size;
    int			sect;
    int			plrx;
    int			plry;
    int			lx;
    int			ty;
    int			rx;
    int			by;
    int			idx;
    int			width;
    int			height;
    int			rsquared;	/* radius squared */
    int			row;
    int			col;
    int			count;
    int			x;
    int			y;

    EDIT_AREA( ch, pArea );

    if ( !IS_VIRTUAL( ch->in_room ) )
    {
	send_to_char( "AEdit:  You are not in an Overlands.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /*
     * If two arguments are given, one must be a sector and the other
     * must be a radius.
    */
    if ( is_number( arg1 ) && is_number( arg2 ) )
    {
	send_to_char( "AEdit:  Only one radius can be specified.\n\r", ch );
	return FALSE;
    }

    if ( arg2[0] != '\0' && !is_number( arg1 ) && !is_number( arg2 ) )
    {
	send_to_char( "Aedit:  Only one sector type can be specified.\n\r", ch );
	return FALSE;
    }

    if ( is_number( arg1 ) )
	size = atoi( arg1 );
    else if ( is_number( arg2 ) )
	size = atoi( arg2 );
    else
	size = 10;

    if ( size < 1 || size > 200 )
    {
	send_to_char( "AEdit:  Radius must be in the range 1 to 200.\n\r", ch );
	return FALSE;
    }

    rsquared = size * ( size + 1 );

    if ( is_number( arg1 ) )
	strcpy( sect_name, arg2 );
    else
	strcpy( sect_name, arg1 );

    if ( sect_name[0] == '\0' || !str_cmp( sect_name, "." ) )
	sect = ch->in_room->sector_type;
    else
	sect = flag_value( sector_types, sect_name );

    if ( sect == NO_FLAG )
    {
	ch_printf( ch, "AEdit:  Bad sector type '%s'.\n\r", sect_name );
	return FALSE;
    }

    if ( sect == SECT_INSIDE || sect == SECT_CITY || sect == SECT_ROAD )
    {
	send_to_char( "AEdit:  Cannot make circles of type inside, city or road.\n\r", ch );
	return FALSE;
    }

    map = pArea->overland->map;
    get_coordinates( ch->in_room, &plrx, &plry );
    lx = UMAX( plrx - size, 0 );
    ty = UMAX( plry - size, 0 );
    rx = UMIN( plrx + size, pArea->maxx - 1 );
    by = UMIN( plry + size, pArea->maxy - 1 );
    width = rx - lx + 1;
    height = by - ty + 1;
    idx = pArea->maxx * ty + lx;
    count = 0;

    for ( row = 0; row < height; row++ )
    {
	for ( col = 0; col < width; col++ )
	{
	    x = lx + row - plrx;
	    y = ty + col - plry;
	    if ( x * x + y * y < rsquared )
	    {
		idx = pArea->maxx * ( ty + row ) + lx + col;
		if ( map[idx].sector_type != sect
		&&   map[idx].sector_type != SECT_CITY
		&&   map[idx].sector_type != SECT_ROAD
		&&   get_room_index( pArea->min_vnum + idx ) == NULL )
		{
		    map[idx].sector_type = sect;
		    count++;
		}
	    }
	}
    }

    fix_sectors( pArea );

    ch_printf( ch, "%d sector%s modified.\n\r",
	       count, count == 1 ? "" : "s" );

    return count != 0;
}


bool
aedit_color( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    int		color;

    EDIT_AREA( ch, pArea );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  color <color>\n\r", ch );
	return FALSE;
    }

    if ( is_number( argument ) )
    {
	color = atoi( argument );
	if ( color < AT_BLOOD || color > AT_WHITE || color == AT_DGREY )
	    color = NO_FLAG;
    }
    else
	color = flag_value( color_types, argument );

    if ( color == NO_FLAG )
    {
	send_to_char( "AEdit:  Invalid color specified.\n\r", ch );
	return FALSE;
    }

    pArea->color = color;
    send_to_char( "Color set.\n\r", ch );
    return TRUE;
}


bool
aedit_continent( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	arg[MAX_INPUT_LENGTH];
    int		continent;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, NULL ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  continent <continent name>\n\r", ch );
        return FALSE;
    }

    if ( ( continent = flag_value( continent_types, arg ) ) == NO_FLAG )
    {
        send_to_char( "AEdit:  Unknown continent.\n\r", ch );
        send_to_char( "Type \"? continents\" for list.\n\r", ch );
        return FALSE;
    }

    pArea->continent = continent;
    send_to_char( "Continent set.\n\r", ch );
    return TRUE;
}


bool
aedit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;

    if ( !can_aedit_all( ch, NULL ) )
    {
	send_to_char( "AEdit:  Insufficient security to create area.\n\r", ch );
	return FALSE;
    }

    pArea               =   new_area();
    pArea->next         =   area_last->next;
    area_last->next     =   pArea;
    area_last		=   pArea;
    ch->desc->pEdit     =   (void *)pArea;
    pArea->created	=   current_time;
    SET_BIT( pArea->area_flags, AREA_PROTOTYPE );
    send_to_char( "Area Created.\n\r", ch );
    return TRUE;
}


bool
aedit_created( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	arg[MAX_INPUT_LENGTH];
    struct tm	timedata;
    time_t	created;

    if ( !can_aedit_all( ch, NULL ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    EDIT_AREA( ch, pArea );

    memset( &timedata, 0, sizeof( timedata ) );
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
/*      send_to_char( "Syntax:  created YYYY/MM/DD [HH:MM[:SS]]\n\r", ch ); */
        send_to_char( "Syntax:  created YYYY/MM/DD\n\r", ch );
        return FALSE;
    }

    if ( !isdigit( arg[0] )
    ||	 !isdigit( arg[1] )
    ||	 !isdigit( arg[2] )
    ||	 !isdigit( arg[3] )
    ||	 arg[4] != '/'
    ||	 !isdigit( arg[5] )
    ||	 !isdigit( arg[6] )
    ||	 arg[7] != '/'
    ||	 !isdigit( arg[8] )
    ||	 !isdigit( arg[9] )
    ||	 arg[10] != '\0' )
    {
/*      send_to_char( "Syntax:  created YYYY/MM/DD [HH:MM[:SS]]\n\r", ch ); */
        send_to_char( "Syntax:  created YYYY/MM/DD\n\r", ch );
        return FALSE;
    }

    timedata.tm_isdst = -1; /* let system figure out DST */
    timedata.tm_year  = atoi( &arg[0] ) - 1900;
    timedata.tm_mon   = atoi( &arg[5] ) - 1;
    timedata.tm_mday  = atoi( &arg[8] );

    created = mktime( &timedata );
    if ( created == ((time_t) -1) || created > current_time )
    {
        ch_printf( ch, "AEdit:  Bad date \"%s\".\n\r", arg );
        return FALSE;
    }

    pArea->created = created;
    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


bool
aedit_credits( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( "Syntax:    credits [credit_text]\n\r", ch );
	return FALSE;
    }

    free_string( pArea->credits );

    if ( !str_cmp( argument, "none" ) )
	pArea->credits = str_dup( "" );
    else
	pArea->credits = str_dup( argument );

    send_to_char( "Area credits set.\n\r",ch );
    return TRUE;
}


bool
aedit_exitsize( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	arg[MAX_INPUT_LENGTH];
    int		value;

    EDIT_AREA( ch, pArea );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  exitsize <size>\n\r", ch );
        return FALSE;
    }

    if ( is_number( arg ) )
        value = atoi( arg );
    else
        value = flag_value( size_types, arg );

    if ( value < 0 || value >= SIZE_TITANIC )
    {
        send_to_char( "AEdit:  Bad value for exit size.\n\r", ch );
        return FALSE;
    }

    pArea->exitsize = value;
    send_to_char( "Default indoor exit size set.\n\r", ch );
    return TRUE;
}


bool
aedit_file( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	file[MAX_STRING_LENGTH];
    int		i;
    int		length;
    FILE *	fp;
    char *	p;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, file );	/* Forces Lowercase */

    /* Truncate .are extension, if given */
    if ( ( p = strstr( file, ".are" ) ) != NULL )
	*p = '\0';

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  filename [$file]\n\r", ch );
	return FALSE;
    }

    /*
     * Simple Syntax Check.
     */
    length = strlen( file );
    if ( length > 8 )
    {
	send_to_char( "No more than eight characters allowed.\n\r", ch );
	return FALSE;
    }

    /*
     * Allow only letters and numbers.
     */
    for ( i = 0; i < length; i++ )
    {
	if ( !isalnum( file[i] ) )
	{
	    send_to_char( "Only letters and numbers are valid.\n\r", ch );
	    return FALSE;
	}
    }    

    strcat( file, ".are" );
    fclose( fpReserve );
    if ( ( fp = fopen( file, "r" ) ) != NULL )
    {
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
	send_to_char( "Sorry, that file name is in use.\n\r", ch );
	return FALSE;
    }
    fpReserve = fopen( NULL_FILE, "r" );
    free_string( pArea->file_name );
    pArea->file_name = str_dup( file );

    send_to_char( "Filename set.\n\r", ch );
    return TRUE;
}


bool
aedit_kingdom( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    char		arg[MAX_INPUT_LENGTH];
    int			kingdom;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  kingdom <kingdomname>\n\r", ch );
        return FALSE;
    }

    if ( ( kingdom = flag_value( kingdom_types, arg ) ) == NO_FLAG )
    {
        send_to_char( "AEdit:  Unknown kingdom.\n\r", ch );
        send_to_char( "Type \"? kingdoms\" for list.\n\r", ch );
        return FALSE;
    }

    pArea->kingdom = kingdom;
    send_to_char( "Kingdom set.\n\r", ch );
    return TRUE;
}


bool
aedit_level( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char 	lower[MAX_INPUT_LENGTH];
    char 	upper[MAX_INPUT_LENGTH];
    int		ilower;
    int		iupper;
    bool	fFlag = FALSE;

    EDIT_AREA( ch, pArea );

    argument = one_argument( argument, lower );
    one_argument( argument, upper );

    if ( upper[0] == 'h' && upper[1] == 'e' )
	sprintf( upper, "%d", LEVEL_HERO );

    if ( !str_cmp( lower, "all" ) && upper[0] == '\0' )
    {
	ilower = 1;
	iupper = LEVEL_HERO;
    }
    else if ( !str_cmp( lower, "none" ) && upper[0] == '\0' )
    {
	ilower = iupper = 0;
	fFlag = TRUE;
    }
    else if ( !str_cmp( lower, "clan" ) && upper[0] == '\0')
    {
	ilower = iupper = -1;
	fFlag = TRUE;
    }
    else if ( !str_cmp( lower, "hero" ) )
    {
	ilower = iupper = LEVEL_HERO;
    }
    else if ( lower[0] == 'i' && lower[1] == 'm' && lower[2] == 'm' )
    {
	ilower = iupper = LEVEL_IMMORTAL;
    }
    else if ( !is_number( lower ) || !is_number( upper ) )
    {
	send_to_char( "Syntax:  level [lower] [upper] | ALL | CLAN | HERO | IMM | None\n\r", ch );
	return FALSE;
    }
    else
    {
	ilower = atoi( lower );
	iupper = atoi( upper );
    }

    if ( ilower > iupper )
    {
	ilower ^= iupper;
	iupper ^= ilower;
	ilower ^= iupper;
    }

    if ( ilower != 0 && iupper != 0 )
    {
        if ( ( ilower < 1 || iupper > MAX_LEVEL ) && !fFlag )
	{
	    ch_printf( ch,
		"AEdit: levels must be between 1 and %d, or CLAN.\n\r",
		MAX_LEVEL );
	    return FALSE;
        }
    }

    pArea->low_level = ilower;
    send_to_char( "Lower level set.\n\r", ch );

    pArea->high_level = iupper;
    send_to_char( "Upper level set.\n\r", ch );

    return TRUE;
}


bool
aedit_line( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			ncount;
    AREA_DATA *		pArea;
    int			sector;
    int			value;
    int			count;
    int			dy;
    int			dx;
    int			x0;
    int			x1;
    int			y0;
    int			y1;

    EDIT_AREA( ch, pArea );
    if ( pArea->overland == NULL )
    {
	send_to_char( "AEdit:  Area has no Overlands.\n\r", ch );
	return FALSE;
    }

    ncount = 0;
    sector = SECT_NONE;

    argument = one_argument( argument, arg );
    while ( arg[0] != '\0' )
    {
	if ( !str_cmp( arg, "." ) )
	{
	    if ( !IS_VIRTUAL( ch->in_room ) || ch->in_room->area != pArea )
	    {
		send_to_char( "AEdit:  You are not in the area's overlands.\n\r", ch );
		return FALSE;
	    }
	    if ( ncount == 0 )
	    {
		get_coordinates( ch->in_room, &x0, &y0 );
		ncount = 2;
	    }
	    else if ( ncount == 2 )
	    {
		get_coordinates( ch->in_room, &x1, &y1 );
		ncount = 4;
	    }
	    else
	    {
		aedit_line( ch, "help" );
		return FALSE;
	    }
	}
	else if ( is_number( arg ) )
	{
	    value = atoi( arg );
	    if ( value >= pArea->min_vnum && value < pArea->min_vnum + pArea->maxx * pArea->maxy )
	    {
		if ( ncount == 0 )
		{
		    x0 = ( value - pArea->min_vnum ) % pArea->maxx;
		    y0 = ( value - pArea->min_vnum ) / pArea->maxx;
		    ncount = 2;
		}
		else if ( ncount == 2 )
		{
		    x1 = ( value - pArea->min_vnum ) % pArea->maxx;
		    y1 = ( value - pArea->min_vnum ) / pArea->maxx;
		    ncount = 4;
		}
		else
		{
		    aedit_line( ch, "help" );
		    return FALSE;
		}
	    }
	    else
	    {
		switch( ncount )
		{
		    case 0:	x0 = value; break;
		    case 1:	y0 = value; break;
		    case 2:	x1 = value; break;
		    case 3:	y1 = value; break;
		    default:
			aedit_line( ch, "help" );
			return FALSE;
		}
		ncount++;
	    }
	}
	else
	{
	    value = flag_value( sector_types, arg );
	    if ( value == NO_FLAG )
	    {
		/* This is where "help" finally lands */
		send_to_char( "Syntax:  line x0 y0 x1 y1 sector-type\n\r", ch );
		send_to_char( "where x0 y0 are the coordinates of the first room, x1 and y1 are the\n\r", ch );
		send_to_char( "coordinates of the second room.  You may also specify either or both\n\r", ch );
		send_to_char( "rooms by vnum.\n\r", ch );
		return FALSE;
	    }
	    if ( sector == SECT_NONE )
		sector = value;
	    else
	    {
		send_to_char( "AEdit:  More than one sector type given.\n\r", ch );
		return FALSE;
	    }
	}

	argument = one_argument( argument, arg );
    }

    if ( ncount != 4 )
    {
	aedit_line( ch, "help" );
	return FALSE;
    }

    if ( sector == SECT_NONE )
	sector = SECT_ROAD;

    if ( sector == SECT_INSIDE || sector == SECT_CITY )
    {
	send_to_char( "AEdit: line sector type cannor be \"inside\" or \"city\".", ch );
	return FALSE;
    }

    if ( x0 < 0 || x0 >= pArea->maxx || x1 < 0 || x1 >= pArea->maxx )
    {
	ch_printf( ch, "X ordinate must be within range 0 to %d.\n\r", pArea->maxx - 1 );
	return FALSE;
    }

    if ( y0 < 0 || y0 >= pArea->maxy || y1 < 0 || y1 >= pArea->maxy )
    {
	ch_printf( ch, "Y ordinate must be within range 0 to %d.\n\r", pArea->maxy - 1 );
	return FALSE;
    }

    ch_printf( ch, "Drawing line of type %s from %d(%d,%d) to %d(%d,%d).\n\r",
		flag_string( sector_types, sector ),
		pArea->min_vnum + pArea->maxx * y0 + x0, x0, y0,
		pArea->min_vnum + pArea->maxx * y1 + x1, x1, y1 );

    /*
     * Implementation of Bresenham's Algorithm from
     * "Zen of Graphics Programming" by Michael Abrash.
     */

    if ( y0 > y1 )
    {
	value = y0;
	y0 = y1;
	y1 = value;
	value = x0;
	x0 = x1;
	x1 = value;
    }

    dy = y1 - y0;
    dx = x1 - x0;

    if ( dx > 0 )
    {
	if ( dx > dy )
	    count = line_x_major( x0, y0, dx, dy, 1, pArea->maxx,
				  sector, pArea->overland->map );
	else
	    count = line_y_major( x0, y0, dx, dy, 1, pArea->maxx,
				  sector, pArea->overland->map );
    }
    else
    {
	dx = -dx;
	if ( dx > dy )
	    count = line_x_major( x0, y0, dx, dy, -1, pArea->maxx,
				  sector, pArea->overland->map );
	else
	    count = line_y_major( x0, y0, dx, dy, -1, pArea->maxx,
				  sector, pArea->overland->map );
    }

    fix_sectors( pArea );

    ch_printf( ch, "%d sector%s modified.\n\r",
	       count, count == 1 ? "" : "s" );

    return count != 0;
}


bool
aedit_lvnum( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;
    char	 lower[MAX_STRING_LENGTH];
    int		 ilower;
    int		 iupper;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    if ( pArea->overland != NULL )
    {
	send_to_char( "AEdit:  Cannot change lower vnum of an Overland area.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, lower );

    if ( !is_number( lower ) || lower[0] == '\0' )
    {
	send_to_char( "Syntax:  lvnum [#lower]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = pArea->max_vnum ) )
    {
	send_to_char( "AEdit:  Value must be less than the uvnum.\n\r", ch );
	return FALSE;
    }

    if ( ilower <= 0 || ilower >= MAX_VNUM || iupper <= 0 || iupper >= MAX_VNUM )
    {
	ch_printf( ch, "AEdit: vnum must be between 0 and %d.\n\r", MAX_VNUM );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
	send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->min_vnum = ilower;
    send_to_char( "Lower vnum set.\n\r", ch );
    return TRUE;
}


bool
aedit_mark( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	arg[MAX_INPUT_LENGTH];
    int		sector;

    EDIT_AREA( ch, pArea );

    argument = one_argument( argument, arg );

    if ( IS_SET( ch->act2, PLR_MARKING ) && arg[0] == '\0' )
    {
	send_to_char( "Sector marking is now OFF.\n\r", ch );
	REMOVE_BIT( ch->act2, PLR_MARKING );
	return FALSE;
    }

    if ( pArea->overland == NULL )
    {
	send_to_char( "AEdit:  This area has no Overlands to mark.\n\r", ch );
	return FALSE;
    }

    if ( arg[0] == '\0' )
	sector = ch->in_room->sector_type;
    else
	sector = flag_value( sector_types, arg );

    if ( sector == NO_FLAG )
    {
	ch_printf( ch, "AEdit:  bad sector type '%s'.\n\r", arg );
	return FALSE;
    }

    if ( sector == SECT_INSIDE || sector == SECT_CITY )
    {
	send_to_char( "AEdit:  Cannot mark sectors of type 'inside' or 'city'.\n\r", ch );
	return FALSE;
    }

    SET_BIT( ch->act2, PLR_MARKING );
    ch->pcdata->mark_sector = sector;
    ch_printf( ch, "Sector marking with type %s ON.\n\r",
	       flag_string( sector_types, sector ) );

    if ( get_room_index( ch->in_room->vnum ) != NULL )
	return FALSE;

    if ( ch->in_room->sector_type == sector )
	return FALSE;

    ch->in_room->sector_type = sector;
    pArea->overland->map[ch->in_room->vnum - pArea->min_vnum].sector_type = sector;
    ch_printf( ch, "Sector type set to %s.\n\r",
	       flag_string( sector_types, sector ) );
    return TRUE;
}


bool
aedit_name( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:   name [$name]\n\r", ch );
	return FALSE;
    }

    free_string( pArea->name );
    pArea->name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}


bool
aedit_norecall( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;
    char	 oldmsg[MAX_INPUT_LENGTH];
    char	*newmsg;
    bool	fChanged;

    EDIT_AREA( ch, pArea );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:   norecall [no_recall_message]\n\r", ch );
	return FALSE;
    }

    strcpy( oldmsg, pArea->norecall ? pArea->norecall : "" );
    if ( !str_cmp( argument, "none" ) )
    {
	free_string( pArea->norecall );
	pArea->norecall = &str_empty[0];
	fChanged = TRUE;
    }
    else
    {
	newmsg = string_change( ch, pArea->norecall, argument );
	if ( newmsg )
	{
	    pArea->norecall = newmsg;
	    fChanged = TRUE;
	}
	else
	{
	    pArea->norecall = str_dup( oldmsg );
	    fChanged = FALSE;
	}
    }
    send_to_char( fChanged ? *pArea->norecall != '\0' ? "Norecall message set.\n\r" : "Reset message removed" : "Reset message not changed.\n\r", ch );
    return fChanged;
}


bool
aedit_overland( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	arg[MAX_INPUT_LENGTH];

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return overland_help( ch, argument );
    else if ( !str_prefix( arg, "create" ) )
	return overland_create( ch, argument );
    else if ( !str_cmp( arg, "delete" ) )
	return overland_delete( ch, argument );
    else if ( str_match( arg, "de", "desc" ) )
        return overland_desc( ch, argument );
    else if ( !str_prefix( arg, "dump" ) )
	return overland_dump( ch, argument );
    else if ( !str_prefix( arg, "edesc" ) )
        return overland_edesc( ch, argument );
    else if ( !str_prefix( arg, "help" ) )
	return overland_help( ch, argument );
    else if ( !str_prefix( arg, "import" ) )
	return overland_import( ch, argument );
    else if ( !str_prefix( arg, "mdesc" ) )
        return overland_mdesc( ch, argument );
    else if ( !str_prefix( arg, "ndesc" ) )
        return overland_ndesc( ch, argument );
    else
	return overland_help( ch, argument );
}


bool
aedit_recall( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char room[MAX_STRING_LENGTH];
    int  value;

    EDIT_AREA( ch, pArea );

    one_argument( argument, room );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  recall [#rvnum]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( argument, "clear" ) || !str_cmp( argument, "none" ) )
    {
	pArea->recall = 0;
	send_to_char( "Recall point removed.\n\r", ch );
	return TRUE;
    }

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  recall [#rvnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( room );

    if ( !get_room_index( value ) )
    {
	send_to_char( "AEdit:  Room vnum does not exist.\n\r", ch );
	return FALSE;
    }

    pArea->recall = value;

    send_to_char( "Recall set.\n\r", ch );
    return TRUE;
}


bool
aedit_resetmsg( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;
    char	 oldmsg[MAX_INPUT_LENGTH];
    char	*newmsg;
    bool	fChanged;

    EDIT_AREA( ch, pArea );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:   resetmsg [reset_message]\n\r", ch );
	return FALSE;
    }

    strcpy( oldmsg, pArea->resetmsg ? pArea->resetmsg : "" );
    if ( !str_cmp( argument, "none" ) )
    {
	free_string( pArea->resetmsg );
	pArea->resetmsg = &str_empty[0];
	fChanged = TRUE;
    }
    else
    {
	newmsg = string_change( ch, pArea->resetmsg, argument );
	if ( newmsg )
	{
	    pArea->resetmsg = newmsg;
	    fChanged = TRUE;
	}
	else
	{
	    pArea->resetmsg = str_dup( oldmsg );
	    fChanged = FALSE;
	}
    }
    send_to_char( fChanged ? *pArea->resetmsg != '\0' ? "Reset message set.\n\r" : "Reset message removed" : "Reset message not changed.\n\r", ch );
    return fChanged;
}


bool
aedit_rooms( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char *		msg;
    int			value = 0;
    bitvector		flag;
    int			function;
    int			vnum;
    int			iCount;

    EDIT_AREA( ch, pArea );

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' || *argument == '\0' )
    {
	send_to_char( "Syntax:  rooms sector <sectortype>\n\r", ch );
	send_to_char( "         rooms clear <flaglist>\n\r", ch );
	send_to_char( "         rooms set <flaglist>\n\r", ch );
	send_to_char( "         rooms toggle <flaglist>\n\r", ch );
	return FALSE;
    }

    if ( !str_prefix( "sec", arg1 ) && is_name( arg1, "sector" ) )
    {
	iCount = 0;
	value = flag_value( sector_types, argument );
	if ( value == NO_FLAG )
	{
	    send_to_char( "AEdit:  Invalid sector type.\n\r", ch );
	    return FALSE;
	}
	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
	    if ( ( pRoom = get_room_index( vnum ) ) != NULL )
	    {
		pRoom->sector_type = value ;
		iCount++;
	    }
	}
	if ( iCount == 0 )
	{
	    send_to_char( "AEdit:  No rooms in area.\n\r", ch );
	    return FALSE;
	}
	send_to_char( "All room sectors set.\n\r", ch );
	return TRUE;
    }

    if ( is_name( arg1, "set"    ) )
    {
	function = FLAG_SET;
	msg = "set";
    }
    else if ( is_name( arg1, "clear"  ) )
    {
	function = FLAG_CLEAR;
	msg = "cleared";
    }
    else if ( is_name( arg1, "toggle" ) )
    {
	function = FLAG_TOGGLE;
	msg = "toggled";
    }
    else
    {
	aedit_rooms( ch, "" ); /* display syntax message */
	return FALSE;
    }

    /* Hmmm... probably going to add "clear all" in here someday */
    argument = one_argument( argument, arg2 );
    do
    {
	flag = flag_value( room_flags, arg2 );
	if ( flag == 0 || flag == NO_FLAG )
	{
	    ch_printf( ch, "AEdit:  Invalid flag %s\n\r", arg2 );
	    return FALSE;
	}
	value |= flag;
	argument = one_argument( argument, arg2 );
    }
    while ( arg2[0] != '\0' );

    iCount = 0;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pRoom = get_room_index( vnum ) ) == NULL )
	    continue;
	switch( function )
	{
	    case FLAG_SET:
		SET_BIT( pRoom->room_flags, value );
		break;
	    case FLAG_CLEAR:
		REMOVE_BIT( pRoom->room_flags, value );
		break;
	    case FLAG_TOGGLE:
		TOGGLE_BIT( pRoom->room_flags, value );
		break;
	}
	iCount++;
    }

    if ( iCount == 0 )
    {
	send_to_char( "AEdit:  No rooms in area.\n\r", ch );
	return FALSE;
    }

    ch_printf( ch, "Room flags in area %s.\n\r", msg );

    return TRUE;
}


bool
aedit_security( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;
    char	 sec[MAX_STRING_LENGTH];
    int		 value;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, sec );

    if ( !is_number( sec ) || sec[0] == '\0' )
    {
	send_to_char( "Syntax:  security [#level]\n\r", ch );
	return FALSE;
    }

    value = atoi( sec );

    if ( value > ch->pcdata->security || value < 0 )
    {
	if ( ch->pcdata->security != 0 )
	{
	    ch_printf( ch, "Security is 0-%d.\n\r", ch->pcdata->security );
	}
	else
	    send_to_char( "Security is 0 only.\n\r", ch );
	return FALSE;
    }

    pArea->security = value;

    send_to_char( "Security set.\n\r", ch );
    return TRUE;
}


bool
aedit_setmobs( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    MOB_INDEX_DATA *	pMob;
    int			vnum;
    int			count;
    int			skip;

    EDIT_AREA( ch, pArea );

    count = 0;
    skip = 0;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pMob = get_mob_index( vnum ) ) != NULL )
	{
	    if ( pMob->autoset != DICE_NOSET )
	    {
	        set_mob_dice( pMob, pMob->autoset );
	        count++;
            }
            else
                skip++;
	}
    }

    if ( count != 0 || skip != 0 )
	ch_printf( ch, "%d mob%s set ( %d skipped).\n\r", count,
	           count == 1 ? "" : "s", skip );
    else
	send_to_char( "No Mobiles found.\n\r", ch );

    return count != 0;
}


bool
aedit_setobjs( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    OBJ_INDEX_DATA *	pObj;
    int			vnum;
    int			count;

    EDIT_AREA( ch, pArea );

    count = 0;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pObj = get_obj_index( vnum ) ) != NULL )
	{
	    if ( set_obj_stats( pObj ) )
	        count++;
	}
    }

    if ( count != 0 )
	ch_printf( ch, "%d object%s set.\n\r", count,
	           count == 1 ? "" : "s" );
    else
	send_to_char( "No Objects found.\n\r", ch );

    return count != 0;
}


bool
aedit_show( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    BUFFER *	buf;

    EDIT_AREA( ch, pArea );
    buf = new_buf( );

    show_area_info( pArea, buf );

    set_char_color( C_DEFAULT, ch );
    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return FALSE;
}


bool
aedit_stamp( CHAR_DATA *ch, char *argument )
{
    VIRTUAL_ROOM_DATA *	map;
    AREA_DATA *		pArea;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		sect_name[MAX_INPUT_LENGTH];
    int			size;
    int			sect;
    int			plrx;
    int			plry;
    int			lx;
    int			ty;
    int			rx;
    int			by;
    int			idx;
    int			width;
    int			height;
    int			row;
    int			col;
    int			count;

    EDIT_AREA( ch, pArea );

    if ( !IS_VIRTUAL( ch->in_room ) )
    {
	send_to_char( "AEdit:  You are not in an Overlands.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /*
     * If two arguments are given, one must be a sector and the other
     * must be a size.
    */
    if ( is_number( arg1 ) && is_number( arg2 ) )
    {
	send_to_char( "AEdit:  Only one size can be specified.\n\r", ch );
	return FALSE;
    }

    if ( arg2[0] != '\0' && !is_number( arg1 ) && !is_number( arg2 ) )
    {
	send_to_char( "Aedit:  Only one sector type can be specified.\n\r", ch );
	return FALSE;
    }

    if ( is_number( arg1 ) )
	size = atoi( arg1 );
    else if ( is_number( arg2 ) )
	size = atoi( arg2 );
    else
	size = 10;

    if ( size < 1 || size > 200 )
    {
	send_to_char( "AEdit:  Size must be in the range 1 to 200.\n\r", ch );
	return FALSE;
    }

    if ( is_number( arg1 ) )
	strcpy( sect_name, arg2 );
    else
	strcpy( sect_name, arg1 );

    if ( sect_name[0] == '\0' || !str_cmp( sect_name, "." ) )
	sect = ch->in_room->sector_type;
    else
	sect = flag_value( sector_types, sect_name );

    if ( sect == NO_FLAG )
    {
	ch_printf( ch, "AEdit:  Bad sector type '%s'.\n\r", sect_name );
	return FALSE;
    }

    if ( sect == SECT_INSIDE || sect == SECT_CITY || sect == SECT_ROAD )
    {
	send_to_char( "AEdit:  Cannot stamp sectors of type inside, city or road.\n\r", ch );
	return FALSE;
    }

    map = pArea->overland->map;
    get_coordinates( ch->in_room, &plrx, &plry );
    lx = UMAX( plrx - size, 0 );
    ty = UMAX( plry - size, 0 );
    rx = UMIN( plrx + size, pArea->maxx - 1 );
    by = UMIN( plry + size, pArea->maxy - 1 );
    width = rx - lx + 1;
    height = by - ty + 1;
    idx = pArea->maxx * ty + lx;
    count = 0;

    for ( row = 0; row < height; row++ )
    {
	for ( col = 0; col < width; col++ )
	{
	    if ( map[idx].sector_type != sect
//	    &&	 map[idx].sector_type != SECT_INSIDE
	    &&	 map[idx].sector_type != SECT_CITY
	    &&	 map[idx].sector_type != SECT_ROAD
	    &&	 get_room_index( pArea->min_vnum + idx ) == NULL )
	    {
		map[idx].sector_type = sect;
		count++;
	    }
	    idx++;
	}
	idx += pArea->maxx - width;
    }

    fix_sectors ( pArea );

    ch_printf( ch, "%d sector%s modified.\n\r",
	       count, count == 1 ? "" : "s" );

    return count != 0;
}


bool
aedit_travel( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    AREA_DATA *		pArea;

    EDIT_AREA( ch, pArea );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' || !str_prefix( arg, "help" ) )
    {
	send_to_char(	"Syntax:\n\r"
			"travel add <vnum>    - add room <vnum> to list.\n\r"
			"travel create <vnum> - create traveling room.\n\r"
			"travel delete <vnum> - delete room from list.\n\r"
			"travel edit <number> - edit a travel list entry.\n\r"
			"travel help          - show this help.\n\r"
			"travel show          - list all stops for traveling room.\n\r"
			, ch );
	return FALSE;
    }
    else if ( !str_prefix( arg, "add" ) )
    {
	return aedit_travel_add( ch, argument );
    }
    else if ( !str_prefix( arg, "create" ) )
    {
	return aedit_travel_create( ch, argument );
    }
    else if ( !str_cmp( arg, "delete" ) )
    {
	return aedit_travel_delete( ch, argument );
    }
    else if ( !str_prefix( arg, "edit" ) )
    {
	return aedit_travel_edit( ch, argument );
    }
    else if ( !str_prefix( arg, "list" ) || !str_prefix( arg, "show" ) )
    {
	aedit_travel_show( ch, argument );
	return FALSE;
    }

    return aedit_travel( ch, "help" );
}


static bool
aedit_travel_add( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    AREA_DATA *		pArea;
    TRAVEL_DATA *	pPrev;
    ROOM_INDEX_DATA *	pRoom;
    TRAVEL_DATA *	pTravel;
    int			vnum;

    EDIT_AREA( ch, pArea );

    if ( !IS_BUILDER( ch, pArea ) || !IS_SET( pArea->area_flags, AREA_TRAVEDIT ) )
    {
	send_to_char( "AEdit:  Insufficient security to perform this function.\n\r", ch );
	return FALSE;
    }

    if ( pArea->travel_room == NULL )
    {
	send_to_char( "This area has no traveling room assigned.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( is_number( arg ) )
	vnum = atoi( arg );
    else if ( !str_cmp( arg, "." ) )
	vnum = ch->in_room->vnum;
    else
    {
	send_to_char( "Syntax:  room <vnum>\n\r", ch );
	return FALSE;
    }

    pRoom = get_room_index( vnum );
    if ( pRoom == NULL )
    {
	send_to_char( "AEdit:  Room does not exist.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pRoom->area ) )
    {
	send_to_char( "AEdit:  Insufficient security to perform this function.\n\r", ch );
	return FALSE;
    }

    pTravel = new_travel( );
    if ( pArea->travel_first == NULL )
    {
	pArea->travel_first = pTravel;
	pArea->travel_curr = pTravel;
    }
    else
    {
	for ( pPrev = pArea->travel_first; pPrev->next != NULL; pPrev = pPrev->next )
	    ;
	pPrev->next = pTravel;	
    }

    pTravel->area	= pArea;
    pTravel->room_vnum	= vnum;
    pTravel->room	= pRoom;
    pTravel->exit_dir	= DIR_NONE;
    pTravel->stop_timer = 1;
    pTravel->move_timer	= 1;

    ch->desc->editin = ch->desc->editor;
    ch->desc->editor = ED_TRAVEL;
    ch->desc->inEdit = ch->desc->pEdit;
    ch->desc->pEdit  = (void *)pTravel;

    send_to_char( "Room added to travel list.\n\r", ch );

    travedit_show( ch, "" );

    return TRUE;
}


static bool
aedit_travel_create( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    int			vnum;

    EDIT_AREA( ch, pArea );

    if ( !IS_BUILDER( ch, pArea ) || !IS_SET( pArea->area_flags, AREA_TRAVEDIT ) )
    {
	send_to_char( "AEdit:  Insufficient security to perform this funciton.\n\r", ch );
	return FALSE;
    }

    if ( pArea->travel_room != NULL )
    {
	send_to_char( "AEdit:  Area already has a traveling room.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  travel create <vnum>\n\r", ch );
	return FALSE;
    }

    vnum = atoi( arg );
    if ( vnum < pArea->min_vnum || vnum > pArea->max_vnum )
    {
	send_to_char( "AEdit:  Vnum is not from area being edited.\n\r", ch );
	return FALSE;
    }

    pRoom = get_room_index( vnum );
    if ( pRoom == NULL )
    {
	send_to_char( "AEdit:  Room does not exist.\n\r", ch );
	return FALSE;
    }

    pArea->travel_vnum	= vnum;
    pArea->travel_room	= pRoom;
    pArea->link		= new_exit( );
    pArea->rev_link	= new_exit( );

    send_to_char( "Traveling room created.\n\r", ch );
    return TRUE;
}


static bool
aedit_travel_delete( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			count;
    AREA_DATA *		pArea;
    TRAVEL_DATA *	pPrev;
    TRAVEL_DATA *	pTravel;
    int			vnum;

    EDIT_AREA( ch, pArea );

    if ( !IS_BUILDER( ch, pArea ) || !IS_SET( pArea->area_flags, AREA_TRAVEDIT ) )
    {
	send_to_char( "AEdit:  Insufficient security to perform this function.\n\r", ch );
	return FALSE;
    }

    if ( pArea->travel_room == NULL )
    {
	send_to_char( "This area has no traveling room assigned.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  travel delete <room_vnum or list #>\n\r", ch );
	return FALSE;
    }

    vnum = atoi( arg );

    count = 0;
    for ( pTravel = pArea->travel_first; pTravel != NULL; pTravel = pTravel->next )
    {
	count++;
	if ( count == vnum || pTravel->room_vnum == vnum )
	    break;
    }

    if ( pTravel == NULL )
    {
	if ( vnum <= top_area )
	    send_to_char( "AEdit:  List entry not found.\n\r", ch );
	else
	    send_to_char( "AEdit:  Room vnum not in list.\n\r", ch );
	return FALSE;
    }

    check_travel( pTravel );

    if ( pArea->travel_first == pTravel )
	pArea->travel_first = pTravel->next;
    else
    {
	for ( pPrev = pArea->travel_first; pPrev != NULL; pPrev = pPrev->next )
	    if ( pPrev->next == pTravel )
		break;
	pPrev->next = pTravel->next;
    }

    if ( pArea->travel_curr == pTravel )
    {
	pArea->travel_curr = pArea->travel_curr->next;
	if ( pArea->travel_curr == NULL )
	    pArea->travel_curr = pArea->travel_first;
    }

    save_travel_state( );

    free_travel( pTravel );
    send_to_char( "Room deleted from list.\n\r", ch );
    return TRUE;
}


static bool
aedit_travel_edit( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			count;
    AREA_DATA *		pArea;
    TRAVEL_DATA *	pTravel;
    int			vnum;

    EDIT_AREA( ch, pArea );

    if ( !IS_BUILDER( ch, pArea ) || !IS_SET( pArea->area_flags, AREA_TRAVEDIT ) )
    {
	send_to_char( "AEdit:  Insufficient security to perform this function.\n\r", ch );
	return FALSE;
    }

    if ( pArea->travel_room == NULL )
    {
	send_to_char( "This area has no traveling room assigned.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  travel edit <room_vnum or list #>\n\r", ch );
	return FALSE;
    }

    vnum = atoi( arg );

    count = 0;
    for ( pTravel = pArea->travel_first; pTravel != NULL; pTravel = pTravel->next )
    {
	count++;
	if ( count == vnum || pTravel->room_vnum == vnum )
	    break;
    }

    if ( pTravel == NULL )
    {
	if ( vnum <= top_area )
	    send_to_char( "AEdit:  List entry not found.\n\r", ch );
	else
	    send_to_char( "AEdit:  Room vnum not in list.\n\r", ch );
	return FALSE;
    }

    ch->desc->editin = ch->desc->editor;
    ch->desc->editor = ED_TRAVEL;
    ch->desc->inEdit = ch->desc->pEdit;
    ch->desc->pEdit  = (void *)pTravel;
    travedit_show( ch, "" );

    return FALSE;
}


static void
aedit_travel_show( CHAR_DATA *ch, char *argument )
{
    BUFFER *		buf;
    AREA_DATA *		pArea;

    EDIT_AREA( ch, pArea );

    buf = new_buf( );
    show_travel_info( pArea, buf );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
}


bool
aedit_uvnum( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;
    char	 upper[MAX_STRING_LENGTH];
    int		 ilower;
    int		 iupper;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, upper );

    if ( !is_number( upper ) || upper[0] == '\0' )
    {
	send_to_char( "Syntax:  uvnum [#upper]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = pArea->min_vnum ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
	return FALSE;
    }

    /* OLC 1.1b */
    if ( ilower <= 0 || ilower >= MAX_VNUM || iupper <= 0 || iupper >= MAX_VNUM )
    {
	ch_printf( ch, "AEdit: vnum must be between 0 and %d.\n\r", MAX_VNUM );
	return FALSE;
    }

    if ( pArea->overland != NULL
    &&	 iupper < pArea->min_vnum + pArea->maxx * pArea->maxy - 1 )
    {
	send_to_char( "AEdit:  Upper vnum must be large enough to include Overland area.\n\r", ch );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
	send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->max_vnum = iupper;
    send_to_char( "Upper vnum set.\n\r", ch );

    return TRUE;
}


bool
aedit_vnum( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*pArea;
    char	 lower[MAX_STRING_LENGTH];
    char	 upper[MAX_STRING_LENGTH];
    int		 ilower;
    int		 iupper;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    if ( pArea->overland != NULL )
    {
	send_to_char( "AEdit:  Cannot change lower vnum of an Overland area.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, lower );
    one_argument( argument, upper );

    if ( !is_number( lower ) || lower[0] == '\0'
    || !is_number( upper ) || upper[0] == '\0' )
    {
	send_to_char( "Syntax:  vnum [#lower] [#upper]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
	return FALSE;
    }

    if ( ilower <= 0 || ilower >= MAX_VNUM || iupper <= 0 || iupper >= MAX_VNUM )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "AEdit: vnum must be between 0 and %d.\n\r", MAX_VNUM );
	send_to_char( output, ch );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
	send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->min_vnum = ilower;
    send_to_char( "Lower vnum set.\n\r", ch );

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
	send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
	return TRUE;	/* The lower value has been set. */
    }

    pArea->max_vnum = iupper;
    send_to_char( "Upper vnum set.\n\r", ch );

    return TRUE;
}


bool
aedit_world( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    char	arg[MAX_INPUT_LENGTH];
    int		value;

    EDIT_AREA( ch, pArea );

    if ( !can_aedit_all( ch, pArea ) )
    {
	send_to_char( "AEdit:  Insufficient security to modify field.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, arg );

    value = flag_value( world_types, arg );
    if ( value == NO_FLAG )
    {
	send_to_char( "AEdit:  Invalid world.\n\r", ch );
	return FALSE;
    }

    pArea->world = value;
    send_to_char( "World set.\n\r", ch );
    return TRUE;
}


bool
edit_makelinks( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	srcRoom;
    ROOM_INDEX_DATA *	dstRoom;
    EXIT_DATA *		pExit;
    int			dir;
    const char *	ed_name;
    int			revDir;
    int			vnum;
    int			minVnum;
    int			maxVnum;
    bool		changed;

    EDIT_AREA( ch, pArea );

    if ( ch->desc == NULL )
        return FALSE;


    if ( ch->desc->editor == ED_AREA )
    {
        EDIT_AREA( ch, pArea );
        srcRoom = ch->in_room;
        ed_name = "AEdit";
    }
    else if ( ch->desc->editor == ED_ROOM )
    {
        EDIT_ROOM( ch, srcRoom );
        pArea = srcRoom == NULL ? NULL : srcRoom->area;
        ed_name = "REdit";
    }
    else
    {
        send_to_char( "You must be in AEdit or REdit to use this command.\n\r", ch );
        return FALSE;
    }

    if ( pArea == NULL || pArea->overland == NULL )
    {
	ch_printf( ch, "%s:  This area has no Overlands.\n\r", ed_name );
	return FALSE;
    }

    if ( srcRoom == NULL || srcRoom->area != pArea )
    {
        ch_printf( ch, "%s:  You must be in a room in the overlands.\n\r", ed_name );
        return FALSE;
    }

    minVnum = pArea->min_vnum;
    maxVnum = pArea->overland->x * pArea->overland->y + minVnum - 1;

    if ( srcRoom->vnum < minVnum || srcRoom->vnum > maxVnum )
    {
        ch_printf( ch, "%s:  Current room not in the overlands.\n\r", ed_name );
        return FALSE;
    }

    if ( IS_SET( srcRoom->room_flags, ROOM_VIRTUAL ) )
    {
        ch_printf( ch, "%s:  Current room must be a real room.\n\r", ed_name );
        return FALSE;
    }

    changed = FALSE;

    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
        if ( srcRoom->exit[dir] != NULL )
            continue;

        vnum = srcRoom->vnum;
        switch( dir )
        {
            case DIR_NORTH:	vnum -= pArea->overland->x;	break;
            case DIR_EAST:	vnum += 1;			break;
            case DIR_SOUTH:	vnum += pArea->overland->x;	break;
            case DIR_WEST:	vnum -= 1;			break;
            case DIR_NORTHWEST:	vnum -= pArea->overland->x + 1;	break;
            case DIR_NORTHEAST:	vnum -= pArea->overland->x - 1;	break;
            case DIR_SOUTHWEST:	vnum += pArea->overland->x - 1;	break;
            case DIR_SOUTHEAST:	vnum += pArea->overland->x + 1;	break;
            default:		continue;
        }
        if ( vnum < minVnum || vnum > maxVnum
        || ( dstRoom = get_room_index( vnum ) ) == NULL )
            continue;

        revDir = rev_dir[dir];
        if ( dstRoom->exit[revDir] != NULL )
            continue;

        pExit = new_exit( );
        pExit->from_room = srcRoom;
        pExit->to_room = dstRoom;
        pExit->size = SIZE_TITANIC;
        srcRoom->exit[dir] = pExit;

        pExit = new_exit( );
        pExit->from_room = dstRoom;
        pExit->to_room = srcRoom;
        pExit->size = SIZE_TITANIC;
        dstRoom->exit[revDir] = pExit;

        ch_printf( ch, "Linking %s to %d.\n\r", dir_name[dir], vnum );
        changed = TRUE;
    }

    if ( !changed )
        send_to_char( "No rooms linked.\n\r", ch );

    return changed;
}


/* Local functions */

/*****************************************************************************
 Name:		check_range( lower vnum, upper vnum )
 Purpose:	Ensures the range spans only one area.
 Called by:	aedit_vnum(olc_act.c).
 ****************************************************************************/
static bool
check_range( int lower, int upper )
{
    AREA_DATA *pArea;
    int cnt = 0;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	/*
	 * lower < area < upper
	 */
	if ( ( lower <= pArea->min_vnum && upper >= pArea->min_vnum )
	||   ( upper >= pArea->max_vnum && lower <= pArea->max_vnum ) )
	    cnt++;

	if ( cnt > 1 )
	    return FALSE;
    }
    return TRUE;
}


static void
check_travel( TRAVEL_DATA *pTravel )
{
    AREA_DATA *	pArea;
    int		dir;
    int		rdir;
    EXIT_DATA *	pExit;

    if ( pTravel == NULL )
	return;

    pArea = pTravel->area;

    if ( !pTravel->stopped || pArea->travel_curr != pTravel )
	return;

    pTravel->stopped = FALSE;
    pArea->travel_timer = UMIN( pArea->travel_timer, pTravel->move_timer );
    dir = pTravel->exit_dir;
    rdir = rev_dir[dir];

    pArea->travel_room->exit[dir] = NULL;

    pExit = pTravel->room->exit[rdir];
    pTravel->room->exit[rdir] = pArea->rev_link;
    pArea->rev_link = pExit;

}


static bool
dump_ascii( CHAR_DATA *ch, char *filebase )
{
    char		fname[MAX_INPUT_LENGTH];
    FILE *		fp;
    AREA_DATA *		pArea;
    VIRTUAL_ROOM_DATA *	pMap;
    int			x;
    int			y;

    EDIT_AREA( ch, pArea );

    strcpy( fname, filebase );
    strcat( fname, "txt" );

    if ( ( fp = fopen( fname, "w" ) ) == NULL )
    {
	perror( fname );
	send_to_char( "AEdit:  An error occurred opening the file.\n\r", ch );
	return FALSE;
    }

    pMap = pArea->overland->map;

    for ( y = 0; y < pArea->maxy; y++ )
    {
	for ( x = 0; x < pArea->maxx; x++ )
	{
	    putc( sector_data[pMap->sector_type].symbol, fp );
	    pMap++;
	}
	putc( '\n', fp );
    }

    fclose( fp );
    return FALSE;
}


static bool
dump_html( CHAR_DATA *ch, char *filebase )
{
    char		fname[MAX_INPUT_LENGTH];
    FILE *		fp;
    AREA_DATA *		pArea;
    VIRTUAL_ROOM_DATA *	pMap;
    int			lastsector;
    int			sector;
    int			x;
    int			y;

    EDIT_AREA( ch, pArea );

    strcpy( fname, filebase );
    strcat( fname, "html" );

    if ( ( fp = fopen( fname, "w" ) ) == NULL )
    {
	perror( fname );
	send_to_char( "AEdit:  An error occurred opening the file.\n\r", ch );
	return FALSE;
    }

    fprintf( fp,
	     "<HTML><HEAD><TITLE>%s</TITLE></HEAD>\n", pArea->file_name );
    fprintf( fp, "<BODY BGCOLOR=\"#000000\" TEXT=\"#A0A0A0\" LINK=\"#0000FF\" "
	     "VLINK=\"#000080\" ALINK=\"#FF0000\"" );
    fprintf( fp, "<PRE><FONT FACE=\"courier new\"><FONT COLOR=\"#000000\">\n" );

    pMap = pArea->overland->map;
    lastsector = pMap->sector_type + 1;	/* force unequal on first compare */
    for ( y = 0; y < pArea->maxy; y++ )
    {
	fprintf( fp, "<NOBR>" );
	for ( x = 0; x < pArea->maxx; x++ )
	{
	    sector = pMap->sector_type;
	    if ( sector != lastsector )
	    {
		fprintf( fp, "</FONT><FONT COLOR=%s>", sector_data[sector].html_color );
		lastsector = sector;
	    }
	    fprintf( fp, " %c", sector_data[sector].symbol );
	    pMap++;
	}
	fprintf( fp, "</NOBR>\n" );
    }

    fprintf( fp, "</PRE></BODY></HTML>\n" );
    fclose( fp );
    chmod( fname, S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH );

    return FALSE;
}


static bool
dump_jpeg( CHAR_DATA *ch, char *filebase, char *size_str )
{
    char		fname[MAX_INPUT_LENGTH];
    FILE *		fp;
    int			index;
    AREA_DATA *		pArea;
    VIRTUAL_ROOM_DATA *	pMap;
    gdImagePtr		im;
    int			size;
    int			sx;
    int			sy;
    int			x;
    int			y;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
	return FALSE;

    strcpy( fname, filebase );
    strcat( fname, "jpg" );

    size = is_number( size_str ) ? atoi( size_str ) : 1;
    if ( size < 1 || size > 5 )
    {
	send_to_char( "AEdit:  size must be in the range 1 to 5.\n\r", ch );
	return FALSE;
    }

    /* Create the image */
    im = gdImageCreate( pArea->maxx * size, pArea->maxy * size );

    /* Construct palette */
    for ( index = 0; index < SECT_MAX; index++ )
    {
	int r, g, b;
	r = sector_data[index].red;
	g = sector_data[index].green;
	b = sector_data[index].blue;
	sector_data[index].color_index = gdImageColorAllocate( im, r, g, b );
    }

    /* Construct the actual image */
    for ( y = 0; y < pArea->maxy; y++ )
    {
	for ( sy = 0; sy < size; sy++ )
	{
	    pMap = pArea->overland->map + y * pArea->maxx;
	    for ( x = 0; x < pArea->maxx; x++ )
	    {
		for ( sx = 0; sx < size; sx++ )
		{
		    gdImageSetPixel( im,
		    x * size + sx,
		    y * size + sy,
		    sector_data[pMap->sector_type].color_index );
		}
		pMap++;
	    }
	}
    }

    /* Write the file */
    if ( ( fp = fopen( fname, "w" ) ) == NULL )
    {
	send_to_char( "AEdit:  Error opening file.\n\r", ch );
    }
    else
    {
	gdImageJpeg( im, fp, -1 );
	fclose( fp );
	chmod( fname, S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH );
    }

    /* Free the image */
    gdImageDestroy( im );

    return FALSE;
}


static bool
dump_png( CHAR_DATA *ch, char *filebase, char *size_str )
{
    char		fname[MAX_INPUT_LENGTH];
    FILE *		fp;
    int			index;
    AREA_DATA *		pArea;
    VIRTUAL_ROOM_DATA *	pMap;
    gdImagePtr		im;
    int			size;
    int			sx;
    int			sy;
    int			x;
    int			y;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
	return FALSE;

    strcpy( fname, filebase );
    strcat( fname, "png" );

    size = is_number( size_str ) ? atoi( size_str ) : 1;
    if ( size < 1 || size > 5 )
    {
	send_to_char( "AEdit:  size must be in the range 1 to 5.\n\r", ch );
	return FALSE;
    }

    /* Create the image */
    im = gdImageCreate( pArea->maxx * size, pArea->maxy * size );

    /* Construct palette */
    for ( index = 0; index < SECT_MAX; index++ )
    {
	int r, g, b;
	r = sector_data[index].red;
	g = sector_data[index].green;
	b = sector_data[index].blue;
	sector_data[index].color_index = gdImageColorAllocate( im, r, g, b );
    }

    /* Construct the actual image */
    for ( y = 0; y < pArea->maxy; y++ )
    {
	for ( sy = 0; sy < size; sy++ )
	{
	    pMap = pArea->overland->map + y * pArea->maxx;
	    for ( x = 0; x < pArea->maxx; x++ )
	    {
		for ( sx = 0; sx < size; sx++ )
		{
		    gdImageSetPixel( im,
		    x * size + sx,
		    y * size + sy,
		    sector_data[pMap->sector_type].color_index );
		}
		pMap++;
	    }
	}
    }

    /* Write the file */
    if ( ( fp = fopen( fname, "w" ) ) == NULL )
    {
	send_to_char( "AEdit:  Error opening file.\n\r", ch );
    }
    else
    {
	gdImagePng( im, fp );
	fclose( fp );
	chmod( fname, S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH );
    }

    /* Free the image */
    gdImageDestroy( im );

    return FALSE;
}


AREA_DATA *
get_vnum_area( int vnum )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        if ( vnum >= pArea->min_vnum
          && vnum <= pArea->max_vnum )
            return pArea;
    }

    return 0;
}


static int
line_x_major( unsigned x0, unsigned y0, unsigned dx, unsigned dy, int dir,
	      int maxx, int sector, VIRTUAL_ROOM_DATA *map )
{
    int	dy2;		/* dy times 2 */
    int	dy2Mdx2;	/* dy times 2 minus dx times 2 */
    int	err_term;
    int index;
    int count;

    dy2		= dy * 2;
    dy2Mdx2	= dy2 - (int)(dx * 2 );
    err_term	= dy2 - (int)dx;

    count = 0;

    index = y0 * maxx + x0;
    if ( map[index].sector_type != SECT_CITY )
    {
	map[index].sector_type = sector;
	count++;
    }

    while ( dx-- != 0 )
    {
	if ( err_term >= 0 )
	{
	    y0++;
	    err_term += dy2Mdx2;
	}
	else
	{
	    err_term += dy2;
	}

	x0 += dir;

	index = y0 * maxx + x0;
	if ( map[index].sector_type != SECT_CITY )
	{
	    map[index].sector_type = sector;
	    count++;
	}
    }

    return count;
}


static int
line_y_major( unsigned x0, unsigned y0, unsigned dx, unsigned dy, int dir,
	      int maxx, int sector, VIRTUAL_ROOM_DATA *map )
{
    int	dx2;		/* dx times 2 */
    int	dx2Mdy2;	/* dx times 2 minus dy times 2 */
    int	err_term;
    int index;
    int count;

    count = 0;

    dx2		= dx * 2;
    dx2Mdy2	= dx2 - (int)(dy * 2 );
    err_term	= dx2 - (int)dy;

    index = y0 * maxx + x0;
    if ( map[index].sector_type != SECT_CITY )
    {
	map[index].sector_type = sector;
	count++;
    }

    while ( dy-- != 0 )
    {
	if ( err_term >= 0 )
	{
	    x0 += dir;
	    err_term += dx2Mdy2;
	}
	else
	{
	    err_term += dx2;
	}

	y0++;

	index = y0 * maxx + x0;
	if ( map[index].sector_type != SECT_CITY )
	{
	    map[index].sector_type = sector;
	    count++;
	}
    }

    return count;
}


static bool
overland_create( CHAR_DATA *ch, char *argument )
{
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		arg3[MAX_INPUT_LENGTH];
    int			index;
    AREA_DATA *		pArea;
    VIRTUAL_ROOM_DATA *	pMap;
    ROOM_INDEX_DATA *	pRoom;
    int			sector;
    int			size;
    int			x;
    int			y;

    EDIT_AREA ( ch, pArea );

    if ( pArea->overland != NULL )
    {
	send_to_char( "AEdit:  This area already contains an Overlands.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( !is_number( arg1 ) || !is_number( arg2 ) )
    {
	send_to_char( "Syntax:  overland create <x> <y> [sector_type]\n\r", ch );
	return FALSE;
    }

    if ( arg3[0] == '\0' )
	sector = SECT_FIELD;
    else if ( ( sector = flag_value( sector_types, arg3 ) ) == NO_FLAG )
    {
	ch_printf( ch, "AEdit:  Bad sector type '%s'.\n\r", arg3 );
	return FALSE;
    }

    if ( sector == SECT_INSIDE || sector == SECT_CITY || sector == SECT_ROAD )
    {
	send_to_char( "AEdit:  Cannot set all sectors to type inside, city or road.\n\r", ch );
	return FALSE;
    }

    x = atoi( arg1 );
    y = atoi( arg2 );

    if ( x < 5 || x > 2000 || y < 5 || y > 2000 )
    {
	send_to_char( "AEdit:  X and Y must be in the range 5 to 1000.\n\r", ch );
	return FALSE;
    }

    /* check for reasonable aspect ratio */
    if ( x < y / 2 || y < x / 2 )
    {
	send_to_char( "AEdit:  Y must be more than half and less than double X.\n\r", ch );
	return FALSE;
    }

    size = x * y;
    if ( pArea->min_vnum + size - 1 > pArea->max_vnum )
    {
	ch_printf( ch, "AEdit:  Area does not have enough vnums for %d rooms.\n\r", size );
	return FALSE;
    }

    pArea->overland = new_overland( x, y );
    pArea->maxx = x;
    pArea->maxy = y;
    pArea->overland->area = pArea;

    pMap = pArea->overland->map;
    for ( index = 0; index < size; index++ )
    {
	pMap->sector_type = sector;
	pMap->flags = 0;
	pMap++;
    }

    for ( index = 0; index < MAX_KEY_HASH; index++ )
    {
	for ( pRoom = room_index_hash[index]; pRoom != NULL; pRoom = pRoom->next )
	{
	    if ( pRoom->vnum >= pArea->min_vnum
	    &&	 pRoom->vnum <= pArea->min_vnum + size - 1 )
	    {
		pArea->overland->map[pRoom->vnum - pArea->min_vnum].sector_type = pRoom->sector_type;
	    }
	}
    }

    ch_printf( ch, "Overland of %d rooms created.\n\r", size );
    return TRUE;
}


static bool
overland_delete( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    OBJ_DATA *		obj;
    OBJ_DATA *		obj_next;
    CHAR_DATA *		och;
    CHAR_DATA *		och_next;
    ROOM_INDEX_DATA *	room;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
    {
	send_to_char( "AEdit:  This area has no Overlands to delete.\n\r", ch );
	return FALSE;
    }

    /*
     * Move all PCs and their pets to ROOM_VNUM_TEMPLE.
     * Extract all other mobs.
     */
    room = get_room_index( ROOM_VNUM_TEMPLE );
    if ( room == NULL ) /* Uh oh */
	room = get_room_index( ROOM_VNUM_LIMBO );
    for ( och = char_list; och != NULL; och = och_next )
    {
	och_next = och->next;
	if ( och->in_room != NULL
	&&   och->in_room->area == pArea
	&&   get_room_index( och->in_room->vnum ) == NULL )
	{
	    if ( !IS_NPC( och ) || och->desc != NULL )
	    {
		char_from_room( och );
		char_to_room( och, room );
		act_color( AT_ACTION,
			   "The area around you dissolves into mist.  Suddenly you are elsewhere.",
			   och, NULL, NULL, TO_CHAR, POS_RESTING );
		act_color( AT_ACTION, "$n appears in the room.",
			   ch, NULL, NULL, TO_ROOM, POS_RESTING );
		do_function( ch, do_look, "auto" );
	    }
	    else if ( IS_SET( och->act, ACT_PET ) && och->master != NULL )
	    {
		char_from_room( och );
		char_to_room( och, room );
		act_color( AT_ACTION, "$n appears in the room.",
			   ch, NULL, NULL, TO_ROOM, POS_RESTING );
	    }
	    else
	    {
		extract_char( och, TRUE );
	    }
	}
    }

    /* Purge all objects */
    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next;
	if ( obj->in_room != NULL
	&&   obj->in_room->area == pArea
	&&   get_room_index( obj->in_room->vnum ) == NULL )
	{
	    extract_obj( obj );
	}
    }

    free_overland( pArea->overland );
    pArea->overland = NULL;
    pArea->maxx = 0;
    pArea->maxy = 0;
    send_to_char( "Overlands for this area deleted.\n\r", ch );
    return TRUE;
}


static bool
overland_desc( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    char		arg[MAX_INPUT_LENGTH];
    int			sectype;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
    {
	send_to_char( "AEdit:  Area has no overland.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        if ( ( sectype = flag_value( sector_types, arg ) ) == NO_FLAG )
        {
            send_to_char( "AEdit:  Invalid sector type.\n\r", ch );
            return FALSE;
        }
    }
    else
        sectype = ch->in_room->sector_type;

    string_append( ch, &pArea->overland->sect_desc[sectype][DESC_DESC] );
    return TRUE;
}


static bool
overland_dump( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    char		filebase[MAX_INPUT_LENGTH];
    AREA_DATA *		pArea;
    char *		p;
    char *		q;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
    {
	send_to_char( "AEdit:  Area has no overland.\n\r", ch );
	return FALSE;
    }

    q = stpcpy( filebase, WEB_DIR );
    p = pArea->file_name;
    while ( isalpha( *p ) )
	*q++ = *p++;
    strcpy( q, "." );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	dump_png( ch, filebase, "1" );
    else if ( !str_prefix( arg, "ascii" ) )
	dump_ascii( ch, filebase );
    else if ( !str_prefix( arg, "html" ) )
	dump_html( ch, filebase );
    else if ( !str_prefix( arg, "jpeg" ) || !str_prefix( arg, "jpg" ) )
	dump_jpeg( ch, filebase, argument );
    else if ( !str_prefix( arg, "png" ) )
	dump_png( ch, filebase, argument );
    else if ( !str_prefix( arg, "txt" ) || !str_prefix( arg, "text" ) )
	dump_ascii( ch, filebase );
    else
	send_to_char( "AEdit:  File type must be ASCII, HTML, jpeg, png, or text\n\r", ch );

    return FALSE;
}


static bool
overland_edesc( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    char		arg[MAX_INPUT_LENGTH];
    int			sectype;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
    {
	send_to_char( "AEdit:  Area has no overland.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        if ( ( sectype = flag_value( sector_types, arg ) ) == NO_FLAG )
        {
            send_to_char( "AEdit:  Invalid sector type.\n\r", ch );
            return FALSE;
        }
    }
    else
        sectype = ch->in_room->sector_type;

    string_append( ch, &pArea->overland->sect_desc[sectype][DESC_EVEN] );
    return TRUE;
}


static bool
overland_help( CHAR_DATA *ch, char *argument )
{
    send_to_char( "overland create <x> <y> [default sector]\n\r", ch );
    send_to_char( "overland delete\n\r", ch );
    send_to_char( "overland desc [sectortype]\n\r", ch );
    send_to_char( "overland dump [imagetype] [pixelsize]\n\r", ch );
    send_to_char( "overland edesc [sectortype]\n\r", ch );
    send_to_char( "overland help\n\r", ch );
    send_to_char( "overland import\n\r", ch );
    send_to_char( "overland mdesc [sectortype]\n\r", ch );
    send_to_char( "overland ndesc [sectortype]\n\r", ch );
    return FALSE;
}


static bool
overland_import( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    char		filepath[256];
    char *		fn;
    FILE *		fp;
    gdImagePtr		im;
    int			max_virtual;
    int			x;
    int			max_x;
    int			y;
    int			max_y;
    int			vnum_base;
    char *		p;
    char *		q;
    unsigned char *	sectors;
    int			i;
    int			index;
    int			color;
    int			red;
    int			green;
    int			blue;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
    {
        send_to_char( "AEdit:  Area has no overland.\n\r", ch );
        return FALSE;
    }

    q = stpcpy( filepath, UPLOAD_DIR );
    fn = q;
    p = pArea->file_name;
    while ( *p != '.' && *p != '\0' )
        *q++ = *p++;
    q = stpcpy( q, ".png" );

    if ( ( fp = fopen( filepath, "r" ) ) == NULL )
    {
        ch_printf( ch, "AEdit:  File %s not found in upload directory.\n\r", fn );
        return FALSE;
    }

    im = gdImageCreateFromPng( fp );
    fclose( fp );
    if ( im == NULL )
    {
        ch_printf( ch, "AEdit:  Error reading file %s.\n\r", fn );
        return FALSE;
    }

    max_x = pArea->overland->x;
    max_y = pArea->overland->y;
    max_virtual = max_x * max_y;
    vnum_base = pArea->min_vnum;

    if ( gdImageSX( im ) != max_x || gdImageSY( im ) != max_y )
    {
        ch_printf( ch, "AEdit:  Geometry mismatch.\n\r"
                   "Expected X, Y: %d, %d\n\r"
                   "Image X, Y   : %d, %d\n\r",
                   max_x,
                   max_y,
                   gdImageSX( im ),
                   gdImageSY( im ) );
        gdImageDestroy( im );
        return FALSE;
    }

    if ( ( sectors = malloc( max_x * max_y ) ) == NULL )
    {
        send_to_char( "AEdit:  Unable to allocate working storage.\n\r", ch );
        gdImageDestroy( im );
        return FALSE;
    }

    index = 0;
    for ( y = 0; y < max_y; y++ )
    {
        for ( x = 0; x < max_x; x++ )
        {
            color = gdImageGetPixel( im, x, y );
            red   = gdImageRed( im, color );
            green = gdImageGreen( im, color );
            blue  = gdImageBlue( im, color );
            for ( i = 0; i < SECT_MAX; i++ )
            {
                if ( sector_data[i].red   == red
                &&   sector_data[i].green == green
                &&   sector_data[i].blue  == blue )
                    break;
            }
            if ( i >= SECT_MAX )
            {
                ch_printf( ch, "Bad color at %d, %d:  %02X %02X %02X\n\r",
                           x, y, red, green, blue );
                free( sectors );
                gdImageDestroy( im );
                return FALSE;
            }
            sectors[index] = i;
            index++;
        }
    }

    for ( pRoom = pArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
    {
        if ( pRoom->vnum >= vnum_base && pRoom->vnum < vnum_base + max_virtual )
            sectors[pRoom->vnum - vnum_base] = pRoom->sector_type;
    }

    for ( i = 0; i < max_virtual; i++ )
        pArea->overland->map[i].sector_type = sectors[i];

    free( sectors );
    gdImageDestroy( im );
    send_to_char( "Map image imported.\n\r", ch );
    return TRUE;
}


static bool
overland_mdesc( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    char		arg[MAX_INPUT_LENGTH];
    int			sectype;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
    {
	send_to_char( "AEdit:  Area has no overland.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        if ( ( sectype = flag_value( sector_types, arg ) ) == NO_FLAG )
        {
            send_to_char( "AEdit:  Invalid sector type.\n\r", ch );
            return FALSE;
        }
    }
    else
        sectype = ch->in_room->sector_type;

    string_append( ch, &pArea->overland->sect_desc[sectype][DESC_MORN] );
    return TRUE;
}


static bool
overland_ndesc( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    char		arg[MAX_INPUT_LENGTH];
    int			sectype;

    EDIT_AREA( ch, pArea );

    if ( pArea->overland == NULL )
    {
	send_to_char( "AEdit:  Area has no overland.\n\r", ch );
	return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        if ( ( sectype = flag_value( sector_types, arg ) ) == NO_FLAG )
        {
            send_to_char( "AEdit:  Invalid sector type.\n\r", ch );
            return FALSE;
        }
    }
    else
        sectype = ch->in_room->sector_type;

    string_append( ch, &pArea->overland->sect_desc[sectype][DESC_NIGHT] );
    return TRUE;
}


void
show_area_info( AREA_DATA *pArea, BUFFER *buf )
{
    char	changed[SHORT_STRING_LENGTH];
    char *	p;

    if ( pArea == NULL )
	return;

    if ( IS_SET( pArea->area_flags, AREA_CHANGED ) )
	strcpy( changed, ctime( &current_time ) );
    else if ( pArea->last_changed )
	strcpy( changed, ctime( &pArea->last_changed ) );
    else
	changed[0] = '\0';

    if ( ( p = strchr( changed, '\n' ) ) != NULL )
	*p = '\0';

    buf_printf( buf, "Name:      [%4d] %s`w\n\r", pArea->vnum, pArea->name );
    buf_printf( buf, "Credits:   [%s]\n\r", pArea->credits );
    if ( pArea->created != 0 )
        buf_printf( buf, "Created:   [%s]\n\r", date_string( pArea->created ) );
    if ( changed[0] != '\0' )
	buf_printf( buf, "Changed:   [%s]\n\r", changed );
    buf_printf( buf, "Recall:    [%5d] %s\n\r", pArea->recall,
		get_room_index( pArea->recall )
		? get_room_index( pArea->recall )->name : "none" );
    buf_printf( buf, "Norecall:  [%s`w]\n\r",
		!IS_NULLSTR( pArea->norecall )
			? pArea->norecall : "(default)" );
    buf_printf( buf, "Reset msg: [%s`w]\n\r",
		pArea->resetmsg && *pArea->resetmsg != '\0'
			? pArea->resetmsg : "(default)" );
    buf_printf( buf, "Exit size: [%s]\n\r", flag_string( size_types, pArea->exitsize ) );
/*  buf_printf( buf, "Color:     [%s]\n\r", flag_string( color_types, pArea->color ) ); */
    buf_printf( buf, "File:      [%s]\n\r", pArea->file_name );
    buf_printf( buf, "Kingdom:   [%s]\n\r", flag_string( kingdom_types, pArea->kingdom ) );
    buf_printf( buf, "Continent: [%s]\n\r", flag_string( continent_types, pArea->continent ) );
    buf_printf( buf, "World:     [%s]\n\r", flag_string( world_types, pArea->world ) );
    buf_printf( buf, "Vnums:     [%d-%d]\n\r", pArea->min_vnum, pArea->max_vnum );

    if ( pArea->travel_room != NULL )
    {
        buf_printf( buf, "Travel:    [%d] %s\n\r", pArea->travel_room->vnum,
		    pArea->travel_room->name );
    }

    if ( pArea->overland != NULL )
    {
	buf_printf( buf, " (Virtual) [%d-%d] (%d,%d)\n\r", pArea->min_vnum,
		    pArea->min_vnum + pArea->maxx * pArea->maxy -1,
		    pArea->maxx, pArea->maxy );
    }

    buf_printf( buf, "Levels:    [ %s ]\n\r", level_range( pArea ) );

    buf_printf( buf, "Age:       [%d]\n\r", pArea->age );
    buf_printf( buf, "Players:   [%d]\n\r", pArea->nplayer );
    buf_printf( buf, "Security:  [%d]\n\r", pArea->security );
    buf_printf( buf, "Builders:  [%s]\n\r", pArea->builders );
    buf_printf( buf, "Flags:     [%s]\n\r", flag_string( area_flags, pArea->area_flags ) );

    return;
}


void
travedit( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    char		command[MAX_INPUT_LENGTH];
    int			cmd;
    int			dir;
    TRAVEL_DATA *	pTravel;

    if ( IS_NPC( ch ) )
    {
	interpret( ch, argument );
	return;
    }

    EDIT_TRAVEL( ch, pTravel );
    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, command );

    if ( !IS_BUILDER( ch, pTravel->area ) )
	send_to_char( "TravEdit:  Insufficient security to modify area.\n\r", ch );

    if ( command[0] == '\0' )
    {
	travedit_show( ch, "" );
	return;
    }

    if ( !str_cmp( command, "done" ) )
    {
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pTravel->area ) )
    {
	interpret( ch, arg );
	return;
    }

    /* check for direction */
    if ( ( dir = dir_lookup( command ) ) != DIR_NONE )
    {
	if ( travedit_setdir( ch, dir ) )
	    SET_BIT( pTravel->area->area_flags, AREA_CHANGED );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *travedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, travedit_table[cmd].name ) )
	{
	    if ( (*travedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pTravel->area->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
travedit_afixed( CHAR_DATA *ch, char *argument )
{
    TRAVEL_DATA *	pTravel;

    EDIT_TRAVEL( ch, pTravel );

    if ( *argument == '\0' )
    {
	string_append( ch, &pTravel->arrive_room );
	return TRUE;
    }

    send_to_char( "Syntax:  afixed\n\r", ch );
    return FALSE;
}


bool
travedit_atravel( CHAR_DATA *ch, char *argument )
{
    TRAVEL_DATA *	pTravel;

    EDIT_TRAVEL( ch, pTravel );

    if ( *argument == '\0' )
    {
	string_append( ch, &pTravel->arrive_travel );
	return TRUE;
    }

    send_to_char( "Syntax:  atravel\n\r", ch );
    return FALSE;
}


bool
travedit_dfixed( CHAR_DATA *ch, char *argument )
{
    TRAVEL_DATA *	pTravel;

    EDIT_TRAVEL( ch, pTravel );

    if ( *argument == '\0' )
    {
	string_append( ch, &pTravel->depart_room );
	return TRUE;
    }

    send_to_char( "Syntax:  dfixed\n\r", ch );
    return FALSE;
}


bool
travedit_dtravel( CHAR_DATA *ch, char *argument )
{
    TRAVEL_DATA *	pTravel;

    EDIT_TRAVEL( ch, pTravel );

    if ( *argument == '\0' )
    {
	string_append( ch, &pTravel->depart_travel );
	return TRUE;
    }

    send_to_char( "Syntax:  dtravel\n\r", ch );
    return FALSE;
}


bool
travedit_movetime( CHAR_DATA *ch, char *argument )
{
    TRAVEL_DATA *	pTravel;
    int			value;

    EDIT_TRAVEL( ch, pTravel );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  movetime <value>\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );

    if ( value < 1 )
    {
	send_to_char( "TravEdit:  Time must be a positive number.\n\r", ch );
	return FALSE;
    }

    pTravel->move_timer = value;
    if ( pTravel->area->travel_curr == pTravel && !pTravel->stopped )
	pTravel->area->travel_timer = UMIN( value, pTravel->area->travel_timer );

    send_to_char( "Move time set.\n\r", ch );
    return TRUE;
}


bool
travedit_room( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    TRAVEL_DATA *	pTravel;
    ROOM_INDEX_DATA *	pRoom;
    int			vnum;

    EDIT_TRAVEL( ch, pTravel );
    one_argument( argument, arg );

    if ( is_number( arg ) )
	vnum = atoi( arg );
    else if ( !str_cmp( arg, "." ) )
	vnum = ch->in_room->vnum;
    else
    {
	send_to_char( "Syntax:  room <vnum>\n\r", ch );
	return FALSE;
    }

    if ( ( pRoom = get_room_index( vnum ) ) == NULL )
    {
	send_to_char( "TravEdit:  That room does not exist.\n\r", ch );
	return FALSE;
    }

    check_travel( pTravel );

    if ( pTravel->exit_dir != DIR_NONE
    &&	 pRoom->exit[rev_dir[pTravel->exit_dir]] != NULL )
	pTravel->exit_dir = DIR_NONE;

    pTravel->room_vnum = vnum;
    pTravel->room = pRoom;

    send_to_char( "Room set.\n\r", ch );
    return TRUE;
}


bool
travedit_stoptime( CHAR_DATA *ch, char *argument )
{
    TRAVEL_DATA *	pTravel;
    int			value;

    EDIT_TRAVEL( ch, pTravel );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  stoptime <value>\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );

    if ( value < 1 )
    {
	send_to_char( "TravEdit:  Time must be a positive number.\n\r", ch );
	return FALSE;
    }

    pTravel->stop_timer = value;
    if ( pTravel->area->travel_curr == pTravel && pTravel->stopped )
	pTravel->area->travel_timer = UMIN( value, pTravel->area->travel_timer );

    send_to_char( "Stop time set.\n\r", ch );
    return TRUE;
}


bool
travedit_setdir( CHAR_DATA *ch, int dir )
{
    TRAVEL_DATA *	pTravel;
    int			rdir;

    EDIT_TRAVEL( ch, pTravel );

    check_travel( pTravel );

    if ( pTravel->area->travel_room->exit[dir] != NULL )
    {
	send_to_char( "TravEdit:  Moving room already has an exit in that direction.\n\r", ch );
	return FALSE;
    }

    rdir = rev_dir[dir];
    if ( pTravel->room->exit[rdir] != NULL )
    {
	send_to_char( "TravEdit:  Fixed room already has an exit in that direction.\n\r", ch );
	return FALSE;
    }

    pTravel->exit_dir = dir;
    send_to_char( "Exit direction set.\n\r", ch );
    return TRUE;
}


bool
travedit_show( CHAR_DATA *ch, char *argument )
{
    BUFFER *		buf;
    TRAVEL_DATA *	pTravel;

    EDIT_TRAVEL( ch, pTravel );
    buf = new_buf( );

    buf_printf( buf, "`WArrive traveling room:`w\n\r%s", pTravel->arrive_travel );
    buf_printf( buf, "`WArrive fixed room:`w\n\r%s", pTravel->arrive_room );
    buf_printf( buf, "`WDepart traveling room:`w\n\r%s", pTravel->depart_travel );
    buf_printf( buf, "`WDepart fixed room:`w\n\r%s", pTravel->depart_room );
    buf_printf( buf, "Link:      [%5d] %s`X\n\r", pTravel->room_vnum, pTravel->room->name );
    buf_printf( buf, "Exit dir:  [%s]\n\r", pTravel->exit_dir != DIR_NONE ?
		dir_name[pTravel->exit_dir] : "(none)" );
    buf_printf( buf, "Move time: [%d]\n\r", pTravel->move_timer );
    buf_printf( buf, "Stop time: [%d]\n\r", pTravel->stop_timer );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return FALSE;
}


void
show_travel_info( AREA_DATA *pArea, BUFFER *pBuf )
{
    TRAVEL_DATA *	pTravel;
    char		roomname[SHORT_STRING_LENGTH];
    int			vnum;

    if ( pArea->travel_room == NULL )
    {
        add_buf( pBuf, "Area does not have a travel room.\n\r" );
        return;
    }

    strip_color( roomname, pArea->travel_room->name );
    buf_printf( pBuf, "[%5d] %s\n\r", pArea->travel_room->vnum, roomname );

    if ( pArea->travel_first == NULL )
        add_buf( pBuf, "  (Travel list is empty.)\n\r" );
    vnum = 0;
    for ( pTravel = pArea->travel_first; pTravel != NULL; pTravel = pTravel->next )
    {
        vnum++;
        if ( pTravel->room == NULL )
            add_buf( pBuf, "  (unassigned)\n\r" );
        else
        {
            strip_color( roomname, pTravel->room->name );
            roomname[50] = '\0';
            buf_printf( pBuf, "  %2d", vnum );
            if ( pTravel == pArea->travel_curr )
            {
                if ( pTravel->stopped )
                    add_buf( pBuf, "*" );
                else
                    add_buf( pBuf, ">" );
            }
            else
            {
                add_buf( pBuf, " " );
            }
            buf_printf( pBuf, "[%5d] ", pTravel->room->vnum );
            buf_printf( pBuf, "%5d %5d ", pTravel->stop_timer, pTravel->move_timer );
            buf_printf( pBuf, "%-2s ",
                        pTravel->exit_dir == DIR_NONE
                            ? "-"
                            : dir_letter[pTravel->exit_dir] );
            add_buf( pBuf, roomname );
            add_buf( pBuf, "\n\r" );
        }
    }
}

