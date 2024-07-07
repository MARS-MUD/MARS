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
#include <time.h>
#include "merc.h"

bool can_use_skpell( CHAR_DATA *ch, int sn )
{
/*
    int iClass;

    if ( IS_NPC( ch ) )
	return TRUE;

    for ( iClass = 0; iClass < ch->nclass; iClass++ )
    {
	if ( ch->level >= skill_table[sn].skill_level[ch->class[iClass]] )
	    return TRUE;
    }
*/
    if ( IS_NPC( ch ) )
	return TRUE;

    if ( ch->level >= skill_table[sn].skill_level[ch->class] )
	return TRUE;

    return FALSE;
}

/*
bool has_spells ( CHAR_DATA *ch )
{
    int iClass;

    if ( IS_NPC ( ch ) )
	return FALSE;

    for ( iClass = 0; iClass < ch->nclass; iClass++ )
    {
	if ( ch->class[iClass] == class_vampire || 
		ch->class[iClass] == class_deathknight )
	    continue;
	if ( IS_SET( class_table[ ch->class[iClass] ].flags, CF_USES_MANA ) )
	    return TRUE;
    }

    return FALSE;
}

*/
bool is_class ( CHAR_DATA *ch, int classnum )
{
/*
    int iClass;

    for ( iClass = 0; iClass < ch->nclass; iClass++ )
	if ( ch->class[iClass] == classnum )
	    return TRUE;
*/
    if ( ch->class == classnum )
	return TRUE;

    return FALSE;
}

int prime_class( CHAR_DATA *ch )
{
    return ch->class;

/*    return ch->class[0]; */
}

/* Returns the lowest level at which a spell/skill can be used */
/*
int least_level( CHAR_DATA *ch, int sn )
{
    int		minimum = MAX_LEVEL + 1;
    int		i;
    int		l;

    for ( i=0; i<ch->nclass; i++ )
    {
	if ( ( l = skill_table[sn].skill_level[ch->class[i]] ) < minimum )
	    minimum = l;
    }
    return minimum;
}

*/

/*
int spell_usage( CHAR_DATA *ch )
{
    int iClass;

    for ( iClass = 0; iClass < ch->nclass; iClass++ )
    {
	if ( ch->class[iClass] == class_vampire 
		|| ch->class[iClass] == class_deathknight )
	    return ST_BLOOD;
	if ( IS_SET( class_table[ ch->class[iClass] ].flags, CF_USES_MANA ) )
	    return ST_MANA;
    }
    if ( IS_SET( class_table[ ch->multied ].flags, CF_USES_MANA ) )
	return ST_MANA;
    return ST_NONE;
}
*/
