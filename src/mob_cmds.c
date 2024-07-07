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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "recycle.h"
#include "tables.h"

#define DECLARE_MOB_FUN( fun )		MOB_FUN   fun

typedef	int MOB_FUN			args( ( CHAR_DATA *ch,
					       char *argument ) );

struct mob_cmd_type
{
    char * const	name;
    MOB_FUN *		mob_fun;
    int			flags;
};

DECLARE_MOB_FUN( mob_acquaint	);
DECLARE_MOB_FUN( mob_aecho	);
DECLARE_MOB_FUN( mob_asound	);
DECLARE_MOB_FUN( mob_at		);
DECLARE_MOB_FUN( mob_cancel	);
DECLARE_MOB_FUN( mob_close	);
DECLARE_MOB_FUN( mob_commands	);
DECLARE_MOB_FUN( mob_damage	);
DECLARE_MOB_FUN( mob_delay	);
DECLARE_MOB_FUN( mob_echo	);
DECLARE_MOB_FUN( mob_echoaround	);
DECLARE_MOB_FUN( mob_echoat	);
DECLARE_MOB_FUN( mob_force	);
DECLARE_MOB_FUN( mob_forget	);
DECLARE_MOB_FUN( mob_give	);
DECLARE_MOB_FUN( mob_goto	);
DECLARE_MOB_FUN( mob_gtransfer	);
DECLARE_MOB_FUN( mob_junk	);
DECLARE_MOB_FUN( mob_kill	);
DECLARE_MOB_FUN( mob_lock	);
DECLARE_MOB_FUN( mob_mload	);
DECLARE_MOB_FUN( mob_oload	);
DECLARE_MOB_FUN( mob_open	);
DECLARE_MOB_FUN( mob_peace	);
DECLARE_MOB_FUN( mob_purge	);
DECLARE_MOB_FUN( mob_remember	);
DECLARE_MOB_FUN( mob_secho	);
DECLARE_MOB_FUN( mob_sechoaround);
DECLARE_MOB_FUN( mob_sechoat	);
DECLARE_MOB_FUN( mob_secret	);
DECLARE_MOB_FUN( mob_setpos	);
DECLARE_MOB_FUN( mob_take	);
DECLARE_MOB_FUN( mob_teleport	);
DECLARE_MOB_FUN( mob_transfer	);
DECLARE_MOB_FUN( mob_unhide	);
DECLARE_MOB_FUN( mob_unlock	);

/*
 *  The mob command table
 */
static const struct mob_cmd_type mob_cmd_table[] =
{
    {	"acquaint",	mob_acquaint,	0	},
    {	"aecho",	mob_aecho,	0	},
    {	"asound",	mob_asound,	0	},
    {	"at",		mob_at,		0	},
    {	"cancel",	mob_cancel,	0	},
    {	"close",	mob_close,	0	},
    {	"commands",	mob_commands,	0,	},
    {	"damage",	mob_damage,	0	},
    {	"delay",	mob_delay,	0	},
    {	"echo",		mob_echo,	0	},
    {	"echoaround",	mob_echoaround,	0	},
    {	"echoat",	mob_echoat,	0	},
    {	"force",	mob_force,	0,	},
    {	"forget",	mob_forget,	0	},
    {	"give",		mob_give,	0,	},
    {	"goto",		mob_goto,	0	},
    {	"gtransfer",	mob_gtransfer,	0	},
    {	"junk",		mob_junk,	0	},
    {	"kill",		mob_kill,	0	},
    {	"lock",		mob_lock,	0,	},
    {	"mload",	mob_mload,	0	},
    {	"oload",	mob_oload,	0	},
    {	"open",		mob_open,	0	},
    {	"peace",	mob_peace,	0	},
    {	"purge",	mob_purge,	0	},
    {	"remember",	mob_remember,	0	},
    {	"secho",	mob_secho,	0	},
    {	"sechoaround",	mob_sechoaround,0	},
    {	"sechoat",	mob_sechoat,	0	},
    {	"secret",	mob_secret,	0	},
    {	"setpos",	mob_setpos,	0	},
    {	"take",		mob_take,	0	},
    {	"teleport",	mob_teleport,	0	},
    {	"transfer",	mob_transfer,	0	},
    {	"unhide",	mob_unhide,	0	},
    {	"unlock",	mob_unlock,	0	},
    {	NULL,		NULL,		0	}
};


void
do_mob( CHAR_DATA *ch, char *argument )
{
    int		cmd;
    char	command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );

    if ( !IS_NPC( ch )
	 && IS_IMMORTAL( ch )	/* need to change this to same level as mpcommands */
	 && command[0] != '\0'
	 && !str_prefix( command, "commands" ) )
    {
	mob_commands( ch, argument );
	return;
    }

    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( command[0] == '\0' )
    {
	progbug( ch, "MOB: no command" );
	return;
    }

    for ( cmd = 0; mob_cmd_table[cmd].name; cmd++ )
    {
	if ( !str_cmp( command, mob_cmd_table[cmd].name ) )
	{
	    (*mob_cmd_table[cmd].mob_fun) ( ch, argument );
	    return;
	}
    }

    progbug( ch, "MOB: bad mob command \"%s\"", command );
    return;
}


void
do_mpcommands( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    int		cmd;
    int		col;

    pBuf = new_buf( );
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( IS_SET( cmd_table[cmd].flags, CMD_MOB ) )
	{
	    buf_printf( pBuf, "%-16s", cmd_table[cmd].name );
	    if ( ++col % 5 == 0 )
		add_buf( pBuf, "\n\r" );
	}
    }

    if ( col % 5 != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_mplist( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    MPROG_DATA *mprg;
    MOB_INDEX_DATA *	pMob;
    CHAR_DATA  *	victim;
    int			vnum;
    int			count;
    bool		fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "MobProg list whom?\n\r", ch );
	return;
    }


    if ( is_number( arg1 ) )
	vnum = atoi( arg1 );
    else
    {
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Only Mobiles can have Programs!\n\r", ch );
	    return;
	}
	vnum = victim->pIndexData->vnum;
    }

    if ( ( pMob = get_mob_index( vnum ) ) == NULL )
    {
	send_to_char( "That mobile does not exist.\n\r", ch );
	return;
    }

    if ( pMob->progtypes == 0 )
    {
	send_to_char( "That Mobile has no Programs set.\n\r", ch );
	return;
    }

    if ( is_number( arg2 ) )
    {
	vnum = atoi( arg2 );
	fAll = FALSE;
    }
    else if ( !str_cmp( arg2, "all" ) )
    {
	fAll = TRUE;
    }
    else if ( arg2[0] == '\0' )
    {
	vnum = 0;
	fAll = FALSE;
    }
    else
    {
	send_to_char( "Syntax: mplist <mobile> <number>\n\r", ch );
	send_to_char( "        mplist <mobile> all\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    count = 0;

    buf_printf( pBuf, "Name: %s.  Vnum: %d.\n\r",
	pMob->player_name, pMob->vnum );

    buf_printf( pBuf, "Short description: %s`X.\n\rLong  description: %s`X",
	    pMob->short_descr,
	    pMob->long_descr[0] != '\0' ?
	    pMob->long_descr : "(none).\n\r" );

    for ( mprg = pMob->mobprogs; mprg != NULL;
	  mprg = mprg->next )
    {
	if ( fAll || count == vnum )
	{
	    buf_printf( pBuf, ">%s %s\n\r%s\n\r",
			mprog_name( mprg->type ),
			mprg->arglist,
			mprg->comlist );
	    if ( !fAll )
		break;
	}
	count++;
    }

    if ( !fAll && !mprg )
	ch_printf( ch, "Program %d not found on %s.\n\r", vnum, pMob->short_descr );
    else
	page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


/*
 * A trivial rehack of do_mstat.  This doesnt show all the data, but just
 * enough to identify the mob and give its basic condition.  It does however,
 * show the MOBprograms which are set.
 */
void
do_mpstat( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;
    char		arg[MAX_INPUT_LENGTH];
    MPROG_DATA *	mprg;
    MOB_INDEX_DATA *	pMob;
    CHAR_DATA *		victim;
    int			vnum;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( ch->desc != NULL
	&&   ch->desc->editor == ED_MOBILE )
	{
	    sprintf( arg, "%d", ((MOB_INDEX_DATA *)ch->desc->pEdit)->vnum );
	}
	else
	{
	    send_to_char( "MobProg stat whom?\n\r", ch );
	    return;
	}
    }

    if ( is_number( arg ) )
	vnum = atoi( arg );
    else
    {
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Only Mobiles can have Programs!\n\r", ch );
	    return;
	}
	vnum = victim->pIndexData->vnum;
    }

    if ( ( pMob = get_mob_index( vnum ) ) == NULL )
    {
	send_to_char( "That mobile does not exist.\n\r", ch );
	return;
    }

    if ( pMob->progtypes == 0 )
    {
	send_to_char( "That Mobile has no Programs set.\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    vnum = 0;

    buf_printf( pBuf, "Name: %s.  Vnum: %d.\n\r",
	pMob->player_name, pMob->vnum );

    buf_printf( pBuf, "Short description: %s`X.\n\rLong  description: %s`X",
	    pMob->short_descr,
	    pMob->long_descr[0] != '\0' ?
	    pMob->long_descr : "(none).\n\r" );

    for ( mprg = pMob->mobprogs; mprg != NULL;
	  mprg = mprg->next )
    {
	buf_printf( pBuf, "[%d] %s %s\n\r",
		    vnum++,
		    mprog_name( mprg->type ),
		    mprg->arglist );
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


int
mob_acquaint( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    CHAR_DATA *	target;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( vch = get_char_world( ch, arg1 ) ) == NULL )
    {
        progbug( ch, "Mpacquaint: no target mob '%s'", arg1 );
        return 1;
    }

    if ( !IS_NPC( vch ) )
    {
        progbug( ch, "Mpacquaint: '%s' not NPC", vch->name );
        return 1;
    }

    if ( arg2[0] == '\0' )
        target = ch->qmem;
    else
    {
        if ( ( target = get_char_world( ch, arg2 ) ) == NULL )
        {
            progbug( ch, "Mpacquaint: no such target: '%s'", arg2 );
            return 1;
        }
    }

    mob_forget( vch, "" );
    vch->qmem = target;

    return 0;
}


int
mob_aecho( CHAR_DATA *ch, char *argument )
{
    do_mpaecho( ch, argument );
    return 0;
}


int
mob_asound( CHAR_DATA *ch, char *argument )
{
    do_mpasound( ch, argument );
    return 0;
}


int
mob_at( CHAR_DATA *ch, char *argument )
{
    do_mpat( ch, argument );
    return 0;
}


int
mob_cancel( CHAR_DATA *ch, char *argument )
{
    do_mpcancel( ch, argument );
    return 0;
}


/* Lets the mobile silently close a door */
int
mob_close( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			door;
    EXIT_DATA *		pExit;
    EXIT_DATA *		pRevExit;
    ROOM_INDEX_DATA *	to_room;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        progbug( ch, "Mpclose - no door given" );
        return 1;
    }

    if ( ( door = find_door( ch, arg, TRUE ) ) == DIR_NONE )
    {
        progbug( ch, "Mpclose - room #%d - no door '%s'",
                 ch->in_room == NULL ? 0 : ch->in_room->vnum,
                 arg );
        return 1;
    }

    pExit = ch->in_room->exit[door];
    SET_BIT( pExit->exit_info, EX_CLOSED );

    /* close the other side */
    if ( ( to_room   = pExit->to_room            ) != NULL
    &&   ( pRevExit = to_room->exit[rev_dir[door]] ) != NULL
    &&   pRevExit->to_room == ch->in_room )
    {
	SET_BIT( pRevExit->exit_info, EX_CLOSED );
    }

    return 0;
}


int
mob_damage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char *	p;
    int		amount;


    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg2[0] == '\0' )
    {
        progbug( ch, "Mpdamage - Bad syntax" );
        return 1;
    }

    if ( ( vch = get_char_room( ch, arg1 ) ) == NULL )
    {
        progbug( ch, "Mpdamage - Victim not in room: %s", arg1 );
        return 1;
    }

    if ( vch == ch )
    {
        progbug( ch, "Mpdamage - Bad victim: %s", arg1 );
        return 1;
    }

    p = arg2;
    if ( !isdigit( *p ) )
    {
        progbug( ch, "Mpdamage - \"%s\" not number", arg2 );
        return 1;
    }

    while ( isdigit( *p ) )
        p++;
    if ( *p != '%' && *p != '\0' )
    {
        progbug( ch, "Mpdamage - bad argument: %s", arg2 );
        return 1;
    }

    if ( ( amount = atoi( arg2 ) ) <= 0 )
    {
        progbug( ch, "Mpdamage - bad argument: %s", arg2 );
        return 1;
    }

    if ( *p == '%' )
    {
        if ( amount > 50 )
        {
            progbug( ch, "Mpdamage - %s too much damage", arg2 );
            return 1;
        }
        amount = ( vch->max_hit * amount ) / 100;
    }

    amount = UMIN( amount, vch->max_hit / 2 );
    amount = UMIN( amount, vch->hit + 1 );
    if ( amount < 1 )
        return 0;

    vch->hit -= amount;
    return 0;
}


int
mob_delay( CHAR_DATA *ch, char *argument )
{
    do_mpdelay( ch, argument );
    return 0;
}


int
mob_echo( CHAR_DATA *ch, char *argument )
{
    do_mpecho( ch, argument );
    return 0;
}


int
mob_echoaround( CHAR_DATA *ch, char *argument )
{
    do_mpechoaround( ch, argument );
    return 0;
}


int
mob_echoat( CHAR_DATA *ch, char *argument )
{
    do_mpechoat( ch, argument );
    return 0;
}


int
mob_force( CHAR_DATA *ch, char *argument )
{
    do_mpforce( ch, argument );
    return 0;
}


int
mob_forget( CHAR_DATA *ch, char *argument )
{
    ch->qmem = NULL;

    /* need to put something in for oprogs, rprogs and maybe even eprogs */

    return 0;
}


int
mob_give( CHAR_DATA *ch, char *argument )
{
    do_mpgive( ch, argument );
    return 0;
}


int
mob_goto( CHAR_DATA *ch, char *argument )
{
    do_mpgoto( ch, argument );
    return 0;
}


int
mob_gtransfer( CHAR_DATA *ch, char *argument )
{
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *	location;
    CHAR_DATA *		victim;
    CHAR_DATA *		vch;
    CHAR_DATA *		v_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	progbug( ch, "Mob gtransfer - Bad syntax" );
	return 1;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    progbug( ch, "Mob gtransfer - No such location: %s", arg2 );
	    return 2;
	}

	if ( room_is_private( location ) )
	{
	    progbug( ch, "Mob gtransfer - Private room: vnum %d", location->vnum );
	    return 2;
	}
    }

    if ( location->sector_type == SECT_BOILING )
    {
	progbug( ch, "Mpgtransger - boiling ocean: %s", arg2 );
	return 2;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	progbug( ch, "Mob gtransfer - No such person: %s", arg1 );
	return 2;
    }

    if ( victim->in_room == NULL )
    {
	progbug( ch, "Mob gtransfer - Victim in Limbo" );
	return 2;
    }

    if ( victim->in_room == location )
	return 0;

    for ( vch = victim->in_room->people; vch != NULL; vch = v_next )
    {
        v_next = vch->next_in_room;
        if ( is_same_group( victim, vch ) )
        {
            if ( vch->fighting != NULL )
                stop_fighting( vch, TRUE );
            char_from_room( vch );
            char_to_room( vch, location );
        }
    }

    return 0;
}


int
mob_junk( CHAR_DATA *ch, char *argument )
{
    do_mpjunk( ch, argument );
    return 0;
}


int
mob_kill( CHAR_DATA *ch, char *argument )
{
    do_mpkill( ch, argument );
    return 0;
}


int
mob_lock( CHAR_DATA *ch, char *argument )
{
    do_mplock( ch, argument );
    return 0;
}


int
mob_mload( CHAR_DATA *ch, char *argument )
{
    do_mpmload( ch, argument );
    return 0;
}


int
mob_oload( CHAR_DATA *ch, char *argument )
{
    do_mpoload( ch, argument );
    return 0;
}


/* Lets the mobile silently open a door */
int
mob_open( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			door;
    EXIT_DATA *		pExit;
    EXIT_DATA *		pRevExit;
    ROOM_INDEX_DATA *	to_room;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        progbug( ch, "Mpopen - no door given" );
        return 1;
    }

    if ( ( door = find_door( ch, arg, TRUE ) ) == DIR_NONE )
    {
        progbug( ch, "Mpopen - room #%d - no door '%s'",
                 ch->in_room == NULL ? 0 : ch->in_room->vnum,
                 arg );
        return 1;
    }

    pExit = ch->in_room->exit[door];
    REMOVE_BIT( pExit->exit_info, EX_LOCKED );
    REMOVE_BIT( pExit->exit_info, EX_CLOSED );

    /* close the other side */
    if ( ( to_room   = pExit->to_room            ) != NULL
    &&   ( pRevExit = to_room->exit[rev_dir[door]] ) != NULL
    &&   pRevExit->to_room == ch->in_room )
    {
	REMOVE_BIT( pRevExit->exit_info, EX_LOCKED );
	REMOVE_BIT( pRevExit->exit_info, EX_CLOSED );
    }

    return 0;
}


int
mob_peace( CHAR_DATA *ch, char *argument )
{
    do_mppeace( ch, argument );
    return 0;
}


int
mob_purge( CHAR_DATA *ch, char *argument )
{
    do_mppurge( ch, argument );
    return 0;
}


int
mob_remember( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    CHAR_DATA *	vch;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        progbug( ch, "Mpremember: missing argument." );
        return 1;
    }

    if ( ( vch = get_char_world( ch, arg ) ) == NULL )
        return 1;

    ch->qmem = vch;

    /* need to put something here for objs, rooms, exit progs */

    return 0;
}


int
mob_secho( CHAR_DATA *ch, char *argument )
{
    do_mpsecho( ch, argument );
    return 0;
}


int
mob_sechoaround( CHAR_DATA *ch, char *argument )
{
    do_mpsechoaround( ch, argument );
    return 0;
}


int
mob_sechoat( CHAR_DATA *ch, char *argument )
{
    do_mpsechoat( ch, argument );
    return 0;
}


int
mob_secret( CHAR_DATA *ch, char *argument )
{
    int		door;
    EXIT_DATA *	pExit;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        progbug( ch, "Mpsecret: no arguments" );
        return 1;
    }

    if ( ( door = door_lookup ( arg1 ) ) == DIR_NONE )
    {
        progbug( ch, "Mpsecret: no door \"%s\" in room #%d", arg1,
                 ch->in_room == NULL ? 0 : ch->in_room->vnum );
        return 1;
    }

    if ( ( pExit = get_exit( ch->in_room, door ) ) == NULL )
    {
        progbug( ch, "Mpsecret: no door %s in room #%d", arg1,
                 ch->in_room == NULL ? 0 : ch->in_room->vnum );
        return 1;
    }

    if ( arg2[0] == '\0' )
    {
        progbug( ch, "Mpsecret: missing second argument" );
        return 1;
    }

    if ( !IS_SET( pExit->exit_info, EX_ISDOOR ) )
    {
        progbug( ch, "Mpsecret: door %s in room %d not flagged \"door\"",
                 arg1, ch->in_room == NULL ? 0 : ch->in_room->vnum );
        return 1;
    }

    if ( !str_cmp( arg2, "set" ) || !str_cmp( arg2, "on" ) )
    {
        SET_BIT( pExit->exit_info, EX_SECRET );
        SET_BIT( pExit->exit_info, EX_CLOSED );
        if ( pExit->key != 0 )
            SET_BIT( pExit->exit_info, EX_LOCKED );
    }
    else if ( !str_cmp( arg2, "off" ) || !str_cmp( arg2, "unset" )
         ||   !str_cmp( arg2, "reset" ) || !str_cmp( arg2, "clear" ) )
        REMOVE_BIT( pExit->exit_info, EX_SECRET );
    else
    {
        progbug( ch, "Mpsecret: unknown setting \"%s\"", arg2 );
        return 1;
    }

    return 0;
}


int
mob_setpos( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	vch;
    OBJ_DATA *	obj;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    int		pos;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( vch = get_char_room( ch, arg1 ) ) == NULL )
    {
        progbug( ch, "Mpsetpos: Victim not in room: %s", arg1 );
        return 1;
    }

    if ( ( pos = position_lookup( arg2 ) ) == NO_POSITION )
    {
        progbug( ch, "Mpsetpos: bad position argument \"%s\"", arg2 );
        return 1;
    }

    switch ( pos )
    {
        case POS_DEAD:
        case POS_MORTAL:
        case POS_INCAP:
        case POS_STUNNED:
        case POS_FIGHTING:
            progbug( ch, "Mpsetpos: Illegal position: %s", position_table[pos].name );
            return 1;
        case POS_SLEEPING:
        case POS_RESTING:
        case POS_SITTING:
        case POS_STANDING:
            break;
        default:
            progbug( ch, "Mpsetpos: program logic error %d, blame Mac", pos );
            return 1;
    }

    if ( pos == vch->position )
        return 0;

    switch ( vch->position )
    {
        case POS_DEAD:
        case POS_MORTAL:
        case POS_INCAP:
        case POS_STUNNED:
        case POS_FIGHTING:
            return 2;
        case POS_STANDING:
            if ( MOUNTED( vch ) != NULL || RIDDEN( ch ) != NULL )
                return 2;
            break;
        case POS_SLEEPING:
            if ( IS_AFFECTED( ch, AFF_SLEEP ) )
                return 2;
            break;
        case POS_RESTING:
        case POS_SITTING:
            break;
        default:
            progbug( ch, "Mpsetpos: program logic error %d, blame Mac", pos );
            return 1;
    }

    /* Okay, the only positions we need worry about now are */
    /* sleeping, resting, sitting and standing.		    */

    if ( ( obj = vch->on ) != NULL )
    {
        if ( ( pos == POS_SLEEPING && !IS_SET( obj->value[2], SLEEP_AT )
             && !IS_SET( obj->value[2], SLEEP_ON ) && !IS_SET( obj->value[2], SLEEP_IN ) )
        ||   ( pos == POS_RESTING && !IS_SET( obj->value[2], REST_AT )
             && !IS_SET( obj->value[2], REST_ON ) && !IS_SET( obj->value[2], REST_IN ) )
        ||   ( pos == POS_SITTING && !IS_SET( obj->value[2], SIT_AT )
             && !IS_SET( obj->value[2], SIT_ON ) && !IS_SET( obj->value[2], SIT_IN ) )
        ||   ( pos == POS_STANDING && !IS_SET( obj->value[2], STAND_AT )
             && !IS_SET( obj->value[2], STAND_ON ) && !IS_SET( obj->value[2], STAND_IN ) ) )
            ch->on = NULL;
    }

    vch->position = pos;

    return 0;
}


int
mob_take( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *	obj;
    CHAR_DATA *	vch;
    int		vnum;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	progbug( ch, "Mptake - bad or missing object" );
	return 1;
    }

    if ( arg2[0] == '\0' )
    {
        /* take object from room */
        if ( is_number( arg1 ) )
        {
            vnum = atoi( arg1 );
            for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
            {
                if ( obj->pIndexData->vnum == vnum )
                    break;
            }
        }
        else
        {
            obj = get_obj_list( ch, arg1, ch->in_room->contents );
        }
        if ( obj == NULL )
        {
            progbug( ch, "Mptake - object \"%s\" not in room %d", arg1, ch->in_room->vnum );
            return 1;
        }

        obj_from_room( obj );
        obj_to_char( obj, ch );
    }
    else
    {
        if ( ( vch = get_char_room( ch, arg2 ) ) == NULL )
        {
            progbug( ch, "Mptake - no such victim \"%s\"", arg2 );
            return 1;
        }

        if ( is_number( arg1 ) )
        {
            vnum = atoi( arg1 );
            for ( obj = vch->carrying; obj != NULL; obj = obj->next_content )
            {
                if ( obj->pIndexData->vnum == vnum )
                    break;
            }
        }
        else
        {
            if ( ( obj = get_obj_carry( vch, arg1, ch ) ) == NULL )
                obj = get_obj_wear( vch, arg1 );
        }

        if ( obj == NULL )
        {
            progbug( ch, "Mptake - object \"%s\" not carried by %s", arg1, arg2 );
            return 1;
        }

        obj_from_char( obj );
        obj_to_char( obj, ch );

    }

    return 0;
}


int
mob_teleport( CHAR_DATA *ch, char *argument )
{
    do_mpteleport( ch, argument );
    return 0;
}


int
mob_transfer( CHAR_DATA *ch, char *argument )
{
    do_mptransfer( ch, argument );
    return 0;
}


int
mob_unhide( CHAR_DATA *ch, char *argument )
{
    do_mpunhide( ch, argument );
    return 0;
}


int
mob_unlock( CHAR_DATA *ch, char *argument )
{
    do_mpunlock( ch, argument );
    return 0;
}


void
do_mpacquaint( CHAR_DATA *ch, char *argument )
{
    mob_acquaint( ch, argument );
    return;
}


void
do_mpaecho( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	pRoom;
    int			lvnum;
    int			uvnum;
    int			vnum;

    if ( *argument == '\0' )
    {
	progbug( ch, "Mpaecho - No argument" );
	return;
    }

    lvnum = ch->in_room->area->min_vnum;
    uvnum = ch->in_room->area->max_vnum;
    for ( vnum = lvnum; vnum <= uvnum; vnum++ )
    {
	if ( ( pRoom = get_room_index( vnum ) ) != NULL && pRoom->people != NULL )
	{
	    act( "$t", pRoom->people, argument, NULL, TO_CHAR );
	    act( "$t", pRoom->people, argument, NULL, TO_ROOM );
	}
    }
}


/* prints the argument to all the rooms aroud the mobile */
void
do_mpasound( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	was_in_room;
    EXIT_DATA *		pexit;
    int			door;

    if ( argument[0] == '\0' )
    {
        progbug( ch, "Mpasound - No argument" );
	return;
    }

    was_in_room = ch->in_room;

    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( ( pexit = was_in_room->exit[door] ) != NULL
	    && pexit->to_room != NULL
	    && pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    MOBtrigger  = FALSE;
	    act_color( C_DEFAULT, argument, ch, NULL, NULL, TO_ROOM,
			POS_RESTING );
	}
    }

    ch->in_room = was_in_room;
    return;

}


int
mob_commands( CHAR_DATA *ch, char *argument )
{
    BUFFER *	pBuf;
    int iIndex;
    int col = 0;

    pBuf = new_buf( );
    for ( iIndex = 0; mob_cmd_table[iIndex].name; iIndex++ )
    {
	buf_printf( pBuf, "%-16s", mob_cmd_table[iIndex].name );
	col++;
	if ( col % 5 == 0 )
	    add_buf( pBuf, "\n\r" );
    }
    if ( ( col % 5 ) != 0 )
	add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return 0;
}


void
do_mpat( CHAR_DATA *ch, char *argument )
{
    char		arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *	location;
    ROOM_INDEX_DATA *	original;
    OBJ_DATA * 		tmp_obj;
    CHAR_DATA *		wch;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	progbug( ch, "Mpat - Bad argument" );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	progbug( ch, "Mpat - No such location: %s", arg );
	return;
    }

    tmp_obj = ch->on;
    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    ch->on = tmp_obj;
	    break;
	}
    }

    return;
}


void
do_mpcancel( CHAR_DATA *ch, char *argument )
{
    strip_event_char( ch, EVENT_CHAR_MPDELAY );

    if ( IS_NPC( ch ) && ch->pIndexData->vnum == MOB_VNUM_SUPERMOB )
    {
	switch ( ch->smob_type )
	{
	    case PTYPE_EXIT:
		strip_event_exit( ch->smob_ptr, EVENT_EXIT_MPDELAY );
		break;
	    case PTYPE_OBJ:
		strip_event_obj( ch->smob_ptr, EVENT_OBJ_MPDELAY );
		break;
	    case PTYPE_ROOM:
		strip_event_room( ch->smob_ptr, EVENT_ROOM_MPDELAY );
		break;
	}
    }

    return;
}


void
do_mpclose( CHAR_DATA *ch, char *argument )
{
    mob_close( ch, argument );
}


void
do_mpdamage( CHAR_DATA *ch, char *argument )
{
    mob_damage( ch, argument );
}


void
do_mpdelay( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			delay;
    EVENT_DATA *	event;
    EXIT_DATA *		pExit;
    OBJ_DATA *		pObj;
    ROOM_INDEX_DATA *	pRoom;

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	progbug( ch, "Mpdelay: no time value" );
	return;
    }

    if ( *argument == '\0' )
    {
	progbug( ch, "Mpdelay: no command" );
	return;
    }

    delay = atoi( arg );

    event           = new_event( );
    event->argument = str_dup( argument );

    if ( IS_NPC( ch ) && ch->pIndexData->vnum == MOB_VNUM_SUPERMOB )
    {
	if ( ch->smob_type == PTYPE_OBJ )
	{
	    event->type = EVENT_OBJ_MPDELAY;
	    pObj = (OBJ_DATA *)ch->smob_ptr;
	    event->fun = event_obj_mpdelay;
	    add_event_obj( event, pObj, delay * PULSE_PER_SECOND );
	    return;
	}
	else if ( ch->smob_type == PTYPE_ROOM )
	{
	    event->type = EVENT_ROOM_MPDELAY;
	    pRoom = (ROOM_INDEX_DATA *)ch->smob_ptr;
	    event->fun = event_room_mpdelay;
	    add_event_room( event, pRoom, delay * PULSE_PER_SECOND );
	}
	else if ( ch->smob_type == PTYPE_EXIT )
	{
	    event->type = EVENT_EXIT_MPDELAY;
	    pExit = (EXIT_DATA *)ch->smob_ptr;
	    event->fun = event_exit_mpdelay;
	    add_event_exit( event, pExit, delay * PULSE_PER_SECOND );
	}
	else
	{
	    free_event( event );
	    return;
	}
    }
    else
    {
	event->type     = EVENT_CHAR_MPDELAY;
	event->fun      = event_char_mpdelay;
	add_event_char( event, ch, delay * PULSE_PER_SECOND );
    }

    return;
}


void
do_mpecho( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	progbug( ch, "Mpecho - called w/o argument" );
	return;
    }

    act( "$t", ch, argument, NULL, TO_ROOM );
    return;

}


/* prints the message to everyone in the room other than the mob and victim */
void
do_mpechoaround( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	progbug( ch, "Mpechoaround - No argument" );
	return;
    }

    if ( !( victim=get_char_room( ch, arg ) ) )
    {
	progbug( ch, "Mpechoaround - victim does not exist: %s", arg );
	return;
    }

    act( "$t", ch, argument, victim, TO_NOTVICT );
    return;
}

/* prints the message to only the victim */

void
do_mpechoat( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	progbug( ch, "Mpechoat - No argument" );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	progbug( ch, "Mpechoat - victim does not exist: %s", arg );
	return;
    }

    act( "$t", ch, argument, victim, TO_VICT );
    return;
}


/* lets the mobile force someone to do something.  Must be mortal level
   and the all argument only affects those in the room with the mobile. */
void
do_mpforce( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char cmd[MAX_INPUT_LENGTH];
    int  c;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	progbug( ch, "Mpforce - Bad syntax" );
	return;
    }
    if ( !isalpha( argument[0] ) && !isdigit( argument[0] ) )
    {
	cmd[0] = argument[0];
	cmd[1] = '\0';
    }
    else
    {
	one_argument( argument, cmd );
    }

    for ( c = 0; !IS_NULLSTR( cmd_table[c].name ); c++ )
    {
	if ( cmd[0] == cmd_table[c].name[0] && !str_prefix( cmd, cmd_table[c].name ) )
	    break;
    }

    if ( cmd_table[c].level >= LEVEL_IMMORTAL )
    {
	progbug( ch, "Mpforce: forcing Immortal command %s", cmd_table[c].name );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( vch->in_room == ch->in_room
		&& vch != ch
/*		&& get_trust( vch ) < get_trust( ch ) */
		&& can_see( ch, vch ) )
	    {
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    progbug( ch, "Mpforce - No such victim %s", arg );
	    return;
	}

	if ( victim == ch )
    	{
	    progbug( ch, "Mpforce - Forcing oneself" );
	    return;
	}

	if ( victim->wait )
	    victim->wait = 0;
	interpret( victim, argument );
    }

    return;
}


void
do_mpforget( CHAR_DATA *ch, char *argument )
{
    mob_forget( ch, argument );
    return;
}


void
do_mpgive( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *	obj;
    CHAR_DATA *	vch;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	progbug( ch, "Mpgive - bad or missing argument(s)" );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	progbug( ch, "Mpgive - mob not carrying '%s'", arg1 );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	progbug( ch, "Mpgive - '%s' not in inventory", arg1 );
	return;
    }

    if ( ( vch = get_char_room( ch, arg2 ) ) == NULL )
    {
	progbug( ch, "Mpgive: victim not found: %s", arg2 );
	return;
    }
#if 0
    if ( vch->carry_number + get_obj_number( obj ) > can_carry_n( vch ) )
    {
	act( "$N has $S hands full.", ch, NULL, vch, TO_CHAR );
	return;
    }

    if ( get_carry_weight( vch ) + get_obj_weight( obj ) > can_carry_w( vch ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, vch, TO_CHAR );
	return;
    }

    if ( !can_see_obj( vch, obj ) )
    {
	act( "$N can't see it.", ch, NULL, vch, TO_CHAR );
	return;
    }
#endif
    obj_from_char( obj );
    obj_to_char( obj, vch );
    oprog_give_trigger( obj, ch, vch );
    if ( obj->deleted || ch->deleted || vch->deleted )
      return;
    mprog_give_trigger( vch, ch, obj );
    return;

}


/* lets the mobile goto any location it wishes that is not private */
void
do_mpgoto( CHAR_DATA *ch, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	progbug( ch, "Mpgoto - No argument" );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	progbug( ch, "Mpgoto - No such location: %s", arg );
	return;
    }

    if ( location->sector_type == SECT_BOILING )
    {
	progbug( ch, "Mpgoto - boiling ocean: %s", arg );
	return;
    }

    if ( location == ch->in_room )
      return;

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );

    char_from_room( ch );
    char_to_room( ch, location );

    return;
}


void
do_mpgtransfer( CHAR_DATA *ch, char *argument )
{
    mob_gtransfer( ch, argument );
}


/* lets the mobile destroy an object in its inventory
   it can also destroy a worn object and it can destroy 
   items using all.xxxxx or just plain all of them */

void
do_mpjunk( CHAR_DATA *ch, char *argument )
{
    char		arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *		obj;
    OBJ_DATA *		obj_next;
    ROOM_INDEX_DATA *	pTrashbin;

    one_argument( argument, arg );

    if ( arg[0] == '\0')
    {
	progbug( ch, "Mpjunk - No argument" );
	return;
    }

    pTrashbin = get_room_index( ROOM_VNUM_DISPOSAL );

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	if ( ( obj = get_obj_wear( ch, arg ) ) != NULL )
	{
	    unequip_char( ch, obj );
	    if ( pTrashbin == NULL )
	        extract_obj( obj );
            else
            {
                obj_from_char( obj );
                obj_to_room( obj, pTrashbin );
            }
	    return;
	}
	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	    return; 
	if ( pTrashbin == NULL )
	    extract_obj( obj );
	else
	{
	    obj_from_char( obj );
	    obj_to_room( obj, pTrashbin );
	}
    }
    else
    {
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
	    {
		if ( obj->wear_loc != WEAR_NONE )
		    unequip_char( ch, obj );
		if ( pTrashbin == NULL )
		    extract_obj( obj );
		else
		{
		    obj_from_char( obj );
		    obj_to_room( obj, pTrashbin );
		}
	    } 
	}
    }
    return;
}


/* lets the mobile kill any player or mobile without murder*/
void
do_mpkill( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	progbug( ch, "MpKill - no argument" );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	progbug( ch, "MpKill - Victim not in room: %s", arg );
	return;
    }

    if ( victim == ch )
    {
	progbug( ch, "MpKill - Bad victim to attack: %s", arg );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	progbug( ch, "MpKill - Charmed mob attacking master: %s", arg );
	return;
    }

    if ( IS_DEAD( victim ) )
    {
	progbug( ch, "MpKill - Attacking spirit: %s", arg );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {	
	progbug( ch, "MpKill - Already fighting" );
	return;
    }

    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}


/* Lets the mobile silently lock a door */
void
do_mplock( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			door;
    EXIT_DATA *		pExit;
    EXIT_DATA *		pRevExit;
    ROOM_INDEX_DATA *	to_room;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        progbug( ch, "Mplock - no door given" );
        return;
    }

    if ( ( door = find_door( ch, arg, TRUE ) ) == DIR_NONE )
    {
        progbug( ch, "Mplock - room #%d - no door '%s'",
                 ch->in_room == NULL ? 0 : ch->in_room->vnum,
                 arg );
        return;
    }

    pExit = ch->in_room->exit[door];
    SET_BIT( pExit->exit_info, EX_CLOSED );
    SET_BIT( pExit->exit_info, EX_LOCKED );

    /* lock the other side */
    if ( ( to_room   = pExit->to_room            ) != NULL
    &&   ( pRevExit = to_room->exit[rev_dir[door]] ) != NULL
    &&   pRevExit->to_room == ch->in_room )
    {
	SET_BIT( pRevExit->exit_info, EX_CLOSED );
	SET_BIT( pRevExit->exit_info, EX_LOCKED );
    }
}


/* Lets the mobile load a mobile. */
void
do_mpmload( CHAR_DATA *ch, char *argument )
{
    char	    arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *	    victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	progbug( ch, "Mpmload - Bad vnum as arg: %s", arg );
	return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	progbug( ch, "Mpmload - Bad mob vnum: %s", arg );
	return;
    }

    if ( pMobIndex->count >= MAX_MLOAD )
    {
        progbug( ch, "Mpmload - too many mobiles vnum #%d already loaded", atoi( arg ) );
        return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    return;
}


/* lets the mobile load an item.  Objects are loaded into the mob's inventory
   unless object's "take" bit is not set or the arg "room" is given.
   You can specify a level with the load object portion as well. */
void
do_mpoload( CHAR_DATA *ch, char *argument )
{
    char	    arg1[ MAX_INPUT_LENGTH ];
    char	    arg2[ MAX_INPUT_LENGTH ];
    char	    arg3[ MAX_INPUT_LENGTH ];
    char *	    args;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *	    obj;
    int		    level;

    args     = argument;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
	progbug( ch, "Mpoload - Bad syntax: %s", args );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	level = get_trust( ch );
    }
    else
    {
	if ( arg3[0] == '\0' && !str_cmp( arg2, "room" ) )
	{
	    strcpy( arg3, "room" );
	    strcpy( arg2, "0" );
	}

	if ( !is_number( arg2 ) )
	{
	    progbug( ch, "Mpoload - Bad syntax: %s", args );
	    return;
        }
	level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    progbug( ch, "Mpoload - Bad level: %s", arg2 );
	    return;
	}
    }

    if ( arg3[0] != '\0' && str_cmp( arg3, "room" ) )
    {
	progbug( ch, "Mpoload - arg 3 not \"room\"" );
	return;
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	progbug( ch, "Mpoload - Bad vnum arg: vnum %s", arg1 );
	return;
    }

    if ( level == 0 )
	level = pObjIndex->level;

    if ( level != pObjIndex->level && pObjIndex->level >= LEVEL_IMMORTAL )
    {
        progbug( ch, "Mpoload - bad load level %d for obj #%s", level, arg1 );
        return;
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR( obj, ITEM_TAKE ) && str_cmp( arg3, "room" ) )
    {
	obj_to_char( obj, ch );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
    }

    return;
}


void
do_mpopen( CHAR_DATA *ch, char *argument )
{
    mob_open( ch, argument );
}


void
do_mppeace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL )
	    stop_fighting( rch, TRUE );
    }

    return;
}


/* lets the mobile purge all objects and other npcs in the room,
   or purge a specified object or mob in the room.  It can purge
   itself, but this had best be the last command in the MOBprogram
   otherwise ugly stuff will happen */
void
do_mppurge( CHAR_DATA *ch, char *argument )
{
    char		arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *		victim;
    CHAR_DATA *		v_next;
    OBJ_DATA *		obj;
    OBJ_DATA *		obj_next;
    ROOM_INDEX_DATA *	inRoom;
    ROOM_INDEX_DATA *	pTrashbin;

    one_argument( argument, arg );

    inRoom = ch->in_room;

    if ( ( pTrashbin = get_room_index( ROOM_VNUM_DISPOSAL ) ) != NULL )
    {
        if ( inRoom == pTrashbin )	/* already in purge room? */
            return;

        if ( arg[0] == '\0' )
        {
            /* 'purge' -- purge everything except itself */
            for ( victim = inRoom->people; victim != NULL; victim = v_next )
            {
                v_next = victim->next_in_room;
                if ( IS_NPC( victim )
                &&   victim != ch
                &&   victim->pIndexData->vnum != MOB_VNUM_SUPERMOB
                &&   !IS_SET( victim->act, ACT_NOPURGE ) )
                {
                    char_from_room( victim );
                    char_to_room( victim, pTrashbin );
                }
            }

            for ( obj = inRoom->contents; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_content;
                if ( !IS_SET( obj->extra_flags, ITEM_NOPURGE ) )
                {
                    obj_from_room( obj );
                    obj_to_room( obj, pTrashbin );
                }
            }

            return;
        }

        if ( ( victim = get_char_room( ch, arg ) ) == NULL )
        {
            if ( ( obj = get_obj_here( ch, arg ) ) )
            {
                if ( IS_SET( obj->extra_flags, ITEM_NOPURGE ) )
                    progbug( ch, "Mppurge: Purging NOPURGE object: %s", arg );
                else
                {
                    if ( obj->in_room != NULL )
                        obj_from_room( obj );
                    else if ( obj->carried_by != NULL )
                        obj_from_char( obj );
                    obj_to_room( obj, pTrashbin );
                }
            }
            else
            {
                progbug( ch, "Mppurge: Bad argument: %s", arg );
            }
            return;
        }

        if ( !IS_NPC( victim ) )
        {
            progbug( ch, "Mppurge - Purging a PC: %s", arg );
            return;
        }

        if ( IS_SET( victim->act, ACT_NOPURGE ) )
        {
	    progbug( ch, "Mppurge: purging NOPURGE mobile %s", arg );
	    return;
        }

        char_from_room( victim );
        char_to_room( victim, pTrashbin );

        return;
    }

    /* No ROOM_VNUM_DISPOSAL, do it the old (unsafe) way. */
    if ( arg[0] == '\0' )
    {
	/* 'purge' -- purge everything except itself */
	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
	    v_next = victim->next_in_room;
	    if ( IS_NPC( victim ) && victim != ch && !IS_SET( victim->act, ACT_NOPURGE ) )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( !IS_SET( obj->extra_flags, ITEM_NOPURGE ) )
		extract_obj( obj );
	}

	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	if ( ( obj = get_obj_here( ch, arg ) ) )
	{
	    if ( IS_SET( obj->extra_flags, ITEM_NOPURGE ) )
		progbug( ch, "Mppurge: Purging NOPURGE object: %s", arg );
	    else
		extract_obj( obj );
	}
	else
	{
	    progbug( ch, "Mppurge: Bad argument: %s", arg );
	}
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	progbug( ch, "Mppurge - Purging a PC: %s", arg );
	return;
    }

    if ( IS_SET( victim->act, ACT_NOPURGE ) )
    {
	progbug( ch, "Mppurge: purging NOPURGE mobile %s", arg );
	return;
    }

    extract_char( victim, TRUE );
    return;
}


void
do_mpremember( CHAR_DATA *ch, char *argument )
{
    mob_remember( ch, argument );
    return;
}


void
do_mpsecho( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	progbug( ch, "Mpsecho - called w/o argument" );
	return;
    }

    act_new( "$t", ch, argument, NULL, TO_ROOM, POS_DEAD );
    return;

}


/* prints the message to everyone in the room other than the mob and victim */
void
do_mpsechoaround( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	progbug( ch, "Mpsechoaround - No argument" );
	return;
    }

    if ( !( victim=get_char_room( ch, arg ) ) )
    {
	progbug( ch, "Mpsechoaround - victim does not exist: %s", arg );
	return;
    }

    act_new( "$t", ch, argument, victim, TO_NOTVICT, POS_DEAD );
    return;
}

/* prints the message to only the victim */

void
do_mpsechoat( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	progbug( ch, "Mpsechoat - No argument" );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	progbug( ch, "Mpeschoat - victim does not exist: %s", arg );
	return;
    }

    act_new( "$t", ch, argument, victim, TO_VICT, POS_DEAD );
    return;
}


void
do_mpsecret( CHAR_DATA *ch, char *argument )
{
    mob_secret( ch, argument );
}


void
do_mpsetpos( CHAR_DATA *ch, char *argument )
{
    mob_setpos( ch, argument );
}


void
do_mptake( CHAR_DATA *ch, char *argument )
{
    mob_take( ch, argument );
}


/* Specialized teleport, only teleports to another room in SAME area. */
void
do_mpteleport( CHAR_DATA *ch, char *argument )
{
    int		     vnum;
    int		     to_vnum;
    ROOM_INDEX_DATA *to_room = NULL;
    AREA_DATA *	     pArea;

    pArea = ch->in_room->area;

    if ( !pArea )
    {
	progbug( ch, "Mpteleport: not in area" );
	return;
    }

    if ( IS_NPC( ch ) && ch->pIndexData->vnum == MOB_VNUM_SUPERMOB )
    {
	progbug( ch, "Mpteleport: Supermob using mpteleport" );
	return;
    }

    for ( vnum = 0; vnum < (pArea->min_vnum - pArea->max_vnum) / 2; vnum++ )
    {
	to_vnum = number_range( pArea->min_vnum, pArea->max_vnum );
	if ( ( to_room = get_room_index( to_vnum ) ) != NULL )
	    break;
    }

    if ( !to_room )
	return;

    char_from_room( ch );
    char_to_room( ch, to_room );
    return;
}


/* lets the mobile transfer people.  The all argument transfers
   everyone in the current room to the specified location */

void
do_mptransfer( CHAR_DATA *ch, char *argument )
{
    char	     arg1[ MAX_INPUT_LENGTH ];
    char	     arg2[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *	     victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	progbug( ch, "Mptransfer - Bad syntax" );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   d->character != ch
	    &&   d->character->in_room == ch->in_room
	    &&   can_see( ch, d->character ) )
	    {
		char buf[MAX_STRING_LENGTH];
		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    progbug( ch, "Mptransfer - No such location: %s", arg2 );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    progbug( ch, "Mptransfer - Private room: vnum %d", location->vnum );
	    return;
	}
    }

    if ( location->sector_type == SECT_BOILING )
    {
	progbug( ch, "Mptransfer - boiling ocean: %s", arg2 );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	progbug( ch, "Mptransfer - No such person: %s", arg1 );
	return;
    }

    if ( victim->in_room == NULL )
    {
	progbug( ch, "Mptransfer - Victim in Limbo" );
	return;
    }

    if ( victim->in_room == location )
	return;

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );

    char_from_room( victim );
    char_to_room( victim, location );

    return;
}


void
do_mpunhide( CHAR_DATA *ch, char *argument )
{
    int		door;
    EXIT_DATA *	pExit;
    char	buf[MAX_INPUT_LENGTH*2];

    if ( *argument == '\0' )
    {
        progbug( ch, "Mpunhide - No argument" );
        return;
    }

    if ( ( door = door_lookup( argument ) ) == DIR_NONE )
    {
        sprintf( buf, "Mpunhide - no door \"%s\" in room %d",
                 argument,
                 ch->in_room == NULL ? 0 : ch->in_room->vnum );
        progbug( ch, "%s",buf );
        return;
    }

    if ( ( pExit = get_exit( ch->in_room, door ) ) == NULL )
    {
        sprintf( buf, "Mpunhide - no door %s in room %d",
                 argument,
                 ch->in_room == NULL ? 0 : ch->in_room->vnum );
        progbug( ch, "%s", buf );
        return;
    }

    REMOVE_BIT( pExit->exit_info, EX_SECRET );
    return;
}


/* Lets the mobile silently unlock a door */
void
do_mpunlock( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			door;
    EXIT_DATA *		pExit;
    EXIT_DATA *		pRevExit;
    ROOM_INDEX_DATA *	to_room;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        progbug( ch, "Mpunlock - no door given" );
        return;
    }

    if ( ( door = find_door( ch, arg, TRUE ) ) == DIR_NONE )
    {
        progbug( ch, "Mpunlock - room #%d - no door '%s'",
                 ch->in_room == NULL ? 0 : ch->in_room->vnum,
                 arg );
        return;
    }

    pExit = ch->in_room->exit[door];
    REMOVE_BIT( pExit->exit_info, EX_CLOSED );
    REMOVE_BIT( pExit->exit_info, EX_LOCKED );

    /* lock the other side */
    if ( ( to_room   = pExit->to_room            ) != NULL
    &&   ( pRevExit = to_room->exit[rev_dir[door]] ) != NULL
    &&   pRevExit->to_room == ch->in_room )
    {
	REMOVE_BIT( pRevExit->exit_info, EX_CLOSED );
	REMOVE_BIT( pRevExit->exit_info, EX_LOCKED );
    }
}


void
do_eplist( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;
    EXIT_DATA *		pExit;
    OREPROG_DATA *	prog;
    ROOM_INDEX_DATA *	pRoom;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		arg3[MAX_INPUT_LENGTH];
    int			dir;
    int			count;
    int			prognum;
    bool		fAll;
    bool		found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( dir_lookup( arg1 ) != DIR_NONE && arg3[0] == '\0' )
    {
        strcpy( arg3, arg2 );
        strcpy( arg2, arg1 );
        arg1[0] = '\0';
    }

    if ( arg1[0] == '\0' )
        pRoom = ch->in_room;
    else
    {
        if ( ( pRoom = find_location( ch, arg1 ) ) == NULL )
        {
            send_to_char( "No such location.\n\r", ch );
            return;
        }
    }

    if ( ( dir = dir_lookup( arg2 ) ) == DIR_NONE )
    {
        ch_printf( ch, "No such direction '%s'.\n\r", arg2 );
        return;
    }

    if ( ( pExit = pRoom->exit[dir] ) == NULL )
    {
        ch_printf( ch, "Room #%d has no exit %s.\n\r",
                   pRoom->vnum, dir_name[dir] );
        return;
    }

    if ( pExit->eprogs == NULL )
    {
        ch_printf( ch, "Room #%d exit %s has no progs.\n\r",
                   pRoom->vnum, dir_name[dir] );
        return;
    }

    fAll = FALSE;
    prognum = atoi( arg3 );
    if ( arg3[0] == '\0' )
        prognum = 0;
    else if ( !str_cmp( arg3, "all" ) )
        fAll = TRUE;
    else if ( !is_number( arg3 ) || prognum < 0 )
    {
        send_to_char( "Bad program number.\n\r", ch );
        return;
    }

    count = 0;
    found = FALSE;
    pBuf = new_buf( );
    for ( prog = pExit->eprogs; prog != NULL; prog = prog->next )
    {
        if ( fAll || count == prognum )
        {
            if ( !found )
            {
                buf_printf( pBuf, "%s [%d] %s`X\n\r",
                            capitalize( dir_name[dir] ),
                            pRoom->vnum, pRoom->name );
                found = TRUE;
            }
            else
                add_buf( pBuf, "\n\r" );
            buf_printf( pBuf, "[%2d] %s:  %s\n\r%s",
                        count,
                        flag_string( eprog_types, prog->type ),
                        prog->arglist,
                        prog->comlist );
            if ( !fAll )
                break;
        }
        count++;
    }

    if ( !found )
        add_buf( pBuf, "Program not found\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_epstat( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;
    EXIT_DATA *		pExit;
    OREPROG_DATA *	prog;
    ROOM_INDEX_DATA *	pRoom;
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    int			dir;
    int			doorcount;
    int			progcount;
    bool		found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( dir_lookup( arg1 ) != DIR_NONE && arg2[0] == '\0' )
    {
        strcpy( arg2, arg1 );
        arg1[0] = '\0';
    }

    if ( arg1[0] == '\0' )
        pRoom = ch->in_room;
    else
    {
        if ( ( pRoom = find_location( ch, arg1 ) ) == NULL )
        {
            send_to_char( "No such location.\n\r", ch );
            return;
        }
    }

    if ( arg2[0] == '\0' )
    {
        pBuf = new_buf( );
        doorcount = 0;
        found = FALSE;
        for ( dir = 0; dir < MAX_DIR; dir++ )
        {
            if ( ( pExit = pRoom->exit[dir] ) != NULL )
            {
                progcount = 0;
                for ( prog = pExit->eprogs; prog != NULL; prog = prog->next )
                {
                    if ( !found )
                    {
                        buf_printf( pBuf, "[%d] %s`X\n\r", pRoom->vnum, pRoom->name );
                        found = TRUE;
                    }
                    if ( progcount == 0 )
                        buf_printf( pBuf, "%s:\n\r", capitalize( dir_name[dir] ) );
                    buf_printf( pBuf, " [%2d] %s :  %s\n\r", progcount++,
                                flag_string( eprog_types, prog->type ),
                                prog->arglist );
                }
                doorcount++;
            }
        }

        if ( doorcount == 0 )
            buf_printf( pBuf, "Room #%d has no exits.\n\r", pRoom->vnum );
        else if ( !found )
            buf_printf( pBuf, "There are no exit progs on any of the exits in room #%d.\n\r",
                        pRoom->vnum );
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
        return;
    }

    if ( ( dir = dir_lookup( arg2 ) ) == DIR_NONE )
    {
        ch_printf( ch, "No such direction '%s'.\n\r", arg2 );
        return;
    }

    if ( ( pExit = pRoom->exit[dir] ) == NULL )
    {
        ch_printf( ch, "Room #%d has no exit %s.\n\r",
                   pRoom->vnum, dir_name[dir] );
        return;
    }

    progcount = 0;
    pBuf = new_buf( );
    for ( prog = pExit->eprogs; prog != NULL; prog = prog->next )
    {
        if ( progcount == 0 )
        {
            buf_printf( pBuf, "%s [%d] %s`X\n\r",
                        capitalize( dir_name[dir] ), pRoom->vnum,
                        pRoom->name );
        }
        buf_printf( pBuf, "[%2d] %s :  %s\n\r", progcount++,
                    flag_string( eprog_types, prog->type ),
                    prog->arglist );
    }

    if ( progcount == 0 )
        buf_printf( pBuf, "Room #%d has no progs on exit %s.\n\r",
                    pRoom->vnum, dir_name[dir] );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_oplist( CHAR_DATA *ch, char *argument )
{
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    BUFFER *		buf;
    int			count;
    bool		fAll;
    bool		found;
    OBJ_DATA *		obj;
    int			pnum;
    OBJ_INDEX_DATA *	pObj;
    OREPROG_DATA *	prog;
    int			vnum;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Oplist what?\n\r", ch );
	return;
    }

    if ( is_number( arg1 ) )
	vnum = atoi( arg1 );
    else
    {
	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
	vnum = obj->pIndexData->vnum;
    }

    if ( ( pObj = get_obj_index( vnum ) ) == NULL )
    {
	send_to_char( "That object does not exist.\n\r", ch );
	return;
    }

    if ( pObj->progtypes == 0 )
    {
	send_to_char( "That object has no Object Programs.\n\r", ch );
	return;
    }

    if ( is_number( arg2 ) )
    {
	pnum = atoi( arg2 );
	fAll = FALSE;
    }
    else if ( arg2[0] == '\0' )
    {
	pnum = 0;
	fAll = FALSE;
    }
    else if ( !str_cmp( arg2, "all" ) )
    {
	pnum = 0;
	fAll = TRUE;
    }
    else
    {
	send_to_char( "Syntax: oplist <object> <prog_number>\n\r", ch );
	send_to_char( "        oplist <object> all\n\r", ch );
	return;
    }

    buf = new_buf( );

    buf_printf( buf, "Name: %s\n\r", pObj->name );
    buf_printf( buf, "Vnum: %d\n\r", pObj->vnum );
    buf_printf( buf, "Short description: %s`X\n\r", pObj->short_descr );
    buf_printf( buf, "Long description: %s`X\n\r", pObj->description );

    found = FALSE;
    count = 0;
    for ( prog = pObj->oprogs; prog != NULL; prog = prog->next )
    {
	if ( fAll || count == pnum )
	{
	    buf_printf( buf, "\n\r[%2d] %s  %s\n\r%s",
			count,
			flag_string( oprog_types, prog->type ),
			prog->arglist,
			prog->comlist );
	    found = TRUE;
	    if ( !fAll )
		break;
	}
	count++;
    }

    if ( found )
    {
	page_to_char( buf_string( buf ), ch );
	free_buf( buf );
    }
    else
	ch_printf( ch, "Program %d not found on %s`X\n\r",
		   pnum,
		   pObj->short_descr );

    return;
}


void
do_opstat( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    BUFFER *		buf;
    int			num;
    OBJ_DATA *		obj;
    OBJ_INDEX_DATA *	pObj;
    OREPROG_DATA *	prog;
    int			vnum;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	if ( ch->desc != NULL
	&&   ch->desc->editor == ED_OBJECT )
	{
	    sprintf( arg, "%d", ((OBJ_INDEX_DATA *)ch->desc->pEdit)->vnum );
	}
	else
	{
	    send_to_char( "Opstat what?\n\r", ch );
	    return;
	}
    }

    if ( is_number( arg ) )
	vnum = atoi( arg );
    else
    {
	if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
	vnum = obj->pIndexData->vnum;
    }

    if ( ( pObj = get_obj_index( vnum ) ) == NULL )
    {
	send_to_char( "That object does not exist.\n\r", ch );
	return;
    }

    if ( pObj->progtypes == 0 )
    {
	send_to_char( "That object has no Object Programs.\n\r", ch );
	return;
    }

    buf = new_buf( );

    buf_printf( buf, "Name: %s\n\r", pObj->name );
    buf_printf( buf, "Vnum: %d\n\r", pObj->vnum );
    buf_printf( buf, "Short description: %s`X\n\r", pObj->short_descr );
    buf_printf( buf, "Long description: %s`X\n\r", pObj->description );

    num = 0;
    for ( prog = pObj->oprogs; prog != NULL; prog = prog->next )
    {
	buf_printf( buf, "[%2d] %s :  %s\n\r",
		    num++,
		    flag_string( oprog_types, prog->type ),
		    prog->arglist );
    }

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


void
do_rplist( CHAR_DATA *ch, char *argument )
{
    char		arg1[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    BUFFER *		buf;
    int			count;
    bool		fAll;
    bool		found;
    int			pnum;
    OREPROG_DATA *	prog;
    ROOM_INDEX_DATA *	room;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
	room = ch->in_room;
    else
	room = find_location( ch, arg1 );

    if ( room == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room->progtypes == 0 )
    {
	ch_printf( ch, "%s room has no Room Programs.\n\r",
		   room == ch->in_room ? "This" : "That" );
	return;
    }

    if ( is_number( arg2 ) )
    {
	pnum = atoi( arg2 );
	fAll = FALSE;
    }
    else if ( arg2[0] == '\0' )
    {
	pnum = 0;
	fAll = FALSE;
    }
    else if ( !str_cmp( arg2, "all" ) )
    {
	pnum = 0;
	fAll = TRUE;
    }
    else
    {
	send_to_char( "Syntax: rplist <room> <prog_number>\n\r", ch );
	send_to_char( "        rplist <room> all\n\r", ch );
	return;
    }

    buf = new_buf( );
    buf_printf( buf, "Name: %s`X\n\r", room->name );
    buf_printf( buf, "Vnum: %d\n\r", room->vnum );

    found = FALSE;
    count = 0;
    for ( prog = room->rprogs; prog != NULL; prog = prog->next )
    {
	if ( fAll || count == pnum )
	{
	    buf_printf( buf, "\n\r[%2d] %s :  %s\n\r%s",
			count,
			flag_string( rprog_types, prog->type ),
			prog->arglist,
			prog->comlist );

	    if ( !fAll )
		break;
	}
	count++;
    }

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


void
do_rpstat( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    BUFFER *		buf;
    int			num;
    ROOM_INDEX_DATA *	room;
    OREPROG_DATA *	prog;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	room = ch->in_room;
    else
	room = find_location( ch, arg );

    if ( room == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room->progtypes == 0 )
    {
	ch_printf( ch, "%s room has no Room Programs.\n\r",
		   room == ch->in_room ? "This" : "That" );
	return;
    }

    buf = new_buf( );
    buf_printf( buf, "Name: %s`X\n\r", room->name );
    buf_printf( buf, "Vnum: %d\n\r", room->vnum );
    num = 0;
    for ( prog = room->rprogs; prog != NULL; prog = prog->next )
    {
	buf_printf( buf, "[%2d] %s :  %s\n\r",
		    num++,
		    flag_string( rprog_types, prog->type ),
		    prog->arglist );
    }

    page_to_char( buf_string( buf ), ch );
    free_buf( buf );
    return;
}


bool
event_char_mpdelay( EVENT_DATA *event )
{
    if ( event->owner.ch == NULL || event->owner.ch->deleted )
    {
	bug( "Event_char_mpdelay: bad mob.", 0 );
	return FALSE;
    }

    interpret( event->owner.ch, event->argument );
    return FALSE;
}

