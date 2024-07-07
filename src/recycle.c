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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "db.h"

/*
 * Local functions
 */
int	get_size	args( ( int val ) );


/* stuff for recycling affects */
AFFECT_DATA *affect_free;

AFFECT_DATA *
new_affect( void )
{
    static AFFECT_DATA af_zero;
    AFFECT_DATA *af;

    if ( affect_free == NULL )
    {
	af = (AFFECT_DATA *)alloc_perm( sizeof( *af ) );
	top_affect++;
    }
    else
    {
	af = affect_free;
	affect_free = affect_free->next;
    }

    *af = af_zero;

    VALIDATE( af );
    return af;
}


void
free_affect( AFFECT_DATA *af )
{
    if ( !IS_VALID( af ) )
	return;

    INVALIDATE( af );
    af->next = affect_free;
    affect_free = af;
}


/* Stuff for recycling aliases */
ALIAS_DATA *alias_free;

ALIAS_DATA *new_alias( void )
{
    ALIAS_DATA *alias;

    if ( alias_free == NULL )
    {
	alias = (ALIAS_DATA *)alloc_perm( sizeof( *alias ) );
	top_alias++;
    }
    else
    {
	alias = alias_free;
	alias_free = alias_free->next;
    }

    alias->next = NULL;
    alias->old  = &str_empty[0];
    alias->new  = &str_empty[0];

    return alias;
}

void
free_alias( ALIAS_DATA *alias )
{
    free_string( alias->old );
    free_string( alias->new );
    alias->next = alias_free;
    alias_free  = alias;
}


/* stuff for creating area data (areas are never freed) */
AREA_DATA *
new_area( void )
{
    AREA_DATA *pArea;

    pArea		= (AREA_DATA *)alloc_perm( sizeof( *pArea ) );
    top_area++;
    pArea->next		= NULL;
    pArea->name		= str_dup( "New area" );
    pArea->recall	= ROOM_VNUM_TEMPLE;
    pArea->resetmsg	= &str_empty[0];
    pArea->norecall	= &str_empty[0];
    pArea->area_flags	= AREA_NONE;
    pArea->security	= 0;
    pArea->builders	= str_dup( "None" );
    pArea->credits	= &str_empty[0];
    pArea->min_vnum	= 0;
    pArea->max_vnum	= 0;
    pArea->low_level	= 0;
    pArea->high_level	= 0;
    pArea->age		= 0;
    pArea->nplayer	= 0;
    pArea->vnum		= top_area - 1;
    pArea->color	= AT_AREA;
    pArea->exitsize	= SIZE_MEDIUM;
    pArea->world	= WORLD_ALL;
    pArea->file_name	= &str_empty[0];

    return pArea;
}


/* Stuff for recycling avatar data */
AVATAR_DATA *avatar_free;

AVATAR_DATA *
new_avatar_data( void )
{
    AVATAR_DATA *	pAd;

    if ( avatar_free == NULL )
    {
        pAd = (AVATAR_DATA *)alloc_perm( sizeof( *pAd ) );
        top_avatar++;
    }
    else
    {
        pAd = avatar_free;
        avatar_free = avatar_free->next;
    }

    memset( pAd, 0, sizeof( *pAd ) );

    VALIDATE( pAd );
    return pAd;
}

void
free_avatar_data( AVATAR_DATA *pAd )
{
    if ( !IS_VALID( pAd ) )
        return;

    INVALIDATE( pAd );
    pAd->next = avatar_free;
    avatar_free = pAd;
}


/* stuff for recycling ban structures */
BAN_DATA *ban_free;

BAN_DATA *
new_ban(void)
{
    static BAN_DATA ban_zero;
    BAN_DATA *ban;

    if (ban_free == NULL)
    {
	ban = (BAN_DATA *)alloc_perm(sizeof(*ban));
	top_ban++;
    }
    else
    {
	ban = ban_free;
	ban_free = ban_free->next;
    }

    *ban = ban_zero;
    VALIDATE(ban);
    ban->name = &str_empty[0];
    return ban;
}

void
free_ban(BAN_DATA *ban)
{
    if (!IS_VALID(ban))
	return;

    free_string(ban->name);
    INVALIDATE(ban);

    ban->next = ban_free;
    ban_free = ban;
}

/* stuff for recycling bank structures */
BANK_DATA *bank_free;

BANK_DATA *
new_bank( void )
{
    BANK_DATA *bank;

    if ( bank_free == NULL )
    {
	bank = (BANK_DATA *)alloc_perm( sizeof( *bank ) );
	top_bank++;
    }
    else
    {
	bank = bank_free;
	bank_free = bank->next;
    }

    memset( bank, 0, sizeof( *bank ) );
    VALIDATE( bank );

    return bank;
}

void
free_bank( BANK_DATA *bank )
{
    OBJ_DATA *	obj;
    OBJ_DATA *	obj_next;

    if ( !IS_VALID( bank ) )
	return;

    for ( obj = bank->storage; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->deleted )
	    continue;
	extract_obj( obj );
    }
    INVALIDATE( bank );
    bank->next = bank_free;
    bank_free = bank;
}


/* stuff for recycling boards */
BOARD_DATA *	board_free;

BOARD_DATA *
new_board( void )
{
    BOARD_DATA *	board;

    if ( board_free == NULL )
    {
	board = (BOARD_DATA *)alloc_perm( sizeof( *board ) );
	top_board++;
    }
    else
    {
	board = board_free;
	board_free = board_free->next;
    }
    memset( board, 0, sizeof( *board ) );

    board->name		= &str_empty[0];
    board->desc		= &str_empty[0];
    board->read		= 1;
    board->write	= 1;
    board->expire	= 14;

    VALIDATE( board );
    return board;
}

void
free_board( BOARD_DATA *board )
{
    if ( !IS_VALID( board ) )
	return;

    free_string( board->name );
    free_string( board->desc );
    INVALIDATE( board );
    board->next = board_free;
    board_free  = board;
    return;
}


/* stuff for recycling last board read info */
BOARD_INFO *board_info_free;

BOARD_INFO *
new_board_info( void )
{
    BOARD_INFO *	info;

    if ( board_info_free == NULL )
    {
	info = (BOARD_INFO *)alloc_perm( sizeof( *info ) );
	top_board_info++;
    }
    else
    {
	info = board_info_free;
	board_info_free = info->next;
    }

    memset( info, 0, sizeof( *info ) );
    info->name = &str_empty[0];

    VALIDATE( info );
    return info;
}

void
free_board_info( BOARD_INFO *info )
{
    if ( !IS_VALID( info ) )
	return;

    free_string( info->name );
    INVALIDATE( info );
    info->next = board_info_free;
    board_info_free = info;
    return;
}


/* stuff for recycling descriptors */
DESCRIPTOR_DATA *descriptor_free;

DESCRIPTOR_DATA *
new_descriptor( void )
{
    static DESCRIPTOR_DATA d_zero;
    DESCRIPTOR_DATA *d;

    if (descriptor_free == NULL)
    {
	d = (DESCRIPTOR_DATA *)alloc_perm( sizeof( *d ) );
	top_desc++;
    }
    else
    {
	d = descriptor_free;
	descriptor_free = descriptor_free->next;
    }

    *d = d_zero;
    VALIDATE( d );

    d->connected	= CON_GET_ANSI;
    d->host		= &str_empty[0];
    d->ip		= &str_empty[0];
    d->showstr_head	= NULL;
    d->showstr_point	= NULL;
    d->outsize		= 2000;
    d->outbuf		= alloc_mem( d->outsize );
    d->pEdit		= NULL;
    d->inEdit		= NULL;
    d->editor		= 0;
    d->editin		= 0;
    d->pString		= NULL;
    d->ansi		= TRUE;
    d->lastcolor	= C_DEFAULT;

    return d;
}

void
free_descriptor( DESCRIPTOR_DATA *d )
{
    if ( !IS_VALID( d ) )
	return;

    free_string( d->host );
    free_string( d->ip );
    free_mem( d->outbuf, d->outsize );

    /* Don't free *d->pString! */

    if ( d->showstr_head != NULL )
	free_mem( d->showstr_head, strlen( d->showstr_head ) + 1 );

    while ( d->event_first != NULL )
	dequeue_event( d->event_first );

    INVALIDATE( d );

    d->next = descriptor_free;
    descriptor_free = d;
}


/* Stuff for recycling dreams */
DREAM_DATA *dream_free;

DREAM_DATA *
new_dream( void )
{
    DREAM_DATA *	dream;

    if ( dream_free == NULL )
    {
        dream = (DREAM_DATA *)alloc_perm( sizeof( *dream ) );
        top_dream++;
    }
    else
    {
        dream = dream_free;
        dream_free = dream->next;
    }

    memset( dream, 0, sizeof( *dream ) );

    dream->sex    = SEX_EITHER;
    dream->author = &str_empty[0];
    dream->title  = &str_empty[0];

    return dream;
}

void
free_dream( DREAM_DATA *dream )
{
    DREAMSEG_DATA *	seg;

    free_string( dream->title );
    free_string( dream->author );
    while ( dream->seg != NULL )
    {
        seg = dream->seg;
        dream->seg = seg->next;
        free_dream_seg( seg );
    }

    dream->next = dream_free;
    dream_free = dream;

    return;
}



DREAMSEG_DATA *	dreamseg_free;

DREAMSEG_DATA *
new_dream_seg( void )
{
    DREAMSEG_DATA *	seg;

    if ( dreamseg_free == NULL )
    {
        seg	= (DREAMSEG_DATA *)alloc_perm( sizeof( *seg ) );
        top_dream_seg++;
    }
    else
    {
        seg = dreamseg_free;
        dreamseg_free = seg->next;
    }

    memset( seg, 0, sizeof( *seg ) );

    seg->text = &str_empty[0];
    return seg;
}

void
free_dream_seg( DREAMSEG_DATA *seg )
{
    free_string( seg->text );

    seg->next = dreamseg_free;
    dreamseg_free = seg;
    return;
}


DREAMLIST_DATA *dreamlist_free;

DREAMLIST_DATA *
new_dreamlist( void )
{
    DREAMLIST_DATA *	item;

    if ( dreamlist_free == NULL )
    {
        item	= (DREAMLIST_DATA *)alloc_perm( sizeof( *item ) );
        top_dreamlist++;
    }
    else
    {
        item = dreamlist_free;
        dreamlist_free = item->next;
    }

    memset( item, 0, sizeof( *item ) );

    return item;
}

void
free_dreamlist( DREAMLIST_DATA *item )
{
    item->next = dreamlist_free;
    dreamlist_free = item;
    return;
}


/* Stuff for events */
EVENT_DATA *event_free;

EVENT_DATA *
new_event( void )
{
    EVENT_DATA *event;

    if ( event_free == NULL )
    {
	event	= (EVENT_DATA *)alloc_perm( sizeof( *event ) );
	top_event++;
    }
    else
    {
	event		= event_free;
	event_free	= event_free->next_global;
    }

    memset( event, 0, sizeof( *event ) );
    event->argument	= &str_empty[0];

    top_event_used++;

    return event;
}

void
free_event( EVENT_DATA *event )
{
    free_string( event->argument );
    event->next_global = event_free;
    event_free = event;
    top_event_used--;
}


/* Stuff for recycling exits */
EXIT_DATA *exit_free;

EXIT_DATA *new_exit( void )
{
    EXIT_DATA *pExit;

    if ( !exit_free )
    {
        pExit           =   (EXIT_DATA *)alloc_perm( sizeof(*pExit) );
        top_exit++;
    }
    else
    {
        pExit           =   exit_free;
        exit_free       =   exit_free->next;
    }

    memset( pExit, 0, sizeof( *pExit ) );

    pExit->to_room      =   NULL;
    pExit->next         =   NULL;
    pExit->vnum         =   0;
    pExit->exit_info    =   0;
    pExit->key          =   0;
    pExit->keyword      =   &str_empty[0];
    pExit->description  =   &str_empty[0];
    pExit->sound_closed	=   &str_empty[0];
    pExit->sound_open	=   &str_empty[0];
    pExit->rs_flags     =   0;

    return pExit;
}

void free_exit( EXIT_DATA *pExit )
{
    free_string( pExit->keyword );
    free_string( pExit->description );
    free_string( pExit->sound_closed );
    free_string( pExit->sound_open );

    while ( pExit->event_first != NULL )
        dequeue_event( pExit->event_first );

    pExit->next = exit_free;
    exit_free = pExit;
    return;
}



/* stuff for recycling extended descs */
EXTRA_DESCR_DATA *extra_descr_free;

EXTRA_DESCR_DATA *
new_extra_descr( void )
{
    EXTRA_DESCR_DATA *ed;

    if ( extra_descr_free == NULL )
    {
	ed = (EXTRA_DESCR_DATA *)alloc_perm( sizeof( *ed ) );
	top_ed++;
    }
    else
    {
	ed = extra_descr_free;
	extra_descr_free = extra_descr_free->next;
    }

    ed->keyword = &str_empty[0];
    ed->description = &str_empty[0];
    VALIDATE( ed );
    return ed;
}


void
free_extra_descr( EXTRA_DESCR_DATA *ed )
{
    if ( !IS_VALID( ed ) )
	return;

    free_string( ed->keyword );
    free_string( ed->description );
    INVALIDATE( ed);

    ed->next = extra_descr_free;
    extra_descr_free = ed;
}


/* stuff for recycling gen_data */
GEN_DATA *gen_data_free;

GEN_DATA *
new_gen_data( void )
{
    GEN_DATA *gen;

    if ( gen_data_free == NULL )
	gen = (GEN_DATA *)alloc_perm(sizeof(*gen));
    else
    {
	gen = gen_data_free;
	gen_data_free = gen_data_free->next;
    }

    memset( gen, 0, sizeof( *gen ) );
    gen->skill_chosen = alloc_mem( top_skill * sizeof( *gen->skill_chosen ) );
    memset( gen->skill_chosen, 0, top_skill * sizeof( *gen->skill_chosen ) );

    VALIDATE( gen );
    return gen;
}

void
free_gen_data( GEN_DATA *gen )
{
    if ( !IS_VALID( gen ) )
	return;

    INVALIDATE( gen );

    free_mem( gen->skill_chosen, top_skill * sizeof( *gen->skill_chosen ) );
    gen->next = gen_data_free;
    gen_data_free = gen;
} 


HELP_DATA *help_free;

/* stuff for creating helps */
HELP_DATA *
new_help( void )
{
    HELP_DATA *pHelp;

    if ( help_free == NULL )
	pHelp	   = (HELP_DATA *)alloc_perm( sizeof( *pHelp ) );
    else
    {
	pHelp = help_free;
	help_free = pHelp->next;
    }
    pHelp->next    = NULL;
    pHelp->area    = NULL;
    pHelp->keyword = &str_empty[0];
    pHelp->level   = 1;
    pHelp->text    = &str_empty[0];
    top_help++;
    return pHelp;
}

void
free_help( HELP_DATA *pHelp )
{
    free_string( pHelp->keyword );
    free_string( pHelp->text );
    pHelp->next = help_free;
    help_free = pHelp;
}


/* Stuff for ignore lists */
IGNORE_DATA *ignore_free;

IGNORE_DATA *
new_ignore_data( void )
{
    IGNORE_DATA *	pId;

    if ( ignore_free == NULL )
    {
        pId	= (IGNORE_DATA *)alloc_perm( sizeof( *pId ) );
        top_ignore++;
    }
    else
    {
        pId = ignore_free;
        ignore_free = pId->next;
    }
    memset( pId, 0, sizeof( *pId ) );
    pId->name	= &str_empty[0];
    pId->flags	= IGNORE_SET;
    VALIDATE( pId );
    return pId;
}

void
free_ignore_data( IGNORE_DATA *pId )
{
    if ( !IS_VALID( pId ) )
        return;

    INVALIDATE( pId );
    free_string( pId->name );
    pId->next = ignore_free;
    ignore_free = pId;
}


/* Stuff for mobprogs */
MPROG_DATA *mprog_free;

MPROG_DATA *
new_mprog_data( void )
{
    MPROG_DATA *pMProg;

    if ( !mprog_free )
    {
	pMProg = (MPROG_DATA *)alloc_perm( sizeof(*pMProg) );
	top_mprog++;
    }
    else
    {
	pMProg = mprog_free;
	mprog_free = mprog_free->next;
    }

    pMProg->next     = NULL;
    pMProg->type     = MP_ACT_PROG;
    pMProg->arglist  = str_dup( "" );
    pMProg->comlist  = str_dup( "" );
    return pMProg;
}


void
free_mprog_data( MPROG_DATA *pMProg )
{
    free_string( pMProg->arglist );
    free_string( pMProg->comlist );

    pMProg->next = mprog_free;
    mprog_free = pMProg;
}


MPROG_ACT_LIST *	mpact_free;

MPROG_ACT_LIST *
new_mpact_data( void )
{
    MPROG_ACT_LIST *	mpact;

    if ( mpact_free == NULL )
    {
        mpact = (MPROG_ACT_LIST *)alloc_perm( sizeof( *mpact ) );
        top_mpact++;
    }
    else
    {
        mpact = mpact_free;
        mpact_free = mpact->next;
    }
    memset( mpact, 0, sizeof( *mpact ) );
    return mpact;
}

void
free_mpact_data( MPROG_ACT_LIST *mpact )
{
    free_string( mpact->buf );
    mpact->next = mpact_free;
    mpact_free = mpact;
}


/* stuff for recyling notes */
NOTE_DATA *note_free;

NOTE_DATA *
new_note( )
{
    NOTE_DATA *note;

    if ( note_free == NULL )
    {
	note = (NOTE_DATA *)alloc_perm( sizeof( *note ) );
	top_note++;
    }
    else
    { 
	note = note_free;
	note_free = note_free->next;
    }
    memset( note, 0, sizeof( *note ) );
    note->date		= &str_empty[0];
    note->sender	= &str_empty[0];
    note->subject	= &str_empty[0];
    note->to_list	= &str_empty[0];
    note->text		= &str_empty[0];
    VALIDATE( note );
    return note;
}

void
free_note( NOTE_DATA *note )
{
    if ( !IS_VALID( note ) )
	return;

    free_string( note->text    );
    free_string( note->subject );
    free_string( note->to_list );
    free_string( note->date    );
    free_string( note->sender  );
    INVALIDATE( note );

    note->next = note_free;
    note_free   = note;
}


/* stuff for recycling objects */
OBJ_DATA *obj_free;

OBJ_DATA *
new_obj( void )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;

    if (obj_free == NULL)
	obj = (OBJ_DATA *)alloc_perm( sizeof( *obj ) );
    else
    {
	obj = obj_free;
	obj_free = obj_free->next;
    }
    *obj = obj_zero;
    VALIDATE( obj );
    obj->stype = STYPE_OBJ;

    return obj;
}

void
free_obj( OBJ_DATA *obj )
{
    AFFECT_DATA *paf, *paf_next;
    EXTRA_DESCR_DATA *ed, *ed_next;

    if ( !IS_VALID( obj ) )
	return;

    object_count--;

    for ( paf = obj->affected; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	free_affect(paf);
    }
    obj->affected = NULL;

    for ( ed = obj->extra_descr; ed != NULL; ed = ed_next )
    {
	ed_next = ed->next;
	free_extra_descr( ed );
     }
     obj->extra_descr = NULL;

    free_string( obj->name        );
    free_string( obj->description );
    free_string( obj->short_descr );
    free_string( obj->owner     );

    while ( obj->event_first != NULL )
	dequeue_event( obj->event_first );

    INVALIDATE( obj );

    obj->next   = obj_free;
    obj_free    = obj; 
}


OBJ_INDEX_DATA * obj_index_free;

OBJ_INDEX_DATA *
new_obj_index( void )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( !obj_index_free )
    {
        pObj		=   (OBJ_INDEX_DATA *)alloc_perm( sizeof(*pObj) );
        top_obj_index++;
    }
    else
    {
        pObj		=   obj_index_free;
        obj_index_free	=   obj_index_free->next;
    }

    pObj->next          =   NULL;
    pObj->extra_descr   =   NULL;
    pObj->affected      =   NULL;
    pObj->area          =   NULL;
    pObj->name          =   str_dup( "no name" );
    pObj->short_descr   =   str_dup( "(no short description)" );
    pObj->description   =   str_dup( "(no description)" );
    pObj->vnum          =   0;
    pObj->item_type     =   ITEM_TRASH;
    pObj->extra_flags   =   0;
    pObj->wear_flags    =   0;
    pObj->count         =   0;
    pObj->weight        =   0;
    pObj->cost          =   0;
    pObj->qcost         =   0;
    for ( value = 0; value < 5; value++ )
        pObj->value[value]  =   0;
    pObj->join		=   0;
    pObj->sep_one	=   0;
    pObj->sep_two	=   0;
    pObj->ac_type	=   0;
    pObj->ac_vnum	=   0;
    pObj->ac_charge[0]	=   0;
    pObj->ac_charge[1]  =   0;

    return pObj;
}

void
free_obj_index( OBJ_INDEX_DATA *pObj )
{
    EXTRA_DESCR_DATA *	pExtra;
    AFFECT_DATA *	pAf;
    TEXT_DATA *		page;
    OREPROG_DATA *	pProg;

    free_string( pObj->name );
    free_string( pObj->short_descr );
    free_string( pObj->description );

    for ( pAf = pObj->affected; pAf; pAf = pAf->next )
    {
        free_affect( pAf );
    }

    for ( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra->next )
    {
        free_extra_descr( pExtra );
    }

    while ( pObj->page != NULL )
    {
        page = pObj->page;
        pObj->page = page->next;
        free_textlist( page );
    }

    while ( pObj->oprogs != NULL )
    {
        pProg = pObj->oprogs;
        pObj->oprogs = pProg->next;
        free_oreprog_data( pProg );
    }

    pObj->next = obj_index_free;
    obj_index_free = pObj;
    return;
}


/* Stuff for recycling o/r/eprog data */
OREPROG_DATA *oreprog_free;

OREPROG_DATA *
new_oreprog_data( void )
{
    OREPROG_DATA *pProg;

    if ( oreprog_free )
    {
	pProg = oreprog_free;
	oreprog_free = oreprog_free->next;
    }
    else
    {
	pProg = (OREPROG_DATA *)alloc_perm( sizeof( *pProg ) );
	top_oreprog++;
    }
    pProg->arglist = str_dup( "100" );
    pProg->comlist = &str_empty[0];    pProg->next = NULL;
    return pProg;
}

void
free_oreprog_data( OREPROG_DATA *pProg )
{
    free_string( pProg->arglist );
    free_string( pProg->comlist );
    pProg->next = oreprog_free;
    oreprog_free = pProg;
}


/* Overlands (virtual room support) stuff */
OVERLAND_DATA *overland_free;

OVERLAND_DATA *
new_overland( int x, int y )
{
    OVERLAND_DATA *	pOverland;
    VIRTUAL_ROOM_DATA *	pMap;
    int			size;
    int			i;

    size = x * y * sizeof( *pMap );
    pMap = malloc( size );
    if ( pMap == NULL )
    {
	bug( "New_overland: unable to allocate space for map of size %d", size );
	return NULL;
    }
    memset( pMap, 0, sizeof( *pMap ) );

    if ( overland_free != NULL )
    {
	pOverland = overland_free;
	overland_free = pOverland->next;
    }
    else
    {
	pOverland = (OVERLAND_DATA *)alloc_perm( sizeof( *pOverland ) );
    }

    memset( pOverland, 0, sizeof( *pOverland ) );
    pOverland->x = x;
    pOverland->y = y;
    pOverland->map = pMap;
    pOverland->room_first = NULL;
    pOverland->room_last = NULL;

    for ( i = 0; i < SECT_MAX; i++ )
    {
        pOverland->sect_desc[i][DESC_DESC]  = &str_empty[0];
        pOverland->sect_desc[i][DESC_MORN]  = &str_empty[0];
        pOverland->sect_desc[i][DESC_EVEN]  = &str_empty[0];
        pOverland->sect_desc[i][DESC_NIGHT] = &str_empty[0];
    }

    top_overland++;
    map_mem += size;

    return pOverland;
}

void
free_overland( OVERLAND_DATA *overland )
{
    ROOM_INDEX_DATA *	pRoom;
    size_t		size;
    int			i;

    size = overland->x * overland->y * sizeof( VIRTUAL_ROOM_DATA );
    if ( size <= 0 )
    {
	bug( "Free_overland: attemp to free map of size %d.", size );
    }
    else
    {
	free( overland->map );
	map_mem -= size;
    }
    while ( ( pRoom = overland->room_first ) != NULL )
    {
	overland->room_first = pRoom->next;
	free_room_index( pRoom );
	top_vroom--;
    }
    overland->x = 0;
    overland->y = 0;
    overland->area = NULL;

    for ( i = 0; i < SECT_MAX; i++ )
    {
        free_string( overland->sect_desc[i][DESC_DESC]  );
        free_string( overland->sect_desc[i][DESC_MORN]  );
        free_string( overland->sect_desc[i][DESC_EVEN]  );
        free_string( overland->sect_desc[i][DESC_NIGHT] );
    }

    overland->room_last = NULL;
    overland->next = overland_free;
    overland_free = overland;
}


/* Stuff for recycling resets */
RESET_DATA *reset_free;

RESET_DATA *
new_reset_data( void )
{
    RESET_DATA *pReset;

    if ( reset_free )
    {
	pReset = reset_free;
	reset_free = pReset->next;
    }
    else
    {
	pReset = (RESET_DATA *)alloc_perm( sizeof( *pReset ) );
	top_reset++;
    }

    pReset->next	= NULL;
    pReset->command	= 'X';
    pReset->arg1	= 0;
    pReset->arg2	= 0;
    pReset->arg3	= 0;
    pReset->arg4	= 0;
    return pReset;
}

void free_reset_data( RESET_DATA *pReset )
{
    pReset->next = reset_free;
    reset_free = pReset;
    return;
}

/* Stuff for recycling rooms index data */
ROOM_INDEX_DATA *room_index_free;

ROOM_INDEX_DATA *
new_room_index( void )
{
    ROOM_INDEX_DATA *pRoom;
    int door;

    if ( !room_index_free )
    {
	pRoom		=   (ROOM_INDEX_DATA *)alloc_perm( sizeof( *pRoom ) );
	top_room++;
    }
    else
    {
	pRoom		=   room_index_free;
	room_index_free =   room_index_free->next;
    }

    pRoom->next             =   NULL;
    pRoom->prev		    =   NULL;
    pRoom->people           =   NULL;
    pRoom->contents         =   NULL;
    pRoom->extra_descr      =   NULL;
    pRoom->area             =   NULL;

    for ( door=0; door < MAX_DIR; door++ )
        pRoom->exit[door]   =   NULL;

    pRoom->name             =   &str_empty[0];
    pRoom->description      =   &str_empty[0];
    pRoom->morning_desc	    =	&str_empty[0];
    pRoom->evening_desc	    =	&str_empty[0];
    pRoom->night_desc	    =	&str_empty[0];
    pRoom->sound	    =	&str_empty[0];
    pRoom->smell	    =	&str_empty[0];
    pRoom->short_descr	    =	&str_empty[0];
    pRoom->vnum             =   0;
    pRoom->room_flags       =   0;
    pRoom->light            =   0;
    pRoom->sector_type      =   0;
    pRoom->heal_rate	    =   100;
    pRoom->mana_rate	    =   100;

    return pRoom;
}

void
free_room_index( ROOM_INDEX_DATA *pRoom )
{
    int			door;
    RESET_DATA *	pReset;
    EXTRA_DESCR_DATA *	ed;
    OREPROG_DATA *	prog;

    pRoom->prev = NULL;
    free_string( pRoom->name );
    free_string( pRoom->description );
    free_string( pRoom->morning_desc );
    free_string( pRoom->evening_desc );
    free_string( pRoom->night_desc );
    free_string( pRoom->sound );
    free_string( pRoom->smell );
    for ( door = 0; door < MAX_DIR; door++ )
	if ( pRoom->exit[door] != NULL )
	{
	    free_exit( pRoom->exit[door] );
	    pRoom->exit[door] = NULL;
	}

    while ( pRoom->event_first != NULL )
	dequeue_event( pRoom->event_first );

    while ( ( pReset = pRoom->reset_first ) != NULL )
    {
        pRoom->reset_first = pReset->next;
        free_reset_data( pReset );
    }

    while ( ( ed = pRoom->extra_descr ) != NULL )
    {
        pRoom->extra_descr = ed->next;
        free_extra_descr( ed );
    }

    while ( ( prog = pRoom->rprogs ) != NULL )
    {
        pRoom->rprogs = prog->next;
        free_oreprog_data( prog );
    }

    pRoom->next = room_index_free;
    room_index_free = pRoom;
}


/* Stuff for recycling shops */
SHOP_DATA *shop_free;

SHOP_DATA *new_shop( void )
{
    SHOP_DATA *pShop;
    int buy;

    if ( !shop_free )
    {
        pShop		=   (SHOP_DATA *)alloc_perm( sizeof(*pShop) );
        top_shop++;
    }
    else
    {
        pShop		=   shop_free;
        shop_free	=   shop_free->next;
    }

    pShop->next		=   NULL;
    pShop->keeper	=   0;

    for ( buy=0; buy<MAX_TRADE; buy++ )
        pShop->buy_type[buy]    =   0;

    pShop->profit_buy   =   100;
    pShop->profit_sell  =   75;
    pShop->open_hour    =   0;
    pShop->close_hour   =   23;

    return pShop;
}



void free_shop( SHOP_DATA *pShop )
{
    pShop->next = shop_free;
    shop_free = pShop;
    return;
}


/* stuff for recycling travel structs */
TRAVEL_DATA *travel_free;

TRAVEL_DATA *
new_travel( void )
{
    TRAVEL_DATA *pTravel;

    if ( travel_free != NULL )
    {
	pTravel = travel_free;
	travel_free = pTravel->next;
    }
    else
    {
	pTravel = (TRAVEL_DATA *)alloc_perm( sizeof( *pTravel ) );
	top_travel++;
    }

    memset( pTravel, 0, sizeof( *pTravel ) );

    pTravel->arrive_travel	= &str_empty[0];
    pTravel->arrive_room	= &str_empty[0];
    pTravel->depart_travel	= &str_empty[0];
    pTravel->depart_room	= &str_empty[0];
    pTravel->stopped		= FALSE;

    return pTravel;
}


void
free_travel( TRAVEL_DATA *pTravel )
{
    free_string( pTravel->arrive_travel );
    free_string( pTravel->arrive_room );
    free_string( pTravel->depart_travel );
    free_string( pTravel->depart_room );
    pTravel->next = travel_free;
    travel_free = pTravel;
}


/* stuff for recyling characters */
CHAR_DATA *char_free;

CHAR_DATA *
new_char( void )
{
    static CHAR_DATA ch_zero;
    CHAR_DATA *ch;
    int i;

    if ( char_free == NULL )
    {
	ch = (CHAR_DATA *)alloc_perm( sizeof( *ch ) );
	top_char++;
    }
    else
    {
	ch = char_free;
	char_free = char_free->next;
    }

    *ch				= ch_zero;
    VALIDATE(ch);
    ch->stype			= STYPE_CHAR;
    ch->name                    = &str_empty[0];
    ch->short_descr             = &str_empty[0];
    ch->long_descr              = &str_empty[0];
    ch->description             = &str_empty[0];
    ch->prompt                  = &str_empty[0];
    ch->prefix			= &str_empty[0];
    ch->logon                   = current_time;
    ch->lines                   = PAGELEN;

    /*
     * Set up starting stats.  Note that for new PC's, these stats
     * get tweaked a bit in case CON_READ_MOTD in nanny().
     */
    for (i = 0; i < 4; i++)
        ch->armor[i]            = 100;
    ch->position                = POS_STANDING;
    ch->hit                     = 20;
    ch->max_hit                 = 20;
    ch->mana                    = 100;
    ch->max_mana                = 100;
    ch->move                    = 200;
    ch->max_move                = 200;
    for (i = 0; i < MAX_STATS; i ++)
    {
        ch->perm_stat[i] = 13;
        ch->mod_stat[i] = 0;
    }
    ch->from_dir		= DIR_NONE;

    return ch;
}


void
free_char( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    if (!IS_VALID(ch))
	return;

    if (IS_NPC(ch))
	mobile_count--;

    for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
	obj_next = obj->next_content;
	extract_obj(obj);
    }

    for (paf = ch->affected; paf != NULL; paf = paf_next)
    {
	paf_next = paf->next;
	affect_remove(ch,paf);
    }

    free_string(ch->name);
    free_string(ch->short_descr);
    free_string(ch->long_descr);
    free_string(ch->description);
    free_string(ch->prompt);
    free_string(ch->prefix);
    free_note  (ch->pnote);
    free_pcdata(ch->pcdata);

    while ( ch->event_first != NULL )
	dequeue_event( ch->event_first );

    ch->next = char_free;
    char_free  = ch;

    INVALIDATE(ch);
    return;
}


/* Clan stuff */
CLAN_DATA *clan_free;

CLAN_DATA *
new_clan_data( void )
{
    CLAN_DATA * pClan;
    int		i;

    if ( clan_free )
    {
	pClan = clan_free;
	clan_free = clan_free->next;
    }
    else
    {
	pClan = (CLAN_DATA *)alloc_perm( sizeof( *pClan ) );
    }

    memset( pClan, 0, sizeof( *pClan ) );
    pClan->name		= &str_empty[0];
    pClan->who_name	= &str_empty[0];
    pClan->description	= &str_empty[0];
    pClan->leader	= &str_empty[0];
    for ( i = 0; i < MAX_CLAN_LEVELS; i++ )
    {
	pClan->ctitle[i][0]=&str_empty[0];
	pClan->ctitle[i][1]=&str_empty[0];
    }
    pClan->members	= 0;
    pClan->pkills	= 0;
    pClan->plosses	= 0;
    pClan->pkill	= TRUE;
    pClan->fHouse	= FALSE;
    pClan->recall	= 0;
    pClan->bank		= 0;
    pClan->honor	= 0;
    pClan->vnum		= ++top_clan;
    pClan->next		= NULL;

    return pClan;
}

void
free_clan_data( CLAN_DATA *pClan )
{
    int i;

    free_string( pClan->name );
    free_string( pClan->who_name );
    free_string( pClan->description );
    free_string( pClan->leader );
    for ( i = 0; i < MAX_CLAN_LEVELS; i++ )
    {
	free_string( pClan->ctitle[i][0] );
	free_string( pClan->ctitle[i][1] );
    }
    pClan->next = clan_free;
    clan_free = pClan;
}


/* character lists */
CLIST_DATA *clist_free;

CLIST_DATA *new_clist_data( void )
{
    CLIST_DATA *pList;

    if ( clist_free )
    {
	pList = clist_free;
	clist_free = pList->next;
    }
    else
    {
	pList = (CLIST_DATA *)alloc_perm( sizeof( *pList ) );
	top_clist++;
    }

    return pList;
}

void free_clist_data( CLIST_DATA *pList )
{
    pList->next = clist_free;
    clist_free = pList;
}


/* stuff for setting ids */
long	last_pc_id;
long	last_mob_id;

long get_pc_id(void)
{
    int val;

    val = (current_time <= last_pc_id) ? last_pc_id + 1 : current_time;
    last_pc_id = val;
    return val;
}

long get_mob_id(void)
{
    last_mob_id++;
    return last_mob_id;
}


/* Mob index lists */
MILIST_DATA *milist_free;

MILIST_DATA *
new_milist_data( void )
{
    MILIST_DATA *	pList;
    if ( milist_free == NULL )
    {
        pList = (MILIST_DATA *)alloc_perm( sizeof( *pList ) );
        top_milist++;
    }
    else
    {
        pList = milist_free;
        milist_free = milist_free->next;
    }
    pList->next = NULL;
    pList->lch = NULL;
    return pList;
}

void
free_milist_data ( MILIST_DATA *pList )
{
    pList->next = milist_free;
    milist_free = pList;
}


/* Mob index stuff */
MOB_INDEX_DATA	*mob_index_free;

MOB_INDEX_DATA *
new_mob_index ( void )
{
    MOB_INDEX_DATA *pMob;

    if ( !mob_index_free )
    {
        pMob		=   (MOB_INDEX_DATA *)alloc_perm( sizeof(*pMob) );
        top_mob_index++;
    }
    else
    {
        pMob		=   mob_index_free;
        mob_index_free	=   mob_index_free->next;
    }

    memset( pMob, 0, sizeof( *pMob ) );

    pMob->next          =   NULL;
    pMob->spec_fun      =   NULL;
    pMob->pShop         =   NULL;
    pMob->area          =   NULL;
    pMob->player_name   =   str_dup( "no name" );
    pMob->short_descr   =   str_dup( "(no short description)" );
    pMob->long_descr    =   str_dup( "(no long description)\n\r" );
    pMob->description   =   &str_empty[0];
    pMob->clan		=   &str_empty[0];
    pMob->vnum          =   0;
    pMob->material	=   &str_empty[0];
    pMob->autoset	=   DICE_MEDIUM;

    /* Need more initialization stuff here */

    return pMob;

}

void
free_mob_index( MOB_INDEX_DATA *pMob )
{
    MPROG_DATA *	pProg;

    free_string( pMob->player_name );
    free_string( pMob->short_descr );
    free_string( pMob->long_descr );
    free_string( pMob->description );
    free_string( pMob->material );

    free_shop( pMob->pShop );

    while( pMob->mobprogs != NULL )
    {
        pProg = pMob->mobprogs;
        pMob->mobprogs = pProg->next;
        free_mprog_data( pProg );
    }

    pMob->next = mob_index_free;
    mob_index_free = pMob;

    return;
}


/* Obj index lists */
OILIST_DATA *oilist_free;

OILIST_DATA *
new_oilist_data( void )
{
    OILIST_DATA *	pList;
    if ( oilist_free == NULL )
    {
        pList = (OILIST_DATA *)alloc_perm( sizeof( *pList ) );
        top_oilist++;
    }
    else
    {
        pList = oilist_free;
        oilist_free = oilist_free->next;
    }
    pList->next = NULL;
    pList->o = NULL;
    return pList;
}

void
free_oilist_data ( OILIST_DATA *pList )
{
    pList->next = oilist_free;
    oilist_free = pList;
}


PC_DATA *pcdata_free;

PC_DATA *
new_pcdata( void )
{
    PC_DATA *	pcdata;
    int		index;

    if ( pcdata_free == NULL )
    {
	pcdata = (PC_DATA *)alloc_perm( sizeof( *pcdata ) );
	top_pcdata++;
    }
    else
    {
	pcdata = pcdata_free;
	pcdata_free = pcdata_free->next;
    }

    memset( pcdata, 0, sizeof( *pcdata ) );
    pcdata->skill = (struct skill_data *)alloc_mem( top_skill * sizeof( *pcdata->skill ) );
    memset( pcdata->skill, 0, top_skill * sizeof( *pcdata->skill ) );

    pcdata->bamfin	  = &str_empty[0];
    pcdata->bamfout	  = &str_empty[0];
    pcdata->lname	  = &str_empty[0];
    pcdata->pwd		  = &str_empty[0];
    pcdata->title	  = &str_empty[0];
    pcdata->hair_color	  = &str_empty[0];
    pcdata->hair_adj    = &str_empty[0];
    pcdata->eye_color	  = &str_empty[0];
    pcdata->eye_adj     = &str_empty[0];
    pcdata->who_text	  = &str_empty[0];
    pcdata->empower	  = &str_empty[0];
    pcdata->detract	  = &str_empty[0];
    pcdata->current_board = &str_empty[0];
    pcdata->pose	  = &str_empty[0];

    for ( index = 0; index < TELLBUF_SIZE; index++ )
        pcdata->tell_buf[index] = NULL;

    pcdata->buffer = new_buf( );

    VALIDATE( pcdata );
    return pcdata;
}

void
free_pcdata( PC_DATA *pcdata )
{
    BANK_DATA *		bank;
    BANK_DATA *		bank_next;
    ALIAS_DATA *	pAlias;
    ALIAS_DATA *	alias_next;
    IGNORE_DATA *	pId;
    BOARD_INFO *	info;
    int			index;

    if ( !IS_VALID( pcdata ) )
	return;

    free_string( pcdata->bamfin );
    free_string( pcdata->bamfout );
    free_string( pcdata->lname );
    free_string( pcdata->pwd );
    free_string( pcdata->title );
    free_string( pcdata->hair_color );
    free_string( pcdata->eye_color );
    free_string( pcdata->who_text );
    free_string( pcdata->empower );
    free_string( pcdata->detract );
    free_string( pcdata->current_board );
    free_string( pcdata->pose );
    free_buf( pcdata->buffer );
    free_mem( pcdata->skill, top_skill * sizeof( *pcdata->skill ) );

    free_avatar_data( pcdata->avatar );

    for ( pAlias = pcdata->alias_list; pAlias != NULL; pAlias = alias_next )
    {
	alias_next = pAlias->next;
	free_alias( pAlias );
    }

    for ( bank = pcdata->bank; bank != NULL; bank = bank_next )
    {
	bank_next = bank->next;
	free_bank( bank );
    }

    while ( pcdata->last_read != NULL )
    {
	info = pcdata->last_read;
	pcdata->last_read = info->next;
	free_board_info( info );
    }

    while ( pcdata->ignoring != NULL )
    {
        pId = pcdata->ignoring;
        pcdata->ignoring = pId->next;
        free_ignore_data( pId );
    }

    for ( index = 0; index < TELLBUF_SIZE; index++ )
        free_string( pcdata->tell_buf[index] );

    INVALIDATE( pcdata );
    pcdata->next = pcdata_free;
    pcdata_free = pcdata;

    return;
}


PROJECT_DATA *	project_free;

PROJECT_DATA *
new_project( void )
{
    PROJECT_DATA *	pProj;

    if ( project_free == NULL )
    {
	pProj = (PROJECT_DATA *)alloc_perm( sizeof( *pProj ) );
	top_project++;
    }
    else
    {
	pProj = project_free;
	project_free = pProj->next;
    }

    memset( pProj, 0, sizeof( *pProj ) );

    pProj->name		= &str_empty[0];
    pProj->priority	= PRI_MEDIUM;
    pProj->vnum		= 0;
    pProj->assigner	= &str_empty[0];
    pProj->assignee	= &str_empty[0];
    pProj->date		= 0;
    pProj->deadline	= 0;
    pProj->completed	= 0;
    pProj->status	= &str_empty[0];
    pProj->description	= &str_empty[0];
    pProj->progress	= &str_empty[0];

    return pProj;
}

void
free_project( PROJECT_DATA *pProj )
{
    free_string( pProj->name );
    free_string( pProj->assigner );
    free_string( pProj->assignee );
    free_string( pProj->status );
    free_string( pProj->description );
    free_string( pProj->progress );

    pProj->next  = project_free;
    project_free = pProj->next;

    return;
}


SOCIAL_DATA	*social_free;

SOCIAL_DATA *
new_social( void )
{
    SOCIAL_DATA *pSocial;

    if ( social_free == NULL )
    {
	pSocial = (SOCIAL_DATA *)alloc_perm( sizeof( *pSocial ) );
	top_social++;
    }
    else
    {
	pSocial = social_free;
	social_free = social_free->next;
    }

    pSocial->next = NULL;
    pSocial->cnoarg	= &str_empty[0];
    pSocial->onoarg	= &str_empty[0];
    pSocial->cfound	= &str_empty[0];
    pSocial->ofound	= &str_empty[0];
    pSocial->vfound	= &str_empty[0];
    pSocial->nfound	= &str_empty[0];
    pSocial->cself	= &str_empty[0];
    pSocial->oself	= &str_empty[0];
    pSocial->ed_level	= LEVEL_IMMORTAL;
    pSocial->deleted	= FALSE;

    return( pSocial );
}

void
free_social( SOCIAL_DATA *pSocial )
{
    free_string( pSocial->cnoarg );
    free_string( pSocial->onoarg );
    free_string( pSocial->cfound );
    free_string( pSocial->ofound );
    free_string( pSocial->vfound );
    free_string( pSocial->nfound );
    free_string( pSocial->cself );
    free_string( pSocial->oself );

    pSocial->next = social_free;
    social_free = pSocial;
}


/* Generic string list stuff */
STRLIST_DATA *strlist_free;

STRLIST_DATA *
new_strlist( void )
{
    STRLIST_DATA *	strlist;

    if ( strlist_free == NULL )
	strlist = (STRLIST_DATA *)alloc_perm( sizeof( *strlist ) );
    else
    {
	strlist = strlist_free;
	strlist_free = strlist->next;
    }

    strlist->next = NULL;
    strlist->str = &str_empty[0];
    strlist->len = 0;
    return strlist;
}

void
free_strlist( STRLIST_DATA *strlist )
{
    free_string( strlist->str );
    strlist->next = strlist_free;
    strlist_free = strlist;
}


/* Generic lists of text */
TEXT_DATA *textlist_free;

TEXT_DATA *
new_textlist( void )
{
    TEXT_DATA *	textlist;

    if ( textlist_free == NULL )
    {
	textlist = (TEXT_DATA *)alloc_perm( sizeof( *textlist ) );
	top_textlist++;
    }
    else
    {
	textlist = textlist_free;
	textlist_free = textlist->next;
    }

    textlist->next = NULL;
    textlist->text = &str_empty[0];
    return textlist;
}

void
free_textlist( TEXT_DATA *textlist )
{
    free_string( textlist->text );
    textlist->next = textlist_free;
    textlist_free = textlist;
}


/* Procedures for recycling user list stuff */
USERLIST_DATA *userlist_free;

USERLIST_DATA *
new_user_rec( void )
{
    USERLIST_DATA *	pUser;

    if ( userlist_free == NULL )
    {
	pUser = (USERLIST_DATA *)alloc_perm( sizeof( *pUser ) );
	top_userlist++;
    }
    else
    {
	pUser = userlist_free;
	userlist_free = pUser->next;
    }

    memset( pUser, 0, sizeof( *pUser ) );
    VALIDATE( pUser );
    pUser->name 	= &str_empty[0];
    pUser->host		= &str_empty[0];
    pUser->race         = &str_empty[0];
    pUser->class        = &str_empty[0];
    pUser->lastlogon	= 0;

    return pUser;
}

void
free_user_rec( USERLIST_DATA *pUser )
{
    if ( !IS_VALID( pUser ) )
	return;

    free_string( pUser->name );
    free_string( pUser->host );
    free_string( pUser->race );
    free_string( pUser->class );
    INVALIDATE( pUser );
    pUser->next = userlist_free;
    userlist_free = pUser;
}


/* procedures and constants needed for buffering */

MEM_DATA *mem_data_free;

BUFFER *buf_free;

MEM_DATA *
new_mem_data( void )
{
    MEM_DATA *memory;

    if (mem_data_free == NULL)
	memory = (MEM_DATA *)alloc_mem(sizeof(*memory));
    else
    {
	memory = mem_data_free;
	mem_data_free = mem_data_free->next;
    }

    memory->next = NULL;
    memory->id = 0;
    memory->reaction = 0;
    memory->when = 0;
    VALIDATE(memory);

    return memory;
}

void
free_mem_data(MEM_DATA *memory)
{
    if (!IS_VALID(memory))
	return;

    memory->next = mem_data_free;
    mem_data_free = memory;
    INVALIDATE(memory);
}


/* buffer sizes */
const int buf_size[MAX_BUF_LIST] =
{
    16,32,64,128,256,1024,2048,4096,8192,16384
};

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
int
get_size ( int val )
{
    int i;

    for ( i = 0; i < MAX_BUF_LIST; i++ )
	if ( buf_size[i] >= val )
	{
	    return buf_size[i];
	}

    return -1;
}


BUFFER *
new_buf( )
{
    BUFFER *buffer;

    if ( buf_free == NULL ) 
    {
	buffer = (BUFFER *)alloc_perm( sizeof( *buffer ) );
	top_buffer++;
    }
    else
    {
	buffer = buf_free;
	buf_free = buf_free->next;
    }

    buffer->next	= NULL;
    buffer->state	= BUFFER_SAFE;
    buffer->size	= get_size( BASE_BUF );

    buffer->string	= alloc_mem( buffer->size );
    buffer->string[0]	= '\0';
    buffer->length	= 0;
    VALIDATE( buffer );

    return buffer;
}


BUFFER *
new_buf_size( int size )
{
    BUFFER *buffer;

    if ( buf_free == NULL )
        buffer = (BUFFER *)alloc_perm( sizeof( *buffer ) );
    else
    {
        buffer = buf_free;
        buf_free = buf_free->next;
    }

    buffer->next        = NULL;
    buffer->state       = BUFFER_SAFE;
    buffer->size        = get_size( size );
    if ( buffer->size == -1 )
    {
        bug( "new_buf: buffer size %d too large.", size );
        exit( 1 );
    }
    buffer->string      = alloc_mem( buffer->size );
    buffer->string[0]   = '\0';
    buffer->length	= 0;
    VALIDATE( buffer );

    return buffer;
}


void
free_buf( BUFFER *buffer )
{
    if ( !IS_VALID(buffer ) )
	return;

    free_mem( buffer->string, buffer->size );
    buffer->string = NULL;
    buffer->size   = 0;
    buffer->state  = BUFFER_FREED;
    INVALIDATE( buffer );

    buffer->next  = buf_free;
    buf_free      = buffer;
}


bool
add_buf( BUFFER *buffer, const char *string )
{
    int		len;
    char *	oldstr;
    int		oldsize;
    char *	p;

    oldstr = buffer->string;
    oldsize = buffer->size;

    if ( buffer->state == BUFFER_OVERFLOW ) /* don't waste time on bad strings! */
	return FALSE;

    len = buffer->length + strlen( string ) + 1;

    while ( len >= buffer->size ) /* increase the buffer size */
    {
	buffer->size 	= get_size( buffer->size + 1 );
	{
	    if ( buffer->size == -1 ) /* overflow */
	    {
		buffer->size = oldsize;
		buffer->state = BUFFER_OVERFLOW;
		bug( "buffer overflow past size %d", buffer->size );
		return FALSE;
	    }
  	}
    }

    if ( buffer->size != oldsize )
    {
	buffer->string	= alloc_mem( buffer->size );

	strcpy( buffer->string,oldstr );
	free_mem( oldstr, oldsize );
    }

    p = stpcpy( buffer->string + buffer->length, string );
    buffer->length = p - buffer->string;
    return TRUE;
}


void
clear_buf( BUFFER *buffer )
{
    buffer->string[0] = '\0';
    buffer->length    = 0;
    buffer->state     = BUFFER_SAFE;
}


char *buf_string( BUFFER *buffer )
{
    return buffer->string;
}

bool buf_printf( BUFFER *pBuf, const char *fmt, ... )
{
    char buf[MAX_STRING_LENGTH];
    va_list args;
    bool fResult;

    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );
    fResult = add_buf( pBuf, buf );
    return fResult;
}
