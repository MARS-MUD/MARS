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
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

#define PATH_MAX_VNUM MAX_VNUM
#define PATH_MAX_DIR  MAX_DIR
// #define BITS_PER_INT (sizeof(int)*8)
#define BITS_PER_INT             32
#define PATH_IS_FLAG(flag, bit)  ((unsigned)flag[bit/BITS_PER_INT]>>bit%BITS_PER_INT&01)
#define PATH_SET_FLAG(flag, bit) (flag[bit/BITS_PER_INT] |= 1 << bit%BITS_PER_INT)

void do_track args((CHAR_DATA*ch, char *argument));
int find_path(ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to, int max_depth);
//extern char * const dir_name[];

void do_track(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int i;
  int skill;

  if (((skill = get_skill(ch,gsn_track)) == 0) || (IS_NPC(ch)))
  {
    send_to_char("You do not know how to track.\n\r",ch);
    return;
  }
  victim = get_char_world(ch, argument);
  if( victim == NULL )
  {
    send_to_char("No one around by that name.\n\r", ch);
    return;
  }
  if(ch->in_room == victim->in_room)
  {
    act( "$N is here!", ch, NULL, victim, TO_CHAR );
    return;
  }
  act("$n carefully sniffs the air.", ch, NULL, NULL, TO_ROOM);
  WAIT_STATE(ch, skill_table[gsn_track].beats);

// What I do is use the player's track skill and level
// to get the maximum length path track will find.
// modify this however you want your track to work...

  skill /= 4;
  skill += ch->level/5;
  i = find_path(ch->in_room, victim->in_room, skill);

//If you don't have ch_printf, GET IT.  Will save you about a billion buf[]'s.

  if (i == -1)
    ch_printf(ch, "Unable to find a path to %s.\n\r", PERS(victim, ch));
  else
    ch_printf(ch, "%s is %s from here.\n\r", PERS(victim, ch), dir_name[i]);
  check_improve(ch, gsn_track, TRUE, 1);
}

int find_path(ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to, int max_depth)
{
  int bitvector[PATH_MAX_VNUM/BITS_PER_INT];
  ROOM_INDEX_DATA *rlist;
  ROOM_INDEX_DATA *track_room_list;
  int i, depth;

  bzero(bitvector, sizeof(bitvector));
  PATH_SET_FLAG(bitvector, from->vnum);
  track_room_list = from;
  track_room_list->next_track = NULL;
  for (depth = 0; depth < max_depth; depth++)
  {
    rlist = track_room_list;
    track_room_list = NULL;
    for (; rlist; rlist = rlist->next_track)
    {
      for (i = 0; i < PATH_MAX_DIR; i++)
      {
        if (!rlist->exit[i] || !rlist->exit[i]->u1.to_room ||
          PATH_IS_FLAG(bitvector, rlist->exit[i]->u1.to_room->vnum))
          continue;
        PATH_SET_FLAG(bitvector, rlist->exit[i]->u1.to_room->vnum);
        rlist->exit[i]->u1.to_room->track_came_from = rlist;
        if (rlist->exit[i]->u1.to_room == to)
        {
          if (rlist == from)
            return i;
//  if you need access to the entire path, this is the place to get it.
//  basically it's back-tracking how it got to the destination.
//  Also a good place to hinder track based on sector, weather, etc.
          while (rlist->track_came_from != from)
            rlist = rlist->track_came_from;
          for (i = 0; i < PATH_MAX_DIR; i++)
            if (from->exit[i] && from->exit[i]->u1.to_room == rlist)
               return i;
          return -1;
        }
        else
        {
          rlist->exit[i]->u1.to_room->next_track = track_room_list;
          track_room_list = rlist->exit[i]->u1.to_room;
        }
      }
    }
  }
  return -1;
}