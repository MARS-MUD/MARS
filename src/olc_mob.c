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
static	bool	set_mob_race	args( ( MOB_INDEX_DATA *pMob, int race ) );

/* Entry point for editing mob_index_data. */
void
do_medit( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    CHAR_DATA *		pM;
    AREA_DATA *		pArea;
    char		arg[MAX_INPUT_LENGTH];
    char		command[MAX_INPUT_LENGTH];
    char *		argp;

    argument = one_argument( argument, command );

    if( IS_NPC( ch ) )
    {
	send_to_char( "Mobs dont build, they are built!\n\r", ch );
	return;
    }

    if ( command[0] == '\0' )
    {
	if ( ch->pcdata->last_medit == 0 )
	{
	    send_to_char( "MEdit:  There is no default mobile to edit.\n\r", ch );
	}
	else
	{
	    sprintf( arg, "%d", ch->pcdata->last_medit );
	    do_medit( ch, arg );
	}
	return;
    }

    if ( is_number( command ) )
    {
	if ( !( pMob = get_mob_index( atoi( command ) ) ))
	{
	    ch_printf( ch, "MEdit:  Vnum %d does not exist.\n\r", atoi( command ) );
	    return;
	}

	ch->desc->pEdit = (void *)pMob;
	ch->desc->editor = ED_MOBILE;
	ch->pcdata->last_medit = atoi( command );
	medit_show( ch, "" );
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	if ( medit_create( ch, argument ) )
	{
	    if ( argument[ 0 ] == '\0' )
		pArea = ch->in_room->area;
	    else
		pArea = get_vnum_area( atoi( argument ) );
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
	    ch->desc->editor = ED_MOBILE;
	    medit_show( ch, "" );
	}
	return;
    }

    if ( !str_prefix( command, "clear" ) )
    {
	ch->pcdata->last_medit = 0;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( command[0] == 'd' && !str_prefix( command, "delete" ) )
    {
        argp = one_argument( argument, arg );
        if ( !str_match( arg, "mob","mobile" ) || !is_number( argp ) )
        {
            send_to_char( "Syntax:  medit delete mobile <vnum>\n\r", ch );
            return;
        }

        pArea = get_vnum_area( atoi( argp ) );

        if ( medit_delete( ch, argument ) )
        {
            if ( pArea != NULL )
            {
                SET_BIT( pArea->area_flags, AREA_CHANGED );
            }
        }
        return;
    }

    if ( ( pM = get_char_room( ch, command ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( pM ) )
    {
	send_to_char( "MEdit:  Only mobiles can be medited.\n\r", ch );
	return;
    }

    ch->desc->pEdit = (void *)pM->pIndexData;
    ch->desc->editor = ED_MOBILE;
    ch->pcdata->last_medit = pM->pIndexData->vnum;
    medit_show( ch, "" );
    return;
}


/* Mobile Interpreter, called by do_medit. */
void
medit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA		*pArea;
    MOB_INDEX_DATA 	*pMob;
    char		command[MAX_INPUT_LENGTH];
    char		arg[MAX_STRING_LENGTH];
    int 		cmd;
    bitvector		flags;
    int			value;

    if ( IS_NPC( ch ) )
    {
	interpret( ch, argument );
	return;
    }

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_MOB( ch, pMob );
    pArea = pMob->area;

    if ( !IS_BUILDER( ch, pArea ) )
	send_to_char( "MEdit: Insufficient security to modify area.\n\r", ch );

    if ( command[0] == '\0' )
    {
        medit_show( ch, argument );
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
    for ( cmd = 0; *medit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, medit_table[cmd].name ) )
	{
	    if ( (*medit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Take care of flags. */
    if ( ( value = sex_lookup( arg ) ) != NO_FLAG )
    {
        pMob->sex = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Sex set.\n\r", ch);
        return;
    }

    value = race_lookup( arg );
    if ( !str_cmp( arg, race_table[value].name ) )
    {
	set_mob_race( pMob, value );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Race set.\n\r", ch );
	return;
    }

    if ( ( value = flag_value( act_flags, arg ) ) != NO_FLAG )
    {
        flags = pMob->act ^ value;

        if ( IS_SET( flags, ACT_PET ) && IS_SET( flags, ACT_MOUNT ) )
        {
            send_to_char( "MEdit:  Mobile cannot have both pet and mount flags.\n\r", ch );
            return;
        }

        pMob->act = flags;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Act flag toggled.\n\r", ch);
        return;
    }

    if ( ( value = flag_value( shield_types, arg ) ) != NO_FLAG )
    {
        xTOGGLE_BIT( pMob->shielded_by, value );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Shield flag toggled.\n\r", ch);
        return;
    }

    if ( ( value = flag_value( affect_flags, arg ) ) != NO_FLAG )
    {
        xTOGGLE_BIT( pMob->affected_by, value );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Affect flag toggled.\n\r", ch);
        return;
    }

    if ( ( value = flag_value( class_flags, arg ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->class, value );

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Class flag toggled.\n\r", ch );
	return;
    }

    if ( ( value = flag_value( off_flags, arg ) ) != NO_FLAG )
    {
	xTOGGLE_BIT( pMob->off_flags, value );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Offense flag toggled.\n\r", ch );
	return;
    }

    if ( ( value = flag_value( form_flags, arg ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->form, value );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Form flag toggled.\n\r", ch );
	return;
    }

    if ( ( value = flag_value( part_flags, arg ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->parts, value );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Parts flag toggled.\n\r", ch );
	return;
    }

    if ( ( value = flag_value( weapon_flags, arg ) ) != NO_FLAG )
    {
	pMob->dam_type = value;
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Damage noun set.\n\r", ch );
	return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
medit_ac( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    char arg[MAX_INPUT_LENGTH];
    int pierce, bash, slash, exotic;

    do   /* So that I can use break and send the syntax in one place */
    {
	if ( argument[0] == '\0' )  break;

	EDIT_MOB(ch, pMob);
	argument = one_argument( argument, arg );

	if ( !is_number( arg ) )  break;
	pierce = atoi( arg );
	argument = one_argument( argument, arg );

	if ( arg[0] != '\0' )
	{
	    if ( !is_number( arg ) )  break;
	    bash = atoi( arg );
	    argument = one_argument( argument, arg );
	}
	else
	    bash = pMob->ac[AC_BASH];

	if ( arg[0] != '\0' )
	{
	    if ( !is_number( arg ) )  break;
	    slash = atoi( arg );
	    argument = one_argument( argument, arg );
	}
	else
	    slash = pMob->ac[AC_SLASH];

	if ( arg[0] != '\0' )
	{
	    if ( !is_number( arg ) )  break;
	    exotic = atoi( arg );
	}
	else
	    exotic = pMob->ac[AC_EXOTIC];

	pMob->ac[AC_PIERCE] = pierce;
	pMob->ac[AC_BASH]   = bash;
	pMob->ac[AC_SLASH]  = slash;
	pMob->ac[AC_EXOTIC] = exotic;

	send_to_char( "Ac set.\n\r", ch );
	return TRUE;
    } while ( FALSE );    /* Just do it once.. */

    send_to_char( "Syntax:  ac [ac-pierce [ac-bash [ac-slash [ac-exotic]]]]\n\r"
		  "help MOB_AC  gives a list of reasonable ac-values.\n\r", ch );
    return FALSE;
}


bool
medit_align( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int	value;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  alignment [number]\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < -1000 || value > 1000 )
    {
	send_to_char( "MEdit:  Range for alignment is -1000 to 1000.\n\r", ch );
	return FALSE;
    }

    pMob->alignment = value;
    send_to_char( "Alignment set.\n\r", ch);
    return TRUE;
}

bool
medit_ethos( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int value;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  ethos [number]\n\r", ch );
    return FALSE;
    }

    value = atoi( argument );
    if ( value < -1000 || value > 1000 )
    {
    send_to_char( "MEdit:  Range for ethos is -1000 to 1000.\n\r", ch );
    return FALSE;
    }

    pMob->ethos = value;
    send_to_char( "Ethos set.\n\r", ch);
    return TRUE;
}


bool
medit_automob( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    char		arg[MAX_INPUT_LENGTH];
    int			autotype;

    EDIT_MOB( ch, pMob );

    one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        if ( !str_cmp( arg, "none" ) || !str_cmp( arg, "noauto" ) )
            strcpy( arg, "noset" );
        if ( ( autotype = flag_value( automob_types, arg ) ) == NO_FLAG )
        {
            send_to_char( "Syntax:  automob none|easy|medium|hard.\n\r", ch );
            return FALSE;
        }

        if ( pMob->level == 0 )
        {
	    send_to_char( "MEdit:  Level not set.\n\r", ch );
	    return FALSE;
	}

        pMob->autoset = autotype;

        send_to_char( "Automob type set.\n\r", ch );

        if ( autotype == DICE_NOSET )
        {
            return TRUE;
        }
    }

    if ( pMob->level == 0 )
    {
	send_to_char( "MEdit:  Level not set.\n\r", ch );
	return FALSE;
    }

    set_mob_dice( pMob, pMob->autoset );

    send_to_char( "Hit, damage and mana dice set.\n\r", ch );
    return TRUE;
}


bool
medit_create( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    AREA_DATA *		pArea;
    int			value;
    int			iHash;

    if ( argument[ 0 ] == '\0' )
    {
	pArea = ch->in_room->area;
	for ( value = pArea->min_vnum; value <= pArea->max_vnum; value++ )
	    if ( !get_mob_index( value ) )
		break;
	if ( value > pArea->max_vnum )
	{
	    send_to_char( "MEdit:  No free mobile vnums in this area.\n\r", ch );
	    return FALSE;
	}
    }
    else
    {
	value = atoi( argument );
    }

    /* OLC 1.1b */
    if ( value <= 0 || value > MAX_VNUM )
    {
	ch_printf( ch, "Syntax:  create [0 < vnum < %d]\n\r", MAX_VNUM );
	return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
	send_to_char( "MEdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_mob_index( value ) )
    {
	send_to_char( "MEdit:  Mobile vnum already exists.\n\r", ch );
	return FALSE;
    }

    pMob			= new_mob_index();
    pMob->vnum			= value;
    pMob->area			= pArea;

    if ( value > top_vnum_mob )
	top_vnum_mob = value;        

    pMob->race			= race_lookup( "human" );
    pMob->start_pos		= POS_STANDING;
    pMob->default_pos		= POS_STANDING;
    pMob->hit[0]		= hitdice_table[0].hit[0];
    pMob->hit[1]		= hitdice_table[0].hit[1];

    set_mob_race( pMob, pMob->race );
    SET_BIT( pMob->act, ACT_STAY_AREA );

    iHash			= value % MAX_KEY_HASH;
    pMob->next			= mob_index_hash[iHash];
    mob_index_hash[iHash]	= pMob;
    ch->desc->pEdit		= (void *)pMob;
    ch->pcdata->last_medit	= pMob->vnum;

    send_to_char( "Mobile Created.\n\r", ch );
    return TRUE;
}


bool
medit_clan( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *		pClan;
    MOB_INDEX_DATA *	pMob;
    char		arg[MAX_INPUT_LENGTH];

    EDIT_MOB( ch, pMob );

    if ( !IS_SET( pMob->area->area_flags, AREA_CLAN ) )
    {
	send_to_char( "MEdit:  This area is not a clan area.\n\r", ch );
	return FALSE;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax: clan <clan_name>\n\r", ch );
        return FALSE;
    }

    if ( !str_cmp( arg, "None" ) )
    {
        free_string( pMob->clan );
        pMob->clan = &str_empty[0];
        send_to_char( "Mob clan cleared.\n\r", ch );
        return TRUE;
    }

    if ( ( pClan = get_clan( arg ) ) == NULL )
    {
        ch_printf( ch, "MEdit:  No such clan \"%s\".\n\r", arg );
        return FALSE;
    }

    free_string( pMob->clan );
    pMob->clan = str_dup( pClan->name );
    send_to_char( "Mob clan set.\n\r", ch );

    return TRUE;
}


bool
medit_copy( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    MOB_INDEX_DATA *	pSrc;
    SHOP_DATA *		pPrev;
    SHOP_DATA *		pShop;
    MPROG_DATA *	pProg;
    MPROG_DATA *	pSrcProg;
    MPROG_DATA *	pLastProg;
    char		arg[MAX_INPUT_LENGTH];
    int			i;

    EDIT_MOB( ch, pMob );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  copy <mob_vnum> (progs)\n\r", ch );
	return FALSE;
    }

    if ( ( pSrc = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	send_to_char( "MEdit:  Mobile vnum does not exist.\n\r", ch );
	return FALSE;
    }

    if ( pSrc == pMob )
    {
	send_to_char( "MEdit:  Cannot copy a mobile onto itself.\n\r", ch );
	return FALSE;
    }

    if ( IS_SET( pSrc->area->area_flags, AREA_PROTOTYPE )
    &&  !IS_BUILDER( ch, pSrc->area ) )
    {
	send_to_char( "You may not copy mobiless from that area.\n\r", ch );
	return FALSE;
    }

    free_string( pMob->player_name );
    pMob->player_name	= str_dup( pSrc->player_name );
    free_string( pMob->short_descr );
    pMob->short_descr	= str_dup( pSrc->short_descr );
    free_string( pMob->long_descr );
    pMob->long_descr	= str_dup( pSrc->long_descr );
    free_string( pMob->description );
    pMob->description	= str_dup( pSrc->description );
    free_string( pMob->clan );
    pMob->clan		= str_dup( pSrc->clan );

    pMob->spec_fun	= pSrc->spec_fun;
    pMob->act		= pSrc->act;
    pMob->affected_by	= pSrc->affected_by;
    pMob->shielded_by	= pSrc->shielded_by;
    pMob->alignment	= pSrc->alignment;
    pMob->ethos = pSrc->ethos;
    pMob->level		= pSrc->level;
    pMob->hitroll	= pSrc->hitroll;
    for ( i = 0; i < 3; i++ )
    {
	pMob->hit[i]	= pSrc->hit[i];
	pMob->mana[i]	= pSrc->mana[i];
	pMob->damage[i]	= pSrc->damage[i];
    }
    for ( i = 0; i < 4; i++ )
	pMob->ac[i]	= pSrc->ac[i];
    pMob->dam_type	= pSrc->dam_type;
    pMob->off_flags	= pSrc->off_flags;
    pMob->imm_flags	= pSrc->imm_flags;
    pMob->res_flags	= pSrc->res_flags;
    pMob->vuln_flags	= pSrc->vuln_flags;
    pMob->start_pos	= pSrc->start_pos;
    pMob->default_pos	= pSrc->default_pos;
    pMob->sex		= pSrc->sex;
    pMob->race		= pSrc->race;
    pMob->class		= pSrc->class;
    pMob->train		= pSrc->train;
    pMob->wealth	= pSrc->wealth;
    pMob->form		= pSrc->form;
    pMob->parts		= pSrc->parts;
    pMob->size		= pSrc->size;

    if ( pMob->pShop != NULL && pSrc->pShop == NULL )
    {
	for ( pShop = shop_first, pPrev = NULL; pShop; pPrev = pShop,
	      pShop = pShop->next )
	{
	    if ( pShop == pMob->pShop )
	      break;
	}

	if ( pPrev == NULL )
	    shop_first = shop_first->next;
	else
	    pPrev->next = pShop->next;

	free_shop( pShop );
	pMob->pShop = NULL;
    }
    else if ( pMob->pShop == NULL && pSrc->pShop != NULL )
    {
	pMob->pShop         = new_shop();
	pMob->pShop->keeper = pMob->vnum;
	shop_last->next     = pMob->pShop;
	shop_last	    = pMob->pShop;
    }
    if ( pMob->pShop != NULL && pSrc->pShop != NULL )
    {
	for ( i = 0; i < MAX_TRADE; i++ )
	{
	    pMob->pShop->buy_type[i]	= pSrc->pShop->buy_type[i];
	}
	pMob->pShop->profit_buy		= pSrc->pShop->profit_buy;
	pMob->pShop->profit_sell	= pSrc->pShop->profit_sell;
	pMob->pShop->open_hour		= pSrc->pShop->open_hour;
	pMob->pShop->close_hour		= pSrc->pShop->close_hour;
    }

    if ( LOWER( *argument == 'p' ) && !str_prefix( argument, "progs" ) )
    {
        if ( pSrc->mobprogs == NULL )
        {
            send_to_char( "MEdit:  Source mobile has no progs.\n\r", ch );
        }
        else
        {
            /* Fix compiler warning; code is correct. */
            pLastProg = NULL;

            /* Clear existing progs */
            while ( ( pProg = pMob->mobprogs ) != NULL )
            {
                pMob->mobprogs = pProg->next;
                free_mprog_data( pProg );
            }

            /* Copy the progs, preserving order. */
            for ( pSrcProg = pSrc->mobprogs; pSrcProg != NULL; pSrcProg = pSrcProg->next )
            {
                pProg = new_mprog_data( );
                pProg->type = pSrcProg->type;
                pProg->arglist = str_dup( pSrcProg->arglist );
                pProg->comlist = str_dup( pSrcProg->comlist );
                pProg->triggered = pSrcProg->triggered;

                if ( pMob->mobprogs == NULL )
                    pMob->mobprogs = pProg;
                else
                    pLastProg->next = pProg;
                pLastProg = pProg;
            }
            pMob->progtypes = pSrc->progtypes;
        }
    }

    medit_show( ch, "" );
    return TRUE;
}


bool
medit_damage( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int numdice = 1;
    int sizedice = 1;
    int plus = 0;
    int num = 1;
    char *p;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  damage [num d size +bonus]\n\r", ch );
	return FALSE;
    }

    p = argument;
    while ( isspace( *p ) ) p++;
    if ( isdigit( *p ) )
    {
	num = atoi( p );
	while ( isdigit( *p ) ) p++;
	while ( isspace( *p ) ) p++;
    }

    if ( *p == '\0' )
    {
	plus = num;
    }
    else if ( LOWER( *p ) != 'd' )
    {
	send_to_char( "Syntax:  damage [num d size +bonus]\n\r", ch );
	return FALSE;
    }
    else
    {
	numdice = num;
	p++;
	while( isspace( *p ) ) p++;
	if ( !isdigit( *p ) )
	{
	    send_to_char( "Syntax:  damage [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	sizedice = atoi( p );
	while ( isdigit( *p ) ) p++;
	while ( isspace( *p ) ) p++;

	if ( *p != '+' && *p != '\0' )
	{
	    send_to_char( "Syntax:  damage [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	if ( *p == '+' )
	    p++;
	while ( isspace( *p ) ) p++;
	if ( *p != '\0' && !isdigit( *p ) )
	{
	    send_to_char( "Syntax:  damage [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	plus = atoi( p );
    }
    if ( numdice >= 200 )
    {
        send_to_char( "MEdit:  Number of dice cannot be greater than 200.\n\r", ch );
        return FALSE;
    }
    if ( numdice == 0 )
    {
	send_to_char( "MEdit:  Number of dice cannot be zero.\n\r", ch );
	return FALSE;
    }
    if ( sizedice >= 200 )
    {
        send_to_char( "MEdit:  Dice sides cannot be greater than 200.\n\r", ch );
        return FALSE;
    }
    if ( sizedice == 0 )
    {
	send_to_char( "MEdit:  Dice sides cannot be zero.\n\r", ch );
	return FALSE;
    }

    pMob->damage[DICE_NUMBER]	= numdice;
    pMob->damage[DICE_TYPE]	= sizedice;
    pMob->damage[DICE_BONUS]	= plus;

    send_to_char( "Damage dice set.\n\r", ch);
    return TRUE;
}


bool
medit_damtype( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    int			value;

    EDIT_MOB( ch, pMob );

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  damtype <damtype>\n\r", ch );
	return FALSE;
    }

    if ( ( value = flag_value( weapon_flags, argument ) ) == NO_FLAG )
    {
	send_to_char( "MEdit:  Bad damage type.  See '? weapon'.\n\r", ch );
	return FALSE;
    }

    pMob->dam_type = value;
    send_to_char( "Damage type set.\n\r", ch );
    return TRUE;
}


bool
medit_delete( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *	d;
    char		logbuf[MAX_INPUT_LENGTH];
    AREA_DATA *		pArea;
    MOB_INDEX_DATA *	pMobIndex;
    RESET_DATA *	pReset;
    ROOM_INDEX_DATA *	pRoom;
    int			iHash;
    int			vnum;

    argument = one_argument( argument, arg );

    if ( !str_match( arg, "mob", "mobile" ) || !is_number( argument ) )
    {
        send_to_char( "Syntax:  delete mobile <vnum>\n\r", ch );
        return FALSE;
    }

    vnum = atoi( argument );
    if ( vnum < 1 || vnum > MAX_VNUM )
    {
        ch_printf( ch, "MEdit:  Vnum must be between 1 and %d.\n\r", MAX_VNUM );
        return FALSE;
    }

    if ( ( pArea = get_vnum_area( vnum ) ) == NULL )
    {
        send_to_char( "MEdit:  That vnum is not assigned to an area.\n\r", ch );
        return FALSE;
    }

    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
        send_to_char( "MEdit:  That mob vnum does not exist.\n\r", ch );
        return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "MEdit:  That vnum is in an area you cannot build in.\n\r", ch );
        return FALSE;
    }

    if ( pArea == get_vnum_area( ROOM_VNUM_LIMBO ) && !IS_CODER( ch ) )
    {
	send_to_char( "MEdit:  Only Coders may delete mobiles in Limbo.\n\r", ch );
	return FALSE;
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pRoom = room_index_hash[iHash]; pRoom != NULL; pRoom = pRoom->next )
        {
            for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
            {
                if ( pReset->command == 'M' && pReset->arg1 == vnum )
                {
                    ch_printf( ch, "MEdit:  Mobile is reset in room #%d.  Remove it first.\n\r", pRoom->vnum );
                    return FALSE;
                }
            }
        }
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING
        &&   d != ch->desc
        &&   ( d->pEdit == pMobIndex || d->inEdit == pMobIndex ) )
        {
            send_to_char( "MEdit:  Somebody else is editing that mobile.\n\r", ch );
            return FALSE;
        }
    }

    if ( ch->desc->inEdit == pMobIndex )
        edit_done( ch );
    if ( ch->desc->pEdit == pMobIndex )
        edit_done( ch );

    sprintf( logbuf, "Medit_delete: %s deleting mob #%d.", ch->name, vnum );
    wiznet( "$t", ch, logbuf, WIZ_BUILD, 0, get_trust( ch ) );
    log_string( logbuf );
    delete_mobile( pMobIndex );
    send_to_char( "Mobile deleted.\n\r", ch );

    return TRUE;
}


bool
medit_desc( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pMob->description );
	return TRUE;
    }

    send_to_char( "Syntax:  desc    - line edit\n\r", ch );
    return FALSE;
}


bool
medit_form( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    bitvector		value;

    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( form_flags, argument ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->form, value );
	send_to_char( "Form toggled.\n\r", ch );
	return TRUE;
    }

    send_to_char( "Flag not found.\n\r", ch );
    return FALSE;
}


bool
medit_group( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *	    pArea;
    int		    value;
    int		    max_val;

    EDIT_MOB( ch, pMob );
    pArea = pMob->area;

    if ( *argument == '\0' )
    {
	send_to_char( "Syntax:  group <number> or \"none\".\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( argument, "none" ) )
    {
	if ( !pMob->group )
	{
	    send_to_char( "Ok.\n\r", ch );
	    return FALSE;
	}
	pMob->group = 0;
	send_to_char( "Group removed.\n\r", ch );
	return TRUE;
    }

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  group <number> or \"none\".\n\r", ch );
	return FALSE;
    }

    max_val = UMIN( pArea->max_vnum - pArea->min_vnum, 100 );
    value = atoi( argument );
    if ( value < 0 || value > max_val )
    {
	ch_printf( ch, "MEdit:  Group value is 0 to %d.\n\r", max_val );
	return FALSE;
    }

    pMob->group = pArea->min_vnum + value;
    send_to_char( "Group set.\n\r", ch );
    return TRUE;
}


bool
medit_hitdice( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int numdice = 1;
    int sizedice = 1;
    int plus = 0;
    int num = 1;
    char *p;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  hitdice [num d size +bonus]\n\r", ch );
	return FALSE;
    }

    p = argument;
    while ( isspace( *p ) ) p++;
    if ( isdigit( *p ) )
    {
	num = atoi( p );
	while ( isdigit( *p ) ) p++;
	while ( isspace( *p ) ) p++;
    }

    if ( *p == '\0' )
    {
	plus = num;
    }
    else if ( LOWER( *p ) != 'd' )
    {
	send_to_char( "Syntax:  hitdice [num d size +bonus]\n\r", ch );
	return FALSE;
    }
    else
    {
	numdice = num;
	p++;
	while( isspace( *p ) ) p++;
	if ( !isdigit( *p ) )
	{
	    send_to_char( "Syntax:  hitdice [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	sizedice = atoi( p );
	while ( isdigit( *p ) ) p++;
	while ( isspace( *p ) ) p++;

	if ( *p != '+' && *p != '\0' )
	{
	    send_to_char( "Syntax:  hitdice [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	if ( *p == '+' )
	    p++;
	while ( isspace( *p ) ) p++;
	if ( *p != '\0' && !isdigit( *p ) )
	{
	    send_to_char( "Syntax:  hitdice [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	plus = atoi( p );
    }

    if ( numdice >= 200 )
    {
        send_to_char( "MEdit:  Number of dice cannot be higher than 200.\n\r", ch );
        return FALSE;
    }
    if ( numdice == 0 )
    {
	send_to_char( "MEdit:  Number of dice cannot be zero.\n\r", ch );
	return FALSE;
    }
    if ( sizedice >= 200 )
    {
        send_to_char( "MEdit:  Dice sides cannot be greater than 200.\n\r", ch );
        return FALSE;
    }
    if ( sizedice == 0 )
    {
	send_to_char( "MEdit:  Dice sides cannot be zero.\n\r", ch );
	return FALSE;
    }

    pMob->hit[DICE_NUMBER]	= numdice;
    pMob->hit[DICE_TYPE]	= sizedice;
    pMob->hit[DICE_BONUS]	= plus;

    send_to_char( "Hit dice set.\n\r", ch);
    return TRUE;
}


bool
medit_hitroll( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  hitroll [number]\n\r", ch );
	return FALSE;
    }

    pMob->hitroll = atoi( argument );

    send_to_char( "Hitroll set.\n\r", ch);
    return TRUE;
}


bool
medit_immune( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int value;

    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( imm_flags, argument ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->imm_flags, value );
	send_to_char( "Immune toggled.\n\r", ch );
	return TRUE;
    }

    send_to_char( "Flag not found.\n\r", ch );
    return FALSE;
}

/*
bool
medit_level( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int level;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    level = atoi( argument );
    if ( level < 1 )
    {
	send_to_char( "MEdit:  Level must be a positive number.\n\r", ch );
	return FALSE;
    }

    pMob->level = level;
    set_mob_dice( pMob, pMob->autoset );

    send_to_char( "Level set.\n\r", ch );
    return TRUE;
}

*/

bool
medit_level( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB (ch, pMob);
    int argfun;

    if (argument[0] == '\0' || !is_number (argument))
    {
        send_to_char ("Syntax:  level [number]\n\r", ch);
        return FALSE;
    }

    argfun = atoi (argument);

    pMob->level = argfun;

    pMob->hit[DICE_NUMBER] = ( argfun );
    pMob->hit[DICE_TYPE] = ( argfun * 2 );
    pMob->hit[DICE_BONUS] = 1;
    pMob->damage[DICE_NUMBER] = ( argfun );
    pMob->damage[DICE_TYPE] = ( argfun / 20 );
    pMob->damage[DICE_BONUS] = number_range (1, 4);
    pMob->hitroll = ( argfun * 2 / 6 );
    pMob->wealth = ( argfun * 10);
    pMob->ac[AC_PIERCE] = ( argfun - ( argfun * 2 ) * 2 );
    pMob->ac[AC_BASH] = ( argfun - ( argfun * 2 ) * 2 );
    pMob->ac[AC_SLASH] = ( argfun - ( argfun * 2 ) * 2 );
    pMob->ac[AC_EXOTIC] = ( argfun - ( argfun * 2 ) * 2 );

    send_to_char ("Level set.\n\r", ch);
    return TRUE;
}
bool
medit_long( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    char		buf[ MAX_INPUT_LENGTH + 4 ];
    char *		p;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pMob->long_descr, argument ) ) != NULL )
    {
	strcpy( buf, p );
	free_string( p );
	buf[0] = UPPER( buf[0] );
	if ( ( *argument != '.' && *argument != '/' ) || *(argument+1) != 'r' )
	    strcat( buf, "\n\r" );
	pMob->long_descr = str_dup( buf );
	send_to_char( "Long description set.\n\r", ch );
	return TRUE;
    }
    else
    {
	return FALSE;
    }

}


bool
medit_mana( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int numdice = 1;
    int sizedice = 1;
    int plus = 0;
    int num = 1;
    char *p;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  mana [num d size +bonus]\n\r", ch );
	return FALSE;
    }

    p = argument;
    while ( isspace( *p ) ) p++;
    if ( isdigit( *p ) )
    {
	num = atoi( p );
	while ( isdigit( *p ) ) p++;
	while ( isspace( *p ) ) p++;
    }

    if ( *p == '\0' )
    {
	plus = num;
    }
    else if ( LOWER( *p ) != 'd' )
    {
	send_to_char( "Syntax:  mana [num d size +bonus]\n\r", ch );
	return FALSE;
    }
    else
    {
	numdice = num;
	p++;
	while( isspace( *p ) ) p++;
	if ( !isdigit( *p ) )
	{
	    send_to_char( "Syntax:  mana [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	sizedice = atoi( p );
	while ( isdigit( *p ) ) p++;
	while ( isspace( *p ) ) p++;

	if ( *p != '+' && *p != '\0' )
	{
	    send_to_char( "Syntax:  mana [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	if ( *p == '+' )
	    p++;
	while ( isspace( *p ) ) p++;
	if ( *p != '\0' && !isdigit( *p ) )
	{
	    send_to_char( "Syntax:  mana [num d size +bonus]\n\r", ch );
	    return FALSE;
	}
	plus = atoi( p );
    }

    if ( numdice == 0 )
    {
	send_to_char( "MEdit:  Number of dice cannot be zero.\n\r", ch );
	return FALSE;
    }
    if ( sizedice == 0 )
    {
	send_to_char( "MEdit:  Dice sides cannot be zero.\n\r", ch );
	return FALSE;
    }

    pMob->mana[DICE_NUMBER]	= numdice;
    pMob->mana[DICE_TYPE]	= sizedice;
    pMob->mana[DICE_BONUS]	= plus;

    send_to_char( "Mana dice set.\n\r", ch);
    return TRUE;
}


bool
medit_name( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    char *		p;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pMob->player_name, argument ) ) != NULL )
    {
	pMob->player_name = p;
	send_to_char( "Name set.\n\r", ch);
	return TRUE;
    }
    return FALSE;
}


bool
medit_offensive( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    bitvector		value;

    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( off_flags, argument ) ) != NO_FLAG )
    {
        xTOGGLE_BIT( pMob->off_flags, value );
        send_to_char( "Offense flag toggled.\n\r", ch );
        return TRUE;
    }

    send_to_char( "Flag not found.\n\r", ch );
    return FALSE;
}


bool
medit_parts( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    bitvector		value;

    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( part_flags, argument ) ) != NO_FLAG )
    {
        TOGGLE_BIT( pMob->parts, value );
        send_to_char( "Parts flag toggled.\n\r", ch );
        return TRUE;
    }

    send_to_char( "Flag not found.\n\r", ch );
    return FALSE;
}


bool
medit_position( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    char		arg[MAX_INPUT_LENGTH];
    int			value;

    EDIT_MOB( ch, pMob );
    argument = one_argument( argument, arg );

    switch ( arg[0] )
    {
    default:
	break;

    case 'S':
    case 's':
	if ( str_prefix( arg, "start" ) )
	    break;

	if ( ( value = position_lookup( argument ) ) < 0 )
	    break;

	pMob->start_pos = value;
	send_to_char( "Start position set.\n\r", ch );
	return TRUE;

    case 'D':
    case 'd':
	if ( str_prefix( arg, "default" ) )
	    break;

	if ( ( value = position_lookup( argument ) ) < 0 )
	    break;

	pMob->default_pos = value;
	send_to_char( "Default position set.\n\r", ch );
	return TRUE;
    }

    send_to_char( "Syntax:  position [start/default] [position]\n\r"
		  "Type '? position' for a list of positions.\n\r", ch );
    return FALSE;
}


bool
medit_resistant( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int value;

    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( res_flags, argument ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->res_flags, value );
	send_to_char( "Resistant toggled.\n\r", ch );
	return TRUE;
    }

    send_to_char( "Flag not found.\n\r", ch );
    return FALSE;
}


bool
medit_shop( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    char		command[MAX_INPUT_LENGTH];
    char		arg1[MAX_INPUT_LENGTH];
    int			item_number;

    argument = one_argument( argument, command );
    argument = one_argument( argument, arg1 );

    EDIT_MOB( ch, pMob );

    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax:  shop create\n\r", ch );
	send_to_char( "         shop hours [#opening] [#closing]\n\r", ch );
	send_to_char( "         shop profit [#buying%] [#selling%]\n\r", ch );
	send_to_char( "         shop type [#1-5] [item type]\n\r", ch );
	send_to_char( "         shop remove [#1-5]\n\r", ch );
	send_to_char( "         shop delete\n\r", ch );
	return FALSE;
    }


    if ( !str_cmp( command, "hours" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char( "Syntax:  shop hours [#opening] [#closing]\n\r", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    pMob->pShop         = new_shop( );
	    pMob->pShop->keeper = pMob->vnum;
	    shop_last->next     = pMob->pShop;
	    shop_last		= pMob->pShop;
	}

	pMob->pShop->open_hour = atoi( arg1 );
	pMob->pShop->close_hour = atoi( argument );

	send_to_char( "Shop hours set.\n\r", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "profit" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char( "Syntax:  shop profit [#buying%] [#selling%]\n\r", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    pMob->pShop         = new_shop( );
	    pMob->pShop->keeper = pMob->vnum;
	    shop_last->next     = pMob->pShop;
	    shop_last		= pMob->pShop;
	}

	pMob->pShop->profit_buy     = atoi( arg1 );
	pMob->pShop->profit_sell    = atoi( argument );

	send_to_char( "Shop profit set.\n\r", ch );
	return TRUE;
    }


    if ( !str_cmp( command, "create" ) )
    {
	if ( pMob->pShop != NULL )
	{
	    send_to_char( "MEdit:  Shop already exists.\n\r", ch );
	    return FALSE;
	}

	/*pMob->pShop		= new_shop( );
	pMob->pShop->keeper	= pMob->vnum;
	shop_last->next		= pMob->pShop;
	shop_last		= pMob->pShop;*/
	 pMob->pShop = new_shop ();                                                                                                            
        if (!shop_first)                                                                                                                      
            shop_first = pMob->pShop;                                                                                                         
        if (shop_last)                                                                                                                        
            shop_last->next = pMob->pShop;                                                                                                    
        shop_last = pMob->pShop;                                                                                                              
                                                                                                                                              
        pMob->pShop->keeper = pMob->vnum;  

	send_to_char( "Shop created.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "type" ) )
    {
	int value;

	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' )
	{
	    send_to_char( "Syntax:  shop type [#1-5] [item type]\n\r", ch );
	    return FALSE;
	}

	item_number = atoi( arg1 );

	if ( item_number < 1 || item_number > MAX_TRADE )
	{
	    ch_printf( ch, "MEdit:  May buy %d items max.\n\r", MAX_TRADE );
	    return FALSE;
	}

	if ( ( value = flag_value( item_types, argument ) ) == NO_FLAG )
	{
	    send_to_char( "MEdit:  That type of item is not known.\n\r", ch );
	    return FALSE;
	}

	if ( pMob->pShop == NULL )
	{
	    pMob->pShop         = new_shop( );
	    pMob->pShop->keeper = pMob->vnum;
	    shop_last->next     = pMob->pShop;
	    shop_last		= pMob->pShop;
	}

	pMob->pShop->buy_type[item_number-1] = value;

	send_to_char( "Shop type set.\n\r", ch );
	return TRUE;
    }


    if ( !str_cmp( command, "remove" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 ) )
	{
	    send_to_char( "Syntax:  shop remove [#1-5]\n\r", ch );
	    return FALSE;
	}

	if ( pMob->pShop == NULL )
	{
	    send_to_char( "MEdit:  Mob is not a shopkeeper.\n\r", ch );
	    return FALSE;
	}

	item_number = atoi( arg1 );

	if ( item_number < 1 || item_number > MAX_TRADE )
	{
	    ch_printf( ch, "MEdit:  May buy %d items max.\n\r", MAX_TRADE );
	    return FALSE;
	}

	item_number--;

	if ( pMob->pShop->buy_type[item_number] != 0 )
	{
	    pMob->pShop->buy_type[item_number] = 0;
	    send_to_char( "Shop type removed.\n\r", ch );
	    return TRUE;
	}
	send_to_char( "MEdit:  Shop type already unset.\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "delete" ) )
    {
        SHOP_DATA *pShop;
	SHOP_DATA *pPrev;

	if ( pMob->pShop == NULL )
	{
	    send_to_char( "MEdit:  No shop to delete.\n\r", ch );
	    return FALSE;
	}

	pPrev = NULL;
	for ( pShop = shop_first; pShop != NULL; pPrev = pShop,
	      pShop = pShop->next )
	{
	    if ( pShop == pMob->pShop )
		break;
	}

	if ( pShop == NULL )
	{
	    send_to_char( "Oops.  Can't find the shop.\n\r", ch );
	    bugf( "Shop remove: shop for %d not on global list.", pMob->vnum );
	    return FALSE;
	}

	if ( pPrev == NULL )
	    shop_first = shop_first->next;
	else
	    pPrev->next = pShop->next;

	if ( shop_last == pShop )
	    shop_last = pPrev;

	free_shop( pShop );
	pMob->pShop = NULL;
	send_to_char( "Shop removed.\n\r", ch );
	return TRUE;
    }

    medit_shop( ch, "" );
    return FALSE;
}


bool
medit_short( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    char *		p;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }

//    free_string( pMob->short_descr );
//    pMob->short_descr = str_dup( argument );

    if ( ( p = string_change( ch, pMob->short_descr, argument ) ) != NULL )
    {
	pMob->short_descr = p;
	send_to_char( "Short description set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;
}


bool
medit_show( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    BUFFER *		pBuf;

    EDIT_MOB( ch, pMob );

    pBuf = new_buf( );
    show_mob_info( ch, pMob, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}


bool medit_size( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( size_types, argument ) ) != NO_FLAG
	&&   value != SIZE_TITANIC )
	{
	    pMob->size = value;
	    send_to_char( "Size set.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: size [size]\n\r"
		  "Type '? size' for a list of sizes.\n\r", ch );
    return FALSE;
}


bool
medit_special( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  special [special function]\n\r", ch );
	return FALSE;
    }


    if ( !str_cmp( argument, "none" ) )
    {
        pMob->spec_fun = NULL;

        send_to_char( "Special function removed.\n\r", ch );
        return TRUE;
    }

    if ( spec_lookup( argument ) )
    {
	pMob->spec_fun = spec_lookup( argument );
	send_to_char( "Special function set.\n\r", ch );
	return TRUE;
    }

    send_to_char( "MEdit: No such special function.\n\r", ch );
    return FALSE;
}


bool
medit_train( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    char		arg[MAX_INPUT_LENGTH];
    int			iClass;
    int			value;

    EDIT_MOB( ch, pMob );

    if ( !IS_SET( pMob->act, ACT_PRACTICE )
    &&	 !IS_SET( pMob->act, ACT_GAIN )
    &&	 !IS_SET( pMob->act, ACT_TRAIN ) )
    {
        send_to_char( "MEdit:  Mobile needs practice or train flag.\n\r", ch );
        return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  teaches <class>|none|all\n\r", ch );
        return FALSE;
    }

    if ( !str_cmp( arg, "none" ) )
    {
        xCLEAR_BITS( pMob->train );
        send_to_char( "Mobile no longer trains any classes.\n\r", ch );
        return TRUE;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
            xSET_BIT( pMob->train, iClass );
        send_to_char( "Mobile now trains all classes.\n\r", ch );
        return TRUE;
    }

    if ( ( value = class_lookup( arg ) ) == NO_CLASS )
    {
        send_to_char( "MEdit:  No such class.\n\r", ch );
        return FALSE;
    }

    xTOGGLE_BIT( pMob->train, value );
    send_to_char( "Class train flag toggled.\n\r", ch );

    return TRUE;
}


bool
medit_vulnerable( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int value;

    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( vuln_flags, argument ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->vuln_flags, value );
	send_to_char( "Vulnerability toggled.\n\r", ch );
	return TRUE;
    }

    send_to_char( "Flag not found.\n\r", ch );
    return FALSE;
}


bool medit_wealth( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *	pMob;
    MONEY		amt;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' || money_value( &amt, argument ) == NULL )
    {
	send_to_char( "Syntax:  wealth [amount]\n\r", ch );
	return FALSE;
    }

    pMob->wealth = CASH_VALUE( amt );

    send_to_char( "Wealth set.\n\r", ch);
    return TRUE;
}


bool
delete_mobile( MOB_INDEX_DATA *pMobIndex )
{
    MOB_INDEX_DATA *	exMob;
    int			iHash;
    CHAR_DATA *		pMob;
    CHAR_DATA *		pMob_next;

    if ( pMobIndex == NULL )
        return FALSE;

    for ( pMob = char_list; pMob != NULL; pMob = pMob_next )
    {
        pMob_next = pMob->next;
        if ( IS_NPC( pMob ) && pMob->pIndexData == pMobIndex )
        {
            act_color( AT_IMM, "$n disappears in a shower of sparks.", pMob, NULL, NULL, TO_ROOM, POS_RESTING );
            extract_char( pMob, TRUE );
        }
    }

    iHash = pMobIndex->vnum % MAX_KEY_HASH;
    if ( mob_index_hash[iHash] == pMobIndex )
    {
        mob_index_hash[iHash] = pMobIndex->next;
    }
    else
    {
        for ( exMob = mob_index_hash[iHash]; exMob != NULL; exMob = exMob->next )
        {
            if ( exMob->next == pMobIndex )
            {
                exMob->next = pMobIndex->next;
                break;
            }
        }
    }

    return TRUE;
}


/*****************************************************************************
 Name:		set_mob_dice
 Purpose:	Sets mob hit mana and damage dice and AC
 		according to difficulty level.
 Called by:	medit_level, medit_automob
 ****************************************************************************/
bool set_mob_dice( MOB_INDEX_DATA *pMob, int difficulty )
{
    int efflevel;
    int	maxtable;

    if ( difficulty == DICE_NOSET )
        return FALSE;

    /* find upper level in table */
    for ( maxtable = 1; hitdice_table[maxtable].level; maxtable++ )
	;

    /* adjust effective level for difficulty factor */
    switch( difficulty )
    {
	case DICE_EASY:
	    efflevel = 0 - ( pMob->level + 20 ) / 20;
	    break;
	default:
	case DICE_MEDIUM:
	    efflevel = 0;
	    break;
	case DICE_HARD:
	    efflevel = ( pMob->level + 20 ) / 20;
	    break;
    }

    /* adjust effective level for mob size */
    switch( pMob->size )
    {
	case SIZE_TINY:
	    efflevel -= 2 * ( ( 33 + pMob->level ) / 33 );
	    break;
	case SIZE_SMALL:
	    efflevel -= ( ( 33 + pMob->level ) / 33 );
	    break;
	default:
	case SIZE_MEDIUM:
	    break;
	case SIZE_LARGE:
	    efflevel += ( ( 33 + pMob->level ) / 33 );
	    break;
	case SIZE_HUGE:
	    efflevel += 2 * ( ( 33 + pMob->level ) / 33 );
	    break;
	case SIZE_GIANT:
	case SIZE_TITANIC:
	    efflevel += 3 * ( ( 33 + pMob->level ) / 33 );
	    break;
    }

    efflevel = URANGE( 0, pMob->level + efflevel, maxtable - 1 );

    pMob->hit[DICE_NUMBER]	= hitdice_table[efflevel].hit[DICE_NUMBER];
    pMob->hit[DICE_TYPE]	= hitdice_table[efflevel].hit[DICE_TYPE];
    pMob->hit[DICE_BONUS]	= hitdice_table[efflevel].hit[DICE_BONUS];
    pMob->damage[DICE_NUMBER]	= hitdice_table[efflevel].dam[DICE_NUMBER];
    pMob->damage[DICE_TYPE]	= hitdice_table[efflevel].dam[DICE_TYPE];
    pMob->damage[DICE_BONUS]	= hitdice_table[efflevel].dam[DICE_BONUS];
    pMob->mana[DICE_NUMBER]	= hitdice_table[efflevel].mana[DICE_NUMBER];
    pMob->mana[DICE_TYPE]	= hitdice_table[efflevel].mana[DICE_TYPE];
    pMob->mana[DICE_BONUS]	= hitdice_table[efflevel].mana[DICE_BONUS];
    pMob->ac[AC_PIERCE]		= number_fuzzy( 10 ) + hitdice_table[efflevel].ac - 10;
    pMob->ac[AC_BASH]		= number_fuzzy( 10 ) + hitdice_table[efflevel].ac - 10;
    pMob->ac[AC_SLASH]		= number_fuzzy( 10 ) + hitdice_table[efflevel].ac - 10;
    pMob->ac[AC_EXOTIC]		= number_fuzzy( 10 ) + hitdice_table[efflevel].ac - 10;
    pMob->hitroll		= hitdice_table[efflevel].hitroll;

    if ( pMob->level <= LEVEL_NEWBIE
    &&	 IS_SET( pMob->area->area_flags, AREA_MUDSCHOOL ) )
    {
	pMob->ac[AC_PIERCE] += 20;
	pMob->ac[AC_BASH] += 20;
	pMob->ac[AC_SLASH] += 20;
	pMob->ac[AC_EXOTIC] += 20;
    }

    return TRUE;
}


static bool set_mob_race( MOB_INDEX_DATA *pMob, int value )
{
    bool sentinel;
    bool stay_area;

    sentinel		= IS_SET( pMob->act, ACT_SENTINEL );
    stay_area		= IS_SET( pMob->act, ACT_STAY_AREA );
    pMob->race		= value;
    pMob->act		= race_table[value].act | ACT_IS_NPC;
    if ( sentinel )
	SET_BIT( pMob->act, ACT_SENTINEL );
    if ( stay_area )
	SET_BIT( pMob->act, ACT_STAY_AREA );
    pMob->affected_by	= race_table[value].aff;
    pMob->off_flags	= race_table[value].off;
    pMob->imm_flags	= race_table[value].imm;
    pMob->res_flags	= race_table[value].res;
    pMob->vuln_flags	= race_table[value].vuln;
    pMob->form		= race_table[value].form;
    pMob->parts		= race_table[value].parts;
    pMob->size		= race_table[value].size;
    return TRUE;
}


void
show_mob_info( CHAR_DATA *ch, MOB_INDEX_DATA *pMob, BUFFER *pBuf )
{
    SHOP_DATA *		pShop;
    char		buf[MAX_INPUT_LENGTH];
    int			iClass;
    int			iTrade;
    char *		p;
    MONEY		wealth;
    MPROG_DATA *	prog;
    int			pcount;
    bool		found;

    buf_printf( pBuf, "Mobile:      [%d][%s]    ", pMob->vnum,
    		pMob->player_name );
    buf_printf( pBuf, "Area:  [%3d] %s\n\r",
		!pMob->area ? -1        : pMob->area->vnum,
		!pMob->area ? "No Area" : pMob->area->name );
    buf_printf( pBuf, "Level:       [%d]  ", pMob->level );
    buf_printf( pBuf, "Sex: [%s]  ", sex_name( pMob->sex ) );
    buf_printf( pBuf, "Ethos: [%d]\n\r", pMob->ethos );
    buf_printf( pBuf, "Align: [%d]\n\r", pMob->alignment );
    buf_printf( pBuf, "Race:        [%s]  ", race_table[pMob->race].name );
    buf_printf( pBuf, "Class: [%s]  ", flag_string( class_flags, pMob->class ) );
    buf_printf( pBuf, "Attack: [%s] (%s)\n\r", flag_string( weapon_flags, pMob->dam_type ), attack_table[pMob->dam_type].noun );
    buf_printf( pBuf, "Hit points:  [%dd%d+%d]  ",  pMob->hit[DICE_NUMBER],
						    pMob->hit[DICE_TYPE],
						    pMob->hit[DICE_BONUS] );
    buf_printf( pBuf, "Damage: [%dd%d+%d]  ",	pMob->damage[DICE_NUMBER],
						pMob->damage[DICE_TYPE],
						pMob->damage[DICE_BONUS] );
    buf_printf( pBuf, "Mana: [%dd%d+%d] ",	pMob->mana[DICE_NUMBER],
						pMob->mana[DICE_TYPE],
						pMob->mana[DICE_BONUS] );
    buf_printf( pBuf, "Auto: [%s]\n\r", flag_string( automob_types, pMob->autoset ) );
    buf_printf( pBuf, "Armor class: [%d] pierce  [%d] bash  [%d] slash  [%d] exotic\n\r",
		pMob->ac[AC_PIERCE],
		pMob->ac[AC_BASH],
		pMob->ac[AC_SLASH],
		pMob->ac[AC_EXOTIC] );
    buf_printf( pBuf, "Hitroll:     [%d]  ", pMob->hitroll );
    pMob->group ? sprintf( buf, "%2d", pMob->group - pMob->area->min_vnum ) : sprintf( buf, "--" );
    buf_printf( pBuf, "Group:  [%2s]   ", buf );
    buf_printf( pBuf, "Size:  [%s]   ", size_name( pMob->size ) );

    wealth.gold = 0;
    wealth.silver = 0;
    wealth.copper = 0;
    wealth.fract = pMob->wealth;
    normalize( &wealth );
    buf_printf( pBuf, "Wealth:  [%s]\n\r", money_string( &wealth, FALSE, TRUE ) );

    buf_printf( pBuf, "Start pos:   [%s]   ", position_table[pMob->start_pos].name );
    buf_printf( pBuf, "Default pos: [%s]\n\r", position_table[pMob->default_pos].name );
    buf_printf( pBuf, "Affected by: [%s]\n\r",	xbit_string( affect_flags, pMob->affected_by ) );
    buf_printf( pBuf, "Shielded by: [%s]\n\r",	xbit_string( shield_types, pMob->shielded_by ) );
    buf_printf( pBuf, "Act flags:   [%s]\n\r",
		flag_string( act_flags, pMob->act ) );
    if ( IS_SET( pMob->act, ACT_PRACTICE )
    ||	 IS_SET( pMob->act, ACT_TRAIN )
    ||	 IS_SET( pMob->act, ACT_GAIN ) )
    {
        add_buf( pBuf, "Teaches:     [" );
        p = buf;
        for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
            if ( xIS_SET( pMob->train, iClass ) )
                p += sprintf( p, " %s", class_table[iClass].name );
        if ( p == buf )
            strcpy( buf, " none" );
        buf_printf( pBuf, "%s]\n\r", buf + 1 );
    }
    buf_printf( pBuf, "Offense:     [%s]\n\r",	xbit_string( off_flags, pMob->off_flags ) );
    buf_printf( pBuf, "Immune:      [%s]\n\r",	flag_string( imm_flags, pMob->imm_flags ) );
    buf_printf( pBuf, "Resist:      [%s]\n\r",	flag_string( res_flags, pMob->res_flags ) );
    buf_printf( pBuf, "Vulnerable:  [%s]\n\r",	flag_string( vuln_flags, pMob->vuln_flags ) );
    buf_printf( pBuf, "Form:        [%s]\n\r",	flag_string( form_flags, pMob->form ) );
    buf_printf( pBuf, "Parts:       [%s]\n\r",	flag_string( part_flags, pMob->parts ) );
    pcount = 0;
    for ( prog = pMob->mobprogs; prog != NULL; prog = prog->next )
        pcount++;
    buf_printf( pBuf, "Mprogs:      [%d]\n\r",  pcount );
    if ( pMob->spec_fun )
	buf_printf( pBuf, "Spec fun:    [%s]\n\r", spec_name( pMob->spec_fun ) );
    if ( IS_SET( pMob->area->area_flags, AREA_CLAN ) )
    {
        buf_printf( pBuf, "Clan:        [%s]\n\r", IS_NULLSTR( pMob->clan ) ? "(none)" : pMob->clan );
    }
    buf_printf( pBuf, "Short descr: %s`X\n\r",	pMob->short_descr );
    buf_printf( pBuf, "Long descr:\n\r%s`X",	pMob->long_descr );
    if ( pMob->description && pMob->description[0] != '\0' )
	buf_printf( pBuf, "Description:\n\r%s", pMob->description );

    if ( ( pShop = pMob->pShop ) != NULL )
    {
	buf_printf( pBuf, "Shop data for [%d]:\n\r", pMob->vnum );
	buf_printf( pBuf, "  Markup: %d%%   Discount: %d%%\n\r",
		    pShop->profit_buy, pShop->profit_sell );
	buf_printf( pBuf, "  Hours: %d to %d\n\r",
		    pShop->open_hour, pShop->close_hour );
	found = FALSE;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	{
	    if ( pShop->buy_type[iTrade] )
	    {
		if ( !found )
		{
		    found = TRUE;
		    add_buf( pBuf, "  Number Trades Type\n\r" );
		    add_buf( pBuf, "  ------ ------ ----\n\r" );
		}
		buf_printf( pBuf, "  [%4d] %s\n\r", iTrade + 1,
		    item_name( pShop->buy_type[iTrade] ) );
	    }
	}
	if ( !found )
	    add_buf( pBuf, "  (Mob does not buy objects)\n\r" );
    }
}
