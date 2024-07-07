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

/******************************************************************************
*                                                                             *
*   Spell Templates                                                           *
*  Each spell in this file is a termplate for a different spell type,         *
*  with the idea being that any spell you wish to add that is a simple        *
*  buff, debuff, or damage spell that all you would need to do is take        *
*  one of the template spells and change the name and echoed text, as         * 
*  well as things like damage type (don't want lightning bolts doing          * 
*  frost damage after all) and then add the spells to the correct place       *
*  in skilltable.c and magic.h then assign it to the classes in-game with     *
*  skedit command.                                                            *
******************************************************************************/


/*************************************
BUFF Spells
**************************************/

void spell_aid(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

   if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already aided.\n\r",ch);
	else
	  act("$N is already aided.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 6+level;
    af.location  = APPLY_HITROLL;
    af.modifier  = level / 8;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 8;
    affect_to_char(victim,&af);

    send_to_char("You feel more confident as you are aided by the gods!\n\r",victim);
    act("$n is aided by $s's connection to the gods!",victim,NULL,NULL,TO_ROOM);
}


/*************************************
DEBUFF spells
**************************************/

void spell_bane( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ))
    {
	if (victim == ch)
	  send_to_char("You're already feeling a bane upon your soul.\n\r",ch);
	else
	  act("$N already has a bane upon $s soul.",ch,NULL,victim,TO_CHAR);
	return;
    }
    victim = (CHAR_DATA *) vo;

    if (IS_AFFECTED(victim,AFF_CURSE) || saves_spell(level,victim,DAM_NEGATIVE))
	return;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 2*level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1 * (level / 8);
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = level / 8;
    affect_to_char( victim, &af );
    send_to_char( "You feel a sense of dread.\n\r", victim );
    act("$n looks fearful for a moment.",victim,NULL,NULL,TO_ROOM);
    return;
}


/*************************************
DAMAGE spells
**************************************/

void
spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice( level, 12 );
    if ( saves_spell( level, victim, DAM_ACID ) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_ACID, TRUE);
    return;
}

/*
 * This version of fireball strikes only a single target.
 */

void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,	 82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,	102, 104, 106, 108, 110,
	112, 114, 116, 118, 120,	122, 124, 126, 128, 130
    };
    int dam;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim, DAM_FIRE) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_FIRE ,TRUE);
    return;
}

/*
*
* This version of Fireball creates an explosion that damages others
* in the same room, closer to that of the d&d spell
*/

void spell_fireball(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *tmp_vict,*last_vict,*next_vict;
    bool found;
    int dam;

    /* first strike */

    act("A small flickering flame jumps from $n's hand and grows into a massive fireball, hurled at $N.",
        ch,NULL,victim,TO_ROOM);
    act("A small flame launched from your hand grows into a massive fireball and heads for $N.",
	ch,NULL,victim,TO_CHAR);
    act("A small flame from $n's hand grows into a massive fireball and engulfs you!",
	ch,NULL,victim,TO_VICT);

    dam = dice(level,6);
    if (saves_spell(level,victim,DAM_FIRE))
 	dam /= 3;
    damage(ch,victim,dam,sn,DAM_FIRE,TRUE);
    last_vict = victim;
    level -= 4;   /* decrement damage */

    /* new targets */
    while (level > 0)
    {
	found = FALSE;
	for (tmp_vict = ch->in_room->people;
	     tmp_vict != NULL;
	     tmp_vict = next_vict)
	{
	  next_vict = tmp_vict->next_in_room;
	  if (!is_safe_spell(ch,tmp_vict,TRUE) && tmp_vict != last_vict)
	  {
	    found = TRUE;
	    last_vict = tmp_vict;
	    act("The fireball explodes and catches $n in the blast!",tmp_vict,NULL,NULL,TO_ROOM);
	    act("The blast from the fireball hits you!",tmp_vict,NULL,NULL,TO_CHAR);
	    dam = dice(level,6);
	    if (saves_spell(level,tmp_vict,DAM_FIRE))
		dam /= 3;
	    damage(ch,tmp_vict,dam,sn,DAM_FIRE,TRUE);
	    level -= 4;  /* decrement damage */
	  }
	}   /* end target searching loop */

	if (!found) /* no target found, hit the caster */
	{
	  if (ch == NULL)
     	    return;

	  if (last_vict == ch) /* no double hits */
	  {
	    act("The flame seems to have fizzled out.",ch,NULL,NULL,TO_ROOM);
	    act("The flame fizzles out into a puff of smoke.",
		ch,NULL,NULL,TO_CHAR);
	    return;
	  }

	  last_vict = ch;
	  act("$n is caught in the explosion of their own fireball!",ch,NULL,NULL,TO_ROOM);
	  send_to_char("You are caught in the explosion of your own fireball!\n\r",ch);
	  dam = dice(level,6);
	  if (saves_spell(level,ch,DAM_FIRE))
	    dam /= 3;
	  damage(ch,ch,dam,sn,DAM_FIRE,TRUE);
	  level -= 4;  /* decrement damage */
	  if (ch == NULL)
	    return;
	}
    /* now go back and find more targets */
    }
}
