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
#include "db.h"
#include "interp.h"
#include "lookup.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"


static int	get_stat_index( const char *str );


void
do_racedit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    bool	found;
    int		iRace;

    if( IS_NPC( ch ) )
    {
	send_to_char( "Mobs dont build, they are built!\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "RacEdit:  There is no default race to edit.\n\r", ch );
	return;
    }

    found = FALSE;
    for ( iRace = 0; !IS_NULLSTR( race_table[iRace].name ); iRace++ )
    {
	if ( !str_prefix( arg, race_table[iRace].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
    {
	for ( iRace = 0; !IS_NULLSTR( race_table[iRace].name ); iRace++ )
	{
	    if ( !str_prefix( arg, race_table[iRace].who_name ) )
	    {
		found = TRUE;
		break;
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "RacEdit:  No such race.\n\r", ch );
	return;
    }

    if ( !str_cmp( race_table[iRace].name, "unique" ) )
    {
	send_to_char( "RacEdit:  That race cannot be edited.\n\r", ch );
	return;
    }

    ch->desc->pEdit  = (void *)iRace;
    ch->desc->editor = ED_RACE;

    racedit_show( ch, "" );
    return;
}


void
racedit( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		cmd;
    char	command[MAX_INPUT_LENGTH];
    int		iClass;
    int		iRace;
    int		value;

    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	racedit_show( ch, "" );
	return;
    }

    if ( !str_cmp( command, "done" ) )
    {
	edit_done( ch );
	save_race_file( );
	return;
    }

    /* Search table and dispatch command */
    for ( cmd = 0; racedit_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( !str_prefix( command, racedit_table[cmd].name ) )
	{
	    changed_race |= (*racedit_table[cmd].olc_fun) ( ch, argument );
	    return;
	}
    }

    /* Check for class multiplier */
    if ( ( iClass = class_lookup( command ) ) != NO_CLASS
    &&   !IS_SET( class_table[iClass].flags, CLASS_DISABLED )
    &&   is_number( argument ) )
    {
	value = atoi( argument );
	if ( value < 0 )
	{
	    send_to_char( "RacEdit:  Bad value for class multiplier.\n\r", ch );
	    return;
	}
	EDIT_RACE( ch, iRace );
	race_table[iRace].class_mult[iClass] = value;
	send_to_char( "Class multiplier set.\n\r", ch );
	changed_race = TRUE;
	return;
    }

    /* Default to standard interpreter */
    interpret( ch, arg );
    return;
}


bool
racedit_act( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    bitvector	flag;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  act <act flag list>\n\r", ch );
	return FALSE;
    }

    flag = flag_value( act_flags, argument );
    if ( flag == NO_FLAG )
    {
	send_to_char( "RacEdit:  Bad flag.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].act ^= flag;
    send_to_char( "Act flags changed.\n\r", ch );
    return TRUE;
}


bool
racedit_affect( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    int		aff;
    char	arg[MAX_INPUT_LENGTH];

    EDIT_RACE( ch, iRace );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  affect <affect>\n\r", ch );
	return FALSE;
    }

    aff = flag_value( affect_flags, arg );
    if ( aff == NO_FLAG || aff == AFF_NONE )
    {
	send_to_char( "RacEdit:  Bad affect.  See \"? affect\".\n\r", ch );
	return FALSE;
    }

    xTOGGLE_BIT( race_table[iRace].aff, aff );
    send_to_char( "Affect toggled.\n\r", ch );
    return TRUE;

}


bool
racedit_align( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    char	arg[MAX_INPUT_LENGTH];
    char *	p;
    int		min;
    int		max;

    EDIT_RACE( ch, iRace );
    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  alignment [G|N|E]\n\r", ch );
	return FALSE;
    }

    min = 1000;
    max = -1000;

    while ( *argument != '\0' )
    {
	argument = one_argument( argument, arg );
	p = arg;
	while ( *p != '\0' )
	{
	    if ( *p == 'g' )
	    {
	        max = UMAX( max, 1000 );
	        min = UMIN( min, 350 );
	    }
	    else if ( *p == 'n' )
	    {
		max = UMAX( max, 349 );
		min = UMIN( min, -349 );
	    }
	    else if ( *p == 'e' )
	    {
		max = UMAX( max, -350 );
		min = UMIN( min, -1000 );
	    }
	    else
	    {
		ch_printf( ch, "RacEdit:  Unknown alignment '%c'.\n\r", UPPER( *p ) );
		return FALSE;
	    }
	    p++;
	}
    }

    race_table[iRace].max_align = max;
    race_table[iRace].min_align = min;
    send_to_char( "Alignment set.\n\r", ch );
    return TRUE;
}


bool
racedit_allclass( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    int		iClass;
    int		value;
    char	arg[MAX_INPUT_LENGTH];

    EDIT_RACE( ch, iRace );

    one_argument( argument, arg );
    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	send_to_char( "Syntax:  allclass <value>\n\r", ch );
	return FALSE;
    }

    value = atoi( arg );

    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	race_table[iRace].class_mult[iClass] = value;

    send_to_char( "Class multiplier for all classes set.\n\r", ch );
    return TRUE;
}


bool
racedit_form( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    bitvector	flag;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  form <flag list>\n\r", ch );
	return FALSE;
    }

    flag = flag_value( form_flags, argument );
    if ( flag == NO_FLAG )
    {
	send_to_char( "RacEdit:  Bad flag.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].form ^= flag;
    send_to_char( "Form flags changed.\n\r", ch );
    return TRUE;
}


bool
racedit_immune( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    bitvector	flag;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  immune <flag list>\n\r", ch );
	return FALSE;
    }

    flag = flag_value( imm_flags, argument );
    if ( flag == NO_FLAG )
    {
	send_to_char( "RacEdit:  Bad flag.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].imm ^= flag;
    send_to_char( "Immune flags changed.\n\r", ch );
    return TRUE;
}


bool
racedit_maxstat( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		iRace;
    int		stat;
    int		value;

    EDIT_RACE( ch, iRace );

    argument = one_argument( argument, arg );

    if ( !is_number( argument ) || ( stat = get_stat_index( arg ) ) == NO_VALUE )
    {
	send_to_char( "Syntax:  maxstat <stat> <value>\n\r"
		      "Stat must be one of str, int, dex, wis, con.\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < 9 || value > 22 )
    {
	send_to_char( "RacEdit:  Bad value.\n\r", ch );
	return FALSE;
    }

    if ( value < race_table[iRace].stats[stat] )
    {
	send_to_char( "RacEdit:  Value must not be less than starting stat.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].max_stats[stat] = value;
    send_to_char( "Max stat set.\n\r", ch );
    return TRUE;
}


bool
racedit_offensive( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    int		flag;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  offensive < flag list>\n\r", ch );
	return FALSE;
    }

    flag = flag_value( off_flags, argument );
    if ( flag == NO_FLAG )
    {
	send_to_char( "RacEdit:  Bad flag.\n\r", ch );
	return FALSE;
    }

    xTOGGLE_BIT( race_table[iRace].off, flag );
    send_to_char( "Offense flags changed.\n\r", ch );
    return TRUE;
}


bool
racedit_parts( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    bitvector	flag;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  parts <flag list>\n\r", ch );
	return FALSE;
    }

    flag = flag_value( part_flags, argument );
    if ( flag == NO_FLAG )
    {
	send_to_char( "RacEdit:  Bad flag.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].parts ^= flag;
    send_to_char( "Parts flags changed.\n\r", ch );
    return TRUE;
}


bool
racedit_pcrace( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    bool	value;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
	value = !race_table[iRace].pc_race;
    else if ( !str_prefix( argument, "true" )
    ||	      !str_prefix( argument, "yes" ) )
	value = TRUE;
    else if ( !str_prefix( argument, "false" )
    ||	      !str_prefix( argument, "no" ) )
	value = FALSE;
    else
    {
	send_to_char( "Syntax:  pcrace\n\r"
		      "         pcrace <yes | no>\n\r"
		      "         pcrace <true | false>\n\r", ch );
	return FALSE;
    }

    if ( race_table[iRace].pc_race == value )
    {
	send_to_char( "PC race setting not changed.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].pc_race = value;
    send_to_char( value
	? "Race now available to PC's\n\r"
	: "Race now NPC only.\n\r", ch );
    return TRUE;
}


bool
racedit_points( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    char	arg[MAX_INPUT_LENGTH];
    int		value;

    EDIT_RACE( ch, iRace );

    one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  points <value>\n\r", ch );
	return FALSE;
    }

    value = atoi( arg );

    race_table[iRace].points = value;
    send_to_char( "Creation points set.\n\r", ch );
    return TRUE;
}


bool
racedit_resist( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    bitvector	flag;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  resist <flag list>\n\r", ch );
	return FALSE;
    }

    flag = flag_value( res_flags, argument );
    if ( flag == NO_FLAG )
    {
	send_to_char( "RacEdit:  Bad flag.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].res ^= flag;
    send_to_char( "Resist flags changed.\n\r", ch );
    return TRUE;
}


bool
racedit_show( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    BUFFER *	buf;

    EDIT_RACE( ch, iRace );

    buf = new_buf( );

    show_race_info( iRace, buf );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );

    return FALSE;
}


bool
racedit_size( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    char	arg[MAX_INPUT_LENGTH];
    int		value;

    EDIT_RACE( ch, iRace );

    one_argument( argument, arg );

    if ( arg[0] == '\0'
    ||	 ( value = size_lookup( arg ) ) == NO_FLAG
    ||	 value == SIZE_TITANIC )
    {
	send_to_char( "Syntax:  size <size>\n\r"
		      "See \"? size\" for a list of valid sizes.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].size = value;
    send_to_char( "Size set.\n\r", ch );

    return TRUE;
}


bool
racedit_skill( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    int		sn;
    int		index;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  skill <skill>\n\r", ch );
	return FALSE;
    }

    if ( ( sn = skill_lookup( argument ) ) == NO_SKILL || sn == 0 )
    {
	send_to_char( "RacEdit:  No such skill.\n\r", ch );
	return FALSE;
    }

    for ( index = 0; index < MAX_RACE_SKILLS; index++ )
	if ( IS_NULLSTR( race_table[iRace].skills[index] )
	||   !str_cmp( race_table[iRace].skills[index], skill_table[sn].name ) )
	    break;

    if ( index >= MAX_RACE_SKILLS )
    {
	/* Entire list searched, no match found; no room for another. */
	ch_printf( ch, "This race already has the maximum of %d skills.\n\r", MAX_RACE_SKILLS );
	return FALSE;
    }

    if ( IS_NULLSTR( race_table[iRace].skills[index] ) )
    {
	/* Empty slot found, add skill */
	race_table[iRace].skills[index] = str_dup( skill_table[sn].name );
	send_to_char( "Skill added.\n\r", ch );
	return TRUE;
    }

    /* Found the skill in the list, delete it. */
    free_string( race_table[iRace].skills[index] );
    while ( index < MAX_RACE_SKILLS - 1 )
    {
	race_table[iRace].skills[index] = race_table[iRace].skills[index+1];
	index++;
    }
    race_table[iRace].skills[index] = NULL;
    send_to_char( "Skill removed.\n\r", ch );
    return TRUE;
}


bool
racedit_startstat( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    int		iRace;
    int		stat;
    int		value;

    EDIT_RACE( ch, iRace );

    argument = one_argument( argument, arg );

    if ( !is_number( argument ) || ( stat = get_stat_index( arg ) ) == NO_VALUE )
    {
	send_to_char( "Syntax:  startstat <stat> <value>\n\r"
		      "Stat must be one of str, int, dex, wis, con.\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < 6 || value > 18 )
    {
	send_to_char( "RacEdit:  Bad value.\n\r", ch );
	return FALSE;
    }

    if ( value > race_table[iRace].max_stats[stat] )
    {
	send_to_char( "RacEdit:  Value must not be greater than max stat.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].stats[stat] = value;
    send_to_char( "Starting stat set.\n\r", ch );
    return TRUE;
}


bool
racedit_vulnerable( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    bitvector	flag;

    EDIT_RACE( ch, iRace );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  vulnerable <flag list>\n\r", ch );
	return FALSE;
    }

    flag = flag_value( vuln_flags, argument );
    if ( flag == NO_FLAG )
    {
	send_to_char( "RacEdit:  Bad flag.\n\r", ch );
	return FALSE;
    }

    race_table[iRace].vuln ^= flag;
    send_to_char( "Vulnerable flags changed.\n\r", ch );
    return TRUE;
}


bool
racedit_weight( CHAR_DATA *ch, char *argument )
{
    int		iRace;
    char	arg[MAX_INPUT_LENGTH];
    char *	p;
    int		min;
    int		max;

    EDIT_RACE( ch, iRace );

    argument = one_argument( argument, arg );

    /*
     * Quick hack to allow syntax such as "10 lb 50 lb".
     * Does not do a full weight string parse.
     */
    if ( isdigit( arg[0] ) && !isdigit( *argument ) )
    {
        p = arg + strlen( arg );
        while ( *argument != '\0' && !isspace( *argument ) )
            *p++ = *argument++;
        while ( isspace( *argument ) )
            argument++;
        *p = '\0';
    }

    if ( !str_cmp( arg, "min" ) && weight_value( argument, &min ) )
    {
        max = race_table[iRace].weight_max;
    }
    else if ( !str_cmp( arg, "max" ) && weight_value( argument, &max ) )
    {
        min = race_table[iRace].weight_min;
        if ( max <= min )
        {
            send_to_char( "RacEdit:  Max weight must be greater than min.\n\r", ch );
            return FALSE;
        }
    }
    else if ( !weight_value( arg, &min ) || !weight_value( argument, &max ) )
    {
        send_to_char( "Syntax:  weight <min_value> <max_value>\n\r", ch );
        send_to_char( "         weight min <min_value>\n\r", ch );
        send_to_char( "         weight max <max_value>\n\r", ch );
        return FALSE;
    }

    if ( min < 1 )
    {
        send_to_char( "RacEdit:  Min weight must be at least 1.\n\r", ch );
        return FALSE;
    }

    if ( min >= max )
    {
        send_to_char( "RacEdit:  Min weight must be less than max.\n\r", ch );
        return FALSE;
    }

    race_table[iRace].weight_min = min;
    race_table[iRace].weight_max = max;

    send_to_char( "Weight set.\n\r", ch );
    return TRUE;
}


static int
get_stat_index( const char *stat )
{
    if ( !str_cmp( stat, "str" ) )	return STAT_STR;
    if ( !str_cmp( stat, "int" ) )	return STAT_INT;
    if ( !str_cmp( stat, "wis" ) )	return STAT_WIS;
    if ( !str_cmp( stat, "dex" ) )	return STAT_DEX;
    if ( !str_cmp( stat, "con" ) )	return STAT_CON;

    return NO_VALUE;
}


void
show_race_info( int iRace, BUFFER *buf )
{
    int		col;
    int		i;
    char *	p;
    char	tmp[MAX_INPUT_LENGTH];

    buf_printf( buf, "Name:         %s", race_table[iRace].name );
    if ( race_table[iRace].pc_race )
	buf_printf( buf, "    Who name:  %s", race_table[iRace].who_name );
    add_buf( buf, "\n\r" );
    buf_printf( buf, "PC race?      [%s]\n\r", race_table[iRace].pc_race ? "Yes" : "No" );

    tmp[0] = '\0';
    p = tmp;
    if ( race_table[iRace].max_align >= 350 )
	*p++ = 'G';
    if ( race_table[iRace].min_align < 350 && race_table[iRace].max_align > -350 )
	*p++ = 'N';
    if ( race_table[iRace].min_align <= -350 )
	*p++ = 'E';
    *p = '\0';
    buf_printf( buf, "Align:        [%s]\n\r", tmp );

    buf_printf( buf, "Act flags:    [%s]\n\r", flag_string( act_flags, race_table[iRace].act ) );
    buf_printf( buf, "Affects:      [%s]\n\r", xbit_string( affect_flags, race_table[iRace].aff ) );
    buf_printf( buf, "Offensive:    [%s]\n\r", xbit_string( off_flags, race_table[iRace].off ) );
    buf_printf( buf, "Immune:       [%s]\n\r", flag_string( imm_flags, race_table[iRace].imm ) );
    buf_printf( buf, "Resist:       [%s]\n\r", flag_string( res_flags, race_table[iRace].res ) );
    buf_printf( buf, "Vuln:         [%s]\n\r", flag_string( vuln_flags, race_table[iRace].vuln ) );
    buf_printf( buf, "Form:         [%s]\n\r", flag_string( form_flags, race_table[iRace].form ) );
    buf_printf( buf, "Parts:        [%s]\n\r", flag_string( part_flags, race_table[iRace].parts ) );
    buf_printf( buf, "Size:         [%s]  ", size_name( race_table[iRace].size ) );
    buf_printf( buf, "Weight min [%s] ", weight_string( race_table[iRace].weight_min ) );
    buf_printf( buf, "max [%s]\n\r", weight_string( race_table[iRace].weight_max ) );
    add_buf( buf, "\n\r" );
    buf_printf( buf, "Points:       [%d]\n\r", race_table[iRace].points );
    buf_printf( buf, "Skills:       [" );
    tmp[0] = '\0';
    p = tmp;
    for ( i = 0; i < MAX_RACE_SKILLS; i++ )
    {
	if ( !IS_NULLSTR( race_table[iRace].skills[i] ) )
	    p += sprintf( p, " '%s'", race_table[iRace].skills[i] );
	else
	    break;
    }
    if ( tmp[0] == '\0' )
	strcpy( tmp, " (none)" );
    add_buf( buf, &tmp[1] );
    add_buf( buf, "]\n\r" );
    buf_printf( buf, "Start stats:  str[%2d]  int [%2d]  wis [%2d]  dex [%2d]  con [%2d]\n\r",
		race_table[iRace].stats[STAT_STR],
		race_table[iRace].stats[STAT_INT],
		race_table[iRace].stats[STAT_WIS],
		race_table[iRace].stats[STAT_DEX],
		race_table[iRace].stats[STAT_CON] );
    buf_printf( buf, "Max stats:    str[%2d]  int [%2d]  wis [%2d]  dex [%2d]  con [%2d]\n\r",
		race_table[iRace].max_stats[STAT_STR],
		race_table[iRace].max_stats[STAT_INT],
		race_table[iRace].max_stats[STAT_WIS],
		race_table[iRace].max_stats[STAT_DEX],
		race_table[iRace].max_stats[STAT_CON] );

    add_buf( buf, "Class multipliers:\n\r" );
    col = 0;
    for ( i = 0; i < MAX_CLASS; i++ )
    {
        if ( IS_SET( class_table[i].flags, CLASS_DISABLED ) )
            continue;
	buf_printf( buf, "   %s [%3d]", class_table[i].who_name, race_table[iRace].class_mult[i] );
	if ( ++col % 6 == 0 )
	    add_buf( buf, "\n\r" );
    }
    if ( col % 6 != 0 )
	add_buf( buf, "\n\r" );

    return;
}


