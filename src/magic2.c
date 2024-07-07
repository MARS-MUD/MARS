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
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "magic.h"
#include "recycle.h"


const	struct	animal_summon_type	animal_table[] =
{
    {
	4,	100,	100,	&race_fox,	SIZE_SMALL,
	"squirrel",	"a squirrel",
	"A fuzzy little squirrel is here following someone.\n",
	"A small squirrel appears from the woods and chitters.",
	"$N appears from the woods and chitters at $m."
    },
    {
	8,	150,	150,	&race_fox,	SIZE_SMALL,
	"red fox",	"a red fox",
	"A small red fox is here looking around inquisitively.\n\r",
	"A small red fox inquisitvely peers at you.",
	"N peers inquisitively at $m."
    },
    {
	12,	200,	200,	&race_fox,	SIZE_SMALL,
	"badger",	"a fat badger",
	"A fat little badger is here following someone.\n",
	"A fat little badger toddles towards you.",
	"$N toddles out of the woods towards $m."
    },
    {
	16,	250,	250,	&race_fox,	SIZE_SMALL,
	"porcupine",	"a porcupine",
	"A bristling porcupine is here following someone.\n\r",
	"A bristling porcupine ruffles its quills as it comes towards you.",
	"$N bristles its quills at $m."
    },
    {
	20,	300,	300,	&race_dog,	SIZE_SMALL,
	"wolverine",	"a wolverine",
        "A mean little wolverine is here following someone.\n\r",
	"A small angry wolverine walks towards you.",
	"$N looks highly annoyed as it walks towards $m."
    },
    {
	24,	440,	350,	&race_bear,	SIZE_MEDIUM,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	28,	650,	400,	&race_bear,	SIZE_MEDIUM,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	32,	850,	450,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	36,	1100,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	40,	1450,	550,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	44,	1800,	600,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	48,	2050,	650,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	52,	2370,	700,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	56,	2650,	750,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	60,	2900,	800,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	64,	3300,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	68,	3700,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	72,	4250,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	76,	4850,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	80,	5400,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	84,	6000,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	88,	6600,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	92,	7200,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	96,	7800,	500,	&race_bear,	SIZE_LARGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	100,	8400,	500,	&race_bear,	SIZE_HUGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	110,	9900,	500,	&race_bear,	SIZE_HUGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	120,	12000,	500,	&race_bear,	SIZE_HUGE,
	"grizzly bear",	"a grizzly bear",
	"A large brown bear is here looking menacing.\n\r",
	"A large grizzly bear lumbers out of the woods towards you.",
	"$N lumbers out of the woods and sniffs at $m."
    },
    {
	0,	0,	0,	NULL,		0,
	NULL,		NULL,
	NULL,
	NULL,
	NULL
    },
};


void
spell_acid_rain( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *	victim;
    int		dam;
    int		i;

    i = 8;

    victim = (CHAR_DATA *)vo;

    if ( victim->in_room != ch->in_room )
	return;

    while ( i > 0 )
    {
	dam = dice( level, 8 );
	if ( saves_spell( level, victim, DAM_ACID ) )
	    dam /= 2;
	act_color( AT_RED, "`RWaves of `GA`gc`Gi`gd`Gi`gc `BRain `gcalled by $n shower down upon $N!`X",
		   ch, NULL, victim, TO_CHAR, POS_RESTING );
	act_color( AT_RED, "`RWaves of `GA`gc`Gi`gd`Gi`gc `BRain `gcalled by $n shower down upon $N!`X",
		   ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	act_color( AT_RED, "`RWaves of `GA`gc`Gi`gd`Gi`gc `BRain `gcalled by $n shower down upon you!`X",
		   ch, NULL, victim, TO_VICT, POS_RESTING );
	damage( ch, victim, dam, sn, DAM_ACID,TRUE );

	if ( victim->in_room != ch->in_room )
	    break;

	i--;
    }
    return;
}


void
spell_adrenaline( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    AFFECT_DATA	af;

    victim = (CHAR_DATA *)vo;

    if ( is_affected( victim, sn ) || IS_AFFECTED( victim, AFF_BERSERK ) )
    {
        if ( victim == ch )
            act_color( AT_MAGIC, "You are already pumped.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
        else
            act_color( AT_MAGIC, "$N is already pumped.", ch, NULL, victim, TO_CHAR, POS_RESTING );
        return;
    }

    if ( is_affected( victim, skill_lookup( "calm" ) ) )
    {
        if ( victim == ch )
            act_color( AT_MAGIC, "Why don't you just relax for a while?", ch, NULL, NULL, TO_CHAR, POS_RESTING );
        else
            act_color( AT_MAGIC, "$N doesn't look like $e wants to fight anymore.", ch, NULL, victim, TO_CHAR, POS_RESTING );
        return;
    }

    af.where     = TO_AFFECTS;
    af.type 	 = sn;
    af.level	 = level;
    af.duration	 = level / 3;
    af.modifier  = level / 6;
    af.bitvector = 0;

    af.location  = APPLY_HITROLL;
    affect_to_char( victim, &af );

    af.location  = APPLY_DAMROLL;
    affect_to_char( victim, &af );

    af.modifier  = 10 * (level / 36);
    af.location  = APPLY_AC;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "You are pumped up!", victim, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n gets a wild look in $s eyes!", victim, NULL, NULL, TO_ROOM, POS_RESTING );

    return;
}

/*
void
spell_aid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    AFFECT_DATA	af;

    victim = (CHAR_DATA *)vo;

    if ( is_affected( victim, sn ) )
    {
	if ( victim == ch )
	    send_to_char( "You have already been aided.\n\r", ch );
	else
	    act( "$N has already been aided.", ch, NULL, victim, TO_CHAR );
	return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level/2;
    af.location	 = APPLY_HITROLL;
    af.modifier	 = 1;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if ( victim == ch )
    {
	act_color( AT_MAGIC, "You grant yourself divine aid.", ch, NULL, victim, TO_CHAR, POS_RESTING );
	act_color( AT_MAGIC, "$n grants divine aid to $mself.", ch, NULL, victim, TO_ROOM, POS_RESTING );
    }
    else
    {
	act_color( AT_MAGIC, "You grant them divine aid.", ch, NULL, victim, TO_CHAR, POS_RESTING );
	act_color( AT_MAGIC, "$n grants you divine aid.", ch, NULL, victim, TO_VICT, POS_RESTING );
	act_color( AT_MAGIC, "$n grants divine aid to $N.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
    }

    return;
}
*/

void
spell_animal_summoning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA		af;
    CHAR_DATA *		animal;
    int			i;
    int			iAuto;
    int			index;
    int			iRace;
    MOB_INDEX_DATA *	pMob;
    EXIT_DATA *		pExit;
    int			door;
    bool		found;

    if ( ch->in_room == NULL )
    {
        send_to_char( "You're nowhere!  youi can't do that here.\n\r", ch );
        return;
    }

    found = FALSE;
    if ( ch->in_room->sector_type == SECT_FOREST )
        found = TRUE;
    else if ( ch->in_room->sector_type == SECT_FIELD )
    {
        for ( door = 0; door < MAX_DIR; door++ )
        {
            pExit = get_exit( ch->in_room, door );
            if ( pExit != NULL
            &&	 pExit->to_room != NULL
            &&	 pExit->to_room->sector_type == SECT_FOREST )
            {
                found = TRUE;
                break;
            }
        }
    }

    if ( !found )
    {
        send_to_char( "You must be in or near the woods to summon animals.\n\r", ch );
        return;
    }

    if ( ch->pet != NULL )
    {
        send_to_char( "You already have an animal following you.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_WEARINESS ) )
    {
        send_to_char( "You are too weary to beckon forth an animal companion.\n\r", ch );
        return;
    }

    for ( index = 0; animal_table[index].level != 0; index++ )
        if ( ch->level <= animal_table[index].level )
            break;

    if ( animal_table[index].level == 0 )
    {
        bugf( "Spell_animal_summoning: no entry for level %d", ch->level );
        send_to_char( "Due to technical difficulties, your spell cannot be completed.\n\r", ch );
        return;
    }

    pMob = get_mob_index( MOB_VNUM_GREYBLOB );
    if ( pMob == NULL )
    {
        bugf( "Spell_animal_summoning: no GREYBLOB" );
        send_to_char( "Due to technical difficulties, your spell cannot be completed.\n\r", ch );
        return;
    }

    af.type	 = gsn_weariness;
    af.where	 = TO_AFFECTS;
    af.duration	 = ch->level / 2;
    af.bitvector = AFF_WEARINESS;
    af.level	 = level;
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    affect_join( ch, &af );

    animal = create_mobile( pMob );

    free_string( animal->name );
    animal->name = str_dup( animal_table[index].name );
    free_string( animal->short_descr );
    animal->short_descr = str_dup( animal_table[index].short_descr );
    free_string( animal->long_descr );
    animal->long_descr = str_dup( animal_table[index].long_descr );
    animal->level	= animal_table[index].level;
    animal->hit		= animal_table[index].hit;
    animal->max_hit	= animal->hit;
    animal->move	= animal_table[index].move;
    animal->max_move	= animal->move;
    animal->size	= animal_table[index].size;
    iRace		= *animal_table[index].race;
    animal->race	= iRace;

    /* Fix up stats from automob table */
    for ( iAuto = 1; hitdice_table[iAuto].level != 0; iAuto++ )
        if ( animal->level <= hitdice_table[iAuto].level )
            break;
    animal->hit = dice( hitdice_table[iAuto].hit[0], hitdice_table[iAuto].hit[1] ) +
                        hitdice_table[iAuto].hit[2];
    animal->max_hit = animal->hit;
    animal->damage[DICE_NUMBER] = hitdice_table[iAuto].dam[DICE_NUMBER];
    animal->damage[DICE_TYPE  ] = hitdice_table[iAuto].dam[DICE_TYPE  ];
    for ( i = 0; i < 4; i++ )
        animal->armor[i] = hitdice_table[iAuto].ac;
    animal->hitroll = hitdice_table[iAuto].hitroll;

    /* Fix up flags from race table */
    animal->off_flags	= race_table[iRace].off;
    animal->imm_flags	= race_table[iRace].imm;
    animal->res_flags	= race_table[iRace].res;
    animal->vuln_flags	= race_table[iRace].vuln;
    animal->form	= race_table[iRace].form;
    animal->parts	= race_table[iRace].parts;

    char_to_room( animal, ch->in_room );
    xSET_BIT( animal->affected_by, AFF_CHARM );
    SET_BIT( animal->act, ACT_PET );

    act_color( AT_MAGIC, animal_table[index].to_char, ch, NULL, animal, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, animal_table[index].to_room, ch, NULL, animal, TO_ROOM, POS_RESTING );

    add_follower( animal, ch );
    animal->leader = ch;
    ch->pet = animal;

    return;
}


void
spell_atrophy( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    AFFECT_DATA	af;

    victim = (CHAR_DATA *)vo;


    if ( is_affected( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char( "You are already wasting away.\n\r", ch );
        else
            act_color( AT_NONE, "$E is already wasting away.", ch, NULL, victim, TO_CHAR, POS_RESTING );
        return;
    }

    if ( saves_spell( level, victim, DAM_OTHER ) || victim->race == race_troll )
    {
        send_to_char("Spell failed.\n\r", ch );
        return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.duration	 = level/7;
    af.location	 = APPLY_REGENERATION;
    af.modifier	 = number_range( -3, -level / 12 );
    af.bitvector = 0;
    af.level	 = level;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "You feel an intense pain as your body begins to waste away.\n\r",
               victim, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n looks very sick as $s body starts wasting away before your eyes!",
               victim, NULL, NULL, TO_ROOM, POS_RESTING );
    return;
}


void
spell_aura_peace( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *	victim;

    victim = vo;

    if ( IS_AFFECTED( victim, AFF_PEACE ) )
    {
	send_to_char( "You fail.\n\r", ch );
	return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = number_fuzzy( level / 5 );
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    af.bitvector = AFF_PEACE;

    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "You feel a wave of peace flow over your body.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n looks very peaceful.", ch, NULL, NULL, TO_ROOM, POS_RESTING );

    return;
}


void
spell_barkskin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *	victim;

    victim = (CHAR_DATA *)vo;

    if ( is_affected( victim, sn ) )
    {
        if ( victim == ch )
	    act_color( AT_MAGIC, "Your skin is already thick as bark.{x\n\r", ch, NULL, NULL, TO_CHAR, POS_RESTING );
	else
	    act_color( AT_MAGIC, "$N already has bark for skin.", ch, NULL, victim, TO_CHAR, POS_RESTING );
	return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level;
    af.location	 = APPLY_AC;
    af.modifier	 = -40;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.where	 = TO_VULN;
    af.location	 = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = VULN_FIRE;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "$n's skin turns to bark.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "Your skin turns to bark.", victim, NULL, NULL, TO_CHAR, POS_RESTING );

    return;
}


void
spell_bear_claws( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    OBJ_INDEX_DATA *	pObj;
    OBJ_DATA *		bclaw;
    AFFECT_DATA		af;

    pObj = get_obj_index( OBJ_VNUM_BEARCLAW );
    if ( pObj == NULL )
    {
        bugf( "Spell_bear_claws: no BEARCLAW" );
        send_to_char( "Due to technical difficulties, your spell cannot be completed.\n\r", ch );
        return;
    }


    bclaw		= create_object( pObj, 0 );
    bclaw->value[0]	= ch->level/10;
    bclaw->value[1]	= ch->level/6;
    bclaw->value[2]	= ch->level/6;
    bclaw->value[3] 	= ch->level/6;
    bclaw->timer	= ch->level/3;
    bclaw->level	= ch->level;

    af.where		= TO_OBJECT;
    af.type		= sn;
    af.level		= ch->level;
    af.duration		= -1;
    af.location		= APPLY_DAMROLL;
    af.modifier		= ch->level / 10;
    af.bitvector	= 0;
    affect_to_obj( bclaw, &af );

    af.where		= TO_OBJECT;
    af.type		= sn;
    af.level		= ch->level;
    af.duration		= -1;
    af.location		= APPLY_HITROLL;
    af.modifier		= ch->level / 10;
    af.bitvector	= 0;
    affect_to_obj(bclaw, &af);

    act_color( AT_MAGIC, "$n draws upon the essence of the bear spirits around $m.", ch, bclaw, NULL, TO_ROOM, POS_RESTING );
    WAIT_STATE( ch, PULSE_VIOLENCE );
    act_color( AT_MAGIC, "The spirit of the bear surrounds $n, the only sign remaining on $s hands.", ch, bclaw, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "A set of mighty claws forms from the essence of the mighty bear", ch, NULL, NULL, TO_CHAR, POS_RESTING );

    obj_to_char( bclaw, ch );
    wear_obj( ch,bclaw,TRUE );
    return;
}


void
spell_barrage( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    int		dam;

    victim = (CHAR_DATA *) vo;

    dam = dice( level, 12 );
    spell_blindness( gsn_blindness, level/2, ch, (void *) victim, TARGET_CHAR );
    if ( saves_spell( level, victim, DAM_LIGHT ) )
        dam /= 2;
    damage( ch, victim, dam, sn, DAM_LIGHT, TRUE );
    return;
}


void
spell_bear_fat( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    AFFECT_DATA af;

    victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, sn ) )
    {
	af.where	= TO_RESIST;
	af.type		= sn;
	af.level	= level;
	af.duration	= level/2;
	af.location 	= APPLY_NONE;
	af.modifier 	= 0;
	af.bitvector	= RES_COLD;
	affect_to_char( victim, &af );
	act_color( AT_MAGIC, "You slather yourself with magical bear fat.", victim, NULL, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_MAGIC, "$e slathers $mself with magical bear fat.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    }
    else
        send_to_char( "You are already slathered in bear fat.\n\r", victim );

    return ;
}


void
spell_blade_barrier( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *	victim;
    AFFECT_DATA	af;

    victim = (CHAR_DATA *)vo;

    if ( is_affected( victim, sn ) )
    {
	if ( victim == ch )
	    send_to_char( "You are already surrounded by spinning blades.\n\r", ch );
	else
	    act( "$N is already surrounded by spinning blades.", ch, NULL, victim, TO_CHAR );
	return;
    }

    af.where	 = TO_SHIELDS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = (level / 10);
    af.location	 = APPLY_AC;
    af.modifier	 = (level / 2) * -1;
    af.bitvector = SHLD_BLADES;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "$n is surrounded by whirling blades.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "You are surrounded by whirling blades.", victim, NULL, NULL, TO_CHAR, POS_RESTING );

    return;
}


void
spell_cone_of_cold( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	vch;
    CHAR_DATA *	vch_next;
    int		dam;
    AFFECT_DATA af;

    dam = dice( level , 3 ) + 50;

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/10;
    af.location  = APPLY_STR;
    af.modifier  = -level/10;
    af.bitvector = 0;

    act_color( AT_MAGIC, "You send forth a cone of cold!",
               ch, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n places out $s hands and blasts forth a cone of cold!",
               ch, NULL, NULL, TO_ROOM, POS_RESTING );

    for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next_in_room;

        if ( vch == ch )
            continue;

        if ( IS_SHIELDED( vch, SHLD_PROTECT_COLD ) || IS_SHIELDED( vch, SHLD_ICE ) )
        {
            send_to_char( "The ice flows around your shield!\n\r", vch );
            continue;
        }

        if ( !is_same_group( ch, vch ) )
        {
            damage( ch, vch, saves_spell( level, vch, DAM_COLD ) ? dam /2 : dam, sn, DAM_COLD, TRUE );
            if ( !saves_spell( level, vch, DAM_COLD ) )
            {
                affect_join( vch, &af );
                send_to_char( "The cold seeps into your bones.", vch );
            }
        }
        continue ;
    }

    return ;
}


void
spell_cure_drunk( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	vch;
    int		amt;

    vch = (CHAR_DATA *)vo;
    amt = number_fuzzy( 2 * level / 5 );
    if ( IS_NPC( vch ) || vch->pcdata->condition[COND_DRUNK] <= 0 )
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    act_color( AT_MAGIC, "A jolt of energy runs thorugh your mind.", vch, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "A jolt of energy runs through $n.", vch, NULL, NULL, TO_ROOM, POS_RESTING );
    gain_condition( vch, COND_DRUNK, -amt );
}


void
spell_curse_of_nature( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *	victim;

    victim = (CHAR_DATA *)vo;

    if ( IS_AFFECTED( victim, AFF_CURSE_NATURE )
    ||	 IS_AFFECTED( victim, AFF_CURSE )
    ||	 saves_spell( level, victim, DAM_NEGATIVE ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level / 6;
    af.location	 = APPLY_HITROLL;
    af.modifier	 = 0 - level / 4;
    af.bitvector = AFF_CURSE_NATURE;
    affect_to_char( victim, &af );

    af.location	 = APPLY_DAMROLL;
    af.modifier	 = 0 - level / 3;
    affect_to_char( victim, &af );

    af.location	 = APPLY_STR;
    af.modifier	 = 0 - level / 50;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "The curse of nature falls upon you.", victim, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "The curse of nature falls upon $n.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
}


void
spell_dancing_boots( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    OBJ_DATA *		dboot;
    OBJ_INDEX_DATA *	pObj;

    pObj = get_obj_index( OBJ_VNUM_DBOOT );
    if ( pObj == NULL )
    {
        bugf( "Spell_dancing_boots: no OBJ_VNUM_DBOOT" );
        send_to_char( "Due to technical difficulties, your spell cannot be completed.\n\r", ch );
        return;
    }

    dboot = create_object( pObj, 0 );
    dboot->value[0] = ch->level/10;
    dboot->value[1] = ch->level/6;
    dboot->value[2] = ch->level/6;
    dboot->value[3] = ch->level/6;
    dboot->timer = ch->level/3;
    dboot->level = ch->level;

    WAIT_STATE( ch, PULSE_VIOLENCE );
    act_color( AT_MAGIC, "$n pulls a pair of boots from $s sleeve.", ch, dboot, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "You conjure a pair of boots from nothingness.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
    obj_to_char(dboot,ch);
    wear_obj( ch,dboot,TRUE );
    return;
}


void
spell_dancing_lights( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *	victim;

    victim = (CHAR_DATA *)vo;

    if ( IS_AFFECTED( victim, AFF_DANCING ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( victim, AFF_BLIND ) || saves_spell( level, victim, DAM_LIGHT ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = number_fuzzy( level / 6 );
    af.location	 = APPLY_HITROLL;
    af.modifier	 = -level / 6;
    af.bitvector = AFF_DANCING;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "`.Thou`.sand`.s `.of `.danci`.ng `.ligh`.ts `.surr`.ound `.you`.!`w", victim, NULL, victim, TO_VICT, POS_RESTING );
    act_color( AT_MAGIC, "$n's `.body `.is `.surr`.ounded `.by d`.anci`.ng l`.ights.", victim, NULL, NULL, TO_ROOM, POS_RESTING );

    return;
}


void
spell_darkness_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    OBJ_INDEX_DATA *	pObj;
    OBJ_DATA *		shield;
    int			value;
    int			magvalue;

    value = 0;
    magvalue = 0;

    if ( ch->level < 72 )
    {
        value = 25;
        magvalue = 14;
    }
    if ( ch->level < 68 )
    {
        value = 24;
        magvalue = 13;
    }
    if ( ch->level < 64 )
    {
        value = 23;
        magvalue = 12;
    }
    if ( ch->level < 60 )
    {
        value = 22;
        magvalue = 11;
    }
    if ( ch->level < 56 )
    {
        value = 21;
        magvalue = 10;
    }
    if ( ch->level < 51 )
    {
        value = 19;
        magvalue = 9;
    }
    if ( ch->level < 46 )
    {
        value = 17;
        magvalue = 8;
    }
    if ( ch->level < 41 )
    {
        value = 15;
        magvalue = 7;
    }
    if ( ch->level < 36 )
    {
        value = 13;
        magvalue = 6;
    }
    if ( ch->level < 31 )
    {
        value = 11;
        magvalue = 5;
    }
    if ( ch->level < 26 )
    {
        value = 9;
        magvalue = 4;
    }
    if ( ch->level < 21 )
    {
        value = 7;
        magvalue = 3;
    }
    if ( ch->level < 16 )
    {
        value = 5;
        magvalue = 2;
    }
    if ( ch->level < 11 )
    {
        value = 3;
        magvalue = 1;
    }
    if ( ch->level < 6 )
    {
        value = 1;
        magvalue = 0;
    }

    pObj = get_obj_index( OBJ_VNUM_LSHIELD );
    if ( pObj == NULL )
    {
        bugf( "Spell_darkness_shield: no OBJ_VNUM_LSHIELD" );
        send_to_char( "Due to technical difficulties, your spell cannot be completed.\n\r", ch );
        return;
    }

    shield = create_object(get_obj_index(OBJ_VNUM_LSHIELD), 0);
    shield->value[0] = value;
    shield->value[1] = value;
    shield->value[2] = value;
    shield->value[3] = magvalue;
    shield->timer    = ch->level;
    shield->level    = ch->level;

    act_color( AT_MAGIC, "$n has created a shield made of darkness.", ch, shield, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "You create a shield of darkness.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
    obj_to_char( shield, ch );
    return;
}


void
spell_dark_ritual( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *	obj;
    int		mana;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	if ( obj->item_type == ITEM_CORPSE_NPC )
	    break;
    }

    if ( obj != NULL )
    {
	mana = UMAX( 30, number_fuzzy( level / 2 ) );
	ch->mana = UMIN( ch->mana + mana, ch->max_mana );
	act_color( AT_DGREY, "You extract the last of the energy from the corpse.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
	act_color( AT_DGREY, "$n extracts the last of the energy from the corpse.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
	extract_obj( obj );
    }
    else
	send_to_char( "You must have a corpse to sacrifice to perform a dark ritual.\n\r", ch );
}


void
spell_deafen( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA * vch;

    vch = (CHAR_DATA *)vo;

    if ( ch == vch )
    {
	send_to_char( "`cYou try to block out the horror, but fail.", ch );
	return;
    }

    if ( xIS_SET( vch->affected_by, AFF_DEAF ) )
    {
	send_to_char( "`cThey are already deaf.", ch );
    }

    if ( saves_spell( level, vch, DAM_HOLY ) )
    {
	if ( saves_spell( level, vch, DAM_HOLY ) )
	{
	    act_color( AT_MAGIC, "Your attempt to deafen them failed.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_MAGIC, "You hear a faint buzzing in your ears, but it soon dissapates.",
		       vch, NULL, NULL, TO_CHAR, POS_RESTING );
	    return;
	}

	af.where      = TO_AFFECTS;
	af.type       = sn;
	af.level      = level/2;
	af.duration   = (int)level/10;
	af.location   = APPLY_NONE;
	af.modifier   = 0;
	af.bitvector  = AFF_DEAF;
    }
    else
    {
	af.where      = TO_AFFECTS;
	af.type       = sn;
	af.level      = level;
	af.duration   = (int)level/5;
	af.location   = APPLY_NONE;
	af.modifier   = 0;
	af.bitvector  = AFF_DEAF;
    }

    affect_join( vch, &af );

    act_color( AT_MAGIC, "Your ears ring loudly!", vch, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n is now deaf.", vch, NULL, NULL, TO_ROOM, POS_RESTING );

    return;
}


void
spell_enlarge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *	victim;

    victim = (CHAR_DATA *)vo;

    if ( is_affected( victim, gsn_enlarge ) || victim->size >= SIZE_GIANT )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( is_affected( victim, gsn_shrink ) )
    {
	if ( !check_dispel( level, victim, gsn_shrink ) )
	    send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level / 2;
    af.location	 = APPLY_SIZE;
    af.modifier	 = UMIN( SIZE_GIANT - victim->size, 3);
    af.bitvector = AFF_NONE;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "You grow as large as a building!", victim, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n grows as large as a building!", victim, NULL, NULL, TO_ROOM, POS_RESTING );
}


void
spell_farsight( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *		vch;
    ROOM_INDEX_DATA *	oldroom;
    ROOM_INDEX_DATA *	inroom;

    if ( IS_AFFECTED( ch, AFF_BLIND ) )
    {
	send_to_char( "Maybe it would help if you could see?\n\r", ch );
	return;
    }

    if ( ( vch = get_char_world( ch, target_name ) ) == NULL )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( ( inroom = vch->in_room ) == NULL
    ||	 room_is_private( inroom )
    ||	 IS_SET( inroom->room_flags, ROOM_NO_MAGIC )
    ||	 ( !IS_IMMORTAL( ch ) && IS_SET( inroom->area->area_flags, AREA_PROTOTYPE ) )
    ||	 ( !is_same_landmass( ch->in_room, vch->in_room ) && ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    ||	 !can_see_room( ch, inroom ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch )
    &&	 ( !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
    &&	 ch->class == class_druid
    &&	 IS_SET( inroom->room_flags, ROOM_INDOORS ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    oldroom = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, inroom );
    do_look( ch, "auto" );
    char_from_room( ch );
    char_to_room( ch, oldroom );
}


void
spell_flameshield( int sn, int level, CHAR_DATA * ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    victim = (CHAR_DATA *)vo;

    if ( is_affected( victim, sn ) )
    {
	if ( victim == ch )
	    send_to_char( "You are already protected by fire.\n\r", ch );
	else
	    act( "$N is already protected by fire.", ch, NULL, victim, TO_CHAR );
	return;
    }

    af.where	 = TO_SHIELDS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = (level / 10);
    af.location	 = APPLY_AC;
    af.modifier	 = (level / 2) * -1;
    af.bitvector = SHLD_FLAME;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "$n is shielded by red walls of flame.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "You are shielded by red walls of flame.", victim, NULL, NULL, TO_CHAR, POS_RESTING );

    return;
}


void
spell_forceshield( int sn, int level, CHAR_DATA * ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    AFFECT_DATA	af;

    victim = (CHAR_DATA *)vo;

    if ( is_affected ( victim, sn ) )
    {
	if (victim == ch)
	    send_to_char ( "You already have a force shield.\n\r", ch );
	else
	    act( "$N already has a force shield.", ch, NULL, victim, TO_CHAR );
	return;
    }

    af.where	 = TO_SHIELDS;
    af.type	 = sn;
    af.level	 = level;
    af.duration  = level / 4;
    af.location  = APPLY_AC;
    af.modifier  = (level / 5) * -1;
    af.bitvector = SHLD_FORCE;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "A sparkling force-shield encircles $n.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "You are encircled by a sparkling force-shield.", victim, NULL, NULL, TO_CHAR, POS_RESTING );

    return;
}


void
spell_ghost_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    AFFECT_DATA	af;

    victim = (CHAR_DATA *)vo;
    if ( IS_SHIELDED( victim, SHLD_GHOST ) )
        return;

    af.where	= TO_SHIELDS;
    af.type	= sn;
    af.level	= level;
    af.duration	= UMAX( 5, level / 8 );
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    af.bitvector= SHLD_GHOST;
    affect_to_char( victim, &af );
    act_color( AT_MAGIC, "You conjure hundreds of ghosts to surround you.", ch, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n conjures hundreds of ghosts to surround $m.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    return;
}


void
spell_goodberry( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *	obj;
    OBJ_DATA *	berry;

    obj = (OBJ_DATA *)vo;

    if ( obj->item_type != ITEM_FOOD || IS_OBJ_STAT( obj, ITEM_MAGIC ) )
    {
	send_to_char( "You can do nothing to that item.\n\r", ch );
	return;
    }

    act_color( AT_MAGIC, "You pass your hand over $p slowly.", ch, obj, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n has created a goodberry.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    berry = create_object( get_obj_index( OBJ_VNUM_BERRY ), 0 );
    berry->timer = ch->level;
    berry->value[0] = ch->level * 2;
    berry->value[1] = ch->level * 5;
    extract_obj( obj );
    obj_to_char( berry, ch );
    return;
}


void
spell_mana( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    int		eff_level;
    int		mana;

    victim = (CHAR_DATA *)vo;
    eff_level = UMIN( level, ch->level );
    mana = ( dice( 2, 8 ) + eff_level / 4 ) * 3;
    victim->mana = UMIN( victim->mana + mana, victim->max_mana );
    update_pos( victim );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );

    act_color( AT_MAGIC, "You feel a surge of energy.", victim, NULL, NULL, TO_CHAR, POS_RESTING );
    return;
}


void
spell_nexus( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *		victim;
    OBJ_DATA *		portal;
    OBJ_DATA *		stone;
    ROOM_INDEX_DATA *	to_room;
    ROOM_INDEX_DATA *	from_room;

    from_room = ch->in_room;

        if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   ( to_room = victim->in_room ) == NULL
    ||   !can_see_room( ch, to_room )
    ||	 !can_see_room( ch, from_room )
    ||   IS_SET( to_room->room_flags, ROOM_SAFE )
    ||	 IS_SET( from_room->room_flags,ROOM_SAFE )
    ||   IS_SET( to_room->room_flags, ROOM_PRIVATE )
    ||   IS_SET( to_room->room_flags, ROOM_SOLITARY )
    ||   IS_SET( to_room->room_flags, ROOM_NO_RECALL )
    ||   IS_SET( from_room->room_flags, ROOM_NO_RECALL )
    ||	 IS_SET( ch->in_room->room_flags, ROOM_NO_IN )
    ||	 IS_SET( ch->in_room->room_flags, ROOM_NO_OUT )
    ||	 IS_SET( victim->in_room->room_flags, ROOM_NO_IN )
    ||	 IS_SET( victim->in_room->room_flags, ROOM_NO_OUT )
    ||	 IS_SET( victim->in_room->room_flags, ROOM_NO_MAGIC )
    ||   victim->level >= level + 3
    ||   ( !IS_NPC( victim ) && victim->level >= LEVEL_HERO )  /* NOT trust */
    ||   ( IS_NPC( victim ) && IS_SET( victim->imm_flags, IMM_SUMMON ) )
    ||   ( IS_NPC( victim ) && saves_spell( level, victim, DAM_NONE ) )
    ||	 ( !is_same_landmass( ch->in_room, victim->in_room ) && ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    ||	 ( is_clan( victim ) && !is_same_clan( ch, victim ) ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    stone = get_eq_char( ch, WEAR_HOLD );
    if ( !IS_IMMORTAL( ch )
    &&  ( stone == NULL || stone->item_type != ITEM_WARP_STONE ) )
    {
        send_to_char( "You lack the proper component for this spell.\n\r", ch );
        return;
    }

    if ( stone != NULL && stone->item_type == ITEM_WARP_STONE )
    {
        act( "You draw upon the power of $p.", ch, stone, NULL, TO_CHAR );
        act( "It flares brightly and vanishes!", ch, stone, NULL, TO_CHAR );
        extract_obj( stone );
    }

    /* portal one */
    portal = create_object( get_obj_index( OBJ_VNUM_PORTAL), 0 );
    portal->timer = 1 + level / 10;
    portal->value[3] = to_room->vnum;

    obj_to_room( portal, from_room );

    act( "$p rises up from the ground.", ch, portal, NULL, TO_ROOM );
    act( "$p rises up before you.", ch, portal, NULL, TO_CHAR );

    /* no second portal if rooms are the same */
    if ( to_room == from_room )
	return;

    /* portal two */
    portal = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    portal->timer = 1 + level/10;
    portal->value[3] = from_room->vnum;

    obj_to_room( portal, to_room );

    if ( to_room->people != NULL )
    {
	act( "$p rises up from the ground.", to_room->people, portal, NULL, TO_ROOM );
	act( "$p rises up from the ground.", to_room->people, portal, NULL, TO_CHAR );
    }
}


void
spell_portal( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *	victim;
    OBJ_DATA *	portal;
    OBJ_DATA *	stone;

        if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   !can_see_room( ch, victim->in_room )
    ||   IS_SET( victim->in_room->room_flags, ROOM_SAFE )
    ||   IS_SET( victim->in_room->room_flags, ROOM_PRIVATE )
    ||   IS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
    ||   IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
    ||   IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
    ||	 IS_SET( ch->in_room->room_flags, ROOM_NO_OUT )
    ||	 IS_SET( victim->in_room->room_flags, ROOM_NO_IN )
    ||	 IS_SET( victim->in_room->room_flags, ROOM_NO_MAGIC )
    ||   victim->level >= level + 3
    ||   ( !IS_NPC( victim ) && victim->level >= LEVEL_HERO )  /* NOT trust */
    ||   ( IS_NPC( victim ) && IS_SET( victim->imm_flags,IMM_SUMMON ) )
    ||   ( IS_NPC( victim ) && saves_spell( level, victim,DAM_NONE ) )
    ||	 ( is_clan( victim ) && !is_same_clan( ch, victim ) ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    stone = get_eq_char( ch, WEAR_HOLD );
    if ( !IS_IMMORTAL( ch )
    &&  ( stone == NULL || stone->item_type != ITEM_WARP_STONE ) )
    {
	send_to_char( "You lack the proper component for this spell.\n\r", ch );
	return;
    }

    if ( stone != NULL && stone->item_type == ITEM_WARP_STONE )
    {
     	act( "You draw upon the power of $p.", ch, stone, NULL, TO_CHAR );
     	act( "It flares brightly and vanishes!", ch, stone, NULL, TO_CHAR );
     	extract_obj( stone );
    }

    portal = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    portal->timer = 2 + level / 25;
    portal->value[3] = victim->in_room->vnum;

    obj_to_room( portal, ch->in_room );

    act( "$p rises up from the ground.", ch, portal, NULL, TO_ROOM );
    act( "$p rises up before you.", ch, portal, NULL, TO_CHAR );
}


void
spell_resurrect( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    OBJ_DATA *	corpse;
    EVENT_DATA *event;

    victim = (CHAR_DATA *)vo;

    if ( victim == ch )
    {
        send_to_char( "You can't resurrect yourself.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char( "You can only resurrect players.\n\r", ch );
        return;
    }

    if ( !IS_DEAD( victim ) )
    {
        act( "$E doesn't need to be resurrected.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( !IS_NPC( ch )
    &&	 ch->class == class_cleric
    &&	 ch->in_room->sector_type != SECT_GRAVEYARD )
    {
        send_to_char( "You can only do this in a graveyard.\n\r", ch );
        return;
    }

    victim->pcdata->deathstate = DEATH_NONE;
    victim->pcdata->deathroom  = 0;
    victim->pcdata->deathmob   = 0;

    act_color( AT_MAGIC, "You resurrect $N.", ch, NULL, victim, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n resurrects you.", ch, NULL, victim, TO_VICT, POS_RESTING );
    act_color( AT_MAGIC, "$n resurrects $N.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );

    reset_char( victim );

    event = new_event( );
    event->type = EVENT_CHAR_NOPK;
    event->fun = event_char_nopk;
    add_event_char( event, victim, number_range(  1 * 60 * PULSE_PER_SECOND,
                                                 10 * 60 * PULSE_PER_SECOND ) );

    if ( ( corpse = find_corpse( victim ) ) != NULL )
    {
        if ( corpse->in_room != NULL && corpse->in_room->people != NULL )
            act_color( AT_MAGIC, "$p disappears in a flash of light.",
                       corpse->in_room->people, corpse, NULL, TO_ALL, POS_RESTING );
        else if ( corpse->carried_by != NULL )
            act_color( AT_MAGIC, "$p disappears in a flash of light.",
                       corpse->carried_by, corpse, NULL, TO_CHAR, POS_RESTING );
        extract_obj( corpse );
    }

}

void
spell_scry( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    int			door;
    EXIT_DATA *		pExit;
    ROOM_INDEX_DATA *	from_room;
    ROOM_INDEX_DATA *	to_room;

    if ( !check_blind( ch ) )
	return;

    if ( ( door = dir_lookup( target_name ) ) == DIR_NONE )
    {
	send_to_char( "Spell failed.\n\r", ch );
	return;
    }

    if ( ( pExit = get_exit( ch->in_room, door ) ) == NULL
    ||	 ( to_room = pExit->to_room ) == NULL
    ||	 !can_see_room( ch, to_room )
    ||	 room_is_private( to_room ) )
    {
	send_to_char( "Spell failed.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch )
    &&	 ( !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
    &&	 ch->class == class_druid
    &&	 IS_SET( to_room->room_flags, ROOM_INDOORS ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    from_room = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, to_room );
    do_look( ch, "auto" );
    char_from_room( ch );
    char_to_room( ch, from_room );
    eprog_scry_trigger( pExit, ch->in_room, ch );
}


void
spell_shrink( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA	af;
    CHAR_DATA *	victim;

    victim = (CHAR_DATA *)vo;

    if ( is_affected( victim, gsn_shrink ) || victim->size <= SIZE_TINY )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( is_affected( victim, gsn_enlarge ) )
    {
	if ( !check_dispel( level, victim, gsn_enlarge ) )
	    send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level / 2;
    af.location	 = APPLY_SIZE;
    af.modifier	 = UMAX( SIZE_TINY - victim->size, -3 );
    af.bitvector = AFF_NONE;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "You shrink to the size of a mouse.", victim, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n shrinks to the size of a mouse.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
}


void
spell_staticshield( int sn, int level, CHAR_DATA * ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    AFFECT_DATA af;

    victim = (CHAR_DATA *)vo;

    if ( is_affected( victim, sn ) )
    {
	if ( victim == ch )
	    send_to_char( "You are already surrounded by static charge.\n\r", ch );
	else
	    act ( "$N is already surrounded by static charge.", ch, NULL, victim,
		  TO_CHAR );
	return;
    }

    af.where	 = TO_SHIELDS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level / 3;
    af.location  = APPLY_AC;
    af.modifier  = (level / 4) * -1;
    af.bitvector = SHLD_STATIC;
    affect_to_char( victim, &af );

    act_color( AT_MAGIC, "$n is surrounded by a pulse of static charge.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "You are surrounded by a pulse of static charge.", victim, NULL, NULL, TO_CHAR, POS_RESTING );

    return;
}


void
spell_summon_fire_elemental( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	mob;
    CHAR_DATA *	fch;
    AFFECT_DATA	af;

    if( ch->summon_timer > 0 )
    {
        send_to_char( "You cast the spell, but nothing appears.\n\r", ch );
        return;
    }

    mob = create_mobile( get_mob_index( MOB_VNUM_FIRE_ELEMENTAL ) );
    mob->level = URANGE( 51, level, 100 ) - 5;
    set_mob_stats( mob, DICE_HARD );
    mob->summon_timer = level/10;
    ch->summon_timer = 16;
    char_to_room(mob, ch->in_room);
    act_color(AT_MAGIC, "You summon $N from a blazing inferno.", ch, NULL, mob, TO_CHAR, POS_RESTING);
    if ( ch->mana < level * 2 )
    {
        act_color( AT_MAGIC,
		   "You don't have enough mana to bind $N!",
		   ch, NULL, mob, TO_CHAR, POS_RESTING );
        extract_char( mob, TRUE );
        return;
    }
    ch->mana -= level * 2;
    act_color( AT_MAGIC, "$n summons $N from a blazing inferno.", ch, NULL, mob, TO_ROOM, POS_RESTING);


    mob->master = ch;
    mob->leader = ch;
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("charm person");
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if( ch->position == POS_FIGHTING )
    {
    act_color( AT_ACTION, "$n rescues you!", mob, NULL, ch, TO_VICT, POS_RESTING    );
    act_color( AT_ACTION, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT, POS_RESTING );

        fch = ch->fighting;
        stop_fighting( fch, FALSE );
        stop_fighting( ch, FALSE );
        set_fighting( mob, fch );
        set_fighting( fch, mob );
    }
    return;
}

void
spell_summon_air_elemental( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	mob;
    CHAR_DATA *	fch;
    AFFECT_DATA	af;

    if( ch->summon_timer > 0 )
    {
        send_to_char( "You cast the spell, but nothing appears.\n\r", ch );
        return;
    }

    mob = create_mobile( get_mob_index( MOB_VNUM_AIR_ELEMENTAL ) );
    mob->level = URANGE( 51, level, 100 ) - 5;
    set_mob_stats( mob, DICE_HARD );
    mob->summon_timer = level/10;
    ch->summon_timer = 16;
    char_to_room(mob, ch->in_room);
    act_color(AT_MAGIC, "You summon $N from the air around you.", ch, NULL, mob, TO_CHAR, POS_RESTING);
    if ( ch->mana < level * 2 )
    {
        act_color( AT_MAGIC,
		   "You don't have enough mana to bind $N!",
		   ch, NULL, mob, TO_CHAR, POS_RESTING );
        extract_char( mob, TRUE );
        return;
    }
    ch->mana -= level * 2;
    act_color( AT_MAGIC, "$n summons $N from the very air around themselves.", ch, NULL, mob, TO_ROOM, POS_RESTING);


    mob->master = ch;
    mob->leader = ch;
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("charm person");
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if( ch->position == POS_FIGHTING )
    {
    act_color( AT_ACTION, "$n rescues you!", mob, NULL, ch, TO_VICT, POS_RESTING    );
    act_color( AT_ACTION, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT, POS_RESTING );

        fch = ch->fighting;
        stop_fighting( fch, FALSE );
        stop_fighting( ch, FALSE );
        set_fighting( mob, fch );
        set_fighting( fch, mob );
    }
    return;
}

void
spell_summon_earth_elemental( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	mob;
    CHAR_DATA *	fch;
    AFFECT_DATA	af;

    if( ch->summon_timer > 0 )
    {
        send_to_char( "You cast the spell, but nothing appears.\n\r", ch );
        return;
    }

    mob = create_mobile( get_mob_index( MOB_VNUM_EARTH_ELEMENTAL ) );
    mob->level = URANGE( 51, level, 100 ) - 5;
    set_mob_stats( mob, DICE_HARD );
    mob->summon_timer = level/10;
    ch->summon_timer = 16;
    char_to_room(mob, ch->in_room);
    act_color(AT_MAGIC, "You summon $N from the ground at your feet.", ch, NULL, mob, TO_CHAR, POS_RESTING);
    if ( ch->mana < level * 2 )
    {
        act_color( AT_MAGIC,
		   "You don't have enough mana to bind $N!",
		   ch, NULL, mob, TO_CHAR, POS_RESTING );
        extract_char( mob, TRUE );
        return;
    }
    ch->mana -= level * 2;
    act_color( AT_MAGIC, "$n summons $N from the ground at their feet.", ch, NULL, mob, TO_ROOM, POS_RESTING);


    mob->master = ch;
    mob->leader = ch;
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("charm person");
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if( ch->position == POS_FIGHTING )
    {
    act_color( AT_ACTION, "$n rescues you!", mob, NULL, ch, TO_VICT, POS_RESTING    );
    act_color( AT_ACTION, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT, POS_RESTING );

        fch = ch->fighting;
        stop_fighting( fch, FALSE );
        stop_fighting( ch, FALSE );
        set_fighting( mob, fch );
        set_fighting( fch, mob );
    }
    return;
}

void
spell_summon_water_elemental( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	mob;
    CHAR_DATA *	fch;
    AFFECT_DATA	af;

    if( ch->summon_timer > 0 )
    {
        send_to_char( "You cast the spell, but nothing appears.\n\r", ch );
        return;
    }

    mob = create_mobile( get_mob_index( MOB_VNUM_WATER_ELEMENTAL ) );
    mob->level = URANGE( 51, level, 100 ) - 5;
    set_mob_stats( mob, DICE_HARD );
    mob->summon_timer = level/10;
    ch->summon_timer = 16;
    char_to_room(mob, ch->in_room);
    act_color(AT_MAGIC, "You summon $N from the water around you.", ch, NULL, mob, TO_CHAR, POS_RESTING);
    if ( ch->mana < level * 2 )
    {
        act_color( AT_MAGIC,
		   "You don't have enough mana to bind $N!",
		   ch, NULL, mob, TO_CHAR, POS_RESTING );
        extract_char( mob, TRUE );
        return;
    }
    ch->mana -= level * 2;
    act_color( AT_MAGIC, "$n summons $N from the water around themselves.", ch, NULL, mob, TO_ROOM, POS_RESTING);


    mob->master = ch;
    mob->leader = ch;
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("charm person");
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if( ch->position == POS_FIGHTING )
    {
    act_color( AT_ACTION, "$n rescues you!", mob, NULL, ch, TO_VICT, POS_RESTING    );
    act_color( AT_ACTION, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT, POS_RESTING );

        fch = ch->fighting;
        stop_fighting( fch, FALSE );
        stop_fighting( ch, FALSE );
        set_fighting( mob, fch );
        set_fighting( fch, mob );
    }
    return;
}

void
spell_summon_ggolem( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	gch;
    CHAR_DATA *	golem;
    AFFECT_DATA af;
    int		i;

    if ( is_affected( ch, sn ) )
    {
	send_to_char( "You lack the power to summon another golem right now.\n\r", ch );
	return;
    }

    send_to_char( "You attempt to summon a greater golem.\n\r", ch );

    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC( gch ) && IS_AFFECTED( gch, AFF_CHARM )
	&&   gch->master == ch
	&&   gch->pIndexData->vnum == MOB_VNUM_GGOLEM )
	{
	    send_to_char( "You cannot control another golem!\n\r", ch );
	    act_color( AT_MAGIC, "$n attempts to summon a greater golem.",
		       ch, NULL, NULL, TO_ROOM, POS_RESTING );
	    return;
	}
    }

    golem = create_mobile( get_mob_index( MOB_VNUM_GGOLEM ) );

    for ( i = 0; i < MAX_STATS; i++ )
	golem->perm_stat[i] = UMIN( 22, 15 + ch->level/10 );

    golem->perm_stat[STAT_STR] += 3;
    golem->perm_stat[STAT_INT] -= 1;
    golem->perm_stat[STAT_CON] += 2;

    golem->level = ch->level;
    golem->max_hit = IS_NPC( ch )? URANGE( ch->max_hit, 2 * ch->max_hit, 30000 )
		: UMIN( ( 10 * ch->pcdata->perm_hit ) + 4000, 30000 );

    golem->hit = golem->max_hit;
    golem->max_mana = IS_NPC( ch )? ch->max_mana : ch->pcdata->perm_mana;
    golem->mana = golem->max_mana;
    for ( i=0; i < 3; i++ )
	golem->armor[i] = interpolate( golem->level, 100, -100 );
    golem->armor[3] = interpolate( golem->level, 100, 0 );
    golem->timer = 0;
    golem->damage[DICE_NUMBER] = 13;
    golem->damage[DICE_TYPE] = 9;
    golem->damage[DICE_BONUS] = ch->level / 2 + 10;

    char_to_room( golem, ch->in_room );

    act_color( AT_MAGIC, "You summon a greater golem!",
	       ch, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n summons a greater golem!",
	       ch, NULL, NULL, TO_ROOM, POS_RESTING );

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = 30;
    af.bitvector = 0;
    af.modifier	 = 0;
    af.location	 = APPLY_NONE;
    affect_to_char( ch, &af );

    xSET_BIT( golem->affected_by, AFF_CHARM );
    golem->master = golem->leader = ch;

}


void
spell_summon_greater_wolf( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	mob;
    CHAR_DATA *	fch;
    AFFECT_DATA af;

    if ( ch->summon_timer > 0 )
    {
	send_to_char( "You cast the spell, but nothing appears.\n\r", ch );
	return;
    }

    mob = create_mobile( get_mob_index( MOB_VNUM_GREATER_WOLF ) );
    mob->level = URANGE( 90, level, 110 ) - 10;
    set_mob_stats( mob, DICE_HARD );
    mob->summon_timer = level/10;
    ch->summon_timer = 16;
    char_to_room( mob, ch->in_room );
    act_color( AT_MAGIC, "You summon $N from the plane of nature.", ch, NULL, mob, TO_CHAR, POS_RESTING );

    if( ch->mana < level * 2 )
    {
	act_color( AT_MAGIC, "You don't have enough mana to bind $N!",
		   ch, NULL, mob, TO_CHAR, POS_RESTING );
	extract_char( mob, TRUE );
	return;
    }

    ch->mana -= level * 2;
    act_color( AT_MAGIC, "$n calls forth $N from the plane of nature.",
	       ch, NULL, mob, TO_ROOM, POS_RESTING );

    mob->master  = ch;
    mob->leader  = ch;
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup( "charm person" );
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if ( ch->position == POS_FIGHTING )
    {
	act_color( AT_WHITE, "$n rescues you!", mob, NULL, ch, TO_VICT, POS_RESTING );
	act_color( AT_WHITE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT, POS_RESTING );

	fch = ch->fighting;
	stop_fighting( fch, FALSE );
        stop_fighting( ch,  FALSE );
	set_fighting ( mob, fch );
	set_fighting ( fch, mob );
    }

    return;
}


void
spell_summon_hawk( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	mob;
    CHAR_DATA *	fch;
    AFFECT_DATA	af;

    if ( ch->summon_timer > 0 )
    {
	send_to_char( "You cast the spell, but nothing appears.\n\r", ch );
	return;
    }

    mob = create_mobile( get_mob_index( MOB_VNUM_HAWK ) );
    mob->level = URANGE( 31, level, 60 ) - 10;
    set_mob_stats( mob, DICE_MEDIUM );
    mob->summon_timer = level/10;
    ch->summon_timer = 16;
    char_to_room( mob, ch->in_room );
    act_color( AT_MAGIC,
	       "You summon $N from the plane of nature.",
	       ch, NULL, mob, TO_CHAR, POS_RESTING );
    if( ch->mana < level * 2 )
    {
	act_color( AT_MAGIC,
		   "You don't have enough mana to bind $N!",
		   ch, NULL, mob, TO_CHAR, POS_RESTING );
	extract_char( mob, TRUE );
	return;
    }

    ch->mana -= level * 2;
    act_color( AT_MAGIC, "$n calls forth $N from the plane of nature.",
	       ch, NULL, mob, TO_ROOM, POS_RESTING );

    mob->master = ch;
    mob->leader = ch;
    af.where	 = TO_AFFECTS;
    af.type      = skill_lookup( "charm person" );
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if ( ch->position == POS_FIGHTING )
    {
	act_color( AT_WHITE, "$n rescues you!",
		   mob, NULL, ch, TO_VICT, POS_RESTING );
	act_color( AT_WHITE, "$n rescues $N!",
		   mob, NULL, ch, TO_NOTVICT, POS_RESTING );

	fch = ch->fighting;
	stop_fighting( fch, FALSE );
	stop_fighting( ch, FALSE );
	set_fighting( mob, fch );
	set_fighting( fch, mob );
    }

    return;
}


void
spell_summon_tiger( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	mob;
    CHAR_DATA *	fch;
    AFFECT_DATA af;

    if ( ch->summon_timer > 0 )
    {
	send_to_char( "You cast the spell, but nothing appears.\n\r", ch );
	return;
    }

    mob = create_mobile( get_mob_index( MOB_VNUM_TIGER ) );
    mob->level = URANGE( 41, level, 70 ) - 10;
    mob->max_hit = mob->level * 30 + dice( 20, mob->level );
    mob->hit = mob->max_hit;
    mob->summon_timer = level/10;
    ch->summon_timer = 16;
    char_to_room( mob, ch->in_room );
    act_color( AT_MAGIC, "You summon $N from the plane of nature.", ch, NULL, mob, TO_CHAR, POS_RESTING );

    if( ch->mana < level * 2 )
    {
	act_color( AT_MAGIC, "You don't have enough mana to bind $N!",
		   ch, NULL, mob, TO_CHAR, POS_RESTING );
	extract_char(mob, TRUE);
	return;
    }

    ch->mana -= level * 2;
    act_color( AT_MAGIC, "$n calls forth $N from the plane of nature.", ch, NULL, mob, TO_ROOM, POS_RESTING );

    mob->master = ch;
    mob->leader = ch;
    af.type      = skill_lookup( "charm person" );
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if( ch->position == POS_FIGHTING )
    {
	act_color( AT_WHITE, "$n rescues you!", mob, NULL, ch, TO_VICT, POS_RESTING );
	act_color( AT_WHITE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT, POS_RESTING );

	fch = ch->fighting;
	stop_fighting( fch, FALSE );
	stop_fighting( ch,  FALSE );
	set_fighting ( mob, fch );
	set_fighting ( fch, mob );
    }

    return;
}


void
spell_summon_wolf( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	mob;
    CHAR_DATA *	fch;
    AFFECT_DATA	af;

    if ( ch->summon_timer > 0 )
    {
        send_to_char( "You cast the spell, but nothing appears.\n\r", ch );
        return;
    }

    mob = create_mobile( get_mob_index( MOB_VNUM_WOLF ) );
    mob->level = URANGE( 31, level, 60 ) - 10;
    set_mob_stats( mob, DICE_HARD );
    mob->summon_timer = level/10;
    ch->summon_timer = 16;
    char_to_room( mob, ch->in_room );
    act_color( AT_MAGIC, "You summon $N from the plane of nature.", ch, NULL, mob, TO_CHAR, POS_RESTING);
    if( ch->mana < level * 2 )
    {
        act_color( AT_WHITE,
                   "You don't have enough mana to bind $N!", ch, NULL,
                   mob, TO_CHAR, POS_RESTING );
        extract_char(mob, TRUE);
        return;
    }
  ch->mana -= level * 2;
  act_color( AT_MAGIC, "$n calls forth $N from the plane of nature.", ch, NULL, mob, TO_ROOM, POS_RESTING );

    mob->master  = ch;
    mob->leader  = ch;
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup( "charm person" );
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if( ch->position == POS_FIGHTING )
    {
        act_color( AT_WHITE, "$n rescues you!", mob, NULL, ch, TO_VICT, POS_RESTING );
        act_color( AT_WHITE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT, POS_RESTING );

        fch = ch->fighting;
        stop_fighting( fch, FALSE );
        stop_fighting( ch, FALSE );
        set_fighting( mob, fch );
        set_fighting( fch, mob );
    }
    return;
}

/* Wail of the Banshee spell
 * Add's DEAF affect.
 * REN - 1/22/07
 */
void
spell_wail_of_the_banshee( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	vict;
    AFFECT_DATA af;
    int		dam;
    int		dur;
    bool	fail;

    vict = (CHAR_DATA *) vo;

    if ( ch == vict )
    {
	send_to_char( "That would be silly!\n\r", ch );
	return;
    }

    if ( is_affected( vict, sn ) || xIS_SET( vict->affected_by, AFF_DEAF ) )
    {
	send_to_char( "They are already deaf!\n\r", ch );
	return;
    }

    dam = dice( level/2, 8 );
    dur = dice( level/2, 2 );

    if( saves_spell( level, vict, DAM_SOUND ) )
    {
	if ( saves_spell( level, vict, DAM_SOUND ) )
        {
	    damage( ch, vict, dam/2, sn, DAM_SOUND, TRUE );
	    fail = TRUE;
        }
	else
        {
	    damage( ch, vict, dam/2, sn, DAM_SOUND, TRUE );

	    af.where     = TO_AFFECTS;
	    af.type      = sn;
	    af.level     = level/2;
	    af.duration  = dur/2;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_DEAF;

	    fail = FALSE;
        }
    }
    else
    {
	damage( ch, vict, dam, sn, DAM_SOUND, TRUE );

        af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = dur;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_DEAF;

        fail = FALSE;
    }
    if ( !fail )
    {
        affect_join( vict, &af );

        act_color( AT_MAGIC, "You're ears start to `rbl`Re`red`X!", vict, NULL, NULL, TO_CHAR, POS_RESTING );
        act_color( AT_MAGIC, "$n's ears start to `rbl`Re`red`X!",vict, NULL, NULL, TO_ROOM, POS_RESTING );
    }
    else
    {
	act_color( AT_MAGIC, "Despite the loud wail, $n appears unaffacted.", vict, NULL, NULL, TO_ROOM, POS_RESTING );
	act_color (AT_MAGIC, "You hear a loud keening, but manage to protect your ears from serious damage.",
		   vict, NULL, NULL, TO_CHAR, POS_RESTING );
    }

    return;
}


/* The purpose of this spell is mostly to set a value for gsn_weariness */
void
spell_weariness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim;
    AFFECT_DATA	af;

    victim = (CHAR_DATA *)vo;

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = 5;
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    af.bitvector = AFF_WEARINESS;

    affect_join( victim, &af );

    act_color( AT_MAGIC, "You feel weary.", victim, NULL, NULL, TO_CHAR, POS_RESTING );
    act_color( AT_MAGIC, "$n looks weary.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    return;
}

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
