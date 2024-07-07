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


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"

/*
 * Local functions.
 */
static void	expand_greet_table( void );
static int	new_greet_id( void );
static bool	event_save_greet( EVENT_DATA *pEvent );


/* friend stuff -- for NPC's mostly */
bool
is_friend( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( is_same_group( ch, victim ) )
	return TRUE;

    if ( !IS_NPC( ch ) )
	return FALSE;

    if ( !IS_NPC( victim ) )
    {
	if ( xIS_SET( ch->off_flags, ASSIST_PLAYERS ) )
	    return TRUE;
	else
	    return FALSE;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
	return FALSE;

    if ( xIS_SET( ch->off_flags, ASSIST_ALL ) )
	return TRUE;

    if ( ch->group && ch->group == victim->group )
	return TRUE;

    if ( xIS_SET( ch->off_flags, ASSIST_VNUM ) 
    &&  ch->pIndexData == victim->pIndexData )
	return TRUE;

    if ( xIS_SET( ch->off_flags, ASSIST_RACE ) && ch->race == victim->race )
	return TRUE;

    if ( xIS_SET( ch->off_flags, ASSIST_ALIGN )
    &&  !IS_SET( ch->act, ACT_NOALIGN ) && !IS_SET( victim->act, ACT_NOALIGN )
    &&  ( ( IS_GOOD( ch ) && IS_GOOD( victim ) )
    ||	 ( IS_EVIL( ch ) && IS_EVIL( victim ) )
    ||   ( IS_NEUTRAL( ch ) && IS_NEUTRAL( victim ) ) ) )
	return TRUE;

    return FALSE;
}


/* returns number of lines in a string */
int
count_lines( const char *str )
{
    int n;

    if ( IS_NULLSTR( str ) )
        return 0;

    n = 0;
    while ( *str != '\0' )
    {
        if ( *str == '\n' )
            n++;
        str++;
    }

    return n;
}


/* Returns the number of pages in a book. */
int
count_pages( OBJ_INDEX_DATA *book )
{
    int		count;
    TEXT_DATA *	page;

    if ( book->item_type != ITEM_BOOK )
        return 0;

    count = 0;
    for ( page = book->page; page != NULL; page = page->next )
        count++;

    return count;
}


/* returns number of people on an object */
int
count_users(OBJ_DATA *obj)
{
    CHAR_DATA *fch;
    int count = 0;

    if (obj->in_room == NULL)
	return 0;

    for (fch = obj->in_room->people; fch != NULL; fch = fch->next_in_room)
	if (fch->on == obj)
	    count++;

    return count;
}


int
baseclass( CHAR_DATA *ch )
{
    int		base;
    int		i;

    if ( IS_NPC( ch ) )
        return NO_CLASS;

    base = class_table[ch->class].base_class;
    for ( i = 0; i < MAX_CLASS; i++ )
        if ( class_table[i].base_class == base && class_table[i].tier == TIER_ONE )
            return i;

    return NO_CLASS;
}


const char *
baseclassname( int iClass )
{
    int base;

    base = class_table[iClass].base_class;
    switch( base )
    {
	case BASE_CLERIC:	return "Cleric";
	case BASE_MAGE:		return "Mage";
	case BASE_ROGUE:	return "Rogue";
	case BASE_FIGHTER:	return "Fighter";
	default:		return "None";
    }
}


/* Returns TRUE if ch knows vch */
bool
knows_char( CHAR_DATA *ch, CHAR_DATA *vch )
{
    if ( ( IS_NPC( ch ) || IS_NPC( vch ) )
    ||	 ( ( IS_IMMORTAL( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) ) || IS_IMMORTAL( vch ) )
    ||	 ( ch == vch ) )
	return TRUE;

    /*
     * REAL quick hack for now, return TRUE if ch is the same race or class
     * as vch else return FALSE.
     * Will rewrite this when design of actual "knows" mechanism is complete.
    if ( ch->race == vch->race || ch->class == vch->class )
        return TRUE;
*/

    if ( ch->pcdata->greet_id == 0 || vch->pcdata->greet_id == 0 )
	return FALSE;

    return *( greet_table + ch->pcdata->greet_id * greet_size + vch->pcdata->greet_id );
}


/* returns material number */
int
material_lookup (const char *name)
{
    return 0;
}


/* returns race number */
int
race_lookup (const char *name)
{
   int race;

   for ( race = 0; race_table[race].name != NULL; race++)
   {
	if (LOWER(name[0]) == LOWER(race_table[race].name[0])
	&&  !str_prefix( name,race_table[race].name))
	    return race;
   }

   return 0;
} 


int
liq_lookup (const char *name)
{
    int liq;

    for ( liq = 0; liq_table[liq].liq_name != NULL; liq++)
    {
	if (LOWER(name[0]) == LOWER(liq_table[liq].liq_name[0])
	&& !str_prefix(name,liq_table[liq].liq_name))
	    return liq;
    }

    return -1;
}


int
weapon_lookup (const char *name)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
    {
	if (LOWER(name[0]) == LOWER(weapon_table[type].name[0])
	&&  !str_prefix(name,weapon_table[type].name))
	    return type;
    }

    return -1;
}


int
weapon_type (const char *name)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
    {
        if (LOWER(name[0]) == LOWER(weapon_table[type].name[0])
        &&  !str_prefix(name,weapon_table[type].name))
            return weapon_table[type].type;
    }

    return WEAPON_EXOTIC;
}


char *
weapon_name( int weapon_type)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
        if (weapon_type == weapon_table[type].type)
            return weapon_table[type].name;
    return "exotic";
}


int
attack_lookup  (const char *name)
{
    int att;

    for ( att = 0; attack_table[att].name != NULL; att++)
    {
	if (LOWER(name[0]) == LOWER(attack_table[att].name[0])
	&&  !str_prefix(name,attack_table[att].name))
	    return att;
    }

    return 0;
}

/* returns a flag for the info channel */
int
info_lookup( const char *name )
{
    int indx;

    for ( indx = 0; info_table[indx].name != NULL; indx++ )
    {
	if ( LOWER( name[0] ) == LOWER( info_table[indx].name[0] )
	&&   !str_prefix( name, info_table[indx].name ) )
	    return indx;
    }

    return NO_VALUE;
}


/* returns the name of a landmass */
const char *
landmass_name( int land )
{
    int	index;

    for ( index = 0; index < LAND_MAX; index++ )
        if ( land_name_table[index].land == land )
            return land_name_table[index].name;

    return "Nowhere!";
}


/* returns a flag for wiznet */
int
wiznet_lookup (const char *name)
{
    int flag;

    for (flag = 0; wiznet_table[flag].name != NULL; flag++)
    {
	if (LOWER(name[0]) == LOWER(wiznet_table[flag].name[0])
	&& !str_prefix(name,wiznet_table[flag].name))
	    return flag;
    }

    return -1;
}


/* returns class number */
int
class_lookup (const char *name)
{
    int class;

    if ( name == NULL || *name == '\0' )
	return NO_CLASS;

    for ( class = 0; class < MAX_CLASS; class++)
    {
	if ( LOWER( name[0] ) == LOWER( class_table[class].name[0] )
		&&  !str_prefix( name,class_table[class].name ) )
	    return class;
    }

    for ( class = 0; class < MAX_CLASS; class++ )
    {
	if ( !str_prefix( name, class_table[class].who_name ) )
	    return class;
    }

    return NO_CLASS;
}

/* for immunity, vulnerabiltiy, and resistant
   the 'globals' (magic and weapons) may be overriden
   three other cases -- wood, silver, and iron -- are checked in fight.c */

int
check_immune(CHAR_DATA *ch, int dam_type)
{
    int immune, def;
    int bit;

    immune = -1;
    def = IS_NORMAL;

    if (dam_type == DAM_NONE)
	return immune;

    if (dam_type <= 3)
    {
	if (IS_SET(ch->imm_flags,IMM_WEAPON))
	    def = IS_IMMUNE;
	else if (IS_SET(ch->res_flags,RES_WEAPON))
	    def = IS_RESISTANT;
	else if (IS_SET(ch->vuln_flags,VULN_WEAPON))
	    def = IS_VULNERABLE;
    }
    else /* magical attack */
    {	
	if (IS_SET(ch->imm_flags,IMM_MAGIC))
	    def = IS_IMMUNE;
	else if (IS_SET(ch->res_flags,RES_MAGIC))
	    def = IS_RESISTANT;
	else if (IS_SET(ch->vuln_flags,VULN_MAGIC))
	    def = IS_VULNERABLE;
    }

    /* set bits to check -- VULN etc. must ALL be the same or this will fail */
    switch (dam_type)
    {
	case(DAM_BASH):		bit = IMM_BASH;		break;
	case(DAM_PIERCE):	bit = IMM_PIERCE;	break;
	case(DAM_SLASH):	bit = IMM_SLASH;	break;
	case(DAM_FIRE):		bit = IMM_FIRE;		break;
	case(DAM_COLD):		bit = IMM_COLD;		break;
	case(DAM_LIGHTNING):	bit = IMM_LIGHTNING;	break;
	case(DAM_ACID):		bit = IMM_ACID;		break;
	case(DAM_POISON):	bit = IMM_POISON;	break;
	case(DAM_NEGATIVE):	bit = IMM_NEGATIVE;	break;
	case(DAM_HOLY):		bit = IMM_HOLY;		break;
	case(DAM_ENERGY):	bit = IMM_ENERGY;	break;
	case(DAM_MENTAL):	bit = IMM_MENTAL;	break;
	case(DAM_DISEASE):	bit = IMM_DISEASE;	break;
	case(DAM_DROWNING):	bit = IMM_DROWNING;	break;
	case(DAM_LIGHT):	bit = IMM_LIGHT;	break;
	case(DAM_CHARM):	bit = IMM_CHARM;	break;
	case(DAM_SOUND):	bit = IMM_SOUND;	break;
	default:		return def;
    }

    if (IS_SET(ch->imm_flags,bit))
	immune = IS_IMMUNE;
    else if (IS_SET(ch->res_flags,bit) && immune != IS_IMMUNE)
	immune = IS_RESISTANT;
    else if (IS_SET(ch->vuln_flags,bit))
    {
	if (immune == IS_IMMUNE)
	    immune = IS_RESISTANT;
	else if (immune == IS_RESISTANT)
	    immune = IS_NORMAL;
	else
	    immune = IS_VULNERABLE;
    }

    if (immune == -1)
	return def;
    else
      	return immune;
}


bool
check_nightmare( CHAR_DATA *ch )
{
    DREAM_DATA *	pDream;
    EVENT_DATA *	pEvent;

    if ( ( pEvent = get_event_char( ch, EVENT_CHAR_DREAM ) ) == NULL )
        return FALSE;
    if ( ( pDream = get_dream_index( pEvent->value0 ) ) == NULL )
        return FALSE;
    if ( pDream->type != DREAM_NIGHTMARE )
        return FALSE;

    return TRUE;
}


/*
 * Insert a room into an area's list of rooms.
 */
void
insert_room( ROOM_INDEX_DATA *room )
{
    ROOM_INDEX_DATA *	prev;

    if ( room->area->room_list == NULL )
    {
        room->area->room_list = room;
        return;
    }

    if ( room->area->room_list->vnum > room->vnum )
    {
        room->next_in_area = room->area->room_list;
        room->area->room_list = room;
        return;
    }

    for ( prev = room->area->room_list; prev->next_in_area != NULL; prev = prev->next_in_area )
    {
        if ( room->vnum < prev->next_in_area->vnum )
            break;
    }
    room->next_in_area = prev->next_in_area;
    prev->next_in_area = room;

    return;
}


bool
is_clan( CHAR_DATA *ch )
{
    return ch->clan != NULL;
}


bool
is_same_clan( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->clan && victim->clan && ch->clan == victim->clan )
	return TRUE;
    else
	return FALSE;
}


/*
 * Returns pointer to the area with the given vnum, which is allocated the
 * given vnum, or has the given string as part of its name.
 */
AREA_DATA *
find_area( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *	pArea;
    int		vnum;

    if ( IS_NULLSTR( argument ) )
	return NULL;

    if ( !str_cmp( argument, "." ) )
    {
	if ( ch == NULL || ch->in_room == NULL )
	    return NULL;
	return ch->in_room->area;
    }

    if ( is_number( argument ) )
    {
	vnum = atoi( argument );
	if ( vnum < 0 )
	    return NULL;

	if ( vnum < top_area )
	{
	    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
		if ( pArea->vnum == vnum )
		    return pArea;
	}

	for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
	    if ( pArea->min_vnum <= vnum && pArea->max_vnum >= vnum )
		break;
	return pArea;

    }

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
	if ( is_name( argument, pArea->name ) )
	    break;

    return pArea;
}


/*
 *  Returns pointer to the area which contains a mob/obj/room vnum, or NULL
 *  Vnum need not be assigned, just allocated to the area.
 */
AREA_DATA *
get_area( int vnum )
{
    AREA_DATA *pArea;

    /*
     * just in case we have a freshly created area that doesn't have vnums yet
     */
    if ( vnum <= 0 )
	return NULL;

    for ( pArea = area_first; pArea; pArea = pArea->next )
	if ( pArea->min_vnum <= vnum && pArea->max_vnum >= vnum )
	    break;

    return pArea;
}

/* for returning skill information */
int
get_skill( CHAR_DATA *ch, int sn )
{
    int skill;

    if ( sn == -1 ) /* shorthand for level based skills */
    {
	skill = ch->level * 5 / 2;
    }

    else if ( sn < -1 || sn >= top_skill )
    {
	bug( "Bad sn %d in get_skill.", sn );
	skill = 0;
    }

    else if ( !IS_NPC( ch ) )
    {
	if ( ch->level < skill_table[sn].skill_level[ch->class] )
	    skill = 0;
	else
	    skill = ch->pcdata->skill[sn].percent;
    }

    else /* mobiles */
    {

        if ( skill_table[sn].spell_fun != spell_null )
	    skill = 40 + 2 * ch->level;

	else if ( sn == gsn_sneak || sn == gsn_hide )
	    skill = ch->level * 2 + 20;

        else if ( ( sn == gsn_dodge && xIS_SET( ch->off_flags, OFF_DODGE ) )
 	||        ( sn == gsn_parry && xIS_SET( ch->off_flags, OFF_PARRY ) ) )
	    skill = ch->level * 2;

 	else if (sn == gsn_shield_block)
	    skill = 10 + 2 * ch->level;

	else if ( sn == gsn_second_attack 
	&& ( IS_SET( ch->pIndexData->class, MCLASS_FIGHTER )
	|| IS_SET( ch->pIndexData->class, MCLASS_ROGUE ) ) )
	    skill = 10 + 3 * ch->level;

	else if ( sn == gsn_third_attack
	&& IS_SET( ch->pIndexData->class, MCLASS_FIGHTER ) )
	    skill = 4 * ch->level - 40;

	else if (sn == gsn_fourth_attack
	&& IS_SET( ch->pIndexData->class, MCLASS_FIGHTER ) )
	    skill = 4 * ch->level - 40;

	else if (sn == gsn_fifth_attack
	&& IS_SET( ch->pIndexData->class, MCLASS_FIGHTER ) )
	    skill = 4 * ch->level - 40;

	else if (sn == gsn_hand_to_hand)
	    skill = 40 + 2 * ch->level;

 	else if ( sn == gsn_trip && xIS_SET( ch->off_flags, OFF_TRIP ) )
	    skill = 10 + 3 * ch->level;

 	else if ( sn == gsn_bash && xIS_SET( ch->off_flags, OFF_BASH ) )
	    skill = 10 + 3 * ch->level;

	else if ( sn == gsn_disarm 
	     &&  ( xIS_SET( ch->off_flags, OFF_DISARM ) 
	     ||   IS_SET( ch->pIndexData->class, MCLASS_FIGHTER )
	     ||	  IS_SET( ch->pIndexData->class, MCLASS_ROGUE ) ) )
	    skill = 20 + 3 * ch->level;

	else if ( sn == gsn_berserk && xIS_SET( ch->off_flags, OFF_BERSERK ) )
	    skill = 3 * ch->level;

	else if (sn == gsn_kick)
	    skill = 10 + 3 * ch->level;

	else if ( sn == gsn_backstab
	&& IS_SET( ch->pIndexData->class, MCLASS_ROGUE ) )
	    skill = 20 + 2 * ch->level;

	 else if ( sn == gsn_assassinate
        && IS_SET( ch->pIndexData->class, MCLASS_ROGUE ) )
            skill = 20 + 2 * ch->level;


  	else if (sn == gsn_rescue)
	    skill = 40 + ch->level; 

	else if (sn == gsn_recall)
	    skill = 40 + ch->level;

	else if ( sn == gsn_charge )
	    skill = 40 + ch->level / 4;

	else if (sn == gsn_sword
	||  sn == gsn_dagger
	||  sn == gsn_spear
	||  sn == gsn_mace
	||  sn == gsn_axe
	||  sn == gsn_flail
	||  sn == gsn_whip
	||  sn == gsn_polearm)
	    skill = 40 + 5 * ch->level / 2;

	else 
	   skill = 0;
    }

    if ( ch->daze > 0 )
    {
	if ( skill_table[sn].spell_fun != spell_null )
	    skill /= 2;
	else
	    skill = 2 * skill / 3;
    }

    if ( !IS_NPC( ch ) && IS_DRUNK( ch ) )
	skill = 9 * skill / 10;

    return URANGE( 0, skill, 100 );
}

/* for returning weapon information */
int
get_weapon_sn(CHAR_DATA *ch)
{
    OBJ_DATA *wield;
    int sn;

    wield = get_eq_char( ch, WEAR_WIELD );
    if (wield == NULL || wield->item_type != ITEM_WEAPON)
        sn = gsn_hand_to_hand;
    else switch (wield->value[0])
    {
        default :               sn = -1;                break;
        case(WEAPON_SWORD):     sn = gsn_sword;         break;
        case(WEAPON_DAGGER):    sn = gsn_dagger;        break;
        case(WEAPON_SPEAR):     sn = gsn_spear;         break;
        case(WEAPON_MACE):      sn = gsn_mace;          break;
	case(WEAPON_HAMMER):	sn = gsn_hammer;	break;
        case(WEAPON_AXE):       sn = gsn_axe;           break;
        case(WEAPON_FLAIL):     sn = gsn_flail;         break;
        case(WEAPON_WHIP):      sn = gsn_whip;          break;
        case(WEAPON_POLEARM):   sn = gsn_polearm;       break;
   }
   return sn;
}

int
get_weapon_skill( CHAR_DATA *ch, int sn )
{
     int skill;

     /* -1 is exotic */
    if (IS_NPC(ch))
    {
	if ( sn == -1 )
	    skill = 3 * ch->level;
	else if ( sn == gsn_hand_to_hand )
	    skill = 40 + 2 * ch->level;
	else 
	    skill = 40 + 5 * ch->level / 2;
    }

    else
    {
	if ( sn == -1 )
	    skill = 3 * ch->level;
	else
	    skill = ch->pcdata->skill[sn].percent;
    }

    return URANGE( 0, skill, 100 );
} 


bool
mount_success ( CHAR_DATA *ch, CHAR_DATA *mount, bool canattack )
{
    int percent;
    int success;

    percent = number_percent( ) + ( ch->level < mount->level ? 
				    ( mount->level - ch->level ) * 3 : 
				    ( mount->level - ch->level ) * 2);

    if ( ch->fighting == NULL )
	percent -= 25;

    if ( !IS_NPC( ch ) && IS_DRUNK( ch ) )
    {
	percent += ch->pcdata->skill[gsn_riding].percent / 2;
	send_to_char( "Due to your being under the influence, riding seems a bit harder...\n\r", ch );
    }

    success = percent - get_skill( ch, gsn_riding );

    if( success <= 0 )
    {
	check_improve( ch, gsn_riding, TRUE, 1 );
	return( TRUE );
    }
    else
    {
	check_improve( ch, gsn_riding, FALSE, 1 );

	if ( success >= 10 && MOUNTED( ch ) == mount )
	{
	    act_color( AT_ACTION, "You lose control and fall off of $N.", ch, NULL, mount, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n loses control and falls off of $N.", ch, NULL, mount, TO_ROOM, POS_RESTING );
	    act_color( AT_ACTION, "$n loses control and falls off of you.", ch, NULL, mount, TO_VICT, POS_RESTING );

	    ch->riding = FALSE;
	    mount->riding = FALSE;

	    if (ch->position > POS_STUNNED ) 
		ch->position = POS_SITTING;

	    ch->hit -= 5;
	    update_pos( ch );
	}

	if ( success >= 40 && canattack)
	{
	    act_color( AT_ACTION, "$N doesn't like the way you've been treating $M.", ch, NULL, mount, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$N doesn't like the way $n has been treating $M.", ch, NULL, mount, TO_ROOM, POS_RESTING );
	    act_color( AT_ACTION, "You don't like the way $n has been treating you.", ch, NULL, mount, TO_VICT, POS_RESTING );

	    act_color( AT_ACTION, "$N snarls and attacks you!", ch, NULL, mount, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$N snarls and attacks $n!", ch, NULL, mount, TO_ROOM, POS_RESTING );
	    act_color( AT_ACTION, "You snarl and attack $n!", ch, NULL, mount, TO_VICT, POS_RESTING );  

	    damage( mount, ch, number_range( 1, mount->level), gsn_kick, DAM_BASH, FALSE );

	}
    }

    return( FALSE );
}

void transfer_char(CHAR_DATA *ch, CHAR_DATA *vch, ROOM_INDEX_DATA *to_room,     
                   const char *msg_out,     
                   const char *msg_travel,     
                   const char *msg_in)     
{     
        ROOM_INDEX_DATA *was_in = ch->in_room;     

        if (ch != vch)     
                act(msg_travel, vch, NULL, ch, TO_VICT);          
                                                                                                                                              
        char_from_room(ch);                                                                                                                   
                                                                                                                                              
        act(msg_out, was_in->people, NULL, ch, TO_ALL);                                                                                       
        act(msg_in, to_room->people, NULL, ch, TO_ALL);                                                                                       
                                                                                                                                              
        char_to_room(ch, to_room);                                                                                                            
                                                                                                                                              
                do_look(ch, "auto");                                                                                                          
}                                                                                                                                             


bool can_gate(CHAR_DATA *ch, CHAR_DATA *victim)     
{        
        if (victim == ch     
        ||  ch->fighting != NULL     
        ||  victim->in_room == NULL           
        ||  !can_see_room(ch, victim->in_room)     
        ||  IS_SET(ch->in_room->room_flags, ROOM_SAFE | ROOM_NO_RECALL |     
                                            ROOM_NO_OUT | ROOM_SOLITARY)     
        ||  IS_SET(victim->in_room->room_flags, ROOM_SAFE | ROOM_NO_RECALL |     
                                                ROOM_NO_OUT | ROOM_SOLITARY)                                                                   
        ||  room_is_private(victim->in_room)                                                                                                  
        ||  IS_SET(victim->imm_flags, IMM_SUMMON))                                                                                            
                return FALSE;                                                                                                                 
                                                                                                                                              
        if (IS_NPC(victim))                                                                                                                   
                return TRUE;                                                                                                                  
                                                                                                                                              
        return TRUE;     
}     

/* used to de-screw characters */
void
reset_char( CHAR_DATA *ch )
{
     int loc,mod,stat;
     OBJ_DATA *obj;
     AFFECT_DATA *af;
     int i;

     if (IS_NPC(ch))
	return;

    if (ch->pcdata->perm_hit == 0 
    ||	ch->pcdata->perm_mana == 0
    ||  ch->pcdata->perm_move == 0
    ||	ch->pcdata->last_level == 0)
    {
    /* do a FULL reset */
	for (loc = 0; loc < MAX_WEAR; loc++)
	{
	    obj = get_eq_char(ch,loc);
	    if (obj == NULL)
		continue;
	    if (!obj->enchanted)
	    for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
	    {
		mod = af->modifier;
		switch(af->location)
		{
		    case APPLY_SEX:	ch->sex		-= mod;
					if (ch->sex < 0 || ch->sex >2)
					    ch->sex = IS_NPC(ch) ?
						0 :
						ch->pcdata->true_sex;
									break;
		    case APPLY_MANA:	ch->max_mana	-= mod;		break;
		    case APPLY_HIT:	ch->max_hit	-= mod;		break;
		    case APPLY_MOVE:	ch->max_move	-= mod;		break;
		}
	    }

            for ( af = obj->affected; af != NULL; af = af->next )
            {
                mod = af->modifier;
                switch(af->location)
                {
                    case APPLY_SEX:     ch->sex         -= mod;         break;
                    case APPLY_MANA:    ch->max_mana    -= mod;         break;
                    case APPLY_HIT:     ch->max_hit     -= mod;         break;
                    case APPLY_MOVE:    ch->max_move    -= mod;         break;
                }
            }
	}
	/* now reset the permanent stats */
	ch->pcdata->perm_hit 	= ch->max_hit;
	ch->pcdata->perm_mana 	= ch->max_mana;
	ch->pcdata->perm_move	= ch->max_move;
	ch->pcdata->last_level	= ch->played/3600;
	if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
	{
	    if ( ch->sex > 0 && ch->sex < 3 )
		ch->pcdata->true_sex	= ch->sex;
	    else
		ch->pcdata->true_sex 	= 0;
	}
    }

    /* now restore the character to his/her true condition */
    for (stat = 0; stat < MAX_STATS; stat++)
	ch->mod_stat[stat] = 0;

    if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
	ch->pcdata->true_sex = 0; 
    ch->sex		= ch->pcdata->true_sex;
    ch->max_hit 	= ch->pcdata->perm_hit;
    ch->max_mana	= ch->pcdata->perm_mana;
    ch->max_move	= ch->pcdata->perm_move;

    for (i = 0; i < 4; i++)
    	ch->armor[i]	= 100;

    ch->hitroll		= 0;
    ch->damroll		= 0;
    ch->saving_throw	= 0;

    /* now start adding back the effects */
    for (loc = 0; loc < MAX_WEAR; loc++)
    {
        obj = get_eq_char(ch,loc);
        if (obj == NULL)
            continue;
	for (i = 0; i < 4; i++)
	    ch->armor[i] -= apply_ac( obj, loc, i );

        if (!obj->enchanted)
	for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
        {
            mod = af->modifier;
            switch(af->location)
            {
		case APPLY_STR:		ch->mod_stat[STAT_STR]	+= mod;	break;
		case APPLY_DEX:		ch->mod_stat[STAT_DEX]	+= mod; break;
		case APPLY_INT:		ch->mod_stat[STAT_INT]	+= mod; break;
		case APPLY_WIS:		ch->mod_stat[STAT_WIS]	+= mod; break;
		case APPLY_CON:		ch->mod_stat[STAT_CON]	+= mod; break;

		case APPLY_SEX:		ch->sex			+= mod; break;
		case APPLY_MANA:	ch->max_mana		+= mod; break;
		case APPLY_HIT:		ch->max_hit		+= mod; break;
		case APPLY_MOVE:	ch->max_move		+= mod; break;

		case APPLY_AC:		
		    for (i = 0; i < 4; i ++)
			ch->armor[i] += mod; 
		    break;
		case APPLY_HITROLL:	ch->hitroll		+= mod; break;
		case APPLY_DAMROLL:	ch->damroll		+= mod; break;
		case APPLY_SAVES:		ch->saving_throw += mod; break;
		case APPLY_SAVING_ROD: 		ch->saving_throw += mod; break;
		case APPLY_SAVING_PETRI:	ch->saving_throw += mod; break;
		case APPLY_SAVING_BREATH: 	ch->saving_throw += mod; break;
		case APPLY_SAVING_SPELL:	ch->saving_throw += mod; break;
		case APPLY_REGENERATION:	ch->regen_rate	 += mod; break;
	    }
        }

        for ( af = obj->affected; af != NULL; af = af->next )
        {
            mod = af->modifier;
            switch(af->location)
            {
                case APPLY_STR:         ch->mod_stat[STAT_STR]  += mod; break;
                case APPLY_DEX:         ch->mod_stat[STAT_DEX]  += mod; break;
                case APPLY_INT:         ch->mod_stat[STAT_INT]  += mod; break;
                case APPLY_WIS:         ch->mod_stat[STAT_WIS]  += mod; break;
                case APPLY_CON:         ch->mod_stat[STAT_CON]  += mod; break;

                case APPLY_SEX:         ch->sex                 += mod; break;
                case APPLY_MANA:        ch->max_mana            += mod; break;
                case APPLY_HIT:         ch->max_hit             += mod; break;
                case APPLY_MOVE:        ch->max_move            += mod; break;

                case APPLY_AC:
                    for (i = 0; i < 4; i ++)
                        ch->armor[i] += mod;
                    break;
		case APPLY_HITROLL:     ch->hitroll             += mod; break;
                case APPLY_DAMROLL:     ch->damroll             += mod; break;

                case APPLY_SAVES:         ch->saving_throw += mod; break;
                case APPLY_SAVING_ROD:          ch->saving_throw += mod; break;
                case APPLY_SAVING_PETRI:        ch->saving_throw += mod; break;
                case APPLY_SAVING_BREATH:       ch->saving_throw += mod; break;
                case APPLY_SAVING_SPELL:        ch->saving_throw += mod; break;
            }
	}
    }

    /* now add back spell effects */
    for (af = ch->affected; af != NULL; af = af->next)
    {
        mod = af->modifier;
        switch(af->location)
        {
                case APPLY_STR:         ch->mod_stat[STAT_STR]  += mod; break;
                case APPLY_DEX:         ch->mod_stat[STAT_DEX]  += mod; break;
                case APPLY_INT:         ch->mod_stat[STAT_INT]  += mod; break;
                case APPLY_WIS:         ch->mod_stat[STAT_WIS]  += mod; break;
                case APPLY_CON:         ch->mod_stat[STAT_CON]  += mod; break;

                case APPLY_SEX:         ch->sex                 += mod; break;
                case APPLY_MANA:        ch->max_mana            += mod; break;
                case APPLY_HIT:         ch->max_hit             += mod; break;
                case APPLY_MOVE:        ch->max_move            += mod; break;

                case APPLY_AC:
                    for (i = 0; i < 4; i ++)
                        ch->armor[i] += mod;
                    break;
                case APPLY_HITROLL:     ch->hitroll             += mod; break;
                case APPLY_DAMROLL:     ch->damroll             += mod; break;

                case APPLY_SAVES:         ch->saving_throw += mod; break;
                case APPLY_SAVING_ROD:          ch->saving_throw += mod; break;
                case APPLY_SAVING_PETRI:        ch->saving_throw += mod; break;
                case APPLY_SAVING_BREATH:       ch->saving_throw += mod; break;
                case APPLY_SAVING_SPELL:        ch->saving_throw += mod; break;
        } 
    }

    /* make sure sex is RIGHT!!!! */
    if (ch->sex < 0 || ch->sex > 2)
	ch->sex = ch->pcdata->true_sex;
}


/*
 * Retrieve a character's trusted level for permission checking.
 */
int
get_trust( CHAR_DATA *ch )
{
    if ( ch->desc != NULL && ch->desc->original != NULL )
	ch = ch->desc->original;

    if (ch->trust)
	return ch->trust;

    if ( IS_NPC(ch) && ch->level >= LEVEL_HERO )
	return LEVEL_HERO - 1;
    else
	return ch->level;
}


/*
 * Retrieve a character's age.
 */
int
get_age( CHAR_DATA *ch )
{
    int		age;
    int		plr_secs;

    if ( IS_NPC( ch ) )
        return 17 + ch->level / 4;

    plr_secs = ch->played + current_time - ch->logon;
    age = 17 + plr_secs / ( ( PULSE_TICK / PULSE_PER_SECOND )
                        * 24 * DAYS_PER_MONTH * MONTHS_PER_YEAR );

    return age;
}

/* command for retrieving stats */
int
get_curr_stat( CHAR_DATA *ch, int stat )
{
    int		drunk;
    int		max;
    int		mod_stat;

    mod_stat = ch->perm_stat[stat] + ch->mod_stat[stat];

    if ( IS_NPC( ch ) || ch->level > LEVEL_IMMORTAL )
	max = 25;

    else
    {
	drunk = URANGE( 0, ch->pcdata->condition[COND_DRUNK], 48 );

	switch ( stat )
	{
	    case STAT_STR:
		mod_stat += drunk / 10;
		break;
	    case STAT_INT:
		if ( drunk > 20 )
		    mod_stat -= 2;
		else if ( drunk > 5 )
		    mod_stat -= 1;
		break;
	    case STAT_WIS:
		mod_stat -= drunk / 10;
		break;
	    case STAT_DEX:
		mod_stat -= drunk / 8;
		break;
	    case STAT_CON:
		break;
	}

	max = race_table[ch->race].max_stats[stat] + 4;

	if ( class_table[ch->class].attr_prime == stat )
	    max += 2;

	if ( ch->race == race_human )
	    max += 1;

 	max = UMIN( max, 25 );
    }

    return URANGE( 3, mod_stat, max );
}


/* command for returning max training score */
int
get_max_train( CHAR_DATA *ch, int stat )
{
    int max;

    if (IS_NPC(ch) || ch->level > LEVEL_IMMORTAL)
	return 25;

    max = race_table[ch->race].max_stats[stat];
    if ( class_table[ch->class].attr_prime == stat )
    {
	if ( ch->race == race_lookup( "human" ) )
	   max += 3;
	else
	   max += 2;
    }

    return UMIN(max,25);
}


/*
 * Retrieve a character's carry capacity.
 */
int
can_carry_n( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
	return 1000;

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
	return 0;

    return MAX_WEAR +  2 * get_curr_stat(ch,STAT_DEX) + ch->level;
}


/*
 * Retrieve a character's carry capacity.
 */
int
can_carry_w( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
	return 10000000;

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
	return 0;

    return str_app[get_curr_stat(ch,STAT_STR)].carry * 160 + ch->level * 25;
}


/*
 * See if two wordlists contain the same word.
 */
bool
has_same_word( const char *alist, const char *blist )
{
    char		buf[MAX_STRING_LENGTH];
    char		word[MAX_STRING_LENGTH];
    const char *	p;
    char		lastchar;

    if ( IS_NULLSTR( alist ) || IS_NULLSTR( blist ) )
        return FALSE;

    p = alist;
    do
    {
        p = one_argument( p, word );
        if ( is_exact_name( word, blist ) )
            return TRUE;
    }
    while ( *p != '\0' );

    lastchar = blist[strlen(blist)-1];

    if ( lastchar != '.' && lastchar != '?' && lastchar != '!' )
	return FALSE;

    if ( strchr( alist, lastchar ) != NULL )
	return FALSE;

    strcpy( buf, blist );
    buf[strlen(buf)-1] = '\0';
    if ( buf[0] == '\0' )
	return FALSE;
    p = alist;
    do
    {
        p = one_argument( p, word );
        if ( is_exact_name( word, buf ) )
            return TRUE;
    }
    while ( *p != '\0' );

    return FALSE;
}


/*
 * See if a string is one of the names of an object.
 */
bool
is_name ( char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
    char *list, *string;

    /* fix crash on NULL namelist */
    if (namelist == NULL || namelist[0] == '\0')
    	return FALSE;

    /* fixed to prevent is_name on "" returning TRUE */
    if (str[0] == '\0')
	return FALSE;

    string = str;
    /* we need ALL parts of string to match part of namelist */
    for ( ; ; )  /* start parsing string */
    {
	str = one_argument(str,part);

	if (part[0] == '\0' )
	    return TRUE;

	/* check to see if this is part of namelist */
	list = namelist;
	for ( ; ; )  /* start parsing namelist */
	{
	    list = one_argument(list,name);
	    if (name[0] == '\0')  /* this name was not found */
		return FALSE;

	    if (!str_prefix(string,name))
		return TRUE; /* full pattern match */

	    if (!str_prefix(part,name))
		break;
	}
    }
}


bool
is_exact_name( char *str, const char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    if (namelist == NULL)
	return FALSE;

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_cmp( str, name ) )
	    return TRUE;
    }
}

/* enchanted stuff for eq */
void
affect_enchant(OBJ_DATA *obj)
{
    /* okay, move all the old flags into new vectors if we have to */
    if (!obj->enchanted)
    {
        AFFECT_DATA *paf, *af_new;
        obj->enchanted = TRUE;

        for (paf = obj->pIndexData->affected;
             paf != NULL; paf = paf->next)
        {
	    af_new = new_affect();

            af_new->next = obj->affected;
            obj->affected = af_new;

	    af_new->where	= paf->where;
            af_new->type        = UMAX(0,paf->type);
            af_new->level       = paf->level;
            af_new->duration    = paf->duration;
            af_new->location    = paf->location;
            af_new->modifier    = paf->modifier;
            af_new->bitvector   = paf->bitvector;
        }
    }
}


/*
 * Apply or remove an affect to a character.
 */
void
affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA *wield;
    int mod,i;

    mod = paf->modifier;

    if ( fAdd )
    {
	switch (paf->where)
	{
	case TO_AFFECTS:
	    xSET_BIT( ch->affected_by, paf->bitvector );
	    break;
	case TO_IMMUNE:
	    SET_BIT(ch->imm_flags,paf->bitvector);
	    break;
	case TO_RESIST:
	    SET_BIT(ch->res_flags,paf->bitvector);
	    break;
	case TO_VULN:
	    SET_BIT(ch->vuln_flags,paf->bitvector);
	    break;
	case TO_SHIELDS:
	    xSET_BIT( ch->shielded_by, paf->bitvector );
	    break;
	}
    }
    else
    {
        switch (paf->where)
        {
        case TO_AFFECTS:
	    xREMOVE_BIT( ch->affected_by, paf->bitvector );
            break;
        case TO_IMMUNE:
            REMOVE_BIT(ch->imm_flags,paf->bitvector);
            break;
        case TO_RESIST:
            REMOVE_BIT(ch->res_flags,paf->bitvector);
            break;
        case TO_VULN:
            REMOVE_BIT(ch->vuln_flags,paf->bitvector);
            break;
        case TO_SHIELDS:
	    xREMOVE_BIT( ch->shielded_by, paf->bitvector );
            break;
        }
	mod = 0 - mod;
    }

    switch ( paf->location )
    {
    default:
	bug( "Affect_modify: unknown location %d.", paf->location );
	return;

    case APPLY_NONE:						break;
    case APPLY_STR:           ch->mod_stat[STAT_STR]	+= mod;	break;
    case APPLY_DEX:           ch->mod_stat[STAT_DEX]	+= mod;	break;
    case APPLY_INT:           ch->mod_stat[STAT_INT]	+= mod;	break;
    case APPLY_WIS:           ch->mod_stat[STAT_WIS]	+= mod;	break;
    case APPLY_CON:           ch->mod_stat[STAT_CON]	+= mod;	break;
    case APPLY_SEX:           ch->sex			+= mod;	break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
    case APPLY_SIZE:	      ch->size			+= mod; break;
    case APPLY_MANA:          ch->max_mana		+= mod;	break;
    case APPLY_HIT:           ch->max_hit		+= mod;	break;
    case APPLY_MOVE:          ch->max_move		+= mod;	break;
    case APPLY_WEALTH:						break;
    case APPLY_EXP:						break;
    case APPLY_AC:
        for (i = 0; i < 4; i ++)
            ch->armor[i] += mod;
        break;
    case APPLY_HITROLL:       ch->hitroll		+= mod;	break;
    case APPLY_DAMROLL:       ch->damroll		+= mod;	break;
    case APPLY_SAVES:   ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_ROD:    ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_PETRI:  ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_BREATH: ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_SPELL:  ch->saving_throw		+= mod;	break;
    case APPLY_SPELL_AFFECT:  					break;
    case APPLY_REGENERATION:	ch->regen_rate		+= mod; break;
    }

    /*
     * Check for weapon wielding.
     * Guard against recursion (for weapons with affects).
     */
    if ( !IS_NPC(ch) && ( wield = get_eq_char( ch, WEAR_WIELD ) ) != NULL
    &&   get_obj_weight(wield) > (str_app[get_curr_stat(ch,STAT_STR)].wield*10))
    {
	static int depth;

	if ( depth == 0 )
	{
	    depth++;
	    act_color( AT_ACTION, "You drop $p.", ch, wield, NULL, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n drops $p.", ch, wield, NULL, TO_ROOM, POS_RESTING );
	    obj_from_char( wield );
	    obj_to_room( wield, ch->in_room );
	    add_obj_fall_event( wield );
	    depth--;
	}
    }

    return;
}


/* find an effect in an affect list */
AFFECT_DATA  *affect_find(AFFECT_DATA *paf, int sn)
{
    AFFECT_DATA *paf_find;

    for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
    {
        if ( paf_find->type == sn )
	return paf_find;
    }

    return NULL;
}

/* fix object affects when removing one */
void
affect_check( CHAR_DATA *ch, int where, int vector )
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    if (where == TO_OBJECT || where == TO_WEAPON || vector == 0)
	return;

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
	if ( paf->where == where && paf->bitvector == vector )
	{
	    switch ( where )
	    {
	        case TO_AFFECTS:
		    xSET_BIT( ch->affected_by, vector );
		    break;
	        case TO_IMMUNE:
		    SET_BIT(ch->imm_flags,vector);   
		    break;
	        case TO_RESIST:
		    SET_BIT(ch->res_flags,vector);
		    break;
	        case TO_VULN:
		    SET_BIT(ch->vuln_flags,vector);
		    break;
	        case TO_SHIELDS:
		    xSET_BIT( ch->shielded_by, vector );
		    break;
	    }
	    return;
	}

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
	if (obj->wear_loc == -1)
	    continue;

            for (paf = obj->affected; paf != NULL; paf = paf->next)
            if (paf->where == where && paf->bitvector == vector)
            {
                switch (where)
                {
                    case TO_AFFECTS:
			xSET_BIT( ch->affected_by, vector );
                        break;
                    case TO_IMMUNE:
                        SET_BIT(ch->imm_flags,vector);
                        break;
                    case TO_RESIST:
                        SET_BIT(ch->res_flags,vector);
                        break;
                    case TO_VULN:
                        SET_BIT(ch->vuln_flags,vector);
                    case TO_SHIELDS:
			xSET_BIT( ch->shielded_by, vector );
                        break;

                }
                return;
            }

        if (obj->enchanted)
	    continue;

        for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
            if (paf->where == where && paf->bitvector == vector)
            {
                switch (where)
                {
                    case TO_AFFECTS:
			xSET_BIT( ch->affected_by, vector );
                        break;
                    case TO_IMMUNE:
                        SET_BIT(ch->imm_flags,vector);
                        break;
                    case TO_RESIST:
                        SET_BIT(ch->res_flags,vector);
                        break;
                    case TO_VULN:
                        SET_BIT(ch->vuln_flags,vector);
                        break;
                    case TO_SHIELDS:
			xSET_BIT( ch->shielded_by, vector );
                        break;
                }
                return;
            }
    }
}

/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;

    paf_new = new_affect();

    *paf_new		= *paf;

    VALIDATE(paf);	/* in case we missed it when we set up paf */
    paf_new->next	= ch->affected;
    ch->affected	= paf_new;

    affect_modify( ch, paf_new, TRUE );
    return;
}

/* give an affect to an object */
void affect_to_obj(OBJ_DATA *obj, AFFECT_DATA *paf)
{
    AFFECT_DATA *paf_new;

    paf_new = new_affect();

    *paf_new		= *paf;

    VALIDATE(paf);	/* in case we missed it when we set up paf */
    paf_new->next	= obj->affected;
    obj->affected	= paf_new;

    /* apply any affect vectors to the object's extra_flags */
    if (paf->bitvector)
        switch (paf->where)
        {
        case TO_OBJECT:
    	    SET_BIT(obj->extra_flags,paf->bitvector);
	    break;
        case TO_WEAPON:
	    if (obj->item_type == ITEM_WEAPON)
	        SET_BIT(obj->value[4],paf->bitvector);
	    break;
        }

    return;
}



/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    int where;
    int vector;

    if ( ch->affected == NULL )
    {
	bug( "Affect_remove: no affect.", 0 );
	return;
    }

    affect_modify( ch, paf, FALSE );
    where = paf->where;
    vector = paf->bitvector;

    if ( paf == ch->affected )
    {
	ch->affected	= paf->next;
    }
    else
    {
	AFFECT_DATA *prev;

	for ( prev = ch->affected; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == paf )
	    {
		prev->next = paf->next;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Affect_remove: cannot find paf.", 0 );
	    return;
	}
    }

    free_affect(paf);

    affect_check(ch,where,vector);
    return;
}

void affect_remove_obj( OBJ_DATA *obj, AFFECT_DATA *paf)
{
    int where, vector;
    if ( obj->affected == NULL )
    {
        bug( "Affect_remove_object: no affect.", 0 );
        return;
    }

    if (obj->carried_by != NULL && obj->wear_loc != -1)
	affect_modify( obj->carried_by, paf, FALSE );

    where = paf->where;
    vector = paf->bitvector;

    /* remove flags from the object if needed */
    if (paf->bitvector)
	switch( paf->where)
        {
        case TO_OBJECT:
            REMOVE_BIT(obj->extra_flags,paf->bitvector);
            break;
        case TO_WEAPON:
            if (obj->item_type == ITEM_WEAPON)
                REMOVE_BIT(obj->value[4],paf->bitvector);
            break;
        }

    if ( paf == obj->affected )
    {
        obj->affected    = paf->next;
    }
    else
    {
        AFFECT_DATA *prev;

        for ( prev = obj->affected; prev != NULL; prev = prev->next )
        {
            if ( prev->next == paf )
            {
                prev->next = paf->next;
                break;
            }
        }

        if ( prev == NULL )
        {
            bug( "Affect_remove_object: cannot find paf.", 0 );
            return;
        }
    }

    free_affect(paf);

    if (obj->carried_by != NULL && obj->wear_loc != -1)
	affect_check(obj->carried_by,where,vector);
    return;
}



/*
 * Strip all affects of a given sn.
 */
void affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	if ( paf->type == sn )
	    affect_remove( ch, paf );
    }

    return;
}



/*
 * Return true if a char is affected by a spell.
 */
bool is_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->type == sn )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Add or enhance an affect.
 */
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;
    bool found;

    found = FALSE;
    for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next )
    {
	if ( paf_old->type == paf->type )
	{
	    paf->level = (paf->level += paf_old->level) / 2;
	    paf->duration += paf_old->duration;
	    paf->modifier += paf_old->modifier;
	    affect_remove( ch, paf_old );
	    break;
	}
    }

    affect_to_char( ch, paf );
    return;
}


/*
 * Move a char out of a room.
 */
void
char_from_room( CHAR_DATA *ch )
{
    OBJ_DATA *		obj;
    ROOM_INDEX_DATA *	in_room;

    if ( ch->in_room == NULL )
    {
	bug( "Char_from_room: NULL.", 0 );
	return;
    }

    in_room = ch->in_room->in_room;

    if ( !IS_NPC( ch ) )
    {
	--ch->in_room->area->nplayer;
	if ( in_room != NULL
	&&   in_room->area != ch->in_room->area )
	    --in_room->area->nplayer;
    }

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room->light > 0 )
    {
	--ch->in_room->light;
	if ( in_room != NULL
	&&   in_room->area != ch->in_room->area )
	    --in_room->light;
    }

    ch->in_room->vtimer = 2;

    if ( ch->in_room->helmsman == ch )
	ch->in_room->helmsman = NULL;

    if ( ch == ch->in_room->people )
    {
	ch->in_room->people = ch->next_in_room;
    }
    else
    {
	CHAR_DATA *prev;

	for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
	{
	    if ( prev->next_in_room == ch )
	    {
		prev->next_in_room = ch->next_in_room;
		break;
	    }
	}

	if ( prev == NULL )
	    bug( "Char_from_room: ch not found.", 0 );
    }

    ch->in_room      = NULL;
    ch->next_in_room = NULL;
    ch->on 	     = NULL;  /* sanity check! */
    return;
}


/*
 * Move a char into a room.
 */
void
char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *		obj;
    ROOM_INDEX_DATA *	in_room;

    if ( pRoomIndex == NULL )
    {
	ROOM_INDEX_DATA *room;

	bugf( "Char_to_room: %s: NULL.", ch && ch->name ? ch->name : "(null)" );

	if ( ( room = get_room_index( ROOM_VNUM_TEMPLE ) ) != NULL )
	    char_to_room( ch, room );

	return;
    }

    in_room = pRoomIndex->in_room;

    ch->in_room		= pRoomIndex;
    ch->next_in_room	= pRoomIndex->people;
    pRoomIndex->people	= ch;

    if ( !IS_NPC( ch ) )
    {
	if ( ch->in_room->area->empty )
	{
	    ch->in_room->area->empty = FALSE;
	    if ( !IS_SET( ch->in_room->area->area_flags, AREA_MUDSCHOOL ) )
		ch->in_room->area->age = 0;
	}
	++ch->in_room->area->nplayer;

	if ( in_room != NULL )
	{
	    if ( in_room->area->empty )
	    {
		in_room->area->empty = FALSE;
		in_room->area->age = 0;
	    }
	    ++in_room->area->nplayer;
	}
    }

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0 )
    {
	++ch->in_room->light;
	if ( in_room != NULL )
	    ++in_room->light;
    }

    if ( IS_AFFECTED( ch, AFF_PLAGUE ) )
    {
        AFFECT_DATA *af, plague;
        CHAR_DATA *vch;

        for ( af = ch->affected; af != NULL; af = af->next )
        {
            if ( af->type == gsn_plague )
                break;
        }

        if ( af == NULL )
        {
            xREMOVE_BIT( ch->affected_by, AFF_PLAGUE );
            return;
        }

        if ( af->level == 1 )
            return;

	plague.where		= TO_AFFECTS;
        plague.type 		= gsn_plague;
        plague.level 		= af->level - 1; 
        plague.duration 	= number_range(1,2 * plague.level);
        plague.location		= APPLY_STR;
        plague.modifier 	= -5;
        plague.bitvector 	= AFF_PLAGUE;

        for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
        {
            if (!saves_spell(plague.level - 2,vch,DAM_DISEASE) 
	    &&  !IS_IMMORTAL(vch) &&
            	!IS_AFFECTED(vch,AFF_PLAGUE) && number_bits(6) == 0)
            {
            	send_to_char("You feel hot and feverish.\n\r",vch);
            	act("$n shivers and looks very ill.",vch,NULL,NULL,TO_ROOM);
            	affect_join(vch,&plague);
            }
        }
    }

    return;
}



/*
 * Give an obj to a char.
 */
void
obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    obj->next_content	 = ch->carrying;
    ch->carrying	 = obj;
    obj->carried_by	 = ch;
    obj->in_room	 = NULL;
    obj->in_obj		 = NULL;
    obj->stored_in	 = NULL;
    ch->carry_number	+= get_obj_number( obj );
    ch->carry_weight	+= get_obj_weight( obj );
    if ( !IS_NPC( ch )
    &&	 obj->pIndexData->timer > 0
    &&	 obj->timer == 0 )
        obj->timer = obj->pIndexData->timer;
}



/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( ( ch = obj->carried_by ) == NULL )
    {
	bug( "Obj_from_char: null ch.", 0 );
	return;
    }

    if ( !IS_NPC( ch ) && ch->desc != NULL
    &&	 ch->desc->editor == RENAME_OBJECT && ch->desc->pEdit == obj )
    {
	ch->desc->editor = ED_NONE;
	ch->desc->pEdit = NULL;
    }

    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );

    if ( ch->carrying == obj )
    {
	ch->carrying = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = ch->carrying; prev != NULL; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( prev == NULL )
	    bug( "Obj_from_char: obj not in list.", 0 );
    }

    obj->carried_by	 = NULL;
    obj->next_content	 = NULL;
    ch->carry_number	-= get_obj_number( obj );
    ch->carry_weight	-= get_obj_weight( obj );
    return;
}



/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear, int type )
{
    if ( obj->item_type != ITEM_ARMOR )
	return 0;

    switch ( iWear )
    {
    case WEAR_BODY:	return 3 * obj->value[type];
    case WEAR_HEAD:	return 2 * obj->value[type];
    case WEAR_LEGS:	return 2 * obj->value[type];
    case WEAR_FEET:	return     obj->value[type];
    case WEAR_HANDS:	return     obj->value[type];
    case WEAR_ARMS:	return     obj->value[type];
    case WEAR_SHIELD:	return     obj->value[type];
    case WEAR_NECK_1:	return     obj->value[type];
    case WEAR_NECK_2:	return     obj->value[type];
    case WEAR_ABOUT:	return 2 * obj->value[type];
    case WEAR_WAIST:	return     obj->value[type];
    case WEAR_WRIST_L:	return     obj->value[type];
    case WEAR_WRIST_R:	return     obj->value[type];
    case WEAR_HOLD:	return     obj->value[type];
    }

    return 0;
}



/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;

    if (ch == NULL)
	return NULL;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->wear_loc == iWear )
	    return obj;
    }

    return NULL;
}



/*
 * Equip a char with an obj.
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    AFFECT_DATA *paf;
    int i;

    if ( get_eq_char( ch, iWear ) != NULL )
    {
	if ( IS_NPC( ch ) )
	    buildbug( "Equip_char: %s (#%d) already equipped (%d) in room %d.",
		      ch->short_descr, ch->pIndexData->vnum, iWear,
		      ch->in_room != NULL ? ch->in_room->vnum : 0 );
	else
	    bugf( "Equip_char: %s already equipped(%d).",
		  ch->name, iWear );
	return;
    }

    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
    {
	/*
	 * Thanks to Morgenes for the bug fix here!
	 */
	act( "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
	act( "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	add_obj_fall_event( obj );
	return;
    }

    for (i = 0; i < 4; i++)
    	ch->armor[i]      	-= apply_ac( obj, iWear,i );
    obj->wear_loc	 = iWear;

    if (!obj->enchanted)
	for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
	    if ( paf->location != APPLY_SPELL_AFFECT )
	        affect_modify( ch, paf, TRUE );
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
	if ( paf->location == APPLY_SPELL_AFFECT )
    	    affect_to_char ( ch, paf );
	else
	    affect_modify( ch, paf, TRUE );

    if ( obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room != NULL )
	++ch->in_room->light;

    oprog_wear_trigger( obj, ch );

    return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA *paf = NULL;
    AFFECT_DATA *lpaf = NULL;
    AFFECT_DATA *lpaf_next = NULL;
    int i;

    if ( obj->wear_loc == WEAR_NONE )
    {
	bug( "Unequip_char: already unequipped.", 0 );
	return;
    }

    for (i = 0; i < 4; i++)
    	ch->armor[i]	+= apply_ac( obj, obj->wear_loc,i );
    obj->wear_loc	 = -1;

    if (!obj->enchanted)
    {
	for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
	{
	    if ( paf->location == APPLY_SPELL_AFFECT )
	    {
	        for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
	        {
		    lpaf_next = lpaf->next;
		    if ((lpaf->type == paf->type) &&
		        (lpaf->level == paf->level) &&
		        (lpaf->location == APPLY_SPELL_AFFECT))
		    {
		        affect_remove( ch, lpaf );
			lpaf_next = NULL;
		    }
	        }
	    }
	    else
	    {
	        affect_modify( ch, paf, FALSE );
		affect_check(ch,paf->where,paf->bitvector);
	    }
	}
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
	if ( paf->location == APPLY_SPELL_AFFECT )
	{
	    bug ( "Norm-Apply: %d", 0 );
	    for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
	    {
		lpaf_next = lpaf->next;
		if ((lpaf->type == paf->type) &&
		    (lpaf->level == paf->level) &&
		    (lpaf->location == APPLY_SPELL_AFFECT))
		{
		    bug ( "location = %d", lpaf->location );
		    bug ( "type = %d", lpaf->type );
		    affect_remove( ch, lpaf );
		    lpaf_next = NULL;
		}
	    }
	}
	else
	{
	    affect_modify( ch, paf, FALSE );
	    affect_check(ch,paf->where,paf->bitvector);	
	}

    if ( obj->item_type == ITEM_BOOK )
        obj->value[0] = 0;

    if ( obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room != NULL
    &&   ch->in_room->light > 0 )
	--ch->in_room->light;

    return;
}


/*
 * Return the level of an object, plus the levels of every object it contains.
 */
int
count_obj_levels( OBJ_DATA *obj )
{
    OBJ_DATA *	inObj;
    int		levels;

    if ( obj == NULL )
	return 0;

    levels = obj->level;

    for ( inObj = obj->contains; inObj != NULL; inObj = inObj->next_content )
        levels += count_obj_levels( inObj );

    return levels;
}


/*
 * Count occurrences of an obj in a list.
 */
int
count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    int nMatch;

    nMatch = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
	if ( obj->pIndexData == pObjIndex )
	    nMatch++;
    }

    return nMatch;
}


/*
 * Move an obj out of a room.
 */
void
obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;
    CHAR_DATA *ch;

    if ( ( in_room = obj->in_room ) == NULL )
    {
	bug( "obj_from_room: NULL.", 0 );
	return;
    }

    for (ch = in_room->people; ch != NULL; ch = ch->next_in_room)
	if (ch->on == obj)
	    ch->on = NULL;

    if ( obj == in_room->contents )
    {
	in_room->contents = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = in_room->contents; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Obj_from_room: obj not found.", 0 );
	    return;
	}
    }

    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
        in_room->light--;

    obj->in_room      = NULL;
    obj->next_content = NULL;
    return;
}


/*
 * Move an obj into a room.
 */
void
obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
    obj->next_content		= pRoomIndex->contents;
    pRoomIndex->contents	= obj;
    obj->in_room		= pRoomIndex;
    obj->carried_by		= NULL;
    obj->in_obj			= NULL;
    obj->stored_in		= NULL;

    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
        pRoomIndex->light++;

    return;
}


/*
 * Move an object into an object.
 */
void
obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    obj->next_content		= obj_to->contains;
    obj_to->contains		= obj;
    obj->in_obj			= obj_to;
    obj->in_room		= NULL;
    obj->carried_by		= NULL;
    obj->stored_in		= NULL;

#if 0
    if (obj_to->pIndexData->vnum == OBJ_VNUM_PIT)
        obj->cost = 0; 
#endif

    for ( ; obj_to != NULL; obj_to = obj_to->in_obj )
    {
	if ( obj_to->carried_by != NULL )
	{
	    obj_to->carried_by->carry_number += get_obj_number( obj );
	    obj_to->carried_by->carry_weight += get_obj_weight( obj )
		* WEIGHT_MULT(obj_to) / 100;
	}
    }

    return;
}


/*
 * Move an object out of an object.
 */
void
obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;

    if ( ( obj_from = obj->in_obj ) == NULL )
    {
	bug( "Obj_from_obj: null obj_from.", 0 );
	return;
    }

    if ( obj == obj_from->contains )
    {
	obj_from->contains = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = obj_from->contains; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Obj_from_obj: obj not found.", 0 );
	    return;
	}
    }

    obj->next_content = NULL;
    obj->in_obj       = NULL;

    for ( ; obj_from != NULL; obj_from = obj_from->in_obj )
    {
	if ( obj_from->carried_by != NULL )
	{
	    obj_from->carried_by->carry_number -= get_obj_number( obj );
	    obj_from->carried_by->carry_weight -= get_obj_weight( obj ) 
		* WEIGHT_MULT(obj_from) / 100;
	}
    }

    return;
}


/*
 * Place an object in storage
 */
void
obj_to_bank( OBJ_DATA *obj, BANK_DATA *bank )
{
    obj->next_content = bank->storage;
    bank->storage	= obj;
    bank->count++;
    obj->carried_by	= NULL;
    obj->in_room	= NULL;
    obj->in_obj		= NULL;
    obj->stored_in	= bank;
}


/*
 * Remove an object from storage
 */
void
obj_from_bank( OBJ_DATA *obj )
{
    BANK_DATA *	bank;
    OBJ_DATA *	prev;

    if ( ( bank = obj->stored_in ) == NULL )
    {
	bug( "Obj_from_storage: vnum %d: NULL bank.", obj->pIndexData->vnum );
	return;
    }

    if ( bank->storage == obj )
	bank->storage = obj->next_content;
    else
    {
	for ( prev = bank->storage; prev != NULL; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( prev == NULL )
	    bug( "Obj_from storage: object %d not in bank.", obj->pIndexData->vnum );
    }

    obj->stored_in	= NULL;
    obj->next_content	= NULL;
    bank->count--;
    return;
}


/*
 * Extract an obj from the world.
 */
void
extract_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_content;
    OBJ_DATA *obj_next;

    if ( obj->in_room != NULL )
	obj_from_room( obj );
    else if ( obj->carried_by != NULL )
	obj_from_char( obj );
    else if ( obj->in_obj != NULL )
	obj_from_obj( obj );
    else if ( obj->stored_in != NULL )
	obj_from_bank( obj );

    for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
    {
	obj_next = obj_content->next_content;
	extract_obj( obj_content );
    }

    if ( object_list == obj )
    {
	object_list = obj->next;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = object_list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == obj )
	    {
		prev->next = obj->next;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Extract_obj: obj %d not found.", obj->pIndexData->vnum );
	    return;
	}
    }

    --obj->pIndexData->count;
    free_obj(obj);
    return;
}



/*
 * Extract a char from the world.
 * See comment below on char_died()
 */
void
extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA *		wch;
    OBJ_DATA *		obj;
    OBJ_DATA *		obj_next;
    ROOM_INDEX_DATA *	morgue;

    /* doesn't seem to be necessary
    if ( ch->in_room == NULL )
    {
	bug( "Extract_char: NULL.", 0 );
	return;
    }
    */

    nuke_pets( ch );
    ch->pet = NULL; /* just in case */

    if ( fPull )

	die_follower( ch );

    stop_fighting( ch, TRUE );

    if ( fPull )
    {
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
         {
	    obj_next = obj->next_content;
	    extract_obj( obj );
	}
    }

    if ( ch->mount != NULL && ch->mount->mount == ch )
    {
	ch->mount->mount = NULL;

	if ( ch->mount->riding )
	{
	    act_color( AT_ACTION, "You fall off of $N.", ch->mount, NULL, ch, TO_CHAR, POS_RESTING );
	    act_color( AT_ACTION, "$n falls off of $N.", ch->mount, NULL, ch, TO_ROOM, POS_RESTING );
	    ch->mount->riding = FALSE;
	    if ( !IS_IMMORTAL( ch->mount ) )
		ch->mount->position = POS_SITTING;
	}
    }

    if ( ch->in_room != NULL )
        char_from_room( ch );

    if ( !fPull )
    {
	if ( ( morgue = get_room_index( ROOM_VNUM_RIFT ) ) == NULL )
	{
	    bugf( "Extract_char: no ROOM_VNUM_RIFT." );
	    morgue = get_room_index( ROOM_VNUM_LIMBO );
	}
	char_to_room( ch, morgue );
	return;
    }

    if ( IS_NPC(ch) )
	--ch->pIndexData->count;

    if ( ch->desc != NULL && ch->desc->original != NULL )
    {
	do_function(ch, &do_return, "" );
	ch->desc = NULL;
    }

    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch->questgiver == ch )
	    wch->questgiver = NULL;
	if ( wch->reply == ch )
	    wch->reply = NULL;
        if ( wch->qmem == ch )
            wch->qmem = NULL;
    }
    if ( ch == char_list )
    {
       char_list = ch->next;
    }
    else
    {
	CHAR_DATA *prev;

	for ( prev = char_list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == ch )
	    {
		prev->next = ch->next;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Extract_char: char not found.", 0 );
	    return;
	}
    }

    if ( ch->desc != NULL )
	ch->desc->character = NULL;
    ch->deleted = TRUE;
    free_char( ch );
    return;
}


/*
 * Return TRUE if a char recently died and is scheduled for extraction.
 *
 * NOTE:
 * We really need to implement this.  Basically we want extract_char() to
 * simply set ch->deleted to TRUE, then at the end of update_handler()
 * add a call to a function that does the actual removal from the char_data
 * list.
 */
bool char_died( CHAR_DATA *ch )
{
    return ch->deleted;
}
/*
 * Same thing as above, only in regards to objects
 */
bool obj_extracted( OBJ_DATA *pObj )
{
    return FALSE;
}


/*
 * Move a room out of a room.
 */
void
room_from_room( ROOM_INDEX_DATA *room )
{
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	prev;
    CHAR_DATA *		ch;
    int			people;

    if ( ( in_room = room->in_room ) == NULL )
    {
	bug( "Room_from_room: room #%d not in room", room->vnum );
	return;
    }

    if ( room == in_room )
    {
	bug( "Room_from_room: room #%d inside itself?!?", room->vnum );
	return;
    }

    if ( room == in_room->next_in_room )
    {
	in_room->next_in_room = room->next_in_room;
    }
    else
    {
	for ( prev = in_room->next_in_room; prev != NULL; prev = prev->next_in_room )
	{
	    if ( prev->next_in_room == room )
	    {
		prev->next_in_room = room->next_in_room;
		break;
	    }

	}

	if ( prev == NULL )
	{
	    bug( "Room_from_room: room #%d not found.", room->vnum );
	    return;
	}
    }

    in_room->light -= room->light;

    if ( room->area != in_room->area )
    {
	people = 0;
	for ( ch = room->people; ch != NULL; ch = ch->next_in_room )
	    if ( !IS_NPC( ch ) )
		people++;

	in_room->area->nplayer -= people;
    }

    room->in_room = NULL;
    room->next_in_room = NULL;

    return;
}


/*
 * Move a room into a room.
 */
void
room_to_room( ROOM_INDEX_DATA *room, ROOM_INDEX_DATA *to_room )
{
    CHAR_DATA *	ch;
    int		people;

    room->in_room = to_room;
    room->next_in_room = to_room->next_in_room;
    to_room->next_in_room = room;
    to_room->light += room->light;

    if ( room->area != to_room->area )
    {
	people = 0;
	for ( ch = room->people; ch != NULL; ch = ch->next_in_room )
	    if ( !IS_NPC( ch ) )
		people++;
	to_room->area->nplayer += people;
    }

    vehicle_moved = TRUE;
    return;
}


/*
 * Purges all mobs and objects in a room.
 * Returns TRUE if any mobs or objs purged/
 */
bool
purge_room( ROOM_INDEX_DATA *pRoom )
{
    CHAR_DATA *	vch;
    CHAR_DATA * vch_next;
    OBJ_DATA *	obj;
    OBJ_DATA *	obj_next;
    bool	flag;

    flag = FALSE;

    if ( pRoom == NULL )
        return FALSE;

    for ( vch = pRoom->people; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next_in_room;
        if ( IS_NPC( vch ) && !IS_SET( vch->act, ACT_NOPURGE ) )
        {
            extract_char( vch, TRUE );
            flag = TRUE;
        }
    }

    for ( obj = pRoom->contents; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if ( !IS_SET( obj->extra_flags, ITEM_NOPURGE ) )
        {
            extract_obj( obj );
            flag = TRUE;
        }
    }

    return flag;
}

/* deduct movement. makes some hackish adjustments */     
void deduct_move(CHAR_DATA *ch, int amount)     
{     
    if (ch->level < 30)     
        amount /= 4;              
    else if (ch->level < 60)     
        amount /= 3;     
    else if (ch->level < 90)                                                                                                                            
        amount /= 2;       

    amount = UMAX(amount, 1);     

    ch->move -= amount;     
    if (ch->move < 0)     
        ch->move = 0;       
}         



/*
 * Find a char in the area.
 */
CHAR_DATA *
get_char_area( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *ach;
    int number;
    int count;

    if ( ( ach = get_char_room( ch, argument ) ) != NULL )
	return ach;

    number = number_argument( argument, arg );
    count  = 0;
    for ( ach = char_list; ach != NULL ; ach = ach->next )
    {
	if ( ach->in_room->area != ch->in_room->area || !can_see( ch, ach ) )
	    continue;
	if ( knows_char( ch, ach ) && !is_name( arg, ach->name ) )
	    continue;
	if ( !knows_char( ch, ach )
	&&   str_prefix( arg, class_table[ach->class].name )
	&&   str_prefix( arg, race_table[ach->race].name ) )
	    continue;
	if ( ++count == number )
	    return ach;
    }

    return NULL;
}


/*
 * Find a char in the room.
 */
CHAR_DATA *
get_char_room( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *rch;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    if ( !str_cmp( arg, "self" ) )
	return ch;
    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( !can_see( ch, rch ) )
	    continue;
	if ( knows_char( ch, rch ) && !is_name( arg, rch->name ) )
	    continue;
	if ( !knows_char( ch, rch )
	&&   str_prefix( arg, class_table[rch->class].name )
	&&   str_prefix( arg, race_table[rch->race].name ) )
	    continue;
	if ( ++count == number )
	    return rch;
    }

    return NULL;
}


/*
 * Find a char in the world.
 */
CHAR_DATA *
get_char_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *wch;
    int number;
    int count;

    if ( ( wch = get_char_area( ch, argument ) ) != NULL )
	return wch;

    number = number_argument( argument, arg );
    count  = 0;
    for ( wch = char_list; wch != NULL ; wch = wch->next )
    {
	if ( wch->in_room == NULL || !can_see( ch, wch ) )
	    continue;
	if ( knows_char( ch, wch ) && !is_name( arg, wch->name ) )
	    continue;
	if ( !knows_char( ch, wch )
	&&   str_prefix( arg, class_table[wch->class].name )
	&&   str_prefix( arg, race_table[wch->race].name ) )
	    continue;
	if ( ++count == number )
	    return wch;
    }

    return NULL;
}


CLAN_DATA *
get_clan( const char *argument )
{
    CLAN_DATA *pClan;

    if ( !argument || *argument == '\0' )
	return NULL;

    for ( pClan = clan_first; pClan; pClan = pClan->next )
	if ( !str_prefix( argument, pClan->name ) )
	    return pClan;
    return NULL;
}


CLAN_DATA *
get_clan_index( int vnum )
{
    CLAN_DATA *pClan;

    for ( pClan = clan_first; pClan; pClan = pClan->next )
	if ( pClan->vnum == vnum )
	    return pClan;
    return NULL;
}


/*
 * Get a help section
 */
HELP_DATA *
get_help( char *keyword, bool fExact )
{
    HELP_DATA *pHelp;

    if ( keyword == NULL || *keyword == '\0' )
	return NULL;

    if ( fExact )
    {
	for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
	{
	    if ( is_exact_name( keyword, pHelp->keyword ) )
		return pHelp;
	}
    }
    else
    {
	for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
	{
	    if ( is_name( keyword, pHelp->keyword ) )
		return pHelp;
	}
    }

    return NULL;
}


/*
 * Find some object with a given index data in a room.
 * Room contents and inventories of mobs are searched.
 * Containers contents are not.
 */
OBJ_DATA *
get_obj_room( OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA *pRoom )
{
    OBJ_DATA *	pObj;
    CHAR_DATA *	pMob;

    if ( pObjIndex == NULL || pRoom == NULL )
        return NULL;

    for ( pObj = pRoom->contents; pObj != NULL; pObj = pObj->next_content )
    {
        if ( pObj->pIndexData == pObjIndex )
            return pObj;
    }

    for ( pMob = pRoom->people; pMob != NULL; pMob = pMob->next_in_room )
    {
        if ( !IS_NPC( pMob ) )
            continue;

        for ( pObj = pMob->carrying; pObj != NULL; pObj = pObj->next_content )
        {
            if ( pObj->pIndexData == pObjIndex )
                return pObj;
        }
    }

    return NULL;
}


/*
 * Find some object with a given index data in global object list.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *
get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_DATA *obj;

    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	if ( obj->pIndexData == pObjIndex )
	    return obj;
    }

    return NULL;
}


/*
 * Find an obj in a list.
 */
OBJ_DATA *
get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}


/*
 * Get an object from a bank
 */
OBJ_DATA *
get_obj_bank( CHAR_DATA *ch, BANK_DATA *bank, char *argument )
{
    OBJ_DATA *	obj;
    char	arg[MAX_INPUT_LENGTH];
    int		number;
    int		count;

    if ( bank == NULL )
    {
	bug( "Get_obj_bank: NULL bank.", 0 );
	return NULL;
    }

    number = number_argument( argument, arg );
    count = 0;

    for ( obj = bank->storage; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj )
	&&   is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}


/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *
get_obj_carry( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->wear_loc == WEAR_NONE
	&&   (can_see_obj( viewer, obj ) ) 
	&&   is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}


/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *
get_obj_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->wear_loc != WEAR_NONE
	&&   can_see_obj( ch, obj )
	&&   is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}


/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *
get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    obj = get_obj_list( ch, argument, ch->in_room->contents );
    if ( obj != NULL )
	return obj;

    if ( ( obj = get_obj_carry( ch, argument, ch ) ) != NULL )
	return obj;

    if ( ( obj = get_obj_wear( ch, argument ) ) != NULL )
	return obj;

    return NULL;
}


/*
 * Find an obj in the world.
 */
OBJ_DATA *
get_obj_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    if ( ( obj = get_obj_here( ch, argument ) ) != NULL )
	return obj;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}


/*
 * Find a room in a room
 */
ROOM_INDEX_DATA *
get_room_room( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *	room;
    int			number;
    int			count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( room = ch->in_room->next_in_room; room != NULL; room = room->next_in_room )
	if ( can_see_room( ch, room ) && is_name( arg, room->name ) )
	    if ( ++count == number )
		return room;

    return NULL;
}


/*
 * deduct cost from a character
 */
void
deduct_cost( CHAR_DATA *ch, int cost )
{
    MONEY	amt;

    amt.gold	= 0;
    amt.silver = 0;
    amt.copper	= 0;
    amt.fract	= cost;
    normalize( &amt );

    while ( ch->money.fract < amt.fract )
    {
	ch->money.fract += FRACT_PER_COPPER;
	ch->money.copper--;
    }
    while( ch->money.copper < amt.copper )
    {
	ch->money.copper += COPPER_PER_SILVER;
	ch->money.silver--;
    }
    while( ch->money.silver < amt.silver )
    {
	ch->money.silver += SILVER_PER_GOLD;
	ch->money.gold--;
    }

    money_subtract( &ch->money, &amt, FALSE );

    if ( ch->money.gold < 0 )
    {
	bugf( "Deduct_cost: %s %d < 0", GOLD_NOUN, ch->money.gold );
	ch->money.gold = 0;
    }
}   


/*
 * Create a 'money' obj.
 */
OBJ_DATA *
create_money( MONEY *money )
{
    char	buf[MAX_STRING_LENGTH];
    OBJ_DATA *	obj;
    int		gold;
    int		silver;
    int		copper;
    int		fract;

    gold   = money->gold;
    silver = money->silver;
    copper = money->copper;
    fract  = money->fract;
    if ( gold  < 0 || silver  < 0 || copper  < 0 || fract  < 0
    || ( gold == 0 && silver == 0 && copper == 0 && fract == 0 ) )
    {
	bug( "Create_money: zero or negative money.", 0 );
	gold   = UMAX( 0, gold );
	silver = UMAX( 0, silver );
	copper = UMAX( 0, copper );
	fract  = UMAX( 0, fract );
	if ( gold + silver + copper + fract < 1 )
	    fract = 1;
    }

    if ( silver == 0 && copper == 0 && fract == 0 )
    {
	if ( gold == 1 )
	    obj = create_object( get_obj_index( OBJ_VNUM_GOLD_ONE ), 0 );
	else
	{
	    obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0 );
	    sprintf( buf, obj->short_descr, gold );
	    free_string( obj->short_descr );
	    obj->short_descr	= str_dup( buf );
	    obj->value[0]	= gold;
	    obj->cost		= gold * FRACT_PER_COPPER * COPPER_PER_SILVER * SILVER_PER_GOLD;
	    obj->weight		= gold * 8;
	}
    }
    else if ( gold == 0 && copper == 0 && fract == 0 )
    {
	if ( silver == 1 )
	    obj = create_object( get_obj_index( OBJ_VNUM_SILVER_ONE ), 0 );
	else
	{
	    obj = create_object( get_obj_index( OBJ_VNUM_SILVER_SOME ), 0 );
	    sprintf( buf, obj->short_descr, silver );
	    free_string( obj->short_descr );
	    obj->short_descr	= str_dup( buf );
	    obj->value[1]	= silver;
	    obj->cost		= silver * FRACT_PER_COPPER * COPPER_PER_SILVER;
	    obj->weight		= silver * 8;
	}
    }
    else if ( gold == 0 && silver == 0 && fract == 0 )
    {
	if ( copper == 1 )
	    obj = create_object( get_obj_index( OBJ_VNUM_COPPER_ONE ), 0 );
	else
	{
	    obj = create_object( get_obj_index( OBJ_VNUM_COPPER_SOME ), 0 );
	    sprintf( buf, obj->short_descr, copper );
	    free_string( obj->short_descr );
	    obj->short_descr	= str_dup( buf );
	    obj->value[2]	= copper;
	    obj->cost		= copper * FRACT_PER_COPPER;
	    obj->weight		= copper * 8;
	}
    }
    else if ( gold == 0 && silver == 0 && copper == 0 )
    {
	if ( fract == 1 )
	    obj = create_object( get_obj_index( OBJ_VNUM_FRACT_ONE ), 0 );
	else
	{
	    obj = create_object( get_obj_index( OBJ_VNUM_FRACT_SOME ), 0 );
	    sprintf( buf, obj->short_descr, fract );
	    free_string( obj->short_descr );
	    obj->short_descr	= str_dup( buf );
	    obj->value[3]	= fract;
	    obj->cost		= fract;
	    obj->weight		= fract;
	}
    }

    else
    {
	obj = create_object( get_obj_index( OBJ_VNUM_COINS ), 0 );
	obj->value[0]		= gold;
	obj->value[1]		= silver;
	obj->value[2]		= copper;
	obj->value[3]		= fract;
	obj->cost		= gold    * FRACT_PER_COPPER * COPPER_PER_SILVER * SILVER_PER_GOLD
				+ silver * FRACT_PER_COPPER * COPPER_PER_SILVER
				+ copper   * FRACT_PER_COPPER
				+ fract;
	obj->weight		= gold * 8 + silver * 8 + copper * 8 + fract;
    }

    return obj;
}


/*
 * Return # of objects which an object counts as.
 * Works like get_obj_number bout counts containers as 1.
 */
int
get_obj_count( OBJ_DATA *obj )
{
    int number;

    if ( obj->item_type == ITEM_MONEY ||  obj->item_type == ITEM_GEM
    ||   obj->item_type == ITEM_JEWELRY )
        number = 0;
    else
        number = 1;

    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
        number += get_obj_count( obj );

    return number;
}


/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int
get_obj_number( OBJ_DATA *obj )
{
    int number;

    if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_MONEY
    ||  obj->item_type == ITEM_GEM || obj->item_type == ITEM_JEWELRY)
        number = 0;
    else
        number = 1;

    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
        number += get_obj_number( obj );

    return number;
}


/*
 * Return weight of an object, including weight of contents.
 */
int
get_obj_weight( OBJ_DATA *obj )
{
    int weight;
    OBJ_DATA *tobj;

    weight = obj->weight;
    if ( obj->item_type == ITEM_DRINK_CON )
        weight += obj->value[1];
    for ( tobj = obj->contains; tobj != NULL; tobj = tobj->next_content )
	weight += get_obj_weight( tobj ) * WEIGHT_MULT( obj ) / 100;

    return weight;
}


int
get_true_weight( OBJ_DATA *obj )
{
    int weight;

    weight = obj->weight;
    if ( obj->item_type == ITEM_DRINK_CON )
        weight += obj->value[1];
    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
        weight += get_obj_weight( obj );

    return weight;
}


/*
 * True if room is dark.
 */
bool
room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    if ( pRoomIndex->light > 0 )
	return FALSE;

    if ( IS_SET( pRoomIndex->room_flags, ROOM_DARK ) )
	return TRUE;

    if ( pRoomIndex->sector_type == SECT_CITY
    &&	 !IS_SET( pRoomIndex->room_flags, ROOM_SUNLIGHT ) )
	return FALSE;

    if ( pRoomIndex->sector_type == SECT_INSIDE
    &&	 !IS_SET( pRoomIndex->room_flags, ROOM_SUNLIGHT ) )
	return FALSE;

    if ( pRoomIndex->sector_type == SECT_UNDERGROUND
    &&	 !IS_SET( pRoomIndex->room_flags, ROOM_SUNLIGHT ) )
	return TRUE;

    if ( weather_info.sunlight == SUN_SET
    ||   weather_info.sunlight == SUN_DARK )
	return TRUE;

    return FALSE;
}


bool
is_metal( OBJ_DATA *obj )
{
    if ( obj == NULL )
        return FALSE;

    if ( xIS_SET( obj->material, MAT_BRASS )
    ||	 xIS_SET( obj->material, MAT_BRONZE )
    ||	 xIS_SET( obj->material, MAT_COPPER )
    ||	 xIS_SET( obj->material, MAT_GOLD )
    ||   xIS_SET( obj->material, MAT_IRON )
    ||   xIS_SET( obj->material, MAT_LEAD )
    ||   xIS_SET( obj->material, MAT_MERCURY )
    ||   xIS_SET( obj->material, MAT_MITHRIL )
    ||   xIS_SET( obj->material, MAT_PLATINUM )
    ||   xIS_SET( obj->material, MAT_SILVER )
    ||   xIS_SET( obj->material, MAT_STEEL )
    ||   xIS_SET( obj->material, MAT_TIN )
    ||   xIS_SET( obj->material, MAT_ZINC ) )
        return TRUE;

    return FALSE;
}


bool
is_room_owner(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
{
    if (room->owner == NULL || room->owner[0] == '\0')
	return FALSE;

    return is_name(ch->name,room->owner);
}

/*
 * True if room is private.
 */
bool
room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;
    int count;


    if (pRoomIndex->owner != NULL && pRoomIndex->owner[0] != '\0')
	return TRUE;

    count = 0;
    for ( rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room )
	count++;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)  && count >= 2 )
	return TRUE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) && count >= 1 )
	return TRUE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) )
	return TRUE;

    return FALSE;
}

/* visibility on a room -- for entering and exits */
bool
can_see_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    if (IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) 
    &&  get_trust(ch) < MAX_LEVEL)
	return FALSE;

    if (IS_SET(pRoomIndex->room_flags, ROOM_GODS_ONLY)
    &&  !IS_IMMORTAL(ch))
	return FALSE;

    if (IS_SET(pRoomIndex->room_flags, ROOM_HEROES_ONLY)
    &&  !IS_IMMORTAL(ch))
	return FALSE;

    if ( IS_SET( pRoomIndex->room_flags, ROOM_NEWBIES_ONLY )
    &&  ch->level > LEVEL_NEWBIE && !IS_IMMORTAL( ch ) )
	return FALSE;

    if ( !( IS_IMMORTAL( ch ) || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
	  && pRoomIndex->clan
	  && ( !ch->clan || ch->clan->vnum != pRoomIndex->clan ) )
    {
	return FALSE;
    }

    return TRUE;
}



/*
 * True if char can see victim.
 */
bool
can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
/* RT changed so that WIZ_INVIS has levels */
    if ( ch == victim )
	return TRUE;

    if ( get_trust(ch) < victim->invis_level )
	return FALSE;

    if ( IS_SET( victim->act2, PLR_PLOADED ) && !IS_IMMORTAL( ch ) )
	return FALSE;

    if ( get_trust( ch ) < victim->incog_level
    &&	 ch->in_room != victim->in_room )
	return FALSE;

    if ( IS_NPC( victim ) && victim->in_room != NULL
    &&	 victim->in_room->vnum == ROOM_VNUM_DISPOSAL
    &&	 !IS_CODER( ch ) )
        return FALSE;

    if ( (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) 
    ||   (IS_NPC(ch) && IS_IMMORTAL(ch)))
	return TRUE;

    if ( ch->in_room == victim->in_room
    &&	 IS_NPC( ch )
    &&	 ch->pIndexData->vnum == MOB_VNUM_SUPERMOB
    &&	 ( IS_NPC( victim ) || victim->invis_level <= ch->level ) )
	return TRUE;

    if ( !is_same_world( ch, victim ) )
	return FALSE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
	return FALSE;

    if ( ch->in_room != NULL && room_is_dark( ch->in_room ) && !IS_AFFECTED(ch, AFF_INFRARED) )
	return FALSE;

    if ( IS_AFFECTED(victim, AFF_INVISIBLE)
    &&   !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
	return FALSE;

#if 0
    /* sneaking */
    if ( IS_AFFECTED(victim, AFF_SNEAK)
    &&   !IS_AFFECTED(ch,AFF_DETECT_HIDDEN)
    &&   victim->fighting == NULL)
    {
	int chance;
	chance = get_skill(victim,gsn_sneak);
	chance += get_curr_stat(victim,STAT_DEX) * 3/2;
 	chance -= get_curr_stat(ch,STAT_INT) * 2;
	chance -= ch->level - victim->level * 3/2;

	if (number_percent() < chance)
	    return FALSE;
    }
#endif

    if ( IS_AFFECTED(victim, AFF_HIDE)
    &&   !IS_AFFECTED(ch, AFF_DETECT_HIDDEN)
    &&   victim->fighting == NULL)
	return FALSE;

    return TRUE;
}



/*
 * True if char can see obj.
 */
bool
can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( IS_SET( obj->extra_flags, ITEM_VIS_DEATH ) )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) && obj->item_type != ITEM_POTION )
	return FALSE;

    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
	return TRUE;

    if ( IS_SET( obj->extra_flags, ITEM_INVIS )
    &&   !IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
        return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_GLOW ) )
	return TRUE;

    if ( IS_OBJ_STAT( obj, ITEM_HIDDEN ) )
	return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_INVIS_DAY )
    &&	 time_info.hour >= hour_sunrise
    &&	 time_info.hour < hour_sunset
    &&	 !IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
	return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_INVIS_NIGHT )
    &&	 ( time_info.hour < hour_sunrise || time_info.hour >= hour_sunset )
    &&	 !IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
	return FALSE;

    if ( room_is_dark( ch->in_room ) && !IS_AFFECTED( ch, AFF_DARK_VISION ) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_SET(obj->extra_flags, ITEM_NODROP) )
	return TRUE;

    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
	return TRUE;
    if ( !IS_SET(obj->extra_flags,  ITEM_QUESTITEM) )
        return TRUE;

    return FALSE;
}


/*
 * Remove carriage returns and tildes from a string
 */
char *fix_string( const char *string )
{
    const char *p;
    char *q;
    char c;
    static char strfix[MAX_STRING_LENGTH*2];

    p = string;
    q = strfix;
    while ( ( c = *p ) != '\0' )
    {
	if ( c != '\r' && c != '~' )
	    *q++ = *p;
	p++;
    }
    *q = '\0';
    return strfix;
}


/*
 * Copies string s to string d, showing color codes.
 * s and d must not be the same string.
 */
char *
show_color_codes( char *d, const char *s )
{
    char *dst;

    dst = d;
    while ( *s != '\0' )
    {
	if ( *s == '`' )
	{
	    *d++ = *s++;
	    if ( *s == '\0' )
		break;
	    if ( *s == '`' )
	    {
		d = stpcpy( d, "```" );
		s++;
		continue;
	    }
	    if ( strchr( colorcode_list, *s ) )
	    {
		*d++ = *s++;
		d = stpcpy( d, "``" );
		*d++ = *s++;
	    }
	    else
	    {
		*d++ = *s++;
	    }
	}
	else
	    *d++ = *s++;
    }
    *d = '\0';
    return dst;
}


/* Copies string s to string d, removing color codes. */
/* s and d may be the same string. */
char *
strip_color( char *d, const char *s )
{
    char *p = d;

    while ( *s )
    {
	if ( is_colcode( s ) )
	{
	    s += 2;
	}
	else
	{
	    if ( *s == '`' && *( s + 1 ) != '\0' )
		s++;
	    *p++ = *s++;
	}
    }
    *p = '\0';
    return d;
}


/* Returns TRUE if *s contains any color codes. */
bool
has_colcode( const char *s )
{
    const char *p;

    p = s;
    while ( ( p = strchr( p, '`' ) ) != NULL )
    {
        p++;
        if ( *p == '\0' )
            return FALSE;
        if ( *p == '`' )
        {
            p++;
            continue;
        }
        if ( strchr( colorcode_list, *p ) )
            return TRUE;
    }

    return FALSE;
}


/* Returns TRUE if c[0] and c[1] are a color code. */
bool
is_colcode( const char *c )
{
    if ( *c != '`' )
	return FALSE;
    c++;
    if ( strchr( colorcode_list, *c ) )
	return TRUE;
    return FALSE;
}


char *
strip_trailing_comma( char *s )
{
    char *p;

    if ( IS_NULLSTR( s ) )
        return s;

    p = s + strlen( s ) - 1;
    while ( p > s && ( *p == ',' || *p == ';' ) )
    {
        *p = '\0';
        p--;
    }

    return s;
}


int
strlen_wo_col( const char *s )
{
    int count = 0;

    while ( *s != '\0' )
    {
	if ( is_colcode( s ) )
	{
	    s += 2;
	}
	else if ( *s == '`' && *( s + 1 ) == '`' )
	{
	    s += 2;
	    count++;
	}
	else if ( *s == '`' && *( s + 1 ) == '-' )
	{
	    s += 2;
	    count++;
	}
	else
	{
	    s++;
	    count++;
	}
    }
    return count;
}


char *
strncpy_wocol( char *dest, const char *src, int n )
{
    char *p = dest;

    if ( dest == NULL || src == NULL || n < 0 )
	return NULL;

    while ( n )
    {
	if ( *src == '\0' )
	    break;
	if ( is_colcode( src ) )
	{
	    *p++ = *src++;
	    *p++ = *src++;
	    continue;
	}
	if ( *src == '`' && *( src + 1 ) == '`' )
	{
	    *p++ = *src++;
	    *p++ = *src++;
	    n--;
	    continue;
	}
	*p++ = *src++;
	n--;
    }
    *p = '\0';
    return dest;
}

/*
 *  Copy a string from src to dst, converting to lowercase.
 *  Return pointer to dst.
 *  Src and dst may be the same string.
 */
char *str_lower( char *dst, const char *src )
{
    char  c;
    char *d;

    if ( dst == NULL )
	return dst;
    if ( src == NULL )
    {
	*dst = '\0';
	return dst;
    }

    d = dst;
    while( ( c = *src++ ) != '\0' )
    {
	*d++ = LOWER( c );
    }

    *d = '\0';
    return dst;
}


/*
 *  Copy a string from src to dst, converting to uppercase.
 *  Return pointer to dst.
 *  Src and dst may be the same string.
 */
char *str_upper( char *dst, const char *src )
{
    char  c;
    char *d;

    if ( dst == NULL )
	return dst;
    if ( src == NULL )
    {
	*dst = '\0';
	return dst;
    }

    d = dst;
    while( ( c = *src++ ) != '\0' )
    {
	*d++ = UPPER( c );
    }

    *d = '\0';
    return dst;
}


/*
 * Return ascii name of an affect location.
 */
char *
affect_loc_name( int location )
{
    switch ( location )
    {
    case APPLY_NONE:		return "none";
    case APPLY_STR:		return "strength";
    case APPLY_DEX:		return "dexterity";
    case APPLY_INT:		return "intelligence";
    case APPLY_WIS:		return "wisdom";
    case APPLY_CON:		return "constitution";
    case APPLY_SEX:		return "sex";
    case APPLY_CLASS:		return "class";
    case APPLY_LEVEL:		return "level";
    case APPLY_AGE:		return "age";
    case APPLY_SIZE:		return "size";
    case APPLY_MANA:		return "mana";
    case APPLY_HIT:		return "hp";
    case APPLY_MOVE:		return "moves";
    case APPLY_WEALTH:		return "wealth";
    case APPLY_EXP:		return "experience";
    case APPLY_AC:		return "armor class";
    case APPLY_HITROLL:		return "hit roll";
    case APPLY_DAMROLL:		return "damage roll";
    case APPLY_SAVES:		return "saves";
    case APPLY_SAVING_ROD:	return "save vs rod";
    case APPLY_SAVING_PETRI:	return "save vs petrification";
    case APPLY_SAVING_BREATH:	return "save vs breath";
    case APPLY_SAVING_SPELL:	return "save vs spell";
    case APPLY_SPELL_AFFECT:	return "none";
    case APPLY_REGENERATION:	return "regeneration";
    }

    bug( "Affect_location_name: unknown location %d.", location );
    return "(unknown)";
}



/*
 * Return ascii name of an affect bit vector.
 */
char *
affect_bit_name( EXT_BV vector )
{
    static char buf[512];
    char *	p;

    buf[0] = '\0';
    p = buf;
    if ( xIS_SET( vector, AFF_BLACK_MANTLE     ) ) p = stpcpy( p, " black_mantle"     );
    if ( xIS_SET( vector, AFF_BLIND         ) ) p = stpcpy( p, " blind"         );
    if ( xIS_SET( vector, AFF_INVISIBLE     ) ) p = stpcpy( p, " invisible"     );
    if ( xIS_SET( vector, AFF_DETECT_EVIL   ) ) p = stpcpy( p, " detect_evil"   );
    if ( xIS_SET( vector, AFF_DETECT_GOOD   ) ) p = stpcpy( p, " detect_good"   );
    if ( xIS_SET( vector, AFF_DETECT_INVIS  ) ) p = stpcpy( p, " detect_invis"  );
    if ( xIS_SET( vector, AFF_DETECT_MAGIC  ) ) p = stpcpy( p, " detect_magic"  );
    if ( xIS_SET( vector, AFF_DETECT_HIDDEN ) ) p = stpcpy( p, " detect_hidden" );
    if ( xIS_SET( vector, AFF_SANCTUARY     ) ) p = stpcpy( p, " sanctuary"     );
    if ( xIS_SET( vector, AFF_FAERIE_FIRE   ) ) p = stpcpy( p, " faerie_fire"   );
    if ( xIS_SET( vector, AFF_INFRARED      ) ) p = stpcpy( p, " infrared"      );
    if ( xIS_SET( vector, AFF_PEACE	    ) ) p = stpcpy( p, " peace"		);
    if ( xIS_SET( vector, AFF_CURSE         ) ) p = stpcpy( p, " curse"         );
    if ( xIS_SET( vector, AFF_POISON        ) ) p = stpcpy( p, " poison"        );
    if ( xIS_SET( vector, AFF_PROTECT_EVIL  ) ) p = stpcpy( p, " prot_evil"     );
    if ( xIS_SET( vector, AFF_PROTECT_GOOD  ) ) p = stpcpy( p, " prot_good"     );
    if ( xIS_SET( vector, AFF_SLEEP         ) ) p = stpcpy( p, " sleep"         );
    if ( xIS_SET( vector, AFF_SNEAK         ) ) p = stpcpy( p, " sneak"         );
    if ( xIS_SET( vector, AFF_HIDE          ) ) p = stpcpy( p, " hide"          );
    if ( xIS_SET( vector, AFF_CHARM         ) ) p = stpcpy( p, " charm"         );
    if ( xIS_SET( vector, AFF_FLYING        ) ) p = stpcpy( p, " flying"        );
    if ( xIS_SET( vector, AFF_PASS_DOOR     ) ) p = stpcpy( p, " pass_door"     );
    if ( xIS_SET( vector, AFF_BERSERK	    ) ) p = stpcpy( p, " berserk"	    );
    if ( xIS_SET( vector, AFF_CALM	    ) ) p = stpcpy( p, " calm"	    );
    if ( xIS_SET( vector, AFF_HASTE	    ) ) p = stpcpy( p, " haste"	    );
    if ( xIS_SET( vector, AFF_SLOW          ) ) p = stpcpy( p, " slow"          );
    if ( xIS_SET( vector, AFF_PLAGUE	    ) ) p = stpcpy( p, " plague" 	    );
    if ( xIS_SET( vector, AFF_CURSE_NATURE  ) ) p = stpcpy( p, " curse_of_nature" );
    if ( xIS_SET( vector, AFF_DANCING	    ) ) p = stpcpy( p, " dancing_lights" );
    if ( xIS_SET( vector, AFF_DARK_VISION   ) ) p = stpcpy( p, " dark_vision"   );
    if ( xIS_SET( vector, AFF_WEARINESS	    ) ) p = stpcpy( p, " weariness"   );
    if ( xIS_SET( vector, AFF_DREAM	    ) ) p = stpcpy( p, " dream"   );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}


char *
affect_bitval_name( int vector )
{
    static char buf[512];

    buf[0] = '\0';
     if ( vector == AFF_BLACK_MANTLE     ) strcat( buf, " black_mantle"     ); 
    if ( vector == AFF_BLIND         ) strcat( buf, " blind"         );
    if ( vector == AFF_INVISIBLE     ) strcat( buf, " invisible"     );
    if ( vector == AFF_DETECT_EVIL   ) strcat( buf, " detect_evil"   );
    if ( vector == AFF_DETECT_GOOD   ) strcat( buf, " detect_good"   );
    if ( vector == AFF_DETECT_INVIS  ) strcat( buf, " detect_invis"  );
    if ( vector == AFF_DETECT_MAGIC  ) strcat( buf, " detect_magic"  );
    if ( vector == AFF_DETECT_HIDDEN ) strcat( buf, " detect_hidden" );
    if ( vector == AFF_SANCTUARY     ) strcat( buf, " sanctuary"     );
    if ( vector == AFF_FAERIE_FIRE   ) strcat( buf, " faerie_fire"   );
    if ( vector == AFF_INFRARED      ) strcat( buf, " infrared"      );
    if ( vector == AFF_CURSE         ) strcat( buf, " curse"         );
    if ( vector == AFF_POISON        ) strcat( buf, " poison"        );
    if ( vector == AFF_PROTECT_EVIL  ) strcat( buf, " prot_evil"     );
    if ( vector == AFF_PROTECT_GOOD  ) strcat( buf, " prot_good"     );
    if ( vector == AFF_SLEEP         ) strcat( buf, " sleep"         );
    if ( vector == AFF_SNEAK         ) strcat( buf, " sneak"         );
    if ( vector == AFF_HIDE          ) strcat( buf, " hide"          );
    if ( vector == AFF_CHARM         ) strcat( buf, " charm"         );
    if ( vector == AFF_FLYING        ) strcat( buf, " flying"        );
    if ( vector == AFF_PASS_DOOR     ) strcat( buf, " pass_door"     );
    if ( vector == AFF_BERSERK	     ) strcat( buf, " berserk"	     );
    if ( vector == AFF_CALM	     ) strcat( buf, " calm"	     );
    if ( vector == AFF_HASTE	     ) strcat( buf, " haste"	     );
    if ( vector == AFF_SLOW          ) strcat( buf, " slow"          );
    if ( vector == AFF_PLAGUE	     ) strcat( buf, " plague" 	     );
    if ( vector == AFF_DARK_VISION   ) strcat( buf, " dark_vision"   );
    if ( vector == AFF_PEACE	     ) strcat( buf, " peace"	     );
    if ( vector == AFF_CURSE_NATURE  ) strcat( buf, " curse_of_nature" );
    if ( vector == AFF_DANCING	     ) strcat( buf, " dancing_lights");
    if ( vector == AFF_WEARINESS     ) strcat( buf, " weariness"      );
    if ( vector == AFF_DREAM         ) strcat( buf, " dream"      );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}


char *
shield_bit_name( EXT_BV vector )
{
    static char	buf[512];
    char *	p;

    p = buf;
    *p = '\0';

    if ( xIS_SET( vector, SHLD_ACID		) )	p = stpcpy( p, " acid shield" );
    if ( xIS_SET( vector, SHLD_BLADE		) )	p = stpcpy( p, " blade barrier" );
    if ( xIS_SET( vector, SHLD_FLAME		) )	p = stpcpy( p, " flame shield" );
    if ( xIS_SET( vector, SHLD_FORCE		) )	p = stpcpy( p, " force shield" );
    if ( xIS_SET( vector, SHLD_GHOST		) )	p = stpcpy( p, " ghost shield" );
    if ( xIS_SET( vector, SHLD_HOLY		) )	p = stpcpy( p, " holy shield" );
    if ( xIS_SET( vector, SHLD_ICE		) )	p = stpcpy( p, " ice shield" );
    if ( xIS_SET( vector, SHLD_LIGHT		) )	p = stpcpy( p, " light shield" );
    if ( xIS_SET( vector, SHLD_PROTECT_ACID	) )	p = stpcpy( p, " protect_acid" );
    if ( xIS_SET( vector, SHLD_PROTECT_AIR	) )	p = stpcpy( p, " protect_air" );
    if ( xIS_SET( vector, SHLD_PROTECT_COLD	) )	p = stpcpy( p, " protect_cold" );
    if ( xIS_SET( vector, SHLD_PROTECT_EARTH	) )	p = stpcpy( p, " protect_earth" );
    if ( xIS_SET( vector, SHLD_PROTECT_ENERGY	) )	p = stpcpy( p, " protect_energy" );
    if ( xIS_SET( vector, SHLD_PROTECT_EVIL	) )	p = stpcpy( p, " protect_evil" );
    if ( xIS_SET( vector, SHLD_PROTECT_FIRE	) )	p = stpcpy( p, " protect_fire" );
    if ( xIS_SET( vector, SHLD_PROTECT_GOOD	) ) 	p = stpcpy( p, " protect_good" );
    if ( xIS_SET( vector, SHLD_PROTECT_LIGHTNING ) )	p = stpcpy( p, " protect_lightning" );
    if ( xIS_SET( vector, SHLD_PROTECT_WATER	) )	p = stpcpy( p, " protect_water" );
    if ( xIS_SET( vector, SHLD_SANCTUARY	) ) 	p = stpcpy( p, " sanctuary" );
    if ( xIS_SET( vector, SHLD_BLACK_MANTLE	) ) 	p = stpcpy( p, " black_mantle" );
    if ( xIS_SET( vector, SHLD_SONIC		) )	p = stpcpy( p, " sonic shield" );
    if ( xIS_SET( vector, SHLD_STATIC		) )	p = stpcpy( p, " static shield" );
    if ( xIS_SET( vector, SHLD_UNHOLY		) )	p = stpcpy( p, " unholy shield" );
    if ( xIS_SET( vector, SHLD_BLADES		) )	p = stpcpy( p, " blade barrier" );

    return buf[0] != '\0' ? buf + 1 : "none";
}


char *
shield_bitval_name( int vector )
{
    if ( vector == SHLD_ACID		)	return "acid shield";
    if ( vector == SHLD_BLADE		)	return "blade barrier";
    if ( vector == SHLD_FLAME		)	return "flame shield";
    if ( vector == SHLD_FORCE		)	return "force shield";
    if ( vector == SHLD_GHOST		)	return "ghost shield";
    if ( vector == SHLD_HOLY		)	return "holy shield";
    if ( vector == SHLD_ICE		)	return "ice shield";
    if ( vector == SHLD_LIGHT		)	return "light shield";
    if ( vector == SHLD_PROTECT_ACID	)	return "protect_acid";
    if ( vector == SHLD_PROTECT_AIR	)	return "protect_air";
    if ( vector == SHLD_PROTECT_COLD	)	return "protect_cold";
    if ( vector == SHLD_PROTECT_EARTH	)	return "protect_earth";
    if ( vector == SHLD_PROTECT_ENERGY	)	return "protect_energy";
    if ( vector == SHLD_PROTECT_EVIL	)	return "protect_evil";
    if ( vector == SHLD_PROTECT_FIRE	)	return "protect_fire";
    if ( vector == SHLD_PROTECT_GOOD	) 	return "protect_good";
    if ( vector == SHLD_PROTECT_LIGHTNING )	return "protect_lightning";
    if ( vector == SHLD_PROTECT_WATER	)	return "protect_water";
    if ( vector == SHLD_SANCTUARY	) 	return "sanctuary";
    if ( vector == SHLD_BLACK_MANTLE	) 	return "black_mantle";
    if ( vector == SHLD_SONIC		)	return "sonic shield";
    if ( vector == SHLD_STATIC		)	return "static shield";
    if ( vector == SHLD_UNHOLY		)	return "unholy shield";
    if ( vector == SHLD_BLADES		)	return "blade barrier";
    return "none";
}


/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name( bitvector extra_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
    if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
    if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
    if ( extra_flags & ITEM_HIDDEN	 ) strcat( buf, " hidden"	);
    if ( extra_flags & ITEM_EVIL         ) strcat( buf, " evil"         );
    if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
    if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
    if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
    if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
    if ( extra_flags & ITEM_ANTI_GOOD    ) strcat( buf, " anti-good"    );
    if ( extra_flags & ITEM_ANTI_EVIL    ) strcat( buf, " anti-evil"    );
    if ( extra_flags & ITEM_ANTI_NEUTRAL ) strcat( buf, " anti-neutral" );
    if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
    if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
    if ( extra_flags & ITEM_NOPURGE	 ) strcat( buf, " nopurge"	);
    if ( extra_flags & ITEM_VIS_DEATH	 ) strcat( buf, " vis_death"	);
    if ( extra_flags & ITEM_ROT_DEATH	 ) strcat( buf, " rot_death"	);
    if ( extra_flags & ITEM_NOLOCATE	 ) strcat( buf, " no_locate"	);
    if ( extra_flags & ITEM_SELL_EXTRACT ) strcat( buf, " sell_extract" );
    if ( extra_flags & ITEM_BURN_PROOF	 ) strcat( buf, " burn_proof"	);
    if ( extra_flags & ITEM_NOUNCURSE	 ) strcat( buf, " no_uncurse"	);
    if ( extra_flags & ITEM_QUESTOBJ	 ) strcat( buf, " quest_item"	);
    if ( extra_flags & ITEM_NOLIST	 ) strcat( buf, " nolist"	);
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/*
 * Return ascii name of extra flags vector.
 */
char *extra2_bit_name( bitvector extra_flags2 )
{
    static char buf[512];

    buf[0] = '\0';
    if ( extra_flags2 & ITEM2_NONE         ) strcat( buf, " none"         );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/* return ascii name of an act vector */
char *
act_bit_name( bitvector act_flags )
{
    static char buf[512];

    buf[0] = '\0';

    if ( IS_SET( act_flags, ACT_IS_NPC ) )
    { 
 	strcat(buf," npc" );
	if ( act_flags & ACT_AGGRESSIVE	) strcat( buf, " aggressive" );
	if ( act_flags & ACT_ARTIFICER	) strcat( buf, " artificer" );
	if ( act_flags & ACT_IS_CHANGER	) strcat( buf, " changer" );
	if ( act_flags & ACT_GAIN	) strcat( buf, " gain" );
	if ( act_flags & ACT_IS_HEALER	) strcat( buf, " healer" );
	if ( act_flags & ACT_INDOORS	) strcat( buf, " indoors" );
	if ( act_flags & ACT_MOUNT	) strcat( buf, " mount" );
	if ( act_flags & ACT_NOALIGN	) strcat( buf, " no_align" );
	if ( act_flags & ACT_NOPURGE	) strcat( buf, " no_purge" );
	if ( act_flags & ACT_NOQUEST	) strcat( buf, " no_quest" );
	if ( act_flags & ACT_OUTDOORS	) strcat( buf, " outdoors" );
	if ( act_flags & ACT_PET	) strcat( buf, " pet" );
	if ( act_flags & ACT_PRACTICE	) strcat( buf, " practice" );
    	if ( act_flags & ACT_SCAVENGER	) strcat( buf, " scavenger" );
    	if ( act_flags & ACT_SENTINEL 	) strcat( buf, " sentinel" );
	if ( act_flags & ACT_GAIN	) strcat( buf, " skill_train" );
	if ( act_flags & ACT_STAY_AREA	) strcat( buf, " stay_area" );
	if ( act_flags & ACT_TETHERED	) strcat( buf, " tethered" );
	if ( act_flags & ACT_TRAIN	) strcat( buf, " train" );
	if ( act_flags & ACT_UNDEAD	) strcat( buf, " undead" );
	if ( act_flags & ACT_UPDATE_ALWAYS) strcat( buf," update_always" );
	if ( act_flags & ACT_WIMPY	) strcat( buf, " wimpy" );
    }
    else
    {
	strcat(buf," player" );
	if (act_flags & PLR_AUTOASSIST	) strcat(buf, " autoassist" );
	if (act_flags & PLR_AUTOEXIT	) strcat(buf, " autoexit" );
	if (act_flags & PLR_AUTOLOOT	) strcat(buf, " autoloot" );
	if (act_flags & PLR_AUTOSAC	) strcat(buf, " autosac" );
	if (act_flags & PLR_AUTOCOINS	) strcat(buf, " autocoins" );
	if (act_flags & PLR_AUTOSPLIT	) strcat(buf, " autosplit" );
	if (act_flags & PLR_BLINK	) strcat(buf, " blink" );
	if (act_flags & PLR_HOLYLIGHT	) strcat(buf, " holy_light" );
	if (act_flags & PLR_CANLOOT	) strcat(buf, " loot_corpse" );
	if (act_flags & PLR_NOSUMMON	) strcat(buf, " no_summon" );
	if (act_flags & PLR_NOFOLLOW	) strcat(buf, " no_follow" );
	if (act_flags & PLR_FREEZE	) strcat(buf, " frozen" );
	if (act_flags & PLR_THIEF	) strcat(buf, " thief" );
	if (act_flags & PLR_KILLER	) strcat(buf, " killer" );
	//if (act_flags & PLR_AUTOTICK	) strcat(buf, " autotick" );
    }
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *
comm_bit_name( int comm_flags )
{
    static char buf[512];

    buf[0] = '\0';

    if ( comm_flags & COMM_QUIET	) strcat( buf, " quiet" );
    if ( comm_flags & COMM_DEAF		) strcat( buf, " deaf" );
    if ( comm_flags & COMM_DETAIL	) strcat( buf, " detail" );
    if ( comm_flags & COMM_COMPACT	) strcat( buf, " compact" );
    if ( comm_flags & COMM_BRIEF	) strcat( buf, " brief" );
    if ( comm_flags & COMM_PROMPT	) strcat( buf, " prompt" );
    if ( comm_flags & COMM_COMBINE	) strcat( buf, " combine" );
    if ( comm_flags & COMM_TELNET_GA	) strcat( buf, " telnet_ga" );
    if ( comm_flags & COMM_SHOW_AFFECTS	) strcat( buf, " affects" );
    if ( comm_flags & COMM_VNUMS	) strcat( buf, " vnums" );
    if ( comm_flags & COMM_NOEMOTE	) strcat( buf, " no_emote" );
    if ( comm_flags & COMM_NOSHOUT	) strcat( buf, " no_shout" );
    if ( comm_flags & COMM_NOTELL	) strcat( buf, " no_tell" );
    if ( comm_flags & COMM_NOCHANNELS	) strcat( buf, " no_channels" );

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *imm_bit_name( bitvector imm_flags )
{
    static char buf[512];

    buf[0] = '\0';

    if (imm_flags & IMM_SUMMON		) strcat(buf, " summon" );
    if (imm_flags & IMM_CHARM		) strcat(buf, " charm" );
    if (imm_flags & IMM_MAGIC		) strcat(buf, " magic" );
    if (imm_flags & IMM_WEAPON		) strcat(buf, " weapon" );
    if (imm_flags & IMM_BASH		) strcat(buf, " blunt" );
    if (imm_flags & IMM_PIERCE		) strcat(buf, " piercing" );
    if (imm_flags & IMM_SLASH		) strcat(buf, " slashing" );
    if (imm_flags & IMM_FIRE		) strcat(buf, " fire" );
    if (imm_flags & IMM_COLD		) strcat(buf, " cold" );
    if (imm_flags & IMM_LIGHTNING	) strcat(buf, " lightning" );
    if (imm_flags & IMM_ACID		) strcat(buf, " acid" );
    if (imm_flags & IMM_POISON		) strcat(buf, " poison" );
    if (imm_flags & IMM_NEGATIVE	) strcat(buf, " negative" );
    if (imm_flags & IMM_HOLY		) strcat(buf, " holy" );
    if (imm_flags & IMM_ENERGY		) strcat(buf, " energy" );
    if (imm_flags & IMM_MENTAL		) strcat(buf, " mental" );
    if (imm_flags & IMM_DISEASE	) strcat(buf, " disease" );
    if (imm_flags & IMM_DROWNING	) strcat(buf, " drowning" );
    if (imm_flags & IMM_LIGHT		) strcat(buf, " light" );
    if (imm_flags & VULN_IRON		) strcat(buf, " iron" );
    if (imm_flags & VULN_WOOD		) strcat(buf, " wood" );
    if (imm_flags & VULN_SILVER	) strcat(buf, " silver" );

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *wear_bit_name( bitvector wear_flags )
{
    static char buf[512];

    buf [0] = '\0';
    if (wear_flags & ITEM_TAKE		) strcat(buf, " take" );
    if (wear_flags & ITEM_WEAR_FINGER	) strcat(buf, " finger" );
    if (wear_flags & ITEM_WEAR_NECK	) strcat(buf, " neck" );
    if (wear_flags & ITEM_WEAR_BODY	) strcat(buf, " torso" );
    if (wear_flags & ITEM_WEAR_HEAD	) strcat(buf, " head" );
    if (wear_flags & ITEM_WEAR_LEGS	) strcat(buf, " legs" );
    if (wear_flags & ITEM_WEAR_ANKLE	) strcat( buf, " ankles" );
    if (wear_flags & ITEM_WEAR_FEET	) strcat(buf, " feet" );
    if (wear_flags & ITEM_WEAR_HANDS	) strcat(buf, " hands" );
    if (wear_flags & ITEM_WEAR_ARMS	) strcat(buf, " arms" );
    if (wear_flags & ITEM_WEAR_EAR	) strcat( buf, " ears" );
    if (wear_flags & ITEM_WEAR_SHIELD	) strcat(buf, " shield" );
    if (wear_flags & ITEM_WEAR_ABOUT	) strcat(buf, " body" );
    if (wear_flags & ITEM_WEAR_WAIST	) strcat(buf, " waist" );
    if (wear_flags & ITEM_WEAR_WRIST	) strcat(buf, " wrist" );
    if (wear_flags & ITEM_WIELD		) strcat(buf, " wield" );
    if (wear_flags & ITEM_HOLD		) strcat(buf, " hold" );
    if (wear_flags & ITEM_NO_SAC	) strcat(buf, " nosac" );
    if ( wear_flags & ITEM_WEAR_FACE	) strcat( buf, " face" );
    if ( wear_flags & ITEM_WEAR_CONTACT	) strcat( buf, " eyes" );
    if ( wear_flags & ITEM_WEAR_SHOULDER) strcat( buf, " shoulder" );
    if ( wear_flags & ITEM_WEAR_CPATCH	) strcat( buf, " clan" );
    if ( wear_flags & ITEM_WEAR_RPATCH	) strcat( buf, " religion" );
    if (wear_flags & ITEM_WEAR_FLOAT	) strcat( buf, " float" );
    if (wear_flags & ITEM_WEAR_TAIL	) strcat( buf, " tail" );
    if (wear_flags & ITEM_WEAR_HORNS	) strcat( buf, " horns" );

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *form_bit_name( bitvector form_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if (form_flags & FORM_POISON	) strcat(buf, " poison" );
    else if (form_flags & FORM_EDIBLE	) strcat(buf, " edible" );
    if (form_flags & FORM_MAGICAL	) strcat(buf, " magical" );
    if (form_flags & FORM_INSTANT_DECAY	) strcat(buf, " instant_rot" );
    if (form_flags & FORM_OTHER		) strcat(buf, " other" );
    if (form_flags & FORM_ANIMAL	) strcat(buf, " animal" );
    if (form_flags & FORM_SENTIENT	) strcat(buf, " sentient" );
    if (form_flags & FORM_UNDEAD	) strcat(buf, " undead" );
    if (form_flags & FORM_CONSTRUCT	) strcat(buf, " construct" );
    if (form_flags & FORM_MIST		) strcat(buf, " mist" );
    if (form_flags & FORM_INTANGIBLE	) strcat(buf, " intangible" );
    if (form_flags & FORM_BIPED		) strcat(buf, " biped" );
    if (form_flags & FORM_CENTAUR	) strcat(buf, " centaur" );
    if (form_flags & FORM_INSECT	) strcat(buf, " insect" );
    if (form_flags & FORM_SPIDER	) strcat(buf, " spider" );
    if (form_flags & FORM_CRUSTACEAN	) strcat(buf, " crustacean" );
    if (form_flags & FORM_WORM		) strcat(buf, " worm" );
    if (form_flags & FORM_BLOB		) strcat(buf, " blob" );
    if (form_flags & FORM_MAMMAL	) strcat(buf, " mammal" );
    if (form_flags & FORM_BIRD		) strcat(buf, " bird" );
    if (form_flags & FORM_REPTILE	) strcat(buf, " reptile" );
    if (form_flags & FORM_SNAKE		) strcat(buf, " snake" );
    if (form_flags & FORM_DRAGON	) strcat(buf, " dragon" );
    if (form_flags & FORM_AMPHIBIAN	) strcat(buf, " amphibian" );
    if (form_flags & FORM_FISH		) strcat(buf, " fish" );
    if (form_flags & FORM_COLD_BLOOD 	) strcat(buf, " cold_blooded" );

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *part_bit_name( bitvector part_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if (part_flags & PART_HEAD		) strcat(buf, " head");
    if (part_flags & PART_ARMS		) strcat(buf, " arms");
    if (part_flags & PART_LEGS		) strcat(buf, " legs");
    if (part_flags & PART_HEART		) strcat(buf, " heart");
    if (part_flags & PART_BRAINS	) strcat(buf, " brains");
    if (part_flags & PART_GUTS		) strcat(buf, " guts");
    if (part_flags & PART_HANDS		) strcat(buf, " hands");
    if (part_flags & PART_FEET		) strcat(buf, " feet");
    if (part_flags & PART_FINGERS	) strcat(buf, " fingers");
    if (part_flags & PART_EAR		) strcat(buf, " ears");
    if (part_flags & PART_EYE		) strcat(buf, " eyes");
    if (part_flags & PART_LONG_TONGUE	) strcat(buf, " long_tongue");
    if (part_flags & PART_EYESTALKS	) strcat(buf, " eyestalks");
    if (part_flags & PART_TENTACLES	) strcat(buf, " tentacles");
    if (part_flags & PART_FINS		) strcat(buf, " fins");
    if (part_flags & PART_WINGS		) strcat(buf, " wings");
    if (part_flags & PART_TAIL		) strcat(buf, " tail");
    if (part_flags & PART_CLAWS		) strcat(buf, " claws");
    if (part_flags & PART_FANGS		) strcat(buf, " fangs");
    if (part_flags & PART_HORNS		) strcat(buf, " horns");
    if (part_flags & PART_SCALES	) strcat(buf, " scales");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *weapon_bit_name( bitvector weapon_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if (weapon_flags & WEAPON_FLAMING	) strcat(buf, " flaming");
    if (weapon_flags & WEAPON_FROST	) strcat(buf, " frost");
    if (weapon_flags & WEAPON_VAMPIRIC	) strcat(buf, " vampiric");
    if (weapon_flags & WEAPON_SHARP	) strcat(buf, " sharp");
    if (weapon_flags & WEAPON_VORPAL	) strcat(buf, " vorpal");
    if (weapon_flags & WEAPON_TWO_HANDS ) strcat(buf, " two-handed");
    if (weapon_flags & WEAPON_SHOCKING 	) strcat(buf, " shocking");
    if (weapon_flags & WEAPON_POISON	) strcat(buf, " poison");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *cont_bit_name( bitvector cont_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (cont_flags & CONT_CLOSEABLE	) strcat(buf, " closable");
    if (cont_flags & CONT_PICKPROOF	) strcat(buf, " pickproof");
    if (cont_flags & CONT_CLOSED	) strcat(buf, " closed");
    if (cont_flags & CONT_LOCKED	) strcat(buf, " locked");

    return (buf[0] != '\0' ) ? buf+1 : "none";
}


char *
date_string( time_t date_val )
{
    static char	buf[SHORT_STRING_LENGTH];
    struct tm	time_data;

    time_data = *localtime( &date_val );

    sprintf( buf, "%04d/%02d/%02d",
	     time_data.tm_year + 1900,
	     time_data.tm_mon + 1,
	     time_data.tm_mday );
    return buf;
}


/*
 Returns TRUE if ch is ignoring vch.
 Need the IGNORE_SET to detect if an ignore record exists (do_who() etc ).
 */
bool
is_ignoring( CHAR_DATA *ch, CHAR_DATA *vch, bitvector flag )
{
    IGNORE_DATA *	pId;
    CHAR_DATA *		rch;

    if ( ch == NULL || vch == NULL )
        return FALSE;

    rch = ( ch->desc != NULL && ch->desc->original != NULL ? ch->desc->original : ch );

    if ( flag != IGNORE_SET )
    {
        if ( IS_NPC( rch ) || IS_NPC( vch ) )
            return FALSE;
        if ( get_trust( vch ) >= MAX_LEVEL )
            return FALSE;
        if ( IS_IMMORTAL( vch ) && get_trust( vch ) > get_trust( rch ) )
            return FALSE;
    }

    if ( rch->pcdata == NULL )
    	return FALSE;

    for ( pId = rch->pcdata->ignoring; pId != NULL; pId = pId->next )
    {
        if ( IS_SET( pId->flags, flag ) && !str_cmp( pId->name, vch->name ) )
            return TRUE;
    }

    return FALSE;
}


bool
is_donation_box( OBJ_DATA *obj )
{
    int		i;

    if ( obj == NULL )
        return FALSE;

    for ( i = 0; i < MAX_KINGDOM; i++ )
        if ( obj->pIndexData->vnum == kingdom_table[i].box )
            return TRUE;

    return FALSE;
}

bool can_pack(CHAR_DATA *ch)     
{                          
    OBJ_DATA *object;     
    bool found;     
      
    if ( ch->desc == NULL )     
        return TRUE;     
      
    if ( ch->level > HERO )     
        return TRUE;     

    /*                           
     * search the list of objects.     
     */     
    found = TRUE;      

    for ( object = ch->carrying; object != NULL; object = object->next_content )     
    {     
        if (object->pIndexData->vnum == OBJ_VNUM_SCHOOL_SATCHEL)     
            found = FALSE;     
    }     

    if (found)     
        return TRUE;     
                     
    return FALSE;                                                                                                       
}  
/*
 * Returns the landmass a room is in.
 */
int
get_landmass( ROOM_INDEX_DATA *room )
{
    OVERLAND_DATA *	overland;
    int			index;
    int			x;
    int			y;

    if ( room == NULL || room->area == NULL )
        return LAND_NONE;

    while ( room->in_room != NULL )
        room = room->in_room;

    if ( ( overland = room->area->overland ) == NULL )
        return room->area->continent;

    if ( room->sector_type == SECT_OCEAN || room->sector_type == SECT_BOILING )
        return LAND_OVERLAND;

    if ( room->vnum >= overland->x * overland->y + room->area->min_vnum )
        return room->area->continent;

    get_coordinates( room, &x, &y );

    for ( index = 0; landmass_table[index].landmass != LAND_NONE; index++ )
        if ( landmass_table[index].lx < x && landmass_table[index].rx > x
        &&   landmass_table[index].uy < y && landmass_table[index].ly > y )
            break;

    return landmass_table[index].landmass;
}


/*
 * Returns TRUE if src_room and dst_room are on the same landmass.
 */
bool
is_same_landmass( ROOM_INDEX_DATA *src_room, ROOM_INDEX_DATA *dst_room )
{
    int		src_land;
    int		dst_land;

    if ( src_room == NULL || dst_room == NULL )
        return FALSE;

    while ( src_room->in_room != NULL )
        src_room = src_room->in_room;
    while ( dst_room->in_room != NULL )
        dst_room = dst_room->in_room;

    if ( src_room->area == dst_room->area
    &&	 src_room->area->continent != LAND_OVERLAND )
        return TRUE;

    src_land = get_landmass( src_room );
    dst_land = get_landmass( dst_room );

    if ( src_land == LAND_OVERLAND )
        return FALSE;

    return src_land == dst_land;
}


/*
 * Returns TRUE if ch and victim are in the same world.
 */
bool
is_same_world( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int	world_c;
    int world_v;

    /* Deleted and invalid chars -- no */
    if ( ch == NULL || victim == NULL || ch->deleted || victim->deleted )
	return FALSE;

    /* either/both chars are imms -- yes */
    if ( IS_IMMORTAL( ch ) || IS_IMMORTAL( victim ) )
	return TRUE;

    if ( ch->in_room == NULL )
        return TRUE;	/* assume ch is web who */

    world_c = ch->in_room->area->world;
    world_v = victim->in_room->area->world;

    /* Both in same area -- yes */
    if ( ch->in_room->area == victim->in_room->area )
	return TRUE;

    /* either char is in all world area -- yes */
    if ( world_c == WORLD_ALL || world_v == WORLD_ALL )
	return TRUE;

    /* either char is in no_world area -- no */
    if ( world_c == WORLD_NONE || world_v == WORLD_NONE )
	return FALSE;

    /* chars are in same world -- yes; different worlds -- no */
    return world_c == world_v;
}


/*
 * Returns TRUE if the room is a water sector.
 */
bool
is_water( ROOM_INDEX_DATA *room )
{
    if ( room == NULL )
        return FALSE;

    if ( IS_WATER( room->sector_type ) )
        return TRUE;

    return FALSE;
}


/*
 * Returns the difference between two (time_t) times as a string.
 */
const char *
strtimediff( time_t start, time_t stop )
{
    static char	buf[SHORT_STRING_LENGTH];
    char *	p;
    time_t	temp;
    time_t	diff;
    int		days;
    int		hours;
    int		minutes;
    int		seconds;

    if ( start > stop )
    {
        temp = start;
        start = stop;
        stop = temp;
    }

    diff    = stop - start;
    days    = diff / 86400;	diff %= 86400;	/* 86400 seconds = 1 day    */
    hours   = diff / 3600;	diff %= 3600;	/* 3600  seconds = 1 hour   */
    minutes = diff / 60;	diff %= 60;	/* 60    seconds = 1 minute */
    seconds = diff;

    p = buf;

    if ( days )
        p += sprintf( p, "%d day%s ", days, days == 1 ? "" : "s" );
    if ( days || hours )
	p += sprintf( p, "%d hour%s ", hours, hours == 1 ? "" : "s" );
    if ( days || hours || minutes )
        p += sprintf( p, "%d minute%s ", minutes, minutes == 1 ? "" : "s" );
    if ( !days )
        p += sprintf( p, "%d second%s ", seconds, seconds == 1 ? "" : "s" );

    if ( p > buf && *(p-1) == ' ' )
        *(p-1) = '\0';

    return buf;
}


/*
 * Add two money structs
 */
int
money_add( MONEY *augend, MONEY *addend, bool fNormalize )
{
    augend->gold	+= addend->gold;
    augend->silver	+= addend->silver;
    augend->copper	+= addend->copper;
    augend->fract	+= addend->fract;
    if ( fNormalize )
	normalize( augend );

    return augend->fract
	 + augend->copper   * FRACT_PER_COPPER
	 + augend->silver * FRACT_PER_COPPER * COPPER_PER_SILVER;
}

/*
 * Compare two money amounts.
 * Returns a negative number if amt1 < amt2
 * Returns 0 if equal
 * Returns a positive number if amt1 > amt2
 */
int
money_compare( MONEY *amt1, MONEY *amt2 )
{
    MONEY a;
    MONEY b;
    int	  diff;

    a = *amt1;
    b = *amt2;
    normalize( &a );
    normalize( &b );
    if ( ( diff = a.gold - b.gold ) != 0 )
	return diff;
    if ( ( diff = a.silver - b.silver ) != 0 )
	return diff;
    if ( ( diff = a.copper - b.copper ) != 0 )
	return diff;
    return a.fract - b.fract;
}


/*
 * Multiply and divide a money struct
 * Result always normalized
 */
int
money_muldiv( MONEY *amt, int multiplier, int divisor )
{
    int		remainder;
    MONEY	tmp;

    if ( divisor == 0 )
    {
	bug( "Money_muldiv: zero divisor.", 0 );
	return 0;
    }

    tmp = *amt;
    normalize( &tmp );
    tmp.gold = UMIN( tmp.gold, MAX_GOLD );
    tmp.gold *= multiplier;
    remainder = tmp.gold % divisor;
    tmp.gold /= divisor;
    tmp.silver += remainder * SILVER_PER_GOLD;

    tmp.silver *= multiplier;
    remainder = tmp.silver % divisor;
    tmp.silver /= divisor;
    tmp.copper += remainder * COPPER_PER_SILVER;

    tmp.copper *= multiplier;
    remainder = tmp.copper % divisor;
    tmp.copper /= divisor;
    tmp.fract += FRACT_PER_COPPER;

    tmp.fract *= multiplier;
    tmp.fract /= divisor;

    normalize( &tmp );
    *amt = tmp;
    return tmp.gold    * FRACT_PER_COPPER * COPPER_PER_SILVER * SILVER_PER_GOLD
	 + tmp.silver * FRACT_PER_COPPER * COPPER_PER_SILVER
	 + tmp.copper   * FRACT_PER_COPPER
	 + tmp.fract;
}


/*
 * Subtract two money structs
 * Caller must check for negative values
 */
int
money_subtract( MONEY *minuend, MONEY *subtrahend, bool fNormalize )
{
    minuend->gold	-= subtrahend->gold;
    minuend->silver	-= subtrahend->silver;
    minuend->copper	-= subtrahend->copper;
    minuend->fract	-= subtrahend->fract;
    if ( fNormalize
    ||	 minuend->gold < 0 || minuend->silver < 0
    ||	 minuend->copper < 0 || minuend->fract < 0 )
	normalize( minuend );

    return minuend->fract
	 + minuend->copper   * FRACT_PER_COPPER
	 + minuend->silver * FRACT_PER_COPPER * COPPER_PER_SILVER;
}


/*
 * Convert a money struct to a printable string.
 * If fZero, include denominations whose value is zero.
 * if fShort, return string in the form "1g 2s 3c 4f"
 *    else return "1 gold 2 silvers 3 copperss 4 fracts"
 */
char *
money_string( MONEY *amt, bool fZero, bool fShort )
{
    static char	buf[SHORT_STRING_LENGTH];
    char *	p;

    buf[0] = '\0';
    p = buf;

    if ( fShort )
    {
	if ( fZero || amt->gold != 0 )
	    p += sprintf( p, " %d%c", amt->gold, GOLD_INITIAL );
	if ( fZero || amt->silver != 0 )
	    p += sprintf( p, " %d%c", amt->silver, SILVER_INITIAL );
	if ( fZero || amt->copper != 0 )
	    p += sprintf( p, " %d%c", amt->copper, COPPER_INITIAL );
	if ( fZero || amt->fract != 0 )
	    p += sprintf( p, " %d%c", amt->fract, FRACT_INITIAL );
	if ( buf[0] == '\0' )
	    sprintf( buf, " 0%c", FRACT_INITIAL );
	return &buf[1];
    }

    if ( amt->gold != 0 || fZero )
	p += sprintf( p, " %d %s", amt->gold, amt->gold == 1 ? GOLD_NOUN : GOLD_PLURAL );

    if ( fZero )
	p += sprintf( p, " %d %s", amt->silver, amt->silver == 1 ? SILVER_NOUN  : SILVER_PLURAL );
    else if ( amt->silver != 0 )
    {
	if ( amt->gold != 0 && amt->copper == 0 && amt->fract == 0 )
	    p = stpcpy( p, " and" );
	p += sprintf( p, " %d %s", amt->silver, amt->silver == 1 ? SILVER_NOUN : SILVER_PLURAL );
    }

    if ( fZero )
	p += sprintf( p, " %d %s", amt->copper, amt->copper == 1 ? COPPER_NOUN : COPPER_PLURAL );
    else if ( amt->copper != 0 )
    {
	if ( ( amt->gold != 0 || amt->silver != 0 ) && amt->fract == 0 )
	    p = stpcpy( p, " and" );
	p += sprintf( p, " %d %s", amt->copper, amt->copper == 1 ? COPPER_NOUN : COPPER_PLURAL );
    }

    if ( fZero )
	p += sprintf( p, " and %d %s", amt->fract, amt->fract == 1 ? FRACT_NOUN : FRACT_PLURAL );
    else if ( amt->fract != 0 )
    {
	if ( amt->gold != 0 || amt->silver != 0 || amt->copper != 0 )
	    p = stpcpy( p, " and" );
	p += sprintf( p, " %d %s", amt->fract, amt->fract == 1 ? FRACT_NOUN : FRACT_PLURAL );
    }

    if ( !fZero && amt->gold == 0 && amt->silver == 0
    &&	 amt->copper == 0 && amt->fract == 0 )
	strcpy( buf, " nothing" );

    return buf;
}


/*
 * Parses a money string of the forms
 * "2g3s4c5f"
 * "2 gold 3 silbver 4 copper 5 fract"
 * "15"  (15 fract)
 * Returns NULL if any part of argument is invalid.
 */
MONEY *
money_value( MONEY *money, const char *argument )
{
    char	buf[MAX_INPUT_LENGTH];
    int		gold;
    int		silver;
    int		copper;
    int		fract;
    const char *p;
    char *	q;
    int		value;

    if ( money == NULL || IS_NULLSTR( argument ) )
	return NULL;

    gold = 0;
    silver = 0;
    copper = 0;
    fract = 0;

    p = argument;
    while( isspace( *p ) )
	p++;
    while ( *p != '\0' )
    {
	/* get the value */
	if ( !isdigit( *p ) )
	    return NULL;
	value = atoi( p );
	while ( isdigit( *p ) )
	    p++;
	while ( isspace( *p ) )
	    p++;
	if ( *p == '\0' && gold == 0 && silver == 0 && copper == 0 && fract == 0 )
	{
	    /* special case for no denomination specified */
	    money->gold    = gold;
	    money->silver = silver;
	    money->copper   = copper;
	    money->fract	   = value;
	    return money;
	}

	/* get the denomination */
	if ( !isalpha( *p ) )
	    return NULL;
	q = buf;
	while ( isalpha( *p ) )
	    *q++ = *p++;
	*q = '\0';
	if ( ( !str_prefix( buf, GOLD_NOUN ) || !str_prefix( buf, GOLD_PLURAL ) )
        &&   gold == 0 )
	    gold = value;
	else if ( ( !str_prefix( buf, SILVER_NOUN ) || !str_prefix( buf, SILVER_PLURAL ) )
	&&	  silver == 0 )
	    silver = value;
	else if ( ( !str_prefix( buf, COPPER_NOUN ) || !str_prefix( buf, COPPER_PLURAL ) )
	          && copper == 0 )
	    copper = value;
	else if ( ( !str_prefix( buf, FRACT_NOUN ) || !str_prefix( buf, FRACT_PLURAL ) )
	          && fract == 0 )
	    fract = value;
	else
	    return NULL;

	while ( isspace( *p ) )
	    p++;
    }

    money->gold   = gold;
    money->silver = silver;
    money->copper = copper;
    money->fract  = fract;
    return money;
}


void
normalize( MONEY *money )
{
    int amt;

    if ( money->fract < 0 )
    {
	amt = -( ( money->fract - FRACT_PER_COPPER + 1 ) / FRACT_PER_COPPER );
	money->fract += amt * FRACT_PER_COPPER;
	money->copper -= amt;
    }
    if ( money->copper < 0 )
    {
	amt = -( ( money->copper - COPPER_PER_SILVER + 1 ) / COPPER_PER_SILVER );
	money->copper += amt * COPPER_PER_SILVER;
	money->silver -= amt;
    }
    if ( money->silver < 0 )
    {
	amt = -( ( money->silver - SILVER_PER_GOLD + 1 ) / SILVER_PER_GOLD );
	money->silver += amt * SILVER_PER_GOLD;
	money->gold -= amt;
    }
    money->copper   += money->fract / FRACT_PER_COPPER;
    money->silver += money->copper / COPPER_PER_SILVER;
    money->gold    += money->silver / SILVER_PER_GOLD;
    money->fract    %= FRACT_PER_COPPER;
    money->copper   %= COPPER_PER_SILVER;
    money->silver %= SILVER_PER_GOLD;

    if ( money->gold < 0 )
	bugf( "Normalize: negative %s: %d", GOLD_NOUN, money->gold );

    return;
}


void
update_known( CHAR_DATA *ch, CHAR_DATA *vch, bool fSet )
{
    if ( IS_NPC( ch ) || IS_NPC( vch ) )
        return;

    if ( ch->pcdata->greet_id == 0 )
        ch->pcdata->greet_id = new_greet_id( );

    if ( vch->pcdata->greet_id == 0 )
        vch->pcdata->greet_id = new_greet_id( );

    *( greet_table + greet_size * vch->pcdata->greet_id + ch->pcdata->greet_id ) = fSet;
    update_greet_data( );
    return;
}


static int
new_greet_id( void )
{
    int		i;
    bool *	p;
    int		iCount;

    for ( i = 1; i < greet_size; i++ )
    {
        if ( !greet_table[i] )
            break;
    }

    if ( i >= greet_size )
        expand_greet_table( );

    greet_table[i] = TRUE;
    greet_table[i * greet_size] = TRUE;

    p = greet_table + i + greet_size;
    for ( iCount = 1; iCount < greet_size; iCount++ )
    {
        *p = FALSE;
        p += greet_size;
    }

    p = greet_table + i * greet_size + 1;
    for ( iCount = 1; iCount < greet_size; iCount++ )
        *p++ = FALSE;

    return i;
}


static void
expand_greet_table( void )
{
    bool *	new_table;
    bool *	src;
    bool *	dest;
    int		new_size;
    int		i;

    new_size = greet_size + GREET_INCREMENT;
    if ( ( new_table = (bool *)malloc( new_size * new_size ) ) == NULL )
    {
        perror( "Expand_greet_table" );
        abort_mud( "Malloc failed." );
    }

    src = greet_table;
    dest = new_table;

    for ( i = 0; i < greet_size; i++ )
    {
        memcpy( dest, src, greet_size * sizeof( bool ) );
        src += greet_size;
        dest += greet_size;
        memset( dest, FALSE, GREET_INCREMENT * sizeof( bool ) );
        dest += GREET_INCREMENT;
    }

    memset( dest, FALSE, new_size * GREET_INCREMENT * sizeof( bool ) );

    free( greet_table );
    greet_table = new_table;
    greet_size = new_size;

    return;
}


void
update_greet_data( void )
{
    EVENT_DATA *	pEvent;

    if ( get_event_game( EVENT_GAME_SAVE_GREET ) != NULL )
	return;

    pEvent = new_event( );
    pEvent->fun = event_save_greet;
    pEvent->type = EVENT_GAME_SAVE_GREET;
    add_event_game( pEvent, 60 * PULSE_PER_SECOND );
    return;
}


static bool
event_save_greet( EVENT_DATA *pEvent )
{
    save_greet_data( );
    return TRUE;
}

USERLIST_DATA *
user_lookup( const char *name )
{
    USERLIST_DATA *pUser;

    for ( pUser = user_first; pUser != NULL; pUser = pUser->next )
    {
	if ( !str_cmp( pUser->name, name ) )
	    break;
    }

    return pUser;
}


const char *
weight_string( int weight )
{
    static char	buf[SHORT_STRING_LENGTH];
    int		lbs;
    int		oz;
    int		tenths;
    char *	p;

    if ( weight == 0 )
        return "0";

    p = buf;

    if ( weight < 0 )
    {
        weight = -weight;
        p = stpcpy( p, "-" );
    }

    if ( weight >= 160 * 5 )
    {
        weight += 5;		/* round to nearest ounce if > 5 lb */
        weight -= weight % 10;
    }

    if ( weight > 160 * 50 )
    {
        weight += 80;		/* round to nearest pound if > 50 lb */
        weight -= weight % 160;
    }

    tenths = weight % 10;
    weight /= 10;
    oz = weight % 16;
    weight /= 16;
    lbs = weight;

    if ( lbs != 0 )
        p += sprintf( p, "%dlb", lbs );

    if ( oz != 0 || tenths != 0 )
    {
        if ( lbs != 0 )
            *p++ = ' ';
        p += sprintf( p, "%d", oz );
        if ( tenths != 0 )
            p += sprintf( p, ".%d", tenths );
        p = stpcpy( p, "oz" );
    }

    *p = '\0';
    return buf;
}


/*
 * Return a weight value from a string.
 * Returns TRUE if string is a valid weight string.
 * Weight can be NULL, so the function can be used to simply validate a string.
 */
bool
weight_value( const char *str, int *weight )
{
    const char *p;
    int		decdigits;
    int		fract;
    int		negative;
    int		number;
    int		pounds;

    if ( IS_NULLSTR( str ) )
        return FALSE;

    p = str;
    negative = 1;

    while ( isspace( *p ) )
        p++;

    if ( *p == '-' )
    {
        negative = -1;
        p++;
    }
    else if ( *p == '+' )
        p++;

    if ( !isdigit( *p ) && *p != '.' )
        return FALSE;

    if ( is_number( p ) )
    {
        if ( weight != NULL )
            *weight = atoi( p ) * 10 * negative;
        return TRUE;
    }

    pounds = 0;
    number = 0;
    decdigits = 1;
    fract = 0;
    while ( isdigit( *p ) )
    {
        number *= 10;
        number += *p - '0';
        p++;
    }

    if ( *p == '.' )
    {
        p++;
        while ( isdigit( *p ) )
        {
            decdigits *= 10;
            fract *= 10;
            fract += *p - '0';
            p++;
        }
    }

    while ( isspace( *p ) )
        p++;

    if ( ( *p == '\0' )
    ||	 ( LOWER( *p )  == 'o' && LOWER( *(p+1) ) == 'z' ) )
    {
        number *= 10;
        number += ( 10 * fract ) / decdigits;
        if ( weight != NULL )
            *weight = number * negative;
        return TRUE;
    }

    if ( *p == '#' )
    {
        p++;
    }
    else if ( LOWER( *p ) == 'l' && LOWER( *(p+1) ) == 'b' )
    {
        p += 2;
        if ( LOWER( *p ) == 's' )
            p++;
    }

    pounds = 160 * number + ( 160 * fract ) / decdigits;

    number = 0;
    decdigits = 1;
    fract = 0;

    while ( isspace( *p ) )
        p++;

    if ( *p == '\0' )
    {
        if ( weight != NULL )
            *weight = pounds;
        return TRUE;
    }
    else if ( !isdigit( *p ) && *p != '.' )
        return FALSE;

    while ( isdigit( *p ) )
    {
        number *= 10;
        number += *p - '0';
        p++;
    }

    if ( *p == '.' )
    {
        p++;
        while ( isdigit( *p ) )
        {
            decdigits *= 10;
            fract *= 10;
            fract += *p - '0';
            p++;
        }
    }

    while ( isspace( *p ) )
        p++;

    if ( *p != '\0' && LOWER( *p ) != 'o' && LOWER( *(p+1) ) != 'z' )
        return FALSE;

    if ( weight != NULL )
        *weight = pounds + number * 10 + ( fract * 10 ) / decdigits;

    return TRUE;
}


/*
 * Extended Bitvector Routines					-Thoric
 */

/* check to see if the extended bitvector is completely empty */
bool
ext_is_empty( EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	if ( bits->bits[x] != 0 )
	    return FALSE;

    return TRUE;
}


void
ext_clear_bits( EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	bits->bits[x] = 0;
}


/* for use by xHAS_BITS() -- works like IS_SET() */
int
ext_has_bits( EXT_BV *var, EXT_BV *bits )
{
    int x, bit;

    for ( x = 0; x < XBI; x++ )
	if ( (bit=(var->bits[x] & bits->bits[x])) != 0 )
	    return bit;

    return 0;
}


/* for use by xSAME_BITS() -- works like == */
bool
ext_same_bits( EXT_BV *var, EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	if ( var->bits[x] != bits->bits[x] )
	    return FALSE;

    return TRUE;
}


/* for use by xSET_BITS() -- works like SET_BIT() */
void
ext_set_bits( EXT_BV *var, EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	var->bits[x] |= bits->bits[x];
}


/* for use by xREMOVE_BITS() -- works like REMOVE_BIT() */
void
ext_remove_bits( EXT_BV *var, EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	var->bits[x] &= ~(bits->bits[x]);
}


/* for use by xTOGGLE_BITS() -- works like TOGGLE_BIT() */
void
ext_toggle_bits( EXT_BV *var, EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	var->bits[x] ^= bits->bits[x];
}

