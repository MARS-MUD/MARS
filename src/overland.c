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
#include <stdlib.h>
#include <time.h>

#include "merc.h"
#include "interp.h"
#include "recycle.h"


/*
 * Local functions
 */
static	int	get_exit_sectors( ROOM_INDEX_DATA *pRoom, int *list );
static	void	set_virtual_desc( ROOM_INDEX_DATA *pRoom );

void
do_map( CHAR_DATA *ch, char *argument )
{
    VIRTUAL_ROOM_DATA * map;
    ROOM_INDEX_DATA *	veh_room;
    char	arg[MAX_INPUT_LENGTH];
    BUFFER *	buf;
    int		plrx;
    int		plry;
    int		char_loc;
    int		lx;		/* left x	*/
    int		rx;		/* right x	*/
    int		ty;		/* top y	*/
    int		by;		/* bottom y	*/
    int		lines;
    int		width;
    int		row;
    int		col;
    int		idx;
    int		idx_newline;
    int		sector;
    int		symbol;
    int		color;
    int		last_color;

    if ( ch->in_room == NULL )
	return;

    if ( ch->in_room->in_room != NULL )
    {
	veh_room = ch->in_room;
	char_from_room( ch );
	char_to_room( ch, veh_room->in_room );
    }
    else
	veh_room = NULL;

    if ( !IS_VIRTUAL( ch->in_room ) )
    {
	send_to_char( "You are not in the overlands.\n\r", ch );
	if ( veh_room != NULL )
	{
	    char_from_room( ch );
	    char_to_room( ch, veh_room );
	}
	return;
    }

    one_argument( argument, arg );
    if ( arg[0] != '\0' )
	lines = atoi( arg );
    else
	lines = UMIN( ch->lines, 25 );

    if ( lines < 2 || lines > 50 )
    {
	send_to_char( "Number of lines must be between 2 and 50.\n\r", ch );
	if ( veh_room != NULL )
	{
	    char_from_room( ch );
	    char_to_room( ch, veh_room );
	}
	return;
    }

    width = 77 / 2 + 1;
    get_coordinates( ch->in_room, &plrx, &plry );
    char_loc = ch->in_room->area->maxx * plry + plrx;
    lx = UMAX( plrx - width / 2, 0 );
    ty = UMAX( plry - lines / 2, 0 );
    rx = UMIN( plrx + width / 2, ch->in_room->area->maxx - 1 );
    by = UMIN( plry + lines / 2, ch->in_room->area->maxy - 1 );
    width = rx - lx + 1;
    lines = by - ty + 1;
    idx = ch->in_room->area->maxx * ty + lx;
    idx_newline = ch->in_room->area->maxx - width;

    last_color = '\0';	/* set to invalid color to always force first color */
    map = ch->in_room->area->overland->map;
    buf = new_buf( );

    for ( row = 0; row < lines; row++ )
    {
	for ( col = 0; col < width; col++ )
	{
	    sector = map[idx].sector_type;
	    if ( idx == char_loc )
	    {
		symbol = '@';
		color  = 'R';
	    }
	    else
	    {
		symbol = sector_data[sector].symbol;
		color  = sector_data[sector].color_char;
	    }
	    if ( color == last_color )
		buf_printf( buf, "%c ", symbol );
	    else
	    {
		buf_printf( buf, "`%c%c ", color, symbol );
		last_color = color;
	    }
	    idx++;
	}

	add_buf( buf, "\n\r" );
	idx += idx_newline;

    }

    send_to_char( buf_string( buf ), ch );
    free_buf( buf );

    if ( veh_room != NULL )
    {
	char_from_room( ch );
	char_to_room( ch, veh_room );
    }

    return;
}


/*
 * Merge a minimap with a string.
 * Assumes the desc has already been formatted by the strng editor
 * format command.  Since I might have to format 36 descs at once,
 * performance is a major conern.
 */
char *
format_map( char *map, char *desc, char area_color )
{
    char	xbuf1[MAX_STRING_LENGTH];
    char	xbuf2[MAX_STRING_LENGTH];
    char	mapbuf[MAX_STRING_LENGTH];
    char	ccbuf[SHORT_STRING_LENGTH];	/* color code buffer */
    char *	rdesc;
    char *	dst;
    char *	p;
    int		linelen;
    int		i;

    if ( IS_NULLSTR( desc ) )
        return desc;

    xbuf1[0] = '\0';
    xbuf2[0] = '\0';
    i = 0;

    /* save any leading color code */
    p = ccbuf;
    if ( is_colcode( desc ) )
    {
        *p++ = *desc++;
        *p++ = *desc++;
    }
    else
    {
        *p++ = '`';
        *p++ = area_color;
    }
    *p = '\0';

    /* Copy the string, substituting spaces for newlines */
    for ( rdesc = desc; *rdesc != '\0'; rdesc++ )
    {
        if ( *rdesc == '\n' )
        {
            if ( i > 0 && ( xbuf1[i-1] == '.' || xbuf1[i-1] == '!' || xbuf1[i-1] == '?' ) )
                xbuf1[i++] = ' ';
            xbuf1[i++] = ' ';
        }
        else if ( *rdesc == '\r' )
            ;
        else
            xbuf1[i++] = *rdesc;
    }
    xbuf1[i] = '\0';

    rdesc = xbuf1;
    dst = xbuf2;
    for ( ; ; )
    {
        p = mapbuf;
        while ( *map != '\0' )
        {
            if ( *map == '\n' )
            {
                map++;
                break;
            }
            *p++ = *map++;
        }
        *p = '\0';

        dst = stpcpy( dst, mapbuf );
        dst = stpcpy( dst, ccbuf );
        linelen = 77 - strlen_wo_col( mapbuf );

        for ( i = 0; i <= linelen; i++ )
        {
            if ( *(rdesc + i) == '\0' )
                break;
            if ( *(rdesc + i) == '`' && strchr( colorcode_list, *(rdesc + i + 1) ) != NULL )
            {
                i++;
                linelen += 2;
            }
        }
        if ( i <= linelen )
            break;

        for ( i = ( xbuf2[0] != '\0' ? linelen : linelen - 3 ); i != 0; i-- )
        {
            if ( *(rdesc + i) == ' ' )
                break;
        }

        if ( i != 0 )
        {
            *(rdesc + i) = '\0';
            dst = stpcpy( dst, rdesc );
            dst = stpcpy( dst, "\n\r" );
            rdesc += i + 1;
            while ( *rdesc == ' ' )
                rdesc++;
        }
        else
        {
            *(rdesc + linelen - 1) = '\0';
            dst = stpcpy( dst, rdesc );
            dst = stpcpy( dst, "-\n\r" );
            rdesc += linelen;
        }
    }

    while ( *(rdesc + i) != '\0' &&
            (  *(rdesc + i) == ' '
            || *(rdesc + i) == '\n'
            || *(rdesc + i) == '\r' ) )
    {
        i--;
    }

    *(rdesc + i + 1) = '\0';
    dst = stpcpy( dst, rdesc );
    if ( *(dst - 2) != '\n' )
        dst = stpcpy( dst, "\n\r" );

    while ( *map != '\0' )
    {
        if ( *map == '\n' )
        {
            *dst++ = *map++;
            *dst++ = '\r';
        }
        else if ( *map == '\r' )
            map++;
        else
            *dst++ = *map++;
    }
    *dst = '\0';

    return str_dup( xbuf2 );
}


bool
get_coordinates( ROOM_INDEX_DATA *pRoom, int *x, int *y )
{
    int offset;

    if ( pRoom->area->overland == NULL )
    {
	*x = -1;
	*y = -1;
	return FALSE;
    }

    offset = pRoom->vnum - pRoom->area->min_vnum;
    *x = offset % pRoom->area->overland->x;
    *y = offset / pRoom->area->overland->x;
    return TRUE;
}


EXIT_DATA *
get_exit( ROOM_INDEX_DATA *room, int dir )
{
    static EXIT_DATA	vexit;
    int			x;
    int			y;

    if ( dir < 0 || dir >= MAX_DIR )
    {
	bug( "Get_exit: bad direction %d.", dir );
	return NULL;
    }

    if ( room->exit[dir] != NULL )
	return room->exit[dir];

    if ( dir == DIR_UP || dir == DIR_DOWN || !IS_VIRTUAL( room ) )
	return NULL;

    get_coordinates( room, &x, &y );

    /* Get adjustment for X */
    if ( dir == DIR_NORTHWEST || dir == DIR_WEST || dir == DIR_SOUTHWEST )
	x--;
    else if ( dir == DIR_NORTHEAST || dir == DIR_EAST || dir == DIR_SOUTHEAST )
	x++;

    /* Get adjustment for Y */
    if ( dir == DIR_NORTHWEST || dir == DIR_NORTH || dir == DIR_NORTHEAST )
	y--;
    else if ( dir == DIR_SOUTHWEST || dir == DIR_SOUTH || dir == DIR_SOUTHEAST )
	y++;

    if ( x < 0 || y < 0 || x >= room->area->maxx || y >= room->area->maxy )
	return NULL;

    vexit.next = NULL;
    vexit.eprogs = NULL;
    vexit.progtypes = 0;
    vexit.vnum = room->area->min_vnum + x + y * room->area->maxx;
    vexit.to_room = get_vroom_index( vexit.vnum );
    if ( !IS_SET( vexit.to_room->room_flags, ROOM_VIRTUAL )
    &&	 !IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return NULL;
    vexit.from_room = room;
    vexit.rs_flags = 0;
    vexit.exit_info = 0;
    vexit.size = SIZE_TITANIC;
    vexit.key = 0;
    vexit.keyword = &str_empty[0];
    vexit.description = &str_empty[0];
    return &vexit;
}


static int
get_exit_sectors( ROOM_INDEX_DATA *pRoom, int *list )
{
    OVERLAND_DATA *	overland;
    int			count;
    int			door;
    int			i;
    int			offset;
    int			x;
    int			y;
    int			maxx;
    int			maxy;

    for ( i = 0; i < MAX_DIR; i++ )
	list[i] = SECT_NONE;

    count = 0;
    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( pRoom->exit[door] != NULL && pRoom->exit[door]->to_room != NULL)
	    list[door] = pRoom->exit[door]->to_room->sector_type;
    }

    if ( !IS_VIRTUAL( pRoom ) )
    {
	for ( i = 0; i < MAX_DIR; i++ )
	    if ( list[i] == pRoom->sector_type )
		count++;
	return count;
    }

    /* Virtual room */
    overland = pRoom->area->overland;
    offset = pRoom->vnum - pRoom->area->min_vnum;
    get_coordinates( pRoom, &x, &y );
    maxx = pRoom->area->overland->x;
    maxy = pRoom->area->overland->y;
    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( pRoom->exit[door] != NULL )
	    continue;

	switch( door )
	{
	    case DIR_NORTH:
		if ( y <= 0 )
		    count++;
		else
		    list[DIR_NORTH] = overland->map[offset - maxx].sector_type;
		break;
	    case DIR_EAST:
		if ( x + 1 >= maxx )
		    count++;
		else
		    list[DIR_EAST] = overland->map[offset + 1].sector_type;
		break;
	    case DIR_SOUTH:
		if ( y + 1 >= maxy )
		    count++;
		else
		    list[DIR_SOUTH] = overland->map[offset + maxx].sector_type;
		break;
	    case DIR_WEST:
		if ( x <= 0 )
		    count++;
		else
		    list[DIR_WEST] = overland->map[offset - 1].sector_type;
		break;
	    case DIR_NORTHWEST:
		if ( x <= 0 || y <= 0 )
		    count++;
		else
		    list[DIR_NORTHWEST] = overland->map[offset - maxx - 1].sector_type;
		break;
	    case DIR_NORTHEAST:
		if ( x + 1 >= maxx || y <= 0 )
		    count++;
		else
		    list[DIR_NORTHEAST] = overland->map[offset - maxx + 1].sector_type;
		break;
	    case DIR_SOUTHWEST:
		if ( x <= 0 || y + 1 >= maxy )
		    count++;
		else
		    list[DIR_SOUTHWEST] = overland->map[offset + maxx - 1].sector_type;
		break;
	    case DIR_SOUTHEAST:
		if ( x + 1 >= maxx || y + 1 >= maxy )
		    count++;
		else
		    list[DIR_SOUTHEAST] = overland->map[offset + maxx + 1].sector_type;
		break;
	    default:
		break;
	}
    }

    for ( i = 0; i < MAX_DIR; i++ )
	if ( list[i] == pRoom->sector_type )
	    count++;

    return count;
}


ROOM_INDEX_DATA *
get_virtual_room( int vnum )
{
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    int			sector;
    int			offset;

    for ( pArea = area_first; pArea; pArea = pArea->next )
	if ( pArea->min_vnum <= vnum && pArea->max_vnum >= vnum )
	    break;

    if ( pArea == NULL
    ||	 pArea->overland == NULL )
	return NULL;

    /* Return the virtual room if it exists */
    for ( pRoom = pArea->overland->room_first; pRoom != NULL; pRoom = pRoom->next )
	if ( pRoom->vnum == vnum )
	    return pRoom;

    /* Check if within virtual room range */
    if ( vnum >= pArea->min_vnum + pArea->maxx * pArea->maxy )
	return NULL;

    /* Else create a new virtual room */
    pRoom = new_room_index( );
    top_vroom++;

    pRoom->prev = NULL;
    if ( pArea->overland->room_first == NULL )
	pArea->overland->room_last = pRoom;
    else
	pArea->overland->room_first->prev = pRoom;
    pRoom->next = pArea->overland->room_first;
    pArea->overland->room_first = pRoom;

    pRoom->vnum = vnum;
    pRoom->area = pArea;
    SET_BIT( pRoom->room_flags, ROOM_VIRTUAL );

    /* Look up the data for the room */
    offset = vnum - pArea->min_vnum;
    sector = pArea->overland->map[offset].sector_type;
    pRoom->sector_type = sector;

    set_virtual_desc( pRoom );

    return pRoom;
}


/*
 * Returns a 7x7 map centered on x and y ordinates.
 */
char *
make_minimap( AREA_DATA *pArea, int offset )
{
    VIRTUAL_ROOM_DATA *	map;
    static char		buf[MAX_STRING_LENGTH];
    char *		p;
    int			idx;
    int			x;
    int			y;
    int			row;
    int			col;
    int			sector;
    int			color;
    int			last_color;
    int			symbol;

    if ( pArea == NULL || pArea->overland == NULL )
    {
        buf[0] = '\0';
        return buf;
    }

    color = '\0';
    p = buf;
    map = pArea->overland->map;
    x = offset % pArea->maxx;
    y = ( offset - x ) / pArea->maxx;
    idx = ( y - 3 ) * pArea->maxx + x - 3;
    for ( row = y - 3; row <= y + 3; row++ )
    {
        if ( row < 0 || row >= pArea->maxy )
        {
            p = stpcpy( p, "               \n" );	/* 15 spaces */
            idx += pArea->maxx;
            continue;
        }

        last_color = '\0';	/* invalid color to force first color */
        for ( col = x - 3; col <= x + 3; col++ )
        {
            if ( col < 0 || col >= pArea->maxx )
                symbol = ' ';
            else
            {
                sector = map[idx].sector_type;
                symbol = sector_data[sector].symbol;
                color  = sector_data [sector].color_char;
            }
            if ( color != last_color )
            {
                *p++ = '`';
                *p++ = color;
                last_color = color;
            }
            *p++ = symbol;
            *p++ = ' ';
            idx++;
        }
        idx += pArea->maxx - 7;
        *p++ = ' ';
        *p++ = '\n';
    }

    *p = '\0';
    return buf;
}


static void
set_virtual_desc( ROOM_INDEX_DATA *pRoom )
{
    OVERLAND_DATA *	overland;
    int			list[MAX_DIR];
    int			countlist[SECT_MAX];
    char		wordlist[MAX_STRING_LENGTH];
    char		buf[MAX_STRING_LENGTH];
    char		word[SHORT_STRING_LENGTH];
    int			sect_count;
    char *		map;
    char *		desc;
    char *		edesc;
    char *		mdesc;
    char *		ndesc;
    char *		str;
    char *		p;
    char *		q;
    char		cchar;
    int			i;
    int			adjoin;

    if ( !IS_SET( pRoom->room_flags, ROOM_VIRTUAL ) )
        return;

    overland = pRoom->area->overland;
    if ( overland == NULL )
    {
        bug( "Virtual room #%d not in overland area.", pRoom->vnum );
        return;
    }

    sect_count = get_exit_sectors( pRoom, list );

    /*
     * Determine most common adjoining sector type
     */
    for ( i = 0; i < ( sizeof( countlist ) / sizeof( countlist[0] ) ); i++ )
	countlist[i] = 0;
    for ( i = 0; i < MAX_DIR; i++ )
    {
	if ( list[i] >= 0 && list[i] < SECT_MAX )
	    countlist[list[i]]++;
    }
    adjoin = countlist[0];
    for ( i = 1; i < ( sizeof( countlist ) / sizeof( countlist[0] ) ); i++ )
	if ( countlist[i] > countlist[adjoin] )
	    adjoin = i;

    /*
     * Set room name.
     */
    switch( pRoom->sector_type )
    {
	case SECT_AIR:
	    if ( sect_count >= 4 )
		str = "In the Open Air";
	    else
		str = "In the Air";
	    break;

	case SECT_ARCTIC:
	    if ( sect_count >= 4 )
		str = "Within the Arctic Waste";
	    else
		str = "Edge of the Arctic Waste";
	    break;

	case SECT_BEACH:
	    str = "On the Beach";
	    break;

	case SECT_BOILING:
	    if ( sect_count >= 4 )
		str = "Within the Boiling Waters";
	    else
		str = "Boiling Water";
	    break;

	case SECT_DESERT:
	    if ( sect_count >= 4 )
		str = "Within the Desert";
	    else
		str = "Desert Wasteland";
	    break;

	case SECT_FIELD:
	    if ( sect_count >= 4 )
		str = "On the Plains";
	    else
		str = "In a Field";
	    break;

	case SECT_FOREST:
	    if ( sect_count >= 4 )
		str = "Within a Forest";
	    else
		str = "In a Forest";
	    break;

	case SECT_HILLS:
	    if ( sect_count >= 4 )
		str = "In the Foothills";
	    else
		str = "On a Hill";
	    break;
	
	case SECT_LAKE:
	    if ( sect_count >= 4 )
		str = "On a Lake";
	    else
		str = "On a Lake Shore";
	    break;

	case SECT_MOUNTAIN:
	    if ( sect_count >= 4 )
		str = "In the Mountains";
	    else
		str = "On a Mountain";
	    break;

	case SECT_OCEAN:
	    if ( sect_count >= 4 )
		str = "On the Open Ocean";
	    else
		str = "Deep Water";
	    break;

	case SECT_RIVER:
	    str = "On a River";
	    break;

	case SECT_ROAD:
	    switch( sect_count )
	    {
		case 1:  str = "End of the Road";	break;
		case 2:  str = "On the Road";		break;
		case 3:  str = "Fork in the Road";	break;
		case 4:  str = "Four Way Intersection";break;
		case 5:  str = "Five Corners";		break;
		case 6:  str = "Six Points";		break;
		default: str = "At an Intersection";	break;
	    }
	    break;

	case SECT_SWAMP:
	    if ( sect_count >= 4 )
		str = "Within the Swamp";
	    else
		str = "Edge of a Swamp";
	    break;

	case SECT_UNDERWATER:
	    str = "Underwater";
	    break;

	case SECT_WASTELAND:
	    if (sect_count >= 4 )
		str = "In the Wastelands";
	    else
		str = "Border of the Wastelands";
	    break;

	case SECT_WATER_NOSWIM:
	    if ( sect_count >= 4 )
		str = "On the Sea";
	    else
		str = "On the Water";
	    break;

	case SECT_WATER_SWIM:
	    if ( sect_count >= 4 )
		str = "Shallow Basin";
	    else
		str = "Shallow Water";
	    break;

	default:
	    str = "Nothingness";
	    break;
    }
    pRoom->name = str_dup( str );

    /*
     * Generate the room description.
     */
    desc  = "This sector type needs a description.\n\r";
    edesc = &str_empty[0];
    mdesc = &str_empty[0];
    ndesc = &str_empty[0];

    switch( pRoom->sector_type )
    {
	case SECT_AIR:
	    break;

	case SECT_ARCTIC:
	case SECT_BEACH:
	case SECT_BOILING:
	case SECT_DESERT:
	case SECT_FIELD:
	case SECT_FOREST:
	case SECT_HILLS:
	case SECT_LAKE:
	case SECT_MOUNTAIN:
	case SECT_OCEAN:
	case SECT_RIVER:
	case SECT_SWAMP:
	case SECT_UNDERWATER:
	case SECT_WASTELAND:
	case SECT_WATER_NOSWIM:
	case SECT_WATER_SWIM:
	    desc = overland->sect_desc[pRoom->sector_type][DESC_DESC];
	    if ( IS_NULLSTR( desc ) )
            {
                sprintf( buf, "`RSector type %s needs a description.\n\r", sector_data[pRoom->sector_type].name );
                desc = buf;
            }
            edesc = overland->sect_desc[pRoom->sector_type][DESC_EVEN];
            mdesc = overland->sect_desc[pRoom->sector_type][DESC_MORN];
            ndesc = overland->sect_desc[pRoom->sector_type][DESC_NIGHT];
	    break;
	case SECT_ROAD:
	    q = wordlist;
	    for ( i = 0; i < ( sizeof( list ) / sizeof( list[0] ) ); i++ )
	    {
	        if ( list[i] == SECT_ROAD )
	        {
	            *q++ = ' ';
	            q = stpcpy( q, dir_name[i] );
                }
            }
            *q = '\0';
            q = wordlist + 1;
	    p = buf + sprintf( buf,
	    "A wide road has been constructed and the surrounding lands\n\r"
	    "cleared to allow passage to various locations about the\n\r"
	    "continent.  At times road workers may be seen fixing a hole\n\r"
	    "or clearing a ditch to assure it remains in good repair.\n\r"
	    "The road passes through %s.\n\r",
		     sector_data[adjoin].desc );
	    switch( sect_count )
	    {
		case 0:
		    str = "It doesn't seem to lead anywhere, however.\n\r";
		    break;
		case 1:
		    str = "The road comes to an end here.\n\r";
		    break;
		case 2:
		    p = stpcpy( p, "The road leads " );
		    q = one_argument( q, word );
		    p = stpcpy( p, word );
		    p = stpcpy( p, " and " );
		    q = one_argument( q, word );
		    p = stpcpy( p, word );
		    str = ".\n\r";
		    break;
		case 3:
		    p = stpcpy( p, "Roads lead " );
		    q = one_argument( q, word );
		    p = stpcpy( p, word );
		    p = stpcpy( p, ", " );
		    q = one_argument( q, word );
		    p = stpcpy( p, word );
		    p = stpcpy( p, " and " );
		    q = one_argument( q, word );
		    p = stpcpy( p, word );
		    str = " from here.\n\r";
		    break;
		case 4:  str = "Four Way Intersection\n\r";break;
		case 5:  str = "Five Corners\n\r";		break;
		case 6:  str = "Six Points\n\r";		break;
		default: str = "At an Intersection\n\r";	break;
	    }
	    strcpy( p, str );
	    desc = buf;
	    break;

	default:
	    desc = "Nothingness\n\r";
	    break;
    }

    map = make_minimap( pRoom->area, pRoom->vnum - pRoom->area->min_vnum );
    cchar = colorcode_list[sector_data[pRoom->sector_type].room_color];

    free_string( pRoom->description  );
    free_string( pRoom->evening_desc );
    free_string( pRoom->morning_desc );
    free_string( pRoom->night_desc   );
    pRoom->description  = format_map( map, desc, cchar );
    pRoom->evening_desc = format_map( map, edesc, cchar );
    pRoom->morning_desc = format_map( map, mdesc, cchar );
    pRoom->night_desc   = format_map( map, ndesc, cchar );
}

