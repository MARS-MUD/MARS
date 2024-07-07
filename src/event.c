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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "merc.h"
#include "recycle.h"


static EVENT_DATA *	event_q_first[MAX_EVENT_HASH];
static EVENT_DATA *	event_q_last[MAX_EVENT_HASH];

static EVENT_DATA *	pEventNext;

static EVENT_DATA *	global_event_first;
static EVENT_DATA *	global_event_last;

static int		current_bucket;


static bool enqueue_event( EVENT_DATA *event, int game_pulses );


#define LINK_LOCAL(item, list, last)				\
do {								\
    if ( last == NULL )						\
    {								\
	last = item;						\
	list = item;						\
	item->next_local = NULL;				\
	item->prev_local = NULL;				\
    }								\
    else							\
    {								\
	item->next_local = list;				\
	item->prev_local = NULL;				\
	list->prev_local = item;				\
	list = item;						\
    }								\
} while ( 0 )
#define UNLINK_GLOBAL(item, list, last, bugmess, bugarg)	\
do {								\
    if ( last == list )						\
    {								\
	last = NULL;						\
	list = NULL;						\
    }								\
    else if ( item == list )					\
    {								\
	list = item->next_global;				\
	item->next_global->prev_global = NULL;			\
    }								\
    else if ( item == last )					\
    {								\
	last = item->prev_global;				\
	item->prev_global->next_global = NULL;			\
    }								\
    else if ( !item->prev_global || !item->next_global )	\
    {								\
	bug( bugmess, bugarg );					\
    }								\
    else							\
    {								\
	item->prev_global->next_global = item->next_global;	\
	item->next_global->prev_global = item->prev_global;	\
    }								\
} while ( 0 )

#define UNLINK_LOCAL(item, list, last, bugmess, bugarg)		\
do {								\
    if ( last == list )						\
    {								\
	last = NULL;						\
	list = NULL;						\
    }								\
    else if ( item == list )					\
    {								\
	list = item->next_local;				\
	item->next_local->prev_local = NULL;			\
    }								\
    else if (item == last)					\
    {								\
	last = item->prev_local;				\
	item->prev_local->next_local = NULL;			\
    }								\
    else if ( !item->prev_local || !item->next_local )		\
    {								\
	bug( bugmess, bugarg );					\
    }								\
    else							\
    {								\
	item->prev_local->next_local = item->next_local;	\
	item->next_local->prev_local = item->prev_local;	\
    }								\
} while ( 0 )


void
add_event_area( EVENT_DATA *event, AREA_DATA *area, int delay )
{
    if ( event->type == EVENT_NONE )
    {
	bug( "Add_event_area: no type.", 0 );
	return;
    }

    if ( event->fun == NULL )
    {
	bug( "Add_event_area: NULL EVENT_FUN.", 0 );
	return;
    }

    event->ownertype = EVENT_OWNER_AREA;
    event->owner.area = area;

    LINK_LOCAL( event, area->event_first, area->event_last );

    if ( !enqueue_event( event, delay ) )
	bug( "Add_event_area: event type %d failed to enqueue.", event->type );
}


void
add_event_char( EVENT_DATA *event, CHAR_DATA *ch, int delay )
{
    if ( event->type == EVENT_NONE )
    {
	bug( "Add_event_char: no type.", 0 );
	return;
    }

    if ( event->fun == NULL )
    {
	bug( "Add_event_char: NULL EVENT_FUN.", 0 );
	return;
    }

    event->ownertype = EVENT_OWNER_CHAR;
    event->owner.ch = ch;

    LINK_LOCAL( event, ch->event_first, ch->event_last );

    if ( !enqueue_event( event, delay ) )
	bug( "Add_event_char: event type %d failed to enqueue.", event->type );
}


void
add_event_desc( EVENT_DATA *event, DESCRIPTOR_DATA *d, int delay )
{
    if ( event->type == EVENT_NONE )
    {
	bug( "Add_event_desc: no type.", 0 );
	return;
    }

    if ( event->fun == NULL )
    {
	bug( "Add_event_desc: NULL EVENT_FUN.", 0 );
	return;
    }

    event->ownertype = EVENT_OWNER_DESC;
    event->owner.d = d;

    LINK_LOCAL( event, d->event_first, d->event_last );

    if ( !enqueue_event( event, delay ) )
	bug( "Add_event_desc: event type %d failed to enqueue.", event->type );
}


void
add_event_exit( EVENT_DATA *event, EXIT_DATA *exit, int delay )
{
    if ( event->type == EVENT_NONE )
    {
	bug( "Add_event_exit: no type.", 0 );
	return;
    }

    if ( event->fun == NULL )
    {
	bug( "Add_event_exit: NULL EVENT_FUN.", 0 );
	return;
    }

    event->ownertype = EVENT_OWNER_EXIT;
    event->owner.exit = exit;

    LINK_LOCAL( event, exit->event_first, exit->event_last );

    if ( !enqueue_event( event, delay ) )
	bug( "Add_event_exit: event type %d failed to enqueue.", event->type );
}


void
add_event_game( EVENT_DATA *event, int delay )
{
    if ( event->type == EVENT_NONE )
    {
	bug( "Add_event_game: no type.", 0 );
	return;
    }

    if ( event->fun == NULL )
    {
	bug( "Add_event_game: NULL EVENT_FUN.", 0 );
	return;
    }

    event->ownertype = EVENT_OWNER_GAME;

    LINK_LOCAL( event, global_event_first, global_event_last );

    if ( !enqueue_event( event, delay ) )
	bug( "Add_event_game: event type %d failed to enqueue.", event->type );
}


void
add_event_obj( EVENT_DATA *event, OBJ_DATA *obj, int delay )
{
    if ( event->type == EVENT_NONE )
    {
	bug( "Add_event_obj: no type.", 0 );
	return;
    }

    if ( event->fun == NULL )
    {
	bug( "Add_event_obj: NULL EVENT_FUN.", 0 );
	return;
    }

    event->ownertype = EVENT_OWNER_OBJ;
    event->owner.obj = obj;

    LINK_LOCAL( event, obj->event_first, obj->event_last );

    if ( !enqueue_event( event, delay ) )
	bug( "Add_event_obj: event type %d failed to enqueue.", event->type );
}


void
add_event_room( EVENT_DATA *event, ROOM_INDEX_DATA *room, int delay )
{
    if ( event->type == EVENT_NONE )
    {
	bug( "Add_event_room: no type.", 0 );
	return;
    }

    if ( event->fun == NULL )
    {
	bug( "Add_event_room: NULL EVENT_FUN.", 0 );
	return;
    }

    event->ownertype = EVENT_OWNER_ROOM;
    event->owner.room = room;

    LINK_LOCAL( event, room->event_first, room->event_last );

    if ( !enqueue_event( event, delay ) )
	bug( "Add_event_room: event type %d failed to enqueue.", event->type );
}


void
dequeue_event( EVENT_DATA *event )
{
    EVENT_DATA *link;
    EVENT_DATA *chain;

    link = event->link;
    event->link = NULL;
    if ( link != NULL )
    {
	for ( chain = link; chain != NULL; chain = chain->link )
	    if ( chain->link == event )
		chain->link = NULL;
	dequeue_event( link );
    }

    UNLINK_GLOBAL( event, event_q_first[event->bucket], event_q_last[event->bucket],
		   "Dequeue_event: event type %d not found in global queue.",
		   event->type );

    if ( event == pEventNext )
	pEventNext = pEventNext->next_global;

    switch( event->ownertype )
    {
	default:
	    bug( "Dequeue_event: event type %d has no owner.", event->type );
	    break;

	case EVENT_OWNER_AREA:
	    UNLINK_LOCAL( event, event->owner.area->event_first,
			  event->owner.area->event_last,
			  "Dequeue_event(area): event type %d not found in local list.",
			  event->type );
	    break;

	case EVENT_OWNER_CHAR:
	    UNLINK_LOCAL( event, event->owner.ch->event_first,
			  event->owner.ch->event_last,
			  "Dequeue_event(char): event type %d not found in local list.",
			  event->type );
	    break;

	case EVENT_OWNER_DESC:
	    UNLINK_LOCAL( event, event->owner.d->event_first,
			  event->owner.d->event_last,
			  "Dequeue_event(descriptor): event type %d not found in local list.",
			  event->type );
	    break;
	case EVENT_OWNER_EXIT:
	    UNLINK_LOCAL( event, event->owner.exit->event_first,
			  event->owner.exit->event_last,
			  "Dequeue_event(exit): event type %d not found in local list.",
			  event->type );
	    break;
	case EVENT_OWNER_GAME:
	    UNLINK_LOCAL( event, global_event_first, global_event_last,
			  "Dequeue_event(game): event type %d not found in local list.",
			  event->type );
	    break;

	case EVENT_OWNER_OBJ:
	    UNLINK_LOCAL( event, event->owner.obj->event_first,
			  event->owner.obj->event_last,
			  "Dequeue_event(obj): event type %d not found in local list.",
			  event->type );
	    break;

	case EVENT_OWNER_ROOM:
	    UNLINK_LOCAL( event, event->owner.room->event_first,
			  event->owner.room->event_last,
			  "Dequeue_event(room): event type %d not found in local list.",
			  event->type );
	    break;
    }

    free_event( event );
}


static bool
enqueue_event( EVENT_DATA *event, int game_pulses )
{
    int bucket;
    int passes;

    if ( event->ownertype == EVENT_OWNER_UNOWNED )
    {
	bug( "enqueue_event: event type %d with no owner.", event->type );
	return FALSE;
    }

    if ( game_pulses < 1 )
	game_pulses = 1;

    bucket = ( game_pulses + current_bucket ) % MAX_EVENT_HASH;
    passes = game_pulses / MAX_EVENT_HASH;

    event->bucket = bucket;
    event->passes = passes;

    if ( event_q_last[bucket] == NULL )
    {
	event_q_last[bucket] = event;
	event_q_first[bucket] = event;
	event->next_global = NULL;
	event->prev_global = NULL;
    }
    else
    {
	event->next_global = NULL;
	event->prev_global = event_q_last[bucket];
	event_q_last[bucket]->next_global = event;
	event_q_last[bucket] = event;
    }

    return TRUE;
}


int
event_time_left( EVENT_DATA *pEvent )
{
    int	pulses;

    pulses = pEvent->bucket - current_bucket + pEvent->passes * MAX_EVENT_HASH;


    if ( pulses < 0 )
	pulses += MAX_EVENT_HASH;

    return pulses;
}


EVENT_DATA *
get_event_area( AREA_DATA *area, int type )
{
    EVENT_DATA *event;

    for ( event = area->event_first; event != NULL; event = event->next_local )
	if ( event->type == type )
	    return event;

    return NULL;
}


EVENT_DATA *
get_event_char( CHAR_DATA *ch, int type )
{
    EVENT_DATA *event;

    for ( event = ch->event_first; event != NULL; event = event->next_local )
	if ( event->type == type )
	    return event;

    return NULL;
}


EVENT_DATA *
get_event_desc( DESCRIPTOR_DATA *d, int type )
{
    EVENT_DATA *event;

    for ( event = d->event_first; event != NULL; event = event->next_local )
	if ( event->type == type )
	    return event;

    return NULL;
}


EVENT_DATA *
get_event_exit( EXIT_DATA *d, int type )
{
    EVENT_DATA *event;

    for ( event = d->event_first; event != NULL; event = event->next_local )
	if ( event->type == type )
	    return event;

    return NULL;
}


EVENT_DATA *
get_event_game( int type )
{
    EVENT_DATA *event;

    for ( event = global_event_first; event != NULL; event = event->next_local )
	if ( event->type == type )
	    return event;

    return NULL;
}


EVENT_DATA *
get_event_obj( OBJ_DATA *obj, int type )
{
    EVENT_DATA *event;

    for ( event = obj->event_first; event != NULL; event = event->next_local )
	if ( event->type == type )
	    return event;

    return NULL;
}


EVENT_DATA *
get_event_room( ROOM_INDEX_DATA *room, int type )
{
    EVENT_DATA *event;

    for ( event = room->event_first; event != NULL; event = event->next_local )
	if ( event->type == type )
	    return event;

    return NULL;
}


void
strip_event_area( AREA_DATA *area, int type )
{
    EVENT_DATA *event;
    EVENT_DATA *event_next;

    for ( event = area->event_first; event != NULL; event = event_next )
    {
	event_next = event->next_local;
	if ( event->type == type )
	    dequeue_event( event );
    }
}


void
strip_event_char( CHAR_DATA *ch, int type )
{
    EVENT_DATA *event;
    EVENT_DATA *event_next;

    for ( event = ch->event_first; event != NULL; event = event_next )
    {
	event_next = event->next_local;
	if ( event->type == type )
	    dequeue_event( event );
    }
}


void strip_event_desc( DESCRIPTOR_DATA *d, int type )
{
    EVENT_DATA *event;
    EVENT_DATA *event_next;

    for ( event = d->event_first; event != NULL; event = event_next )
    {
	event_next = event->next_local;
	if ( event->type == type )
	    dequeue_event( event );
    }
}


void strip_event_exit( EXIT_DATA *exit, int type )
{
    EVENT_DATA *event;
    EVENT_DATA *event_next;

    for ( event = exit->event_first; event != NULL; event = event_next )
    {
	event_next = event->next_local;
	if ( event->type == type )
	    dequeue_event( event );
    }
}


void
strip_event_game( int type )
{
    EVENT_DATA *event;
    EVENT_DATA *event_next;

    for ( event = global_event_first; event != NULL; event = event_next )
    {
	event_next = event->next_local;
	if ( event->type == type )
	    dequeue_event( event );
    }
}


void
strip_event_obj( OBJ_DATA *obj, int type )
{
    EVENT_DATA *event;
    EVENT_DATA *event_next;

    for ( event = obj->event_first; event != NULL; event = event_next )
    {
	event_next = event->next_local;
	if ( event->type == type )
	    dequeue_event( event );
    }
}


void
strip_event_room( ROOM_INDEX_DATA *room, int type )
{
    EVENT_DATA *event;
    EVENT_DATA *event_next;

    for ( event = room->event_first; event != NULL; event = event_next )
    {
	event_next = event->next_local;
	if ( event->type == type )
	    dequeue_event( event );
    }
}


void
update_events( void )
{
    EVENT_DATA *event;

    current_bucket = ( current_bucket + 1 ) % MAX_EVENT_HASH;

    for ( event = event_q_first[current_bucket]; event != NULL; event = pEventNext )
    {
	pEventNext = event->next_global;
	if ( event->type == EVENT_DUMMY )
	    continue;
	if ( event->passes-- > 0 )
	    continue;
	if ( ! ( ( event->fun )( event ) ) )
	    dequeue_event( event );
    }
}


