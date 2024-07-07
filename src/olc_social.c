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
#include "olc.h"
#include "recycle.h"
#include "tables.h"

/*
 * Local functions
 */
bool social_replace( CHAR_DATA *ch, char **socialstr, char *argument );
bool social_update( CHAR_DATA *ch, char **socialstr, char *argument, bool flag );


void do_sedit( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;
    SOCIAL_DATA *pTmp;
    char	 arg [MAX_INPUT_LENGTH];
    char	 arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "SEdit:  There is no default social to edit.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "show" ) )
    {
	do_socstat( ch, argument );
	return;
    }

    if ( !str_prefix( arg, "create" ) )
    {
	if ( *argument == '\0' )
	{
	    send_to_char( "SEdit:  Create what social?", ch );
	    return;
	}

	argument = one_argument( argument, arg2 );
	if ( *argument != '\0' )
	{
	    send_to_char( "SEdit:  Social names must be one word.\n\r", ch );
	    return;
	}
	for ( pSocial = social_first; pSocial; pSocial = pSocial->next )
	{
	    if ( !str_cmp( arg2, pSocial->name ) )
	    {
		if ( !pSocial->deleted )
		{
		    send_to_char( "SEdit:  Social already exists.\n\r", ch );
		    return;
		}
		/* recycle a deleted social */
		free_string( pSocial->cnoarg );
		free_string( pSocial->onoarg );
		free_string( pSocial->cfound );
		free_string( pSocial->ofound );
		free_string( pSocial->vfound );
		free_string( pSocial->nfound );
		free_string( pSocial->cself );
		free_string( pSocial->oself );
		pSocial->cnoarg   = str_dup( "" );
		pSocial->onoarg   = str_dup( "" );
		pSocial->cfound   = str_dup( "" );
		pSocial->ofound   = str_dup( "" );
		pSocial->vfound   = str_dup( "" );
		pSocial->nfound	  = str_dup( "They're not here" );
		pSocial->cself	  = str_dup( "" );
		pSocial->oself	  = str_dup( "" );
		pSocial->ed_level = ch->level;
		pSocial->deleted  = FALSE;
		changed_social	  = TRUE;

		ch->desc->pEdit = (void *)pSocial;
		ch->desc->editor = ED_SOCIAL;
		sedit_show( ch, "" );
		return;
	    }
	}

	pSocial = new_social( );
	pSocial->name = str_dup( arg2 );
	pSocial->nfound = str_dup( "They aren't here." );
	pSocial->ed_level = ch->level;

	if ( social_first == NULL || strcmp( arg2, social_first->name ) < 0 )
	{
	    pSocial->next = social_first;
	    social_first = pSocial;
	    if ( social_last == NULL )
	    {
		social_last = social_first;
	    }
	}
	else if ( strcmp( arg2, social_last->name ) > 0 )
	{
	    social_last->next = pSocial;
	    pSocial->next = NULL;
	    social_last = pSocial;
	}
	else
	{
	    for ( pTmp = social_first; pTmp; pTmp = pTmp->next )
	    {
		if ( strcmp( arg2, pTmp->next->name ) < 0 )
		{
		    pSocial->next = pTmp->next;
		    pTmp->next = pSocial;
		    break;
		}
	    }
	}
	strcpy( arg, arg2 );
	changed_social = TRUE;
    }


    for ( pSocial = social_first; pSocial; pSocial = pSocial->next )
    {
	if ( !str_cmp( arg, pSocial->name ) )
	    break;
    }

    if ( pSocial == NULL )
    {
	send_to_char( "SEdit: No such social exists.\n\r", ch );
	return;
    }

    ch->desc->pEdit = (void *)pSocial;
    ch->desc->editor = ED_SOCIAL;
    sedit_show( ch, "" );
    return;
}


/* Social interpreter, called by do_sedit() */
void sedit( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;
    char	 arg[MAX_INPUT_LENGTH];
    char	 command[MAX_INPUT_LENGTH];
    int		 cmd;

    EDIT_SOCIAL( ch, pSocial );

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, command );

    if ( !str_cmp( "done", command ) )
    {
	edit_done( ch );

	if ( get_trust( ch ) >= pSocial->ed_level )
	    save_socials( );

	return;
    }

    if ( command[0] == '\0' )
    {
	sedit_show( ch, "" );
	return;
    }

    if ( get_trust( ch ) < pSocial->ed_level )
    {
	send_to_char( "SEdit:  Insufficient security to modify social.\n\r", ch );
	interpret( ch, arg );
	return;
    }

    /* Search table and Dispatch Command */
    for ( cmd = 0; *sedit_table[ cmd ].name; cmd++ )
    {
	if ( !str_prefix( command, sedit_table[ cmd ].name ) )
	{
	    changed_social |= (*sedit_table[ cmd ].olc_fun ) (ch, argument );
	    return;
	}
    }

    /* Default to standard interpreter */
    interpret( ch, arg );
    return;
}

bool sedit_show( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;
    char	 buf[ MAX_STRING_LENGTH ];
    char	*p;

    EDIT_SOCIAL( ch, pSocial );

    p = buf;
    p += sprintf( p, "Name    : %s\n\r", pSocial->name    );
    p += sprintf( p, "Level   : [%d] %s\n\r", pSocial->ed_level,
		  flag_string( level_types, pSocial->ed_level ) );
    p += sprintf( p, "cnoarg  : %s`X\n\r", pSocial->cnoarg  );
    p += sprintf( p, "onoarg  : %s`X\n\r", pSocial->onoarg  );
    p += sprintf( p, "cfound  : %s`X\n\r", pSocial->cfound  );
    p += sprintf( p, "ofound  : %s`X\n\r", pSocial->ofound  );
    p += sprintf( p, "vfound  : %s`X\n\r", pSocial->vfound  );
    p += sprintf( p, "nfound  : %s`X\n\r", pSocial->nfound );
    p += sprintf( p, "cself   : %s`X\n\r", pSocial->cself   );
    p += sprintf( p, "oself   : %s`X\n\r", pSocial->oself   );
    if ( pSocial->deleted )
    {
	p = stpcpy( p, "\n\rThis social is scheduled to be deleted.\n\r" );
    }

    send_to_char( buf, ch );
    return FALSE;
}

bool sedit_edlevel( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;
    char	 arg[MAX_INPUT_LENGTH];
    int		 level;

    EDIT_SOCIAL( ch, pSocial );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  level <level>\n\r", ch );
	send_to_char( " Level is the level required to edit this social.\n\r", ch );
	return FALSE;
    }

    if ( is_number( arg ) )
	level = atoi( arg );
    else
    {
	level = flag_value( level_types, arg );
	if ( level == NO_FLAG )
	{
	    send_to_char( "SEdit:  Invalid level.\n\r", ch );
	    return FALSE;
	}
    }

    if ( level < LEVEL_IMMORTAL )
    {
	send_to_char( "SEdit:  Level must be an immortal level.\n\r", ch );
	return FALSE;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "SEdit:  Limited to your trust level.\n\r", ch );
	return FALSE;
    }

    pSocial->ed_level = level;
    send_to_char( "Edit level set.\n\r", ch );
    return TRUE;
}

bool sedit_cnoarg( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    return social_update( ch, &(pSocial->cnoarg), argument, FALSE );
}

bool sedit_onoarg( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    return social_update( ch, &(pSocial->onoarg), argument, FALSE );
}

bool sedit_cfound( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    return social_update( ch, &(pSocial->cfound), argument, TRUE );
}

bool sedit_ofound( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    return social_update( ch, &(pSocial->ofound), argument, TRUE );
}

bool sedit_vfound( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    return social_update( ch, &(pSocial->vfound), argument, TRUE );
}

bool sedit_nfound( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    return social_update( ch, &(pSocial->nfound), argument, FALSE );
}

bool sedit_cself( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    return social_update( ch, &(pSocial->cself), argument, TRUE );
}

bool sedit_oself( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    return social_update( ch, &(pSocial->oself), argument, TRUE );
}

bool sedit_delet( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you wish to delete this social, spell it out.\n\r", ch );
    return FALSE;
}

bool sedit_delete( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    pSocial->deleted = !pSocial->deleted;
    if ( pSocial->deleted )
    {
	send_to_char( "This social has been scheduled for deletion.\n\r", ch );
    }
    else
    {
	send_to_char( "Social un-deleted.\n\r", ch );
    }
    return TRUE;
}

bool sedit_undelete( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;

    EDIT_SOCIAL( ch, pSocial );

    pSocial->deleted = FALSE;
    send_to_char( "Social un-deleted.\n\r", ch );
    return TRUE;
}

/* Replaces part of the text of a social.  Works like string editor .r cmd */
bool social_replace( CHAR_DATA *ch, char **socialstr, char *argument )
{
    char buf[ MAX_INPUT_LENGTH];
    char oldstr[MAX_INPUT_LENGTH];
    char newstr[MAX_INPUT_LENGTH];
    char *p;
    char *s;
    char *d;

    argument = first_arg( argument, oldstr, FALSE );
    argument = first_arg( argument, newstr, FALSE );
    if ( oldstr[0] == '\0' )
    {
	send_to_char( "SEdit:  Replace what?\n\r", ch );
	return FALSE;
    }
    if ( newstr[0] == '\0' )
    {
	send_to_char( "SEdit:  Replace what with what?\n\r", ch );
	return FALSE;
    }
    if ( strlen( *socialstr ) + strlen( newstr ) - strlen( oldstr ) > MAX_INPUT_LENGTH - 2 )
    {
	send_to_char( "SEdit:  String too long.\n\r", ch );
	return FALSE;
    }

    if ( ( p = strstr( *socialstr, oldstr ) ) == NULL )
    {
	send_to_char( "SEdit:  String not found.\n\r", ch );
	return FALSE;
    }

    s = *socialstr;
    d = buf;
    while ( s < p )
    {
	*d++ = *s++;
    }

    d = stpcpy( d, newstr );
    strcpy( d, p + strlen( oldstr ) );

    free_string( *socialstr );
    *socialstr = str_dup( buf );
    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}

/* Updates a field in a social.  Checks for bad act() codes */
/*  if flag is TRUE, allow $E/$M/$N/$S codes */
bool social_update( CHAR_DATA *ch, char **socialstr, char *argument, bool flag )
{
    bool	 fGood = TRUE;
    char	 buf[MAX_INPUT_LENGTH];
    char	*p;

    if ( *argument == '.' && LOWER(*(argument+1)) == 'r' )
    {
	return social_replace( ch, socialstr, argument+2 );
    }

    if ( !str_cmp( argument, "clear" ) || !str_cmp( argument, "none" ) )
	*argument = '\0';

    p = buf;
    while ( *argument != '\0' )
    {
	if ( *argument != '$' )
	{
	    *p++ = *argument++;
	    continue;
	}
	*p++ = *argument++;
	switch( *argument )
	{
	    case 'e' :
	    case 'm' :
	    case 'n' :
	    case 's' :  break;
	    case 'E' :
	    case 'M' :
	    case 'N' :
	    case 'S' :  fGood = flag;  break;
	    default  :  fGood = FALSE; break;
	}
	if ( !fGood )
	{
	    if ( *argument == '\0' )
		strcpy ( buf, "SEdit:  Bad $ code at end of line.\n\r" );
	    else
		sprintf( buf, "SEdit:  Bad $ code: $%c.\n\r", *argument );
	    send_to_char( buf, ch );
	    return FALSE;
	}
	*p++ = *argument++;
    }
    *p = '\0';

    free_string( *socialstr );
    *socialstr = str_dup( buf );
    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}

