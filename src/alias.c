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
#include <string.h>
#include <time.h>

#include "merc.h"
#include "interp.h"
#include "recycle.h"

/* does aliasing and other fun stuff */
void
substitute_alias( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH],prefix[MAX_INPUT_LENGTH],name[MAX_INPUT_LENGTH];
    char *point;
    ALIAS_DATA *alias;
    char *	p;

    ch = d->original ? d->original : d->character;

    /* check for prefix */
    if (ch->prefix[0] != '\0' && str_prefix("prefix",argument))
    {
	if (strlen(ch->prefix) + strlen(argument) > MAX_INPUT_LENGTH)
	    send_to_char("Line to long, prefix not processed.\r\n",ch);
	else
	{
	    sprintf(prefix,"%s %s",ch->prefix,argument);
	    argument = prefix;
	}
    }

    if ( IS_NPC( ch ) || ch->pcdata->alias_list == NULL
    ||	 !str_prefix( "alias", argument )
    ||	 !str_prefix( "una",argument ) 
    ||	 !str_prefix( "prefix", argument ) ) 
    {
	if ( !run_olc_editor( d, argument ) )
	    interpret( d->character, argument );
	return;
    }

    strcpy( buf, argument );

    for ( alias = ch->pcdata->alias_list; alias != NULL; alias = alias->next )
    {
	if ( !str_prefix( alias->old, argument ) )
	{
	    point = one_argument( argument, name );
	    if ( !strcmp( alias->old, name ) )
	    {
		p = stpcpy( buf, alias->new );
		*p++ = ' ';
		p = stpcpy( p, point );
		if ( p - buf > MAX_INPUT_LENGTH - 1 )
	        {
		    send_to_char(
			"Alias substitution too long. Truncated.\r\n",
			d->character );
		    buf[MAX_INPUT_LENGTH -1] = '\0';
	        }
		break;
	    }
	}
    }

    if ( !run_olc_editor( d, buf ) )
	interpret( d->character, buf );
}

void do_alia(CHAR_DATA *ch, char *argument)
{
    send_to_char("I'm sorry, alias must be entered in full.\n\r",ch);
    return;
}

void
do_alias( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	rch;
    char 	arg[MAX_INPUT_LENGTH];
    BUFFER *	buf;
    ALIAS_DATA *pAlias;
    ALIAS_DATA *alias_new;

    smash_tilde( argument );

    if ( ch->desc == NULL )
	rch = ch;
    else
	rch = ch->desc->original ? ch->desc->original : ch;

    if ( IS_NPC( rch ) )
	return;

    argument = one_argument( argument, arg );

    if (arg[0] == '\0')
    {
	buf = new_buf( );
	if ( rch->pcdata->alias_list == NULL )
	    add_buf( buf, "You have no aliases defined.\n\r" );
	else
	    add_buf( buf, "Your current aliases are:\n\r" );

	for ( pAlias = rch->pcdata->alias_list; pAlias != NULL; pAlias = pAlias->next )
	    buf_printf( buf,"    %s:  %s\n\r", pAlias->old, pAlias->new );

	page_to_char( buf_string( buf ), ch );
	free_buf( buf );
	return;
    }

    if ( !str_prefix( "una", arg ) || !str_cmp( "alias", arg ) )
    {
	send_to_char( "Sorry, that word is reserved.\n\r", ch );
	return;
    }

    if ( *argument == '\0' )
    {
	for ( pAlias = rch->pcdata->alias_list; pAlias != NULL; pAlias = pAlias->next )
	{
	    if ( !str_cmp( arg, pAlias->old ) )
	    {
		ch_printf( ch, "%s aliases to '%s'.\n\r",
			   pAlias->old, pAlias->new );
		return;
	    }
	}

	send_to_char( "That alias is not defined.\n\r", ch );
	return;
    }

    if ( !str_prefix( argument, "delete" )
    ||	 !str_prefix( argument, "prefix" ) )
    {
	send_to_char( "That shall not be done!\n\r", ch );
	return;
    }

    /* Search for an alias to replace */
    for ( pAlias = rch->pcdata->alias_list; pAlias != NULL; pAlias = pAlias->next )
    {
	if ( !str_cmp( arg, pAlias->old ) ) /* redefine an alias */
	{
	    free_string( pAlias->new );
	    pAlias->new =  str_dup( argument );
	    ch_printf( ch, "%s is now realiased to '%s'.\n\r", arg, argument );
	    return;
	}
     }

     /* make a new alias */
    alias_new = new_alias( );
    alias_new->old		= str_dup( arg );
    alias_new->new		= str_dup( argument );
    alias_new->next		= rch->pcdata->alias_list;
    rch->pcdata->alias_list	= alias_new;

    ch_printf( ch, "%s is now aliased to '%s'.\n\r", arg, argument );
}


void do_unalias(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *	rch;
    ALIAS_DATA *pAlias;
    ALIAS_DATA *alias_delete;
    char	arg[MAX_INPUT_LENGTH];

    if ( ch->desc == NULL )
	rch = ch;
    else
	rch = ch->desc->original ? ch->desc->original : ch;

    if ( IS_NPC( rch ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unalias what?\n\r", ch );
	return;
    }

    if ( rch->pcdata->alias_list == NULL )
    {
	send_to_char( "You have no aliases to remove.\n\r", ch );
	return;
    }

    alias_delete = NULL;
    if ( !str_cmp( arg, rch->pcdata->alias_list->old ) )
    {
	alias_delete = rch->pcdata->alias_list;
	rch->pcdata->alias_list = alias_delete->next;
    }
    else
    {
	for ( pAlias = rch->pcdata->alias_list; ; pAlias = pAlias->next )
	{
	    if ( pAlias->next == NULL )
		break;
	    if ( !str_cmp( arg, pAlias->next->old ) )
	    {
		alias_delete = pAlias->next;
		pAlias->next = pAlias->next->next;
		break;
	    }
	}
    }

    if ( alias_delete == NULL )
	send_to_char( "No alias of that name to remove.\n\r", ch );
    else
    {
	free_alias( alias_delete );
	send_to_char( "Alias removed.\n\r", ch );
    }
}


