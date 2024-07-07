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


/*
 * Local functions
 */
static bool gedit_class( CHAR_DATA *ch, int class, char *argument );


void
do_gedit( CHAR_DATA *ch, char *argument )
{
    int		gn;
    char	arg[MAX_INPUT_LENGTH];
    char *	arglist;


    if ( *argument == '\0' )
    {
	send_to_char( "GEdit:  There is no default skill group to edit.\n\r", ch );
	return;
    }
    arglist = argument;
    argument = one_argument( argument, arg );

    gn = group_lookup( arglist );
    if ( gn == NO_SKILL )
    {
	send_to_char( "GEdit:  No such skill group exists.\n\r", ch );
	return;
    }

    ch->desc->pEdit = (void *)gn;
    ch->desc->editor = ED_GROUP;
    gedit_show( ch, "" );

    return;

}


/* Skill group editor, called by do_gedit() */
void
gedit( CHAR_DATA *ch, char *argument )
{
    int	 gn;
    int  cmd;
    int  class;
    char arg[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    char buf1[MAX_INPUT_LENGTH];

    EDIT_GROUP( ch, gn );

    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	gedit_show( ch, "" );
	return;
    }

    if ( !str_cmp( "done", command ) )
    {
	edit_done( ch );
	save_skill_files( );
	return;
    }

    /* Search table and Dispatch Command */
    for ( cmd = 0; *gedit_table[ cmd ].name; cmd++ )
    {
	if ( !str_prefix( command, gedit_table[ cmd ].name ) )
	{
	    changed_group |= (*gedit_table[ cmd ].olc_fun ) (ch, argument );
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
	    changed_group |= gedit_class( ch, class, argument );
	    return;
	}	
    }
    while ( *argument != '\0' );

    /* Default to standard interpreter */
    interpret( ch, arg );
    return;
}


bool
gedit_add( CHAR_DATA *ch, char *argument )
{
    int		gn;
    int		sn;
    int		i;
    bool	fGroup;

    EDIT_GROUP( ch, gn );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  add spell|skill|group\n\r", ch );
	return FALSE;
    }

    if ( ( sn = group_lookup( argument ) ) != NO_SKILL )
	fGroup = TRUE;
    else if ( ( sn = skill_lookup( argument ) ) != NO_SKILL )
	fGroup = FALSE;

    if ( sn == NO_SKILL )
    {
	send_to_char( "GEdit:  No such skill, group or spell.\n\r", ch );
	return FALSE;
    }

    if ( fGroup )
    {
	for ( i = 0; i < MAX_IN_GROUP; i++ )
	{
	    if ( group_table[gn].spells[i] && !str_cmp( group_table[sn].name, group_table[gn].spells[i] ) )
	    {
		send_to_char( "GEdit:  Group is already on list.\n\r", ch );
		return FALSE;
	    }
	}
    }
    else
    {
	for ( i = 0; i < MAX_IN_GROUP; i++ )
	{
	    if ( group_table[gn].spells[i] && !str_cmp( skill_table[sn].name, group_table[gn].spells[i] ) )
	    {
		send_to_char( "GEdit:  Skill/spell is already on list.\n\r", ch );
		return FALSE;
	    }
	}
    }

    for ( i = 0; i < MAX_IN_GROUP; i++ )
	if ( group_table[gn].spells[i] == NULL || *group_table[gn].spells[i] == '\0' )
	    break;
    if ( i >= MAX_IN_GROUP )
    {
	send_to_char( "GEdit:  No more skills may be added to this group.\n\r", ch );
	return FALSE;
    }

    if ( fGroup && sn == gn )
    {
	send_to_char( "GEdit:  Cannot add a group to itself.\n\r", ch );
	return FALSE;
    }

    /* Put a check for circular inclusions here? */

    group_table[gn].spells[i] = str_dup( fGroup ? group_table[sn].name
						 : skill_table[sn].name );

    ch_printf( ch, "%s added.\n\r", fGroup ? "Group" :
	       skill_table[sn].spell_fun == spell_null ? "Skill" : "Spell" );
    return TRUE;
}


static
bool gedit_class( CHAR_DATA *ch, int iClass, char *argument )
{
    int  gn;
    char arg[MAX_INPUT_LENGTH];
    int  rating;

    EDIT_GROUP( ch, gn );

    argument = one_argument( argument, arg );
    rating = atoi( arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  <classname> rating\n\r", ch );
	return FALSE;
    }

    rating = atoi( arg );
    if ( rating < 0 )
	rating = -1;
    group_table[gn].rating[iClass] = rating;

    send_to_char( "Rating set.\n\r", ch );

    return TRUE;
}


bool
gedit_drop( CHAR_DATA *ch, char *argument )
{
    int		gn;
    int		sn;
    int		count;

    EDIT_GROUP( ch, gn );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  drop spell|skill|group\n\r", ch );
	return FALSE;
    }

    if ( !group_table[gn].spells[0] || *group_table[gn].spells[0] == '\0' )
    {
	send_to_char( "GEdit:  List is empty, nothing to drop.\n\r", ch );
	return FALSE;
    }

    for ( sn = 0; sn < MAX_IN_GROUP; sn++ )
    {
	if ( group_table[gn].spells[sn]
	     && *group_table[gn].spells[sn]
	     && !str_prefix( argument, group_table[gn].spells[sn] ) )
	{
	    break;
	}
    }
    if ( sn >= MAX_IN_GROUP )
    {
	send_to_char( "GEdit:  Not on list.\n\r", ch );
	return FALSE;
    }

    free_string( group_table[gn].spells[sn] );
    for ( count = sn + 1; count < MAX_IN_GROUP; count++ )
    {
	group_table[gn].spells[count - 1] = group_table[gn].spells[count];
    }
    group_table[gn].spells[MAX_IN_GROUP - 1] = NULL;

    send_to_char( "Group/skill/spell dropped.\n\r", ch );
    return TRUE;
}


bool
gedit_show( CHAR_DATA *ch, char *argument )
{
    int		gn;
    int		sn;
    BUFFER *	pBuf;
    int		iClass;
    bool	found;

    EDIT_GROUP( ch, gn );

    pBuf = new_buf( );

    buf_printf( pBuf, "Name:  %s\n\r", group_table[gn].name );

    found = FALSE;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
	if ( !class_table[iClass].name || class_table[iClass].name[0] == '\0' )
	    break;
	if ( group_table[gn].rating[iClass] >= 0 )
	{
	    buf_printf( pBuf, "%s  %2d\n\r", class_table[iClass].who_name,
			group_table[gn].rating[iClass] );
	    found = TRUE;
	}
    }
    if ( !found )
	add_buf( pBuf, "This group has not been assigned a class.\n\r" );

    found = FALSE;
    for ( sn = 0; sn < MAX_IN_GROUP; sn++ )
    {
	if ( group_table[gn].spells[sn] && *group_table[gn].spells[sn] != '\0' )
	{
	    buf_printf( pBuf, "%s\n\r", group_table[gn].spells[sn] );
	    found = TRUE;
	}
    }
    if ( !found )
	add_buf( pBuf, "This group has not been assigned any skills.\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return FALSE;
}


