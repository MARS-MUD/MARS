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
#include <sys/types.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "olc.h"
#include "db.h"
#include "lookup.h"
#include "recycle.h"
#include "tables.h"

/*
 * Local functions
 */
static OREPROG_DATA *get_oprog_data( OBJ_INDEX_DATA *pObj, int prognum );
static bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj,
			    int value_num, char *argument );
static bool set_value( CHAR_DATA *ch, char *argument, int value );
static void show_obj_values( OBJ_INDEX_DATA *pObj, BUFFER *pBuf );


void
do_oedit( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    OBJ_DATA *		pO;
    AREA_DATA *		pArea;
    char		command[MAX_INPUT_LENGTH];
    char		arg[MAX_INPUT_LENGTH];
    char *		argp;

    if( IS_NPC( ch ) )
    {
	send_to_char( "Mobs dont build, they are built!\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	if ( ch->pcdata->last_oedit != 0 )
	{
	    sprintf( arg, "%d", ch->pcdata->last_oedit );
	    do_oedit( ch, arg );
	}
	else
	{
	    send_to_char( "OEdit:  There is no default object to edit.\n\r", ch );
	}
	return;
    }

    argument = one_argument( argument, command );

    if ( is_number( command ) )
    {
	if ( !( pObj = get_obj_index( atoi( command ) ) ) )
	{
	    ch_printf( ch, "OEdit:  Vnum %d does not exist.\n\r", atoi( command ) );
	    return;
	}

	ch->desc->pEdit = (void *)pObj;
	ch->desc->editor = ED_OBJECT;
	ch->pcdata->last_oedit = atoi( command );
	oedit_show( ch, "" );
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
        if ( oedit_create( ch, argument ) )
	{
	    if ( argument[ 0 ] == '\0' )
		pArea = ch->in_room->area;
	    else
		pArea = get_vnum_area( atoi( argument ) );
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
	    ch->desc->editor = ED_OBJECT;
	    oedit_show( ch, "" );
	}
	return;
    }

    if ( !str_prefix( command, "clear" ) )
    {
	ch->pcdata->last_oedit = 0;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( command[0] == 'd' && !str_prefix( command, "delete" ) )
    {
        argp = one_argument( argument, arg );
        if ( !str_match( arg, "obj","object" ) || !is_number( argp ) )
        {
            send_to_char( "Syntax:  oedit delete object <vnum>\n\r", ch );
            return;
        }

        pArea = get_vnum_area( atoi( argp ) );

        if ( oedit_delete( ch, argument ) )
        {
            if ( pArea != NULL )
            {
                SET_BIT( pArea->area_flags, AREA_CHANGED );
            }
        }
        return;
    }

    if ( ( pO = get_obj_here( ch, command ) ) == NULL )
    {
	send_to_char( "OEdit:  No object by that name in room or on your person.\n\r", ch );
	return;
    }
    ch->desc->pEdit = (void *)pO->pIndexData;
    ch->desc->editor = ED_OBJECT;
    ch->pcdata->last_oedit = pO->pIndexData->vnum;
    oedit_show( ch, "" );
    return;
}


/* Object Interpreter, called by do_oedit. */
void
oedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		pArea;
    OBJ_INDEX_DATA *	pObj;
    char		arg[MAX_STRING_LENGTH];
    char		command[MAX_INPUT_LENGTH];
    int			cmd;
    int			value;

    if ( IS_NPC( ch ) )
    {
	interpret( ch, argument );
	return;
    }

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_OBJ( ch, pObj );
    pArea = pObj->area;

    if ( command[0] == '\0' )
    {
	oedit_show( ch, argument );
	return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "OEdit: Insufficient security to modify area.\n\r", ch );
	interpret( ch, arg );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *oedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, oedit_table[cmd].name ) )
	{
	    if ( (*oedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Take care of flags. */
    if ( ( value = flag_value( item_types, arg ) ) != NO_FLAG )
    {
        pObj->item_type = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Type set.\n\r", ch );

        /*
         * Clear the values.
         */
        pObj->value[0] = 0;
        pObj->value[1] = value == ITEM_WEAPON ? 1 : 0;
        pObj->value[2] = value == ITEM_WEAPON ? 6 : 0;
        pObj->value[3] = ( value == ITEM_FURNITURE ? 100 : 0 );
        pObj->value[4] = ( value == ITEM_CONTAINER || value == ITEM_FURNITURE ? 100 : value == ITEM_PORTAL ? SIZE_TITANIC : 0 );
        pObj->value[5] = 0;

        if ( pObj->item_type == ITEM_MAP )
            pObj->value[0] = 1;

        return;
    }

    if ( ( value = flag_value( extra_flags, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT( pObj->extra_flags, value );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Extra flag toggled.\n\r", ch );
        return;
    }

if ( ( value = flag_value( extra_flags2, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT( pObj->extra_flags2, value );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Extra2 flag toggled.\n\r", ch );
        return;
    }

    if ( ( value = flag_value( wear_flags, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT( pObj->wear_flags, value );
	if ( IS_WEARABLE( pObj ) )
	    SET_BIT( pObj->wear_flags, ITEM_TAKE );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Wear flag toggled.\n\r", ch );
        return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
oedit_addaffect( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    AFFECT_DATA *	pAf;
    char		loc[MAX_INPUT_LENGTH];
    char		mod[MAX_INPUT_LENGTH];
    int			value;

    EDIT_OBJ( ch, pObj );

    argument = one_argument( argument, loc );
    one_argument( argument, mod );

    if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod ) )
    {
	send_to_char( "Syntax:  addaffect [location] [#mod]\n\r", ch );
	return FALSE;
    }

    if ( ( value = flag_value( apply_types, loc ) ) == NO_FLAG )
    {
        send_to_char( "Valid affects are:\n\r", ch );
	show_help( ch, "apply" );
	return FALSE;
    }

    if ( pObj->item_type == ITEM_WEAPON && value == APPLY_DAMROLL )
    {
	send_to_char( "OEdit:  Cannot add damroll to weapons.  Use v5 instead.\n\r", ch );
	return FALSE;
    }

    if ( pObj->item_type == ITEM_ARMOR && value == APPLY_AC )
    {
	send_to_char( "OEdit:  Cannot add AC affect to armor.  Use v0 through v3 instead.\n\r", ch );
	return FALSE;
    }

    pAf             =   new_affect();
    pAf->location   =   value;
    pAf->modifier   =   atoi( mod );
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;

    send_to_char( "Affect added.\n\r", ch );
    return TRUE;
}


bool
oedit_anti( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    char		arg[MAX_INPUT_LENGTH];
    int			value;

    EDIT_OBJ( ch, pObj );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Sytax:  anti <race or class>\n\r", ch );
        return FALSE;
    }

    if ( ( value = race_lookup( arg ) ) != 0 )
    {
        xTOGGLE_BIT( pObj->race_flags, value );
        send_to_char( "Race flag toggled.\n\r", ch );
        return TRUE;
    }

    if ( ( value = class_lookup( arg ) ) != NO_CLASS )
    {
        xTOGGLE_BIT( pObj->class_flags, value );
        send_to_char( "Class flag toggled.\n\r", ch );
        return TRUE;
    }

    send_to_char( "OEdit:  Class or race not found.\n\r", ch );
    return FALSE;
}


bool
oedit_autoset( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;

    EDIT_OBJ( ch, pObj );

    if ( set_obj_stats( pObj ) )
    {
        switch( pObj->item_type )
        {
            case ITEM_ARMOR:
                send_to_char( "Armor values set.\n\r", ch );
                return TRUE;
            case ITEM_WEAPON:
                send_to_char( "Weapon values set.\n\r", ch );
                return TRUE;
            default:	/* should never get here */
                send_to_char( "Object values set.\n\r", ch );
                return TRUE;
        }
    }

    return FALSE;
}


bool
oedit_condition( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    int		    value;

    if ( argument[0] != '\0'
    && is_number( argument )
    && ( value = atoi (argument ) ) >= 0
    && ( value <= 100 ) )
    {
	EDIT_OBJ( ch, pObj );

	pObj->condition = value;
	send_to_char( "Condition set.\n\r", ch );

	return TRUE;
    }

    send_to_char( "Syntax:  condition [number]\n\r"
		  "Where number can range from 0 (ruined) to 100 (perfect).\n\r",
		  ch );
    return FALSE;
}


bool
oedit_copy( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    OBJ_INDEX_DATA *	pSrc;
    EXTRA_DESCR_DATA *	pEd;
    EXTRA_DESCR_DATA *	pEd_next;
    EXTRA_DESCR_DATA *	pSrcEd;
    TEXT_DATA *		page;
    TEXT_DATA *		pSrcPage;
    TEXT_DATA *		page_last;
    OREPROG_DATA *	pProg;
    OREPROG_DATA *	pSrcProg;
    OREPROG_DATA *	pLastProg;
    char		arg[MAX_INPUT_LENGTH];
    int			i;

    EDIT_OBJ( ch, pObj );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  copy <obj_vnum> (progs)\n\r", ch );
	return FALSE;
    }

    if ( ( pSrc = get_obj_index( atoi( arg ) ) ) == NULL )
    {
	send_to_char( "OEdit:  Object vnum does not exist.\n\r", ch );
	return FALSE;
    }

    if ( pSrc == pObj )
    {
	send_to_char( "OEdit:  Cannot copy an object onto itself.\n\r", ch );
	return FALSE;
    }

    if ( IS_SET( pSrc->area->area_flags, AREA_PROTOTYPE )
    &&  !IS_BUILDER( ch, pSrc->area ) )
    {
	send_to_char( "You may not copy objects from that area.\n\r", ch );
	return FALSE;
    }

    free_string( pObj->name );
    pObj->name = str_dup( pSrc->name );
    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( pSrc->short_descr );
    free_string( pObj->description );
    pObj->description = str_dup( pSrc->description );

    pObj->material	= pSrc->material;
    pObj->item_type	= pSrc->item_type;
    pObj->extra_flags	= pSrc->extra_flags;
	pObj->extra_flags2	= pSrc->extra_flags2;
    pObj->wear_flags	= pSrc->wear_flags;
    pObj->race_flags	= pSrc->race_flags;
    pObj->class_flags	= pSrc->class_flags;
    pObj->level		= pSrc->level;
    pObj->condition	= pSrc->condition;
    pObj->weight	= pSrc->weight;
    pObj->cost		= pSrc->cost;
    for ( i = 0; i < 6; i++ )
	pObj->value[i]	= pSrc->value[i];
    if ( pSrc->ac_type == INVOKE_SPELL )
    {
	pObj->ac_type	= pSrc->ac_type;
	pObj->ac_vnum	= pSrc->ac_vnum;
	pObj->ac_charge[0] = pSrc->ac_charge[0];
	pObj->ac_charge[1] = pSrc->ac_charge[1];
    }

    /* free up any existing extra descs */
    for ( pEd = pObj->extra_descr; pEd; pEd = pEd_next )
    {
	pEd_next = pEd->next;
	free_extra_descr( pEd );
    }
    pObj->extra_descr = NULL;

    for ( pSrcEd = pSrc->extra_descr; pSrcEd; pSrcEd = pSrcEd->next )
    {
	pEd			= new_extra_descr( );
	pEd->keyword		= str_dup( pSrcEd->keyword );
	pEd->description	= str_dup( pSrcEd->description );
	pEd->next		= pObj->extra_descr;
	pObj->extra_descr	= pEd;
    }

    /* free up any existing pages */
    while ( pObj->page != NULL )
    {
        page = pObj->page;
        pObj->page = page->next;
        free_textlist( page );
    }

    if ( pSrc->item_type == ITEM_BOOK )
    {
        page_last = NULL;
        for ( pSrcPage = pSrc->page; pSrcPage != NULL; pSrcPage = pSrcPage->next )
        {
            page = new_textlist( );
            page->text = str_dup( pSrcPage->text );
            if ( page_last == NULL )
                pObj->page = page;
            else
                page_last->next = page;
            page_last = page;
        }
    }

    if ( LOWER( *argument ) == 'p' && !str_prefix( argument, "progs" ) )
    {
        if ( pSrc->oprogs == NULL )
        {
            send_to_char( "OEdit:  Source object has no progs.\n\r", ch );
        }
        else
        {
            /* Fix compiler warning; code is correct. */
            pLastProg = NULL;

            /* Clear existing progs */
            while ( ( pProg = pObj->oprogs ) != NULL )
            {
                pObj->oprogs = pProg->next;
                free_oreprog_data( pProg );
            }

            /* Copy the progs, preserving order. */
            for ( pSrcProg = pSrc->oprogs; pSrcProg != NULL; pSrcProg = pSrcProg->next )
            {
                pProg = new_oreprog_data( );
                pProg->type = pSrcProg->type;
                pProg->arglist = str_dup( pSrcProg->arglist );
                pProg->comlist = str_dup( pSrcProg->comlist );
                pProg->triggered = pSrcProg->triggered;

                if ( pObj->oprogs == NULL )
                    pObj->oprogs = pProg;
                else
                    pLastProg->next = pProg;
                pLastProg = pProg;
            }
            pObj->progtypes = pSrc->progtypes;
        }
    }

    oedit_show( ch, "" );
    return TRUE;
}


bool
oedit_cost( CHAR_DATA *ch, char *argument )
{
    MONEY	    money;
    OBJ_INDEX_DATA *pObj;
    int		    value;

    EDIT_OBJ( ch, pObj );

    if ( money_value( &money, argument ) == NULL )
    {
	send_to_char(  "Syntax:  cost [amount [amount]]\n\r", ch );
	ch_printf( ch, "Amount is a number, followed by one or more of %s, %s,\n\r",
                   GOLD_PLURAL, SILVER_PLURAL );
	ch_printf( ch, "%s or %s.\n\r", COPPER_PLURAL, FRACT_PLURAL );
	ch_printf( ch, "Sample amounts: 3 %s\n\r", COPPER_PLURAL );
	ch_printf( ch, "                2 %s 3 %s\n\r", COPPER_PLURAL, FRACT_PLURAL );
	ch_printf( ch, "                2%c3%c\n\r", COPPER_INITIAL, FRACT_INITIAL );
	ch_printf( ch, "                3 %c 2%s\n\r", COPPER_INITIAL, SILVER_PLURAL );
	ch_printf( ch, "                3   (will default to %s)\n\r", FRACT_PLURAL );
	return FALSE;
    }

    if ( money.gold  < 0 || money.silver < 0
    || money.copper  < 0 || money.fract  < 0 )
    {
	send_to_char( "OEdit:  Cost must be a non-negative number.\n\r", ch );
	return FALSE;
    }

    /* Truncate non-adjacent denominations */
    if ( money.gold != 0 )
    {
	money.copper = 0;
	money.fract = 0;
    }
    if ( money.silver != 0 )
	money.fract = 0;

    value = money.gold    * FRACT_PER_COPPER * COPPER_PER_SILVER * SILVER_PER_GOLD
	  + money.silver * FRACT_PER_COPPER * COPPER_PER_SILVER
	  + money.copper   * FRACT_PER_COPPER
	  + money.fract;

    pObj->cost = value;

    send_to_char( "Cost set.\n\r", ch );
    return TRUE;
}


bool
oedit_qcost( CHAR_DATA *ch, char *argument )
{
   // OBJ_INDEX_DATA *pObj;
 //   int		    value;

  OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  qcost [number]\n\r", ch );
	return FALSE;
    }

    pObj->qcost = atoi( argument );

    send_to_char( "Cost set.\n\r", ch);
    return TRUE;
}

bool
oedit_create( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    AREA_DATA *		pArea;
    int			value;
    int			iHash;

    if ( argument[ 0 ] == '\0' )
    {
	pArea = ch->in_room->area;
	for ( value = pArea->min_vnum; value <= pArea->max_vnum; value++ )
	    if ( !get_obj_index( value ) )
		break;
	if ( value > pArea->max_vnum )
	{
	    send_to_char( "OEdit:  No free object vnums in this area.\n\r", ch );
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
	send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_obj_index( value ) )
    {
	send_to_char( "OEdit:  Object vnum already exists.\n\r", ch );
	return FALSE;
    }

    pObj			= new_obj_index();
    pObj->vnum			= value;
    pObj->area			= pArea;
    if ( value > top_vnum_obj )
	top_vnum_obj = value;
    pObj->condition		= 100;

    iHash			= value % MAX_KEY_HASH;
    pObj->next			= obj_index_hash[iHash];
    obj_index_hash[iHash]	= pObj;
    ch->desc->pEdit		= (void *)pObj;
    ch->pcdata->last_oedit	= pObj->vnum;

    send_to_char( "Object Created.\n\r", ch );
    return TRUE;
}


bool
oedit_delaffect( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    AFFECT_DATA *	pAf;
    AFFECT_DATA *	pAf_next;
    char		affect[MAX_INPUT_LENGTH];
    int			value;
    int			cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument( argument, affect );

    if ( !is_number( affect ) || affect[0] == '\0' )
    {
	send_to_char( "Syntax:  delaffect [#affect]\n\r", ch );
	return FALSE;
    }

    value = atoi( affect );

    if ( value < 0 )
    {
	send_to_char( "Only non-negative affect-numbers allowed.\n\r", ch );
	return FALSE;
    }

    if ( !( pAf = pObj->affected ) )
    {
	send_to_char( "OEdit:  No such affect.\n\r", ch );
	return FALSE;
    }

    if( value == 0 )	/* First case: Remove first affect */
    {
	pAf = pObj->affected;
	pObj->affected = pAf->next;
	free_affect( pAf );
    }
    else		/* Affect to remove is not the first */
    {
	while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
	     pAf = pAf_next;

	if( pAf_next )		/* See if it's the next affect */
	{
	    pAf->next = pAf_next->next;
	    free_affect( pAf_next );
	}
	else                                 /* Doesn't exist */
	{
	     send_to_char( "No such affect.\n\r", ch );
	     return FALSE;
	}
    }

    send_to_char( "Affect removed.\n\r", ch );
    return TRUE;
}


bool
oedit_delete( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *	d;
    char		logbuf[MAX_INPUT_LENGTH];
    AREA_DATA *		pArea;
    OBJ_DATA *		pEdit;
    OBJ_INDEX_DATA *	pObjIndex;
    RESET_DATA *	pReset;
    ROOM_INDEX_DATA *	pRoom;
    int			iHash;
    int			vnum;

    argument = one_argument( argument, arg );

    if ( !str_match( arg, "obj", "object" ) || !is_number( argument ) )
    {
        send_to_char( "Syntax:  delete object <vnum>\n\r", ch );
        return FALSE;
    }

    vnum = atoi( argument );
    if ( vnum < 1 || vnum > MAX_VNUM )
    {
        ch_printf( ch, "OEdit:  Vnum must be between 1 and %d.\n\r", MAX_VNUM );
        return FALSE;
    }

    if ( ( pArea = get_vnum_area( vnum ) ) == NULL )
    {
        send_to_char( "OEdit:  That vnum is not assigned to an area.\n\r", ch );
        return FALSE;
    }

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
        send_to_char( "OEdit:  That mob vnum does not exist.\n\r", ch );
        return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "OEdit:  That vnum is in an area you cannot build in.\n\r", ch );
        return FALSE;
    }

    if ( pArea == get_vnum_area( ROOM_VNUM_LIMBO ) && !IS_CODER( ch ) )
    {
	send_to_char( "OEdit:  Only Coders may delete objects in Limbo.\n\r", ch );
	return FALSE;
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pRoom = room_index_hash[iHash]; pRoom != NULL; pRoom = pRoom->next )
        {
            for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
            {
                if ( pReset->arg1 == vnum
                && ( pReset->command == 'O'
                  || pReset->command == 'P'
                  || pReset->command == 'E'
                  || pReset->command == 'G' ) )
                {
                    ch_printf( ch, "OEdit:  Object is reset in room #%d.  Remove it first.\n\r", pRoom->vnum );
                    return FALSE;
                }
            }
        }
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING
        &&   d != ch->desc
        &&   ( d->pEdit == pObjIndex || d->inEdit == pObjIndex ) )
        {
            send_to_char( "OEdit:  Somebody else is editing that object.\n\r", ch );
            return FALSE;
        }
        if ( d->connected == CON_PLAYING
        &&   d->editor == RENAME_OBJECT
        &&   ( pEdit = d->pEdit ) != NULL
        &&   pEdit->pIndexData == pObjIndex )
        {
            send_to_char( "OEdit:  Somebody is editing an instance of that object.\n\r", ch );
            return FALSE;
        }
    }

    if ( ch->desc->inEdit == pObjIndex )
        edit_done( ch );
    if ( ch->desc->pEdit == pObjIndex )
        edit_done( ch );

    sprintf( logbuf, "Oedit_delete: %s deleting object #%d.", ch->name, vnum );
    wiznet( "$t", ch, logbuf, WIZ_BUILD, 0, get_trust( ch ) );
    log_string( logbuf );
    delete_object( pObjIndex );
    send_to_char( "Object deleted.\n\r", ch );

    return TRUE;;
}


bool
oedit_description( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    EXTRA_DESCR_DATA *	ed;
    char		key[MAX_INPUT_LENGTH];
    char		cmd[MAX_INPUT_LENGTH];

    EDIT_OBJ( ch, pObj );

    sprintf( key, "'%s'", pObj->name );

    for ( ed = pObj->extra_descr; ed != NULL; ed = ed->next )
    {
        if ( !str_cmp( pObj->name, ed->keyword ) )
            break;
    }

    if ( !str_cmp( argument, "show" ) )
        sprintf( cmd, "show %s", key );
    else if ( ed != NULL )
        sprintf( cmd, "edit %s", key );
    else
        sprintf( cmd, "add %s", key );

    return oedit_ed( ch, cmd );
}


bool
oedit_duration( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    int		    value;

    EDIT_OBJ( ch, pObj );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  duration [number]\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < 0 )
    {
	ch_printf( ch, "OEdit:  Duration timer must be 0 or positive.\n\r" );
	return FALSE;
    }

    pObj->duration = value;

    send_to_char( value == 0 ? "Duration timer cleared.\n\r" : "Duration timer set.\n\r", ch );
    return TRUE;
}


bool
oedit_ed( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    EXTRA_DESCR_DATA *	ed;
    char		command[MAX_INPUT_LENGTH];
    char		keyword[MAX_INPUT_LENGTH];
    char		keybuf[MAX_INPUT_LENGTH];

    EDIT_OBJ( ch, pObj );

    argument = one_argument( argument, command );
    argument = one_argument( argument, keyword );

    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	send_to_char( "         ed delete [keyword]\n\r", ch );
	send_to_char( "         ed edit [keyword]\n\r", ch );
	send_to_char( "         ed show   [keyword]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	    return FALSE;
	}

	strcpy( keybuf, keyword );
	while ( *argument )
	{
	    argument = one_argument( argument, keyword );
	    strcat( keybuf, " " );
	    strcat( keybuf, keyword );
	}
	ed                  =   new_extra_descr();
	ed->keyword         =   str_dup( keybuf );
	ed->next            =   pObj->extra_descr;
	pObj->extra_descr   =   ed;

	string_append( ch, &ed->description );

	return TRUE;
    }

    if ( !str_cmp( command, "edit" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	string_append( ch, &ed->description );

	return TRUE;
    }

    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	if ( !ped )
	    pObj->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr( ed );

	send_to_char( "Extra description deleted.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "show" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed show [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	page_to_char( ed->description, ch );
	return FALSE;
    }

    oedit_ed( ch, "" );
    return FALSE;
}


bool
oedit_invoke( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    char		arg[MAX_INPUT_LENGTH];
    int			value;

    EDIT_OBJ( ch, pObj );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' || *argument == '\0' )
    {
	send_to_char( "Syntax:  invoke type <type>\n\r", ch );
	send_to_char( "         invoke vnum <vnum>\n\r", ch );
	send_to_char( "         invoke spell <spell>\n\r", ch );
	send_to_char( "         invoke charges <max_charges>\n\r", ch );
	return FALSE;
    }

    if ( !str_prefix( arg, "charges" ) )
    {
	if ( !pObj->ac_type )
	{
	    send_to_char( "OEdit:  Invoke type not set.\n\r", ch );
	    return FALSE;
	}
	if ( !is_number( argument ) )
	{
	    send_to_char( "OEdit:  Invoke charges must be numeric.\n\r", ch );
	    return FALSE;
	}
	pObj->ac_charge[1] = pObj->ac_charge[0] = atoi( argument );
	send_to_char( "Charges set.\n\r", ch );
	return TRUE;	
    }
    else if ( !str_prefix( arg, "spell" ) )
    {
	if ( pObj->ac_type != INVOKE_SPELL )
	{
	    send_to_char( "OEdit:  Invoke type is not \"spell\".\n\r", ch );
	    return FALSE;
	}
	if ( ( value = skill_lookup( argument ) ) == NO_SKILL
		|| skill_table[value].spell_fun == spell_null )
	{
	    send_to_char( "OEdit:  No such spell.\n\r", ch );
	    return FALSE;
	}
	if ( IS_SET( skill_table[value].spell_flags, SPELL_NOINVOKE ) )
	{
	    send_to_char( "That spell cannot be invoked.\n\r", ch );
	    return FALSE;
	}
	pObj->ac_vnum = value;
	send_to_char( "Invoke spell set.\n\r", ch );
	return TRUE;
    }
    else if ( !str_prefix( arg, "type" ) )
    {
	if ( ( value = flag_value( invoke_types, argument ) ) == NO_FLAG )
	{
	    send_to_char( "OEdit:  Invalid invoke type.\n\r", ch );
	    return FALSE;
	}
	if ( value == INVOKE_SPELL &&
	    ( pObj->item_type == ITEM_SCROLL
	      || pObj->item_type == ITEM_WAND
	      || pObj->item_type == ITEM_STAFF
	      || pObj->item_type == ITEM_LENS
	      || pObj->item_type == ITEM_HERB
	      || pObj->item_type == ITEM_POTION
	      || pObj->item_type == ITEM_PILL ) )
	{
	    send_to_char( "OEdit:  That object type cannot invoke spells.\n\r", ch );
	    return FALSE;
	}
	pObj->ac_type = value;
	pObj->ac_vnum = 0;
	if ( value )
	    send_to_char( "Invoke type set.\n\r", ch );
	else
	    send_to_char( "Invoke type removed.\n\r", ch );
	return TRUE;
    }
    else if ( !str_prefix( arg, "vnum" ) )
    {
	if ( !pObj->ac_type )
	{
	    send_to_char( "OEdit:  Invoke type not set.\n\r", ch );
	    return FALSE;
	}
	if ( pObj->ac_type == INVOKE_SPELL )
	{
	    send_to_char( "OEdit:  Invoke type is \"spell\"; cannot set vnum\n\r", ch );
	    return FALSE;
	}
	if ( !is_number( argument ) || ( value = atoi( argument ) ) < 1
		|| value > MAX_VNUM )
	{
	    send_to_char( "OEdit:  Bad invoke vnum.\n\r", ch );
	    return FALSE;
	}
	switch( pObj->ac_type )
	{
	    case INVOKE_MOB:
		if ( !get_mob_index( value ) )
		{
		    send_to_char( "OEdit:  Mobile does not exist.\n\r", ch );
		    return FALSE;
		}
		break;
	    case INVOKE_OBJ:
		if ( !get_obj_index( value ) )
		{
		    send_to_char( "OEdit:  Object does not exist.\n\r", ch );
		    return FALSE;
		}
		if ( value == pObj->vnum )
		{
		    send_to_char( "OEdit:  Object cannot invoke itself.\n\r", ch );
		    return FALSE;
		}
		break;
	    case INVOKE_TRANS:
		if ( !get_room_index( value ) )
		{
		    send_to_char( "OEdit:  Room does not exist.\n\r", ch );
		    return FALSE;
		}
		break;
	    case INVOKE_MORPH:
		if ( !get_obj_index( value ) )
		{
		    send_to_char( "OEdit:  Object does not exist.\n\r", ch );
		    return FALSE;
		}
		if ( value == pObj->vnum )
		{
		    send_to_char( "OEdit:  Object cannot morph into itself.\n\r", ch );
		    return FALSE;
		}
		break;
	}
        pObj->ac_vnum = value;
        send_to_char( "Invoke vnum set.\n\r", ch );
        return TRUE;
    }
    else
	return ( oedit_invoke( ch, "" ) );
}


bool
oedit_join( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    char		arg[MAX_STRING_LENGTH];
    int			value = 0;

    EDIT_OBJ( ch, pObj );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( " Syntax: join [vnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( arg );

    if ( value < 0 || value > MAX_VNUM )
    {
	send_to_char( "Invalid vnum.\n\r", ch );
	return FALSE;
    }

    pObj->join = value; 
    if ( value )
	send_to_char( "Join object set.\n\r", ch );
    else
	send_to_char( "Join object removed.\n\r", ch );
    return TRUE;  
}


bool
oedit_level( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    int		    value;

    EDIT_OBJ( ch, pObj );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < 1 || value > MAX_LEVEL )
    {
	ch_printf( ch, "OEdit:  Level must be between 1 and %d.\n\r", MAX_LEVEL );
	return FALSE;
    }

    pObj->level = value;

    send_to_char( "Level set.\n\r", ch );
    return TRUE;
}


bool
oedit_long( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    char *p;

    EDIT_OBJ( ch, pObj );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pObj->description, argument ) ) != NULL )
    {
	*p = UPPER( *p  );
	pObj->description = p;
	send_to_char( "Long description set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;
}


bool
oedit_material( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    int		    value;

    if ( argument[0] != '\0' )
    {
	EDIT_OBJ( ch, pObj );

	if ( ( value = flag_value( material_types, argument ) ) != NO_FLAG )
	{
	    xTOGGLE_BIT( pObj->material, value );
	    send_to_char( "Material type set.\n\r", ch);
	    return TRUE;
	}
    }

    send_to_char( "Syntax:  material [material-name]\n\r"
		  "Type '? material' for a list of materials.\n\r", ch );
    return FALSE;
}


bool
oedit_name( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    EXTRA_DESCR_DATA *	ed;
    char *		p;
    char		old_name[MAX_INPUT_LENGTH];

    EDIT_OBJ( ch, pObj );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }

    strcpy( old_name, pObj->name );
    if ( ( p = string_change( ch, pObj->name, argument ) ) != NULL )
    {
	pObj->name = p;
	for ( ed = pObj->extra_descr; ed != NULL; ed = ed->next )
	    if ( !str_cmp( pObj->name, ed->keyword ) )
	        break;
	if ( ed == NULL )
	{
	    for ( ed = pObj->extra_descr; ed != NULL; ed = ed->next )
                if ( !str_cmp( old_name, ed->keyword ) )
                    break;
            if ( ed != NULL )
            {
                free_string( ed->keyword );
                ed->keyword = str_dup( pObj->name );
            }
        }
	send_to_char( "Name set.\n\r", ch );
	return TRUE;
    }
    return FALSE;
}


bool
oedit_opedit( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *	pProg;
    OBJ_INDEX_DATA *	pObj;
    char		command[MAX_INPUT_LENGTH];

    EDIT_OBJ( ch, pObj );
    argument = one_argument( argument, command );

    if ( is_number( command ) )
    {
	if ( ( pProg = get_oprog_data( pObj, atoi( command ) ) ) == NULL )
	{
	    send_to_char( "OPEdit:  Object has no such ObjProg.\n\r", ch );
	    return FALSE;
	}
	ch->desc->editin = ch->desc->editor;
	ch->desc->editor = ED_OPROG;
	ch->desc->inEdit = ch->desc->pEdit;
	ch->desc->pEdit  = (void *)pProg;
	opedit_show( ch, "" );
	return FALSE;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	ch->desc->editin = ch->desc->editor;
	ch->desc->editor = ED_OPROG;
	if ( opedit_create( ch, argument ) )
	{
	    opedit_show( ch, "" );
	    return TRUE;
	}
	ch->desc->editor = ch->desc->editin;
	ch->desc->editin = 0;
	return FALSE;
    }

    if ( command[0] == '\0'
    &&	 pObj != NULL
    &&	 pObj->oprogs != NULL
    &&	 pObj->oprogs->next == NULL )
    {
        return oedit_opedit( ch, "0" );
    }

    send_to_char( "OPEdit:  There is no default ObjProg to edit.\n\r", ch );
    return FALSE;
}


bool
oedit_oplist( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *	pProg;
    OBJ_INDEX_DATA *	pObj;
    BUFFER *		pBuf;
    int			value;

    EDIT_OBJ( ch, pObj );
    pBuf = new_buf( );
    value = 0;

    for ( pProg = pObj->oprogs; pProg; pProg = pProg->next )
    {
	buf_printf( pBuf, "[%2d] (%12s)  %s\n\r", value,
		flag_string( oprog_types, pProg->type ), pProg->arglist );
	value++;
    }

    if ( !value )
	add_buf( pBuf, "This object has no ObjProgs.\n\r" );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}


bool
oedit_opremove( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *	pProg;
    OREPROG_DATA *	pPrev;
    OBJ_INDEX_DATA *	pObj;
    int			value;
    int			vnum;

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  opremove #", ch );
	return FALSE;
    }

    EDIT_OBJ( ch, pObj );
    value = 0;
    vnum = atoi( argument );

    if ( !pObj->oprogs )
    {
	send_to_char( "No such ObjProg.\n\r", ch );
	return FALSE;
    }

    pPrev = NULL;
    for ( pProg = pObj->oprogs; value < vnum; value++ )
    {
	pPrev = pProg;
	pProg = pProg->next;
	if ( pProg == NULL )
	{
	    send_to_char( "No such ObjProg.\n\r", ch );
	    return FALSE;
	}
    }

    if ( !pPrev )
	pObj->oprogs = pObj->oprogs->next;
    else
	pPrev->next = pProg->next;

    free_oreprog_data( pProg );
    pObj->progtypes = 0;
    for ( pProg = pObj->oprogs; pProg; pProg = pProg->next )
	SET_BIT( pObj->progtypes, pProg->type );
    send_to_char( "Ok.\n\r", ch );
    return TRUE;
}


bool
oedit_page( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    TEXT_DATA *		page;
    TEXT_DATA *		pPrev;
    char		arg[MAX_INPUT_LENGTH];
    int			count;
    int			value;

    EDIT_OBJ( ch, pObj );

    if ( pObj->item_type != ITEM_BOOK )
    {
        ch_printf( ch, "OEdit:  %s`X is not a book.\n\r", pObj->short_descr );
        return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp( arg, "add" ) )
    {
        page = new_textlist( );
        if ( pObj->page == NULL )
            pObj->page = page;
        else
        {
            for ( pPrev = pObj->page; pPrev->next != NULL; pPrev = pPrev->next )
                ;
            pPrev->next = page;
        }
        string_append( ch, &page->text );
        return TRUE;
    }

    if ( !is_number( argument ) || ( value = atoi( argument ) ) < 1 )
    {
        send_to_char( "OEdit:  Bad page number.\n\r", ch );
        return FALSE;
    }

    count = 0;
    for ( page = pObj->page; page != NULL; page = page->next )
        count++;
    if ( count == 0 )
    {
        send_to_char( "OEdit:  No pages to modify or insert before.\n\r", ch );
        return FALSE;
    }
    else if ( value > count )
    {
        ch_printf( ch, "The object only has %d page%s.\n\r", count, count == 1 ? "" : "s" );
        return FALSE;
    }

    if ( str_match( arg, "del", "delete" ) )
    {
        if ( value == 1 )
        {
            page = pObj->page;
            pObj->page = page->next;
        }
        else
        {
            count = 1;
            for ( pPrev = pObj->page; pPrev != NULL; pPrev = pPrev->next )
                if ( ++count == value )
                    break;

            if ( pPrev == NULL )
            {
                bugf( "Page delete: Can't find page %d on #%d", value, pObj->vnum );
                return FALSE;
            }

            page = pPrev->next;
            pPrev->next = page->next;
        }

        free_textlist( page );
        send_to_char( "Page deleted.\n\r", ch );
        return TRUE;
    }

    if ( !str_prefix( arg, "edit" ) )
    {
        count = 0;
        for ( page = pObj->page; page != NULL; page = page->next )
            if ( ++count == value )
                break;

        if ( page == NULL )
        {
            bugf( "Page edit: Can't find page %d on #%d", value, pObj->vnum );
            return FALSE;
        }

        string_append( ch, &page->text );
        return TRUE;
    }

    if ( !str_prefix( arg, "insert" ) )
    {
        page = new_textlist( );
        if ( value == 1 )
        {
            page->next = pObj->page;
            pObj->page = page;
        }
        else
        {
            count = 1;
            for ( pPrev = pObj->page; pPrev != NULL; pPrev = pPrev->next )
                if ( ++count == value )
                    break;

            if ( pPrev == NULL )
            {
                bugf( "Page insert: Can't find page %d on #%d", value, pObj->vnum );
                return FALSE;
            }

            page->next = pPrev->next;
            pPrev->next = page;
        }

        string_append( ch, &page->text );
        return TRUE;
    }

    send_to_char( "Syntax:  page (or page add):  Add new page to end of book.\n\r", ch );
    send_to_char( "         page delete <#num>:  Delete page #num.\n\r", ch );
    send_to_char( "         page edit <#num>:    Edit page #num.\n\r", ch );
    send_to_char( "         page insert <#num>:  Insert new page before page #num.\n\r", ch );

    return FALSE;
}


bool
oedit_sepone( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    char		arg[MAX_INPUT_LENGTH];
    int			value;

    EDIT_OBJ( ch, pObj );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
	send_to_char( "Syntax:  sepone [vnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( arg );
    if ( value < 0 || value > MAX_VNUM )
    {
	send_to_char( "Invalid vnum.\n\r", ch );
	return FALSE;
    }

    pObj->sep_one = value;

    if ( value )
	send_to_char( "First separated object set.\n\r", ch );
    else
	send_to_char( "First separated object removed.\n\r", ch );
    return TRUE;
}


bool
oedit_septwo( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    char		arg[MAX_INPUT_LENGTH];
    int			value;

    EDIT_OBJ( ch, pObj );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	send_to_char( "Syntax: septwo [vnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( arg );
    if ( value < 0 || value > MAX_VNUM )
    {
	send_to_char( "Invalid vnum.\n\r", ch );
	return FALSE;
    }

    pObj->sep_two = value;
    if ( value )
	send_to_char( "Second separated object set.\n\r", ch );
    else
	send_to_char( "Second separated object removed.\n\r", ch );
    return TRUE;
}


bool
oedit_short( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    char *p;

    EDIT_OBJ( ch, pObj );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }

    if ( ( p = string_change( ch, pObj->short_descr, argument ) ) != NULL )
    {
	/* *p = LOWER( *p ); */
	pObj->short_descr = p;
	send_to_char( "Short description set.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;

}

bool
oedit_autoweapon( CHAR_DATA *ch, char *argument)
{
	OBJ_INDEX_DATA *pObj;
	AFFECT_DATA *pAf;
	int dice, size, bonus;
	double avg;

	EDIT_OBJ(ch, pObj);
	if (pObj->item_type != ITEM_WEAPON)
	{
		 send_to_char( " `rAutoweapn only works on weapons...`X\n\r", ch);
	return FALSE;
	}
	if (pObj->level < 1)
	{
		send_to_char( " `cAutoweapon requires a level to be set on the weapon first.`X\n\r", ch);
		return FALSE;
	}
   bonus = UMAX(0, pObj->level/10 - 1);
/* adjust this next line to change the avg dmg your weapons will get! */
	avg = (pObj->level * .76);
	dice = (pObj->level/10 + 1);
	size = dice/2;
/* loop through dice sizes until we find that the Next dice size's avg
will be too high... ie, find the "best fit" */
	for (size=dice/2 ; dice * (size +2)/2 < avg ; size++ )
	{ }

	dice = UMAX(1, dice);
	size = UMAX(2, size);

	switch (pObj->value[0]) {
	default:
	case WEAPON_EXOTIC:
	case WEAPON_SWORD:
		break;
	case WEAPON_DAGGER:
		dice = UMAX(1, dice - 1);
		size = UMAX(2, size - 1);
	  break;
	case WEAPON_SPEAR:
	case WEAPON_POLEARM:
		size++;
		break;
	case WEAPON_MACE:
	case WEAPON_AXE:
		size = UMAX(2,size - 1);
		break;
	case WEAPON_FLAIL:
	case WEAPON_WHIP:
		dice = UMAX(1, dice - 1);
		break;
	}
	dice = UMAX(1, dice);
	size = UMAX(2, size);
	
	
	pObj->cost = 25 * (size * (dice + 1)) + 20 * bonus + 20 * pObj->level;
	pObj->weight = pObj->level + 1;
	pObj->value[1] = dice;
pObj->value[2] = size;
if (bonus > 0) {
pAf             =   new_affect();
    pAf->location   =   APPLY_DAMROLL;
    pAf->modifier   =   bonus;
    pAf->where	    =   TO_OBJECT;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =	pObj->level;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;

pAf             =   new_affect();
    pAf->location   =   APPLY_HITROLL;
    pAf->modifier   =   bonus;
    pAf->where	    =   TO_OBJECT;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =	pObj->level;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;
}
send_to_char(" `cExperimental values set on weapon...`X\n\r", ch);
return TRUE;
}

bool
oedit_autoarmor( CHAR_DATA *ch, char *argument)
{
   OBJ_INDEX_DATA *pObj;
	int size;

	EDIT_OBJ(ch, pObj);
	if (pObj->item_type != ITEM_ARMOR)
	{
		 send_to_char( " `rAutoArmor only works on Armor ...`X\n\r", ch);
	return FALSE;
	}
	if (pObj->level < 1)
	{
		send_to_char( " `cAutoArmor requires a level to be set on the armor first.`X\n\r", ch);
		return FALSE;
	}
	size = UMAX(1, pObj->level/2.8 + 1);
	pObj->weight = pObj->level + 1;
pObj->cost = pObj->level^2 * 2;	
pObj->value[0] = size;
	pObj->value[1] = size;
	pObj->value[2] = size;
		pObj->value[3] = (size - 1);
		send_to_char( " `cAutoArmor has set experimental values for AC.`X\n\r", ch);
		return TRUE;
}



bool
oedit_show( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    BUFFER *		pBuf;

    EDIT_OBJ( ch, pObj );

    pBuf = new_buf( );
    show_obj_info( ch, pObj, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return FALSE;
}


bool
oedit_timer( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    int		    value;

    EDIT_OBJ( ch, pObj );

    if ( !is_number( argument ) )
    {
	send_to_char( "Syntax:  timer [number]\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );
    if ( value < 0 )
    {
	ch_printf( ch, "OEdit:  Timer must be 0 or positive.\n\r" );
	return FALSE;
    }

    pObj->timer = value;

    send_to_char( value == 0 ? "Timer cleared.\n\r" : "Timer set.\n\r", ch );
    return TRUE;
}


bool
oedit_value0( CHAR_DATA *ch, char *argument )
{
    return set_value( ch, argument, 0 );
}

bool
oedit_value1( CHAR_DATA *ch, char *argument )
{
    return set_value( ch, argument, 1 );
}

bool
oedit_value2( CHAR_DATA *ch, char *argument )
{
    return set_value( ch, argument, 2 );
}

bool
oedit_value3( CHAR_DATA *ch, char *argument )
{
    return set_value( ch, argument, 3 );
}

bool
oedit_value4( CHAR_DATA *ch, char *argument )
{
    return set_value( ch, argument, 4 );
}

bool
oedit_value5( CHAR_DATA *ch, char *argument )
{
    return set_value( ch, argument, 5 );
}


bool
oedit_weight( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    int		    value;

    EDIT_OBJ( ch, pObj );

    if ( !weight_value( argument, &value ) )
    {
	send_to_char( "Syntax:  weight [number]\n\r", ch );
	return FALSE;
    }

    if ( value < 1 )
    {
	send_to_char( "OEdit:  Weight must be a positive number.\n\r", ch );
	return FALSE;
    }

    pObj->weight = value;

    send_to_char( "Weight set.\n\r", ch );
    return TRUE;
}


void
opedit( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *	pProg;
    OBJ_INDEX_DATA *	pObj;
    char		arg[MAX_INPUT_LENGTH];
    char		command[MAX_INPUT_LENGTH];
    int			cmd;
    bitvector		value;

    EDIT_OREPROG( ch, pProg );
    pObj = (OBJ_INDEX_DATA *)ch->desc->inEdit;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
	opedit_show( ch, "" );
	return;
    }

    if ( !str_cmp( command, "done" ) )
    {
	edit_done( ch );
	return;
    }

    /* Search table and dispatch command. */
    for ( cmd = 0; *opedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, opedit_table[cmd].name ) )
	{
	    if ( (*opedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pObj->area->area_flags, AREA_CHANGED );
	    return;
	}
    }

    if ( ( value = flag_value( oprog_types, arg ) ) != NO_FLAG )
    {
	pProg->type = value;
	pObj->progtypes = 0;

	for ( pProg = pObj->oprogs; pProg; pProg = pProg->next )
	    SET_BIT( pObj->progtypes, pProg->type );
	SET_BIT( pObj->area->area_flags, AREA_CHANGED );
	send_to_char( "ObjProg type set.\n\r",ch);
	return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}


bool
opedit_arglist( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *pProg;

    EDIT_OREPROG( ch, pProg );

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  arglist [string]\n\r", ch );
	return FALSE;
    }
    free_string( pProg->arglist );
    pProg->arglist = str_dup( argument );

    send_to_char( "Arglist set.\n\r", ch );
    return TRUE;
}


bool
opedit_comlist( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *pProg;

    EDIT_OREPROG( ch, pProg );

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pProg->comlist );
	return TRUE;
    }

    send_to_char( "Syntax:  comlist    - line edit\n\r", ch );
    return FALSE;
}


bool
opedit_create( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    OREPROG_DATA *	pProg;
    OREPROG_DATA *	pNext;

    EDIT_OBJ( ch, pObj );

    SET_BIT( pObj->progtypes, OBJ_PROG_DROP );
    pProg		= new_oreprog_data( );
    pProg->type		= OBJ_PROG_DROP;
    pProg->next		= NULL;

    if ( !pObj->oprogs )
    {
	pObj->oprogs = pProg;
    }
    else
    {
	for ( pNext = pObj->oprogs; pNext->next; pNext = pNext->next )
	    ;
	pNext->next = pProg;
    }

    ch->desc->inEdit = ch->desc->pEdit;
    ch->desc->pEdit = (void *)pProg;
    send_to_char( "ObjProg created.\n\r", ch );
    return TRUE;
}


bool
opedit_show( CHAR_DATA *ch, char *argument )
{
    OREPROG_DATA *	pProg;
    OBJ_INDEX_DATA *	pObj;
    BUFFER *		pBuf;

    EDIT_OREPROG( ch, pProg );
    pObj = (OBJ_INDEX_DATA *)ch->desc->inEdit;

    pBuf = new_buf( );
    buf_printf( pBuf, "[%5d] %s`X\n\r", pObj->vnum, pObj->short_descr );
    buf_printf( pBuf, "ObjProg type: %s\n\r", flag_string( oprog_types, pProg->type ) );
    buf_printf( pBuf, "Arguments: %s\n\r", pProg->arglist );
    buf_printf( pBuf, "Commands:\n\r%s", pProg->comlist );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return FALSE;
}


static OREPROG_DATA *
get_oprog_data( OBJ_INDEX_DATA *pObj, int vnum )
{
    OREPROG_DATA *	pProg;
    int			value;

    if ( pObj == NULL )
	return NULL;

    value = 0;
    for ( pProg = pObj->oprogs; pProg; pProg = pProg->next )
    {
	if ( value == vnum )
	    return pProg;
	value++;
    }

    return NULL;
}


bool
delete_object( OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_INDEX_DATA *	exObj;
    int			iHash;
    OBJ_DATA *		pObj;
    OBJ_DATA *		pObj_next;

    if ( pObjIndex == NULL )
        return FALSE;

    for ( pObj = object_list; pObj != NULL; pObj = pObj_next )
    {
        pObj_next = pObj->next;
        if ( pObj->pIndexData == pObjIndex )
        {
            extract_obj( pObj );
        }
    }

    iHash = pObjIndex->vnum % MAX_KEY_HASH;
    if ( obj_index_hash[iHash] == pObjIndex )
    {
        obj_index_hash[iHash] = pObjIndex->next;
    }
    else
    {
        for ( exObj = obj_index_hash[iHash]; exObj != NULL; exObj = exObj->next )
        {
            if ( exObj->next == pObjIndex )
            {
                exObj->next = pObjIndex->next;
                break;
            }
        }
    }

    return TRUE;
}


bool
set_obj_stats( OBJ_INDEX_DATA *pObj )
{
    int		index;
    int		maxtable;
    int		type;

    switch( pObj->item_type )
    {
	case ITEM_ARMOR:
	    SET_BIT( pObj->wear_flags, ITEM_TAKE );
	    /* look up values for v1, v2 and v5 */
	    for ( maxtable = 1; armor_values[maxtable].level != 0; maxtable++ )
		;
	    index = maxtable;
	    do
		index--;
	    while ( index >= 0 && armor_values[index].level > pObj->level );
	    pObj->value[0] = number_fuzzy( armor_values[index].value );
	    pObj->value[1] = number_fuzzy( armor_values[index].value );
	    pObj->value[2] = number_fuzzy( armor_values[index].value );
	    pObj->value[3] = number_fuzzy( armor_values[index].value ) / 2;
	    return TRUE;

	case ITEM_WEAPON:
            type = pObj->value[0];	/* use current class if set */

            /* look up weapon class in weapon_val_types */
            for ( index = 0; weapon_values[index].class != WEAPON_NONE; index++ )
                if ( weapon_values[index].class == type )
                    break;

            /* make sure we found something valid */
            if ( weapon_values[index].class == WEAPON_NONE )
                index = 0;
            pObj->value[0] = weapon_values[index].class;

            /* set number of sides from table plus 1 for every 5 levels */
            pObj->value[1] = weapon_values[index].v1 + pObj->level / 5;
            /* set other values right from table */
            pObj->value[2] = weapon_values[index].v2;
            pObj->value[3] = weapon_values[index].v3;
            pObj->value[5] = weapon_values[index].v5;
	    pObj->wear_flags = ITEM_TAKE | ITEM_WIELD;
	    return TRUE;

	default:
	    return FALSE;
    }
}


static bool
set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj,
			    int value_num, char *argument )
{
    BUFFER *	pBuf;
    char	buf[MAX_INPUT_LENGTH];
    int		sn;
    bitvector	bits;
    int		otype;
    int		value;

    otype = pObj->item_type;
    switch( otype )
    {
        default:
	    send_to_char( "That object type has no values to set.\n\r", ch );
	    return FALSE;

        case ITEM_LIGHT:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_LIGHT" );
	            return FALSE;
	        case 2:
		    send_to_char( "Hours of light set.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
	    }
	    break;

        case ITEM_WAND:
        case ITEM_LENS:
        case ITEM_STAFF:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_STAFF_WAND" );
	            return FALSE;
	        case 0:
	            send_to_char( "Spell level set.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            pObj->value[1] = atoi( argument );
	            if ( pObj->value[2] == 0 || pObj->value[2] > pObj->value[1] )
	            {
	                pObj->value[2] = pObj->value[1];
	                send_to_char( "Current and maximum charges set.\n\r", ch );
	            }
	            else
	            {
	                send_to_char( "Total number of charges set.\n\r\n\r", ch );
	            }
	            break;
	        case 2:
	            send_to_char( "Current number of charges set.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	        case 3:
		    sn = skill_lookup( argument );
		    if ( sn > 0 && skill_table[sn].spell_fun == spell_null )
		    {
			send_to_char( "That's a skill, not a spell.\n\r", ch );
			return FALSE;
		    }
		    bits = skill_table[sn].spell_flags;
		    if ( ( IS_SET( bits, SPELL_NOWAND  ) && otype == ITEM_WAND )
		    ||	 ( IS_SET( bits, SPELL_NOLENS  ) && otype == ITEM_LENS )
		    ||	 ( IS_SET( bits, SPELL_NOSTAFF ) && otype == ITEM_STAFF ) )
		    {
			send_to_char( "Object cannot cast that spell.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Spell type set.\n\r", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	    }
            break;

        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_SCROLL" );
	            return FALSE;
	        case 0:
	            send_to_char( "Spell level set.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	        case 2:
	        case 3:
	        case 4:
		    sn = skill_lookup( argument );
		    if ( sn > 0 && skill_table[sn].spell_fun == spell_null )
		    {
			send_to_char( "That's a skill, not a spell.\n\r", ch );
			return FALSE;
		    }
		    bits = skill_table[sn].spell_flags;
		    if ( ( IS_SET( bits, SPELL_NOSCROLL ) && otype == ITEM_SCROLL )
		    ||	 ( IS_SET( bits, SPELL_NOPOTION ) && otype == ITEM_POTION )
		    ||	 ( IS_SET( bits, SPELL_NOPILL   ) && otype == ITEM_PILL ) )
		    {
			send_to_char( "Object cannot cast that spell.\n\r", ch );
			return FALSE;
		    }
	            ch_printf( ch, "Spell type %d set.\n\r\n\r", value_num );
	            pObj->value[value_num] = skill_lookup( argument );
	            break;
/*	        case 2:
		    sn = skill_lookup( argument );
		    if ( sn > 0 && skill_table[sn].spell_fun == spell_null )
		    {
			send_to_char( "That's a skill, not a spell.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Spell type 2 set.\n\r\n\r", ch );
	            pObj->value[2] = skill_lookup( argument );
	            break;
	        case 3:
		    sn = skill_lookup( argument );
		    if ( sn > 0 && skill_table[sn].spell_fun == spell_null )
		    {
			send_to_char( "That's a skill, not a spell.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Spell type 3 set.\n\r\n\r", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	        case 4:
		    sn = skill_lookup( argument );
		    if ( sn > 0 && skill_table[sn].spell_fun == spell_null )
		    {
			send_to_char( "That's a skill, not a spell.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Spell type 4 set.\n\r\n\r", ch );
	            pObj->value[4] = skill_lookup( argument );
	            break;
*/
 	    }
	    break;

        case ITEM_HERB:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_HERB" );
	            return FALSE;
	        case 0:
	            send_to_char( "Spell level set.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
		    sn = skill_lookup( argument );
		    if ( sn > 0 && skill_table[sn].spell_fun == spell_null )
		    {
			send_to_char( "That's a skill, not a spell.\n\r", ch );
			return FALSE;
		    }
		    if ( IS_SET( skill_table[sn].spell_flags, SPELL_NOHERB ) )
		    {
			send_to_char( "Object cannot cast that spell.\n\r", ch );
			return FALSE;
		    }
		    send_to_char( "Spell when eaten set.\n\r\n\r", ch );
		    pObj->value[1] = skill_lookup( argument );
		    break;
		case 2:
		    sn = skill_lookup( argument );
		    if ( sn > 0 && skill_table[sn].spell_fun == spell_null )
		    {
			send_to_char( "That's a skill, not a spell.\n\r", ch );
			return FALSE;
		    }
		    if ( IS_SET( skill_table[sn].spell_flags, SPELL_NOHERB ) )
		    {
			send_to_char( "Object cannot cast that spell.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Spell when rubbed set.\n\r\n\r", ch );
	            pObj->value[2] = skill_lookup( argument );
	            break;
	        case 3:
		    sn = skill_lookup( argument );
		    if ( sn > 0 && skill_table[sn].spell_fun == spell_null )
		    {
			send_to_char( "That's a skill, not a spell.\n\r", ch );
			return FALSE;
		    }
		    if ( IS_SET( skill_table[sn].spell_flags, SPELL_NOHERB ) )
		    {
			send_to_char( "Object cannot cast that spell.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Spell when brewed set.\n\r\n\r", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	        case 5:
	            value = flag_value( herb_flags, argument );
	            if ( value == NO_FLAG )
	            {
			send_to_char( "OEdit:  Bad flag value.\n\r", ch );
			return FALSE;
	            }
	            send_to_char( "Poison flags toggled.\n\r", ch );
	            pObj->value[5] ^= value;
	            break;
 	    }
	    break;
         
        case ITEM_TOKEN:
       switch ( value_num )
       {
           default:
          do_help( ch, "ITEM_TOKEN" );
               return FALSE;
           case 0:
               send_to_char( "Quest point value set.\n\r\n\r", ch );
               pObj->value[0] = atoi( argument );
               break;
      }

            break;

/* ARMOR for ROM: */

        case ITEM_ARMOR:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_ARMOR" );
		    return FALSE;
	        case 0:
		    send_to_char( "AC Pierce set.\n\r\n\r", ch );
		    pObj->value[0] = atoi( argument );
		    break;
	        case 1:
		    send_to_char( "AC Bash set.\n\r\n\r", ch );
		    pObj->value[1] = atoi( argument );
		    break;
	        case 2:
		    send_to_char( "AC Slash set.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
	        case 3:
		    send_to_char( "AC Exotic set.\n\r\n\r", ch );
		    pObj->value[3] = atoi( argument );
		    break;
		case 4:
		    send_to_char( "Bulk set.\n\r\n\r", ch );
		    pObj->value[4] = atoi( argument );
		    break;
	    }
	    break;

/* WEAPONS changed in ROM */

        case ITEM_WEAPON:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_WEAPON" );
	            return FALSE;
	        case 0:
		    bits = flag_value( weapon_types, argument );
		    if ( bits == NO_FLAG )
		    {
			send_to_char( "OEdit:  Bad weapon class.\n\r", ch );
			return FALSE;
		    }
		    send_to_char( "Weapon class set.\n\r\n\r", ch );
		    pObj->value[0] = bits;
		    break;
	        case 1:
                    if ( atoi( argument ) < 1 || atoi( argument ) > 1500 )
                    {
                        send_to_char( "Number of Dice must be between 1 and 1500.\n\r", ch);
                        return FALSE;
                    }
                    else
                    {
                        send_to_char( "Number of dice set.\n\r\n\r", ch );
                        pObj->value[1] = atoi( argument );
                        break;
                    }
                    break;
	        case 2:
                    if ( atoi( argument ) < 1 || atoi( argument ) > 1500 )
                    {
                        send_to_char( "Type of Dice must be between 1 and 1500.\n\r", ch);
                        return FALSE;
                    }
                    else
                    {
                        send_to_char( "Type of dice set.\n\r\n\r", ch );
                        pObj->value[2] = atoi( argument );
                        break;
                    }
                    break;
	        case 3:
		    bits = flag_value( weapon_flags, argument );
		    if ( bits == NO_FLAG )
		    {
			send_to_char( "OEdit:  Bad weapon type.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Weapon type set.\n\r\n\r", ch );
	            pObj->value[3] = bits;
	            break;
	        case 4:
		    if ( !str_cmp( argument, "none" ) )
			value = 0;
		    else
		    {
			value = flag_value( weapon_type_olc, argument );
			if ( value == NO_FLAG )
			{
			    send_to_char( "OEdit:  Bad special type.\n\r", ch );
			    return FALSE;
			}
			TOGGLE_BIT( value, pObj->value[4] );
                    }
                    send_to_char( "Special weapon type set.\n\r\n\r", ch );
		    pObj->value[4] = value;
		    break;
		case 5:
                    if ( atoi( argument ) < 1 || atoi( argument ) > 1500 )
                    {
                        send_to_char( "Bonus Dice must be between 1 and 1500.\n\r", ch);
                        return FALSE;
                    }
                    else
                    {
                        send_to_char( "Bonus dice set.\n\r\n\r", ch );
                        pObj->value[5] = atoi( argument );
                        break;
                    }
                    break;
	    }
            break;

	case ITEM_QUIVER:
	    switch ( value_num )
	    {
		default:
		    do_help( ch, "ITEM_QUIVER" );
		    return FALSE;
		case 0:
		    pObj->value[0] = atoi( argument );
		    send_to_char( "Number of arrows set.\n\r\n\r", ch );
		    return TRUE;
		case 1:
                    if ( atoi( argument ) < 1 || atoi( argument ) > 150 )
                    {
                        send_to_char( "Number of Dice must be between 1 and 150.\n\r", ch);
                        return FALSE;
                    }
                    else
                    {
                        send_to_char( "Number of dice set.\n\r\n\r", ch );
                        pObj->value[1] = atoi( argument );
                        break;
                    }
                    break;
		case 2:
		    if ( atoi( argument ) < 1 || atoi( argument ) > 150 )
                    {
                        send_to_char( "Type of Dice must be between 1 and 150.\n\r", ch);
                        return FALSE;
                    }
                    else
                    {
                        send_to_char( "Type of dice set.\n\r\n\r", ch );
                        pObj->value[2] = atoi( argument );
                        break;
                    }
                    break;
	    }
	    break;

	case ITEM_BERRY:
	    switch( value_num )
	    {
		default:
		    do_help( ch, "ITEM_BERRY" );
		    return FALSE;
		case 0:
		    pObj->value[0] = atoi( argument );
		    send_to_char( "Minimum healing set.\n\r", ch );
		    break;
		case 1:
		    pObj->value[1] = atoi( argument );
		    send_to_char( "Maximum healing set.\n\r", ch );
		    break;
	    }
	    break;

        case ITEM_BOOK:
            switch( value_num )
            {
                default:
                    do_help( ch, "ITEM_BOOK" );
                    return FALSE;
		case 1:
	            if ( ( value = flag_value( book_flags, argument ) )
	              != NO_FLAG )
	        	TOGGLE_BIT( pObj->value[1], value );
		    else
		    {
			do_help ( ch, "ITEM_BOOK" );
			return FALSE;
		    }
	            send_to_char( "Book flags set.\n\r\n\r", ch );
	            break;
		case 2:
		    if ( atoi( argument) != 0 )
		    {
			if ( !get_obj_index( atoi( argument ) ) )
			{
			    send_to_char( "There is no such item.\n\r\n\r", ch );
			    return FALSE;
			}

			if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY
			&&   get_obj_index( atoi( argument ) )->item_type != ITEM_PERMKEY )
			{
			    send_to_char( "That item is not a key.\n\r\n\r", ch );
			    return FALSE;
			}
		    }
		    send_to_char( "Book key set.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
            }
            break;

        case ITEM_CONTAINER:
	    switch ( value_num )
	    {
		int value;

		default:
		    do_help( ch, "ITEM_CONTAINER" );
	            return FALSE;
		case 0:
	            if ( !weight_value( argument, &pObj->value[0] ) )
	            {
	                send_to_char( "OEdit:  Not a valid weight.\n\r", ch );
	                return FALSE;
	            }
	            send_to_char( "Weight capacity set.\n\r\n\r", ch );
	            break;
		case 1:
	            if ( ( value = flag_value( container_flags, argument ) )
	              != NO_FLAG )
	        	TOGGLE_BIT(pObj->value[1], value);
		    else
		    {
			do_help ( ch, "ITEM_CONTAINER" );
			return FALSE;
		    }
	            send_to_char( "Container type set.\n\r\n\r", ch );
	            break;
		case 2:
		    if ( atoi(argument) != 0 )
		    {
			if ( !get_obj_index( atoi( argument ) ) )
			{
			    send_to_char( "There is no such item.\n\r\n\r", ch );
			    return FALSE;
			}

			if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY
			&&   get_obj_index( atoi( argument ) )->item_type != ITEM_PERMKEY )
			{
			    send_to_char( "That item is not a key.\n\r\n\r", ch );
			    return FALSE;
			}
		    }
		    send_to_char( "Container key set.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
		case 3:
	            if ( !weight_value( argument, &pObj->value[3] ) )
	            {
	                send_to_char( "OEdit:  Not a valid weight.\n\r", ch );
	                return FALSE;
	            }
	            send_to_char( "Single item max weight set.\n\r\n\r", ch );
	            break;
		case 4:
		    value = atoi( argument );
		    if ( value < 10 || value > 200 )
		    {
			send_to_char( "OEdit:  Value must between 10% and 200%.\n\r", ch );
			return FALSE;
		    }
		    send_to_char( "Weight multiplier set.\n\r", ch );
		    pObj->value[4] = value;
		    break;
		case 5:
		    value = atoi( argument );
		    if ( value < 0 )
			value = 0;
		    pObj->value[5] = value;
		    break;
	    }
	    break;

	case ITEM_DRINK_CON:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_DRINK" );
	            return FALSE;
	        case 0:
	            pObj->value[0] = atoi( argument );
		    if ( pObj->value[1] == 0 )
		    {
			pObj->value[1] = pObj->value[0];
			send_to_char( "Maximum and current amounts of liquid set.\n\r", ch );
		    }
		    else
		    {
			send_to_char( "Maximum amount of liquid set.\n\r", ch );
		    }
	            break;
	        case 1:
	            send_to_char( "Current amount of liquid set.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
		    value = liq_lookup( argument );
		    if ( value == NO_VALUE )
		    {
			send_to_char( "OEdit:  Bad liquid type.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Liquid type set.\n\r\n\r", ch );
	            pObj->value[2] = value;
	            break;
	        case 3:
		    if ( is_number( argument ) )
			pObj->value[3] = atoi( argument ) == 0 ? 0 : 1;
		    else if ( UPPER( *argument ) == 'Y' )
			pObj->value[3] = 1;
		    else if ( UPPER( *argument ) == 'N' )
			pObj->value[3] = 0;
		    else
		    {
			do_help ( ch, "ITEM_DRINK" );
			return FALSE;
		    }
	            send_to_char( "Poison value set.\n\r", ch );
	            break;
                case 4:
                    send_to_char( "Spoil hours set.\n\r", ch );
                    pObj->value[4] = atoi( argument );
                    break;
	    }
            break;

	case ITEM_FOOD:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_FOOD" );
	            return FALSE;
	        case 0:
	            send_to_char( "Fullness hours set.\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "Hunger hours set.\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 3:
		    if ( is_number( argument ) )
			pObj->value[3] = atoi( argument ) == 0 ? 0 : 1;
		    else if ( UPPER( *argument ) == 'Y' )
			pObj->value[3] = 1;
		    else if ( UPPER( *argument ) == 'N' )
			pObj->value[3] = 0;
		    else
		    {
			do_help ( ch, "ITEM_FOOD" );
			return FALSE;
		    }
	            send_to_char( "Poison value set.\n\r", ch );
	            break;
                case 4:
                    send_to_char( "Spoil hours set.\n\r", ch );
                    pObj->value[4] = atoi( argument );
                    break;
	    }
            break;

	case ITEM_FOUNTAIN:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_FOUNTAIN" );
	            return FALSE;
		case 1:
		    if ( is_number( argument ) )
		        pObj->value[1] = atoi( argument ) == 0 ? 0 : 1;
                    else if ( !str_cmp( argument, "wet" ) )
                        pObj->value[1] = 0;
                    else if ( !str_cmp( argument, "dry" ) )
                        pObj->value[1] = 1;
                    else
                    {
                        send_to_char( "OEdit:  Value must be \"wet\" or \"dry\".\n\r", ch );
                        return FALSE;
                    }
                    ch_printf( ch, "Fountain is %s.\n\r", pObj->value[1] == 0 ? "wet" : "dry" );
                    break;
	        case 2:
		    value = liq_lookup( argument );
		    if ( value == NO_VALUE )
		    {
			send_to_char( "OEdit:  Bad liquid type.\n\r", ch );
			return FALSE;
		    }
	            send_to_char( "Liquid type set.\n\r\n\r", ch );
	            pObj->value[2] = value;
	            break;
	        case 3:
		    if ( is_number( argument ) )
			pObj->value[3] = atoi( argument ) == 0 ? 0 : 1;
		    else if ( UPPER( *argument ) == 'Y' )
			pObj->value[3] = 1;
		    else if ( UPPER( *argument ) == 'N' )
			pObj->value[3] = 0;
		    else
		    {
			do_help ( ch, "ITEM_FOUNTAIN" );
			return FALSE;
		    }
	            send_to_char( "Poison value set.\n\r", ch );
	            break;
	    }
            break;

	case ITEM_INK:
	    switch( value_num )
	    {
		default:
		    do_help( ch, "ITEM_INK" );
		    return FALSE;
		case 0:
		    value = atoi( argument );
		    if ( value > 0 )
		    {
			pObj->value[0] = value;
			pObj->value[1] = value;
			send_to_char( "Max and current capacities set.\n\r", ch );
		    }
		    else
		    {
			send_to_char( "OEdit:  capacity must be a positive value.\n\r", ch );
			return FALSE;
		    }
		    break;
		case 1:
		    value = atoi( argument );
		    if ( value < 0 )
		    {
			send_to_char( "OEdit: capacity must be a non-negative value.\n\r", ch );
			return FALSE;
		    }
		    if ( value > pObj->value[0] )
		    {
			send_to_char( "OEdit:  Current capacity must be less than or equal to max capacity.\n\r", ch );
			return FALSE;
		    }
		    pObj->value[1] = value;
		    send_to_char( "Current capacity set.\n\r", ch );
		    break;
	    }
	    break;

	case ITEM_MONEY:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_MONEY" );
	            return FALSE;
	        case 0:
	            sprintf( buf, "%s amount set.\n\r\n\r", GOLD_NOUN );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            sprintf( buf, "%s amount set.\n\r\n\r", SILVER_NOUN );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            sprintf( buf, "%s amount set.\n\r\n\r", COPPER_NOUN );
	            pObj->value[2] = atoi( argument );
	            break;
	        case 3:
	            sprintf( buf, "%s amount set.\n\r\n\r", FRACT_NOUN );
	            pObj->value[3] = atoi( argument );
	            break;
	    }
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
            break;

	case ITEM_FURNITURE:
	    switch( value_num )
	    {
		default:
		    do_help( ch, "ITEM_FURNITURE" );
		    return FALSE;
		case 0:
		    send_to_char( "Capacity set.\n\r", ch );
		    pObj->value[0] = atoi( argument );
		    break;
		case 2:
		    if ( !str_cmp( "0", argument ) || !str_prefix( argument, "none" ) )
		    {
			send_to_char( "Position flags cleared.\n\r", ch );
			pObj->value[2] = 0;
			break;
		    }
		    if ( ( bits = flag_value( furniture_flags, argument ) ) == NO_FLAG )
		    {
			send_to_char( "OEdit:  Bad position flag.\n\r", ch );
			return FALSE;
		    }
		    send_to_char( "Position set.\n\r", ch );
		    pObj->value[2] ^= bits;
		    break;
		case 3:
		    if ( atoi( argument ) < -300 || atoi( argument ) > 300 )
		    {
			send_to_char( "Value must be between -300% and 300%.\n\r", ch );
			return FALSE;
		    }
		    else
		    {
			send_to_char(  "Recovery set.\n\r\n\r", ch );
			pObj->value[3] = atoi( argument );
		    }
		    break;
		case 4:
		    if ( atoi( argument ) < -300 || atoi( argument ) > 300 )
		    {
			send_to_char( "Value must be between -300% and 300%.\n\r", ch );
			return FALSE;
		    }
		    else
		    {
			send_to_char(  "Mana recovery set.\n\r\n\r", ch );
			pObj->value[4] = atoi( argument );
		    }
		    break;
	    }
	    break;

	case ITEM_PORTAL:
	    switch( value_num )
	    {
		int	value;

		default:
		    do_help( ch, "ITEM_PORTAL" );
		    return FALSE;
		case 0:
		    pObj->value[0] = atoi( argument );
		    send_to_char( "Charges set.\n\r", ch );
		    break;
		case 1:
		    if ( ( value = flag_value( portal_flags, argument ) ) == NO_FLAG )
		    {
		        do_help( ch, "item_portal" );
		        return FALSE;
                    }
                    TOGGLE_BIT( pObj->value[1], value );
                    send_to_char( "Flags set.\n\r", ch );
                    break;
		case 2:
		    if ( atoi(argument) != 0 )
		    {
			if ( get_obj_index( atoi( argument ) ) == NULL )
			{
			    send_to_char( "There is no such item.\n\r\n\r", ch );
			    return FALSE;
			}

			if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY
			&&   get_obj_index( atoi( argument ) )->item_type != ITEM_PERMKEY )
			{
			    send_to_char( "That item is not a key.\n\r\n\r", ch );
			    return FALSE;
			}
		    }
		    send_to_char( "Portal key set.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
		case 3:
		    if ( !str_cmp( "0", argument ) || !str_cmp( argument, "none" ) )
		    {
			pObj->value[3] = 0;
			send_to_char( "Room cleared.\n\r", ch );
			return TRUE;
		    }
		    if ( !is_number( argument ) )
		    {
			send_to_char( "OEdit:  value3 must be a room vnum.\n\r", ch );
			return FALSE;
		    }
		    value = atoi( argument );
		    if ( get_room_index( value ) == NULL )
		    {
			send_to_char( "OEdit:  value3 must be a valid room vnum, or \"none\".\n\r", ch );
			return FALSE;
		    }
		    send_to_char( "Destination room set.\n\r", ch );
		    pObj->value[3] = value;
		    break;
		case 4:
		    if ( is_number( argument ) )
		        value = atoi( argument );
                    else
                        value = flag_value( size_types, argument );
                    if ( value < 0 || value >= MAX_SIZE )
                    {
                        send_to_char( "OEdit:  Bad value for size.\n\r", ch );
                        return FALSE;
                    }
                    pObj->value[4] = value;
                    send_to_char( "Size set.\n\r", ch );
                    break;
	    }
	    break;

	case ITEM_SHEATH:
	    switch( value_num )
	    {
		bitvector		flags;
		OBJ_INDEX_DATA *	pKey;
		int			value;

		default:
		    do_help( ch, "ITEM_SHEATH" );
		    return FALSE;
		case 1:
		    if ( ( flags = flag_value( sheath_flags, argument ) ) != NO_FLAG )
			TOGGLE_BIT( pObj->value[1], flags );
		    else
		    {
			do_help( ch, "ITEM_SHEATH" );
			return FALSE;
		    }
		    send_to_char( "Sheath flags set.\n\r", ch );
		    break;
		case 2:
		    if ( ( value = atoi( argument ) ) != 0 )
		    {
			if ( ( pKey = get_obj_index( value ) ) == NULL )
			{
			    send_to_char( "There is no such item.\n\r\n\r", ch );
			    return FALSE;
			}

			if ( pKey->item_type != ITEM_KEY
			&&   pKey->item_type != ITEM_PERMKEY )
			{
			    send_to_char( "That item is not a key.\n\r\n\r", ch );
			    return FALSE;
			}
		    }
		    send_to_char( "Sheath key set.\n\r\n\r", ch );
		    pObj->value[2] = value;
		    break;
		case 3:
		case 4:
		case 5:
		    if ( !str_cmp( argument, "none" ) )
			value = 0;
		    else
		    {
			if ( ( value = flag_value( weapon_types, argument ) ) == NO_FLAG
			||   value == WEAPON_EXOTIC )
			{
			    send_to_char( "OEdit:  Bad weapon class.\n\r", ch );
			    return FALSE;
			}
		    }
		    pObj->value[value_num] = value;
		    if ( pObj->value[4] == 0 && pObj->value[5] != 0 )
		    {
			pObj->value[4] = pObj->value[5];
			pObj->value[5] = 0;
		    }
		    if ( pObj->value[3] == 0 && pObj->value[4] != 0 )
		    {
			pObj->value[3] = pObj->value[4];
			pObj->value[4] = 0;
		    }
		    if ( pObj->value[4] == 0 && pObj->value[5] != 0 )
		    {
			pObj->value[4] = pObj->value[5];
			pObj->value[5] = 0;
		    }
		    send_to_char( "Weapon class set.\n\r", ch );
		    break;
	    }
	    break;

	case ITEM_MAP:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_MAP" );
		    return FALSE;
		case 0:
		    if ( is_number( argument ) )
		    {
		        sn = atoi( argument );
		        pObj->value[0] = ( sn != 0 );
                    }
                    else
		    {
			if ( str_match( argument, "y", "yes" ) )
			    pObj->value[0] = 1;
			else if ( str_match( argument, "n", "no" ) )
			    pObj->value[0] = 0;
			else
			{
			    do_help( ch, "ITEM_MAP" );
			    return FALSE;
			}
		    }
		    send_to_char( "Save state set.\n\r", ch );
		    break;
	    }
	    break;
    }

    pBuf = new_buf( );
    show_obj_values( pObj, pBuf );
    send_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return TRUE;
}


static bool
set_value( CHAR_DATA *ch, char *argument, int value )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ( ch, pObj );

    if ( argument[0] == '\0' )
    {
	set_obj_values( ch, pObj, -1, "" );
	return FALSE;
    }

    return set_obj_values( ch, pObj, value, argument );
}


void
show_obj_info( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, BUFFER *pBuf )
{
    AFFECT_DATA *	paf;
    EXTRA_DESCR_DATA *	ed;
    OREPROG_DATA *	prog;
    TEXT_DATA *		page;
    char		buf[MAX_INPUT_LENGTH];
    char *		p;
    int			cnt;
    int			i;
    int			pcount;
    int			weight;
    int			cost;
	//int         qcost;
    int			gold;
    int			silver;
    int			copper;
    int			fract;
    bool		prev_cost;

    buf_printf( pBuf, "Name:         [%s]\n\rArea:         [%5d] %s\n\r",
	pObj->name,
	!pObj->area ? -1        : pObj->area->vnum,
	!pObj->area ? "No Area" : pObj->area->name );


    buf_printf( pBuf, "Vnum:         [%5d]\n\rType:         [%s]\n\r",
	pObj->vnum,
	item_name( pObj->item_type ) );
    if ( pObj->item_type == ITEM_BOOK )
    {
        cnt = 0;
        for ( page = pObj->page; page != NULL; page = page->next )
            cnt++;
        buf_printf( pBuf, "Pages:        [%d]\n\r", cnt );
    }
    buf_printf( pBuf, "Level:        [%d]\n\r", pObj->level );

    weight = pObj->weight;
    buf_printf( pBuf, "Weight:       [%d] (%s)\n\r", weight, weight_string( weight ) );

    buf_printf( pBuf, "Condition:    [%d]\n\r", pObj->condition );

    cost    = pObj->cost;
    fract   = cost % FRACT_PER_COPPER;
    cost   /= FRACT_PER_COPPER;
    copper  = cost % COPPER_PER_SILVER;
    cost   /= COPPER_PER_SILVER;
    silver  = cost % SILVER_PER_GOLD;
    gold    = cost / SILVER_PER_GOLD;
    prev_cost = FALSE;
    add_buf( pBuf, "Cost:         [" );
    if ( gold != 0 )
    {
	buf_printf( pBuf, "%d %s", gold, gold == 1 ? GOLD_NOUN : GOLD_PLURAL );
	prev_cost = TRUE;
    }
    if ( silver != 0 )
    {
	if ( prev_cost )
	    add_buf( pBuf, " " );
	buf_printf( pBuf, "%d %s", silver, silver == 1 ? SILVER_NOUN : SILVER_PLURAL );
	prev_cost = TRUE;
    }
    if ( copper != 0 )
    {
	if ( prev_cost )
	    add_buf( pBuf, " " );
	buf_printf( pBuf, "%d %s", copper, copper == 1 ? COPPER_NOUN : COPPER_PLURAL );
	prev_cost = TRUE;
    }
    if ( fract != 0 )
    {
	if ( prev_cost )
	    add_buf( pBuf, " " );
	buf_printf( pBuf, "%d %s", fract, fract == 1 ? FRACT_NOUN : FRACT_PLURAL );
	prev_cost = TRUE;
    }
    if ( !prev_cost )
	add_buf( pBuf, "0" );
    add_buf( pBuf, "]\n\r" );

//    qcost  = pObj->qcost;
  //  buf_printf( pBuf, "QP Cost:  %s`X\n\r", qcost );



    buf_printf( pBuf, "Wear flags:   [%s]\n\r",
	flag_string( wear_flags, pObj->wear_flags ) );
    buf_printf( pBuf, "Extra flags:  [%s]\n\r",
	flag_string( extra_flags, pObj->extra_flags ) );
	buf_printf( pBuf, "Extra2 flags:  [%s]\n\r",
	flag_string( extra_flags2, pObj->extra_flags2 ) );
    add_buf( pBuf, "Anti flags:   [" );
    p = buf;
    for ( i = 1; !IS_NULLSTR( race_table[i].name ); i++ )
        if ( xIS_SET( pObj->race_flags, i ) )
            p += sprintf( p, " %s", race_table[i].name );
    for ( i = 0; i < MAX_CLASS; i++ )
        if ( xIS_SET( pObj->class_flags, i ) )
            p += sprintf( p, " %s", class_table[i].name );
    if ( p == buf )
        strcpy( buf, " none" );
    buf_printf( pBuf, "%s]\n\r", buf + 1 );

    buf_printf( pBuf, "Timer:        [%d]\n\r", pObj->timer );
    buf_printf( pBuf, "Duration:     [%d]\n\r", pObj->duration );
    buf_printf( pBuf, "Material:     [%s]\n\r", xbit_string( material_types, pObj->material ) );

    pcount = 0;
    for ( prog = pObj->oprogs; prog != NULL; prog = prog->next )
        pcount++;
    buf_printf( pBuf, "Oprogs:       [%d]\n\r", pcount );

    if ( pObj->join )
	buf_printf( pBuf, "Joins to create: [%d]\n\r", pObj->join );
    if ( pObj->sep_one )
	buf_printf( pBuf, "First separated object:  [%d]\n\r", pObj->sep_one );
    if ( pObj->sep_two )
	buf_printf( pBuf, "Second separated object: [%d]\n\r", pObj->sep_two );

    buf_printf( pBuf, "Invoke type:  [%s]\n\r",
		flag_string( invoke_types, pObj->ac_type ) );
    switch ( pObj->ac_type )
    {
	case INVOKE_MOB:
	    buf_printf( pBuf, "Invokes mob:  [%d] %s\n\r",
			pObj->ac_vnum,
			get_mob_index( pObj->ac_vnum ) ?
			    get_mob_index( pObj->ac_vnum )->short_descr :
			    "(none)" );
	    break;
	case INVOKE_OBJ:
	    buf_printf( pBuf, "Invokes obj:  [%d] %s\n\r",
			pObj->ac_vnum,
			get_obj_index( pObj->ac_vnum ) ?
			    get_obj_index( pObj->ac_vnum )->short_descr :
			    "(none)" );
	    break;
	case INVOKE_TRANS:
	    buf_printf( pBuf, "Transfers to: [%d] %s\n\r",
			pObj->ac_vnum,
			get_room_index( pObj->ac_vnum ) ?
			    get_room_index( pObj->ac_vnum )->name :
			    "(none)" );
	    break;
	case INVOKE_MORPH:
	    buf_printf( pBuf, "Morphs to:    [%d] %s\n\r",
			pObj->ac_vnum,
			get_obj_index( pObj->ac_vnum ) ?
			    get_obj_index( pObj->ac_vnum )->short_descr :
			    "(none)" );
	    break;
	case INVOKE_SPELL:
	    buf_printf( pBuf, "Casts spell:  [%s]\n\r",
			pObj->ac_vnum < 1 ? "(none)"
			: skill_table[pObj->ac_vnum].name );
	    break;
    }
    if ( pObj->ac_type )
    {
	add_buf( pBuf, "Charges:      [" );
	if ( pObj->ac_charge[1] == -1 )
	    add_buf( pBuf, "permanent" );
	else
	    buf_printf( pBuf, "%d", pObj->ac_charge[1] );
	add_buf( pBuf, "]\n\r" );
    }

    if ( pObj->extra_descr )
    {
	add_buf( pBuf, "Ex desc kwd: " );
	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    add_buf( pBuf, " [" );
	    add_buf( pBuf, ed->keyword );
	    add_buf( pBuf, "]" );
	}
	add_buf( pBuf, "\n\r" );
    }

    buf_printf( pBuf, "Short desc:  %s`X\n\r", pObj->short_descr );
    buf_printf( pBuf, "Long desc:\n\r     %s`X\n\r", pObj->description );

    for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
    {
	if ( cnt == 0 )
	{
	    add_buf( pBuf, "Number Modifier Affects\n\r" );
	    add_buf( pBuf, "------ -------- -------\n\r" );
	}
	buf_printf( pBuf, "[%4d] %-8d %s\n\r", cnt,
	    paf->modifier,
	    flag_string( apply_types, paf->location ) );
	cnt++;
    }

    show_obj_values( pObj, pBuf );

}


static void
show_obj_values( OBJ_INDEX_DATA *pObj, BUFFER *pBuf )
{
    ROOM_INDEX_DATA *	room;
    char		buf[SHORT_STRING_LENGTH];;
    char		value3[MAX_INPUT_LENGTH];
    char		value4[MAX_INPUT_LENGTH];
    char		value5[MAX_INPUT_LENGTH];
    int			len;
    int			drink_cur;
    int			drink_max;
    bool		cur_fract;
    bool		max_fract;

    switch( pObj->item_type )
    {
	default:
	    break;

	case ITEM_ARMOR:
	    buf_printf( pBuf,
		"[v0] Ac pierce       [%d]\n\r"
		"[v1] Ac bash         [%d]\n\r"
		"[v2] Ac slash        [%d]\n\r"
		"[v3] Ac exotic       [%d]\n\r"
		"[v4] Bulk            [%d]\n\r",
		pObj->value[0],
		pObj->value[1],
		pObj->value[2],
		pObj->value[3],
		pObj->value[4] );
	    break;

	case ITEM_BERRY:
	    buf_printf( pBuf,
		"[v0] Minimum healing [%d]\n\r"
		"[v1] Maximum healing [%d]\n\r",
		pObj->value[0],
		pObj->value[1] );
	    break;

	case ITEM_BOOK:
	    buf_printf( pBuf,
		"[v1] Flags:       [%s]\n\r"
		"[v2] Key:         %s`X [%d]\n\r",
		flag_string( book_flags, pObj->value[1] ),
		get_obj_index( pObj->value[2] )
		    ? get_obj_index( pObj->value[2] )->short_descr
		    : "none",
		pObj->value[2] );
	    break;

	case ITEM_CONTAINER:
	    buf_printf( pBuf,
		"[v0] Weight:      [%d] (%s)\n\r", pObj->value[0],
		weight_string( pObj->value[0] ) );
	    buf_printf( pBuf,
		"[v1] Flags:       [%s]\n\r"
		"[v2] Key:         %s`X [%d]\n\r"
		"[v3] Max 1 obj:   [%d] (%s)\n\r"
		"[v4] Weight Mult: [%d%%]\n\r"
		"[v5] Max obj:     [%d]\n\r",
		flag_string( container_flags, pObj->value[1] ),
		get_obj_index( pObj->value[2] )
		    ? get_obj_index( pObj->value[2] )->short_descr
		    : "none",
		pObj->value[2],
		pObj->value[3], weight_string( pObj->value[3] ),
		pObj->value[4], pObj->value[5] );
	    break;

	case ITEM_DRINK_CON:
	    drink_cur = pObj->value[0] / liq_table[pObj->value[2]].liq_affect[4];
	    drink_max = pObj->value[1] / liq_table[pObj->value[2]].liq_affect[4];
	    cur_fract = pObj->value[0] == liq_table[pObj->value[2]].liq_affect[4]* drink_cur;
	    max_fract = pObj->value[1] == liq_table[pObj->value[2]].liq_affect[4]* drink_max;
	    buf_printf( pBuf,
	        "[v0] Liquid Total: [%d] (%d%s drink%s)\n\r"
	        "[v1] Liquid Left:  [%d] (%d%s drink%s)\n\r"
	        "[v2] Liquid:       [%s] (%d oz/drink)\n\r"
	        "[v3] Poisoned:     [%s]\n\r"
		"[v4] Spoil hours   [%d]\n\r",
		pObj->value[0], drink_cur, cur_fract ? "" : "+", drink_cur == 1 ? "" : "s",
		pObj->value[1], drink_max, max_fract ? "" : "+",  drink_max == 1 ? "" : "s",
		liq_table[pObj->value[2]].liq_name,
		liq_table[pObj->value[2]].liq_affect[4],
		pObj->value[3] != 0 ? "Yes" : "No",
                pObj->value[4] );
	    break;

	case ITEM_FOOD:
	    buf_printf( pBuf,
		"[v0] Full hours:   [%d]\n\r"
		"[v1] Hunger hours: [%d]\n\r"
		"[v3] Poisoned:     %s\n\r"
		"[v4] Spoil hours   [%d]\n\r",
		pObj->value[0], pObj->value[1],
		pObj->value[3] != 0 ? "Yes" : "No",
                pObj->value[4] );
	    break;

	case ITEM_FOUNTAIN:
	    buf_printf( pBuf,
		"[v1] Wet/dry:  %s\n\r"
		"[v2] Liquid:   %s\n\r"
		"[v3] Poisoned: %s\n\r",
		pObj->value[1] == 0 ? "wet" : "dry",
		liq_table[pObj->value[2]].liq_name,
		pObj->value[3] != 0 ? "Yes" : "No" );
	    break;

	case ITEM_INK:
	    buf_printf( pBuf,
		"[v0] Maximum capacity: %d\n\r"
		"[v1] Current capacity: %d\n\r",
		pObj->value[0], pObj->value[1] );
	    break;

	case ITEM_LIGHT:
	    if ( pObj->value[2] == -1 )
		add_buf( pBuf, "[v2] Light:  Infinite[-1]\n\r" );
	    else
		buf_printf( pBuf, "[v2] Light:  [%d]\n\r", pObj->value[2] );
	    break;

	case ITEM_MONEY:
	    len = UMAX( strlen( GOLD_PLURAL   ), strlen( SILVER_PLURAL ) );
	    len = UMAX( strlen( COPPER_PLURAL ), len );
	    len = UMAX( strlen( FRACT_PLURAL  ), len );
	    sprintf( buf, "%s:", GOLD_PLURAL );
	    buf[0] = UPPER( buf[0] );
	    buf_printf( pBuf, "[v0] %-*s [%d]\n\r", len, buf, pObj->value[0] );
	    sprintf( buf, "%s:", SILVER_PLURAL );
	    buf[0] = UPPER( buf[0] );
	    buf_printf( pBuf, "[v1] %-*s [%d]\n\r", len, buf, pObj->value[1] );
	    sprintf( buf, "%s:", COPPER_PLURAL );
	    buf[0] = UPPER( buf[0] );
	    buf_printf( pBuf, "[v2] %-*s [%d]\n\r", len, buf, pObj->value[2] );
	    sprintf( buf, "%s:", FRACT_PLURAL );
	    buf[0] = UPPER( buf[0] );
	    buf_printf( pBuf, "[v3] %-*s [%d]\n\r", len, buf, pObj->value[3] );
	    break;

	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
	    buf_printf( pBuf,
		"[v0] Level:  [%d]\n\r"
		"[v1] Spell:  %s\n\r"
		"[v2] Spell:  %s\n\r"
		"[v3] Spell:  %s\n\r",
		pObj->value[0],
		pObj->value[1] > 0 ? skill_table[pObj->value[1]].name : "none",
		pObj->value[2] > 0 ? skill_table[pObj->value[2]].name : "none",
		pObj->value[3] > 0 ? skill_table[pObj->value[3]].name : "none" );
	    break;

	case ITEM_HERB:
	    buf_printf( pBuf,
		"[v0] Level:          [%d]\n\r"
		"[v1] Spell (eaten) : %s\n\r"
		"[v2] Spell (rubbed): %s\n\r"
		"[v3] Spell (brewed): %s\n\r",
		pObj->value[0],
		pObj->value[1] > 0 ? skill_table[pObj->value[1]].name : "none",
		pObj->value[2] > 0 ? skill_table[pObj->value[2]].name : "none",
		pObj->value[3] > 0 ? skill_table[pObj->value[3]].name : "none" );
	    buf_printf( pBuf,
		"[v5] Poison:         [%s]\n\r", flag_string( herb_flags, pObj->value[5] ) );
	    break;

	case ITEM_PORTAL:
	    buf_printf( pBuf, "[v0] Charges:     [%d]\n\r", pObj->value[0] );
	    buf_printf( pBuf, "[v1] Flags:       [%s]\n\r",
	                flag_string( portal_flags, pObj->value[1] ) );
	    buf_printf( pBuf, "[v2] Key:         [%d] %s`X\n\r",
	                pObj->value[2],
	                get_obj_index( pObj->value[2] )
                        ? get_obj_index( pObj->value[2] )->short_descr : "(none)" );
	    buf_printf( pBuf, "[v3] Destination: [%d] ", pObj->value[3] );
	    room = get_room_index( pObj->value[3] );
	    buf_printf( pBuf, "%s`X\n\r", room == NULL ? "(none)" : room->name );
	    buf_printf( pBuf, "[v4] Size:        [%s]\n\r",
                        flag_string( size_types, pObj->value[4] ) );
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
	case ITEM_LENS:
	    buf_printf( pBuf, 
		"[v0] Level:          [%d]\n\r"
		"[v1] Charges Total:  [%d]\n\r"
		"[v2] Charges Left:   [%d]\n\r"
		"[v3] Spell:          %s\n\r",
		pObj->value[0],
		pObj->value[1],
		pObj->value[2],
		pObj->value[3] > 0 ? skill_table[pObj->value[3]].name : "none" );
	    break; 

	case ITEM_SHEATH:
	    if ( pObj->value[3] == 0 )
		strcpy( value3, "none" );
	    else
		strcpy( value3, flag_string( weapon_types, pObj->value[3] ) );
	    if ( pObj->value[4] == 0 )
		strcpy( value4, "none" );
	    else
		strcpy( value4, flag_string( weapon_types, pObj->value[4] ) );
	    if ( pObj->value[5] == 0 )
		strcpy( value5, "none" );
	    else
		strcpy( value5, flag_string( weapon_types, pObj->value[5] ) );

	    buf_printf( pBuf,
		"[v1] Flags:          [%s]\n\r"
		"[v2] Key:            [%d] %s`X\n\r"
		"[v3] Weapon class 1: %s\n\r"
		"[v4] Weapon class 2: %s\n\r"
		"[v5] Weapon class 3: %s\n\r",
		flag_string( sheath_flags, pObj->value[1] ),
		pObj->value[2],
		get_obj_index( pObj->value[2] )
		    ? get_obj_index( pObj->value[2] )->short_descr
		    : "none",
		value3,
		value4,
		value5 );
	    break;

	case ITEM_WEAPON:
	    buf_printf( pBuf, "[v0] Weapon class:   %s\n\r",
		     flag_string( weapon_types, pObj->value[0] ) );
	    buf_printf( pBuf, "[v1] Number of dice: [%d]\n\r", pObj->value[1] );
	    buf_printf( pBuf, "[v2] Type of dice:   [%d]\n\r", pObj->value[2] );
	    buf_printf( pBuf, "[v3] Type:           %s\n\r",
		    flag_string( weapon_flags, pObj->value[3] ) );
	    buf_printf( pBuf, "[v4] Special type:   %s\n\r",
		     flag_string( weapon_type_olc,  pObj->value[4] ) );
	    buf_printf( pBuf, "[v5] Damage bonus:   %d\n\r", pObj->value[5] );
	    break;

	case ITEM_FURNITURE:
	    buf_printf( pBuf, "[v0] Capacity: [%d]\n\r", pObj->value[0] );
	    buf_printf( pBuf, "[v2] Position: [%s]\n\r",
			flag_string( furniture_flags, pObj->value[2] ) );
	    buf_printf( pBuf, "[v3] Recovery: [%d%%]\n\r", pObj->value[3] );
	    buf_printf( pBuf, "[v4] Mana rec: [%d%%]\n\r", pObj->value[4] );
	    break;

	case ITEM_QUIVER:
	    buf_printf( pBuf, "[v0] Number of arrows: [%2d]\n\r", pObj->value[0] );
	    buf_printf( pBuf, "[v1] Number of dice:   [%2d]\n\r", pObj->value[1] );
	    buf_printf( pBuf, "[v2] Type of dice:     [%d]\n\r",  pObj->value[2] );
	    break;

	case ITEM_ARROW:
	    buf_printf( pBuf, "[v1] Number of dice:   [%2d]\n\r", pObj->value[1] );
	    buf_printf( pBuf, "[v2] Type of dice:     [%d]\n\r",  pObj->value[3] );
	    break;

	case ITEM_MAP:
	    buf_printf( pBuf, "[v0] Saves in pfiles:  [%s]\n\r", pObj->value[0] != 0 ? "yes" : "no" );
	    break;
 case ITEM_TOKEN:
        buf_printf( pBuf, "[v0] Quest Point Value: [%d]\n\r", pObj->value[0]);
        break;
    

	case ITEM_BOAT:
	case ITEM_CLOTHING:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_GEM:
	case ITEM_JEWELRY:
	case ITEM_KEY:
	case ITEM_PERMKEY:
/* 	case ITEM_PROTECT: */
/* 	case ITEM_ROOM_KEY: */
	case ITEM_TRASH:
	case ITEM_TREASURE:
	case ITEM_WARP_STONE:
	    break;
    }

    return;
}


