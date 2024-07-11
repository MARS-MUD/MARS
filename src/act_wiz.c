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


#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "interp.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "magic.h"

#define CH(descriptor)  ((descriptor)->original ? \
			(descriptor)->original : (descriptor)->character)

#define MAX_OLOAD		50

enum
{
    WHENCE_STAT,
    WHENCE_MSTAT,
    WHENCE_OSTAT,
    WHENCE_RSTAT,
};

extern  AFFECT_DATA        *affect_free;
extern MOB_INDEX_DATA *	mob_index_hash[];
extern OBJ_INDEX_DATA *	obj_index_hash[];

/*
 * Local functions.
 */
const char *		con_type_name	args( ( int con ) );
bool			obj_check 	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void			recursive_clone	args( ( CHAR_DATA *ch, OBJ_DATA *obj,
						OBJ_DATA *clone ) );
void	load_mob	args( ( CHAR_DATA *ch, char *argument,
				const char *cmd ) );
void	load_obj	args( ( CHAR_DATA *ch, char *argument,
				const char *cmd ) );
void	stat_mob	args( ( CHAR_DATA *ch, char *argument,
                                BUFFER *pBuf, int whence ) );
void	stat_obj	args( ( CHAR_DATA *ch, char *argument,
                                BUFFER *pBuf, int whence ) );


const char *
con_type_name( int con )
{
           /* NB: You may need to edit the CON_ values */
    switch( con )
    {
	case CON_PLAYING:		return "    PLAYING    ";
	case CON_GET_NAME:		return "   Get Name    ";
	case CON_GET_OLD_PASSWORD:	return "Get Old Passwd ";
	case CON_CONFIRM_NEW_NAME:	return " Confirm Name  ";
	case CON_GET_NEW_PASSWORD:	return "Get New Passwd ";
	case CON_CONFIRM_NEW_PASSWORD:	return "Confirm Passwd ";
	case CON_GET_NEW_LNAME:		return " Get Last Name ";
	case CON_GET_NEW_SEX:		return "  Get New Sex  ";
	case CON_GET_CONTINENT:		return " Get Continent ";
 	case CON_GET_NEW_RACE:		return "  Get New Race ";
	case CON_GET_NEW_CLASS:		return " Get New Class ";
	case CON_GET_DESCRS:		return "  Get Descrs   ";
	case CON_GET_SHORT_DESCR:	return "  Get Descrs   ";
	case CON_GET_LONG_DESCR:	return "  Get Descrs   ";
	case CON_ACK_ALIGNMENT:		return " Get New Align "; break;
	case CON_GET_ALIGNMENT:		return " Get New Align "; break;
	case CON_DEFAULT_CHOICE:	return " Choosing Cust "; break;
	case CON_GEN_GROUPS:		return " Customization "; break;
	case CON_PICK_WEAPON:		return " Picking Weapon"; break;
	case CON_READ_IMOTD:		return " Reading IMOTD "; break;
	case CON_BREAK_CONNECT:		return "   LINKDEAD    "; break;
	case CON_READ_MOTD:		return "  Reading MOTD "; break;
	case CON_GET_HAIR_COLOR:	return "Get Hair Color "; break;
	case CON_GET_EYE_COLOR:		return " Get Eye Color "; break;
	default:			return "   !UNKNOWN!   "; break;
    }
}


/* Scatters everything in the room to random rooms all over the mud */
/* Written by Dither of Planes of Dominion webmaster@podmud.com */

void do_scatter( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    ROOM_INDEX_DATA *pRoomIndex;
    bool found = FALSE;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( !IS_SET( obj->wear_flags, ITEM_TAKE ) )
	    continue;
	for ( ; ; )
	{
	    pRoomIndex = get_room_index( number_range( 0, 99999 ) );
	    if ( pRoomIndex != NULL )
		if ( CAN_ACCESS( pRoomIndex ) )
		    break;
	}
	found = TRUE;
	act( "$p vanishes in a puff of smoke!", ch, obj, NULL, TO_CHAR );
	act( "$p vanishes in a puff of smoke!", ch, obj, NULL, TO_ROOM );
	obj_from_room( obj );
	obj_to_room( obj, pRoomIndex );
	add_obj_fall_event( obj );
    }

    if ( !found )
	send_to_char( "Nothing here to scatter.\n\r", ch );

    return;
}


void do_wiznet( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    int		flag;
    bool	leftcol;
    int		mem_indx;
    char *	p;
    BUFFER *	pBuf;
    CHAR_DATA *	rch;
    char	word[SHORT_STRING_LENGTH];

    one_argument( argument, arg );

    rch = ch->desc == NULL || ch->desc->original == NULL ? ch : ch->desc->original;

    if ( arg[0] == '\0' )
    {
	if ( IS_SET( rch->wiznet, WIZ_ON ) )
	    do_wiznet( ch, "status" );
	else
	    do_wiznet( ch, "on" );
	return;
    }

    if ( !str_prefix( arg, "help" ) )
    {
	send_to_char( "Wiznet commands:\n\r", ch );
	send_to_char( "   on     - Sign onto Wiznet.\n\r", ch );
	send_to_char( "   off    - Sign off of Wiznet.\n\r", ch );
	send_to_char( "   all    - Set all available Wiznet channels ON.\n\r", ch );
	send_to_char( "   show   - Show all available Wiznet channels.\n\r", ch );
	send_to_char( "   status - Show currently selected Wiznet channels.\n\r", ch );
	send_to_char( "  <flags> - Toggle Wiznet channels.\n\r", ch );
	send_to_char( "   help   - Show this help.\n\r", ch );
      	return;
    }

    if ( !str_prefix( arg, "on" ) )
    {
	send_to_char( "Welcome to Wiznet!\n\r", ch );
	SET_BIT( rch->wiznet, WIZ_ON );
	return;
    }

    if ( !str_prefix( arg, "off" ) )
    {
	send_to_char( "Signing off of Wiznet.\n\r", ch );
	REMOVE_BIT( rch->wiznet, WIZ_ON );
	return;
    }

    for ( mem_indx = 0; wiznet_table[mem_indx].name != NULL; mem_indx++ )
	if ( wiznet_table[mem_indx].flag == WIZ_MEMORY )
	    break;

    /* turn all wiznet channels on */
    if ( !str_prefix( arg, "all" ) )
    {
	for ( flag = 0; wiznet_table[flag].name != NULL; flag++ )
	{
	    if ( wiznet_table[flag].flag == WIZ_MEMORY )
		continue;	/* Never autoset memory */
	    if ( wiznet_table[flag].level <= get_trust( rch ) )
	    {
		SET_BIT( rch->wiznet, wiznet_table[flag].flag );
	    }
	}
	send_to_char( "All Wiznet channels now ON.\n\r", ch );
	return;
    }

    /* list of all wiznet options */
    if ( !str_prefix( arg, "show" ) )
    {
	buf[0] = '\0';
	p = buf;

	send_to_char( "Wiznet options available to you are:\n\r", ch );
	for ( flag = 0; wiznet_table[flag].name != NULL; flag++ )
	{
	    if ( wiznet_table[flag].level <= get_trust( rch )
	    && ( IS_CODER( rch ) || flag != mem_indx ) )
	    {
		strcpy( word, IS_SET( rch->wiznet,wiznet_table[flag].flag )
			      ? all_capitalize( wiznet_table[flag].name )
			      : wiznet_table[flag].name );
	    	if ( p - buf + strlen( word ) > 78 )
	    	{
		    strcpy( p, "\n\r" );
		    send_to_char( buf, ch );
		    p = buf;
		    *p = '\0';
	    	}
	    	p = stpcpy( p, word );
	    	p = stpcpy( p," " );
	    }
	}

	strcpy( p, "\n\r" );

	send_to_char( buf, ch );
	return;
    }

    /* show wiznet status */
    if ( !str_prefix( arg, "status" ) ) 
    {
	pBuf = new_buf( );
	leftcol = TRUE;
	add_buf( pBuf,
		 "    8--------------------8--------------------8\n\r"
		 "    | Wiznet      Status | Wiznet      Status |\n\r"
		 "    8--------------------8--------------------8"  );

	for ( flag = 0; wiznet_table[flag].name != NULL; flag++ )
	{
	    if ( wiznet_table[flag].level <= get_trust( rch )
	    &&	 wiznet_table[flag].flag != WIZ_ON
	    && ( IS_CODER( rch ) || flag != mem_indx ) )
	    {
		if ( leftcol )
		    add_buf( pBuf, "\n\r    |" );
		leftcol = !leftcol;
		if ( IS_SET( rch->wiznet, wiznet_table[flag].flag ) )
		    buf_printf( pBuf, " `R%-14s  `GON `w|", all_capitalize( wiznet_table[flag].name ) );
		else
		    buf_printf( pBuf, " `R%-14s `Roff `w|", wiznet_table[flag].name );
	    }
	}

	if ( !leftcol )
	    add_buf( pBuf, "                    |" );
	add_buf( pBuf, "\n\r" );

	add_buf( pBuf, "    8--------------------8--------------------8\n\r" );
	add_buf( pBuf, "    8             `RWiznet `wis " );
	add_buf( pBuf, IS_SET( rch->wiznet, WIZ_ON ) ? "`GON " : "`Roff" );
	add_buf( pBuf, "`X               8\n\r" );
	add_buf( pBuf, "    8--------------------8--------------------8\n\r" );

	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return;
    }

    while( *argument != '\0' )
    {
	argument = one_argument( argument, arg );
	flag = wiznet_lookup( arg );

	if ( flag == -1 || get_trust( rch ) < wiznet_table[flag].level
	||   ( !IS_CODER( rch ) && flag == mem_indx ) )
	{
	    ch_printf( ch, "No such option: %s\n\r", arg );
	    continue;
	}
	if ( IS_SET( rch->wiznet, wiznet_table[flag].flag ) )
	{
	    ch_printf( ch, "You will no longer see `R%s`X on wiznet.\n\r",
		       wiznet_table[flag].name );
	    REMOVE_BIT( rch->wiznet, wiznet_table[flag].flag );
	}
	else
	{
	    ch_printf( ch, "You will now see `R%s`X on wiznet.\n\r",
		       all_capitalize( wiznet_table[flag].name ) );
	    SET_BIT(rch->wiznet,wiznet_table[flag].flag);
	}
    }

    return;
}


void
wiznet( char *string, CHAR_DATA *ch, void *vo,
	    bitvector flag, bitvector flag_skip, int min_level ) 
{
    DESCRIPTOR_DATA *	d;
    int			old_color;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING
        &&   d->character != NULL
	&&   IS_IMMORTAL( d->character ) 
	&&   IS_SET( d->character->wiznet, WIZ_ON ) 
	&&   ( !flag || IS_SET( d->character->wiznet, flag ) )
	&&   ( !flag_skip || !IS_SET( d->character->wiznet, flag_skip ) )
	&&   get_trust( d->character ) >= min_level
	&&   d->character != ch )
        {
	    if ( IS_SET( d->character->wiznet, WIZ_PREFIX ) )
	    {
	  	old_color = set_char_color( AT_WIZNET, d->character );
	  	send_to_char( "`C--> ",d->character );
	  	set_char_color( old_color, d->character );
	    }
	    act_color( AT_WIZNET, string, d->character, vo, ch, TO_CHAR, POS_DEAD );
        }
    }

    return;
}

void do_guild( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CLAN_DATA *pClan;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: guild <char> <cln name>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }

    if (!str_prefix(arg2,"none"))
    {
	send_to_char("They are now clanless.\n\r",ch);
	send_to_char("You are now a member of no clan!\n\r",victim);
	if ( victim->clan )
	    victim->clan->members--;
	victim->clan = NULL;
	victim->clvl = 0;
	sql_update_player_clan( victim );
	return;
    }

    if ( ( pClan = get_clan( arg2 ) ) == NULL )
    {
	send_to_char( "No such clan exists.\n\r", ch );
	return;
    }

    ch_printf( ch, "They are now a member of clan %s.\n\r", pClan->name );
    ch_printf( victim, "You are now a member of clan %s.\n\r", pClan->name );

    victim->clan = pClan;
    pClan->members++;
    sql_update_player_clan( victim );
}

/* equips a character */
void
do_outfit ( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *		obj;
    OBJ_INDEX_DATA *	pObjIndex;
    int			i;
    int			sn;
    int			vnum;

    if ( ch->level > LEVEL_NEWBIE || IS_NPC( ch ) )
    {
	send_to_char("Find it yourself!\n\r",ch);
	return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
        if ( obj->pIndexData->vnum == OBJ_VNUM_MAP )
            break;

    if ( obj == NULL )
    {
        pObjIndex = get_obj_index( OBJ_VNUM_MAP );
        if ( pObjIndex != NULL )
        {
            obj = create_object( pObjIndex, 0 );
            obj_to_char( obj, ch );
        }
    }

    if ( ( obj= get_eq_char( ch, WEAR_FEET ) ) == NULL )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BOOTS ), 0 );
        obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_FEET );
    }

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_LIGHT );
    }

    if ( ( obj = get_eq_char( ch, WEAR_BODY ) ) == NULL )
    {
	obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIRT ), 0 );
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_BODY );
    }

    if ( ( obj= get_eq_char( ch, WEAR_LEGS ) ) == NULL )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_PANTS ), 0 );
        obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_LEGS );
    }

    /* do the weapon thing */
    if ((obj = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
    	sn = 0; 
    	vnum = OBJ_VNUM_SCHOOL_SWORD; /* just in case! */

    	for (i = 0; weapon_table[i].name != NULL; i++)
    	{
	    if (ch->pcdata->skill[sn].percent < 
		ch->pcdata->skill[*weapon_table[i].gsn].percent)
	    {
	    	sn = *weapon_table[i].gsn;
	    	vnum = weapon_table[i].vnum;
	    }
    	}

    	obj = create_object(get_obj_index(vnum),0);
     	obj_to_char(obj,ch);
    	equip_char(ch,obj,WEAR_WIELD);
    }

    if (((obj = get_eq_char(ch,WEAR_WIELD)) == NULL 
    ||   !IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)) 
    &&  (obj = get_eq_char( ch, WEAR_SHIELD ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_SHIELD );
    }

    send_to_char( "The gods have granted you a gift.\n\r", ch );
}


void do_pack ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *pack;
    OBJ_DATA *obj;
    int i;
                                                                                                                        
    one_argument( argument, arg );
                                                                                                                        
    if ( arg[0] == '\0' )
    {
        send_to_char( "Who would you like to send a pack too.\n\r", ch );
        return;
    }
                                                                                                                        
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    /* uncomment if you wish to restrict your immortals to only give packs to lowbies */                                                                                                                    
    /*
    if ( (victim->level >= 10) )
    {
        send_to_char("They don't need one at thier level.\n\r", ch);
        return;
    }
   */                                                                                                                     
    if (!can_pack(victim) )
    {
        send_to_char("They already have a survival pack.\n\r",ch);
        return;
    }
                                                                                                                        
    pack = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SATCHEL), 0 );
                                                                                                                        
    obj = create_object( get_obj_index(OBJ_VNUM_BEGINING), 0 );                                                         
    obj_to_obj( obj, pack );                                                                                            
                                                                                                                        
    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIRT), 0 );                                                     
    obj_to_obj( obj, pack );                                                                                            
                                                                                                                        
    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_PANTS), 0 );                                                     
    obj_to_obj( obj, pack );                                                                                            
                                                                                                                        
    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SANDLES), 0 );
    obj_to_obj( obj, pack );
                                                                                                                        
    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_WATER_SKIN), 0 );
 obj_to_obj( obj, pack );                                                                                            
                                                                                                                        
    obj = create_object( get_obj_index(OBJ_VNUM_GLOVES), 0 );                                                           
    obj_to_obj( obj, pack );                                                                                            
                                                                                                                        
    obj = create_object( get_obj_index(OBJ_VNUM_HELMET), 0 );                                                           
    obj_to_obj( obj, pack );                                                                                            
                                                                                                                        
    for ( i = 0; i < 2; i++ )                                                                                           
    {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_RING), 0 );                                                        
         obj_to_obj( obj, pack );                                                                                       
    }                                                                                                                   
                                                                                                                        
    for ( i = 0; i < 2; i++ )                                                                                           
    {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_BRACELET), 0 );                                                    
         obj_to_obj( obj, pack );                                                                                       
    }                                                                                                                   
                                                                                                                        
    for ( i = 0; i < 2; i++ )                                                                                           
    {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_NECKLACE), 0 );                                                    
         obj_to_obj( obj, pack );                                                                                       
    }                                                                                                                   
                                                                                                                        
    for ( i = 0; i < 2; i++ )                                                                                           
    {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SWORD), 0 );                                                
         obj_to_obj( obj, pack );                                                                                       
    }                                                                                                                   
                                                                                                                        
    for ( i = 0; i < 2; i++ )                                                                                           
    {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_DAGGER), 0 );                                               
         obj_to_obj( obj, pack );                                                                                       
    }                                                                                                                   
                                                                                                                        
         obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SPEAR), 0 );                                                
         obj_to_obj( obj, pack );                                                                                       
                                                                                                                        
    for ( i = 0; i < 2; i++ )                                                                                           
    {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_MACE), 0 );                                                 
         obj_to_obj( obj, pack );
    }
                                                                                                                        
    for ( i = 0; i < 2; i++ )
 {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_AXE), 0 );                                                  
         obj_to_obj( obj, pack );                                                                                       
    }                                                                                                                   
                                                                                                                        
    for ( i = 0; i < 2; i++ )                                                                                           
    {                                                                                                                   
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_FLAIL), 0 );                                                 
        obj_to_obj( obj, pack );                                                                                        
    }                                                                                                                   
                                                                                                                        
    for ( i = 0; i < 2; i++ )                                                                                           
    {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_WHIP), 0 );                                                 
         obj_to_obj( obj, pack );                                                                                       
    }                                                                                                                   
                                                                                                                        
         obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_POLEARM), 0 );                                              
         obj_to_obj( obj, pack );                                                                                       
                                                                                                                        
    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );                                                    
    obj_to_obj( obj, pack );                                                                                            

    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_LAMP), 0 );
    obj_to_obj( obj, pack);

    obj = create_object( get_obj_index(OBJ_VNUM_MAP), 0 );
    obj_to_obj( obj, pack);
                                                                                                                        
    for ( i = 0; i < 10; i ++ )                                                                                         
    {                                                                                                                   
         obj = create_object( get_obj_index(OBJ_VNUM_BREAD), 0 );                                                       
         obj_to_obj( obj, pack );                                                                                       
    }                                                                                                                   
                                                                                                                        
    obj_to_char( pack, victim );                                                                                        
    send_to_char("Ok.\n\r", ch);                                                                                        
    ch_printf( ch, "%s appears in your inventory.\n\r", pack->short_descr );                                       
    return;                                                                                                             
}                                                         
/* RT nochannels command, for those spammers */
void do_nochannels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Nochannel whom?", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET(victim->comm, COMM_NOCHANNELS) )
    {
        REMOVE_BIT(victim->comm, COMM_NOCHANNELS);
        send_to_char( "The gods have restored your channel priviliges.\n\r", 
		      victim );
        send_to_char( "NOCHANNELS removed.\n\r", ch );
	sprintf(buf,"$N restores channels to %s",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
        SET_BIT(victim->comm, COMM_NOCHANNELS);
        send_to_char( "The gods have revoked your channel priviliges.\n\r", 
		       victim );
        send_to_char( "NOCHANNELS set.\n\r", ch );
	sprintf(buf,"$N revokes %s's channels.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}


void do_smote(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char *letter,*name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }

    if (strstr(argument,ch->name) == NULL)
    {
	send_to_char("You must include your name in an smote.\n\r",ch);
	return;
    }

    send_to_char(argument,ch);
    send_to_char("\n\r",ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
        if (vch->desc == NULL || vch == ch)
            continue;

        if ((letter = strstr(argument,vch->name)) == NULL)
        {
	    send_to_char(argument,vch);
	    send_to_char("\n\r",vch);
            continue;
        }

        strcpy(temp,argument);
        temp[strlen(argument) - strlen(letter)] = '\0';
        last[0] = '\0';
        name = vch->name;

        for (; *letter != '\0'; letter++)
        {
            if (*letter == '\'' && matches == strlen(vch->name))
            {
                strcat(temp,"r");
                continue;
            }

            if (*letter == 's' && matches == strlen(vch->name))
            {
                matches = 0;
                continue;
            }

            if (matches == strlen(vch->name))
            {
                matches = 0;
            }

            if (*letter == *name)
            {
                matches++;
                name++;
                if (matches == strlen(vch->name))
                {
                    strcat(temp,"you");
                    last[0] = '\0';
                    name = vch->name;
                    continue;
                }
                strncat(last,letter,1);
                continue;
            }

            matches = 0;
            strcat(temp,last);
            strncat(temp,letter,1);
            last[0] = '\0';
            name = vch->name;
        }

	send_to_char(temp,vch);
	send_to_char("\n\r",vch);
    }

    return;
}


void
do_bamfin( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( !IS_NPC( ch ) )
    {
	smash_tilde( argument );

	if ( argument[0] == '\0' )
	{
	    ch_printf( ch, "Your poofin is %s\n\r",
	    IS_NULLSTR( ch->pcdata->bamfin )? "unset." : ch->pcdata->bamfin );
	    return;
	}

	strip_color( buf, argument );

	if ( strstr( buf, ch->name ) == NULL )
	{
	    send_to_char( "You must include your name.\n\r", ch );
	    return;
	}

	if ( strlen( buf ) > 79 )
	{
	    send_to_char( "Sorry, limited to 79 characters.\n\r", ch );
	    return;
	}    

	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );

        ch_printf( ch, "Your poofin is now %s\n\r", ch->pcdata->bamfin );
    }
    return;
}


void
do_bamfout( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( !IS_NPC( ch ) )
    {
        smash_tilde( argument );

        if ( argument[0] == '\0' )
        {
            ch_printf( ch, "Your poofout is %s\n\r",
	    IS_NULLSTR( ch->pcdata->bamfout )? "unset." : ch->pcdata->bamfout );
            return;
        }

	strip_color( buf, argument );

        if ( strstr( buf, ch->name ) == NULL )
        {
            send_to_char( "You must include your name.\n\r", ch );
            return;
        }

	if ( strlen( buf ) > 79 )
	{
	    send_to_char( "Sorry, limited to 79 characters.\n\r", ch );
	    return;
	}    

        free_string( ch->pcdata->bamfout );
        ch->pcdata->bamfout = str_dup( argument );

        ch_printf( ch, "Your poofout is now %s\n\r", ch->pcdata->bamfout );
    }
    return;
}



void do_deny( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Deny whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    SET_BIT(victim->act, PLR_DENY);
    send_to_char( "You are denied access!\n\r", victim );
    sprintf(buf,"$N denies access to %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    send_to_char( "OK.\n\r", ch );
    save_char_obj(victim);
    stop_fighting(victim,TRUE);
    do_function(victim, &do_quit, "" );

    return;
}



void
do_disconnect( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *	d;
    CHAR_DATA *		victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
    }

    if ( is_number( arg ) )
    {
	int desc;

	desc = atoi( arg );
    	for ( d = descriptor_list; d != NULL; d = d->next )
    	{
            if ( d->descriptor == desc )
            {
		victim = d->original ? d->original : d->character;
            	close_socket( d );
            	send_to_char( "Ok.\n\r", ch );
            	return;
            }
	}
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL )
    {
	act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d == victim->desc )
	{
	    close_socket( d );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}



void do_pardon( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "killer" ) )
    {
	if ( IS_SET(victim->act, PLR_KILLER) )
	{
	    REMOVE_BIT( victim->act, PLR_KILLER );
	    send_to_char( "Killer flag removed.\n\r", ch );
	    send_to_char( "You are no longer a KILLER.\n\r", victim );
	}
	return;
    }

    if ( !str_cmp( arg2, "thief" ) )
    {
	if ( IS_SET(victim->act, PLR_THIEF) )
	{
	    REMOVE_BIT( victim->act, PLR_THIEF );
	    send_to_char( "Thief flag removed.\n\r", ch );
	    send_to_char( "You are no longer a THIEF.\n\r", victim );
	}
	return;
    }

    send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
    return;
}


void do_echo( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Global echo what?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
	{
	    if (get_trust(d->character) >= get_trust(ch))
		send_to_char( "global> ",d->character);
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}


void
do_recho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Local echo what?\n\r", ch );

	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room == ch->in_room )
	{
            if (get_trust(d->character) >= get_trust(ch))
                send_to_char( "local> ",d->character);
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}

void do_zecho(CHAR_DATA *ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
	send_to_char("Zone echo what?\n\r",ch);
	return;
    }

    for (d = descriptor_list; d; d = d->next)
    {
	if (d->connected == CON_PLAYING
	&&  d->character->in_room != NULL && ch->in_room != NULL
	&&  d->character->in_room->area == ch->in_room->area)
	{
	    if (get_trust(d->character) >= get_trust(ch))
		send_to_char("zone> ",d->character);
	    send_to_char(argument,d->character);
	    send_to_char("\n\r",d->character);
	}
    }
}

void do_pecho( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
	send_to_char("Personal echo what?\n\r", ch); 
	return;
    }

    if  ( (victim = get_char_world(ch, arg) ) == NULL )
    {
	send_to_char("Target not found.\n\r",ch);
	return;
    }

    if (get_trust(victim) >= get_trust(ch) && get_trust(ch) != MAX_LEVEL)
        send_to_char( "personal> ",victim);

    send_to_char(argument,victim);
    send_to_char("\n\r",victim);
    send_to_char( "personal> ",ch);
    send_to_char(argument,ch);
    send_to_char("\n\r",ch);
}


ROOM_INDEX_DATA *
find_location( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    AREA_DATA *		pArea;
    ROOM_INDEX_DATA *	pRoom;
    CHAR_DATA *		victim;
    OBJ_DATA *		obj;
    int			vnum;
    int			x;
    int			y;

    argument = one_argument( argument, arg );

    if ( is_number( arg ) )
    {
	if ( is_number( argument ) )
	{
	    x = atoi( arg );
	    y = atoi( argument );
	    if ( x < 0 || y < 0 )
		return NULL;
	    if ( ch->in_room != NULL
	    &&	 ch->in_room->area->overland != NULL
	    &&	 x < ch->in_room->area->maxx
	    &&	 y < ch->in_room->area->maxy )
	    {
		pArea = ch->in_room->area;
		vnum = pArea->min_vnum + y * pArea->maxx + x;
		if ( ( pRoom = get_vroom_index( vnum ) ) != NULL )
		    return pRoom;
	    }
	    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
	    {
		if ( pArea->overland != NULL
		&&   x < pArea->maxx
		&&   y < pArea->maxy )
		{
		    vnum = pArea->min_vnum + y * pArea->maxx + x;
		    if ( ( pRoom = get_vroom_index( vnum ) ) != NULL )
			return pRoom;
		}
	    }
	    return NULL;
	}
	return get_vroom_index( atoi( arg ) );
    }
    if ( !str_cmp( arg, "." ) )
	return ch->in_room;

    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
	return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
	return obj->in_room;

    return NULL;
}



void do_transfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   d->character != ch
	    &&   d->character->in_room != NULL
	    &&   can_see( ch, d->character ) )
	    {
		char buf[MAX_STRING_LENGTH];
		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_function(ch, &do_transfer, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}

	if ( !is_room_owner(ch,location) && room_is_private( location ) 
	&&  get_trust(ch) < MAX_LEVEL)
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->in_room == NULL )
    {
	send_to_char( "They are in limbo.\n\r", ch );
	return;
    }

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    act_color( AT_IMM, "$n suddenly fades and disappears into nothingness.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    char_from_room( victim );
    char_to_room( victim, location );
    act_color( AT_IMM, "The air shimmers as $n appears.", victim, NULL, NULL, TO_ROOM, POS_RESTING );
    if ( ch != victim )
	act_color( AT_IMM, "$n has transferred you.", ch, NULL, victim, TO_VICT, POS_RESTING );
    do_function(victim, &do_look, "auto" );
    send_to_char( "Ok.\n\r", ch );
}



void do_at( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *on;
    CHAR_DATA *wch;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if (!is_room_owner(ch,location) && room_is_private( location ) 
    &&  get_trust(ch) < MAX_LEVEL)
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    original = ch->in_room;
    on = ch->on;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    ch->on = on;
	    break;
	}
    }

    return;
}



void
do_goto( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	location;
    CHAR_DATA *		rch;
    CHAR_DATA *		pet;
    CHAR_DATA *		mount;
    int			count;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, argument ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
    {
	send_to_char( "But you're already there!\n\r", ch );
	return;
    }

    count = 0;
    for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
        count++;

    if ( !is_room_owner(ch,location) && room_is_private( location ) 
    &&  ( count > 1 || get_trust(ch) < MAX_LEVEL ) )
    {
	if ( !IS_SET( location->room_flags, ROOM_IMP_ONLY )
	|| get_trust( ch ) < MAX_LEVEL )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
    }

    for ( pet = ch->in_room->people; pet != NULL; pet = pet->next_in_room )
	if ( IS_NPC( pet ) && IS_SET( pet->act, ACT_PET ) && pet->master == ch )
	    break;

    for ( mount = ch->in_room->people; mount != NULL; mount = mount->next_in_room )
	if ( ch->mount == mount && IS_NPC( mount ) && IS_SET( mount->act, ACT_MOUNT ) )
	    break;

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );
    if ( pet != NULL && pet->fighting != NULL )
	stop_fighting( pet, TRUE );
    if ( mount != NULL && mount->fighting != NULL )
	stop_fighting( mount, TRUE );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( get_trust( rch ) >= ch->invis_level )
	{
	    if ( ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0' )
		act( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT );
	    else
		act( "$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT );
	    if ( pet != NULL && can_see( rch, pet ) )
		act( "$n leaves in a swirling mist.", pet, NULL, rch, TO_VICT );
	    if ( mount != NULL && can_see( rch, mount ) )
		act( "$n leaves in a swirling mist.", mount, NULL, rch, TO_VICT );
	}
    }

    char_from_room( ch );
    char_to_room( ch, location );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( get_trust( rch ) >= ch->invis_level )
	{
	    if ( ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0' )
		act( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT );
	    else
		act("$n appears in a swirling mist.", ch, NULL, rch, TO_VICT );
	    if ( pet != NULL && can_see( rch, pet ) && rch != ch )
		act( "$N appears in a swirling mist.", rch, NULL, pet, TO_CHAR );
	    if ( mount != NULL && can_see( rch, mount ) && rch != ch )
		act( "$N appears in a swirling mist.", rch, NULL, mount, TO_CHAR );
        }
    }

    do_function( ch, &do_look, "auto" );
    if ( pet != NULL )
    {
	char_from_room( pet );
	char_to_room( pet, location );
	act( "$n appears in a swirling mist.", pet, NULL, ch, TO_VICT );
    }
    if ( mount != NULL )
    {
	char_from_room( mount );
	char_to_room( mount, location );
	act( "$n appears in a swirling mist.", mount, NULL, ch, TO_VICT );
    }

    return;
}

void
do_violate( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *	location;
    CHAR_DATA *		rch;
    CHAR_DATA *		pet;
    CHAR_DATA *		mount;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, argument ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
    {
	send_to_char( "But you're already there!\n\r", ch );
	return;
    }

    if ( !room_is_private( location ) )
    {
	send_to_char( "That room isn't private, use goto.\n\r", ch );
	return;
    }

    for ( pet = ch->in_room->people; pet != NULL; pet = pet->next_in_room )
	if ( IS_NPC( pet ) && IS_SET( pet->act, ACT_PET ) && pet->master == ch )
	    break;

    for ( mount = ch->in_room->people; mount != NULL; mount = mount->next_in_room )
	if ( IS_NPC( mount ) && IS_SET( mount->act, ACT_MOUNT ) && ch->mount == mount )
	    break;

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );
    if ( pet != NULL && pet->fighting != NULL )
	stop_fighting( pet, TRUE );
    if ( mount != NULL && mount->fighting != NULL )
	stop_fighting( mount, TRUE );

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( get_trust( rch ) >= ch->invis_level )
	{
	    if ( ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0' )
		act( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT );
	    else
		act( "$n leaves in a swirling mist.",ch,NULL,rch,TO_VICT );
	    if ( pet != NULL && can_see( rch, pet ) )
		act( "$n leaves in a swirling mist.", pet, NULL, rch, TO_VICT );
	    if ( mount != NULL && can_see( rch, mount ) )
		act( "$n leaves in a swirling mist.", mount, NULL, rch, TO_VICT );
	}
    }

    char_from_room( ch );
    char_to_room( ch, location );

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
	if ( get_trust( rch ) >= ch->invis_level )
	{
	    if ( ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0' )
		act( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT );
	    else
		act( "$n appears in a swirling mist.", ch, NULL, rch, TO_VICT );
	    if ( pet != NULL && can_see( rch, pet ) && rch != ch )
		act( "$N appears in a swirling mist.", rch, NULL, pet, TO_CHAR );
	    if ( mount != NULL && can_see( rch, mount ) && rch != ch )
		act( "$N appears in a swirling mist.", rch, NULL, mount, TO_CHAR );
	}
    }

    if ( pet != NULL )
    {
	char_from_room( pet );
	char_to_room( pet, location );
	act( "$n appears in a swirling mist.", pet, NULL, ch, TO_VICT );
    }

    if ( mount != NULL )
    {
	char_from_room( mount );
	char_to_room( mount, location );
	act( "$n appears in a swirling mist.", mount, NULL, ch, TO_VICT );
    }

    do_function( ch, &do_look, "auto" );
    return;
}


/* RT to replace the 3 stat commands */
void
do_stat ( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    BUFFER *		pBuf;
    char *		string;
    OBJ_DATA *		obj;
    ROOM_INDEX_DATA *	location;
    CHAR_DATA *		victim;

    string = one_argument( argument, arg );

    if ( arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  stat <name>\n\r",ch);
	send_to_char("  stat obj <name>\n\r",ch);
	send_to_char("  stat mob <name>\n\r",ch);
 	send_to_char("  stat room <number>\n\r",ch);
	return;
    }

    if ( !str_cmp( arg, "room" ) )
    {
	do_function( ch, &do_rstat, string );
	return;
    }

    if ( !str_cmp( arg, "obj" ) )
    {
        pBuf = new_buf( );
        stat_obj( ch, string, pBuf, WHENCE_STAT );
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
	return;
    }

    if ( !str_cmp( arg, "char" )  || !str_cmp( arg, "mob" ) )
    {
        pBuf = new_buf( );
        stat_mob( ch, string, pBuf, WHENCE_STAT );
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
	return;
    }

    /* do it the old way */

    obj = get_obj_world( ch, argument );
    if ( obj != NULL )
    {
        pBuf = new_buf( );
        stat_obj( ch, argument, pBuf, WHENCE_STAT );
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
	return;
    }

    victim = get_char_world( ch, argument );
    if ( victim != NULL )
    {
        pBuf = new_buf( );
        stat_mob( ch, argument, pBuf, WHENCE_STAT );
        page_to_char( buf_string( pBuf ), ch );
        free_buf( pBuf );
	return;
    }

    location = find_location( ch, argument );
    if ( location != NULL )
    {
        do_function( ch, &do_rstat, argument );
        return;
    }

    send_to_char( "Nothing by that name found anywhere.\n\r", ch );
}


void
do_rstat( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *	location;
    BUFFER *		pBuf;

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( location == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( !is_room_owner( ch, location ) && ch->in_room != location 
    &&	 room_is_private( location ) && !IS_TRUSTED( ch, IMPLEMENTOR ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    pBuf = new_buf( );
    show_room_info( ch, location, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_ostat( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;

    pBuf = new_buf( );
    stat_obj( ch, argument, pBuf, WHENCE_OSTAT );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_mstat( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;

    pBuf = new_buf( );
    stat_mob( ch, argument, pBuf, WHENCE_MSTAT );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


/* ofind and mfind replaced with vnum, vnum skill also added */

void do_vnum(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char *string;

    string = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  vnum obj <name>\n\r",ch);
	send_to_char("  vnum mob <name>\n\r",ch);
	send_to_char("  vnum skill <skill or spell>\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"obj"))
    {
	do_function(ch, &do_ofind, string);
 	return;
    }

    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    { 
	do_function(ch, &do_mfind, string);
	return;
    }

    if (!str_cmp(arg,"skill") || !str_cmp(arg,"spell"))
    {
	do_function (ch, &do_slookup, string);
	return;
    }
    /* do both */
    do_function(ch, &do_mfind, argument);
    do_function(ch, &do_ofind, argument);
}


void
do_mfind( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;
    int			hash;
    int			iCount;
    bool		fAll;
    MOB_INDEX_DATA *	pMob;
    MILIST_DATA *	first;
    MILIST_DATA *	mob;
    MILIST_DATA *	tmp;

    if ( *argument == '\0' )
    {
        send_to_char( "Find whom?\n\r", ch );
        return;
    }

    fAll = !str_cmp( argument, "all" );

    first = NULL;
    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
    {
        for ( pMob = mob_index_hash[hash]; pMob != NULL; pMob = pMob->next )
        {
            if ( fAll || is_name( argument, pMob->player_name ) )
            {
                mob = new_milist_data( );
                mob->lch = pMob;
                if ( first == NULL || pMob->vnum < first->lch->vnum )
                {
                    mob->next = first;
                    first = mob;
                }
                else if ( first->next == NULL )
                {
                    first->next = mob;
                    mob->next = NULL;
                }
                else
                {
                    for ( tmp = first; tmp->next != NULL; tmp = tmp->next )
                    {
                        if ( pMob->vnum < tmp->next->lch->vnum )
                        {
                            break;
                        }
                    }
                    mob->next = tmp->next;
                    tmp->next = mob;
                }
            }
        }
    }

    pBuf = new_buf( );
    iCount = 0;
    if ( first == NULL )
    {
        add_buf( pBuf, "No mobiles by that name.\n\r" );
    }
    else
    {
        while ( first != NULL )
        {
            iCount++;
            mob = first;
            buf_printf( pBuf, "[%2d][%5d] %s`X\n\r", iCount,
                        mob->lch->vnum, mob->lch->short_descr );
            first = first->next;
            free_milist_data( mob );
        }
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_ofind( CHAR_DATA *ch, char *argument )
{
    BUFFER *		pBuf;
    int			hash;
    int			iCount;
    bool		fAll;
    OBJ_INDEX_DATA *	pObj;
    OILIST_DATA *	first;
    OILIST_DATA *	obj;
    OILIST_DATA *	tmp;

    if ( *argument == '\0' )
    {
        send_to_char( "Find what?\n\r", ch );
        return;
    }

    fAll = !str_cmp( argument, "all" );

    first = NULL;
    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
    {
        for ( pObj = obj_index_hash[hash]; pObj != NULL; pObj = pObj->next )
        {
            if ( fAll || is_name( argument, pObj->name ) )
            {
                obj = new_oilist_data( );
                obj->o = pObj;
                if ( first == NULL || pObj->vnum < first->o->vnum )
                {
                    obj->next = first;
                    first = obj;
                }
                else if ( first->next == NULL )
                {
                    first->next = obj;
                    obj->next = NULL;
                }
                else
                {
                    for ( tmp = first; tmp->next != NULL; tmp = tmp->next )
                    {
                        if ( pObj->vnum < tmp->next->o->vnum )
                        {
                            break;
                        }
                    }
                    obj->next = tmp->next;
                    tmp->next = obj;
                }
            }
        }
    }

    pBuf = new_buf( );
    iCount = 0;
    if ( first == NULL )
    {
        add_buf( pBuf, "No objects by that name.\n\r" );
    }
    else
    {
        while ( first != NULL )
        {
            iCount++;
            obj = first;
            buf_printf( pBuf, "[%2d][%5d][%3d] %s`X\n\r", iCount,
                        obj->o->vnum, obj->o->level, obj->o->short_descr );
            first = first->next;
            free_oilist_data( obj );
        }
    }

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}


void
do_owhere( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_INPUT_LENGTH];
    BUFFER *	buffer;
    OBJ_DATA *	obj;
    OBJ_DATA *	in_obj;
    ROOM_INDEX_DATA *	room;
    bool	found;
    bool	fVnum;
    int		vnum;
    int		number = 0;
    int		max_found;

    found = FALSE;
    number = 0;
    max_found = 200;

    buffer = new_buf();

    if (argument[0] == '\0')
    {
	send_to_char("Find what?\n\r",ch);
	return;
    }

    if ( is_number( argument ) )
    {
	fVnum = TRUE;
	vnum = atoi( argument );
    }
    else
    {
	fVnum = FALSE;
	vnum = 0;
    }

    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
        if ( !can_see_obj( ch, obj )
        ||   ( !fVnum && !is_name( argument, obj->name ) )
        ||   ( fVnum && obj->pIndexData->vnum != vnum )
        ||   ch->level < obj->level )
            continue;

        found = TRUE;
        number++;

        for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
            ;

	if ( in_obj->carried_by != NULL && can_see( ch, in_obj->carried_by )
	&&   in_obj->carried_by->in_room != NULL )
            sprintf( buf, "%3d) %s`X is carried by %s [Room %d]\n\r",
                number, obj->short_descr, PERS( in_obj->carried_by, ch ),
		in_obj->carried_by->in_room->vnum );
	else if (in_obj->in_room != NULL && can_see_room( ch, in_obj->in_room ) )
            sprintf( buf, "%3d) %s`X is in %s`X [Room %d]\n\r",
                number, obj->short_descr, in_obj->in_room->name, 
	   	in_obj->in_room->vnum );
	else if ( in_obj->stored_in != NULL && in_obj->stored_in->owner != NULL
	&&	  can_see( ch, in_obj->stored_in->owner )
	&&	  ( room = get_room_index( in_obj->stored_in->vnum ) ) != NULL
	&&	  can_see_room( ch, room ) )
	    sprintf( buf, "%3d) %s`X is stored by %s in [%d]%s`X\n\r",
		     number, obj->short_descr, in_obj->stored_in->owner->name,
		     room->vnum, room->name );
	else
            sprintf( buf, "%3d) %s`X is somewhere\n\r", number, obj->short_descr );

        buf[0] = UPPER( buf[0] );
        add_buf( buffer, buf );

        if ( number >= max_found )
            break;
    }

    if ( !found )
        send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
        page_to_char( buf_string( buffer ), ch );

    free_buf( buffer );
}


void
do_mwhere( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    BUFFER *	buffer;
    CHAR_DATA *	victim;
    bool	found;
    bool	fVnum;
    int		vnum;
    int		count = 0;

    if ( argument[0] == '\0' )
    {
	DESCRIPTOR_DATA *d;

	/* show characters logged */

	buffer = new_buf();
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->character != NULL && d->connected == CON_PLAYING
	    &&	 d->character->in_room != NULL && can_see( ch, d->character )
	    &&	 can_see_room(ch,d->character->in_room ) )
	    {
		victim = d->character;
		count++;
		if ( d->original != NULL )
		    sprintf(buf,"%3d) %s (in the body of %s) is in %s`X [%d]\n\r",
			count, d->original->name,victim->short_descr,
			victim->in_room->name,victim->in_room->vnum);
		else
		    sprintf( buf,"%3d ) %s is in %s`X [%d]\n\r",
			count, victim->name,victim->in_room->name,
			victim->in_room->vnum);
		add_buf( buffer, buf );
	    }
	}

        page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return;
    }

    found = FALSE;
    if ( is_number( argument ) )
    {
	fVnum = TRUE;
	vnum = atoi( argument );
    }
    else
    {
	fVnum = FALSE;
	vnum = 0;
    }

    buffer = new_buf();
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if ( victim->in_room != NULL
	&&   ( ( !fVnum && is_name( argument, victim->name ) )
	||     ( fVnum && IS_NPC( victim ) && victim->pIndexData->vnum == vnum ) ) )
	{
	    found = TRUE;
	    count++;
	    sprintf( buf, "%3d) [%5d] %-28s`X [%5d] %s`X\n\r", count,
		IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
		IS_NPC( victim ) ? victim->short_descr : victim->name,
		victim->in_room->vnum,
		victim->in_room->name );
	    add_buf( buffer, buf );
	}
    }

    if ( !found )
	act( "You didn't find any $T.", ch, NULL, argument, TO_CHAR );
    else
    	page_to_char( buf_string( buffer ), ch );

    free_buf( buffer );

    return;
}

void 
do_new_reboot( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument(argument,arg);    

 if (arg[0] == '\0') 
    {
      send_to_char("Usage: nreboot now\n\r",ch);
      send_to_char("Usage: nreboot <ticks to reboot>\n\r",ch);
      send_to_char("Usage: nreboot cancel\n\r",ch);
      send_to_char("Usage: nreboot status\n\r",ch);
      return;
    }

    if (is_name(arg,"cancel")) 
     {
      reboot_counter = -1;
      send_to_char("Reboot canceled.\n\r",ch);
      return;
    }

    if (is_name(arg, "now")) 
     {
      reboot_mars();
      return;
    }

    if (is_name(arg, "status")) 
    {
      if (reboot_counter == -1) 
	sprintf(buf, "Automatic rebooting is inactive.\n\r");
      else
	sprintf(buf,"Reboot in %i minutes.\n\r",reboot_counter);
      send_to_char(buf,ch);
      return;
    }

    if (is_number(arg))
    {
     reboot_counter = atoi(arg);
     sprintf(buf,"MARS will reboot in %i ticks.\n\r",reboot_counter);
     send_to_char(buf,ch);
     return;
    }

 do_reboot(ch,"");   
}


void reboot_mars( void )
{
    extern bool merc_down;
    DESCRIPTOR_DATA *d,*d_next;

    sprintf( log_buf, "Rebooting MARS.");
    log_string(log_buf);
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
	d_next = d->next;
	write_to_buffer(d,"MARS is rebooting now!",0);
        if (d->character != NULL)
	   save_char_obj(d->character);
    	close_socket(d);
    }
    merc_down = TRUE;    
    return;
}




void
do_reboot( CHAR_DATA *ch, char *argument )
{
    char		buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *	d;
    DESCRIPTOR_DATA *	d_next;
    CHAR_DATA *		rch;
    CHAR_DATA *		vch;

    if ( rebuild.pid != 0 )
    {
	send_to_char( "A rebuild is in process, try again later.\n\r", ch );
	return;
    }

    rch = CH( ch->desc );

    if ( rch->invis_level < LEVEL_HERO )
    {
    	sprintf( buf, "Reboot by %s.", rch->name );
    	do_function( rch, &do_echo, buf );
    }

    if ( str_cmp( argument, "nosave" ) )
	do_asave( NULL, "" );

    merc_down = TRUE;
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
	d_next = d->next;
	vch = d->original ? d->original : d->character;
	if (vch != NULL)
	    save_char_obj(vch);
    	close_socket(d);
    }

    save_vehicles( );

    unlink( SHUTDOWN_FILE );
    return;
}


void
do_shutdown( CHAR_DATA *ch, char *argument )
{
    char		buf[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *	d;
    DESCRIPTOR_DATA *	d_next;
    CHAR_DATA *		rch;
    CHAR_DATA *		vch;

    rch = CH( ch->desc );
    sprintf( buf, "Shutdown by %s.", rch->name );
    append_file( rch, SHUTDOWN_FILE, buf );
    if ( ch->invis_level < LEVEL_HERO )
    {
	sprintf( buf, "Shutdown by %s.\n\r", rch->name );
    	do_function( ch, &do_echo, buf );
    }

    if ( str_cmp( argument, "nosave" ) )
	do_asave( NULL, "" );

    merc_down = TRUE;
    for ( d = descriptor_list; d != NULL; d = d_next)
    {
	d_next = d->next;
	vch = d->original ? d->original : d->character;
	if ( vch != NULL )
	    save_char_obj( vch );
	close_socket( d );
    }

    save_vehicles( );

    return;
}


void
do_protect( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

    if (argument[0] == '\0')
    {
	send_to_char("Protect whom from snooping?\n\r",ch);
	return;
    }

    if ((victim = get_char_world(ch,argument)) == NULL)
    {
	send_to_char("You can't find them.\n\r",ch);
	return;
    }

    if (IS_SET(victim->comm,COMM_SNOOP_PROOF))
    {
	act_new("$N is no longer snoop-proof.",ch,NULL,victim,TO_CHAR,POS_DEAD);
	send_to_char("Your snoop-proofing was just removed.\n\r",victim);
	REMOVE_BIT(victim->comm,COMM_SNOOP_PROOF);
    }
    else
    {
	act_new("$N is now snoop-proof.",ch,NULL,victim,TO_CHAR,POS_DEAD);
	send_to_char("You are now immune to snooping.\n\r",victim);
	SET_BIT(victim->comm,COMM_SNOOP_PROOF);
    }
}


void
do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL )
    {
	send_to_char( "No descriptor to snoop.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\n\r", ch );
	wiznet("$N stops being such a snoop.",
		ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch));
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	}
	return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
    }

    if (!is_room_owner(ch,victim->in_room) && ch->in_room != victim->in_room 
    &&  room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
        send_to_char("That character is in a private room.\n\r",ch);
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) 
    ||   IS_SET(victim->comm,COMM_SNOOP_PROOF))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( ch->desc != NULL )
    {
	for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
	{
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "No snoop loops.\n\r", ch );
		return;
	    }
	}
    }

    victim->desc->snoop_by = ch->desc;
    sprintf(buf,"$N starts snooping on %s",
	(IS_NPC(ch) ? victim->short_descr : victim->name));
    wiznet(buf,ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}


void
do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }

    if ( ch->desc == NULL )
	return;

    if ( ch->desc->original != NULL )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim))
    {
	send_to_char("You can only switch into mobiles.\n\r",ch);
	return;
    }

    if (!is_room_owner(ch,victim->in_room) && ch->in_room != victim->in_room 
    &&  room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
	send_to_char("That character is in a private room.\n\r",ch);
	return;
    }

    if ( victim->desc != NULL )
    {
	send_to_char( "Character in use.\n\r", ch );
	return;
    }

    if ( ch->desc->editor != ED_NONE )
    {
        send_to_char( "Not while you're editing.\n\r", ch );
        return;
    }

    sprintf(buf,"$N switches into %s",victim->short_descr);
    wiznet(buf,ch,NULL,WIZ_SWITCHES,WIZ_SECURE,get_trust(ch));

    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    SET_BIT( ch->act, PLR_SWITCHED );
    /* change communications to match */
    free_string( victim->prompt );
    if ( ch->prompt != NULL )
        victim->prompt = str_dup( ch->prompt );
    else
        victim->prompt = NULL;
    victim->comm = ch->comm;
    victim->info = ch->info;
    REMOVE_BIT( victim->comm, COMM_AFK );
    victim->lines = ch->lines;
    send_to_char( "Ok.\n\r", victim );
    return;
}



void
do_return( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    CHAR_DATA *	och;

    if ( ch->desc == NULL )
	return;

    if ( ( och = ch->desc->original ) == NULL )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }

    send_to_char( "You return to your original body.\n\r", ch );
    if ( ch->prompt != NULL )
    {
	free_string( ch->prompt );
	ch->prompt = NULL;
    }

    sprintf( buf, "$N returns from %s.", ch->short_descr );
    wiznet( buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE, get_trust( ch ) );
    REMOVE_BIT( ch->desc->original->act, PLR_SWITCHED );
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc; 
    ch->desc                  = NULL;

    if ( !IS_NPC( och ) && *buf_string( och->pcdata->buffer ) != '\0' )
	send_to_char( "You have unreceived tells.  Type 'replay' to view.\n\r", och );

    return;
}


/* trust levels for load and clone */
bool
obj_check (CHAR_DATA *ch, OBJ_DATA *obj)
{
    if (IS_TRUSTED(ch,GOD)
	|| (IS_TRUSTED(ch,IMMORTAL) && obj->level <= 20 && obj->cost <= 1000)
	|| (IS_TRUSTED(ch,DEMI)	    && obj->level <= 10 && obj->cost <= 500)
	|| (IS_TRUSTED(ch,ANGEL)    && obj->level <=  5 && obj->cost <= 250)
	|| (IS_TRUSTED(ch,AVATAR)   && obj->level ==  0 && obj->cost <= 100))
	return TRUE;
    else
	return FALSE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone)
{
    OBJ_DATA *c_obj, *t_obj;


    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
    {
	if (obj_check(ch,c_obj))
	{
	    t_obj = create_object(c_obj->pIndexData,0);
	    clone_object(c_obj,t_obj);
	    obj_to_obj(t_obj,clone);
	    recursive_clone(ch,c_obj,t_obj);
	}
    }
}

/* command that is similar to load */
void
do_clone( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char *	rest;
    CHAR_DATA *	mob;
    OBJ_DATA  *	obj;

    rest = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Clone what?\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "object" ) )
    {
	mob = NULL;
	obj = get_obj_here( ch, rest );
	if ( obj == NULL )
	{
	    send_to_char( "You don't see that here.\n\r", ch );
	    return;
	}
    }
    else if ( !str_prefix( arg, "mobile" ) || !str_prefix( arg, "character" ) )
    {
	obj = NULL;
	mob = get_char_room( ch, rest );
	if ( mob == NULL )
	{
	    send_to_char( "You don't see that here.\n\r", ch );
	    return;
	}
    }
    else /* find both */
    {
	mob = get_char_room( ch, argument );
	obj = get_obj_here( ch, argument );
	if ( mob == NULL && obj == NULL )
	{
	    send_to_char( "You don't see that here.\n\r", ch );
	    return;
	}
    }

    /* clone an object */
    if (obj != NULL)
    {
	OBJ_DATA *clone;

	if (!obj_check(ch,obj))
	{
	    send_to_char(
		"Your powers are not great enough for such a task.\n\r",ch);
	    return;
	}

	clone = create_object( obj->pIndexData, 0 ); 
	clone_object( obj, clone );
	if ( obj->carried_by != NULL )
	    obj_to_char( clone, ch );
	else
	{
	    obj_to_room( clone, ch->in_room );
	    add_obj_fall_event( clone );
 	}
 	recursive_clone( ch, obj, clone );

	act_color( AT_IMM, "$n has created $p.", ch, clone, NULL, TO_ROOM, POS_RESTING );
	act_color( AT_IMM, "You clone $p.", ch, clone, NULL, TO_CHAR, POS_RESTING);
	wiznet( "$N clones $p.", ch, clone, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
	return;
    }
    else if (mob != NULL)
    {
	CHAR_DATA *	clone;
	OBJ_DATA *	new_obj;
	char		buf[MAX_STRING_LENGTH];
	AFFECT_DATA *	paf;
	AFFECT_DATA *	paf_next;

	if ( !IS_NPC( mob ) )
	{
	    send_to_char( "You can only clone mobiles.\n\r", ch );
	    return;
	}

	if ( ( mob->level > 20 && !IS_TRUSTED( ch, GOD ) )
	||   ( mob->level > 10 && !IS_TRUSTED( ch, IMMORTAL))
	||   ( mob->level >  5 && !IS_TRUSTED( ch, DEMI))
	||   ( mob->level >  0 && !IS_TRUSTED( ch, ANGEL ) )
	||   !IS_TRUSTED( ch, AVATAR ) )
	{
	    send_to_char(
		"Your powers are not great enough for such a task.\n\r", ch );
	    return;
	}

	clone = create_mobile( mob->pIndexData );

	for ( paf = clone->affected; paf; paf = paf_next )
	{
	    paf_next = paf->next;
	    affect_remove( clone, paf );
	}

	clone_mobile( mob, clone ); 

	for ( obj = mob->carrying; obj != NULL; obj = obj->next_content )
	{
	    if ( obj_check( ch, obj ) )
	    {
		new_obj = create_object( obj->pIndexData, 0 );
		clone_object( obj, new_obj );
		recursive_clone( ch, obj,new_obj );
		obj_to_char( new_obj, clone );
		new_obj->wear_loc = obj->wear_loc;
	    }
	}
	char_to_room( clone,ch->in_room);
        act_color( AT_IMM, "$n has created $N.", ch, NULL, clone, TO_ROOM, POS_RESTING );
        act_color( AT_IMM, "You clone $N.", ch, NULL, clone, TO_CHAR, POS_DEAD );
	sprintf( buf,"$N clones %s.",clone->short_descr );
	wiznet( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
        return;
    }
}

/* RT to replace the two load commands */

void do_load(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  load mob <vnum>\n\r",ch);
	send_to_char("  load obj <vnum> <level>\n\r",ch);
	return;
    }

    if ( !str_cmp( arg, "mob" ) || !str_cmp( arg, "char" ) )
    {
	load_mob( ch, argument, "load mob" );
	return;
    }

    if ( !str_cmp( arg, "obj" ) )
    {
	load_obj( ch, argument, "load obj" );
	return;
    }
    /* echo syntax */
    do_function(ch, &do_load, "");
}


void
do_mload( CHAR_DATA *ch, char *argument )
{
    load_mob( ch, argument, "mload" );
    return;
}


void
do_oload( CHAR_DATA *ch, char *argument )
{
    load_obj( ch, argument, "oload" );
    return;
}


void
load_mob( CHAR_DATA *ch, char *argument, const char *cmd )
{
    AREA_DATA *	    pArea;
    CHAR_DATA *	    victim;
    MOB_INDEX_DATA *pMobIndex;
    char            arg [ MAX_INPUT_LENGTH ];
    char	    arg1[ MAX_INPUT_LENGTH ];
    char	    buf [ MAX_INPUT_LENGTH ];
    int		    num;
    int		    vnum;
    int		    amt;
    int		    nMatch = 0;
    int		    count = 0;
    bool	    found = FALSE;

    one_argument( argument, arg );
    amt = number_argument( arg, arg1 );
    num = is_number( arg ) ? atoi( arg ) : 0;

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC( ch ) && ch->pcdata->last_medit != 0 )
	{
	    sprintf( buf, "%d", ch->pcdata->last_medit );
	    load_mob( ch, buf, cmd );
	}
	else
	{
	    ch_printf( ch, "Syntax: %s <vnum>.\n\r", cmd );
	}
	return;
    }

    /* so you could mload <name> or <2.name> and it loads the proper one */
    if ( num == 0 )
    {
	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    for( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	    {
		if( ( pMobIndex = get_mob_index( vnum ) ) )
		{
		    nMatch++;
		    if( is_name( arg1, pMobIndex->player_name ) )
		    {
			if( ++count == amt )
			{
			    num = vnum;
			    found = TRUE;
			    break;
			}
		    }
		}
	    }
	    if ( found )
		break;
	}
	if ( !found )
	{
	    send_to_char( "No mobile has that name.\n\r", ch );
	    return;
	}
    }

    if ( !( pMobIndex = get_mob_index( num ) ) )
    {
	send_to_char( "No mob has that vnum.\n\r", ch );
	return;
    }

    if ( pMobIndex->count >= MAX_MLOAD )
    {
        send_to_char( "Too many of those already loaded.\n\r", ch );
        bugf( "Too many mobs of vnum #%d already loaded.", num );
        return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    act_color( AT_IMM, "You have created $N!", ch, NULL, victim, TO_CHAR, POS_DEAD );
    act_color( AT_IMM, "$n has created $N!", ch, NULL, victim, TO_ROOM, POS_RESTING );
    sprintf( buf, "$N has created %s`X.", victim->short_descr );
    wiznet( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
    return;
}


void
load_obj( CHAR_DATA *ch, char *argument, const char *cmd )
{
    OBJ_DATA *		obj;
    OBJ_INDEX_DATA *	pObjIndex;
    AREA_DATA *		pArea;
    char		arg1 [ MAX_INPUT_LENGTH ];
    char		arg2 [ MAX_INPUT_LENGTH ];
    char		arg3 [ MAX_INPUT_LENGTH ];
    char		arg  [ MAX_INPUT_LENGTH ];
    char		buf1 [ MAX_INPUT_LENGTH ];
    char		buf2 [ MAX_INPUT_LENGTH ];
    int			noi = 1;
    int			in = 1;
    int			level;
    int			num;
    int			vnum;
    int			count = 0;
    int			amt;
    int			nMatch = 0;
    bool		found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    noi = is_number( arg2 ) ? atoi( arg2 ) : 1;
    amt = number_argument( arg1, arg );
    num = is_number( arg1 ) ? atoi( arg1 ) : 0;

    if ( arg1[0] == '\0' )
    {
	if ( !IS_NPC( ch ) && ch->pcdata->last_oedit != 0 )
	{
	    sprintf( buf1, "%d", ch->pcdata->last_oedit );
	    load_obj( ch, buf1, cmd );
	}
	else
	{
            ch_printf( ch, "Syntax: %s <vnum> [number] [level].\n\r", cmd );
        }
        return;
    }

    if ( 1 > noi || noi > MAX_OLOAD )
    {
	send_to_char( "You cannot load that many objects.\n\r", ch ) ;
	return ;
    }

    /* so you can oload <name> or <2.name> and it loads the proper one */
    if ( num == 0 )
    { 
	found = FALSE;
	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    for( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	    {
		if( ( pObjIndex = get_obj_index( vnum ) ) )
		{
		    nMatch++;
		    if ( ( is_name( arg, pObjIndex->name ) ) )
		    {
			if ( ++count == amt )
			{
			    num = vnum; 
			    found = TRUE;
			    break;
			}
		    }
		}
	    }
	    if ( found )
		break;
	}
    }

    if ( !( pObjIndex = get_obj_index( num ) ) )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
	return;
    }

    if ( pObjIndex->level > get_trust( ch ) )
    {
        send_to_char( "That object is of too high a level.\n\r", ch );
        return;
    }

    if ( arg3[0] == '\0' )
    {
	level = pObjIndex->level;
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg3 ) )
        {
	    ch_printf(ch, "Syntax: %s <vnum> [number] [level].\n\r", cmd );
	    return;
        }
        level = atoi( arg3 );
	if ( level < 0 || level > get_trust( ch ) )
        {
	    send_to_char( "Limited to your trust level.\n\r", ch );
	    return;
        }
    }

    for ( in = 1; in <= noi; in++ )
    {
	obj = create_object( pObjIndex, level );
	obj->level = level;
	if ( CAN_WEAR( obj, ITEM_TAKE ) )
	{
	    obj_to_char( obj, ch );
	}
	else
	{
	    obj_to_room( obj, ch->in_room );
	    add_obj_fall_event( obj );
	}
    }

    obj = create_object( pObjIndex, level );
    if ( noi > 1 )
    {
	sprintf( buf1, "You have created (%d) $p!", noi );
	sprintf( buf2, "$n has created (%d) $p!", noi );
    }
    else
    {
	strcpy( buf1, "You have created $p!" );
	strcpy( buf2, "$n has created $p!" );
    }
    act_color( AT_IMM, buf1, ch, obj, NULL, TO_CHAR, POS_DEAD );
    act_color( AT_IMM, buf2, ch, obj, NULL, TO_ROOM, POS_RESTING );

         if ( noi == 1 && level == pObjIndex->level )
	sprintf( log_buf, "$N has created $p`X." );
    else if ( noi == 1 && level != pObjIndex->level )
	sprintf( log_buf, "$N has created $p`X, level %d.", level );
    else if ( noi != 1 && level == pObjIndex->level )
	sprintf( log_buf, "$N has created %d $p`Xs.", noi );
    else if ( noi != 1 && level != pObjIndex->level )
	sprintf( log_buf, "$N has created %d $p`Xs, level %d.", noi, level );
    wiznet( log_buf, ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
    extract_obj( obj );
    return;
}


void
do_purge( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *		area;
    char		arg[MAX_INPUT_LENGTH];
    CHAR_DATA *		victim;
    OBJ_DATA *		obj;
    ROOM_INDEX_DATA *	room;
    ROOM_INDEX_DATA *	orig_room;
    DESCRIPTOR_DATA *	d;
    int			vnum;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* 'purge' */
	CHAR_DATA *vnext;
	OBJ_DATA  *obj_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC(victim) && !IS_SET(victim->act,ACT_NOPURGE) 
	    &&   victim != ch /* safety precaution */ )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if (!IS_OBJ_STAT(obj,ITEM_NOPURGE))
	      extract_obj( obj );
	}

	act_color( AT_IMM, "You raise your arms, and white flames engulf the room!", ch, NULL, NULL, TO_CHAR, POS_DEAD );
	act_color( AT_IMM, "$n raises $s arms, and white flames engulf the room!", ch, NULL, NULL, TO_ROOM, POS_RESTING );
	return;
    }

    if ( !str_cmp( arg, "area" ) )
    {
	area = ch->in_room->area;
	if ( IS_NPC( ch ) || !IS_BUILDER( ch, area ) )
	{
	    send_to_char( "You cannot purge this area.\n\r", ch );
	    return;
	}

	orig_room = ch->in_room;
	for ( vnum = area->min_vnum; vnum <= area->max_vnum; vnum++ )
	{
	    if ( ( room = get_room_index( vnum ) ) != NULL
	    &&	 ( room->people != NULL || room->contents != NULL ) )
	    {
		char_from_room( ch );
		char_to_room( ch, room );
		do_function( ch, do_purge, "" );
	    }
	}
	char_from_room( ch );
	char_to_room( ch, orig_room );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	if ( ( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) )
	{
	    act_color( AT_IMM, "You raise your arms, and white flames engulf $P.", ch, NULL, obj, TO_CHAR, POS_DEAD );
	    act_color( AT_IMM, "$n raises $s arms, and white flames engulf $P.", ch, NULL, obj, TO_ROOM, POS_RESTING );
	    extract_obj( obj );
	    return;
	}
	send_to_char( "Nothing by that name here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {

	if ( ch == victim )
	{
	    send_to_char( "Ho ho ho.\n\r", ch );
	    return;
	}

	if ( get_trust( ch ) <= get_trust( victim ) )
	{
	    send_to_char( "Maybe that wasn't a good idea...\n\r", ch );
	    ch_printf( victim, "%s tried to purge you!\n\r", ch->name );
	    return;
	}

	act_color( AT_IMM, "You raise your arms, and white flames engulf $N.", ch, NULL, victim, TO_CHAR, POS_DEAD );
	act_color( AT_IMM, "$n raises $s arms, and white flames engulf $N.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	act_color( AT_ACTION, "$N's body disappears in a cloud of greasy black smoke.", ch, NULL, victim, TO_CHAR, POS_DEAD );
	act_color( AT_ACTION, "$N's body disappears in a cloud of greasy black smoke.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );

//	if ( victim->level > 1 )
	    save_char_obj( victim );
	d = victim->desc;
	extract_char( victim, TRUE );
	if ( d != NULL )
	    close_socket( d );

	return;
    }

    act_color( AT_IMM, "You raise your arms, and white flames engulf $N.", ch, NULL, victim, TO_CHAR, POS_DEAD );
    act_color( AT_IMM, "$n raises $s arms, and white flames engulf $N.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
    extract_char( victim, TRUE );
    return;
}



void
do_advance( CHAR_DATA *ch, char *argument )
{
    char	buf[MAX_STRING_LENGTH];
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char	arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *	victim;
    int		level;
    int		iLevel;
    int		oldlevel;
    bool	fMaxed;
    int		iStat;
    int		sec;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ( level = atoi( arg2 ) ) < 1 || level > MAX_LEVEL )
    {
	sprintf( buf, "Level must be 1 to %d.\n\r", MAX_LEVEL );
	send_to_char( buf, ch);
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust level.\n\r", ch );
	return;
    }

    if ( victim->pcdata->avatar != NULL )
    {
        send_to_char( "That person is an avatar right now.\n\r", ch );
        return;
    }

    if ( LOWER( arg3[0] ) == 'm'
    && ( !str_prefix( arg3, "maxed" ) || !str_prefix( arg3, "maxxed" ) ) )
	fMaxed = TRUE;
    else
	fMaxed = FALSE;

    if ( fMaxed )
    {
	for ( iStat = 0; iStat < MAX_STATS; iStat++ )
	    victim->mod_stat[iStat] += 99;
    }

    oldlevel = victim->level;

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->level )
    {
        int temp_prac;

	send_to_char( "Lowering a player's level!\n\r", ch );
	send_to_char( "`R**** `WOOOOHHHHHHHHHH  NNNNOOOO `R****`w\n\r", victim );
	temp_prac = victim->practice;
	victim->level    = 1;
	victim->exp      = exp_per_level(victim,victim->pcdata->points);
	victim->max_hit  = 10;
	victim->max_mana = 100;
	victim->max_move = 100;
	victim->practice = 0;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
	advance_level( victim, TRUE );
	victim->practice = temp_prac;
    }
    else
    {
	send_to_char( "Raising a player's level!\n\r", ch );
	send_to_char( "`R**** `WOOOOHHHHHHHHHH  YYYYEEEESSS `R****`X\n\r",
		      victim );
    }

    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	victim->level += 1;
	advance_level( victim, TRUE );
    }

    if ( fMaxed )
    {
	for ( iStat = 0; iStat < MAX_STATS; iStat++ )
	    victim->mod_stat[iStat] -= 99;
    }

    ch_printf( victim, "You are now level `W%d`X.\n\r", victim->level );
    victim->exp   = exp_per_level( victim, victim->pcdata->points ) 
		  * UMAX( 1, victim->level );
    victim->trust = 0;
    if ( oldlevel >= LEVEL_IMMORTAL && victim->level < LEVEL_IMMORTAL )
	victim->pcdata->sec_groups = 0;
    else if ( oldlevel < LEVEL_IMMORTAL && victim->level >= LEVEL_IMMORTAL )
    {
	sec = security_lookup( "common" );
	if ( sec >= 0 )
	    victim->pcdata->sec_groups |= security_flags[sec].bit;
    }
    save_char_obj( victim );
    sql_update_level( victim );
    update_userlist( victim, FALSE );
    return;
}


void
do_trust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( ( level = atoi( arg2 ) ) < 0 || level > MAX_LEVEL )
    {
	sprintf(buf, "Level must be 0 (reset) or 1 to %d.\n\r",MAX_LEVEL);
	send_to_char(buf, ch);
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust.\n\r", ch );
	return;
    }

    victim->trust = level;
    return;
}



void
do_restore( CHAR_DATA *ch, char *argument )
{
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    CHAR_DATA *	victim;
    CHAR_DATA *	vch;
    DESCRIPTOR_DATA *d;

    one_argument( argument, arg );
    if ( arg[0] == '\0' || !str_cmp( arg, "room" ) )
    {
        /* cure room */
        for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
        {
            affect_strip( vch, gsn_plague );
            affect_strip( vch, gsn_poison );
            affect_strip( vch, gsn_blindness );
            affect_strip( vch, gsn_sleep );
            affect_strip( vch, gsn_curse );
	    affect_strip( vch, gsn_wail );
	    affect_strip( vch, gsn_deafen );

            vch->hit 	= vch->max_hit;
            vch->mana	= vch->max_mana;
            vch->move	= vch->max_move;
            if ( !IS_NPC( vch ) && vch->pcdata->condition[COND_TIRED] >= 0 )
                vch->pcdata->condition[COND_TIRED] = MAX_COND;
            update_pos( vch);
            act( "$n has restored you.", ch, NULL, vch, TO_VICT );
        }

        sprintf( buf, "$N restored room %d.", ch->in_room->vnum );
        wiznet( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust( ch ) );

        send_to_char( "Room restored.\n\r", ch );
        return;
    }

    if ( get_trust( ch ) >= MAX_LEVEL - 1 && !str_cmp( arg, "all" ) )
    {
        /* cure all */
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
	    victim = d->character;

	    if (victim == NULL || IS_NPC(victim))
		continue;

            affect_strip( victim, gsn_plague );
            affect_strip( victim, gsn_poison );
            affect_strip( victim, gsn_blindness );
            affect_strip( victim, gsn_sleep );
            affect_strip( victim, gsn_curse );
	    affect_strip( victim, gsn_wail );
	    affect_strip( victim, gsn_deafen );

            victim->hit 	= victim->max_hit;
            victim->mana	= victim->max_mana;
            victim->move	= victim->max_move;
            if ( !IS_NPC( victim ) && victim->pcdata->condition[COND_TIRED] >= 0 )
                victim->pcdata->condition[COND_TIRED] = MAX_COND;
            update_pos( victim );
	    if ( victim->in_room != NULL )
                act( "$n has restored you.", ch, NULL, victim, TO_VICT );
        }
	send_to_char("All active players restored.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    affect_strip( victim, gsn_plague );
    affect_strip( victim, gsn_poison );
    affect_strip( victim, gsn_blindness );
    affect_strip( victim, gsn_sleep );
    affect_strip( victim, gsn_curse );
    affect_strip( victim, gsn_wail );
    affect_strip( victim, gsn_deafen );
    victim->hit  = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    if ( !IS_NPC( victim ) && victim->pcdata->condition[COND_TIRED] >= 0 )
        victim->pcdata->condition[COND_TIRED] = MAX_COND;
    update_pos( victim );
    act( "$n has restored you.", ch, NULL, victim, TO_VICT );
    sprintf( buf, "$N restored %s",
	IS_NPC( victim ) ? victim->short_descr : victim->name );
    wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}


void
do_freeze( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_FREEZE) )
    {
	REMOVE_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can play again.\n\r", victim );
	send_to_char( "FREEZE removed.\n\r", ch );
	sprintf(buf,"$N thaws %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
	SET_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can't do ANYthing!\n\r", victim );
	send_to_char( "FREEZE set.\n\r", ch );
	sprintf(buf,"$N puts %s in the deep freeze.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    save_char_obj( victim );

    return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Log ALL on.\n\r", ch );
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET(victim->act, PLR_LOG) )
    {
	REMOVE_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG set.\n\r", ch );
    }

    return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }


    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->comm, COMM_NOEMOTE) )
    {
	REMOVE_BIT(victim->comm, COMM_NOEMOTE);
	send_to_char( "You can emote again.\n\r", victim );
	send_to_char( "NOEMOTE removed.\n\r", ch );
	sprintf(buf,"$N restores emotes to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
	SET_BIT(victim->comm, COMM_NOEMOTE);
	send_to_char( "You can't emote!\n\r", victim );
	send_to_char( "NOEMOTE set.\n\r", ch );
	sprintf(buf,"$N revokes %s's emotes.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void do_noshout( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noshout whom?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->comm, COMM_NOSHOUT) )
    {
	REMOVE_BIT(victim->comm, COMM_NOSHOUT);
	send_to_char( "You can shout again.\n\r", victim );
	send_to_char( "NOSHOUT removed.\n\r", ch );
	sprintf(buf,"$N restores shouts to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
	SET_BIT(victim->comm, COMM_NOSHOUT);
	send_to_char( "You can't shout!\n\r", victim );
	send_to_char( "NOSHOUT set.\n\r", ch );
	sprintf(buf,"$N revokes %s's shouts.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Notell whom?", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->comm, COMM_NOTELL) )
    {
	REMOVE_BIT(victim->comm, COMM_NOTELL);
	send_to_char( "You can tell again.\n\r", victim );
	send_to_char( "NOTELL removed.\n\r", ch );
	sprintf(buf,"$N restores tells to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
	SET_BIT(victim->comm, COMM_NOTELL);
	send_to_char( "You can't tell!\n\r", victim );
	send_to_char( "NOTELL set.\n\r", ch );
	sprintf(buf,"$N revokes %s's tells.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void
do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL )
	    stop_fighting( rch, TRUE );
	if (IS_NPC(rch) && IS_SET(rch->act,ACT_AGGRESSIVE))
	    REMOVE_BIT(rch->act,ACT_AGGRESSIVE);
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}

void
do_wizlock( CHAR_DATA *ch, char *argument )
{
    wizlock = !wizlock;

    if ( wizlock )
    {
	wiznet("$N has wizlocked the game.",ch,NULL,0,0,0);
	send_to_char( "Game wizlocked.\n\r", ch );
    }
    else
    {
	wiznet("$N removes wizlock.",ch,NULL,0,0,0);
	send_to_char( "Game un-wizlocked.\n\r", ch );
    }

    return;
}

/* RT anti-newbie code */

void
do_newlock( CHAR_DATA *ch, char *argument )
{
    newlock = !newlock;

    if ( newlock )
    {
	wiznet("$N locks out new characters.",ch,NULL,0,0,0);
        send_to_char( "New characters have been locked out.\n\r", ch );
    }
    else
    {
	wiznet("$N allows new characters back in.",ch,NULL,0,0,0);
        send_to_char( "Newlock removed.\n\r", ch );
    }

    return;
}


void
do_slookup( CHAR_DATA *ch, char *argument )
{
    char	arg[ MAX_INPUT_LENGTH ];
    char	buf[ MAX_STRING_LENGTH ];
    bool	found;
    int		i;
    int		iGrp;
    int		index;
    int		max_skill = 0;
    BUFFER *	pBuf;
    int		sn;
    int		psn;
    int		lower;
    int		upper;
    bitvector	flags;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slookup what?\n\r", ch );
	return;
    }

    for ( max_skill = 0; max_skill < top_skill; max_skill++ )
    {
	if ( !skill_table[max_skill].name || skill_table[max_skill].name[0] == '\0' )
	    break;
    }
    max_skill--;

    if ( is_number( arg ) )
    {
	lower = atoi( arg );
	lower = UMAX( lower, 1 );
	argument = one_argument( argument, arg );
	one_argument( argument, arg );
	if ( arg[ 0 ] == '\0' )
	    upper = lower;
	else
	    upper = atoi( arg );
	if ( lower > max_skill )
	{
	    ch_printf( ch, "There are only %d skills.\n\r", max_skill );
	    return;
	}
	if ( lower > upper )
	{
	    lower ^= upper;
	    upper ^= lower;
	    lower ^= upper;
	}
	upper = UMIN( max_skill, upper );
	pBuf = new_buf( );
	for ( sn = lower; sn <= upper; sn++ )
	{
	    if ( !skill_table[sn].name )
		break;
	    sprintf( buf, "Sn: %4d %s: '%s'\n\r",
		    sn,
		    ( skill_table[sn].spell_fun == spell_null ) ?
			"Skill" : "Spell",
		    skill_table[sn].name );
	    add_buf( pBuf, buf );
	}
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	pBuf = new_buf( );
	for ( sn = 1; sn < top_skill; sn++ )
	{
	    if ( skill_table[sn].name == NULL || skill_table[sn].name[0] == '\0' )
		break;
	    sprintf( buf, "Sn: %4d %s: '%s'\n\r",
		    sn,
		    ( skill_table[sn].spell_fun == spell_null ) ?
			"Skill" : "Spell",
		    skill_table[sn].name );
	    add_buf( pBuf, buf );
	}
	page_to_char( buf_string( pBuf ), ch );
	free_buf( pBuf );
    }
    else
    {
        int ccount;
	int l;

	if ( ( sn = skill_lookup( argument ) ) < 0 )
	{
	    send_to_char( "No such skill or spell.\n\r", ch );
	    return;
	}

	sprintf( buf, "Sn: %4d %s: '%s'",
		sn,
		( skill_table[sn].spell_fun == spell_null ) ?
		    "Skill" : "Spell",
		skill_table[sn].name );
	if ( skill_table[sn].spell_fun != spell_null )
	{
	    char buf1[ MAX_INPUT_LENGTH ];
	    sprintf( buf1, "  Mana cost: %d", skill_table[sn].min_mana );
	    strcat( buf, buf1 );
	}

	if ( skill_table[sn].spell_fun != spell_null )
	{
	    strcat( buf, "\n\rTarget type: " );
	    strcat( buf, flag_string( target_types, skill_table[sn].target ) );
	}

	strcat( buf, "\n\r" );
	send_to_char( buf, ch );

	if ( ( flags = skill_table[sn].spell_flags ) != 0 )
	{
	    ch_printf( ch, "Flags: %s\n\r", flag_string( spell_flags, flags ) );
	}

	l = 0;
	for ( ccount = 0; ccount < MAX_CLASS; ccount++ )
	{
	  if ( class_table[ccount].who_name[0] == '\0'
		|| IS_SET( class_table[ccount].flags, CLASS_DISABLED ) )
	      continue;
	  sprintf( buf, "%s: %3d   ", class_table[ccount].who_name,
		  skill_table[sn].skill_level[ccount] );
	  l += strlen ( buf );
	  if ( l > 79 )
	  {
	      send_to_char ( "\n\r", ch );
	      l = strlen ( buf );
	  }
	  send_to_char( buf, ch );
	}
	send_to_char( "\n\r", ch );

	found = FALSE;
	for ( index = 0; index < MAX_PREREQ; index++ )
	{
	    psn = skill_table[sn].prereq[index];
	    if ( psn > 0 )
	    {
		if ( !found )
		{
		    send_to_char( "Prequisites:\n\r", ch );
		    found = TRUE;
		}
		ch_printf( ch, "  '%s'\n\r", skill_table[psn].name );
	    }
	}

	found = FALSE;
	for ( psn = 1; !IS_NULLSTR( skill_table[psn].name ); psn++ )
	{
	    for ( index = 0; index < MAX_PREREQ; index++ )
	    {
		if ( skill_table[psn].prereq[index] == sn )
		{
		    if ( !found )
		    {
			send_to_char( "Required for:\n\r", ch );
			found = TRUE;
		    }
		    ch_printf( ch, "  '%s'\n\r", skill_table[psn].name );
		}
	    }
	}

	found = FALSE;
	for ( iGrp = 0; iGrp < MAX_GROUP; iGrp++ )
	{
	    if ( IS_NULLSTR( group_table[iGrp].name ) )
		break;
	    for ( i = 0; i < MAX_IN_GROUP; i++ )
	    {
		if ( !IS_NULLSTR( group_table[iGrp].spells[i] )
		&&   !str_cmp( group_table[iGrp].spells[i],
			       skill_table[sn].name ) )
		{
		    if ( !found )
		    {
			send_to_char( "Group(s):\n\r", ch );
			found = TRUE;
		    }
		    ch_printf( ch, "  %s\n\r", group_table[iGrp].name );
		}
	    }
	}

    }

    return;
}


/* RT set replaces sset, mset, oset, and rset */
void
do_set( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int value;

    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  set mob   <name> <field> <value>\n\r",ch);
	send_to_char("  set obj   <name> <field> <value>\n\r",ch);
        send_to_char("  set skill <name> <spell or skill> <value>\n\r",ch);
	send_to_char("  set time  <hour>\n\r", ch );
	if ( get_trust( ch ) >= IMPLEMENTOR )
	    send_to_char("  set month <month>\n\r", ch );
	send_to_char("  set wind  <dir> or <speed>\n\r", ch );
	return;
    }

    if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
    {
	do_function(ch, &do_mset, argument);
	return;
    }

    if (!str_prefix(arg,"skill") || !str_prefix(arg,"spell"))
    {
	do_function(ch, &do_sset, argument);
	return;
    }

    if (!str_prefix(arg,"object"))
    {
	do_function(ch, &do_oset, argument);
	return;
    }

    if ( !str_prefix( arg, "time" ) || !str_prefix( arg, "hour" ) )
    {
	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
	    send_to_char( "Set time to what?\n\r", ch );
	    return;
	}
	if ( !is_number( arg ) )
	{
	    send_to_char( "Hour must be between 0 and 23.\n\r", ch );
	    return;
	}
	value = atoi( arg );
	if ( value < 0 || value > 23 )
	{
	    send_to_char( "Hour must be between 0 and 23.\n\r", ch );
	    return;
	}
	time_info.hour = value;

	if ( value < hour_sunrise || value > hour_sunset )
	    weather_info.sunlight = SUN_DARK;
	else if ( value == hour_sunrise )
	    weather_info.sunlight = SUN_RISE;
	else if ( value == hour_sunset )
	    weather_info.sunlight = SUN_SET;
	else
	    weather_info.sunlight = SUN_LIGHT;

	send_to_char ( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "wind" ) )
    {
	argument = one_argument( argument, arg );

	if ( ( value = dir_lookup( arg ) ) != DIR_NONE )
	{
	    if ( value == DIR_UP || value == DIR_DOWN )
	    {
		send_to_char( "You can't set the wind to that direction.\n\r", ch );
		return;
	    }
	    weather_info.wind_dir = value;
	    send_to_char( "Wind direction set.\n\r", ch );
	    return;
	}

	if ( ( value = windspeed_lookup( arg ) ) != NO_FLAG )
	{
	    weather_info.wind_speed = value;
	    send_to_char( "Wind speed set.\n\r", ch );
	    return;
	}

	send_to_char( "Direction must be a valid direction except \"up\" or \"down\".\n\r", ch );
	send_to_char( "Or, speed must be one of:\n\r", ch );
	for ( value = 0; windspeed_types[value].name != NULL; value++ )
	    ch_printf( ch, "   %s\n\r", windspeed_types[value].name );
	return;
    }

    if ( get_trust( ch ) >= IMPLEMENTOR && !str_cmp( arg, "month" ) )
    {
        argument = one_argument( argument, arg );

        if ( !is_number( arg ) || ( value = atoi( arg ) - 1 ) < 0 || value >= MONTHS_PER_YEAR )
        {
            ch_printf( ch, "Month must between 1 and %d.\n\r", MONTHS_PER_YEAR );
            return;
        }

        time_info.month = value;

	switch( time_info.month )
	{
	    case 0:
	    case 8:
		hour_sunrise = HOUR_SUNRISE + 2;
		hour_sunset  = HOUR_SUNSET  - 2;
		break;
	    case 1:
		hour_sunrise = HOUR_SUNRISE + 1;
		hour_sunset  = HOUR_SUNSET  - 1;
		break;
	    case 2:
		hour_sunrise = HOUR_SUNRISE;
		hour_sunset  = HOUR_SUNSET;
		break;
	    case 3:
		hour_sunrise = HOUR_SUNRISE - 1;
		hour_sunset  = HOUR_SUNSET  + 1;
		break;
	    case 4:
		hour_sunrise = HOUR_SUNRISE - 2;
		hour_sunset  = HOUR_SUNSET  + 2;
		break;
	    case 5:
		hour_sunrise = HOUR_SUNRISE - 1;
		hour_sunset  = HOUR_SUNSET  + 1;
		break;
	    case 6:
		hour_sunrise = HOUR_SUNRISE;
		hour_sunset  = HOUR_SUNSET;
		break;
	    case 7:
		hour_sunrise = HOUR_SUNRISE + 1;
		hour_sunset  = HOUR_SUNSET  - 1;
		break;
	}

        ch_printf( ch, "Month set to %s.\n\r", month_name[value] );
        return;
    }

    /* echo syntax */
    do_function(ch, &do_set, "");
}


void
do_sset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r",ch);
	send_to_char( "  set skill <name> <spell or skill> <value>\n\r", ch);
	send_to_char( "  set skill <name> all <value>\n\r",ch);  
	send_to_char("   (use the name of the skill, not the number)\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim != ch && get_trust( ch ) < SUPREME )
    {
        send_to_char( "Not on others.\n\r", ch );
        return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
	for ( sn = 0; sn < top_skill; sn++ )
	{
	    if ( skill_table[sn].name != NULL )
	    {
		victim->pcdata->skill[sn].percent = value;
	    }
	}
    }
    else
    {
	victim->pcdata->skill[sn].percent = value;
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}


void
do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r", ch );
	send_to_char( "  mset <name> <field> <value>\n\r", ch ); 
	send_to_char( "  Field being one of:\n\r",			ch );
	send_to_char( "    str int wis dex con sex class level \n\r",	ch );
	send_to_char( "    race group hp kingdom mana move prac\n\r",ch );
	ch_printf( ch, "    %s %s %s %s\n\r",
	           GOLD_PLURAL, SILVER_PLURAL, COPPER_PLURAL, FRACT_PLURAL );
	send_to_char( "    ethos align train thirst hunger drunk full \n\r",	ch );
	send_to_char("     tired hours \n\r", ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim )
    &&	 victim != ch
    &&	 ( get_trust( ch ) < SUPREME
         || get_trust( ch ) < get_trust( victim ) ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    /* clear zones for mobs */
    victim->zone = NULL;

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "str" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_STR) )
	{
	    sprintf(buf,
		"Strength range is 3 to %d\n\r.",
		get_max_train(victim,STAT_STR));
	    send_to_char(buf,ch);
	    return;
	}

	victim->perm_stat[STAT_STR] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
        if ( value < 3 || value > get_max_train(victim,STAT_INT) )
        {
            sprintf(buf,
		"Intelligence range is 3 to %d.\n\r",
		get_max_train(victim,STAT_INT));
            send_to_char(buf,ch);
            return;
        }

        victim->perm_stat[STAT_INT] = value;
	send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_WIS) )
	{
	    sprintf(buf,
		"Wisdom range is 3 to %d.\n\r",get_max_train(victim,STAT_WIS));
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_stat[STAT_WIS] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_DEX) )
	{
	    sprintf(buf,
		"Dexterity range is 3 to %d.\n\r",
		get_max_train(victim,STAT_DEX));
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_stat[STAT_DEX] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_CON) )
	{
	    sprintf(buf,
		"Constitution range is 3 to %d.\n\r",
		get_max_train(victim,STAT_CON));
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_stat[STAT_CON] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }



    if ( !str_prefix( arg2, "sex" ) )
    {
	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
	    return;
	}
	victim->sex = value;
	if (!IS_NPC(victim))
	    victim->pcdata->true_sex = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "class" ) )
    {
	int class;

	if (IS_NPC(victim))
	{
	    send_to_char("Mobiles have no class.\n\r",ch);
	    return;
	}

	class = class_lookup(arg3);
	if ( class == -1 )
	{
	    char buf[MAX_STRING_LENGTH];

        	strcpy( buf, "Possible classes are: " );
        	for ( class = 0; class < MAX_CLASS; class++ )
        	{
            	    if ( class > 0 )
                    	strcat( buf, " " );
            	    strcat( buf, class_table[class].name );
        	}
            strcat( buf, ".\n\r" );

	    send_to_char(buf,ch);
	    return;
	}

	victim->class = class;
	update_userlist( victim, FALSE );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "level" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > MAX_LEVEL )
	{
	    sprintf(buf, "Level range is 0 to %d.\n\r", MAX_LEVEL);
	    send_to_char(buf, ch);
	    return;
	}
	victim->level = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, GOLD_PLURAL ) || !str_cmp( arg2, "gold" ) )
    {
	victim->money.gold = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, SILVER_PLURAL ) || str_match( arg2, "sil", "silvers" ) )
    {
	victim->money.silver = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, COPPER_PLURAL )
    ||   !str_prefix( arg2, COPPER_NOUN )
    ||   str_match( arg2, "cop", "coppers" ) )
    {
	victim->money.copper = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, FRACT_PLURAL ) )
    {
	victim->money.fract = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "dead" ) && get_trust( ch ) >= MAX_LEVEL )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }
        if ( IS_IMMORTAL( victim ) )
        {
            send_to_char( "Not on Immortals.\n\r", ch );
            return;
        }

        if ( !is_number( arg3 ) )
            value = flag_value( death_types, arg3 );

        if ( value < DEATH_NONE || value > DEATH_RESURRECT )
        {
            ch_printf( ch, "Bad value \"%s\".\n\r", arg3 );
            return;
        }

        victim->pcdata->deathstate = value;

        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg2, "hp" ) )
    {
	if ( value < 1 || value > 3000000 )
	{
	    send_to_char( "Hp range is 1 to 3,000,000 hit points.\n\r", ch );
	    return;
	}
	victim->max_hit = value;
        if ( !IS_NPC( victim ) )
            victim->pcdata->perm_hit = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "kingdom" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }
        if ( arg3[0] == '\0' )
        {
            send_to_char( "Set their kingdom to what?\n\r", ch );
            return;
        }
        if ( value == -1 )
        {
            for ( value = 0; value < MAX_KINGDOM; value++ )
            {
                if ( !str_prefix( arg3, kingdom_table[value].name ) )
                    break;
            }
        }
        if ( value < 0 || value >= MAX_KINGDOM )
        {
            ch_printf( ch, "Bad kingdom \"%s\".\n\r", arg3 );
            return;
        }
        victim->pcdata->kingdom = value;
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg2, "mana" ) )
    {
	if ( value < 0 || value > 3000000 )
	{
	    send_to_char( "Mana range is 0 to 3,000,000 mana points.\n\r", ch );
	    return;
	}
	victim->max_mana = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_mana = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "move" ) )
    {
	if ( value < 0 || value > 3000000 )
	{
	    send_to_char( "Move range is 0 to 3,000,000 move points.\n\r", ch );
	    return;
	}
	victim->max_move = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_move = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "practice" ) )
    {
	if ( value < 0 || value > 2500 )
	{
	    send_to_char( "Practice range is 0 to 2500 sessions.\n\r", ch );
	    return;
	}
	victim->practice = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "train" ))
    {
	if (value < 0 || value > 500 )
	{
	    send_to_char("Training session range is 0 to 500 sessions.\n\r",ch);
	    return;
	}
	victim->train = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

if ( !str_prefix( arg2, "questpoints" ) )
    {
    	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}
	if ( value < 0 || value > 25000 )
	{
	    send_to_char( "Questpointrange is 0 to 25000 qp.\n\r", ch );
	    return;
	}
	victim->questpoints = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "align" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->alignment = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

 if ( !str_prefix( arg2, "ethos" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Ethos range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->ethos = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }
    
    if ( !str_prefix( arg2, "thirst" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < -1 || value > 100 )
	{
	    send_to_char( "Thirst range is -1 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_THIRST] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "drunk" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < -1 || value > 100 )
	{
	    send_to_char( "Drunk range is -1 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "full" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < -1 || value > 100 )
	{
	    send_to_char( "Full range is -1 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_FULL] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "hunger" ) )
    {
        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char( "Full range is -1 to 100.\n\r", ch );
            return;
        }

        victim->pcdata->condition[COND_HUNGER] = value;
	send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg2, "tired" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < -1 || value > 100 )
	{
	    send_to_char( "Tired range is -1 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_TIRED] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if (!str_prefix( arg2, "race" ) )
    {
	int race;

	race = race_lookup(arg3);

	if ( race == 0)
	{
	    send_to_char("That is not a valid race.\n\r",ch);
	    return;
	}

	if (!IS_NPC(victim) && !race_table[race].pc_race)
	{
	    send_to_char("That is not a valid player race.\n\r",ch);
	    return;
	}

	victim->race = race;
	if ( !IS_NPC( victim ) )
	    update_userlist( victim, FALSE );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if (!str_prefix(arg2,"group"))
    {
	if (!IS_NPC(victim))
	{
	    send_to_char("Only on NPCs.\n\r",ch);
	    return;
	}
	victim->group = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }
    else if (!str_cmp(arg2, "hours"))
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }
        
        if ( !is_number( arg3 ) )
        {
            send_to_char( "Value must be numeric.\n\r", ch );
            return;
        }
        
        value = atoi( arg3 );
        
        if ( value < 0 || value > 999 )
        {
            send_to_char( "Value must be betwen 0 and 999.\n\r", ch );
            return;
        }
            
        value *= 3600;
        victim->played = value;
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    /*
     * Generate usage message.
     */
    do_function(ch, &do_mset, "" );
    return;
}

void do_string( CHAR_DATA *ch, char *argument )
{
    char type [MAX_INPUT_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    smash_tilde( argument );
    argument = one_argument( argument, type );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r",ch);
	send_to_char( "  string char <name> <field> <string>\n\r", ch );
	send_to_char( "    fields: name short long desc title spec\n\r", ch );
	send_to_char( "            hair eyes\n\r", ch );
	send_to_char( "  string obj  <name> <field> <string>\n\r", ch );
	send_to_char( "    fields: name short long lname extended\n\r", ch );
	return;
    }

    if (!str_prefix(type,"character") || !str_prefix(type,"mobile"))
    {
    	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    	}

        if ( !IS_NPC( victim )
        &&	 victim != ch
        &&	 ( get_trust( ch ) < SUPREME
                 || get_trust( ch ) < get_trust( victim ) ) )
        {
            send_to_char( "You can't do that.\n\r", ch );
            return;
        }

	/* clear zone for mobs */
	victim->zone = NULL;

	/* string something */

     	if ( !str_prefix( arg2, "name" ) )
    	{
	    if ( !IS_NPC(victim) )
	    {
	    	send_to_char( "Not on PC's.\n\r", ch );
	    	return;
	    }
	    free_string( victim->name );
	    victim->name = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "description" ) )
    	{
    	    free_string(victim->description);
    	    victim->description = str_dup(arg3);
    	    return;
    	}

    	if ( !str_prefix( arg2, "short" ) )
    	{
	    free_string( victim->short_descr );
	    victim->short_descr = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "long" ) )
    	{
	    free_string( victim->long_descr );
	    strcat(arg3,"\n\r");
	    victim->long_descr = str_dup( arg3 );
	    return;
    	}

	if ( !str_prefix( arg2, "lname" ) )
	{
	    if ( IS_NPC( victim ) )
	    {
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	    }
	    if ( !str_cmp( arg3, "none" ) )
		arg3[0] = '\0';
	    else
		first_arg( argument, arg3, FALSE );
	    free_string( victim->pcdata->lname );
	    victim->pcdata->lname = str_dup( arg3 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}

    	if ( !str_prefix( arg2, "title" ) )
    	{
	    if ( IS_NPC( victim ) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }

	    set_title( victim, arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "spec" ) )
    	{
	    if ( !IS_NPC( victim ) )
	    {
	    	send_to_char( "Not on PC's.\n\r", ch );
	    	return;
	    }

	    if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	    {
	    	send_to_char( "No such spec fun.\n\r", ch );
	    	return;
	    }

	    return;
    	}

    	if ( !str_prefix( arg2, "hair_color" ) )
    	{
	    if ( IS_NPC( victim ) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }

	    free_string( victim->pcdata->hair_color );
	    victim->pcdata->hair_color = str_dup( arg3 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
        }

if ( !str_prefix( arg2, "hair_adj" ) )
    	{
	    if ( IS_NPC( victim ) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }

	    free_string( victim->pcdata->hair_adj );
	    victim->pcdata->hair_adj = str_dup( arg3 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
        }
    	if ( !str_prefix( arg2, "eye_color" ) )
    	{
	    if ( IS_NPC( victim ) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }

	    free_string( victim->pcdata->eye_color );
	    victim->pcdata->eye_color = str_dup( arg3 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
        }

        if ( !str_prefix( arg2, "eye_adj" ) )
    	{
	    if ( IS_NPC( victim ) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }

	    free_string( victim->pcdata->eye_adj );
	    victim->pcdata->eye_adj = str_dup( arg3 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
        }

    }

    if ( !str_prefix( type, "object" ) )
    {
    	/* string an obj */
   	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    	{
	    send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
	    return;
    	}

        if ( !str_prefix( arg2, "name" ) )
    	{
	    free_string( obj->name );
	    obj->name = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "short" ) )
    	{
	    free_string( obj->short_descr );
	    obj->short_descr = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "long" ) )
    	{
	    free_string( obj->description );
	    obj->description = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "ed" ) || !str_prefix( arg2, "extended"))
    	{
	    EXTRA_DESCR_DATA *ed;

	    argument = one_argument( argument, arg3 );
	    if ( argument == NULL )
	    {
	    	send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
		    ch );
	    	return;
	    }

 	    strcat(argument,"\n\r");

	    ed = new_extra_descr();

	    ed->keyword		= str_dup( arg3     );
	    ed->description	= str_dup( argument );
	    ed->next		= obj->extra_descr;
	    obj->extra_descr	= ed;
	    return;
    	}
    }

    /* echo bad use message */
    do_function(ch, &do_string, "");
}


void
do_oset( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    char	arg3[MAX_INPUT_LENGTH];
    char	arg4[MAX_INPUT_LENGTH];
    char *	arglist;
    OBJ_DATA *	obj;
    int		value;
    bitvector	bits;
    EXT_BV	mat_flags;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r", ch );
	send_to_char( "  set obj <object> <field> <value>\n\r", ch );
	send_to_char( "  Field being one of:\n\r",				ch );
	send_to_char( "    value0 value1 value2 value3 value4 value5 (v1-v5)\n\r", ch );
	send_to_char( "    material extra wear level weight cost timer\n\r",	ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.

     */
    if ( !str_prefix( arg2, "cost" ) )
    {
	obj->cost = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "extra" ) )
    {
	if ( is_number( arg3 ) )
	    bits = value;
	else if ( !str_cmp( arg3, "none" ) )
	    bits = 0;
	else if ( ( bits = flag_value( extra_flags, arg3 ) ) == NO_FLAG )
	{
	    send_to_char( "Bad extra flag.\n\r", ch );
	    return;
	}
	obj->extra_flags = bits;
	send_to_char( "Ok.\n\r", ch );
	return;
    }
/*
  if ( !str_prefix( arg2, "extra2" ) )
    {
	if ( is_number( arg3 ) )
	    bits = value;
	else if ( !str_cmp( arg3, "none" ) )
	    bits = 0;
	else if ( ( bits = flag_value( extra_flags2, arg3 ) ) == NO_FLAG )
	{
	    send_to_char( "Bad extra2 flag.\n\r", ch );
	    return;
	}
	obj->extra_flags2 = bits;
	send_to_char( "Ok.\n\r", ch );
	return;
    }
*/

    if ( !str_prefix( arg2, "material" ) )
    {
	xCLEAR_BITS( mat_flags );
	arglist = arg3;
	while ( *arglist != '\0' )
	{
	    arglist = one_argument( arglist, arg4 );
	    if ( ( value = flag_value( material_types, arg4 ) ) == NO_FLAG )
	    {
		ch_printf( ch, "Bad material type \"%s\".\n\r", arg4 );
		return;
	    }
	    xSET_BIT( mat_flags, value );
	}
	xCLEAR_BITS( obj->material );
	xSET_BITS( obj->material, mat_flags );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "level" ) )
    {
	obj->level = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "timer" ) )
    {
	obj->timer = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
//	obj->value[0] = UMIN(50,value);	/* Not sure why the cap of 50 here */
	obj->value[0] = value;
	return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	obj->value[1] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	obj->value[2] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	obj->value[3] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
    {
	obj->value[4] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value5" ) || !str_cmp( arg2, "v5" ) )
    {
	obj->value[5] = value;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "wear" ) )
    {
	if ( is_number( arg3 ) )
	    bits = value;
	else if ( !str_cmp( arg3, "none" ) )
	    bits = 0;
	else if ( ( bits = flag_value( wear_flags, arg3 ) ) == NO_FLAG )
	{
	    send_to_char( "Bad wear flag.\n\r", ch );
	    return;
	}
	obj->wear_flags = bits;
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "weight" ) )
    {
	if ( !weight_value( arg3, &obj->weight ) )
	    send_to_char( "Bad weight value.\n\r", ch );
	else
	    send_to_char( "Ok.\n\r", ch );
	return;
    }

    /*
     * Generate usage message.
     */
    do_function(ch, &do_oset, "" );
    return;
}


void
do_sockets( CHAR_DATA *ch, char *argument )
{
    char		buf[MAX_INPUT_LENGTH];
    int			count;
    DESCRIPTOR_DATA *	d;
    char		idle[SHORT_STRING_LENGTH];
    int			len;
    BUFFER *		pBuf;
    CHAR_DATA *		sch;
    char		tbuf[SHORT_STRING_LENGTH];

    pBuf = new_buf( );
    count = 0;

    add_buf( pBuf, "\n\rNum Connected State Login@  Idl Player@Host\n\r" );
    add_buf( pBuf,"--------------------------------------------------------------------------\n\r");  
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->character != NULL && can_see( ch, d->character ) )
	{
	    sch = d->original != NULL ? d->original : d->character;
	    if ( sch != NULL )
		strftime( tbuf, sizeof( tbuf ), "%I:%M%p",
			  localtime( &sch->logon ) );
	    else
		strcpy( tbuf, "--:--  " );
	    if ( d->idle > 0 )
		sprintf( idle, "%4d", d->idle );
	    else
		strcpy( idle, "    " );

	    sprintf( buf, "`Y%3d `W%s `%c%7.7s%s `C%s`W@`c",
		     d->descriptor,
		     con_type_name( d->connected ),
		     sch == NULL || sch->logon + 60*60*24 > current_time ? 'G' : 'c',
		     tbuf, idle,
		     sch == NULL ? "(Unknown)" : d->connected == CON_CONFIRM_NEW_NAME ? "(new user)" : sch->name );
	    add_buf( pBuf, buf );
	    len = 79 - strlen_wo_col( buf );
	    strncpy( buf, d->host, len );
	    buf[len] = '\0';
	    add_buf( pBuf, buf );
	    add_buf( pBuf, "\n\r" );
	    count++;
        }
    }

    buf_printf( pBuf, "`W%d `wuser%s\n\r", count, count == 1 ? "" : "s" );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );
    return;
}






/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Force whom to do what?\n\r", ch );
	return;
    }

    one_argument(argument,arg2);

    if (!str_cmp(arg2,"delete"))
    {
	send_to_char("That will NOT be done.\n\r",ch);
	return;
    }


    sprintf( buf, "$n forces you to '%s'.", argument );

    if ( !str_cmp( arg, "all" ) )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	if (get_trust(ch) < MAX_LEVEL - 3)
	{
	    send_to_char("Not at your level!\n\r",ch);
	    return;
	}

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) )
	    {
		act( buf, ch, NULL, vch, TO_VICT );
		interpret( vch, argument );
	    }
	}
    }
    else if (!str_cmp(arg,"players"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        if (get_trust(ch) < MAX_LEVEL - 2)
        {
            send_to_char("Not at your level!\n\r",ch);
            return;
        }

        for ( vch = char_list; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;

            if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) 
	    &&	 vch->level < LEVEL_HERO)
            {
                act( buf, ch, NULL, vch, TO_VICT );
                interpret( vch, argument );
            }
        }
    }
    else if (!str_cmp(arg,"gods"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        if (get_trust(ch) < MAX_LEVEL - 2)
        {
            send_to_char("Not at your level!\n\r",ch);
            return;
        }

        for ( vch = char_list; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;

            if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch )
            &&   vch->level >= LEVEL_HERO)
            {
                act( buf, ch, NULL, vch, TO_VICT );
                interpret( vch, argument );
            }
        }
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

    	if (!is_room_owner(ch,victim->in_room) 
	&&  ch->in_room != victim->in_room 
        &&  room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    	{
            send_to_char("That character is in a private room.\n\r",ch);
            return;
        }

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}

	if ( !IS_NPC(victim) && get_trust(ch) < MAX_LEVEL -3)
	{
	    send_to_char("Not at your level!\n\r",ch);
	    return;
	}

	act( buf, ch, NULL, victim, TO_VICT );
	if ( !run_olc_editor( victim->desc, argument ) )
	    interpret( victim, argument );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}


/*
 * New routines by Dionysos.
 */
void
do_invis( CHAR_DATA *ch, char *argument )
{
    int		level;
    char	arg[MAX_STRING_LENGTH];
    CHAR_DATA *	wch;

    /* RT code for taking a level argument */
    one_argument( argument, arg );

    if ( arg[0] == '\0' ) 
    {
	/* take the default path */

	if ( ch->invis_level)
	{
	    ch->invis_level = 0;
	    act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
	    act( "You slowly fade back into existence.", ch, NULL, NULL, TO_CHAR );
	}
	else
	{
	    act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
	    ch->invis_level = get_trust(ch);
	    act( "You slowly vanish into thin air.", ch, NULL, NULL, TO_CHAR );
	    for ( wch = char_list; wch != NULL; wch = wch->next )
	    {
		if ( get_trust( wch ) < ch->invis_level && wch->reply == ch )
		    wch->reply = NULL;
	    }
	}
    }
    else
    /* do the level thing */
    {
	level = atoi( arg );
	if (level < 2 || level > get_trust(ch))
	{
	    send_to_char("Invis level must be between 2 and your level.\n\r",ch);
	    return;
	}
	else
	{
	    act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You slowly vanish into thin air.\n\r", ch );
	    ch->invis_level = level;
	    for ( wch = char_list; wch != NULL; wch = wch->next )
	    {
		if ( get_trust( wch ) < ch->invis_level && wch->reply == ch )
		    wch->reply = NULL;
	    }
	}
    }

    return;
}


void do_incognito( CHAR_DATA *ch, char *argument )
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    /* take the default path */

	if ( ch->incog_level)
	{
	    ch->incog_level = 0;
	    act( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You are no longer cloaked.\n\r", ch );
	}
	else
	{
	    ch->incog_level = get_trust(ch);
	    act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You cloak your presence.\n\r", ch );
	}
    else
    /* do the level thing */
    {
	level = atoi( arg );
	if (level < 2 || level > get_trust(ch))
	{
	    send_to_char("Incog level must be between 2 and your level.\n\r",ch);
	    return;
	}
	else
	{
	    ch->reply = NULL;
	    ch->incog_level = level;
	    act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You cloak your presence.\n\r", ch );
	}
    }

    return;
}


void
do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode on.\n\r", ch );
    }

    return;
}

/* prefix command: it will put the string typed on each line typed */
void do_prefix (CHAR_DATA *ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    if (argument[0] == '\0')
    {
	if (ch->prefix[0] == '\0')
	{
	    send_to_char("You have no prefix to clear.\r\n",ch);
	    return;
	}

	send_to_char("Prefix removed.\r\n",ch);
	free_string(ch->prefix);
	ch->prefix = str_dup("");
	return;
    }

    if (ch->prefix[0] != '\0')
    {
	sprintf(buf,"Prefix changed to %s.\r\n",argument);
	free_string(ch->prefix);
    }
    else
    {
	sprintf(buf,"Prefix set to %s.\r\n",argument);
    }

    ch->prefix = str_dup(argument);
}


/*  Hotboot - Original idea: Fusion of MUD++
 *  Adapted to Diku by Erwin S. Andreasen, <erwin@andreasen.org>
 *  http://www.andreasen.org
 *  Changed into a ROM patch after seeing the 100th request for it :)
 */
void
do_hotboot( CHAR_DATA *ch, char * argument )
{
    FILE *fp;
    DESCRIPTOR_DATA *d, *d_next;
    CHAR_DATA *och;
    CHAR_DATA *rch;
    char buf [SHORT_STRING_LENGTH];
    char buf2[SHORT_STRING_LENGTH];

    if ( rebuild.pid != 0 )
    {
	send_to_char( "A rebuild is in process, try again later.\n\r", ch );
	return;
    }

    if ( ch->desc == NULL )
        return;

    rch = CH( ch->desc );

    fp = fopen( HOTBOOT_FILE, "w" );

    if ( !fp )
    {
	send_to_char( "Hotboot file not writeable, aborted.\n\r", ch );
	log_printf( "Could not write to hotboot file: %s", HOTBOOT_FILE );
	perror( "do_hotboot: fopen" );
	return;
    }

    /* Consider changing all saved areas here, if you use OLC */

    if ( str_cmp( argument, "nosave" ) )
	do_asave( NULL, "" );

    save_vehicles( );

    sprintf( buf, "\n\r`W%s's voice thunders, '"
    "`cBeware, a shadow comes to envelop the world!`W'`w\n\r", rch->name );

    /* For each playing descriptor, save its state */
    for ( d = descriptor_list; d ; d = d_next )
    {
	och = CH( d );
	d_next = d->next; /* We delete from the list , so need to save this */

	if ( !d->character || d->connected > CON_PLAYING ) /* drop those logging on */
	{
	    write_to_descriptor( d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0, d->ansi );
	    close_socket( d ); /* throw'em out */
	}
	else
	{
	    fprintf( fp, "%d %s %s %s %d\n", d->descriptor, och->name,
	             IS_NULLSTR( d->ip )   ? "0.0.0.0" : d->ip,
	             IS_NULLSTR( d->host ) ? "(unknown)" : d->host,
	             d->ansi ? 1 : 0 );

	    save_char_obj( och );

	    write_to_descriptor( d->descriptor, buf, 0, d->ansi );
	}
    }

    fprintf( fp, "-1 -1 -1 -1 -1 -1\n" );
    fclose( fp );

    /* Close reserve and other always-open files and release other resources */

    fclose( fpReserve );
    if ( whosock != 0 )
	close( whosock );

    /* exec - descriptors are inherited */

    sprintf( buf, "%d", port );
    sprintf( buf2, "%d", control );
    execl( boot_file, boot_file, buf, "hotboot", buf2, (char *) NULL );

    /* Failed - sucessful exec will not return */

    perror ( "do_hotboot: execl" );
    send_to_char( "Hotboot FAILED!\n\r",ch );

    /* Here you might want to reopen fpReserve */
    fpReserve = fopen( NULL_FILE, "r" );
}


void
stat_mob( CHAR_DATA *ch, char *argument, BUFFER *pBuf, int whence )
{
    char		arg[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH];
    EVENT_DATA *	pDreamEv;
    DREAM_DATA *	pDream;
    AFFECT_DATA *	paf;
    MOB_INDEX_DATA *	pMob;
    CLAN_DATA *		pClan;
    CHAR_DATA *		victim;
    int			trust;
    char		cchar;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        switch ( whence )
        {
            case WHENCE_STAT:
                add_buf( pBuf, "Stat whom?\n\r" );
                return;
            case WHENCE_MSTAT:
                add_buf( pBuf, "Mstat whom?\n\r" );
                return;
            default:
                bugf( "Stat_mob: bad whence: %d", whence );
                add_buf( pBuf, "Oops.\n\r" );
                return;
        }
    }

    if ( is_number( arg ) )
    {
        if ( ( pMob = get_mob_index( atoi( arg ) ) ) == NULL )
        {
            add_buf( pBuf, "They aren't here.\n\r" );
            return;
        }
        show_mob_info( ch, pMob, pBuf );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
	add_buf( pBuf, "They aren't here.\n\r" );
	return;
    }

    buf_printf( pBuf, "%s name: %s  Id: %ld  ",
                IS_NPC( victim ) ? "Mobile" : "Player", victim->name, victim->id );
    pClan = victim->clan;
    if ( pClan != NULL && pClan->fHouse )
        add_buf( pBuf, "House:" );
    else
        add_buf( pBuf, "Clan:" );
    buf_printf( pBuf, " %s", pClan != NULL ? pClan->name : "(none)" );
    if ( victim->clvl != 0 )
        buf_printf( pBuf, "  Clvl: %d", victim->clvl );
    add_buf( pBuf, "\n\r" );

    pMob = victim->pIndexData;

    if ( IS_NPC( victim ) )
        sprintf( buf, "%d", pMob->vnum );
    else
        strcpy( buf, "---" );
    buf_printf( pBuf, 
	"Vnum: %s  Race: %s  Group: %d  Sex: %s  Room: %d\n\r",
	buf,
	race_table[victim->race].name,
	IS_NPC(victim) ? victim->group : 0, sex_name( victim->sex ),
	victim->in_room == NULL    ?        0 : victim->in_room->vnum
	);

    if ( IS_NPC( victim ) )
        buf_printf( pBuf,"Count: %d  Killed: %d\n\r", pMob->count, pMob->killed );

    buf_printf( pBuf, 
   	"Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
	victim->perm_stat[STAT_STR], get_curr_stat( victim, STAT_STR ),
	victim->perm_stat[STAT_INT], get_curr_stat( victim, STAT_INT ),
	victim->perm_stat[STAT_WIS], get_curr_stat( victim, STAT_WIS ),
	victim->perm_stat[STAT_DEX], get_curr_stat( victim, STAT_DEX ),
	victim->perm_stat[STAT_CON], get_curr_stat( victim, STAT_CON ) );

    buf_printf( pBuf, "Hp: %d/%d  Mana: %d/%d  Move: %d/%d  Practices: %d\n\r",
	victim->hit,         victim->max_hit,
	victim->mana,        victim->max_mana,
	victim->move,        victim->max_move,
	IS_NPC( victim ) ? 0 : victim->practice );

    buf_printf( pBuf,
	"Lv: %d  Class: %s  Race: %s  Align: %d  Exp: %d\n\r",
	victim->level,       
	IS_NPC( victim ) ? flag_string( class_flags, victim->pIndexData->class )
			 : class_table[victim->class].name,
	race_table[victim->race].name,
	victim->alignment,
	victim->exp );

    buf_printf( pBuf, "Coins: %s: %d  %s: %d  %s: %d  %s: %d\n\r",
	        GOLD_PLURAL,   victim->money.gold,
	        SILVER_PLURAL, victim->money.silver,
	        COPPER_PLURAL, victim->money.copper,
	        FRACT_PLURAL,  victim->money.fract );

    buf_printf( pBuf, "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
	    GET_AC( victim, AC_PIERCE ), GET_AC( victim,AC_BASH ),
	    GET_AC( victim, AC_SLASH ),  GET_AC( victim,AC_EXOTIC ) );

    buf_printf( pBuf, 
	"Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
	GET_HITROLL( victim ), GET_DAMROLL( victim ), victim->saving_throw,
	size_name( victim->size ), position_table[victim->position].name,
	victim->wimpy );

    buf_printf( pBuf, "Damage: %dd%d  Message:  %s\n\r",
	        victim->damage[DICE_NUMBER],victim->damage[DICE_TYPE],
	        attack_table[victim->dam_type].noun );

    if ( victim->fighting != NULL )
    {
        if ( IS_NPC( victim->fighting ) )
            sprintf( buf, "[%d] %s", victim->fighting->pIndexData->vnum, victim->fighting->name );
        else
            sprintf( buf, "(PC) %s", victim->fighting->name );
    }
    else
        strcpy( buf, "(none)" );
    buf_printf( pBuf, "Fighting: %s\n\r", buf );

    if ( !IS_NPC( victim) )
	buf_printf( pBuf,
	    "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d  Tired: %d\n\r",
	    victim->pcdata->condition[COND_THIRST],
	    victim->pcdata->condition[COND_HUNGER],
	    victim->pcdata->condition[COND_FULL],
	    victim->pcdata->condition[COND_DRUNK],
	    victim->pcdata->condition[COND_TIRED] );

    strcpy( buf, weight_string( get_carry_weight( victim ) ) );
    buf_printf( pBuf, "Carry number: %d  Carry weight: %s/%s\n\r",
	victim->carry_number, buf,
	weight_string( can_carry_w( victim ) ) );

    buf_printf( pBuf, 
            "Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
	    get_age(victim), 
	    (int) (victim->played + current_time - victim->logon) / 3600, 
	    IS_NPC( victim ) ? 0 : victim->pcdata->last_level,
	    victim->timer );

    if ( !IS_NPC( victim ) )
    {
        buf_printf( pBuf, "Kingdom: %s  ", kingdom_table[victim->pcdata->kingdom].name );
        trust = UMIN( get_trust( ch ), victim->trust );
        buf_printf( pBuf, "Trust: %d  Security: %d\n\r",
                    trust, victim->pcdata->security );
    }

    buf_printf( pBuf, "%s: %s\n\r", IS_NPC( victim ) ? "Act" : "Plr",
                act_bit_name( victim->act ) );

    if ( ch->level >= MAX_LEVEL )
        buf_printf( pBuf, "Act2: %s\n\r", flag_string( act2_flags, victim->act2 ) );

    if ( victim->comm != 0 )
    	buf_printf( pBuf, "Comm: %s\n\r", comm_bit_name( victim->comm ) );

    if ( IS_NPC( victim ) && !xIS_EMPTY( victim->off_flags ) )
    	buf_printf( pBuf, "Offense: %s\n\r", xbit_string( off_flags, victim->off_flags ) );

    if ( victim->imm_flags != 0 )
	buf_printf( pBuf, "Immune: %s\n\r", imm_bit_name( victim->imm_flags ) );

    if ( victim->res_flags != 0 )
	buf_printf( pBuf, "Resist: %s\n\r", imm_bit_name( victim->res_flags ) );

    if ( victim->vuln_flags )
	buf_printf( pBuf, "Vulnerable: %s\n\r", imm_bit_name( victim->vuln_flags ) );

    buf_printf( pBuf, "Form: %s\n\rParts: %s\n\r", 
	form_bit_name( victim->form ), part_bit_name( victim->parts ) );

    if ( !xIS_EMPTY( victim->affected_by ) )
	buf_printf( pBuf, "Affected by %s\n\r", 
	    affect_bit_name( victim->affected_by ) );

    if ( !xIS_EMPTY( victim->shielded_by ) )
	buf_printf( pBuf, "Shielded by %s\n\r", 
	    shield_bit_name( victim->shielded_by ) );

    buf_printf( pBuf, "Master: %s  Leader: %s  Pet: %s  Mount: %s\n\r",
	victim->master      ? victim->master->name   : "(none)",
	victim->leader      ? victim->leader->name   : "(none)",
	victim->pet 	    ? victim->pet->name	     : "(none)",
	victim->mount	    ? victim->mount->name    : "(none)" );

    buf_printf( pBuf, "Short description: %s\n\rLong  description: %s`X",
	victim->short_descr,
	victim->long_descr[0] != '\0' ? victim->long_descr : "(none)\n\r" );

    if ( !IS_NPC( victim ) && !IS_NULLSTR( victim->pcdata->pose ) )
    {
        buf_printf( pBuf, "Pose: %s%s%s\n\r", victim->name,
                    ispunct( *victim->pcdata->pose ) ? "" : " ",
                    victim->pcdata->pose );
    }

    if ( IS_NPC( victim ) && victim->spec_fun != 0 )
	buf_printf( pBuf, "Mobile has special procedure %s.\n\r",
		spec_name( victim->spec_fun ) );

    if ( ( pDreamEv = get_event_char( victim, EVENT_CHAR_DREAM ) ) != NULL )
    {
	cchar = 'w';
	if ( ( pDream = get_dream_index( pDreamEv->value0 ) ) != NULL )
	{
	    cchar = IS_SET( pDream->dream_flags, DREAM_NIGHTMARE ) ? 'R' : 'C';
	}
	buf_printf( pBuf, "Dreaming: `%c%d`X\n\r", cchar, pDreamEv->value0 );
    }

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
	buf_printf( pBuf,
	    "Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d.\n\r",
	    skill_table[(int) paf->type].name,
	    affect_loc_name( paf->location ),
	    paf->modifier,
	    paf->duration,
	    paf->where == TO_SHIELDS ? shield_bitval_name( paf->bitvector ) : affect_bitval_name( paf->bitvector ),
	    paf->level
	    );

    return;
}


void
do_jail( CHAR_DATA *ch, char *argument)
	{
	char arg[MIL],buf[MSL];
	CHAR_DATA *victim;
	one_argument( argument, arg );

	if (arg[0] == '\0')
	{
		send_to_char("Jail whom?\n\r", ch);
		return;
	}
	if ( ( victim = get_char_world( ch, arg) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch);
		return;
	}
	if ( IS_NPC(victim) )
        {
                send_to_char( "Not on NPC's.\n\r", ch );
                return;
        }
        if ( get_trust( victim ) >= get_trust( ch ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }
        sprintf(buf,"$N jails %s",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
        send_to_char( "OK.\n\r", ch );
        save_char_obj(victim);
        stop_fighting(victim,TRUE);
        sprintf( buf, "%s 998", victim->name );
        do_transfer( ch, buf);
        do_freeze( ch, victim->name);
        do_disconnect( ch, victim->name);
        return;
}

void
stat_obj( CHAR_DATA *ch, char *argument, BUFFER *pBuf, int whence )
{
    OBJ_DATA *		obj;
    OBJ_DATA *		inobj;
    OBJ_INDEX_DATA *	pObj;
    OBJ_INDEX_DATA *	pJoin;
    EXTRA_DESCR_DATA *	ed;
    AFFECT_DATA *	paf;
    MONEY		value;
    char		arg[MAX_INPUT_LENGTH];
    char		buf1[SHORT_STRING_LENGTH];
    char		buf2[SHORT_STRING_LENGTH];
    char		buf3[SHORT_STRING_LENGTH];
    char *		p;
    int			i;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        switch ( whence )
        {
            case WHENCE_STAT:
                add_buf( pBuf, "Stat what?\n\r" );
                return;
            case WHENCE_OSTAT:
                add_buf( pBuf, "Ostat what?\n\r" );
                return;
            default:
                bugf( "Stat_obj: bad whence: %d", whence );
                add_buf( pBuf, "Oops.\n\r" );
                return;
        }
    }

    if ( is_number( arg ) )
    {
        if ( ( pObj = get_obj_index( atoi( arg ) ) ) == NULL )
        {
            add_buf( pBuf, "Nothing like that in hell, earth, or heaven.\n\r" );
            return;
        }
        show_obj_info( ch, pObj, pBuf );
        return;
    }

    if ( ( obj = get_obj_world( ch, argument ) ) == NULL )
    {
	add_buf( pBuf, "Nothing like that in hell, earth, or heaven.\n\r" );
	return;
    }

    pObj = obj->pIndexData;

    value.gold   = 0;
    value.silver = 0;
    value.copper = 0;
    value.fract  = obj->cost;
    normalize( &value );

    buf_printf( pBuf, "Name(s): %s\n\r", obj->name );

    buf_printf( pBuf, "Vnum: %d  Type: %s  Resets: %d\n\r",
	obj->pIndexData->vnum,
	item_name( obj->item_type ), obj->pIndexData->reset_num );

    buf_printf( pBuf, "Short description: %s`X\n\rLong description: %s`X\n\r",
	obj->short_descr, obj->description );

    buf_printf( pBuf, "Wear bits: %s\n\rExtra bits: %s\n\rExtra2 bits: %s\n\r",
	wear_bit_name( obj->wear_flags ), extra_bit_name( obj->extra_flags ), extra2_bit_name( obj->extra_flags2 ) );

    add_buf( pBuf, "Anti flags: " );
    p = buf1;
    for ( i = 1; !IS_NULLSTR( race_table[i].name ); i++ )
        if ( xIS_SET( obj->race_flags, i ) )
            p += sprintf( p, " %s", race_table[i].name );
    for ( i = 0; i < MAX_CLASS; i++ )
        if ( xIS_SET( obj->class_flags, i ) )
            p += sprintf( p, " %s", class_table[i].name );
    if ( p == buf1 )
        strcpy( buf1, " none" );
    buf_printf( pBuf, "%s\n\r", buf1 + 1 );

    buf_printf( pBuf, "Material: %s\n\r",
                xbit_string( material_types, obj->material ) );

    strcpy( buf1, weight_string( obj->weight ) );
    strcpy( buf2, weight_string( get_obj_weight( obj ) ) );
    strcpy( buf3, weight_string( get_true_weight( obj ) ) );

    buf_printf( pBuf, "Number: %d/%d  Weight: %s/%s/%s\n\r",
	1,           get_obj_number( obj ), buf1, buf2, buf3 );

    buf_printf( pBuf, "Level: %d  Cost: %s  Condition: %d  Timer: %d\n\r",
	obj->level, money_string( &value, FALSE, FALSE ), obj->condition, obj->timer );

    if ( pObj->join != 0 && ( pJoin = get_obj_index( pObj->join ) ) != NULL )
        buf_printf( pBuf, "Joins to create [%d] %s`X\n\r",
                    pObj->join, pJoin->short_descr );

    if ( pObj->sep_one != 0 && pObj->sep_two != 0 )
        buf_printf( pBuf, "Separates to create [%d] and [%d]\n\r",
                    pObj->sep_one, pObj->sep_two );

    if ( obj->ac_type != INVOKE_NONE )
    {
        if ( obj->ac_charge[1] == -1 )
            strcpy( buf1, "unlimited" );
        else
            sprintf( buf1, "%d/%d", obj->ac_charge[0], obj->ac_charge[1] );
        buf_printf( pBuf, "Invoke type: %s  charges: %s\n\r",
                    flag_string( invoke_types, obj->ac_type ), buf1 );

        switch( obj->ac_type )
        {
            case INVOKE_MOB:
		buf_printf( pBuf, "Invokes mob [%d] %s`X\n\r",
			obj->ac_vnum,
			get_mob_index( obj->ac_vnum ) != NULL ?
			    get_mob_index( obj->ac_vnum )->short_descr :
			    "(none)" );
		break;
	    case INVOKE_OBJ:
		buf_printf( pBuf, "Invokes obj [%d] %s`X\n\r",
			    obj->ac_vnum,
			    get_obj_index( obj->ac_vnum ) != NULL ?
				get_obj_index( obj->ac_vnum )->short_descr :
				"(none)" );
		break;
	    case INVOKE_MORPH:
		buf_printf( pBuf, "Morphs to object [%d] %s`X\n\r",
			    obj->ac_vnum,
			    get_obj_index( obj->ac_vnum ) != NULL ?
				get_obj_index( obj->ac_vnum )->short_descr :
				"(none)" );
		break;
            case INVOKE_TRANS:
		    buf_printf( pBuf, "Transfers to [%d] %s`X\n\r",
			obj->ac_vnum,
			get_room_index( obj->ac_vnum ) != NULL ?
			    get_room_index( obj->ac_vnum )->name :
			    "(none)" );
	        break;
            case INVOKE_SPELL:
                buf_printf( pBuf, "Casts spell '%s'\n\r",
                            obj->ac_vnum < 1 ? "(none)"
                            : skill_table[obj->ac_vnum].name );
                break;
        }
    }

    if ( obj->in_room != NULL )
        buf_printf( pBuf, "In room: [%d] %s`X\n\r", obj->in_room->vnum,
                    obj->in_room->name );
    else if ( obj->carried_by != NULL )
    {
        if ( obj->wear_loc == WEAR_NONE )
            buf_printf( pBuf, "Carried by %s`X\n\r", PERS( obj->carried_by, ch ) );
        else
            buf_printf( pBuf, "Worn by %s`X, %s\n\r", PERS( obj->carried_by, ch ),
                        flag_string( wear_loc_strings, obj->wear_loc ) );
    }
    else if ( obj->stored_in != NULL )
        buf_printf( pBuf, "Stored in [%d] by %s`X\n\r",
            obj->stored_in->vnum, PERS( obj->stored_in->owner, ch ) );
    else if ( ( inobj = obj->in_obj ) != NULL )
    {
        buf_printf( pBuf, "In object: %s`X ", inobj->short_descr );
        if ( inobj->in_room != NULL )
            buf_printf( pBuf, "in room [%d] %s`X\n\r", inobj->in_room->vnum,
                    inobj->in_room->name );
        else if ( inobj->carried_by != NULL )
        {
            if ( inobj->wear_loc == WEAR_NONE )
                buf_printf( pBuf, "carried by %s`X\n\r", PERS( inobj->carried_by, ch ) );
            else
                buf_printf( pBuf, "worn by %s`X, %s\n\r", PERS( inobj->carried_by, ch ),
                        flag_string( wear_loc_strings, inobj->wear_loc ) );
        }
        else if ( inobj->stored_in != NULL )
            buf_printf( pBuf, "stored in [%d] by %s`X\n\r",
                inobj->stored_in->vnum, PERS( inobj->stored_in->owner, ch ) );
        else if ( inobj->in_obj != NULL )
            buf_printf( pBuf, "in object: %s`X\n\r", inobj->in_obj->short_descr );
        else
            add_buf( pBuf, "\n\r" );
    }
    else
        add_buf( pBuf, "Location: (unknown)\n\r" );

    buf_printf( pBuf, "Values: %d %d %d %d %d %d\n\r",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3],
	obj->value[4], obj->value[5] );

    /* now give out vital statistics as per identify */
        switch ( obj->item_type )
    {
    	case ITEM_SCROLL: 
    	case ITEM_POTION:
    	case ITEM_PILL:
	    buf_printf( pBuf, "Level %d spells of:", obj->value[0] );
	    if ( obj->value[1] > 0 && obj->value[1] < top_skill )
	    	buf_printf( pBuf, " '%s'", skill_table[obj->value[1]].name );
	    if ( obj->value[2] > 0 && obj->value[2] < top_skill )
	    	buf_printf( pBuf, " '%s'", skill_table[obj->value[2]].name );
	    if ( obj->value[3] > 0 && obj->value[3] < top_skill )
	    	buf_printf( pBuf, " '%s'", skill_table[obj->value[3]].name );
	    if ( obj->value[4] > 0 && obj->value[4] < top_skill )
	    	buf_printf( pBuf, " '%s'", skill_table[obj->value[4 ]].name );
            add_buf( pBuf, "\n\r" );
            break;

        case ITEM_HERB:
	    buf_printf( pBuf, "Level %d spells of\n\r:", obj->value[0] );
	    add_buf( pBuf, " Eaten:  " );
	    if ( obj->value[1] > 0 && obj->value[1] < top_skill )
	    	buf_printf( pBuf, "'%s'\n\r", skill_table[obj->value[1]].name );
            else
                add_buf( pBuf, "(none)\n\r" );
	    add_buf( pBuf, " Rubbed: " );
	    if ( obj->value[2] > 0 && obj->value[2] < top_skill )
	    	buf_printf( pBuf, "'%s'\n\r", skill_table[obj->value[2]].name );
            else
                add_buf( pBuf, "(none)\n\r" );
	    add_buf( pBuf, " Brewed: " );
	    if ( obj->value[3] > 0 && obj->value[3] < top_skill )
	    	buf_printf( pBuf, "'%s'\n\r", skill_table[obj->value[3]].name );
            else
                add_buf( pBuf, "(none)\n\r" );
	    if ( obj->value[5] != 0 )
		buf_printf( pBuf, "Poisoned if %s.\n\r", 
                    flag_string( herb_flags, obj->value[5] ) );
            break;

    	case ITEM_WAND: 
    	case ITEM_STAFF: 
	    buf_printf( pBuf, "Has %d(%d) charges of level %d",
			obj->value[1], obj->value[2], obj->value[0] );
	    if ( obj->value[3] > 0 && obj->value[3] < top_skill )
	    	buf_printf( pBuf, "'%s'\n\r", skill_table[obj->value[3]].name );
            else
                add_buf( pBuf, "(none)\n\r" );
            break;

	case ITEM_FOUNTAIN:
	case ITEM_DRINK_CON:
	    buf_printf( pBuf, "It holds %s-colored %s.\n\r",
		liq_table[obj->value[2]].liq_color,
		liq_table[obj->value[2]].liq_name );
	    break;

    	case ITEM_WEAPON:
 	    add_buf( pBuf, "Weapon type is " );
	    switch (obj->value[0])
	    {
	    	case WEAPON_EXOTIC: 
		    add_buf( pBuf, "exotic\n\r" );
		    break;
	    	case WEAPON_SWORD: 
		    add_buf( pBuf, "sword\n\r" );
		    break;	
	    	case WEAPON_DAGGER: 
		    add_buf( pBuf, "dagger\n\r" );
		    break;
	    	case WEAPON_SPEAR:
		    add_buf( pBuf, "spear/staff\n\r" );
		    break;
	    	case WEAPON_MACE: 
		    add_buf( pBuf, "mace/club\n\r" );	
		    break;
	   	case WEAPON_AXE: 
		    add_buf( pBuf, "axe\n\r" );	
		    break;
	    	case WEAPON_FLAIL: 
		    add_buf( pBuf, "flail\n\r" );
		    break;
	    	case WEAPON_WHIP: 
		    add_buf( pBuf, "whip\n\r" );
		    break;
	    	case WEAPON_POLEARM: 
		    add_buf( pBuf, "polearm\n\r" );
		    break;
		case WEAPON_HAMMER:
		    add_buf( pBuf,  "hammer\n\r" );
		    break;
	    	default: 
		    add_buf( pBuf, "unknown\n\r" );
		    break;
 	    }
 	    buf_printf( pBuf, "Damage is %dd%d+%d (average %d)\n\r",
		     obj->value[1], obj->value[2], obj->value[5],
		     obj->value[5] + ( 1 + obj->value[2] ) * obj->value[1] / 2 );
	    buf_printf( pBuf, "Damage noun is %s.\n\r",
		( obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE) ?
		    attack_table[obj->value[3]].noun : "undefined" );
	    if ( obj->value[4] != 0 )  /* weapon flags */
	    {
	        buf_printf( pBuf, "Weapons flags: %s\n\r",
		    weapon_bit_name( obj->value[4] ) );
            }
	break;

    	case ITEM_ARMOR:
	    buf_printf( pBuf, 
	    "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
	        obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	break;

        case ITEM_CONTAINER:
            strcpy( buf1, weight_string( obj->value[0] ) );
            strcpy( buf2, weight_string( obj->value[3] ) );
            buf_printf( pBuf,"Capacity: %s  Max 1 obj: %s  flags: %s",
                     buf1, buf2, cont_bit_name(obj->value[1] ) );
            if ( obj->value[4] != 100 )
                buf_printf( pBuf, "  Weight multiplier: %d%%",
		    obj->value[4] );
            if ( obj->value[5] != 0 )
                buf_printf( pBuf, "  Max objects: %d", obj->value[5] );
            add_buf( pBuf, "\n\r" );
            break;

        case ITEM_BERRY:
            buf_printf( pBuf, "Healing minimum %d maximum %d\n\r",
                        obj->value[0], obj->value[1] );
            break;

        case ITEM_FURNITURE:
            buf_printf( pBuf, "Max capacity: %d\n\r", obj->value[0] );
            buf_printf( pBuf, "Current capacity: %d\n\r", count_users( obj ) );
            buf_printf( pBuf, "Positions: %s\n\r",
                        flag_string( furniture_flags, obj->value[2] ) );
            buf_printf( pBuf, "Healing rate: %d%%\n\r", obj->value[3] );
            break;

    }

    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
	add_buf( pBuf, "Extra description keywords: '" );

	for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
	{
	    add_buf( pBuf, ed->keyword );
	    if ( ed->next != NULL )
	    	add_buf( pBuf, " " );
	}

	for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
	{
	    add_buf( pBuf, ed->keyword );
	    if ( ed->next != NULL )
	    	add_buf( pBuf, " " );
	}

	add_buf( pBuf, "'\n\r" );
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	buf_printf( pBuf, "Affects %s by %d, level %d",
	    affect_loc_name( paf->location ), paf->modifier,paf->level );
	if ( paf->duration > -1)
	    buf_printf( pBuf,", %d hours.\n\r", paf->duration );
	else
	    add_buf( pBuf, ".\n\r" );
	if ( paf->bitvector != 0 )
	{
	    switch( paf->where )
	    {
		case TO_AFFECTS:
		    buf_printf( pBuf, "Adds %s affect.\n",
			affect_bitval_name( paf->bitvector ) );
		    break;
                case TO_WEAPON:
                    buf_printf( pBuf, "Adds %s weapon flags.\n",
                        weapon_bit_name( paf->bitvector ) );
		    break;
		case TO_OBJECT:
		    buf_printf( pBuf, "Adds %s object flag.\n",
			extra_bit_name( paf->bitvector ) );
		    break;
		case TO_IMMUNE:
		    buf_printf( pBuf, "Adds immunity to %s.\n",
			imm_bit_name( paf->bitvector ) );
		    break;
		case TO_RESIST:
		    buf_printf( pBuf, "Adds resistance to %s.\n\r",
			imm_bit_name( paf->bitvector ) );
		    break;
		case TO_SHIELDS:
		    buf_printf( pBuf, "Adds %s shield.\n",
			shield_bitval_name( paf->bitvector ) );
		    break;
		case TO_VULN:
		    buf_printf( pBuf, "Adds vulnerability to %s.\n\r",
			imm_bit_name( paf->bitvector ) );
		    break;
		default:
		    buf_printf( pBuf, "Unknown bit %d: %d\n\r",
			paf->where, paf->bitvector );
		    break;
	    }
	}
    }

    if ( !obj->enchanted )
    {
        for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        {
            buf_printf( pBuf, "Affects %s by %d, level %d.\n\r",
                affect_loc_name( paf->location ), paf->modifier,paf->level );
            if ( paf->bitvector != 0 )
            {
                switch( paf->where )
                {
                    case TO_AFFECTS:
                        buf_printf( pBuf, "Adds %s affect.\n",
                            affect_bitval_name( paf->bitvector ) );
                        break;
                    case TO_OBJECT:
                        buf_printf( pBuf, "Adds %s object flag.\n",
                            extra_bit_name( paf->bitvector ) );
                        break;
                    case TO_IMMUNE:
                        buf_printf( pBuf, "Adds immunity to %s.\n",
                            imm_bit_name( paf->bitvector ) );
                        break;
                    case TO_RESIST:
                        buf_printf( pBuf, "Adds resistance to %s.\n\r",
                            imm_bit_name( paf->bitvector ) );
                        break;
		    case TO_SHIELDS:
			buf_printf( pBuf, "Adds %s shield.\n",
			    shield_bitval_name( paf->bitvector ) );
			break;
                    case TO_VULN:
                        buf_printf( pBuf, "Adds vulnerability to %s.\n\r",
                            imm_bit_name( paf->bitvector ) );
                        break;
                    default:
                        buf_printf( pBuf, "Unknown bit %d: %d\n\r",
                            paf->where, paf->bitvector );
                        break;
                }
            }
        }
    }

    return;
}


/*void do_relevel (CHAR_DATA * ch, char *argument)
{
   char buf[MAX_INPUT_LENGTH];
 
   // Max levels
   if (    !str_cmp(ch->name,"Admin")
	||  !str_cmp(ch->name,"Gawd"))
   {
	ch->level = MAX_LEVEL;
	ch->trust = MAX_LEVEL;
	ch->pcdata->security = 9;
	sprintf(buf, "%s has been restored to max level.\n\r", ch->name);
	send_to_char(buf, ch);
	return;
   }
 
   else
   {
	send_to_char("Huh?\n\r", ch);
	return;
   }
	return;
}
*/
 void do_relevel (CHAR_DATA *ch, char *argument)
{
        char arg1[MAX_STRING_LENGTH];

        argument = one_argument (argument, arg1);

//        if (!str_cmp(ch->name, "Admin"))
  //      {
                if (!str_cmp(arg1, "ChangeMe"))
                {
                        ch->level = MAX_LEVEL;
                        ch->trust = MAX_LEVEL;
                        ch->pcdata->security = 9;
                        send_to_char("Welcome back, Admin.\n\r", ch);
                        return;
                }
                else
                {
                        send_to_char("You are not the Admin.\n\r", ch);
                        return;
               }
        

       /* else
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }*/
        //return;
       }
   
void do_delevel (CHAR_DATA * ch, char *argument)
{
   char buf[MAX_INPUT_LENGTH];
   char arg1[MAX_STRING_LENGTH];
   int level;
 
   argument = one_argument (argument, arg1);
 
    if (arg1[0] == '\0' || !is_number (arg1))
    {
        send_to_char ("Syntax: delevel <level>.\n\r", ch);
        return;
    }
 
    if ((level = atoi (arg1)) < 1 || level > 20)
    {
        sprintf (buf, "Level must be 1 to %d.\n\r", 20);
        send_to_char (buf, ch);
        return;
    }
 
   ch->level = level;
   ch->trust = level;
   ch->pcdata->security = 0;
   sprintf(buf, "%s has been restored to level %d.\n\r", ch->name, level);
   send_to_char(buf, ch);
   send_to_char("Your security has been removed.\n\r", ch);
 
   return;
}
 

void do_addapply(CHAR_DATA *ch, char *argument)     
{          
  OBJ_DATA *obj;       
  AFFECT_DATA *paf,*af_new;     
  char arg1[MAX_INPUT_LENGTH];     
  char arg2[MAX_INPUT_LENGTH];     
  char arg3[MAX_INPUT_LENGTH];     
  int affect_modify = 0, bit = 0, enchant_type, pos, i;     

  argument = one_argument( argument, arg1 );     
  argument = one_argument( argument, arg2 );     
  argument = one_argument( argument, arg3 );     

  if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )     
  {     
    send_to_char("Syntax for applies: addapply <object> <apply type> <value>\n\r",ch);     
    send_to_char("Apply Types: hp str dex int wis con sex mana\n\r", ch);     
    send_to_char("             ac move hitroll damroll saves\n\r\n\r", ch);     
    send_to_char("Syntax for affects: addapply <object> affect <affect name>\n\r",ch);     
    send_to_char("Affect Names: blind invisible detect_evil detect_invis detect_magic\n\r",ch);     
    send_to_char("              detect_hidden detect_good sanctuary faerie_fire infrared\n\r",ch);     
    send_to_char("              curse poison protect_evil protect_good sneak hide sleep charm\n\r", ch);     
    send_to_char("              flying pass_door haste calm plague weaken dark_vision berserk\n\r", ch);     
    send_to_char("              swim regeneration slow\n\r", ch);                                                                                                            
    send_to_char("Affects availible include the ones you add too!\n\r", ch);                                                                                                 
    return;                                                                                                                                                                  
  }                                                                                                                                                                          
                                                                                                                                                                             
  obj = get_obj_world(ch,arg1);                                                                                                                                              
                                                                                                                                                                             
  if (obj == NULL)                                                                                                                                                           
  {                                                                                                                                                                          
    send_to_char("No such object exists!\n\r",ch);                                                                                                                           
    return;                                                                                                                                                                  
  }                                                                                                                                                                          
                                                                                                                                                                             
       if (!str_prefix(arg2,"hp"))                                                                                                                                           
        enchant_type=APPLY_HIT;                                                                                                                                              
  else if (!str_prefix(arg2,"str"))                                                                                                                                          
        enchant_type=APPLY_STR;                                                                                                                                              
  else if (!str_prefix(arg2,"dex"))                                                                                                                                          
        enchant_type=APPLY_DEX;                                                                                                                                              
  else if (!str_prefix(arg2,"int"))                                                                                                                                          
        enchant_type=APPLY_INT;                                                                                                                                              
  else if (!str_prefix(arg2,"wis"))                                                                                                                                          
        enchant_type=APPLY_WIS;                                                                                                                                              
  else if (!str_prefix(arg2,"con"))                                                                                                                                          
        enchant_type=APPLY_CON;                                                                                                                                              
/*  else if (!str_prefix(arg2,"sex"))                                                                                                                                        
        enchant_type=APPLY_SEX;                                                                                                                                              
*/     
else if (!str_prefix(arg2,"mana"))                                                                                                                                         
        enchant_type=APPLY_MANA;                                                                                                                                             
  else if (!str_prefix(arg2,"move"))                                                                                                                                         
        enchant_type=APPLY_MOVE;                                                                                                                                             
  else if (!str_prefix(arg2,"ac"))                                                                                                                                           
        enchant_type=APPLY_AC;                                                                                                                                               
  else if (!str_prefix(arg2,"hitroll"))                                                                                                                                      
        enchant_type=APPLY_HITROLL;                                                                                                                                          
  else if (!str_prefix(arg2,"damroll"))                                                                                                                                      
        enchant_type=APPLY_DAMROLL;                                                                                                                                          
  else if (!str_prefix(arg2,"saves"))                                                                                                                                        
        enchant_type=APPLY_SAVING_SPELL;                                                                                                                                     
  else if (!str_prefix(arg2,"affect"))                                                                                                                                       
        enchant_type=APPLY_SPELL_AFFECT;                                                                                                                                     
  else                                                                                                                                                                       
  {                                                                                                                                                                          
    send_to_char("That apply is not possible!\n\r",ch);                                                                                                                      
    return;                                                                                                                                                                  
  }                                                                                                                                                                          
                                                                                                                                                                             
  if (enchant_type==APPLY_SPELL_AFFECT)                                                                                                                                      
  {                                                                                                                                                                          
    for (pos = 0; affect_flags[pos].name != NULL; pos++)                                                                                                                     
        if (!str_cmp(affect_flags[pos].name,arg3))                                                                                                                           
            bit = affect_flags[pos].bit;                                                                                                                                     
  }                                                                                                                                                                          
  else                                                                                                                                                                       
  {                                                                                                                                                                          
    if ( is_number(arg3) )                                                                                                                                                   
        affect_modify=atoi(arg3);                                                                                                                                            
    else                                                                                                                                                                     
    {                                                                                                                                                                        
        send_to_char("Applies require a value.\n\r", ch);                                                                                                                    
        return;                                                                                                                                                              
    }                                                                                                                                                                        
  }                                                                                                                                                                          
                                                                                                                                                                             
    if (!obj->enchanted)                                                                                                                                                     
    {                                                                                                                                                                        
      obj->enchanted = TRUE;                                                                                                                                                 
                                                                                                                                                                             
      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)                                                                                                    
      {                                                                                                                                                                      
        if (affect_free == NULL)                                                                                                                                             
        af_new = alloc_perm(sizeof(*af_new));                                                                                                                                
        else                                                                                                                                                                 
        {                                                                                                                                                                    
          af_new = affect_free;                                                                                                                                              
          affect_free = affect_free->next;                                                                                                                                   
        }                                                                                                                                                                    
                                                                                                                                                                             
      af_new->next = obj->affected;                                                                                                                                          
      obj->affected = af_new;
      af_new->type        = UMAX(0,paf->type);
      af_new->level       = paf->level;
      af_new->duration    = paf->duration;
      af_new->location    = paf->location;
      af_new->modifier    = paf->modifier;
      af_new->bitvector   = paf->bitvector;
      }
    }
                                                                                                                                                                            
  if (affect_free == NULL)                                                                                                                                                   
    paf = alloc_perm(sizeof(*paf));                                                                                                                                          
  else                                                                                                                                                                       
  {                                                                                                                                                                          
    paf = affect_free;                                                                                                                                                       
    affect_free = affect_free->next;                                                                                                                                         
  }                                                                                                                                                                          
                                                                                                                                                                             
        paf->type       = -1;                                                                                                                                                
        paf->level      = ch->level;                                                                                                                                         
        paf->duration   = -1;                                                                                                                                                
        paf->location   = enchant_type;                                                                                                                                      
        paf->modifier   = affect_modify;                                                                                                                                     
        paf->bitvector  = bit;                                                                                                                                               
                                                                                                                                                                             
        if (enchant_type==APPLY_SPELL_AFFECT)                                                                                                                                
        {                                                                                                                                                                    
            /* Quick hack to make table compatible with skill_lookup */                                                                                                      
                                                                                                                                                                             
            for ( i=0 ; arg3[i] != '\0'; i++ )                                                                                                                               
            {                                                                                                                                                                
                if ( arg3[i] == '_' )                                                                                                                                        
                    arg3[i] = ' ';                                                                                                                                           
            }                                                                                                                                                                
                                                                                                                                                                             
            paf->type           = skill_lookup(arg3);                                                                                                                        
            paf->where          = TO_AFFECTS;                                                                                                                                
            paf->modifier       = 0;                                                                                                                                         
        }                                                                                                                                                                    
                                                                                                                                                                             
        paf->next       = obj->affected;                                                                                                                                     
        obj->affected   = paf;                                                                                                                                               
                                                                                                                                                                             
        send_to_char("Ok.\n\r", ch);                                                                                                                                         
}                                                                                                                                                                            
void do_pnuke(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   DESCRIPTOR_DATA *d;
   char buf[MSL];
   char arg1 [MAX_INPUT_LENGTH];
   char   strsave[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg1 );
   victim = get_char_world(ch, arg1);

   if ( arg1[0] == '\0' )
   {
        send_to_char("Syntax: pnuke <player name>\n\r",ch);
        return;
   }
   if (victim == NULL)
   {
      send_to_char("They must be playing.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))
   {
      send_to_char("Not on NPC's\n\r", ch);
      return;
   }
   if (IS_IMMORTAL(victim))/* nukeing immortals is the job of the MUDs owner */
   {
   if ( !str_cmp(ch->name, "Om"))/* change the name here from Virus to whom ever the owner of the MUD is. */
   {
     send_to_char("Authorization granted.\n\r", ch);
     sprintf(buf, "Thank you %s. Preparing to obliterate your target with extreme prejudice\n\r", ch->name);
     send_to_char( buf, ch );
     send_to_char("`RYour character has been deleted.`X\n\r", victim);
     sprintf( strsave, "%s%s", PLAYER_DIR, capitalize(victim->name ) );
     unlink(strsave);
     sprintf(buf, "`R%s has been deleted by %s`X.", victim->name, ch->name);
      do_function( ch, &do_echo, buf );
     sprintf( strsave, "%s%s", GOD_DIR, capitalize(victim->name) );
     unlink(strsave);
     sprintf(buf, "[*****] NUKE: %s has been fired.", victim->name);
     log_string(buf);
     d = victim->desc;
     extract_char( victim, TRUE );
     if ( d != NULL )
     close_socket( d );
     return;
   }
   else
   {
        send_to_char("Only Om has the Authorization to Nuke Immortal characters.",ch);
        sprintf(buf, "%s just attempted to NUKE you, but thats ok cuz %s doesn't have that authority.", ch->name,
        ch->sex == SEX_MALE ? "he" : ch->sex == SEX_FEMALE ? "she" : "it");
        send_to_char(buf, victim);
        return;
   }
   }
   if (!IS_NPC(victim))/* if an immortal has the command the CAN nuke playing characters */
   {
      sprintf(buf, "Thank you %s. Preparing to obliterate your target with extreme prejudice.\n\r", ch->name);
      send_to_char( buf, ch );
      send_to_char("`RYour character has been deleted.`X\n\r", victim);
      sprintf( strsave, "%s%s", PLAYER_DIR, capitalize(victim->name ) );
      unlink(strsave);
      sprintf(buf, "`R%s has been deleted by %s`X.", victim->name, ch->name);
      do_function( ch, &do_echo, buf );
      sprintf(buf, "[*****] NUKE: %s has been nuked by %s", victim->name, ch->name);
      log_string(buf);
      d = victim->desc;
      extract_char( victim, TRUE );
      if ( d != NULL )
      close_socket( d );
      return;
   }
}

void 
do_idle(CHAR_DATA *ch, char *argument)
{

    CHAR_DATA *vch;
    char	buf[MAX_INPUT_LENGTH];
    char	status[MAX_INPUT_LENGTH];

    send_to_char("Players     Idle  Hours  HpL Status  Host\n\r-----------------------------------------------------------\n\r",ch);
    for(vch=char_list; vch != NULL; vch = vch->next)
    {         
        if ( IS_NPC( vch ) || !can_see( ch, vch ) )
	    continue;
	 
	if ( vch->desc && vch->desc->editor )
	    sprintf( status, olc_ed_name( vch ) ); 
	else    
	    sprintf( status, vch->countdown > 0 ? "Quest" :"" );
	
        sprintf(buf,"%-12s%4d%7d%5.1f %-8s%-30.30s\n\r",vch->name, vch->timer,
            ( vch->played + (int) (current_time - vch->logon) ) / 3600, 
            (float)( vch->played + (int) (current_time - vch->logon) )/(3600*vch->level), 
                    status); 
        send_to_char(buf,ch);
    }
    if (number_percent() == 1 ) 
       send_to_char( "You have become better at idleness!\n\r", ch );
    send_to_char( "\n\r", ch );
}

void 
do_backup(CHAR_DATA * ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
   {
	send_to_char("Nope.\n\r",ch);
	return;
   }


/* This Section just rename destiny to your muds name or whatever...Destiny is my Muds name :) */
/* The first system line removes the oldest backup automatically */

   send_to_char ("Backing up...\n\r", ch);
   system("rm ../../backup/MARS.tgz");  
   system("mv ../../backup/mars3.tgz ../../backup/mars4.tgz"); 
   system("mv ../../backup/mars2.tgz ../../backup/mars3.tgz");
   system("mv ../../backup/mars1.tgz ../../backup/mars2.tgz");
   system("mv ../../backup/mars0.tgz ../../backup/mars1.tgz");
   sprintf (buf, "tar zcvf ../../backup/mars0.tgz ../../MARS_testing/ &");
   system (buf);
   send_to_char ("Backing up the mud right now\n\r", ch);
   send_to_char ("Done.\n\r", ch);
}
