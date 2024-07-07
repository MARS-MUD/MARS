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

/*
 * Local functions
 */
static	MPROG_DATA *	get_mprog_data	args( ( MOB_INDEX_DATA *pMob, int vnum ) );

/************************************************************************/
/*			MPEdit section					*/
/************************************************************************/

bool
medit_mpedit( CHAR_DATA *ch, char *argument )
{
    MPROG_DATA *	pMProg;
    MOB_INDEX_DATA *	pMob;
    char		command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );

    if ( is_number( command ) )
    {
	if ( !( pMProg = get_mprog_data( (MOB_INDEX_DATA *)ch->desc->pEdit,
				    atoi( command ) ) ) )
	{
	    send_to_char( "MPEdit:  Mobile has no such MobProg.\n\r", ch );
	    return FALSE;
	}
	ch->desc->editin = ch->desc->editor;
	ch->desc->editor = ED_MPROG;
	ch->desc->inEdit = ch->desc->pEdit;
	ch->desc->pEdit  = (void *)pMProg;
	mpedit_show( ch, "" );
	return FALSE;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	if ( mpedit_create( ch, argument ) )
	{
	    ch->desc->editin = ch->desc->editor;
	    ch->desc->editor = ED_MPROG;

	    mpedit_show( ch, "" );
	    return TRUE;
	}
	return FALSE;
    }

    if ( command[0] == '\0'
    &&	 ch->desc->editor == ED_MOBILE
    &&	 ( pMob = (MOB_INDEX_DATA *)ch->desc->pEdit ) != NULL
    &&	 pMob->mobprogs != NULL
    &&	 pMob->mobprogs->next == NULL )
    {
	return medit_mpedit( ch, "0" );
    }

    send_to_char( "MPEdit:  There is no default MobProg to edit.\n\r", ch );
    return FALSE;
}


bool medit_mplist( CHAR_DATA *ch, char *argument )
{
    MPROG_DATA *	pMProg;
    MOB_INDEX_DATA *	pMob;
    int			value = 0;

    EDIT_MOB( ch, pMob );

    if ( !pMob->mobprogs )
    {
	send_to_char( "This mobile has no MobProgs.\n\r", ch );
	return FALSE;
    }

    for ( pMProg = pMob->mobprogs; pMProg; pMProg = pMProg->next, ++value )
    {
	ch_printf( ch, "[%2d] (%14s)  %s\n\r", value,
	    mprog_name( pMProg->type ), pMProg->arglist );
    }

    return FALSE;
}


bool medit_mpremove( CHAR_DATA *ch, char *argument )
{
    MPROG_DATA *	pMProg;
    MPROG_DATA *	pMPrev;
    MOB_INDEX_DATA *	pMob;
    int			value = 0;
    int			vnum;

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  mpremove #\n\r", ch );
	return FALSE;
    }

    vnum = atoi( argument );

    EDIT_MOB( ch, pMob );

    if ( !pMob->mobprogs )
    {
	send_to_char( "No such MobProg.\n\r", ch );
	return FALSE;
    }

    pMPrev = NULL;
    for ( pMProg = pMob->mobprogs; value < vnum; value++ )
    {
	pMPrev = pMProg;
	pMProg = pMProg->next;
	if ( pMProg == NULL )
	{
	    send_to_char( "No such MobProg.\n\r", ch );
	    return FALSE;
	}
    }

    if ( pMPrev == NULL )
	pMob->mobprogs = pMob->mobprogs->next;
    else
	pMPrev->next = pMProg->next;

    free_mprog_data( pMProg );

    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


bool medit_mpswap( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    MPROG_DATA *	pMProg1;
    MPROG_DATA *	pMProg2;
    MPROG_DATA *	pPrev1;
    MPROG_DATA *	pPrev2;
    MPROG_DATA *	pTemp;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    int			value;
    int			vnum1;
    int			vnum2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !is_number( arg1 ) || !is_number( arg2 ) )
    {
	send_to_char( "MEdit:  Syntax: mpswap #m #n\n\r", ch );
	return FALSE;
    }

    EDIT_MOB( ch, pMob );

    if ( !pMob->mobprogs )
    {
	send_to_char( "MEdit:  Mobile has no MobProgs.\n\r", ch );
	return FALSE;
    }
    if ( !pMob->mobprogs->next )
    {
	send_to_char( "MEdit:  Mobile only has one MobProg.\n\r", ch );
	return FALSE;
    }

    vnum1 = atoi( arg1 );
    vnum2 = atoi( arg2 );
    if ( vnum1 < 0 )
    {
	ch_printf( ch, "MEdit:  Bad argument: %s\n\r", arg1 );
	return FALSE;
    }
    if ( vnum2 < 0 )
    {
	ch_printf( ch, "MEdit:  Bad argument: %s\n\r", arg2 );
	return FALSE;
    }

    if ( vnum1 == vnum2 )
    {
	send_to_char( "MEdit:  Nothing to do!\n\r", ch );
	return FALSE;
    }

    value = 0;
    pPrev1 = NULL;
    for ( pMProg1 = pMob->mobprogs; value < vnum1; value++ )
    {
	pPrev1 = pMProg1;
	pMProg1 = pMProg1->next;
	if ( pMProg1 == NULL )
	{
	    send_to_char( "MEdit:  Mobile has no such MobProg.\n\r", ch );
	    return FALSE;
	}
    }

    value = 0;
    pPrev2 = NULL;
    for ( pMProg2 = pMob->mobprogs; value < vnum2; value++ )
    {
	pPrev2 = pMProg2;
	pMProg2 = pMProg2->next;
	if ( pMProg2 == NULL )
	{
	    send_to_char( "MEdit:  Mobile has no such MobProg.\n\r", ch );
	    return FALSE;
	}
    }

    if ( pMProg1->next == pMProg2 )
    {
	pMProg1->next = pMProg2->next;
	pMProg2->next = pMProg1;
	if ( pPrev1 == NULL )
	{
	    pMob->mobprogs = pMProg2;
	}
	else
	{
	    pPrev1->next = pMProg2;
	}
    }
    else if ( pMProg2->next == pMProg1 )
    {
	pMProg2->next = pMProg1->next;
	pMProg1->next = pMProg2;
	if ( pPrev2 == NULL )
	{
	    pMob->mobprogs = pMProg1;
	}
	else
	{
	    pPrev2->next = pMProg1;
	}
    }
    else if ( pPrev1 == NULL )
    {
	pTemp = pMProg1->next;
	pMProg1->next = pMProg2->next;
	pMProg2->next = pTemp;
	pPrev2->next = pMProg1;
	pMob->mobprogs = pMProg2;
    }
    else if ( pPrev2 == NULL )
    {
	pTemp = pMProg1->next;
	pMProg1->next = pMProg2->next;
	pMProg2->next = pTemp;
	pPrev1->next = pMProg2;
	pMob->mobprogs = pMProg1;
    }
    else
    {
	pTemp = pMProg1->next;
	pMProg1->next = pMProg2->next;
	pMProg2->next = pTemp;
	pTemp = pPrev1->next;
	pPrev1->next = pPrev2->next;
	pPrev2->next = pTemp;
    }

    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


void mpedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    MOB_INDEX_DATA *	pMob;
    MPROG_DATA *	pMProg;
    MPROG_DATA *	prog;
    char		command[MAX_INPUT_LENGTH];
    char		arg[MAX_STRING_LENGTH];
    int			cmd;
    int			value;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_MPROG( ch, pMProg );
    pMob = (MOB_INDEX_DATA *)ch->desc->inEdit;
    pArea = pMob->area;

    if ( command[0] == '\0' )
    {
	mpedit_show( ch, "" );
	return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }  

    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *mpedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, mpedit_table[cmd].name ) )
	{
	    if ( (*mpedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    if ( ( value = flag_value( mprog_types, arg ) ) != NO_FLAG )
    {
	pMProg->type = value;
	pMob = (MOB_INDEX_DATA *)ch->desc->inEdit;
	/* need to re-set progtypes here */
	pMob->progtypes = 0;
	for ( prog = pMob->mobprogs; prog; prog = prog->next )
	    SET_BIT( pMob->progtypes, prog->type );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "MobProg type set.\n\r", ch );
	return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool mpedit_arglist( CHAR_DATA *ch, char *argument )
{
    MPROG_DATA *pMProg;
    int prc = 0;

    EDIT_MPROG( ch, pMProg );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  arglist [string]\n\r", ch );
	return FALSE;
    }

    prc = ( is_number( argument ) ? atoi( argument ) : 0 );
    if ( pMProg->type == MP_RAND_PROG && prc > 95 )
    {
	send_to_char( "You can't set the percentage that high on a rand_prog.\n\r", ch );
	return FALSE;
    }
    free_string( pMProg->arglist );
    pMProg->arglist = str_dup( argument );

    send_to_char( "Arglist set.\n\r", ch );
    return TRUE;
}


bool mpedit_comlist( CHAR_DATA *ch, char *argument )
{
    MPROG_DATA *pMProg;

    EDIT_MPROG( ch, pMProg );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pMProg->comlist );
	return TRUE;
    }

    send_to_char( "Syntax:  comlist    - line edit\n\r", ch );
    return FALSE;
}


bool mpedit_create( CHAR_DATA *ch, char *argument )
{
    MPROG_DATA *	pMProg;
    MPROG_DATA *	pMLast;
    MOB_INDEX_DATA *	pMob;

    EDIT_MOB( ch, pMob );

    pMProg = new_mprog_data( );

    if ( !pMob->mobprogs )
	pMob->mobprogs = pMProg;
    else
    {
	for ( pMLast = pMob->mobprogs; pMLast->next; pMLast = pMLast->next )
	    ;	/* find end of list */
	pMLast->next = pMProg;
    }

    SET_BIT( pMob->progtypes, MP_ACT_PROG );

    ch->desc->inEdit = (void *)ch->desc->pEdit;
    ch->desc->pEdit  = (void *)pMProg;

    send_to_char( "MobProg created.\n\r", ch );
    return TRUE;
}


bool mpedit_show( CHAR_DATA *ch, char *argument )
{
    MPROG_DATA *	pMProg;
    MOB_INDEX_DATA *	pMob;

    EDIT_MPROG( ch, pMProg );
    pMob = (MOB_INDEX_DATA *)ch->desc->inEdit;

    ch_printf( ch, "Mobile: [%5d] %s\n\r", pMob->vnum, pMob->player_name );
    ch_printf( ch, "MobProg type: %s\n\r", mprog_name( pMProg->type ) );
    ch_printf( ch, "Arguments: %s\n\r", pMProg->arglist );
    ch_printf( ch, "Commands:\n\r%s", pMProg->comlist );
    return FALSE;
}


static MPROG_DATA *get_mprog_data( MOB_INDEX_DATA *pMob, int vnum )
{
    int value = 0;
    MPROG_DATA *pMProg;

    for ( pMProg = pMob->mobprogs; pMProg; pMProg = pMProg->next, ++value )
    {
	if ( value == vnum )
	    return pMProg;
    }
    return NULL;
}

