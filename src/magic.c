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
#include <sys/types.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"

/*
 * The kludgy global is for spells who want more stuff from command line.
 * It should be of type "const char *" too, but that'll involve a *lot* of work.
 */
char *target_name;

/*
 * Local functions.
 */
bool	event_char_heal	args( ( EVENT_DATA *pEvent ) );
int	get_heal_rate	args( ( int level ) );
bool	heal_spell	args( ( CHAR_DATA *victim, int total, int amt, int level ) );
bool	is_sn		args( ( int sn ) );
void	magic_mob	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum ) );
void	say_spell	args( ( CHAR_DATA *ch, int sn ) );

/* imported functions */
bool    remove_obj      args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );


bool
check_spellcraft( CHAR_DATA *ch, int sn )
{
    OBJ_DATA *	ring;
    int		chance;

    chance = get_skill( ch, gsn_spellcraft );
    if ( chance == 0 || ch->level < skill_table[gsn_spellcraft].skill_level[ch->class ] )
        return FALSE;

    chance /= 5;

    ring = get_eq_char( ch, WEAR_FINGER_L );
    if ( ring != NULL && ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1 )
        chance += 2;
    else if ( ring != NULL && ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2 )
        chance += 5;

    ring = get_eq_char( ch, WEAR_FINGER_R );
    if ( ring != NULL && ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1 )
        chance += 2;
    else if ( ring != NULL && ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2 )
        chance += 5;

    if ( number_percent( ) > chance )
    {
        check_improve( ch, gsn_spellcraft, FALSE, 6 );
        return FALSE;
    }

    check_improve( ch, gsn_spellcraft, TRUE, 6 );
    return TRUE;
}


/*
Booster function finds the spellcraft damage..higher average damage and
much higher min. Max is same per spell though
*/
int
spellcraft_dam( int num, int die )
{
    int		dam;

    if ( num == 0 || die == 0 )
        return 0;

    if ( die == 1 )
        return num;
    else if ( die == 2 )
        return ( num*2 );
    else if ( die == 3 )
        return ( num * number_range( 2, 3 ) );

    dam = ( num * die ) / 2;
    dam += dice( num/2, die );
    return dam;
}


/*
 * Lookup a skill by name.
 */
int
skill_lookup( const char *name )
{
    int sn;

    for ( sn = 0; sn < top_skill; sn++ )
    {
	if ( skill_table[sn].name == NULL )
	    break;
	if ( LOWER(name[0]) == LOWER(skill_table[sn].name[0])
	&&   !str_prefix( name, skill_table[sn].name ) )
	    return sn;
    }

    return NO_SKILL;
}


/*
 * Lookup a skill by name.
 * Exact match required.
 */
int
skill_find( const char *name )
{
    int sn;

    for ( sn = 0; sn < top_skill; sn++ )
    {
	if ( skill_table[sn].name == NULL )
	    break;
	if ( LOWER(name[0]) == LOWER(skill_table[sn].name[0])
	&&   !str_cmp( name, skill_table[sn].name ) )
	    return sn;
    }

    return NO_SKILL;
}


int
find_spell( CHAR_DATA *ch, const char *name )
{
    /* finds a spell the character can cast if possible */
    int sn, found = -1;

    if ( IS_NPC( ch ) )
	return skill_lookup( name );

    for ( sn = 0; sn < top_skill; sn++ )
    {
	if ( skill_table[sn].name == NULL )
	    break;
	if ( LOWER( name[0]) == LOWER( skill_table[sn].name[0] )
	&&  !str_prefix( name, skill_table[sn].name ) )
	{
	    if ( found == -1)
		found = sn;
	    if ( ch->level >= skill_table[sn].skill_level[ch->class]
	    &&   ch->pcdata->skill[sn].percent > 0)
		    return sn;
	}
    }
    return found;
}


bool
event_char_heal( EVENT_DATA *event )
{
    CHAR_DATA *	ch;
    EVENT_DATA *pEvent;
    int		amt;

    ch = event->owner.ch;

    if ( ch == NULL || ch->deleted )
    {
	bug( "Event_char_heal: invalid char.", 0 );
	return FALSE;
    }

    amt = UMIN( event->value1, ( ch->max_hit - ch->hit ) );
    if ( amt <= 0 )
	return FALSE;
    ch->hit += amt;
    update_pos( ch );
    if ( ch->hit >= ch->max_hit || event->value0 - amt <= 0 )
	return FALSE;

    pEvent = new_event( );
    pEvent->fun = event_char_heal;
    pEvent->type = EVENT_CHAR_HEAL;
    amt = UMIN( amt, ch->max_hit - ch->hit );
    pEvent->value2 = event->value2;
    pEvent->value1 = UMIN( event->value1, amt );
    pEvent->value0 = event->value0 - amt;
    add_event_char( pEvent, ch, pEvent->value2 );

    return FALSE;
}


/* Replaced get_heal_rate with math function, instead of giant else if chain */
int
get_heal_rate( int level )
{
    if ( level >= LEVEL_IMMORTAL )
        return 1;
    else
        return ( 12 - ( (int)( level / 10 ) ) );
}
/*
int
get_heal_rate( int level )
{
    if ( level >= LEVEL_IMMORTAL )
	return 1;
    else if ( level >= LEVEL_HERO )
	return 2;
    else if ( level >= 90 )
	return 3;
    else if ( level >= 80 )
	return 4;
    else if ( level >= 70 )
	return 5;
    else if ( level >= 60 )
	return 6;
    else if ( level >= 50 )
	return 7;
    else if ( level >= 40 )
	return 8;
    else if ( level >= 30 )
	return 9;
    else if ( level >= 20 )
	return 10;
    else if ( level >= 10 )
	return 11;
    else
	return 12;
}
*/

bool
heal_spell( CHAR_DATA *victim, int total, int amt, int level )
{
    EVENT_DATA *	event;

    if ( victim == NULL || victim->deleted )
	return FALSE;

    strip_event_char( victim, EVENT_CHAR_HEAL );
    total = UMIN( total, victim->max_hit - victim->hit );
    amt = UMIN( amt, total );
    if ( total <= 0 )
	return FALSE;

    event = new_event( );
    event->fun = event_char_heal;
    event->type = EVENT_CHAR_HEAL;
    event->value0 = total;
    event->value1 = amt;
    event->value2 = get_heal_rate( level );
    add_event_char( event, victim, event->value2 );
    return TRUE;
}


/*
 * Return TRUE if sn is a valid skill/spell
 */
bool
is_sn( int sn )
{
    int cnt;

    for ( cnt = 1; skill_table[cnt].name; cnt++ )
	if ( cnt == sn )
	    return TRUE;
    return FALSE;
}


/* Magic_Mob for Animate Spell */

void
magic_mob ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum )
{
   CHAR_DATA      *victim;
   CHAR_DATA      *zombie;
   MOB_INDEX_DATA *ZombIndex;
   MOB_INDEX_DATA *pMobIndex;
   char           *name;
   char            buf [MAX_STRING_LENGTH];

    if ( !( pMobIndex = get_mob_index( vnum ) ) )
    {
         send_to_char( "Nothing happens.\n\r", ch);
         return;
    }
    ZombIndex = get_mob_index( MOB_VNUM_CORPSE );
    victim = create_mobile( pMobIndex );
    zombie = create_mobile( ZombIndex );
    name = victim->short_descr;
    sprintf( buf, zombie->short_descr, name );
    free_string( zombie->short_descr );
    zombie->short_descr = str_dup(buf);
    sprintf( buf, zombie->long_descr, name );
    free_string( zombie->long_descr );
    zombie->long_descr = str_dup(buf);
    victim->max_hit /= 2;
    victim->hit = victim->max_hit;
    zombie->max_hit = victim->max_hit;
    zombie->hit = victim->hit;
    zombie->level = victim->level;
    SET_BIT( zombie->act, ACT_UNDEAD );
    //SET_BIT( zombie->act, ACT_PET );
    xSET_BIT( zombie->affected_by, AFF_CHARM );
    zombie->summon_timer = ch->level / 4 + 20;
    char_to_room( zombie, ch->in_room );
    add_follower( zombie, ch );
    update_pos( zombie );
    act_color( AT_MAGIC, "$n passes $s hands over $p, $E slowly rises to serve $S new master.", ch, obj, zombie, TO_ROOM, POS_RESTING );
    act_color( AT_MAGIC, "You animate $p, it rises to serve you.", ch, obj, NULL, TO_CHAR, POS_RESTING );
    char_to_room( victim, ch->in_room );
    extract_char ( victim, TRUE );
    return;
}


/*
 * Utter mystical words for an sn.
 */
void
say_spell( CHAR_DATA *ch, int sn )
{
    char buf  [MAX_STRING_LENGTH];
    char buf2 [MAX_STRING_LENGTH];
    CHAR_DATA *rch;
    char *pName;
    int iSyl;
    int length;

    struct syl_type
    {
	char *	old;
	char *	new;
    };

    static const struct syl_type syl_table[] =
    {
	{ " ",		" "		},
	{ "ar",		"abra"		},
	{ "au",		"kada"		},
	{ "bless",	"fido"		},
	{ "blind",	"nose"		},
	{ "bur",	"mosa"		},
	{ "cu",		"judi"		},
	{ "de",		"oculo"		},
	{ "en",		"unso"		},
	{ "light",	"dies"		},
	{ "lo",		"hi"		},
	{ "mor",	"zak"		},
	{ "move",	"sido"		},
	{ "ness",	"lacri"		},
	{ "ning",	"illa"		},
	{ "per",	"duda"		},
	{ "ra",		"gru"		},
	{ "fresh",	"ima"		},
	{ "re",		"candus"	},
	{ "son",	"sabru"		},
	{ "tect",	"infra"		},
	{ "tri",	"cula"		},
	{ "ven",	"nofo"		},
	{ "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
	{ "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
	{ "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
	{ "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
	{ "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
	{ "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
	{ "y", "l" }, { "z", "k" },
	{ "", "" }
    };

    buf[0]	= '\0';
    for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( syl_table[iSyl].old, pName ) )
	    {
		strcat( buf, syl_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    sprintf( buf2, "$n utters the words, '%s'.", buf );
    sprintf( buf,  "$n utters the words, '%s'.", skill_table[sn].name );

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch != ch )
	    act((!IS_NPC(rch) && ch->class==rch->class) ? buf : buf2,
	        ch, NULL, rch, TO_VICT );
    }

    return;
}


/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool
saves_spell( int level, CHAR_DATA *victim, int dam_type )
{
    int save;

    save = 50 + ( victim->level - level) * 5 - victim->saving_throw * 2;
    if (IS_AFFECTED(victim,AFF_BERSERK))
	save += victim->level/2;

    switch(check_immune(victim,dam_type))
    {
	case IS_IMMUNE:		return TRUE;
	case IS_RESISTANT:	save += 2;	break;
	case IS_VULNERABLE:	save -= 2;	break;
    }

    if ( !IS_NPC( victim ) && IS_SET( class_table[victim->class].flags, CLASS_FMANA ) )
	save = 9 * save / 10;
    save = URANGE( 5, save, 95 );
    return number_percent( ) < save;
}


/* RT save for dispels */
bool
saves_dispel( int dis_level, int spell_level, int duration)
{
    int save;

    if (duration == -1)
      spell_level += 5;
      /* very hard to dispel permanent effects */

    save = 50 + (spell_level - dis_level) * 5;
    save = URANGE( 5, save, 95 );
    return number_percent( ) < save;
}


/* co-routine for dispel magic and cancellation */
bool
check_dispel( int dis_level, CHAR_DATA *victim, int sn)
{
    AFFECT_DATA *af;

    if (is_affected(victim, sn))
    {
        for ( af = victim->affected; af != NULL; af = af->next )
        {
            if ( af->type == sn )
            {
                if (!saves_dispel(dis_level,af->level,af->duration))
                {
                    affect_strip(victim,sn);
        	    if ( skill_table[sn].msg_off && skill_table[sn].msg_off[0] != '\0' )
        	    {
			send_to_char( skill_table[sn].msg_off, victim );
			send_to_char( "\n\r", victim );
        	    }
		    if ( skill_table[sn].msg_room
			&& skill_table[sn].msg_room[0] != '\0'
			&& victim->in_room
			&& victim->in_room->people )
        	    {
			act_color( AT_DGREEN, skill_table[sn].msg_room,
				   victim, NULL, NULL, TO_ROOM, POS_RESTING );
        	    }
        	    if ( af->type == AFF_FLYING )
        	        check_nofloor( victim );
		    return TRUE;
		}
		else
		    af->level--;
            }
        }
    }
    return FALSE;
}

/* for finding mana costs -- temporary version */
/*
int
mana_cost (CHAR_DATA *ch, int min_mana, int level)
{
    if (ch->level + 2 == level)
	return 1000;
    return UMAX(min_mana,(100/(2 + ch->level - level)));
}
*/

void
do_cast( CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    void *vo;
    int mana;
    int sn;
    int target;

    target_name = one_argument( argument, arg1 );
    one_argument( target_name, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Cast which what where?\n\r", ch );
	return;
    }

    /*
     * Need to place restrictions on mobs in here.
     * For instance, right now charmed mobs can cast anything.
     */
    if ( ( sn = find_spell( ch, arg1 ) ) < 1
    ||  skill_table[sn].spell_fun == spell_null
    || ( !IS_NPC( ch ) && ( ch->level < skill_table[sn].skill_level[ch->class]
    ||   		 ch->pcdata->skill[sn].percent == 0 ) ) )
    {
	send_to_char( "You don't know any spells of that name.\n\r", ch );
	return;
    }

    if ( ch->position < skill_table[sn].minimum_position )
    {
	send_to_char( "You can't concentrate enough.\n\r", ch );
	return;
    }
/*
    if (ch->level + 2 == skill_table[sn].skill_level[ch->class])
	mana = 50;
    else
    	mana = UMAX(
	    skill_table[sn].min_mana,
	    100 / ( 2 + ch->level - skill_table[sn].skill_level[ch->class] ) );
*/
    mana = UMAX (skill_table[sn].min_mana, skill_table[sn].skill_level[ch->class]);
    if ( IS_NPC( ch ) && IS_SET( skill_table[sn].spell_flags, SPELL_NOMOB ) )
    {
        progbug( ch, "Mob in room #%d attempting to cast '%s'",
                 ch->in_room == NULL ? 0 : ch->in_room->vnum,
                 skill_table[sn].name );
        return;
    }

    if ( !IS_NPC( ch ) && IS_SET( skill_table[sn].spell_flags, SPELL_NOPC ) )
    {
        send_to_char( "You cannot cast that spell.\n\r", ch );
        return;
    }

    /*
     * Locate targets.
     */
    victim	= NULL;
    obj		= NULL;
    vo		= NULL;
    target	= TARGET_NONE;

    switch ( skill_table[sn].target )
    {
    default:
	bug( "Do_cast: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
	break;

    case TAR_CHAR_OFFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    if ( ( victim = ch->fighting ) == NULL )
	    {
		send_to_char( "Cast the spell on whom?\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( ( victim = get_char_room( ch, target_name ) ) == NULL )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}
/*
        if ( ch == victim )
        {
            send_to_char( "You can't do that to yourself.\n\r", ch );
            return;
        }
*/


	if ( !IS_NPC(ch) )
	{

            if (is_safe(ch,victim) && victim != ch)
	    {
		send_to_char("Not on that target.\n\r",ch);
		return;
	    }
	check_killer(ch,victim);
	}

        if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
	    send_to_char( "You can't do that on your own follower.\n\r",
		ch );
	    return;
	}

	vo = (void *) victim;
	target = TARGET_CHAR;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    victim = ch;
	}
	else
	{
	    if ( ( victim = get_char_room( ch, target_name ) ) == NULL )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}

	vo = (void *) victim;
	target = TARGET_CHAR;
	break;

    case TAR_CHAR_SELF:
	if ( arg2[0] != '\0' && !is_name( target_name, ch->name ) )
	{
	    send_to_char( "You cannot cast this spell on another.\n\r", ch );
	    return;
	}

	vo = (void *) ch;
	target = TARGET_CHAR;
	break;

    case TAR_OBJ_INV:
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "What should the spell be cast upon?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, target_name ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}

	vo = (void *) obj;
	target = TARGET_OBJ;
	break;

    case TAR_OBJ_CHAR_OFF:
	if (arg2[0] == '\0')
	{
	    if ( ( victim = ch->fighting ) == NULL )
	    {
		send_to_char("Cast the spell on whom or what?\n\r",ch);
		return;
	    }

	    target = TARGET_CHAR;
	}
	else if ((victim = get_char_room(ch,target_name)) != NULL)
	{
	    target = TARGET_CHAR;
	}

	if (target == TARGET_CHAR) /* check the sanity of the attack */
	{
	    if(is_safe_spell(ch,victim,FALSE) && victim != ch)
	    {
		send_to_char("Not on that target.\n\r",ch);
		return;
	    }

            if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
            {
                send_to_char( "You can't do that on your own follower.\n\r",
                    ch );
                return;
            }

	    if (!IS_NPC(ch))
		check_killer(ch,victim);

	    vo = (void *) victim;
 	}
	else if ((obj = get_obj_here(ch,target_name)) != NULL)
	{
	    vo = (void *) obj;
	    target = TARGET_OBJ;
	}
	else
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
	break;

    case TAR_OBJ_CHAR_DEF:
        if (arg2[0] == '\0')
        {
            vo = (void *) ch;
            target = TARGET_CHAR;
        }
        else if ((victim = get_char_room(ch,target_name)) != NULL)
        {
            vo = (void *) victim;
            target = TARGET_CHAR;
	}
	else if ((obj = get_obj_carry(ch,target_name,ch)) != NULL)
	{
	    vo = (void *) obj;
	    target = TARGET_OBJ;
	}
	else
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
	break;
    }

    if ( vo != NULL && target == TARGET_CHAR && IS_DEAD( (CHAR_DATA *)vo ) )
    {
        if ( str_cmp( skill_table[sn].name, "resurrect" ) )
        {
            send_to_char( "You cannot cast that on one who is a spirit.\n\r", ch );
            return;
        }
    }

    if ( !IS_NPC(ch) && ch->mana < mana )
    {
	send_to_char( "You don't have enough mana.\n\r", ch );
	return;
    }

    if ( str_cmp( skill_table[sn].name, "ventriloquate" ) )
	say_spell( ch, sn );

    if ( ( IS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC )
    ||	   ch->in_room->sector_type == SECT_OCEAN )
    && ( IS_NPC( ch ) || !IS_IMMORTAL( ch )
                || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    {
	send_to_char( "You failed.\n\r", ch );
	ch->mana -= mana / 2;
	return;
    }

    if ( !IS_IMMORTAL( ch ) )
	WAIT_STATE( ch, skill_table[sn].beats );

    if ( number_percent( ) > get_skill(ch,sn) )
    {
	send_to_char( "You lost your concentration.\n\r", ch );
	check_improve(ch,sn,FALSE,1);
	ch->mana -= mana / 2;
    }
    else
    {
        ch->mana -= mana;
        if ( IS_NPC( ch ) || IS_SET( class_table[ch->class].flags, CLASS_FMANA ) )
	/* class has spells */
            (*skill_table[sn].spell_fun) ( sn, ch->level, ch, vo,target);
        else
            (*skill_table[sn].spell_fun) (sn, 3 * ch->level/4, ch, vo,target);
        check_improve(ch,sn,TRUE,1);
    }

    if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
    ||   (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
    &&   victim != ch
    &&   victim->master != ch)
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;
	    if ( victim == vch && victim->fighting == NULL )
	    {	check_killer(victim,ch);
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}


/*
 * Cast spells at targets using a magical object.
 */
void
obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
    void *vo;
    int target = TARGET_NONE;

    if ( sn <= 0 )
	return;

    if ( !is_sn( sn ) || skill_table[sn].spell_fun == NULL )
    {
	bugf( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }

    if ( victim != NULL && IS_DEAD( victim ) )
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    if ( ch->in_room == NULL || IS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC ) )
    {
	send_to_char( "The magic of the item fizzles.\n\r", ch );
	return;
    }

    if ( ch->in_room->sector_type == SECT_OCEAN  )
    {
	send_to_char( "The water short circuits the spell.\n\r", ch );
	return;
    }

    if ( sn >= top_skill || skill_table[sn].spell_fun == 0 )
    {
	bug( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }

    switch ( skill_table[sn].target )
    {
    default:
	bug( "Obj_cast_spell: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
	vo = NULL;
	break;

    case TAR_CHAR_OFFENSIVE:
	if ( victim == NULL )
	    victim = ch->fighting;
	if ( victim == NULL )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	if (is_safe(ch,victim) && ch != victim)
	{
	    send_to_char("Something isn't right...\n\r",ch);
	    return;
	}
	vo = (void *) victim;
	target = TARGET_CHAR;
	break;

    case TAR_CHAR_DEFENSIVE:
    case TAR_CHAR_SELF:
	if ( victim == NULL )
	    victim = ch;
	vo = (void *) victim;
	target = TARGET_CHAR;
	break;

    case TAR_OBJ_INV:
	if ( obj == NULL )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	target = TARGET_OBJ;
	break;

    case TAR_OBJ_CHAR_OFF:
        if ( victim == NULL && obj == NULL)
        {
	    if (ch->fighting != NULL)
		victim = ch->fighting;
	    else
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }
	}

	    if (victim != NULL)
	    {
		if (is_safe_spell(ch,victim,FALSE) && ch != victim)
		{
		    send_to_char("Somehting isn't right...\n\r",ch);
		    return;
		}

		vo = (void *) victim;
		target = TARGET_CHAR;
	    }
	    else
	    {
	    	vo = (void *) obj;
	    	target = TARGET_OBJ;
	    }
        break;


    case TAR_OBJ_CHAR_DEF:
	if (victim == NULL && obj == NULL)
	{
	    vo = (void *) ch;
	    target = TARGET_CHAR;
	}
	else if (victim != NULL)
	{
	    vo = (void *) victim;
	    target = TARGET_CHAR;
	}
	else
	{
	    vo = (void *) obj;
	    target = TARGET_OBJ;
	}

	break;
    }

    target_name = "";
    (*skill_table[sn].spell_fun) ( sn, level, ch, vo,target);

    if ( (skill_table[sn].target == TAR_CHAR_OFFENSIVE
    ||   (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
    &&   victim != ch
    &&   victim->master != ch )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;
	    if ( victim == vch && victim->fighting == NULL )
	    {
		check_killer(victim,ch);
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}



/*
 * Spell functions.
 */
 
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


void
spell_animate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA      *obj = (OBJ_DATA *) vo;
   OBJ_DATA      *obj_next;


    if ( obj->item_type != ITEM_CORPSE_NPC )
    {
	send_to_char( "You cannot animate that.\n\r", ch );
	return;
    }
    obj_next = obj->next;
    if ( obj->deleted )
	return;
    magic_mob( ch, obj, obj->value[5] );
    extract_obj( obj );
    return;
}


void
spell_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already armored.\n\r",ch);
	else
	  act("$N is already armored.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 24;
    af.modifier  = -20;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel someone protecting you.\n\r", victim );
    if ( ch != victim )
	act("$N is protected by your magic.",ch,NULL,victim,TO_CHAR);
    return;
}



void
spell_bless( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *	victim;
    OBJ_DATA *	holy;
    int		water;
    OBJ_DATA *	obj;
    AFFECT_DATA af;

    water = liq_lookup( "water" );

    for ( holy = ch->carrying; holy != NULL; holy = holy->next_content )
	if ( holy->item_type == ITEM_DRINK_CON
	&&   IS_OBJ_STAT( holy, ITEM_BLESS )
	&&   holy->value[2] == water
	&&   holy->value[1] > 1 )
	    break;

    if ( !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
	if ( holy == NULL )
	{
	    send_to_char( "You have no holy water.\n\r", ch );
	    return;
	}

	holy->value[1] -= 2;

	if ( target == TARGET_OBJ )
	{
	    act_color( AT_ACTION, "You sprinkle some holy water on $P.",
		       ch, NULL, vo, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n sprinkles some holy water on $P.",
		       ch, NULL, vo, TO_ROOM, POS_RESTING );
	}
	else
	{
	    if ( ch == vo )
	    {
                act_color( AT_ACTION, "You sprinkle some holy water on yourself.",
                           ch, NULL, NULL, TO_CHAR, POS_RESTING );
                act_color( AT_ACTION, "$n sprinkles some holy water on $mself.",
                           ch, NULL, vo, TO_NOTVICT, POS_RESTING );
	    }
	    else
	    {
                act_color( AT_ACTION, "You sprinkle some holy water on $N.",
                           ch, NULL, vo, TO_CHAR, POS_RESTING );
                act_color( AT_ACTION, "$n sprinkles some holy water on $N.",
                           ch, NULL, vo, TO_NOTVICT, POS_RESTING );
                act_color( AT_ACTION, "$n sprinkles some holy water on you.",
                           ch, NULL, vo, TO_VICT, POS_RESTING );
            }
	}

	if ( holy->value[1] <= 0 )
	{
	    act_color( C_DEFAULT, "Your $p is empty.", ch, holy, NULL, TO_CHAR, POS_RESTING );
	}
    }

    /* deal with the object case first */
    if (target == TARGET_OBJ)
    {
	obj = (OBJ_DATA *) vo;
	if (IS_OBJ_STAT(obj,ITEM_BLESS))
	{
	    act("$p is already blessed.",ch,obj,NULL,TO_CHAR);
	    return;
	}

	if (IS_OBJ_STAT(obj,ITEM_EVIL))
	{
	    AFFECT_DATA *paf;

	    paf = affect_find(obj->affected,gsn_curse);
	    if (!saves_dispel(level,paf != NULL ? paf->level : obj->level,0))
	    {
		if (paf != NULL)
		    affect_remove_obj(obj,paf);
		act("$p glows a pale blue.",ch,obj,NULL,TO_ALL);
		REMOVE_BIT(obj->extra_flags,ITEM_EVIL);
		return;
	    }
	    else
	    {
		act("The evil of $p is too powerful for you to overcome.",
		    ch,obj,NULL,TO_CHAR);
		return;
	    }
	}

	af.where	= TO_OBJECT;
	af.type		= sn;
	af.level	= level;
	af.duration	= 6 + level;
	af.location	= APPLY_SAVES;
	af.modifier	= -1;
	af.bitvector	= ITEM_BLESS;
	affect_to_obj(obj,&af);

	act("$p glows with a holy aura.",ch,obj,NULL,TO_ALL);

	if (obj->wear_loc != WEAR_NONE)
	    ch->saving_throw -= 1;
	return;
    }

    /* character target */
    victim = (CHAR_DATA *) vo;


    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already blessed.\n\r",ch);
	else
	  act("$N already has divine favor.",ch,NULL,victim,TO_CHAR);
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
    affect_to_char( victim, &af );
    send_to_char( "You feel righteous.\n\r", victim );
    if ( ch != victim )
	act("You grant $N the favor of your god.",ch,NULL,victim,TO_CHAR);
    return;
}



void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_BLIND) || saves_spell(level,victim,DAM_OTHER))
	return;


    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -4;
    af.duration  = 1+level;
    af.bitvector = AFF_BLIND;
    affect_to_char( victim, &af );
    send_to_char( "You are blinded!\n\r", victim );
    act("$n appears to be blinded.",victim,NULL,NULL,TO_ROOM);
    return;
}



void spell_burning_hands(int sn,int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 0,  0,  0,  0,	14,	17, 20, 23, 26, 29,
	29, 29, 30, 30,	31,	31, 32, 32, 33, 33,
	34, 34, 35, 35,	36,	36, 37, 37, 38, 38,
	39, 39, 40, 40,	41,	41, 42, 42, 43, 43,
	44, 44, 45, 45,	46,	46, 47, 47, 48, 48
    };
    int dam;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim,DAM_FIRE) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_FIRE,TRUE);
    return;
}



void spell_call_lightning( int sn, int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You must be out of doors.\n\r", ch );
	return;
    }

    if ( weather_info.sky < SKY_RAINING )
    {
	send_to_char( "You need bad weather.\n\r", ch );
	return;
    }

    dam = dice(level/2, 8);

    send_to_char( "The lightning of the Kinsar strikes your foes!\n\r", ch );
    act( "$n calls the lightning of the Kinsar to strike $s foes!",
	ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) ) )
		damage( ch, vch, saves_spell( level,vch,DAM_LIGHTNING)
		? dam / 2 : dam, sn,DAM_LIGHTNING,TRUE);
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area
	&&   IS_OUTSIDE(vch)
	&&   IS_AWAKE(vch) )
	    send_to_char( "Lightning flashes in the sky.\n\r", vch );
    }

    return;
}

/* RT calm spell stops all fighting in the room */

void spell_calm( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *vch;
    int mlevel = 0;
    int count = 0;
    int high_level = 0;
    int chance;
    AFFECT_DATA af;

    /* get sum of all mobile levels in the room */
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->position == POS_FIGHTING)
	{
	    count++;
	    if (IS_NPC(vch))
	      mlevel += vch->level;
	    else
	      mlevel += vch->level/2;
	    high_level = UMAX(high_level,vch->level);
	}
    }

    /* compute chance of stopping combat */
    chance = 4 * level - high_level + 2 * count;

    if (IS_IMMORTAL(ch)) /* always works */
      mlevel = 0;

    if (number_range(0, chance) >= mlevel)  /* hard to stop large fights */
    {
	for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   	{
	    if (IS_NPC(vch) && (IS_SET(vch->imm_flags,IMM_MAGIC) ||
				IS_SET(vch->act,ACT_UNDEAD)))
	      return;

	    if (IS_AFFECTED(vch,AFF_CALM) || IS_AFFECTED(vch,AFF_BERSERK)
	    ||  is_affected(vch,skill_lookup("frenzy")))
	      return;

	    send_to_char("A wave of calm passes over you.\n\r",vch);

	    if (vch->fighting || vch->position == POS_FIGHTING)
	      stop_fighting(vch,FALSE);


	    af.where = TO_AFFECTS;
	    af.type = sn;
  	    af.level = level;
	    af.duration = level/4;
	    af.location = APPLY_HITROLL;
	    if (!IS_NPC(vch))
	      af.modifier = -5;
	    else
	      af.modifier = -2;
	    af.bitvector = AFF_CALM;
	    affect_to_char(vch,&af);

	    af.location = APPLY_DAMROLL;
	    affect_to_char(vch,&af);
	}
    }
}

void spell_cancellation( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    bool found = FALSE;

    level += 2;

    //if ((!IS_NPC(ch) && IS_NPC(victim) &&
//	 !(IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) ) ||
        //(IS_NPC(ch) && !IS_NPC(victim) && !IS_SET(ch, ACT_IS_HEALER)) ))
  //      (IS_NPC(ch) && !IS_NPC (victim)))
     //if ((!IS_SET(ch, ACT_IS_HEALER)))
    //{
//	send_to_char("You failed, try dispel magic.\n\r",ch);
//	return;
  //  }

    /* unlike dispel magic, the victim gets NO save */

if ((!IS_NPC (ch) && IS_NPC (victim) &&
                 !(IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)) ||
                (IS_NPC (ch) && !IS_NPC (victim) && !IS_SET(ch->act, ACT_IS_HEALER))) {
                send_to_char ("You failed, try dispel magic.\n\r", ch);
                return;
        }

    /* begin running through the spells */

    if ( check_dispel( level, victim, gsn_aura_peace ) )
	found = TRUE;

    if ( check_dispel( level, victim, skill_lookup( "armor" ) ) )
        found = TRUE;

         if (check_dispel(level,victim,skill_lookup("black mantle")))
    {
        act("The dark aura around $n's body vanishes.",
            victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if ( check_dispel( level, victim, skill_lookup( "bless" ) ) )
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("blindness")))
    {
        found = TRUE;
        act("$n is no longer blinded.",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("calm")))
    {
	found = TRUE;
	act("$n no longer looks so peaceful...",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("change sex")))
    {
        found = TRUE;
        act("$n looks more like $mself again.",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("charm person")))
    {
        found = TRUE;
        act("$n regains $s free will.",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("chill touch")))
    {
        found = TRUE;
        act("$n looks warmer.",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("curse")))
        found = TRUE;

    if ( check_dispel( level, victim, skill_lookup( "deafen" ) ) )
    {
	found = TRUE;
	act( "$n can hear again.", victim, NULL, NULL, TO_ROOM );
    }

    if (check_dispel(level,victim,skill_lookup("detect evil")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect good")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect hidden")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect invis")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect magic")))
        found = TRUE;

    if ( check_dispel( level, victim, gsn_enlarge ) )
	found = TRUE;

    if (check_dispel(level,victim,skill_lookup("faerie fire")))
    {
        act("$n's outline fades.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("fly")))
    {
        act("$n falls to the ground!",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
        check_nofloor( victim );
    }

    if (check_dispel(level,victim,skill_lookup("frenzy")))
    {
	act("$n no longer looks so wild.",victim,NULL,NULL,TO_ROOM);;
	found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("giant strength")))
    {
        act("$n no longer looks so mighty.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("haste")))
    {
	act("$n is no longer moving so quickly.",victim,NULL,NULL,TO_ROOM);
	found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("infravision")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("invis")))
    {
        act("$n fades into existance.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("mass invis")))
    {
        act("$n fades into existance.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("pass door")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("protection evil")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("protection good")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("sanctuary")))
    {
        act("The white aura around $n's body vanishes.",
            victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("shield")))
    {
        act("The shield protecting $n vanishes.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if ( check_dispel( level, victim, gsn_shrink ) )
	found = TRUE;

    if (check_dispel(level,victim,skill_lookup("sleep")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("slow")))
    {
        act("$n is no longer moving so slowly.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("stone skin")))
    {
        act("$n's skin regains its normal texture.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if ( check_dispel( level, victim, skill_lookup( "wail of the banshee") ) )
    {
	found = TRUE;
	act( "$n can hear again.", victim, NULL, NULL, TO_ROOM );
    }

    if (check_dispel(level,victim,skill_lookup("weaken")))
    {
        act("$n looks stronger.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("channel")))
    {
        found = TRUE;
        act("$n no longer has good grip on the magix.",victim,NULL,NULL,TO_ROOM);
    }

    if (found)
        send_to_char("Ok.\n\r",ch);
    else
        send_to_char("Spell failed.\n\r",ch);
}

void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    damage( ch, (CHAR_DATA *) vo, dice(1, 8) + level / 3, sn,DAM_HARM,TRUE);
    return;
}



void spell_cause_critical(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    damage( ch, (CHAR_DATA *) vo, dice(3, 8) + level - 6, sn,DAM_HARM,TRUE);
    return;
}



void spell_cause_serious(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    damage( ch, (CHAR_DATA *) vo, dice(2, 8) + level / 2, sn,DAM_HARM,TRUE);
    return;
}

void spell_chain_lightning(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *tmp_vict,*last_vict,*next_vict;
    bool found;
    int dam;

    /* first strike */

    act("A lightning bolt leaps from $n's hand and arcs to $N.",
        ch,NULL,victim,TO_ROOM);
    act("A lightning bolt leaps from your hand and arcs to $N.",
	ch,NULL,victim,TO_CHAR);
    act("A lightning bolt leaps from $n's hand and hits you!",
	ch,NULL,victim,TO_VICT);

    dam = dice(level,6);
    if (saves_spell(level,victim,DAM_LIGHTNING))
 	dam /= 3;
    damage(ch,victim,dam,sn,DAM_LIGHTNING,TRUE);
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
	    act("The bolt arcs to $n!",tmp_vict,NULL,NULL,TO_ROOM);
	    act("The bolt hits you!",tmp_vict,NULL,NULL,TO_CHAR);
	    dam = dice(level,6);
	    if (saves_spell(level,tmp_vict,DAM_LIGHTNING))
		dam /= 3;
	    damage(ch,tmp_vict,dam,sn,DAM_LIGHTNING,TRUE);
	    level -= 4;  /* decrement damage */
	  }
	}   /* end target searching loop */

	if (!found) /* no target found, hit the caster */
	{
	  if (ch == NULL)
     	    return;

	  if (last_vict == ch) /* no double hits */
	  {
	    act("The bolt seems to have fizzled out.",ch,NULL,NULL,TO_ROOM);
	    act("The bolt grounds out through your body.",
		ch,NULL,NULL,TO_CHAR);
	    return;
	  }

	  last_vict = ch;
	  act("The bolt arcs to $n...whoops!",ch,NULL,NULL,TO_ROOM);
	  send_to_char("You are struck by your own lightning!\n\r",ch);
	  dam = dice(level,6);
	  if (saves_spell(level,ch,DAM_LIGHTNING))
	    dam /= 3;
	  damage(ch,ch,dam,sn,DAM_LIGHTNING,TRUE);
	  level -= 4;  /* decrement damage */
	  if (ch == NULL)
	    return;
	}
    /* now go back and find more targets */
    }
}


void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ))
    {
	if (victim == ch)
	  send_to_char("You've already been changed.\n\r",ch);
	else
	  act("$N has already had $s(?) sex changed.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (saves_spell(level , victim,DAM_OTHER))
	return;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 2 * level;
    af.location  = APPLY_SEX;
    do
    {
	af.modifier  = number_range( 0, 2 ) - victim->sex;
    }
    while ( af.modifier == 0 );
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel different.\n\r", victim );
    act("$n doesn't look like $mself anymore...",victim,NULL,NULL,TO_ROOM);
    return;
}



void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_safe(ch,victim)) return;

    if ( victim == ch )
    {
	send_to_char( "You like yourself even better!\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_CHARM)
    ||   IS_AFFECTED(ch, AFF_CHARM)
    ||   level < victim->level
    ||   IS_SET(victim->imm_flags,IMM_CHARM)
    ||   saves_spell( level, victim,DAM_CHARM) )
	return;


    if (IS_SET(victim->in_room->room_flags,ROOM_LAW))
    {
	send_to_char(
	    "The mayor does not allow charming in the city limits.\n\r",ch);
	return;
    }

    if ( victim->master )
	stop_follower( victim );
    add_follower( victim, ch );
    victim->leader = ch;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    act( "Isn't $n just so nice?", ch, NULL, victim, TO_VICT );
    if ( ch != victim )
	act("$N looks at you with adoring eyes.",ch,NULL,victim,TO_CHAR);
    return;
}



void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 0,  0,  6,  7,  8,	 9, 12, 13, 13, 13,
	14, 14, 14, 15, 15,	15, 16, 16, 16, 17,
	17, 17, 18, 18, 18,	19, 19, 19, 20, 20,
	20, 21, 21, 21, 22,	22, 22, 23, 23, 23,
	24, 24, 24, 25, 25,	25, 26, 26, 26, 27
    };
    AFFECT_DATA af;
    int dam;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( !saves_spell( level, victim,DAM_COLD ) )
    {
	act("$n turns blue and shivers.",victim,NULL,NULL,TO_ROOM);
	af.where     = TO_AFFECTS;
	af.type      = sn;
        af.level     = level;
	af.duration  = 6;
	af.location  = APPLY_STR;
	af.modifier  = -1;
	af.bitvector = 0;
	affect_join( victim, &af );
    }
    else
    {
	dam /= 2;
    }

    damage( ch, victim, dam, sn, DAM_COLD,TRUE );
    return;
}



void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0,  0,  0,
	30, 35, 40, 45, 50,	55, 55, 55, 56, 57,
	58, 58, 59, 60, 61,	61, 62, 63, 64, 64,
	65, 66, 67, 67, 68,	69, 70, 70, 71, 72,
	73, 73, 74, 75, 76,	76, 77, 78, 79, 79
    };
    int dam;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2,  dam_each[level] * 2 );
    if ( saves_spell( level, victim,DAM_LIGHT) )
	dam /= 2;
    else
	spell_blindness(skill_lookup("blindness"),
	    level/2,ch,(void *) victim,TARGET_CHAR);

    damage( ch, victim, dam, sn, DAM_LIGHT,TRUE );
    return;
}



void spell_continual_light(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    OBJ_DATA *light;

    if (target_name[0] != '\0')  /* do a glow on some object */
    {
	light = get_obj_carry(ch,target_name,ch);

	if (light == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}

	if (IS_OBJ_STAT(light,ITEM_GLOW))
	{
	    act("$p is already glowing.",ch,light,NULL,TO_CHAR);
	    return;
	}

	SET_BIT(light->extra_flags,ITEM_GLOW);
	act("$p glows with a white light.",ch,light,NULL,TO_ALL);
	return;
    }

    light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), 0 );
    obj_to_room( light, ch->in_room );
    add_obj_fall_event( light );
    act( "$n twiddles $s thumbs and $p appears.",   ch, light, NULL, TO_ROOM );
    act( "You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR );
    return;
}



void spell_control_weather(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    if ( !str_cmp( target_name, "better" ) )
	weather_info.change += dice( level / 3, 4 );
    else if ( !str_cmp( target_name, "worse" ) )
	weather_info.change -= dice( level / 3, 4 );
    else
	send_to_char ("Do you want it to get better or worse?\n\r", ch );

    send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *mushroom;

    mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
    mushroom->value[0] = level / 2;
    mushroom->value[1] = level;
    obj_to_room( mushroom, ch->in_room );
    add_obj_fall_event( mushroom );
    act( "$p suddenly appears.", ch, mushroom, NULL, TO_ROOM );
    act( "$p suddenly appears.", ch, mushroom, NULL, TO_CHAR );
    return;
}

void spell_create_rose( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    OBJ_DATA *rose;
    rose = create_object(get_obj_index(OBJ_VNUM_ROSE), 0);
    act("$n has created a beautiful red rose.",ch,rose,NULL,TO_ROOM);
    send_to_char("You create a beautiful red rose.\n\r",ch);
    obj_to_char(rose,ch);
    return;
}

void spell_create_spring(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    OBJ_DATA *spring;

    spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0 );
    spring->timer = level;
    obj_to_room( spring, ch->in_room );
    add_obj_fall_event( spring );
    act( "$p flows from the ground.", ch, spring, NULL, TO_ROOM );
    act( "$p flows from the ground.", ch, spring, NULL, TO_CHAR );
    return;
}


void
spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int water;

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "It is unable to hold water.\n\r", ch );
	return;
    }

    if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
    {
	send_to_char( "It contains some other liquid.\n\r", ch );
	return;
    }

    water = UMIN(
		level * (weather_info.sky >= SKY_RAINING ? 4 : 2),
		obj->value[0] - obj->value[1]
		);

    if ( water > 0 )
    {
	obj->value[2] = LIQ_WATER;
	obj->value[1] += water;
	if ( !is_name( "water", obj->name ) )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "%s water", obj->name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
	act( "$p is filled.", ch, obj, NULL, TO_CHAR );
    }

    return;
}


void
spell_cure_blindness(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_blindness ) )
    {
        if (victim == ch)
          send_to_char("You aren't blind.\n\r",ch);
        else
          act("$N doesn't appear to be blinded.",ch,NULL,victim,TO_CHAR);
        return;
    }

    if (check_dispel(level,victim,gsn_blindness))
    {
        send_to_char( "Your vision returns!\n\r", victim );
        act("$n is no longer blinded.",victim,NULL,NULL,TO_ROOM);
    }
    else
        send_to_char("Spell failed.\n\r",ch);
}


void
spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

//    heal = dice(3, 8) + level - 6;
    if ( heal_spell( victim, dice( 3, 4 + level / 10 ), 3, level ) )
	send_to_char( "You feel better!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

/* RT added to cure plague */
void spell_cure_disease( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_plague ) )
    {
        if (victim == ch)
          send_to_char("You aren't ill.\n\r",ch);
        else
          act("$N doesn't appear to be diseased.",ch,NULL,victim,TO_CHAR);
        return;
    }

    if (check_dispel(level,victim,gsn_plague))
    {
	send_to_char("Your sores vanish.\n\r",victim);
	act("$n looks relieved as $s sores vanish.",victim,NULL,NULL,TO_ROOM);
    }
    else
	send_to_char("Spell failed.\n\r",ch);
}


void
spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

//    heal = dice(1, 8) + level / 3;
    update_pos( victim );
    if ( heal_spell( victim, number_range( 1, 4 + level / 10 ), 1, level ) )
	send_to_char( "You feel better!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}


void
spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_poison ) )
    {
        if (victim == ch)
          send_to_char("You aren't poisoned.\n\r",ch);
        else
          act("$N doesn't appear to be poisoned.",ch,NULL,victim,TO_CHAR);
        return;
    }

    if (check_dispel(level,victim,gsn_poison))
    {
        send_to_char("A warm feeling runs through your body.\n\r",victim);
        act("$n looks much better.",victim,NULL,NULL,TO_ROOM);
    }
    else
        send_to_char("Spell failed.\n\r",ch);
}


void
spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

//    heal = dice(2, 8) + level /2 ;
    if ( heal_spell( victim, dice( 2, 4 + level / 5 ), 2, level ) )
	send_to_char( "You feel better!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    AFFECT_DATA af;

    /* deal with the object case first */
    if (target == TARGET_OBJ)
    {
        obj = (OBJ_DATA *) vo;
        if (IS_OBJ_STAT(obj,ITEM_EVIL))
        {
            act("$p is already filled with evil.",ch,obj,NULL,TO_CHAR);
            return;
        }

        if (IS_OBJ_STAT(obj,ITEM_BLESS))
        {
            AFFECT_DATA *paf;

            paf = affect_find(obj->affected,skill_lookup("bless"));
            if (!saves_dispel(level,paf != NULL ? paf->level : obj->level,0))
            {
                if (paf != NULL)
                    affect_remove_obj(obj,paf);
                act("$p glows with a red aura.",ch,obj,NULL,TO_ALL);
                REMOVE_BIT(obj->extra_flags,ITEM_BLESS);
                return;
            }
            else
            {
                act("The holy aura of $p is too powerful for you to overcome.",
                    ch,obj,NULL,TO_CHAR);
                return;
            }
        }

        af.where        = TO_OBJECT;
        af.type         = sn;
        af.level        = level;
        af.duration     = 2 * level;
        af.location     = APPLY_SAVES;
        af.modifier     = +1;
        af.bitvector    = ITEM_EVIL;
        affect_to_obj(obj,&af);

        act("$p glows with a malevolent aura.",ch,obj,NULL,TO_ALL);

	if (obj->wear_loc != WEAR_NONE)
	    ch->saving_throw += 1;
        return;
    }

    /* character curses */
    victim = (CHAR_DATA *) vo;

    if (IS_AFFECTED(victim,AFF_CURSE) || saves_spell(level,victim,DAM_NEGATIVE))
	return;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 2*level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1 * (level / 8);
    af.bitvector = AFF_CURSE;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = level / 8;
    affect_to_char( victim, &af );

    send_to_char( "You feel unclean.\n\r", victim );
    if ( ch != victim )
	act("$N looks very uncomfortable.",ch,NULL,victim,TO_CHAR);
    return;
}

/* RT replacement demonfire spell */

void spell_demonfire(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if ( !IS_NPC(ch) && !IS_EVIL(ch) )
    {
        victim = ch;
	send_to_char("The demons turn upon you!\n\r",ch);
    }

    ch->alignment = UMAX(-1000, ch->alignment - 50);

    if (victim != ch)
    {
	act("$n calls forth the demons of Hell upon $N!",
	    ch,NULL,victim,TO_ROOM);
        act("$n has assailed you with the demons of Hell!",
	    ch,NULL,victim,TO_VICT);
	send_to_char("You conjure forth the demons of hell!\n\r",ch);
    }
    dam = dice( level, 10 );
    if ( saves_spell( level, victim,DAM_NEGATIVE) )
        dam /= 2;
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
    spell_curse(gsn_curse, 3 * level / 4, ch, (void *) victim,TARGET_CHAR);
}

void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_EVIL) )
    {
	if (victim == ch)
	  send_to_char("You can already sense evil.\n\r",ch);
	else
	  act("$N can already detect evil.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_EVIL;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}


void spell_detect_good( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_GOOD) )
    {
        if (victim == ch)
          send_to_char("You can already sense good.\n\r",ch);
        else
          act("$N can already detect good.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_GOOD;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_hidden(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_HIDDEN) )
    {
        if (victim == ch)
          send_to_char("You are already as alert as you can be. \n\r",ch);
        else
          act("$N can already sense hidden lifeforms.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );
    send_to_char( "Your awareness improves.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_INVIS) )
    {
        if (victim == ch)
          send_to_char("You can already see invisible.\n\r",ch);
        else
          act("$N can already see invisible things.",ch,NULL,victim,TO_CHAR);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_MAGIC) )
    {
        if (victim == ch)
          send_to_char("You can already sense magical auras.\n\r",ch);
        else
          act("$N can already detect magic.",ch,NULL,victim,TO_CHAR);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
    {
	if ( obj->value[3] != 0 )
	    send_to_char( "You smell poisonous fumes.\n\r", ch );
	else
	    send_to_char( "It looks delicious.\n\r", ch );
    }
    else
    {
	send_to_char( "It doesn't look poisoned.\n\r", ch );
    }

    return;
}



void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if ( !IS_NPC(ch) && IS_EVIL(ch) )
	victim = ch;

    if ( IS_GOOD(victim) )
    {
	act( "The Kinsar protect $N.", ch, NULL, victim, TO_ROOM );
	return;
    }

    if ( IS_NEUTRAL(victim) )
    {
	act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (victim->hit > (ch->level * 4))
      dam = dice( level, 4 );
    else
      dam = UMAX(victim->hit, dice(level,4));
    if ( saves_spell( level, victim,DAM_HOLY) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_HOLY ,TRUE);
    return;
}


void spell_dispel_good( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if ( !IS_NPC(ch) && IS_GOOD(ch) )
        victim = ch;

    if ( IS_EVIL(victim) )
    {
        act( "$N is protected by $S evil.", ch, NULL, victim, TO_ROOM );
        return;
    }

    if ( IS_NEUTRAL(victim) )
    {
        act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if (victim->hit > (ch->level * 4))
      dam = dice( level, 4 );
    else
      dam = UMAX(victim->hit, dice(level,4));
    if ( saves_spell( level, victim,DAM_NEGATIVE) )
        dam /= 2;
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
    return;
}


/* modified for enhanced use */

void
spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    bool found = FALSE;

    if (saves_spell(level, victim,DAM_OTHER))
    {
	send_to_char( "You feel a brief tingling sensation.\n\r",victim);
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    /* begin running through the spells */

    if (check_dispel(level,victim,skill_lookup("armor")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("bless")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("blindness")))
    {
        found = TRUE;
        act("$n is no longer blinded.",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("calm")))
    {
        found = TRUE;
        act("$n no longer looks so peaceful...",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("change sex")))
    {
        found = TRUE;
        act("$n looks more like $mself again.",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("charm person")))
    {
        found = TRUE;
        act("$n regains $s free will.",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("chill touch")))
    {
        found = TRUE;
        act("$n looks warmer.",victim,NULL,NULL,TO_ROOM);
    }

    if (check_dispel(level,victim,skill_lookup("curse")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect evil")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect good")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect hidden")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect invis")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("detect magic")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("faerie fire")))
    {
        act("$n's outline fades.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("fly")))
    {
        act("$n falls to the ground!",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("frenzy")))
    {
        act("$n no longer looks so wild.",victim,NULL,NULL,TO_ROOM);;
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("giant strength")))
    {
        act("$n no longer looks so mighty.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("haste")))
    {
        act("$n is no longer moving so quickly.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("infravision")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("invis")))
    {
        act("$n fades into existance.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("mass invis")))
    {
        act("$n fades into existance.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("pass door")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("protection evil")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("protection good")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("sanctuary")))
    {
        act("The white aura around $n's body vanishes.",
            victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if ( IS_SHIELDED( victim, SHLD_SANCTUARY )
	&& !saves_dispel( level, victim->level, -1 )
	&& !is_affected( victim,skill_lookup( "sanctuary" ) ) )
    {
	xREMOVE_BIT( victim->shielded_by, SHLD_SANCTUARY );
        act( "The white aura around $n's body vanishes.",
             victim, NULL,NULL, TO_ROOM );
        found = TRUE;
    }
     if (check_dispel(level,victim,skill_lookup("black mantle")))
    {
        act("The dark aura around $n's body vanishes.",
            victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if ( IS_SHIELDED( victim, SHLD_BLACK_MANTLE )
	&& !saves_dispel( level, victim->level, -1 )
	&& !is_affected( victim,skill_lookup( "black mantle" ) ) )
    {
	xREMOVE_BIT( victim->shielded_by, SHLD_BLACK_MANTLE );
        act( "The dark aura around $n's body vanishes.",
             victim, NULL,NULL, TO_ROOM );
        found = TRUE;
    }


    if (check_dispel(level,victim,skill_lookup("shield")))
    {
        act("The shield protecting $n vanishes.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("sleep")))
        found = TRUE;

    if (check_dispel(level,victim,skill_lookup("slow")))
    {
        act("$n is no longer moving so slowly.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("stone skin")))
    {
        act("$n's skin regains its normal texture.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (check_dispel(level,victim,skill_lookup("weaken")))
    {
        act("$n looks stronger.",victim,NULL,NULL,TO_ROOM);
        found = TRUE;
    }

    if (found)
        send_to_char("Ok.\n\r",ch);
    else
        send_to_char("Spell failed.\n\r",ch);
	return;
}


void
spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    send_to_char( "The earth trembles beneath your feet!\n\r", ch );
    act( "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && !is_safe_spell(ch,vch,TRUE))
	    {
		if (IS_AFFECTED(vch,AFF_FLYING))
		    damage(ch,vch,0,sn,DAM_BASH,TRUE);
		else
		    damage( ch,vch,level + dice(2, 8), sn, DAM_BASH,TRUE);
	    }
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "The earth trembles and shivers.\n\r", vch );
    }

    return;
}

void spell_enchant_armor( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    int result, fail;
    int ac_bonus, added;
    bool ac_found = FALSE;

    if (obj->item_type != ITEM_ARMOR)
    {
	send_to_char("That isn't an armor.\n\r",ch);
	return;
    }

    if (obj->wear_loc != -1)
    {
	send_to_char("The item must be carried to be enchanted.\n\r",ch);
	return;
    }

    /* this means they have no bonus */
    ac_bonus = 0;
    fail = 25;	/* base 25% chance of failure */

    /* find the bonuses */

    if (!obj->enchanted)
    	for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    	{
            if ( paf->location == APPLY_AC )
            {
	    	ac_bonus = paf->modifier;
		ac_found = TRUE;
	    	fail += 5 * (ac_bonus * ac_bonus);
 	    }

	    else  /* things get a little harder */
	    	fail += 20;
    	}

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location == APPLY_AC )
  	{
	    ac_bonus = paf->modifier;
	    ac_found = TRUE;
	    fail += 5 * (ac_bonus * ac_bonus);
	}

	else /* things get a little harder */
	    fail += 20;
    }

    /* apply other modifiers */
    fail -= level;

    if (IS_OBJ_STAT(obj,ITEM_BLESS))
	fail -= 15;
    if (IS_OBJ_STAT(obj,ITEM_GLOW))
	fail -= 5;

    fail = URANGE(5,fail,85);

    result = number_percent();

    /* the moment of truth */
    if (result < (fail / 5))  /* item destroyed */
    {
	act("$p flares blindingly... and evaporates!",ch,obj,NULL,TO_CHAR);
	act("$p flares blindingly... and evaporates!",ch,obj,NULL,TO_ROOM);
	extract_obj(obj);
	return;
    }

    if (result < (fail / 3)) /* item disenchanted */
    {
	AFFECT_DATA *paf_next;

	act("$p glows brightly, then fades...oops.",ch,obj,NULL,TO_CHAR);
	act("$p glows brightly, then fades.",ch,obj,NULL,TO_ROOM);
	obj->enchanted = TRUE;

	/* remove all affects */
	for (paf = obj->affected; paf != NULL; paf = paf_next)
	{
	    paf_next = paf->next;
	    free_affect(paf);
	}
	obj->affected = NULL;

	/* clear all flags */
	obj->extra_flags = 0;
	return;
    }

    if ( result <= fail )  /* failed, no bad result */
    {
	send_to_char("Nothing seemed to happen.\n\r",ch);
	return;
    }

    /* okay, move all the old flags into new vectors if we have to */
    if (!obj->enchanted)
    {
	AFFECT_DATA *af_new;
	obj->enchanted = TRUE;

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
	{
	    af_new = new_affect();

	    af_new->next = obj->affected;
	    obj->affected = af_new;

	    af_new->where	= paf->where;
	    af_new->type 	= UMAX(0,paf->type);
//	    af_new->level	= paf->level;
	    af_new->duration	= paf->duration;
	    af_new->location	= paf->location;
	    af_new->modifier	= paf->modifier;
	    af_new->bitvector	= paf->bitvector;
	}
    }

    if (result <= (90 - level/5))  /* success! */
    {
	act("$p shimmers with a gold aura.",ch,obj,NULL,TO_CHAR);
	act("$p shimmers with a gold aura.",ch,obj,NULL,TO_ROOM);
	SET_BIT(obj->extra_flags, ITEM_MAGIC);
	added = -1;
    }

    else  /* exceptional enchant */
    {
	act("$p glows a brillant gold!",ch,obj,NULL,TO_CHAR);
	act("$p glows a brillant gold!",ch,obj,NULL,TO_ROOM);
	SET_BIT(obj->extra_flags,ITEM_MAGIC);
	SET_BIT(obj->extra_flags,ITEM_GLOW);
	added = -2;
    }

    /* now add the enchantments */

//    if (obj->level < LEVEL_HERO)
//	obj->level = UMIN(LEVEL_HERO - 1,obj->level + 1);

    if (ac_found)
    {
	for ( paf = obj->affected; paf != NULL; paf = paf->next)
	{
	    if ( paf->location == APPLY_AC)
	    {
		paf->type = sn;
		paf->modifier += added;
//		paf->level = UMAX(paf->level,level);
	    }
	}
    }
    else /* add a new affect */
    {
 	paf = new_affect();

	paf->where	= TO_OBJECT;
	paf->type	= sn;
	paf->level	= level;
	paf->duration	= -1;
	paf->location	= APPLY_AC;
	paf->modifier	=  added;
	paf->bitvector  = 0;
    	paf->next	= obj->affected;
    	obj->affected	= paf;
    }

}




void spell_enchant_weapon(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    int result, fail;
    int hit_bonus, dam_bonus, added;
    bool hit_found = FALSE, dam_found = FALSE;

    if (obj->item_type != ITEM_WEAPON)
    {
	send_to_char("That isn't a weapon.\n\r",ch);
	return;
    }

    if (obj->wear_loc != -1)
    {
	send_to_char("The item must be carried to be enchanted.\n\r",ch);
	return;
    }

    /* this means they have no bonus */
    hit_bonus = 0;
    dam_bonus = 0;
    fail = 25;	/* base 25% chance of failure */

    /* find the bonuses */

    if (!obj->enchanted)
    	for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    	{
            if ( paf->location == APPLY_HITROLL )
            {
	    	hit_bonus = paf->modifier;
		hit_found = TRUE;
	    	fail += 2 * (hit_bonus * hit_bonus);
 	    }

	    else if (paf->location == APPLY_DAMROLL )
	    {
	    	dam_bonus = paf->modifier;
		dam_found = TRUE;
	    	fail += 2 * (dam_bonus * dam_bonus);
	    }

	    else  /* things get a little harder */
	    	fail += 25;
    	}

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location == APPLY_HITROLL )
  	{
	    hit_bonus = paf->modifier;
	    hit_found = TRUE;
	    fail += 2 * (hit_bonus * hit_bonus);
	}

	else if (paf->location == APPLY_DAMROLL )
  	{
	    dam_bonus = paf->modifier;
	    dam_found = TRUE;
	    fail += 2 * (dam_bonus * dam_bonus);
	}

	else /* things get a little harder */
	    fail += 25;
    }

    /* apply other modifiers */
    fail -= 3 * level/2;

    if (IS_OBJ_STAT(obj,ITEM_BLESS))
	fail -= 15;
    if (IS_OBJ_STAT(obj,ITEM_GLOW))
	fail -= 5;

    fail = URANGE(5,fail,95);

    result = number_percent();

    /* the moment of truth */
    if (result < (fail / 5))  /* item destroyed */
    {
	act("$p shivers violently and explodes!",ch,obj,NULL,TO_CHAR);
	act("$p shivers violently and explodeds!",ch,obj,NULL,TO_ROOM);
	extract_obj(obj);
	return;
    }

    if (result < (fail / 2)) /* item disenchanted */
    {
	AFFECT_DATA *paf_next;

	act("$p glows brightly, then fades...oops.",ch,obj,NULL,TO_CHAR);
	act("$p glows brightly, then fades.",ch,obj,NULL,TO_ROOM);
	obj->enchanted = TRUE;

	/* remove all affects */
	for (paf = obj->affected; paf != NULL; paf = paf_next)
	{
	    paf_next = paf->next;
	    free_affect(paf);
	}
	obj->affected = NULL;

	/* clear all flags */
	obj->extra_flags = 0;
	return;
    }

    if ( result <= fail )  /* failed, no bad result */
    {
	send_to_char("Nothing seemed to happen.\n\r",ch);
	return;
    }

    /* okay, move all the old flags into new vectors if we have to */
    if (!obj->enchanted)
    {
	AFFECT_DATA *af_new;
	obj->enchanted = TRUE;

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
	{
	    af_new = new_affect();

	    af_new->next = obj->affected;
	    obj->affected = af_new;

	    af_new->where	= paf->where;
	    af_new->type 	= UMAX(0,paf->type);
//	    af_new->level	= paf->level;
	    af_new->duration	= paf->duration;
	    af_new->location	= paf->location;
	    af_new->modifier	= paf->modifier;
	    af_new->bitvector	= paf->bitvector;
	}
    }

    if (result <= (100 - level/5))  /* success! */
    {
	act("$p glows blue.",ch,obj,NULL,TO_CHAR);
	act("$p glows blue.",ch,obj,NULL,TO_ROOM);
	SET_BIT(obj->extra_flags, ITEM_MAGIC);
	added = 1;
    }

    else  /* exceptional enchant */
    {
	act("$p glows a brillant blue!",ch,obj,NULL,TO_CHAR);
	act("$p glows a brillant blue!",ch,obj,NULL,TO_ROOM);
	SET_BIT(obj->extra_flags,ITEM_MAGIC);
	SET_BIT(obj->extra_flags,ITEM_GLOW);
	added = 2;
    }

    /* now add the enchantments */

//    if (obj->level < LEVEL_HERO - 1)
//	obj->level = UMIN(LEVEL_HERO - 1,obj->level + 1);

    if (dam_found)
    {
	for ( paf = obj->affected; paf != NULL; paf = paf->next)
	{
	    if ( paf->location == APPLY_DAMROLL)
	    {
		paf->type = sn;
		paf->modifier += added;
//		paf->level = UMAX(paf->level,level);
		if (paf->modifier > 4)
		    SET_BIT(obj->extra_flags,ITEM_HUM);
	    }
	}
    }
    else /* add a new affect */
    {
	paf = new_affect();

	paf->where	= TO_OBJECT;
	paf->type	= sn;
	paf->level	= level;
	paf->duration	= -1;
	paf->location	= APPLY_DAMROLL;
	paf->modifier	=  added;
	paf->bitvector  = 0;
    	paf->next	= obj->affected;
    	obj->affected	= paf;
    }

    if (hit_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
	{
            if ( paf->location == APPLY_HITROLL)
            {
		paf->type = sn;
                paf->modifier += added;
           //     paf->level = UMAX(paf->level,level);
                if (paf->modifier > 4)
                    SET_BIT(obj->extra_flags,ITEM_HUM);
            }
	}
    }
    else /* add a new affect */
    {
        paf = new_affect();

        paf->type       = sn;
        paf->level      = level;
        paf->duration   = -1;
        paf->location   = APPLY_HITROLL;
        paf->modifier   =  added;
        paf->bitvector  = 0;
        paf->next       = obj->affected;
        obj->affected   = paf;
    }

}



/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 */
void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if (victim != ch)
        ch->alignment = UMAX(-1000, ch->alignment - 50);

    if ( saves_spell( level, victim,DAM_NEGATIVE) )
    {
	send_to_char("You feel a momentary chill.\n\r",victim);
	return;
    }


    if ( victim->level <= 2 )
    {
	dam		 = ch->hit + 1;
    }
    else
    {
	gain_exp( victim, 0 - number_range( level/2, 3 * level / 2 ) );
	victim->mana	/= 2;
	victim->move	/= 2;
	dam		 = dice(1, level);
	ch->hit		+= dam;
    }

    send_to_char("You feel your life slipping away!\n\r",victim);
    send_to_char("Wow....what a rush!\n\r",ch);
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);

    return;
}


/*
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


void spell_fireproof(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA af;

    if (IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
    {
        act("$p is already protected from burning.",ch,obj,NULL,TO_CHAR);
        return;
    }

    af.where     = TO_OBJECT;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy(level / 4);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = ITEM_BURN_PROOF;

    affect_to_obj(obj,&af);

    act("You protect $p from fire.",ch,obj,NULL,TO_CHAR);
    act("$p is surrounded by a protective aura.",ch,obj,NULL,TO_ROOM);
}


void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice(6 + level / 2, 8);
    if ( saves_spell( level, victim,DAM_FIRE) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_FIRE ,TRUE);
    return;
}


void
spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
	return;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = 2 * level;
    af.bitvector = AFF_FAERIE_FIRE;
    affect_to_char( victim, &af );
    send_to_char( "You are surrounded by a pink outline.\n\r", victim );
    act( "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM );
    return;
}


void
spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *ich;

    act( "$n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You conjure a cloud of purple smoke.\n\r", ch );

    for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
    {
	if (ich->invis_level > 0)
	    continue;

	if ( ich == ch || saves_spell( level, ich,DAM_OTHER) )
	    continue;

	affect_strip ( ich, gsn_invis			);
	affect_strip ( ich, gsn_mass_invis		);
	affect_strip ( ich, gsn_sneak			);
	xREMOVE_BIT  ( ich->affected_by, AFF_HIDE	);
	xREMOVE_BIT  ( ich->affected_by, AFF_INVISIBLE	);
	xREMOVE_BIT  ( ich->affected_by, AFF_SNEAK	);
	act( "$n is revealed!", ich, NULL, NULL, TO_ROOM );
	send_to_char( "You are revealed!\n\r", ich );
    }

    return;
}


void
spell_floating_disc( int sn, int level,CHAR_DATA *ch,void *vo,int target )
{
    OBJ_DATA *disc, *floating;

    floating = get_eq_char(ch,WEAR_FLOAT);
    if (floating != NULL && IS_OBJ_STAT(floating,ITEM_NOREMOVE))
    {
	act("You can't remove $p.",ch,floating,NULL,TO_CHAR);
	return;
    }

    disc = create_object(get_obj_index(OBJ_VNUM_DISC), 0);
    disc->value[0]	= ch->level * 10; /* 10 pounds per level capacity */
    disc->value[3]	= ch->level * 5; /* 5 pounds per level max per item */
    disc->timer		= ch->level * 2 - number_range(0,level / 2);

    act("$n has created a floating black disc.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You create a floating disc.\n\r",ch);
    obj_to_char(disc,ch);
    wear_obj(ch,disc,TRUE);
    return;
}


void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_FLYING) )
    {
	if (victim == ch)
	  send_to_char("You are already airborne.\n\r",ch);
	else
	  act("$N doesn't need your help to fly.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level + 3;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );
    send_to_char( "Your feet rise off the ground.\n\r", victim );
    act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}

/* RT clerical berserking spell */

void spell_frenzy(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim,sn) || IS_AFFECTED(victim,AFF_BERSERK))
    {
	if (victim == ch)
	  send_to_char("You are already in a frenzy.\n\r",ch);
	else
	  act("$N is already in a frenzy.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (is_affected(victim,skill_lookup("calm")))
    {
	if (victim == ch)
	  send_to_char("Why don't you just relax for a while?\n\r",ch);
	else
	  act("$N doesn't look like $e wants to fight anymore.",
	      ch,NULL,victim,TO_CHAR);
	return;
    }

    if ((IS_GOOD(ch) && !IS_GOOD(victim)) ||
	(IS_NEUTRAL(ch) && !IS_NEUTRAL(victim)) ||
	(IS_EVIL(ch) && !IS_EVIL(victim))
       )
    {
	act("Your god doesn't seem to like $N",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type 	 = sn;
    af.level	 = level;
    af.duration	 = level / 3;
    af.modifier  = level / 6;
    af.bitvector = 0;

    af.location  = APPLY_HITROLL;
    affect_to_char(victim,&af);

    af.location  = APPLY_DAMROLL;
    affect_to_char(victim,&af);

    af.modifier  = 10 * (level / 12);
    af.location  = APPLY_AC;
    affect_to_char(victim,&af);

    send_to_char("You are filled with holy wrath!\n\r",victim);
    act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
}

/* RT ROM-style gate */

void
spell_gate( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim;
    bool gate_pet;

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
    ||	 ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_NOSUMMON ) )
    ||   ( is_clan( victim ) && !is_same_clan( ch, victim ) )
    ||   ( !IS_NPC(victim) && victim->level >= LEVEL_HERO ) /* NOT trust */
    ||   ( IS_NPC( victim ) && IS_SET( victim->imm_flags,IMM_SUMMON ) )
    ||	 ( !is_same_landmass( ch->in_room, victim->in_room ) && ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    ||   ( IS_NPC( victim ) && saves_spell( level, victim, DAM_OTHER ) ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
    if ( ch->pet != NULL && ch->in_room == ch->pet->in_room )
	gate_pet = TRUE;
    else
	gate_pet = FALSE;

    act("$n steps through a gate and vanishes.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You step through a gate and vanish.\n\r",ch);
    char_from_room(ch);
    char_to_room(ch,victim->in_room);

    act("$n has arrived through a gate.",ch,NULL,NULL,TO_ROOM);
    do_function(ch, &do_look, "auto");

    if ( gate_pet )
    {
	act("$n steps through a gate and vanishes.",ch->pet,NULL,NULL,TO_ROOM);
	send_to_char("You step through a gate and vanish.\n\r",ch->pet);
	char_from_room(ch->pet);
	char_to_room(ch->pet,victim->in_room);
	act("$n has arrived through a gate.",ch->pet,NULL,NULL,TO_ROOM);
	do_function(ch->pet, &do_look, "auto");
    }

    if ( MOUNTED( ch ) )
    {
        act("$n steps through a gate and vanishes.",MOUNTED(ch),NULL,NULL,TO_ROOM);
        send_to_char("You step through a gate and vanish.\n\r",MOUNTED(ch));
        char_from_room(MOUNTED(ch));
        char_to_room(MOUNTED(ch),victim->in_room);
        act("$n has arrived through a gate.",MOUNTED(ch),NULL,NULL,TO_ROOM);
        do_look(MOUNTED(ch),"auto");
    }
}


void
spell_giant_strength(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already as strong as you can get!\n\r",ch);
	else
	  act("$N can't get any stronger.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "Your muscles surge with heightened power!\n\r", victim );
    act("$n's muscles surge with heightened power.",victim,NULL,NULL,TO_ROOM);
    return;
}



void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = UMAX(  20, victim->hit - dice(1,4) );
    if ( saves_spell( level, victim,DAM_HARM) )
	dam = UMIN( 50, dam / 2 );
    dam = UMIN( 100, dam );
    damage( ch, victim, dam, sn, DAM_HARM ,TRUE);
    return;
}

/* RT haste spell */

void spell_haste( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || IS_AFFECTED( victim, AFF_HASTE )
    ||   xIS_SET( victim->off_flags, OFF_FAST ) )
    {
	if (victim == ch)
	  send_to_char("You can't move any faster!\n\r",ch);
 	else
	  act("$N is already moving as fast as $E can.",
	      ch,NULL,victim,TO_CHAR);
        return;
    }

    if (IS_AFFECTED(victim,AFF_SLOW))
    {
	if (!check_dispel(level,victim,skill_lookup("slow")))
	{
	    if (victim != ch)
	        send_to_char("Spell failed.\n\r",ch);
	    send_to_char("You feel momentarily faster.\n\r",victim);
	    return;
	}
        act("$n is moving less slowly.",victim,NULL,NULL,TO_ROOM);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    if (victim == ch)
      af.duration  = level/2;
    else
      af.duration  = level/4;
    af.location  = APPLY_DEX;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = AFF_HASTE;
    affect_to_char( victim, &af );
    send_to_char( "You feel yourself moving more quickly.\n\r", victim );
    act("$n is moving more quickly.",victim,NULL,NULL,TO_ROOM);
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    return;
}



void
spell_heal( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *	victim = (CHAR_DATA *) vo;

//  victim->hit = UMIN( victim->hit + 100, victim->max_hit );
//  update_pos( victim );

    //heal_spell( victim, dice( 10, 20 ), 4, level );
      heal_spell( victim, dice( 2, 100+level / 2 ), 2, level);
    send_to_char( "A warm feeling fills your body.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}


void
spell_healing_hands(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *)vo;
    int divisor = 1;
    int heal = 0;
    int mana = 0;

    if ( ch == victim )
    {
	if ( ch->level + 2 == skill_table[sn].skill_level[ch->class] )
            mana = 50;
	else
	    mana = UMAX( skill_table[sn].min_mana,
	    100 / ( 2 + ch->level - skill_table[sn].skill_level[ch->class] ) );

	ch->mana += mana;
	send_to_char("You cannot cast this spell on yourself.\n\r", ch );
	return;
    }

    if (victim->hit >= victim->max_hit )
    {
	act_color(AT_MAGIC, "You heal $N.", ch, NULL, victim, TO_CHAR, POS_RESTING );
	return;
    }

    if ( IS_NEUTRAL( ch ) )
    {
	divisor = 2;
	level /= 2;
    }
    else if ( IS_EVIL( ch ) )
    {
	divisor = 4;
	level /= 4;
    }
    level = UMAX( 1, level );

    heal = (victim->max_hit - victim->hit) / divisor;
    heal_spell( victim, heal, UMAX( heal / 15, 1 ), level );

    act_color(AT_MAGIC, "You heal $N", ch, NULL, victim, TO_CHAR, POS_RESTING );
    act_color(AT_MAGIC, "$n heals you.", ch, NULL, victim, TO_VICT, POS_RESTING );
    act_color(AT_MAGIC, "$n heals $N.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
    return;
}


void
spell_heat_metal( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *	victim = (CHAR_DATA *) vo;
    OBJ_DATA *	obj_lose;
    OBJ_DATA *	obj_next;
    int		dam = 0;
    bool	fail;

   fail = TRUE;
   dam = 0;

   if ( !saves_spell( level + 2, victim, DAM_FIRE )
   &&   !IS_SET( victim->imm_flags, IMM_FIRE ) )
   {
        for ( obj_lose = victim->carrying;
	      obj_lose != NULL;
	      obj_lose = obj_next)
        {
	    obj_next = obj_lose->next_content;
            if ( number_range( 1, 2 * level ) > obj_lose->level
	    &&   !saves_spell( level, victim, DAM_FIRE )
	    &&	 is_metal( obj_lose )
//	    &&   !IS_OBJ_STAT( obj_lose, ITEM_NONMETAL )
	    &&   !IS_OBJ_STAT( obj_lose, ITEM_BURN_PROOF ) )
            {
                switch ( obj_lose->item_type )
                {
               	case ITEM_ARMOR:
		if ( obj_lose->wear_loc != WEAR_NONE ) /* remove the item */
		{
		    if ( can_drop_obj( victim, obj_lose )
		    &&  ( obj_lose->weight / 160 ) <
			number_range( 1, 2 * get_curr_stat( victim, STAT_DEX ) )
		    &&  remove_obj( victim, obj_lose->wear_loc, TRUE ) )
		    {
		        act_color( AT_ACTION, "$n yelps and throws $p to the ground!",
			    victim, obj_lose, NULL, TO_ROOM, POS_RESTING );
		        act_color( AT_ACTION, "You remove and drop $p before it burns you.",
			    victim, obj_lose, NULL, TO_CHAR, POS_RESTING );
			dam += ( number_range( 1, obj_lose->level ) / 3 );
                        obj_from_char( obj_lose );
                        obj_to_room( obj_lose, victim->in_room );
                        add_obj_fall_event( obj_lose );
                        fail = FALSE;
                    }
		    else /* stuck on the body! ouch! */
		    {
			act_color( AT_MAGIC, "Your skin is seared by $p!",
			    victim, obj_lose, NULL, TO_CHAR, POS_RESTING );
			dam += (number_range( 1, obj_lose->level ) );
			fail = FALSE;
		    }

		}
		else /* drop it if we can */
		{
		    if ( can_drop_obj( victim, obj_lose ) )
		    {
                        act_color( AT_ACTION, "$n yelps and throws $p to the ground!",
                            victim, obj_lose, NULL, TO_ROOM, POS_RESTING );
                        act_color( AT_ACTION, "You yelp and drop $p before it burns you.",
                            victim, obj_lose, NULL, TO_CHAR, POS_RESTING );
                        dam += ( number_range( 1, obj_lose->level ) / 6 );
                        obj_from_char( obj_lose );
                        obj_to_room( obj_lose, victim->in_room );
                        add_obj_fall_event( obj_lose );
			fail = FALSE;
                    }
		    else /* cannot drop */
		    {
                        act_color( AT_MAGIC, "Your skin is seared by $p!",
                            victim, obj_lose, NULL, TO_CHAR, POS_RESTING );
                        dam += ( number_range( 1, obj_lose->level ) / 2 );
			fail = FALSE;
                    }
		}
                break;
                case ITEM_WEAPON:
		if ( obj_lose->wear_loc != WEAR_NONE ) /* try to drop it */
		{
		    if ( IS_WEAPON_STAT( obj_lose, WEAPON_FLAMING ) )
			continue;

		    if ( can_drop_obj( victim, obj_lose )
		    &&  remove_obj( victim, obj_lose->wear_loc, TRUE ) )
		    {
			act_color( AT_ACTION, "$n is burned by $p, and throws it to the ground.",
			    victim, obj_lose, NULL, TO_ROOM, POS_RESTING );
			send_to_char(
			    "You throw your red-hot weapon to the ground!\n\r",
			    victim );
			dam += 1;
			obj_from_char( obj_lose );
			obj_to_room( obj_lose, victim->in_room );
			add_obj_fall_event( obj_lose );
			fail = FALSE;
		    }
		    else /* YOWCH! */
		    {
			send_to_char("Your weapon sears your flesh!\n\r",
			    victim );
			dam += number_range( 1, obj_lose->level );
			fail = FALSE;
		    }
		}
                else /* drop it if we can */
                {
                    if ( can_drop_obj( victim, obj_lose ) )
                    {
                        act_color( AT_ACTION, "$n throws a burning hot $p to the ground!",
                            victim, obj_lose, NULL, TO_ROOM, POS_RESTING );
                        act_color( AT_ACTION, "You and drop $p before it burns you.",
                            victim, obj_lose, NULL, TO_CHAR, POS_RESTING );
                        dam += ( number_range( 1, obj_lose->level) / 6 );
                        obj_from_char( obj_lose );
                        obj_to_room( obj_lose, victim->in_room );
                        add_obj_fall_event( obj_lose );
                        fail = FALSE;
                    }
                    else /* cannot drop */
                    {
                        act_color( AT_MAGIC, "Your skin is seared by $p!",
                            victim, obj_lose, NULL, TO_CHAR, POS_RESTING );
                        dam += ( number_range( 1, obj_lose->level ) / 2 );
                        fail = FALSE;
                    }
                }
                break;
		}
	    }
	}
    }
    if ( fail )
    {
        send_to_char( "Your spell had no effect.\n\r", ch );
	send_to_char( "You feel momentarily warmer.\n\r", victim );
    }
    else /* damage! */
    {
	if ( saves_spell( level, victim, DAM_FIRE ) )
	    dam = 2 * dam / 3;
	damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
    }
}


/* RT really nasty high-level attack spell */
void spell_holy_word(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;
    int bless_num, curse_num, frenzy_num;

    bless_num = skill_lookup("bless");
    curse_num = skill_lookup("curse");
    frenzy_num = skill_lookup("frenzy");

    act("$n utters a word of divine power!",ch,NULL,NULL,TO_ROOM);
    send_to_char("You utter a word of divine power.\n\r",ch);

    for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next_in_room;

	if ((IS_GOOD(ch) && IS_GOOD(vch)) ||
	    (IS_EVIL(ch) && IS_EVIL(vch)) ||
	    (IS_NEUTRAL(ch) && IS_NEUTRAL(vch)) )
	{
 	  send_to_char("You feel full more powerful.\n\r",vch);
	  spell_frenzy(frenzy_num,level,ch,(void *) vch,TARGET_CHAR);
	  spell_bless(bless_num,level,ch,(void *) vch,TARGET_CHAR);
	}

	else if ((IS_GOOD(ch) && IS_EVIL(vch)) ||
		 (IS_EVIL(ch) && IS_GOOD(vch)) )
	{
	  if (!is_safe_spell(ch,vch,TRUE))
	  {
            spell_curse(curse_num,level,ch,(void *) vch,TARGET_CHAR);
	    send_to_char("You are struck down!\n\r",vch);
	    dam = dice(level,6);
	    damage(ch,vch,dam,sn,DAM_ENERGY,TRUE);
	  }
	}

        else if (IS_NEUTRAL(ch))
	{
	  if (!is_safe_spell(ch,vch,TRUE))
	  {
            spell_curse(curse_num,level/2,ch,(void *) vch,TARGET_CHAR);
	    send_to_char("You are struck down!\n\r",vch);
	    dam = dice(level,4);
	    damage(ch,vch,dam,sn,DAM_ENERGY,TRUE);
   	  }
	}
    }

    send_to_char("You feel drained.\n\r",ch);
    ch->move = 0;
    ch->hit /= 2;
}

void
spell_identify( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    OBJ_DATA *		obj;
    char		buf[MAX_STRING_LENGTH];
    char		buf1[SHORT_STRING_LENGTH];
    char		buf2[SHORT_STRING_LENGTH];
    int			count;
    bool		found;
    int			iValue;
    char *		p;
    AFFECT_DATA *	paf;
    TEXT_DATA *		page;
    char *		q;
    MONEY		value;

    obj = (OBJ_DATA *)vo;

    value.gold   = 0;
    value.silver = 0;
    value.copper = 0;
    value.fract  = obj->cost;
    normalize( &value );

    if ( IS_SET( obj->extra_flags, ITEM_NOIDENTIFY ) && !IS_IMMORTAL( ch ) )
    {
        act( "The magic of $p proves stronger than your spell.", ch, obj, NULL, TO_CHAR );
        return;
    }

    if ( IS_IMMORTAL( ch ) )
	ch_printf( ch,
	    "Object '%s' (vnum #%d) is type %s, extra flags %s.\n\r"
	    "Weight is %d(%s), value is %s, level is %d.\n\r",

	    obj->name,
	    obj->pIndexData->vnum,
	    item_name(obj->item_type),
	    extra_bit_name( obj->extra_flags ),
	    obj->weight, weight_string( obj->weight ),
	    money_string( &value, FALSE, FALSE ),
	    obj->level );
    else
	ch_printf( ch,
	    "Object '%s' is type %s, extra flags %s.\n\r"
	    "Weight is %s, value is %s, level is %d.\n\r",

	    obj->name,
	    item_name(obj->item_type),
	    extra_bit_name( obj->extra_flags ),
	    weight_string( obj->weight ),
	    money_string( &value, FALSE, FALSE ),
	    obj->level );

    switch ( obj->item_type )
    {

    case ITEM_LIGHT:
        if (obj->value[2] > 0)
            sprintf( buf, "Hours of light remaining: %d", obj->value[2]);
        else if (obj->value[2] < 0)
	    strcpy( buf, "Hours of light remaining: Infinte!");
        else
	    strcpy( buf, "Hours of light remaining: None.");

        send_to_char( buf, ch );

       break;
    case ITEM_BOOK:
	count = 0;
	for ( page = obj->pIndexData->page; page != NULL; page = page->next )
	    count++;
	sprintf( buf, "Pages: %d\n\r", count );
	send_to_char( buf, ch );
	break;
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf( buf, "Level %d spells of:", obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[1] >= 0 && obj->value[1] < top_skill )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[2] >= 0 && obj->value[2] < top_skill )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[3] >= 0 && obj->value[3] < top_skill )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	if (obj->value[4] >= 0 && obj->value[4] < top_skill)
	{
	    send_to_char(" '",ch);
	    send_to_char(skill_table[obj->value[4]].name,ch);
	    send_to_char("'",ch);
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_HERB:
	sprintf( buf, "Level %d spells of:", obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[1] >= 0 && obj->value[1] < top_skill )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[2] >= 0 && obj->value[2] < top_skill )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[3] >= 0 && obj->value[3] < top_skill )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_LENS:
    case ITEM_WAND:
    case ITEM_STAFF:
	if ( obj->value[1] != -1 )
	    ch_printf( ch, "Has %d charges of level %d",
		obj->value[2], obj->value[0] );
	else
	    ch_printf( ch, "Has unlimited charges of level %d",
		obj->value[0] );

	if ( obj->value[3] >= 0 && obj->value[3] < top_skill )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_DRINK_CON:
        sprintf(buf,"It holds %s-colored %s.\n\r",
            liq_table[obj->value[2]].liq_color,
            liq_table[obj->value[2]].liq_name);
        send_to_char(buf,ch);
        break;

    case ITEM_CONTAINER:
	strcpy( buf1, weight_string( obj->value[0] ) );
	strcpy( buf2, weight_string( obj->value[3] ) );
	if ( IS_IMMORTAL( ch ) )
	sprintf( buf, "Capacity: %d(%s)  Max 1 item: %d(%s)  flags: %s\n\r",
		 obj->value[0], buf1, obj->value[3], buf2,
		 cont_bit_name(obj->value[1] ) );
	else
	    sprintf( buf, "Capacity: %s  Max 1 item: %s  flags: %s\n\r",
		     buf1, buf2, cont_bit_name(obj->value[1] ) );
	send_to_char( buf, ch );
	if ( obj->value[4] != 100 )
	{
	    sprintf( buf, "Weight multiplier: %d%%\n\r",
		obj->value[4] );
	    send_to_char( buf, ch );
	}
	break;

    case ITEM_WEAPON:
	ch_printf( ch, "Weapon type is %s.\n\r", flag_string( weapon_types, obj->value[0] ) );
	sprintf( buf, "Damage is %dd%d%+d (average %d).\n\r",
		obj->value[1],obj->value[2], obj->value[5],
		( ( 1 + obj->value[2]) * obj->value[1] + obj->value[5] ) / 2 );
	send_to_char( buf, ch );
        if (obj->value[4])  /* weapon flags */
        {
            sprintf(buf,"Weapons flags: %s\n\r",weapon_bit_name(obj->value[4]));
            send_to_char(buf,ch);
        }
	break;

    case ITEM_ARMOR:
	sprintf( buf,
	"Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r",
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	send_to_char( buf, ch );
	break;

    case ITEM_SHEATH:
	p = stpcpy( buf, "Weapons:" );
	found = FALSE;
	for ( iValue = 3; iValue < 6; iValue++ )
	{
	    q = weapon_name( obj->value[iValue] );
	    if ( str_cmp( q, "exotic" ) )
	    {
		p += sprintf( p, " %s", q );
		found = TRUE;
	    }
	}
	if ( !found )
	    p = stpcpy( p, " (none)" );
	send_to_char( buf, ch );
	break;
    }

    if ( obj->ac_type != INVOKE_NONE )
    {
	switch( obj->ac_type )
	{
	    default:
		send_to_char( "Invoke Type Unknown.\n\r", ch );
		break;

	    case INVOKE_OBJ:
		if ( obj->ac_charge[1] != -1 )
		    ch_printf( ch,
			   "Object creation invoke, with [%d/%d] charges.\n\r",
			   obj->ac_charge[0], obj->ac_charge[1] );
		else
		    ch_printf( ch,
			"Object creation invoke, with unlimited charges.\n\r" );
		break;

	    case INVOKE_MOB:
		if ( obj->ac_charge[1] != -1 )
		    ch_printf( ch,
			 "Monster creation invoke, with [%d/%d] charges.\n\r",
			  obj->ac_charge[0], obj->ac_charge[1] );
		else
		    ch_printf( ch,
			"Monster creation invoke, with unlimited charges.\n\r" );
		break;
	    case INVOKE_TRANS:
		if ( obj->ac_charge[1] != -1 )
		    ch_printf( ch,
			"Transfer invoke, with [%d/%d] charges.\n\r",
			obj->ac_charge[0], obj->ac_charge[1] );
		else
		    ch_printf( ch,
			"Transfer invoke, with unlimited charges.\n\r" );
		break;

	    case INVOKE_MORPH:
		if ( obj->ac_charge[1] != -1 )
		    ch_printf( ch,
			"Object morph invoke, with [%d/%d] charges.\n\r",
			 obj->ac_charge[0], obj->ac_charge[1] );
		else
		    ch_printf( ch,
			"Object morph invoke, with unlimited charges.\n\r" );
		break;

	    case INVOKE_SPELL:
		if ( obj->ac_charge[1] != -1 )
		    ch_printf( ch,
			"Spell invoke, has [%d/%d] charges of ",
			 obj->ac_charge[0], obj->ac_charge[1] );
		else
		    ch_printf( ch,
			"Spell invoke, with unlimited charges of " );
		if ( is_sn( obj->ac_vnum ) )
		{
		    send_to_char( " '", ch );
		    send_to_char( skill_table[obj->ac_vnum].name, ch );
		    send_to_char( "'\n\r", ch );
		}
		else
		    send_to_char( "(none)\n\r", ch );
		break;
	}
    }

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char(buf,ch);
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                    case TO_AFFECTS:
                        sprintf( buf,"Adds %s affect.\n",
                            affect_bitval_name( paf->bitvector ) );
                        break;
                    case TO_OBJECT:
                        sprintf(buf,"Adds %s object flag.\n",
                            extra_bit_name(paf->bitvector));
                        break;
                    case TO_IMMUNE:
                        sprintf(buf,"Adds immunity to %s.\n",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_RESIST:
                        sprintf(buf,"Adds resistance to %s.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_VULN:
                        sprintf(buf,"Adds vulnerability to %s.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    default:
                        sprintf(buf,"Unknown bit %d: %d\n\r",
                            paf->where,paf->bitvector);
                        break;
                }
	        send_to_char( buf, ch );
	    }
	}
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d",
	    	affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
            if ( paf->duration > -1)
                sprintf(buf,", %d hours.\n\r",paf->duration);
            else
                sprintf(buf,".\n\r");
	    send_to_char(buf,ch);
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                    case TO_AFFECTS:
                        sprintf(buf,"Adds %s affect.\n",
                            affect_bitval_name( paf->bitvector ) );
                        break;
                    case TO_OBJECT:
                        sprintf(buf,"Adds %s object flag.\n",
                            extra_bit_name(paf->bitvector));
                        break;
		    case TO_WEAPON:
			sprintf(buf,"Adds %s weapon flags.\n",
			    weapon_bit_name(paf->bitvector));
			break;
                    case TO_IMMUNE:
                        sprintf(buf,"Adds immunity to %s.\n",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_RESIST:
                        sprintf(buf,"Adds resistance to %s.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_VULN:
                        sprintf(buf,"Adds vulnerability to %s.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    default:
                        sprintf(buf,"Unknown bit %d: %d\n\r",
                            paf->where,paf->bitvector);
                        break;
                }
                send_to_char(buf,ch);
            }
	}
    }

    return;
}



void
spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_INFRARED ) )
    {
	if ( victim == ch )
	    send_to_char( "You can already see in the dark.\n\r", ch );
	else
            act( "$N already has infravision.\n\r", ch, NULL, victim, TO_CHAR );
	return;
    }
    act_color( AT_MAGIC, "$n's eyes glow red.\n\r",
               victim, NULL, NULL, TO_ROOM, POS_RESTING );

    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 2 * level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INFRARED;
    affect_to_char( victim, &af );
    act_color( AT_MAGIC, "Your eyes glow red.\n\r",
               victim, NULL, NULL, TO_CHAR, POS_RESTING );
    return;
}



void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    AFFECT_DATA af;

    /* object invisibility */
    if (target == TARGET_OBJ)
    {
	obj = (OBJ_DATA *) vo;

	if (IS_OBJ_STAT(obj,ITEM_INVIS))
	{
	    act("$p is already invisible.",ch,obj,NULL,TO_CHAR);
	    return;
	}

	af.where	= TO_OBJECT;
	af.type		= sn;
	af.level	= level;
	af.duration	= level + 12;
	af.location	= APPLY_NONE;
	af.modifier	= 0;
	af.bitvector	= ITEM_INVIS;
	affect_to_obj(obj,&af);

	act("$p fades out of sight.",ch,obj,NULL,TO_ALL);
	return;
    }

    /* character invisibility */
    victim = (CHAR_DATA *) vo;

    if ( IS_AFFECTED(victim, AFF_INVISIBLE) )
	return;

    act( "$n fades out of existence.", victim, NULL, NULL, TO_ROOM );

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level + 12;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( victim, &af );
    send_to_char( "You fade out of existence.\n\r", victim );
    return;
}



void spell_know_alignment(int sn,int level,CHAR_DATA *ch,void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char *msg;
    int ap;

    ap = victim->alignment;

         if ( ap >  700 ) msg = "$N has a pure and good aura.";
    else if ( ap >  350 ) msg = "$N is of excellent moral character.";
    else if ( ap >  100 ) msg = "$N is often kind and thoughtful.";
    else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.";
    else if ( ap > -350 ) msg = "$N lies to $S friends.";
    else if ( ap > -700 ) msg = "$N is a black-hearted murderer.";
    else msg = "$N is the embodiment of pure evil!";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}



void spell_lightning_bolt(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0, 25, 28,
	31, 34, 37, 40, 40,	41, 42, 42, 43, 44,
	44, 45, 46, 46, 47,	48, 48, 49, 50, 50,
	51, 52, 52, 53, 54,	54, 55, 56, 56, 57,
	58, 58, 59, 60, 60,	61, 62, 62, 63, 64
    };
    int dam;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim,DAM_LIGHTNING) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_LIGHTNING ,TRUE);
    return;
}



void
spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    char		buf[MAX_INPUT_LENGTH];
    BUFFER *		buffer;
    OBJ_DATA *		obj;
    OBJ_DATA *		in_obj;
    bool		found;
    int			number;
    int			max_found;
    ROOM_INDEX_DATA *	room;

    found = FALSE;
    number = 0;
    max_found = IS_IMMORTAL( ch ) ? 250 : 2 * level;

    buffer = new_buf( );

    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	if ( !can_see_obj( ch, obj ) || !is_name( target_name, obj->name )
    	||   IS_OBJ_STAT( obj, ITEM_NOLOCATE ) || number_percent( ) > 2 * level
	||   ch->level < obj->level )
	    continue;

	if ( obj->item_type == ITEM_POTION
	||   obj->item_type == ITEM_PILL
	||   obj->item_type == ITEM_HERB )
	    continue;

	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by != NULL )
	    room = in_obj->carried_by->in_room;
	else if ( in_obj->in_room != NULL )
	    room = in_obj->in_room;
	else
	    room = NULL;

	if ( room == NULL
	|| ( !is_same_landmass( room, ch->in_room ) && ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) ) )
	    continue;

	found = TRUE;

	if ( in_obj->carried_by != NULL && can_see( ch, in_obj->carried_by ) )
	{
	    sprintf( buf, "one is carried by %s\n\r",
		     PERS( in_obj->carried_by, ch ) );
	}
	else
	{
	    if ( IS_IMMORTAL(ch) && in_obj->in_room != NULL )
		sprintf( buf, "one is in %s [Room %d]\n\r",
		    in_obj->in_room->name, in_obj->in_room->vnum );
	    else
	    	sprintf( buf, "one is in %s\n\r",
		    in_obj->in_room == NULL
		    	? "somewhere" : in_obj->in_room->name );
	}

	buf[0] = UPPER( buf[0] );
	add_buf( buffer, buf );
	number++;

    	if ( number >= max_found )
	    break;
    }

    if ( !found )
	send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
	page_to_char(buf_string(buffer),ch);

    free_buf(buffer);

    return;
}



void spell_magic_missile( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 3,  3,  4,  4,  5,	 6,  6,  6,  6,  6,
	 7,  7,  7,  7,  7,	 8,  8,  8,  8,  8,
	 9,  9,  9,  9,  9,	10, 10, 10, 10, 10,
	11, 11, 11, 11, 11,	12, 12, 12, 12, 12,
	13, 13, 13, 13, 13,	14, 14, 14, 14, 14
    };
    int dam, i;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    /* 1 missile for every 2 caster levels */
    if(ch->level<2) i=1; else i=ch->level/2;
	for(i; i>0; i--){
    dam		= number_range( dam_each[level] / 1, dam_each[level] * 2 );
    if ( saves_spell( level, victim,DAM_ENERGY) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_ENERGY ,TRUE);
}
    return;
}

void spell_mass_healing(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *gch;
    int heal_num, refresh_num;

    heal_num = skill_lookup("heal");
    refresh_num = skill_lookup("refresh");

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ((IS_NPC(ch) && IS_NPC(gch)) ||
	    (!IS_NPC(ch) && !IS_NPC(gch)))
	{
	    spell_heal(heal_num,level,ch,(void *) gch,TARGET_CHAR);
	    spell_refresh(refresh_num,level,ch,(void *) gch,TARGET_CHAR);
	}
    }
}


void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) || IS_AFFECTED(gch, AFF_INVISIBLE) )
	    continue;
	act( "$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly fade out of existence.\n\r", gch );

	af.where     = TO_AFFECTS;
	af.type      = sn;
    	af.level     = level/2;
	af.duration  = 24;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_INVISIBLE;
	affect_to_char( gch, &af );
    }
    send_to_char( "Ok.\n\r", ch );

    return;
}



void spell_null( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    send_to_char( "That's not a spell!\n\r", ch );
    return;
}



void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
    {
	if (victim == ch)
	  send_to_char("You are already out of phase.\n\r",ch);
	else
	  act("$N is already shifted out of phase.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( victim, &af );
    act( "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You turn translucent.\n\r", victim );
    return;
}

/* RT plague spell, very nasty */

void spell_plague( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (saves_spell(level,victim,DAM_DISEASE) ||
        (IS_NPC(victim) && IS_SET(victim->act,ACT_UNDEAD)))
    {
	if (ch == victim)
	  send_to_char("You feel momentarily ill, but it passes.\n\r",ch);
	else
	  act("$N seems to be unaffected.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type 	  = sn;
    af.level	  = level * 3/4;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = -5;
    af.bitvector = AFF_PLAGUE;
    affect_join(victim,&af);

    send_to_char
      ("You scream in agony as plague sores erupt from your skin.\n\r",victim);
    act("$n screams in agony as plague sores erupt from $s skin.",
	victim,NULL,NULL,TO_ROOM);
}

void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    AFFECT_DATA af;


    if (target == TARGET_OBJ)
    {
	obj = (OBJ_DATA *) vo;

	if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
	{
	    if (IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
	    {
		act("Your spell fails to corrupt $p.",ch,obj,NULL,TO_CHAR);
		return;
	    }
	    obj->value[3] = 1;
	    act("$p is infused with poisonous vapors.",ch,obj,NULL,TO_ALL);
	    return;
	}

	if (obj->item_type == ITEM_WEAPON)
	{
	    if (IS_WEAPON_STAT(obj,WEAPON_FLAMING)
	    ||  IS_WEAPON_STAT(obj,WEAPON_FROST)
	    ||  IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC)
	    ||  IS_WEAPON_STAT(obj,WEAPON_SHARP)
	    ||  IS_WEAPON_STAT(obj,WEAPON_VORPAL)
	    ||  IS_WEAPON_STAT(obj,WEAPON_SHOCKING)
	    ||  IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
	    {
		act("You can't seem to envenom $p.",ch,obj,NULL,TO_CHAR);
		return;
	    }

	    if (IS_WEAPON_STAT(obj,WEAPON_POISON))
	    {
		act("$p is already envenomed.",ch,obj,NULL,TO_CHAR);
		return;
	    }

	    af.where	 = TO_WEAPON;
	    af.type	 = sn;
	    af.level	 = level / 2;
	    af.duration	 = level/8;
 	    af.location	 = 0;
	    af.modifier	 = 0;
	    af.bitvector = WEAPON_POISON;
	    affect_to_obj(obj,&af);

	    act("$p is coated with deadly venom.",ch,obj,NULL,TO_ALL);
	    return;
	}

	act("You can't poison $p.",ch,obj,NULL,TO_CHAR);
	return;
    }

    victim = (CHAR_DATA *) vo;

    if ( saves_spell( level, victim,DAM_POISON) )
    {
	act("$n turns slightly green, but it passes.",victim,NULL,NULL,TO_ROOM);
	send_to_char("You feel momentarily ill, but it passes.\n\r",victim);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = AFF_POISON;
    affect_join( victim, &af );
    send_to_char( "You feel very sick.\n\r", victim );
    act("$n looks very ill.",victim,NULL,NULL,TO_ROOM);
    return;
}



void
spell_protection_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_SHIELDED( victim, SHLD_PROTECT_EVIL )
    ||   IS_SHIELDED( victim, SHLD_PROTECT_GOOD ) )
    {
        if ( victim == ch )
            send_to_char( "You are already protected.\n\r",ch );
        else
            act( "$N is already protected.", ch, NULL, victim, TO_CHAR );
        return;
    }

    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 1;
    af.bitvector = SHLD_PROTECT_EVIL;
    affect_to_char( victim, &af );
    send_to_char( "You feel holy and pure.\n\r", victim );
    if ( ch != victim )
        act( "$N is protected from evil.", ch, NULL, victim, TO_CHAR );
    return;
}


void
spell_protection_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_SHIELDED( victim, SHLD_PROTECT_GOOD )
    ||   IS_SHIELDED( victim, SHLD_PROTECT_EVIL ) )
    {
        if ( victim == ch )
            send_to_char( "You are already protected.\n\r", ch );
        else
            act( "$N is already protected.", ch, NULL, victim, TO_CHAR );
        return;
    }

    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 1;
    af.bitvector = SHLD_PROTECT_GOOD;
    affect_to_char( victim, &af );
    send_to_char( "You feel aligned with darkness.\n\r", victim );
    if ( ch != victim )
        act( "$N is protected from good.", ch, NULL, victim, TO_CHAR );
    return;
}


void spell_ray_of_truth (int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam, align;

    if (IS_EVIL(ch) )
    {
        victim = ch;
        send_to_char("The energy explodes inside you!\n\r",ch);
    }

    if (victim != ch)
    {
        act("$n raises $s hand, and a blinding ray of light shoots forth!",
            ch,NULL,NULL,TO_ROOM);
        send_to_char(
	   "You raise your hand and a blinding ray of light shoots forth!\n\r",
	   ch);
    }

    if (IS_GOOD(victim))
    {
	act("$n seems unharmed by the light.",victim,NULL,victim,TO_ROOM);
	send_to_char("The light seems powerless to affect you.\n\r",victim);
	return;
    }

    dam = dice( level, 10 );
    if ( saves_spell( level, victim,DAM_HOLY) )
        dam /= 2;

    align = victim->alignment;
    align -= 350;

    if (align < -1000)
	align = -1000 + (align + 1000) / 3;

    dam = (dam * align * align) / 1000000;

    damage( ch, victim, dam, sn, DAM_HOLY ,TRUE);
    spell_blindness(gsn_blindness,
	3 * level / 4, ch, (void *) victim,TARGET_CHAR);
}


void spell_recharge( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int chance, percent;

    if (obj->item_type != ITEM_WAND && obj->item_type != ITEM_STAFF)
    {
	send_to_char("That item does not carry charges.\n\r",ch);
	return;
    }

    if (obj->value[3] >= 3 * level / 2)
    {
	send_to_char("Your skills are not great enough for that.\n\r",ch);
	return;
    }

    if (obj->value[1] == 0)
    {
	send_to_char("That item has already been recharged once.\n\r",ch);
	return;
    }

    chance = 40 + 2 * level;

    chance -= obj->value[3]; /* harder to do high-level spells */
    chance -= (obj->value[1] - obj->value[2]) *
	      (obj->value[1] - obj->value[2]);

    chance = UMAX(level/2,chance);

    percent = number_percent();

    if (percent < chance / 2)
    {
	act("$p glows softly.",ch,obj,NULL,TO_CHAR);
	act("$p glows softly.",ch,obj,NULL,TO_ROOM);
	obj->value[2] = UMAX(obj->value[1],obj->value[2]);
	obj->value[1] = 0;
	return;
    }

    else if (percent <= chance)
    {
	int chargeback,chargemax;

	act("$p glows softly.",ch,obj,NULL,TO_CHAR);
	act("$p glows softly.",ch,obj,NULL,TO_CHAR);

	chargemax = obj->value[1] - obj->value[2];

	if (chargemax > 0)
	    chargeback = UMAX(1,chargemax * percent / 100);
	else
	    chargeback = 0;

	obj->value[2] += chargeback;
	obj->value[1] = 0;
	return;
    }

    else if (percent <= UMIN(95, 3 * chance / 2))
    {
	send_to_char("Nothing seems to happen.\n\r",ch);
	if (obj->value[1] > 1)
	    obj->value[1]--;
	return;
    }

    else /* whoops! */
    {
	act("$p glows brightly and explodes!",ch,obj,NULL,TO_CHAR);
	act("$p glows brightly and explodes!",ch,obj,NULL,TO_ROOM);
	extract_obj(obj);
    }
}

void
spell_refresh( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *	victim = (CHAR_DATA *) vo;
    int		condition;
    bool	tired;

    if ( !IS_NPC( victim ) )
    {
        condition = victim->pcdata->condition[COND_TIRED];
        if ( condition >= 0 && condition < MAX_COND )
        {
            condition = UMIN( condition + level / 2, MAX_COND );
            victim->pcdata->condition[COND_TIRED] = condition;
        }
        tired = ( condition >= 0 && condition < MAX_COND );
    }
    else
        tired = FALSE;

    victim->move = UMIN( victim->move + level, victim->max_move );
    if ( victim->max_move == victim->move && !tired )
        send_to_char( "You feel fully refreshed!\n\r", victim );
    else
        send_to_char( "You feel less tired.\n\r", victim );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    return;
}


void
spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    bool found = FALSE;

    /* do object cases first */
    if ( target == TARGET_OBJ )
    {
	obj = (OBJ_DATA *) vo;

	if ( IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
	{
	    if ( ( !IS_OBJ_STAT( obj, ITEM_NOUNCURSE )
	         &&  !saves_dispel (level + 2, obj->level, 0 ) )
	    || ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) ) )
	    {
		REMOVE_BIT(obj->extra_flags,ITEM_NODROP);
		REMOVE_BIT(obj->extra_flags,ITEM_NOREMOVE);
		act("$p glows blue.",ch,obj,NULL,TO_ALL);
		return;
	    }

	    act("The curse on $p is beyond your power.",ch,obj,NULL,TO_CHAR);
	    return;
	}
	act("There doesn't seem to be a curse on $p.",ch,obj,NULL,TO_CHAR);
	return;
    }

    /* characters */
    victim = (CHAR_DATA *) vo;

    if ( check_dispel( level, victim,gsn_curse ) )
    {
	send_to_char( "You feel better.\n\r", victim );
	act( "$n looks more relaxed.", victim, NULL, NULL, TO_ROOM );
    }

    for ( obj = victim->carrying; (obj != NULL && !found); obj = obj->next_content )
    {
        if ( ( IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
	&&   ( ( !IS_OBJ_STAT( obj, ITEM_NOUNCURSE ) ) || IS_IMMORTAL( ch ) ) )
        {   /* attempt to remove curse */
            if ( !saves_dispel( level, obj->level, 0 ) )
            {
                found = TRUE;
                REMOVE_BIT( obj->extra_flags, ITEM_NODROP );
                REMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );
                act( "Your $p glows blue.", victim, obj, NULL, TO_CHAR );
                act( "$n's $p glows blue.", victim, obj, NULL, TO_ROOM );
            }
         }
    }
}


void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *	victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_SHIELDED( victim, SHLD_SANCTUARY ) )
    {
	if ( victim == ch )
	    send_to_char( "You are already in sanctuary.\n\r", ch );
	else
	    act( "$N is already in sanctuary.", ch, NULL, victim, TO_CHAR );
	return;
    }

    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = SHLD_SANCTUARY;
    affect_to_char( victim, &af );
    act( "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You are surrounded by a white aura.\n\r", victim );
    return;
}

void spell_black_mantle( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *	victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_SHIELDED( victim, SHLD_BLACK_MANTLE ) )
    {
	if ( victim == ch )
	    send_to_char( "You are already protected by dark forces.\n\r", ch );
	else
	    act( "$N is already protected by dark forces.", ch, NULL, victim, TO_CHAR );
	return;
    }

    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = SHLD_BLACK_MANTLE;
    affect_to_char( victim, &af );
    act( "$n is surrounded by a dark aura.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You are surrounded by a dark aura.\n\r", victim );
    return;
}



void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already shielded from harm.\n\r",ch);
	else
	  act("$N is already protected by a shield.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 8 + level;
    af.location  = APPLY_AC;
    af.modifier  = -20;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act( "$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You are surrounded by a force shield.\n\r", victim );
    return;
}



void spell_shocking_grasp(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const int dam_each[] =
    {
	 0,
	 0,  0,  0,  0,  0,	 0, 20, 25, 29, 33,
	36, 39, 39, 39, 40,	40, 41, 41, 42, 42,
	43, 43, 44, 44, 45,	45, 46, 46, 47, 47,
	48, 48, 49, 49, 50,	50, 51, 51, 52, 52,
	53, 53, 54, 54, 55,	55, 56, 56, 57, 57
    };
    int dam;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim,DAM_LIGHTNING) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_LIGHTNING ,TRUE);
    return;
}



void
spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *		victim;
    AFFECT_DATA		af;

    victim = (CHAR_DATA *) vo;

    if ( IS_AFFECTED( victim, AFF_SLEEP ) )
    {
        act( "$N is already sleeping.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ( IS_NPC( victim ) && IS_SET( victim->act, ACT_UNDEAD ) )
    ||   (level + 2) < victim->level
    ||   saves_spell( level-4, victim, DAM_CHARM ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( get_event_char( victim, EVENT_CHAR_DREAM ) != NULL )
    {
        strip_event_char( victim, EVENT_CHAR_DREAM );
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 4 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_join( victim, &af );

    if ( IS_AWAKE( victim ) )
    {
	send_to_char( "You feel very sleepy ..... zzzzzz.\n\r", victim );
	act( "$n goes to sleep.", victim, NULL, NULL, TO_ROOM );
	victim->position = POS_SLEEPING;
    }
    else
    {
        act_color( AT_MAGIC,
                   "A magic force overtakes you and drags you into a state of darkness from which you can not wake.",
                   victim, NULL, NULL, TO_CHAR,POS_SLEEPING );
        act_color( AT_MAGIC,
                   "$n is pulled from a normal sleep into a darkness from which $e can not wake.",
                   victim, NULL, NULL, TO_ROOM, POS_RESTING );
    }
    return;
}


void spell_slow( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || IS_AFFECTED(victim,AFF_SLOW))
    {
        if (victim == ch)
          send_to_char("You can't move any slower!\n\r",ch);
        else
          act("$N can't get any slower than that.",
              ch,NULL,victim,TO_CHAR);
        return;
    }

    if (saves_spell(level,victim,DAM_OTHER)
    ||  IS_SET(victim->imm_flags,IMM_MAGIC))
    {
	if (victim != ch)
            send_to_char("Nothing seemed to happen.\n\r",ch);
        send_to_char("You feel momentarily lethargic.\n\r",victim);
        return;
    }

    if (IS_AFFECTED(victim,AFF_HASTE))
    {
        if (!check_dispel(level,victim,skill_lookup("haste")))
        {
	    if (victim != ch)
            	send_to_char("Spell failed.\n\r",ch);
            send_to_char("You feel momentarily slower.\n\r",victim);
            return;
        }

        act("$n is moving less quickly.",victim,NULL,NULL,TO_ROOM);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/2;
    af.location  = APPLY_DEX;
    af.modifier  = -1 - (level >= 18) - (level >= 25) - (level >= 32);
    af.bitvector = AFF_SLOW;
    affect_to_char( victim, &af );
    send_to_char( "You feel yourself slowing d o w n...\n\r", victim );
    act("$n starts to move in slow motion.",victim,NULL,NULL,TO_ROOM);
    return;
}




void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
    {
	if (victim == ch)
	  send_to_char("Your skin is already as hard as a rock.\n\r",ch);
	else
	  act("$N is already as hard as can be.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = -40;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act( "$n's skin turns to stone.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "Your skin turns to stone.\n\r", victim );
    return;
}



void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim;

    if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   IS_SET(ch->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||	 IS_SET( ch->in_room->room_flags, ROOM_NO_IN )
    ||	 IS_SET( victim->in_room->room_flags, ROOM_NO_OUT )
    ||	 IS_SET( victim->in_room->room_flags, ROOM_NO_MAGIC )
    ||	 IS_SET( victim->in_room->area->area_flags, AREA_QUEST )
    ||	 ( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && IS_SET( victim->in_room->area->area_flags, AREA_PROTOTYPE ) )
    ||   ( IS_NPC( victim ) && IS_SET( victim->act, ACT_AGGRESSIVE ) )
    ||   victim->level >= level + 3
    ||   (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL)
    ||   victim->fighting != NULL
    ||	 ( IS_NPC( victim ) && IS_SET( victim->act, ACT_TRAIN ) )
    ||	 ( IS_NPC( victim ) && IS_SET( victim->act, ACT_PRACTICE ) )
    ||	 ( IS_NPC( victim ) && IS_SET( victim->act, ACT_GAIN ) )
    ||   (IS_NPC(victim) && IS_SET(victim->imm_flags,IMM_SUMMON))
    ||	 (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    ||   (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOSUMMON))
    ||	 ( !is_same_landmass( ch->in_room, victim->in_room ) && ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    ||   (IS_NPC(victim) && saves_spell( level, victim,DAM_OTHER)) )

    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    act( "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM );
    act( "$n has summoned you!", ch, NULL, victim,   TO_VICT );
    do_function(victim, &do_look, "auto" );
    return;
}



void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *pRoomIndex;

    if ( victim->in_room == NULL
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    || ( victim != ch && IS_SET(victim->imm_flags,IMM_SUMMON))
    || ( !IS_NPC(ch) && victim->fighting != NULL )
    || ( victim != ch
    && ( saves_spell( level - 5, victim,DAM_OTHER))))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    pRoomIndex = get_random_room(victim);
    if ( pRoomIndex == NULL )
    {
        send_to_char( "Spell fails.\n\r", ch );
        return;
    }

    if (victim != ch)
	send_to_char("You have been teleported!\n\r",victim);

    act( "$n vanishes!", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( "$n slowly fades into existence.", victim, NULL, NULL, TO_ROOM );
    do_function(victim, &do_look, "auto" );
    return;
}


void
spell_ventriloquate( int sn, int level, CHAR_DATA *ch,void *vo, int target )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	buf1[MAX_STRING_LENGTH];
    char	buf2[MAX_STRING_LENGTH];
    char	speaker[MAX_INPUT_LENGTH];
    CHAR_DATA *	vch;
    CHAR_DATA *	victim;
    char *	p;

    target_name = one_argument( target_name, arg1 );
    victim = get_char_room( ch, arg1 );
    if ( victim == NULL )
    {
        say_spell( ch, sn );
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) )
    {
        strcpy( speaker, victim->short_descr );
        p = speaker;
        while ( is_colcode ( p ) )
            p += 2;
        *p = UPPER( *p );
    }
    else
        strcpy( speaker, victim->name );

    sprintf( buf1, "`W%s says '`G%s`W'.\n\r",              speaker, target_name );
    sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
    buf1[0] = UPPER(buf1[0]);

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
	if ( vch != victim && IS_AWAKE( vch ) )
	    send_to_char( saves_spell( level, vch, DAM_OTHER ) ? buf2 : buf1, vch );
    }

    return;
}


void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || saves_spell( level, victim,DAM_OTHER) )
	return;

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 2;
    af.location  = APPLY_STR;
    af.modifier  = -1 * (level / 5);
    af.bitvector = AFF_WEAKEN;
    affect_to_char( victim, &af );
    send_to_char( "You feel your strength slip away.\n\r", victim );
    act("$n looks tired and weak.",victim,NULL,NULL,TO_ROOM);
    return;
}



/* RT recall spell is back */

void spell_word_of_recall( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *location;

    if (IS_NPC(victim))
	return;

    if ((location = get_room_index( ROOM_VNUM_TEMPLE)) == NULL)
    {
	send_to_char("You are completely lost.\n\r",victim);
	return;
    }

    if (IS_SET(victim->in_room->room_flags,ROOM_NO_RECALL) ||
	IS_AFFECTED(victim,AFF_CURSE))
    {
	send_to_char("Spell failed.\n\r",victim);
	return;
    }

    if (victim->fighting != NULL)
	stop_fighting(victim,TRUE);

    ch->move /= 2;
    act("$n disappears.",victim,NULL,NULL,TO_ROOM);
    char_from_room(victim);
    char_to_room(victim,location);
    act("$n appears in the room.",victim,NULL,NULL,TO_ROOM);
    do_function(victim, &do_look, "auto");
}

/*
 * NPC spells.
 */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam,hp_dam,dice_dam,hpch;

    act("$n spits acid at $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n spits a stream of corrosive acid at you.",ch,NULL,victim,TO_VICT);
    act("You spit acid at $N.",ch,NULL,victim,TO_CHAR);

    hpch = UMAX(12,ch->hit);
    hp_dam = number_range(hpch/11 + 1, hpch/6);
    dice_dam = dice(level,16);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);

    if (saves_spell(level,victim,DAM_ACID))
    {
	acid_effect(victim,level/2,dam/4,TARGET_CHAR);
	damage(ch,victim,dam/2,sn,DAM_ACID,TRUE);
    }
    else
    {
	acid_effect(victim,level,dam,TARGET_CHAR);
	damage(ch,victim,dam,sn,DAM_ACID,TRUE);
    }
}


void
spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch, *vch_next;
    int dam,hp_dam,dice_dam;
    int hpch;

    act("$n breathes forth a cone of fire.",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a cone of hot fire over you!",ch,NULL,victim,TO_VICT);
    act("You breath forth a cone of fire.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX( 10, ch->hit );
    hp_dam  = number_range( hpch/9+1, hpch/5 );
    dice_dam = dice(level,20);

    dam = UMAX(hp_dam + dice_dam /10, dice_dam + hp_dam / 10);
    fire_effect(victim->in_room,level,dam/2,TARGET_ROOM);

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch,vch,TRUE)
	||  (IS_NPC(vch) && IS_NPC(ch)
	&&   (ch->fighting != vch || vch->fighting != ch)))
	    continue;

	if (vch == victim) /* full damage */
	{
	    if (saves_spell(level,vch,DAM_FIRE))
	    {
		fire_effect(vch,level/2,dam/4,TARGET_CHAR);
		damage(ch,vch,dam/2,sn,DAM_FIRE,TRUE);
	    }
	    else
	    {
		fire_effect(vch,level,dam,TARGET_CHAR);
		damage(ch,vch,dam,sn,DAM_FIRE,TRUE);
	    }
	}
	else /* partial damage */
	{
	    if (saves_spell(level - 2,vch,DAM_FIRE))
	    {
		fire_effect(vch,level/4,dam/8,TARGET_CHAR);
		damage(ch,vch,dam/4,sn,DAM_FIRE,TRUE);
	    }
	    else
	    {
		fire_effect(vch,level/2,dam/4,TARGET_CHAR);
		damage(ch,vch,dam/2,sn,DAM_FIRE,TRUE);
	    }
	}
    }
}

void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch, *vch_next;
    int dam,hp_dam,dice_dam, hpch;

    act("$n breathes out a freezing cone of frost!",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a freezing cone of frost over you!",
	ch,NULL,victim,TO_VICT);
    act("You breath out a cone of frost.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX(12,ch->hit);
    hp_dam = number_range(hpch/11 + 1, hpch/6);
    dice_dam = dice(level,16);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);
    cold_effect(victim->in_room,level,dam/2,TARGET_ROOM);

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch,vch,TRUE)
	||  (IS_NPC(vch) && IS_NPC(ch)
	&&   (ch->fighting != vch || vch->fighting != ch)))
	    continue;

	if (vch == victim) /* full damage */
	{
	    if (saves_spell(level,vch,DAM_COLD))
	    {
		cold_effect(vch,level/2,dam/4,TARGET_CHAR);
		damage(ch,vch,dam/2,sn,DAM_COLD,TRUE);
	    }
	    else
	    {
		cold_effect(vch,level,dam,TARGET_CHAR);
		damage(ch,vch,dam,sn,DAM_COLD,TRUE);
	    }
	}
	else
	{
	    if (saves_spell(level - 2,vch,DAM_COLD))
	    {
		cold_effect(vch,level/4,dam/8,TARGET_CHAR);
		damage(ch,vch,dam/4,sn,DAM_COLD,TRUE);
	    }
	    else
	    {
		cold_effect(vch,level/2,dam/4,TARGET_CHAR);
		damage(ch,vch,dam/2,sn,DAM_COLD,TRUE);
	    }
	}
    }
}


void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam,hp_dam,dice_dam,hpch;

    act("$n breathes out a cloud of poisonous gas!",ch,NULL,NULL,TO_ROOM);
    act("You breath out a cloud of poisonous gas.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX(16,ch->hit);
    hp_dam = number_range(hpch/15+1,8);
    dice_dam = dice(level,12);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);
    poison_effect(ch->in_room,level,dam,TARGET_ROOM);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch,vch,TRUE)
	||  (IS_NPC(ch) && IS_NPC(vch)
	&&   (ch->fighting == vch || vch->fighting == ch)))
	    continue;

	if (saves_spell(level,vch,DAM_POISON))
	{
	    poison_effect(vch,level/2,dam/4,TARGET_CHAR);
	    damage(ch,vch,dam/2,sn,DAM_POISON,TRUE);
	}
	else
	{
	    poison_effect(vch,level,dam,TARGET_CHAR);
	    damage(ch,vch,dam,sn,DAM_POISON,TRUE);
	}
    }
}

void spell_lightning_breath(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam,hp_dam,dice_dam,hpch;

    act("$n breathes a bolt of lightning at $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a bolt of lightning at you!",ch,NULL,victim,TO_VICT);
    act("You breathe a bolt of lightning at $N.",ch,NULL,victim,TO_CHAR);

    hpch = UMAX(10,ch->hit);
    hp_dam = number_range(hpch/9+1,hpch/5);
    dice_dam = dice(level,20);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);

    if (saves_spell(level,victim,DAM_LIGHTNING))
    {
	shock_effect(victim,level/2,dam/4,TARGET_CHAR);
	damage(ch,victim,dam/2,sn,DAM_LIGHTNING,TRUE);
    }
    else
    {
	shock_effect(victim,level,dam,TARGET_CHAR);
	damage(ch,victim,dam,sn,DAM_LIGHTNING,TRUE);
    }
}

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
void spell_general_purpose(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = number_range( 25, 100 );
    if ( saves_spell( level, victim, DAM_PIERCE) )
        dam /= 2;
    damage( ch, victim, dam, sn, DAM_PIERCE ,TRUE);
    return;
}

void spell_high_explosive(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = number_range( 30, 120 );
    if ( saves_spell( level, victim, DAM_PIERCE) )
        dam /= 2;
    damage( ch, victim, dam, sn, DAM_PIERCE ,TRUE);
    return;
}

void spell_channel( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
    {
        if (victim == ch)
          send_to_char("You are already channeling maximum power.\n\r",ch);
        else
          act("$N is already channeling maximum power.",ch,NULL,victim,TO_CHAR);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 8;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act( "$n channeles maximum power.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You channel maximum power.\n\r", victim );
    return;
}

void
spell_shadowstep (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
        ROOM_INDEX_DATA *from_room;

        from_room = ch->in_room;

    if ((victim = get_char_world (ch, target_name)) == NULL || victim == ch
            || victim->in_room == NULL
            || IS_SET (victim->in_room->room_flags, ROOM_SAFE)
            || IS_SET (victim->in_room->room_flags, ROOM_PRIVATE)
            || IS_SET (victim->in_room->room_flags, ROOM_SOLITARY)
            || IS_SET (victim->in_room->room_flags, ROOM_NO_RECALL)
                    || IS_SET (victim->in_room->room_flags, ROOM_NO_OUT)
                    || IS_SET (from_room->room_flags, ROOM_NO_OUT)
                    || victim->fighting != NULL
                    || ( IS_NPC( victim ) && saves_spell( level, victim, DAM_OTHER ) ) )
    {
        send_to_char ("Your spell fizzles.\n\r", ch);
        return;
    }

    act ("$n steps into the shadows and vanishes.", ch, NULL, NULL, TO_ROOM);
    char_from_room (ch);
    char_to_room (ch, victim->in_room);
    act ("$n steps out of a nearby shadow.", ch, NULL, NULL, TO_ROOM);
        act ("You travel through the shadows.", ch, NULL, NULL, TO_CHAR);
    do_look (ch, "auto");
    return;
}

void
spell_hellfire(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        damage(ch, victim, dice(level, 7), sn, DAM_FIRE, TRUE);
}

void spell_acid_arrow(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 12);
        if (saves_spell(level, victim, DAM_ACID))
                dam /= 2;
        damage(ch, victim, dam, sn,DAM_ACID,TRUE);
}

/* energy spells */
void spell_rushing_fist(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 12);
        if (saves_spell(level, victim, DAM_BASH))
                dam /= 2;
        act("A fist of black, otherworldly force rams into $N, leaving $M looking stunned!"
                        ,ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_BASH,TRUE);
}

void spell_telekinetic_force(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 8);
        if (saves_spell(level, victim, DAM_ENERGY))
                dam /= 2;
        act("$N is slammed with telekinetic force!",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_ENERGY,TRUE);
}

void spell_disruption(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 9);
        if (saves_spell(level, victim, DAM_ENERGY))
                dam /= 2;
        act("A weird energy encompasses $N, causing you to question $S continued existence.",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_ENERGY,TRUE);
}


void spell_banshee_blast(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 7);
        if (saves_spell(level, victim, DAM_ENERGY))
                dam /= 2;
        act("A wave of screaming sonic energy enshrouds $N causing $S to grip their ears in pain.",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_ENERGY,TRUE);
}
/* mental */
void spell_daze(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 7);
        if (saves_spell(level, victim, DAM_MENTAL))
                dam /= 2;
        act("$n stares intently at $N, causing $N to seem very lethargic.",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_MENTAL,TRUE);
}

void spell_damnation(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 9);
        if (saves_spell(level, victim, DAM_MENTAL))
                dam /= 2;
        act("$n stares intently at $N, causing $N to see images of their own damnation.",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_MENTAL,TRUE);
}

/* acid */
void spell_acid_geyser(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 7);
        if (saves_spell(level, victim, DAM_ACID))
                dam /= 2;
        act("A geyser of acid erupts from the ground, spraying $N." ,
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_ACID,TRUE);
}

void spell_caustic_eruption(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 9);
        if (saves_spell(level, victim, DAM_ACID))
                dam /= 2;
        act("A fountain of caustic liquid forms below $N and $S body begins smoking and hissing! ",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_ACID,TRUE);
}
void spell_corrosive_burst(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 8);
        if (saves_spell(level, victim, DAM_ACID))
                dam /= 2;
        act("Corrosive mist enshrouds $N, and begins eating away at their gear. ",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_ACID,TRUE);
}

/*  Electrical  */

void spell_ion_blast(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 7);
        if (saves_spell(level, victim, DAM_LIGHTNING))
                dam /= 2;
        act("$n launches a bolt of ionized particles at $N.",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_LIGHTNING,TRUE);
}

void spell_celestial_lightning(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 8);
        if (saves_spell(level, victim, DAM_LIGHTNING))
                dam /= 2;
        act("Celestial lightning strikes $N!",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_LIGHTNING,TRUE);
}

void spell_electrocute(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 9);
        if (saves_spell(level, victim, DAM_LIGHTNING))
                dam /= 2;
        act("$N is violently electrocuted.",
                        ch,NULL,victim,TO_NOTVICT);
        damage(ch, victim, dam, sn,DAM_LIGHTNING,TRUE);
}

/* negative */

static inline void
astral_walk(CHAR_DATA *ch, CHAR_DATA *victim)
{
        transfer_char(ch, victim, victim->in_room,
                      "$N disappears in a flash of light!",
                      "You travel via astral planes and go to $n.",
                      "$N appears in a flash of light!");
}

/* travel via astral plains */
void spell_astral_walk(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
        CHAR_DATA *victim;
        CHAR_DATA *pet = NULL;

        if ((victim = get_char_world(ch, target_name)) == NULL
        ||  victim->level >= level + 3
        ||  saves_spell(level, victim, DAM_OTHER)
        ||  !can_gate(ch, victim)) {
                send_to_char("You failed.\n", ch);
                return;
        }

        if (ch->pet && ch->in_room == ch->pet->in_room)
                pet = ch->pet;

        astral_walk(ch, victim);
        if (pet && !IS_AFFECTED(pet, AFF_SLEEP)) {
                if (pet->position != POS_STANDING)
                        do_stand(pet, str_empty);
                astral_walk(ch, victim);
        }
}

void spell_hurricane(int sn,int level,CHAR_DATA *ch,void *vo, int target)
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int dam,hp_dam,dice_dam,hpch;

        act("$n prays to Thor for a mighty storm.",ch,NULL,NULL,TO_NOTVICT);
        act("You pray to Thor for a mighty storm.",ch,NULL,NULL,TO_CHAR);

        hpch = UMAX(16,ch->hit);
        hp_dam = number_range(hpch/15+1,8);
        dice_dam = dice(level,12);

        dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);

        for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
        {
                vch_next = vch->next_in_room;

                if (is_safe_spell(ch,vch,TRUE)
                ||  (IS_NPC(ch) && IS_NPC(vch)
                &&   (ch->fighting == vch || vch->fighting == ch)))
                    continue;

                if (!IS_AFFECTED(vch,AFF_FLYING)) dam /= 2;

                if (vch->size == SIZE_TINY)  dam *= 1.5;
                else if (vch->size == SIZE_SMALL)  dam *= 1.3;
                else if (vch->size == SIZE_MEDIUM)  dam *= 1;
                else if (vch->size == SIZE_LARGE)  dam *= 0.9;
                else if (vch->size == SIZE_HUGE)  dam *= 0.7;
                else dam *= 0.5;

                if (saves_spell(level,vch,DAM_OTHER))
                    damage(ch,vch,dam/2,sn,DAM_OTHER,TRUE);
                else
                    damage(ch,vch,dam,sn,DAM_OTHER,TRUE);
        }
}

void spell_dragonscales( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already coverd in dragon scales.\n\r",ch);
	else
	  act("$N is already covered in dragon scales.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 24;
    af.modifier  = -20;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel dragon scales painfully burst forth from your skin!\n\r", victim );
    if ( ch != victim )
	act("$N howls in pain as dragon scales burst forth from their skin!",ch,NULL,victim,TO_CHAR);
    return;
}

void spell_natural_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
          send_to_char("You are already naturally armored.\n\r",ch);
	else
          act("$N is already naturally armored.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 24;
    af.modifier  = -20;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You body becomes more armored.\n\r", victim );
    if ( ch != victim )
        act("$N is harder to damage.",ch,NULL,victim,TO_CHAR);
    return;
}

void spell_strike_true( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    victim = (CHAR_DATA *) vo;


    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
    {
	if (victim == ch)
          send_to_char("You already strike true.\n\r",ch);
	else
          act("$N already strikes true.",ch,NULL,victim,TO_CHAR);
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

    send_to_char( "You feel like your blows will be more accurate.\n\r", victim );
    if ( ch != victim )
        act("You grant $N combat accuracy.",ch,NULL,victim,TO_CHAR);
    return;
}

void spell_spellmantle( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    victim = (CHAR_DATA *) vo;


    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
    {
	if (victim == ch)
          send_to_char("You already have a spellmantle.\n\r",ch);
	else
          act("$N already has a spellmantle.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 6+level;
    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 8;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "You feel protected against magic.\n\r", victim );
    if ( ch != victim )
        act("You grant $N protection against magic.",ch,NULL,victim,TO_CHAR);
    return;
}

void spell_project_force(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *tmp_vict,*last_vict,*next_vict;
    bool found;
    int dam;

    /* first strike */

    act("$n shouts and unleashes a wave of sonic force at $N.",
        ch,NULL,victim,TO_ROOM);
    act("The sonic wave expands to strike $N.",
	ch,NULL,victim,TO_CHAR);
    act("$n lets out a deafening shout and their sonic wave hits you!",
	ch,NULL,victim,TO_VICT);

    dam = dice(level,6);
    if (saves_spell(level,victim,DAM_SOUND))
 	dam /= 3;
    damage(ch,victim,dam,sn,DAM_SOUND,TRUE);
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
            act("The sonic wave expands to strike $n!",tmp_vict,NULL,NULL,TO_ROOM);
            act("The sonic wave expands to strike you!",tmp_vict,NULL,NULL,TO_CHAR);
	    dam = dice(level,6);
	    if (saves_spell(level,tmp_vict,DAM_SOUND))
		dam /= 3;
	    damage(ch,tmp_vict,dam,sn,DAM_SOUND,TRUE);
	    level -= 4;  /* decrement damage */
	  }
	}   /* end target searching loop */

	if (!found) /* no target found, hit the caster */
	{
	  if (ch == NULL)
     	    return;

	  if (last_vict == ch) /* no double hits */
	  {
            act("The sonic wave seems to die out.",ch,NULL,NULL,TO_ROOM);
            act("The sonic wave seems to die out.",
		ch,NULL,NULL,TO_CHAR);
	    return;
	  }

	  last_vict = ch;
          act("The sonic wave bounces back and strikes $n!",ch,NULL,NULL,TO_ROOM);
          send_to_char("You are struck by your own sonic wave as it reflects back at you!\n\r",ch);
	  dam = dice(level,6);
	  if (saves_spell(level,ch,DAM_SOUND))
	    dam /= 3;
	  damage(ch,ch,dam,sn,DAM_SOUND,TRUE);
	  level -= 4;  /* decrement damage */
	  if (ch == NULL)
	    return;
	}
    /* now go back and find more targets */
    }
}

void spell_aurasight( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_GOOD) )
    {
        if (victim == ch)
          send_to_char("Your aura sight is on already.\n\r",ch);
        else
          act("$N can already see auras.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_GOOD;
    affect_to_char( victim, &af );

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_EVIL;

    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_life_detect(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_HIDDEN) )
    {
        if (victim == ch)
          send_to_char("You can already detect life. \n\r",ch);
        else
          act("$N can already detect life.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );
    send_to_char( "You can now detect all life.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_truesight(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_HIDDEN) )
    {
        if (victim == ch)
          send_to_char("Your already see all. \n\r",ch);
        else
          act("$N can see everything.",ch,NULL,victim,TO_CHAR);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_GOOD;
    affect_to_char( victim, &af );

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_EVIL;

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_INVIS;

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char( victim, &af );

    af.where = TO_AFFECTS; 
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_INFRARED;
    affect_to_char( victim, &af );
  send_to_char( "Nothing can escape your gaze.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

void 
spell_full_heal( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

 int gain = victim->max_hit - victim->hit;
     
    victim->hit = UMIN(victim->max_hit, victim->hit + gain);
    send_to_char( "Warmth spreads through your body as your `Rwounds`X are fully healed.\n\r", victim );
    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    return;
}

void
spell_bears_endurance(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already as enduring as a bear!\n\r",ch);
	else
	  act("$N can't get any more enduring.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_CON;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "Your body becomes more resilient!\n\r", victim );
    act("$n's becomes more resilient.",victim,NULL,NULL,TO_ROOM);
    return;
}

void
spell_bulls_strength(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already as strong as you can get!\n\r",ch);
	else
	  act("$N can't get any stronger.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "Your feel stronger than a bull!\n\r", victim );
    act("$n's channels the strength of a bull.",victim,NULL,NULL,TO_ROOM);
    return;
}

void
spell_cats_grace(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already as graceful as you can be!\n\r",ch);
	else
	  act("$N can't get any more graceful.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_DEX;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You become as graceful as a cat!\n\r", victim );
    act("$n's becomes as graceful as a cat.",victim,NULL,NULL,TO_ROOM);
    return;
}

void
spell_fox_cunning(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You cannot be any more cunning!\n\r",ch);
	else
	  act("$N is already as cunning as they can be.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_INT;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel your mind sharpen with the cunning of the fox!\n\r", victim );
    act("$n's eyes take on a more cunning gaze.",victim,NULL,NULL,TO_ROOM);
    return;
}

void
spell_owls_wisdom(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already as wise as you can get!\n\r",ch);
	else
	  act("$N can't get any wiser.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_WIS;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel your wisdom deepen!\n\r", victim );
    act("$n gives a sagely nod.",victim,NULL,NULL,TO_ROOM);
    return;
}



/* Necros use this to keep body parts longer...for lesser elementals */
void spell_preserve(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int vnum, chance;

    vnum = obj->pIndexData->vnum;
	if (vnum != OBJ_VNUM_SEVERED_HEAD
	&& vnum != OBJ_VNUM_TORN_HEART
	&& vnum != OBJ_VNUM_SLICED_ARM
	&& vnum != OBJ_VNUM_SLICED_LEG
	&& vnum != OBJ_VNUM_GUTS
	&& vnum != OBJ_VNUM_BRAINS)
	{
    send_to_char("You can't preserve that.\n\r",ch);
	return;
    }

    if (obj->timer > 10)
    {
        send_to_char("It's already in very well preserved condition.\n\r",ch);
        return;
    }
    chance = get_skill(ch,sn);
    chance *= 90;

    if (number_percent() > chance)
    {
        act("$n destroys $p.",ch,obj,NULL,TO_ROOM);
        act("You fail and destroy $p.",ch,obj,NULL,TO_CHAR);
        extract_obj(obj);
        return;
    }

    act("You surround $p with necromantic magic to slow it's decay.",ch,obj,NULL,TO_CHAR);
    obj->timer += number_range(level/2,level);
    return;
}



/* The summon elemental spells for elementalists. Fun. */


void spell_find_familiar(int sn, int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *familiar;
    CHAR_DATA *	mob;
    AFFECT_DATA af;
    int count;
    int i = 0;
    CHAR_DATA *check;
        MOB_INDEX_DATA *pMobIndex;

    if (is_affected(ch,sn))
    {
        send_to_char("You can't call upon a familiar yet.\n\r",ch);
        return;
    }
    count = 0;

    for (check = char_list; check != NULL; check = check->next)
    {
    if (IS_NPC(check))
        if ( (check->master == ch)
        && ( (check->pIndexData->vnum == MOB_VNUM_FAMILIAR)
          || (check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL)
          || (check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL)
          || (check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL) ) )
            count++;
    }
    if ( (count == 1 && ch->level < 30) || (count == 2 && ch->level < 37)
      || (count == 3) )
    {
        send_to_char("You already control as many familiars as you can.\n\r",ch);
        return;
    }
   if(ch->position == POS_FIGHTING)
   {
      send_to_char("You can't study the ritual while in combat!\n\r",ch);
      return;
   }
    
   if ( ( pMobIndex = get_mob_index(MOB_VNUM_FAMILIAR) ) == NULL )
   {
       send_to_char( "The familiar mob doesn't exist.\n\r", ch );
       return;
   }
    /* can't cast the spell in these sectors */
    if(ch->in_room->sector_type == SECT_INSIDE
    || ch->in_room->sector_type == SECT_WATER_SWIM 
    || ch->in_room->sector_type == SECT_WATER_NOSWIM
    || ch->in_room->sector_type == SECT_AIR )
    {
       send_to_char("You are feeling too sick to concentrate.\n\r",ch);
       return;
    }
     	

    familiar = create_mobile(get_mob_index(MOB_VNUM_FAMILIAR) );
      for (i = 0; i < MAX_STATS; i ++)
       familiar->perm_stat[i] = UMIN(25,15 + ch->level/10);
            
  familiar->perm_stat[STAT_STR] += 3;
  familiar->perm_stat[STAT_INT] -= 1;
  familiar->perm_stat[STAT_CON] += 2;
    familiar->level = level;
    familiar->max_hit = ch->max_hit + dice(level,10);
    familiar->hit = familiar->max_hit;
    familiar->damroll += level/2;
    familiar->alignment = ch->alignment;
      familiar->damage[DICE_NUMBER] = 13;   
  familiar->damage[DICE_TYPE] = 9;
  familiar->damage[DICE_BONUS] = ch->level / 2 + 10;
  /* free up the old mob names */ 
    free_string(familiar->description);
    free_string(familiar->name);
    free_string(familiar->short_descr);
    free_string(familiar->long_descr);
    
    /* terrain */
    switch(ch->in_room->sector_type)
    {
	case(SECT_CITY): /* rat */
	case(SECT_FIELD):
	    familiar->description = 
	          str_dup("You see a large furry rat.  Long whiskers hang down from it's nose.\n\r"
	                  "You can feel the dirt and disease crawling off this beast.\n\r");
	    familiar->short_descr = str_dup("large rat");
	    familiar->long_descr = str_dup("A large furry rodent is here.\n\r");
	    familiar->name = str_dup("familiar rat");
	    familiar->dam_type = 22; /* scratch */
        familiar->race =(race_lookup("rat"));
	    break;
	case(SECT_FOREST):
	familiar->description = 
	          str_dup("You see a large brown bear.  Thick fur and a tough hide cover\n\r"
	                  "its body, as it stretches and shows that beneath that hide is solid\n\r"
	                  "muscle.\n\r");
	    familiar->short_descr = str_dup("large brown bear");
	    familiar->long_descr = str_dup("A large brown bear is here.\n\r");
	    familiar->name = str_dup("familiar brown bear");
	    familiar->dam_type = 5; /* claw */
        familiar->race =(race_lookup("bear"));
	    break;  /* falcon */
	case(SECT_HILLS):
	    familiar->description = 
	          str_dup("You see a large falcon.  Golden brown feathers frame powerful\n\r"
	                  "wings.  Long talons grasp at nothingness in vain attempts at\n\r"
	                  "getting some rabbit or rodent for dinner.\n\r");
	    familiar->short_descr = str_dup("large falcon");
	    familiar->long_descr = str_dup("A large falcon screams here.\n\r");
	    familiar->name = str_dup("familiar falcon");
	    familiar->dam_type = 5; /* claw */
        familiar->race =(race_lookup("falcon"));
	    break;
	case(SECT_MOUNTAIN): /* familiarain lion */
	    familiar->description = 
	          str_dup("You see a very large familiarain lion.  One wrong look and it could\n\r"
	                  "have your head lying at your feet.  You should think better than\n\r"
	                  "cross this beast especial if you have a weapon in your hand.\n\r");
	    familiar->short_descr = str_dup("large familiarain lion");
	    familiar->long_descr = str_dup("A large familiarain lion claws the ground here.\n\r");
	    familiar->name = str_dup("familiar familiarain lion");
	    familiar->dam_type = 10; /* bite */
        familiar->race =(race_lookup("lion"));
	    break;
	case(SECT_DESERT): /* sandworm */
	    familiar->description = 
	          str_dup("You see a large white sandworm wiggling in the light.\n\r"
	                  "A red spot on one end makes you guess it is a mouth.\n\r"
	                  "A loud moan comes from the direction of that red spot.\n\r");
	    familiar->short_descr = str_dup("sandworm");
	    familiar->long_descr = str_dup("A white sandworm wiggles on the ground here.\n\r");
	    familiar->name = str_dup("familiar sandworm");
	    familiar->dam_type = 12; /* suction */
        familiar->race =(race_lookup("sandworm"));
	    break;
    }
   familiar->master = ch;
    familiar->leader = ch;
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("charm person");
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    //affect_to_char( mob, &af );
    affect_to_char( familiar, &af );
  char_to_room(familiar,ch->in_room);
    act( "You begin to chant and call to a $N!",ch,NULL,familiar,TO_CHAR);
    act( "$n begins to chant and calls to a $N!", ch, NULL, familiar, TO_ROOM );
    //add_follower(familiar,ch);
 return;
}
