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
#include "interp.h"
#include "magic.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"

/*
 * Local functions
 */
static bool skedit_class( CHAR_DATA *ch, int class, char *argument );
       int  check_prereq( int sn, int psn );

void
do_skedit( CHAR_DATA *ch, char *argument )
{
    int		 sn;
    char	 arg[MAX_INPUT_LENGTH];
    char	*arglist;


    if ( *argument == '\0' )
    {
	send_to_char( "SKEdit:  There is no default skill/spell to edit.\n\r", ch );
	return;
    }
    arglist = argument;
    argument = one_argument( argument, arg );

    sn = skill_lookup( arglist );
    if ( sn == NO_SKILL )
    {
	send_to_char( "SKEdit:  No such skill/spell exists.\n\r", ch );
	return;
    }

    ch->desc->pEdit = (void *)sn;
    ch->desc->editor = ED_SKILL;
    skedit_show( ch, "" );

    return;

}


/* Skill editor, called by do_skedit() */
void
skedit( CHAR_DATA *ch, char *argument )
{
    int		sn;
    int		cmd;
    int		class;
    char	arg[MAX_INPUT_LENGTH];
    char	command[MAX_INPUT_LENGTH];
    char	buf[MAX_INPUT_LENGTH];
    char	buf1[MAX_INPUT_LENGTH];
    bitvector	value;

    EDIT_SKILL( ch, sn );

    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	skedit_show( ch, "" );
	return;
    }

    if ( !str_cmp( "done", command ) )
    {
	edit_done( ch );
	save_skill_files( );
	return;
    }

    if ( ( value = flag_value( spell_flags, arg ) ) != NO_FLAG )
    {
        if ( skill_table[sn].spell_fun == spell_null )
        {
            ch_printf( ch, "SkEdit:  Cannot set flags on skills.\n\r" );
            return;
        }

        TOGGLE_BIT( skill_table[sn].spell_flags, value );
        send_to_char( "Spell flag(s) toggled.\n\r", ch );
        changed_skill = TRUE;
        return;
    }

    /* Search table and Dispatch Command */
    for ( cmd = 0; *skedit_table[ cmd ].name; cmd++ )
    {
	if ( !str_prefix( command, skedit_table[ cmd ].name ) )
	{
	    changed_skill |= (*skedit_table[ cmd ].olc_fun ) (ch, argument );
	    return;
	}
    }

    /* Couldn't find a command, look for a class name */
    argument = arg;
    buf[0] = '\0';
    do
    {
	strcat( buf, " " );
	argument = one_argument( argument, buf1 );
	strcat( buf, buf1 );
	if ( ( class = class_lookup( &buf[1] ) ) != NO_CLASS )
	{
	    changed_skill |= skedit_class( ch, class, argument );
	    return;
	}	
    }
    while ( *argument != '\0' );

    /* Default to standard interpreter */
    interpret( ch, arg );
    return;
}


bool
skedit_show( CHAR_DATA *ch, char *argument )
{
    int		sn;
    BUFFER *	buf;

    EDIT_SKILL( ch, sn );
    buf = new_buf( );

    show_skill_info( sn, buf );

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );

    return FALSE;
}


static bool
skedit_class( CHAR_DATA *ch, int iClass, char *argument )
{
    int  sn;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int  level;
    int  rating;

    EDIT_SKILL( ch, sn );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    level = atoi( arg1 );
    rating = atoi( arg2 );

    if ( !is_number( arg1 )
	 || ( arg2[0] != '\0' && !is_number( arg2 ) )
	 || ( level < 0 || level >= L_APP ) )
    {
	send_to_char( "Syntax:  <classname> level\n\r", ch );
	send_to_char( "         <classname> level rating\n\r", ch );
	ch_printf( ch, "Level must be between 1 and %d, or 0 to disable for that class.\n\r", L_APP - 1 );
	return FALSE;
    }

    if ( rating < 0 )
    {
	send_to_char( "SKEdit:  Rating must be non-negative.\n\r", ch );
	return FALSE;
    }

    if ( level == 0 )
	level = L_APP;

    skill_table[sn].skill_level[iClass] = level;

    if ( arg2[0] != '\0' )
    {
	skill_table[sn].rating[iClass] = rating;
	send_to_char( "Level and rating set.\n\r", ch );
    }
    else
    {
	send_to_char( "Level set.\n\r", ch );
    }

    return TRUE;
}


bool
skedit_dammsg( CHAR_DATA *ch, char *argument )
{
    int sn;
    char *p;

    EDIT_SKILL( ch, sn );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  dammsg <text>\n\r", ch );
	send_to_char( "         dammsg none\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( argument, "none" ) )
    {
	free_string( skill_table[sn].noun_damage );
	skill_table[sn].noun_damage = str_dup( "" );
	send_to_char( "Damage message removed.\n\r", ch );
	return TRUE;
    }

    if ( skill_table[sn].noun_damage == NULL )
	skill_table[sn].noun_damage = str_dup( "" );
    if ( ( p = string_change( ch, skill_table[sn].noun_damage, argument ) ) != NULL )
    {
	skill_table[sn].noun_damage = p;
	send_to_char( "Damage message set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;
}


bool
skedit_delay( CHAR_DATA *ch, char *argument )
{
    int sn;
    int delay;

    EDIT_SKILL( ch, sn );

    if ( !is_number( argument ) || ( delay = atoi( argument ) ) < 0 )
    {
	send_to_char( "Syntax:  delay <number>\n\r", ch );
	return FALSE;
    }

    skill_table[sn].beats = delay;
    send_to_char( "Delay set.\n\r", ch );
    return TRUE;
}


bool
skedit_forget( CHAR_DATA *ch, char *argument )
{
    int		sn;
    int		forget;

    EDIT_SKILL( ch, sn );

    if ( !is_number( argument ) || ( forget = atoi( argument ) ) < 0 )
    {
	send_to_char( "Syntax:  forget <number>\n\r", ch );
	return FALSE;
    }

    skill_table[sn].forget = forget;
    send_to_char( "Forget factor set.\n\r", ch );
    return TRUE;
}


bool
skedit_mana( CHAR_DATA *ch, char *argument )
{
    int sn;
    int mana;

    EDIT_SKILL( ch, sn );

    if ( !is_number( argument ) || ( mana = atoi( argument ) ) < 0 )
    {
	send_to_char( "Syntax:  mana <number>\n\r", ch );
	return FALSE;
    }

    skill_table[sn].min_mana = mana;
    send_to_char( "Mana set.\n\r", ch );
    return TRUE;
}


bool
skedit_objmsg( CHAR_DATA *ch, char *argument )
{
    int sn;
    char *p;

    EDIT_SKILL( ch, sn );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  objmsg <text>\n\r", ch );
	send_to_char( "         objmsg none\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( argument, "none" ) )
    {
	free_string( skill_table[sn].msg_obj );
	skill_table[sn].msg_obj = str_dup( "" );
	send_to_char( "Object wearoff message removed.\n\r", ch );
	return TRUE;
    }

    if ( skill_table[sn].msg_obj == NULL )
	skill_table[sn].msg_obj = str_dup( "" );
    if ( ( p = string_change( ch, skill_table[sn].msg_obj, argument ) ) != NULL )
    {
	skill_table[sn].msg_obj = p;
	send_to_char( "Object wearoff message set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;
}


bool
skedit_offmsg( CHAR_DATA *ch, char *argument )
{
    int sn;
    char *p;

    EDIT_SKILL( ch, sn );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  offmsg <text>\n\r", ch );
	send_to_char( "         offmsg none\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( argument, "none" ) )
    {
	free_string( skill_table[sn].msg_off );
	skill_table[sn].msg_off = str_dup( "" );
	send_to_char( "Wearoff message removed.\n\r", ch );
	return TRUE;
    }

    if ( skill_table[sn].msg_off == NULL )
	skill_table[sn].msg_off = str_dup( "" );
    if ( ( p = string_change( ch, skill_table[sn].msg_off, argument ) ) != NULL )
    {
	skill_table[sn].msg_off = p;
	send_to_char( "Wearoff message set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;
}


bool
skedit_prereq( CHAR_DATA *ch, char *argument )
{
    int		sn;
    int		index;
    int		ppsn;
    int		psn;

    EDIT_SKILL( ch, sn );

    if ( ( psn = skill_lookup( argument ) ) == NO_SKILL )
    {
	send_to_char( "SkEdit:  Skill not found.\n\r", ch );
	return FALSE;
    }

    if ( sn == psn )
    {
	send_to_char( "SkEdit:  Skill cannot be a prerequisite of itself.\n\r", ch );
	return FALSE;
    }

    /* See if we're deleting a prerequisite */
    for ( index = 0; index < MAX_PREREQ; index++ )
	if ( skill_table[sn].prereq[index] == psn )
	    break;
    if ( index < MAX_PREREQ )
    {
	skill_table[sn].prereq[index] = NO_SKILL;
	send_to_char( "Prerequisite skill removed.\n\r", ch );
	return TRUE;
    }

    /* okay, we're attempting to add a prereq */
    for ( index = 0; index < MAX_PREREQ; index++ )
	if ( skill_table[sn].prereq[index] < 1 )
	    break;

    if ( index >= MAX_PREREQ )
    {
	ch_printf( ch,
		   "SkEdit:  This skill already has the maximum of %d prerequisites.\n\r",
		   MAX_PREREQ );
	return FALSE;
    }

    /* Should add a check for circular prereqs here */
    if ( ( ppsn = check_prereq( sn, psn ) ) != 0 )
    {
	ch_printf( ch, "SkEdit: '%s' is a prereqisite of '%s'\n\r",
		   skill_table[sn].name, skill_table[psn].name );
	return FALSE;
    }

    skill_table[sn].prereq[index] = psn;
    send_to_char( "Prerequisite set.\n\r", ch );
    return TRUE;
}


bool
skedit_roommsg( CHAR_DATA *ch, char *argument )
{
    int sn;
    char *p;

    EDIT_SKILL( ch, sn );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  roommsg <text>\n\r", ch );
	send_to_char( "         roommsg none\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( argument, "none" ) )
    {
	free_string( skill_table[sn].msg_room );
	skill_table[sn].msg_room = str_dup( "" );
	send_to_char( "Wearoff message to room removed.\n\r", ch );
	return TRUE;
    }

    if ( skill_table[sn].msg_room == NULL )
	skill_table[sn].msg_room = str_dup( "" );
    if ( ( p = string_change( ch, skill_table[sn].msg_room, argument ) ) != NULL )
    {
	skill_table[sn].msg_room = p;
	send_to_char( "Wearoff message to room set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;
}


void
show_skill_info( int sn, BUFFER *pBuf )
{
    int		psn;
    bool	isSkill;
    int		iClass;
    int		index;
    int		col;
    bool	found;


    isSkill = ( skill_table[sn].spell_fun == spell_null );
    buf_printf( pBuf, "%s :  [%d] '%s'\n\r", isSkill ? "Skill" : "Spell",
		  sn, skill_table[sn].name );
    if ( !isSkill )
    {
	buf_printf( pBuf, "Flags :  [%s]\n\r",
		    flag_string( spell_flags, skill_table[sn].spell_flags ) );
	buf_printf( pBuf, "Mana  :  %d\n\r", skill_table[sn].min_mana );
    }
    buf_printf( pBuf, "Delay :  %d\n\r", skill_table[sn].beats );
    buf_printf( pBuf, "Forget:  %d\n\r", skill_table[sn].forget );
    buf_printf( pBuf, "Dammsg:  %s`X\n\r",
		  ( skill_table[sn].noun_damage == NULL
		    || skill_table[sn].noun_damage[0] == '\0' )
		    ? "(none)" : skill_table[sn].noun_damage );
    buf_printf( pBuf, "Offmsg:  %s`X\n\r",
		  ( skill_table[sn].msg_off == NULL
		    || skill_table[sn].msg_off[0] == '\0' )
		    ? "(none)" : skill_table[sn].msg_off );
    buf_printf( pBuf, "Objmsg:  %s`X\n\r",
		  ( skill_table[sn].msg_obj == NULL
		    || skill_table[sn].msg_obj[0] == '\0' )
		    ? "(none)" : skill_table[sn].msg_obj );
    buf_printf( pBuf, "Roommsg: %s`X\n\r",
		  ( skill_table[sn].msg_room == NULL
		    || skill_table[sn].msg_room[0] == '\0' )
		    ? "(none)" : skill_table[sn].msg_room );

    found = FALSE;
    add_buf( pBuf, "Prequisites:\n\r" );
    for ( index = 0; index < MAX_PREREQ; index++ )
    {
	psn = skill_table[sn].prereq[index];
	if ( psn > 0 )
	{
	    buf_printf( pBuf, "  '%s'\n\r", skill_table[psn].name );
	    found = TRUE;
	}
    }
    if ( !found )
	add_buf( pBuf, "  (none)\n\r" );

    col = 0;
    found = FALSE;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
	if ( skill_table[sn].skill_level[iClass] != L_APP )
	{
	    if ( !found )
	    {
		add_buf( pBuf, "\n\r" );
		found = TRUE;
	    }
	    buf_printf( pBuf, "%s: %3d %2d  ", class_table[iClass].who_name,
			  skill_table[sn].skill_level[iClass],
			  skill_table[sn].rating[iClass] );
	    if ( !( (++col) % 6 ) )
		add_buf( pBuf, "\n\r" );
	}
    }
    if ( col % 6 )
	add_buf( pBuf, "\n\r" );

    return;
}


int
check_prereq( int sn, int psn )
{
    int index;
    int ppsn;

    if ( psn < 1 )
	return 0;

    if ( sn == psn )
	return sn;

    for ( index = 0; index < MAX_PREREQ; index++ )
	if ( ( ppsn = check_prereq( sn, skill_table[psn].prereq[index] ) ) != 0 )
	    return ppsn;
    return 0;
}

