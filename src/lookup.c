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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "lookup.h"
#include "tables.h"

/*
 *  Local functions
 */
static bool is_stat		args( ( const struct flag_type *flag_table ) );
static int  flag_lookup_exact	args( ( const char *name,
					const struct flag_type *flag_table ) );

int
dir_lookup( const char *name )
{
   	 if ( IS_NULLSTR( name ) )
	return DIR_NONE;
    else if ( !str_prefix( name, "north" ) )
	return DIR_NORTH;
    else if ( !str_prefix( name, "east" ) )
	return DIR_EAST;
    else if ( !str_prefix( name, "south" ) )
	return DIR_SOUTH;
    else if ( !str_prefix( name, "west" ) )
	return DIR_WEST;
    else if ( !str_prefix( name, "up" ) )
	return DIR_UP;
    else if ( !str_prefix( name, "down" ) )
	return DIR_DOWN;
    else if ( !str_prefix( name, "northwest" ) || !str_prefix( name, "nwest" ) )
	return DIR_NORTHWEST;
    else if ( !str_prefix( name, "northeast" ) || !str_prefix( name, "neast" ) )
	return DIR_NORTHEAST;
    else if ( !str_prefix( name, "southwest" ) || !str_prefix( name, "swest" ) )
	return DIR_SOUTHWEST;
    else if ( !str_prefix( name, "southeast" ) || !str_prefix( name, "seast" ) )
	return DIR_SOUTHEAST;
    else
	return DIR_NONE;
}


int
door_lookup( const char *name )
{
    int door;

    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( !str_cmp( name, dir_name[door] )
	|| !str_cmp( name, dir_letter[door] ) )
	    return door;
    }

    return DIR_NONE;
}


int
event_lookup( const char *name, int etype )
{
    const struct flag_type *	table;
    int				i;

    switch( etype )
    {
	case EVENT_OWNER_AREA:
	    table = event_area_types;	break;
	case EVENT_OWNER_CHAR:
	    table = event_char_types;	break;
	case EVENT_OWNER_DESC:
	    table = event_desc_types;	break;
	case EVENT_OWNER_GAME:
	    table = event_game_types;	break;
	case EVENT_OWNER_OBJ:
	    table = event_obj_types;	break;
	case EVENT_OWNER_ROOM:
	    table = event_room_types;	break;
	default:
	    bugf( "Bad event type %d for '%s'.", etype, name );
	    return EVENT_NONE;
    }

    for ( i = 0; table[i].name != NULL; i++ )
	if ( !str_cmp( table[i].name, name ) )
	    return table[i].bit;

    return EVENT_NONE;
}


const char *
event_name( int vnum, int etype )
{
    const struct flag_type *	table;
    int				i;

    switch( etype )
    {
	case EVENT_OWNER_AREA:
	    table = event_area_types;	break;
	case EVENT_OWNER_CHAR:
	    table = event_char_types;	break;
	case EVENT_OWNER_DESC:
	    table = event_desc_types;	break;
	case EVENT_OWNER_GAME:
	    table = event_game_types;	break;
	case EVENT_OWNER_OBJ:
	    table = event_obj_types;	break;
	case EVENT_OWNER_ROOM:
	    table = event_room_types;	break;
	default:
	    bugf( "Bad event type %d for %d.", etype, vnum );
	    return "none";
    }

    for ( i = 0; table[i].name != NULL; i++ )
	if ( table[i].bit == vnum )
	    return table[i].name;

    return "none";
}


static int
flag_lookup_exact ( const char *name,
	const struct flag_type *flag_table )
{
    int flag;

    for ( flag = 0; flag_table[flag].name != NULL; flag++ )
    {
	if (    !str_cmp( name, flag_table[flag].name )
	     && flag_table[flag].settable )
	return flag_table[flag].bit;
    }

    return NO_FLAG;
}

int
flag_lookup( const char *name, const struct flag_type *flag_table )
{
    int flag;

    for ( flag = 0; flag_table[flag].name != NULL; flag++ )
    {
	if (    LOWER( *name ) == LOWER( flag_table[flag].name[0] )
	     && !str_prefix(name,flag_table[flag].name)
	     && flag_table[flag].settable )
	return flag_table[flag].bit;
    }

    return NO_FLAG;
}


int
clan_lookup( const char *name )
{
    CLAN_DATA *pClan;

    for ( pClan = clan_first; pClan; pClan = pClan->next )
    {
	if ( LOWER(name[0] ) == LOWER( pClan->name[0] )
	&&  !str_prefix( name, pClan->name ) )
	    return pClan->vnum;
    }

    return 0;
}

int
level_lookup( const char *lev )
{
    int level;

    if ( is_number( lev ) )
	return atoi( lev );
    level = flag_value( level_types, lev );
    if ( level == NO_FLAG )
	level = flag_value( old_level_types, lev );
    if ( level == NO_FLAG )
	bugf( "Bad level \"%s\"", lev );
    return level == NO_FLAG ? 1 : level;
}

const char *
level_name( int level )
{
    static char buf[SHORT_STRING_LENGTH];
    const char *lev;

    lev = capitalize( flag_string( level_types, level ) );
    if ( !str_cmp( lev, "None" ) )
    {
	sprintf( buf, "%d", level );
	lev = buf;
    }
    return lev;
}


const char *
mprog_name( bitvector value )
{
    int index;

    for ( index = 0; mprog_types[index].name != NULL; index++ )
	if ( mprog_types[index].bit == value )
	    return mprog_types[index].name;

    return "error_prog";
}


bitvector
mprog_type( const char *name )
{
    bitvector type;

    type = flag_value( mprog_types, name );
    return ( type == NO_FLAG ? ERROR_PROG : type );
}


int
position_lookup ( const char *name )
{
   int pos;

   for ( pos = 0; position_table[pos].name != NULL; pos++ )
   {
	if ( LOWER( name[0] ) == LOWER( position_table[pos].name[0] )
	&&  !str_prefix( name,position_table[pos].name ) )
	    return pos;
   }

   return NO_POSITION;
}


int
security_lookup( const char *name )
{
    int sec;

    if ( name == NULL || *name == '\0' )
	return NO_FLAG;

    for ( sec = 0; security_flags[sec].name != NULL; sec++ )
    {
	if ( LOWER( name[0] ) == LOWER( *security_flags[sec].name )
	&&  !str_prefix( name, security_flags[sec].name ) )
	    return sec;
    }

    return NO_FLAG;
}


int
sex_lookup( const char *name )
{
    return flag_value( sex_types, name );
}

const char *
sex_name( int value )
{
    return flag_string( sex_types, value );
}


int
size_lookup( const char *name )
{
   return flag_value( size_types, name );
}

const char *
size_name( int value )
{
    return flag_string( size_types, value );
}


/*****************************************************************************
 Name:		flag_string( table, flags/stat )
 Purpose:	Returns string with name(s) of the flags or stat entered.
 Called by:	act_olc.c, olc.c, and olc_save.c.
 ****************************************************************************/
char *
flag_string( const struct flag_type *flag_table, int bits )
{
    static char buf[512];
    int  flag;

    buf[0] = '\0';

    for (flag = 0; flag_table[flag].name; flag++)	/* OLC 1.1b */
    {
	if ( !is_stat( flag_table ) && IS_SET(bits, flag_table[flag].bit) )
	{
	    strcat( buf, " " );
	    strcat( buf, flag_table[flag].name );
	}
	else
	if ( flag_table[flag].bit == bits )
	{
	    strcat( buf, " " );
	    strcat( buf, flag_table[flag].name );
	    break;
	}
    }
    return (buf[0] != '\0') ? buf+1 : "none";
}


/*****************************************************************************
 Name:		flag_value( table, flag )
 Purpose:	Returns the value of the flags entered.  Multi-flags accepted.
 ****************************************************************************/
int
flag_value( const struct flag_type *flag_table, const char *argument)
{
    char	word[MAX_INPUT_LENGTH];
    bitvector	bit;
    bitvector	marked = 0;
    int		value;
    bool	found = FALSE;

    if ( is_stat( flag_table ) )
    {
	one_argument( argument, word );

	if ( ( value = flag_lookup( word, flag_table ) ) != NO_FLAG )
	    return value;
	else
	    return NO_FLAG;
    }

    /*
     * Accept multiple flags.
     */
    for ( ; ; )
    {
        argument = one_argument( argument, word );

        if ( word[0] == '\0' )
	    break;

        if ( ( bit = flag_lookup_exact( word, flag_table ) ) == NO_FLAG )
        {
	    return NO_FLAG;
        }
        SET_BIT( marked, bit );
        found = TRUE;
    }

    if ( found )
	return marked;
    else
	return NO_FLAG;
}


/*****************************************************************************
 Name:		is_stat( table )
 Purpose:	Returns TRUE if the table is a stat table and FALSE if flag.
 Called by:	flag_value and flag_string.
 Note:		This function is local and used only in flags.c.
 ****************************************************************************/
static bool
is_stat( const struct flag_type *flag_table )
{
    int flag;

    for (flag = 0; flag_stat_table[flag].structure; flag++)
    {
	if ( flag_stat_table[flag].structure == flag_table
	  && flag_stat_table[flag].stat )
	    return TRUE;
    }
    return FALSE;
}


int
item_lookup( const char *name )
{
    int type;

    for ( type = 0; item_types[type].name != NULL; type++ )
    {
	if ( LOWER( name[0] ) == LOWER( item_types[type].name[0] )
	&&  !str_prefix( name, item_types[type].name ) )
	    return item_types[type].bit;
    }

    return NO_FLAG;
}


char *
item_name(int item_type)
{
    return flag_string( item_types, item_type );
}


int
windspeed_lookup( const char *name )
{
    int speed;

    for ( speed = 0; windspeed_types[speed].name != NULL; speed++ )
    {
	if ( LOWER( name[0] ) == LOWER( windspeed_types[speed].name[0] )
	&&   !str_prefix( name, windspeed_types[speed].name ) )
	    return speed;
    }

    return NO_FLAG;
}


/*****************************************************************************
 Name:		flag_xbits( table, flags )
 Purpose:	Returns string with name(s) of the flags entered.
 Called by:	act_olc.c, olc.c, and olc_save.c.
 ****************************************************************************/
char *
xbit_string( const struct flag_type *flag_table, EXT_BV bits )
{
    static char buf[2048];
    char *	p;
    int		flag;

    buf[0] = '\0';
    p = buf;

    for ( flag = 0; flag_table[flag].name; flag++ )	/* OLC 1.1b */
    {
	if ( /*!is_stat( flag_table ) &&*/ xIS_SET( bits, flag_table[flag].bit ) )
	{
	    *p++ = ' ';
	    p = stpcpy( p, flag_table[flag].name );
	}
    }
    return ( buf[0] != '\0') ? buf+1 : "none" ;
}


